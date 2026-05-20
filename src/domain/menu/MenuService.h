#pragma once

#include "domain/menu/MenuItem.h"
#include "domain/menu/MenuRepository.h"

#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace pos::domain {

class MenuService {
public:
    explicit MenuService(MenuRepository& repo);

    void load();   // populate cache from repository (call once on startup)
    void persist();// write cache to repository
    void seedDefaultsIfEmpty();

    const std::vector<MenuItem>& all() const { return items_; }

    std::vector<MenuItem> filter(std::optional<Category> cat, std::string_view query) const;

    const MenuItem& byId(MenuItemId id) const;   // throws NotFoundException

    void add(MenuItem item);                     // throws DuplicateIdException
    void update(const MenuItem& item);           // throws NotFoundException
    void remove(MenuItemId id);                  // throws NotFoundException

    std::uint32_t nextId() const;

private:
    MenuRepository&        repo_;
    std::vector<MenuItem>  items_;
};

} // namespace pos::domain
