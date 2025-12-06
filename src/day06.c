#include <ctype.h>
#include <stdio.h>
#include <limits.h>
#define AIDS_IMPLEMENTATION
#include "aids.h"

#define MAX_PROBLEMS 5000
#define MAX_LINES 1000

typedef struct {
    Aids_Array numbers;
    char op;
} Problem;

typedef struct {
    size_t start;
    size_t end;
} Column_Range;

size_t find_column_ranges(Aids_String_Slice buffer, Column_Range *ranges) {
    size_t max_len = 0;
    Aids_String_Slice tmp = buffer;
    Aids_String_Slice line = {0};
    while (aids_string_slice_tokenize(&tmp, '\n', &line)) {
        if (line.len > max_len) max_len = line.len;
    }

    boolean *is_gap = AIDS_REALLOC(NULL, max_len * sizeof(boolean));
    for (size_t i = 0; i < max_len; i++) {
        is_gap[i] = true;
    }

    tmp = buffer;
    while (aids_string_slice_tokenize(&tmp, '\n', &line)) {
        for (size_t i = 0; i < line.len; i++) {
            if (line.str[i] != ' ') {
                is_gap[i] = false;
            }
        }
    }

    size_t range_count = 0;
    boolean in_range = false;
    for (size_t i = 0; i < max_len; i++) {
        if (!is_gap[i] && !in_range) {
            ranges[range_count].start = i;
            in_range = true;
        } else if (is_gap[i] && in_range) {
            ranges[range_count].end = i;
            range_count++;
            in_range = false;
        }
    }
    if (in_range) {
        ranges[range_count].end = max_len;
        range_count++;
    }

    AIDS_FREE(is_gap);
    return range_count;
}

size_t parse_input(Aids_String_Slice buffer, Problem *problems) {
    Column_Range ranges[MAX_PROBLEMS] = {0};
    size_t num_ranges = find_column_ranges(buffer, ranges);

    for (size_t col_idx = 0; col_idx < num_ranges; col_idx++) {
        Column_Range range = ranges[col_idx];

        Aids_String_Slice tmp = buffer;
        Aids_String_Slice line = {0};
        while (aids_string_slice_tokenize(&tmp, '\n', &line)) {
            if (range.start >= line.len) continue;

            size_t end = range.end < line.len ? range.end : line.len;
            Aids_String_Slice column_text = {
                .str = line.str + range.start,
                .len = end - range.start
            };

            while (column_text.len > 0 && *column_text.str == ' ') {
                column_text.str++;
                column_text.len--;
            }
            while (column_text.len > 0 && column_text.str[column_text.len - 1] == ' ') {
                column_text.len--;
            }

            if (column_text.len == 0) continue;

            if (*column_text.str == '*' || *column_text.str == '+') {
                problems[col_idx].op = *column_text.str;
            } else if (isdigit(*column_text.str)) {
                long number = 0;
                aids_string_slice_atol(&column_text, &number, 10);
                AIDS_ASSERT(aids_array_append(&problems[col_idx].numbers, (const unsigned char *)&number) == AIDS_OK, aids_failure_reason());
            }
        }
    }

    return num_ranges;
}

long part(Problem *problems, size_t count) {
    long sum = 0;
    for (size_t i = 0; i < count; i++) {
        Problem problem = problems[i];

        if (problem.numbers.count == 0) continue;

        long *number = NULL;
        AIDS_ASSERT(aids_array_get(&problem.numbers, 0, (unsigned char**)&number) == AIDS_OK, aids_failure_reason());
        long result = *number;

        for (size_t j = 1; j < problem.numbers.count; j++) {
            long *number = NULL;
            AIDS_ASSERT(aids_array_get(&problem.numbers, j, (unsigned char**)&number) == AIDS_OK, aids_failure_reason());

            if (problem.op == '*') {
                result *= *number;
            } else {
                result += *number;
            }
        }

        sum += result;
    }

    return sum;
}

void part1(Aids_String_Slice buffer) {
    Problem problems[MAX_PROBLEMS] = {0};
    for (size_t i = 0; i < MAX_PROBLEMS; i++) {
        aids_array_init(&problems[i].numbers, sizeof(long));
    }

    size_t count = parse_input(buffer, problems);

    long sum = part(problems, count);
    printf("PART1: %ld\n", sum);

    for (size_t i = 0; i < MAX_PROBLEMS; i++) {
        aids_array_free(&problems[i].numbers);
    }
}

