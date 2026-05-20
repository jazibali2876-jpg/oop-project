#pragma once

#include <cstdint>
#include <string>

namespace pos::domain {

enum class OrderStatus : std::uint8_t {
    Pending   = 0,
    Preparing = 1,
    Ready     = 2,
    Served    = 3,
    Cancelled = 4
};

std::string orderStatusName(OrderStatus s);

} // namespace pos::domain
