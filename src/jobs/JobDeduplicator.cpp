
#include "JobDeduplicator.h"

#include <algorithm>
#include <cctype>
#include <string>

namespace orbit {
namespace {

std::string normalize(std::string value) {
    const auto first = std::find_if_not(
        value.begin(),
        value.end(),
        [](unsigned char c) {
            return std::isspace(c);
        }
    );

    const auto last = std::find_if_not(
        value.rbegin(),
        value.rend(),
        [](unsigned char c) {
            return std::isspace(c);
        }
    ).base();

    if (first >= last) {
        return {};
    }

    value = std::string(first, last);

    std::transform(
        value.begin(),
        value.end(),
        value.begin(),
        [](unsigned char c) {
            return static_cast<char>(std::tolower(c));
        }
    );

    return value;
}

std::string normalize_url(std::string url) {
    url = normalize(url);

    while (url.size() > 1 && url.back() == '/') {
        url.pop_back();
    }

    return url;
}

} // namespace

bool JobDeduplicator::are_duplicates(
    const Job& first,
    const Job& second
) const {
    const auto first_url = normalize_url(first.application_url);
    const auto second_url = normalize_url(second.application_url);

    // Matching non-empty URLs are strong duplicate evidence.
    if (!first_url.empty() &&
        !second_url.empty() &&
        first_url == second_url) {
        return true;
    }

    // Fall back to normalized company, title, and location.
    return !normalize(first.company).empty() &&
           !normalize(first.title).empty() &&
           normalize(first.company) == normalize(second.company) &&
           normalize(first.title) == normalize(second.title) &&
           normalize(first.location) == normalize(second.location);
}

} // namespace orbit
