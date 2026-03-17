#include <algorithm>
#include <print>
#include <vector>
#include <ranges>
#include "../common.h"

[[nodiscard]] auto initialize_beam(const auto& line) -> std::vector<long long> {
    std::vector<long long> beam(line.length(), 0);

    for (auto i{0uz}; i < line.length(); ++i)
        if (line[i] == 'S') beam[i] = 1;

    return beam;
}

auto update_line_path_counts(auto& current_beams, const auto& line) -> void {
    for (auto i{0uz}; i < line.length(); ++i) {
        if (line[i] == '^' && current_beams[i] > 0) {
            if (i > 0) current_beams[i - 1] += current_beams[i];
            if (i < current_beams.size() - 1) current_beams[i + 1] += current_beams[i];
            current_beams[i] = 0;
        }
    }
}

[[nodiscard]] auto count_beam_splits() -> long long {
    auto f{AOC_INPUT()};

    std::string line;
    std::getline(f, line);

    auto current_beams{initialize_beam(line)};

    while (std::getline(f, line) && !line.empty())
        update_line_path_counts(current_beams, line);

    return std::ranges::fold_left(current_beams, 0LL, std::plus<long long>{});
}

auto main() -> int {
    std::println("{}", count_beam_splits());
}
