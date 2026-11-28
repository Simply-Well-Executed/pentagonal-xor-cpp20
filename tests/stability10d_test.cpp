/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
// Copyright (c) 2026 Paul Allen Cheek
// PR #17 cross-language contract independently recomputed with 16 minterms.
#include "../shared/stability10d.hpp"
#include <array>
#include <bit>
#include <cmath>
#include <cstring>
#include <iostream>
#include <limits>

namespace {
using Input = std::array<double, 9>;
bool near(double a, double b, double eps=1e-12) {
    return std::fabs(a - b) <= eps;
}
double minterms(const Input& h, unsigned ring) {
    double sum = 0.0;
    for (unsigned mask=0; mask<32u; ++mask) {
        if ((std::popcount(mask) & 1u) == 0u) continue;
        double term=1.0;
        for(unsigned j=0; j<5u; ++j) {
            const double value=h[swe::stability10d::indices[ring][j]];
            term *= (mask & (1u<<j)) ? value : (1.0-value);
        }
        sum += term;
    }
    return sum;
}
bool golden(const Input& h, const char* hex, double scalar) {
    double copy[9]{};
    for(unsigned i=0; i<9u; ++i) copy[i]=h[i];
    swe::stability10d::Result result{};
    if (!swe::stability10d::calculate(copy, result) || !result.admissible)
        return false;
    char generated[13]{};
    swe::stability10d::hex(result, generated);
    if (std::strcmp(generated, hex) != 0 ||
        !near(result.stability, scalar, 0.0)) return false;
    for(unsigned i=0; i<9u; ++i)
        if (copy[i] != h[i] || result.ligands[i] != h[i]) return false;
    for(unsigned ring=0; ring<3u; ++ring) {
        const double independent = minterms(h, ring);
        if (!near(result.rings[ring], independent)) return false;
        const unsigned word=static_cast<unsigned>(independent*65535.0+0.5);
        if (word != result.words[ring]) return false;
        if ((word & 255u) != result.hexabyte[2u*ring] ||
            ((word>>8u)&255u) != result.hexabyte[2u*ring+1u]) return false;
    }
    return true;
}
} // namespace

int main() {
    if (!golden({0,0,0,0,0,0,0,0,0}, "000000000000", 1.0)) return 1;
    if (!golden({1,1,1,1,1,1,1,1,1}, "ffffffffffff", 1.0)) return 2;
    if (!golden({.5,.5,.5,.5,.5,.5,.5,.5,.5},
                "008000800080", 1.0)) return 3;
    if (!golden({0,0,0,1,0,0,0,0,0}, "ffffffff0000", 0.0)) return 4;

    // Every 9-ligand pattern is compared against independently expanded
    // 16 minterms per ring and round-tripped through all six bytes.
    unsigned state=0x12578A3Du;
    for(unsigned k=0; k<2048u; ++k) {
        Input h{};
        for(double& element : h) {
            state=state*1664525u+1013904223u;
            element=static_cast<double>((state >> 16u) % 1001u)/1000.0;
        }
        double raw[9]{};
        for(unsigned i=0; i<9u; ++i) raw[i]=h[i];
        swe::stability10d::Result out{};
        if (!swe::stability10d::calculate(raw,out)) return 5;
        double low=1.0,high=0.0;
        for(unsigned ring=0; ring<3u; ++ring) {
            if (!near(out.rings[ring],minterms(h,ring))) return 6;
            if (out.rings[ring]<low) low=out.rings[ring];
            if (out.rings[ring]>high) high=out.rings[ring];
        }
        if (!near(out.stability,1.0-(high-low))) return 7;
    }

    // Strict fail-closed preconditions and no success-shaped output mutation.
    swe::stability10d::Result sentinel{};
    sentinel.stability = -123.0;
    double bad[9] = {0,0,0,0,0,0,0,0,0};
    bad[3] = -0.001;
    if (swe::stability10d::calculate(bad,sentinel) ||
        sentinel.stability != -123.0 || sentinel.admissible) return 8;
    bad[3] = std::numeric_limits<double>::quiet_NaN();
    if (swe::stability10d::calculate(bad,sentinel) ||
        sentinel.stability != -123.0) return 9;
    bad[3] = 1.01;
    if (swe::stability10d::calculate(bad,sentinel)) return 10;

    // Exact 9-measurement source order, no recasting the input CORE.
    const double measurements[9] = {3,2,20,
        std::sqrt(14.0/3.0), std::sqrt(1400.0/3.0),3,30,0,0};
    double h[9]{};
    if (!swe::stability10d::normalize(measurements,h)) return 11;
    if (!near(h[0],3.0/1027.0) ||
        !near(h[1],0.5*(1.0+2.0/3.0)) || h[7]!=0.0) return 12;
    if (!swe::stability10d::from_measurements(measurements,sentinel) ||
        !sentinel.admissible) return 13;
    double noninteger[9] = {3.5,2,20,2,20,3,30,0,0};
    if (swe::stability10d::normalize(noninteger,h)) return 14;
    double impossible[9] = {3,2,20,2,20,3,30,3,0};
    if (swe::stability10d::normalize(impossible,h)) return 15;
    double overflow[9] = {1e308,2,20,2,20,3,30,0,0};
    if (swe::stability10d::normalize(overflow,h)) return 16;

    std::cout << "PASS Stability-9L3PX5-U16LE-v1: four exact hexabytes, "
                 "2048 independent 3x16-minterm fixtures, no mutation/invalid guards\n";
    return 0;
}
