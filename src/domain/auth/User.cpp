#include "domain/auth/User.h"

namespace pos::domain {

User::User(UserId id, std::string username, std::string passwordHash, std::string salt,
           std::string fullName, DateTime createdAt, bool active)
    : id_(id), username_(std::move(username)),
      passwordHash_(std::move(passwordHash)), salt_(std::move(salt)),
      fullName_(std::move(fullName)), createdAt_(createdAt), active_(active) {}

} // namespace pos::domain
