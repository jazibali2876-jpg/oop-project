#pragma once

#include "domain/kitchen/KitchenTicket.h"

#include <vector>

namespace pos::domain {

class Order;

class KitchenService {
public:
    void enqueue(const Order& o);

    // Hydrate the in-memory queue from already-persisted active orders on startup.
    void rehydrate(const std::vector<Order>& activeOrders);

    // Advance Pending -> Preparing -> Ready -> Served. Served removes the ticket.
    void advance(OrderId id);

    // Mark explicit status (used by Cancel from elsewhere).
    void setStatus(OrderId id, OrderStatus s);

    const std::vector<KitchenTicket>& tickets() const { return tickets_; }
    std::vector<KitchenTicket>        lane(OrderStatus s) const;

private:
    std::vector<KitchenTicket> tickets_;
};

} // namespace pos::domain
