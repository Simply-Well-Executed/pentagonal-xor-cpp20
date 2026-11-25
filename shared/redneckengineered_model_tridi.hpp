// SPDX-License-Identifier: 0BSD
// Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824
// REDNECKENGINEERED MODEL-TRIDI | EXPLORATORY | RELEASE=HOLD
// An additive third lane. Never edit the source BiDi or invent DIFZ,
// COR, or a numeric meaning for the author's NULL EULER'S JIG sequence.
#ifndef SWE_REDNECKENGINEERED_MODEL_TRIDI_HPP
#define SWE_REDNECKENGINEERED_MODEL_TRIDI_HPP
#include "redneckengineered_model_bidi.hpp"
#include <array>
#include <string_view>
namespace swe::redneckengineered::model_tridi {
namespace bi=swe::redneckengineered::model_bidi;
using u64=std::uint64_t;
inline constexpr std::string_view raw_sequence=
    "NULLEULER'SJIGIGAVEYOUXORORNORCOR"
    "FLOPFLOPFLIPFLIPFLOPFLIPFLOPFLOPFLOPFLOP"
    "FLIPFLIPFLOPFLOPFLIPFLOPFLOPFLOPFLOPFLIPPING";
inline constexpr std::string_view prefix=
    "NULLEULER'SJIGIGAVEYOUXORORNORCOR";
inline constexpr std::string_view terminal="PING";
inline constexpr unsigned motion_steps=20u;
enum class Motion : unsigned { FLOP,FLIP };
enum class Role : unsigned { FORWARD,BACKWARD,NULL_EULERS_JIG };
enum class Status : unsigned { PING,HOLD,FAIL };
struct Parsed {
    std::array<Motion,motion_steps> steps{};
    unsigned count{};
    unsigned flop_count{};
    unsigned flip_count{};
    bool exact_parse{};
};
[[nodiscard]] constexpr Parsed parse(std::string_view raw) noexcept {
    Parsed p{};
    if(!raw.starts_with(prefix) || !raw.ends_with(terminal))
        return p;
    auto middle=raw.substr(prefix.size(),
                           raw.size()-prefix.size()-terminal.size());
    unsigned cursor=0u;
    while(cursor<middle.size() && p.count<motion_steps) {
        const auto remain=middle.substr(cursor);
        if(remain.starts_with("FLOP")) {
            p.steps[p.count++]=Motion::FLOP;
            ++p.flop_count; cursor+=4u;
        } else if(remain.starts_with("FLIP")) {
            p.steps[p.count++]=Motion::FLIP;
            ++p.flip_count; cursor+=4u;
        } else return p;
    }
    p.exact_parse=(cursor==middle.size() && p.count==motion_steps &&
                   p.flop_count+p.flip_count==motion_steps);
    return p;
}
inline constexpr Parsed author_motion=parse(raw_sequence);
static_assert(author_motion.exact_parse);
static_assert(author_motion.flop_count==13u);
static_assert(author_motion.flip_count==7u);
// A local preservation witness, not XOR/OR/NOR/COR evaluation.
// Null is a named third role, not data absence, permission or numeric 0.
struct Lane {
    Role role{};
    u64 token{};
    bool source_present{};
};
struct Three {
    std::array<Lane,3u> lanes{};
};
[[nodiscard]] constexpr bool equal(Lane a,Lane b) noexcept {
    return a.role==b.role && a.token==b.token &&
           a.source_present==b.source_present;
}
[[nodiscard]] constexpr bool equal(Three a,Three b) noexcept {
    for(unsigned i=0;i<3u;++i)
        if(!equal(a.lanes[i],b.lanes[i]))return false;
    return true;
}
// Reuse the established TriDi rotation: {a,b,c}->{b,c,a}.
[[nodiscard]] constexpr Three TRIDI(Three t) noexcept {
    return {{t.lanes[1],t.lanes[2],t.lanes[0]}};
}
[[nodiscard]] constexpr Three UNTRIDI(Three t) noexcept {
    return {{t.lanes[2],t.lanes[0],t.lanes[1]}};
}
[[nodiscard]] constexpr bool source_roles(Three t) noexcept {
    bool seen[3]{};
    for(unsigned i=0;i<3u;++i) {
        const unsigned role=static_cast<unsigned>(t.lanes[i].role);
        if(role>=3u || seen[role] || !t.lanes[i].source_present ||
           t.lanes[i].token==0u)return false;
        seen[role]=true;
    }
    return seen[0]&&seen[1]&&seen[2];
}
struct Report {
    bi::Trace existing_bidi{};
    Three original{};
    Three first{};
    Three second{};
    Three third{};
    Parsed motion{};
    bool bidi_record_ping{};
    bool third_lane_sequence_ping{};
    bool tridi_roundtrip_ping{};
    Status model_processing{};
    Status finalized_model{};
};
// Tokens are caller-supplied provenance identifiers. The third token names
// the author's *exact* source sequence; do not fabricate model or bitmap data.
[[nodiscard]] constexpr Report audit(const bi::Trace& trace,
                                     u64 forward_token,
                                     u64 backward_token,
                                     u64 jig_token,
                                     std::string_view source_text) noexcept {
    Report out{};
    out.existing_bidi=bi::bidi(trace);
    out.motion=parse(source_text);
    out.original={{{Role::FORWARD,forward_token,true},
                   {Role::BACKWARD,backward_token,true},
                   {Role::NULL_EULERS_JIG,jig_token,true}}};
    out.first=TRIDI(out.original);
    out.second=TRIDI(out.first);
    out.third=TRIDI(out.second);
    out.bidi_record_ping=out.existing_bidi.forward_ping &&
                         out.existing_bidi.backward_ping;
    out.third_lane_sequence_ping=(source_text==raw_sequence &&
                                  out.motion.exact_parse);
    out.tridi_roundtrip_ping=source_roles(out.original) &&
        equal(out.third,out.original) &&
        equal(UNTRIDI(out.first),out.original) &&
        !equal(out.first,out.original);
    // These are *audit* proofs, not a model baker, renderer, DIFZ or publish.
    out.model_processing=Status::HOLD;
    out.finalized_model=Status::HOLD;
    return out;
}
[[nodiscard]] constexpr bool provenance_ping(const Report& r) noexcept {
    return r.bidi_record_ping &&
           r.third_lane_sequence_ping &&
           r.tridi_roundtrip_ping &&
           r.existing_bidi.publish_authorized==false &&
           r.model_processing==Status::HOLD &&
           r.finalized_model==Status::HOLD;
}
static_assert(raw_sequence.starts_with(prefix));
static_assert(raw_sequence.ends_with(terminal));
static_assert(raw_sequence.size()>prefix.size()+terminal.size());
static_assert(!parse("NULLEULER'SJIGIGAVEYOUXORORNORCORFLOPPING").exact_parse);
inline constexpr bool NUMERIC_GATES_VERIFIED=false;
inline constexpr bool DIFZ_VERIFIED=false;
inline constexpr bool MODEL_PUBLISH_AUTHORIZED=false;
static_assert(!NUMERIC_GATES_VERIFIED && !DIFZ_VERIFIED &&
              !MODEL_PUBLISH_AUTHORIZED);
} // namespace swe::redneckengineered::model_tridi
#endif
