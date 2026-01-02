#include <algorithm>
#include <iterator>
#include <set>
#include <utility>
#include <sstream>
#include <string>
#include <vector>
#include <functional>
#include <ostream>
#include "../common.h"

using operation = char;
using operation_group = std::pair<operation, std::vector<long>>;

[[nodiscard]] auto rotate_and_cast(const auto& unrotated_grid) -> std::vector<operation_group> {
    std::vector<operation_group> rotated;
    const auto op_index{unrotated_grid.size() - 1};

    for (auto j{0uz}; j < unrotated_grid[op_index].size(); ++j) {
        char operation{unrotated_grid[op_index][j][0]};
        rotated.emplace_back(operation, std::vector<long>{});

        for (auto i{0uz}; i < unrotated_grid.size() - 1; ++i)
            rotated.back().second.emplace_back(stoi(unrotated_grid[i][j]));
    }

    return rotated;
}

[[nodiscard]] auto get_operations() -> std::vector<operation_group> {
    auto f{AOC_INPUT()};

    std::string line;
    std::vector<std::vector<std::string>> unrotated_grid;

    while (std::getline(f, line) && !line.empty()) {
        std::istringstream line_stream{line};
        unrotated_grid.emplace_back(std::istream_iterator<std::string>{line_stream},
            std::istream_iterator<std::string>{});
    }

    return rotate_and_cast(unrotated_grid);
}

[[nodiscard]] auto execute_and_sum_operations() -> long long {
    const auto operation_vector_pairs{get_operations()};
    long long total{};

    for (const auto& [operation, numbers] : operation_vector_pairs) {
        if (operation == '+')
            total += std::ranges::fold_left(numbers, 0, std::plus());
        else
            total += std::ranges::fold_left(numbers, 1, std::multiplies());
    }

    return total;
}

auto main() -> int {
    std::println("{}", execute_and_sum_operations());
}
