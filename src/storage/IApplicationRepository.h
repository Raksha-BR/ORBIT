
#pragma once

#include "../core/Application.h"

#include <optional>
#include <vector>

namespace orbit {

class IApplicationRepository {
public:
    virtual ~IApplicationRepository() = default;

    virtual void save(
        const Application& application
    ) = 0;

    virtual std::optional<Application> find(
        ApplicationId id
    ) const = 0;

    virtual std::vector<Application> find_all() const = 0;
};

} // namespace orbit
