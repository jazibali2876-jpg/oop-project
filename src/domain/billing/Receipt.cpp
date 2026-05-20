#include "domain/billing/Receipt.h"

#include <cstdio>
#include <iomanip>
#include <ostream>
#include <sstream>

namespace pos::domain {

std::string paymentMethodName(PaymentMethod m) {
    switch (m) {
        case PaymentMethod::Cash:      return "CASH";
        case PaymentMethod::Card:      return "CARD";
        case PaymentMethod::OnlineSim: return "ONLINE (sim)";
        case PaymentMethod::Unpaid:    return "UNPAID";
    }
    return "?";
}

std::ostream& operator<<(std::ostream& os, const Receipt& r) {
    auto line = [&](char c){ for (int i = 0; i < 44; ++i) os << c; os << '\n'; };
    os << "============================================\n";
    os << "         SMART RESTAURANT POS\n";
    os << "        123 Main Street, City\n";
    os << "         Tel: +XX-XXX-XXXXXXX\n";
    os << "============================================\n";
    {
        char buf[64];
        std::snprintf(buf, sizeof(buf), "RCPT-%06u", r.orderId().value());
        os << "Receipt #:  " << buf << "\n";
    }
    os << "Order #:    " << r.orderId().value() << "\n";
    os << "Date:       " << r.issuedAt().format() << "\n";
    os << "Cashier:    " << r.cashier() << "\n";
    if (!r.customerName().empty()) {
        os << "Customer:   " << r.customerName() << "\n";
    }
    line('-');
    os << "Qty  Item                     Price   Total\n";
    line('-');
    for (const auto& it : r.items()) {
        char qbuf[8], pbuf[16], tbuf[16];
        std::snprintf(qbuf, sizeof(qbuf), "%3d", it.qty());
        std::snprintf(pbuf, sizeof(pbuf), "%7s", it.unitPrice().format("").c_str());
        std::snprintf(tbuf, sizeof(tbuf), "%7s", it.lineTotal().format("").c_str());

        std::string name = it.notes().empty()
                            ? ("Item #" + std::to_string(it.menuItem().value()))
                            : ("Item #" + std::to_string(it.menuItem().value()));
        // Actual menu item names are looked up by BillingService and stored via
        // OrderItem::notes? — no; menu name is filled by Receipt builder.
        // Fall back to id if notes is empty; BillingService injects names via notes-pseudo.
        if (!it.notes().empty()) name = it.notes();

        char row[64];
        std::snprintf(row, sizeof(row), "%s  %-22.22s %s %s",
                      qbuf, name.c_str(), pbuf, tbuf);
        os << row << "\n";
    }
    line('-');
    {
        std::ostringstream ss;
        ss << "Subtotal:" << std::setw(34) << std::right << r.subtotal().format();
        os << ss.str() << "\n";
    }
    if (r.discount() > Money{}) {
        std::ostringstream ss;
        Money neg = Money{} - r.discount();
        ss << "Discount:" << std::setw(34) << std::right << neg.format();
        os << ss.str() << "\n";
    }
    {
        char taxLabel[24];
        std::snprintf(taxLabel, sizeof(taxLabel), "Tax (%.2f%%):", r.taxRate() * 100.0);
        std::ostringstream ss;
        ss << taxLabel << std::setw(43 - (int)std::string(taxLabel).size()) << std::right << r.tax().format();
        os << ss.str() << "\n";
    }
    line('-');
    {
        std::ostringstream ss;
        ss << "TOTAL:" << std::setw(37) << std::right << r.total().format();
        os << ss.str() << "\n";
    }
    os << "Payment:    " << paymentMethodName(r.method()) << "\n";
    line('-');
    if (r.loyaltyEarned() > 0) {
        os << "Loyalty Points Earned: " << r.loyaltyEarned() << "\n";
        if (!r.loyaltyTier().empty()) os << "Tier:                  " << r.loyaltyTier() << "\n";
    }
    os << "\n       Thank you and please come again!\n";
    os << "============================================\n";
    return os;
}

} // namespace pos::domain
