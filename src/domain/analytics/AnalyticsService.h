#pragma once

#include "domain/analytics/Report.h"
#include "domain/analytics/SalesHistoryRepository.h"
#include "domain/common/DateTime.h"

#include <vector>

namespace pos::domain {

class MenuRepository;
class OrderRepository;

class AnalyticsService {
public:
    AnalyticsService(SalesHistoryRepository& sales, OrderRepository& orders);

    Report buildReport(DateTime from, DateTime to) const;
    Report today() const;
    Report thisWeek() const;
    Report thisMonth() const;

    // Returns one (revenue, orderCount, AOV) data point per day in the range.
    struct DailyPoint { DateTime day; Money revenue; int orderCount; };
    std::vector<DailyPoint> daily(DateTime from, DateTime to) const;

private:
    SalesHistoryRepository&   sales_;
    OrderRepository&          orders_;
};

} // namespace pos::domain
