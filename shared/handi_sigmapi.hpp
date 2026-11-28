/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
// HANDI SigmaPi: independent read-only check of previously derived SigmaDi PING.
#ifndef SWE_HANDI_SIGMAPI_HPP
#define SWE_HANDI_SIGMAPI_HPP
#include "minza_ninza_ping_unfold.hpp"
namespace swe::handi::sigmapi {
inline constexpr char protocol[]="HANDI-SIGMAPI-ADHERENCE-v1";
enum class Verdict : unsigned char { needs_review, contradicted };
struct Evidence {
    // Caller attestations are not independently verified.
    bool source_provenance_supplied{};
    bool independent_x_supplied{};
    bool independent_y_supplied{};
    bool independent_z_supplied{};
    bool reviewer_approved{};
    bool contrary_evidence{};
};
struct Observation {
    bool minza_valid{}, lane_valid{}, phase_valid{};
    bool ninza_agrees{}, ninza_cached_agrees{};
    bool ping_bijective{}, ping_roundtrip{}, ping_cached_restored_agrees{};
    bool ping_cached_flag_agrees{}, supplied_claim_valid{};
    bool locally_consistent{};
    bool claimed_source_provenance{}, claimed_independent_xyz{}, claimed_approval{};
    unsigned mirror_differences{}, inverse_differences{}, cached_differences{};
    Verdict verdict{Verdict::needs_review};
};
[[nodiscard]] inline Observation observe(
    const swe::sigma_penta_ping::Report& source,
    const Evidence& evidence={}) noexcept {
    using namespace swe::sigma_penta_ping;
    Observation r{};
    r.claimed_source_provenance=evidence.source_provenance_supplied;
    r.claimed_independent_xyz=evidence.independent_x_supplied &&
        evidence.independent_y_supplied && evidence.independent_z_supplied;
    r.claimed_approval=evidence.reviewer_approved;
    r.supplied_claim_valid=source.valid && source.vista.control_flow_consistent &&
        source.vista.extended_ticks==81u && source.vista.complete_loops==9u;
    r.minza_valid=true;
    for(unsigned group=0;group<minzas;++group) {
        if(source.minza[group].index!=group) r.minza_valid=false;
        for(unsigned child=0;child<minza_depth;++child)
            if(source.minza[group].loop_indices[child]!=group*minza_depth+child)
                r.minza_valid=false;
    }
    r.lane_valid=true;
    r.phase_valid=true;
    bool reversed=false;
    for(unsigned n=0;n<loops;++n) {
        const auto& row=source.loop[n];
        if(row.index!=n || row.terminal!=terminal(n))
            r.lane_valid=false;
        if(row.route_reversed_before!=reversed)
            r.phase_valid=false;
        if(row.terminal==Terminal::flip) reversed=!reversed;
        if(row.route_reversed_after!=reversed)
            r.phase_valid=false;
    }
    // Reuse producer data and existing inverse; do not mint a second braid.
    const auto mirror=unfold::unforge(source.ninza);
    const auto inverse=unfold::untwirl(source.ping,source.ninza);
    r.mirror_differences=mirror.mismatched_bytes;
    r.inverse_differences=inverse.mismatched_vs_primary;
    r.cached_differences=inverse.mismatched_vs_cached_restored;
    r.ninza_agrees=mirror.copies_match;
    r.ninza_cached_agrees=(source.ninza.matches==mirror.copies_match);
    r.ping_bijective=inverse.one_to_one_mapping;
    r.ping_roundtrip=inverse.roundtrip_vs_primary;
    r.ping_cached_restored_agrees=(r.cached_differences==0u);
    r.ping_cached_flag_agrees=(source.ping.roundtrip==inverse.roundtrip_vs_primary);
    r.locally_consistent=r.supplied_claim_valid && r.minza_valid && r.lane_valid &&
        r.phase_valid && r.ninza_agrees && r.ninza_cached_agrees &&
        r.ping_bijective && r.ping_roundtrip &&
        r.ping_cached_restored_agrees && r.ping_cached_flag_agrees;
    // Local symmetry is not external proof. No auto-PASS.
    r.verdict=(!r.locally_consistent || evidence.contrary_evidence)
        ? Verdict::contradicted : Verdict::needs_review;
    return r;
}
} // namespace swe::handi::sigmapi
#endif
