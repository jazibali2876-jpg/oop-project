#include "domain/inventory/RecipeRepository.h"
#include "domain/common/Exceptions.h"
#include "domain/persistence/FilePaths.h"
#include "util/Strings.h"

#include <sstream>

namespace pos::domain {

RecipeRepository::RecipeRepository() : TextRepository(FilePaths::Recipes) {}

std::vector<Recipe> RecipeRepository::loadAll() const {
    std::vector<Recipe> out;
    for (const auto& line : readAllLines()) {
        auto parts = pos::util::split(line, '|', true);
        if (parts.size() < 2) throw CorruptDataException("recipes.txt: " + line);
        std::uint32_t mid = static_cast<std::uint32_t>(std::stoul(parts[0]));
        std::vector<RecipeComponent> comps;
        for (auto& tok : pos::util::split(parts[1], ',', false)) {
            auto kv = pos::util::split(tok, ':', false);
            if (kv.size() != 2) throw CorruptDataException("recipes.txt component: " + tok);
            std::uint32_t iid = static_cast<std::uint32_t>(std::stoul(kv[0]));
            double qty = std::stod(kv[1]);
            comps.push_back({IngredientId{iid}, qty});
        }
        out.emplace_back(MenuItemId{mid}, std::move(comps));
    }
    return out;
}

void RecipeRepository::saveAll(const std::vector<Recipe>& recipes) {
    std::vector<std::string> lines;
    lines.reserve(recipes.size());
    for (const auto& r : recipes) {
        std::ostringstream oss;
        oss << r.menuItem().value() << "|";
        for (std::size_t i = 0; i < r.components().size(); ++i) {
            const auto& c = r.components()[i];
            if (i) oss << ",";
            oss << c.ingredient.value() << ":" << c.quantity;
        }
        lines.push_back(oss.str());
    }
    writeAllLines(lines, "menuItemId|ingredientId:qty,ingredientId:qty,...");
}

std::optional<Recipe> RecipeRepository::byMenuItem(MenuItemId id) const {
    for (auto& r : loadAll()) if (r.menuItem() == id) return r;
    return std::nullopt;
}

} // namespace pos::domain
