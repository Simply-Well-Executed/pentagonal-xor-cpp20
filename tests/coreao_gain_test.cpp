/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
// Copyright (c) 2026 Paul Allen Cheek
// Independent +9 dBi RF COREAO reference test; no antenna is measured.
#include "../shared/coreao_gain.hpp"
#include <cmath>
#include <iostream>
#include <limits>

namespace {
bool near(double a, double b, double relative = 1e-12) {
    return std::fabs(a - b) <= relative *
        (std::fabs(b) > 1.0 ? std::fabs(b) : 1.0);
}
} // namespace

int main() {
    using swe::coreao::rf_gain::apply;

    // 1 W of input is equivalent to ~7.943282 W isotropic power in the
    // antenna's reference direction, NOT ~7.943 W of total radiated watts.
    constexpr double power = 1.0;
    const auto one = apply(power);
    if (!one.admissible || !one.passed() || one.gain_dbi != 9.0 ||
        !near(one.output_eirp_watts, std::pow(10.0, 0.9)) ||
        !near(one.gain_linear, std::pow(10.0, 0.9)) ||
        !near(one.recovered_input_watts, power) ||
        power != 1.0) return 1;

    // 1 mW input -> ~7.943282 mW EIRP. Source input unchanged.
    const auto milliwatt = apply(0.001);
    if (!milliwatt.passed() ||
        !near(milliwatt.output_eirp_watts, 0.001 * std::pow(10.0, 0.9)) ||
        !near(milliwatt.recovered_input_watts, 0.001)) return 2;

    const auto zero = apply(0.0);
    if (!zero.admissible || !zero.passed() ||
        zero.output_eirp_watts != 0.0) return 3;

    // Independent amplitude conversion for equal-impedance far-field
    // comparisons. A field ratio is not a new power source.
    if (!near(swe::coreao::rf_gain::field_factor,
              std::sqrt(std::pow(10.0, 0.9)))) return 4;

    // Invalid input does not produce a fake success-shaped result.
    if (apply(-0.01).passed() ||
        apply(std::numeric_limits<double>::quiet_NaN()).passed() ||
        apply(std::numeric_limits<double>::infinity()).passed() ||
        apply(-std::numeric_limits<double>::infinity()).passed() ||
        apply(std::numeric_limits<double>::max()).passed()) return 5;

    // This operator must not change the original DecaSigma COREAO audit.
    const auto trace = swe::deca_sigma::run(-9.0);
    if (!swe::coreao::inspect(trace).passed() ||
        swe::coreao::rf_gain::apply(1.0).input_watts != 1.0) return 6;

    const auto normal = apply(12345.6789);
    if (!normal.passed() ||
        !near(normal.output_eirp_watts,
              12345.6789 * std::pow(10.0, 0.9))) return 7;

    if (one.passed(-1.0) ||
        one.passed(std::numeric_limits<double>::quiet_NaN())) return 8;

    std::cout << "PASS COREAO RF: input W -> +9 dBi -> EIRP W, "
                 "reverse witness and invalid-input guards\n";
    return 0;
}
