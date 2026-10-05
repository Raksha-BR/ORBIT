#pragma once

#include "Application.h"

namespace orbit {

class ApplicationStateMachine {
public:
    bool can_transition(
        ApplicationStatus from,
        ApplicationStatus to
    ) const;

    void transition(
        Application& application,
        ApplicationStatus new_status
    ) const;
};

} // namespace orbit
