#include "x11_shell.hpp"

#include "../core/address_router.hpp"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

#include <algorithm>
#include <array>
#include <cctype>
#include <cstdint>
#include <cstdio>
#include <iomanip>
#include <random>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace interrun {
namespace {

struct Rect {
    int x{};
    int y{};
    int w{};
    int h{};
    bool hit(int px, int py) const {
        return px >= x && px < x + w && py >= y && py < y + h;
    }
};

enum class View { Home, Search, Messages, Calls, Media, Privacy };

class App {
public:
    App(std::filesystem::path profile, std::string initial)
        : profile_(std::move(profile)), address_(std::move(initial)) {}

    int run() {
        d_ = XOpenDisplay(nullptr);
        if (!d_) {
            std::fprintf(stderr, "Interrun: could not open X display.\n");
            return 2;
        }

        s_ = DefaultScreen(d_);
        cmap_ = DefaultColormap(d_, s_);
        bg_ = colour("#15181b", BlackPixel(d_, s_));
        panel_ = colour("#202428", bg_);
        panel2_ = colour("#1b1f23", panel_);
        border_ = colour("#3a4148", WhitePixel(d_, s_));
        fg_ = colour("#f2f4f5", WhitePixel(d_, s_));
        muted_ = colour("#aab3bb", fg_);
        active_ = colour("#e9edf0", fg_);
        dark_ = colour("#101214", BlackPixel(d_, s_));

        w_ = XCreateSimpleWindow(
            d_, RootWindow(d_, s_), 120, 80, width_, height_, 1, border_, bg_);
        XStoreName(d_, w_, "Interrun");

        XClassHint hint{};
        hint.res_name = const_cast<char*>("interrun");
        hint.res_class = const_cast<char*>("Interrun");
        XSetClassHint(d_, w_, &hint);

        XSelectInput(d_, w_, ExposureMask | KeyPressMask | ButtonPressMask | StructureNotifyMask);
        wm_delete_ = XInternAtom(d_, "WM_DELETE_WINDOW", False);
        XSetWMProtocols(d_, w_, &wm_delete_, 1);

        gc_ = XCreateGC(d_, w_, 0, nullptr);
        font_ = XLoadQueryFont(d_, "fixed");
        if (font_) {
            XSetFont(d_, gc_, font_->fid);
            cw_ = std::max(6, static_cast<int>(font_->max_bounds.width));
            lh_ = std::max(14, font_->ascent + font_->descent + 4);
        }

        XMapWindow(d_, w_);
        XSetInputFocus(d_, w_, RevertToParent, CurrentTime);
        if (!address_.empty()) route();

        bool open = true;
        while (open) {
            XEvent e{};
            XNextEvent(d_, &e);
            if (e.type == Expose && e.xexpose.count == 0) draw();
            else if (e.type == ConfigureNotify) {
                width_ = std::max(760, e.xconfigure.width);
                height_ = std::max(520, e.xconfigure.height);
                draw();
            } else if (e.type == ButtonPress) {
                click(e.xbutton.x, e.xbutton.y);
            } else if (e.type == KeyPress) {
                key(e.xkey);
            } else if (e.type == ClientMessage &&
                       static_cast<Atom>(e.xclient.data.l[0]) == wm_delete_) {
                open = false;
            }
        }

        if (font_) XFreeFont(d_, font_);
        XFreeGC(d_, gc_);
        XDestroyWindow(d_, w_);
        XCloseDisplay(d_);
        return 0;
    }

private:
    unsigned long colour(const char* name, unsigned long fallback) {
        XColor a{}, b{};
        return XAllocNamedColor(d_, cmap_, name, &a, &b) ? a.pixel : fallback;
    }

    int tw(const std::string& v) const {
        return font_ ? XTextWidth(font_, v.c_str(), static_cast<int>(v.size()))
                     : static_cast<int>(v.size()) * cw_;
    }

