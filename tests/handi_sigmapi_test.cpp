// SPDX-License-Identifier: 0BSD
// Deterministic C++20 HANDI SigmaPi plus noncanonical ROT triad tests.
#include "../shared/handi_sigmapi.hpp"
#include "../shared/handi_rot_triad.hpp"
int main() {
    using namespace swe::sigma_penta_ping;
    using swe::handi::sigmapi::observe;
    using swe::handi::sigmapi::Verdict;
    using swe::handi::sigmapi::Evidence;
    Input input{};
    input.threshold=128u;
    for(unsigned n=0;n<loops;++n) {
        input.samples[n].pixel={0u,64u,128u,192u,255u};
        input.source_tokens[n]=202600u+n;
        input.expected30[n]=31u;
    }
    Report report{};
    if(!execute(input,report)||!report.valid) return 1;
    const Report saved=report;
    const auto good=observe(report);
    if(!good.locally_consistent || good.verdict!=Verdict::needs_review ||
       !good.minza_valid || !good.lane_valid || !good.phase_valid ||
       !good.ninza_agrees || !good.ping_bijective || !good.ping_roundtrip ||
       good.mirror_differences || good.inverse_differences ||
       good.cached_differences) return 2;
    if(report.ping.braid!=saved.ping.braid ||
       report.ninza.primary!=saved.ninza.primary) return 3;
    Evidence claims{};
    claims.source_provenance_supplied=true;
    claims.independent_x_supplied=true;
    claims.independent_y_supplied=true;
    claims.independent_z_supplied=true;
    claims.reviewer_approved=true;
    if(!observe(report,claims).locally_consistent ||
       observe(report,claims).verdict!=Verdict::needs_review) return 4;
    Report bad=saved; bad.ping.braid[17]^=0x80u;
    const auto inverseFailure=observe(bad);
    if(inverseFailure.locally_consistent ||
       inverseFailure.verdict!=Verdict::contradicted ||
       inverseFailure.inverse_differences!=1u) return 5;
    bad=saved; bad.ninza.mirrored[8][5]^=0x20u;
    const auto mirrorFailure=observe(bad);
    if(mirrorFailure.locally_consistent ||
       mirrorFailure.verdict!=Verdict::contradicted ||
       mirrorFailure.mirror_differences!=1u) return 6;
    bad=saved; bad.minza[1].loop_indices[0]=8u;
    if(observe(bad).minza_valid || observe(bad).verdict!=Verdict::contradicted) return 7;
    bad=saved; bad.loop[1].route_reversed_after=false;
    if(observe(bad).phase_valid || observe(bad).verdict!=Verdict::contradicted) return 8;
    claims.contrary_evidence=true;
    if(observe(report,claims).verdict!=Verdict::contradicted) return 9;
    if(report.ping.braid!=saved.ping.braid ||
       report.ninza.primary!=saved.ninza.primary) return 10;
    using swe::handi::rot_triad::display;
    const auto ion=display("SEX",3u);
    if(!ion.valid || ion.text[0]!='I' || ion.text[1]!='O' ||
       ion.text[2]!='N' || ion.text[3]!='\0' || ion.discarded_suffix) return 11;
    const auto truncated=display("COREAO",6u);
    if(!truncated.valid || truncated.text[0]!='S' ||
       truncated.text[1]!='Y' || truncated.text[2]!='H' ||
       truncated.discarded_suffix!=3u) return 12;
    const auto yin=display("YIN",3u),ying=display("YING",4u);
    if(!yin.valid || !ying.valid ||
       yin.text[0]!=ying.text[0] || yin.text[1]!=ying.text[1] ||
       yin.text[2]!=ying.text[2] || ying.discarded_suffix!=1u) return 13;
    if(display("SE",2u).valid || display("S=E",3u).valid ||
       display(nullptr,0u).valid) return 14;
    return 0;
}
