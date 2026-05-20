#pragma once

#include "domain/common/Id.h"
#include "domain/common/Money.h"
#include "domain/order/Order.h"
#include "domain/order/OrderItem.h"

#include <optional>
#include <string>
#include <vector>

namespace pos::ui {

// Transient cart state held by the Order screen. Becomes a real domain::Order
// only when the cashier taps "Place Order".
class OrderDraft {
public:
    void  addItem(pos::domain::MenuItemId id, pos::domain::Money price, const std::string& name);
    void  increment(std::size_t idx);
    void  decrement(std::size_t idx);
    void  removeAt(std::size_t idx);
    void  clear();

    const std::vector<pos::domain::OrderItem>& items() const { return items_; }
    const std::vector<std::string>&            names() const { return names_; }

    pos::domain::Money discount() const           { return discount_; }
    void               setDiscount(pos::domain::Money m) { discount_ = m; }

    double taxRate() const                        { return taxRate_; }
    void   setTaxRate(double r)                   { taxRate_ = r; }

    std::optional<pos::domain::CustomerId> customer() const { return customer_; }
    void  setCustomer(std::optional<pos::domain::CustomerId> c) { customer_ = c; }

    const std::string& specialInstructions() const             { return specialInstructions_; }
    void  setSpecialInstructions(const std::string& s)         { specialInstructions_ = s; }

    pos::domain::Money subtotal() const;
    pos::domain::Money tax() const;
    pos::domain::Money total() const;
    int                totalQty() const;

    // Materialize into a domain Order ready to pass to OrderService::place.
    pos::domain::Order toOrder() const;

    bool empty() const { return items_.empty(); }

private:
    std::vector<pos::domain::OrderItem> items_;
    std::vector<std::string>            names_;       // parallel to items_
    pos::domain::Money                  discount_;
    double                              taxRate_ = 0.10;
    std::optional<pos::domain::CustomerId> customer_;
    std::string                         specialInstructions_;
};

} // namespace pos::ui
