#include "domain/analytics/AnalyticsService.h"
#include "domain/order/Order.h"
#include "domain/order/OrderRepository.h"

#include <cstddef>
#include <ctime>
#include <map>

namespace pos::domain {

namespace {
std::size_t cstringLen(const char* p, std::size_t maxN) {
    std::size_t i = 0;
    while (i < maxN && p[i] != '\0') ++i;
    return i;
}
}

AnalyticsService::AnalyticsService(SalesHistoryRepository& sales, OrderRepository& orders)
    : sales_(sales), orders_(orders) {}

Report AnalyticsService::buildReport(DateTime from, DateTime to) const {
    Report rep(from, to);
    auto records = sales_.all();
    for (const auto& s : records) {
        if (s.epoch < from.epoch() || s.epoch > to.epoch()) continue;
        rep.addRevenue(Money::fromCents(s.totalCents));
        rep.incOrders();
        rep.incHour(static_cast<int>(s.hour));
        std::string cashier(s.cashier, cstringLen(s.cashier, sizeof(s.cashier)));
        if (!cashier.empty()) rep.incEmployee(cashier);
    }
    // Item counts must come from orders.dat (sales_history only stores totals).
    for (const auto& o : orders_.loadAll()) {
        if (o.placedAt().epoch() < from.epoch() || o.placedAt().epoch() > to.epoch()) continue;
        for (const auto& it : o.items()) {
            rep.incItem(it.menuItem().value(), it.qty());
        }
    }
    return rep;
}

Report AnalyticsService::today() const {
    DateTime start = DateTime::now().startOfDay();
    DateTime end   = start.addSeconds(86400 - 1);
    return buildReport(start, end);
}

Report AnalyticsService::thisWeek() const {
    DateTime now = DateTime::now();
    DateTime start = now.startOfDay().addSeconds(-6 * 86400);
    return buildReport(start, now);
}

Report AnalyticsService::thisMonth() const {
    DateTime now = DateTime::now();
    DateTime start = now.startOfDay().addSeconds(-29 * 86400);
    return buildReport(start, now);
}

std::vector<AnalyticsService::DailyPoint>
AnalyticsService::daily(DateTime from, DateTime to) const {
    std::map<std::int64_t, std::pair<Money,int>> byDay;
    for (const auto& s : sales_.all()) {
        if (s.epoch < from.epoch() || s.epoch > to.epoch()) continue;
        DateTime dt = DateTime::fromEpoch(s.epoch).startOfDay();
        auto& slot = byDay[dt.epoch()];
        slot.first += Money::fromCents(s.totalCents);
        slot.second += 1;
    }
    std::vector<DailyPoint> out;
    out.reserve(byDay.size());
    for (auto& [k, v] : byDay) {
        out.push_back({DateTime::fromEpoch(k), v.first, v.second});
    }
    return out;
}

} // namespace pos::domain
