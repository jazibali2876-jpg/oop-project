#pragma once

#include "domain/common/DateTime.h"
#include "domain/inventory/Ingredient.h"
#include "domain/inventory/InventoryRepository.h"
#include "domain/inventory/Recipe.h"
#include "domain/inventory/RecipeRepository.h"

#include <map>
#include <optional>
#include <vector>

namespace pos::domain {

class Order;

class InventoryService {
public:
    InventoryService(InventoryRepository& inv, RecipeRepository& rec);

    void load();
    void persist();
    void seedDefaultsIfEmpty();

    const std::vector<Ingredient>& all() const { return ingredients_; }

    bool canFulfill(const Order& order) const;
    void deduct(const Order& order);          // throws InsufficientStockException

    std::vector<Ingredient> lowStock() const;

    // Direct CRUD for admin screen.
    void   addIngredient(Ingredient i);
    void   updateIngredient(const Ingredient& i);
    void   removeIngredient(IngredientId id);
    std::uint32_t nextId() const;

    // Recipes
    const std::vector<Recipe>& recipes() const { return recipes_; }
    std::optional<Recipe>      recipeFor(MenuItemId id) const;
    void                       setRecipe(const Recipe& r);
    void                       seedDefaultRecipesIfEmpty();

private:
    InventoryRepository&    invRepo_;
    RecipeRepository&       recRepo_;
    std::vector<Ingredient> ingredients_;
    std::vector<Recipe>     recipes_;
};

} // namespace pos::domain
