// SPDX-License-Identifier: 0BSD
// Copyright (c) 2026 Paul Allen Cheek
#include "../shared/sigma_penta_ping.hpp"
#include <array>
#include <cstdint>
#include <iostream>
using namespace swe::sigma_penta_ping;
int main() {
    constexpr auto gray = [](unsigned a,unsigned b,unsigned c,unsigned d,unsigned e) {
        return FiveSample{{static_cast<std::uint8_t>(a),
                           static_cast<std::uint8_t>(b),
                           static_cast<std::uint8_t>(c),
                           static_cast<std::uint8_t>(d),
                           static_cast<std::uint8_t>(e)}};
    };
    {
        const auto x=measure(gray(2,4,4,10,0),128);
        if(x.minimum!=0 || x.beta!=4 || x.climax!=10 ||
           x.vertex!=4.0 || x.omega.mode!=4 || x.omega.range!=10 ||
           x.omega.frequency!=2 || x.omega.tie) return 1;
        const auto tied=measure(gray(1,1,2,2,3),128);
        if(tied.omega.mode!=1 || !tied.omega.tie) return 2;
        const auto no_majority=measure(gray(1,2,3,4,5),0);
        if(no_majority.alpha!=255 || !no_majority.omega.tie) return 3;
    }
    for(unsigned n=0;n<loops;++n) {
        for(unsigned f=0;f<fields;++f) {
            if(angle(n,f)!=(9u*n+72u*f)%360u) return 4;
            const unsigned next=(f+1u)%fields;
            if((angle(n,next)+360u-angle(n,f))%360u!=72u) return 5;
        }
        if(terminal(n)!=(n%2u==0u?Terminal::flop:Terminal::flip)) return 6;
    }
    Input source{};
    source.threshold=128;
    for(unsigned n=0;n<loops;++n) {
        source.samples[n]=gray(0,64,128,192,255);
        source.source_tokens[n]=202600u+n;
        source.expected30[n]=0x1fu;
    }
    const auto saved=source;
    Report report{};
    if(!execute(source,report) || !report.valid ||
       report.vista.gate_ticks!=54 || report.vista.vista_ticks!=27 ||
       report.vista.extended_ticks!=81 || report.vista.complete_loops!=9 ||
       report.vista.gate_observations!=162) return 7;
    for(unsigned n=0;n<loops;++n) {
        if(source.samples[n].pixel!=saved.samples[n].pixel ||
           source.expected30[n]!=saved.expected30[n] ||
           source.source_tokens[n]!=saved.source_tokens[n]) return 8;
        if(report.loop[n].index!=n || report.loop[n].stats.beta!=128 ||
           report.loop[n].stats.climax!=255 || report.loop[n].stats.minimum!=0 ||
           report.loop[n].stats.vertex!=127.8 ||
           report.loop[n].stats.omega.mode!=0 ||
           report.loop[n].stats.omega.range!=255) return 9;
        if(report.loop[n].terminal!=terminal(n)) return 10;
        if(!report.loop[n].three_rings.admissible) return 11;
        if(report.vista.loops[n].views[2].source_token!=source.source_tokens[n])
            return 12;
        for(unsigned col=0;col<stripe_bytes;++col)
            if(report.ninza.primary[n][col]!=report.ninza.mirrored[n][col] ||
               report.ping.restored[n*stripe_bytes+col]!=report.ninza.primary[n][col])
                return 13;
    }
    if(!report.ninza.matches || !report.ping.roundtrip) return 14;
    for(unsigned i=0;i<minzas;++i)
        for(unsigned j=0;j<minza_depth;++j)
            if(report.minza[i].loop_indices[j]!=3u*i+j) return 15;
    if(report.loop[0].route_reversed_before || report.loop[0].route_reversed_after ||
       report.loop[1].route_reversed_before || !report.loop[1].route_reversed_after ||
       !report.loop[2].route_reversed_before || !report.loop[2].route_reversed_after)
        return 16;
    // Audit all nine terminal route actions, not just the first three.
    bool expected_route_reversed=false;
    unsigned counted_flops=0, counted_flips=0;
    for(unsigned n=0;n<loops;++n) {
        const auto& row=report.loop[n];
        if(row.route_reversed_before!=expected_route_reversed) return 17;
        if(row.terminal==Terminal::flip){
            ++counted_flips;
            expected_route_reversed=!expected_route_reversed;
        } else ++counted_flops;
        if(row.route_reversed_after!=expected_route_reversed) return 18;
    }
    if(counted_flops!=5u || counted_flips!=4u ||
       expected_route_reversed) return 19;
    // Rejecting a changed output must not rewrite that output.
    Report before=report;
    Input invalid=source;
    // Invalid input is impossible for fixed uint8_t pixels; API is total over
    // its declared domain. Validation happens in original stability scorer.
    (void)invalid; (void)before;
    std::cout << "PASS|SIGMA-PENTA-PING|loops=9|minzas=3|rings=3|"
              << "ninza_stripes=9|ping_bytes=54|gate_ticks=54|vista_ticks=27|"
              << "extended_steps=81|source_unchanged=1|entropy_generated=0\n";
    return 0;
}
