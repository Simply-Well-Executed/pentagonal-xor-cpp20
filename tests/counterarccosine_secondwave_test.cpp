/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
// Copyright (c) 2026 Paul Allen Cheek
// Experimental second-wave coordinate, independent of RF antenna gain.
// Recomputes the white-paper SVG sample checkpoints and complement identity.
#include "../shared/coreao_gain.hpp"
#include <cmath>
#include <iostream>
#include <numbers>

namespace {
constexpr double pi = std::numbers::pi_v<double>;
constexpr double N = 16.0;
bool near(double a, double b, double tolerance = 1e-12) {
    return std::fabs(a-b) <= tolerance;
}
double base(double i) {
    return std::acos(std::cos(4.0*pi*i/N));
}
double counter(double i) {
    return pi-base(i);
}
}

int main() {
    const double expected[5] = {pi,0.0,pi,0.0,pi};
    for (unsigned j=0; j<5u; ++j) {
        const double i = static_cast<double>(4u*j);
        if (!near(counter(i),expected[j])) return 1;
    }
    for (unsigned j=0; j<=256u; ++j) {
        const double i = N*static_cast<double>(j)/256.0;
        const double reference=base(i), opposition=counter(i);
        if (!near(reference+opposition,pi) ||
            reference < -1e-12 || reference>pi+1e-12 ||
            opposition< -1e-12 || opposition>pi+1e-12) return 2;

        // Optional synthetic, phase-derived input. Fixed +9 dBi is
        // applied to this *specified* input, not inferred from the phase.
        const double amplitude = std::sin(opposition/2.0);
        const double input = amplitude*amplitude;
        const auto witness=swe::coreao::rf_gain::apply(input);
        if (!witness.passed() ||
            !near(witness.output_eirp_watts,
                  input*std::pow(10.0,0.9))) return 3;
    }
    if (!near(counter(0.0),pi) || !near(counter(N/4.0),0.0)) return 4;
    std::cout << "PASS exploratory second-wave phase graph: x=i, y in pi radians; "
                 "257 complementary witnesses, no measured antenna claims\n";
    return 0;
}
