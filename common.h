#pragma once

#include <filesystem>
#include <fstream>
#include <unordered_set>

inline auto open_input(const std::filesystem::path& source_file) -> std::ifstream {
    const auto path = std::filesystem::path{source_file}.parent_path() / "input.txt";
    std::ifstream f{path};

    if (!f)
        throw std::runtime_error(std::format("Unable to open {}", path.string()));

    return f;
}

#define AOC_INPUT() open_input(__FILE__)

inline auto modulo(const int value, const int modulus) -> int {
    const int result = value % modulus;
    return result < 0 ? result + modulus : result;
}

// hashing method is from boost::hash_combine
template <typename T>
auto hash_combine(size_t& seed, const T& val) -> void {
    seed ^= std::hash<T>{}(val) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

struct PairHash {
    size_t operator()(const std::pair<int, int>& p) const {
        size_t seed = 0;
        hash_combine(seed, p.first);
        hash_combine(seed, p.second);

        return seed;
    }
};
