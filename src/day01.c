#include <stdio.h>
#define AIDS_IMPLEMENTATION
#include "aids.h"

void part1(Aids_String_Slice buffer) {
    long value = 50;
    long counter = 0;

    Aids_String_Slice token = {0};
    while (aids_string_slice_tokenize(&buffer, '\n', &token)) {
        char direction = token.str[0];
        long steps = 0;

        aids_string_slice_skip(&token, 1);
        AIDS_ASSERT(aids_string_slice_atol(&token, &steps, 10), "Expected long number");

        long sign = (direction == 'L') ? -1 : 1;
        value = (value + sign * steps);
        if (value < 0) {
            value = value + 100;
        }
        value = value % 100;

        if (value == 0) {
            counter += 1;
        }
    }

    printf("PART1: %ld\n", counter);
}

void part2(Aids_String_Slice buffer) {
    long value = 50;
    long counter = 0;

    Aids_String_Slice token = {0};
    while (aids_string_slice_tokenize(&buffer, '\n', &token)) {
        char direction = token.str[0];
        long steps = 0;

        aids_string_slice_skip(&token, 1);
        AIDS_ASSERT(aids_string_slice_atol(&token, &steps, 10), "Expected long number");

        counter += steps / 100;
        long rest = steps % 100;
        if (rest == 0) continue;

        long sign = (direction == 'L') ? -1 : 1;
        long next = value + sign * rest;
        if (value != 0 && (next <= 0 || next >= 100)) {
            counter += 1;
        }
        value = (next + 100) % 100;
    }

    printf("PART2: %ld\n", counter);
}

int main() {
    Aids_String_Slice buffer = {0};
    aids_io_read(NULL, &buffer, "r");
    aids_string_slice_trim(&buffer);

    part1(buffer);
    part2(buffer);

    return 0;
}
