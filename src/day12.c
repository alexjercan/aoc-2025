#include <stdio.h>
#define AIDS_IMPLEMENTATION
#include "aids.h"

#define NUM_SHAPES 6
#define SHAPE_SIZE 3

typedef char Shape[SHAPE_SIZE][SHAPE_SIZE + 1];

typedef struct {
    size_t width, height;
    long counts[NUM_SHAPES];
} Region;

void parse_input(Aids_String_Slice buffer, Shape *shapes, Aids_Array *regions) {
    aids_array_init(regions, sizeof(Region));

    Aids_String_Slice line = {0};
    for (size_t i = 0; i < NUM_SHAPES; i++) {
        AIDS_ASSERT(aids_string_slice_tokenize(&buffer, '\n', &line), "Expected shape index");

        AIDS_ASSERT(aids_string_slice_tokenize(&buffer, '\n', &line), "Expected shape line 1");
        aids_string_slice_trim(&line);
        memcpy(shapes[i][0], line.str, line.len);

        AIDS_ASSERT(aids_string_slice_tokenize(&buffer, '\n', &line), "Expected shape line 2");
        aids_string_slice_trim(&line);
        memcpy(shapes[i][1], line.str, line.len);

        AIDS_ASSERT(aids_string_slice_tokenize(&buffer, '\n', &line), "Expected shape line 3");
        aids_string_slice_trim(&line);
        memcpy(shapes[i][2], line.str, line.len);

        aids_string_slice_trim(&buffer);
    }

    while (aids_string_slice_tokenize(&buffer, '\n', &line)) {
        Region region = {0};

        Aids_String_Slice token = {0};
        aids_string_slice_trim(&line);

        AIDS_ASSERT(aids_string_slice_tokenize(&line, 'x', &token), "Expected a region width");
        AIDS_ASSERT(aids_string_slice_atol(&token, (long *)&region.width, 10), "Expected a number");

        AIDS_ASSERT(aids_string_slice_tokenize(&line, ':', &token), "Expected a region height");
        AIDS_ASSERT(aids_string_slice_atol(&token, (long *)&region.height, 10), "Expected a number");

        size_t index = 0;
        aids_string_slice_trim(&line);
        while (aids_string_slice_tokenize(&line, ' ', &token)) {
            long count = 0;
            AIDS_ASSERT(aids_string_slice_atol(&token, (long *)&region.counts[index], 10), "Expected a number");
            index += 1;
        }

        AIDS_ASSERT(aids_array_append(regions, &region) == AIDS_OK, aids_failure_reason());
    }
}

void part1(Shape shapes[NUM_SHAPES], Aids_Array regions) {
    long shape_sizes[NUM_SHAPES] = {0};
    for (size_t i = 0; i < NUM_SHAPES; i++) {
      long size = 0;
      for (size_t j = 0; j < SHAPE_SIZE; j++) {
          for (size_t k = 0; k < SHAPE_SIZE; k++) {
              if (shapes[i][j][k] == '#') {
                  size += 1;
              }
          }
      }
      shape_sizes[i] = size;
    }

    size_t valid_count = 0;
    for (size_t i = 0; i < regions.count; i++) {
        Region *region = NULL;
        AIDS_ASSERT(aids_array_get(&regions, i, (void **)&region) == AIDS_OK, aids_failure_reason());

        long total_shape_size = 0;
        for (size_t j = 0; j < NUM_SHAPES; j++) {
            total_shape_size += region->counts[j] * shape_sizes[j];
        }

        long extra_space_est = 200;
        if ((long)(region->width * region->height) - extra_space_est >= total_shape_size) {
            valid_count += 1;
        }
    }

    printf("PART1: %zu\n", valid_count);
}

int main() {
    Aids_String_Slice buffer = {0};
    aids_io_read(NULL, &buffer, "r");
    aids_string_slice_trim(&buffer);

    Shape shapes[NUM_SHAPES] = {0};
    Aids_Array regions = {0};
    parse_input(buffer, shapes, &regions);

    part1(shapes, regions);

    return 0;
}
