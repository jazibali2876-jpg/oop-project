#include "domain/persistence/SettingsRepository.h"
#include "domain/persistence/FilePaths.h"
#include "util/Strings.h"

#include <sstream>

namespace pos::domain {

SettingsRepository::SettingsRepository() : TextRepository(FilePaths::Settings) {}

void SettingsRepository::load() {
    values_.clear();
    for (const auto& line : readAllLines()) {
        auto eq = line.find('=');
        if (eq == std::string::npos) continue;
        std::string k = pos::util::trim(line.substr(0, eq));
        std::string v = pos::util::trim(line.substr(eq + 1));
        values_[k] = v;
    }
}

void SettingsRepository::persist() {
    std::vector<std::string> lines;
    lines.reserve(values_.size());
    for (const auto& [k, v] : values_) lines.push_back(k + "=" + v);
    writeAllLines(lines, "Smart POS settings");
}

std::optional<std::string> SettingsRepository::get(const std::string& key) const {
    auto it = values_.find(key);
    if (it == values_.end()) return std::nullopt;
    return it->second;
}

std::string SettingsRepository::get(const std::string& key, const std::string& fallback) const {
    auto v = get(key);
    return v ? *v : fallback;
}

int SettingsRepository::getInt(const std::string& key, int fallback) const {
    auto v = get(key);
    if (!v) return fallback;
    try { return std::stoi(*v); } catch (...) { return fallback; }
}

bool SettingsRepository::getBool(const std::string& key, bool fallback) const {
    auto v = get(key);
    if (!v) return fallback;
    return *v == "1" || pos::util::toLower(*v) == "true";
}

void SettingsRepository::set(const std::string& key, const std::string& value) {
    values_[key] = value;
    persist();
}

} // namespace pos::domain
