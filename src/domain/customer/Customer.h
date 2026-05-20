#pragma once

#include "domain/common/DateTime.h"
#include "domain/common/Id.h"
#include "domain/common/Money.h"

#include <string>

namespace pos::domain {

class Customer {
public:
    Customer() = default;
    Customer(CustomerId id, std::string name, std::string phone,
             DateTime joinedAt, Money totalSpent, int loyaltyPoints, int orderCount);

    CustomerId         id() const             { return id_; }
    const std::string& name() const           { return name_; }
    const std::string& phone() const          { return phone_; }
    DateTime           joinedAt() const       { return joinedAt_; }
    Money              totalSpent() const     { return totalSpent_; }
    int                loyaltyPoints() const  { return loyaltyPoints_; }
    int                orderCount() const     { return orderCount_; }

    void setName(std::string n)           { name_ = std::move(n); }
    void setPhone(std::string p)          { phone_ = std::move(p); }
    void addPoints(int p)                 { loyaltyPoints_ += p; }
    void recordOrder(Money orderTotal)    { totalSpent_ += orderTotal; ++orderCount_; }

    std::string tier() const;

private:
    CustomerId  id_{};
    std::string name_;
    std::string phone_;
    DateTime    joinedAt_;
    Money       totalSpent_;
    int         loyaltyPoints_ = 0;
    int         orderCount_    = 0;
};

} // namespace pos::domain
