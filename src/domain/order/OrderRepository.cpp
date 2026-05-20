#include "domain/order/OrderRepository.h"
#include "domain/common/Exceptions.h"
#include "domain/persistence/FilePaths.h"
#include "util/Files.h"

#include <algorithm>
#include <cstring>
#include <filesystem>
#include <fstream>

namespace pos::domain {

namespace {

template <std::size_t N>
void copyFixed(char (&dst)[N], const std::string& src) {
    std::memset(dst, 0, N);
    const auto n = std::min(src.size(), static_cast<std::size_t>(N - 1));
    std::memcpy(dst, src.data(), n);
}

std::string fromFixed(const char* src, std::size_t n) {
    std::size_t len = 0;
    while (len < n && src[len] != '\0') ++len;
    return std::string(src, len);
}

} // namespace

OrderRepository::OrderRepository() : path_(FilePaths::Orders) {}

OrderHeaderRecord OrderRepository::headerOf(const Order& o) {
    OrderHeaderRecord r{};
    r.id              = o.id().value();
    r.placedAtEpoch   = o.placedAt().epoch();
    r.itemCount       = static_cast<std::uint16_t>(o.items().size());
    r.status          = static_cast<std::uint8_t>(o.status());
    r.paymentMethod   = 255; // unpaid by default; billing updates this on rewrite
    r.discountCents   = o.discount().cents();
    r.taxBp           = static_cast<std::uint16_t>(o.taxRate() * 10000.0 + 0.5);
    r.customerId      = o.customer() ? o.customer()->value() : 0u;
    copyFixed(r.cashierUsername,     o.cashierUsername());
    copyFixed(r.specialInstructions, o.specialInstructions());
    r.totalCents      = o.total().cents();
    return r;
}

OrderItemRecord OrderRepository::itemOf(const OrderItem& i) {
    OrderItemRecord r{};
    r.menuItemId     = i.menuItem().value();
    r.qty            = static_cast<std::uint16_t>(i.qty());
    r.unitPriceCents = i.unitPrice().cents();
    copyFixed(r.notes, i.notes());
    return r;
}

OrderItem OrderRepository::itemFrom(const OrderItemRecord& r) {
    return OrderItem(MenuItemId{r.menuItemId},
                     static_cast<int>(r.qty),
                     Money::fromCents(r.unitPriceCents),
                     fromFixed(r.notes, sizeof(r.notes)));
}

Order OrderRepository::orderFrom(const OrderHeaderRecord& h,
                                  const std::vector<OrderItemRecord>& items) {
    Order o(OrderId{h.id});
    o.setPlacedAt(DateTime::fromEpoch(h.placedAtEpoch));
    o.setStatus(static_cast<OrderStatus>(h.status));
    o.setDiscount(Money::fromCents(h.discountCents));
    o.setTaxRate(static_cast<double>(h.taxBp) / 10000.0);
    o.setCashier(fromFixed(h.cashierUsername, sizeof(h.cashierUsername)));
    o.setSpecialInstructions(fromFixed(h.specialInstructions, sizeof(h.specialInstructions)));
    if (h.customerId != 0u) o.setCustomer(CustomerId{h.customerId});
    for (const auto& ir : items) o.add(itemFrom(ir));
    return o;
}

std::vector<Order> OrderRepository::loadAll() const {
    std::vector<Order> out;
    std::ifstream in(path_, std::ios::binary);
    if (!in) return out;
    while (true) {
        OrderHeaderRecord h{};
        if (!in.read(reinterpret_cast<char*>(&h), sizeof(h))) break;
        std::vector<OrderItemRecord> items(h.itemCount);
        for (std::uint16_t i = 0; i < h.itemCount; ++i) {
            if (!in.read(reinterpret_cast<char*>(&items[i]), sizeof(OrderItemRecord))) {
                throw CorruptDataException(path_ + " (truncated item)");
            }
        }
        out.push_back(orderFrom(h, items));
    }
    if (in.bad()) throw FileIOException(path_, "read");
    return out;
}

std::optional<Order> OrderRepository::byId(OrderId id) const {
    for (auto& o : loadAll()) if (o.id() == id) return o;
    return std::nullopt;
}

void OrderRepository::append(const Order& o) {
    std::filesystem::path p(path_);
    auto dir = p.parent_path();
    if (!dir.empty()) pos::util::mkdirs(dir.string());
    std::ofstream out(path_, std::ios::binary | std::ios::app);
    if (!out) throw FileIOException(path_, "open-for-append");
    auto h = headerOf(o);
    out.write(reinterpret_cast<const char*>(&h), sizeof(h));
    for (const auto& it : o.items()) {
        auto ir = itemOf(it);
        out.write(reinterpret_cast<const char*>(&ir), sizeof(ir));
    }
    if (!out) throw FileIOException(path_, "append");
}

void OrderRepository::overwriteAll(const std::vector<Order>& orders) {
    std::filesystem::path p(path_);
    auto dir = p.parent_path();
    if (!dir.empty()) pos::util::mkdirs(dir.string());
    const std::string tmp = path_ + ".tmp";
    {
        std::ofstream out(tmp, std::ios::binary | std::ios::trunc);
        if (!out) throw FileIOException(tmp, "open-for-write");
        for (const auto& o : orders) {
            auto h = headerOf(o);
            out.write(reinterpret_cast<const char*>(&h), sizeof(h));
            for (const auto& it : o.items()) {
                auto ir = itemOf(it);
                out.write(reinterpret_cast<const char*>(&ir), sizeof(ir));
            }
            if (!out) throw FileIOException(tmp, "write");
        }
    }
    std::error_code ec;
    std::filesystem::rename(tmp, path_, ec);
    if (ec) {
        std::filesystem::remove(path_, ec);
        std::filesystem::rename(tmp, path_, ec);
        if (ec) throw FileIOException(path_, "rename");
    }
}

std::uint32_t OrderRepository::nextId() const {
    std::uint32_t maxId = 0;
    for (auto& o : loadAll()) if (o.id().value() > maxId) maxId = o.id().value();
    return maxId + 1;
}

} // namespace pos::domain
