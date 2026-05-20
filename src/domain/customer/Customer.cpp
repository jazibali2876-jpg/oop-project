#include "domain/customer/Customer.h"

namespace pos::domain {

Customer::Customer(CustomerId id, std::string name, std::string phone,
                   DateTime joinedAt, Money totalSpent, int loyaltyPoints, int orderCount)
    : id_(id), name_(std::move(name)), phone_(std::move(phone)),
      joinedAt_(joinedAt), totalSpent_(totalSpent),
      loyaltyPoints_(loyaltyPoints), orderCount_(orderCount) {}

std::string Customer::tier() const {
    if (loyaltyPoints_ >= 500) return "Gold";
    if (loyaltyPoints_ >= 200) return "Silver";
    if (loyaltyPoints_ >= 50)  return "Bronze";
    return "New";
}

} // namespace pos::domain
