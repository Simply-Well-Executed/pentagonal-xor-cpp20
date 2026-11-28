/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
// Copyright (c) 2026 Paul Allen Cheek
// COREAO RF-domain extension: input -> fixed +9 dBi directional gain -> EIRP.
// Conducted input power is unchanged. This is NOT an audio amplification,
// artificial-intelligence, wireless simulation, or crypto security function.
// No heap, I/O, std headers, run-time mode switches or measurements.

#ifndef SWE_COREAO_GAIN_HPP
#define SWE_COREAO_GAIN_HPP

#include "coreao.hpp"

namespace swe::coreao::rf_gain {

inline constexpr double gain_dbi = 9.0;
// 10^(9/10), evaluated as an explicit versioned binary64 constant.
// dBi is a directional gain relative to an isotropic radiator.
inline constexpr double power_factor = 7.943282347242816;
inline constexpr double field_factor = 2.8183829312644537;

struct Result {
    double input_watts = 0.0;
    double gain_dbi = rf_gain::gain_dbi;
    double gain_linear = rf_gain::power_factor;
    double output_eirp_watts = 0.0;
    double recovered_input_watts = 0.0;
    double normalized_residual = 0.0;
    bool admissible = false;

    constexpr bool passed(double tolerance = 1e-12) const noexcept {
        return admissible && deca_sigma::finite(tolerance) &&
               tolerance >= 0.0 && normalized_residual <= tolerance;
    }
};

// SOURCE input (conducted W) -> gain (+9 dBi) -> derived output (EIRP W).
// EIRP is the equivalent isotropic radiated power in the reference direction.
// It is NOT physical total antenna radiated watts or actual RF measurement.
// Assumptions: no feedline/connector loss, ideal matching, reference direction.
// A negative, infinite or overflowing power is inadmissible.
constexpr Result apply(double input_watts) noexcept {
    Result result{};
    if (!deca_sigma::finite(input_watts) || input_watts < 0.0 ||
        input_watts > 1.7976931348623157e308 / power_factor) {
        return result;
    }

    const double output = input_watts * power_factor;
    const double recovered = output / power_factor;
    if (!deca_sigma::finite(output) || !deca_sigma::finite(recovered)) {
        return result;
    }

    result.input_watts = input_watts;
    result.output_eirp_watts = output;
    result.recovered_input_watts = recovered;
    result.normalized_residual =
        coreao::normalized_residual(input_watts, recovered);
    result.admissible = true;
    return result;
}

static_assert(apply(0.0).passed());
static_assert(apply(1.0).passed());
static_assert(apply(1.0).output_eirp_watts > 7.94 &&
              apply(1.0).output_eirp_watts < 7.95);
static_assert(!apply(-1.0).passed());

} // namespace swe::coreao::rf_gain

#endif // SWE_COREAO_GAIN_HPP
