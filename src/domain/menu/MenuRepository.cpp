#include "domain/menu/MenuRepository.h"
#include "domain/common/Exceptions.h"
#include "domain/persistence/FilePaths.h"
#include "util/Strings.h"

#include <sstream>

namespace pos::domain {

MenuRepository::MenuRepository() : TextRepository(FilePaths::Menu) {}

std::vector<MenuItem> MenuRepository::loadAll() const {
    std::vector<MenuItem> out;
    for (const auto& line : readAllLines()) {
        auto parts = pos::util::split(line, '|', true);
        if (parts.size() < 6) {
            throw CorruptDataException("menu.txt: " + line);
        }
        std::uint32_t id = static_cast<std::uint32_t>(std::stoul(parts[0]));
        std::string  name      = pos::util::trim(parts[1]);
        Category cat = Category::FastFood;
        if (!parseCategory(pos::util::trim(parts[2]), cat)) {
            throw CorruptDataException("menu.txt: bad category in: " + line);
        }
        std::int64_t cents = std::stoll(parts[3]);
        std::string  image = pos::util::trim(parts[4]);
        bool         avail = pos::util::trim(parts[5]) != "0";
        out.emplace_back(MenuItemId{id}, std::move(name), cat,
                         Money::fromCents(cents), std::move(image), avail);
    }
    return out;
}

void MenuRepository::saveAll(const std::vector<MenuItem>& items) {
    std::vector<std::string> lines;
    lines.reserve(items.size());
    for (const auto& it : items) {
        std::ostringstream oss;
        oss << it.id().value() << "|"
            << it.name() << "|"
            << categoryName(it.category()) << "|"
            << it.price().cents() << "|"
            << it.imagePath() << "|"
            << (it.available() ? 1 : 0);
        lines.push_back(oss.str());
    }
    writeAllLines(lines, "id|name|category|priceCents|imagePath|available");
}

} // namespace pos::domain
