/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
// SIGMA-PENTA-LIGAND-COREAO-PING-v1: zero-based, source-preserving adapter.
// Exploratory; input samples are supplied by a separately audited image sampler.
#ifndef SWE_SIGMA_PENTA_PING_HPP
#define SWE_SIGMA_PENTA_PING_HPP
#include "pentagonal_vista_9x9.hpp"
#include "stability10d.hpp"
#include <array>
#include <cstdint>

namespace swe::sigma_penta_ping {
inline constexpr unsigned fields = 5, loops = 9, rings = 3;
inline constexpr unsigned minzas = 3, minza_depth = 3;
inline constexpr unsigned stripe_bytes = 6, ping_bytes = loops * stripe_bytes;
inline constexpr char protocol[] = "SIGMA-PENTA-LIGAND-COREAO-PING-v1";
enum class Terminal : std::uint8_t { flop, flip };
struct FiveSample { std::array<std::uint8_t,fields> pixel{}; };
struct Input {
    std::array<FiveSample,loops> samples{};
    std::array<std::uint32_t,loops> expected30{};
    std::array<std::uint64_t,loops> source_tokens{};
    std::uint8_t threshold{128};
    bool witness_claimed{false}; // NOT independently authenticated
};
struct Omega {
    std::uint8_t mode{}, range{}, frequency{};
    bool tie{};
};
struct Statistics {
    std::uint8_t alpha{}, beta{}, climax{}, minimum{};
    double vertex{};
    Omega omega{};
};
struct Loop {
    unsigned index{}; // 0..8
    Terminal terminal{};
    bool route_reversed_before{}, route_reversed_after{};
    std::array<unsigned,fields> grating_degrees{};
    Statistics stats{};
    std::array<double,9> ligands{};
    swe::stability10d::Result three_rings{};
};
struct Minza {
    unsigned index{};
    std::array<unsigned,minza_depth> loop_indices{};
};
struct Ninza {
    std::array<std::array<std::uint8_t,stripe_bytes>,loops> primary{};
    std::array<std::array<std::uint8_t,stripe_bytes>,loops> mirrored{};
    bool matches{};
};
struct Ping {
    std::array<std::uint8_t,ping_bytes> braid{};
    std::array<std::uint8_t,ping_bytes> restored{};
    bool roundtrip{};
};
struct Report {
    std::array<Loop,loops> loop{};
    std::array<Minza,minzas> minza{};
    Ninza ninza{};
    Ping ping{};
    swe::pentagonal::vista9x9::Report vista{};
    bool valid{};
};
constexpr unsigned angle(unsigned n, unsigned f) noexcept {
    return (n*9u + f*72u)%360u;
}
constexpr Terminal terminal(unsigned n) noexcept {
    return n%2u==0u ? Terminal::flop : Terminal::flip;
}
constexpr unsigned lane(unsigned column, unsigned position) noexcept {
    const unsigned s=(position+column)%loops;
    return column%2u==0u ? s : loops-1u-s;
}
inline Statistics measure(const FiveSample& sample, std::uint8_t threshold) noexcept {
    Statistics s{};
    auto sorted=sample.pixel;
    unsigned sum=0, active=0;
    for(unsigned i=0;i<fields;++i) {
        sum+=sample.pixel[i];
        active += sample.pixel[i]>=threshold ? 1u : 0u;
    }
    for(unsigned i=0;i<fields;++i)
        for(unsigned j=i+1u;j<fields;++j)
            if(sorted[j]<sorted[i]) {
                const auto old=sorted[i]; sorted[i]=sorted[j]; sorted[j]=old;
            }
    s.minimum=sorted[0];
    s.beta=sorted[2];
    s.climax=sorted[4];
    s.vertex=static_cast<double>(sum)/5.0;
    s.alpha=static_cast<std::uint8_t>(51u*active);
    s.omega.range=static_cast<std::uint8_t>(sorted[4]-sorted[0]);
    unsigned best=0, winners=0;
    for(unsigned i=0;i<fields;) {
        unsigned j=i+1u;
        while(j<fields && sorted[j]==sorted[i])++j;
        const unsigned count=j-i;
        if(count>best) {
            best=count; winners=1; s.omega.mode=sorted[i];
        } else if(count==best) ++winners;
        i=j;
    }
    s.omega.frequency=static_cast<std::uint8_t>(best);
    s.omega.tie=winners>1u;
    return s;
}
inline bool execute(const Input& source, Report& output) noexcept {
    Report result{};
    std::array<swe::pentagonal::vista9x9::Input,loops> gate_inputs{};
    bool reversed=false;
    for(unsigned n=0;n<loops;++n) {
        auto& row=result.loop[n];
        row.index=n;
        row.terminal=terminal(n);
        row.route_reversed_before=reversed;
        for(unsigned f=0;f<fields;++f) row.grating_degrees[f]=angle(n,f);
        row.stats=measure(source.samples[n],source.threshold);
        for(unsigned f=0;f<fields;++f)
            row.ligands[f]=source.samples[n].pixel[f]/255.0;
        row.ligands[5]=row.stats.minimum/255.0;
        row.ligands[6]=row.stats.climax/255.0;
        row.ligands[7]=row.stats.vertex/255.0;
        row.ligands[8]=row.stats.omega.range/255.0;
        double nine[9]{};
        for(unsigned k=0;k<9;++k) nine[k]=row.ligands[k];
        if(!swe::stability10d::calculate(nine,row.three_rings)) return false;
        for(unsigned k=0;k<stripe_bytes;++k) {
            const auto value=row.three_rings.hexabyte[k];
            result.ninza.primary[n][k]=value;
            result.ninza.mirrored[n][k]=value;
        }
        // A five-field binary observation is a separate, auditable view.
        // Additional COR blocks remain empty (NOT independent observations).
        std::uint32_t observed=0u;
        for(unsigned f=0;f<fields;++f)
            if(source.samples[n].pixel[f]>=source.threshold)
                observed |= 1u<<f;
        auto& gate=gate_inputs[n];
        gate.observed=observed;
        gate.expected=source.expected30[n];
        gate.source_token=source.source_tokens[n];
        gate.witness_present=source.witness_claimed;
        if(row.terminal==Terminal::flip) reversed=!reversed;
        row.route_reversed_after=reversed;
    }
    for(unsigned n=0;n<minzas;++n) {
        auto& group=result.minza[n];
        group.index=n;
        for(unsigned d=0;d<minza_depth;++d)
            group.loop_indices[d]=n*minza_depth+d;
    }
    result.vista=swe::pentagonal::vista9x9::execute(gate_inputs);
    result.ninza.matches=true;
    for(unsigned n=0;n<loops;++n)
        for(unsigned col=0;col<stripe_bytes;++col)
            result.ninza.matches = result.ninza.matches &&
                (result.ninza.primary[n][col]==result.ninza.mirrored[n][col]);
    // Invertible stripe permutation, no compression, no added entropy.
    for(unsigned col=0;col<stripe_bytes;++col)
        for(unsigned position=0;position<loops;++position) {
            const unsigned source_lane=lane(col,position);
            const auto v=result.ninza.primary[source_lane][col];
            result.ping.braid[col*loops+position]=v;
            result.ping.restored[source_lane*stripe_bytes+col]=v;
        }
    result.ping.roundtrip=true;
    for(unsigned n=0;n<loops;++n)
        for(unsigned col=0;col<stripe_bytes;++col)
            result.ping.roundtrip=result.ping.roundtrip &&
              (result.ping.restored[n*stripe_bytes+col]==result.ninza.primary[n][col]);
    result.valid=result.vista.control_flow_consistent &&
                 result.vista.extended_ticks==81u &&
                 result.vista.complete_loops==9u &&
                 result.ninza.matches && result.ping.roundtrip;
    if(!result.valid) return false;
    output=result; // no caller input or prior output mutation on rejection
    return true;
}
static_assert(angle(0,0)==0 && angle(0,1)==72 && angle(0,4)==288);
static_assert(angle(8,0)==72 && angle(8,1)==144);
static_assert(terminal(0)==Terminal::flop && terminal(1)==Terminal::flip);
static_assert(loops==minzas*minza_depth);
} // namespace swe::sigma_penta_ping
#endif
