#pragma once

#include "domain/order/Order.h"
#include "domain/order/OrderRepository.h"

#include <optional>
#include <string>
#include <vector>

namespace pos::domain {

class InventoryService;
class KitchenService;
class SalesHistoryRepository;
class User;

class OrderService {
public:
    OrderService(OrderRepository& orders,
                 InventoryService& inv,
                 KitchenService& kit,
                 SalesHistoryRepository& sales);

    // Places a new order: validates, checks stock, deducts inventory, persists,
    // enqueues a kitchen ticket, appends a sales record. Returns the persisted order.
    // Throws: EmptyOrderException, InsufficientStockException, FileIOException.
    Order place(Order draft, const User& by, std::optional<CustomerId> customer = std::nullopt);

    void  cancel(OrderId id, const User& by);
    void  setStatus(OrderId id, OrderStatus s, const User& by);

    std::vector<Order>   active() const;
    std::optional<Order> byId(OrderId id) const;

private:
    OrderRepository&         orders_;
    InventoryService&        inv_;
    KitchenService&          kit_;
    SalesHistoryRepository&  sales_;
};

} // namespace pos::domain
