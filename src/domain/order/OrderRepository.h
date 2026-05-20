#pragma once

#include "domain/order/Order.h"

#include <cstdint>
#include <optional>
#include <string>
#include <type_traits>
#include <vector>

namespace pos::domain {

// Variable-length binary file: each order = one header record + N item records.
// Records are POD; itemCount in the header tells the reader how many items follow.
#pragma pack(push, 1)
struct OrderHeaderRecord {
    std::uint32_t id;                       //  4
    std::int64_t  placedAtEpoch;            //  8
    std::uint16_t itemCount;                //  2
    std::uint8_t  status;                   //  1
    std::uint8_t  paymentMethod;            //  1   255 = unpaid
    std::int64_t  discountCents;            //  8
    std::uint16_t taxBp;                    //  2   tax rate in basis points
    std::uint32_t customerId;               //  4   0 if no customer
    char          cashierUsername[24];      // 24
    char          specialInstructions[64];  // 64
    std::int64_t  totalCents;               //  8
    std::uint8_t  pad[2];                   //  2   -> total 128
};

struct OrderItemRecord {
    std::uint32_t menuItemId;       //  4
    std::uint16_t qty;              //  2
    std::int64_t  unitPriceCents;   //  8
    char          notes[40];        // 40
    std::uint8_t  pad[2];           //  2   -> total 56
};
#pragma pack(pop)
static_assert(sizeof(OrderHeaderRecord) == 128, "OrderHeaderRecord layout drift");
static_assert(sizeof(OrderItemRecord)   == 56,  "OrderItemRecord layout drift");
static_assert(std::is_trivially_copyable_v<OrderHeaderRecord>);
static_assert(std::is_trivially_copyable_v<OrderItemRecord>);

class OrderRepository {
public:
    OrderRepository();
    const std::string& path() const { return path_; }

    std::vector<Order>     loadAll() const;
    std::optional<Order>   byId(OrderId id) const;
    void                   append(const Order& order);
    void                   overwriteAll(const std::vector<Order>& orders);

    std::uint32_t          nextId() const;

private:
    static OrderHeaderRecord headerOf(const Order& o);
    static OrderItemRecord   itemOf(const OrderItem& i);
    static OrderItem         itemFrom(const OrderItemRecord& r);
    static Order             orderFrom(const OrderHeaderRecord& h,
                                       const std::vector<OrderItemRecord>& items);

    std::string path_;
};

} // namespace pos::domain
