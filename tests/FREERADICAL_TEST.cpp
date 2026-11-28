// SPDX-License-Identifier: 0BSD
#include "../shared/FREERADICAL.hpp"
using F=SWE::FREERADICAL;
constexpr F::Character a{30,-1,{0.3,0.4,0.0},{1,0}};
constexpr F::Axis zero{1,0};
constexpr auto r=F::REGISTERING(a,zero);
static_assert(F::PENNYWORTH(a,zero,r));
static_assert(r.first.v.y==-0.4 && r.second.v.y==0.4);
static_assert(r.first.light.exponent==1 && r.second.light.exponent==2);
static_assert(r.first.polarity==-1 && r.second.id==30);
static_assert(F::REGISTERING(a,{0,0}).state==F::State::HELD);
static_assert(F::PENNYWORTH(a,{0,1},F::REGISTERING(a,{0,1})));
constexpr bool corrupt() {
    auto bad=r; bad.first.polarity=1;
    if(F::PENNYWORTH(a,zero,bad))return false;
    auto recovered=F::DOVER(a,zero,bad);
    if(recovered.previous.first.polarity!=1 || !F::PENNYWORTH(a,zero,recovered.repeated))return false;
    bad=r; bad.first.v.x=0.4;
    if(F::PENNYWORTH(a,zero,bad))return false;
    bad=r; bad.second.light.exponent=0;
    if(F::PENNYWORTH(a,zero,bad))return false;
    auto overflow=a; overflow.light.exponent=~0u-1u;
    return F::REGISTERING(overflow,zero).state==F::State::HELD;
}
static_assert(corrupt());
int main(){
    constexpr F::Axis axes[]={{1,0},{0,1},{-1,0},{0,-1},{0.6,0.8},{-0.6,0.8}};
    for(auto axis:axes) for(int x=-10;x<=10;++x) for(int y=-10;y<=10;++y){
        auto v=a; v.v={x/10.0,y/10.0,0.5};
        if(!F::PENNYWORTH(v,axis,F::REGISTERING(v,axis)))return 1;
    }
    return 0;
}
