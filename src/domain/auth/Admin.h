#pragma once

#include "domain/auth/User.h"

namespace pos::domain {

class Admin final : public User {
public:
    using User::User;
    bool        can(Capability) const override { return true; }
    std::string roleName() const override { return "Admin"; }
    Role        role() const override { return Role::Admin; }
};

} // namespace pos::domain
