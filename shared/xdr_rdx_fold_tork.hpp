// SPDX-License-Identifier: 0BSD
// XDR-RDX-FOLD-TORK-PR34-010 | experimental, additive, source-preserving
// Two verified 90-degree folded-edge witnesses wrapped with reversible
// XDR6/7/8/9 and RDX9/8/7/6 envelopes, interleaved, then PINGed.
// Each named Di/Pi/Sigma lane is a distinct, auditable local adapter;
// none claims a new canonical math definition or external protocol.
// Requires u_circle_fit.hpp and its preceding source headers first.
// ISO C++20: zero #include, STL, heap, compiler extensions or I/O.
#ifndef SWE_XDR_RDX_FOLD_TORK_HPP
#define SWE_XDR_RDX_FOLD_TORK_HPP
#ifndef SWE_U_CIRCLE_FIT_HPP
#error "Include u_circle_fit.hpp before xdr_rdx_fold_tork.hpp"
#endif
namespace swe::xdr_rdx_fold_tork {
using u64 = unsigned long long;
using s64 = long long;
inline constexpr const char* experiment_id =
    "XDR-RDX-FOLD-TORK-PR34-010";

// =============================================
// SOURCE CORE: exact 90-degree rotation around
// A=(0,0), C=(30,20) of D=(0,20).
// Coordinates x,y are expressed over 13 and
// squared z is expressed over 13^2=169.
// =============================================
static constexpr s64 AX=0,AY=0,CX=30,CY=20,DX=0,DY=20;
static constexpr s64 UX=CX-AX,UY=CY-AY;
static constexpr s64 VX=DX-AX,VY=DY-AY;
static constexpr s64 AXIS2=UX*UX+UY*UY;
static constexpr s64 DOT=UX*VX+UY*VY;
static constexpr s64 CROSS=UX*VY-UY*VX;
static constexpr s64 DEN=13;
static_assert(AXIS2==1300 && DOT==400 && CROSS==600);
static_assert((DEN*UX*DOT)%AXIS2==0);
static_assert((DEN*UY*DOT)%AXIS2==0);
static_assert((CROSS*CROSS*DEN*DEN)%AXIS2==0);
static constexpr s64 X13=DEN*UX*DOT/AXIS2;
static constexpr s64 Y13=DEN*UY*DOT/AXIS2;
static constexpr s64 Z2_169=CROSS*CROSS*DEN*DEN/AXIS2;
static constexpr s64 AD2=VX*VX+VY*VY;
static constexpr s64 CD2=(DX-CX)*(DX-CX)+(DY-CY)*(DY-CY);
static constexpr s64 AD_FOLDED_SIGNED=
    X13*X13+Y13*Y13+Z2_169;
static constexpr s64 CD_FOLDED_SIGNED=
    (DEN*CX-X13)*(DEN*CX-X13)+
    (DEN*CY-Y13)*(DEN*CY-Y13)+Z2_169;
static_assert(X13==120 && Y13==80 && Z2_169==46800);
static_assert(AD2==400 && CD2==900);
static_assert(AD_FOLDED_SIGNED==DEN*DEN*AD2);
static_assert(CD_FOLDED_SIGNED==DEN*DEN*CD2);
static constexpr u64 AD_FOLDED=static_cast<u64>(AD_FOLDED_SIGNED);
static constexpr u64 CD_FOLDED=static_cast<u64>(CD_FOLDED_SIGNED);
static_assert(AD_FOLDED==67600ULL && CD_FOLDED==152100ULL);
static_assert(AD_FOLDED!=CD_FOLDED);

// =============================================
// XDR/RDX6..9: reversible word rotations/XOR.
// These are user-proposed names for an exploratory
// reversible binary envelope, not ONC-XDR or XDR
// network protocol format. Unsigned overflow is
// defined modulo 2^64 by the C++ standard.
// =============================================
[[nodiscard]] constexpr u64 ROTL(u64 v,unsigned shift) noexcept {
    return (v<<shift)|(v>>(64u-shift));
}
[[nodiscard]] constexpr u64 ROTR(u64 v,unsigned shift) noexcept {
    return (v>>shift)|(v<<(64u-shift));
}
[[nodiscard]] constexpr u64 MASK(unsigned stage) noexcept {
    return 0x9E3779B97F4A7C15ULL ^
           (static_cast<u64>(stage)*0x0101010101010101ULL);
}
[[nodiscard]] constexpr u64 XDR(u64 v,unsigned stage) noexcept {
    if (stage<6u||stage>9u)return v; // invalid stages rejected by adapters
    return ROTL(v^MASK(stage),stage);
}
[[nodiscard]] constexpr u64 RDX(u64 v,unsigned stage) noexcept {
    if (stage<6u||stage>9u)return v;
    return ROTR(v,stage)^MASK(stage);
}
[[nodiscard]] constexpr u64 XDR6(u64 v) noexcept {return XDR(v,6u);}
[[nodiscard]] constexpr u64 XDR7(u64 v) noexcept {return XDR(v,7u);}
[[nodiscard]] constexpr u64 XDR8(u64 v) noexcept {return XDR(v,8u);}
[[nodiscard]] constexpr u64 XDR9(u64 v) noexcept {return XDR(v,9u);}
[[nodiscard]] constexpr u64 RDX6(u64 v) noexcept {return RDX(v,6u);}
[[nodiscard]] constexpr u64 RDX7(u64 v) noexcept {return RDX(v,7u);}
[[nodiscard]] constexpr u64 RDX8(u64 v) noexcept {return RDX(v,8u);}
[[nodiscard]] constexpr u64 RDX9(u64 v) noexcept {return RDX(v,9u);}
static_assert(RDX6(XDR6(AD_FOLDED))==AD_FOLDED);
static_assert(RDX7(XDR7(AD_FOLDED))==AD_FOLDED);
static_assert(RDX8(XDR8(CD_FOLDED))==CD_FOLDED);
static_assert(RDX9(XDR9(CD_FOLDED))==CD_FOLDED);

[[nodiscard]] constexpr u64 FORWARD(u64 v) noexcept {
    return XDR9(XDR8(XDR7(XDR6(v))));
}
[[nodiscard]] constexpr u64 REVERSE(u64 v) noexcept {
    return RDX6(RDX7(RDX8(RDX9(v))));
}
static_assert(REVERSE(FORWARD(AD_FOLDED))==AD_FOLDED);
static_assert(FORWARD(REVERSE(CD_FOLDED))==CD_FOLDED);

// =============================================
// Two distinct topological witness envelopes.
// Tags are tamper-evident only within local test
// boundaries, NOT cryptographic signatures.
// =============================================
enum class Source : unsigned { AD=1u, CD=2u, AIRGAP=3u };
struct Lane { u64 value; Source source; };
struct Envelope {
    Lane original;
    u64 encoded;
    u64 tag;
};
static constexpr u64 HEAD_MAGIC=0xA19D9F43C1375E61ULL;
static constexpr u64 TAIL_MAGIC=0xC21D4E176B083A95ULL;
[[nodiscard]] constexpr u64 TAG(Lane original,u64 encoded) noexcept {
    const u64 magic=original.source==Source::AD?HEAD_MAGIC:
                    original.source==Source::CD?TAIL_MAGIC:0ULL;
    return ROTL(original.value^encoded^magic,11u);
}
[[nodiscard]] constexpr Envelope HEAD(u64 value) noexcept {
    const Lane source{value,Source::AD};
    const u64 code=FORWARD(value);
    return {source,code,TAG(source,code)};
}
[[nodiscard]] constexpr Envelope TAIL(u64 value) noexcept {
    const Lane source{value,Source::CD};
    const u64 code=REVERSE(value);
    return {source,code,TAG(source,code)};
}
[[nodiscard]] constexpr bool PING_HEAD(Envelope e) noexcept {
    return e.original.source==Source::AD &&
           e.original.value==AD_FOLDED &&
           e.tag==TAG(e.original,e.encoded) &&
           REVERSE(e.encoded)==e.original.value;
}
[[nodiscard]] constexpr bool PING_TAIL(Envelope e) noexcept {
    return e.original.source==Source::CD &&
           e.original.value==CD_FOLDED &&
           e.tag==TAG(e.original,e.encoded) &&
           FORWARD(e.encoded)==e.original.value;
}
static constexpr Envelope HEAD_TORK=HEAD(AD_FOLDED);
static constexpr Envelope TAIL_TORK=TAIL(CD_FOLDED);
static_assert(PING_HEAD(HEAD_TORK));
static_assert(PING_TAIL(TAIL_TORK));
static_assert(HEAD_TORK.encoded!=TAIL_TORK.encoded);

// =============================================
// BiDi: reversible two-slot interchange.
// TriDi: 3-view forward cycle with independent
// AIRGAP marker; C^3=identity.
// Neither operator changes source identity.
// =============================================
struct BiPair { Lane head,tail; };
[[nodiscard]] constexpr BiPair BIDI(BiPair p) noexcept {
    return {p.tail,p.head};
}
[[nodiscard]] constexpr bool EQUAL(Lane a,Lane b) noexcept {
    return a.source==b.source && a.value==b.value;
}
[[nodiscard]] constexpr bool EQUAL(BiPair a,BiPair b) noexcept {
    return EQUAL(a.head,b.head) && EQUAL(a.tail,b.tail);
}
static constexpr BiPair INPUT_PAIR{
    {AD_FOLDED,Source::AD},{CD_FOLDED,Source::CD}
};
static_assert(EQUAL(BIDI(BIDI(INPUT_PAIR)),INPUT_PAIR));
struct TriView { Lane first,second,third; };
[[nodiscard]] constexpr TriView TRIDI(TriView t) noexcept {
    return {t.second,t.third,t.first};
}
[[nodiscard]] constexpr bool EQUAL(TriView a,TriView b) noexcept {
    return EQUAL(a.first,b.first) &&
           EQUAL(a.second,b.second) &&
           EQUAL(a.third,b.third);
}
static constexpr Lane AIRGAP_MARKER{
    0xA19A6610A19A6610ULL,Source::AIRGAP
};
static constexpr TriView VIEWS{
    INPUT_PAIR.head,INPUT_PAIR.tail,AIRGAP_MARKER
};
static_assert(EQUAL(TRIDI(TRIDI(TRIDI(VIEWS))),VIEWS));
static_assert(!EQUAL(TRIDI(VIEWS),VIEWS));

// =============================================
// PiSigma = UNION two distinct labeled branches,
// TALON separates them without dropping either.
// PiDi = user's finite symbolic pi modulo 4.
// SigmaPi = logical NOT of that symbolic pi.
// No inferred algorithm for numeric PiSigma;
// this is a branch-preserving adapter only.
// =============================================
struct UnionPair { Lane first,second; bool distinct; };
[[nodiscard]] constexpr UnionPair PI_SIGMA(BiPair p) noexcept {
    return {p.head,p.tail,p.head.source!=p.tail.source};
}
[[nodiscard]] constexpr BiPair TALON(UnionPair u) noexcept {
    return {u.first,u.second};
}
static constexpr UnionPair PI_SIGMA_WITNESS=PI_SIGMA(INPUT_PAIR);
static_assert(PI_SIGMA_WITNESS.distinct);
static_assert(EQUAL(TALON(PI_SIGMA_WITNESS),INPUT_PAIR));
static constexpr u64 PI_SYMBOL=314159265358979323ULL;
static constexpr u64 PI_DI=PI_SYMBOL%4ULL;
static constexpr bool SIGMA_PI= !PI_SYMBOL;
static_assert(PI_DI==3ULL && !SIGMA_PI);

// =============================================
// Interleave / deinterleave: a reversible tork
// collision in a local, bounded integer domain.
// We never confuse temporary mixed bytes with
// actual geometric equality. No network or smash.
// =============================================
struct Mixed { u64 head,tail; };
[[nodiscard]] constexpr Mixed COLLIDE(Mixed v,unsigned turn) noexcept {
    const unsigned a=6u+turn%4u,b=9u-turn%4u;
    const u64 h=ROTL(v.head^v.tail,a);
    const u64 t=ROTL(v.tail^h,b);
    return {h,t};
}
[[nodiscard]] constexpr Mixed SEPARATE(Mixed v,unsigned turn) noexcept {
    const unsigned a=6u+turn%4u,b=9u-turn%4u;
    const u64 t=ROTR(v.tail,b)^v.head;
    const u64 h=ROTR(v.head,a)^t;
    return {h,t};
}
[[nodiscard]] constexpr bool EQUAL(Mixed a,Mixed b) noexcept {
    return a.head==b.head && a.tail==b.tail;
}
static constexpr Mixed INPUT_MIXED{
    HEAD_TORK.encoded,TAIL_TORK.encoded
};
static_assert(EQUAL(SEPARATE(COLLIDE(INPUT_MIXED,0u),0u),
                    INPUT_MIXED));
static_assert(EQUAL(SEPARATE(COLLIDE(INPUT_MIXED,3u),3u),
                    INPUT_MIXED));

// =============================================
// SigmaSigma: two independent source checks.
// SigmaSigmaSigma: geometric + topology +
// fully reversed interleaved PING, all required.
// No authority / approval signal is produced.
// =============================================
[[nodiscard]] constexpr bool SIGMA_SIGMA() noexcept {
    return PING_HEAD(HEAD_TORK) && PING_TAIL(TAIL_TORK) &&
           AD_FOLDED==static_cast<u64>(DEN*DEN*AD2) &&
           CD_FOLDED==static_cast<u64>(DEN*DEN*CD2);
}
[[nodiscard]] constexpr bool PING_STRESS(unsigned rounds) noexcept {
    if(rounds==0u || rounds>4096u)return false;
    Mixed live=INPUT_MIXED;
    for(unsigned i=0u;i<rounds;++i)
        live=COLLIDE(live,i);
    for(unsigned i=rounds;i>0u;--i)
        live=SEPARATE(live,i-1u);
    return EQUAL(live,INPUT_MIXED) &&
           PING_HEAD(HEAD_TORK) && PING_TAIL(TAIL_TORK);
}
[[nodiscard]] constexpr bool SIGMA_SIGMA_SIGMA(
    unsigned rounds) noexcept {
    return SIGMA_SIGMA() &&
           EQUAL(BIDI(BIDI(INPUT_PAIR)),INPUT_PAIR) &&
           EQUAL(TRIDI(TRIDI(TRIDI(VIEWS))),VIEWS) &&
           PI_SIGMA_WITNESS.distinct &&
           EQUAL(TALON(PI_SIGMA_WITNESS),INPUT_PAIR) &&
           PI_DI==3ULL && SIGMA_PI==false &&
           PING_STRESS(rounds);
}
static_assert(SIGMA_SIGMA());
static_assert(PING_STRESS(6u));
static_assert(PING_STRESS(7u));
static_assert(PING_STRESS(8u));
static_assert(PING_STRESS(9u));
static_assert(SIGMA_SIGMA_SIGMA(64u));
static_assert(!SIGMA_SIGMA_SIGMA(0u));
static_assert(!PING_STRESS(4097u));

// Fail-closed negative witnesses: payload, role and tag mutations.
[[nodiscard]] constexpr bool CORRUPTION_REJECTED() noexcept {
    Envelope bad_head=HEAD_TORK;
    bad_head.encoded^=1ULL;
    Envelope bad_tail=TAIL_TORK;
    bad_tail.tag^=1ULL;
    Envelope misrole=HEAD_TORK;
    misrole.original.source=Source::CD;
    return !PING_HEAD(bad_head) && !PING_TAIL(bad_tail) &&
           !PING_HEAD(misrole) &&
           !PING_HEAD(TAIL_TORK) && !PING_TAIL(HEAD_TORK);
}
static_assert(CORRUPTION_REJECTED());
inline constexpr bool PHYSICAL_FOLD_INFERRED=false;
inline constexpr bool EXTERNAL_APPROVAL_INFERRED=false;
static_assert(!PHYSICAL_FOLD_INFERRED && !EXTERNAL_APPROVAL_INFERRED);
} // namespace swe::xdr_rdx_fold_tork
#endif // SWE_XDR_RDX_FOLD_TORK_HPP
