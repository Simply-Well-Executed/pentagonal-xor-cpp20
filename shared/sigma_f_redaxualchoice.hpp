/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
// SIGMA-F-REDAXUALCHOICE-PR34-007
// Additive carbon-copy expansion of the user's complete F1/F2/F3 CORE.
// Preserve original right_triangle_f123_original.hpp and SigmaE source.
// Include-free, namespace-isolated C++20. Require earlier source headers
// sigma_form_area_q62.hpp and right_triangle_f123_original.hpp first.
// On compilers with __uint128_t, evaluate the exact expression supplied.
// On MSVC, use the already-established exact unsigned 128-bit limb
// multiplication, avoiding undefined overflow and platform extensions.
#ifndef SWE_SIGMA_F_REDAXUALCHOICE_HPP
#define SWE_SIGMA_F_REDAXUALCHOICE_HPP
#ifndef SWE_SIGMA_FORM_AREA_Q62_HPP
#error "Include sigma_form_area_q62.hpp first"
#endif
#ifndef SWE_RIGHT_TRIANGLE_F123_ORIGINAL_HPP
#error "Include right_triangle_f123_original.hpp first"
#endif
namespace swe::sigma_f_redaxualchoice {
using u64 = unsigned long long;
namespace area = swe::sigma_form_area;
namespace original = swe::right_triangle_f123_original;
#if defined(__SIZEOF_INT128__) && !defined(_MSC_VER)
using u128 = __uint128_t;
#endif
inline constexpr const char* experiment_id =
    "SIGMA-F-REDAXUALCHOICE-PR34-007";

// =============================================
// CORE: carbon-copy of original right triangle
// =============================================
static constexpr u64 Q = 1ULL << 62u;

// Three perpendicular edges
static constexpr u64 X = Q / 2ULL;
static constexpr u64 Y = Q / 2ULL;
static constexpr u64 Z = Q;

// Oblique face double-area = 0.75
static constexpr u64 OBLIQUE = 3ULL * Q / 4ULL;

// Four independent triangular faces.
// Preserve the original __uint128_t Q62 expressions where supported.
#if defined(__SIZEOF_INT128__) && !defined(_MSC_VER)
static constexpr u64 BASE = (u64)(
    ((u128)X * Y) >> 63u
);
static constexpr u64 FACE_XZ = (u64)(
    ((u128)X * Z) >> 63u
);
static constexpr u64 FACE_YZ = (u64)(
    ((u128)Y * Z) >> 63u
);
#else
// Exact native MSVC substitute, no headers or STL.
static constexpr u64 BASE = area::shift63(area::multiply(X,Y));
static constexpr u64 FACE_XZ = area::shift63(area::multiply(X,Z));
static constexpr u64 FACE_YZ = area::shift63(area::multiply(Y,Z));
#endif
static constexpr u64 FACE_OBLIQUE = OBLIQUE / 2ULL;

// Original unchanged F witnesses
static constexpr u64 F1 = FACE_XZ;
static constexpr u64 F2 = FACE_YZ;
static constexpr u64 F3 = FACE_OBLIQUE;
static constexpr u64 TOTAL = BASE + F1 + F2 + F3;
static_assert(F1 == 1152921504606846976ULL);
static_assert(F2 == 1152921504606846976ULL);
static_assert(F3 == 1729382256910270464ULL);
static_assert(TOTAL == 4611686018427387904ULL);
static_assert(TOTAL == Q);

// Independent frozen-source equality (no mutation or aliasing).
static_assert(X == original::x && Y == original::y &&
              Z == original::z);
static_assert(OBLIQUE == original::oblique_double_area);
static_assert(BASE == original::BASE_Q62);
static_assert(F1 == original::F1_Q62);
static_assert(F2 == original::F2_Q62);
static_assert(F3 == original::F3_Q62);
static_assert(TOTAL == original::TOTAL_Q62);

// =============================================
// SIGMA: three lateral faces
// =============================================
static constexpr u64 SIGMA_F123 = F1 + F2 + F3;
static_assert(SIGMA_F123 == 4035225266123964416ULL);
static_assert(SIGMA_F123 == original::LATERAL_Q62);

// =============================================
// EXPLORATORY REDAXUALCHOICE
// R[k](F) = (2^k*F)/(2^(k+1)) = F/2
// k constrained to [-3,+3] and F even/divisible by eight.
// The infinite sum does NOT converge for finite nonzero F.
// =============================================
template<int K, u64 F_VALUE>
[[nodiscard]] constexpr u64 REDAXUALCHOICE() noexcept {
    static_assert(K >= -3 && K <= 3);
    static_assert(F_VALUE <= Q);
    static_assert(F_VALUE % 8ULL == 0ULL);

#if defined(__SIZEOF_INT128__) && !defined(_MSC_VER)
    // Faithful wide-integer expression with exact Q62 scaling.
    constexpr u128 LAMBDA = []() constexpr {
        if constexpr (K >= 0)
            return (u128)Q << K;
        else
            return (u128)Q >> (-K);
    }();
    constexpr u128 NUMERATOR =
        ((u128)F_VALUE * LAMBDA) >> 62u;
    constexpr u128 DENOMINATOR = 2u * LAMBDA;
    constexpr u128 SCALED = NUMERATOR << 62u;
    static_assert(DENOMINATOR != 0u);
    static_assert(SCALED % DENOMINATOR == 0u);
    return (u64)(SCALED / DENOMINATOR);
#else
    // Native MSVC arithmetic: preserve the same fraction exactly.
    if constexpr (K >= 0) {
        constexpr unsigned SHIFT = static_cast<unsigned>(K+1);
        constexpr area::Wide PRODUCT =
            area::multiply(F_VALUE,1ULL << K);
        constexpr u64 DIVISOR = 1ULL << SHIFT;
        static_assert((PRODUCT.low & (DIVISOR-1ULL)) == 0ULL);
        return (PRODUCT.high << (64u-SHIFT)) |
               (PRODUCT.low >> SHIFT);
    } else {
        constexpr u64 FACTOR = 1ULL << (-K);
        // (F / FACTOR) / (2 / FACTOR) = F/2;
        // the denominator is rational when FACTOR > 2.
        static_assert((F_VALUE % FACTOR) == 0ULL);
        return ((F_VALUE / FACTOR) * FACTOR) / 2ULL;
    }
#endif
}

// =============================================
// Independent expansion around zero
// =============================================
static constexpr u64 F = SIGMA_F123;

static constexpr u64 R_NEG3 = REDAXUALCHOICE<-3,F>();
static constexpr u64 R_NEG2 = REDAXUALCHOICE<-2,F>();
static constexpr u64 R_NEG1 = REDAXUALCHOICE<-1,F>();
static constexpr u64 R_ZERO = REDAXUALCHOICE< 0,F>();
static constexpr u64 R_POS1 = REDAXUALCHOICE< 1,F>();
static constexpr u64 R_POS2 = REDAXUALCHOICE< 2,F>();
static constexpr u64 R_POS3 = REDAXUALCHOICE< 3,F>();

static_assert(R_NEG3 == F / 2ULL);
static_assert(R_NEG2 == F / 2ULL);
static_assert(R_NEG1 == F / 2ULL);
static_assert(R_ZERO == F / 2ULL);
static_assert(R_POS1 == F / 2ULL);
static_assert(R_POS2 == F / 2ULL);
static_assert(R_POS3 == F / 2ULL);

// =============================================
// SIGMA F REDAXUALCHOICE: sum k=-3 to +3
// =============================================
static constexpr u64 SIGMA_REDAXUALCHOICE =
    R_NEG3 + R_NEG2 + R_NEG1 + R_ZERO +
    R_POS1 + R_POS2 + R_POS3;

static_assert(SIGMA_REDAXUALCHOICE == 7ULL*(F/2ULL));
static_assert(SIGMA_REDAXUALCHOICE == 14123288431433875456ULL);

// Sigma sum = 3.06250000000000000 (not pyramid surface area).
// Preserve the original area and dimensional semantics.
static_assert(BASE + SIGMA_F123 == TOTAL);
static_assert(area::decimal17(
    {SIGMA_REDAXUALCHOICE,true}) == 306250000000000000ULL);
} // namespace swe::sigma_f_redaxualchoice
#endif // SWE_SIGMA_F_REDAXUALCHOICE_HPP
