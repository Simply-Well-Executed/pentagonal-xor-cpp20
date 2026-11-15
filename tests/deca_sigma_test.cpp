// SPDX-License-Identifier: 0BSD
// Copyright (c) 2026 Paul Allen Cheek
// Validate the DecaSigma audit trace separately from the XOR parity engine.

#include "../shared/deca_sigma.hpp"
#include <cmath>
#include <iostream>

int main() {
    using swe::deca_sigma::run;
    using swe::deca_sigma::witnesses_return;

    if (run(0.0).admissible || run(1.0).admissible) return 1;
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
