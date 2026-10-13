#pragma once

namespace orbit {

class ITransactionManager {
public:
    virtual ~ITransactionManager() = default;

    virtual void begin() = 0;
    virtual void commit() = 0;
    virtual void rollback() noexcept = 0;
};

} // namespace orbit