#include <print>
#include <string>
#include <ranges>
#include <vector>
#include "../common.h"

[[nodiscard]] auto get_banks() -> std::vector<std::vector<int>> {
    std::vector<std::vector<int>> banks;

    auto f{AOC_INPUT()};
    std::string line;

    while (std::getline(f, line)) {
        banks.emplace_back();
        for (const char jolt : line)
            banks.back().push_back(jolt - '0');
    }

    return banks;
}

[[nodiscard]] auto max_bank_joltage(const std::vector<int>& bank) -> int {
    auto n{bank.size()};
    int left_max{bank[n - 2]};
    int right_max{bank[n - 1]};

    for (const int jolt : std::views::reverse(bank) | std::views::drop(2)) {
        if (jolt >= left_max) {
            right_max = std::max(left_max, right_max);
            left_max = jolt;
        }
    }

    return left_max * 10 + right_max;
}

[[nodiscard]] auto get_total_joltage() -> int {
    auto banks{get_banks()};
    int total{};

    for (const auto& bank : banks)
        total += max_bank_joltage(bank);

    return total;
}

auto main() -> int {
    std::println("{}", get_total_joltage());

    return 0;
}
