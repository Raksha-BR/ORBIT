#include "ApplicationWorkflowService.h"

#include <stdexcept>
#include <string>

namespace orbit {

namespace {

std::string status_to_string(ApplicationStatus status) {
    switch (status) {
        case ApplicationStatus::Discovered:
            return "DISCOVERED";
        case ApplicationStatus::Shortlisted:
            return "SHORTLISTED";
        case ApplicationStatus::Preparing:
            return "PREPARING";
        case ApplicationStatus::ReadyForReview:
            return "READY_FOR_REVIEW";
        case ApplicationStatus::Approved:
            return "APPROVED";
        case ApplicationStatus::Applying:
            return "APPLYING";
        case ApplicationStatus::Applied:
            return "APPLIED";
        case ApplicationStatus::Assessment:
            return "ASSESSMENT";
        case ApplicationStatus::Interview:
            return "INTERVIEW";
        case ApplicationStatus::Offer:
            return "OFFER";
        case ApplicationStatus::Rejected:
            return "REJECTED";
        case ApplicationStatus::Withdrawn:
            return "WITHDRAWN";
        case ApplicationStatus::Closed:
            return "CLOSED";
    }

    throw std::invalid_argument("Unknown application status");
}

} // namespace

ApplicationWorkflowService::ApplicationWorkflowService(
    IApplicationRepository& application_repository,
    IApplicationEventRepository& event_repository
)
    : application_repository_(application_repository),
      event_repository_(event_repository) {
}

void ApplicationWorkflowService::transition(
    ApplicationId application_id,
    ApplicationStatus new_status
) {
    auto application =
        application_repository_.find(application_id);

    if (!application.has_value()) {
        throw std::runtime_error(
            "Application not found: " +
            std::to_string(application_id)
        );
    }

    const auto previous_status = application->status;

    // Validates the transition and updates timestamps.
    state_machine_.transition(*application, new_status);

    // Persist the new application state.
    application_repository_.save(*application);

    // Record the transition in the application's timeline.
    ApplicationEvent event{
        .application_id = application_id,
        .event_type = "STATUS_CHANGED",
        .description =
            status_to_string(previous_status) +
            " -> " +
            status_to_string(new_status),
        .occurred_at = std::chrono::system_clock::now()
    };

    event_repository_.save(event);
}

} // namespace orbit