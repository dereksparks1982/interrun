#include "core/address_router.hpp"
#include "privacy/privacy_policy.hpp"
#include "privacy/privacy_receipt.hpp"
#include "profile/profile_store.hpp"

#include <iostream>
#include <string>

int main(int argc, char** argv) {
    interrun::ProfileStore profile;
    std::string error;
    if (!profile.initialize(error)) {
        std::cerr << "Interrun profile error: " << error << '\n';
        return 1;
    }

    const auto policy = interrun::PrivacyPolicy::interrun_default();
    if (!policy.valid(error)) {
        std::cerr << "Interrun privacy policy error: " << error << '\n';
        return 1;
    }

    std::cout << "Interrun v0.0.1 bootstrap\n";
    std::cout << "Local profile: " << profile.root() << '\n';

    if (argc > 1) {
        std::string input;
        for (int i = 1; i < argc; ++i) {
            if (!input.empty()) input += ' ';
            input += argv[i];
        }

        const auto route = interrun::route_input(input);
        std::cout << "Route: " << interrun::target_kind_name(route.kind) << '\n';
        std::cout << "Target: " << route.normalized << '\n';
    } else {
        std::cout << "No target supplied. Browser/rendering integration is the next native milestone.\n";
    }

    interrun::PrivacyReceipt receipt;
    std::cout << '\n' << receipt.to_text() << '\n';
    return 0;
}
