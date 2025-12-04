#include <stdio.h>
#include <limits.h>
#define AIDS_IMPLEMENTATION
#include "aids.h"

void parse_input(Aids_String_Slice buffer, Aids_Array *lines) {
    aids_array_init(lines, sizeof(Aids_String_Slice));

    Aids_String_Slice token = {0};
    while (aids_string_slice_tokenize(&buffer, '\n', &token)) {
        AIDS_ASSERT(aids_array_append(lines, (const unsigned char *)&token) == AIDS_OK, aids_failure_reason());
    }
}

const int dirs[8][2] = {
    { -1, -1 },
    { -1, 0 },
    { -1, 1 },
    { 0, -1 },
    { 0, 1 },
    { 1, -1 },
    { 1, 0 },
    { 1, 1 }
};
#define DIRS_LEN 8

size_t neighbors(Aids_Array lines, size_t row, size_t col) {
    size_t count = 0;
    for (size_t i = 0; i < DIRS_LEN; i++) {
        int row_i = (int)row + dirs[i][0];
        int col_i = (int)col + dirs[i][1];
        if (row_i < 0 || row_i >= lines.count) continue;

        Aids_String_Slice *line = NULL;
        AIDS_ASSERT(aids_array_get(&lines, row_i, (unsigned char **)&line) == AIDS_OK, aids_failure_reason());
        if (col_i < 0 || col_i >= line->len) continue;

        boolean is_paper = line->str[col_i] == '@';
        if (is_paper) count += 1;
    }

    return count;
}

void part1(Aids_Array lines) {
    long count = 0;

    for (size_t row = 0; row < lines.count; row++) {
        Aids_String_Slice *line = NULL;
        AIDS_ASSERT(aids_array_get(&lines, row, (unsigned char **)&line) == AIDS_OK, aids_failure_reason());

        for (size_t col = 0; col < line->len; col++) {
            if (line->str[col] == '@') {
                boolean can_remove = neighbors(lines, row, col) < 4;
                if (can_remove) count += 1;
            }
        }
    }

    printf("PART1: %ld\n", count);
}

void part2(Aids_Array lines) {
    long count = 0;

    while (true) {
        long prev_count = count;

        for (size_t row = 0; row < lines.count; row++) {
            Aids_String_Slice *line = NULL;
            AIDS_ASSERT(aids_array_get(&lines, row, (unsigned char **)&line) == AIDS_OK, aids_failure_reason());

            for (size_t col = 0; col < line->len; col++) {
                if (line->str[col] == '@') {
                    boolean can_remove = neighbors(lines, row, col) < 4;
                    if (can_remove) {
                        count += 1;
                        line->str[col] = '.';
                    }
                }
            }
        }

        if (prev_count == count) break;
    }

    printf("PART2: %ld\n", count);
}

int main() {
    Aids_String_Slice buffer = {0};
    aids_io_read(NULL, &buffer, "r");
    aids_string_slice_trim(&buffer);

    Aids_Array lines = {0};
    parse_input(buffer, &lines);

    part1(lines);
    part2(lines);

    return 0;
}
