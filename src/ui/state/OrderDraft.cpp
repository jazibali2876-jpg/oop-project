#include "ui/state/OrderDraft.h"

namespace pos::ui {

void OrderDraft::addItem(pos::domain::MenuItemId id, pos::domain::Money price, const std::string& name) {
    // If already present, increment qty.
    for (std::size_t i = 0; i < items_.size(); ++i) {
        if (items_[i].menuItem() == id) {
            items_[i].setQty(items_[i].qty() + 1);
            return;
        }
    }
    pos::domain::OrderItem oi(id, 1, price);
    items_.push_back(std::move(oi));
    names_.push_back(name);
}

void OrderDraft::increment(std::size_t idx) {
    if (idx < items_.size()) items_[idx].setQty(items_[idx].qty() + 1);
}

void OrderDraft::decrement(std::size_t idx) {
    if (idx < items_.size()) {
        int q = items_[idx].qty() - 1;
        if (q <= 0) removeAt(idx);
        else        items_[idx].setQty(q);
    }
}

void OrderDraft::removeAt(std::size_t idx) {
    if (idx < items_.size()) {
        items_.erase(items_.begin() + idx);
        if (idx < names_.size()) names_.erase(names_.begin() + idx);
    }
}

void OrderDraft::clear() {
    items_.clear();
    names_.clear();
    discount_ = pos::domain::Money{};
    specialInstructions_.clear();
    customer_.reset();
}

pos::domain::Money OrderDraft::subtotal() const {
    pos::domain::Money m;
    for (const auto& it : items_) m += it.lineTotal();
    return m;
}

pos::domain::Money OrderDraft::tax() const {
    auto base = subtotal() - discount_;
    if (base < pos::domain::Money{}) base = pos::domain::Money{};
    return base.withTax(taxRate_);
}

pos::domain::Money OrderDraft::total() const {
    auto base = subtotal() - discount_ + tax();
    if (base < pos::domain::Money{}) base = pos::domain::Money{};
    return base;
}

int OrderDraft::totalQty() const {
    int q = 0;
    for (const auto& it : items_) q += it.qty();
    return q;
}

pos::domain::Order OrderDraft::toOrder() const {
    pos::domain::Order o;
    o.setDiscount(discount_);
    o.setTaxRate(taxRate_);
    o.setSpecialInstructions(specialInstructions_);
    o.setCustomer(customer_);
    for (const auto& it : items_) o.add(it);
    return o;
}

} // namespace pos::ui
