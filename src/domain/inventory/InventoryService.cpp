#include "domain/inventory/InventoryService.h"
#include "domain/common/Exceptions.h"
#include "domain/order/Order.h"
#include "domain/order/OrderItem.h"

#include <algorithm>

namespace pos::domain {

InventoryService::InventoryService(InventoryRepository& inv, RecipeRepository& rec)
    : invRepo_(inv), recRepo_(rec) {}

void InventoryService::load() {
    ingredients_ = invRepo_.loadAll();
    recipes_     = recRepo_.loadAll();
}

void InventoryService::persist() {
    invRepo_.saveAll(ingredients_);
    recRepo_.saveAll(recipes_);
}

void InventoryService::seedDefaultsIfEmpty() {
    if (!ingredients_.empty()) return;
    ingredients_.emplace_back(IngredientId{1}, "Beef Patty",     "pcs", 48,  10, Money::fromCents(250));
    ingredients_.emplace_back(IngredientId{2}, "Burger Bun",     "pcs", 60,  15, Money::fromCents(45));
    ingredients_.emplace_back(IngredientId{3}, "Cheese Slice",   "pcs", 72,  20, Money::fromCents(30));
    ingredients_.emplace_back(IngredientId{4}, "Lettuce",        "g",   800, 200, Money::fromCents(2));
    ingredients_.emplace_back(IngredientId{5}, "Espresso Beans", "g",   1500,400, Money::fromCents(8));
    invRepo_.saveAll(ingredients_);
}

void InventoryService::seedDefaultRecipesIfEmpty() {
    if (!recipes_.empty()) return;
    recipes_.emplace_back(MenuItemId{1}, std::vector<RecipeComponent>{
        {IngredientId{1}, 1}, {IngredientId{2}, 1}, {IngredientId{3}, 1}, {IngredientId{4}, 30}});
    recipes_.emplace_back(MenuItemId{4}, std::vector<RecipeComponent>{
        {IngredientId{5}, 18}});
    recRepo_.saveAll(recipes_);
}

std::optional<Recipe> InventoryService::recipeFor(MenuItemId id) const {
    for (const auto& r : recipes_) if (r.menuItem() == id) return r;
    return std::nullopt;
}

void InventoryService::setRecipe(const Recipe& r) {
    for (auto& existing : recipes_) {
        if (existing.menuItem() == r.menuItem()) { existing = r; persist(); return; }
    }
    recipes_.push_back(r);
    persist();
}

static const Ingredient* findIng(const std::vector<Ingredient>& v, IngredientId id) {
    for (const auto& i : v) if (i.id() == id) return &i;
    return nullptr;
}

bool InventoryService::canFulfill(const Order& order) const {
    std::map<std::uint32_t, double> needed;
    for (const auto& oi : order.items()) {
        auto rec = recipeFor(oi.menuItem());
        if (!rec) continue;
        for (const auto& c : rec->components()) {
            needed[c.ingredient.value()] += c.quantity * oi.qty();
        }
    }
    for (const auto& [iid, qty] : needed) {
        const Ingredient* ing = findIng(ingredients_, IngredientId{iid});
        if (!ing) return false;
        if (ing->stock() < qty) return false;
    }
    return true;
}

void InventoryService::deduct(const Order& order) {
    std::map<std::uint32_t, double> needed;
    for (const auto& oi : order.items()) {
        auto rec = recipeFor(oi.menuItem());
        if (!rec) continue;
        for (const auto& c : rec->components()) {
            needed[c.ingredient.value()] += c.quantity * oi.qty();
        }
    }
    // First verify everything is available.
    for (const auto& [iid, qty] : needed) {
        const Ingredient* ing = findIng(ingredients_, IngredientId{iid});
        if (!ing || ing->stock() < qty) {
            double have = ing ? ing->stock() : 0.0;
            throw InsufficientStockException(iid, qty, have);
        }
    }
    // Then deduct.
    for (auto& ing : ingredients_) {
        auto it = needed.find(ing.id().value());
        if (it != needed.end()) ing.addStock(-it->second);
    }
    invRepo_.saveAll(ingredients_);
}

std::vector<Ingredient> InventoryService::lowStock() const {
    std::vector<Ingredient> out;
    for (const auto& i : ingredients_) if (i.isLow()) out.push_back(i);
    return out;
}

void InventoryService::addIngredient(Ingredient i) {
    for (const auto& x : ingredients_) {
        if (x.id() == i.id()) {
            throw DuplicateIdException("ingredient #" + std::to_string(i.id().value()));
        }
    }
    ingredients_.push_back(std::move(i));
    invRepo_.saveAll(ingredients_);
}

void InventoryService::updateIngredient(const Ingredient& i) {
    for (auto& x : ingredients_) {
        if (x.id() == i.id()) { x = i; invRepo_.saveAll(ingredients_); return; }
    }
    throw NotFoundException("ingredient #" + std::to_string(i.id().value()));
}

void InventoryService::removeIngredient(IngredientId id) {
    auto before = ingredients_.size();
    ingredients_.erase(std::remove_if(ingredients_.begin(), ingredients_.end(),
        [&](const Ingredient& x){ return x.id() == id; }), ingredients_.end());
    if (ingredients_.size() == before) {
        throw NotFoundException("ingredient #" + std::to_string(id.value()));
    }
    invRepo_.saveAll(ingredients_);
}

std::uint32_t InventoryService::nextId() const {
    std::uint32_t m = 0;
    for (const auto& i : ingredients_) if (i.id().value() > m) m = i.id().value();
    return m + 1;
}

} // namespace pos::domain
