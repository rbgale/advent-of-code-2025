#include <algorithm>
#include <sstream>
#include <vector>
#include <print>
#include "../common.h"

struct Point {
    long long x, y;
    auto operator<=>(const Point&) const = default;
};
enum class Seek { Low, High };

auto parse_input() -> std::vector<Point> {
    auto f{AOC_INPUT()};

    std::vector<Point> corners{};
    std::string line;

    while (std::getline(f, line) && !line.empty()) {
        std::stringstream line_stream{line};
        std::string x{}, y{};

        std::getline(line_stream, x, ',');
        std::getline(line_stream, y);

        corners.emplace_back(stoll(x), stoll(y));
    }

    return corners;
}

// unnecessary, but avoids full O(n^2) search except in worst case
template<Seek X, Seek Y>
auto pareto_frontier(auto corners) -> std::vector<Point> {
    if constexpr (X == Seek::Low) std::ranges::sort(corners, std::less{});
    else std::ranges::sort(corners, std::greater{});

    std::vector<Point> frontier{};

    long long best_y = Y == Seek::High ? LLONG_MIN : LLONG_MAX;
    constexpr auto is_better = [](const auto x, const auto y) {
        return Y == Seek::High ? x >= y : x <= y;
    };

    for (const auto& [x, y] : corners) {
        if (is_better(y, best_y)) {
            best_y = y;
            frontier.push_back({x, y});
        }
    }

    return frontier;
}

auto max_product(const auto& frontier1, const auto& frontier2) {
    long long current_max{LLONG_MIN};

    for (const auto& [x1, y1] : frontier1)
        for (const auto& [x2, y2] : frontier2)
            current_max = std::max(current_max, (1 + std::abs(x2 - x1)) * (1 + std::abs(y2 - y1)));

    return current_max;
}

auto get_max_area() -> long long {
    const auto corners{parse_input()};

    const auto upper_left{pareto_frontier<Seek::Low, Seek::High>(corners)};
    const auto upper_right{pareto_frontier<Seek::High, Seek::High>(corners)};
    const auto lower_left{pareto_frontier<Seek::Low, Seek::Low>(corners)};
    const auto lower_right{pareto_frontier<Seek::High, Seek::Low>(corners)};

    return std::max(max_product(upper_left, lower_right), max_product(upper_right, lower_left));
}

auto main() -> int {
    std::println("{}", get_max_area());
}
