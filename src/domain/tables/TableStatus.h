#pragma once

#include <cstdint>
#include <string>
#include <string_view>

namespace pos::domain {

enum class TableStatus : std::uint8_t {
    Free     = 0,
    Occupied = 1,
    Reserved = 2
};

std::string tableStatusName(TableStatus s);
bool        parseTableStatus(std::string_view s, TableStatus& out);

} // namespace pos::domain
