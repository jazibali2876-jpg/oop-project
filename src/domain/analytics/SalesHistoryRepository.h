#pragma once

#include "domain/analytics/SalesRecord.h"
#include "domain/persistence/BinaryRepository.h"

namespace pos::domain {

class SalesHistoryRepository : public BinaryRepository<SalesRecord> {
public:
    SalesHistoryRepository();
};

} // namespace pos::domain
