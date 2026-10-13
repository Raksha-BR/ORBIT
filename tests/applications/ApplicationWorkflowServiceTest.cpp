
#include <gtest/gtest.h>

#include "applications/ApplicationWorkflowService.h"

#include <algorithm>
#include <stdexcept>
#include <unordered_map>
#include <vector>

namespace orbit {

namespace {

class FakeApplicationRepository : public IApplicationRepository {
public:
    void save(const Application& application) override {
        applications_[application.id] = application;
    }

    std::optional<Application> find(
        ApplicationId id
    ) const override {
        const auto it = applications_.find(id);

        if (it == applications_.end()) {
            return std::nullopt;
        }

        return it->second;
    }

    std::vector<Application> find_all() const override {
        std::vector<Application> result;
        result.reserve(applications_.size());

        for (const auto& [id, application] : applications_) {
            result.push_back(application);
        }

        return result;
    }

private:
    std::unordered_map<ApplicationId, Application> applications_;
};

class FakeApplicationEventRepository
    : public IApplicationEventRepository {
public:
    void save(const ApplicationEvent& event) override {
        events_.push_back(event);
    }

    std::vector<ApplicationEvent> find_by_application(
        ApplicationId application_id
    ) const override {
        std::vector<ApplicationEvent> result;

        for (const auto& event : events_) {
            if (event.application_id == application_id) {
                result.push_back(event);
            }
        }

        return result;
    }

private:
    std::vector<ApplicationEvent> events_;
};

TEST(
    ApplicationWorkflowServiceTest,
    ValidTransitionUpdatesStatusAndRecordsEvent
) {
    FakeApplicationRepository applications;
    FakeApplicationEventRepository events;

    Application application;
    application.id = 101;
    application.candidate_id = 1;
    application.job_id = 501;
    application.status = ApplicationStatus::Discovered;

    applications.save(application);

    ApplicationWorkflowService workflow(applications, events);

    workflow.transition(
        101,
        ApplicationStatus::Shortlisted
    );

    const auto updated = applications.find(101);

    ASSERT_TRUE(updated.has_value());
    EXPECT_EQ(
        updated->status,
        ApplicationStatus::Shortlisted
    );

    const auto timeline = events.find_by_application(101);

    ASSERT_EQ(timeline.size(), 1);
    EXPECT_EQ(timeline[0].event_type, "STATUS_CHANGED");
    EXPECT_EQ(
        timeline[0].description,
        "DISCOVERED -> SHORTLISTED"
    );
}

TEST(
    ApplicationWorkflowServiceTest,
    InvalidTransitionDoesNotChangeStatusOrRecordEvent
) {
    FakeApplicationRepository applications;
    FakeApplicationEventRepository events;

    Application application;
    application.id = 102;
    application.status = ApplicationStatus::Discovered;

    applications.save(application);

    ApplicationWorkflowService workflow(applications, events);

    EXPECT_THROW(
        workflow.transition(102, ApplicationStatus::Interview),
        std::invalid_argument
    );

    const auto unchanged = applications.find(102);

    ASSERT_TRUE(unchanged.has_value());
    EXPECT_EQ(
        unchanged->status,
        ApplicationStatus::Discovered
    );
    EXPECT_TRUE(events.find_by_application(102).empty());
}

TEST(
    ApplicationWorkflowServiceTest,
    MissingApplicationThrowsAndRecordsNoEvent
) {
    FakeApplicationRepository applications;
    FakeApplicationEventRepository events;

    ApplicationWorkflowService workflow(applications, events);

    EXPECT_THROW(
        workflow.transition(999, ApplicationStatus::Shortlisted),
        std::runtime_error
    );

    EXPECT_TRUE(events.find_by_application(999).empty());
}

TEST(
    ApplicationWorkflowServiceTest,
    MultipleTransitionsCreateSeparateTimelineEvents
) {
    FakeApplicationRepository applications;
    FakeApplicationEventRepository events;

    Application application;
    application.id = 103;
    application.status = ApplicationStatus::Discovered;

    applications.save(application);

    ApplicationWorkflowService workflow(applications, events);

    workflow.transition(103, ApplicationStatus::Shortlisted);
    workflow.transition(103, ApplicationStatus::Preparing);

    const auto updated = applications.find(103);
    ASSERT_TRUE(updated.has_value());
    EXPECT_EQ(updated->status, ApplicationStatus::Preparing);

    const auto timeline = events.find_by_application(103);

    ASSERT_EQ(timeline.size(), 2);
    EXPECT_EQ(
        timeline[0].description,
        "DISCOVERED -> SHORTLISTED"
    );
    EXPECT_EQ(
        timeline[1].description,
        "SHORTLISTED -> PREPARING"
    );
}

} // namespace

} // namespace orbit
