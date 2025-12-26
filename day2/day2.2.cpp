#include <algorithm>
#include <sstream>
#include <print>
#include <ranges>
#include <vector>
#include <__ranges/iota_view.h>
#include "../common.h"

[[nodiscard]] auto get_ids() -> std::vector<std::pair<long, long>> {
    std::vector<std::pair<long, long>> id_ranges;

    {
        auto f{AOC_INPUT()};
        std::string line;

        while (std::getline(f, line)) {
            std::istringstream line_stream{line};
            std::string token;

            while (std::getline(line_stream, token, ',')) {
                long first{};
                long second{};
                char dash{};
                std::istringstream{token} >> first >> dash >> second;

                id_ranges.emplace_back(first, second);
            }
        }
    }

    return id_ranges;
}

[[nodiscard]] auto all_parts_equal(const std::string& id_string, const int num_parts) -> bool {
    const auto n{id_string.length()};

    if (modulo(static_cast<int>(n), num_parts) != 0) return false;

    const int d{static_cast<int>(n) / num_parts};

    for (auto i{d}; i < n; i += d)
        if (id_string.compare(i, d, id_string, 0, d) != 0)
            return false;

    return true;
}

[[nodiscard]] auto sum_invalid_range(const long start, const long end) -> long {
    long total{};

    for (const auto product_id : std::ranges::views::iota(start, end + 1)) {
        const auto id_string = std::to_string(product_id);
        const auto n = static_cast<int>(id_string.length());

        if (std::ranges::any_of(std::views::iota(2, n + 1), [&](const int parts) {
            return all_parts_equal(id_string, parts);
        }))
            total += product_id;
    }

    return total;
}

[[nodiscard]] auto sum_invalid() -> long {
    long total{};

    for (auto [start, end] : get_ids())
        total += sum_invalid_range(start, end);

    return total;
}

auto main() -> int {
    std::println("{}", sum_invalid());

    return 0;
}
