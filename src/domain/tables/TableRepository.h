#pragma once

#include "domain/persistence/TextRepository.h"
#include "domain/tables/Table.h"

#include <vector>

namespace pos::domain {

class TableRepository : public TextRepository {
public:
    TableRepository();
    std::vector<Table> loadAll() const;
    void               saveAll(const std::vector<Table>& tables);
};

} // namespace pos::domain
