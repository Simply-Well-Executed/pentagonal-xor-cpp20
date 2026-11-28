/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
#include "../shared/THAILONGA3D.hpp"
#include "../shared/NULL0_TOPOLOGY.hpp"
using T=THAILONGA3D;using N=NULL0_TOPOLOGY;
constexpr auto c=T::fold(8,-10,12);
static_assert(c.scale==2&&T::equal(c.primitive,{4,-5,6}));
static_assert(T::AERIS({8,-10,12},c).ping);
static_assert(T::fold(-2147483647-1,0,0).scale==2147483648ULL);
static_assert(T::unfold(T::fold(-2147483647-1,2147483647,0)).valid);
static_assert(T::unfold(T::fold(0,0,0)).valid);
static_assert(!T::unfold({{2,4,6},1}).valid);
static_assert(!T::unfold({{1,0,0},2147483648ULL}).valid);
static_assert(!T::unfold({{1,0,0},0}).valid);
static_assert(!T::audit({8,-10,12},{{4,5,6},2}).ping);
constexpr unsigned bad[5]={16,17,18,19,16},good[5]={16,17,18,19,20};
static_assert(!N::pair(bad).valid&&N::pair(bad).missing==1);
static_assert(N::pair(good).valid);
static_assert(N::active_stars(2047)==180&&N::active_stars(0)==0);
static_assert(N::groups(64).reserve==4&&N::groups(97).reserve==17);
static_assert(N::interface(true,true)&&!N::interface(true,false));
static_assert(N::pole_delta(0,0)==0&&N::pole_bias(0,0)==1048575u);
static_assert(N::energy(120,false).watt_minutes==0);
static_assert(N::energy(119,false).watt_minutes==50);
static_assert(N::energy(90,true).watt_minutes==4200);
static_assert(!N::energy(181,true).valid);
int main(){
 for(int x=-16;x<=16;++x)for(int y=-16;y<=16;++y)for(int z=-16;z<=16;++z){
  const auto w=T::audit({x,y,z},T::fold(x,y,z));
  if(!w.xy||!w.yz||!w.zx||!w.ping)return 1;
 }
 unsigned passes=0,failures=0;
 for(unsigned a=16;a<=20;++a)for(unsigned b=16;b<=20;++b)
 for(unsigned d=16;d<=20;++d)for(unsigned e=16;e<=20;++e)
 for(unsigned f=16;f<=20;++f){
  const unsigned p[5]={a,b,d,e,f};if(N::pair(p).valid)++passes;else ++failures;
 }
 if(passes!=120||failures!=3005)return 2;
 for(unsigned a=0;a<=N::mask;++a)for(unsigned b=0;b<=N::mask;++b)
  if((N::delta(a,b)&N::bias(a,b))||(N::delta(a,b)|N::bias(a,b))!=N::mask)return 3;
 return 0;
}
