#include "domain/common/Money.h"

#include <cmath>
#include <cstdio>
#include <ostream>

namespace pos::domain {

Money Money::fromDouble(double d) {
    return Money{static_cast<std::int64_t>(std::llround(d * 100.0))};
}

std::string Money::format(const char* symbol) const {
    char buf[32];
    std::int64_t c = cents_;
    bool neg = c < 0;
    if (neg) c = -c;
    std::snprintf(buf, sizeof(buf), "%s%s%lld.%02lld",
                  neg ? "-" : "",
                  symbol ? symbol : "",
                  static_cast<long long>(c / 100),
                  static_cast<long long>(c % 100));
    return std::string(buf);
}

std::ostream& operator<<(std::ostream& os, const Money& m) {
    return os << m.format();
}

} // namespace pos::domain
