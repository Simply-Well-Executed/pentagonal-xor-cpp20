/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
// FOUR-PYRAMID-REDUXUALCHOICE-PR34-008 | EXPLORATORY
// Independent right square pyramid, bounded seven-term Sigma
// Reduxualchoice, exact nine-triangle square-base partition,
// and three rational apex-ray markers at z=2*h/3.
// Zero includes, STL, heap, float, network, or compiler extensions.
// Requires sigma_form_area_q62.hpp, right_triangle_f123_original.hpp,
// and sigma_f_redaxualchoice.hpp before this header.
// All previous source witnesses and historical lanes remain intact.
#ifndef SWE_FOUR_PYRAMID_REDUXUALCHOICE_HPP
#define SWE_FOUR_PYRAMID_REDUXUALCHOICE_HPP
#ifndef SWE_SIGMA_FORM_AREA_Q62_HPP
#error "Include sigma_form_area_q62.hpp first"
#endif
#ifndef SWE_RIGHT_TRIANGLE_F123_ORIGINAL_HPP
#error "Include right_triangle_f123_original.hpp first"
#endif
#ifndef SWE_SIGMA_F_REDAXUALCHOICE_HPP
#error "Include sigma_f_redaxualchoice.hpp first"
#endif
namespace swe::four_pyramid_reduxualchoice {
using u64 = unsigned long long;
namespace area = swe::sigma_form_area;
namespace three = swe::right_triangle_f123_original;
inline constexpr const char* experiment_id =
    "FOUR-PYRAMID-REDUXUALCHOICE-PR34-008";
static constexpr u64 Q = 1ULL << 62u;

// CORE: the already witnessed right-square-pyramid sample.
static constexpr u64 SIDE = 3ULL*(Q/4ULL);
static constexpr u64 HEIGHT = Q/2ULL;
static constexpr u64 SLANT = 5ULL*(Q/8ULL);
static constexpr area::SquarePyramid CORE =
    area::right_square_pyramid(SIDE,HEIGHT,SLANT);
static_assert(CORE.right_square_geometry_verified);
static constexpr u64 BASE = CORE.base.value;
static constexpr u64 F1 = CORE.one_triangle.value;
static constexpr u64 F2 = CORE.one_triangle.value;
static constexpr u64 F3 = CORE.one_triangle.value;
static constexpr u64 F4 = CORE.one_triangle.value;
static constexpr u64 F = F1 + F2 + F3 + F4;
static constexpr u64 TOTAL = BASE + F;
static_assert(BASE == 2594073385365405696ULL);
static_assert(F1 == 1080863910568919040ULL);
static_assert(F2 == F1 && F3 == F1 && F4 == F1);
static_assert(F == 4323455642275676160ULL);
static_assert(TOTAL == 6917529027641081856ULL);
static_assert(F == CORE.lateral.value && TOTAL == CORE.total.value);

// NINE triangle partition: divide the square into 3 horizontal
// strips. In each strip, join the top-edge midpoint to the two
// bottom corners and the two top corners. Areas are
// (base/6, base/12, base/12) per strip, exactly.
// The nine triangles are NOT nine equal-area triangles.
struct NineBaseTriangles {
    area::Q62Area face[9];
    area::Q62Area sum;
    bool exact_partition_verified;
};
[[nodiscard]] constexpr NineBaseTriangles nine_base_triangles(
    u64 side,area::SquarePyramid geometry) noexcept {
    NineBaseTriangles witness{};
    if (!geometry.right_square_geometry_verified ||
        side == 0ULL || side > Q || side % 6ULL != 0ULL ||
        geometry.base.value != area::sigma_a_square_base(side).value ||
        geometry.base.value % 12ULL != 0ULL)
        return witness;
    const u64 large=geometry.base.value/6ULL;
    const u64 small=geometry.base.value/12ULL;
    u64 sum=0ULL;
    for (unsigned strip=0u;strip<3u;++strip) {
        const unsigned i=3u*strip;
        witness.face[i]={large,true};
        witness.face[i+1u]={small,true};
        witness.face[i+2u]={small,true};
        sum+=large+small+small;
    }
    witness.sum={sum,sum==geometry.base.value};
    witness.exact_partition_verified=witness.sum.valid;
    return witness;
}
static constexpr NineBaseTriangles NINE =
    nine_base_triangles(SIDE,CORE);
static_assert(NINE.exact_partition_verified &&
              NINE.sum.value==BASE);
static_assert(NINE.face[0].value==432345564227567616ULL);
static_assert(NINE.face[1].value==216172782113783808ULL);
static_assert(NINE.face[2].value==NINE.face[1].value);
static_assert(NINE.face[3].value==NINE.face[0].value);
static_assert(NINE.face[6].value==NINE.face[0].value);

// Three EXPLORATORY apex-edge markers at two-thirds of height.
// Base corners A=(0,0,0), B=(s,0,0), C=(s,s,0) and
// apex P=(s/2,s/2,h). Markers are P + (corner-P)/3.
// Their rational coordinates share denominator 3. No rounding
// of z=2*h/3 into an inexact binary Q62 grid is permitted.
// Three markers alone do NOT define a four-vertex diamond.
struct Marker3 {
    u64 x_numerator,y_numerator,z_numerator,denominator;
};
struct ThreeUpperMarkers {
    Marker3 on_PA,on_PB,on_PC;
    bool rational_geometry_verified;
    bool physical_fold_or_diamond_verified;
};
[[nodiscard]] constexpr ThreeUpperMarkers upper_markers(
    u64 side,u64 height) noexcept {
    if (side==0ULL || height==0ULL || side>Q ||
        height>Q || (side & 1ULL)!=0ULL)
        return {{0,0,0,0},{0,0,0,0},{0,0,0,0},false,false};
    const u64 znum=2ULL*height;
    const Marker3 a{side,side,znum,3ULL};
    const Marker3 b{2ULL*side,side,znum,3ULL};
    const Marker3 c{2ULL*side,2ULL*side,znum,3ULL};
    return {a,b,c,true,false};
}
static constexpr ThreeUpperMarkers TOP3 =
    upper_markers(SIDE,HEIGHT);
static_assert(TOP3.rational_geometry_verified);
static_assert(TOP3.on_PA.x_numerator==SIDE &&
              TOP3.on_PA.y_numerator==SIDE);
static_assert(TOP3.on_PB.x_numerator==2ULL*SIDE &&
              TOP3.on_PB.y_numerator==SIDE);
static_assert(TOP3.on_PC.x_numerator==2ULL*SIDE &&
              TOP3.on_PC.y_numerator==2ULL*SIDE);
static_assert(TOP3.on_PA.z_numerator==2ULL*HEIGHT &&
              TOP3.on_PB.z_numerator==2ULL*HEIGHT &&
              TOP3.on_PC.z_numerator==2ULL*HEIGHT);
static_assert(TOP3.on_PA.denominator==3ULL &&
              TOP3.on_PB.denominator==3ULL &&
              TOP3.on_PC.denominator==3ULL);
static_assert(!TOP3.physical_fold_or_diamond_verified);

// SAMPLE-ONLY step-up scalar ratio, not a universal morphism:
// four-sided lateral sample F / frozen three-sided lateral = 15/14.
struct LateralSampleRatio {
    u64 four_lateral_q62,three_lateral_q62;
    u64 numerator,denominator;
    bool exact_sample_ratio;
    bool shape_transition_proved;
};
[[nodiscard]] constexpr LateralSampleRatio sample_step_up() noexcept {
    const bool exact=area::equal_wide(
        area::multiply(F,14ULL),
        area::multiply(three::LATERAL_Q62,15ULL));
    return {F,three::LATERAL_Q62,15ULL,14ULL,exact,false};
}
static constexpr LateralSampleRatio STEP_UP=sample_step_up();
static_assert(STEP_UP.exact_sample_ratio);
static_assert(!STEP_UP.shape_transition_proved);

// Reuse the original, separately verified Redaxualchoice arithmetic.
// The alternate spelling REDUXUALCHOICE names this new four-sided lane;
// do not rename or change the earlier REDAXUALCHOICE implementation.
template<int K>
[[nodiscard]] constexpr u64 REDUXUALCHOICE() noexcept {
    return swe::sigma_f_redaxualchoice::REDAXUALCHOICE<K,F>();
}
static constexpr u64 R_NEG3=REDUXUALCHOICE<-3>();
static constexpr u64 R_NEG2=REDUXUALCHOICE<-2>();
static constexpr u64 R_NEG1=REDUXUALCHOICE<-1>();
static constexpr u64 R_ZERO=REDUXUALCHOICE<0>();
static constexpr u64 R_POS1=REDUXUALCHOICE<1>();
static constexpr u64 R_POS2=REDUXUALCHOICE<2>();
static constexpr u64 R_POS3=REDUXUALCHOICE<3>();
static_assert(R_NEG3==F/2ULL && R_NEG2==F/2ULL &&
              R_NEG1==F/2ULL && R_ZERO==F/2ULL &&
              R_POS1==F/2ULL && R_POS2==F/2ULL &&
              R_POS3==F/2ULL);

static constexpr u64 SIGMA_REDUXUALCHOICE=
    R_NEG3+R_NEG2+R_NEG1+R_ZERO+R_POS1+R_POS2+R_POS3;
static_assert(SIGMA_REDUXUALCHOICE == 7ULL*(F/2ULL));
static_assert(SIGMA_REDUXUALCHOICE == 15132094747964866560ULL);
static_assert(area::decimal17({SIGMA_REDUXUALCHOICE,true}) ==
              328125000000000000ULL);
static_assert(BASE+F==TOTAL);
// Invariants: neither 9 base triangles nor the 3 point markers
// prove the user's proposed fold, and this Sigma is not area.
static constexpr bool missing_tip_or_fold_verified=false;
} // namespace swe::four_pyramid_reduxualchoice
#endif // SWE_FOUR_PYRAMID_REDUXUALCHOICE_HPP
