#include "domain/auth/AuthService.h"
#include "domain/auth/PasswordHash.h"
#include "domain/auth/Admin.h"
#include "domain/auth/Cashier.h"
#include "domain/auth/Kitchen.h"
#include "domain/common/Exceptions.h"
#include "util/Random.h"

namespace pos::domain {

AuthService::AuthService(UserRepository& repo) : repo_(repo) {}

std::unique_ptr<User> AuthService::login(const std::string& username, const std::string& password) {
    auto found = repo_.findByUsername(username);
    if (!found) throw InvalidCredentialsException();
    auto& u = *found;
    if (!u || !u->active()) throw InvalidCredentialsException();
    if (!PasswordHash::verify(u->passwordHash(), u->salt(), password)) {
        throw InvalidCredentialsException();
    }
    return std::move(u);
}

void AuthService::logout(Session& session) {
    session.clear();
}

void AuthService::registerUser(Role role,
                               const std::string& username,
                               const std::string& password,
                               const std::string& fullName) {
    if (repo_.findByUsername(username).has_value()) {
        throw DuplicateIdException("username '" + username + "'");
    }
    std::string salt = pos::util::Random::salt(8);
    std::string hash = PasswordHash::hash(password, salt);
    UserId id{repo_.nextId()};
    auto createdAt = DateTime::now();
    std::unique_ptr<User> u;
    switch (role) {
        case Role::Admin:
            u = std::make_unique<Admin>(id, username, hash, salt, fullName, createdAt, true);
            break;
        case Role::Cashier:
            u = std::make_unique<Cashier>(id, username, hash, salt, fullName, createdAt, true);
            break;
        case Role::Kitchen:
            u = std::make_unique<Kitchen>(id, username, hash, salt, fullName, createdAt, true);
            break;
    }
    repo_.upsert(*u);
}

bool AuthService::seedDefaultAdminIfEmpty(const std::string& username,
                                          const std::string& password) {
    if (!repo_.all().empty()) return false;
    registerUser(Role::Admin, username, password, "Default Admin");
    return true;
}

bool AuthService::seedDemoUsersIfEmpty() {
    bool anyCreated = false;
    auto tryCreate = [&](Role role, const std::string& u, const std::string& p,
                         const std::string& full) {
        if (repo_.findByUsername(u).has_value()) return;
        registerUser(role, u, p, full);
        anyCreated = true;
    };
    tryCreate(Role::Admin,   "admin",   "admin123",   "Default Admin");
    tryCreate(Role::Cashier, "cashier", "cashier123", "Demo Cashier");
    tryCreate(Role::Kitchen, "kitchen", "kitchen123", "Demo Kitchen Staff");
    return anyCreated;
}

} // namespace pos::domain
