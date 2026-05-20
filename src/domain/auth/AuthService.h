#pragma once

#include "domain/auth/User.h"
#include "domain/auth/UserRepository.h"
#include "domain/auth/Session.h"

#include <memory>
#include <string>

namespace pos::domain {

class AuthService {
public:
    explicit AuthService(UserRepository& repo);

    // Throws InvalidCredentialsException on failure. Caller assigns the result
    // into a Session via Session::set(...).
    std::unique_ptr<User> login(const std::string& username, const std::string& password);

    void logout(Session& session);

    // Admin-only operation — caller must enforce. Creates a new user with the
    // chosen role; password is salted and hashed before persistence.
    void registerUser(Role role,
                      const std::string& username,
                      const std::string& password,
                      const std::string& fullName);

    // Seeds a default admin if no users exist. Returns true if seeding happened.
    bool seedDefaultAdminIfEmpty(const std::string& username = "admin",
                                 const std::string& password = "admin123");

private:
    UserRepository& repo_;
};

} // namespace pos::domain
