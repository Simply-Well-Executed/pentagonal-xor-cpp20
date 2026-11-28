/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
// PRECISION-ROUNDING-001: Q62 derived display views; canonical records unchanged.
// ROUND-FIRST: retain a full exact product, separate head and tail by exact
// inverse subtraction, then round ONCE to nearest, halfway ties to ODD.
// Headerless / STL-free ISO C++20; no compiler-specific 128-bit integers.
#ifndef SWE_PRECISION_ROUNDING_POLICY_001_HPP
#define SWE_PRECISION_ROUNDING_POLICY_001_HPP

namespace swe::precision_rounding_policy_001 {
using u64 = unsigned long long;
static_assert(sizeof(u64)==8);
inline constexpr u64 Q62_DENOMINATOR = 1ULL << 62u;

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
 u64 lower_neighbor;           // Head H: candidate integer, not final rounding
 u64 remainder;                // Tail T: exactly N - H*D
 bool rounded_up;
 bool exactly_half;
 bool residual_above_exact;    // Sign of R*D - N (false when below or exact)
 u64 residual_magnitude;      // Magnitude of R*D - N, in numerator units
 Product128 exact_numerator;   // N: source_word * decimal_scale, unreduced
};

// With D=2^62: N=H*D+T; T=N-H*D, 0<=T<D.
// Compare 2*T to D without overflowing: compare T to D/2.
// At an exact half choose the ODD neighbor; NO tail multiplication by 3.
// If a rounded value cannot fit u64, report invalid instead of wrapping.
[[nodiscard]] constexpr RoundedQ62 round_first_nearest_odd(
 u64 q62_word,u64 decimal_scale) noexcept {
 if(decimal_scale==0ULL)return {};
 const Product128 n=multiply_exact(q62_word,decimal_scale);
 if((n.high>>62u)!=0ULL)return {}; // head would exceed u64
 const u64 head=(n.high<<2u)|(n.low>>62u);
 // Compute H*2^62 as TWO u64 words, then subtract from the exact numerator.
 const Product128 head_scaled{head>>2u,head<<62u};
 const u64 borrow=(n.low<head_scaled.low)?1ULL:0ULL;
 if(n.high<head_scaled.high+borrow)return {};
 const u64 tail_high=n.high-head_scaled.high-borrow;
 const u64 tail=n.low-head_scaled.low;
 if(tail_high!=0ULL||tail>=Q62_DENOMINATOR)return {};
 // Reconstruct the exact 128-bit numerator before any rounding decision.
 const u64 recon_low=head_scaled.low+tail;
 const u64 recon_high=head_scaled.high+
                       ((recon_low<head_scaled.low)?1ULL:0ULL);
 if(recon_low!=n.low||recon_high!=n.high)return {};
 const u64 half=Q62_DENOMINATOR>>1u;
 const bool tie=(tail==half);
 const bool up=(tail>half)||(tie&&((head&1ULL)==0ULL));
 if(up&&head==~0ULL)return {};
 const u64 result=head+(up?1ULL:0ULL);
 // Exact signed residual: R*D-N, represented as sign + magnitude.
 const u64 residual=up?(Q62_DENOMINATOR-tail):tail;
 return {true,result,head,tail,up,tie,up,residual,n};
}
} // namespace swe::precision_rounding_policy_001
#endif
