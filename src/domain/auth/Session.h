#pragma once

#include "domain/auth/User.h"

#include <memory>

namespace pos::domain {

class Session {
public:
    bool isAuthenticated() const { return static_cast<bool>(currentUser_); }

    const User& user() const;          // throws DomainException if not authenticated
    User&       user();

    void set(std::unique_ptr<User> u);
    void clear();

    DateTime loginAt() const { return loginAt_; }

private:
    std::unique_ptr<User> currentUser_;
    DateTime              loginAt_;
};

} // namespace pos::domain
