#pragma once

#include <filesystem>
#include <string>

namespace interrun {

class ProfileStore {
public:
    explicit ProfileStore(std::string profile_name = "default");

    const std::filesystem::path& root() const;
    bool initialize(std::string& error) const;

private:
    std::filesystem::path root_;
};

} // namespace interrun
