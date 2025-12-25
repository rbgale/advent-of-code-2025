from typing import Generator

STARTING_POSITION = 50
DEFAULT_DIAL_SIZE = 100


def rotate(start: int, shift: int, modulo: int) -> int:
    return (start + shift) % modulo


def get_rotations() -> Generator[int, None, None]:
    with open('input.txt', 'r') as f:
        for line in f:
            sign: int = 1 if 'R' == line[0] else -1

            yield sign * int(line[1:].strip())


def count_zeros(dial_size: int) -> int:
    total: int = 0
    position: int = STARTING_POSITION

    for shift in get_rotations():
        position = rotate(position, shift, dial_size)

        if 0 == position:
            total += 1

    return total


if __name__ == '__main__':
    print(count_zeros(DEFAULT_DIAL_SIZE))
