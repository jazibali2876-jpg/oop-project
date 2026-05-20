#pragma once

#include <random>
#include <string>

namespace pos::util {

class Random {
public:
    static std::mt19937& engine() {
        thread_local std::mt19937 rng{std::random_device{}()};
        return rng;
    }

    static std::string salt(std::size_t len = 8) {
        static const char alphabet[] = "abcdefghijklmnopqrstuvwxyz0123456789";
        std::uniform_int_distribution<int> dist(0, sizeof(alphabet) - 2);
        std::string out;
        out.reserve(len);
        for (std::size_t i = 0; i < len; ++i) out.push_back(alphabet[dist(engine())]);
        return out;
    }
};

} // namespace pos::util
