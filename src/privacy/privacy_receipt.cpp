#include "privacy_receipt.hpp"

#include <sstream>

namespace interrun {
namespace {

const char* yes_no(bool value) {
    return value ? "Yes" : "No";
}

} // namespace

std::string PrivacyReceipt::to_text() const {
    std::ostringstream out;
    out << "INTERRUN PRIVACY RECEIPT\n"
        << "Action: " << action << '\n'
        << "Network: " << network << '\n'
        << "Telemetry sent: " << yes_no(telemetry_sent) << '\n'
        << "Third-party cookie allowed: " << yes_no(third_party_cookie_allowed) << '\n'
        << "Cloud profile used: " << yes_no(cloud_profile_used);
    return out.str();
}

} // namespace interrun
