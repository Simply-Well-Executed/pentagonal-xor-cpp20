// SPDX-License-Identifier: 0BSD
// Copyright (c) Paul Allen Cheek
// C11 test for the include-free Pentagonal XOR header.

#include "../pentagonal_xor_std_free.h"

static int near(double a, double b, double tolerance)
{
    double delta = a - b;
    if (delta < 0.0) delta = -delta;
    return delta <= tolerance;
}

int main(void)
{
    Features5 f = {0.5, 0.5, 0.5, 0.5, 0.5};
    PentagonalScores s = swe_pxor_scores5(f);
    double a[5] = {0.5, 0.5, 0.5, 0.5, 0.5};

    if (!near(s.and_score, 0.03125, 1e-15)) return 1;
    if (!near(s.or_score, 0.96875, 1e-15)) return 2;
    if (!near(s.xor_score, 0.5, 1e-15)) return 3;
    if (!near(s.partial, 0.9375, 1e-15)) return 4;
    if (!near(s.legacy, 0.5, 1e-12)) return 5;
    if (!near(swe_pxor_xor_parity_arr(a), s.xor_score, 1e-15)) return 6;
    if (!near(swe_pxor_fifth_root(0.0), 0.0, 0.0)) return 7;
    if (!near(swe_pxor_fifth_root(1.0), 1.0, 1e-14)) return 8;
    if (!near(swe_pxor_fifth_root(32.0), 2.0, 1e-12)) return 9;
    if (!near(swe_pxor_fifth_root(0.00000000000000000001), 0.0001, 1e-12)) return 10;
    if (!near(swe_pxor_fifth_root(-32.0), -2.0, 1e-12)) return 11;

    f.h0 = 1.0; f.h1 = 1.0; f.h2 = 1.0; f.h3 = 1.0; f.h4 = 1.0;
    s = swe_pxor_scores5(f);
    if (!near(s.and_score, 1.0, 0.0)) return 12;
    if (!near(s.xor_score, 1.0, 0.0)) return 13;
    return 0;
}
