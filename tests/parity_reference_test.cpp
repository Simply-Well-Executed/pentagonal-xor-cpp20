// SPDX-License-Identifier: 0BSD
// Copyright (c) 2026 Paul Allen Cheek
// Independent parity checks belong in CTest, never the production hot path.

#include "../shared/pentagonal_xor_std_free.h"
#include <array>
#include <bit>
#include <cmath>
#include <iostream>

namespace {
using Features = std::array<double, 5>;

double recursive(const Features& h) {
    double result = 0.0;
    for (double v : h) result = result * (1.0 - v) + (1.0 - result) * v;
    return result;
}

double minterms(const Features& h) {
    double result = 0.0;
    for (unsigned mask = 0; mask < 32u; ++mask) {
        if ((std::popcount(mask) & 1u) == 0u) continue;
        double product = 1.0;
        for (unsigned i = 0; i < 5; ++i) {
            product *= (mask & (1u << i)) ? h[i] : (1.0 - h[i]);
        }
        result += product;
    }
    return result;
}

bool near(double a, double b) { return std::abs(a - b) <= 1e-12; }

bool agree(const Features& h) {
    const double fast = swe_pxor_xor_parity_arr(h.data());
    return std::isfinite(fast) && fast >= 0.0 && fast <= 1.0 &&
           near(fast, recursive(h)) && near(fast, minterms(h));
}
} // namespace

int main() {
    // Exhaust all 32 Boolean configurations, including all zeros and all ones.
    for (unsigned mask = 0; mask < 32u; ++mask) {
        Features h{};
        for (unsigned i = 0; i < 5; ++i) {
            h[i] = (mask & (1u << i)) ? 1.0 : 0.0;
        }
        if (!agree(h)) return 1;
        const double expected = (std::popcount(mask) & 1u) ? 1.0 : 0.0;
        if (swe_pxor_xor_parity_arr(h.data()) != expected) return 2;
    }

    if (!agree({0.80, 0.70, 0.90, 0.60, 0.95})) return 3;
    if (!near(swe_pxor_xor_parity_arr(
                  Features{0.80, 0.70, 0.90, 0.60, 0.95}.data()), 0.51728)) return 4;
    if (!agree({0.5, 0.5, 0.5, 0.5, 0.5})) return 5;

    // Deterministic bounded continuous fixtures, including 0 and 1.
    unsigned seed = 0x12578A3Du;
    for (unsigned trial = 0; trial < 4096u; ++trial) {
        Features h{};
        for (unsigned i = 0; i < 5u; ++i) {
            seed = seed * 1664525u + 1013904223u;
            h[i] = static_cast<double>((seed >> 16) % 1001u) / 1000.0;
        }
        if (!agree(h)) return 6;
    }

    // Signed fifth-root regression: recursion was removed from the C11 header.
    // Preserve odd symmetry and the previous finite-input numeric contract.
    const double positive_root = swe_pxor_fifth_root(32.0);
    const double negative_root = swe_pxor_fifth_root(-32.0);
    if (!near(positive_root, 2.0) || !near(negative_root, -2.0) ||
        !near(negative_root, -positive_root)) return 7;
    if (!near(swe_pxor_fifth_root(-0.00000000000000000001), -0.0001)) return 8;

    std::cout << "PASS: 32 Boolean states + 4096 continuous fixtures; "
                 "signed fifth-root regression and parity references agree\n";
    return 0;
}
