#pragma once

#include "Types.h"

#include <chrono>
#include <optional>

namespace orbit {

struct Application {
    ApplicationId id{};

    CandidateId candidate_id{};
    JobId job_id{};

    ApplicationStatus status{
        ApplicationStatus::Discovered
    };

    std::chrono::system_clock::time_point created_at{
        std::chrono::system_clock::now()
    };

    std::optional<std::chrono::system_clock::time_point> applied_at;

    std::optional<std::chrono::system_clock::time_point>
        last_activity_at;
};

} // namespace orbit
