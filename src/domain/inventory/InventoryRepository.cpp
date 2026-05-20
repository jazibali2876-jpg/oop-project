#include "domain/inventory/InventoryRepository.h"
#include "domain/common/Exceptions.h"
#include "domain/persistence/FilePaths.h"
#include "util/Strings.h"

#include <sstream>

namespace pos::domain {

InventoryRepository::InventoryRepository() : TextRepository(FilePaths::Inventory) {}

std::vector<Ingredient> InventoryRepository::loadAll() const {
    std::vector<Ingredient> out;
    for (const auto& line : readAllLines()) {
        auto parts = pos::util::split(line, '|', true);
        if (parts.size() < 6) throw CorruptDataException("inventory.txt: " + line);
        std::uint32_t id = static_cast<std::uint32_t>(std::stoul(parts[0]));
        std::string name = pos::util::trim(parts[1]);
        std::string unit = pos::util::trim(parts[2]);
        double stock = std::stod(parts[3]);
        double threshold = std::stod(parts[4]);
        std::int64_t cost = std::stoll(parts[5]);
        out.emplace_back(IngredientId{id}, std::move(name), std::move(unit),
                         stock, threshold, Money::fromCents(cost));
    }
    return out;
}

void InventoryRepository::saveAll(const std::vector<Ingredient>& ings) {
    std::vector<std::string> lines;
    lines.reserve(ings.size());
    for (const auto& i : ings) {
        std::ostringstream oss;
        oss << i.id().value() << "|"
            << i.name() << "|"
            << i.unit() << "|"
            << i.stock() << "|"
            << i.reorderThreshold() << "|"
            << i.costPerUnit().cents();
        lines.push_back(oss.str());
    }
    writeAllLines(lines, "id|name|unit|stock|reorderThreshold|costCentsPerUnit");
}

} // namespace pos::domain
