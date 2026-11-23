// SPDX-License-Identifier: 0BSD
// DIAERY-HARMONY-PR34-018 | tolerance is explicit, not guessed.
// Humanistic HARMONY / patience is a metaphor, not a physical model.
// Engineering tolerance applies ONLY to approximate witnesses;
// exact identities and topology never permit silent drift.
// No STL, includes, runtime trigonometry, heap or network operations.
#ifndef SWE_DIAERY_HARMONY_HPP
#define SWE_DIAERY_HARMONY_HPP
#ifndef SWE_SLIDINGTOOLONG_HPP
#error "Include slidingtoolong.hpp before harmony.hpp"
#endif
#ifndef SWE_DIAERY_STRICT_PROFILE_HPP
#error "Include strict_profile.hpp before harmony.hpp"
#endif
namespace swe::diaery::harmony {
using u64=unsigned long long;
using s64=long long;
namespace slide=swe::slidingtoolong;
namespace str=swe::diaery::strict;
static constexpr unsigned TRIANGLES=64u;
static constexpr unsigned ONE_DEGREE=1u;
static constexpr unsigned FULL_CIRCLE_DEGREES=360u;
static constexpr unsigned COVERED_DEGREES=TRIANGLES*ONE_DEGREE;
static constexpr unsigned OPEN_DEGREES=FULL_CIRCLE_DEGREES-COVERED_DEGREES;
static constexpr unsigned SQUARED_64=TRIANGLES*TRIANGLES;
static_assert(COVERED_DEGREES==64u);
static_assert(OPEN_DEGREES==296u);
static_assert(SQUARED_64==4096u);
// sqrt(360) is irrational. This integer algorithm establishes
// its FLOOR at micro-unit scale, with < 1 micro-unit error.
[[nodiscard]] constexpr u64 ISQRT(u64 input) noexcept {
 u64 lo=0ULL,hi=input<4294967295ULL?input:4294967295ULL;
 while(lo<hi) {
  const u64 m=lo+(hi-lo+1ULL)/2ULL;
  if(m<=input/m)lo=m;
  else hi=m-1ULL;
 }
 return lo;
}
static constexpr u64 RADICAL_360_SCALED_TARGET=360000000000000ULL;
static constexpr u64 RADICAL_360_MICRO=ISQRT(RADICAL_360_SCALED_TARGET);
static_assert(RADICAL_360_MICRO==18973665ULL);
static_assert(RADICAL_360_MICRO*RADICAL_360_MICRO<=
 RADICAL_360_SCALED_TARGET);
static_assert((RADICAL_360_MICRO+1ULL)*(RADICAL_360_MICRO+1ULL)>
 RADICAL_360_SCALED_TARGET);
// The tolerance is a strict < 1/1,000,000 numerical interval,
// NOT an allowance for wrong combinatorial/topological structure.
static constexpr u64 ROOT_TOLERANCE_MICRO=1ULL;
// Euler's Jig vs conventional math e: distinct named dimensions.
static constexpr u64 EULERS_JIG_AREA_X1E17=slide::EULER_JIG_ONE_X1E17;
static constexpr u64 MATH_E_SCALAR_X1E18=slide::MATH_E_X1E18;
static_assert(EULERS_JIG_AREA_X1E17==2864498081537971875ULL);
static_assert(MATH_E_SCALAR_X1E18==2718281828459045235ULL);

// FOLDING A TRIANGLE from a 60x60 square sheet:
// 3 panels of 60 x 20 -> 2 hinges at 120 degrees -> equilateral
// triangular tube, two additional 20-edge caps. Closed cross section.
// Algebraic coordinates (U + V*sqrt(3)); sqrt(3) is never rounded.
struct Pt {s64 u,v;};
static constexpr Pt A{0,0},B{20,0},C{10,10};
[[nodiscard]] constexpr u64 LENGTH2(Pt a,Pt b) noexcept {
 const s64 du=a.u-b.u,dv=a.v-b.v;
 return static_cast<u64>(du*du+3LL*dv*dv);
}
static_assert(LENGTH2(A,B)==400ULL);
static_assert(LENGTH2(B,C)==400ULL);
static_assert(LENGTH2(C,A)==400ULL);
static constexpr unsigned PANELS=3u,HINGES=2u;
static constexpr unsigned PANEL_WIDTH=20u,TUBE_LENGTH=60u;
static constexpr unsigned TRI_CAPS=2u;
static constexpr unsigned TRIANGULAR_PERIMETER=3u*PANEL_WIDTH;
static constexpr unsigned AVAILABLE_WALL_STRIP=60u;
static_assert(TRIANGULAR_PERIMETER==AVAILABLE_WALL_STRIP);
// Each cap has exact symbolic area = 100*sqrt(3) square units,
// bound between 100 and 200. It fits inside each 60x60 copy.
// Cap offcuts remain: 3600 - 100*sqrt(3) per copy.
static constexpr u64 CAP_AREA_SQUARED=30000ULL;
static constexpr bool CAPS_FIT_SOURCE_60X60=
  PANEL_WIDTH<=60u && LENGTH2(A,C)<=400ULL;
static_assert(CAP_AREA_SQUARED==30000ULL);
static_assert(CAPS_FIT_SOURCE_60X60);

// By contrast the one-degree Euler's Jig triangle with height=1
// and area >28.5 has base >57. The hypotenuse is longer than
// the base; its three-edge perimeter exceeds 2*57+1 =115.
// A 60-wide folded strip cannot close its side walls.
// But the thin triangle's cap outline still fits 60x60.
static constexpr bool EULER_JIG_BASE_GT_57=
 EULERS_JIG_AREA_X1E17>2850000000000000000ULL;
static constexpr bool EULER_JIG_CAP_FITS_SHEET=
 EULERS_JIG_AREA_X1E17<3000000000000000000ULL;
static constexpr bool EULER_JIG_NEEDS_EXTRA_WALL_MATERIAL=
 EULER_JIG_BASE_GT_57 && 2u*57u+1u>AVAILABLE_WALL_STRIP;
static_assert(EULER_JIG_BASE_GT_57 &&
 EULER_JIG_CAP_FITS_SHEET &&
 EULER_JIG_NEEDS_EXTRA_WALL_MATERIAL);
// This is NOT a geometry change to the prior square-tube source.
[[nodiscard]] constexpr bool INTEGRITY() noexcept {
 return slide::QUALIFIED() && SQUARED_64==4096u &&
  COVERED_DEGREES+OPEN_DEGREES==FULL_CIRCLE_DEGREES &&
  RADICAL_360_MICRO==18973665ULL &&
  LENGTH2(A,B)==LENGTH2(B,C) &&
  LENGTH2(B,C)==LENGTH2(C,A) &&
  TRIANGULAR_PERIMETER==AVAILABLE_WALL_STRIP &&
  CAPS_FIT_SOURCE_60X60 &&
  EULER_JIG_NEEDS_EXTRA_WALL_MATERIAL;
}
// All three records are ordinary, contiguous XHTML Strict paragraphs;
// the parser validates a narrow supported Strict subset, not a DTD.
[[nodiscard]] constexpr str::Builder RECORD(unsigned index) noexcept {
 str::Builder b{};
 switch(index) {
  case 0u:
   b.text("HARMONY-1|triangles=64|square=4096|covered=64|gap=296|root360_micro=");
   b.num(static_cast<long long>(RADICAL_360_MICRO));break;
  case 1u:
   b.text("HARMONY-CAP|panels=3|hinges=2|side=20|wall=60|caps=2|euler_airgap=1");break;
  case 2u:
   b.text("HARMONY-EULERS|area_x1e17=");
   b.num(static_cast<long long>(EULERS_JIG_AREA_X1E17));
   b.text("|math_e_x1e18=");
   b.num(static_cast<long long>(MATH_E_SCALAR_X1E18));break;
  default:b.ok=false;break;
 }
 return b;
}
[[nodiscard]] constexpr bool QUALIFY(const str::Page& page) noexcept {
 if(!page.structurally_valid||!INTEGRITY())return false;
 unsigned found=0u,previous=0u;
 for(unsigned i=0u;i<page.count;++i) {
  const auto& line=page.lines[i];
  if(str::prefix(line.text,"HARMONY-")) {
   if(found>=3u||line.tag!='p')return false;
   if(found!=0u&&i!=previous+1u)return false;
   const auto expected=RECORD(found);
   if(!expected.ok||!str::same(line.text,expected.s))return false;
   previous=i;++found;
  }
 }
 return found==3u;
}
static constexpr auto H0=RECORD(0u),H1=RECORD(1u),H2=RECORD(2u);
static_assert(H0.ok && H1.ok && H2.ok);
static_assert(INTEGRITY());
static constexpr bool REAL_WORLD_SAFETY_CERTIFIED=false;
static constexpr bool QUANTUM_MECHANICS_MODEL_VERIFIED=false;
static constexpr bool FULL_XHTML_DTD_VERIFIED=false;
static_assert(!REAL_WORLD_SAFETY_CERTIFIED &&
 !QUANTUM_MECHANICS_MODEL_VERIFIED && !FULL_XHTML_DTD_VERIFIED);
} // namespace swe::diaery::harmony
#endif
