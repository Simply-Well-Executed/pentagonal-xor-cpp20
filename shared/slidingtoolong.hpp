/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
// SLIDINGTOOLONG-PR34-017 | add-on to historical TOOLONG, never a rename.
// Headerless/STL-free ISO C++20. Include toolong_q62_core.hpp first.
// Three 32-bit data lanes + a 32-bit watchdog. 8/16/24-bit reversible
// slide tests operate at compile time only, not during rendering.
// 64 fractional bits (Q64) and INTEGER-PART are separate fields.
// No physical NVRAM, persistent memory, quantum prediction or time claims.
#ifndef SWE_SLIDINGTOOLONG_HPP
#define SWE_SLIDINGTOOLONG_HPP
#ifndef SWE_TOOLONG_Q62_CORE_HPP
#error "Include toolong_q62_core.hpp before slidingtoolong.hpp"
#endif
#include "eulers_jig_canonical.hpp"
namespace swe::slidingtoolong {
using uint64_t=unsigned long long;
using uint32_t=unsigned int;
static_assert(sizeof(uint64_t)==8 && sizeof(uint32_t)==4);
inline constexpr uint64_t ONE_Q64_FRACTION=0xffffffffffffffffULL;
struct Q64Constant {uint64_t integer_part,fraction;};
// Math e and pi: rounded-to-nearest Q64 fraction using supplied
// long decimal expansions. Not exact irrational numbers.
static constexpr Q64Constant MATH_E{2ULL,13249961062380153451ULL};
static constexpr Q64Constant MATH_PI{3ULL,2611923443488327891ULL};
// INTENSITY Q64: only the prior Q62 word lifted exactly by two bits.
// This does not synthesize two additional bits of measurement precision.
static constexpr Q64Constant INTENSITY_Q64{
  0ULL,swe::toolong_q62::intensity.source_word<<2u
};
// SOURCE Q62 is authoritative and kept unchanged.
static_assert(MATH_E.integer_part==2ULL);
static_assert(MATH_PI.integer_part==3ULL);
static_assert(INTENSITY_Q64.fraction==1263472187042697908ULL);
static_assert(
 ((swe::toolong_q62::euler.source_word & ((1ULL<<62u)-1ULL))<<2u)
 +3ULL==MATH_E.fraction
);
static_assert(
 ((swe::toolong_q62::pi.source_word & ((1ULL<<62u)-1ULL))<<2u)
 ==MATH_PI.fraction+1ULL
);
// Historical decimal witness output, distinct from Q64 fraction.
// 18 decimal places, fully stored in a single uint64_t.
// EULER'S JIG is a DIFFERENT named quantity and appears below.
inline constexpr uint64_t DECIMAL_SCALE_1E18=1000000000000000000ULL;
inline constexpr uint64_t MATH_E_X1E18=2718281828459045235ULL;
inline constexpr uint64_t MATH_PI_X1E18=3141592653589793238ULL;
inline constexpr uint64_t INTENSITY_X1E18=68492964503335230ULL;
static_assert(MATH_E_X1E18/DECIMAL_SCALE_1E18==2ULL);
static_assert(MATH_PI_X1E18/DECIMAL_SCALE_1E18==3ULL);
// User's Euler's Jig is area per triangle, NOT mathematical e.
// Exact quotient of the DISPLAYED total area 1833.278772184302 / 64.
inline constexpr uint64_t EULER_JIG_TOTAL_X1E12=
 swe::eulers_jig::TOTAL_X1E12;
inline constexpr uint64_t EULER_JIG_ONE_X1E17=
 swe::eulers_jig::ONE_TRIANGLE_X1E17;
static_assert(((EULER_JIG_TOTAL_X1E12%64ULL)*100000ULL)%64ULL==0ULL);
static_assert(EULER_JIG_ONE_X1E17==2864498081537971875ULL);
// A fixed, explicitly EXPLORATORY bit-mixing witness, not a
// conventional mathematical Sigma or physical parameter.
inline constexpr uint64_t SIGMA_Q64_XOR=
 MATH_E.fraction ^ MATH_PI.fraction ^ INTENSITY_Q64.fraction;
static_assert(SIGMA_Q64_XOR==9391969674587680780ULL);

[[nodiscard]] constexpr uint32_t ROL32(uint32_t value,unsigned bits) noexcept {
 return bits==8u||bits==16u||bits==24u?
  static_cast<uint32_t>((value<<bits)|(value>>(32u-bits))):value;
}
[[nodiscard]] constexpr uint32_t ROR32(uint32_t value,unsigned bits) noexcept {
 return bits==8u||bits==16u||bits==24u?
  static_cast<uint32_t>((value>>bits)|(value<<(32u-bits))):value;
}
struct Register3Watch {
 uint32_t lane_lo,lane_hi,lane_tag,watchdog;
};
[[nodiscard]] constexpr uint32_t WATCHDOG32(
 uint32_t low,uint32_t high,uint32_t tag) noexcept {
 return low ^ ROL32(high,8u) ^ ROL32(tag,16u) ^ 0x5A39C4E1u;
}
[[nodiscard]] constexpr Register3Watch PACK(
 uint64_t word,uint32_t tag) noexcept {
 const uint32_t low=static_cast<uint32_t>(word & 0xffffffffULL);
 const uint32_t high=static_cast<uint32_t>(word>>32u);
 return {low,high,tag,WATCHDOG32(low,high,tag)};
}
[[nodiscard]] constexpr bool PING(Register3Watch p) noexcept {
 return p.watchdog==WATCHDOG32(p.lane_lo,p.lane_hi,p.lane_tag);
}
[[nodiscard]] constexpr uint64_t JOIN(Register3Watch p) noexcept {
 return (static_cast<uint64_t>(p.lane_hi)<<32u) | p.lane_lo;
}
[[nodiscard]] constexpr Register3Watch SLIDE(
 Register3Watch p,unsigned bits) noexcept {
 if(!PING(p)||(bits!=8u&&bits!=16u&&bits!=24u))return {};
 const uint32_t low=ROL32(p.lane_lo,bits),
                high=ROL32(p.lane_hi,bits),
                tag=ROL32(p.lane_tag,bits);
 return {low,high,tag,WATCHDOG32(low,high,tag)};
}
[[nodiscard]] constexpr Register3Watch UNSLIDE(
 Register3Watch p,unsigned bits) noexcept {
 if(!PING(p)||(bits!=8u&&bits!=16u&&bits!=24u))return {};
 const uint32_t low=ROR32(p.lane_lo,bits),
                high=ROR32(p.lane_hi,bits),
                tag=ROR32(p.lane_tag,bits);
 return {low,high,tag,WATCHDOG32(low,high,tag)};
}
[[nodiscard]] constexpr bool ROUNDTRIP(
 uint64_t value,uint32_t tag,unsigned bits) noexcept {
 const auto source=PACK(value,tag);
 const auto moved=SLIDE(source,bits);
 const auto recovered=UNSLIDE(moved,bits);
 return PING(source)&&PING(moved)&&PING(recovered)&&
  recovered.lane_tag==tag&&JOIN(recovered)==value&&
  recovered.watchdog==source.watchdog;
}
[[nodiscard]] constexpr bool REJECT_CORRUPTION() noexcept {
 auto p=PACK(MATH_E.fraction,0xA5000001u);
 p.lane_lo^=1u;
 auto q=PACK(MATH_PI.fraction,0xA5000002u);
 q.watchdog^=1u;
 auto r=PACK(SIGMA_Q64_XOR,0xA5000003u);
 r.lane_tag^=0x01000000u;
 return !PING(p)&&!PING(q)&&!PING(r);
}
[[nodiscard]] constexpr bool QUALIFIED() noexcept {
 const uint64_t v[4]{MATH_E.fraction,MATH_PI.fraction,
   INTENSITY_Q64.fraction,SIGMA_Q64_XOR};
 for(unsigned i=0u;i<4u;++i) {
  const uint32_t tag=0xA5000000u|static_cast<uint32_t>(i+1u);
  if(!ROUNDTRIP(v[i],tag,8u)||!ROUNDTRIP(v[i],tag,16u)||
     !ROUNDTRIP(v[i],tag,24u))return false;
 }
 return REJECT_CORRUPTION()&&EULER_JIG_ONE_X1E17==
   2864498081537971875ULL;
}
static_assert(QUALIFIED());
static constexpr bool PHYSICAL_NVRAM_VERIFIED=false;
static constexpr bool COMPLETE_PI_DIGITS_VERIFIED=false;
static constexpr bool RUNTIME_SPEEDUP_PROVED=false;
static_assert(!PHYSICAL_NVRAM_VERIFIED &&
 !COMPLETE_PI_DIGITS_VERIFIED && !RUNTIME_SPEEDUP_PROVED);
} // namespace swe::slidingtoolong
#endif
