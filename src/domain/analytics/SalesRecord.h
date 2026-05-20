#pragma once

#include <cstdint>
#include <type_traits>

namespace pos::domain {

#pragma pack(push, 1)
struct SalesRecord {
    std::uint32_t orderId;       //  4
    std::int64_t  epoch;         //  8
    std::int64_t  totalCents;    //  8
    std::int16_t  itemCount;     //  2
    std::uint8_t  paymentMethod; //  1
    std::uint8_t  hour;          //  1   0..23
    char          cashier[24];   // 24
    std::int32_t  customerId;    //  4   0 if none
    std::uint8_t  pad[12];       // 12   -> total 64
};
#pragma pack(pop)
static_assert(sizeof(SalesRecord) == 64, "SalesRecord layout drift");
static_assert(std::is_trivially_copyable_v<SalesRecord>);

} // namespace pos::domain
