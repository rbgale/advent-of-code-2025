#include <algorithm>
#include <vector>
#include <print>
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

auto is_satisfied(const Machine& machine) -> bool {
    return std::ranges::all_of(machine.joltages, [](const int joltage) {
        return joltage == 0;
    });
}

auto minimum_remaining_presses_needed(const Machine& machine) -> int {
    return std::ranges::max(machine.joltages);
}

auto should_prune(
    const Machine& machine,
    const uint64_t used,
    const long long previous_presses,
    const std::optional<long long> min_total_presses
) -> bool {
    if (used == (1ULL << machine.buttons.size()) - 1) return true;

    if (!min_total_presses) return false;

    return previous_presses + minimum_remaining_presses_needed(machine) >= *min_total_presses;
}

auto button_press_limit(const Machine& machine, const int b) -> int {
    return std::ranges::fold_left(machine.buttons[b], INT_MAX, [&](const int acc, const int i) {
        return std::min(acc, machine.joltages[i]);
    });
}

struct ForcedResult {
    long long presses;
    uint64_t newly_forced;
    std::vector<std::pair<int, int>> adjustments;
};

auto force_button_assignments(Machine& machine, uint64_t& used) -> std::optional<ForcedResult> {
    long long forced{};
    uint64_t newly_forced{};
    std::vector<std::pair<int, int>> adjustments{};
    bool changed{true};

    while (changed) {
        changed = false;
        for (int j{}; j < machine.joltages.size(); j++) {
            if (machine.joltages[j] == 0) continue;

            int affecting_button{-1};
            int count{};
            for (int b{}; b < machine.buttons.size(); b++) {
                if (used & (1ULL << b)) continue;
                if (std::ranges::find(machine.buttons[b], j) != machine.buttons[b].end()) {
                    affecting_button = b;
                    count++;
                }
                if (count > 1) break;
            }

            if (count == 0) return std::nullopt;
            if (count == 1) {
                int required = machine.joltages[j];
                for (const int i : machine.buttons[affecting_button]) {
                    if (machine.joltages[i] < required) return std::nullopt;
                }
                for (int idx : machine.buttons[affecting_button]) {
                    machine.joltages[idx] -= required;
                    adjustments.emplace_back(idx, required);
                }
                forced += required;
                newly_forced |= 1ULL << affecting_button;
                used |= 1ULL << affecting_button;
                changed = true;
                break;
            }
        }
    }

    return ForcedResult{forced, newly_forced, adjustments};
}

auto get_last_button(const Machine& machine, const uint64_t used) -> int {
    int last_button{-1};
    for (int b{static_cast<int>(machine.buttons.size()) - 1}; b >= 0; b--) {
        if (!(used & (1ULL << b))) {
            last_button = b;
            break;
        }
    }

    return last_button;
}

auto minimum_presses(
    Machine& machine,
    uint64_t used,
    const long long previous_presses,
    std::optional<long long>& min_total_presses
) -> std::optional<long long> {
    if (is_satisfied(machine)) return 0;
    if (should_prune(machine, used, previous_presses, min_total_presses)) return std::nullopt;

    const auto saved_joltages{machine.joltages};
    const auto forced{force_button_assignments(machine, used)};
    if (!forced) {
        machine.joltages = saved_joltages;
        return std::nullopt;
    }
    if (forced->presses > 0) {
        const auto remaining_presses{minimum_presses(
            machine,
            used,
            previous_presses + forced->presses,
            min_total_presses
        )};
        machine.joltages = saved_joltages;
        return remaining_presses ? std::optional(forced->presses + *remaining_presses) : std::nullopt;
    }
    machine.joltages = saved_joltages;

    std::optional<long long> min_remaining_presses{std::nullopt};
    const auto last_button{get_last_button(machine, used)};
    const auto limit{button_press_limit(machine, last_button)};

    for (auto presses{0}; presses <= limit; ++presses) {
        for (const auto i : machine.buttons[last_button]) machine.joltages[i] -= presses;
        const auto next_presses{minimum_presses(
            machine,
            used | (1ULL << last_button),
            previous_presses + presses,
            min_total_presses
        )};
        for (const auto i : machine.buttons[last_button]) machine.joltages[i] += presses;

        if (!next_presses) continue;

        const auto remaining_presses{presses + *next_presses};
        const auto total_presses{previous_presses + remaining_presses};

        if (!min_remaining_presses || remaining_presses < *min_remaining_presses)
            min_remaining_presses = remaining_presses;

        if (!min_total_presses || total_presses < *min_total_presses)
            min_total_presses = total_presses;
    }

    return min_remaining_presses;
}

auto minimum_total_presses() -> long long {
    return std::ranges::fold_left(parse_input(), 0, [](const long long acc, Machine& machine) {
        std::optional<long long> min_total_presses{std::nullopt};
        return acc + minimum_presses(machine, 0ULL, 0, min_total_presses).value();
    });
}

auto main() -> int {
    std::println("{}", minimum_total_presses());
}
