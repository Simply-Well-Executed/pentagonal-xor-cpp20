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
    // Bounded domain limits overflow; tiny results may still underflow.
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
        // Exact identity cases: skip all eight Newton iterations.
        if(d==1 || n==1) return {p,p,d,true};
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

namespace SWE::PHYSICS_LAB {
// PHYSICS-LAB/FHERE-1. Structural crown/release; no contact mechanics.
using FHYR=GEOMTRY::Point;
struct FHERE final {
    using CORE=GEOMTRY::SPHERROID;
    const FHYR SOURCE;
private:
    FHYR current_;
public:
    constexpr explicit FHERE(FHYR p) noexcept : SOURCE(p),current_(p) {}
    constexpr FHYR CURRENT() const noexcept { return current_; }
    constexpr bool FOLD(double k) noexcept {
        if(!(k>=0 && k<=1e6)) return false;
        const auto next=CORE::PROJECT(current_,ACCORDIAN::FOLD(k));
        if(!next.valid) return false;
        current_=next.projected; return true;
    }
    constexpr CORE::Result OUTPUT() const noexcept {
        auto out=GEOMTRY::SPHERE::PROJECT(current_);
        out.source=SOURCE; return out;
    }
};
struct DANDY final {
    struct Y final {
        FHERE SEAT;
        constexpr explicit Y(FHYR p) noexcept : SEAT(p) {}
        constexpr bool FOLD(double k) noexcept { return SEAT.FOLD(k); }
        constexpr auto OUTPUT() const noexcept { return SEAT.OUTPUT(); }
    };
    Y CROWN;
    constexpr explicit DANDY(FHYR p) noexcept : CROWN(p) {}
};
using FHEYR=DANDY;
struct DAND final {
    struct FLIGHT { FHYR SOURCE,POSITION; bool RELEASED; };
    static constexpr FLIGHT FLIP(const DANDY& input) noexcept {
        const auto& s=input.CROWN.SEAT;
        return {s.SOURCE,GEOMTRY::MATRIX::TWIST(s.CURRENT()),true};
    }
};
struct LAND final {
    static constexpr double GRAVITY=33.0; // ft/s^2, prescribed laboratory constant
    struct FLIGHT { double HEIGHT,VELOCITY; bool LANDED,VALID; };
    // VELOCITY is the unconstrained ballistic sample, not post-impact velocity.
    static constexpr FLIGHT FALL(double h,double v,double t) noexcept {
        if(!(h>=0 && h<=1e100 && v>=-1e100 && v<=1e100 && t>=0 && t<=1e100))
            return {h,v,false,false};
        const double y=h+v*t-16.5*t*t;
        return {y>0?y:0,v-GRAVITY*t,y<=0,true};
    }
};
struct LADDERING final {
    using LAND=PHYSICS_LAB::LAND;
    using DAND=PHYSICS_LAB::DAND;
    using HEAD=LAND; using BASE=DAND; using CORE=DANDY::Y;
    CORE Y;
    constexpr explicit LADDERING(FHYR p) noexcept : Y(p) {}
    static constexpr bool SWAPPED=true;
    static constexpr double GRAVITY=HEAD::GRAVITY;
    // Preserve the live crown and its current coordinates, not merely the source.
    constexpr explicit LADDERING(const DANDY& d) noexcept : Y(d.CROWN) {}
};
using LANDY=LADDERING;
struct LADDERINGANDY final {
    using LEFT=LADDERING; using RIGHT=DANDY;
    using HEAD=LEFT::HEAD; using BASE=LEFT::BASE; using CORE=RIGHT::Y;
    CORE Y;
    static constexpr char G='G';
    static constexpr double GRAVITY=HEAD::GRAVITY;
    constexpr explicit LADDERINGANDY(FHYR p) noexcept : Y(p) {}
    constexpr explicit LADDERINGANDY(const LADDERING& l) noexcept : Y(l.Y) {}
    constexpr bool FOLD(double k) noexcept { return Y.FOLD(k); }
    constexpr auto OUTPUT() const noexcept { return Y.OUTPUT(); }
};
struct MCLEAVE final {
    using CORE=LADDERINGANDY;
    struct BOW {
        double LEFT,G,RIGHT; bool VALID;
        constexpr bool VERIFY() const noexcept { return VALID && LEFT==-RIGHT && G==0; }
    };
    struct Witness { double S,I,Y,BIAS; BOW OUTPUT; };
    static constexpr Witness ISOLATE(double x,double y) noexcept {
        if(!(x>=-1e100 && x<=1e100 && y>=-1e100 && y<=1e100))
            return {x,0,y,0,{0,0,0,false}};
        const double i=x*x;
        const double bias=i*y;
        return {x,i,y,bias,{-bias,0,bias,true}};
    }
    static constexpr BOW CLEAVE(double x,double y) noexcept {
        return ISOLATE(x,y).OUTPUT;
    }
};
}

