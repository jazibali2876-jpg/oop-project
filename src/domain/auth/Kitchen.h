#pragma once

#include "domain/auth/User.h"

namespace pos::domain {

class Kitchen final : public User {
public:
    using User::User;
    bool can(Capability c) const override {
        return c == Capability::ViewKitchen || c == Capability::AdvanceTicket;
    }
    std::string roleName() const override { return "Kitchen"; }
    Role        role() const override { return Role::Kitchen; }
};

} // namespace pos::domain
