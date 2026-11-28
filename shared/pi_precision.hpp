/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
// Copyright (c) 2026 Paul Allen Cheek
// Canonical C++20 OUTPUT precision contract, derived from the owner's π digit string.
// This is a display-formatting policy, not a change to computational precision.
#ifndef SWE_PI_PRECISION_HPP
#define SWE_PI_PRECISION_HPP

#include <iomanip>
#include <limits>
#include <ostream>
#include <string_view>

namespace swe::pi_precision {

inline constexpr std::string_view pi_digits = "314159265358979323";
inline constexpr int reserved_positions = 2; // owner's 1 whole + 1 decimal position
inline constexpr int display_digits =
    static_cast<int>(pi_digits.size()) - reserved_positions;

// Explicit construction prevents inconsistency from scattered literal n.
static_assert(pi_digits.size() == 18u);
static_assert(display_digits == 16);
static_assert(display_digits > 0);

// Apply to floating-point diagnostics emitted through C++20 iostreams.
// With defaultfloat: n significant digits; with fixed: n fractional digits.
// This does not alter binary64 calculations or the exact decimal strings
// and big-integer arithmetic used by GammaGamma.
inline std::ostream& configure(std::ostream& output) {
    output << std::setprecision(display_digits);
    return output;
}

// 17 digits may be needed to round-trip arbitrary binary64 values.
// Keep binary-exact audit records separate from presentation strings.
inline constexpr bool guarantees_double_round_trip =
    display_digits >= std::numeric_limits<double>::max_digits10;
static_assert(!guarantees_double_round_trip);
} // namespace swe::pi_precision

#endif // SWE_PI_PRECISION_HPP
