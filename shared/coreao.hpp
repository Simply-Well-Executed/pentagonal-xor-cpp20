// SPDX-License-Identifier: 0BSD
// Copyright (c) 2026 Paul Allen Cheek
// COREAO: CORE -> TriPi I (forward) -> TriPi II (reverse) -> TriPi III (Z-fold).
// A single, read-only scalar audit of a frozen DecaSigma witness.
// Include-free C++20. No change to the five-input XOR production hot path.

#ifndef SWE_COREAO_HPP
#define SWE_COREAO_HPP

#include "tri_pi.hpp"

namespace swe::coreao {

// The scalar is the largest normalized disagreement across every comparison.
// Zero means all compared values agree to floating-point precision.
// An invalid/undefined route is represented by admissible=false, NOT by zero.
struct Result {
    double scalar = 0.0;
    bool admissible = false;

    constexpr bool passed(double tolerance = 1e-12) const noexcept {
        return admissible && deca_sigma::finite(tolerance) &&
               tolerance >= 0.0 && scalar <= tolerance;
    }
};

constexpr double magnitude(double x) noexcept {
    return x < 0.0 ? -x : x;
}

constexpr double largest(double a, double b) noexcept {
    return a > b ? a : b;
}

// Scale before subtraction to avoid overflow with large finite doubles.
// For finite a,b: 0 <= normalized_residual(a,b) <= 2.
constexpr double normalized_residual(double a, double b) noexcept {
    const double scale = largest(1.0, largest(magnitude(a), magnitude(b)));
    return magnitude(a / scale - b / scale);
}

constexpr bool step(double before, char op, double& after) noexcept {
    if (!deca_sigma::finite(before)) return false;
    if (op == 'R') {
        if (before == 0.0) return false;
        after = 1.0 / before;
    } else if (op == 'S') {
        after = 1.0 - before;
    } else if (op == 'T') {
        // Preserve the original nested R(S(R(x))) domain.
        if (before == 0.0 || before == 1.0) return false;
        after = before / (before - 1.0);
    } else {
        return false;
    }
    return deca_sigma::finite(after);
}

// No route selection, no mutation, one result.
// I: Construct forward states from the original CORE (stage 0).
// II: Populate the reverse path from route I's final state.
// III: Traverse the original trace in Z-order, comparing both paths,
//      then validate all cross-fold and return-to-CORE equalities.
constexpr Result inspect(const deca_sigma::Trace& frozen) noexcept {
    Result result{};
    if (!frozen.admissible ||
        !deca_sigma::finite(frozen.stages[0]) ||
        frozen.stages[0] == 0.0 || frozen.stages[0] == 1.0) return result;
    for (double value : frozen.stages)
        if (!deca_sigma::finite(value)) return result;

    // YIN input: original CORE; regenerate the forward witness.
    const deca_sigma::Trace forward = deca_sigma::run(frozen.stages[0]);
    if (!forward.admissible) return result;

    // YIN output: invert the forward path, NOT the possibly altered
    // intermediate stages of the supplied record.
    double reverse[11]{};
    reverse[10] = forward.stages[10];
    double error = 0.0;
    for (unsigned i = 10u; i > 0u; --i) {
        if (!step(reverse[i], tri_pi::operations[i - 1u], reverse[i - 1u]))
            return result;
        error = largest(error,
                        normalized_residual(reverse[i - 1u],
                                            forward.stages[i - 1u]));
    }

    // YANG output: Z-fold all 11 stages, then weave the two independent
    // *calculation paths* against the one frozen source record.
    for (unsigned index : tri_pi::zfold_order) {
        if (index >= 11u) return result;
        error = largest(error,
                        normalized_residual(frozen.stages[index],
                                            forward.stages[index]));
        error = largest(error,
                        normalized_residual(frozen.stages[index],
                                            reverse[index]));
    }

    // Retain cross-fold witnesses. A matching Z-pattern alone is not enough;
    // the forward and reverse algebra above also has to agree.
    constexpr unsigned pairs[7][2] = {
        {0u,4u}, {4u,8u}, {8u,10u},
        {1u,5u}, {5u,9u}, {2u,6u}, {3u,7u}
    };
    for (const auto& pair : pairs) {
        error = largest(error,
                        normalized_residual(frozen.stages[pair[0]],
                                            frozen.stages[pair[1]]));
    }

    result.scalar = error;
    result.admissible = true;
    return result;
}

static_assert(inspect(deca_sigma::run(2.0)).passed());
static_assert(inspect(deca_sigma::run(-9.0)).passed());
static_assert(!inspect(deca_sigma::run(0.0)).passed());
static_assert(!inspect(deca_sigma::run(1.0)).passed());

} // namespace swe::coreao

#endif // SWE_COREAO_HPP
