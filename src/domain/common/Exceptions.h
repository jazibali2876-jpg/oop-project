#pragma once

#include "domain/common/Capability.h"

#include <stdexcept>
#include <string>

namespace pos::domain {

class DomainException : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

class InvalidCredentialsException : public DomainException {
public:
    InvalidCredentialsException() : DomainException("Invalid username or password.") {}
};

class EmptyOrderException : public DomainException {
public:
    EmptyOrderException() : DomainException("Cannot place an empty order.") {}
};

class InsufficientStockException : public DomainException {
public:
    InsufficientStockException(unsigned ingredientId, double needed, double have)
        : DomainException("Insufficient stock for ingredient #" + std::to_string(ingredientId) +
                          " (need " + std::to_string(needed) + ", have " + std::to_string(have) + ")."),
          ingredientId_(ingredientId), needed_(needed), have_(have) {}
    unsigned ingredientId() const { return ingredientId_; }
    double needed() const { return needed_; }
    double have() const { return have_; }
private:
    unsigned ingredientId_;
    double needed_;
    double have_;
};

class DuplicateIdException : public DomainException {
public:
    explicit DuplicateIdException(const std::string& what) : DomainException("Duplicate id: " + what) {}
};

class NotFoundException : public DomainException {
public:
    explicit NotFoundException(const std::string& what) : DomainException("Not found: " + what) {}
};

class FileIOException : public DomainException {
public:
    FileIOException(const std::string& path, const std::string& op)
        : DomainException("File I/O error (" + op + "): " + path), path_(path), op_(op) {}
    const std::string& path() const { return path_; }
    const std::string& op() const { return op_; }
private:
    std::string path_;
    std::string op_;
};

class CorruptDataException : public DomainException {
public:
    explicit CorruptDataException(const std::string& what) : DomainException("Corrupt data: " + what) {}
};

class UnauthorizedException : public DomainException {
public:
    explicit UnauthorizedException(Capability c)
        : DomainException("Unauthorized capability."), cap_(c) {}
    Capability capability() const { return cap_; }
private:
    Capability cap_;
};

} // namespace pos::domain
