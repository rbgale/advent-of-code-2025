#pragma once

#include <filesystem>
#include <fstream>

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


