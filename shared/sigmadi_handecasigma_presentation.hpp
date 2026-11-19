// SPDX-License-Identifier: 0BSD
// OSI layer 6 presentation ONLY: nine audit eyes, ten language display
// slots, 108 clearly virtual VietQuang display lenses. No new evidence.
#ifndef SWE_SIGMADI_HANDECASIGMA_PRESENTATION_HPP
#define SWE_SIGMADI_HANDECASIGMA_PRESENTATION_HPP
#include "sigma_penta_ping.hpp"
#include "sigmadi_collision12.hpp"
#include <array>
#include <cstdint>

namespace swe::han_presentation {
inline constexpr char protocol[] = "SigmaDi-HANDecaSigma11-VISTA-OSI6-v1";
inline constexpr unsigned audit_eye_count=9;
inline constexpr unsigned language_eye_count=10;
inline constexpr unsigned virtual_vietquang_count=108;
// These ten real languages are provisional UI labels, not a claim that
// the earlier conversation established these exact ten language slots.
struct Language { const char* code; const char* native_name; };
inline constexpr std::array<Language,language_eye_count> languages{{
    {"en","English"}, {"fr","Français"}, {"es","Español"},
    {"pt","Português"}, {"it","Italiano"}, {"el","Ελληνικά"},
    {"ru","Русский"}, {"uk","Українська"}, {"ko","한국어"},
    {"vi","Tiếng Việt"}
}};
enum class Eye : std::uint8_t {
    shape=0, bounds=1, structure=2, scope=3, domain=4,
    reduction=5, inverse=6, comparison=7, provenance=8
};
inline constexpr std::array<const char*,audit_eye_count> eye_names{{
    "shape","bounds","structure","scope","domain",
    "reduction","inverse","comparison","provenance"
}};
struct VirtualLens {
    unsigned index{};        // 0..107
    unsigned trace_index{};  // 0..8
    unsigned variation{};    // 0..11
    bool independent_evidence{false};
};
struct Chirality {
    // Imported from HANDecaSigma[11] source evidence, when supplied.
    // These MUST NOT be inferred from still-image rotation settings.
    std::uint32_t clockwise{}, counterclockwise{};
    bool has_accepted_turns{};
    bool canonical_s10_present{false};
    double canonical_s10{};
};
struct ChiralityView {
    bool h11_available{}, s11_available{}, sampling_invariant{};
    double h11{}, divisor{}, s11{};
    std::uint32_t accepted_turns{};
};
struct SigmaDi {
    unsigned loop_index{};               // 0..8
    std::array<int,4> q_polarity{};      // q0..q3
    bool twirl_reversed{};
};
struct View {
    std::array<SigmaDi,9> sigma{};
    std::array<const char*,audit_eye_count> audit_names{};
    std::array<Language,language_eye_count> language_views{};
    std::array<VirtualLens,virtual_vietquang_count> virtual_views{};
    ChiralityView chirality{};
    std::array<std::string_view, swe::collision12::dimension_count> collision_axes{};
    bool dimension12_guard_available{true};
    bool nine_loop_source_valid{}, presentation_only{true};
    bool has_independent_authentication{false};
    bool relativismos_defined{false}; // deliberately unavailable
    bool empathy_multiplier_measured{false}; // "300%" is imaginative UI
};
inline ChiralityView inspect_chirality(const Chirality& c) noexcept {
    ChiralityView result{};
    const std::uint64_t total=static_cast<std::uint64_t>(c.clockwise)+c.counterclockwise;
    if(!c.has_accepted_turns || total==0) return result;
    const double signed_count=static_cast<double>(c.clockwise)-
                              static_cast<double>(c.counterclockwise);
    result.accepted_turns=static_cast<std::uint32_t>(
        total > 0xffffffffu ? 0xffffffffu : total);
    result.h11=signed_count/static_cast<double>(total);
    result.divisor=2.0-(result.h11<0.0?-result.h11:result.h11);
    result.h11_available=true;
    // A source-present bool alone cannot verify that S10 and H11 refer
    // to the same tracked physical object; leave S11 unavailable here.
    result.s11_available=false;
    result.sampling_invariant=false;
    return result;
}
inline View present(const swe::sigma_penta_ping::Report& source,
                    const Chirality& chirality={}) noexcept {
    View v{};
    v.nine_loop_source_valid=source.valid;
    v.audit_names=eye_names;
    v.collision_axes=swe::collision12::axis_names;
    v.language_views=languages;
    v.chirality=inspect_chirality(chirality);
    bool twirled=false;
    for(unsigned n=0;n<9u;++n) {
        auto& s=v.sigma[n];
        s.loop_index=n;
        s.twirl_reversed=twirled;
        const int sign=twirled ? -1 : 1;
        s.q_polarity={sign,-sign,sign,-sign};
        // Mirror the experimental FLIP result after odd loop indices.
        if(source.loop[n].terminal==swe::sigma_penta_ping::Terminal::flip)
            twirled=!twirled;
    }
    for(unsigned i=0;i<virtual_vietquang_count;++i)
        v.virtual_views[i]={i,i%audit_eye_count,i/audit_eye_count,false};
    return v;
}
static_assert(virtual_vietquang_count>99);
static_assert(language_eye_count==10 && audit_eye_count==9);
static_assert(swe::sigma_penta_ping::angle(0,4)==288);
} // namespace swe::han_presentation
#endif
