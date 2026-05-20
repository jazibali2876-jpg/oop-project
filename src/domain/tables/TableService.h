#pragma once

#include "domain/tables/Table.h"
#include "domain/tables/TableRepository.h"

#include <vector>

namespace pos::domain {

class TableService {
public:
    explicit TableService(TableRepository& repo);

    void                       load();
    void                       persist();
    void                       seedDefaultsIfEmpty();

    const std::vector<Table>&  all() const { return tables_; }

    void                       reserve(TableId id, CustomerId forWhom);
    void                       occupy(TableId id);
    void                       free(TableId id);

    std::uint32_t              nextId() const;

private:
    Table&                     get(TableId id); // throws NotFoundException
    TableRepository&           repo_;
    std::vector<Table>         tables_;
};

} // namespace pos::domain
