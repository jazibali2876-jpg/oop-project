#include "domain/customer/LoyaltyRepository.h"
#include "domain/persistence/FilePaths.h"

namespace pos::domain {

LoyaltyRepository::LoyaltyRepository()
    : BinaryRepository<LoyaltyRecord>(FilePaths::Loyalty) {}

} // namespace pos::domain
