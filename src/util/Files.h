#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace pos::util {

bool exists(const std::string& path);
bool isFile(const std::string& path);
bool isDirectory(const std::string& path);
bool mkdirs(const std::string& path);
bool removeFile(const std::string& path);

// Write the entire content to <path>.tmp then atomically rename to <path>.
// Throws pos::domain::FileIOException on stream errors.
void writeAtomic(const std::string& path, std::string_view content);

// Read whole binary file. Returns empty on missing file (does not throw).
std::vector<unsigned char> readAllBytes(const std::string& path);

// Path helpers
std::string exeDirectory();
std::string joinPath(const std::string& a, const std::string& b);

} // namespace pos::util
