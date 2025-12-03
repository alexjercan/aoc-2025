#include <math.h>
#include <stdio.h>
#include <limits.h>
#define AIDS_IMPLEMENTATION
#include "aids.h"

#define AIDS_MAX(a, b) ((a) > (b) ? (a) : (b))

long largest_numbers_dp(Aids_String_Slice line, int n) {
    long dp[line.len + 1][n + 1];

    for (size_t i = 0; i <= line.len; i++) {
        for (size_t j = 0; j <= n; j++) {
            dp[i][j] = INT_MIN;
        }
    }

    for (size_t i = 0; i <= line.len; i++) {
        dp[i][0] = 0;
    }

    for (size_t i = 1; i <= line.len; i++) {
        char ch = line.str[i - 1];
        int digit = ch - '0';

        for (size_t j = 1; j <= n; j++) {
            long take = dp[i - 1][j - 1];
            if (take != INT_MIN) {
                take += powl(10, n - j) * digit;
            }

            long not_take = dp[i - 1][j];

            dp[i][j] = AIDS_MAX(take, not_take);
        }
    }

    return dp[line.len][n];
}

void part1(Aids_String_Slice buffer) {
    long sum = 0;

    Aids_String_Slice line = {0};
    while (aids_string_slice_tokenize(&buffer, '\n', &line)) {
        sum += largest_numbers_dp(line, 2);
    }

    printf("PART1: %ld\n", sum);
}

void part2(Aids_String_Slice buffer) {
    long sum = 0;

    Aids_String_Slice line = {0};
    while (aids_string_slice_tokenize(&buffer, '\n', &line)) {
        sum += largest_numbers_dp(line, 12);
    }

    printf("PART2: %ld\n", sum);
}

int main() {
    Aids_String_Slice buffer = {0};
    aids_io_read(NULL, &buffer, "r");
    aids_string_slice_trim(&buffer);

    part1(buffer);
    part2(buffer);

    return 0;
}
