#pragma once

#include "domain/common/Id.h"
#include "domain/common/Money.h"
#include "domain/menu/Category.h"

#include <string>
#include <string_view>

namespace pos::domain {

class MenuItem {
public:
    MenuItem() = default;
    MenuItem(MenuItemId id, std::string name, Category cat, Money price,
             std::string imagePath, bool available);

    MenuItemId         id() const         { return id_; }
    const std::string& name() const       { return name_; }
    Category           category() const   { return category_; }
    Money              price() const      { return price_; }
    const std::string& imagePath() const  { return imagePath_; }
    bool               available() const  { return available_; }

    void setName(std::string n)        { name_ = std::move(n); }
    void setCategory(Category c)       { category_ = c; }
    void setPrice(Money p)             { price_ = p; }
    void setImagePath(std::string p)   { imagePath_ = std::move(p); }
    void setAvailable(bool a)          { available_ = a; }

    bool matches(std::string_view query) const;

private:
    MenuItemId  id_{};
    std::string name_;
    Category    category_ = Category::FastFood;
    Money       price_;
    std::string imagePath_;
    bool        available_ = true;
};

} // namespace pos::domain
