// SPDX-License-Identifier: 0BSD
#include "../shared/physics_lab_geometry.hpp"
#include "../shared/physics_lab_formulium.hpp"
namespace P=SWE::PHYSICS_LAB;
namespace F=P::FORMULIUM;
static_assert(P::INVERSESQUARELAW::LONG(100,2).value==25);
static_assert(P::INVERSESQUARELAW::SHORT(100,2).value==25);
static_assert(!P::INVERSESQUARELAW::LONG(1,0).valid);
static_assert(P::HexaDi::DAMPENING(2,1,1)==2.0/3.0);
static_assert(P::ACCORDIAN::FOLD(0)==1);
static_assert(P::ZEROLOSS::OUTPUT(42u).verified);
constexpr auto s=P::GEOMTRY::SPHERE::PROJECT({1,1,1});
static_assert(s.valid && P::GEOMTRY::SPHERROID::NORM2(s.projected)>0.999999999999 && P::GEOMTRY::SPHERROID::NORM2(s.projected)<1.000000000001);
constexpr auto p=P::GEOMTRY::MATRIX::TWIST(P::GEOMTRY::MATRIX::TWIST({1,2,3}));
static_assert(p.x==1 && p.y==2 && p.z==3);
constexpr auto f=F::LADDERDI::CROSS({1,9},{1,4});
static_assert(f.valid && f.left==4 && f.right==9 && f.denominator==36 && !f.equal);
static_assert(!F::LADDERDI::CROSS({1,0},{1,4}).valid);
static_assert(F::TRIAL::ANSWER({1,9,36},{35,0})==1);
static_assert(F::TRIAL::ANSWER({1,9,36},{36,0})==2);
static_assert(F::TRIAL::ANSWER({1,9,36},{36,1})==1);
constexpr auto a=F::AREA_DEFENSE_CONSULTATION::ASSEMBLE({{1,9,36},{36,0}},{100,0,0},0);
constexpr auto b=F::AREA_DEFENSE_CONSULTATION::ASSEMBLE({{1,9,36},{36,0}},{0,100,0},~0ULL);
static_assert(a.position==b.position && a.witness.shell==b.witness.shell && b.witness.standard==~0ULL);
static_assert(F::NOBODY::CONSULT().unresolved==511);
static_assert(F::GREG::CONSULT(F::LAW{},true,true).unresolved==2047);
static_assert(F::RUN_THE_CLOCK::REMAINING(10,11)==0);
struct ExampleEBC9 { static constexpr unsigned TRANSFORM(unsigned i) noexcept { return i; } };
static_assert(F::MONGOLIAN_THE<ExampleEBC9>::DISCOVER(4).valid);
static_assert(!F::MONGOLIAN_THE<ExampleEBC9>::DISCOVER(13).valid);
constexpr bool masks() {
    for(unsigned u=0;u<2048;++u) {
        const auto r=F::ADVANTAGE_BIAS<11>::VERIFY(u);
        if(!r.valid || (r.resolved&r.unresolved)!=0 || (r.resolved|r.unresolved)!=2047 || r.review!=(u!=0)) return false;
    }
    return !F::ADVANTAGE_BIAS<11>::VERIFY(2048).valid;
}
constexpr bool permutations() {
    unsigned seen=0;
    for(unsigned i=0;i<13;++i) {
        const unsigned j=F::JURY_SAMPLING::SAMPLE(i);
        if(j>=13 || (seen&(1u<<j))) return false;
        seen|=1u<<j;
        if(F::JURY_NULLIFICATION::RESTORE(F::JURY_NULLIFICATION::ROTATE(i))!=i) return false;
    }
    if(seen!=8191 || F::JURY_SAMPLING::SAMPLE(13)!=0) return false;
    for(unsigned c=0;c<6;++c) {
        unsigned bits=0;
        for(unsigned i=0;i<3;++i) {
            const auto x=F::INTERROGATION::AT(c,i);
            if(!x.valid || x.value.d!=1 || (bits&(1u<<x.id))) return false;
            bits|=1u<<x.id;
        }
        if(bits!=7) return false;
    }
    return true;
}
constexpr bool legal() {
    for(unsigned i=0;i<4096;++i) {
        const auto bit=[i](unsigned n) constexpr { return (i&(1u<<n))!=0; };
        const F::LEGAL_SYSTEM::Evidence e{bit(0),bit(1),bit(2),bit(3),bit(4),bit(5),bit(6),bit(7),bit(8),bit(9),bit(10),bit(11)};
        const bool flat=!e.silence&&!e.coercion&&e.voluntary&&(!e.fear||e.proof)&&e.lawful&&e.informed&&!e.bias&&!e.misconduct&&(!e.concern||(e.evaluated&&e.confirmed));
        if(F::LEGAL_SYSTEM::ASSEMBLE(e).stands!=flat) return false;
    }
    return true;
}
static_assert(masks());
static_assert(permutations());
// Full 4096-case check at runtime to avoid compiler constexpr step budgets.
int main() { return masks()&&permutations()&&legal()?0:1; }

