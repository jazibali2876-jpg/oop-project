#include "domain/order/OrderService.h"
#include "domain/auth/User.h"
#include "domain/common/Exceptions.h"
#include "domain/inventory/InventoryService.h"
#include "domain/kitchen/KitchenService.h"
#include "domain/analytics/SalesHistoryRepository.h"
#include "domain/analytics/SalesRecord.h"

#include <algorithm>
#include <cstring>

namespace pos::domain {

OrderService::OrderService(OrderRepository& orders,
                           InventoryService& inv,
                           KitchenService& kit,
                           SalesHistoryRepository& sales)
    : orders_(orders), inv_(inv), kit_(kit), sales_(sales) {}

Order OrderService::place(Order draft, const User& by, std::optional<CustomerId> customer) {
    if (!by.can(Capability::PlaceOrder)) {
        throw UnauthorizedException(Capability::PlaceOrder);
    }
    if (draft.empty()) throw EmptyOrderException();

    // canFulfill check is informational; deduct() does the authoritative check.
    inv_.deduct(draft);

    draft.setId(OrderId{orders_.nextId()});
    draft.setPlacedAt(DateTime::now());
    draft.setStatus(OrderStatus::Pending);
    draft.setCashier(by.username());
    draft.setCustomer(customer);

    orders_.append(draft);

    SalesRecord sr{};
    sr.orderId       = draft.id().value();
    sr.epoch         = draft.placedAt().epoch();
    sr.totalCents    = draft.total().cents();
    sr.itemCount     = static_cast<std::int16_t>(draft.totalQty());
    sr.paymentMethod = 255; // unpaid; billing updates later
    sr.hour          = static_cast<std::uint8_t>(draft.placedAt().hour());
    sr.customerId    = customer ? static_cast<std::int32_t>(customer->value()) : 0;
    {
        std::memset(sr.cashier, 0, sizeof(sr.cashier));
        const auto& u = by.username();
        std::size_t n = std::min(u.size(), static_cast<std::size_t>(sizeof(sr.cashier) - 1));
        std::memcpy(sr.cashier, u.data(), n);
    }
    sales_.append(sr);

    kit_.enqueue(draft);
    return draft;
}

void OrderService::cancel(OrderId id, const User& by) {
    if (!by.can(Capability::PlaceOrder)) {
        throw UnauthorizedException(Capability::PlaceOrder);
    }
    auto orders = orders_.loadAll();
    bool found = false;
    for (auto& o : orders) {
        if (o.id() == id) { o.setStatus(OrderStatus::Cancelled); found = true; break; }
    }
    if (!found) throw NotFoundException("order #" + std::to_string(id.value()));
    orders_.overwriteAll(orders);
    kit_.setStatus(id, OrderStatus::Cancelled);
}

void OrderService::setStatus(OrderId id, OrderStatus s, const User& by) {
    if (!by.can(Capability::AdvanceTicket) && !by.can(Capability::PlaceOrder)) {
        throw UnauthorizedException(Capability::AdvanceTicket);
    }
    auto orders = orders_.loadAll();
    bool found = false;
    for (auto& o : orders) {
        if (o.id() == id) { o.setStatus(s); found = true; break; }
    }
    if (!found) throw NotFoundException("order #" + std::to_string(id.value()));
    orders_.overwriteAll(orders);
    kit_.setStatus(id, s);
}

std::vector<Order> OrderService::active() const {
    std::vector<Order> out;
    for (auto& o : orders_.loadAll()) {
        if (o.status() != OrderStatus::Served && o.status() != OrderStatus::Cancelled) {
            out.push_back(o);
        }
    }
    return out;
}

std::optional<Order> OrderService::byId(OrderId id) const {
    return orders_.byId(id);
}

} // namespace pos::domain
