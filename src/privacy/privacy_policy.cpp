#include "privacy_policy.hpp"

namespace interrun {

PrivacyPolicy PrivacyPolicy::interrun_default() {
    return PrivacyPolicy{};
}

bool PrivacyPolicy::valid(std::string& error) const {
    if (telemetry || cloud_profile || third_party_cookies || tracking_storage) {
        error = "Interrun default privacy policy contains a forbidden relaxation.";
        return false;
    }
    if (!storage_explanation_required) {
        error = "Persistent site storage must explain what it is for.";
        return false;
    }
    if (!p2p_preferred || !end_to_end_communications) {
        error = "Interrun communications defaults must remain P2P-preferred and end-to-end encrypted.";
        return false;
    }
    error.clear();
    return true;
}

std::vector<std::string> PrivacyPolicy::invariant_names() const {
    return {
        "no-default-telemetry",
        "no-required-cloud-profile",
        "no-third-party-cookies-by-default",
        "no-tracking-storage-by-default",
        "local-history",
        "local-bookmarks",
        "local-messages",
        "site-storage-explanation",
        "p2p-preferred",
        "end-to-end-communications"
    };
}

} // namespace interrun
