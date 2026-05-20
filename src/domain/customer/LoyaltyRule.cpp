#include "domain/customer/LoyaltyRule.h"

namespace pos::domain {

LoyaltyRule::LoyaltyRule() {
    tierThresholds_["New"]    = 0;
    tierThresholds_["Bronze"] = 50;
    tierThresholds_["Silver"] = 200;
    tierThresholds_["Gold"]   = 500;
}

int LoyaltyRule::pointsFor(Money orderTotal) const {
    // 1 point per whole dollar by default.
    if (orderTotal.cents() <= 0) return 0;
    return static_cast<int>(orderTotal.cents() / 100) * pointsPerDollar_;
}

std::string LoyaltyRule::tierFor(int points) const {
    std::string best = "New";
    int bestThr = -1;
    for (const auto& [name, thr] : tierThresholds_) {
        if (points >= thr && thr > bestThr) {
            best = name;
            bestThr = thr;
        }
    }
    return best;
}

} // namespace pos::domain
