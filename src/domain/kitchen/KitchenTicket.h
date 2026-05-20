#pragma once

#include "domain/common/DateTime.h"
#include "domain/common/Id.h"
#include "domain/order/OrderItem.h"
#include "domain/order/OrderStatus.h"

#include <string>
#include <vector>

namespace pos::domain {

class Order;

struct KitchenTicket {
    OrderId                 orderId;
    DateTime                queuedAt;
    OrderStatus             status = OrderStatus::Pending;
    std::vector<OrderItem>  items;
    std::string             specialInstructions;
    std::string             cashier;

    static KitchenTicket fromOrder(const Order& o);
};

} // namespace pos::domain
