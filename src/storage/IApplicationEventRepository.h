
#pragma once

#include "../core/ApplicationEvent.h"

#include <vector>

namespace orbit {

class IApplicationEventRepository {
public:
    virtual ~IApplicationEventRepository() = default;

    virtual void save(
        const ApplicationEvent& event
    ) = 0;

    virtual std::vector<ApplicationEvent> find_by_application(
        ApplicationId application_id
    ) const = 0;
};

} // namespace orbit
