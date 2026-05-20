#pragma once

#include "domain/menu/MenuItem.h"
#include "domain/persistence/TextRepository.h"

#include <vector>

namespace pos::domain {

class MenuRepository : public TextRepository {
public:
    MenuRepository();

    std::vector<MenuItem> loadAll() const;
    void                  saveAll(const std::vector<MenuItem>& items);
};

} // namespace pos::domain
