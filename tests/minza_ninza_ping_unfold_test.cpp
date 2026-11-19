// SPDX-License-Identifier: 0BSD
// Copyright (c) 2026 Paul Allen Cheek
#include "../shared/minza_ninza_ping_unfold.hpp"
#include <cstdint>
#include <iostream>
using namespace swe::sigma_penta_ping;
using namespace swe::sigma_penta_ping::unfold;
int main(){
    Ninza n{};Ping p{};
    for(unsigned i=0;i<loops;++i)
        for(unsigned c=0;c<stripe_bytes;++c){
            const auto value=static_cast<std::uint8_t>(i*stripe_bytes+c);
            n.primary[i][c]=value;n.mirrored[i][c]=value;
            p.restored[i*stripe_bytes+c]=value;
        }
    for(unsigned c=0;c<stripe_bytes;++c)
        for(unsigned pos=0;pos<loops;++pos)
            p.braid[c*loops+pos]=n.primary[lane(c,pos)][c];
    const Ninza original_n=n;
    const Ping original_p=p;
    const Unforged u=unforge(n);
    if(!u.copies_match || u.mismatched_bytes!=0u) return 1;
    const Untwirled t=untwirl(p,n);
    if(!t.one_to_one_mapping || !t.roundtrip_vs_primary ||
       t.mismatched_vs_primary || t.mismatched_vs_cached_restored) return 2;
    for(unsigned c=0;c<stripe_bytes;++c)
        for(unsigned pos=0;pos<loops;++pos){
            if(t.lane_by_column_position[c][pos]!=lane(c,pos) ||
               t.original_flat_offset[c*loops+pos]!=lane(c,pos)*stripe_bytes+c)
                return 3;
        }
    if(n.primary!=original_n.primary || n.mirrored!=original_n.mirrored ||
       p.braid!=original_p.braid || p.restored!=original_p.restored) return 4;
    Ninza changed=n;changed.mirrored[8][5]^=0x20u;
    const Unforged mismatch=unforge(changed);
    if(mismatch.copies_match || mismatch.mismatched_bytes!=1u ||
       mismatch.xor_difference[8][5]!=0x20u ||
       n.mirrored[8][5]!=original_n.mirrored[8][5]) return 5;
    Ping damaged=p;damaged.braid[17]^=0x80u;
    const Untwirled bad=untwirl(damaged,n);
    if(!bad.one_to_one_mapping || bad.roundtrip_vs_primary ||
       bad.mismatched_vs_primary!=1u ||
       bad.mismatched_vs_cached_restored!=1u) return 6;
    std::cout<<"PASS NINZA split: 9x6 mirror bytes; PING inverse: 54 byte bijection; faults exposed, sources unchanged\n";
    return 0;
}
