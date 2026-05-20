#pragma once

#include "domain/customer/Customer.h"
#include "domain/customer/CustomerRepository.h"
#include "domain/customer/LoyaltyRepository.h"
#include "domain/customer/LoyaltyRule.h"

#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace pos::domain {

class OrderRepository;
class Order;

class CustomerService {
public:
    CustomerService(CustomerRepository& repo, LoyaltyRepository& loyaltyRepo);

    void                       load();
    const std::vector<Customer>& all() const { return customers_; }

    std::optional<Customer>    findById(CustomerId id) const;
    std::optional<Customer>    findByPhone(const std::string& phone) const;

    Customer                   findByPhoneOrCreate(const std::string& phone,
                                                   const std::string& nameIfNew = "");

    std::vector<Customer>      search(std::string_view query) const;

    // Adds points + spend for a completed order. Appends a loyalty ledger entry.
    void                       applyLoyalty(CustomerId id, Money orderTotal, OrderId orderId);

    void                       upsert(const Customer& c);

    const LoyaltyRule&         rule() const { return rule_; }

    std::vector<Order>         history(CustomerId id, OrderRepository& orders) const;

private:
    CustomerRepository&      repo_;
    LoyaltyRepository&       loyaltyRepo_;
    LoyaltyRule              rule_;
    std::vector<Customer>    customers_;
};

} // namespace pos::domain
