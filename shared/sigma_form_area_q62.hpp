// SPDX-License-Identifier: 0BSD
// SIGMA-AREA-BCD-AIRGAP-001 | independent projected-area witnesses.
// No headers. No STL. Native ISO C++20-compatible integer implementation.
// __uint128_t cross-check is optional because MSVC does not provide it.
// All inputs are dimensionless Q62 coordinates in [0, 1]; they are NOT
// measurements of the supplied photographs.
#ifndef SWE_SIGMA_FORM_AREA_Q62_HPP
#define SWE_SIGMA_FORM_AREA_Q62_HPP
namespace swe::sigma_form_area {
using u64 = unsigned long long;
using s64 = long long;
inline constexpr u64 unit = 1ULL << 62u;
inline constexpr u64 scale17 = 100000000000000000ULL;
static_assert(sizeof(u64) == 8 && sizeof(s64) == 8);

struct Q62Area {
    u64 value;
    bool valid;
};
struct Point {
    u64 x, y;
};
struct WidthSample {
    u64 position, width;
};
struct Wide {
    u64 high, low;
};
struct Ratio {
    u64 numerator, denominator;
    bool valid;
};
struct BcdAirgap {
    Q62Area airgap_a;
    Q62Area sigma_b, sigma_c, sigma_d;
    bool inputs_consistent;
    bool physical_area_verified;
};

// Full 64 x 64 -> 128 multiplication, independent of compiler extensions.
[[nodiscard]] constexpr Wide multiply(u64 a, u64 b) noexcept {
    constexpr u64 mask = 0xFFFFFFFFULL;
    const u64 a0 = a & mask, a1 = a >> 32u;
    const u64 b0 = b & mask, b1 = b >> 32u;
    const u64 p00 = a0 * b0;
    const u64 p01 = a0 * b1;
    const u64 p10 = a1 * b0;
    const u64 p11 = a1 * b1;
    const u64 middle = (p00 >> 32u) + (p01 & mask) + (p10 & mask);
    const u64 high = p11 + (p01 >> 32u) + (p10 >> 32u) +
                     (middle >> 32u);
    const u64 low = ((middle & mask) << 32u) | (p00 & mask);
    return {high, low};
}
[[nodiscard]] constexpr bool wide_less(Wide a, Wide b) noexcept {
    return a.high < b.high ||
           (a.high == b.high && a.low < b.low);
}
// Preconditions: unsigned sum fits in 128 bits, or a >= b for subtraction.
[[nodiscard]] constexpr Wide wide_add(Wide a, Wide b) noexcept {
    const u64 lo = a.low + b.low;
    return {a.high + b.high + static_cast<u64>(lo < a.low), lo};
}
[[nodiscard]] constexpr Wide wide_subtract(Wide a, Wide b) noexcept {
    const u64 lo = a.low - b.low;
    return {a.high - b.high - static_cast<u64>(a.low < b.low), lo};
}
[[nodiscard]] constexpr u64 shift62(Wide product) noexcept {
    return (product.high << 2u) | (product.low >> 62u);
}
[[nodiscard]] constexpr u64 shift63(Wide product) noexcept {
    return (product.high << 1u) | (product.low >> 63u);
}
[[nodiscard]] constexpr u64 magnitude(s64 value) noexcept {
    return static_cast<u64>(value < 0 ? -value : value);
}
[[nodiscard]] constexpr bool point_valid(Point p) noexcept {
    return p.x <= unit && p.y <= unit;
}
[[nodiscard]] constexpr Q62Area invalid_area() noexcept {
    return {0ULL, false};
}

// AIRGAP A: projected triangular face/silhouette of a pyramid,
// area = base * altitude / 2. A is an independent reference only;
// it is NOT the total 3-D surface area of the pyramid.
[[nodiscard]] constexpr Q62Area airgap_a(
    u64 base_q62, u64 altitude_q62) noexcept {
    if (base_q62 == 0ULL || altitude_q62 == 0ULL ||
        base_q62 > unit || altitude_q62 > unit)
        return invalid_area();
    return {shift63(multiply(base_q62, altitude_q62)), true};
}

// SIGMA B: area enclosed by quadratic Bezier P0->P2 and closing chord,
// area = abs(det(P1-P0, P2-P0)) / 3.
// Unlike sampling the curve, this is an exact analytic formula.
// Each coordinate is represented as a Q62 fixed-point integer.
[[nodiscard]] constexpr Q62Area sigma_b(
    Point p0, Point p1, Point p2) noexcept {
    if (!point_valid(p0) || !point_valid(p1) || !point_valid(p2))
        return invalid_area();
    const s64 dx1 = static_cast<s64>(p1.x)-static_cast<s64>(p0.x);
    const s64 dy1 = static_cast<s64>(p1.y)-static_cast<s64>(p0.y);
    const s64 dx2 = static_cast<s64>(p2.x)-static_cast<s64>(p0.x);
    const s64 dy2 = static_cast<s64>(p2.y)-static_cast<s64>(p0.y);
    const Wide p = multiply(magnitude(dx1), magnitude(dy2));
    const Wide q = multiply(magnitude(dy1), magnitude(dx2));
    const bool p_negative = (dx1 < 0) != (dy2 < 0);
    const bool q_negative = (dy1 < 0) != (dx2 < 0);
    Wide determinant{};
    if (p_negative != q_negative) {
        determinant = wide_add(p,q);
    } else {
        determinant = wide_less(p,q) ?
            wide_subtract(q,p) : wide_subtract(p,q);
    }
    // floor(det/(3*2^62)) == floor(floor(det/2^62)/3).
    return {shift62(determinant) / 3ULL, true};
}

// SIGMA C: constant-width projected pleat strip.
// Its wavy centerline affects boundary shape but not this 2-D area.
// Unfolded 3-D fabric surface area would require additional geometry.
[[nodiscard]] constexpr Q62Area sigma_c(
    u64 width_q62, u64 vertical_span_q62) noexcept {
    if (width_q62 > unit || vertical_span_q62 > unit)
        return invalid_area();
    return {shift62(multiply(width_q62, vertical_span_q62)), true};
}

// SIGMA D: projected billowing strip with a piecewise-linear width.
// Every sample has position and nonnegative width in Q62.
// Positions must increase and remain in [0,1]. For intervals,
// area = sum((width[i]+width[i+1])*(s[i+1]-s[i])/2).
// The full 128-bit numerator is summed BEFORE the final division,
// avoiding truncation at intermediate samples.
template<unsigned N>
[[nodiscard]] constexpr Q62Area sigma_d(
    const WidthSample (&samples)[N]) noexcept {
    if (N < 2 || samples[0].position > unit ||
        samples[0].width > unit) return invalid_area();
    Wide numerator{0ULL,0ULL};
    for (unsigned i=1; i<N; ++i) {
        if (samples[i].position <= samples[i-1].position ||
            samples[i].position > unit || samples[i].width > unit)
            return invalid_area();
        const u64 step = samples[i].position-samples[i-1].position;
        const u64 widths = samples[i-1].width+samples[i].width;
        numerator = wide_add(numerator,multiply(widths,step));
    }
    // The total span <= 1 and every width <= 1, hence numerator
    // <= 2*unit^2: all intermediates fit within unsigned 128 bits.
    return {shift63(numerator), true};
}

// Preserve all 3 SIGMAs and the separate AIRGAP. Validation is NOT
// majority voting and does NOT claim area equality or calibration.
[[nodiscard]] constexpr BcdAirgap assemble(
    Q62Area a, Q62Area b, Q62Area c, Q62Area d) noexcept {
    const bool consistent = a.valid && a.value != 0ULL &&
                            b.valid && c.valid && d.valid;
    return {a,b,c,d,consistent,false};
}
[[nodiscard]] constexpr Ratio relative_to_airgap(
    Q62Area sigma, Q62Area reference) noexcept {
    if (!sigma.valid || !reference.valid || reference.value == 0ULL)
        return {0ULL,0ULL,false};
    // Exact rational ratio, not a guessed physical-world area ratio.
    return {sigma.value,reference.value,true};
}
// Return first 17 decimal fractional places as an integer in the
// same output convention as the canonical Euler/Pi/I Q62 examples.
[[nodiscard]] constexpr u64 decimal17(Q62Area area) noexcept {
    return area.valid ? shift62(multiply(area.value,scale17)) : 0ULL;
}

#if defined(__SIZEOF_INT128__) && !defined(_MSC_VER)
// Optional direct oracle in compilers supporting the extension,
// matching the user's precise __uint128_t multiplication style.
[[nodiscard]] constexpr bool wide_matches_native_u128(
    u64 a, u64 b) noexcept {
    const __uint128_t native =
        static_cast<__uint128_t>(a) * static_cast<__uint128_t>(b);
    const Wide limbs = multiply(a,b);
    return limbs.high == static_cast<u64>(native >> 64u) &&
           limbs.low == static_cast<u64>(native);
}
#endif
} // namespace swe::sigma_form_area
#endif // SWE_SIGMA_FORM_AREA_Q62_HPP
