#include "domain/menu/MenuService.h"
#include "domain/common/Exceptions.h"

#include <algorithm>
#include <string>

namespace pos::domain {

MenuService::MenuService(MenuRepository& repo) : repo_(repo) {}

void MenuService::load() {
    items_ = repo_.loadAll();
}

void MenuService::persist() {
    repo_.saveAll(items_);
}

void MenuService::seedDefaultsIfEmpty() {
    if (!items_.empty()) return;
    items_.emplace_back(MenuItemId{1}, "Classic Cheeseburger", Category::FastFood,
                       Money::fromCents(799), "burger.png", true);
    items_.emplace_back(MenuItemId{2}, "Grilled Chicken BBQ",  Category::BBQ,
                       Money::fromCents(1299), "bbq_chicken.png", true);
    items_.emplace_back(MenuItemId{3}, "Sweet & Sour Chicken", Category::Chinese,
                       Money::fromCents(1099), "sweet_sour.png", true);
    items_.emplace_back(MenuItemId{4}, "Iced Latte",           Category::Drinks,
                       Money::fromCents(449),  "iced_latte.png", true);
    items_.emplace_back(MenuItemId{5}, "Chocolate Lava Cake",  Category::Desserts,
                       Money::fromCents(599),  "lava_cake.png", true);
    persist();
}

std::vector<MenuItem> MenuService::filter(std::optional<Category> cat,
                                          std::string_view query) const {
    std::vector<MenuItem> out;
    for (const auto& it : items_) {
        if (cat && it.category() != *cat) continue;
        if (!it.matches(query)) continue;
        out.push_back(it);
    }
    return out;
}

const MenuItem& MenuService::byId(MenuItemId id) const {
    auto it = std::find_if(items_.begin(), items_.end(),
        [&](const MenuItem& m){ return m.id() == id; });
    if (it == items_.end()) {
        throw NotFoundException("menu item #" + std::to_string(id.value()));
    }
    return *it;
}

void MenuService::add(MenuItem item) {
    for (const auto& m : items_) {
        if (m.id() == item.id()) {
            throw DuplicateIdException("menu item #" + std::to_string(item.id().value()));
        }
    }
    items_.push_back(std::move(item));
    persist();
}

void MenuService::update(const MenuItem& item) {
    for (auto& m : items_) {
        if (m.id() == item.id()) { m = item; persist(); return; }
    }
    throw NotFoundException("menu item #" + std::to_string(item.id().value()));
}

void MenuService::remove(MenuItemId id) {
    auto before = items_.size();
    items_.erase(std::remove_if(items_.begin(), items_.end(),
        [&](const MenuItem& m){ return m.id() == id; }), items_.end());
    if (items_.size() == before) {
        throw NotFoundException("menu item #" + std::to_string(id.value()));
    }
    persist();
}

std::uint32_t MenuService::nextId() const {
    std::uint32_t max = 0;
    for (const auto& m : items_) if (m.id().value() > max) max = m.id().value();
    return max + 1;
}

} // namespace pos::domain
