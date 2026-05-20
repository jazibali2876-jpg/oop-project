#pragma once

#include "domain/billing/PaymentMethod.h"
#include "domain/common/DateTime.h"
#include "domain/common/Id.h"
#include "domain/common/Money.h"
#include "domain/order/OrderItem.h"

#include <iosfwd>
#include <string>
#include <vector>

namespace pos::domain {

class Receipt {
public:
    Receipt() = default;

    OrderId             orderId() const         { return orderId_; }
    const std::string&  customerName() const    { return customerName_; }
    const std::vector<OrderItem>& items() const { return items_; }
    Money               subtotal() const        { return subtotal_; }
    Money               discount() const        { return discount_; }
    Money               tax() const             { return tax_; }
    double              taxRate() const         { return taxRate_; }
    Money               total() const           { return total_; }
    PaymentMethod       method() const          { return method_; }
    DateTime            issuedAt() const        { return issuedAt_; }
    const std::string&  cashier() const         { return cashier_; }
    int                 loyaltyEarned() const   { return loyaltyEarned_; }
    const std::string&  loyaltyTier() const     { return loyaltyTier_; }

    void setOrderId(OrderId id)                          { orderId_ = id; }
    void setCustomerName(std::string n)                  { customerName_ = std::move(n); }
    void setItems(std::vector<OrderItem> v)              { items_ = std::move(v); }
    void setSubtotal(Money m)                            { subtotal_ = m; }
    void setDiscount(Money m)                            { discount_ = m; }
    void setTax(Money m)                                 { tax_ = m; }
    void setTaxRate(double r)                            { taxRate_ = r; }
    void setTotal(Money m)                               { total_ = m; }
    void setMethod(PaymentMethod m)                      { method_ = m; }
    void setIssuedAt(DateTime t)                         { issuedAt_ = t; }
    void setCashier(std::string c)                       { cashier_ = std::move(c); }
    void setLoyalty(int points, std::string tier)        { loyaltyEarned_ = points;
                                                           loyaltyTier_   = std::move(tier); }

    // Operator overloading: thermal-style 44-column layout.
    friend std::ostream& operator<<(std::ostream& os, const Receipt& r);

private:
    OrderId                orderId_{};
    std::string            customerName_;
    std::vector<OrderItem> items_;
    Money                  subtotal_;
    Money                  discount_;
    Money                  tax_;
    double                 taxRate_ = 0.10;
    Money                  total_;
    PaymentMethod          method_ = PaymentMethod::Unpaid;
    DateTime               issuedAt_;
    std::string            cashier_;
    int                    loyaltyEarned_ = 0;
    std::string            loyaltyTier_;
};

} // namespace pos::domain
