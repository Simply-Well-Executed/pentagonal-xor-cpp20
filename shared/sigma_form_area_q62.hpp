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


// SIGMA-FORM-FOLD-PR34-002: additional independently named exploratory
// sequence. Do not mutate the historical SIGMA-AREA-BCD-AIRGAP-001.
// Distinct SIGMA A (square base) != AIRGAP A (triangular reference).
[[nodiscard]] constexpr Q62Area sigma_a_square_base(u64 side) noexcept {
    if (side == 0ULL || side > unit) return invalid_area();
    return {shift62(multiply(side,side)), true};
}
enum class Lane : unsigned char {
    SigmaA, SigmaB, SigmaC, SigmaD
};
struct OrderedPath {
    Lane first, second, third;
    Q62Area formal_sum;
    Q62Area airgap_reference;
    bool valid;
};
[[nodiscard]] constexpr OrderedPath ordered_path(
    Lane first, Lane second, Lane third,
    Q62Area x, Q62Area y, Q62Area z, Q62Area a) noexcept {
    const bool distinct = first != second && first != third &&
                          second != third;
    const bool valid = distinct && x.valid && y.valid && z.valid &&
                       a.valid && a.value != 0ULL &&
                       x.value <= unit && y.value <= unit &&
                       z.value <= unit && a.value <= unit;
    // Inputs <= unit; 3*unit is less than 2^64. This is a FORMAL SUM,
    // NOT a measure of non-overlapping fabric or geometric union.
    return {first,second,third,
            {valid ? x.value+y.value+z.value : 0ULL,valid},a,valid};
}
[[nodiscard]] constexpr OrderedPath path_abc(
    Q62Area sigma_a, Q62Area sigma_b, Q62Area sigma_c,
    Q62Area airgap_a_ref) noexcept {
    return ordered_path(Lane::SigmaA,Lane::SigmaB,Lane::SigmaC,
                        sigma_a,sigma_b,sigma_c,airgap_a_ref);
}
[[nodiscard]] constexpr OrderedPath path_cdb(
    Q62Area sigma_c, Q62Area sigma_d, Q62Area sigma_b,
    Q62Area airgap_a_ref) noexcept {
    return ordered_path(Lane::SigmaC,Lane::SigmaD,Lane::SigmaB,
                        sigma_c,sigma_d,sigma_b,airgap_a_ref);
}
[[nodiscard]] constexpr OrderedPath path_bcd(
    Q62Area sigma_b, Q62Area sigma_c, Q62Area sigma_d,
    Q62Area airgap_a_ref) noexcept {
    return ordered_path(Lane::SigmaB,Lane::SigmaC,Lane::SigmaD,
                        sigma_b,sigma_c,sigma_d,airgap_a_ref);
}
// ^&^ : conjunction, not identity, equality, voting or record merging.
[[nodiscard]] constexpr bool paired_and(
    OrderedPath abc, OrderedPath cdb) noexcept {
    return abc.valid && cdb.valid &&
           abc.first == Lane::SigmaA &&
           abc.second == Lane::SigmaB &&
           abc.third == Lane::SigmaC &&
           cdb.first == Lane::SigmaC &&
           cdb.second == Lane::SigmaD &&
           cdb.third == Lane::SigmaB &&
           abc.airgap_reference.value == cdb.airgap_reference.value;
}
// Exactly 90 degrees in a complex phase plane: i*(real+i*imag).
// This is orientation bookkeeping, not material twist radius or tension.
struct ImaginaryQuarterTurn {
    int real, imag;
};
[[nodiscard]] constexpr ImaginaryQuarterTurn multiply_by_i(
    ImaginaryQuarterTurn p) noexcept {
    // Only four valid unit quadratures are admitted.
    if ((p.real != 0 && p.imag != 0) ||
        (p.real == 0 && p.imag == 0) ||
        p.real < -1 || p.real > 1 ||
        p.imag < -1 || p.imag > 1) return {0,0};
    return {-p.imag,p.real};
}
enum class FoldStep : unsigned char {
    SeedBCD,
    FoldBIntoC,
    FoldDIntoC,
    PushCIntoD,
    TwirlToRope,
    LoopIntoNeedle,
    InsertIntoAirgapA,
    TwistA90,
    SpindleWithC,
    Invalid
};
struct FoldTrace {
    OrderedPath source;
    FoldStep stage;
    ImaginaryQuarterTurn phase;
    unsigned quarter_turns;
    bool valid;
    bool physical_shape_verified;
};
[[nodiscard]] constexpr FoldTrace start_fold(
    OrderedPath bcd) noexcept {
    const bool ok = bcd.valid &&
                    bcd.first == Lane::SigmaB &&
                    bcd.second == Lane::SigmaC &&
                    bcd.third == Lane::SigmaD;
    return {bcd,ok?FoldStep::SeedBCD:FoldStep::Invalid,
            {1,0},0u,ok,false};
}
[[nodiscard]] constexpr FoldTrace advance(
    FoldTrace t, FoldStep from, FoldStep to) noexcept {
    if (!t.valid || t.stage != from) {
        t.valid=false;
        t.stage=FoldStep::Invalid;
        return t;
    }
    t.stage=to;
    return t;
}
[[nodiscard]] constexpr FoldTrace fold_b_into_c(FoldTrace t) noexcept {
    return advance(t,FoldStep::SeedBCD,FoldStep::FoldBIntoC);
}
[[nodiscard]] constexpr FoldTrace fold_d_into_c(FoldTrace t) noexcept {
    return advance(t,FoldStep::FoldBIntoC,FoldStep::FoldDIntoC);
}
[[nodiscard]] constexpr FoldTrace push_c_into_d(FoldTrace t) noexcept {
    return advance(t,FoldStep::FoldDIntoC,FoldStep::PushCIntoD);
}
[[nodiscard]] constexpr FoldTrace twirl_to_rope(
    FoldTrace t, unsigned quarter_turns) noexcept {
    t=advance(t,FoldStep::PushCIntoD,FoldStep::TwirlToRope);
    if (!t.valid) return t;
    if (quarter_turns == 0u || quarter_turns > 32u) {
        t.valid=false; t.stage=FoldStep::Invalid;
        return t;
    }
    for (unsigned i=0u;i<quarter_turns;++i)
        t.phase=multiply_by_i(t.phase);
    t.quarter_turns=quarter_turns;
    return t;
}
[[nodiscard]] constexpr FoldTrace loop_into_needle(FoldTrace t) noexcept {
    // A loop/needle designation needs independent 3-D evidence to
    // become a physical curvature or topology claim.
    return advance(t,FoldStep::TwirlToRope,FoldStep::LoopIntoNeedle);
}
[[nodiscard]] constexpr FoldTrace insert_into_airgap_a(
    FoldTrace t) noexcept {
    return advance(t,FoldStep::LoopIntoNeedle,
                   FoldStep::InsertIntoAirgapA);
}
[[nodiscard]] constexpr FoldTrace twist_a_90(FoldTrace t) noexcept {
    t=advance(t,FoldStep::InsertIntoAirgapA,FoldStep::TwistA90);
    if (t.valid) {
        t.phase=multiply_by_i(t.phase);
        ++t.quarter_turns;
    }
    return t;
}
[[nodiscard]] constexpr FoldTrace spindle_with_c(FoldTrace t) noexcept {
    t=advance(t,FoldStep::TwistA90,FoldStep::SpindleWithC);
    if (!t.valid) return t;
    // Four exact imaginary quarter-turns close the phase:
    // i^4 = 1. This is NOT infinite physical winding.
    for (unsigned i=0u;i<4u;++i)
        t.phase=multiply_by_i(t.phase);
    t.quarter_turns+=4u;
    return t;
}
// Exact squared-length slant witness:
// 4*slant^2 = 4*vertical_height^2 + side^2.
// This avoids square roots and avoids dividing odd Q62 coordinates by 2.
[[nodiscard]] constexpr Wide times_four(Wide x) noexcept {
    return {(x.high << 2u) | (x.low >> 62u),x.low << 2u};
}
[[nodiscard]] constexpr bool equal_wide(Wide a,Wide b) noexcept {
    return a.high == b.high && a.low == b.low;
}
struct SquarePyramid {
    Q62Area base, one_triangle, lateral, total;
    bool right_square_geometry_verified;
};
[[nodiscard]] constexpr SquarePyramid right_square_pyramid(
    u64 side, u64 height, u64 slant) noexcept {
    if (side == 0ULL || height == 0ULL || slant == 0ULL ||
        side > unit || height > unit || slant > unit)
        return {{0ULL,false},{0ULL,false},{0ULL,false},
                {0ULL,false},false};
    const Wide left=times_four(multiply(slant,slant));
    const Wide right=wide_add(times_four(multiply(height,height)),
                              multiply(side,side));
    if (!equal_wide(left,right))
        return {{0ULL,false},{0ULL,false},{0ULL,false},
                {0ULL,false},false};
    const Q62Area base=sigma_a_square_base(side);
    const Q62Area face{shift63(multiply(side,slant)),true};
    const Q62Area lateral{4ULL*face.value,true};
    const Q62Area total{base.value+lateral.value,true};
    return {base,face,lateral,total,true};
}

