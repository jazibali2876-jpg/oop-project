#include "domain/analytics/Report.h"

#include <algorithm>

namespace pos::domain {

std::vector<std::pair<std::uint32_t,int>> Report::topItems(std::size_t n) const {
    std::vector<std::pair<std::uint32_t,int>> v(itemCounts_.begin(), itemCounts_.end());
    std::sort(v.begin(), v.end(), [](auto& a, auto& b){ return a.second > b.second; });
    if (v.size() > n) v.resize(n);
    return v;
}

int Report::peakHour() const {
    int best = 0, bestVal = -1;
    for (int h = 0; h < 24; ++h) {
        if (hourBuckets_[h] > bestVal) { bestVal = hourBuckets_[h]; best = h; }
    }
    return best;
}

} // namespace pos::domain