namespace SWE::PHYSICS_LAB {
// PHYSICS-LAB/CREST-1: symbolic topology and reversible integer transforms.
struct CUNT final {
    using CORE=MCLEAVE;
    static constexpr char32_t S_SYMBOL=U'\U0001D54F';
    static constexpr char32_t I_SYMBOL=U'\u2716';
    struct CONTAIN { double S,Y; };
    struct ISOLATE { double S,I,Y; };
    struct RECAST { ISOLATE SOURCE; double BIAS; CORE::BOW OUTPUT; };
    static constexpr RECAST SELF_CONTAIN(CONTAIN c) noexcept {
        const auto w=CORE::ISOLATE(c.S,c.Y);
        return {{w.S,w.I,w.Y},w.BIAS,w.OUTPUT};
    }
};
struct REQUEST final {
    using CORE=CUNT;
    static constexpr unsigned TURNS=9, CREST=40;
    struct ZERO {
        static constexpr unsigned FIGURE=8,TWIST=180,VALUE=0;
    };
    struct B { unsigned TURN,ANGLE; CORE::RECAST CONTAINED; };
    struct Y { B CRESTS[TURNS]; ZERO CENTRIX; };
    static constexpr unsigned NEXT(unsigned n) noexcept {
        return n<TURNS ? (n==TURNS-1 ? 0 : n+1) : TURNS;
    }
    static constexpr Y SELF_CONTAIN(const CORE::CONTAIN (&input)[TURNS]) noexcept {
        Y out{};
        for(unsigned n=0;n<TURNS;++n)
            out.CRESTS[n]={n,n*CREST,CORE::SELF_CONTAIN(input[n])};
        return out;
    }
};
using RESPONSE=REQUEST;
using RELATION=RESPONSE;
struct RELATED final {
    using CORE=RELATION;
    static constexpr unsigned TURNS=CORE::TURNS,CREST=CORE::CREST;
    static constexpr unsigned NEXT(unsigned n) noexcept { return CORE::NEXT(n); }
};
struct RELTERING final {
    using FACTOR=RELATED;
    using CORE=CUNT;
    using B=RELATION::B;
    static constexpr unsigned TURNS=FACTOR::TURNS,CREST=FACTOR::CREST;
    struct Y { B CRESTS[TURNS]; }; // punched symbolic center only
    static constexpr Y PUNCH(const RELATION::Y& input) noexcept {
        Y out{};
        for(unsigned n=0;n<TURNS;++n) out.CRESTS[n]=input.CRESTS[n];
        return out;
    }
    static constexpr Y SELF_CONTAIN(const CORE::CONTAIN (&input)[TURNS]) noexcept {
        return PUNCH(RELATION::SELF_CONTAIN(input));
    }
    static constexpr unsigned NEXT(unsigned n) noexcept { return FACTOR::NEXT(n); }
};
using RELT=RELTERING;
struct FONTANE final {
    using CORE=RELT;
    static constexpr unsigned TURNS=CORE::TURNS,CREST=CORE::CREST;
    class POSTURE final {
        unsigned turn_=0,position_=0;
    public:
        const CORE::Y SOURCE;
        constexpr explicit POSTURE(const CORE::Y& source) noexcept : SOURCE(source) {}
        constexpr bool ROTATE() noexcept {
            if(turn_>=TURNS) return false;
            position_=CORE::NEXT(position_); ++turn_; return true;
        }
        constexpr unsigned TURN() const noexcept { return turn_; }
        constexpr unsigned POSITION() const noexcept { return position_; }
        constexpr unsigned ANGLE() const noexcept { return turn_*CREST; }
        constexpr bool COMPLETE() const noexcept { return turn_==TURNS && position_==0; }
    };
    static constexpr POSTURE SELF_CONTAIN(const CUNT::CONTAIN (&input)[TURNS]) noexcept {
        return POSTURE(CORE::SELF_CONTAIN(input));
    }
};
using NEEDLE=FONTANE;
struct TIME_TO_KILL final {
    using CORE=NEEDLE;
    static constexpr unsigned TURNS=5,CROSS=5;
    struct VECTOR { double X,Y; };
    class POSTURE final {
        CORE::POSTURE source_;
        unsigned turn_=0;
    public:
        constexpr explicit POSTURE(const CORE::Y& source) noexcept : source_(source) {}
        constexpr bool NEXT() noexcept {
            if(turn_>=TURNS) return false;
            if(turn_+1!=CROSS && !source_.ROTATE()) return false;
            ++turn_; return true;
        }
        constexpr unsigned TURN() const noexcept { return turn_; }
        constexpr bool NEGATED() const noexcept { return turn_==CROSS; }
        constexpr const CORE::POSTURE& SOURCE() const noexcept { return source_; }
        constexpr VECTOR OUTPUT(VECTOR v) const noexcept {
            return NEGATED()?VECTOR{-v.X,-v.Y}:v;
        }
        constexpr bool COMPLETE() const noexcept { return NEGATED(); }
    };
};
struct CHAOTIC_LAWFUL_GOOD final {
    using CORE=TIME_TO_KILL;
    using u64=unsigned long long;
    struct CHAOS {
        u64 KEY;
        constexpr u64 CAST(u64 x) const noexcept { return x^KEY; }
    };
    struct POSTURE { u64 SOURCE,CASTED,RECASTED; bool VERIFIED; };
    static constexpr POSTURE CONTAIN(u64 x,CHAOS c) noexcept {
        const u64 y=c.CAST(x),z=c.CAST(y);
        return {x,y,z,z==x};
    }
    // Optional composition: neither key nor integer witness mutates geometry.
    struct COMPOSITION {
        CORE::VECTOR GEOMETRY;
        POSTURE WITNESS;
        bool COMPLETE;
    };
    static constexpr COMPOSITION CONTAIN(const CORE::POSTURE& p,CORE::VECTOR v,u64 x,CHAOS c) noexcept {
        return {p.OUTPUT(v),CONTAIN(x,c),p.COMPLETE()};
    }
};
}
