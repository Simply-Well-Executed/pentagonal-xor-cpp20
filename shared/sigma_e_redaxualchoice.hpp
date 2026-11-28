/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
// SIGMA-E-REDAXUALCHOICE-PR34-006
// EXPLORATORY Q62 sum of equivalent ratios, bounded k=-3..+3.
// No includes, STL, floating point, heap, or compiler extensions.
// Requires sigma_form_area_q62.hpp, right_triangle_f123_original.hpp,
// f123_eighth_fold_experiment.hpp in that order from the caller.
// F1/F2/F3 and eighth-fold E are source witnesses, NOT mutable inputs.
#ifndef SWE_SIGMA_E_REDAXUALCHOICE_HPP
#define SWE_SIGMA_E_REDAXUALCHOICE_HPP
#ifndef SWE_F123_EIGHTH_FOLD_EXPERIMENT_HPP
#error "Include f123_eighth_fold_experiment.hpp before sigma_e_redaxualchoice.hpp"
#endif
namespace swe::sigma_e_redaxualchoice {
using u64 = unsigned long long;
namespace area = swe::sigma_form_area;
namespace fold = swe::f123_eighth_fold_experiment;
namespace original = swe::right_triangle_f123_original;
inline constexpr const char* experiment_id =
    "SIGMA-E-REDAXUALCHOICE-PR34-006";
inline constexpr u64 Q = 1ULL << 62u;
// This is finite source E = F1*F2*F3 = 3/128 in the frozen sample.
inline constexpr u64 expected_e = 3ULL * Q / 128ULL;
inline constexpr u64 expected_term = 3ULL * Q / 256ULL;
inline constexpr u64 expected_sigma = 21ULL * (Q / 256ULL);

struct EquivalentRatio {
    int index;
    u64 numerator_q62;
    u64 denominator_numerator;
    u64 denominator_denominator;
    u64 result_q62;
    bool valid;
};
// Exact rational representation of R_k(E)=2^k E / 2^(k+1).
// Denominator is represented as denominator_numerator /
// denominator_denominator to handle k<0 without floating-point.
// The operation is bounded and fail-closed; it does not approximate
// infinity or treat Q62 as a floating point infinity sentinel.
[[nodiscard]] constexpr EquivalentRatio term(u64 E, int k) noexcept {
    if (k < -3 || k > 3 || E > (~0ULL >> 3u) ||
        E == 0ULL || (E & 7ULL) != 0ULL)
        return {k,0ULL,0ULL,0ULL,0ULL,false};
    const unsigned shift=static_cast<unsigned>(k<0?-k:k);
    const u64 factor=1ULL << shift;
    const u64 numerator=k>=0 ? E*factor : E/factor;
    const u64 den_numerator=k>=0 ? 2ULL*factor : 2ULL;
    const u64 den_denominator=k>=0 ? 1ULL : factor;
    const u64 expanded=numerator*den_denominator;
    // E <= UINT64_MAX/8 guarantees these integer products fit.
    const bool exactly_divisible=expanded % den_numerator == 0ULL;
    const u64 result=expanded / den_numerator;
    const bool equivalent=exactly_divisible && result==E/2ULL;
    return {k,numerator,den_numerator,den_denominator,result,
            equivalent};
}
struct SigmaE {
    u64 source_e_q62;
    EquivalentRatio minus3,minus2,minus1,zero,plus1,plus2,plus3;
    u64 sigma_q62;
    bool source_verified;
    bool all_terms_equivalent;
    bool valid;
    bool infinite_sum_converges;
    bool geometric_tip_verified;
};
// The seven-term finite sum is exactly 7*(E/2), not a limit.
// The E input must be derived from the earlier frozen F1/F2/F3
// cross-product and denominator-eight operation.
[[nodiscard]] constexpr SigmaE calculate(
    fold::EighthFoldWitness source) noexcept {
    const u64 E=source.final_egress_q62;
    const bool source_ok=source.valid &&
        source.original_reference_unchanged &&
        source.fraction_equivalent &&
        source.f1_q62==original::F1_Q62 &&
        source.f2_q62==original::F2_Q62 &&
        source.frozen_f3_q62==original::F3_Q62 &&
        source.gap.valid &&
        source.gap.norm_q62==2ULL*original::F3_Q62 &&
        E==expected_e && !source.geometric_tip_verified;
    const EquivalentRatio m3=term(E,-3);
    const EquivalentRatio m2=term(E,-2);
    const EquivalentRatio m1=term(E,-1);
    const EquivalentRatio z=term(E,0);
    const EquivalentRatio p1=term(E,1);
    const EquivalentRatio p2=term(E,2);
    const EquivalentRatio p3=term(E,3);
    const bool equal=m3.valid && m2.valid && m1.valid &&
                     z.valid && p1.valid && p2.valid && p3.valid &&
                     m3.result_q62==m2.result_q62 &&
                     m2.result_q62==m1.result_q62 &&
                     m1.result_q62==z.result_q62 &&
                     z.result_q62==p1.result_q62 &&
                     p1.result_q62==p2.result_q62 &&
                     p2.result_q62==p3.result_q62 &&
                     p3.result_q62==expected_term;
    // Bounded E and fixed 7 terms guarantee no unsigned overflow.
    const u64 sum=equal ? m3.result_q62+m2.result_q62+
        m1.result_q62+z.result_q62+p1.result_q62+
        p2.result_q62+p3.result_q62 : 0ULL;
    const bool verified=source_ok && equal && sum==expected_sigma;
    return {E,m3,m2,m1,z,p1,p2,p3,sum,
            source_ok,equal,verified,false,false};
}
} // namespace swe::sigma_e_redaxualchoice
#endif // SWE_SIGMA_E_REDAXUALCHOICE_HPP