// RIGHT TRIANGULAR PYRAMID (trirectangular tetrahedron):
// Three edges from the right-angle vertex are mutually perpendicular.
// Base vertices: (0,0,0), (x,0,0), (0,y,0); apex: (0,0,z).
// The apex stands directly over the RIGHT-ANGLE VERTEX, not the centroid.
// Four triangular faces, unlike the five faces of a square pyramid.
//
// Let xy=x*y, xz=x*z, yz=y*z. The oblique face has
// double-area q = sqrt(xy^2 + xz^2 + yz^2).
// All inputs and xy/xz/yz/q are Q62 quantities. We REQUIRE the exact
// squared identity in the quantized domain, avoiding sqrt, floats,
// library includes, and false claims of exactness after rounding.
struct TrianglePyramid {
    Q62Area base, face_xz, face_yz, face_oblique;
    Q62Area lateral, total;
    bool right_triangle_geometry_verified;
};
[[nodiscard]] constexpr TrianglePyramid right_triangle_pyramid(
    u64 x_leg, u64 y_leg, u64 vertical_height,
    u64 oblique_double_area) noexcept {
    const TrianglePyramid invalid{
        {0ULL,false},{0ULL,false},{0ULL,false},{0ULL,false},
        {0ULL,false},{0ULL,false},false};
    if (x_leg==0ULL || y_leg==0ULL || vertical_height==0ULL ||
        oblique_double_area==0ULL ||
        x_leg>unit || y_leg>unit || vertical_height>unit ||
        oblique_double_area>2ULL*unit)
        return invalid;
    const u64 xy=shift62(multiply(x_leg,y_leg));
    const u64 xz=shift62(multiply(x_leg,vertical_height));
    const u64 yz=shift62(multiply(y_leg,vertical_height));
    const Wide expected=wide_add(
        wide_add(multiply(xy,xy),multiply(xz,xz)),multiply(yz,yz));
    if (!equal_wide(multiply(oblique_double_area,oblique_double_area),
                    expected))
        return invalid;
    // Quantize each face individually, then sum. Input Q62 scalars
    // are dimensionless, so outputs are normalized square units.
    const Q62Area base{xy/2ULL,true};
    const Q62Area face_xz{xz/2ULL,true};
    const Q62Area face_yz{yz/2ULL,true};
    const Q62Area face_oblique{oblique_double_area/2ULL,true};
    const Q62Area lateral{face_xz.value+face_yz.value+
                          face_oblique.value,true};
    const Q62Area total{base.value+lateral.value,true};
    return {base,face_xz,face_yz,face_oblique,lateral,total,true};
}

