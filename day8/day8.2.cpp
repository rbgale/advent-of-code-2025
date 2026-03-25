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

// apparently not overkill for part 2 of the problem :)
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
    auto operator()(const Point& t) const noexcept -> std::size_t{
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

auto compute_final_circuit_xs() -> long long {
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

    for (const auto& [fst, snd] : pairs) {
        union_find.unite(fst, snd);

        if (union_find.components() == 1)
            return fst.x * snd.x;
    }

    throw std::runtime_error{std::format("Expected 1 circuit; got {}", union_find.components())};
}

auto main() -> int {
    std::println("{}", compute_final_circuit_xs());
}
