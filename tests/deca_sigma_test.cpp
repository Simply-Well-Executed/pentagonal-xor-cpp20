/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
// Copyright (c) 2026 Paul Allen Cheek
// Validate the DecaSigma audit trace separately from the XOR parity engine.

#include "../shared/deca_sigma.hpp"
#include <cmath>
#include <iostream>
#include <limits>

int main() {
    using swe::deca_sigma::run;
    using swe::deca_sigma::witnesses_return;

    if (run(0.0).admissible || run(1.0).admissible) return 1;
    if (run(std::numeric_limits<double>::infinity()).admissible ||
        run(-std::numeric_limits<double>::infinity()).admissible ||
        run(std::numeric_limits<double>::quiet_NaN()).admissible) return 4;
    constexpr double examples[] = {-9.0, 2.0, -2.0, 0.5, 3.5};
    for (double x : examples) {
        const auto trace = run(x);
        if (!witnesses_return(trace)) return 2;
        for (double value : trace.stages) {
            if (!std::isfinite(value)) return 3;
        }
    }
    const auto trace = run(-9.0);
    std::cout << "DecaSigma: 10 operations; CORE=" << trace.stages[0]
              << "; stages[4,8,10]=" << trace.stages[4] << ","
              << trace.stages[8] << "," << trace.stages[10] << '\n'
              << "PASS: guarded numerical round-trip on selected fixtures\n";
    return 0;
}