struct PyramidClosure {
    FoldTrace trace;
    SquarePyramid geometry;
    bool formal_areas_equal;
    bool folding_proves_pyramid;
};
[[nodiscard]] constexpr PyramidClosure evaluate_closure(
    FoldTrace sequence, SquarePyramid geometry) noexcept {
    const bool matching=sequence.valid &&
                        sequence.stage == FoldStep::SpindleWithC &&
                        geometry.right_square_geometry_verified &&
                        sequence.source.formal_sum.value ==
                            geometry.total.value;
    // Even coincident numerical area cannot establish shape equivalence.
    return {sequence,geometry,matching,false};
}


// A distinct typed closure for the four-face triangular pyramid.
// Matching a formal sum cannot authenticate a geometric transformation.
struct TrianglePyramidClosure {
    FoldTrace trace;
    TrianglePyramid geometry;
    bool formal_areas_equal;
    bool folding_proves_pyramid;
};
[[nodiscard]] constexpr TrianglePyramidClosure evaluate_triangle_closure(
    FoldTrace sequence, TrianglePyramid geometry) noexcept {
    const bool matching=sequence.valid &&
                        sequence.stage==FoldStep::SpindleWithC &&
                        geometry.right_triangle_geometry_verified &&
                        sequence.source.formal_sum.value==
                            geometry.total.value;
    return {sequence,geometry,matching,false};
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
