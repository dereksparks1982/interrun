#include "core/address_router.hpp"
#include "privacy/privacy_policy.hpp"

#include <cstdlib>
#include <iostream>
#include <string>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        std::cerr << "FAIL: " << message << '\n';
        std::exit(1);
    }
}

} // namespace

int main() {
    using interrun::TargetKind;

    expect(interrun::route_input("https://example.com").kind == TargetKind::ClearWeb,
           "HTTPS should route to clear web");
    expect(interrun::route_input("example.com").kind == TargetKind::ClearWeb,
           "bare domain should route to clear web");
    expect(interrun::route_input("abcdefghijklmnop.onion").kind == TargetKind::Onion,
           "onion address should route to Tor transport");
    expect(interrun::route_input("movie.mkv").kind == TargetKind::Media,
           "media extension should route to media subsystem");
    expect(interrun::route_input("interrun://join/ABC123").kind == TargetKind::Meeting,
           "join link should route to communications subsystem");
    expect(interrun::route_input("cats wearing hats").kind == TargetKind::Search,
           "plain words should route to search");

    std::string error;
    expect(interrun::PrivacyPolicy::interrun_default().valid(error),
           "default privacy policy must validate");

    std::cout << "Interrun core tests passed.\n";
    return 0;
}
