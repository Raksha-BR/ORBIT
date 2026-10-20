#include "jobs/MockJobSource.h"

namespace orbit {

std::string MockJobSource::name() const {
    return "MockSource";
}

std::vector<Job> MockJobSource::fetch_jobs() {
    Job first;
    first.title = "C++ Backend Engineer";
    first.company = "Orbit Demo";
    first.location = "Bengaluru";
    first.salary = "15-25 LPA";
    first.minimum_years_experience = 1;
    first.skills = {"C++", "Python", "PostgreSQL"};
    first.description =
        "Build backend services using C++ and PostgreSQL.";
    first.application_url = "https://example.com/jobs/orbit-demo-1";
    first.source_name = name();
    first.external_job_id = "ORBIT-DEMO-001";
    first.original_url = first.application_url;

    Job second;
    second.title = "Python Software Engineer";
    second.company = "Orbit Demo";
    second.location = "Remote";
    second.minimum_years_experience = 0;
    second.skills = {"Python", "SQL"};
    second.description =
        "Develop Python APIs and database integrations.";
    second.application_url = "https://example.com/jobs/orbit-demo-2";
    second.source_name = name();
    second.external_job_id = "ORBIT-DEMO-002";
    second.original_url = second.application_url;

    return {first, second};
}

} // namespace orbit