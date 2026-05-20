#include "domain/customer/CustomerService.h"
#include "domain/common/Exceptions.h"
#include "domain/order/Order.h"
#include "domain/order/OrderRepository.h"
#include "util/Strings.h"

#include <algorithm>

namespace pos::domain {

CustomerService::CustomerService(CustomerRepository& repo, LoyaltyRepository& loyaltyRepo)
    : repo_(repo), loyaltyRepo_(loyaltyRepo) {}

void CustomerService::load() {
    customers_ = repo_.loadAll();
}

std::optional<Customer> CustomerService::findById(CustomerId id) const {
    for (const auto& c : customers_) if (c.id() == id) return c;
    return std::nullopt;
}

std::optional<Customer> CustomerService::findByPhone(const std::string& phone) const {
    for (const auto& c : customers_) if (c.phone() == phone) return c;
    return std::nullopt;
}

Customer CustomerService::findByPhoneOrCreate(const std::string& phone,
                                              const std::string& nameIfNew) {
    if (auto existing = findByPhone(phone)) return *existing;
    Customer c{CustomerId{repo_.nextId()},
               nameIfNew.empty() ? "Walk-in" : nameIfNew,
               phone, DateTime::now(), Money{}, 0, 0};
    customers_.push_back(c);
    repo_.saveAll(customers_);
    return c;
}

std::vector<Customer> CustomerService::search(std::string_view query) const {
    if (query.empty()) return customers_;
    std::vector<Customer> out;
    for (const auto& c : customers_) {
        if (pos::util::iContains(c.name(), query) ||
            pos::util::iContains(c.phone(), query)) {
            out.push_back(c);
        }
    }
    return out;
}

void CustomerService::applyLoyalty(CustomerId id, Money orderTotal, OrderId orderId) {
    int pts = rule_.pointsFor(orderTotal);
    for (auto& c : customers_) {
        if (c.id() == id) {
            c.addPoints(pts);
            c.recordOrder(orderTotal);
            repo_.saveAll(customers_);
            LoyaltyRecord lr{};
            lr.customerId  = id.value();
            lr.orderId     = orderId.value();
            lr.atEpoch     = DateTime::now().epoch();
            lr.pointsDelta = pts;
            lr.spendCents  = orderTotal.cents();
            lr.reason      = 0; // earn
            loyaltyRepo_.append(lr);
            return;
        }
    }
    throw NotFoundException("customer #" + std::to_string(id.value()));
}

void CustomerService::upsert(const Customer& c) {
    for (auto& existing : customers_) {
        if (existing.id() == c.id()) { existing = c; repo_.saveAll(customers_); return; }
    }
    customers_.push_back(c);
    repo_.saveAll(customers_);
}

std::vector<Order> CustomerService::history(CustomerId id, OrderRepository& orders) const {
    std::vector<Order> out;
    for (auto& o : orders.loadAll()) {
        if (o.customer() && *o.customer() == id) out.push_back(o);
    }
    return out;
}

} // namespace pos::domain
