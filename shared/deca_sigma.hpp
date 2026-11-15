// SPDX-License-Identifier: 0BSD
// Copyright (c) 2026 Paul Allen Cheek
// DecaSigma: exploratory, reversible algebra witness; NOT an XOR or cipher.
// Include-free C++20. Caller supplies a finite real x; x != 0 and x != 1.
//
// R(x) = 1/x; S(x) = 1-x; T(x) = R(S(R(x))) = x/(x-1).
// Ten named operations: R S R T R S R T R R.
// Floating-point round trips are approximate; see witnesses_return().
// Neither the source x nor any existing Pentagonal XOR score is mutated.

#ifndef SWE_DECA_SIGMA_HPP
#define SWE_DECA_SIGMA_HPP

namespace swe::deca_sigma {

struct Trace {
    double stages[11]{};
    bool admissible = false; // All nested divisions checked for zero.
};

constexpr bool nonzero_number(double x) noexcept {
    return x == x && x != 0.0;
}

constexpr bool span_operand(double x) noexcept {
    return nonzero_number(x) && x != 1.0;
}

constexpr Trace run(double x) noexcept {
    Trace result{};
    result.stages[0] = x;
    if (!span_operand(x)) return result;

    // First QuadSigma: R, S, R, T.
    result.stages[1] = 1.0 / result.stages[0];
    result.stages[2] = 1.0 - result.stages[1];
    if (!nonzero_number(result.stages[2])) return result;
    result.stages[3] = 1.0 / result.stages[2];
    if (!span_operand(result.stages[3])) return result;
    result.stages[4] = result.stages[3] / (result.stages[3] - 1.0);
    if (!nonzero_number(result.stages[4])) return result;

    // Second QuadSigma: R, S, R, T.
    result.stages[5] = 1.0 / result.stages[4];
    result.stages[6] = 1.0 - result.stages[5];
    if (!nonzero_number(result.stages[6])) return result;
    result.stages[7] = 1.0 / result.stages[6];
    if (!span_operand(result.stages[7])) return result;
    result.stages[8] = result.stages[7] / (result.stages[7] - 1.0);
    if (!nonzero_number(result.stages[8])) return result;

    // BiDi hinge: R, R.
    result.stages[9] = 1.0 / result.stages[8];
    if (!nonzero_number(result.stages[9])) return result;
    result.stages[10] = 1.0 / result.stages[9];
    result.admissible = result.stages[10] == result.stages[10];
    return result;
}

constexpr double magnitude(double x) noexcept { return x < 0 ? -x : x; }

constexpr bool near(double a, double b, double relative_tolerance = 1e-12) noexcept {
    const double scale = magnitude(b) > 1.0 ? magnitude(b) : 1.0;
    return magnitude(a - b) <= relative_tolerance * scale;
}

constexpr bool witnesses_return(const Trace& trace,
                                double relative_tolerance = 1e-12) noexcept {
    return trace.admissible &&
           near(trace.stages[4], trace.stages[0], relative_tolerance) &&
           near(trace.stages[8], trace.stages[0], relative_tolerance) &&
           near(trace.stages[10], trace.stages[0], relative_tolerance);
}

static_assert(!run(0.0).admissible);
static_assert(!run(1.0).admissible);
static_assert(witnesses_return(run(2.0)));
static_assert(witnesses_return(run(-9.0)));

} // namespace swe::deca_sigma

#endif // SWE_DECA_SIGMA_HPP
