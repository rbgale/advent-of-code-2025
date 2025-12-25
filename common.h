#pragma once

#include <filesystem>
#include <fstream>

inline std::ifstream open_input(const std::filesystem::path& source_file) {
    const auto path = std::filesystem::path{source_file}.parent_path() / "input.txt";
    std::ifstream f{path};

    if (!f)
        throw std::runtime_error(std::format("Unable to open {}", path.string()));

    return f;
}

#define AOC_INPUT() open_input(__FILE__)
