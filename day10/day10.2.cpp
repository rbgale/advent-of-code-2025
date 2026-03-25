// This solution was borrowed from:
// https://www.reddit.com/r/adventofcode/comments/1pk87hl/2025_day_10_part_2_bifurcate_your_way_to_victory/

#include <algorithm>
#include <vector>
#include <print>
#include <unordered_map>
#include <ranges>
#include <sstream>
#include "../common.h"

struct Machine {
    std::vector<std::vector<int>> buttons{};
    std::vector<int> joltages{};
};

// input string "0,3,4" becomes std::vector<int>{0, 3, 4}
auto parse_int_list(const std::string& int_list) -> std::vector<int> {
    std::vector<int> result{};
    std::istringstream stream{int_list};
    std::string token{};

    while (getline(stream, token, ',') && !token.empty())
        result.emplace_back(std::stoi(token));

    return result;
}

auto parse_line(const std::string& line) -> Machine {
    std::vector<std::vector<int>> buttons{};
    auto button_start{line.find('(')};
    auto button_end{button_start + 1};
    while (button_start != std::string::npos) {
        button_end = line.find(')', button_start);
        buttons.emplace_back(parse_int_list(line.substr(button_start + 1, button_end - button_start - 1)));
        button_start = line.find('(', button_start + 1);
    }

    const auto joltage_start{line.find('{', button_end + 1)};
    const auto joltage_end{line.find('}', joltage_start)};
    const auto joltages{parse_int_list(line.substr(joltage_start + 1, joltage_end - joltage_start - 1))};

    return {buttons, joltages};
}

auto parse_input() -> std::vector<Machine> {
    auto f{AOC_INPUT()};

    std::vector<Machine> machines;
    std::string line;

    while (std::getline(f, line) && !line.empty())
        machines.push_back(parse_line(line));

    return machines;
}

auto build_power_set(const size_t n) -> std::vector<std::vector<size_t>> {
    const long long max_bitmask{1LL << n};
    std::vector<std::vector<size_t>> subsets(max_bitmask);

    for (auto mask{0uz}; mask < max_bitmask; ++mask)
        for (auto bit{0uz}; bit < n; ++bit)
            if (mask & (1LL << bit))
                subsets[mask].push_back(bit);

    return subsets;
}

auto button_joltage_parity(const Machine& machine, const size_t button_index) -> uint64_t {
    return std::ranges::fold_left(machine.buttons[button_index], 0,
        [](const uint64_t acc, const size_t joltage_index) { return acc ^ 1 << joltage_index; }
    );
}

// get the parity with which a set of buttons toggles the joltages (1 = odd; 0 = even)
// e.g., with 4 joltages and buttons {(0, 1), (1, 2), (2, 3)}, the subset {(0, 1), (1, 2)} would
// produce parity 0b1010
auto button_set_joltage_parity(
    const Machine& machine,
    const std::vector<size_t>& button_indices
) -> uint64_t {
    return std::ranges::fold_left(button_indices, 0,
        [&](const uint64_t acc, const size_t button_index) {
            return acc ^ button_joltage_parity(machine, button_index);
        }
    );
}

using ParityMap = std::unordered_map<uint64_t, std::vector<std::vector<size_t>>>;

// map each parity of the joltages to the collection of (indices of) button subsets which produce them
auto build_joltage_parity_map(const Machine& machine)
    -> ParityMap {

    ParityMap parity_to_button_indices{};
    for (const auto& button_indices : build_power_set(machine.buttons.size()))
        parity_to_button_indices[button_set_joltage_parity(machine, button_indices)].push_back(button_indices);

    return parity_to_button_indices;
}

auto is_satisfied(const Machine& machine) -> bool {
    return std::ranges::all_of(machine.joltages, [](const int joltage) {
        return joltage == 0;
    });
}

auto get_joltages_parity(const Machine& machine) -> uint64_t {
    uint64_t joltages_parity{0uz};

    for (auto i{0uz}; i < machine.joltages.size(); ++i) {
        size_t joltage_parity = machine.joltages[i] % 2 ? 1 : 0;
        joltages_parity ^= joltage_parity << i;
    }

    return joltages_parity;
}

auto cannot_press_buttons(const Machine& machine, const std::vector<size_t>& button_subset) -> bool {
    std::vector<int> joltage_reductions(machine.joltages.size());
    for (const auto& button : button_subset | std::views::transform([&](const int i) {return machine.buttons[i];}))
        for (const auto& joltage : button)
            joltage_reductions[joltage]++;

    for (const auto& [i, joltage] : std::views::zip(std::views::iota(0), machine.joltages))
        if (joltage_reductions[i] > joltage) return true;

    return false;
}

auto apply_buttons(Machine& machine, const std::vector<size_t>& button_subset, const int press) -> void {
    for (const auto& button : button_subset | std::views::transform([&](const int i) {return machine.buttons[i];}))
        for (const auto& joltage : button)
            machine.joltages[joltage] += press;
}

auto shift_joltages(Machine& machine, const int shift_amount) -> void {
    for (auto i{0uz}; i < machine.joltages.size(); ++i) {
        if (shift_amount >= 0) machine.joltages[i] <<= shift_amount;
        else machine.joltages[i] >>= -shift_amount;
    }
}

auto minimum_presses(Machine& machine, const ParityMap& parity_to_button_indices) -> std::optional<long long> {
    if (is_satisfied(machine)) return 0;

    std::optional<long long> min_presses{std::nullopt};
    const auto iterator{parity_to_button_indices.find(get_joltages_parity(machine))};

    if (iterator == parity_to_button_indices.end()) return std::nullopt;

    const auto satisfiable_button_subsets{iterator->second};
    for (const auto& button_subset : satisfiable_button_subsets) {
        if (cannot_press_buttons(machine, button_subset)) continue;

        apply_buttons(machine, button_subset, -1);
        shift_joltages(machine, -1);
        if (const auto remainder{minimum_presses(machine, parity_to_button_indices)}) {
            const auto presses{button_subset.size() + 2 * *remainder};
            if (!min_presses || presses < *min_presses)
                min_presses = presses;
        }
        shift_joltages(machine, 1);
        apply_buttons(machine, button_subset, +1);
    }

    return min_presses;
}

auto minimum_total_presses() -> long long {
    return std::ranges::fold_left(parse_input(), 0, [](auto acc, auto& machine) {
        return acc + *minimum_presses(machine, build_joltage_parity_map(machine));
    });
}

auto main() -> int {
    std::println("{}", minimum_total_presses());
}
