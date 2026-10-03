#include "core/address_router.hpp"
#include "privacy/privacy_policy.hpp"
#include "profile/profile_store.hpp"
#include "ui/x11_shell.hpp"

#include <iostream>
#include <string>

namespace {

std::string join_arguments(int argc, char** argv, int start) {
    std::string input;
    for (int i = start; i < argc; ++i) {
        if (!input.empty()) input += ' ';
        input += argv[i];
    }
    return input;
}

} // namespace

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

    if (argc > 1 && std::string(argv[1]) == "--route") {
        const auto route = interrun::route_input(join_arguments(argc, argv, 2));
        std::cout << interrun::target_kind_name(route.kind) << '\n'
                  << route.normalized << '\n';
        return 0;
    }

    const std::string initial = argc > 1 ? join_arguments(argc, argv, 1) : std::string{};
    interrun::X11Shell shell(profile.root());
    return shell.run(initial);
}
