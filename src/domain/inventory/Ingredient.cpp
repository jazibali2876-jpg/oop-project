#include "domain/inventory/Ingredient.h"

namespace pos::domain {

Ingredient::Ingredient(IngredientId id, std::string name, std::string unit,
                       double stock, double reorderThreshold, Money costPerUnit)
    : id_(id), name_(std::move(name)), unit_(std::move(unit)),
      stock_(stock), reorderThreshold_(reorderThreshold), costPerUnit_(costPerUnit) {}

} // namespace pos::domain
