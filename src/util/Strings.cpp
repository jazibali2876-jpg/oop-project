#include "util/Strings.h"

#include <algorithm>
#include <cctype>

namespace pos::util {

std::string trim(std::string_view s) {
    auto start = std::find_if_not(s.begin(), s.end(),
        [](unsigned char c){ return std::isspace(c); });
    auto end = std::find_if_not(s.rbegin(), s.rend(),
        [](unsigned char c){ return std::isspace(c); }).base();
    return (start < end) ? std::string(start, end) : std::string();
}

std::vector<std::string> split(std::string_view s, char delim, bool keepEmpty) {
    std::vector<std::string> out;
    std::string cur;
    for (char c : s) {
        if (c == delim) {
            if (!cur.empty() || keepEmpty) out.push_back(std::move(cur));
            cur.clear();
        } else {
            cur.push_back(c);
        }
    }
    if (!cur.empty() || keepEmpty) out.push_back(std::move(cur));
    return out;
}

std::string toLower(std::string_view s) {
    std::string out;
    out.reserve(s.size());
    for (char c : s) out.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
    return out;
}

bool iContains(std::string_view haystack, std::string_view needle) {
    if (needle.empty()) return true;
    if (needle.size() > haystack.size()) return false;
    auto lh = toLower(haystack);
    auto ln = toLower(needle);
    return lh.find(ln) != std::string::npos;
}

std::string join(const std::vector<std::string>& parts, char delim) {
    std::string out;
    for (std::size_t i = 0; i < parts.size(); ++i) {
        if (i) out.push_back(delim);
        out += parts[i];
    }
    return out;
}

} // namespace pos::util
