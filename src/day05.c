#include <stdio.h>
#include <limits.h>
#define AIDS_IMPLEMENTATION
#include "aids.h"

typedef long Range[2];

void parse_input(Aids_String_Slice buffer, Aids_Array *ranges, Aids_Array *items) {
    aids_array_init(ranges, sizeof(Range));
    aids_array_init(items, sizeof(long));

    Aids_String_Slice line = {0};
    while (aids_string_slice_tokenize(&buffer, '\n', &line)) {
        aids_string_slice_trim(&line);
        if (line.len == 0) {
            break;
        }

        Aids_String_Slice start_token = {0};
        AIDS_ASSERT(aids_string_slice_tokenize(&line, '-', &start_token), "Expected a range");

        long start = 0;
        AIDS_ASSERT(aids_string_slice_atol(&start_token, &start, 10), "Expected a number");

        long end = 0;
        AIDS_ASSERT(aids_string_slice_atol(&line, &end, 10), "Expected a number");

        Range range = {start, end};
        AIDS_ASSERT(aids_array_append(ranges, range) == AIDS_OK, aids_failure_reason());
    }

    while (aids_string_slice_tokenize(&buffer, '\n', &line)) {
        long item = 0;
        AIDS_ASSERT(aids_string_slice_atol(&line, &item, 10), "Expected a number");

        AIDS_ASSERT(aids_array_append(items, &item) == AIDS_OK, aids_failure_reason());
    }
}

boolean is_in_range(Range range, long item) {
    return range[0] <= item && item <= range[1];
}

boolean is_in_any_range(Aids_Array ranges, long item) {
    for (size_t i = 0; i < ranges.count; i++) {
        Range *range = NULL;
        AIDS_ASSERT(aids_array_get(&ranges, i, (void **)&range) == AIDS_OK, aids_failure_reason());

        if (is_in_range(*range, item)) {
            return true;
        }
    }

    return false;
}

void part1(Aids_Array ranges, Aids_Array items) {
    size_t count = 0;

    for (size_t i = 0; i < items.count; i++) {
        long *item = NULL;
        AIDS_ASSERT(aids_array_get(&items, i, (void **)&item) == AIDS_OK, aids_failure_reason());

        if (is_in_any_range(ranges, *item)) {
            count += 1;
        }
    }


    printf("PART1: %zu\n", count);
}

int sort_ranges(const void *a, const void *b) {
    long A = (*(Range *)a)[0];
    long B = (*(Range *)b)[0];
    if (A < B) return -1;
    if (A > B) return 1;
    return 0;
}

void part2(Aids_Array ranges) {
    size_t count = 0;
    aids_array_sort(&ranges, sort_ranges);

    long start = -1;
    for (size_t i = 0; i < ranges.count; i++) {
        Range *range = NULL;
        AIDS_ASSERT(aids_array_get(&ranges, i, (void **)&range) == AIDS_OK, aids_failure_reason());

        if (start < (*range)[0]) start = (*range)[0];
        if (start <= (*range)[1]) {
            count += (*range)[1] - start + 1;
            start = (*range)[1] + 1;
        }
    }

    printf("PART2: %zu\n", count);
}

int main() {
    Aids_String_Slice buffer = {0};
    aids_io_read(NULL, &buffer, "r");
    aids_string_slice_trim(&buffer);

    Aids_Array ranges = {0};
    Aids_Array items = {0};
    parse_input(buffer, &ranges, &items);

    part1(ranges, items);
    part2(ranges);

    return 0;
}
