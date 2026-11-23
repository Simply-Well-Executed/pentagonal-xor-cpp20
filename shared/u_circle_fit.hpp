// SPDX-License-Identifier: 0BSD
// U-CIRCLE-FIT-PR34-009 | INSPO projected-circle containment
// User's U Nice/NotNice source integrated without changing source witnesses.
// ISO C++20, native MSVC, zero includes or STL, constexpr-only examples.
// Include sigma_form_area_q62.hpp and four_pyramid_reduxualchoice.hpp
// before this header. Q62 multiplication belongs to the prior source.
// The grid examples are SYNTHETIC, not traced from a photograph.
#ifndef SWE_U_CIRCLE_FIT_HPP
#define SWE_U_CIRCLE_FIT_HPP
#ifndef SWE_SIGMA_FORM_AREA_Q62_HPP
#error "Include sigma_form_area_q62.hpp before u_circle_fit.hpp"
#endif
#ifndef SWE_FOUR_PYRAMID_REDUXUALCHOICE_HPP
#error "Include four_pyramid_reduxualchoice.hpp before u_circle_fit.hpp"
#endif
namespace swe::u_circle_fit {
using u64 = unsigned long long;
using s64 = long long;
namespace core = swe::sigma_form_area;
namespace pyramid = swe::four_pyramid_reduxualchoice;
inline constexpr const char* experiment_id = "U-CIRCLE-FIT-PR34-009";
static constexpr u64 Q = 1ULL << 62u;

// =============================================
// U: exact circle-fit validation
// =============================================
enum class U { NotNice, Nice };
struct Point { s64 x,y; };
struct Triangle2 { Point a,b,c; };
struct CircleU { s64 cx2,cy2; u64 radius2_squared; };

// API DOMAIN: each grid coordinate must lie in [-1e6,+1e6],
// and each doubled circle center in [-2e6,+2e6].
// This preflight guarantees signed differences and squared-distance
// additions cannot overflow. Coordinates outside domain => NotNice.
static constexpr s64 point_limit=1000000LL;
static constexpr s64 center2_limit=2000000LL;
[[nodiscard]] constexpr bool point_valid(Point p) noexcept {
    return p.x >= -point_limit && p.x <= point_limit &&
           p.y >= -point_limit && p.y <= point_limit;
}
[[nodiscard]] constexpr bool circle_valid(CircleU c) noexcept {
    return c.cx2 >= -center2_limit && c.cx2 <= center2_limit &&
           c.cy2 >= -center2_limit && c.cy2 <= center2_limit &&
           c.radius2_squared != 0ULL;
}
// SQUARE is safe for any signed integer, returning the largest
// unsigned value as an invalid sentinel outside the stated domain.
// INSIDE never calls it without validating the bounded inputs.
[[nodiscard]] constexpr u64 SQUARE(s64 x) noexcept {
    if (x < -4000000LL || x > 4000000LL)
        return ~0ULL;
    const u64 magnitude=static_cast<u64>(x<0 ? -x : x);
    return magnitude*magnitude;
}
[[nodiscard]] constexpr u64 DISTANCE2(Point p,CircleU c) noexcept {
    if (!point_valid(p)||!circle_valid(c))return ~0ULL;
    const s64 dx=2LL*p.x-c.cx2;
    const s64 dy=2LL*p.y-c.cy2;
    return SQUARE(dx)+SQUARE(dy);
}
[[nodiscard]] constexpr bool INSIDE(Point p,CircleU c) noexcept {
    return point_valid(p) && circle_valid(c) &&
           DISTANCE2(p,c)<=c.radius2_squared;
}
[[nodiscard]] constexpr u64 DOUBLE_AREA(Triangle2 t) noexcept {
    if (!point_valid(t.a)||!point_valid(t.b)||!point_valid(t.c))
        return 0ULL;
    const s64 bx=t.b.x-t.a.x,by=t.b.y-t.a.y;
    const s64 cx=t.c.x-t.a.x,cy=t.c.y-t.a.y;
    // Every component <=2e6, cross magnitude <=8e12; safe in s64.
    const s64 cross=bx*cy-by*cx;
    return static_cast<u64>(cross<0 ? -cross : cross);
}
[[nodiscard]] constexpr U VALIDATE(
    Triangle2 triangle,CircleU circle) noexcept {
    // A zero-area triple is not a valid triangle, even if its points
    // happen to lie within the circle.
    const bool fits=DOUBLE_AREA(triangle)!=0ULL &&
        INSIDE(triangle.a,circle) &&
        INSIDE(triangle.b,circle) &&
        INSIDE(triangle.c,circle);
    return fits ? U::Nice : U::NotNice;
}

// =============================================
// CORE: one four-sided pyramid's lateral triangle
// Grid scale 80 per real unit; base=60, altitude=50.
// =============================================
static constexpr Triangle2 FACE{{0,0},{60,0},{30,50}};
static constexpr CircleU HEIGHT_CIRCLE{60,50,2500ULL};
static constexpr CircleU ENCLOSING_CIRCLE{60,32,4624ULL};
static_assert(VALIDATE(FACE,HEIGHT_CIRCLE)==U::NotNice);
static_assert(VALIDATE(FACE,ENCLOSING_CIRCLE)==U::Nice);
// The enclosing radius 34 is exact: all three face vertices
// lie on this boundary. The height-circle radius 25 is too small.
static_assert(DISTANCE2(FACE.a,ENCLOSING_CIRCLE)==4624ULL);
static_assert(DISTANCE2(FACE.b,ENCLOSING_CIRCLE)==4624ULL);
static_assert(DISTANCE2(FACE.c,ENCLOSING_CIRCLE)==4624ULL);
static_assert(DISTANCE2(FACE.a,HEIGHT_CIRCLE)>2500ULL);

// =============================================
// Q: 3-4-5 right triangle "Quaker" example
// =============================================
static constexpr Triangle2 QUAKER{{0,0},{30,0},{0,40}};
static constexpr CircleU QUAKER_CIRCLE{30,40,2500ULL};
static_assert(DOUBLE_AREA(QUAKER)==1200ULL);
static_assert(VALIDATE(QUAKER,QUAKER_CIRCLE)==U::Nice);
static_assert(DISTANCE2(QUAKER.a,QUAKER_CIRCLE)==2500ULL);
static_assert(DISTANCE2(QUAKER.b,QUAKER_CIRCLE)==2500ULL);
static_assert(DISTANCE2(QUAKER.c,QUAKER_CIRCLE)==2500ULL);

// =============================================
// TESSELLATION: nine exact triangles partition
// square [0,60]x[0,60] in three horizontal strips.
// =============================================
struct NineTriangles { Triangle2 face[9]; };
[[nodiscard]] constexpr NineTriangles BUILD_NINE() noexcept {
    NineTriangles result{};
    for (s64 i=0;i<3;++i) {
        const s64 y0=20*i,y1=y0+20;
        const unsigned j=static_cast<unsigned>(3*i);
        result.face[j]={{0,y0},{60,y0},{30,y1}};
        result.face[j+1u]={{0,y0},{0,y1},{30,y1}};
        result.face[j+2u]={{60,y0},{60,y1},{30,y1}};
    }
    return result;
}
static constexpr NineTriangles BASE9=BUILD_NINE();
static constexpr CircleU BASE_CIRCLE{60,60,7200ULL};
[[nodiscard]] constexpr bool VALIDATE_NINE() noexcept {
    for(unsigned i=0u;i<9u;++i)
        if(VALIDATE(BASE9.face[i],BASE_CIRCLE)!=U::Nice)
            return false;
    return true;
}
[[nodiscard]] constexpr u64 SUM_NINE_DOUBLE_AREA() noexcept {
    u64 sum=0ULL;
    for(unsigned i=0u;i<9u;++i)
        sum+=DOUBLE_AREA(BASE9.face[i]);
    return sum;
}
static_assert(VALIDATE_NINE());
static_assert(SUM_NINE_DOUBLE_AREA()==7200ULL);
static_assert(DOUBLE_AREA(BASE9.face[0])==1200ULL);
static_assert(DOUBLE_AREA(BASE9.face[1])==600ULL);
static_assert(DOUBLE_AREA(BASE9.face[2])==600ULL);
static_assert(DOUBLE_AREA(BASE9.face[3])==1200ULL);

// =============================================
// COLUMNAR: D=A/T, ratio from one large patch.
// The ratio is a named exploratory area fraction,
// NOT an established Euclidean triangle type.
// =============================================
static constexpr u64 A=DOUBLE_AREA(BASE9.face[0])/2ULL;
static constexpr u64 T=SUM_NINE_DOUBLE_AREA()/2ULL;
static constexpr u64 D_NUMERATOR=1ULL;
static constexpr u64 D_DENOMINATOR=6ULL;
static_assert(A==600ULL && T==3600ULL);
static_assert(A*D_DENOMINATOR==T*D_NUMERATOR);

// =============================================
// ASTRIX: 5-point star, five vertex indices,
// pentagram connections stay inside convex disk.
// =============================================
static constexpr Point STAR[5]{
    {30,0},{58,21},{48,54},{12,54},{2,21}
};
static constexpr unsigned STAR_ORDER[5]{0u,2u,4u,1u,3u};
[[nodiscard]] constexpr bool VALIDATE_ASTRIX() noexcept {
    for(unsigned i=0u;i<5u;++i) {
        if(STAR_ORDER[i]>=5u || !INSIDE(STAR[i],BASE_CIRCLE))
            return false;
        for(unsigned j=i+1u;j<5u;++j)
            if(STAR_ORDER[i]==STAR_ORDER[j])return false;
    }
    return true;
}
static_assert(VALIDATE_ASTRIX());

// =============================================
// Q62: use the previous canonical limb algorithm.
// Avoid a second copy that could drift from CORE.
// =============================================
using Wide=core::Wide;
[[nodiscard]] constexpr Wide MULTIPLY(u64 a,u64 b) noexcept {
    return core::multiply(a,b);
}
[[nodiscard]] constexpr u64 SHIFT62(Wide value) noexcept {
    return core::shift62(value);
}
[[nodiscard]] constexpr u64 SHIFT63(Wide value) noexcept {
    return core::shift63(value);
}

// =============================================
// FOUR-SIDED PYRAMID CORE, exact Q62 witnesses.
// =============================================
static constexpr u64 SIDE=3ULL*Q/4ULL;
// U-CIRCLE-FIT-PR34-009-REV1: overflow-safe Q62 multiplication order.
// Historic source blob d28c7b7b8604aca910eea47e93b06edbccf9fbe1 stays in Git history.
// 5*Q cannot fit u64 before dividing by 8; Q/8 can.
static constexpr u64 SLANT=5ULL*(Q/8ULL);
static_assert(SLANT==2882303761517117440ULL);
static_assert((5ULL*Q/8ULL)!=SLANT); // deliberate overflow counterexample
static constexpr u64 BASE=SHIFT62(MULTIPLY(SIDE,SIDE));
static constexpr u64 F1=SHIFT63(MULTIPLY(SIDE,SLANT));
static constexpr u64 F2=F1,F3=F1,F4=F1;
static constexpr u64 F=F1+F2+F3+F4;
static constexpr u64 TOTAL=BASE+F;
static_assert(SIDE==pyramid::SIDE && SLANT==pyramid::SLANT);
static_assert(BASE==pyramid::BASE);
static_assert(F1==pyramid::F1 && F2==pyramid::F2 &&
              F3==pyramid::F3 && F4==pyramid::F4);
static_assert(F==pyramid::F && TOTAL==pyramid::TOTAL);
static_assert(TOTAL==6917529027641081856ULL);

// =============================================
// REDUXUALCHOICE: preserve all seven indexed
// ratios using the previously defined operation.
// =============================================
template<int K>
[[nodiscard]] constexpr u64 REDUXUALCHOICE() noexcept {
    static_assert(K>=-3 && K<=3);
    return pyramid::REDUXUALCHOICE<K>();
}
static constexpr u64 R_NEG3=REDUXUALCHOICE<-3>();
static constexpr u64 R_NEG2=REDUXUALCHOICE<-2>();
static constexpr u64 R_NEG1=REDUXUALCHOICE<-1>();
static constexpr u64 R_ZERO=REDUXUALCHOICE<0>();
static constexpr u64 R_POS1=REDUXUALCHOICE<1>();
static constexpr u64 R_POS2=REDUXUALCHOICE<2>();
static constexpr u64 R_POS3=REDUXUALCHOICE<3>();
static constexpr u64 SIGMA=R_NEG3+R_NEG2+R_NEG1+R_ZERO+
    R_POS1+R_POS2+R_POS3;
static_assert(SIGMA==15132094747964866560ULL);
static_assert(SIGMA==pyramid::SIGMA_REDUXUALCHOICE);

// =============================================
// FINAL Ⓤ VALIDATION
// =============================================
static constexpr U TRIANGLE_RESULT=
    VALIDATE(FACE,ENCLOSING_CIRCLE);
static constexpr U TESSELLATION_RESULT=
    VALIDATE_NINE()?U::Nice:U::NotNice;
static constexpr U ASTRIX_RESULT=
    VALIDATE_ASTRIX()?U::Nice:U::NotNice;
static_assert(TRIANGLE_RESULT==U::Nice);
static_assert(TESSELLATION_RESULT==U::Nice);
static_assert(ASTRIX_RESULT==U::Nice);

// Negative witnesses: degenerate, outside, and invalid grid.
static constexpr Triangle2 DEGENERATE{{0,0},{1,1},{2,2}};
static_assert(VALIDATE(DEGENERATE,BASE_CIRCLE)==U::NotNice);
static_assert(!INSIDE({1000001,0},BASE_CIRCLE));
static_assert(VALIDATE(FACE,{60,50,0ULL})==U::NotNice);
// This is projected circle containment, not a physical folded model.
static constexpr bool PHYSICAL_FOLD_VERIFIED=false;
static_assert(!PHYSICAL_FOLD_VERIFIED);
} // namespace swe::u_circle_fit
#endif // SWE_U_CIRCLE_FIT_HPP
