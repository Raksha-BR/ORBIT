#pragma once

#include <cstdint>

namespace orbit {

using JobId = std::uint64_t;
using CandidateId = std::uint64_t;
using ApplicationId = std::uint64_t;

enum class ApplicationStatus {
    Discovered,
    Shortlisted,
    Preparing,
    ReadyForReview,
    Approved,
    Applying,
    Applied,
    Assessment,
    Interview,
    Offer,
    Rejected,
    Withdrawn,
    Closed
};

} // namespace orbit
