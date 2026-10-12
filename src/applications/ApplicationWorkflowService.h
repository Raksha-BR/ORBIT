#pragma once

#include "../core/ApplicationStateMachine.h"
#include "../storage/IApplicationRepository.h"
#include "../storage/IApplicationEventRepository.h"

namespace orbit {

class ApplicationWorkflowService {
public:
    ApplicationWorkflowService(
        IApplicationRepository& application_repository,
        IApplicationEventRepository& event_repository
    );

    void transition(
        ApplicationId application_id,
        ApplicationStatus new_status
    );

private:
    IApplicationRepository& application_repository_;
    IApplicationEventRepository& event_repository_;

    ApplicationStateMachine state_machine_;
};

} // namespace orbit