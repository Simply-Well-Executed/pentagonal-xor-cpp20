/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
// Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824
// REDNECKENGINEERED native CPU graphics: MODEL -> BAKE -> BITMAP
// -> COOK -> CUBEMAP -> [DIFZ HOLD] -> PING -> [PUBLISH HOLD].
// Real pixel and cube-face generation from the established 2D triangle.
// No fabricated 3D geometry, DIFZ law, hidden source modifications or release.
#ifndef SWE_REDNECKENGINEERED_NATIVE_RENDER_HPP
#define SWE_REDNECKENGINEERED_NATIVE_RENDER_HPP
#include "redneckengineered_model_pidi.hpp"
#include <array>
#include <cstdint>
namespace swe::redneckengineered::native_render {
namespace u=swe::u_circle_fit;
namespace pidi=swe::redneckengineered::model_pidi;
inline constexpr unsigned SIDE=64u;
inline constexpr unsigned PIXELS=SIDE*SIDE;
inline constexpr unsigned FACE_COUNT=6u;
using Byte=std::uint8_t;
struct Pixel { Byte r{},g{},b{}; };
using Bitmap=std::array<Pixel,PIXELS>;
struct Cube { std::array<Bitmap,FACE_COUNT> faces{}; };
struct Model { u::Triangle2 triangle{}; u::CircleU circle{}; };
struct Result {
    Model original{};
    Bitmap baked{};
    Bitmap cooked{};
    Cube cubemap{};
    bool geometry_nice{};
    bool bitmap_different_from_background{};
    bool cube_has_six_faces{};
    bool difz_verified{}; // not provided by source
    bool release_authorized{};
};
[[nodiscard]] constexpr bool equal(Pixel a,Pixel b) noexcept {
    return a.r==b.r && a.g==b.g && a.b==b.b;
}
[[nodiscard]] constexpr bool equal(const Bitmap& a,const Bitmap& b) noexcept {
    for(unsigned i=0;i<PIXELS;++i)if(!equal(a[i],b[i]))return false;
    return true;
}
[[nodiscard]] constexpr bool finite(double x) noexcept {
    return x==x && x>=-1000000.0 && x<=1000000.0;
}
[[nodiscard]] constexpr double edge(u::Point a,u::Point b,
                                    double x,double y) noexcept {
    return (static_cast<double>(b.x-a.x)*(y-a.y))-
           (static_cast<double>(b.y-a.y)*(x-a.x));
}
[[nodiscard]] constexpr bool contains(u::Triangle2 t,
                                       double x,double y) noexcept {
    if(!finite(x)||!finite(y))return false;
    const double e1=edge(t.a,t.b,x,y),e2=edge(t.b,t.c,x,y),
                 e3=edge(t.c,t.a,x,y);
    return (e1>=0.0&&e2>=0.0&&e3>=0.0) ||
           (e1<=0.0&&e2<=0.0&&e3<=0.0);
}
// BAKE the source triangle's actual 2D coverage into a 64x64 bitmap.
// This raster is independent of the unresolved Jig-area-to-footprint map.
[[nodiscard]] constexpr Bitmap bake(Model m) noexcept {
    Bitmap image{};
    if(u::VALIDATE(m.triangle,m.circle)!=u::U::Nice)return image;
    for(unsigned y=0;y<SIDE;++y)
        for(unsigned x=0;x<SIDE;++x) {
            const double px=static_cast<double>(x)+0.5;
            const double py=static_cast<double>(y)+0.5;
            if(contains(m.triangle,px,py))
                image[y*SIDE+x]={240u,156u,58u};
        }
    return image;
}
// The cook stage builds a distinct RGB bitmap preserving every baked pixel.
// An identity cook is explicit: no color conversion is silently invented.
[[nodiscard]] constexpr Bitmap cook(const Bitmap& baked) noexcept {
    Bitmap cooked{};
    for(unsigned i=0;i<PIXELS;++i)cooked[i]=baked[i];
    return cooked;
}
struct Vector { double x{},y{},z{}; };
// Right-handed six-face view vectors. No unit normalization required for
// plane intersection: the ratio x/z,y/z is invariant under positive scale.
[[nodiscard]] constexpr Vector direction(unsigned face,
                                        unsigned x,unsigned y) noexcept {
    const double s=(2.0*(static_cast<double>(x)+0.5)/SIDE)-1.0;
    const double t=(2.0*(static_cast<double>(y)+0.5)/SIDE)-1.0;
    switch(face) {
        case 0u: return {1.0,-t,-s};   // +X
        case 1u: return {-1.0,-t,s};   // -X
        case 2u: return {s,1.0,t};     // +Y
        case 3u: return {s,-1.0,-t};   // -Y
        case 4u: return {s,-t,1.0};    // +Z
        default: return {-s,-t,-1.0};  // -Z
    }
}
// Cast a ray toward the existing triangle placed on plane z=1. Only
// positive intersections count. This is a *real* cube-view projection
// of the planar model; it is not an inferred volumetric 3D shape.
[[nodiscard]] constexpr Pixel sample(const Bitmap& cooked,
                                     Vector ray) noexcept {
    if(ray.z<=0.0)return {};
    const double mx=30.0+32.0*ray.x/ray.z;
    const double my=30.0+32.0*ray.y/ray.z;
    if(!finite(mx)||!finite(my) || mx<0.0 || my<0.0 ||
       mx>=static_cast<double>(SIDE) ||
       my>=static_cast<double>(SIDE))return {};
    const unsigned ix=static_cast<unsigned>(mx);
    const unsigned iy=static_cast<unsigned>(my);
    return cooked[iy*SIDE+ix];
}
[[nodiscard]] constexpr Cube cube(const Bitmap& cooked) noexcept {
    Cube output{};
    for(unsigned face=0;face<FACE_COUNT;++face)
        for(unsigned y=0;y<SIDE;++y)
            for(unsigned x=0;x<SIDE;++x)
                output.faces[face][y*SIDE+x]=
                    sample(cooked,direction(face,x,y));
    return output;
}
[[nodiscard]] constexpr bool has_color(const Bitmap& b) noexcept {
    for(const Pixel px:b)if(px.r||px.g||px.b)return true;
    return false;
}
[[nodiscard]] constexpr Result render(Model m) noexcept {
    Result r{};
    r.original=m;
    r.geometry_nice=u::VALIDATE(m.triangle,m.circle)==u::U::Nice;
    if(!r.geometry_nice)return r;
    r.baked=bake(m);
    r.cooked=cook(r.baked);
    r.cubemap=cube(r.cooked);
    r.bitmap_different_from_background=has_color(r.baked);
    // All six views are allocated and populated using direction projection.
    r.cube_has_six_faces=r.bitmap_different_from_background &&
                         has_color(r.cubemap.faces[4u]);
    // A raster is available; the user's DIFZ transform is still undefined.
    r.difz_verified=false;
    r.release_authorized=false;
    return r;
}
[[nodiscard]] constexpr bool check(const Result& r) noexcept {
    if(!r.geometry_nice || !r.bitmap_different_from_background ||
       !r.cube_has_six_faces || r.difz_verified || r.release_authorized)
        return false;
    return equal(r.baked,r.cooked) && has_color(r.cubemap.faces[4u]);
}
inline constexpr Model established_model{u::FACE,u::ENCLOSING_CIRCLE};
// This validates the source geometry but does NOT imply original measurements
// or a genuine Euler-Jig footprint. Hot Jig remains the canonical source.
static_assert(u::VALIDATE(established_model.triangle,
                           established_model.circle)==u::U::Nice);
static_assert(pidi::HOT_JIG_X1E17==
              swe::eulers_jig::ONE_TRIANGLE_X1E17);
inline constexpr bool DIFZ_DEFINED=false;
inline constexpr bool FINALIZEDMODELREADYFORPUBLISHING=false;
static_assert(!DIFZ_DEFINED && !FINALIZEDMODELREADYFORPUBLISHING);
} // namespace swe::redneckengineered::native_render
#endif
