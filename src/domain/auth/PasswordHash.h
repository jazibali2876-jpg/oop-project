#pragma once

#include <string>

namespace pos::domain {

// Demo-grade salted hash. Clearly NOT production crypto — uses a simple
// 64-bit FNV-1a mix over (salt || password), emitted as 16 lowercase hex chars.
class PasswordHash {
public:
    static std::string hash(const std::string& password, const std::string& salt);
    static bool verify(const std::string& storedHash,
                       const std::string& salt,
                       const std::string& attempt);
};

} // namespace pos::domain
