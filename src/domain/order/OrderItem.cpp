#include "domain/order/OrderItem.h"

namespace pos::domain {

OrderItem::OrderItem(MenuItemId mid, int qty, Money unitPrice, std::string notes)
    : menuItem_(mid), qty_(qty), unitPrice_(unitPrice), notes_(std::move(notes)) {}

} // namespace pos::domain
