#pragma once

#include <ctime>
#include <cstdint>
#include <string>

namespace pos::domain {

class DateTime {
public:
    DateTime() = default;
    explicit DateTime(std::int64_t epochSeconds) : epoch_(epochSeconds) {}

    static DateTime now();
    static DateTime fromEpoch(std::int64_t s) { return DateTime{s}; }

    std::int64_t epoch() const { return epoch_; }

    std::string format(const char* fmt = "%Y-%m-%d %H:%M:%S") const;
    std::string formatIso() const { return format("%Y-%m-%dT%H:%M:%SZ"); }

    int year()   const;
    int month()  const;
    int day()    const;
    int hour()   const;
    int dayOfYear() const;

    DateTime addSeconds(std::int64_t s) const { return DateTime{epoch_ + s}; }
    DateTime startOfDay() const;

    friend bool operator==(const DateTime& a, const DateTime& b) { return a.epoch_ == b.epoch_; }
    friend bool operator!=(const DateTime& a, const DateTime& b) { return !(a == b); }
    friend bool operator< (const DateTime& a, const DateTime& b) { return a.epoch_ <  b.epoch_; }
    friend bool operator<=(const DateTime& a, const DateTime& b) { return a.epoch_ <= b.epoch_; }
    friend bool operator> (const DateTime& a, const DateTime& b) { return a.epoch_ >  b.epoch_; }
    friend bool operator>=(const DateTime& a, const DateTime& b) { return a.epoch_ >= b.epoch_; }

private:
    std::int64_t epoch_ = 0;
};

} // namespace pos::domain
