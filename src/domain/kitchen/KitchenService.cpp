#include "domain/kitchen/KitchenService.h"
#include "domain/order/Order.h"

#include <algorithm>

namespace pos::domain {

void KitchenService::enqueue(const Order& o) {
    tickets_.push_back(KitchenTicket::fromOrder(o));
}

void KitchenService::rehydrate(const std::vector<Order>& activeOrders) {
    tickets_.clear();
    for (const auto& o : activeOrders) {
        if (o.status() != OrderStatus::Served && o.status() != OrderStatus::Cancelled) {
            tickets_.push_back(KitchenTicket::fromOrder(o));
        }
    }
}

void KitchenService::advance(OrderId id) {
    for (auto it = tickets_.begin(); it != tickets_.end(); ++it) {
        if (it->orderId == id) {
            switch (it->status) {
                case OrderStatus::Pending:   it->status = OrderStatus::Preparing; return;
                case OrderStatus::Preparing: it->status = OrderStatus::Ready;     return;
                case OrderStatus::Ready:     tickets_.erase(it);                  return;
                default: return;
            }
        }
    }
}

void KitchenService::setStatus(OrderId id, OrderStatus s) {
    if (s == OrderStatus::Served || s == OrderStatus::Cancelled) {
        tickets_.erase(std::remove_if(tickets_.begin(), tickets_.end(),
            [&](const KitchenTicket& t){ return t.orderId == id; }), tickets_.end());
        return;
    }
    for (auto& t : tickets_) if (t.orderId == id) { t.status = s; return; }
}

std::vector<KitchenTicket> KitchenService::lane(OrderStatus s) const {
    std::vector<KitchenTicket> out;
    for (const auto& t : tickets_) if (t.status == s) out.push_back(t);
    return out;
}

} // namespace pos::domain
