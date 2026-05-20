#include "domain/auth/UserRepository.h"
#include "domain/auth/Admin.h"
#include "domain/auth/Cashier.h"
#include "domain/auth/Kitchen.h"
#include "domain/common/Exceptions.h"
#include "domain/persistence/FilePaths.h"

#include <algorithm>
#include <cstring>
#include <utility>

namespace pos::domain {

namespace {

// Copy src into fixed-size dst, NUL-padding, and guaranteeing dst[N-1] = '\0'.
template <std::size_t N>
void copyFixed(char (&dst)[N], const std::string& src) {
    std::memset(dst, 0, N);
    const std::size_t n = std::min(src.size(), static_cast<std::size_t>(N - 1));
    std::memcpy(dst, src.data(), n);
}

std::string fromFixed(const char* src, std::size_t n) {
    std::size_t len = 0;
    while (len < n && src[len] != '\0') ++len;
    return std::string(src, len);
}

} // namespace

UserRepository::UserRepository() : BinaryRepository<UserRecord>(FilePaths::Users) {}

std::unique_ptr<User> UserRepository::toUser(const UserRecord& r) {
    auto username = fromFixed(r.username, sizeof(r.username));
    auto hash     = fromFixed(r.passwordHash, sizeof(r.passwordHash));
    auto salt     = fromFixed(r.salt, sizeof(r.salt));
    auto fullName = fromFixed(r.fullName, sizeof(r.fullName));
    DateTime createdAt{r.createdAtEpoch};
    UserId id{r.id};
    const bool active = r.active != 0;
    switch (static_cast<Role>(r.role)) {
        case Role::Admin:
            return std::make_unique<Admin>(id, std::move(username), std::move(hash),
                                           std::move(salt), std::move(fullName),
                                           createdAt, active);
        case Role::Cashier:
            return std::make_unique<Cashier>(id, std::move(username), std::move(hash),
                                             std::move(salt), std::move(fullName),
                                             createdAt, active);
        case Role::Kitchen:
        default:
            return std::make_unique<Kitchen>(id, std::move(username), std::move(hash),
                                             std::move(salt), std::move(fullName),
                                             createdAt, active);
    }
}

UserRecord UserRepository::toRecord(const User& u) {
    UserRecord r{};
    r.id = u.id().value();
    copyFixed(r.username,     u.username());
    copyFixed(r.passwordHash, u.passwordHash());
    copyFixed(r.salt,         u.salt());
    copyFixed(r.fullName,     u.fullName());
    r.role = static_cast<std::uint8_t>(u.role());
    r.active = u.active() ? 1 : 0;
    r.createdAtEpoch = u.createdAt().epoch();
    return r;
}

std::vector<std::unique_ptr<User>> UserRepository::loadAll() const {
    std::vector<std::unique_ptr<User>> out;
    for (const auto& r : all()) out.push_back(toUser(r));
    return out;
}

std::optional<std::unique_ptr<User>>
UserRepository::findByUsername(const std::string& username) const {
    for (const auto& r : all()) {
        if (fromFixed(r.username, sizeof(r.username)) == username) {
            return toUser(r);
        }
    }
    return std::nullopt;
}

void UserRepository::upsert(const User& user) {
    auto records = all();
    UserRecord newRec = toRecord(user);
    bool replaced = false;
    for (auto& r : records) {
        if (r.id == newRec.id ||
            fromFixed(r.username, sizeof(r.username)) == user.username()) {
            r = newRec;
            replaced = true;
            break;
        }
    }
    if (!replaced) records.push_back(newRec);
    overwriteAll(records);
}

std::uint32_t UserRepository::nextId() const {
    std::uint32_t maxId = 0;
    for (const auto& r : all()) if (r.id > maxId) maxId = r.id;
    return maxId + 1;
}

} // namespace pos::domain
