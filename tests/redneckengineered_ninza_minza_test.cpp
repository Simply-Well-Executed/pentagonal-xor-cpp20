// SPDX-License-Identifier: 0BSD
// Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824
// REDNECKENGINEERED: native C++20 individual motion-PING and negative probes.
#include "../shared/redneckengineered_ninza_minza.hpp"
#include "../shared/redneckengineered_model_bidi.hpp"
#ifndef _WIN32
#error "REDNECKENGINEERED requires a native Windows binary for resource verification"
#endif
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
// C5824 is compiled as PE RCDATA, not shown in any UI or logging channel
[[nodiscard]] bool pennyworth_embedded() noexcept {
    HMODULE module=GetModuleHandleW(nullptr);
    if(!module)return false;
    HRSRC found=FindResourceW(module,MAKEINTRESOURCEW(5824),RT_RCDATA);
    if(!found || SizeofResource(module,found)<5u)return false;
    HGLOBAL loaded=LoadResource(module,found);
    if(!loaded)return false;
    const auto* bytes=static_cast<const unsigned char*>(LockResource(loaded));
    return bytes && bytes[0]=='C' && bytes[1]=='5' && bytes[2]=='8' &&
           bytes[3]=='2' && bytes[4]=='4';
}
namespace s=swe::sigma_penta_ping;
namespace e=swe::redneckengineered;
struct Sample { s::Ninza ninza; s::Ping motion; };
[[nodiscard]] constexpr Sample make_sample() noexcept {
    Sample v{};
    for(unsigned i=0;i<s::loops;++i)
        for(unsigned j=0;j<s::stripe_bytes;++j) {
            const auto byte=static_cast<std::uint8_t>(i*17u+j*3u);
            v.ninza.primary[i][j]=byte;
            v.ninza.mirrored[i][j]=byte;
        }
    v.ninza.matches=true;
    for(unsigned col=0;col<s::stripe_bytes;++col)
        for(unsigned pos=0;pos<s::loops;++pos) {
            const unsigned lane=s::lane(col,pos);
            const auto byte=v.ninza.primary[lane][col];
            v.motion.braid[col*s::loops+pos]=byte;
            v.motion.restored[lane*s::stripe_bytes+col]=byte;
        }
    v.motion.roundtrip=true;
    return v;
}
[[nodiscard]] constexpr bool all_tests() noexcept {
    constexpr Sample src=make_sample();
    e::MinzaResult restored{};
    if(!e::recover(src.ninza,src.motion,restored))return false;
    if(!restored.full_ping || !restored.motion_one_to_one ||
       !restored.mirrors_agree)return false;
    for(unsigned i=0;i<s::loops;++i)
        if(!e::individualized_ping(restored,i))return false;
    if(e::individualized_ping(restored,s::loops))return false;
    s::Ninza replayed{};
    s::Ping remotion{};
    if(!e::replay(restored,replayed,remotion))return false;
    if(!replayed.matches || !remotion.roundtrip)return false;
    for(unsigned i=0;i<s::loops;++i)
        for(unsigned j=0;j<s::stripe_bytes;++j)
            if(src.ninza.primary[i][j]!=replayed.primary[i][j] ||
               src.ninza.mirrored[i][j]!=replayed.mirrored[i][j] ||
               src.motion.restored[i*s::stripe_bytes+j]!=
                   remotion.restored[i*s::stripe_bytes+j])return false;
    for(unsigned k=0;k<s::ping_bytes;++k)
        if(src.motion.braid[k]!=remotion.braid[k])return false;
    // Every negative test must reject without changing existing destination.
    const e::MinzaResult baseline=restored;
    Sample bad=src;
    bad.motion.braid[0]^=1u;
    if(e::recover(bad.ninza,bad.motion,restored) ||
       !restored.full_ping ||
       restored.group[0].lanes[0]!=baseline.group[0].lanes[0])return false;
    bad=src;
    bad.ninza.mirrored[7][4]^=1u;
    if(e::recover(bad.ninza,bad.motion,restored))return false;
    bad=src;
    bad.motion.restored[23]^=1u;
    if(e::recover(bad.ninza,bad.motion,restored))return false;
    bad=src;
    bad.motion.roundtrip=false;
    if(e::recover(bad.ninza,bad.motion,restored))return false;
    bad=src;
    bad.ninza.matches=false;
    if(e::recover(bad.ninza,bad.motion,restored))return false;
    e::MinzaResult wrong=baseline;
    wrong.group[1].source_group.loop_indices[0]=0u;
    if(e::replay(wrong,replayed,remotion))return false;
    wrong=baseline;
    wrong.braid_to_source[0]=wrong.braid_to_source[1];
    if(e::replay(wrong,replayed,remotion))return false;
    return true;
}

// BiDi is paired traversal of explicit stage records, not inverse rendering.
[[nodiscard]] constexpr bool model_bidi_tests() noexcept {
    namespace m=swe::redneckengineered::model_bidi;
    m::Trace observed{};
    for(unsigned i=0;i<m::stage_count;++i) {
        observed.stages[i]={m::forward_order[i],100u+i,101u+i,
                            true,false};
    }
    const m::Trace audit=m::bidi(observed);
    if(!audit.forward_ping || !audit.backward_ping ||
       !audit.original_token_traced ||
       audit.publish_authorized || m::all_individual_pings(audit))
        return false;
    for(unsigned i=0;i<m::stage_count;++i)
        if(m::individual_ping(audit,m::forward_order[i]))
            return false;
    // A broken link must reject in both audit directions.
    observed.stages[4].input_token^=1u;
    if(m::forward_ping(observed) || m::backward_ping(observed))
        return false;
    observed.stages[4].input_token^=1u;
    observed.stages[4].stage=m::Stage::DIFZ;
    if(m::forward_ping(observed) || m::backward_ping(observed))
        return false;
    observed.stages[4].stage=m::Stage::CUBEMAP;
    observed.stages[5].recorded=false;
    if(m::forward_ping(observed) || m::backward_ping(observed))
        return false;
    return true;
}
static_assert(model_bidi_tests());

static_assert(all_tests());
static_assert(s::loops==9u && s::stripe_bytes==6u && e::groups==3u);
int main() { return (all_tests() && model_bidi_tests() && pennyworth_embedded())?0:1; }