    void fill(const Rect& r, unsigned long c) {
        XSetForeground(d_, gc_, c);
        XFillRectangle(d_, w_, gc_, r.x, r.y,
                       static_cast<unsigned int>(std::max(0, r.w)),
                       static_cast<unsigned int>(std::max(0, r.h)));
    }

    void outline(const Rect& r, unsigned long c) {
        XSetForeground(d_, gc_, c);
        XDrawRectangle(d_, w_, gc_, r.x, r.y,
                       static_cast<unsigned int>(std::max(0, r.w - 1)),
                       static_cast<unsigned int>(std::max(0, r.h - 1)));
    }

    void text(int x, int y, const std::string& v, unsigned long c = 0) {
        XSetForeground(d_, gc_, c ? c : fg_);
        XDrawString(d_, w_, gc_, x, y, v.c_str(), static_cast<int>(v.size()));
    }

    void wrap(int x, int y, int max_w, const std::string& value, unsigned long c = 0) {
        std::istringstream in(value);
        std::string word;
        std::string line;
        int yy = y;
        while (in >> word) {
            const std::string next = line.empty() ? word : line + " " + word;
            if (!line.empty() && tw(next) > max_w) {
                text(x, yy, line, c);
                yy += lh_;
                line = word;
            } else line = next;
        }
        if (!line.empty()) text(x, yy, line, c);
    }

    void button(const Rect& r, const std::string& label, bool selected = false) {
        fill(r, selected ? active_ : panel_);
        outline(r, border_);
        text(r.x + std::max(7, (r.w - tw(label)) / 2),
             r.y + (r.h + lh_) / 2 - 3,
             label, selected ? dark_ : fg_);
    }

    void draw() {
        fill({0, 0, width_, height_}, bg_);

        fill({0, 0, width_, 38}, panel_);
        text(14, 24, "INTERRUN");
        const std::string top = "LOCAL PROFILE | TRACKERS BLOCKED";
        text(width_ - tw(top) - 14, 24, top, muted_);

        fill({0, 38, width_, 34}, panel2_);
        button({8, 42, 150, 29}, "New Tab", true);
        button({164, 42, 34, 29}, "+");

        fill({0, 72, width_, 48}, panel_);
        back_ = {10, 78, 36, 34};
        forward_ = {50, 78, 36, 34};
        reload_ = {90, 78, 36, 34};
        home_ = {130, 78, 36, 34};
        button(back_, "<");
        button(forward_, ">");
        button(reload_, "R");
        button(home_, "H");

        address_box_ = {176, 78, std::max(250, width_ - 270), 34};
        fill(address_box_, dark_);
        outline(address_box_, active_);
        const std::string shown = address_.empty()
            ? "Search, web address, .onion, media file, or Interrun join link"
            : address_;
        text(address_box_.x + 10, address_box_.y + 22, shown,
             address_.empty() ? muted_ : fg_);
        go_ = {width_ - 84, 78, 74, 34};
        button(go_, "Go");

        fill({0, 120, 150, height_ - 150}, panel2_);
        static const std::array<std::pair<View, const char*>, 6> nav = {{
            {View::Home, "Home"}, {View::Search, "Search"},
            {View::Messages, "Messages"}, {View::Calls, "Calls"},
            {View::Media, "Media"}, {View::Privacy, "Privacy"}
        }};
        side_.clear();
        int sy = 132;
        for (const auto& item : nav) {
            Rect r{10, sy, 130, 36};
            side_.push_back({item.first, r});
            button(r, item.second, view_ == item.first);
            sy += 42;
        }

        content(172, 145, width_ - 194);

        fill({0, height_ - 30, width_, 30}, panel_);
        text(10, height_ - 10, "Interrun v0.0.2 native preview", muted_);
        text(width_ - tw(status_) - 10, height_ - 10, status_, muted_);
        XFlush(d_);
    }

