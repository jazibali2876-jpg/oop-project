// Standalone probe: read the freshly seeded users.dat and verify each demo
// account's password using the real C++ hash/verify code.

#include "domain/auth/PasswordHash.cpp"
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <string>

#pragma pack(push, 1)
struct UserRec {
    std::uint32_t id;
    char username[24];
    char passwordHash[32];
    char salt[16];
    char fullName[32];
    std::uint8_t role;
    std::uint8_t active;
    std::int64_t createdAtEpoch;
    std::uint8_t pad[10];
};
#pragma pack(pop)
static_assert(sizeof(UserRec) == 128, "");

int main() {
    std::ifstream in("dist/data/users.dat", std::ios::binary);
    UserRec r{};
    const char* attempts[3] = {"admin123", "cashier123", "kitchen123"};
    int idx = 0;
    while (in.read(reinterpret_cast<char*>(&r), sizeof(r))) {
        std::string user(r.username, strnlen(r.username, sizeof(r.username)));
        std::string hash(r.passwordHash, sizeof(r.passwordHash));
        // strip trailing NULs if any
        while (!hash.empty() && hash.back() == '\0') hash.pop_back();
        std::string salt(r.salt, strnlen(r.salt, sizeof(r.salt)));
        bool ok = pos::domain::PasswordHash::verify(hash, salt, attempts[idx]);
        std::printf("  %-8s  hash_len=%zu  verify(%s) = %s\n",
                    user.c_str(), hash.size(), attempts[idx], ok ? "PASS" : "FAIL");
        ++idx;
    }
    return 0;
}
