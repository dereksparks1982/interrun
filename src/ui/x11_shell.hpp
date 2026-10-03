#pragma once

#include <filesystem>
#include <string>

namespace interrun {

class X11Shell {
public:
    explicit X11Shell(std::filesystem::path profile_root);
    int run(const std::string& initial_input = {});

private:
    std::filesystem::path profile_root_;
};

} // namespace interrun
