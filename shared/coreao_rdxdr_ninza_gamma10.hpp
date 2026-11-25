// SPDX-License-Identifier: 0BSD
// Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824
// COREAO-RDXDR-NINZA-GAMMA10 | EXPLORATORY | RELEASE=HOLD
// Additive adapter. Neither GammaGamma nor NINZA nor XDR/RDX is renamed.
// Include coreao_rdxdr.hpp and gammagamma.hpp before this header.
// Ten explicitly supplied decimal tokens are independently evaluated.
// No invented mapping from factorial differences to NINZA's 9x6 bytes.
#ifndef SWE_COREAO_RDXDR_NINZA_GAMMA10_HPP
#define SWE_COREAO_RDXDR_NINZA_GAMMA10_HPP
#ifndef SWE_COREAO_RDXDR_HPP
#error "Include coreao_rdxdr.hpp first"
#endif
#ifndef SWE_GAMMAGAMMA_HPP
#error "Include gammagamma.hpp first"
#endif
#include <array>
#include <string_view>
#include <utility>
namespace swe::coreao_rdxdr_ninza_gamma10 {
namespace route=swe::coreao_rdxdr;
namespace gamma=swe::gammagamma;
inline constexpr unsigned gamma_stages=10u;
inline constexpr char experiment_id[]="COREAO-RDXDR-NINZA-GAMMA10-v1";
enum class Status { PING, HOLD, FAIL };
struct GammaLane {
    unsigned index;
    std::string_view input;
    gamma::Result result;
    bool provenance_matches;
};
struct Report {
    route::CoreaoEnvelope head;
    route::CoreaoEnvelope tail;
    std::array<GammaLane,gamma_stages> lanes;
    unsigned exact_count;
    unsigned symbolic_count;
    unsigned invalid_count;
    bool head_tail_distinct;
    bool xdr_rdx_ping;
    bool gamma10_ping;
    bool ninza_mapping_defined;
    Status ninza_ping;
    Status final_ping;
};
[[nodiscard]] inline Report evaluate(
    const std::array<std::string_view,gamma_stages>& inputs) {
    Report out{};
    out.head=route::THAILONGA_6_AND_8(route::HEAD);
    out.tail=route::THAILONGA_6_AND_8(route::TAIL);
    out.head_tail_distinct=(route::HEAD!=route::TAIL);
    out.xdr_rdx_ping=route::PROVENANCE_PING(out.head) &&
                     route::PROVENANCE_PING(out.tail) &&
                     route::ALL_STAGES_PING(route::HEAD) &&
                     route::ALL_STAGES_PING(route::TAIL);
    bool sources_match=true;
    for(unsigned i=0;i<gamma_stages;++i) {
        auto result=gamma::evaluate(inputs[i]);
        const bool same=result.source==inputs[i];
        sources_match=sources_match&&same;
        if(result.status==gamma::Status::exact)++out.exact_count;
        else if(result.status==gamma::Status::symbolic_only)
            ++out.symbolic_count;
        else ++out.invalid_count;
        out.lanes[i]={i,inputs[i],std::move(result),same};
    }
    out.gamma10_ping=sources_match && out.exact_count==gamma_stages &&
                     out.invalid_count==0u && out.symbolic_count==0u;
    // Explicitly unresolved: GammaGamma gives a tagged decimal-factorial
    // difference; Ninza requires 9 loops x 6 bytes derived from its source
    // sampler/stability10d process. These are not equivalent by naming.
    out.ninza_mapping_defined=false;
    out.ninza_ping=Status::HOLD;
    // Even when every reversible path and decimal witness passes,
    // a missing NINZA mapping cannot be reported as final success.
    out.final_ping=(!out.xdr_rdx_ping || !sources_match ||
                    out.invalid_count!=0u)?Status::FAIL:Status::HOLD;
    return out;
}
// A separate compatibility gate can only become active once the caller
// supplies a documented and independently verified mapping. This adapter
// deliberately cannot fabricate a Ninza or set its status to PING.
[[nodiscard]] inline bool consistent(const Report& report) {
    if(report.exact_count+report.symbolic_count+
       report.invalid_count!=gamma_stages)return false;
    if(!report.head_tail_distinct || !report.xdr_rdx_ping)return false;
    if(report.ninza_mapping_defined || report.ninza_ping!=Status::HOLD ||
       report.final_ping!=Status::HOLD)return false;
    for(unsigned i=0;i<gamma_stages;++i)
        if(report.lanes[i].index!=i || !report.lanes[i].provenance_matches ||
           report.lanes[i].result.source!=report.lanes[i].input)return false;
    return true;
}
static_assert(gamma_stages==10u);
static_assert(route::ALL_STAGES_PING(route::HEAD));
static_assert(route::ALL_STAGES_PING(route::TAIL));
static_assert(route::HEAD!=route::TAIL);
inline constexpr bool NINZA_MAPPING_VERIFIED=false;
inline constexpr bool RELEASE_AUTHORIZED=false;
static_assert(!NINZA_MAPPING_VERIFIED && !RELEASE_AUTHORIZED);
} // namespace swe::coreao_rdxdr_ninza_gamma10
#endif // SWE_COREAO_RDXDR_NINZA_GAMMA10_HPP
