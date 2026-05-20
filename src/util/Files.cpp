#include "util/Files.h"
#include "domain/common/Exceptions.h"

#include <filesystem>
#include <fstream>
#include <system_error>

#if defined(_WIN32)
    #include <windows.h>
#endif

namespace pos::util {

namespace fs = std::filesystem;

bool exists(const std::string& path) {
    std::error_code ec;
    return fs::exists(path, ec);
}

bool isFile(const std::string& path) {
    std::error_code ec;
    return fs::is_regular_file(path, ec);
}

bool isDirectory(const std::string& path) {
    std::error_code ec;
    return fs::is_directory(path, ec);
}

bool mkdirs(const std::string& path) {
    std::error_code ec;
    fs::create_directories(path, ec);
    return !ec;
}

bool removeFile(const std::string& path) {
    std::error_code ec;
    return fs::remove(path, ec);
}

void writeAtomic(const std::string& path, std::string_view content) {
    std::string tmp = path + ".tmp";
    {
        std::ofstream out(tmp, std::ios::binary | std::ios::trunc);
        if (!out) throw pos::domain::FileIOException(tmp, "open-for-write");
        out.write(content.data(), static_cast<std::streamsize>(content.size()));
        if (!out) throw pos::domain::FileIOException(tmp, "write");
    }
    std::error_code ec;
    fs::rename(tmp, path, ec);
    if (ec) {
        // On some Windows filesystems, atomic rename may fail if the destination
        // already exists — fall back to remove-then-rename.
        fs::remove(path, ec);
        fs::rename(tmp, path, ec);
        if (ec) throw pos::domain::FileIOException(path, "rename");
    }
}

std::vector<unsigned char> readAllBytes(const std::string& path) {
    std::ifstream in(path, std::ios::binary | std::ios::ate);
    if (!in) return {};
    std::streamsize n = in.tellg();
    if (n <= 0) return {};
    in.seekg(0, std::ios::beg);
    std::vector<unsigned char> out(static_cast<std::size_t>(n));
    if (!in.read(reinterpret_cast<char*>(out.data()), n)) return {};
    return out;
}

std::string exeDirectory() {
#if defined(_WIN32)
    char buf[MAX_PATH];
    DWORD n = GetModuleFileNameA(nullptr, buf, MAX_PATH);
    if (n == 0 || n == MAX_PATH) return fs::current_path().string();
    return fs::path(buf).parent_path().string();
#else
    return fs::current_path().string();
#endif
}

std::string joinPath(const std::string& a, const std::string& b) {
    fs::path p = fs::path(a) / b;
    return p.string();
}

} // namespace pos::util
