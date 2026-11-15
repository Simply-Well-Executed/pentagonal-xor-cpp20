// SPDX-License-Identifier: 0BSD
// Copyright (c) 2026 Paul Allen Cheek
// COREAO single-scalar inspection: production parity remains unchanged.

#include "../shared/coreao.hpp"
#include <cmath>
#include <iostream>
#include <limits>

int main() {
    using swe::coreao::inspect;
    using swe::deca_sigma::Trace;
    using swe::deca_sigma::run;

    // Defined inputs round-trip. CORE and all intermediate records are read-only.
    constexpr double examples[] = {-9.0, 2.0, -2.0, 0.5, 3.5};
    for (double x : examples) {
        const Trace source = run(x);
        const Trace prior = source;
        const auto audit = inspect(source);
        if (!audit.admissible || !audit.passed() || !std::isfinite(audit.scalar)
            || audit.scalar < 0.0) return 1;
        for (unsigned i = 0; i < 11u; ++i) {
            if (source.stages[i] != prior.stages[i]) return 2;
        }
        if (source.admissible != prior.admissible) return 3;
    }

    // Exact dyadic binary64 fixture returns zero scalar.
    if (inspect(run(2.0)).scalar != 0.0) return 4;

    // One mutated value must raise the scalar, not be averaged away.
    Trace damaged = run(-9.0);
    damaged.stages[3] += 0.25;
    const auto inconsistent = inspect(damaged);
    if (!inconsistent.admissible || inconsistent.passed() ||
        inconsistent.scalar <= 0.001) return 5;

    // Matching edits at repeated stages can fool a Z-only witness.
    // Full COREAO compares against a regenerated forward trace.
    Trace correlated = run(-9.0);
    correlated.stages[1] += 0.125;
    correlated.stages[5] += 0.125;
    correlated.stages[9] += 0.125;
    const auto correlated_audit = inspect(correlated);
    if (!correlated_audit.admissible || correlated_audit.passed() ||
        correlated_audit.scalar <= 0.001) return 6;

    // Replacing the source CORE without regenerating subsequent states fails.
    Trace spoofed = run(-9.0);
    spoofed.stages[0] = 42.0;
    if (inspect(spoofed).passed()) return 7;

    // Invalid cases are not a fake zero-error PASS.
    if (inspect(run(0.0)).passed() || inspect(run(1.0)).passed() ||
        inspect(run(std::numeric_limits<double>::infinity())).passed() ||
        inspect(run(std::numeric_limits<double>::quiet_NaN())).passed()) {
        return 8;
    }
    Trace forbidden = run(-9.0);
    forbidden.admissible = false;
    if (inspect(forbidden).passed()) return 9;
    forbidden = run(-9.0);
    forbidden.stages[6] = std::numeric_limits<double>::infinity();
    if (inspect(forbidden).passed()) return 10;

    // Invalid thresholds cannot override admissibility.
    if (inspect(run(-9.0)).passed(-1.0) ||
        inspect(run(-9.0)).passed(std::numeric_limits<double>::quiet_NaN()))
        return 11;

    const auto example = inspect(run(-9.0));
    std::cout << "COREAO PASS: scalar=" << example.scalar
              << " (lower is better); forward -> reverse -> Z-fold\n";
    return 0;
}
