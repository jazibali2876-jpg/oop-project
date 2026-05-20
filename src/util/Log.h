#pragma once

#include <string>
#include <string_view>

namespace pos::util {

class Log {
public:
    static void info (std::string_view module, std::string_view message);
    static void warn (std::string_view module, std::string_view message);
    static void error(std::string_view module, std::string_view message);

    static void setLogPath(const std::string& path); // default: data/log.txt
private:
    static void writeLine(std::string_view level, std::string_view module, std::string_view message);
};

} // namespace pos::util
