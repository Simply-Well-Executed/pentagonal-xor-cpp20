// SPDX-License-Identifier: 0BSD
// DIAERY-FUSION-QUALIFICATION-PR34-015: distinct, fail-closed named gates.
// Headerless, STL-free, no I/O. Experimental adapters are not new canon.
#ifndef SWE_DIAERY_FUSION_QUALIFICATION_HPP
#define SWE_DIAERY_FUSION_QUALIFICATION_HPP
#ifndef SWE_DIAERY_PARITY_GATE_HPP
#error "Include parity_gate.hpp first"
#endif
namespace swe::diaery::qualification {
namespace str=swe::diaery::strict;
namespace par=swe::diaery::parity;
namespace geo=swe::diaery::geometry;
using u64=unsigned long long;
using s64=long long;

[[nodiscard]] constexpr bool BiDi() noexcept {
 return geo::bidi::REPORT.ping_after_restore &&
  geo::tork::EQUAL(geo::tork::BIDI(geo::tork::BIDI(
    geo::tork::INPUT_PAIR)),geo::tork::INPUT_PAIR);
}
[[nodiscard]] constexpr bool TriDi() noexcept {
 const auto t=geo::tridi::ORIGINAL;
 return geo::tridi::REPORT.valid &&
  geo::tridi::SAME(geo::tridi::TRIDI(geo::tridi::TRIDI(
    geo::tridi::TRIDI(t))),t);
}
[[nodiscard]] constexpr bool PiDi() noexcept {
 return geo::tork::PI_SYMBOL==314159265358979323ULL &&
        geo::tork::PI_DI==3ULL;
}
// PiPi remains symbolic according to existing shared/gammagamma.hpp.
// NOT a numerical evaluation of an irrational exponent.
inline constexpr const char* PIPI_STATUS="SYMBOLIC_ONLY";
[[nodiscard]] constexpr bool PiPi() noexcept {
 return PiDi() && str::same(PIPI_STATUS,"SYMBOLIC_ONLY");
}
// SigmaDi exploratory butterfly: source labels q1=OAT,q2=ATO,
// q3=OTA,q4=TAO. Four prescribed swaps and their reverse.
struct Quad {unsigned q[4];};
[[nodiscard]] constexpr bool SAME(Quad a,Quad b) noexcept {
 for(unsigned i=0;i<4u;++i)if(a.q[i]!=b.q[i])return false;
 return true;
}
constexpr void SWAP(Quad& q,unsigned a,unsigned b) noexcept {
 const unsigned x=q.q[a];q.q[a]=q.q[b];q.q[b]=x;
}
[[nodiscard]] constexpr Quad SigmaDiForward(Quad q) noexcept {
 SWAP(q,0u,2u);SWAP(q,0u,1u);SWAP(q,3u,1u);SWAP(q,3u,2u);
 return q;
}
[[nodiscard]] constexpr Quad SigmaDiReverse(Quad q) noexcept {
 SWAP(q,3u,2u);SWAP(q,3u,1u);SWAP(q,0u,1u);SWAP(q,0u,2u);
 return q;
}
inline constexpr Quad QUAD_SOURCE{{1u,2u,3u,4u}};
[[nodiscard]] constexpr bool SigmaDi() noexcept {
 const auto x=SigmaDiForward(QUAD_SOURCE);
 return !SAME(x,QUAD_SOURCE)&&SAME(SigmaDiReverse(x),QUAD_SOURCE);
}
[[nodiscard]] constexpr bool SigmaSigma() noexcept {
 return geo::tork::SIGMA_SIGMA() &&
  geo::form::PING_THREE_FOLDS().valid && geo::solid::PING().valid;
}
// GammaGamma subset only: tagged bounded decimal COMPONENT factorial
// difference, matching its small exact integer cases. NOT equivalent
// to the full STL-dependent shared/gammagamma.hpp implementation.
[[nodiscard]] constexpr u64 Factorial(unsigned n) noexcept {
 if(n>12u)return 0ULL;
 u64 result=1ULL;
 for(unsigned i=2u;i<=n;++i)result*=i;
 return result;
}
[[nodiscard]] constexpr s64 GammaDelta(unsigned left,unsigned right) noexcept {
 if(left>12u||right>12u)return 0LL;
 return static_cast<s64>(Factorial(left))-
        static_cast<s64>(Factorial(right));
}
[[nodiscard]] constexpr bool GammaGamma() noexcept {
 return Factorial(0u)==1ULL && Factorial(1u)==1ULL &&
  GammaDelta(3u,4u)==-18LL && GammaDelta(5u,3u)==114LL &&
  GammaDelta(0u,1u)==0LL;
}
struct Result {
 bool bidi,tridi,pidi,pipi,sigmadi,sigmasigma,gammagamma;
 bool concurrence,qualified;
};
[[nodiscard]] constexpr Result TEST(const str::Page& p) noexcept {
 const bool a=BiDi(),b=TriDi(),c=PiDi(),d=PiPi(),
            e=SigmaDi(),f=SigmaSigma(),g=GammaGamma(),
            h=par::concurrence(p);
 return {a,b,c,d,e,f,g,h,a&&b&&c&&d&&e&&f&&g&&h};
}
static_assert(BiDi()&&TriDi()&&PiDi()&&PiPi());
static_assert(SigmaDi()&&SigmaSigma()&&GammaGamma());
static_assert(!geo::solid::RELEASE_AUTHORIZED);
inline constexpr bool SAFETY_CERTIFIED=false;
inline constexpr bool RELEASE_AUTHORIZED=false;
static_assert(!SAFETY_CERTIFIED&&!RELEASE_AUTHORIZED);
} // namespace swe::diaery::qualification
#endif
