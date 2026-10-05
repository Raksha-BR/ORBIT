#include <gtest/gtest.h>

#include "core/ApplicationStateMachine.h"

namespace orbit {

TEST(ApplicationStateMachineTest, AllowsValidTransition) {
    Application application;

    application.status = ApplicationStatus::Discovered;

    ApplicationStateMachine state_machine;

    EXPECT_TRUE(
        state_machine.can_transition(
            ApplicationStatus::Discovered,
            ApplicationStatus::Shortlisted
        )
    );

    state_machine.transition(
        application,
        ApplicationStatus::Shortlisted
    );

    EXPECT_EQ(
        application.status,
        ApplicationStatus::Shortlisted
    );
}

TEST(ApplicationStateMachineTest, RejectsInvalidTransition) {
    Application application;

    application.status = ApplicationStatus::Discovered;

    ApplicationStateMachine state_machine;

    EXPECT_THROW(
        state_machine.transition(
            application,
            ApplicationStatus::Interview
        ),
        std::invalid_argument
    );
}

} // namespace orbit
