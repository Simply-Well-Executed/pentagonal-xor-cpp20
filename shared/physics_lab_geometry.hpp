// SPDX-License-Identifier: 0BSD
#pragma once
// PHYSICS-LAB/GEOMTRY-1. Bounded numerical experiments, not physical hardware.
namespace SWE::PHYSICS_LAB {
struct HexaDi final {
    static constexpr double ABS(double x) noexcept { return x<0?-x:x; }
    static constexpr double DAMPENING(double u,double k,double t) noexcept {
        return u/(1+k*ABS(u)*t); // precondition: bounded finite u,k,t; k,t >= 0
    }
};
struct INVERSESQUARELAW final {
    // Bounded domain avoids overflow/underflow in both evaluation orders.
    struct Result { double value; bool valid; };
    static constexpr bool DOMAIN(double k,double r) noexcept {
        return k>=-1e100 && k<=1e100 && r>=1e-100 && r<=1e100;
    }
    static constexpr Result LONG(double k,double r) noexcept {
        if(!DOMAIN(k,r)) return {0,false};
        return {k/(r*r),true};
    }
    static constexpr Result SHORT(double k,double r) noexcept {
        if(!DOMAIN(k,r)) return {0,false};
        const double i=1/r;
        return {k*i*i,true};
    }
};
struct ACCORDIAN final {
    static constexpr double TENZ=300; // ohms, project-local parameter
    static constexpr double FOLD(double k) noexcept { return 1/(1+k*TENZ); }
};
namespace GEOMTRY {
struct Point { double x,y,z; };
struct MATRIX final {
    static constexpr Point TWIST(Point p) noexcept { return {-p.x,-p.y,p.z}; }
    static constexpr Point QUARTER(Point p) noexcept { return {-p.y,p.x,p.z}; }
};
struct TWINAX final {
    double left,right;
    constexpr double VERIFY() const noexcept { return left-right; }
    constexpr double BUTTERFLY() const noexcept { const double v=VERIFY(); return v*v; }
};
struct SPHERROID final {
    struct Result { Point source,projected; double residual; bool valid; };
    static constexpr double NORM2(Point p) noexcept { return p.x*p.x+p.y*p.y+p.z*p.z; }
    static constexpr Result PROJECT(Point p,double d) noexcept {
        const double n=NORM2(p);
        if(!(n>=0.999999999999 && n<=3.000000000001 && d>=0 && d<=1))
            return {p,p,d,false};
        double r=2;
        for(unsigned i=0;i<8;++i) r=(r+n/r)*0.5;
        const double s=d+(1-d)/r;
        return {p,{p.x*s,p.y*s,p.z*s},d,true};
    }
};
struct SPHERE final {
    static constexpr SPHERROID::Result PROJECT(Point p) noexcept {
        return SPHERROID::PROJECT(p,0);
    }
};
struct BOX final {
    TWINAX B{};
    static constexpr double O=0;
    constexpr double X() const noexcept { return B.BUTTERFLY(); }
    static constexpr SPHERROID::Result PUSH(Point p,double k) noexcept {
        if(!(k>=0 && k<=1e6)) return {p,p,0,false};
        return SPHERROID::PROJECT(p,ACCORDIAN::FOLD(k));
    }
};
struct GEOUT final {
    Point O;
    constexpr bool FOLD(double d) noexcept {
        const auto r=SPHERROID::PROJECT(O,d);
        if(!r.valid) return false;
        O=r.projected; return true;
    }
    constexpr SPHERROID::Result OUTPUT() const noexcept { return SPHERE::PROJECT(O); }
};
struct POSTURE final {
    Point source,complement;
    static constexpr POSTURE ASSEMBLE(Point p) noexcept { return {p,MATRIX::TWIST(p)}; }
};
}
struct PETER final {
    template<class T> static constexpr T REPEAT(T x) noexcept { return x; }
};
struct PAUL final {
    template<class T> static constexpr T TRANSFER(T x) noexcept { return PETER::REPEAT(x); }
};
struct BUDDHA final {
    static constexpr double BIAS=0,LOSS=0,GAIN=1;
    using LEFT=PETER; using RIGHT=PAUL;
    template<class T> static constexpr T OUTPUT(T x) noexcept { return RIGHT::TRANSFER(x); }
};
struct ZEROLOSS final {
    template<class T> struct Result { T input,output; bool verified; };
    template<class T> static constexpr Result<T> OUTPUT(T x) noexcept {
        const T y=BUDDHA::OUTPUT(x); return {x,y,x==y};
    }
};
using REPEATER=PETER;
using BIASED_REPEATER=PETER;
using FRACTURED_REPEATER=PETER;
using REREPEATER=PETER;
using HELL=ZEROLOSS; using SANCTUARY=ZEROLOSS; using BAAL=ZEROLOSS;
using NIGHTMARE=ZEROLOSS; using DREAM=ZEROLOSS; using MOONWALK=ZEROLOSS;
using LUMICITIZER=ZEROLOSS; using SOLARPLEX=ZEROLOSS; using SUNDIAL=ZEROLOSS;
using MONWELL=ZEROLOSS; using VAGER=ZEROLOSS; using NIE=ZEROLOSS;
using CADENCE=ZEROLOSS; using FARLOST=ZEROLOSS; using NEVERNEVER=ZEROLOSS;
using WANDERING=ZEROLOSS; using DISCOBLAME=ZEROLOSS; using TOOMUCH=ZEROLOSS;
using INF=ZEROLOSS; using FOXTUNE=ZEROLOSS; using JAMAICANSWITCH=ZEROLOSS;
using LOSERTALES=ZEROLOSS; using SUBMISSIVELOVE=ZEROLOSS; using jack=ZEROLOSS;
using LOVE=ZEROLOSS; using LUST=ZEROLOSS; using AGONY=ZEROLOSS; using HATE=ZEROLOSS;
}
