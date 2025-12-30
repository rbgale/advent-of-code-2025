#include <algorithm>
#include <ostream>
#include <set>
#include <sstream>
#include <vector>
#include "../common.h"

using id = long long;
using Range = std::pair<id, id>;

struct ParsedData {
    std::set<std::pair<id, id>> ranges;
    std::vector<id> ids;
};

std::set<Range> mergeRanges(std::vector<Range> ranges) {
    if (ranges.empty()) return {};

    std::ranges::sort(ranges);

    std::set<Range> merged;
    Range current = ranges[0];

    for (size_t i = 1; i < ranges.size(); i++) {
        if (ranges[i].first <= current.second + 1) {
            current.second = std::max(current.second, ranges[i].second);
        } else {
            merged.insert(merged.end(), current);
            current = ranges[i];
        }
    }

    merged.insert(merged.end(), current);

    return merged;
}

[[nodiscard]] auto get_ranges_and_values() -> ParsedData {
    ParsedData data;

    {
        auto f{AOC_INPUT()};
        std::string line;

        std::vector<Range> unordered_ranges;

        while (std::getline(f, line) && !line.empty()) {
            std::istringstream line_stream{line};
            std::pair<id, id> range;
            char dash{};
            line_stream >> range.first >> dash >> range.second;

            unordered_ranges.emplace_back(range);
        }

        data.ranges = mergeRanges(unordered_ranges);

        while (std::getline(f, line))
            data.ids.push_back(std::stoll(line));
    }

    return data;
}

[[nodiscard]] auto is_in_ranges(const std::set<Range>& ranges, const long long id) -> bool {
    auto iterator{ranges.upper_bound({id, LLONG_MAX})};

    if (iterator == ranges.begin()) return false;

    --iterator;

    return id <= iterator->second;
}

[[nodiscard]] auto count_fresh_ids() -> int {
    int total{};

    for (auto [ranges, ids] = get_ranges_and_values(); const auto id : ids)
        if (is_in_ranges(ranges, id))
            ++total;

    return total;
}

auto main() -> int {
    std::println("{}", count_fresh_ids());
}
