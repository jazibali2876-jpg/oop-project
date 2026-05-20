#pragma once

#include "domain/common/DateTime.h"
#include "domain/common/Id.h"
#include "domain/common/Money.h"
#include "domain/order/OrderItem.h"
#include "domain/order/OrderStatus.h"

#include <cstddef>
#include <iosfwd>
#include <optional>
#include <string>
#include <vector>

namespace pos::domain {

class Order {
public:
    Order() = default;
    explicit Order(OrderId id) : id_(id), placedAt_(DateTime::now()) {}

    OrderId   id() const                                { return id_; }
    DateTime  placedAt() const                          { return placedAt_; }
    OrderStatus status() const                          { return status_; }
    Money     discount() const                          { return discount_; }
    double    taxRate() const                           { return taxRate_; }
    const std::optional<CustomerId>& customer() const   { return customer_; }
    const std::string& cashierUsername() const          { return cashierUsername_; }
    const std::string& specialInstructions() const      { return specialInstructions_; }
    const std::vector<OrderItem>& items() const         { return items_; }

    void setId(OrderId id)                  { id_ = id; }
    void setPlacedAt(DateTime t)            { placedAt_ = t; }
    void setDiscount(Money m)               { discount_ = m; }
    void setTaxRate(double r)               { taxRate_ = r; }
    void setStatus(OrderStatus s)           { status_ = s; }
    void setCustomer(std::optional<CustomerId> c) { customer_ = c; }
    void setCashier(std::string u)          { cashierUsername_ = std::move(u); }
    void setSpecialInstructions(std::string s) { specialInstructions_ = std::move(s); }

    void add(const OrderItem& item)         { items_.push_back(item); }
    void removeAt(std::size_t idx)          { if (idx < items_.size()) items_.erase(items_.begin() + idx); }
    void clearItems()                       { items_.clear(); }

    bool   empty() const                    { return items_.empty(); }
    int    totalQty() const;
    Money  subtotal() const;
    Money  tax() const;
    Money  total() const;
    int    estimatedPrepMinutes() const;

    // Operator overloading for OOP demonstration.
    Order& operator+=(const OrderItem& item) { add(item); return *this; }

    friend std::ostream& operator<<(std::ostream& os, const Order& o);

private:
    OrderId                     id_{};
    DateTime                    placedAt_;
    std::vector<OrderItem>      items_;
    Money                       discount_;
    double                      taxRate_ = 0.10;     // default 10%
    OrderStatus                 status_  = OrderStatus::Pending;
    std::optional<CustomerId>   customer_;
    std::string                 cashierUsername_;
    std::string                 specialInstructions_;
};

} // namespace pos::domain
