#pragma once

#include "domain/common/Money.h"

#include <map>
#include <string>

namespace pos::domain {

class LoyaltyRule {
public:
    LoyaltyRule();

    int pointsPerDollar() const { return pointsPerDollar_; }
    void setPointsPerDollar(int n) { pointsPerDollar_ = n; }

    // Returns how many loyalty points an order of the given total earns.
    int  pointsFor(Money orderTotal) const;

    // Returns the tier name for an accumulated point total.
    std::string tierFor(int points) const;

private:
    int pointsPerDollar_ = 1;
    std::map<std::string, int> tierThresholds_;
};

} // namespace pos::domain
