// SPDX-License-Identifier: 0BSD
// THREE-GEOMETRIC-FOLDS-PR34-013 | EXPLORATORY and ADDITIVE
// Three actual 90-degree folds about THREE distinct, connected hinges.
// Each fold rotates the entire downstream rigid strip assembly.
// This is NOT three cycles of the old TriDi labels.
// Original nine-triangle source and prior operator witnesses untouched.
// Four 60x15 strips use a separate eight-triangle partition.
// Three bends at y=15/30/45 make an open-ended square tube;
// the free seam touches but is not bonded, and it is NOT a cube.
// Include minza_direct_ping_tridi.hpp and prerequisites first.
// ISO C++20; no standard library, include, float, heap or extensions.
#ifndef SWE_THREE_GEOMETRIC_FOLDS_HPP
#define SWE_THREE_GEOMETRIC_FOLDS_HPP
#ifndef SWE_MINZA_DIRECT_PING_TRIDI_HPP
#error "Include minza_direct_ping_tridi.hpp before this header"
#endif
namespace swe::three_geometric_folds {
using s64=long long;
using u64=unsigned long long;
namespace grid=swe::u_circle_fit;
namespace tridi=swe::minza_direct_ping_tridi;
inline constexpr const char* experiment_id=
    "THREE-GEOMETRIC-FOLDS-PR34-013";

// =============================================
// STRUCTURE 1D: square flat sheet, four panels.
// Eight triangles are a NEW partition only.
// =============================================
static constexpr s64 WIDTH=60,STRIP=15;
static constexpr unsigned PANEL_COUNT=4u,TRI_COUNT=8u;
static constexpr unsigned FOLD_COUNT=3u;
struct Point2 { s64 x,y; };
struct Point3 { s64 x,y,z; bool valid; };
struct Triangle2 { Point2 a,b,c; };
struct Triangle3 { Point3 a,b,c; };
struct Mesh8 { Triangle2 face[TRI_COUNT]; };
struct Direction { s64 y,z; };
[[nodiscard]] constexpr bool VALID_FLAT(Point2 p,
    unsigned panel) noexcept {
    return panel<PANEL_COUNT && p.x>=0 && p.x<=WIDTH &&
           p.y>=STRIP*static_cast<s64>(panel) &&
           p.y<=STRIP*static_cast<s64>(panel+1u);
}
[[nodiscard]] constexpr Mesh8 BUILD_EIGHT() noexcept {
    Mesh8 mesh{};
    for(unsigned p=0u;p<PANEL_COUNT;++p) {
        const s64 lo=STRIP*static_cast<s64>(p);
        const s64 hi=lo+STRIP;
        const unsigned j=2u*p;
        mesh.face[j]={{0,lo},{WIDTH,lo},{WIDTH,hi}};
        mesh.face[j+1u]={{0,lo},{WIDTH,hi},{0,hi}};
    }
    return mesh;
}
static constexpr Mesh8 MESH=BUILD_EIGHT();

// =============================================
// FORM 3D: three distinct hinge events.
// Stage 0 = flat, stage 1 = y15 folded,
// stage 2 = y30 folded, stage 3 = y45 folded.
// Count the 90-degree turns inherited by a panel.
// =============================================
[[nodiscard]] constexpr unsigned TURNS(
    unsigned stage,unsigned panel) noexcept {
    if(stage>FOLD_COUNT || panel>=PANEL_COUNT)return 0u;
    unsigned turns=0u;
    for(unsigned hinge=1u;hinge<=FOLD_COUNT;++hinge)
        if(stage>=hinge && panel>=hinge)++turns;
    return turns;
}
[[nodiscard]] constexpr Direction DIR(unsigned turns) noexcept {
    switch(turns) {
        case 0u:return {1,0};
        case 1u:return {0,1};
        case 2u:return {-1,0};
        case 3u:return {0,-1};
        default:return {0,0};
    }
}
[[nodiscard]] constexpr Point3 PROJECT(
    Point2 point,unsigned panel,unsigned stage) noexcept {
    if(stage>FOLD_COUNT || !VALID_FLAT(point,panel))
        return {0,0,0,false};
    Point3 folded{point.x,0,0,true};
    // Every prior strip contributes its length along its
    // current 3D cross-sectional direction.
    for(unsigned p=0u;p<panel;++p) {
        const Direction d=DIR(TURNS(stage,p));
        folded.y+=STRIP*d.y;
        folded.z+=STRIP*d.z;
    }
    const Direction own=DIR(TURNS(stage,panel));
    const s64 local=point.y-STRIP*static_cast<s64>(panel);
    folded.y+=local*own.y;
    folded.z+=local*own.z;
    return folded;
}
[[nodiscard]] constexpr Triangle3 FOLD_TRIANGLE(
    unsigned index,unsigned stage) noexcept {
    if(index>=TRI_COUNT || stage>FOLD_COUNT)
        return {};
    const Triangle2 t=MESH.face[index];
    const unsigned panel=index/2u;
    return {PROJECT(t.a,panel,stage),
            PROJECT(t.b,panel,stage),
            PROJECT(t.c,panel,stage)};
}
[[nodiscard]] constexpr bool SAME(Point3 a,Point3 b) noexcept {
    return a.valid && b.valid &&
        a.x==b.x && a.y==b.y && a.z==b.z;
}
[[nodiscard]] constexpr bool DIFFERENT(Point3 a,Point3 b) noexcept {
    return a.valid && b.valid && !SAME(a,b);
}
[[nodiscard]] constexpr u64 SQUARE(s64 a) noexcept {
    // All points in this bounded construction are within +/-60.
    return static_cast<u64>(a*a);
}
[[nodiscard]] constexpr u64 DISTANCE2(Point3 a,Point3 b) noexcept {
    if(!a.valid || !b.valid)return 0ULL;
    return SQUARE(a.x-b.x)+SQUARE(a.y-b.y)+SQUARE(a.z-b.z);
}
[[nodiscard]] constexpr u64 FLAT_DISTANCE2(Point2 a,Point2 b) noexcept {
    return SQUARE(a.x-b.x)+SQUARE(a.y-b.y);
}
[[nodiscard]] constexpr u64 FLAT_DOUBLE_AREA(Triangle2 t) noexcept {
    const s64 ax=t.b.x-t.a.x,ay=t.b.y-t.a.y;
    const s64 bx=t.c.x-t.a.x,by=t.c.y-t.a.y;
    const s64 cross=ax*by-ay*bx;
    return static_cast<u64>(cross<0 ? -cross : cross);
}
[[nodiscard]] constexpr u64 FOLDED_CROSS_NORM2(
    Triangle3 t) noexcept {
    if(!t.a.valid||!t.b.valid||!t.c.valid)return 0ULL;
    const s64 ax=t.b.x-t.a.x;
    const s64 ay=t.b.y-t.a.y;
    const s64 az=t.b.z-t.a.z;
    const s64 bx=t.c.x-t.a.x;
    const s64 by=t.c.y-t.a.y;
    const s64 bz=t.c.z-t.a.z;
    const s64 cx=ay*bz-az*by;
    const s64 cy=az*bx-ax*bz;
    const s64 cz=ax*by-ay*bx;
    return SQUARE(cx)+SQUARE(cy)+SQUARE(cz);
}
[[nodiscard]] constexpr bool TRIANGLE_RIGID(
    unsigned index,unsigned stage) noexcept {
    if(index>=TRI_COUNT || stage>FOLD_COUNT)return false;
    const auto flat=MESH.face[index];
    const auto folded=FOLD_TRIANGLE(index,stage);
    const u64 area2=FLAT_DOUBLE_AREA(flat);
    return area2==900ULL &&
        DISTANCE2(folded.a,folded.b)==FLAT_DISTANCE2(flat.a,flat.b) &&
        DISTANCE2(folded.b,folded.c)==FLAT_DISTANCE2(flat.b,flat.c) &&
        DISTANCE2(folded.c,folded.a)==FLAT_DISTANCE2(flat.c,flat.a) &&
        FOLDED_CROSS_NORM2(folded)==area2*area2;
}

// =============================================
// PING: exact shared hinge for all four stages.
// 0/30/60 sample points suffice along a straight
// line because both panels are rigid affine maps.
// =============================================
[[nodiscard]] constexpr bool SEAMS_MATCH(unsigned stage) noexcept {
    if(stage>FOLD_COUNT)return false;
    for(unsigned hinge=1u;hinge<=FOLD_COUNT;++hinge)
        for(unsigned j=0u;j<3u;++j) {
            const Point2 edge{
                static_cast<s64>(j)*30,
                static_cast<s64>(hinge)*STRIP
            };
            if(!SAME(PROJECT(edge,hinge-1u,stage),
                     PROJECT(edge,hinge,stage)))
                return false;
        }
    return true;
}
[[nodiscard]] constexpr bool ALL_RIGID(unsigned stage) noexcept {
    if(stage>FOLD_COUNT)return false;
    for(unsigned i=0u;i<TRI_COUNT;++i)
        if(!TRIANGLE_RIGID(i,stage))return false;
    return true;
}
[[nodiscard]] constexpr bool UPSTREAM_FIXED(
    unsigned stage) noexcept {
    if(stage==0u || stage>FOLD_COUNT)return false;
    // No point on panels before the hinge may move.
    for(unsigned panel=0u;panel<stage;++panel) {
        const auto t=MESH.face[2u*panel];
        if(!SAME(PROJECT(t.a,panel,stage),
                 PROJECT(t.a,panel,stage-1u)) ||
           !SAME(PROJECT(t.b,panel,stage),
                 PROJECT(t.b,panel,stage-1u)) ||
           !SAME(PROJECT(t.c,panel,stage),
                 PROJECT(t.c,panel,stage-1u)))
            return false;
    }
    // The current hinge stays fixed in space.
    for(unsigned j=0u;j<3u;++j) {
        const Point2 p{
            30*static_cast<s64>(j),
            STRIP*static_cast<s64>(stage)
        };
        if(!SAME(PROJECT(p,stage-1u,stage),
                 PROJECT(p,stage,stage-1u)))
            return false;
    }
    return true;
}
[[nodiscard]] constexpr bool DOWNSTREAM_MOVED(
    unsigned stage) noexcept {
    if(stage==0u || stage>FOLD_COUNT)return false;
    const Point2 far{
        WIDTH,
        STRIP*static_cast<s64>(stage+1u)
    };
    return DIFFERENT(PROJECT(far,stage,stage),
                     PROJECT(far,stage,stage-1u));
}
[[nodiscard]] constexpr bool RIGHT_ANGLE_HINGE(
    unsigned stage) noexcept {
    if(stage==0u || stage>FOLD_COUNT)return false;
    const Direction before=DIR(TURNS(stage,stage-1u));
    const Direction after=DIR(TURNS(stage,stage));
    const s64 dot=before.y*after.y+before.z*after.z;
    const s64 cross=before.y*after.z-before.z*after.y;
    return dot==0 && cross==1;
}
[[nodiscard]] constexpr bool FOLD_STEP(unsigned stage) noexcept {
    return stage>=1u && stage<=3u &&
           ALL_RIGID(stage) &&
           SEAMS_MATCH(stage) &&
           UPSTREAM_FIXED(stage) &&
           DOWNSTREAM_MOVED(stage) &&
           RIGHT_ANGLE_HINGE(stage);
}
static_assert(ALL_RIGID(0u) && SEAMS_MATCH(0u));
static_assert(FOLD_STEP(1u));
static_assert(FOLD_STEP(2u));
static_assert(FOLD_STEP(3u));

// =============================================
// QUANT 4D: complete three-fold audit.
// Fold 1: y=15, Fold 2: y=30, Fold 3: y=45.
// Eight triangular facets retain exact total area.
// The free ends meet at the fourth edge, giving
// a mathematically closed cross-section, but
// an open tube with no end caps or glued seam.
// =============================================
[[nodiscard]] constexpr u64 SUM_DOUBLE_AREA() noexcept {
    u64 sum=0ULL;
    for(unsigned i=0u;i<TRI_COUNT;++i)
        sum+=FLAT_DOUBLE_AREA(MESH.face[i]);
    return sum;
}
static_assert(SUM_DOUBLE_AREA()==7200ULL);
static_assert(SUM_DOUBLE_AREA()==grid::SUM_NINE_DOUBLE_AREA());
static_assert(SUM_DOUBLE_AREA()/2ULL==grid::T);

// Assert the square cross-section visits
// (y,z) = (0,0)->(15,0)->(15,15)->(0,15)->(0,0).
static constexpr Point3 CORNER_0=PROJECT({0,0},0u,3u);
static constexpr Point3 CORNER_1=PROJECT({0,15},0u,3u);
static constexpr Point3 CORNER_2=PROJECT({0,30},1u,3u);
static constexpr Point3 CORNER_3=PROJECT({0,45},2u,3u);
static constexpr Point3 CORNER_4=PROJECT({0,60},3u,3u);
static_assert(CORNER_0.y==0 && CORNER_0.z==0);
static_assert(CORNER_1.y==15 && CORNER_1.z==0);
static_assert(CORNER_2.y==15 && CORNER_2.z==15);
static_assert(CORNER_3.y==0 && CORNER_3.z==15);
static_assert(SAME(CORNER_0,CORNER_4));
[[nodiscard]] constexpr bool FREE_SEAM_COINCIDENT() noexcept {
    for(unsigned j=0u;j<3u;++j) {
        const s64 x=static_cast<s64>(j)*30;
        if(!SAME(PROJECT({x,0},0u,3u),
                 PROJECT({x,60},3u,3u)))
            return false;
    }
    return true;
}
static_assert(FREE_SEAM_COINCIDENT());
static_assert(!SAME(PROJECT({0,0},0u,2u),
                    PROJECT({0,60},3u,2u)));
// Distinct actual hinges, not 3 permutations of labels.
static_assert(TURNS(3u,0u)==0u);
static_assert(TURNS(3u,1u)==1u);
static_assert(TURNS(3u,2u)==2u);
static_assert(TURNS(3u,3u)==3u);

struct TriFoldWitness {
    bool first_hinge,second_hinge,third_hinge;
    bool facets_rigid,seams_continuous,free_seam_coincident;
    bool distinct_actions,previous_geometry_preserved;
    bool valid;
};
[[nodiscard]] constexpr TriFoldWitness PING_THREE_FOLDS() noexcept {
    const bool f1=FOLD_STEP(1u);
    const bool f2=FOLD_STEP(2u);
    const bool f3=FOLD_STEP(3u);
    const bool rigid=ALL_RIGID(0u)&&ALL_RIGID(1u)&&
                     ALL_RIGID(2u)&&ALL_RIGID(3u);
    const bool seams=SEAMS_MATCH(0u)&&SEAMS_MATCH(1u)&&
                     SEAMS_MATCH(2u)&&SEAMS_MATCH(3u);
    const bool closure=FREE_SEAM_COINCIDENT();
    const bool three=DOWNSTREAM_MOVED(1u)&&
                     DOWNSTREAM_MOVED(2u)&&DOWNSTREAM_MOVED(3u);
    const bool prior=tridi::REPORT.valid &&
                     SUM_DOUBLE_AREA()==grid::SUM_NINE_DOUBLE_AREA();
    return {f1,f2,f3,rigid,seams,closure,three,prior,
            f1&&f2&&f3&&rigid&&seams&&closure&&three&&prior};
}
static constexpr TriFoldWitness WITNESS=PING_THREE_FOLDS();
static_assert(WITNESS.valid);
static constexpr bool OPEN_TUBE_GEOMETRY_VERIFIED=WITNESS.valid;
static constexpr bool SEALED_SEAM_VERIFIED=false;
static constexpr bool CLOSED_CUBE_VERIFIED=false;
static constexpr bool PHYSICAL_PAPER_FOLD_VERIFIED=false;
static constexpr bool RELEASE_AUTHORIZED=false;
static_assert(!SEALED_SEAM_VERIFIED && !CLOSED_CUBE_VERIFIED &&
              !PHYSICAL_PAPER_FOLD_VERIFIED && !RELEASE_AUTHORIZED);
} // namespace swe::three_geometric_folds
#endif // SWE_THREE_GEOMETRIC_FOLDS_HPP
