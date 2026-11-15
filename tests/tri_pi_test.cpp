// SPDX-License-Identifier: 0BSD
// Copyright (c) 2026 Paul Allen Cheek
// TriPi: each audit route is read-only, and each can reject a forged trace.

#include "../shared/tri_pi.hpp"
#include <iostream>
#include <limits>

int main() {
    using swe::deca_sigma::Trace;
    using swe::deca_sigma::run;
    using swe::tri_pi::inspect;

    // All three inspection routes pass legitimate DecaSigma cases.
    constexpr double examples[] = {-9.0, 2.0, -2.0, 0.5, 3.5};
    for (double x : examples) {
        const Trace original = run(x);
        const Trace frozen = original;  // verify no write to the source record
        const auto report = inspect(original);
        if (!report.forward || !report.reverse || !report.zfold ||
            !report.passed()) return 1;
        for (unsigned i = 0; i < 11u; ++i)
            if (original.stages[i] != frozen.stages[i]) return 2;
    }

    // A single altered intermediate state fails all three checks.
    Trace tampered = run(-9.0);
    tampered.stages[3] += 0.25;
    const auto bad = inspect(tampered);
    if (bad.passed() || bad.forward || bad.reverse || bad.zfold) return 3;

    // Correlated edits can satisfy a repeated-stage check, but not all routes.
    Trace correlated = run(-9.0);
    correlated.stages[1] += 0.125;
    correlated.stages[5] += 0.125;
    correlated.stages[9] += 0.125;
    const auto correlated_report = inspect(correlated);
    if (correlated_report.passed() ||
        correlated_report.forward || correlated_report.reverse ||
        !correlated_report.zfold) return 4;

    // Spoofed CORE and illegitimate flags must not be accepted.
    Trace spoofed = run(-9.0);
    spoofed.stages[0] = 42.0;
    if (inspect(spoofed).passed()) return 5;
    spoofed = run(-9.0);
    spoofed.admissible = false;
    if (inspect(spoofed).passed()) return 6;

    // DecaSigma invalid cases and nonfinite values remain invalid.
    if (inspect(run(0.0)).passed() || inspect(run(1.0)).passed() ||
        inspect(run(std::numeric_limits<double>::infinity())).passed() ||
        inspect(run(std::numeric_limits<double>::quiet_NaN())).passed()) {
        return 7;
    }

    // Invalid tolerance is not permitted to manufacture a PASS.
    if (inspect(run(-9.0), -1.0).passed() ||
        inspect(run(-9.0), std::numeric_limits<double>::quiet_NaN()).passed())
        return 8;

    // The three route names retain their exact user-defined spelling.
    if (swe::tri_pi::canonical_names[0][0] != '2' ||
        swe::tri_pi::canonical_names[1][0] != 'Y' ||
        swe::tri_pi::canonical_names[2][0] != 'P') return 9;

    std::cout << "TriPi PASS: 2ndTRACE + Y= + PlySmlt2; "
                 "negative tampering and domain fixtures rejected\n";
    return 0;
}
