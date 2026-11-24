// SPDX-License-Identifier: 0BSD
// MINZA color-eye reference tests; independent of image parsing or identity.
#include "../shared/MINZA_Color_Eyes_Gate_Core_CXX20.hpp"
#include <array>
#include <cstdint>
#include <iostream>
#include <limits>

using namespace minza::color_eyes;

int main() {
    for (unsigned m=0; m<32; ++m) {
        unsigned ones=0;
        for (unsigned k=0; k<5; ++k) ones+=(m>>k)&1u;
        const gate5 g=evaluate(static_cast<std::uint8_t>(m));
        if (g.ligand_mask!=m || g.xor_odd!=(ones%2u==1u) ||
            g.any_or!=(ones!=0u) || g.all_and!=(ones==5u)) return 1;
    }
    constexpr rgba8 red{255,0,0,255};
    constexpr auto expanded=expand_rgba16(red);
    constexpr auto packed=pack_rgba64(red);
    static_assert(expanded.r==65535 && expanded.g==0 && expanded.a==65535);
    static_assert(packed==0xFFFF00000000FFFFull);
    static_assert(unpack_rgba64(packed).r==65535u);
    static_assert(window5(0,67)==std::array<std::size_t,5>{65,66,0,1,2});
    static_assert(window5(66,67)==std::array<std::size_t,5>{64,65,66,0,1});
    static_assert(window5(0,1)==std::array<std::size_t,5>{0,0,0,0,0});
    static_assert(window5(0,2)==std::array<std::size_t,5>{0,1,0,1,0});
    static_assert(window5(0,0)==std::array<std::size_t,5>{0,0,0,0,0});
    const auto max_count=std::numeric_limits<std::size_t>::max();
    const auto near_max=window5(max_count-1,max_count);
    if (near_max[0]!=max_count-3 || near_max[1]!=max_count-2 ||
        near_max[2]!=max_count-1 || near_max[3]!=0 || near_max[4]!=1) return 2;
    constexpr std::array<rgba8,5> all_black{{{0,0,0,255},{0,0,0,255},
        {0,0,0,255},{0,0,0,255},{0,0,0,255}}};
    static_assert(edge_mask(all_black,0)==31);
    constexpr std::array<rgba8,5> broken{{{0,0,0,255},{255,0,0,255},
        {0,0,0,255},{0,0,0,255},{0,0,0,255}}};
    static_assert(edge_mask(broken,0)==0x1C);
    static_assert(edge_mask(broken,85)==31);
    constexpr auto t=femtoseconds{153'000'000'000'000'000LL};
    static_assert(sample_interval{t,t+femtoseconds{100}}.contains(t));
    static_assert(!sample_interval{t,t+femtoseconds{100}}.contains(t+femtoseconds{100}));
    std::cout<<"PASS: 32 gates, RGBA64, 5-ring wrap, edge mask, half-open intervals\n";
    return 0;
}
