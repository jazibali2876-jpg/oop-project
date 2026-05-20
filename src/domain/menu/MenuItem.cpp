#include "domain/menu/MenuItem.h"
#include "util/Strings.h"

namespace pos::domain {

MenuItem::MenuItem(MenuItemId id, std::string name, Category cat, Money price,
                   std::string imagePath, bool available)
    : id_(id), name_(std::move(name)), category_(cat), price_(price),
      imagePath_(std::move(imagePath)), available_(available) {}

bool MenuItem::matches(std::string_view query) const {
    if (query.empty()) return true;
    return pos::util::iContains(name_, query) ||
           pos::util::iContains(categoryName(category_), query);
}

std::string categoryName(Category c) {
    switch (c) {
        case Category::FastFood: return "FastFood";
        case Category::BBQ:      return "BBQ";
        case Category::Chinese:  return "Chinese";
        case Category::Drinks:   return "Drinks";
        case Category::Desserts: return "Desserts";
    }
    return "FastFood";
}

bool parseCategory(std::string_view s, Category& out) {
    if (s == "FastFood") { out = Category::FastFood; return true; }
    if (s == "BBQ")      { out = Category::BBQ;      return true; }
    if (s == "Chinese")  { out = Category::Chinese;  return true; }
    if (s == "Drinks")   { out = Category::Drinks;   return true; }
    if (s == "Desserts") { out = Category::Desserts; return true; }
    return false;
}

} // namespace pos::domain
