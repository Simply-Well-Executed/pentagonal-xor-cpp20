// SPDX-License-Identifier: 0BSD
// RECTANGLOID-PR34-014 | diary primitive, NOT an XHTML parser.
// ISO C++20; zero includes, STL, floats, heap or compiler extensions.
// Include three_geometric_folds.hpp and prerequisites BEFORE this header.
// Existing three-fold source, nine-triangle partition and BiDi untouched.
#ifndef SWE_RECTANGLOID_PRIMITIVE_CXX20_HPP
#define SWE_RECTANGLOID_PRIMITIVE_CXX20_HPP
#ifndef SWE_THREE_GEOMETRIC_FOLDS_HPP
#error "Include three_geometric_folds.hpp before rectangloid_primitive_cxx20.hpp"
#endif
namespace swe::rectangloid_primitive {
using s64=long long;
using u64=unsigned long long;
namespace fold=swe::three_geometric_folds;
inline constexpr const char* experiment_id="RECTANGLOID-PR34-014";

// STRUCTURE: 60 x 60 flat square, four 60 x 15 strips.
// Three genuine 90-degree hinges at flat y=15,30,45.
// Eight triangular wall facets plus two triangular facets per cap.
static constexpr s64 LENGTH=60,SIDE=15;
static constexpr unsigned WALL_TRIANGLES=8u,CAP_TRIANGLES=4u;
static constexpr unsigned TRIANGLES=12u;
struct Point3 {s64 x,y,z;};
struct Triangle {Point3 a,b,c;};
struct Quad {Point3 a,b,c,d;};
struct Mesh {Triangle face[TRIANGLES];};
[[nodiscard]] constexpr Point3 PROJECT(
    s64 x,s64 flat_y,unsigned panel) noexcept {
    const auto p=fold::PROJECT({x,flat_y},panel,3u);
    return {p.x,p.y,p.z};
}
[[nodiscard]] constexpr Quad WALL(unsigned panel) noexcept {
    if(panel>=4u)return {};
    const s64 y=SIDE*static_cast<s64>(panel);
    return {PROJECT(0,y,panel),PROJECT(LENGTH,y,panel),
            PROJECT(LENGTH,y+SIDE,panel),PROJECT(0,y+SIDE,panel)};
}
[[nodiscard]] constexpr Quad END_CAP(s64 x) noexcept {
    return {{x,0,0},{x,SIDE,0},{x,SIDE,SIDE},{x,0,SIDE}};
}
// FORM: outward-facing, oriented triangular boundary mesh.
// 8 walls + 4 cap triangles. Left cap -X, right cap +X.
[[nodiscard]] constexpr Mesh BUILD_CLOSED_MESH() noexcept {
    Mesh m{};
    for(unsigned i=0u;i<4u;++i) {
        const Quad w=WALL(i);
        m.face[2u*i]={w.a,w.c,w.b};
        m.face[2u*i+1u]={w.a,w.d,w.c};
    }
    const Quad l=END_CAP(0),r=END_CAP(LENGTH);
    m.face[8]={l.a,l.c,l.b};
    m.face[9]={l.a,l.d,l.c};
    m.face[10]={r.a,r.b,r.c};
    m.face[11]={r.a,r.c,r.d};
    return m;
}
static constexpr Mesh CLOSED=BUILD_CLOSED_MESH();
[[nodiscard]] constexpr bool SAME(Point3 a,Point3 b) noexcept {
    return a.x==b.x&&a.y==b.y&&a.z==b.z;
}
[[nodiscard]] constexpr Point3 SUB(Point3 a,Point3 b) noexcept {
    return {a.x-b.x,a.y-b.y,a.z-b.z};
}
[[nodiscard]] constexpr Point3 CROSS(Point3 a,Point3 b) noexcept {
    return {a.y*b.z-a.z*b.y,a.z*b.x-a.x*b.z,
            a.x*b.y-a.y*b.x};
}
[[nodiscard]] constexpr s64 DOT(Point3 a,Point3 b) noexcept {
    return a.x*b.x+a.y*b.y+a.z*b.z;
}
[[nodiscard]] constexpr s64 ABS(s64 a) noexcept {
    // Precondition: bounded original grid coordinates only.
    return a<0?-a:a;
}
[[nodiscard]] constexpr s64 DOUBLE_AREA(Triangle t) noexcept {
    const auto n=CROSS(SUB(t.b,t.a),SUB(t.c,t.a));
    // Exact axis-aligned facets, no floating point square root.
    return ABS(n.x)+ABS(n.y)+ABS(n.z);
}
[[nodiscard]] constexpr u64 DOUBLE_AREA_TOTAL() noexcept {
    u64 sum=0ULL;
    for(unsigned i=0u;i<TRIANGLES;++i)
        sum+=static_cast<u64>(DOUBLE_AREA(CLOSED.face[i]));
    return sum;
}
static_assert(DOUBLE_AREA_TOTAL()==8100ULL);

// PING: every directed edge has precisely one reverse twin.
// This rejects any missing cap or unjoined seam in the model.
struct Edge {Point3 from,to;};
[[nodiscard]] constexpr Edge EDGE(Triangle t,unsigned j) noexcept {
    if(j==0u)return {t.a,t.b};
    if(j==1u)return {t.b,t.c};
    return {t.c,t.a};
}
[[nodiscard]] constexpr bool REVERSED(Edge a,Edge b) noexcept {
    return SAME(a.from,b.to)&&SAME(a.to,b.from);
}
[[nodiscard]] constexpr bool ALL_EDGES_PAIRED(Mesh m) noexcept {
    for(unsigned i=0u;i<TRIANGLES;++i) {
        if(DOUBLE_AREA(m.face[i])==0)return false;
        for(unsigned e=0u;e<3u;++e) {
            unsigned matches=0u;
            const auto source=EDGE(m.face[i],e);
            for(unsigned j=0u;j<TRIANGLES;++j)
                for(unsigned k=0u;k<3u;++k)
                    if(REVERSED(source,EDGE(m.face[j],k)))
                        ++matches;
            if(matches!=1u)return false;
        }
    }
    return true;
}
static_assert(ALL_EDGES_PAIRED(CLOSED));

// QUANT: volume from oriented triangle mesh, not invented ledger.
[[nodiscard]] constexpr s64 SIGNED_VOLUME6(Mesh m) noexcept {
    s64 n=0;
    for(unsigned i=0u;i<TRIANGLES;++i) {
        const auto t=m.face[i];
        n+=DOT(t.a,CROSS(t.b,t.c));
    }
    return n;
}
static constexpr s64 VOLUME=LENGTH*SIDE*SIDE;
static constexpr s64 SURFACE_AREA=
    2*(LENGTH*SIDE+LENGTH*SIDE+SIDE*SIDE);
static_assert(VOLUME==13500);
static_assert(SURFACE_AREA==4050);
static_assert(SIGNED_VOLUME6(CLOSED)==6*VOLUME);

// Two 60 x 60 source sheets yield precisely two 15 x 15 caps.
// Their offcuts remain in the provenance ledger.
struct SheetReceipt {u64 source_area,cap_area,offcut_area;};
static constexpr SheetReceipt CAP_0{3600ULL,225ULL,3375ULL};
static constexpr SheetReceipt CAP_1{3600ULL,225ULL,3375ULL};
static_assert(CAP_0.source_area==CAP_0.cap_area+CAP_0.offcut_area);
static_assert(CAP_1.source_area==CAP_1.cap_area+CAP_1.offcut_area);

// Water is the user's elected VISUALIZATION setting, not an
// engineering assumption. Arbitrary model units imply no mass.
enum class Fill {Empty,Water};
struct Material {Fill fill;bool yang_conformal;bool seam_fused_visual;};
static constexpr Material DIARY_VIEW{Fill::Water,true,true};

// YANG: front-face visibility for an OUTSIDE camera.
// The full Z-buffer occlusion pipeline remains unimplemented.
static constexpr Point3 CAMERA{120,45,60};
[[nodiscard]] constexpr bool FRONT_FACING(
    Triangle t,Point3 camera) noexcept {
    const auto n=CROSS(SUB(t.b,t.a),SUB(t.c,t.a));
    const Point3 towards{
        3*camera.x-t.a.x-t.b.x-t.c.x,
        3*camera.y-t.a.y-t.b.y-t.c.y,
        3*camera.z-t.a.z-t.b.z-t.c.z};
    return DOT(n,towards)>0;
}
[[nodiscard]] constexpr unsigned VISIBLE_TRIANGLES(
    Point3 camera) noexcept {
    unsigned count=0u;
    for(unsigned i=0u;i<TRIANGLES;++i)
        if(FRONT_FACING(CLOSED.face[i],camera))++count;
    return count;
}
static_assert(VISIBLE_TRIANGLES(CAMERA)==6u);
static constexpr unsigned CULLED_TRIANGLES=
    TRIANGLES-VISIBLE_TRIANGLES(CAMERA);
static_assert(CULLED_TRIANGLES==6u);

// Negative witness: corrupt one cap and deny manifold closure.
[[nodiscard]] constexpr bool MISSING_CAP_REJECTED() noexcept {
    Mesh bad=CLOSED;
    bad.face[8]=bad.face[0];
    return !ALL_EDGES_PAIRED(bad);
}
static_assert(MISSING_CAP_REJECTED());

struct Ping {
    bool three_folds,triangles_nonzero,oriented_edges_paired;
    bool volume_verified,surface_area_verified,caps_accounted;
    bool corruption_rejected,valid;
};
[[nodiscard]] constexpr Ping PING() noexcept {
    bool nonzero=true;
    for(unsigned i=0u;i<TRIANGLES;++i)
        if(DOUBLE_AREA(CLOSED.face[i])==0)nonzero=false;
    const bool previous=fold::WITNESS.valid;
    const bool edges=ALL_EDGES_PAIRED(CLOSED);
    const bool volume=SIGNED_VOLUME6(CLOSED)==6*VOLUME;
    const bool surface=DOUBLE_AREA_TOTAL()==2ULL*static_cast<u64>(SURFACE_AREA);
    const bool caps=CAP_0.source_area==CAP_0.cap_area+CAP_0.offcut_area &&
                    CAP_1.source_area==CAP_1.cap_area+CAP_1.offcut_area;
    const bool reject=MISSING_CAP_REJECTED();
    return {previous,nonzero,edges,volume,surface,caps,reject,
        previous&&nonzero&&edges&&volume&&surface&&caps&&reject};
}
static constexpr Ping WITNESS=PING();
static_assert(WITNESS.valid);
// Mathematical closure != a manufactured, liquid-tight vessel.
static constexpr bool MATHEMATICAL_BOUNDARY_CLOSED=true;
static constexpr bool PHYSICAL_SEAM_FUSION_VERIFIED=false;
static constexpr bool PHYSICAL_WATER_FILL_VERIFIED=false;
static constexpr bool WATER_DENSITY_OR_MASS_INFERRED=false;
static constexpr bool FULL_Z_BUFFER_OCCLUSION_IMPLEMENTED=false;
static constexpr bool PHYSICAL_SOLID_MATERIAL_VERIFIED=false;
static constexpr bool RELEASE_AUTHORIZED=false;
static_assert(!PHYSICAL_SEAM_FUSION_VERIFIED &&
    !PHYSICAL_WATER_FILL_VERIFIED &&
    !WATER_DENSITY_OR_MASS_INFERRED &&
    !FULL_Z_BUFFER_OCCLUSION_IMPLEMENTED &&
    !PHYSICAL_SOLID_MATERIAL_VERIFIED &&
    !RELEASE_AUTHORIZED);
} // namespace swe::rectangloid_primitive
#endif // SWE_RECTANGLOID_PRIMITIVE_CXX20_HPP
