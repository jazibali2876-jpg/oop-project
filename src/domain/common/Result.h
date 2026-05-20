#pragma once

#include <string>
#include <utility>

namespace pos::domain {

// Small Result<T> for paths where exceptions feel heavy. Used by quick checks
// (e.g. UI draft validation). Throwing remains the primary error mechanism.
template <class T>
class Result {
public:
    static Result success(T value) { return Result{std::move(value), {}, true}; }
    static Result failure(std::string err) { return Result{T{}, std::move(err), false}; }

    bool ok() const { return ok_; }
    explicit operator bool() const { return ok_; }

    T& value() { return value_; }
    const T& value() const { return value_; }
    const std::string& error() const { return error_; }

private:
    Result(T v, std::string e, bool ok) : value_(std::move(v)), error_(std::move(e)), ok_(ok) {}
    T value_;
    std::string error_;
    bool ok_;
};

} // namespace pos::domain
