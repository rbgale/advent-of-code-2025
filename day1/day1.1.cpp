#include <cstdlib>
#include <fstream>
#include <iostream>
#include <print>
#include <string>
#include <vector>
#include "../common.h"

int STARTING_POSITION = 50;
int DEFAULT_DIAL_SIZE = 100;

[[nodiscard]] constexpr auto rotate(const int start, const int shift, const int modulus) -> int {
    return modulo(start + shift, modulus);
}

[[nodiscard]] auto parse_rotation(const std::string_view line) -> int {
    const int sign = line.front() == 'R' ? 1 : -1;
    return sign * std::stoi(std::string{line.substr(1)});
}

[[nodiscard]] auto get_rotations() -> std::vector<int> {
    std::vector<int> rotations;

    {
        auto f = AOC_INPUT();
        std::string line;
        while (std::getline(f, line))
            rotations.push_back(parse_rotation(line));
    }

    return rotations;
}

[[nodiscard]] auto count_zeros(const int dial_size) -> int {
    int total = 0;
    int position = STARTING_POSITION;

    for (const int shift : get_rotations()) {
        position = rotate(position, shift, dial_size);

        if (position == 0) ++total;
    }

    return total;
}

auto main() -> int {
    std::println("{}", count_zeros(DEFAULT_DIAL_SIZE));

    return 0;
}