    void card(const Rect& r, const std::string& title, const std::string& body) {
        fill(r, panel_);
        outline(r, border_);
        text(r.x + 14, r.y + 25, title);
        wrap(r.x + 14, r.y + 50, r.w - 28, body, muted_);
    }

    void content(int x, int y, int width) {
        if (view_ == View::Home) {
            text(x, y + 28, "INTERRUN");
            wrap(x, y + 56, width,
                 "This is Interrun's own native Linux window. No other browser is being used to display it. "
                 "The actual web renderer is the next browser-engine milestone.", muted_);
            const int cw = std::max(230, (width - 12) / 2);
            card({x, y + 105, cw, 112}, "BROWSE",
                 "Address routing works. Netscape/Mozilla page rendering is not wired yet.");
            card({x + cw + 12, y + 105, cw, 112}, "CALL BY LINK",
                 "Local Interrun join links work. P2P media transport comes next.");
            card({x, y + 229, cw, 112}, "MEDIA",
                 "DK Media and Nougat are the donor path for libVLC playback.");
            card({x + cw + 12, y + 229, cw, 112}, "PRIVACY",
                 "No cloud profile. Telemetry off. Third-party cookies blocked.");
            if (!route_detail_.empty()) {
                card({x, y + 355, std::max(420, width), 108}, "ROUTE RESULT", route_detail_);
            }
        } else if (view_ == View::Search) {
            heading(x, y, width, "SEARCH / ADDRESS",
                    "Type in the address bar and press Enter. Nougat Secure Search is the private-search donor path.");
        } else if (view_ == View::Messages) {
            heading(x, y, width, "MESSAGES",
                    "Local-first message storage is mapped. Encrypted P2P transport is not connected yet.");
            card({x, y + 105, std::max(420, width), 120}, "POLICY",
                 "P2P preferred. End-to-end encryption required. No cloud account required.");
        } else if (view_ == View::Calls) {
            heading(x, y, width, "CALLS",
                    "Generate a local Interrun join link. Baresip/libre is the planned native call engine.");
            call_button_ = {x, y + 105, 190, 38};
            button(call_button_, "Create Join Link");
            card({x, y + 157, std::max(420, width), 120}, "LOCAL JOIN LINK",
                 join_.empty() ? "No link created yet." : join_);
        } else if (view_ == View::Media) {
            heading(x, y, width, "MEDIA",
                    "DK Media and Nougat will provide libVLC playback, subtitles, tracks, resume state, streams, and P2P media.");
            card({x, y + 105, std::max(420, width), 120}, "CURRENT BUILD",
                 "Media files are recognized by the native router. libVLC is not linked into v0.0.2 yet.");
        } else {
            heading(x, y, width, "PRIVACY DEFAULTS",
                    "Telemetry off | No required cloud profile | Third-party cookies blocked | Tracking storage blocked");
            card({x, y + 105, std::max(420, width), 138}, "LOCAL FIRST",
                 "Bookmarks, history, messages, contacts, and keys live in the local profile. "
                 "Direct communication is P2P-preferred and governed by an end-to-end encryption policy.");
            wrap(x + 14, y + 275, width - 28, "Profile: " + profile_.string(), muted_);
        }
    }

    void heading(int x, int y, int width, const std::string& title, const std::string& body) {
        text(x, y + 28, title);
        wrap(x, y + 58, width, body, muted_);
    }

    void click(int x, int y) {
        if (go_.hit(x, y)) return route();
        if (home_.hit(x, y)) {
            view_ = View::Home;
            status_ = "Home";
            return draw();
        }
        if (back_.hit(x, y) || forward_.hit(x, y)) {
            status_ = "History wiring comes after the renderer";
            return draw();
        }
        if (reload_.hit(x, y)) {
            status_ = "View refreshed";
            return draw();
        }
        for (const auto& item : side_) {
            if (item.second.hit(x, y)) {
                view_ = item.first;
                status_ = view_name(view_);
                return draw();
            }
        }
        if (view_ == View::Calls && call_button_.hit(x, y)) {
            join_ = make_join();
            address_ = join_;
            status_ = "Join link created locally";
            return draw();
        }
        draw();
    }

