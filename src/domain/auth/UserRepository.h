#pragma once

#include "domain/auth/User.h"
#include "domain/persistence/BinaryRepository.h"

#include <cstdint>
#include <memory>
#include <optional>
#include <vector>

namespace pos::domain {

// 128-byte fixed-width POD record. Phase 2 pinned: passwordHash[32] (our hash is
// 32 hex chars) reduced from the plan's 48 to fit a clean 128-byte total.
#pragma pack(push, 1)
struct UserRecord {
    std::uint32_t id;               //  4
    char          username[24];     // 24
    char          passwordHash[32]; // 32
    char          salt[16];         // 16
    char          fullName[32];     // 32
    std::uint8_t  role;             //  1  0=Admin, 1=Cashier, 2=Kitchen
    std::uint8_t  active;           //  1
    std::int64_t  createdAtEpoch;   //  8
    std::uint8_t  pad[10];          // 10  -> total = 128
};
#pragma pack(pop)
static_assert(sizeof(UserRecord) == 128, "UserRecord layout drift");
static_assert(std::is_trivially_copyable_v<UserRecord>);

class UserRepository : public BinaryRepository<UserRecord> {
public:
    UserRepository();

    std::vector<std::unique_ptr<User>>     loadAll() const;
    std::optional<std::unique_ptr<User>>   findByUsername(const std::string& username) const;
    void                                   upsert(const User& user);
    std::uint32_t                          nextId() const;

    // Convert a record to a polymorphic User.
    static std::unique_ptr<User> toUser(const UserRecord& r);

    // Convert a User to a record (lossy on string lengths — truncates).
    static UserRecord toRecord(const User& u);
};

} // namespace pos::domain
