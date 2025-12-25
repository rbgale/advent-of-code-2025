from typing import Generator

def get_ids() -> Generator[tuple[int, int], None, None]:
    with open('input.txt', 'r') as input_file:
        for line in input_file:
            for id_range in line.split(','):
                start, end = id_range.split('-')

                yield int(start), int(end)


def all_parts_equal(id_string: str, num_parts: int) -> bool:
    n: int = len(id_string)

    d, r = divmod(n, num_parts)
    if 0 != r:
        return False

    return all(id_string[i:i + d] == id_string[:d] for i in range(d, n, d))


def sum_invalid_range(start: int, end: int):
    total: int = 0

    for product_id in range(start, end + 1):
        id_string: str = str(product_id)
        n: int = len(id_string)

        if any(all_parts_equal(id_string, parts) for parts in range(2, n + 1)):
            total += product_id

    return total


def sum_invalid() -> int:
    total: int = 0

    for start, end in get_ids():
        total += sum_invalid_range(start, end)

    return total


if __name__ == '__main__':
    print(sum_invalid())
