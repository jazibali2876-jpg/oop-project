#include "domain/auth/PasswordHash.h"

#include <cstdint>
#include <cstdio>

namespace pos::domain {

static std::uint64_t fnv1a64(const std::string& s) {
    std::uint64_t h = 1469598103934665603ull;
    for (unsigned char c : s) {
        h ^= c;
        h *= 1099511628211ull;
    }
    return h;
}

std::string PasswordHash::hash(const std::string& password, const std::string& salt) {
    // Mix the salt twice and the password three times to thwart very-naive lookups.
    std::string combined = salt + ":" + password + ":" + salt;
    std::uint64_t h1 = fnv1a64(combined);
    std::uint64_t h2 = fnv1a64(std::to_string(h1) + password);
    char buf[33];
    std::snprintf(buf, sizeof(buf), "%016llx%016llx",
                  static_cast<unsigned long long>(h1),
                  static_cast<unsigned long long>(h2));
    return std::string(buf, 32);
}

bool PasswordHash::verify(const std::string& storedHash,
                          const std::string& salt,
                          const std::string& attempt) {
    return hash(attempt, salt) == storedHash;
}

} // namespace pos::domain
