// SPDX-License-Identifier: 0BSD
// Copyright (c) 2026 Paul Allen Cheek
// DecaSigma: reversible algebra witness, NOT an XOR operator or cipher.
// Include-free C++20; zero allocation; preserves all intermediate states.
//
// Exact algebra (x != 0,1):
// R(x)=1/x, S(x)=1-x, T(x)=R(S(R(x)))=x/(x-1)
// Ten named operations: R S R T R S R T R R.
// Floating-point paths are guarded and not guaranteed to reconstruct exactly.

#ifndef SWE_DECA_SIGMA_HPP
#define SWE_DECA_SIGMA_HPP

namespace swe::deca_sigma {

// One source of truth for all ten DecaSigma named operations.
inline constexpr char operations[] = "RSRTRSRTRR";

struct Trace {
    double stages[11]{};
    bool admissible = false;
};

// Finite IEEE-754 binary64 range. These are the intended double semantics.
// The comparison rejects NaN and either infinity without using headers.
constexpr bool finite(double x) noexcept {
    return x <= 1.7976931348623157e308 &&
           x >= -1.7976931348623157e308;
}

constexpr Trace run(double x) noexcept {
    Trace trace{};
    trace.stages[0] = x;
    if (!finite(x) || x == 0.0 || x == 1.0) return trace;

    for (unsigned i = 0; i < 10u; ++i) {
        const double prior = trace.stages[i];
        if (!finite(prior)) return trace;

        double next = 0.0;
        if (operations[i] == 'R') {
            if (prior == 0.0) return trace;
            next = 1.0 / prior;
        } else if (operations[i] == 'S') {
            next = 1.0 - prior;
        } else {
            // T retains its original nested domain guard, not merely x/(x-1).
            if (prior == 0.0 || prior == 1.0) return trace;
            next = prior / (prior - 1.0);
        }
        if (!finite(next)) return trace;
        trace.stages[i + 1u] = next;
    }
    trace.admissible = true;
    return trace;
}

constexpr double magnitude(double x) noexcept { return x < 0.0 ? -x : x; }

constexpr bool near(double a, double b,
                    double relative_tolerance = 1e-12) noexcept {
    if (!finite(a) || !finite(b) || !finite(relative_tolerance) ||
        relative_tolerance < 0.0) return false;
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
static_assert(witnesses_return(run(-9.0)));
static_assert(witnesses_return(run(2.0)));

} // namespace swe::deca_sigma

#endif // SWE_DECA_SIGMA_HPP
