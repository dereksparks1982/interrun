#pragma once

#include <string>

namespace interrun {

enum class TargetKind {
    Search,
    ClearWeb,
    Onion,
    LocalFile,
    Media,
    Meeting,
    Internal
};

struct RouteResult {
    TargetKind kind = TargetKind::Search;
    std::string original;
    std::string normalized;
};

RouteResult route_input(const std::string& input);
const char* target_kind_name(TargetKind kind);

} // namespace interrun
