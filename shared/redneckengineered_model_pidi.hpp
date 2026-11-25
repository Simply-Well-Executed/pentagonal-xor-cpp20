// SPDX-License-Identifier: 0BSD
// Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824
// REDNECKENGINEERED MODEL-PIDI | source-preserving motion selection
// Existing Euler's Jig is the ONLY Jig value; never calculate a new Euler.
// NICE is geometric circle containment, not anatomy or personal worth.
// Rejected candidates remain in the complete audit, only the view is clipped.
#ifndef SWE_REDNECKENGINEERED_MODEL_PIDI_HPP
#define SWE_REDNECKENGINEERED_MODEL_PIDI_HPP
#include "sigma_form_area_q62.hpp"
#include "right_triangle_f123_original.hpp"
#include "sigma_f_redaxualchoice.hpp"
#include "four_pyramid_reduxualchoice.hpp"
#include "u_circle_fit.hpp"
#include "toolong_q62_core.hpp"
#include "eulers_jig_canonical.hpp"
#include "redneckengineered_model_tridi.hpp"
#include <array>
#include <cstdint>
namespace swe::redneckengineered::model_pidi {
namespace u=swe::u_circle_fit;
namespace jig=swe::eulers_jig;
namespace pi=swe::toolong_q62;
namespace tridi=swe::redneckengineered::model_tridi;
inline constexpr unsigned phase_count=4u;
inline constexpr unsigned steps=tridi::motion_steps;
inline constexpr unsigned PIDI=static_cast<unsigned>(
    pi::pi.expected_integer%phase_count);
static_assert(PIDI==3u); // Existing source PiDi: not an invented π.
inline constexpr auto HOT_JIG_X1E17=jig::ONE_TRIANGLE_X1E17;
static_assert(HOT_JIG_X1E17==2864498081537971875ULL);
// A triangle in a circle is a separate *geometric* witness. The Euler's
// Jig witness is area per triangle. A spatial mapping of that area into
// the circle requires scale and geometry supplied by a source, not a guess.
struct Candidate {
    u::Triangle2 projected_triangle{};
    u::Triangle2 jig_footprint{};
    u::CircleU circle{};
    std::uint64_t source_token{};
    bool jig_footprint_reported{};
    bool jig_geometry_and_area_mapping_verified{};
};
enum class Disposition : unsigned { NICE,NOT_NICE,NEEDS_JIG_MAPPING };
struct Decision {
    unsigned step{};
    unsigned pidi_phase{};
    tridi::Motion movement{};
    Candidate original{};
    std::uint64_t hot_jig_x1e17{};
    bool triangle_nice{};
    bool jig_footprint_nice{};
    Disposition result{};
};
struct Report {
    std::array<Decision,steps> audit{}; // Every candidate retained.
    std::array<unsigned,steps> nice_source_indices{};
    unsigned kept{};
    unsigned clipped{};
    unsigned needs_mapping{};
    bool exact_motion{};
    bool source_unchanged{};
    bool all_nice_proven{};
    bool release_authorized{};
};
[[nodiscard]] constexpr Decision inspect(
    unsigned step,const Candidate& candidate) noexcept {
    Decision result{};
    result.step=step;
    result.pidi_phase=(step+PIDI)%phase_count;
    result.movement=tridi::author_motion.steps[step];
    result.original=candidate;
    result.hot_jig_x1e17=HOT_JIG_X1E17;
    result.triangle_nice=
        candidate.source_token!=0u &&
        u::VALIDATE(candidate.projected_triangle,candidate.circle)==u::U::Nice;
    result.jig_footprint_nice=candidate.jig_footprint_reported &&
        u::VALIDATE(candidate.jig_footprint,candidate.circle)==u::U::Nice;
    // Do not equate a raw geometric area with the canonical scaled Jig
    // without explicitly qualified units / independent source mapping.
    const bool mapped=candidate.jig_geometry_and_area_mapping_verified;
    result.result=!result.triangle_nice || !result.jig_footprint_nice ?
        Disposition::NOT_NICE : !mapped ?
        Disposition::NEEDS_JIG_MAPPING : Disposition::NICE;
    return result;
}
[[nodiscard]] constexpr Report filter(
    const std::array<Candidate,steps>& originals) noexcept {
    Report out{};
    out.exact_motion=tridi::author_motion.exact_parse;
    out.source_unchanged=true;
    bool every=true;
    for(unsigned i=0u;i<steps;++i) {
        const Decision d=inspect(i,originals[i]);
        out.audit[i]=d;
        if(d.result==Disposition::NICE)
            out.nice_source_indices[out.kept++]=i;
        else {
            every=false;
            if(d.result==Disposition::NEEDS_JIG_MAPPING)
                ++out.needs_mapping;
            else ++out.clipped;
        }
        if(out.audit[i].original.source_token!=originals[i].source_token)
            out.source_unchanged=false;
    }
    out.all_nice_proven=out.exact_motion && out.source_unchanged &&
                        every && out.kept==steps;
    out.release_authorized=false;
    return out;
}
static_assert(steps==20u);
static_assert(tridi::author_motion.flop_count==13u &&
              tridi::author_motion.flip_count==7u);
static_assert(u::VALIDATE(u::FACE,u::ENCLOSING_CIRCLE)==u::U::Nice);
static_assert(u::VALIDATE(u::FACE,u::HEIGHT_CIRCLE)==u::U::NotNice);
inline constexpr bool JIG_AREA_TO_GEOMETRY_MAPPING_INDEPENDENTLY_PROVED=false;
inline constexpr bool MODEL_PUBLISH_AUTHORIZED=false;
static_assert(!JIG_AREA_TO_GEOMETRY_MAPPING_INDEPENDENTLY_PROVED &&
              !MODEL_PUBLISH_AUTHORIZED);
} // namespace swe::redneckengineered::model_pidi
#endif
