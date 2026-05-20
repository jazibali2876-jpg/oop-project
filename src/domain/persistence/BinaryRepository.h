#pragma once

#include "domain/common/Exceptions.h"
#include "util/Files.h"

#include <cstdio>
#include <filesystem>
#include <fstream>
#include <optional>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace pos::domain {

// Template base for repositories backed by a flat binary file of POD records.
// T must be trivially copyable so we can serialize via raw bytes safely.
template <class T>
class BinaryRepository {
public:
    static_assert(std::is_trivially_copyable_v<T>,
                  "BinaryRepository<T>: T must be trivially copyable");

    explicit BinaryRepository(std::string path) : path_(std::move(path)) {}
    virtual ~BinaryRepository() = default;

    const std::string& path() const { return path_; }

    // Read all records. Missing file -> empty vector. Partial-record file -> CorruptDataException.
    std::vector<T> all() const {
        std::vector<T> out;
        std::ifstream in(path_, std::ios::binary);
        if (!in) return out;
        T rec{};
        while (in.read(reinterpret_cast<char*>(&rec), sizeof(T))) {
            out.push_back(rec);
        }
        if (in.bad()) throw FileIOException(path_, "read");
        if (!in.eof()) throw CorruptDataException(path_ + " (partial record)");
        return out;
    }

    // Append one record. Crash-safe append relies on OS append guarantee.
    void append(const T& rec) {
        ensureDirectory();
        std::ofstream out(path_, std::ios::binary | std::ios::app);
        if (!out) throw FileIOException(path_, "open-for-append");
        out.write(reinterpret_cast<const char*>(&rec), sizeof(T));
        if (!out) throw FileIOException(path_, "append");
    }

    // Replace the whole file atomically with the given vector.
    void overwriteAll(const std::vector<T>& v) {
        ensureDirectory();
        const std::string tmp = path_ + ".tmp";
        {
            std::ofstream out(tmp, std::ios::binary | std::ios::trunc);
            if (!out) throw FileIOException(tmp, "open-for-write");
            if (!v.empty()) {
                out.write(reinterpret_cast<const char*>(v.data()),
                          static_cast<std::streamsize>(v.size() * sizeof(T)));
                if (!out) throw FileIOException(tmp, "write");
            }
        }
        std::error_code ec;
        std::filesystem::rename(tmp, path_, ec);
        if (ec) {
            std::filesystem::remove(path_, ec);
            std::filesystem::rename(tmp, path_, ec);
            if (ec) throw FileIOException(path_, "rename");
        }
    }

    // Templated find-if helper.
    template <class Pred>
    std::optional<T> findIf(Pred p) const {
        for (auto& r : all()) if (p(r)) return r;
        return std::nullopt;
    }

protected:
    void ensureDirectory() const {
        std::filesystem::path p(path_);
        auto dir = p.parent_path();
        if (!dir.empty()) pos::util::mkdirs(dir.string());
    }

private:
    std::string path_;
};

} // namespace pos::domain
