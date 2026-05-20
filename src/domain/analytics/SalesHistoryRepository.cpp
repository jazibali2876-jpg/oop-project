#include "domain/analytics/SalesHistoryRepository.h"
#include "domain/persistence/FilePaths.h"

namespace pos::domain {

SalesHistoryRepository::SalesHistoryRepository()
    : BinaryRepository<SalesRecord>(FilePaths::SalesHistory) {}

} // namespace pos::domain
