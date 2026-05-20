#pragma once

#include "domain/common/DateTime.h"
#include "domain/common/Id.h"
#include "domain/common/Money.h"

#include <array>
#include <map>
#include <string>
#include <utility>
#include <vector>

namespace pos::domain {

class Report {
public:
    Report() = default;
    Report(DateTime from, DateTime to) : from_(from), to_(to) { hourBuckets_.fill(0); }

    DateTime from() const                          { return from_; }
    DateTime to()   const                          { return to_; }
    Money    revenue() const                       { return revenue_; }
    int      orderCount() const                    { return orderCount_; }
    const std::map<std::uint32_t,int>& itemCounts() const { return itemCounts_; }
    const std::array<int,24>& hourBuckets() const  { return hourBuckets_; }
    const std::map<std::string,int>& employee() const { return employee_; }

    void addRevenue(Money m)                       { revenue_ += m; }
    void incOrders()                               { ++orderCount_; }
    void incItem(std::uint32_t id, int n)          { itemCounts_[id] += n; }
    void incHour(int hour)                         { if (hour>=0 && hour<24) hourBuckets_[hour]++; }
    void incEmployee(const std::string& u)         { employee_[u]++; }

    std::vector<std::pair<std::uint32_t,int>> topItems(std::size_t n) const;
    int peakHour() const;

private:
    DateTime                          from_;
    DateTime                          to_;
    Money                             revenue_;
    int                               orderCount_ = 0;
    std::map<std::uint32_t,int>       itemCounts_;
    std::array<int,24>                hourBuckets_{};
    std::map<std::string,int>         employee_;
};

} // namespace pos::domain
