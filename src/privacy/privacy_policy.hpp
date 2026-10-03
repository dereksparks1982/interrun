#pragma once

#include <string>
#include <vector>

namespace interrun {

struct PrivacyPolicy {
    bool telemetry = false;
    bool cloud_profile = false;
    bool third_party_cookies = false;
    bool tracking_storage = false;
    bool local_history = true;
    bool local_bookmarks = true;
    bool local_messages = true;
    bool storage_explanation_required = true;
    bool p2p_preferred = true;
    bool end_to_end_communications = true;

    static PrivacyPolicy interrun_default();
    bool valid(std::string& error) const;
    std::vector<std::string> invariant_names() const;
};

} // namespace interrun
