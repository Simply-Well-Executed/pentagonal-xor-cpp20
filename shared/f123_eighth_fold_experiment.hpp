/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
// EXPERIMENTAL, additive and read-only. Frozen F1/F2/F3 remain untouched.
// Requires sigma_form_area_q62.hpp and right_triangle_f123_original.hpp
// to be included by the caller BEFORE this include-free header.
// F3-in-the-cross-gap is a LABEL for the checked cross-product witness,
// not scalar multiplication and not new physical pyramid area.
#ifndef SWE_F123_EIGHTH_FOLD_EXPERIMENT_HPP
#define SWE_F123_EIGHTH_FOLD_EXPERIMENT_HPP
namespace swe::f123_eighth_fold_experiment {
using u64 = unsigned long long;
namespace area = swe::sigma_form_area;
namespace original = swe::right_triangle_f123_original;
inline constexpr const char* experiment_id = "F123-EIGHTH-FOLD-PR34-005";
inline constexpr const char* frozen_source_commit =
    "65a86d5ecd474efb41a7694d52b656e43636ba3c";

// Models the requested textual replacement
// R(PB) [old F3] R(PC) as an explicitly checked operator slot.
// The slot delegates to the already-validated rotated CROSS product.
// It cannot replace 'x' by multiplying by F3 as an area scalar.
struct F3CrossSlot {
    area::SignedQ62Vec3 cross_vector;
    u64 norm_q62;
    bool valid;
};
[[nodiscard]] constexpr F3CrossSlot f3_in_cross_slot(
    area::RotatedF3Witness rotated) noexcept {
    const bool ok = rotated.valid &&
                    rotated.orientation_verified &&
                    rotated.area_preserved &&
                    rotated.oblique_face.valid &&
                    rotated.oblique_face.value == original::F3_Q62 &&
                    original::F3_Q62 <= area::unit / 2ULL;
    return {rotated.rotated_cross,
            ok ? 2ULL*original::F3_Q62 : 0ULL,ok};
}

// E = F1 * || R(PB) [F3-cross-slot] R(PC) || * (F2/2)
//   = F1 * ||...|| * (2F2/4)
//   = F1 * ||...|| * (4F2/8)
//   = F1*F2*F3, because ||...||=2*F3.
// The result is a dimensionless normalized SCALAR PRODUCT, not
// a pyramid surface area or a proof of the proposed missing tip.
struct EighthFoldWitness {
    F3CrossSlot gap;
    u64 f1_q62;
    u64 f2_q62;
    u64 frozen_f3_q62;
    u64 f2_over_2_q62;
    u64 twice_f2_over_4_q62;
    u64 four_times_f2_over_8_q62;
    u64 first_multiplication_q62;
    u64 final_egress_q62;
    bool fraction_equivalent;
    bool original_reference_unchanged;
    bool valid;
    bool geometric_tip_verified;
};
[[nodiscard]] constexpr EighthFoldWitness calculate(
    area::RotatedF3Witness rotated) noexcept {
    const F3CrossSlot gap=f3_in_cross_slot(rotated);
    const u64 f1=original::F1_Q62;
    const u64 f2=original::F2_Q62;
    const u64 f3=original::F3_Q62;
    // Current experiment uses exactly the frozen original sample.
    // Guard multiplication by 4 against overflowing unsigned 64.
    const bool bounded=f2<=area::unit/4ULL && f1<=area::unit;
    if (!gap.valid || !bounded)
        return {gap,f1,f2,f3,0ULL,0ULL,0ULL,0ULL,0ULL,
                false,false,false,false};
    const u64 half=f2/2ULL;
    const u64 fourth=(2ULL*f2)/4ULL;
    const u64 eighth=(4ULL*f2)/8ULL;
    const bool fraction=(half==fourth && fourth==eighth);
    const u64 first=area::shift62(area::multiply(f1,gap.norm_q62));
    const u64 egress=area::shift62(area::multiply(first,eighth));
    const bool frozen=f1==area::unit/4ULL &&
                      f2==area::unit/4ULL &&
                      f3==3ULL*area::unit/8ULL;
    const bool validated=gap.valid && fraction && frozen &&
                         gap.norm_q62==2ULL*f3;
    return {gap,f1,f2,f3,half,fourth,eighth,first,egress,
            fraction,frozen,validated,false};
}
} // namespace swe::f123_eighth_fold_experiment
#endif // SWE_F123_EIGHTH_FOLD_EXPERIMENT_HPP
