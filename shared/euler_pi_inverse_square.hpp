// SPDX-License-Identifier: 0BSD
// PR #32: exact 18-digit Euler and pi source witnesses, inverse-square ratios.
// Compile-time only; no non-ISO 128-bit extensions, floating point or runtime state.
#ifndef SWE_EULER_PI_INVERSE_SQUARE_HPP
#define SWE_EULER_PI_INVERSE_SQUARE_HPP
#include "toolong_q62_core.hpp"

namespace swe::inverse_square {
using u64 = unsigned long long;
inline constexpr u64 scale = 100000000000000000ULL; // 17 fractional places, 18 total digits
inline constexpr u64 euler_digits18 = toolong_q62::euler.expected_integer;
inline constexpr u64 pi_digits18 = toolong_q62::pi.expected_integer;

// Independent C++20 fixed-point witness of the first 18 digits of e.
// Twenty levels of the finite factorial-series Horner form yield this prefix.
consteval u64 euler_series_prefix() noexcept {
    u64 value = 0;
    for (unsigned n = 20; n != 0; --n) {
        value = scale + value / n;
    }
    return value;
}

// Exact rational value: (source_digits / 10^17) / radius^2.
// A rational pair preserves every source digit; integer division would not.
struct Ratio {
    u64 numerator;
    u64 denominator;
};

template<unsigned Radius>
consteval Ratio at_radius(u64 source_digits18) noexcept {
    static_assert(Radius >= 1 && Radius <= 13,
                  "Radius 1..13 avoids unsigned-64 denominator overflow");
    return {source_digits18, scale * Radius * Radius};
}

// The shared form for any separately identified, verified 18-digit real scalar.
// The attached third symbol is NOT given an assumed numerical identity.
template<u64 SourceDigits18>
struct IsolatedScalar18 {
    static_assert(SourceDigits18 >= scale && SourceDigits18 < 10 * scale,
                  "Source must preserve 18 integer digits with 17 fractional places");
    inline static constexpr u64 source_digits18 = SourceDigits18;
    inline static constexpr Ratio r1 = at_radius<1>(SourceDigits18);
    inline static constexpr Ratio r2 = at_radius<2>(SourceDigits18);
    inline static constexpr Ratio r3 = at_radius<3>(SourceDigits18);
    static_assert(r1.numerator == SourceDigits18 && r1.denominator == scale);
    static_assert(r2.numerator == r1.numerator &&
                  r2.denominator == 4 * r1.denominator);
    static_assert(r3.numerator == r1.numerator &&
                  r3.denominator == 9 * r1.denominator);
};

using Euler = IsolatedScalar18<euler_digits18>;
using Pi = IsolatedScalar18<pi_digits18>;
inline constexpr Ratio e1 = Euler::r1;
inline constexpr Ratio e2 = Euler::r2;
inline constexpr Ratio e3 = Euler::r3;
inline constexpr Ratio p1 = Pi::r1;
inline constexpr Ratio p2 = Pi::r2;
inline constexpr Ratio p3 = Pi::r3;

static_assert(euler_series_prefix() == euler_digits18, "Euler source-prefix mismatch");
static_assert(euler_digits18 >= scale && euler_digits18 < 10 * scale);
static_assert(pi_digits18 >= scale && pi_digits18 < 10 * scale);
static_assert(e1.numerator == euler_digits18 && e1.denominator == scale);
static_assert(p1.numerator == pi_digits18 && p1.denominator == scale);
static_assert(e2.numerator == e1.numerator && e2.denominator == 4 * e1.denominator);
static_assert(e3.numerator == e1.numerator && e3.denominator == 9 * e1.denominator);
static_assert(p2.numerator == p1.numerator && p2.denominator == 4 * p1.denominator);
static_assert(p3.numerator == p1.numerator && p3.denominator == 9 * p1.denominator);

// EXPLORATORY: replace numeric pi in the user's modified circular-area
// equation with GammaGamma SigmaPi (!pi). This is a *symbolic* substitution,
// not a numerical physical intensity or a new definition of pi.
// Canonical source digits and the original inverse-square witnesses above
// remain unchanged. Do not conflate with HANDI sigmapi audit observations.
namespace sigmapi_exploratory {
enum class State : unsigned char { symbolic_only };
struct SymbolicIntensity {
    const char* expression;
    const char* sigma_pi_input;
    State state;
    bool numerically_defined;
};
static constexpr SymbolicIntensity I{
    "(P * e^(-alpha * omega)) / (SigmaPi * omega * omega)",
    "3.14159265358979323",
    State::symbolic_only,
    false
};
static_assert(I.state == State::symbolic_only);
static_assert(!I.numerically_defined);
static_assert(pi_digits18 == 314159265358979323ULL);
} // namespace sigmapi_exploratory

} // namespace swe::inverse_square
#endif // SWE_EULER_PI_INVERSE_SQUARE_HPP
