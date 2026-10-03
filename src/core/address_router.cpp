#include "address_router.hpp"

#include <algorithm>
#include <cctype>
#include <string>
#include <vector>

namespace interrun {
namespace {

std::string trim(std::string value) {
    const auto first = std::find_if_not(value.begin(), value.end(), [](unsigned char c) {
        return std::isspace(c) != 0;
    });
    const auto last = std::find_if_not(value.rbegin(), value.rend(), [](unsigned char c) {
        return std::isspace(c) != 0;
    }).base();
    if (first >= last) return {};
    return std::string(first, last);
}

std::string lower(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return value;
}

bool ends_with(const std::string& value, const std::string& suffix) {
    return value.size() >= suffix.size() &&
           value.compare(value.size() - suffix.size(), suffix.size(), suffix) == 0;
}

bool contains_space(const std::string& value) {
    return std::any_of(value.begin(), value.end(), [](unsigned char c) {
        return std::isspace(c) != 0;
    });
}

bool media_extension(const std::string& value) {
    const std::string lowered = lower(value);
    const auto cut = lowered.find_first_of("?#");
    const std::string clean = cut == std::string::npos ? lowered : lowered.substr(0, cut);
    static const std::vector<std::string> extensions = {
        ".mp4", ".mkv", ".webm", ".mov", ".avi", ".m4v", ".ts", ".m2ts",
        ".mpg", ".mpeg", ".ogv", ".flv", ".wmv", ".mp3", ".flac", ".wav",
        ".ogg", ".opus", ".m4a", ".aac", ".m3u", ".m3u8"
    };
    return std::any_of(extensions.begin(), extensions.end(), [&](const std::string& ext) {
        return ends_with(clean, ext);
    });
}

bool onion_target(const std::string& value) {
    std::string host = lower(value);
    const auto scheme = host.find("://");
    if (scheme != std::string::npos) host = host.substr(scheme + 3);
    const auto slash = host.find_first_of("/?#");
    if (slash != std::string::npos) host.resize(slash);
    const auto colon = host.rfind(':');
    if (colon != std::string::npos) host.resize(colon);
    return ends_with(host, ".onion");
}

} // namespace

RouteResult route_input(const std::string& input) {
    RouteResult out;
    out.original = input;
    out.normalized = trim(input);

    if (out.normalized.empty()) return out;

    const std::string lowered = lower(out.normalized);

    if (lowered.rfind("interrun://join/", 0) == 0 ||
        lowered.rfind("interrun://call/", 0) == 0) {
        out.kind = TargetKind::Meeting;
        return out;
    }

    if (lowered.rfind("interrun:", 0) == 0) {
        out.kind = TargetKind::Internal;
        return out;
    }

    if (media_extension(out.normalized)) {
        out.kind = TargetKind::Media;
        return out;
    }

    if (lowered.rfind("file://", 0) == 0 ||
        out.normalized.rfind("/", 0) == 0 ||
        out.normalized.rfind("./", 0) == 0 ||
        out.normalized.rfind("../", 0) == 0) {
        out.kind = TargetKind::LocalFile;
        return out;
    }

    if (onion_target(out.normalized)) {
        out.kind = TargetKind::Onion;
        if (lowered.rfind("http://", 0) != 0 && lowered.rfind("https://", 0) != 0) {
            out.normalized = "http://" + out.normalized;
        }
        return out;
    }

    if (lowered.rfind("http://", 0) == 0 || lowered.rfind("https://", 0) == 0) {
        out.kind = TargetKind::ClearWeb;
        return out;
    }

    if (!contains_space(out.normalized) && out.normalized.find('.') != std::string::npos) {
        out.kind = TargetKind::ClearWeb;
        out.normalized = "https://" + out.normalized;
        return out;
    }

    out.kind = TargetKind::Search;
    return out;
}

const char* target_kind_name(TargetKind kind) {
    switch (kind) {
        case TargetKind::Search: return "search";
        case TargetKind::ClearWeb: return "clear-web";
        case TargetKind::Onion: return "onion";
        case TargetKind::LocalFile: return "local-file";
        case TargetKind::Media: return "media";
        case TargetKind::Meeting: return "meeting";
        case TargetKind::Internal: return "internal";
    }
    return "search";
}

} // namespace interrun
