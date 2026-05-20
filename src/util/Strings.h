#pragma once

#include <string>
#include <vector>
#include <string_view>

namespace pos::util {

std::string trim(std::string_view s);
std::vector<std::string> split(std::string_view s, char delim, bool keepEmpty = true);
std::string toLower(std::string_view s);
bool iContains(std::string_view haystack, std::string_view needle);
std::string join(const std::vector<std::string>& parts, char delim);

} // namespace pos::util
