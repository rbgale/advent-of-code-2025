#include <ostream>
#include <unordered_set>
#include <vector>
#include "../common.h"

int ACCESS_THRESHOLD{3};

[[nodiscard]] auto get_roll_grid() -> std::vector<std::vector<char>> {
    std::vector<std::vector<char>> roll_grid;

    auto f{AOC_INPUT()};
    std::string line;

    while (std::getline(f, line)) {
        roll_grid.emplace_back();
        for (const char roll : line)
            roll_grid.back().push_back(roll);
    }

    return roll_grid;
}

[[nodiscard]] auto is_in_bounds(
    const std::vector<std::vector<char>>& grid,
    const int i,
    const int j
) -> bool {
    return 0 <= i && i < grid.size() && 0 <= j && j < grid[i].size();
}

[[nodiscard]] auto count_adjacent_rolls(
    const std::vector<std::vector<char>>& grid,
    const int i,
    const int j
) -> int {
    int total{};

    for (auto h_offset{-1}; h_offset <= 1; ++h_offset)
        for (auto v_offset{-1}; v_offset <= 1; ++v_offset)
            if ((h_offset != 0 || v_offset != 0) &&
                is_in_bounds(grid, i + h_offset, j + v_offset) &&
                grid[i + h_offset][j + v_offset] == '@')
                ++total;

    return total;
}

[[nodiscard]] auto count_accessible_rolls(const int access_threshold) -> int {
    int total{};

    auto grid{get_roll_grid()};
    std::unordered_set<std::pair<int, int>, PairHash> to_remove;

    do {
        to_remove.clear();

        for (auto i{0uz}; i < grid.size(); ++i)
            for (auto j{0uz}; j < grid[i].size(); ++j)
                if (grid[i][j] == '@' &&
                    count_adjacent_rolls(grid, static_cast<int>(i), static_cast<int>(j)) <= access_threshold)
                    to_remove.insert({i, j});

        for (const auto& [i, j] : to_remove)
            grid[i][j] = '.';

        total += static_cast<int>(to_remove.size());
    } while (!to_remove.empty());

    return total;
}

auto main() -> int {
    std::println("{}", count_accessible_rolls(ACCESS_THRESHOLD));

    return 0;
}
