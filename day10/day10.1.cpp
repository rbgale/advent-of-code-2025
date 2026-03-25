#include <algorithm>
#include <vector>
#include <print>
#include <ranges>
#include <sstream>
#include "../common.h"

struct Machine {
    uint64_t lights{};
    std::vector<uint64_t> buttons{};
};

// input string "#..##." becomes 25
auto parse_bitstring_to_uint(const std::string& bitstring) -> uint64_t {
    uint64_t result{0};
    for (auto i{0uz}; i < bitstring.size(); ++i)
        if (bitstring[i] == '#') result |= uint64_t{1} << i;

    return result;
}

// input string "0,3,4" becomes 25
auto parse_set_bits_to_uint(const std::string& set_bits) -> uint64_t {
    uint64_t result{0};
    std::istringstream stream{set_bits};
    std::string token{};

    while (getline(stream, token, ',') && !token.empty())
        result |= 1ULL << std::stoi(token);

    return result;
}

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
    const auto lights_start{line.find('[')};
    const auto lights_end{line.find(']', lights_start)};
    const auto lights{parse_bitstring_to_uint(line.substr(lights_start + 1, lights_end - lights_start - 1))};

    std::vector<uint64_t> buttons{};
    auto button_start{line.find('(', lights_end + 1)};
    while (button_start != std::string::npos) {
        const auto button_end{line.find(')', button_start)};
        buttons.emplace_back(parse_set_bits_to_uint(line.substr(button_start + 1, button_end - button_start - 1)));
        button_start = line.find('(', button_start + 1);
    }

    return {lights, buttons};
}

auto parse_input() -> std::vector<Machine> {
    auto f{AOC_INPUT()};

    std::vector<Machine> machines;
    std::string line;

    while (std::getline(f, line) && !line.empty())
        machines.push_back(parse_line(line));

    return machines;
}

auto joltage_configuration(const std::vector<uint64_t>& buttons, const std::vector<bool>& mask) -> uint64_t {
    uint64_t result{0};
    for (const auto& [button, is_active] : std::views::zip(buttons, mask))
        if (is_active) result ^= button;

    return result;
}

auto has_size_n_subset_cover(const Machine& machine, const int k) -> bool {
    const auto n{machine.buttons.size()};
    std::vector mask(n - k, false);
    mask.resize(n, true);

    do {
        if (joltage_configuration(machine.buttons, mask) == machine.lights)
            return true;
    } while (std::ranges::next_permutation(mask).found);

    return false;
}

auto minimum_presses(const Machine& machine) -> int {
    for (auto k{0}; k <= machine.buttons.size(); ++k)
        if (has_size_n_subset_cover(machine, k)) return k;

    throw std::logic_error{"No valid selection of button presses"};
}

auto minimum_total_presses() -> long long {
    return std::ranges::fold_left(parse_input(), 0, [](auto acc, const auto& machine) {
        return acc + minimum_presses(machine);
    });
}

auto main() -> int {
    std::println("{}", minimum_total_presses());
}
