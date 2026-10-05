#include "ApplicationStateMachine.h"

#include <stdexcept>

namespace orbit {

bool ApplicationStateMachine::can_transition(
    ApplicationStatus from,
    ApplicationStatus to
) const {
    switch (from) {

        case ApplicationStatus::Discovered:
            return to == ApplicationStatus::Shortlisted;

        case ApplicationStatus::Shortlisted:
            return to == ApplicationStatus::Preparing ||
                   to == ApplicationStatus::Withdrawn;

        case ApplicationStatus::Preparing:
            return to == ApplicationStatus::ReadyForReview ||
                   to == ApplicationStatus::Withdrawn;

        case ApplicationStatus::ReadyForReview:
            return to == ApplicationStatus::Approved ||
                   to == ApplicationStatus::Preparing ||
                   to == ApplicationStatus::Withdrawn;

        case ApplicationStatus::Approved:
            return to == ApplicationStatus::Applying ||
                   to == ApplicationStatus::Withdrawn;

        case ApplicationStatus::Applying:
            return to == ApplicationStatus::Applied ||
                   to == ApplicationStatus::Preparing;

        case ApplicationStatus::Applied:
            return to == ApplicationStatus::Assessment ||
                   to == ApplicationStatus::Interview ||
                   to == ApplicationStatus::Rejected ||
                   to == ApplicationStatus::Withdrawn;

        case ApplicationStatus::Assessment:
            return to == ApplicationStatus::Interview ||
                   to == ApplicationStatus::Rejected;

        case ApplicationStatus::Interview:
            return to == ApplicationStatus::Offer ||
                   to == ApplicationStatus::Rejected;

        case ApplicationStatus::Offer:
            return to == ApplicationStatus::Closed;

        case ApplicationStatus::Rejected:
            return to == ApplicationStatus::Closed;

        case ApplicationStatus::Withdrawn:
            return to == ApplicationStatus::Closed;

        case ApplicationStatus::Closed:
            return false;
    }

    return false;
}

void ApplicationStateMachine::transition(
    Application& application,
    ApplicationStatus new_status
) const {
    if (!can_transition(application.status, new_status)) {
        throw std::invalid_argument(
            "Invalid application state transition"
        );
    }

    application.status = new_status;

    const auto now = std::chrono::system_clock::now();

    application.last_activity_at = now;

    if (new_status == ApplicationStatus::Applied) {
        application.applied_at = now;
    }
}

} // namespace orbit
