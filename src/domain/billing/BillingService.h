#pragma once

#include "domain/billing/Receipt.h"
#include "domain/billing/ReceiptRepository.h"

#include <string>

namespace pos::domain {

class Order;
class MenuService;
class CustomerService;
class OrderRepository;

class BillingService {
public:
    BillingService(ReceiptRepository& receipts,
                   OrderRepository& orders,
                   MenuService& menu,
                   CustomerService& customers);

    Receipt    makeReceipt(const Order& o) const;

    // Records the payment method on the order and the customer's loyalty points,
    // then returns an updated Receipt.
    Receipt    recordPayment(Order& o, PaymentMethod method);

    std::string saveReceiptFile(const Receipt& r);

private:
    ReceiptRepository& receipts_;
    OrderRepository&   orders_;
    MenuService&       menu_;
    CustomerService&   customers_;
};

} // namespace pos::domain
