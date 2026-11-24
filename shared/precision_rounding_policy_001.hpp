// SPDX-License-Identifier: 0BSD
// PRECISION-ROUNDING-001: derived Q62 display views, no canonical source edits.
// Headerless / STL-free ISO C++20. Full 128-bit product retained as two u64s.
// All conversions keep the exact remainder and round to nearest, ties to even.
// Historical Q62 multiply_shift62 outputs are not rewritten.
#ifndef SWE_PRECISION_ROUNDING_POLICY_001_HPP
#define SWE_PRECISION_ROUNDING_POLICY_001_HPP

namespace swe::precision_rounding_policy_001 {
using u64 = unsigned long long;
static_assert(sizeof(u64)==8);
inline constexpr u64 Q62_DENOMINATOR = 1ULL << 62u;
inline constexpr u64 Q62_REMAINDER_MASK = Q62_DENOMINATOR - 1ULL;

struct Product128 {u64 high,low;};
[[nodiscard]] constexpr Product128 multiply_exact(u64 a,u64 b) noexcept {
 const u64 mask=0xffffffffULL;
 const u64 a0=a&mask,a1=a>>32u,b0=b&mask,b1=b>>32u;
 const u64 p00=a0*b0,p01=a0*b1,p10=a1*b0,p11=a1*b1;
 const u64 middle=(p00>>32u)+(p01&mask)+(p10&mask);
 return {p11+(p01>>32u)+(p10>>32u)+(middle>>32u),
         ((middle&mask)<<32u)|(p00&mask)};
}
struct RoundedQ62 {
 bool valid;
 u64 rounded;
 u64 lower_neighbor;
 u64 remainder;
 bool rounded_up;
 Product128 exact_numerator;
};
// Exact rational before display: (exact_numerator.high:low) / 2^62.
// If quotient or rounded result cannot fit in 64 bits, fail closed.
[[nodiscard]] constexpr RoundedQ62 round_nearest_even(u64 q62_word,u64 decimal_scale) noexcept {
 if(decimal_scale==0ULL)return {};
 const Product128 p=multiply_exact(q62_word,decimal_scale);
 if((p.high>>62u)!=0ULL)return {};
 const u64 lower=(p.high<<2u)|(p.low>>62u);
 const u64 rem=p.low&Q62_REMAINDER_MASK;
 const u64 half=Q62_DENOMINATOR>>1u;
 const bool up=(rem>half)||(rem==half&&((lower&1ULL)!=0ULL));
 if(up&&lower==~0ULL)return {};
 return {true,lower+(up?1ULL:0ULL),lower,rem,up,p};
}
} // namespace swe::precision_rounding_policy_001
#endif
