// SPDX-License-Identifier: 0BSD
#include "LF9WN9Z.hpp"
struct LF9WN9ZCore {
using I=long long;
static constexpr I mass_mg=9000, radius=10000, side=18*radius;
static constexpr I low=radius, high=side-radius, g=980;
static constexpr I jump_speed=14700, roll_speed=5000;
struct State { I x,y,z,vy,vz; };
enum Action : unsigned { ZIPITUP=0, ZIPITDOWN=1, ZIPITOUT=2, FUCKITIN=3 };
static constexpr State reset(bool ceiling=false) noexcept {
    return {side/2,side/2,ceiling?high:low,0,0};
}
static constexpr bool valid(State s) noexcept {
    return s.x>=low && s.x<=high && s.y>=low && s.y<=high &&
        s.z>=low && s.z<=high && s.vy>=-roll_speed && s.vy<=roll_speed &&
        s.vz>=-1000000 && s.vz<=1000000;
}
static constexpr bool grounded(State s) noexcept { return s.z==low && s.vz==0; }
static constexpr bool action(State& s,unsigned a) noexcept {
    if (!valid(s) || a>FUCKITIN) return false;
    if (a==ZIPITDOWN) return true; // gravity only; no artificial downward impulse
    if (!grounded(s)) return false;
    if (a==ZIPITUP) s.vz=jump_speed;
    else s.vy=a==ZIPITOUT?roll_speed:-roll_speed;
    return true;
}
static constexpr State step(State s) noexcept {
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
// DAKATANA-01: step(S)==S at the stationary floor fixed point.
static constexpr bool stationary(State s) noexcept {
    return s.z==low && s.vz==0 && s.vy==0;
}
struct Batch { State state; unsigned evaluated; };
static constexpr Batch advance(State s,unsigned ticks) noexcept {
    unsigned i=0;
    for(;i<ticks;++i) {
        if(stationary(s)) break;
        s=step(s);
    }
    return {s,i};
}
static constexpr bool same(State a,State b) noexcept {
    return a.x==b.x && a.y==b.y && a.z==b.z &&
           a.vy==b.vy && a.vz==b.vz;
}
static constexpr bool redux_fixture() noexcept {
    const State rest=reset();
    if(!same(step(rest),rest) || advance(rest,10000).evaluated!=0)
        return false;
    State jump=rest;
    if(!action(jump,ZIPITUP)) return false;
    const Batch batch=advance(jump,10000);
    // The frozen jump reaches its exact fixed point after 30 ticks.
    return batch.evaluated==30 && same(batch.state,rest);
}
static constexpr bool fixture() noexcept {
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
static constexpr State load(const I* p) noexcept {return {p[0],p[1],p[2],p[3],p[4]};}
static constexpr void store(I* p,State s) noexcept {
    p[0]=s.x;p[1]=s.y;p[2]=s.z;p[3]=s.vy;p[4]=s.vz;
}
};
static_assert(LF9WN9ZCore::mass_mg==9000 && LF9WN9ZCore::side==9*(2*LF9WN9ZCore::radius) && LF9WN9ZCore::g%2==0);
static_assert(LF9WN9ZCore::fixture());
static_assert(LF9WN9ZCore::redux_fixture());
extern "C" int lf9wn9z_reset(long long* p,unsigned n,unsigned ceiling) noexcept {
    if(!p || n!=5 || ceiling>1)return 0;
    LF9WN9ZCore::store(p,LF9WN9ZCore::reset(ceiling!=0));return 1;
}
extern "C" int lf9wn9z_action(long long* p,unsigned n,unsigned a) noexcept {
    if(!p || n!=5)return 0;
    auto s=LF9WN9ZCore::load(p);
    if(!LF9WN9ZCore::action(s,a))return 0;
    LF9WN9ZCore::store(p,s);return 1;
}
extern "C" int lf9wn9z_step(long long* p,unsigned n,unsigned ticks) noexcept {
    if(!p || n!=5 || ticks>10000)return 0;
    auto s=LF9WN9ZCore::load(p);
    if(!LF9WN9ZCore::valid(s))return 0;
    s=LF9WN9ZCore::advance(s,ticks).state;
    LF9WN9ZCore::store(p,s);return 1;
}
