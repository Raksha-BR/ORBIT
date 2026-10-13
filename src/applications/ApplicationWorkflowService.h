
#pragma once

#include "../core/ApplicationStateMachine.h"
#include "../storage/IApplicationRepository.h"
#include "../storage/IApplicationEventRepository.h"
#include "../storage/ITransactionManager.h"

namespace orbit {

class ApplicationWorkflowService {
public:
    ApplicationWorkflowService(
        IApplicationRepository& application_repository,
        IApplicationEventRepository& event_repository,
        ITransactionManager& transaction_manager
    );

    void transition(
        ApplicationId application_id,
        ApplicationStatus new_status
    );

private:
    IApplicationRepository& application_repository_;
    IApplicationEventRepository& event_repository_;
    ITransactionManager& transaction_manager_;

    ApplicationStateMachine state_machine_;
};

} // namespace orbit