constexpr bool crown() {
    P::DANDY d({1,1,1});
    const auto start=d.CROWN.SEAT.CURRENT();
    if(start.x!=1 || start.y!=1 || start.z!=1) return false;
    if(!d.CROWN.FOLD(0) || d.CROWN.SEAT.CURRENT().x!=1) return false;
    if(!d.CROWN.FOLD(1)) return false;
    const auto live=d.CROWN.SEAT.CURRENT();
    if(d.CROWN.FOLD(-1) || d.CROWN.SEAT.CURRENT().x!=live.x) return false;
    const P::LADDERING l(d);
    P::LADDERINGANDY joined(l);
    if(joined.Y.SEAT.CURRENT().x!=live.x || joined.Y.SEAT.SOURCE.x!=1) return false;
    const auto flight=P::DAND::FLIP(d);
    if(!flight.RELEASED || flight.POSITION.x!=-live.x || flight.POSITION.z!=live.z) return false;
    const auto out=joined.OUTPUT();
    return out.valid && out.source.x==1 && P::GEOMTRY::SPHERROID::NORM2(out.projected)>0.999999999999 && P::GEOMTRY::SPHERROID::NORM2(out.projected)<1.000000000001;
}
static_assert(crown());
static_assert(P::GEOMTRY::SPHERROID::PROJECT({1,1,1},1).projected.x==1);
static_assert(P::GEOMTRY::SPHERROID::PROJECT({1,0,0},0).projected.x==1);
static_assert(!P::GEOMTRY::SPHERROID::PROJECT({0,0,0},1).valid);
static_assert(P::LAND::FALL(66,0,1).HEIGHT==49.5);
static_assert(P::LAND::FALL(66,0,2).LANDED);
static_assert(P::LAND::FALL(66,0,2).VELOCITY==-66);
static_assert(!P::LAND::FALL(66,0,-1).VALID);
static_assert(P::MCLEAVE::CLEAVE(2,3).VERIFY());
static_assert(P::MCLEAVE::CLEAVE(2,3).LEFT==-12);
static_assert(P::MCLEAVE::CLEAVE(-2,3).RIGHT==12);
static_assert(P::MCLEAVE::CLEAVE(2,-3).RIGHT==-12);
static_assert(!P::MCLEAVE::CLEAVE(1e101,1).VERIFY());

constexpr bool crest_cycle() {
    const P::CUNT::CONTAIN input[9]={{1,2},{2,3},{3,4},{4,5},{5,6},{6,7},{7,8},{8,9},{9,10}};
    const auto relation=P::RELATION::SELF_CONTAIN(input);
    const auto punched=P::RELTERING::PUNCH(relation);
    for(unsigned i=0;i<9;++i) {
        const auto& c=punched.CRESTS[i];
        if(c.TURN!=i || c.ANGLE!=40*i || c.CONTAINED.SOURCE.S!=input[i].S ||
           c.CONTAINED.SOURCE.Y!=input[i].Y || c.CONTAINED.SOURCE.I!=input[i].S*input[i].S ||
           c.CONTAINED.BIAS!=input[i].S*input[i].S*input[i].Y || !c.CONTAINED.OUTPUT.VERIFY())
            return false;
    }
    auto n=P::NEEDLE::SELF_CONTAIN(input);
    for(unsigned i=0;i<9;++i) {
        if(!n.ROTATE() || n.TURN()!=i+1 || n.POSITION()!=(i+1)%9 || n.ANGLE()!=40*(i+1)) return false;
    }
    if(!n.COMPLETE() || n.ROTATE() || n.SOURCE.CRESTS[8].CONTAINED.BIAS!=810) return false;
    P::TIME_TO_KILL::POSTURE p(punched);
    for(unsigned i=0;i<5;++i) {
        if(!p.NEXT() || p.NEGATED()!=(i==4)) return false;
        const auto v=p.OUTPUT({2,3});
        if(v.X!=(i==4?-2:2) || v.Y!=(i==4?-3:3)) return false;
    }
    if(!p.COMPLETE() || p.NEXT() || p.SOURCE().ANGLE()!=160 || p.SOURCE().POSITION()!=4) return false;
    const auto w=P::CHAOTIC_LAWFUL_GOOD::CONTAIN(p,{2,3},511,{2047});
    return w.COMPLETE && w.GEOMETRY.X==-2 && w.GEOMETRY.Y==-3 &&
           w.WITNESS.CASTED==1536 && w.WITNESS.RECASTED==511 && w.WITNESS.VERIFIED;
}
static_assert(crest_cycle());
static_assert(P::CUNT::S_SYMBOL==0x1D54F && P::CUNT::I_SYMBOL==0x2716);
static_assert(!P::CUNT::SELF_CONTAIN({1e101,1}).OUTPUT.VERIFY());
static_assert(P::RELATED::NEXT(9)==9);
static_assert(P::CHAOTIC_LAWFUL_GOOD::CONTAIN(~0ULL,{~0ULL}).RECASTED==~0ULL);
static_assert(P::CHAOTIC_LAWFUL_GOOD::CONTAIN(42,{0}).CASTED==42);
