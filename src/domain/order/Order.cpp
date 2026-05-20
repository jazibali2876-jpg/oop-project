#include "domain/order/Order.h"

#include <algorithm>
#include <ostream>

namespace pos::domain {

int Order::totalQty() const {
    int q = 0;
    for (const auto& it : items_) q += it.qty();
    return q;
}

Money Order::subtotal() const {
    Money s;
    for (const auto& it : items_) s += it.lineTotal();
    return s;
}

Money Order::tax() const {
    Money base = subtotal() - discount_;
    if (base < Money{}) base = Money{};
    return base.withTax(taxRate_);
}

Money Order::total() const {
    Money base = subtotal() - discount_ + tax();
    if (base < Money{}) base = Money{};
    return base;
}

int Order::estimatedPrepMinutes() const {
    // 2 minutes per item, plus 3 minute base, max 30.
    int q = totalQty();
    int mins = 3 + 2 * q;
    return std::min(mins, 30);
}

std::ostream& operator<<(std::ostream& os, const Order& o) {
    os << "Order #" << o.id_.value()
       << " items=" << o.items_.size()
       << " subtotal=" << o.subtotal()
       << " total=" << o.total()
       << " status=" << static_cast<int>(o.status_);
    return os;
}

std::string orderStatusName(OrderStatus s) {
    switch (s) {
        case OrderStatus::Pending:   return "Pending";
        case OrderStatus::Preparing: return "Preparing";
        case OrderStatus::Ready:     return "Ready";
        case OrderStatus::Served:    return "Served";
        case OrderStatus::Cancelled: return "Cancelled";
    }
    return "Unknown";
}

} // namespace pos::domain
