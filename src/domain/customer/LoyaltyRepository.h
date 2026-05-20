#pragma once

#include "domain/persistence/BinaryRepository.h"

#include <cstdint>
#include <type_traits>

namespace pos::domain {

#pragma pack(push, 1)
struct LoyaltyRecord {
    std::uint32_t customerId;  // 4
    std::uint32_t orderId;     // 4
    std::int64_t  atEpoch;     // 8
    std::int32_t  pointsDelta; // 4   may be negative for redemptions
    std::int64_t  spendCents;  // 8
    std::uint8_t  reason;      // 1   0=Earn, 1=Redeem, 2=Adjust
    std::uint8_t  pad[3];      // 3   -> total 32
};
#pragma pack(pop)
static_assert(sizeof(LoyaltyRecord) == 32, "LoyaltyRecord layout drift");
static_assert(std::is_trivially_copyable_v<LoyaltyRecord>);

class LoyaltyRepository : public BinaryRepository<LoyaltyRecord> {
public:
    LoyaltyRepository();
};

} // namespace pos::domain
