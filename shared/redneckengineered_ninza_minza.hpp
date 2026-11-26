// SPDX-License-Identifier: 0BSD
// Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824
// REDNECKENGINEERED | NINZA in motion -> grouped MINZA | PING per lane.
// Exploratory additive adapter, no original source modification or replacement.
// Distinguish recovering grouped six-byte lanes from reconstructing pixels.
#ifndef SWE_REDNECKENGINEERED_NINZA_MINZA_HPP
#define SWE_REDNECKENGINEERED_NINZA_MINZA_HPP
#include "minza_ninza_ping_unfold.hpp"
#include <array>
#include <cstdint>

namespace swe::redneckengineered {
namespace src=swe::sigma_penta_ping;
namespace inv=swe::sigma_penta_ping::unfold;
inline constexpr char protocol[]="REDNECKENGINEERED-NINZA-MOTION-MINZA-PING-v1";
inline constexpr unsigned groups=src::minzas;
inline constexpr unsigned depth=src::minza_depth;
inline constexpr unsigned loops=src::loops;
inline constexpr unsigned stripe_bytes=src::stripe_bytes;
static_assert(groups*depth==loops);
static_assert(src::ping_bytes==loops*stripe_bytes);
using Stripe=std::array<std::uint8_t,stripe_bytes>;

struct MinzaGroup {
    src::Minza source_group{};
    std::array<Stripe,depth> lanes{};
    std::array<bool,depth> individual_ping{};
    bool ping{};
};
struct MinzaResult {
    std::array<MinzaGroup,groups> group{};
    std::array<unsigned,src::ping_bytes> braid_to_source{};
    bool motion_one_to_one{};
    bool mirrors_agree{};
    bool full_ping{};
};

[[nodiscard]] constexpr bool equal(const Stripe& a,const Stripe& b) noexcept {
    for(unsigned j=0;j<stripe_bytes;++j)
        if(a[j]!=b[j])return false;
    return true;
}
// One motion witness is independently validated per source loop.
// The global PING additionally requires every braid position to be unique.
[[nodiscard]] constexpr bool recover(const src::Ninza& ninza,
                                     const src::Ping& motion,
                                     MinzaResult& destination) noexcept {
    const inv::Unforged original=inv::unforge(ninza);
    if(!original.copies_match || !ninza.matches || !motion.roundtrip)
        return false;
    const inv::Untwirled still=inv::untwirl(motion,ninza);
    if(!still.one_to_one_mapping || !still.roundtrip_vs_primary ||
       still.mismatched_vs_cached_restored!=0u)
        return false;
    MinzaResult candidate{};
    candidate.motion_one_to_one=still.one_to_one_mapping;
    candidate.mirrors_agree=original.copies_match;
    candidate.braid_to_source=still.original_flat_offset;
    bool all=true;
    for(unsigned group=0;group<groups;++group) {
        MinzaGroup& frame=candidate.group[group];
        frame.source_group.index=group;
        frame.ping=true;
        for(unsigned slot=0;slot<depth;++slot) {
            const unsigned source_index=group*depth+slot;
            frame.source_group.loop_indices[slot]=source_index;
            frame.lanes[slot]=still.restored[source_index];
            const bool lane_ok=
                equal(frame.lanes[slot],original.primary[source_index]) &&
                equal(frame.lanes[slot],original.mirrored[source_index]);
            frame.individual_ping[slot]=lane_ok;
            frame.ping=frame.ping&&lane_ok;
        }
        all=all&&frame.ping;
    }
    candidate.full_ping=all && candidate.mirrors_agree &&
                        candidate.motion_one_to_one;
    if(!candidate.full_ping)return false;
    destination=candidate; // Mutation only after all individual PINGs.
    return true;
}
// MINZA -> NINZA in motion again: exact independent rebraid, no new bytes.
[[nodiscard]] constexpr bool replay(const MinzaResult& minza,
                                    src::Ninza& ninza,
                                    src::Ping& motion) noexcept {
    if(!minza.full_ping || !minza.motion_one_to_one || !minza.mirrors_agree)
        return false;
    src::Ninza n{};
    src::Ping p{};
    for(unsigned group=0;group<groups;++group) {
        const MinzaGroup& frame=minza.group[group];
        if(!frame.ping || frame.source_group.index!=group)return false;
        for(unsigned slot=0;slot<depth;++slot) {
            const unsigned index=group*depth+slot;
            if(!frame.individual_ping[slot] ||
               frame.source_group.loop_indices[slot]!=index)return false;
            n.primary[index]=frame.lanes[slot];
            n.mirrored[index]=frame.lanes[slot];
        }
    }
    n.matches=true;
    std::array<bool,src::ping_bytes> used{};
    for(unsigned column=0;column<stripe_bytes;++column)
        for(unsigned position=0;position<loops;++position) {
            const unsigned index=src::lane(column,position);
            const unsigned flat=index*stripe_bytes+column;
            const unsigned braided=column*loops+position;
            if(flat>=src::ping_bytes || used[flat] ||
               minza.braid_to_source[braided]!=flat)return false;
            used[flat]=true;
            const std::uint8_t value=n.primary[index][column];
            p.braid[braided]=value;
            p.restored[flat]=value;
        }
    for(bool seen:used)if(!seen)return false;
    p.roundtrip=true;
    ninza=n;
    motion=p;
    return true;
}
[[nodiscard]] constexpr bool individualized_ping(
    const MinzaResult& minza,unsigned index) noexcept {
    if(index>=loops || !minza.full_ping)return false;
    const unsigned group=index/depth,slot=index%depth;
    return minza.group[group].individual_ping[slot] &&
           minza.group[group].source_group.loop_indices[slot]==index;
}
inline constexpr bool RECONSTRUCTS_ORIGINAL_PIXELS=false;
inline constexpr bool AUTHENTICATES_SOURCE_SAMPLER=false;
inline constexpr bool RELEASE_AUTHORIZED=false;
static_assert(!RECONSTRUCTS_ORIGINAL_PIXELS &&
              !AUTHENTICATES_SOURCE_SAMPLER &&
              !RELEASE_AUTHORIZED);
} // namespace swe::redneckengineered
#endif
