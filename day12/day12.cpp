#include <algorithm>
#include <utility>
#include <vector>
#include <ranges>
#include <sstream>
#include <print>
#include "../common.h"

struct Present {
    int area;
    std::vector<std::vector<char>> shape;

    explicit Present(std::vector<std::vector<char>> present_shape) :
        area{get_area(present_shape)}, shape{std::move(present_shape)} {}

    static auto get_area(const std::vector<std::vector<char>>& present_shape) -> int {
        int total{0};

        for (const auto& row : present_shape)
            for (const auto& value : row)
                if (value == '#') total++;

        return total;
    }
};

struct Region {
    int length;
    int width;
    std::vector<int> present_amounts;
};

auto parse_input() -> std::pair<std::vector<Present>, std::vector<Region>> {
    auto f{AOC_INPUT()};

    std::vector<Present> presents;
    std::string line;
    while (std::getline(f, line) && !line.empty()) {
        if (line.back() != ':') break;

        std::vector<std::vector<char>> shape;
        while (std::getline(f, line) && !line.empty())
            shape.emplace_back(line.begin(), line.end());

        presents.emplace_back(shape);
    }

    std::vector<Region> regions;

    while (std::getline(f, line) && !line.empty()) {
        std::stringstream line_stream{line};
        std::string token;

        std::getline(line_stream, token, 'x');
        int length{std::stoi(token)};
        std::getline(line_stream, token, ':');
        int width{std::stoi(token)};

        line_stream >> std::ws;

        std::vector<int> present_amounts{};
        present_amounts.reserve(presents.size());

        while (std::getline(line_stream, token, ' ') && !token.empty())
            present_amounts.push_back(std::stoi(token));

        regions.emplace_back(length, width, present_amounts);
    }

    return {presents, regions};

}

auto definitely_satisfiable(const std::vector<Present>& presents, const Region& region) -> bool {
    // we see, wastefully placing every present in a 3x3 spot in the region, if we
    // can place all the presents in the region
    const int minimum_packable_presents{(region.length / 3) * (region.width / 3)};
    return minimum_packable_presents >= std::ranges::fold_left(region.present_amounts, 0, std::plus<int>{});
}

auto definitely_unsatisfiable(const std::vector<Present>& presents, const Region& region) -> bool{
    // we see if the total area of the presents exceeds the region area
    const int area{region.length * region.width};
    int total_present_area{0};

    for (const auto& [amount, present] : std::views::zip(region.present_amounts, presents))
        total_present_area += amount * present.area;

    return total_present_area > area;
}

auto get_viable_regions_count() -> int {
    const auto [presents, region_requirements]{parse_input()};

    int total{0};
    for (const auto& present_region : region_requirements) {
        // it turns out all puzzles are either trivially solvable or trivially unsolvable
        /*if (!definitely_satisfiable(presents, present_region) &&
            !definitely_unsatisfiable(presents, present_region))
            total++;*/ // outputs 0
        if (definitely_satisfiable(presents, present_region))
            total++;
    }

    return total;
}

auto main() -> int {
    std::println("{}", get_viable_regions_count());
}