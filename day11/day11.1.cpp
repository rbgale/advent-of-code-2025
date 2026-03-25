#include <unordered_map>
#include <sstream>
#include <vector>
#include <print>
#include "../common.h"

using AdjacencyMap = std::unordered_map<std::string, std::vector<std::string>>;

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
    const std::string& end
) -> long long {
    if (current == end) return 1;

    long long subpath_count{0};

    for (const auto& neighbor : adjacency_map.at(current))
        subpath_count += distinct_subpaths_count(adjacency_map, neighbor, end);

    return subpath_count;
}

constexpr std::string START_VERTEX{"you"};
constexpr std::string END_VERTEX{"out"};

auto distinct_paths_count() -> long long {
    return distinct_subpaths_count(parse_input(), START_VERTEX, END_VERTEX);
}


auto main() -> int {
    std::println("{}", distinct_paths_count());
}