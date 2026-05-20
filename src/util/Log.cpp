#include "util/Log.h"
#include "domain/common/DateTime.h"

#include <fstream>
#include <mutex>
#include <string>

namespace pos::util {

namespace {
    std::mutex g_mu;
    std::string g_path = "data/log.txt";
}

void Log::setLogPath(const std::string& path) {
    std::lock_guard<std::mutex> lk(g_mu);
    g_path = path;
}

void Log::writeLine(std::string_view level, std::string_view module, std::string_view message) {
    std::lock_guard<std::mutex> lk(g_mu);
    std::ofstream out(g_path, std::ios::app);
    if (!out) return; // logging must never throw
    out << pos::domain::DateTime::now().formatIso()
        << " " << level << " "
        << module << " "
        << message << "\n";
}

void Log::info (std::string_view m, std::string_view s) { writeLine("INFO ", m, s); }
void Log::warn (std::string_view m, std::string_view s) { writeLine("WARN ", m, s); }
void Log::error(std::string_view m, std::string_view s) { writeLine("ERROR", m, s); }

} // namespace pos::util
