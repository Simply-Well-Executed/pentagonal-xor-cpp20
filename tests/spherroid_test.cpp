// SPDX-License-Identifier: 0BSD
#include "../shared/spherroid.hpp"
using S=MSL::SPHERROID;
constexpr double radius2(S::Point p) { return p.x*p.x+p.y*p.y+p.z*p.z; }
constexpr bool witness() {
    constexpr S::Point p{1,1,1};
    const auto zero=S::PROJECT(p,1,0);
    const auto a=S::PROJECT(p,1,1);
    const auto b=S::PROJECT(p,1,100);
    const auto c=S::PROJECT(p,1,1000000);
    const auto mirror=S::PROJECT({-1,-1,-1},1,100);
    const auto axis=S::PROJECT({1,0,0},1,100);
    const auto rect=S::RECTANGLOID(60,15,15,1,100);
    return zero.valid && radius2(zero.projected)==3 &&
           a.valid && b.valid && c.valid && mirror.valid && axis.valid && rect.valid &&
           radius2(a.projected)<3 && radius2(b.projected)<radius2(a.projected) &&
           radius2(c.projected)<radius2(b.projected) && radius2(c.projected)>=1 &&
           radius2(c.projected)<1.000002 &&
           b.source.x==1 && b.source.y==1 && b.source.z==1 &&
           mirror.projected.x==-b.projected.x &&
           axis.projected.x==1 &&
           rect.projected.x==b.projected.x &&
           !S::PROJECT({0,0,0},1,1).valid &&
           !S::PROJECT({1,0,0},-1,1).valid;
}
static_assert(witness());
int main() {
    if(!witness()) return 1;
    for(int i=-10;i<=10;++i) for(int j=-10;j<=10;++j) {
        const S::Point p{1,i/10.0,j/10.0};
        const auto a=S::PROJECT(p,1,1), b=S::PROJECT(p,1,100);
        if(!a.valid || !b.valid || radius2(b.projected)>radius2(a.projected)+1e-12 ||
           radius2(b.projected)<1-1e-12) return 2;
    }
    return 0;
}
