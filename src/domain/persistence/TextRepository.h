#pragma once

#include <string>
#include <vector>

namespace pos::domain {

// Base class for repositories backed by line-oriented text files.
// Concrete subclasses are responsible for serialization of one line per record.
class TextRepository {
public:
    explicit TextRepository(std::string path);
    virtual ~TextRepository() = default;

    const std::string& path() const { return path_; }

protected:
    // Reads non-empty lines from the file. Lines starting with '#' are skipped.
    std::vector<std::string> readAllLines() const;

    // Atomically writes all lines back. Each line gets a trailing '\n'.
    // Optional header is written as the first line, prefixed with "# ".
    void writeAllLines(const std::vector<std::string>& lines,
                       const std::string& header = "");

    // Ensures the directory containing this file exists.
    void ensureDirectory() const;

private:
    std::string path_;
};

} // namespace pos::domain
