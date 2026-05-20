#pragma once

#include "domain/common/Capability.h"
#include "domain/common/DateTime.h"
#include "domain/common/Id.h"

#include <string>

namespace pos::domain {

enum class Role : std::uint8_t {
    Admin   = 0,
    Cashier = 1,
    Kitchen = 2
};

// Abstract base. Concrete subclasses Admin/Cashier/Kitchen override can/roleName.
class User {
public:
    User() = default;
    User(UserId id, std::string username, std::string passwordHash, std::string salt,
         std::string fullName, DateTime createdAt, bool active);
    virtual ~User() = default;

    UserId             id() const            { return id_; }
    const std::string& username() const      { return username_; }
    const std::string& passwordHash() const  { return passwordHash_; }
    const std::string& salt() const          { return salt_; }
    const std::string& fullName() const      { return fullName_; }
    DateTime           createdAt() const     { return createdAt_; }
    bool               active() const        { return active_; }

    void setActive(bool v) { active_ = v; }
    void setPasswordHash(std::string h, std::string s) {
        passwordHash_ = std::move(h);
        salt_         = std::move(s);
    }
    void setFullName(std::string f) { fullName_ = std::move(f); }

    virtual bool        can(Capability c) const = 0;
    virtual std::string roleName() const = 0;
    virtual Role        role() const = 0;

    std::string displayName() const {
        return fullName_.empty() ? username_ : fullName_;
    }

protected:
    UserId      id_{};
    std::string username_;
    std::string passwordHash_;
    std::string salt_;
    std::string fullName_;
    DateTime    createdAt_;
    bool        active_ = true;
};

} // namespace pos::domain
