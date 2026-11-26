// SPDX-License-Identifier: 0BSD
// MINZA-NINZA-PING topology design: bounded derived incidence view.
// Include u_circle_fit.hpp first. The nine source faces remain unchanged.
// Exact bounded integer geometry; no change to the source BASE9.
#ifndef SWE_MINZA_TOPOLOGY_INCIDENCE_HPP
#define SWE_MINZA_TOPOLOGY_INCIDENCE_HPP
namespace swe::minza_topology {
namespace grid = swe::u_circle_fit;
using Point = grid::Point;
using s64 = grid::s64;
struct Edge { unsigned lo{}, hi{}, uses{}; int balance{}; };
struct View {
    Point vertex[27]{};
    Edge edge[729]{}; // Safe bounded ledger; overflow rejects the input.
    unsigned vertices{}, edges{}, boundary{}, interior{};
    bool valid{};
};
constexpr bool equal(Point a, Point b) noexcept {
    return a.x == b.x && a.y == b.y;
}
// Call only after point_valid preflight; products are at most 8e12.
constexpr s64 cross(Point a, Point b, Point c) noexcept {
    return (b.x-a.x)*(c.y-a.y)-(b.y-a.y)*(c.x-a.x);
}
constexpr s64 parameter(Point a, Point b, Point p) noexcept {
    return (p.x-a.x)*(b.x-a.x)+(p.y-a.y)*(b.y-a.y);
}
constexpr unsigned add_vertex(View& v, Point p) noexcept {
    for (unsigned i=0; i<v.vertices; ++i)
        if (equal(v.vertex[i],p)) return i;
    if (v.vertices==27) return 27;
    v.vertex[v.vertices]=p;
    return v.vertices++;
}
constexpr bool add_segment(View& v, unsigned a, unsigned b) noexcept {
    if (a==b || a>=v.vertices || b>=v.vertices) return false;
    const unsigned lo=a<b?a:b, hi=a<b?b:a;
    unsigned i=0;
    for (; i<v.edges; ++i)
        if (v.edge[i].lo==lo && v.edge[i].hi==hi) break;
    if (i==v.edges) {
        if (v.edges==729) return false;
        v.edge[v.edges++]={lo,hi,0,0};
    }
    ++v.edge[i].uses;
    v.edge[i].balance += a==lo?1:-1;
    return true;
}
constexpr bool split_side(View& v, Point a, Point b) noexcept {
    unsigned ordered[27]{};
    unsigned count=0;
    const s64 length2=parameter(a,b,b);
    if (length2==0) return false;
    for (unsigned i=0; i<v.vertices; ++i) {
        const s64 t=parameter(a,b,v.vertex[i]);
        if (cross(a,b,v.vertex[i])!=0 || t<0 || t>length2) continue;
        unsigned j=count;
        while (j>0 && parameter(a,b,v.vertex[ordered[j-1]])>t) {
            ordered[j]=ordered[j-1]; --j;
        }
        ordered[j]=i; ++count;
    }
    if (count<2) return false;
    for (unsigned i=1; i<count; ++i)
        if (!add_segment(v,ordered[i-1],ordered[i])) return false;
    return true;
}
constexpr View normalize(const grid::NineTriangles& source) noexcept {
    View v{};
    unsigned key[9][3]{};
    for (unsigned f=0; f<9; ++f) {
        const auto t=source.face[f];
        if (!grid::point_valid(t.a) || !grid::point_valid(t.b) ||
            !grid::point_valid(t.c) || cross(t.a,t.b,t.c)==0) return v;
        key[f][0]=add_vertex(v,t.a);
        key[f][1]=add_vertex(v,t.b);
        key[f][2]=add_vertex(v,t.c);
        for (unsigned i=0; i<3; ++i) {
            if (key[f][i]>=27) return v;
            for (unsigned j=i+1; j<3; ++j)
                if (key[f][j]<key[f][i]) {
                    const auto x=key[f][i];
                    key[f][i]=key[f][j]; key[f][j]=x;
                }
        }
        for (unsigned g=0; g<f; ++g)
            if (key[f][0]==key[g][0] && key[f][1]==key[g][1] &&
                key[f][2]==key[g][2]) return v;
    }
    for (unsigned f=0; f<9; ++f) {
        auto t=source.face[f];
        if (cross(t.a,t.b,t.c)<0) { const auto p=t.b; t.b=t.c; t.c=p; }
        if (!split_side(v,t.a,t.b) || !split_side(v,t.b,t.c) ||
            !split_side(v,t.c,t.a)) return v;
    }
    int boundary_of_boundary[27]{};
    for (unsigned i=0; i<v.edges; ++i) {
        const auto e=v.edge[i];
        if (e.uses==1 && (e.balance==1 || e.balance==-1)) ++v.boundary;
        else if (e.uses==2 && e.balance==0) ++v.interior;
        else return v;
        boundary_of_boundary[e.lo]-=e.balance;
        boundary_of_boundary[e.hi]+=e.balance;
    }
    for (unsigned i=0; i<v.vertices; ++i)
        if (boundary_of_boundary[i]!=0) return v;
    v.valid=true;
    return v;
}
constexpr bool base9_profile(const grid::NineTriangles& source) noexcept {
    const auto v=normalize(source);
    return v.valid && v.vertices==11 && v.edges==19 &&
           v.boundary==9 && v.interior==10 &&
           static_cast<int>(v.vertices)-static_cast<int>(v.edges)+9==1;
}
inline constexpr View normalized_base9=normalize(grid::BASE9);
static_assert(base9_profile(grid::BASE9));
constexpr bool negative_witnesses() noexcept {
    auto changed=grid::BASE9;
    changed.face[0].c.x=31; // Break a shared junction while retaining area.
    if (base9_profile(changed)) return false;
    auto duplicate=grid::BASE9;
    duplicate.face[8]=duplicate.face[0];
    if (normalize(duplicate).valid) return false;
    auto degenerate=grid::BASE9;
    degenerate.face[0].c=degenerate.face[0].a;
    if (normalize(degenerate).valid) return false;
    auto outside=grid::BASE9;
    outside.face[0].a.x=1000001;
    return !normalize(outside).valid;
}
static_assert(negative_witnesses());
} // namespace swe::minza_topology
#endif