    void key(XKeyEvent& e) {
        KeySym sym = NoSymbol;
        char buf[64]{};
        const int n = XLookupString(&e, buf, sizeof(buf), &sym, nullptr);

        if ((e.state & ControlMask) && (sym == XK_l || sym == XK_L)) {
            address_.clear();
            status_ = "Address bar focused";
            return draw();
        }
        if (sym == XK_Return || sym == XK_KP_Enter) return route();
        if (sym == XK_BackSpace) {
            if (!address_.empty()) address_.pop_back();
            return draw();
        }

        if (n > 0 && address_.size() < 2048U) {
            for (int i = 0; i < n; ++i) {
                const unsigned char c = static_cast<unsigned char>(buf[i]);
                if (std::isprint(c)) address_.push_back(static_cast<char>(c));
            }
            draw();
        }
    }

    void route() {
        const RouteResult r = route_input(address_);
        address_ = r.normalized;

        std::ostringstream out;
        out << "Route: " << target_kind_name(r.kind)
            << " | Target: " << (r.normalized.empty() ? "(empty)" : r.normalized);

        if (r.kind == TargetKind::Search) {
            out << " | Nougat Secure Search donor path";
            view_ = View::Search;
        } else if (r.kind == TargetKind::Onion) {
            out << " | Tor transport mapped, not integrated yet";
            view_ = View::Home;
        } else if (r.kind == TargetKind::Media) {
            out << " | DK Media / Nougat libVLC donor path";
            view_ = View::Media;
        } else if (r.kind == TargetKind::Meeting) {
            out << " | P2P communications donor path";
            view_ = View::Calls;
            join_ = r.normalized;
        } else if (r.kind == TargetKind::ClearWeb) {
            out << " | Browser renderer not integrated yet";
            view_ = View::Home;
        }

        route_detail_ = out.str();
        status_ = std::string(target_kind_name(r.kind)) + " route selected";
        draw();
    }

    static std::string view_name(View v) {
        switch (v) {
            case View::Home: return "Home";
            case View::Search: return "Search";
            case View::Messages: return "Messages";
            case View::Calls: return "Calls";
            case View::Media: return "Media";
            case View::Privacy: return "Privacy";
        }
        return "Interrun";
    }

    static std::string make_join() {
        std::random_device rd;
        std::ostringstream out;
        out << "interrun://join/" << std::hex << std::setfill('0');
        for (int i = 0; i < 4; ++i) out << std::setw(8) << rd();
        return out.str();
    }

    std::filesystem::path profile_;
    std::string address_;
    std::string route_detail_;
    std::string join_;
    std::string status_{"Ready | no cloud account required"};

    Display* d_{};
    int s_{};
    Colormap cmap_{};
    Window w_{};
    GC gc_{};
    XFontStruct* font_{};
    Atom wm_delete_{};

    int width_{1100};
    int height_{720};
    int cw_{8};
    int lh_{16};
    View view_{View::Home};

    unsigned long bg_{};
    unsigned long panel_{};
    unsigned long panel2_{};
    unsigned long border_{};
    unsigned long fg_{};
    unsigned long muted_{};
    unsigned long active_{};
    unsigned long dark_{};

    Rect back_{};
    Rect forward_{};
    Rect reload_{};
    Rect home_{};
    Rect address_box_{};
    Rect go_{};
    Rect call_button_{};
    std::vector<std::pair<View, Rect>> side_;
};

} // namespace

X11Shell::X11Shell(std::filesystem::path profile_root)
    : profile_root_(std::move(profile_root)) {}

int X11Shell::run(const std::string& initial_input) {
    App app(profile_root_, initial_input);
    return app.run();
}

} // namespace interrun
