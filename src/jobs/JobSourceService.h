#pragma once

#include "jobs/IJobSource.h"
#include "jobs/JobSourceResult.h"
#include "jobs/JobSearchService.h"

#include <vector>

namespace orbit {

class JobSourceService {
public:
    explicit JobSourceService(JobSearchService& search_service);

    JobSourceResult collect(IJobSource& source);

private:
    JobSearchService& search_service_;
};

} // namespace orbit