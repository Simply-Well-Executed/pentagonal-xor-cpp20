// SPDX-License-Identifier: 0BSD
// THAILONGA "MONKEY TAIL": project-local EXPLORATORY nickname.
// Established mathematics: complex conjugation, modulus, right triangle,
// and integer rational round-to-nearest ties-to-even. Not a Thai provenance claim.
#ifndef SWE_DIAERY_MONKEY_TAIL_HPP
#define SWE_DIAERY_MONKEY_TAIL_HPP
namespace swe::diaery::monkey_tail {
using U64=unsigned long long;
using I64=long long;
enum class State { Verified, InvalidDomain, Overflow, ZeroDenominator };
struct ComplexTriangle {
 State state;
 I64 real,imaginary,conjugate_real,conjugate_imaginary;
 I64 product_real,product_imaginary;
 I64 squared_hypotenuse;
};
// Signed range bound proves all products and sums fit I64.
[[nodiscard]] constexpr ComplexTriangle conjugate_triangle(I64 a,I64 b) noexcept {
 if(a>1000000000LL||a< -1000000000LL||b>1000000000LL||b< -1000000000LL)
     return {State::InvalidDomain,a,b,0,0,0,0,0};
 const I64 real=a*a+b*b; // (a+bi)(a-bi), i*i=-1.
 const I64 imaginary=b*a-a*b; // EXACT cancellation: +ab-ab=0.
 return {State::Verified,a,b,a,-b,real,imaginary,real};
}
struct Rounded {
 State state;
 U64 value, quotient, remainder, denominator;
 bool incremented,exact,tie;
};
// No remainder*2 overflow; ties-to-even is exact for U64 rational.
// This does not truncate source: quotient/remainder are retained for audit.
[[nodiscard]] constexpr Rounded round_nearest_even(U64 numerator,U64 denominator) noexcept {
 if(!denominator)return {State::ZeroDenominator,0ULL,0ULL,0ULL,0ULL,false,false,false};
 const U64 quotient=numerator/denominator;
 const U64 remainder=numerator%denominator;
 const U64 half=denominator/2ULL;
 const bool tie=(denominator%2ULL==0ULL && remainder==half);
 const bool greater=remainder>half || (denominator%2ULL==1ULL&&remainder>half);
 const bool increment=greater||(tie&&(quotient&1ULL));
 if(increment&&quotient==~0ULL)
     return {State::Overflow,0ULL,quotient,remainder,denominator,false,false,tie};
 return {State::Verified,quotient+(increment?1ULL:0ULL),quotient,
         remainder,denominator,increment,remainder==0ULL,tie};
}
struct Gain {
 State state;
 I64 signed_numerator;
 U64 denominator;
 bool shrinking,alternating_sign;
};
[[nodiscard]] constexpr Gain negative_gain_to_zero(I64 numerator,U64 denominator) noexcept {
 if(!denominator)return {State::ZeroDenominator,numerator,denominator,false,false};
 // Avoid absolute value overflow at signed minimum; compare magnitudes as U64.
 const U64 magnitude=numerator<0?
    static_cast<U64>(-(numerator+1LL))+1ULL:static_cast<U64>(numerator);
 // A negative g with |g|<1 alternates sign while shrinking; an i rotation
 // has |i|=1 and does not shrink by itself.
 return {State::Verified,numerator,denominator,
         magnitude<denominator,numerator<0};
}
} // namespace swe::diaery::monkey_tail
#endif
