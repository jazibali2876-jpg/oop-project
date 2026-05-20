#pragma once

#include <cstdint>
#include <iosfwd>
#include <string>

namespace pos::domain {

class Money {
public:
    constexpr Money() = default;
    constexpr explicit Money(std::int64_t cents) : cents_(cents) {}

    static Money fromDouble(double d);
    static Money fromCents(std::int64_t c) { return Money{c}; }

    std::int64_t cents() const { return cents_; }
    double       toDouble() const { return static_cast<double>(cents_) / 100.0; }
    std::string  format(const char* symbol = "$") const;

    Money withTax(double rate) const { return Money{static_cast<std::int64_t>(cents_ * rate)}; }

    // Arithmetic
    Money& operator+=(const Money& rhs) { cents_ += rhs.cents_; return *this; }
    Money& operator-=(const Money& rhs) { cents_ -= rhs.cents_; return *this; }
    Money& operator*=(int n)            { cents_ *= n; return *this; }

    friend Money operator+(Money a, const Money& b) { a += b; return a; }
    friend Money operator-(Money a, const Money& b) { a -= b; return a; }
    friend Money operator*(Money a, int n)          { a *= n; return a; }
    friend Money operator*(int n, Money a)          { a *= n; return a; }

    // Comparisons
    friend bool operator==(const Money& a, const Money& b) { return a.cents_ == b.cents_; }
    friend bool operator!=(const Money& a, const Money& b) { return !(a == b); }
    friend bool operator<(const Money& a, const Money& b)  { return a.cents_ < b.cents_; }
    friend bool operator<=(const Money& a, const Money& b) { return a.cents_ <= b.cents_; }
    friend bool operator>(const Money& a, const Money& b)  { return a.cents_ > b.cents_; }
    friend bool operator>=(const Money& a, const Money& b) { return a.cents_ >= b.cents_; }

    // Stream insertion
    friend std::ostream& operator<<(std::ostream& os, const Money& m);

private:
    std::int64_t cents_ = 0;
};

} // namespace pos::domain
