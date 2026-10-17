#pragma once

#include <cstddef>
#include <vector>

namespace orbit {

template <typename T>
struct Page {
    std::vector<T> items;
    std::size_t offset{0};
    std::size_t limit{0};
    bool has_more{false};
};

} // namespace orbit