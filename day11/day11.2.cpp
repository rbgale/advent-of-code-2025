#include <unordered_map>
#include <sstream>
#include <vector>
#include <print>
#include "../common.h"

using AdjacencyMap = std::unordered_map<std::string, std::vector<std::string>>;

struct RequiredVisits {
    bool dac = false;
    bool fft = false;

    [[nodiscard]] auto satisfied() const -> bool {
        return dac && fft;
    }

    auto operator==(const RequiredVisits&) const -> bool = default;
};

using State = std::tuple<std::string, RequiredVisits>;


template <>
struct std::hash<State> {
    auto operator()(const State& key) const noexcept -> std::size_t{
        const auto& [vertex, visits] = key;

        std::size_t seed{0};
        hash_combine(seed, vertex);
        hash_combine(seed, visits.dac);
        hash_combine(seed, visits.fft);
        return seed;
    }
};

auto parse_input() -> AdjacencyMap {
    auto f{AOC_INPUT()};

    std::string line{};
    AdjacencyMap adjacency_map{};

    while (std::getline(f, line) && !line.empty()) {
        std::stringstream line_stream{line};
        std::string vertex{};
        std::getline(line_stream, vertex, ':');
        line_stream >> std::ws;

        std::vector<std::string> neighbors{};
        for (std::string neighbor; std::getline(line_stream, neighbor, ' ');)
            neighbors.push_back(neighbor);

        adjacency_map[vertex] = neighbors;
    }

    return adjacency_map;
}

auto distinct_subpaths_count(
    const AdjacencyMap& adjacency_map,
    const std::string& current,
    const std::string& end,
    std::unordered_map<State, long long>& cache,
    RequiredVisits visits
) -> long long {
    if (current == "dac") visits.dac = true;
    if (current == "fft") visits.fft = true;

    const State current_state{current, visits};
    if (const auto iterator{cache.find(current_state)}; iterator != cache.end())
        return iterator->second;

    if (current == end) {
        const long long subpaths_count{visits.satisfied() ? 1 : 0};
        cache[current_state] = subpaths_count;

        return subpaths_count;
    }

    long long subpath_count{0};

    for (const auto& neighbor : adjacency_map.at(current))
        subpath_count += distinct_subpaths_count(adjacency_map, neighbor, end, cache, visits);

    cache[current_state] = subpath_count;
    return subpath_count;
}

constexpr std::string START_VERTEX{"svr"};
constexpr std::string END_VERTEX{"out"};

auto distinct_paths_count() -> long long {
    std::unordered_map<State, long long> visited_states{};

    return distinct_subpaths_count(
        parse_input(),
        START_VERTEX,
        END_VERTEX,
        visited_states,
        RequiredVisits{}
    );
}


auto main() -> int {
    std::println("{}", distinct_paths_count());
}