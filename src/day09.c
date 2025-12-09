#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#define AIDS_IMPLEMENTATION
#include "aids.h"

#define AIDS_MAX(a, b) ((a) > (b) ? (a) : (b))
#define AIDS_MIN(a, b) ((a) < (b) ? (a) : (b))

typedef struct {
    long x, y;
} IVec2;

void parse_input(Aids_String_Slice buffer, Aids_Array *tiles) {
    aids_array_init(tiles, sizeof(IVec2));

    Aids_String_Slice line = {0};
    while (aids_string_slice_tokenize(&buffer, '\n', &line)) {
        aids_string_slice_trim(&line);

        IVec2 tile = {0};
        Aids_String_Slice token = {0};

        AIDS_ASSERT(aids_string_slice_tokenize(&line, ',', &token), "Expected x coordinate.");
        aids_string_slice_trim(&token);
        AIDS_ASSERT(aids_string_slice_atol(&token, &tile.x, 10), "Expected a number.");

        AIDS_ASSERT(aids_string_slice_tokenize(&line, ',', &token), "Expected y coordinate.");
        aids_string_slice_trim(&token);
        AIDS_ASSERT(aids_string_slice_atol(&token, &tile.y, 10), "Expected a number.");

        AIDS_ASSERT(aids_array_append(tiles, &tile) == AIDS_OK, aids_failure_reason());
    }
}

long rectangle_area(IVec2 a, IVec2 b) {
    return (llabs(a.x - b.x) + 1) * (llabs(a.y - b.y) + 1);
}

void part1(Aids_Array tiles) {
    long max_area = 0;
    for (size_t i = 0; i < tiles.count; i++) {
        IVec2 *tile_i = NULL;
        AIDS_ASSERT(aids_array_get(&tiles, i, (void **)&tile_i) == AIDS_OK, aids_failure_reason());

        for (size_t j = 0; j < tiles.count; j++) {
            IVec2 *tile_j = NULL;
            AIDS_ASSERT(aids_array_get(&tiles, j, (void **)&tile_j) == AIDS_OK, aids_failure_reason());

            long area = rectangle_area(*tile_i, *tile_j);
            if (area > max_area) {
                max_area = area;
            }
        }
    }

    printf("PART1: %ld\n", max_area);
}

boolean tile_intersects(IVec2 tile_i, IVec2 tile_j, IVec2 tile_k1, IVec2 tile_k2) {
    long rect_left = AIDS_MIN(tile_i.x, tile_j.x);
    long rect_right = AIDS_MAX(tile_i.x, tile_j.x);
    long rect_bottom = AIDS_MIN(tile_i.y, tile_j.y);
    long rect_top = AIDS_MAX(tile_i.y, tile_j.y);

    if (tile_k1.x == tile_k2.x) {
        long x = tile_k1.x;
        long y1 = AIDS_MIN(tile_k1.y, tile_k2.y);
        long y2 = AIDS_MAX(tile_k1.y, tile_k2.y);
        if (x > rect_left && x < rect_right && y2 > rect_bottom && y1 < rect_top) {
            return true;
        }

        return false;
    } else if (tile_k1.y == tile_k2.y) {
        long y = tile_k1.y;
        long x1 = AIDS_MIN(tile_k1.x, tile_k2.x);
        long x2 = AIDS_MAX(tile_k1.x, tile_k2.x);
        if (y > rect_bottom && y < rect_top && x2 > rect_left && x1 < rect_right) {
            return true;
        }

        return false;
    } else {
        AIDS_UNREACHABLE("Line segment is not horizontal or vertical");
    }
}

void part2(Aids_Array tiles) {
    long max_area = 0;
    for (size_t i = 0; i < tiles.count; i++) {
        IVec2 *tile_i = NULL;
        AIDS_ASSERT(aids_array_get(&tiles, i, (void **)&tile_i) == AIDS_OK, aids_failure_reason());

        for (size_t j = 0; j < tiles.count; j++) {
            IVec2 *tile_j = NULL;
            AIDS_ASSERT(aids_array_get(&tiles, j, (void **)&tile_j) == AIDS_OK, aids_failure_reason());

            boolean is_convex = true;
            for (size_t k = 0; k < tiles.count; k++) {
                IVec2 *tile_k1 = NULL;
                AIDS_ASSERT(aids_array_get(&tiles, k, (void **)&tile_k1) == AIDS_OK, aids_failure_reason());

                IVec2 *tile_k2 = NULL;
                AIDS_ASSERT(aids_array_get(&tiles, (k + 1) % tiles.count, (void **)&tile_k2) == AIDS_OK, aids_failure_reason());

                if (tile_intersects(*tile_i, *tile_j, *tile_k1, *tile_k2)) {
                    is_convex = false;
                    break;
                }
            }

            if (is_convex) {
                long area = rectangle_area(*tile_i, *tile_j);
                if (area > max_area) {
                    max_area = area;
                }
            }
        }
    }

    printf("PART2: %ld\n", max_area);
}

int main() {
    Aids_String_Slice buffer = {0};
    aids_io_read(NULL, &buffer, "r");
    aids_string_slice_trim(&buffer);

    Aids_Array tiles = {0};
    parse_input(buffer, &tiles);

    part1(tiles);
    part2(tiles);

    return 0;
}
