#include "domain/common/DateTime.h"

#include <chrono>
#include <cstdio>
#include <ctime>

namespace pos::domain {

DateTime DateTime::now() {
    using namespace std::chrono;
    return DateTime{static_cast<std::int64_t>(
        duration_cast<seconds>(system_clock::now().time_since_epoch()).count()
    )};
}

static std::tm toLocal(std::int64_t epoch) {
    std::time_t t = static_cast<std::time_t>(epoch);
    std::tm out{};
#if defined(_WIN32)
    localtime_s(&out, &t);
#else
    localtime_r(&t, &out);
#endif
    return out;
}

std::string DateTime::format(const char* fmt) const {
    std::tm tm = toLocal(epoch_);
    char buf[64];
    if (!std::strftime(buf, sizeof(buf), fmt, &tm)) return std::string{};
    return std::string{buf};
}

int DateTime::year()       const { return toLocal(epoch_).tm_year + 1900; }
int DateTime::month()      const { return toLocal(epoch_).tm_mon  + 1; }
int DateTime::day()        const { return toLocal(epoch_).tm_mday; }
int DateTime::hour()       const { return toLocal(epoch_).tm_hour; }
int DateTime::dayOfYear()  const { return toLocal(epoch_).tm_yday; }

DateTime DateTime::startOfDay() const {
    std::tm tm = toLocal(epoch_);
    tm.tm_hour = 0; tm.tm_min = 0; tm.tm_sec = 0;
    return DateTime{static_cast<std::int64_t>(std::mktime(&tm))};
}

} // namespace pos::domain
