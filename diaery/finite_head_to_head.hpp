/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
// DIAERY-FINITE-NASCAR-PR34-019
// Exact, bounded 64-step head-to-head: user Euler's Jig area vs
// conventional mathematical e. Neither source definition is replaced.
// BASE=10^9 unsigned limbs, no STL, no system headers, no __uint128_t.
// Fixed capacity is explicit: no infinite-precision claim.
// Work occurs in native qualification, NEVER per-frame rendering.
#ifndef SWE_DIAERY_FINITE_HEAD_TO_HEAD_HPP
#define SWE_DIAERY_FINITE_HEAD_TO_HEAD_HPP
#ifndef SWE_SLIDINGTOOLONG_HPP
#error "Include model_bridge.hpp before finite_head_to_head.hpp"
#endif
namespace swe::diaery::finite_head_to_head {
using u32=unsigned int;
using u64=unsigned long long;
static_assert(sizeof(u32)==4 && sizeof(u64)==8);
namespace slide=swe::slidingtoolong;
inline constexpr u64 BASE=1000000000ULL;
inline constexpr u64 MOD=1000000007ULL;
inline constexpr unsigned MAX_LIMBS=192u;
inline constexpr unsigned MAX_STEPS=64u;
static_assert(slide::EULER_JIG_ONE_X1E17==2864498081537971875ULL);
static_assert(slide::MATH_E_X1E18==2718281828459045235ULL);
struct Big {
 u32 limb[MAX_LIMBS]{};
 unsigned used=1u;
 bool valid=true;
};
[[nodiscard]] constexpr Big FROM64(u64 n) noexcept {
 Big r{};r.used=0u;
 do {
  r.limb[r.used++]=static_cast<u32>(n%BASE);
  n/=BASE;
 } while(n!=0ULL);
 return r;
}
constexpr void TRIM(Big& n) noexcept {
 while(n.used>1u && n.limb[n.used-1u]==0u)--n.used;
}
[[nodiscard]] constexpr bool VALID(Big n) noexcept {
 if(!n.valid||n.used==0u||n.used>MAX_LIMBS)return false;
 if(n.used>1u&&n.limb[n.used-1u]==0u)return false;
 for(unsigned i=0u;i<n.used;++i)if(n.limb[i]>=BASE)return false;
 return true;
}
[[nodiscard]] constexpr Big MULTIPLY(Big a,Big b) noexcept {
 Big out{};
 if(!VALID(a)||!VALID(b)||a.used+b.used>MAX_LIMBS){
  out.valid=false;return out;
 }
 out.used=a.used+b.used;
 for(unsigned i=0u;i<a.used;++i) {
  u64 carry=0ULL;
  for(unsigned j=0u;j<b.used;++j) {
   const unsigned k=i+j;
   const u64 t=static_cast<u64>(a.limb[i])*b.limb[j]+
               static_cast<u64>(out.limb[k])+carry;
   out.limb[k]=static_cast<u32>(t%BASE);
   carry=t/BASE;
  }
  unsigned k=i+b.used;
  while(carry!=0ULL) {
   if(k>=MAX_LIMBS){out.valid=false;return out;}
   const u64 t=static_cast<u64>(out.limb[k])+carry;
   out.limb[k]=static_cast<u32>(t%BASE);
   carry=t/BASE;++k;
  }
 }
 TRIM(out);
 return out;
}
[[nodiscard]] constexpr int COMPARE(Big a,Big b) noexcept {
 if(a.used!=b.used)return a.used<b.used?-1:1;
 for(unsigned i=a.used;i>0u;--i) {
  if(a.limb[i-1u]!=b.limb[i-1u])
   return a.limb[i-1u]<b.limb[i-1u]?-1:1;
 }
 return 0;
}
[[nodiscard]] constexpr bool SAME(Big a,Big b) noexcept {
 return VALID(a)&&VALID(b)&&COMPARE(a,b)==0;
}
[[nodiscard]] constexpr u64 RESIDUE(Big n) noexcept {
 if(!VALID(n))return MOD;
 u64 residue=0ULL;
 for(unsigned i=n.used;i>0u;--i)
  residue=(residue*BASE+static_cast<u64>(n.limb[i-1u]))%MOD;
 return residue;
}
[[nodiscard]] constexpr unsigned DIGITS(Big n) noexcept {
 if(!VALID(n))return 0u;
 u32 top=n.limb[n.used-1u];
 unsigned count=1u;
 while(top>=10u){top/=10u;++count;}
 return (n.used-1u)*9u+count;
}
[[nodiscard]] constexpr bool CORRUPTION_REJECTED() noexcept {
 const Big source=FROM64(slide::EULER_JIG_ONE_X1E17);
 Big mutated=source;
 mutated.limb[0]^=1u;
 return VALID(mutated)&&RESIDUE(source)!=RESIDUE(mutated);
}
static_assert(CORRUPTION_REJECTED());
[[nodiscard]] constexpr Big JIG_BASE() noexcept {
 return MULTIPLY(FROM64(slide::EULER_JIG_ONE_X1E17),FROM64(10ULL));
}
[[nodiscard]] constexpr Big MATH_BASE() noexcept {
 return FROM64(slide::MATH_E_X1E18);
}
[[nodiscard]] constexpr Big COMMON_DENOMINATOR() noexcept {
 return FROM64(slide::DECIMAL_SCALE_1E18);
}
// Each F(n) is EXACT for the two user-provided FINITE DECIMAL values:
// F(n)=n*(numerator/10^18)^n.
// Its reciprocal G(n) is stored by swapping F's numerator/denominator.
// No approximation or floating-point inverse is performed.
struct Fraction {Big numerator;Big denominator;};
[[nodiscard]] constexpr Fraction RECIPROCAL(Fraction f) noexcept {
 return {f.denominator,f.numerator};
}
[[nodiscard]] constexpr bool RECIPROCAL_IS_EXACT(Fraction f) noexcept {
 const Fraction g=RECIPROCAL(f);
 const Fraction h=RECIPROCAL(g);
 return SAME(f.numerator,h.numerator)&&
        SAME(f.denominator,h.denominator)&&
        VALID(g.numerator)&&VALID(g.denominator);
}
struct Report {
 bool valid;
 unsigned steps;
 unsigned jig_u64_first_overflow;
 unsigned conventional_u64_first_overflow;
 unsigned jig_numerator_digits;
 unsigned conventional_numerator_digits;
 u64 jig_residue,conventional_residue,denominator_residue;
 bool finite_inverse_exact;
 bool exceeding_single_u64_is_expected;
};
[[nodiscard]] constexpr Report RUN(unsigned steps) noexcept {
 Report r{};
 if(steps==0u||steps>MAX_STEPS||!CORRUPTION_REJECTED())return r;
 const Big jbase=JIG_BASE(),ebase=MATH_BASE(),
           scale=COMMON_DENOMINATOR(),
           limit=FROM64(0xffffffffffffffffULL);
 if(!VALID(jbase)||!VALID(ebase)||!VALID(scale)||!VALID(limit))
  return r;
 Big jp=FROM64(1ULL),ep=FROM64(1ULL),den=FROM64(1ULL);
 u64 checkJ=1ULL,checkE=1ULL,checkD=1ULL;
 const u64 jm=RESIDUE(jbase),em=RESIDUE(ebase),dm=RESIDUE(scale);
 r.finite_inverse_exact=true;
 for(unsigned n=1u;n<=steps;++n) {
  jp=MULTIPLY(jp,jbase);
  ep=MULTIPLY(ep,ebase);
  den=MULTIPLY(den,scale);
  const Big index=FROM64(static_cast<u64>(n));
  const Big numeratorJ=MULTIPLY(jp,index);
  const Big numeratorE=MULTIPLY(ep,index);
  const Big bound=MULTIPLY(den,limit);
  if(!VALID(jp)||!VALID(ep)||!VALID(den)||
     !VALID(numeratorJ)||!VALID(numeratorE)||!VALID(bound))
   return r;
  // Input definitions differ. The comparison is of like-scaled
  // numbers, not of unrelated geometry and mathematical constants.
  if(COMPARE(numeratorJ,numeratorE)<=0)return r;
  checkJ=(checkJ*jm)%MOD;
  checkE=(checkE*em)%MOD;
  checkD=(checkD*dm)%MOD;
  if(RESIDUE(numeratorJ)!=(checkJ*static_cast<u64>(n))%MOD ||
     RESIDUE(numeratorE)!=(checkE*static_cast<u64>(n))%MOD ||
     RESIDUE(den)!=checkD)return r;
  // A real single uint64 cannot hold floor(F(n)) once
  // numerator > UINT64_MAX*denominator.
  if(r.jig_u64_first_overflow==0u&&COMPARE(numeratorJ,bound)>0)
   r.jig_u64_first_overflow=n;
  if(r.conventional_u64_first_overflow==0u&&
     COMPARE(numeratorE,bound)>0)
   r.conventional_u64_first_overflow=n;
  r.finite_inverse_exact=r.finite_inverse_exact&&
   RECIPROCAL_IS_EXACT({numeratorJ,den})&&
   RECIPROCAL_IS_EXACT({numeratorE,den});
  if(!r.finite_inverse_exact)return r;
  r.steps=n;
  r.jig_residue=RESIDUE(numeratorJ);
  r.conventional_residue=RESIDUE(numeratorE);
  r.denominator_residue=RESIDUE(den);
  r.jig_numerator_digits=DIGITS(numeratorJ);
  r.conventional_numerator_digits=DIGITS(numeratorE);
 }
 if(steps==MAX_STEPS) {
  r.exceeding_single_u64_is_expected=
   r.jig_u64_first_overflow==13u&&
   r.conventional_u64_first_overflow==41u;
  r.valid=r.exceeding_single_u64_is_expected&&
   r.jig_residue==64122618ULL&&
   r.conventional_residue==981146538ULL&&
   r.denominator_residue==692745742ULL&&
   r.jig_numerator_digits==1248u&&
   r.conventional_numerator_digits==1182u&&
   r.finite_inverse_exact;
 } else {
  r.valid=r.steps==steps&&r.finite_inverse_exact;
 }
 return r;
}
// Keep constexpr evaluation bounded. Native /O2 qualification runs 64
// steps and PowerShell's independent BigInteger oracle cross-checks it.
static_assert(RUN(2u).valid);
static constexpr bool INFINITY_RESOLVED=false;
static constexpr bool PHYSICAL_CHEATING_PROVEN=false;
static constexpr bool RELEASE_AUTHORIZED=false;
static_assert(!INFINITY_RESOLVED&&!PHYSICAL_CHEATING_PROVEN&&
              !RELEASE_AUTHORIZED);
} // namespace swe::diaery::finite_head_to_head
#endif
