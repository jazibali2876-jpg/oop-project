#pragma once

#include <cstdint>
#include <iosfwd>
#include <ostream>

namespace pos::domain {

// Strong-typed id template. Same underlying type, different Tag = different id type.
template <class Tag>
class Id {
public:
    constexpr Id() = default;
    constexpr explicit Id(std::uint32_t v) : value_(v) {}

    constexpr std::uint32_t value() const { return value_; }
    constexpr bool valid() const { return value_ != 0; }

    friend constexpr bool operator==(Id a, Id b) { return a.value_ == b.value_; }
    friend constexpr bool operator!=(Id a, Id b) { return !(a == b); }
    friend constexpr bool operator< (Id a, Id b) { return a.value_ <  b.value_; }
    friend constexpr bool operator<=(Id a, Id b) { return a.value_ <= b.value_; }
    friend constexpr bool operator> (Id a, Id b) { return a.value_ >  b.value_; }
    friend constexpr bool operator>=(Id a, Id b) { return a.value_ >= b.value_; }

    friend std::ostream& operator<<(std::ostream& os, Id id) { return os << id.value_; }

private:
    std::uint32_t value_ = 0;
};

// Tags + aliases — declared close to where they're used in the planning docs,
// but the alias lives here so every package can refer to them uniformly.
struct MenuItemIdTag {};   using MenuItemId   = Id<MenuItemIdTag>;
struct OrderIdTag {};      using OrderId      = Id<OrderIdTag>;
struct CustomerIdTag {};   using CustomerId   = Id<CustomerIdTag>;
struct IngredientIdTag {}; using IngredientId = Id<IngredientIdTag>;
struct TableIdTag {};      using TableId      = Id<TableIdTag>;
struct UserIdTag {};       using UserId       = Id<UserIdTag>;

} // namespace pos::domain
