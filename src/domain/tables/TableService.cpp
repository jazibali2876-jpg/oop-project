#include "domain/tables/TableService.h"
#include "domain/common/Exceptions.h"

namespace pos::domain {

TableService::TableService(TableRepository& repo) : repo_(repo) {}

void TableService::load() {
    tables_ = repo_.loadAll();
}

void TableService::persist() {
    repo_.saveAll(tables_);
}

void TableService::seedDefaultsIfEmpty() {
    if (!tables_.empty()) return;
    const int seatPlan[] = {2, 4, 4, 6, 2, 4, 2, 8, 4, 4};
    for (int i = 0; i < 10; ++i) {
        tables_.emplace_back(TableId{static_cast<std::uint32_t>(i + 1)}, seatPlan[i]);
    }
    persist();
}

Table& TableService::get(TableId id) {
    for (auto& t : tables_) if (t.id() == id) return t;
    throw NotFoundException("table #" + std::to_string(id.value()));
}

void TableService::reserve(TableId id, CustomerId forWhom) {
    auto& t = get(id);
    t.setStatus(TableStatus::Reserved);
    t.setReservedFor(forWhom);
    t.setReservedAt(DateTime::now());
    persist();
}

void TableService::occupy(TableId id) {
    auto& t = get(id);
    t.setStatus(TableStatus::Occupied);
    t.setReservedFor(std::nullopt);
    persist();
}

void TableService::free(TableId id) {
    auto& t = get(id);
    t.setStatus(TableStatus::Free);
    t.setReservedFor(std::nullopt);
    t.setReservedAt(DateTime{});
    persist();
}

std::uint32_t TableService::nextId() const {
    std::uint32_t m = 0;
    for (const auto& t : tables_) if (t.id().value() > m) m = t.id().value();
    return m + 1;
}

} // namespace pos::domain
