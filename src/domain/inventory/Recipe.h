#pragma once

#include "domain/common/Id.h"

#include <utility>
#include <vector>

namespace pos::domain {

struct RecipeComponent {
    IngredientId ingredient;
    double       quantity;
};

class Recipe {
public:
    Recipe() = default;
    Recipe(MenuItemId menuItem, std::vector<RecipeComponent> components)
        : menuItem_(menuItem), components_(std::move(components)) {}

    MenuItemId                              menuItem() const   { return menuItem_; }
    const std::vector<RecipeComponent>&     components() const { return components_; }

    void setMenuItem(MenuItemId id)                              { menuItem_ = id; }
    void setComponents(std::vector<RecipeComponent> c)           { components_ = std::move(c); }

private:
    MenuItemId menuItem_{};
    std::vector<RecipeComponent> components_;
};

} // namespace pos::domain
