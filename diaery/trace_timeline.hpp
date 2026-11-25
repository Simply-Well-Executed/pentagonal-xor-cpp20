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
