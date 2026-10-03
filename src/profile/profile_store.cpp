#include "profile_store.hpp"

#include <cstdlib>
#include <system_error>
#include <vector>

namespace interrun {
namespace {

std::filesystem::path data_home() {
    if (const char* xdg = std::getenv("XDG_DATA_HOME")) {
        if (*xdg) return xdg;
    }
    if (const char* home = std::getenv("HOME")) {
        if (*home) return std::filesystem::path(home) / ".local" / "share";
    }
    return std::filesystem::current_path() / ".interrun-data";
}

} // namespace

ProfileStore::ProfileStore(std::string profile_name)
    : root_(data_home() / "interrun" / "profiles" /
            (profile_name.empty() ? "default" : profile_name)) {}

const std::filesystem::path& ProfileStore::root() const {
    return root_;
}

bool ProfileStore::initialize(std::string& error) const {
    static const std::vector<std::string> directories = {
        "bookmarks", "history", "messages", "contacts",
        "site-storage", "cache", "downloads", "keys"
    };

    std::error_code ec;
    std::filesystem::create_directories(root_, ec);
    if (ec) {
        error = "Could not create profile root: " + ec.message();
        return false;
    }

    for (const auto& directory : directories) {
        ec.clear();
        std::filesystem::create_directories(root_ / directory, ec);
        if (ec) {
            error = "Could not create profile directory '" + directory + "': " + ec.message();
            return false;
        }
    }

    error.clear();
    return true;
}

} // namespace interrun
