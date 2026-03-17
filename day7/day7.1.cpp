#include <ostream>
#include <vector>
#include <ranges>
#include "../common.h"

[[nodiscard]] auto initialize_beam(const auto& line) -> std::vector<bool> {
    std::vector<bool> beam(line.length(), false);

    for (auto i{0uz}; i < line.length(); ++i)
        if (line[i] == 'S') beam[i] = true;

    return beam;
}

[[nodiscard]] auto update_and_count_line_splits(auto& current_beams, const auto& line) -> long long {
    long long splits{};

    for (auto i{0uz}; i < line.length(); ++i) {
        if (line[i] == '^' && current_beams[i]) {
            current_beams[i] = false;
            if (i > 0) current_beams[i - 1] = true;
            if (i < current_beams.size() - 1) current_beams[i + 1] = true;

            splits++;
        }
    }

    return splits;
}

[[nodiscard]] auto count_beam_splits() -> long long {
    auto f{AOC_INPUT()};

    std::string line;
    std::getline(f, line);

    auto current_beams{initialize_beam(line)};
    long long total{};

    while (std::getline(f, line) && !line.empty())
        total += update_and_count_line_splits(current_beams, line);

    return total;
}

auto main() -> int {
    std::println("{}", count_beam_splits());
}
