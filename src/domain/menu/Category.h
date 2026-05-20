#pragma once

#include <string>
#include <string_view>

namespace pos::domain {

enum class Category {
    FastFood,
    BBQ,
    Chinese,
    Drinks,
    Desserts
};

std::string categoryName(Category c);
bool        parseCategory(std::string_view s, Category& out);

} // namespace pos::domain
