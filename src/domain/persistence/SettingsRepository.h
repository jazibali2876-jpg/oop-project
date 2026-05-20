#pragma once

#include "domain/persistence/TextRepository.h"

#include <map>
#include <optional>
#include <string>

namespace pos::domain {

class SettingsRepository : public TextRepository {
public:
    SettingsRepository();

    void                                  load();
    void                                  persist();
    const std::map<std::string,std::string>& all() const { return values_; }

    std::optional<std::string>            get(const std::string& key) const;
    std::string                           get(const std::string& key, const std::string& fallback) const;
    int                                   getInt(const std::string& key, int fallback) const;
    bool                                  getBool(const std::string& key, bool fallback) const;

    void                                  set(const std::string& key, const std::string& value);

private:
    std::map<std::string,std::string>     values_;
};

} // namespace pos::domain
