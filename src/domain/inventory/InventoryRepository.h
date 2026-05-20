#pragma once

#include "domain/inventory/Ingredient.h"
#include "domain/persistence/TextRepository.h"

#include <vector>

namespace pos::domain {

class InventoryRepository : public TextRepository {
public:
    InventoryRepository();
    std::vector<Ingredient> loadAll() const;
    void                    saveAll(const std::vector<Ingredient>& ingredients);
};

} // namespace pos::domain
