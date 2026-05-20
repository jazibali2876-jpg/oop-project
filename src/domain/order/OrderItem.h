#pragma once

#include "domain/common/Id.h"
#include "domain/common/Money.h"

#include <string>

namespace pos::domain {

class OrderItem {
public:
    OrderItem() = default;
    OrderItem(MenuItemId mid, int qty, Money unitPrice, std::string notes = {});

    MenuItemId         menuItem() const  { return menuItem_; }
    int                qty() const       { return qty_; }
    Money              unitPrice() const { return unitPrice_; }
    const std::string& notes() const     { return notes_; }

    void setQty(int q)              { qty_ = q; }
    void setUnitPrice(Money p)      { unitPrice_ = p; }
    void setNotes(std::string n)    { notes_ = std::move(n); }

    Money lineTotal() const { return unitPrice_ * qty_; }

private:
    MenuItemId  menuItem_{};
    int         qty_ = 0;
    Money       unitPrice_;
    std::string notes_;
};

} // namespace pos::domain
