#include "domain/customer/CustomerRepository.h"
#include "domain/persistence/FilePaths.h"

#include <algorithm>
#include <cstring>

namespace pos::domain {

namespace {

template <std::size_t N>
void copyFixed(char (&dst)[N], const std::string& src) {
    std::memset(dst, 0, N);
    const auto n = std::min(src.size(), N);
    std::memcpy(dst, src.data(), n);
}

std::string fromFixed(const char* src, std::size_t n) {
    std::size_t len = 0;
    while (len < n && src[len] != '\0') ++len;
    return std::string(src, len);
}

} // namespace

CustomerRepository::CustomerRepository()
    : BinaryRepository<CustomerRecord>(FilePaths::Customers) {}

Customer CustomerRepository::toCustomer(const CustomerRecord& r) {
    return Customer(
        CustomerId{r.id},
        fromFixed(r.name, sizeof(r.name)),
        fromFixed(r.phone, sizeof(r.phone)),
        DateTime::fromEpoch(r.joinedAtEpoch),
        Money::fromCents(r.totalSpentCents),
        static_cast<int>(r.loyaltyPoints),
        static_cast<int>(r.orderCount)
    );
}

CustomerRecord CustomerRepository::toRecord(const Customer& c) {
    CustomerRecord r{};
    r.id              = c.id().value();
    copyFixed(r.name,  c.name());
    copyFixed(r.phone, c.phone());
    r.joinedAtEpoch   = c.joinedAt().epoch();
    r.totalSpentCents = c.totalSpent().cents();
    r.loyaltyPoints   = c.loyaltyPoints();
    r.orderCount      = c.orderCount();
    r.active          = 1;
    return r;
}

std::vector<Customer> CustomerRepository::loadAll() const {
    std::vector<Customer> out;
    for (const auto& r : all()) out.push_back(toCustomer(r));
    return out;
}

void CustomerRepository::saveAll(const std::vector<Customer>& customers) {
    std::vector<CustomerRecord> records;
    records.reserve(customers.size());
    for (const auto& c : customers) records.push_back(toRecord(c));
    overwriteAll(records);
}

std::optional<Customer> CustomerRepository::findByPhone(const std::string& phone) const {
    for (const auto& r : all()) {
        if (fromFixed(r.phone, sizeof(r.phone)) == phone) {
            return toCustomer(r);
        }
    }
    return std::nullopt;
}

std::optional<Customer> CustomerRepository::findById(CustomerId id) const {
    for (const auto& r : all()) {
        if (r.id == id.value()) return toCustomer(r);
    }
    return std::nullopt;
}

std::uint32_t CustomerRepository::nextId() const {
    std::uint32_t m = 0;
    for (const auto& r : all()) if (r.id > m) m = r.id;
    return m + 1;
}

} // namespace pos::domain
