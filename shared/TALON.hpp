// SPDX-License-Identifier: 0BSD
/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
#ifndef TALON_HPP
#define TALON_HPP
#include "LCD.hpp"

// TALON/1: fixed-atmosphere sphere released from rest. SI units.
// LCD carries the exact normalized drop distance, not the physical force.
struct TALON {
    struct Point { double x,y; };
    static constexpr Point twist(Point u,double s) noexcept {
        return {u.x,u.y*(s+(1.0-s)*u.x)};
    }
    static constexpr double mass=.009, radius=.01, gravity=9.80665;
    static constexpr double density=1.225, cd=.47;
    static constexpr double pi=3.14159265358979323846;
    static constexpr double weight=mass*gravity;
    static constexpr double drag=.5*density*cd*pi*radius*radius;
    static constexpr double vt2=weight/drag;
    static constexpr double energy=.5*mass*vt2;
    static constexpr double length=vt2/(2.0*gravity);
    static constexpr unsigned cycles=256;
    static constexpr double step=4.0;
    static constexpr double abs(double x) noexcept {return x<0?-x:x;}
    // Only called with the bounded nonnegative inputs of this experiment.
    static constexpr double exp_neg(double x) noexcept {
        unsigned folds=0;
        while(x>.125){x*=.5;++folds;}
        double term=1,sum=1;
        for(unsigned i=1;i<=20;++i){term*=x/double(i);sum+=term;}
        double r=1/sum;
        while(folds){r*=r;--folds;}
        return r;
    }
    static constexpr double sqrt_positive(double x) noexcept {
        if(x<=0)return 0;
        double lo=0,hi=x>1?x:1;
        for(unsigned i=0;i<64;++i){
            const double mid=(lo+hi)*.5;
            if(mid>x/mid)hi=mid;else lo=mid;
        }
        return (lo+hi)*.5;
    }
    struct State {double q,dissipated;};
    static constexpr State arirong(State s,double h) noexcept {
        const double d=h/length;
        const double q1=s.q,q2=s.q+d*(1-q1)*.5;
        const double q3=s.q+d*(1-q2)*.5,q4=s.q+d*(1-q3);
        const double avg=(q1+2*q2+2*q3+q4)/6;
        return {s.q+d*(1-avg),s.dissipated+weight*h*avg};
    }
    struct Fusion {
        L lcd;
        double height,speed,potential,kinetic,dissipated,analytical,numerical;
    };
    static constexpr Fusion thailonga(unsigned i,State s) noexcept {
        const double h=step*double(i);
        return {LCD(i,cycles),h,sqrt_positive(s.q*vt2),weight*h,
            energy*s.q,s.dissipated,1-exp_neg(h/length),s.q};
    }
    static constexpr bool ariari(const Fusion& f) noexcept {
        return abs(f.potential-f.kinetic-f.dissipated)<=1e-10;
    }
    static constexpr bool airgap(const Fusion& f) noexcept {
        return abs(f.analytical-f.numerical)<=2e-7 &&
            f.numerical>=0 && f.numerical<=1 &&
            f.dissipated>=0 && ariari(f);
    }
    static constexpr double redux(const Fusion& f) noexcept {
        return f.potential>0?f.dissipated/f.potential:0;
    }
    struct Experiment {bool passed;unsigned completed;Fusion orb;};
    static constexpr Experiment run() noexcept {
        State s{};Fusion f=thailonga(0,s);
        for(unsigned i=1;i<=cycles;++i){
            const double previous=s.q;
            s=arirong(s,step);f=thailonga(i,s);
            if(s.q<previous||!airgap(f))return {false,i-1,f};
        }
        return {true,cycles,f};
    }
};
#endif
