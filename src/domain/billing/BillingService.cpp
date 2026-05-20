#include "domain/billing/BillingService.h"
#include "domain/customer/CustomerService.h"
#include "domain/menu/MenuService.h"
#include "domain/order/Order.h"
#include "domain/order/OrderRepository.h"

namespace pos::domain {

BillingService::BillingService(ReceiptRepository& receipts,
                               OrderRepository& orders,
                               MenuService& menu,
                               CustomerService& customers)
    : receipts_(receipts), orders_(orders), menu_(menu), customers_(customers) {}

Receipt BillingService::makeReceipt(const Order& o) const {
    Receipt r;
    r.setOrderId(o.id());
    r.setIssuedAt(DateTime::now());
    r.setCashier(o.cashierUsername());
    r.setSubtotal(o.subtotal());
    r.setDiscount(o.discount());
    r.setTax(o.tax());
    r.setTaxRate(o.taxRate());
    r.setTotal(o.total());

    // Inject item names into a copy of OrderItem.notes so Receipt's operator<< can show them.
    std::vector<OrderItem> withNames = o.items();
    for (auto& it : withNames) {
        try {
            const auto& m = menu_.byId(it.menuItem());
            std::string label = m.name();
            if (!it.notes().empty()) label += " (" + it.notes() + ")";
            it.setNotes(label);
        } catch (...) {
            // leave notes alone if menu item is gone
        }
    }
    r.setItems(std::move(withNames));

    if (o.customer()) {
        if (auto c = customers_.findById(*o.customer())) {
            r.setCustomerName(c->name() + " (" + c->phone() + ")");
            r.setLoyalty(customers_.rule().pointsFor(o.total()), c->tier());
        }
    }
    return r;
}

Receipt BillingService::recordPayment(Order& o, PaymentMethod method) {
    auto recs = orders_.loadAll();
    for (auto& or_ : recs) if (or_.id() == o.id()) { or_.setStatus(o.status()); break; }
    orders_.overwriteAll(recs);

    if (o.customer()) {
        customers_.applyLoyalty(*o.customer(), o.total(), o.id());
    }
    Receipt r = makeReceipt(o);
    r.setMethod(method);
    return r;
}

std::string BillingService::saveReceiptFile(const Receipt& r) {
    return receipts_.write(r);
}

} // namespace pos::domain
