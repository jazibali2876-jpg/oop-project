#include "domain/auth/PasswordHash.h"
#include "domain/auth/PasswordHash.cpp"
#include <iostream>
int main() {
    auto h = pos::domain::PasswordHash::hash("admin123", "p99vjpi9");
    std::cout << "hash(admin123, p99vjpi9) = " << h << " len=" << h.size() << "\n";
    std::cout << "verify d4fc...4de296 = "
              << pos::domain::PasswordHash::verify("d4fc35cac9eba128bb19a0cc494de296", "p99vjpi9", "admin123") << "\n";
    return 0;
}
