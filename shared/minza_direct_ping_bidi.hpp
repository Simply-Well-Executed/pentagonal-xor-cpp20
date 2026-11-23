// SPDX-License-Identifier: 0BSD
// MINZA-DIRECT-PING-BIDI-PR34-011 | EXPLORATORY read-only adapter
// Direct 90-degree hinge fold from the frozen nine-triangle grid:
// MINZA (source geometry) -> exact squared-distance PING.
// NINZA intermediary is not consulted, skipped for this sample ONLY.
// Then BiDi swaps AD/CD labeled lanes and swaps them back.
// All provenance, previous source headers, and governance remain intact.
// Include u_circle_fit.hpp, xdr_rdx_fold_tork.hpp and their prerequisites.
// ISO C++20; no includes, STL, heap, floats, compiler extensions or I/O.
#ifndef SWE_MINZA_DIRECT_PING_BIDI_HPP
#define SWE_MINZA_DIRECT_PING_BIDI_HPP
#ifndef SWE_XDR_RDX_FOLD_TORK_HPP
#error "Include xdr_rdx_fold_tork.hpp before this header"
#endif
namespace swe::minza_direct_ping_bidi {
using s64=long long;
using u64=unsigned long long;
namespace grid=swe::u_circle_fit;
namespace tork=swe::xdr_rdx_fold_tork;
inline constexpr const char* experiment_id=
    "MINZA-DIRECT-PING-BIDI-PR34-011";

// =============================================
// Frozen source geometry: strip zero, triangles
// ABC=(0,0),(60,0),(30,20) (large)
// ADC=(0,0),(0,20),(30,20) (small)
// Shared hinge A--C; D is the moving vertex.
// =============================================
static constexpr grid::Triangle2 LARGE=grid::BASE9.face[0];
static constexpr grid::Triangle2 SMALL=grid::BASE9.face[1];
static constexpr grid::Point A=LARGE.a;
static constexpr grid::Point C=LARGE.c;
static constexpr grid::Point D=SMALL.b;
static constexpr s64 DEN=13;
static constexpr s64 DEN2=DEN*DEN;

[[nodiscard]] constexpr bool HINGE_VALID(
    grid::Triangle2 large,grid::Triangle2 small) noexcept {
    return grid::point_valid(large.a) &&
           grid::point_valid(large.b) &&
           grid::point_valid(large.c) &&
           grid::point_valid(small.a) &&
           grid::point_valid(small.b) &&
           grid::point_valid(small.c) &&
           grid::DOUBLE_AREA(large)!=0ULL &&
           grid::DOUBLE_AREA(small)!=0ULL &&
           large.a.x==small.a.x &&
           large.a.y==small.a.y &&
           large.c.x==small.c.x &&
           large.c.y==small.c.y &&
           // This exact historical sample has a pinned denominator 13.
           large.a.x==0 && large.a.y==0 &&
           large.b.x==60 && large.b.y==0 &&
           large.c.x==30 && large.c.y==20 &&
           small.b.x==0 && small.b.y==20;
}
static_assert(HINGE_VALID(LARGE,SMALL));
static_assert(!HINGE_VALID(LARGE,grid::Triangle2{
    SMALL.a,SMALL.b,{31,20}}));

// =============================================
// MINZA -> PING: one-step geometric derivation.
//
// u=C-A, v=D-A. The 90-degree folded vertex
// is D' = A + u*(dot(u,v)/|u|^2) + z*k,
// z^2 = cross(u,v)^2/|u|^2.
// In the sample, store x and y over 13,
// and squared z over 13^2=169.
// No square roots, Trig, NINZA state or floats.
// =============================================
struct DirectFold {
    s64 x13,y13,z2_169;
    u64 ad2,cd2;
    u64 ad_folded,cd_folded;
    bool source_hinge_valid;
    bool dot_cross_verified;
    bool length_preserved;
    bool valid;
};
[[nodiscard]] constexpr DirectFold MINZA_TO_PING() noexcept {
    if (!HINGE_VALID(LARGE,SMALL))
        return {};
    const s64 ux=C.x-A.x,uy=C.y-A.y;
    const s64 vx=D.x-A.x,vy=D.y-A.y;
    const s64 axis2=ux*ux+uy*uy;
    const s64 dot=ux*vx+uy*vy;
    const s64 cross=ux*vy-uy*vx;
    if(axis2!=1300 || dot!=400 || cross!=600 ||
       (DEN*ux*dot)%axis2!=0 ||
       (DEN*uy*dot)%axis2!=0 ||
       (cross*cross*DEN2)%axis2!=0)
        return {};
    const s64 x13=DEN*A.x+(DEN*ux*dot)/axis2;
    const s64 y13=DEN*A.y+(DEN*uy*dot)/axis2;
    const s64 z2_169=(cross*cross*DEN2)/axis2;
    const s64 ad2=vx*vx+vy*vy;
    const s64 wx=D.x-C.x,wy=D.y-C.y;
    const s64 cd2=wx*wx+wy*wy;
    const s64 ax13=DEN*A.x,ay13=DEN*A.y;
    const s64 cx13=DEN*C.x,cy13=DEN*C.y;
    const s64 ad_f=(x13-ax13)*(x13-ax13)+
                   (y13-ay13)*(y13-ay13)+z2_169;
    const s64 cd_f=(x13-cx13)*(x13-cx13)+
                   (y13-cy13)*(y13-cy13)+z2_169;
    const bool dotcross=dot*dot+cross*cross==axis2*ad2 &&
        (x13-ax13)*ux+(y13-ay13)*uy==DEN*dot &&
        z2_169>0;
    const bool lengths=ad_f==DEN2*ad2 &&
                       cd_f==DEN2*cd2;
    const bool valid=dotcross && lengths &&
        x13==120 && y13==80 && z2_169==46800 &&
        ad2==400 && cd2==900 &&
        ad_f==67600 && cd_f==152100;
    return {x13,y13,z2_169,
            static_cast<u64>(ad2),static_cast<u64>(cd2),
            static_cast<u64>(ad_f),static_cast<u64>(cd_f),
            true,dotcross,lengths,valid};
}
static constexpr DirectFold FOLD=MINZA_TO_PING();
static_assert(FOLD.valid && FOLD.source_hinge_valid);
static_assert(FOLD.dot_cross_verified && FOLD.length_preserved);
static_assert(FOLD.x13==120 && FOLD.y13==80 &&
              FOLD.z2_169==46800);
static_assert(FOLD.ad_folded==67600ULL &&
              FOLD.cd_folded==152100ULL);

// =============================================
// PING: compare independent direct geometry
// against previous historical fold witnesses.
// This checks both lengths and distinct identities.
// =============================================
[[nodiscard]] constexpr bool PING(DirectFold x) noexcept {
    return x.valid && x.source_hinge_valid &&
        x.dot_cross_verified && x.length_preserved &&
        x.x13==FOLD.x13 && x.y13==FOLD.y13 &&
        x.z2_169==FOLD.z2_169 &&
        x.ad2==400ULL && x.cd2==900ULL &&
        x.ad_folded==DEN2*x.ad2 &&
        x.cd_folded==DEN2*x.cd2 &&
        x.ad_folded==tork::AD_FOLDED &&
        x.cd_folded==tork::CD_FOLDED &&
        x.ad_folded!=x.cd_folded;
}
static_assert(PING(FOLD));

// =============================================
// BiDi: labeled HEAD / TAIL swap and undo.
// Source identifiers are NOT changed by swaps.
// Reusing the audited prior BIDI keeps a single
// source definition for the reversible operation.
// =============================================
struct BiDiReport {
    tork::BiPair original,swapped,restored;
    bool bijective;
    bool ping_after_restore;
};
[[nodiscard]] constexpr bool PING_PAIR(tork::BiPair pair) noexcept {
    return pair.head.source==tork::Source::AD &&
           pair.tail.source==tork::Source::CD &&
           pair.head.value==tork::AD_FOLDED &&
           pair.tail.value==tork::CD_FOLDED;
}
[[nodiscard]] constexpr BiDiReport BIDI(DirectFold x) noexcept {
    const tork::BiPair original{
        {x.ad_folded,tork::Source::AD},
        {x.cd_folded,tork::Source::CD}
    };
    const auto swapped=tork::BIDI(original);
    const auto restored=tork::BIDI(swapped);
    const bool bijective=tork::EQUAL(original,restored) &&
        swapped.head.source==tork::Source::CD &&
        swapped.tail.source==tork::Source::AD &&
        swapped.head.value==x.cd_folded &&
        swapped.tail.value==x.ad_folded;
    return {original,swapped,restored,bijective,
            PING(x) && bijective && PING_PAIR(restored)};
}
static constexpr BiDiReport REPORT=BIDI(FOLD);
static_assert(REPORT.bijective && REPORT.ping_after_restore);
static_assert(!PING_PAIR(REPORT.swapped));
static_assert(PING_PAIR(REPORT.restored));

// =============================================
// Optional prior HEAD/TAIL envelope consistency.
// No TriDi, PiDi, PiSigma, SigmaPi, or NINZA
// is a prerequisite to the direct PING check.
// =============================================
static constexpr bool HEAD_PING=
    tork::PING_HEAD(tork::HEAD(FOLD.ad_folded));
static constexpr bool TAIL_PING=
    tork::PING_TAIL(tork::TAIL(FOLD.cd_folded));
static_assert(HEAD_PING && TAIL_PING);

[[nodiscard]] constexpr bool REJECT_CORRUPTION() noexcept {
    DirectFold bad=FOLD;
    bad.ad_folded^=1ULL;
    DirectFold wrong=FOLD;
    wrong.cd_folded=wrong.ad_folded;
    tork::BiPair wrong_label=REPORT.original;
    wrong_label.head.source=tork::Source::CD;
    return !PING(bad) && !PING(wrong) &&
           !PING_PAIR(wrong_label) &&
           !PING_PAIR(REPORT.swapped);
}
static_assert(REJECT_CORRUPTION());
static constexpr bool NINZA_REQUIRED=false; // only this sample
static constexpr bool GENERAL_FOLD_PROVED=false;
static constexpr bool PHYSICAL_FOLD_PROVED=false;
static constexpr bool RELEASE_AUTHORIZED=false;
static_assert(!NINZA_REQUIRED && !GENERAL_FOLD_PROVED &&
              !PHYSICAL_FOLD_PROVED && !RELEASE_AUTHORIZED);
} // namespace swe::minza_direct_ping_bidi
#endif // SWE_MINZA_DIRECT_PING_BIDI_HPP
