// SPDX-License-Identifier: 0BSD
#include "LF9WN9Z.hpp"
namespace lf9wn9z {
using I=long long;
constexpr I mass_mg=9000, radius=10000, side=18*radius;
constexpr I low=radius, high=side-radius, g=980;
constexpr I jump_speed=14700, roll_speed=5000;
struct State { I x,y,z,vy,vz; };
enum Action : unsigned { ZIPITUP=0, ZIPITDOWN=1, ZIPITOUT=2, FUCKITIN=3 };
constexpr State reset(bool ceiling=false) noexcept {
    return {side/2,side/2,ceiling?high:low,0,0};
}
constexpr bool valid(State s) noexcept {
    return s.x>=low && s.x<=high && s.y>=low && s.y<=high &&
        s.z>=low && s.z<=high && s.vy>=-roll_speed && s.vy<=roll_speed &&
        s.vz>=-1000000 && s.vz<=1000000;
}
constexpr bool grounded(State s) noexcept { return s.z==low && s.vz==0; }
constexpr bool action(State& s,unsigned a) noexcept {
    if (!valid(s) || a>FUCKITIN) return false;
    if (a==ZIPITDOWN) return true; // gravity only; no artificial downward impulse
    if (!grounded(s)) return false;
    if (a==ZIPITUP) s.vz=jump_speed;
    else s.vy=a==ZIPITOUT?roll_speed:-roll_speed;
    return true;
}
constexpr State step(State s) noexcept {
    // Exact free-flight integration for constant acceleration, 10ms tick.
    s.y+=s.vy;
    s.z+=s.vz-g/2;
    s.vz-=g;
    // Inelastic end-of-tick wall contacts, no restitution.
    if (s.y<=low) {s.y=low;if(s.vy<0)s.vy=0;}
    if (s.y>=high) {s.y=high;if(s.vy>0)s.vy=0;}
    if (s.z<=low) {s.z=low;if(s.vz<0)s.vz=0;}
    if (s.z>=high) {s.z=high;if(s.vz>0)s.vz=0;}
    return s;
}
constexpr bool fixture() noexcept {
    State s=reset();
    if (!action(s,ZIPITUP)) return false;
    for(unsigned i=0;i<15;++i)s=step(s);
    if(s.z!=120250 || s.vz!=0)return false;
    for(unsigned i=0;i<15;++i)s=step(s);
    if(!grounded(s))return false;
    if(!action(s,ZIPITOUT))return false;
    for(unsigned i=0;i<16;++i)s=step(s);
    if(s.y!=high || s.vy!=0)return false;
    if(!action(s,FUCKITIN))return false;
    for(unsigned i=0;i<32;++i)s=step(s);
    if(s.y!=low || s.vy!=0)return false;
    s=reset(true);
    if(!action(s,ZIPITDOWN) || action(s,ZIPITUP))return false;
    s=step(s);
    if(s.z!=169510 || s.vz!=-980)return false;
    for(unsigned i=1;i<19;++i)s=step(s);
    return grounded(s) && valid(s);
}
static_assert(mass_mg==9000 && side==9*(2*radius) && g%2==0);
static_assert(fixture());
constexpr State load(const I* p) noexcept {return {p[0],p[1],p[2],p[3],p[4]};}
constexpr void store(I* p,State s) noexcept {
    p[0]=s.x;p[1]=s.y;p[2]=s.z;p[3]=s.vy;p[4]=s.vz;
}
}
extern "C" int lf9wn9z_reset(long long* p,unsigned n,unsigned ceiling) noexcept {
    if(!p || n!=5 || ceiling>1)return 0;
    lf9wn9z::store(p,lf9wn9z::reset(ceiling!=0));return 1;
}
extern "C" int lf9wn9z_action(long long* p,unsigned n,unsigned a) noexcept {
    if(!p || n!=5)return 0;
    auto s=lf9wn9z::load(p);
    if(!lf9wn9z::action(s,a))return 0;
    lf9wn9z::store(p,s);return 1;
}
extern "C" int lf9wn9z_step(long long* p,unsigned n,unsigned ticks) noexcept {
    if(!p || n!=5 || ticks>10000)return 0;
    auto s=lf9wn9z::load(p);
    if(!lf9wn9z::valid(s))return 0;
    for(unsigned i=0;i<ticks;++i)s=lf9wn9z::step(s);
    lf9wn9z::store(p,s);return 1;
}
