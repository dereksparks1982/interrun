#pragma once

#include <string>

namespace interrun {

struct PrivacyReceipt {
    std::string action = "bootstrap";
    std::string network = "none";
    bool telemetry_sent = false;
    bool third_party_cookie_allowed = false;
    bool cloud_profile_used = false;

    std::string to_text() const;
};

} // namespace interrun
