#include "domain/tables/TableRepository.h"
#include "domain/common/Exceptions.h"
#include "domain/persistence/FilePaths.h"
#include "util/Strings.h"

#include <sstream>

namespace pos::domain {

TableRepository::TableRepository() : TextRepository(FilePaths::Tables) {}

std::vector<Table> TableRepository::loadAll() const {
    std::vector<Table> out;
    for (const auto& line : readAllLines()) {
        auto parts = pos::util::split(line, '|', true);
        if (parts.size() < 5) throw CorruptDataException("tables.txt: " + line);
        std::uint32_t id     = static_cast<std::uint32_t>(std::stoul(parts[0]));
        int           seats  = std::stoi(parts[1]);
        TableStatus   status = TableStatus::Free;
        if (!parseTableStatus(pos::util::trim(parts[2]), status)) {
            throw CorruptDataException("tables.txt: bad status in: " + line);
        }
        std::uint32_t cid    = static_cast<std::uint32_t>(std::stoul(parts[3]));
        std::int64_t  rEpoch = std::stoll(parts[4]);
        std::optional<CustomerId> resFor;
        if (cid != 0) resFor = CustomerId{cid};
        out.emplace_back(TableId{id}, seats, status, resFor, DateTime::fromEpoch(rEpoch));
    }
    return out;
}

void TableRepository::saveAll(const std::vector<Table>& tables) {
    std::vector<std::string> lines;
    lines.reserve(tables.size());
    for (const auto& t : tables) {
        std::ostringstream oss;
        std::uint32_t cid = t.reservedFor() ? t.reservedFor()->value() : 0u;
        oss << t.id().value() << "|"
            << t.seats() << "|"
            << tableStatusName(t.status()) << "|"
            << cid << "|"
            << t.reservedAt().epoch();
        lines.push_back(oss.str());
    }
    writeAllLines(lines, "id|seats|status|reservedForCustomerId|reservedAtEpoch");
}

} // namespace pos::domain
