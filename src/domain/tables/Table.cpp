#include "domain/tables/Table.h"

namespace pos::domain {

Table::Table(TableId id, int seats, TableStatus status,
             std::optional<CustomerId> reservedFor, DateTime reservedAt)
    : id_(id), seats_(seats), status_(status),
      reservedFor_(reservedFor), reservedAt_(reservedAt) {}

std::string tableStatusName(TableStatus s) {
    switch (s) {
        case TableStatus::Free:     return "Free";
        case TableStatus::Occupied: return "Occupied";
        case TableStatus::Reserved: return "Reserved";
    }
    return "Free";
}

bool parseTableStatus(std::string_view s, TableStatus& out) {
    if (s == "Free")     { out = TableStatus::Free;     return true; }
    if (s == "Occupied") { out = TableStatus::Occupied; return true; }
    if (s == "Reserved") { out = TableStatus::Reserved; return true; }
    return false;
}

} // namespace pos::domain
