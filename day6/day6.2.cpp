#include <algorithm>
#include <iterator>
#include <set>
#include <sstream>
#include <string>
#include <vector>
#include <functional>
#include <ostream>
#include <ranges>
#include "../common.h"

enum class Operation_t : char {
    Plus = '+',
    Multiply = '*'
};
struct OperationGroup {
    Operation_t operation;
    std::vector<long> operands;
};

[[nodiscard]] auto is_nonempty_column(const auto& grid, const auto j) -> bool {
    for (auto i{0uz}; i < grid.size() - 1; ++i) { // skip last row (not a number)
        if (j >= grid[i].size())
            throw std::out_of_range("Column index out of range");

        if (std::isdigit(grid[i][j])) return true;
    }

    return false;
}

[[nodiscard]] auto get_vertical_number(const auto& grid, const auto j) -> long {
    long vertical_number{};

    for (auto i{0uz}; i < grid.size() - 1; ++i) // skip last row (not a number)
        if (std::isdigit(grid[i][j]))
            vertical_number = 10 * vertical_number + grid[i][j] - '0';

    return vertical_number;
}

[[nodiscard]] auto parse_operation_groups(const auto& grid) -> std::vector<OperationGroup> {
    std::vector<OperationGroup> operation_groups;

    const auto operation_index{grid.size() - 1};
    const auto count_columns{std::ranges::max(grid | std::views::transform(&std::string::size))};
    auto j{0uz};

    while (j < count_columns) {
        const auto operation{static_cast<Operation_t>(grid[operation_index][j])};
        std::vector<long> vertical_numbers;

        while (j < count_columns && is_nonempty_column(grid, j))
            vertical_numbers.push_back(get_vertical_number(grid, j++));
        j++;

        operation_groups.emplace_back(operation, std::move(vertical_numbers));
    }

    return operation_groups;
}

[[nodiscard]] auto get_operations() -> std::vector<OperationGroup> {
    auto f{AOC_INPUT()};

    std::string line;
    std::vector<std::string> grid;

    while (std::getline(f, line) && !line.empty())
        grid.emplace_back(line);

    return parse_operation_groups(grid);
}

[[nodiscard]] auto execute_and_sum_operations() -> long long {
    const auto operation_vector_pairs{get_operations()};
    long long total{};

    for (const auto& [operation, numbers] : operation_vector_pairs) {
        if (operation == Operation_t::Plus)
            total += std::ranges::fold_left(numbers, 0, std::plus{});
        else
            total += std::ranges::fold_left(numbers, 1, std::multiplies{});
    }

    return total;
}

auto main() -> int {
    std::println("{}", execute_and_sum_operations());
}
