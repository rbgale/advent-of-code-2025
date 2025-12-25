from typing import Generator

def get_ids() -> Generator[tuple[str, str], None, None]:
    with open('input.txt', 'r') as input_file:
        for line in input_file:
            id_ranges: str = line.split(',')
            
            for id_range in id_ranges:
                start, end = id_range.split('-')

                yield (start, end)

def sum_truncated_range(start: str, end: str) -> int:
    if len(start) % 2:
        return 0

    split: int = len(start) // 2
    lower_bound: int = max(int(start[:split]), int(start[split:]))
    upper_bound: int = min(int(end[:split]), int(end[split:]))

    return sum(range(lower_bound, upper_bound + 1))

def min_n_digit(digits: int) -> str:
    return str(10 ** (digits - 1))

def max_n_digit(digits: int) -> str:
    return str((10 ** digits) - 1)

def sum_invalid_range(start: str, end: str) -> int:
    start_digits: int = len(start)
    end_digits: int = len(end)

    if start_digits == end_digits:
        return sum_truncated_range(start, end)

    total: int = 0
    for digits in range(start_digits + 2 - (start_digits % 2), end_digits, 2):
        total += sum_truncated_range(min_n_digit(digits), max_n_digit(digits))

    total += sum_truncated_range(start, max_n_digit(start_digits))
    total += sum_truncated_range(min_n_digit(end_digits), end)

    return total

def sum_invalid() -> int:
    total: int = 0

    for start, end in get_ids():
        total += sum_invalid_range(start, end)

    return total

if __name__ == '__main__':
    print(sum_invalid())

