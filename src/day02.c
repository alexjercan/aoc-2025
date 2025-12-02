#include <math.h>
#include <stdio.h>
#define AIDS_IMPLEMENTATION
#include "aids.h"

typedef long Range[2];


void parse_input(Aids_String_Slice buffer, Aids_Array *ranges) {
    aids_array_init(ranges, sizeof(Range));

    Aids_String_Slice token = {0};
    while (aids_string_slice_tokenize(&buffer, ',', &token)) {
        Aids_String_Slice start_token = {0};
        AIDS_ASSERT(aids_string_slice_tokenize(&token, '-', &start_token), "Expected a range");

        long start = 0;
        AIDS_ASSERT(aids_string_slice_atol(&start_token, &start, 10), "Expected a number");

        long end = 0;
        AIDS_ASSERT(aids_string_slice_atol(&token, &end, 10), "Expected a number");

        Range range = {start, end};
        AIDS_ASSERT(aids_array_append(ranges, (const unsigned char *)range) == AIDS_OK, aids_failure_reason());
    }

}

size_t count_digits(size_t number) {
    size_t count = 0;
    if (number == 0) {
        return 1;
    }

    while (number) {
        count += 1;
        number /= 10;
    }

    return count;
}

void part1(Aids_Array ranges) {
    long result = 0;

    for (size_t i = 0; i < ranges.count; i++) {
        Range *range = NULL;
        AIDS_ASSERT(aids_array_get(&ranges, i, (unsigned char**)&range) == AIDS_OK, aids_failure_reason());

        long start = (*range)[0];
        long end = (*range)[1];

        for (size_t j = start; j <= end; j++) {
            size_t count = count_digits(j);
            if (count % 2 == 1) continue;

            size_t half = count / 2;
            size_t exp = powl(10, half);

            if (j % exp == j / exp) {
                result += j;
            }
        }
    }

    printf("PART1: %ld\n", result);
}

boolean has_cycle(size_t number) {
    size_t length = count_digits(number);

    for (size_t i = 1; i < length / 2 + 1; i++) {
        if (length % i != 0) continue;

        size_t exp = powl(10, i);

        boolean is_cycle = true;
        size_t value = number % exp;
        size_t rest = number / exp;
        while (rest) {
            size_t new_value = rest % exp;
            if (value != new_value) {
                is_cycle = false;
                break;
            }

            rest /= exp;
        }

        if (is_cycle) return true;
    }

    return false;
}

void part2(Aids_Array ranges) {
    long result = 0;

    for (size_t i = 0; i < ranges.count; i++) {
        Range *range = NULL;
        AIDS_ASSERT(aids_array_get(&ranges, i, (unsigned char**)&range) == AIDS_OK, aids_failure_reason());

        long start = (*range)[0];
        long end = (*range)[1];

        for (size_t j = start; j <= end; j++) {
            if (has_cycle(j)) {
                result += j;
            }
        }
    }

    printf("PART2: %ld\n", result);
}

int main() {
    Aids_String_Slice buffer = {0};
    aids_io_read(NULL, &buffer, "r");
    aids_string_slice_trim(&buffer);

    Aids_Array ranges = {0};
    parse_input(buffer, &ranges);

    part1(ranges);
    part2(ranges);

    return 0;
}
