/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
// MINZA-DIRECT-PING-TRIDI-PR34-012 | EXPLORATORY, additive.
// Derive three geometric side witnesses AD, CD and fixed hinge AC.
// MINZA -> exact PING, then cyclic TriDi. No NINZA intermediary.
// Do not replace earlier BiDi or AIRGAP TriDi adapters: this is a
// distinct three-GEOMETRIC-edge interpretation of the symbolic name.
// Requires minza_direct_ping_bidi.hpp and all prior prerequisites.
// ISO C++20, no includes, STL, floating point, heap or extensions.
#ifndef SWE_MINZA_DIRECT_PING_TRIDI_HPP
#define SWE_MINZA_DIRECT_PING_TRIDI_HPP
#ifndef SWE_MINZA_DIRECT_PING_BIDI_HPP
#error "Include minza_direct_ping_bidi.hpp before this header"
#endif
namespace swe::minza_direct_ping_tridi {
using u64=unsigned long long;
using s64=long long;
namespace core=swe::minza_direct_ping_bidi;
namespace tork=swe::xdr_rdx_fold_tork;
inline constexpr const char* experiment_id=
    "MINZA-DIRECT-PING-TRIDI-PR34-012";

// =============================================
// CORE: two folded edges and one fixed hinge.
// Original triangle ADC has right angle at D.
// Each folded squared length is in grid units
// multiplied by DEN^2 = 13^2 = 169.
// =============================================
static constexpr core::DirectFold FOLD=core::FOLD;
static constexpr s64 UX=core::C.x-core::A.x;
static constexpr s64 UY=core::C.y-core::A.y;
static constexpr u64 AC2=static_cast<u64>(UX*UX+UY*UY);
static constexpr u64 DEN2=static_cast<u64>(core::DEN2);
static constexpr u64 AD2=FOLD.ad2;
static constexpr u64 CD2=FOLD.cd2;
static constexpr u64 AD_FOLDED=FOLD.ad_folded;
static constexpr u64 CD_FOLDED=FOLD.cd_folded;
static constexpr u64 AC_FOLDED=AC2*DEN2;
static_assert(core::HINGE_VALID(core::LARGE,core::SMALL));
static_assert(core::PING(FOLD));
static_assert(AC2==1300ULL && DEN2==169ULL);
static_assert(AD2==400ULL && CD2==900ULL);
static_assert(AD_FOLDED==67600ULL);
static_assert(CD_FOLDED==152100ULL);
static_assert(AC_FOLDED==219700ULL);
// Pythagorean equality here applies to actual ADC geometry,
// not to every triangle or an assumed physical paper fold.
static_assert(AD2+CD2==AC2);
static_assert(AD_FOLDED+CD_FOLDED==AC_FOLDED);

// =============================================
// TriDi: THREE labeled geometric witnesses.
// Identity includes the actual edge label,
// original length squared and folded witness.
// The cycle changes order, NOT the source data.
// =============================================
enum class Edge : unsigned { AD=1u, CD=2u, AC=3u };
struct Witness {
    Edge edge;
    u64 original_squared;
    u64 folded_scaled_squared;
};
struct Triple {
    Witness first,second,third;
};
[[nodiscard]] constexpr bool SAME(Witness a,Witness b) noexcept {
    return a.edge==b.edge &&
           a.original_squared==b.original_squared &&
           a.folded_scaled_squared==b.folded_scaled_squared;
}
[[nodiscard]] constexpr bool SAME(Triple a,Triple b) noexcept {
    return SAME(a.first,b.first)&&
           SAME(a.second,b.second)&&
           SAME(a.third,b.third);
}
static constexpr Witness HEAD_AD{Edge::AD,AD2,AD_FOLDED};
static constexpr Witness TAIL_CD{Edge::CD,CD2,CD_FOLDED};
static constexpr Witness HINGE_AC{Edge::AC,AC2,AC_FOLDED};
static constexpr Triple ORIGINAL{HEAD_AD,TAIL_CD,HINGE_AC};
[[nodiscard]] constexpr bool PING_WITNESS(Witness w) noexcept {
    switch(w.edge) {
        case Edge::AD: return SAME(w,HEAD_AD);
        case Edge::CD: return SAME(w,TAIL_CD);
        case Edge::AC: return SAME(w,HINGE_AC);
    }
    return false;
}
[[nodiscard]] constexpr bool PING_SET(Triple t) noexcept {
    const bool unique=t.first.edge!=t.second.edge &&
                      t.first.edge!=t.third.edge &&
                      t.second.edge!=t.third.edge;
    const bool complete=(t.first.edge==Edge::AD ||
                         t.second.edge==Edge::AD ||
                         t.third.edge==Edge::AD) &&
                        (t.first.edge==Edge::CD ||
                         t.second.edge==Edge::CD ||
                         t.third.edge==Edge::CD) &&
                        (t.first.edge==Edge::AC ||
                         t.second.edge==Edge::AC ||
                         t.third.edge==Edge::AC);
    return unique && complete &&
           PING_WITNESS(t.first)&&
           PING_WITNESS(t.second)&&
           PING_WITNESS(t.third)&&
           AD2+CD2==AC2 &&
           AD_FOLDED+CD_FOLDED==AC_FOLDED &&
           core::PING(FOLD);
}
[[nodiscard]] constexpr bool PING_CANONICAL(Triple t) noexcept {
    return PING_SET(t)&&SAME(t,ORIGINAL);
}
[[nodiscard]] constexpr Triple TRIDI(Triple t) noexcept {
    return {t.second,t.third,t.first};
}
[[nodiscard]] constexpr Triple TRIDI_INVERSE(Triple t) noexcept {
    return {t.third,t.first,t.second};
}
static constexpr Triple ROTATION_1=TRIDI(ORIGINAL);
static constexpr Triple ROTATION_2=TRIDI(ROTATION_1);
static constexpr Triple ROTATION_3=TRIDI(ROTATION_2);
static_assert(PING_CANONICAL(ORIGINAL));
static_assert(PING_SET(ROTATION_1));
static_assert(PING_SET(ROTATION_2));
static_assert(PING_SET(ROTATION_3));
static_assert(!PING_CANONICAL(ROTATION_1));
static_assert(!PING_CANONICAL(ROTATION_2));
static_assert(PING_CANONICAL(ROTATION_3));
static_assert(!SAME(ROTATION_1,ORIGINAL));
static_assert(!SAME(ROTATION_2,ORIGINAL));
static_assert(SAME(ROTATION_3,ORIGINAL));
static_assert(SAME(TRIDI_INVERSE(ROTATION_1),ORIGINAL));
static_assert(SAME(TRIDI_INVERSE(TRIDI(ORIGINAL)),ORIGINAL));

// =============================================
// BiDi provenance remains intact. Its first
// two roles are the AD and CD TriDi witnesses.
// No silent conversion of the old AIRGAP lane.
// =============================================
[[nodiscard]] constexpr bool BIDI_PRESERVED() noexcept {
    return core::REPORT.bijective &&
           core::REPORT.ping_after_restore &&
           core::REPORT.restored.head.source==tork::Source::AD &&
           core::REPORT.restored.tail.source==tork::Source::CD &&
           core::REPORT.restored.head.value==
               ORIGINAL.first.folded_scaled_squared &&
           core::REPORT.restored.tail.value==
               ORIGINAL.second.folded_scaled_squared &&
           tork::EQUAL(tork::BIDI(tork::BIDI(
               core::REPORT.restored)),core::REPORT.restored);
}
static_assert(BIDI_PRESERVED());

// =============================================
// Full direct MINZA -> PING -> TriDi validation.
// A user-selected three-cycle returns the exact
// original witness order. No external approvals.
// =============================================
struct TriDiReport {
    Triple original,first_turn,second_turn,restored;
    bool geometry_ping;
    bool three_lanes_preserved;
    bool bidi_preserved;
    bool valid;
};
[[nodiscard]] constexpr TriDiReport VALIDATE() noexcept {
    const bool geometry=core::PING(FOLD) &&
        AD_FOLDED==tork::AD_FOLDED &&
        CD_FOLDED==tork::CD_FOLDED &&
        AC_FOLDED==AC2*DEN2;
    const bool three=PING_SET(ROTATION_1) &&
        PING_SET(ROTATION_2) &&
        PING_CANONICAL(ROTATION_3);
    const bool bidi=BIDI_PRESERVED();
    return {ORIGINAL,ROTATION_1,ROTATION_2,ROTATION_3,
            geometry,three,bidi,geometry&&three&&bidi};
}
static constexpr TriDiReport REPORT=VALIDATE();
static_assert(REPORT.valid && REPORT.geometry_ping);
static_assert(REPORT.three_lanes_preserved && REPORT.bidi_preserved);

// Deliberate corruption must fail, including equalization or
// rebinding the third label to an already used edge.
[[nodiscard]] constexpr bool REJECT_CORRUPTION() noexcept {
    Triple wrong=ORIGINAL;
    wrong.third.folded_scaled_squared^=1ULL;
    Triple duplicate=ORIGINAL;
    duplicate.third.edge=Edge::CD;
    Triple swapped_values=ORIGINAL;
    swapped_values.first.folded_scaled_squared=
        ORIGINAL.second.folded_scaled_squared;
    return !PING_SET(wrong)&&
           !PING_SET(duplicate)&&
           !PING_SET(swapped_values)&&
           !PING_CANONICAL(ROTATION_1);
}
static_assert(REJECT_CORRUPTION());
static constexpr bool NINZA_REQUIRED=false; // only this pinned fold
static constexpr bool GENERAL_FOLD_PROVED=false;
static constexpr bool PHYSICAL_FOLD_PROVED=false;
static constexpr bool RELEASE_AUTHORIZED=false;
static_assert(!NINZA_REQUIRED && !GENERAL_FOLD_PROVED &&
              !PHYSICAL_FOLD_PROVED && !RELEASE_AUTHORIZED);
} // namespace swe::minza_direct_ping_tridi
#endif // SWE_MINZA_DIRECT_PING_TRIDI_HPP
