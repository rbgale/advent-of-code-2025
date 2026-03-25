#include <algorithm>
#include <print>
#include <sstream>
#include <vector>
#include <unordered_map>
#include "../common.h"

constexpr int NUM_PAIRS = 1000;

struct Point {
    long long x, y, z;
    bool operator==(const Point&) const = default;
};

auto distance(const Point& a, const Point& b) -> double {
    return std::sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y) + (a.z - b.z) * (a.z - b.z));
}

// complete overkill since time complexity doesn't matter in AoC, but I learned
// about it in my algos course and wanted a chance to use it
template <typename T>
class UnionFind {
public:
    T find(const T& x) {
        ensure(x);
        T current = x;
        while (parent_.at(current) != current) {
            auto& p = parent_.at(current);
            p = parent_.at(p);
            current = p;
        }
        return current;
    }

    bool unite(const T& a, const T& b) {
        auto ra = find(a), rb = find(b);
        if (ra == rb) return false;

        if (rank_[ra] < rank_[rb]) std::swap(ra, rb);
        parent_[rb] = ra;
        if (rank_[ra] == rank_[rb]) ++rank_[ra];
        size_[ra] += size_[rb];

        --components_;
        return true;
    }

    bool connected(const T& a, const T& b) {
        return find(a) == find(b);
    }

    [[nodiscard]] std::size_t components() const { return components_; }
    [[nodiscard]] std::size_t size() const { return parent_.size(); }
    [[nodiscard]] std::size_t set_size(const T& x) { return size_[find(x)]; }
private:
    void ensure(const T& x) {
        if (!parent_.contains(x)) {
            parent_[x] = x;
            rank_[x] = 0;
            size_[x] = 1;
            ++components_;
        }
    }

    std::unordered_map<T, T> parent_;
    std::unordered_map<T, std::size_t> rank_;
    std::size_t components_ = 0;
    std::unordered_map<T, std::size_t> size_;
};

template<>
struct std::hash<Point> {
    auto operator()(const Point& t) const noexcept -> std::size_t {
        std::size_t seed{0};
        hash_combine(seed, t.x);
        hash_combine(seed, t.y);
        hash_combine(seed, t.z);

        return seed;
    }
};

auto parse_input() -> std::vector<Point> {
    auto f{AOC_INPUT()};
    std::vector<Point> points{};
    std::string line{};

    while (std::getline(f, line) && !line.empty()) {
        std::stringstream line_stream{line};
        std::string x{}, y{}, z{};
        std::getline(line_stream, x, ',');
        std::getline(line_stream, y, ',');
        std::getline(line_stream, z, ',');
        points.emplace_back(std::stoi(x), std::stoi(y), std::stoi(z));
    }

    return points;
}

auto compute_final_circuit_xs() -> std::vector<std::size_t> {
    auto points{parse_input()};
    if (points.empty())
        throw std::runtime_error("No points found in the input");

    std::vector<std::pair<Point, Point>> pairs{};

    for (auto i{0uz}; i < points.size() - 1; ++i)
        for (auto j{i + 1}; j < points.size(); ++j)
            pairs.emplace_back(points[i], points[j]);

    std::ranges::sort(pairs, {}, [](const auto& p) {
        return distance(p.first, p.second);
    });

    UnionFind<Point> union_find{};

    for (const auto& p : points)
        union_find.find(p);

    int count = 0;
    for (const auto& [fst, snd] : pairs) {
        if (count >= NUM_PAIRS) break;
        union_find.unite(fst, snd);
        ++count;
    }

    std::unordered_set<Point> seen;
    std::vector<std::size_t> sizes;

    for (auto& point : points)
        if (const auto root = union_find.find(point); seen.insert(root).second)
            sizes.push_back(union_find.set_size(point));

    return sizes;
}

auto top_three_circuit_sizes() -> std::array<size_t, 3> {
    auto circuit_sizes{compute_final_circuit_xs()};
    if (circuit_sizes.size() < 3)
        throw std::runtime_error(std::format("Expected at least 3 circuits; got {}", circuit_sizes.size()));

    std::ranges::partial_sort(circuit_sizes, circuit_sizes.begin() + 3, std::greater{});

    return {circuit_sizes[0], circuit_sizes[1], circuit_sizes[2]};
}

auto top_circuits_product() -> size_t {
    return std::ranges::fold_left(top_three_circuit_sizes(), 1, std::multiplies{});
};

auto main() -> int {
    std::println("{}", top_circuits_product());
}
