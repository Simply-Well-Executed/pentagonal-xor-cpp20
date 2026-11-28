/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
// Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824
// COREAO-RDXDR | EXPLORATORY | RELEASE=HOLD
// Additive adapter: no changes to canonical XDR/RDX, THAILONGA or TALON.
// Requires xdr_rdx_fold_tork.hpp (and its prerequisites) first.
// ARIRANG is an explicit unoccupied gap / route marker, NOT a new
// XDR bit-rotation or numeric stage. Other semantics need explicit approval.
// The 6&8 combination and gap-filling are not yet defined as an algorithm.
#ifndef SWE_COREAO_RDXDR_HPP
#define SWE_COREAO_RDXDR_HPP
#ifndef SWE_XDR_RDX_FOLD_TORK_HPP
#error "Include xdr_rdx_fold_tork.hpp first"
#endif
namespace swe::coreao_rdxdr {
namespace source=swe::xdr_rdx_fold_tork;
using u64=unsigned long long;
enum class Stage : unsigned { ARIRANG=0u, SIX=6u, SEVEN=7u, EIGHT=8u, NINE=9u };
enum class Status : unsigned { PING, HOLD, FAIL };

[[nodiscard]] constexpr bool supported(Stage s) noexcept {
    return s==Stage::ARIRANG || s==Stage::SIX ||
           s==Stage::SEVEN || s==Stage::EIGHT || s==Stage::NINE;
}
[[nodiscard]] constexpr bool numeric(Stage s) noexcept {
    return s==Stage::SIX || s==Stage::SEVEN ||
           s==Stage::EIGHT || s==Stage::NINE;
}
struct Route {
    u64 original;
    u64 encoded;
    Stage stage;
    bool gap_unoccupied;
    bool valid;
};
// (ARIRANG|6|7|8|9)XDR: no accidental arithmetic at the gap.
[[nodiscard]] constexpr Route XDR(u64 word,Stage stage) noexcept {
    if(!supported(stage))return {word,word,stage,false,false};
    if(stage==Stage::ARIRANG)return {word,word,stage,true,true};
    return {word,source::XDR(word,static_cast<unsigned>(stage)),stage,false,true};
}
// RDX(ARIRANG|6|7|8|9): stage-labelled reverse of matching XDR.
[[nodiscard]] constexpr bool RDX(Route route,u64& recovered) noexcept {
    if(!route.valid || !supported(route.stage))return false;
    if(route.stage==Stage::ARIRANG) {
        if(!route.gap_unoccupied || route.encoded!=route.original)return false;
        recovered=route.original;
        return true;
    }
    if(route.gap_unoccupied)return false;
    recovered=source::RDX(route.encoded,static_cast<unsigned>(route.stage));
    return recovered==route.original;
}
[[nodiscard]] constexpr bool PING(u64 value,Stage stage) noexcept {
    const Route route=XDR(value,stage);
    u64 decoded=0ULL;
    return RDX(route,decoded) && decoded==value;
}
[[nodiscard]] constexpr bool ALL_STAGES_PING(u64 value) noexcept {
    return PING(value,Stage::ARIRANG) && PING(value,Stage::SIX) &&
           PING(value,Stage::SEVEN) && PING(value,Stage::EIGHT) &&
           PING(value,Stage::NINE);
}
// THAILONGA(6&8) with a reserved gap and outer 9, kept in separate lanes.
// This is a reversible *envelope* witness only, not proof of a new COREAO
// mathematical composition or that the ARIRANG has been filled.
struct CoreaoEnvelope {
    u64 original;
    Route six;
    Route gap;
    Route eight;
    Route nine;
    bool distinct_witnesses;
    Status stage_ping;
    Status fusion_ping;
};
[[nodiscard]] constexpr CoreaoEnvelope THAILONGA_6_AND_8(u64 value) noexcept {
    const Route six=XDR(value,Stage::SIX);
    const Route gap=XDR(value,Stage::ARIRANG);
    const Route eight=XDR(value,Stage::EIGHT);
    // 9 wraps the original value, not a fabricated merge of 6 and 8.
    const Route nine=XDR(value,Stage::NINE);
    const bool independent= six.stage!=eight.stage &&
                            six.stage!=nine.stage && gap.gap_unoccupied;
    const bool passed=independent &&
        ALL_STAGES_PING(value) &&
        source::RDX(source::XDR(value,7u),7u)==value;
    return {value,six,gap,eight,nine,independent,
            passed?Status::PING:Status::FAIL,Status::HOLD};
}
[[nodiscard]] constexpr bool PROVENANCE_PING(CoreaoEnvelope e) noexcept {
    if(e.six.original!=e.original || e.eight.original!=e.original ||
       e.gap.original!=e.original || e.nine.original!=e.original ||
       !e.gap.gap_unoccupied || !e.distinct_witnesses)return false;
    u64 a=0ULL,b=0ULL,c=0ULL,d=0ULL;
    return RDX(e.six,a) && RDX(e.gap,b) && RDX(e.eight,c) &&
           RDX(e.nine,d) &&
           a==e.original && b==e.original &&
           c==e.original && d==e.original &&
           e.stage_ping==Status::PING &&
           e.fusion_ping==Status::HOLD;
}
static constexpr u64 HEAD=source::AD_FOLDED;
static constexpr u64 TAIL=source::CD_FOLDED;
static constexpr CoreaoEnvelope HEAD_ENVELOPE=THAILONGA_6_AND_8(HEAD);
static constexpr CoreaoEnvelope TAIL_ENVELOPE=THAILONGA_6_AND_8(TAIL);
static_assert(ALL_STAGES_PING(HEAD) && ALL_STAGES_PING(TAIL));
static_assert(PROVENANCE_PING(HEAD_ENVELOPE));
static_assert(PROVENANCE_PING(TAIL_ENVELOPE));
static_assert(HEAD_ENVELOPE.original!=TAIL_ENVELOPE.original);
static_assert(HEAD_ENVELOPE.fusion_ping==Status::HOLD);
static_assert(!numeric(Stage::ARIRANG));
[[nodiscard]] constexpr bool NEGATIVE_TESTS() noexcept {
    Route altered=XDR(HEAD,Stage::NINE);
    altered.encoded^=1ULL;
    u64 result=0ULL;
    Route invalid=XDR(HEAD,static_cast<Stage>(5u));
    Route gap=XDR(HEAD,Stage::ARIRANG);
    gap.gap_unoccupied=false;
    return !RDX(altered,result) && !RDX(invalid,result) &&
           !RDX(gap,result) && source::PING_HEAD(source::HEAD(HEAD));
}
static_assert(NEGATIVE_TESTS());
inline constexpr bool COREAO_FUSION_VERIFIED=false;
inline constexpr bool RELEASE_AUTHORIZED=false;
static_assert(!COREAO_FUSION_VERIFIED && !RELEASE_AUTHORIZED);
} // namespace swe::coreao_rdxdr
#endif // SWE_COREAO_RDXDR_HPP
