#include <algorithm>
#include <sstream>
#include <utility>
#include <vector>
#include <unordered_map>
#include <ranges>
#include <set>
#include <queue>
#include <print>
#include "../common.h"

struct Point {
    size_t x, y; // negative coordinates are not allowed
    auto operator<=>(const Point&) const = default;
};

template<>
struct std::hash<Point> {
    auto operator()(const Point& t) const noexcept -> std::size_t {
        std::size_t seed = 0;
        hash_combine(seed, t.x);
        hash_combine(seed, t.y);

        return seed;
    }
};

template <typename T, typename Field>
auto compress_coordinate(const std::vector<T>& vec, Field T::*member) -> std::unordered_map<Field, Field> {
    std::set<Field> seen;
    for (const auto& item : vec)
        seen.insert(item.*member);

    std::unordered_map<Field, Field> old_to_new{};
    for (const auto& [i, coordinate] : std::views::zip(std::views::iota(0), seen))
        old_to_new[coordinate] = i;

    return old_to_new;
}

auto compress_points(const std::vector<Point>& uncompressed_points)
    -> std::pair<std::vector<Point>, std::unordered_map<Point, Point>> {

    auto old_to_new_x{compress_coordinate(uncompressed_points, &Point::x)};
    auto old_to_new_y{compress_coordinate(uncompressed_points, &Point::y)};

    std::vector<Point> compressed_points{};
    compressed_points.reserve(uncompressed_points.size());
    std::unordered_map<Point, Point> compressed_to_uncompressed{};

    for (const auto& uncompressed : uncompressed_points) {
        Point compressed{old_to_new_x.at(uncompressed.x), old_to_new_y.at(uncompressed.y)};
        compressed_points.push_back(compressed);
        compressed_to_uncompressed.emplace(compressed, uncompressed);
    }

    return {compressed_points, compressed_to_uncompressed};
}

struct Coordinates {
    std::vector<Point> uncompressed_corners{};
    std::unordered_map<Point, Point> compressed_to_uncompressed{};
    std::vector<Point> compressed_corners{};

    explicit Coordinates(std::vector<Point> corners) : uncompressed_corners{std::move(corners)} {
        std::tie(compressed_corners, compressed_to_uncompressed) = compress_points(uncompressed_corners);
    }

    [[nodiscard]] auto uncompressed_area(const Point& corner1, const Point& corner2) const -> uint64_t {
        const auto [x1, y1]{compressed_to_uncompressed.at(corner1)};
        const auto [x2, y2]{compressed_to_uncompressed.at(corner2)};

        auto abs_diff = [](const size_t a, const size_t b) -> uint64_t {
            return a > b ? a - b : b - a;
        };

        return (abs_diff(x1, x2) + 1) * (abs_diff(y1, y2) + 1);
    }
};

auto parse_input() -> Coordinates {
    auto f{AOC_INPUT()};

    std::vector<Point> corners{};
    std::string line;

    while (std::getline(f, line) && !line.empty()) {
        std::stringstream line_stream{line};
        std::string x{}, y{};

        std::getline(line_stream, x, ',');
        std::getline(line_stream, y);

        corners.emplace_back(stoull(x), stoull(y));
    }

    return Coordinates{corners};
}

auto coordinate_range(const std::vector<Point>& corners, size_t Point::* field) -> size_t {
    return std::ranges::max(
        corners
        | std::views::transform([&](const auto& entry) { return entry.*field; })
        | std::ranges::to<std::vector<size_t>>()
    );
}

class PolygonGrid {
public:
    const size_t rows{};
    const size_t columns{};
    const std::vector<std::vector<int>> cells{};
    const std::vector<std::vector<size_t>> prefix_sums{};

    explicit PolygonGrid(const std::vector<Point>& corners) :
        rows{1 + coordinate_range(corners, &Point::x)}, // add 1 since values are 0-indexed
        columns{1 + coordinate_range(corners, &Point::y)},
        cells{build_grid(corners)},
        prefix_sums{build_prefix_sums()} { }

    [[nodiscard]] auto is_enclosed_rectangle(const Point& corner1, const Point& corner2) const -> bool;
private:
    static auto draw_edge(std::vector<std::vector<int>>& grid, const Point& start, const Point& end) -> void;
    static auto draw_edges(std::vector<std::vector<int>>& grid, const std::vector<Point>& corners) -> void;
    [[nodiscard]] auto get_border_indices() const -> std::vector<Point>;
    [[nodiscard]] auto in_bounds(int row, int column) const -> bool;
    [[nodiscard]] auto exterior_bitmap(const std::vector<std::vector<int>>& grid) const -> std::vector<std::vector<bool>>;
    auto fill_interior(std::vector<std::vector<int>>& grid) const -> void;
    [[nodiscard]] auto build_grid(const std::vector<Point>& corners) const -> std::vector<std::vector<int>>;
    [[nodiscard]] auto build_prefix_sums() const -> std::vector<std::vector<size_t>>;
    [[nodiscard]] auto count_tiles(const Point& top_left, const Point& bottom_right) const -> size_t;
};

