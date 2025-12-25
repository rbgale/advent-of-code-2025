from typing import Generator


def rotate(start: int, shift: int, modulo: int) -> int:
    return (start + shift) % modulo


def num_passes(start: int, shift: int, modulo: int) -> int:
    res: int = abs(shift) // modulo
    end: int = rotate(start, shift, modulo)

    if start != 0 and (end == 0 or
       (start - end > 0 and shift > 0) or
       (start - end < 0 and shift < 0)):
        res += 1      
   
    return res


def get_rotations() -> Generator[int, None, None]:
    with open('input.txt', 'r') as input_file:
        for line in input_file:
            sign: int = 1 if 'R' == line[0] else -1

            yield sign * int(line[1:].strip())


def count_zeros(dial_size: int) -> int:
    total: int = 0
    position: int = 50

    for shift in get_rotations():
        total += num_passes(position, shift, dial_size)
        position = rotate(position, shift, dial_size)

    return total


if __name__ == '__main__':
    print(count_zeros(100))
