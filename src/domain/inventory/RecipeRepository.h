#pragma once

#include "domain/inventory/Recipe.h"
#include "domain/persistence/TextRepository.h"

#include <optional>
#include <vector>

namespace pos::domain {

class RecipeRepository : public TextRepository {
public:
    RecipeRepository();
    std::vector<Recipe> loadAll() const;
    void                saveAll(const std::vector<Recipe>& recipes);

    std::optional<Recipe> byMenuItem(MenuItemId id) const;
};

} // namespace pos::domain
