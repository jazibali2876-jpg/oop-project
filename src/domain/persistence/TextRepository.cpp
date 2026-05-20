#include "domain/persistence/TextRepository.h"
#include "domain/common/Exceptions.h"
#include "util/Files.h"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <utility>

namespace pos::domain {

TextRepository::TextRepository(std::string path) : path_(std::move(path)) {}

std::vector<std::string> TextRepository::readAllLines() const {
    std::vector<std::string> out;
    std::ifstream in(path_);
    if (!in) return out; // missing file is OK; treat as empty
    std::string line;
    while (std::getline(in, line)) {
        // Strip trailing '\r' from CRLF files.
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.empty()) continue;
        if (line[0] == '#') continue;
        out.push_back(std::move(line));
    }
    if (in.bad()) throw FileIOException(path_, "read");
    return out;
}

void TextRepository::writeAllLines(const std::vector<std::string>& lines,
                                   const std::string& header) {
    ensureDirectory();
    std::ostringstream oss;
    if (!header.empty()) oss << "# " << header << "\n";
    for (const auto& l : lines) oss << l << "\n";
    pos::util::writeAtomic(path_, oss.str());
}

void TextRepository::ensureDirectory() const {
    std::filesystem::path p(path_);
    auto dir = p.parent_path();
    if (!dir.empty()) pos::util::mkdirs(dir.string());
}

} // namespace pos::domain
