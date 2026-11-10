// SPDX-License-Identifier: 0BSD
// Copyright (c) Paul Allen Cheek
// Pentagonal XOR - freestanding-style, include-free C API.
// No standard headers, library calls, allocation, or platform APIs.
// Permission to use, copy, modify, and/or distribute this software
// for any purpose with or without fee is hereby granted.

#ifndef SWE_PENTAGONAL_XOR_STD_FREE_H
#define SWE_PENTAGONAL_XOR_STD_FREE_H

/*
 * This is a compile-target hint, not runtime OS detection.
 * _WIN32 identifies a Windows target. _WIN32_WINNT identifies the
 * minimum Windows API target selected by the build, not the installed OS.
 * If it is absent, conservatively classify the target as legacy.
 */
#if defined(_WIN32)
#  if !defined(_WIN32_WINNT) || (_WIN32_WINNT < 0x0A00)
#    define SWE_PXOR_WINDOWS_LEGACY_TARGET 1
#  else
#    define SWE_PXOR_WINDOWS_LEGACY_TARGET 0
#  endif
#else
#  define SWE_PXOR_WINDOWS_LEGACY_TARGET 0
#endif

/* Application-level selector: legacy Windows targets choose this backend. */
#if SWE_PXOR_WINDOWS_LEGACY_TARGET
#  define SWE_PXOR_USE_STDFREE_BACKEND 1
#else
#  define SWE_PXOR_USE_STDFREE_BACKEND 0
#endif

typedef struct {
    double h0, h1, h2, h3, h4;
} Features5;

typedef struct {
    double legacy;      /* fifth_root(AND); historical compatibility score */
    double and_score;   /* product(h[i]) */
    double or_score;    /* 1 - product(1 - h[i]) */
    double xor_score;   /* probability of odd parity for independent h[i] */
    double partial;     /* OR - AND */
} PentagonalScores;

/*
 * Fifth root for finite IEEE-754 binary64 inputs.
 * No pow(), math.h, headers, or standard-library dependencies.
 *
 * Range reduction keeps the Newton iteration in [1, 2^25):
 * multiplying/dividing the argument by 2^25 changes its fifth root
 * by exactly 2^5. 64 iterations are a conservative fixed bound.
 *
 * NaN and infinities are outside this function's documented contract.
 */
static inline double swe_pxor_fifth_root(double a)
{
    double factor = 1.0;
    double x;
    int scale_up = 0;
    int i;

    if (a == 0.0) return 0.0;
    if (a < 0.0) return -swe_pxor_fifth_root(-a);

    while (a < 1.0) {
        a *= 33554432.0; /* 2^25 */
        factor *= 32.0;
    }
    while (a >= 33554432.0) {
        a /= 33554432.0;
        factor *= 32.0;
        scale_up = 1;
    }

    x = 32.0;
    for (i = 0; i < 64; ++i) {
        double x2 = x * x;
        double x4 = x2 * x2;
        double next = (4.0 * x + a / x4) * 0.2;
        if (next == x) break;
        x = next;
    }

    return scale_up ? x * factor : x / factor;
}

/* Closed-form odd-parity probability for five inputs in [0,1]. */
static inline double swe_pxor_xor_parity5(Features5 f)
{
    return 0.5 * (1.0 - (1.0 - 2.0 * f.h0)
                        * (1.0 - 2.0 * f.h1)
                        * (1.0 - 2.0 * f.h2)
                        * (1.0 - 2.0 * f.h3)
                        * (1.0 - 2.0 * f.h4));
}

/* Array form. f must point to at least five doubles. */
static inline double swe_pxor_xor_parity_arr(const double f[5])
{
    return 0.5 * (1.0 - (1.0 - 2.0 * f[0])
                        * (1.0 - 2.0 * f[1])
                        * (1.0 - 2.0 * f[2])
                        * (1.0 - 2.0 * f[3])
                        * (1.0 - 2.0 * f[4]));
}

/* All scores. Inputs are expected to be finite values in [0,1]. */
static inline PentagonalScores swe_pxor_scores5(Features5 f)
{
    double prod_h = f.h0 * f.h1 * f.h2 * f.h3 * f.h4;
    double prod_not_h = (1.0 - f.h0) * (1.0 - f.h1) * (1.0 - f.h2) *
                        (1.0 - f.h3) * (1.0 - f.h4);
    double prod_parity = (1.0 - 2.0 * f.h0) * (1.0 - 2.0 * f.h1) *
                         (1.0 - 2.0 * f.h2) * (1.0 - 2.0 * f.h3) *
                         (1.0 - 2.0 * f.h4);
    PentagonalScores s;

    s.and_score = prod_h;
    s.or_score = 1.0 - prod_not_h;
    s.xor_score = 0.5 * (1.0 - prod_parity);
    s.partial = s.or_score - s.and_score;
    s.legacy = swe_pxor_fifth_root(prod_h);
    return s;
}

#endif /* SWE_PENTAGONAL_XOR_STD_FREE_H */