auto PolygonGrid::draw_edge(std::vector<std::vector<int>>& grid, const Point& start, const Point& end) -> void {
    if (start.x != end.x && start.y != end.y)
        throw std::invalid_argument("Line must be vertical or horizontal");

    const auto dx = (end.x > start.x) - (end.x < start.x);
    const auto dy = (end.y > start.y) - (end.y < start.y);

    Point p{start};
    while (p != end) { // does not include end point
        grid[p.x][p.y] = 1;
        p.x += dx;
        p.y += dy;
    }
}

auto PolygonGrid::draw_edges(std::vector<std::vector<int>> &grid, const std::vector<Point> &corners) -> void {
    for (auto i{0uz}; i < corners.size(); ++i)
        draw_edge(grid, corners[i], corners[(i + 1) % corners.size()]);
}

auto PolygonGrid::get_border_indices() const -> std::vector<Point> {
    std::vector<Point> border{};
    border.reserve(2 * rows + 2 * columns - 4);

    for (auto i{0uz}; i < rows; ++i)
        for (auto j : {0uz, columns - 1})
            border.emplace_back(i, j);
    for (auto i : {0uz, rows - 1})
        for (auto j{0uz}; j < columns; ++j)
            border.emplace_back(i, j);

    return border;
}

auto PolygonGrid::in_bounds(const int row, const int column) const -> bool {
    return 0 <= row && row < rows && 0 <= column && column < columns;
}

auto PolygonGrid::exterior_bitmap(const std::vector<std::vector<int>>& grid) const -> std::vector<std::vector<bool>> {
    std::vector exterior(rows, std::vector(columns, false));
    std::queue<Point> queue{};

    for (const auto& [i, j] : get_border_indices()) {
        if (grid[i][j] == 0) {
            exterior[i][j] = true;
            queue.emplace(i, j);
        }
    }

    std::vector<std::pair<int, int>> directions = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
    while (!queue.empty()) {
        auto [x, y] = queue.front();
        queue.pop();

        auto new_exterior_point = [&](const std::pair<int, int>& direction) -> bool {
            const auto new_x = static_cast<int>(x) + direction.first;
            const auto new_y = static_cast<int>(y) + direction.second;

            return in_bounds(new_x, new_y) && !exterior[new_x][new_y] && grid[new_x][new_y] == 0;
        };

        for (const auto& [dx, dy] : directions | std::views::filter(new_exterior_point)) {
            queue.emplace(x + dx, y + dy);
            exterior[x + dx][y + dy] = true;
        }
    }

    return exterior;
}

auto PolygonGrid::fill_interior(std::vector<std::vector<int>>& grid) const -> void {
    auto exterior{exterior_bitmap(grid)};
    for (auto i{0uz}; i < rows; ++i)
        for (auto j{0uz}; j < columns; ++j)
            if (!exterior[i][j]) grid[i][j] = 1;
}

auto PolygonGrid::build_grid(const std::vector<Point>& corners) const -> std::vector<std::vector<int>> {
    std::vector grid(rows, std::vector(columns, 0));
    draw_edges(grid, corners);
    fill_interior(grid);

    return grid;
}

auto PolygonGrid::build_prefix_sums() const -> std::vector<std::vector<size_t>> {
    std::vector prefix_sum(rows, std::vector<size_t>(columns, 0));

    for (auto i{0uz}; i < rows; ++i)
        for (auto j{0uz}; j < columns; ++j)
            prefix_sum[i][j] = cells[i][j]
                + (i > 0 ? prefix_sum[i - 1][j] : 0)
                + (j > 0 ? prefix_sum[i][j - 1] : 0)
                - (i > 0 && j > 0 ? prefix_sum[i - 1][j - 1] : 0);

    return prefix_sum;
}

auto PolygonGrid::count_tiles(const Point& top_left, const Point& bottom_right) const -> size_t {
    return prefix_sums[bottom_right.x][bottom_right.y]
        - (top_left.x > 0 ? prefix_sums[top_left.x - 1][bottom_right.y] : 0)
        - (top_left.y > 0 ? prefix_sums[bottom_right.x][top_left.y - 1] : 0)
        + (top_left.x > 0 && top_left.y > 0 ? prefix_sums[top_left.x - 1][top_left.y - 1] : 0);
}

auto PolygonGrid::is_enclosed_rectangle(const Point& corner1, const Point& corner2) const -> bool {
    const Point top_left{std::min(corner1.x, corner2.x), std::min(corner1.y, corner2.y)};
    const Point bottom_right{std::max(corner1.x, corner2.x), std::max(corner1.y, corner2.y)};
    const auto area{(1 + bottom_right.x - top_left.x) * (1 + bottom_right.y - top_left.y)};

    return area == count_tiles(top_left, bottom_right);
}

[[nodiscard]] auto get_max_area() -> uint64_t {
    const auto coordinates{parse_input()};
    const auto compressed_grid{PolygonGrid(coordinates.compressed_corners)};

    uint64_t max_area{0uz};
    for (auto i{0uz}; i < coordinates.compressed_corners.size(); ++i) {
        for (auto j{i}; j < coordinates.compressed_corners.size(); ++j) {
            const auto corner1{coordinates.compressed_corners[i]};
            const auto corner2{coordinates.compressed_corners[j]};

            if (compressed_grid.is_enclosed_rectangle(corner1, corner2))
                max_area = std::max(max_area, coordinates.uncompressed_area(corner1, corner2));
        }
    }

    return max_area;
}

auto main() -> int {
    std::println("{}", get_max_area());
}
