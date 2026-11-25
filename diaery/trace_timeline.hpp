// SPDX-License-Identifier: 0BSD
// DYNAMO exact timeline + bounded trace grammar, no regex engine or remote code.
// RELEASE=HOLD: source and time remain immutable until explicit key events.
#ifndef SWE_DIAERY_TRACE_TIMELINE_HPP
#define SWE_DIAERY_TRACE_TIMELINE_HPP
namespace swe::diaery::trace {
using U64=unsigned long long;
constexpr U64 FS_PER_SECOND=1000000000000000ULL;
enum class Status { Valid, NullInput, InvalidPattern, Overflow };
struct Time { U64 seconds; U64 remainder_femtoseconds; };
struct TimeResult { Status status; Time time; };
[[nodiscard]] constexpr TimeResult split_femtoseconds(U64 fs) noexcept {
    return {Status::Valid,{fs/FS_PER_SECOND,fs%FS_PER_SECOND}};
}
// EXPLORATORY DARN -> YARN display policy; never changes the exact clock.
// DARN is the exact seconds + residual-femtosecond representation.
// One second-DARN is split into 16 equal S P L O O N sectors by integer
// quotient (no binary-floating quantization of the canonical time).
// Offset +0.1 and >0.6 threshold become exact integer inequalities:
// (sector/16 + 1/10) > 3/5  iff 10*sector + 16 > 96.
// A crossing of 1.0 adds ONE more bonus second beyond the threshold carry.
// This is a user-defined display algorithm, NOT standard time rounding.
constexpr unsigned SPLOONS_PER_DARN=16u;
// "IMPEREAL STANDARD MEASURES": user-defined label for the 16
// SPLOON positions. Not an official Imperial measurement system.
// Standard imperial/metric length conversion is separately EXACT:
// 1 inch = 25.4 mm = 25400 micrometres by definition.
// Keep values as integers/rationals instead of rounding into doubles.
struct ExactLength { U64 numerator; U64 denominator; };
[[nodiscard]] constexpr ExactLength inches_to_micrometres(U64 inches) noexcept {
    if(inches>~0ULL/25400ULL)return {0ULL,0ULL}; // HOLD
    return {inches*25400ULL,1ULL};
}
[[nodiscard]] constexpr ExactLength micrometres_to_inches(U64 um) noexcept {
    return {um,25400ULL}; // exact fraction, never rounded
}
// EXPLORATORY n-1663r where r=pi^2. This formula is NOT an
// imperial-to-metric conversion and has no specified physical units.
// Conventional pi is irrational; a finite double is an approximation.
// Do NOT use this transform to change original fs or length authority.
struct ImperealTransform { Status status; double derived; };
[[nodiscard]] constexpr ImperealTransform n_minus_1663_pi_squared(unsigned n) noexcept {
    if(n>=16u)return {Status::NullInput,0.0};
    constexpr double pi_approx=3.14159265358979323846;
    const double r=pi_approx*pi_approx;
    return {Status::Valid,static_cast<double>(n)-1663.0*r};
}

struct Yarn {
    Status status;
    U64 exact_seconds;             // DARN authority
    U64 remainder_femtoseconds;    // exact and never rounded
    unsigned sploon;               // 0..15, derived
    unsigned threshold_carry;      // 0 or 1
    unsigned angular_bonus;        // 0 or 1
    U64 shown_yarn_seconds;        // derived, no source mutation
};
[[nodiscard]] constexpr Yarn make_yarn(U64 fs) noexcept {
    const auto exact=split_femtoseconds(fs);
    const U64 whole=exact.time.seconds;
    const U64 remainder=exact.time.remainder_femtoseconds;
    // Safe since remainder < 1e15, and 16*remainder < 2^64.
    const unsigned sector=static_cast<unsigned>((remainder*16ULL)/FS_PER_SECOND);
    const unsigned adjusted_tenths_numerator=10u*sector+16u;
    const unsigned carry=adjusted_tenths_numerator>96u?1u:0u;
    const unsigned bonus=adjusted_tenths_numerator>=160u?1u:0u;
    const U64 increment=static_cast<U64>(carry+bonus);
    if(whole>~0ULL-increment)
        return {Status::Overflow,whole,remainder,sector,carry,bonus,whole};
    return {Status::Valid,whole,remainder,sector,carry,bonus,whole+increment};
}
[[nodiscard]] constexpr bool add_safe(U64 a,U64 b) noexcept { return a<=~0ULL-b; }
struct Advance { Status status; U64 fs; };
[[nodiscard]] constexpr Advance advance(U64 fs,U64 delta) noexcept {
    return add_safe(fs,delta)?Advance{Status::Valid,fs+delta}:
                              Advance{Status::Overflow,fs};
}
// Static, bounded subline format; not an arbitrary regex or expression evaluator.
// Alternative patterns are case-sensitive and full-string matched.
enum class Pattern { YEqualsMXPlusB, YEqualsX, YEqualsZero, Invalid };
[[nodiscard]] constexpr bool equals(const char* s,const char* lit) noexcept {
    if(!s||!lit)return false;
    unsigned i=0u;
    for(;i<32u&&s[i]&&lit[i];++i)if(s[i]!=lit[i])return false;
    return i<32u&&s[i]==0&&lit[i]==0;
}
[[nodiscard]] constexpr Pattern parse(const char* s) noexcept {
    if(!s)return Pattern::Invalid;
    if(equals(s,"Y=MX+B"))return Pattern::YEqualsMXPlusB;
    if(equals(s,"Y=X"))return Pattern::YEqualsX;
    if(equals(s,"Y=0"))return Pattern::YEqualsZero;
    return Pattern::Invalid;
}
struct Value { Status status; double y; };
[[nodiscard]] constexpr Value calculate(Pattern p,double x,double m,double b) noexcept {
    if(!(x==x&&m==m&&b==b)||x>1000000000.||x< -1000000000.||
       m>1000000000.||m< -1000000000.||
       b>1000000000.||b< -1000000000.)return {Status::NullInput,0.};
    double y=0.;
    if(p==Pattern::YEqualsMXPlusB)y=m*x+b;
    else if(p==Pattern::YEqualsX)y=x;
    else if(p!=Pattern::YEqualsZero)return {Status::InvalidPattern,0.};
    if(!(y==y)||y>1000000000.||y< -1000000000.)
        return {Status::Overflow,0.};
    return {Status::Valid,y};
}
// Axis grid positions use exact rational indices, never accumulating 1/3
// or 1/4 as floating-point coordinate mutations.
[[nodiscard]] constexpr unsigned x_subunit_count(unsigned majors) noexcept {
    return majors*4u;
}
[[nodiscard]] constexpr unsigned y_subunit_count(unsigned majors) noexcept {
    return majors*3u;
}
[[nodiscard]] constexpr bool x_is_major(unsigned quarters) noexcept {
    return quarters%4u==0u;
}
[[nodiscard]] constexpr bool y_is_major(unsigned thirds) noexcept {
    return thirds%3u==0u;
}
} // namespace swe::diaery::trace
#endif
