// SPDX-License-Identifier: 0BSD
// Route-name scope guard: validation is not evidence of additional movie tracks.
#include <array>
#include <iostream>
#include <string_view>
using Phase=std::array<std::string_view,3>;
constexpr Phase original={"PADLOCK AND FLY","TOUGH SHIT!","THAILONGA"};
constexpr Phase tri(Phase p){return {p[1],p[2],p[0]};}
constexpr std::array<std::array<unsigned,2>,4> edges={{{1,3},{1,2},{4,2},{4,3}}};
constexpr std::array<int,4> polarity={1,-1,1,-1};
constexpr auto flip(std::array<int,4> p){for(auto&x:p)x=-x;return p;}
static_assert(tri(tri(tri(original)))==original);
static_assert(flip(flip(polarity))==polarity);
static_assert(edges[0]==std::array<unsigned,2>{1,3});
static_assert(edges[1]==std::array<unsigned,2>{1,2});
static_assert(edges[2]==std::array<unsigned,2>{4,2});
static_assert(edges[3]==std::array<unsigned,2>{4,3});
int main(){
 std::array<std::string_view,2> p{original[0],original[1]};
 std::array<std::string_view,2> q{p[1],p[0]};
 if(std::array<std::string_view,2>{q[1],q[0]}!=p)return 1;
 std::cout<<"PASS BiDi involution; TriDi 3-cycle; SigmaDi fixed butterfly graph, alternating polarity, two-flip roundtrip\n";
 std::cout<<"NOT VALIDATED PiDi, BiPi, TriPi, PiPi, SigmaPi: missing frozen operand schemas\n";
 return 0;
}
