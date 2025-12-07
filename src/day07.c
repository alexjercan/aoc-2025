#include <stdio.h>
#define AIDS_IMPLEMENTATION
#include "aids.h"

void part1(Aids_String_Slice buffer) {
    Aids_String_Slice line = {0};
    AIDS_ASSERT(aids_string_slice_tokenize(&buffer, '\n', &line), "Expected at least one line.");

    size_t count = line.len;
    size_t *beams = AIDS_REALLOC(NULL, sizeof(size_t) * count);
    AIDS_ASSERT(beams != NULL, "Buy more RAM!.");

    for (size_t i = 0; i < count; i++) {
        if (line.str[i] == 'S') {
            beams[i] = 1;
        } else {
            beams[i] = 0;
        }
    }

    size_t splits = 0;
    while (aids_string_slice_tokenize(&buffer, '\n', &line)) {
        for (size_t i = 0; i < count; i++) {
            if (line.str[i] == '^' && beams[i] > 0) {
                splits += 1;
                if (i > 0) beams[i-1] += beams[i];
                if (i < count - 1) beams[i+1] += beams[i];
                beams[i] = 0;
            }
        }
    }

    printf("PART1: %zu\n", splits);
}

void part2(Aids_String_Slice buffer) {
    Aids_String_Slice line = {0};
    AIDS_ASSERT(aids_string_slice_tokenize(&buffer, '\n', &line), "Expected at least one line.");

    size_t count = line.len;
    size_t *beams = AIDS_REALLOC(NULL, sizeof(size_t) * count);
    AIDS_ASSERT(beams != NULL, "Buy more RAM!.");

    for (size_t i = 0; i < count; i++) {
        if (line.str[i] == 'S') {
            beams[i] = 1;
        } else {
            beams[i] = 0;
        }
    }

    size_t splits = 1;
    while (aids_string_slice_tokenize(&buffer, '\n', &line)) {
        for (size_t i = 0; i < count; i++) {
            if (line.str[i] == '^' && beams[i] > 0) {
                splits += beams[i];
                if (i > 0) beams[i-1] += beams[i];
                if (i < count - 1) beams[i+1] += beams[i];
                beams[i] = 0;
            }
        }
    }

    printf("PART2: %zu\n", splits);
}

int main() {
    Aids_String_Slice buffer = {0};
    aids_io_read(NULL, &buffer, "r");
    aids_string_slice_trim(&buffer);

    part1(buffer);
    part2(buffer);

    return 0;
}
