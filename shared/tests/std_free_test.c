// SPDX-License-Identifier: 0BSD
// Copyright (c) Paul Allen Cheek
// C11 test for the include-free Pentagonal XOR header.

#include "../pentagonal_xor_std_free.h"

/* Check preprocessor target identification without runtime OS APIs. */
#if defined(__ANDROID__)
#  if !SWE_PXOR_TARGET_ANDROID || SWE_PXOR_TARGET_LINUX
#    error Android must not be classified as desktop Linux
#  endif
#endif
#if defined(__linux__) || defined(__linux) || defined(linux) || defined(__gnu_linux__)
#  if !SWE_PXOR_TARGET_LINUX_KERNEL
#    error Linux compiler macro not recognized
#  endif
#  if !defined(__ANDROID__) && !SWE_PXOR_TARGET_LINUX
#    error Linux host not recognized
#  endif
#endif
#if defined(_WIN32)
#  if !SWE_PXOR_TARGET_WINDOWS
#    error Windows target not recognized
#  endif
#endif
#if SWE_PXOR_TARGET_LINUX && SWE_PXOR_TARGET_ANDROID
#  error Mutually exclusive desktop Linux and Android
#endif
#if SWE_PXOR_TARGET_WINDOWS && SWE_PXOR_TARGET_LINUX
#  error Mutually exclusive Windows and Linux
#endif

static int near(double a, double b, double tolerance)
{
    double delta = a - b;
    if (delta < 0.0) delta = -delta;
    return delta <= tolerance;
}

static int near_relative(double a, double b, double tolerance)
{
    double delta = a - b;
    double scale = b < 0.0 ? -b : b;
    if (delta < 0.0) delta = -delta;
    if (scale == 0.0) scale = 1.0;
    return delta <= tolerance * scale;
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
    if (!near_relative(swe_pxor_fifth_root(1.0e-300), 1.0e-60, 1e-12)) return 14;
    if (!near_relative(swe_pxor_fifth_root(1.0e300), 1.0e60, 1e-12)) return 15;

    f.h0 = 1.0; f.h1 = 1.0; f.h2 = 1.0; f.h3 = 1.0; f.h4 = 1.0;
    s = swe_pxor_scores5(f);
    if (!near(s.and_score, 1.0, 0.0)) return 12;
    if (!near(s.xor_score, 1.0, 0.0)) return 13;
    return 0;
}
