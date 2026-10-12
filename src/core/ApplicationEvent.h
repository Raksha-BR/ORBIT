
#pragma once

#include "Types.h"

#include <chrono>
#include <string>

namespace orbit {

using ApplicationEventId = std::uint64_t;

struct ApplicationEvent {
    ApplicationEventId id{};

    ApplicationId application_id{};

    std::string event_type;
    std::string description;

    std::chrono::system_clock::time_point occurred_at{
        std::chrono::system_clock::now()
    };
};

} // namespace orbit