void transpose_lines(Aids_String_Slice buffer, Aids_String_Slice *output) {
    Aids_String_Builder builders[MAX_LINES] = {0};
    for (size_t i = 0; i < MAX_LINES; i++) {
        aids_string_builder_init(builders + i);
    }
    Aids_String_Builder ops_builder = {0};
    aids_string_builder_init(&ops_builder);

    Aids_String_Slice line = {0};
    Aids_String_Slice tmp = buffer;
    while (aids_string_slice_tokenize(&tmp, '\n', &line)) {
        if (*line.str == '*' || *line.str == '+' ) {
            break;
        }
    }
    char ops[MAX_PROBLEMS] = {0};
    size_t ops_count = 0;
    while (line.len > 0) {
        if (*line.str != ' ') ops[ops_count++] = *line.str;
        aids_string_slice_skip(&line, 1);
    }

    size_t index = 0;
    size_t builder_index = 0;
    size_t max_used = 0;
    size_t index_op = 0;
    boolean run = true;
    while (run) {
        Aids_String_Slice tmp = buffer;
        Aids_String_Slice line = {0};
        boolean all_space = true;
        size_t num_lines = 0;
        while (aids_string_slice_tokenize(&tmp, '\n', &line)) {
            num_lines += 1;
            char ch = line.str[index];
            if (ch != ' ') all_space = false;

            if (*line.str == '*' || *line.str == '+' ) {
                ch = ' ';
            }

            if (ch == '\n') { run = false; continue; }
            AIDS_ASSERT(aids_string_builder_appendc(builders + builder_index, ch) == AIDS_OK, aids_failure_reason());
            if (builder_index > max_used) max_used = builder_index;
        }

        if (all_space) {
            for (size_t j = 0; j < num_lines; j++) {
                for (size_t i = builder_index + 1; i < MAX_LINES; i++) {
                    AIDS_ASSERT(aids_string_builder_appendc(builders + i, ' ') == AIDS_OK, aids_failure_reason());
                }
                if (j == 0) {
                    AIDS_ASSERT(aids_string_builder_appendc(&ops_builder, ops[index_op]) == AIDS_OK, aids_failure_reason());
                } else {
                    AIDS_ASSERT(aids_string_builder_appendc(&ops_builder, ' ') == AIDS_OK, aids_failure_reason());
                }
            }

            index_op += 1;
            builder_index = 0;
        } else {
            builder_index += 1;
        }

        index += 1;
    }
    AIDS_ASSERT(aids_string_builder_appendc(&ops_builder, ops[index_op]) == AIDS_OK, aids_failure_reason());

    Aids_String_Builder builder = {0};
    aids_string_builder_init(&builder);
    for (size_t i = 0; i < max_used; i++) {
        Aids_String_Slice slice = {0};
        aids_string_builder_to_slice(builders + i, &slice);
        aids_string_builder_append_slice(&builder, slice);
        AIDS_ASSERT(aids_string_builder_appendc(&builder, '\n') == AIDS_OK, aids_failure_reason());
    }

    Aids_String_Slice slice = {0};
    aids_string_builder_to_slice(&ops_builder, &slice);
    aids_string_builder_append_slice(&builder, slice);
    AIDS_ASSERT(aids_string_builder_appendc(&builder, '\n') == AIDS_OK, aids_failure_reason());

    aids_string_builder_to_slice(&builder, output);

    for (size_t i = 0; i < MAX_LINES; i++) {
        aids_string_builder_free(builders + i);
    }
    aids_string_builder_free(&ops_builder);
}

void part2(Aids_String_Slice buffer) {
    Aids_String_Slice transposed = {0};
    transpose_lines(buffer, &transposed);

    Problem problems[MAX_PROBLEMS] = {0};
    for (size_t i = 0; i < MAX_PROBLEMS; i++) {
        aids_array_init(&problems[i].numbers, sizeof(long));
    }

    size_t count = parse_input(transposed, problems);

    long sum = part(problems, count);
    printf("PART2: %ld\n", sum);

    for (size_t i = 0; i < MAX_PROBLEMS; i++) {
        aids_array_free(&problems[i].numbers);
    }
    AIDS_FREE(transposed.str);
}

int main() {
    Aids_String_Slice buffer = {0};
    aids_io_read(NULL, &buffer, "r");
    aids_string_slice_trim(&buffer);

    part1(buffer);
    part2(buffer);

    AIDS_FREE(buffer.str);

    return 0;
}
