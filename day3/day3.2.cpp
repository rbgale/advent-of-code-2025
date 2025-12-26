#include <algorithm>
#include <ostream>
#include <string>
#include <ranges>
#include <vector>
#include "../common.h"

int NUM_BATTERIES{12};

[[nodiscard]] auto get_banks() -> std::vector<std::vector<int>> {
    std::vector<std::vector<int>> banks;

    {
        auto f{AOC_INPUT()};
        std::string line;

        while (std::getline(f, line)) {
            banks.emplace_back();
            for (const char jolt : line)
                banks.back().push_back(jolt - '0');
        }
    }

    return banks;
}

[[nodiscard]] auto restricted_argmax_search(
    const std::vector<int>& vector,
    const int start_offset,
    const int end_offset
) -> int {
    const auto subrange{vector
        | std::views::drop(start_offset)
        | std::views::take(vector.size() - start_offset - end_offset)
    };

    const auto max_iterator{std::ranges::max_element(subrange)};
    return start_offset + static_cast<int>(std::distance(subrange.begin(), max_iterator));
}

[[nodiscard]] auto max_bank_joltage(const std::vector<int>& bank, const int num_batteries) -> long long {
    auto batteries{std::string{}};

    int start_offset{};
    int end_offset{num_batteries - 1};

    for ([[maybe_unused]] auto _ : std::views::iota(0, num_batteries)) {
        int battery_index{restricted_argmax_search(bank, start_offset, end_offset)};
        batteries.push_back(static_cast<char>('0' + bank[battery_index]));

        start_offset = battery_index + 1;
        end_offset -= 1;
    }

    return std::stoll(batteries);
}

[[nodiscard]] auto get_total_joltage(const int num_batteries) -> long long {
    auto banks{get_banks()};
    long long total{};

    for (const auto& bank : banks) {
        total += max_bank_joltage(bank, num_batteries);
    }

    return total;
}

auto main() -> int {
    std::println("{}", get_total_joltage(NUM_BATTERIES));

    return 0;
}
