#include <sstream>
#include <print>
#include <ranges>
#include <vector>
#include <__ranges/iota_view.h>
#include "../common.h"

[[nodiscard]] auto get_ids() -> std::vector<std::pair<long, long>> {
    std::vector<std::pair<long, long>> id_ranges;

    {
        auto f = AOC_INPUT();
        std::string line;

        while (std::getline(f, line)) {
            std::istringstream lineStream{line};
            std::string token;

            while (std::getline(lineStream, token, ',')) {
                long first = 0;
                long second = 0;
                char dash = '\0';
                std::istringstream{token} >> first >> dash >> second;

                id_ranges.emplace_back(first, second);
            }
        }
    }

    return id_ranges;
}

[[nodiscard]] auto sum_invalid_range(const long start, const long end) -> long {
    long total = 0;

    for (const auto product_id : std::ranges::views::iota(start, end + 1)) {
        const auto id_string = std::to_string(product_id);
        const auto n = id_string.length();

        if (0 == modulo(static_cast<int>(n), 2) && id_string.substr(0, n / 2) == id_string.substr(n / 2))
            total += product_id;
    }

    return total;
}

[[nodiscard]] auto sum_invalid() -> long {
    long total = 0;

    for (auto [start, end] : get_ids())
        total += sum_invalid_range(start, end);

    return total;
}

auto main() -> int {
    std::println("{}", sum_invalid());

    return 0;
}
