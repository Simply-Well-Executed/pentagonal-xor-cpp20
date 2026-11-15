// SPDX-License-Identifier: 0BSD
// Copyright (c) 2026 Paul Allen Cheek
// TriPi (Triℼ): three read-only inspection routes over a frozen DecaSigma trace.
// Independent routes do not imply independent experimental observations.
// No standard headers, allocation, I/O or effects on the parity hot path.

#ifndef SWE_TRI_PI_HPP
#define SWE_TRI_PI_HPP

#include "deca_sigma.hpp"

namespace swe::tri_pi {

inline constexpr const char* canonical_names[3] = {
    "2ndTRACE", "Y=", "PlySmlt2"
};

// 11 states, 10 named operations. Traversal is separate from evaluation.
// Clockwise / forward, anticlockwise / reverse, Z-unfold / cross-compare.
inline constexpr unsigned forward_order[11] =
    {0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u};
inline constexpr unsigned reverse_order[11] =
    {10u, 9u, 8u, 7u, 6u, 5u, 4u, 3u, 2u, 1u, 0u};
inline constexpr unsigned zfold_order[11] =
    {0u, 1u, 3u, 2u, 4u, 5u, 7u, 6u, 8u, 9u, 10u};

inline constexpr char operations[] = "RSRTRSRTRR";

struct Report {
    bool forward = false;
    bool reverse = false;
    bool zfold = false;

    constexpr bool passed() const noexcept {
        return forward && reverse && zfold;
    }
};

constexpr bool valid_input(const deca_sigma::Trace& trace,
                           double tolerance) noexcept {
    if (!trace.admissible || !deca_sigma::finite(tolerance) ||
        tolerance < 0.0) return false;
    for (unsigned i = 0; i < 11u; ++i) {
        if (!deca_sigma::finite(trace.stages[i])) return false;
    }
    return trace.stages[0] != 0.0 && trace.stages[0] != 1.0;
}

// First route: 2ndTRACE. Verify each directed operation from CORE to stage 10.
constexpr bool inspect_forward(const deca_sigma::Trace& trace,
                               double tolerance = 1e-12) noexcept {
    if (!valid_input(trace, tolerance)) return false;

    for (unsigned i = 0; i < 10u; ++i) {
        const double before = trace.stages[forward_order[i]];
        const double after = trace.stages[forward_order[i + 1u]];
        double expected = 0.0;
        if (operations[i] == 'R') {
            if (before == 0.0) return false;
            expected = 1.0 / before;
        } else if (operations[i] == 'S') {
            expected = 1.0 - before;
        } else {
            if (before == 0.0 || before == 1.0) return false;
            expected = before / (before - 1.0);
        }
        if (!deca_sigma::near(expected, after, tolerance)) return false;
    }
    return true;
}

// Second route: Y=. Recover each earlier state with inverse operations,
// inspecting in the opposite direction. R, S and T are self-inverse.
constexpr bool inspect_reverse(const deca_sigma::Trace& trace,
                               double tolerance = 1e-12) noexcept {
    if (!valid_input(trace, tolerance)) return false;

    for (unsigned i = 10u; i > 0u; --i) {
        const double after = trace.stages[reverse_order[10u - i]];
        const double before = trace.stages[reverse_order[11u - i]];
        double reconstructed = 0.0;
        const char op = operations[i - 1u];
        if (op == 'R') {
            if (after == 0.0) return false;
            reconstructed = 1.0 / after;
        } else if (op == 'S') {
            reconstructed = 1.0 - after;
        } else {
            // Do not extend T's original nested domain to 0 or 1.
            if (after == 0.0 || after == 1.0 ||
                before == 0.0 || before == 1.0) return false;
            reconstructed = after / (after - 1.0);
        }
        if (!deca_sigma::near(reconstructed, before, tolerance)) return false;
    }
    return true;
}

// Third route: PlySmlt2. Inspect corresponding stages of both four-corner
// folds in a Z-order; verify repeated states and the final BiDi hinge.
// Cross-checks are necessary witness comparisons, not an independent proof.
constexpr bool inspect_zfold(const deca_sigma::Trace& trace,
                             double tolerance = 1e-12) noexcept {
    if (!valid_input(trace, tolerance)) return false;
    const double* s = trace.stages;
    // Make the Z path a complete, non-mutating 11-stage visit.
    bool seen[11]{};
    for (unsigned index : zfold_order) {
        if (index >= 11u || seen[index]) return false;
        seen[index] = true;
    }
    for (bool visited : seen) if (!visited) return false;

    return deca_sigma::near(s[0], s[4], tolerance) &&
           deca_sigma::near(s[4], s[8], tolerance) &&
           deca_sigma::near(s[8], s[10], tolerance) &&
           deca_sigma::near(s[1], s[5], tolerance) &&
           deca_sigma::near(s[5], s[9], tolerance) &&
           deca_sigma::near(s[2], s[6], tolerance) &&
           deca_sigma::near(s[3], s[7], tolerance);
}

constexpr Report inspect(const deca_sigma::Trace& trace,
                         double tolerance = 1e-12) noexcept {
    return {inspect_forward(trace, tolerance),
            inspect_reverse(trace, tolerance),
            inspect_zfold(trace, tolerance)};
}

static_assert(inspect(deca_sigma::run(-9.0)).passed());
static_assert(inspect(deca_sigma::run(2.0)).passed());
static_assert(!inspect(deca_sigma::run(0.0)).passed());
static_assert(!inspect(deca_sigma::run(1.0)).passed());

} // namespace swe::tri_pi

#endif // SWE_TRI_PI_HPP
