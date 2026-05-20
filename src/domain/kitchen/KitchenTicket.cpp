#include "domain/kitchen/KitchenTicket.h"
#include "domain/order/Order.h"

namespace pos::domain {

KitchenTicket KitchenTicket::fromOrder(const Order& o) {
    KitchenTicket t;
    t.orderId             = o.id();
    t.queuedAt            = o.placedAt();
    t.status              = o.status();
    t.items               = o.items();
    t.specialInstructions = o.specialInstructions();
    t.cashier             = o.cashierUsername();
    return t;
}

} // namespace pos::domain
