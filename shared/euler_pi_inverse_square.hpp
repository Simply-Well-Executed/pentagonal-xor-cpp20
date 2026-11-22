// SPDX-License-Identifier: 0BSD
// PR #32: exact 18-digit Euler and pi source witnesses, inverse-square ratios.
// Compile-time only; no non-ISO 128-bit extensions, floating point or runtime state.
#ifndef SWE_EULER_PI_INVERSE_SQUARE_HPP
#define SWE_EULER_PI_INVERSE_SQUARE_HPP

namespace swe::inverse_square {
using u64 = unsigned long long;
inline constexpr u64 scale = 100000000000000000ULL; // 17 fractional places, 18 total digits
inline constexpr u64 euler_digits18 = 271828182845904523ULL;
inline constexpr u64 pi_digits18 = 314159265358979323ULL;

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

inline constexpr Ratio e1 = at_radius<1>(euler_digits18);
inline constexpr Ratio e2 = at_radius<2>(euler_digits18);
inline constexpr Ratio e3 = at_radius<3>(euler_digits18);
inline constexpr Ratio p1 = at_radius<1>(pi_digits18);
inline constexpr Ratio p2 = at_radius<2>(pi_digits18);
inline constexpr Ratio p3 = at_radius<3>(pi_digits18);

static_assert(euler_series_prefix() == euler_digits18, "Euler source-prefix mismatch");
static_assert(euler_digits18 >= scale && euler_digits18 < 10 * scale);
static_assert(pi_digits18 >= scale && pi_digits18 < 10 * scale);
static_assert(e1.numerator == euler_digits18 && e1.denominator == scale);
static_assert(p1.numerator == pi_digits18 && p1.denominator == scale);
static_assert(e2.numerator == e1.numerator && e2.denominator == 4 * e1.denominator);
static_assert(e3.numerator == e1.numerator && e3.denominator == 9 * e1.denominator);
static_assert(p2.numerator == p1.numerator && p2.denominator == 4 * p1.denominator);
static_assert(p3.numerator == p1.numerator && p3.denominator == 9 * p1.denominator);

} // namespace swe::inverse_square
#endif // SWE_EULER_PI_INVERSE_SQUARE_HPP
