#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#define AIDS_IMPLEMENTATION
#include "aids.h"

typedef struct {
    long x, y, z;
} IVec3;

long ivec3_distance_squared(IVec3 a, IVec3 b) {
    return (a.x - b.x) * (a.x - b.x) +
           (a.y - b.y) * (a.y - b.y) +
           (a.z - b.z) * (a.z - b.z);
}

void parse_input(Aids_String_Slice buffer, Aids_Array *points) {
    aids_array_init(points, sizeof(IVec3));

    Aids_String_Slice line = {0};
    while (aids_string_slice_tokenize(&buffer, '\n', &line)) {
        aids_string_slice_trim(&line);

        IVec3 point = {0};
        Aids_String_Slice token = {0};

        AIDS_ASSERT(aids_string_slice_tokenize(&line, ',', &token), "Expected x coordinate.");
        aids_string_slice_trim(&token);
        AIDS_ASSERT(aids_string_slice_atol(&token, &point.x, 10), "Expected a number.");

        AIDS_ASSERT(aids_string_slice_tokenize(&line, ',', &token), "Expected y coordinate.");
        aids_string_slice_trim(&token);
        AIDS_ASSERT(aids_string_slice_atol(&token, &point.y, 10), "Expected a number.");

        AIDS_ASSERT(aids_string_slice_tokenize(&line, ',', &token), "Expected z coordinate.");
        aids_string_slice_trim(&token);
        AIDS_ASSERT(aids_string_slice_atol(&token, &point.z, 10), "Expected a number.");

        AIDS_ASSERT(aids_array_append(points, (const unsigned char *)&point) == AIDS_OK, aids_failure_reason());
    }

}

int compare_int(const void *a, const void *b) {
    return (*(int *)b - *(int *)a);
}

void part1(Aids_Array points) {
    int *cluster = AIDS_REALLOC(NULL, points.count * sizeof(int));
    for (size_t i = 0; i < points.count; i++) {
        cluster[i] = i;
    }

    char *visited = AIDS_REALLOC(NULL, points.count * points.count * sizeof(char));
    for (size_t i = 0; i < points.count * points.count; i++) {
        visited[i] = 0;
    }

    for (size_t iter = 0; iter < 1000; iter++) {
        size_t best_i = 0;
        size_t best_j = 0;
        long best_distance = LONG_MAX;
        for (size_t i = 0; i < points.count; i++) {
            IVec3 *point = NULL;
            AIDS_ASSERT(aids_array_get(&points, i, (unsigned char**)&point) == AIDS_OK, aids_failure_reason());

            for (size_t j = i + 1; j < points.count; j++) {
                IVec3 *other = NULL;
                AIDS_ASSERT(aids_array_get(&points, j, (unsigned char**)&other) == AIDS_OK, aids_failure_reason());

                long distance = ivec3_distance_squared(*point, *other);
                if (distance <= best_distance && visited[i * points.count + j] == 0) {
                    best_distance = distance;
                    best_i = i;
                    best_j = j;
                }
            }
        }

        visited[best_i * points.count + best_j] = 1;

        int cluster_i = cluster[best_i];
        int cluster_j = cluster[best_j];
        if (cluster_i == cluster_j) {
            continue;
        }

        for (size_t i = 0; i < points.count; i++) {
            if (cluster[i] == cluster_j) {
                cluster[i] = cluster_i;
            }
        }
    }

    int *unique_clusters = AIDS_REALLOC(NULL, points.count * sizeof(int));
    memset(unique_clusters, 0, points.count * sizeof(int));
    for (size_t i = 0; i < points.count; i++) {
        unique_clusters[cluster[i]] += 1;
    }
    size_t cluster_result = 1;
    qsort(unique_clusters, points.count, sizeof(int), compare_int);
    for (size_t i = 0; i < 3; i++) {
        if (unique_clusters[i] == 0) break;
        cluster_result *= unique_clusters[i];
    }

    printf("PART1: %zu\n", cluster_result);
}

void part2(Aids_Array points) {
    int *cluster = AIDS_REALLOC(NULL, points.count * sizeof(int));
    for (size_t i = 0; i < points.count; i++) {
        cluster[i] = i;
    }

    char *visited = AIDS_REALLOC(NULL, points.count * points.count * sizeof(char));
    for (size_t i = 0; i < points.count * points.count; i++) {
        visited[i] = 0;
    }

    int *unique_clusters = AIDS_REALLOC(NULL, points.count * sizeof(int));

    while (true) {
        size_t best_i = 0;
        size_t best_j = 0;
        long best_distance = LONG_MAX;
        for (size_t i = 0; i < points.count; i++) {
            IVec3 *point = NULL;
            AIDS_ASSERT(aids_array_get(&points, i, (unsigned char**)&point) == AIDS_OK, aids_failure_reason());

            for (size_t j = i + 1; j < points.count; j++) {
                IVec3 *other = NULL;
                AIDS_ASSERT(aids_array_get(&points, j, (unsigned char**)&other) == AIDS_OK, aids_failure_reason());

                long distance = ivec3_distance_squared(*point, *other);
                if (distance <= best_distance && visited[i * points.count + j] == 0) {
                    best_distance = distance;
                    best_i = i;
                    best_j = j;
                }
            }
        }

        visited[best_i * points.count + best_j] = 1;

        int cluster_i = cluster[best_i];
        int cluster_j = cluster[best_j];
        if (cluster_i == cluster_j) {
            continue;
        }

        for (size_t i = 0; i < points.count; i++) {
            if (cluster[i] == cluster_j) {
                cluster[i] = cluster_i;
            }
        }

        memset(unique_clusters, 0, points.count * sizeof(int));
        for (size_t i = 0; i < points.count; i++) {
            unique_clusters[cluster[i]] += 1;
        }

        size_t cluster_count = 0;
        for (size_t i = 0; i < points.count; i++) {
            if (unique_clusters[i] > 0) {
                cluster_count += 1;
            }
        }

        if (cluster_count == 1) {
            IVec3 *point_a = NULL;
            AIDS_ASSERT(aids_array_get(&points, best_i, (unsigned char**)&point_a) == AIDS_OK, aids_failure_reason());
            IVec3 *point_b = NULL;
            AIDS_ASSERT(aids_array_get(&points, best_j, (unsigned char**)&point_b) == AIDS_OK, aids_failure_reason());

            printf("PART2: %zu\n", point_a->x * point_b->x);
            break;
        }
    }

}

int main() {
    Aids_String_Slice buffer = {0};
    aids_io_read(NULL, &buffer, "r");
    aids_string_slice_trim(&buffer);

    Aids_Array points = {0};
    parse_input(buffer, &points);

    part1(points);
    part2(points);

    return 0;
}
