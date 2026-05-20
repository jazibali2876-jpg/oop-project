#pragma once

#include "domain/auth/User.h"

namespace pos::domain {

class Cashier final : public User {
public:
    using User::User;

    bool can(Capability c) const override {
        switch (c) {
            case Capability::ViewMenu:
            case Capability::PlaceOrder:
            case Capability::ManageTables:
            case Capability::ManageCustomers:
                return true;
            default:
                return false;
        }
    }
    std::string roleName() const override { return "Cashier"; }
    Role        role() const override { return Role::Cashier; }
};

} // namespace pos::domain
