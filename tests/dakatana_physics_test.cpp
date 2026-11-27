// SPDX-License-Identifier: 0BSD
// DAKATANA-01: independent literal tick oracle for the public DLL ABI.
#include "../docs/experiments/LF9WN9Z/LF9WN9Z.hpp"
#include "../shared/redux.hpp"
static_assert(REDUX::dakatana(10000,30)==REDUX::Dakatana::qualified);
static_assert(REDUX::dakatana(10000,0)==REDUX::Dakatana::eliminated);
using I=long long;
struct S { I x,y,z,vy,vz; };
constexpr I lo=10000,hi=170000;
static void tick(S& s) noexcept {
    s.y+=s.vy; s.z+=s.vz-490; s.vz-=980;
    if(s.y<=lo){s.y=lo;if(s.vy<0)s.vy=0;}
    if(s.y>=hi){s.y=hi;if(s.vy>0)s.vy=0;}
    if(s.z<=lo){s.z=lo;if(s.vz<0)s.vz=0;}
    if(s.z>=hi){s.z=hi;if(s.vz>0)s.vz=0;}
}
static bool compare(S s,unsigned n) noexcept {
    I p[5]={s.x,s.y,s.z,s.vy,s.vz};
    for(unsigned i=0;i<n;++i)tick(s);
    return lf9wn9z_step(p,5,n)==1 && p[0]==s.x && p[1]==s.y &&
           p[2]==s.z && p[3]==s.vy && p[4]==s.vz;
}
int main(){
    constexpr I pos[]={lo,lo+1,90000,hi-1,hi};
    constexpr I horizontal[]={-5000,-1,0,1,5000};
    constexpr I vertical[]={-1000000,-14700,-1,0,1,14700,1000000};
    constexpr unsigned ticks[]={0,1,2,15,30,31,100,1000,10000};
    // 5*5*5*7*9=7875 deterministic cases, including contacts and extremes.
    for(I y:pos)for(I z:pos)for(I vy:horizontal)for(I vz:vertical)
        for(unsigned n:ticks)if(!compare({90000,y,z,vy,vz},n))return 1;
    I p[5]={90000,90000,10000,0,0};
    if(lf9wn9z_step(nullptr,5,1)||lf9wn9z_step(p,4,1)||
       lf9wn9z_step(p,5,10001))return 2;
    if(p[0]!=90000||p[1]!=90000||p[2]!=10000||p[3]!=0||p[4]!=0)return 3;
    p[2]=9999;
    if(lf9wn9z_step(p,5,10000)||p[2]!=9999)return 4;
    if(!lf9wn9z_reset(p,5,0)||!lf9wn9z_action(p,5,0)||
       !lf9wn9z_step(p,5,10000)||p[2]!=10000||p[4]!=0)return 5;
    return 0;
}
