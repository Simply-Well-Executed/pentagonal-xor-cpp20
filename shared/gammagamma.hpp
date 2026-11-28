/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
// Copyright (c) 2026 Paul Allen Cheek
// GammaGamma: source-preserving decimal COMPONENT factorial difference.
// Exploratory mathematics, not a radiation or quantum-physics model.
#ifndef SWE_GAMMAGAMMA_HPP
#define SWE_GAMMAGAMMA_HPP

#include "toolong.hpp"
#include "toolong_q62_core.hpp"
#include <algorithm>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace swe::gammagamma {

// Prefix ! acts on a tagged DECIMAL STRING, not on a floating-point number.
// Distinct input strings, including trailing/leading zeros, remain distinct.
enum class Status : std::uint8_t {
    exact, symbolic_only, invalid_decimal, invalid_pi_representation
};
enum class Direction : std::int8_t { negative=-1, neutral=0, positive=1 };

struct Result {
    Status status{Status::invalid_decimal};
    Direction direction{Direction::neutral};
    std::string source;
    std::string left_integer;
    std::string right_integer;
    std::string magnitude;             // NONNEGATIVE decimal integer if exact
    std::string left_factorial;
    std::string right_factorial;
    [[nodiscard]] bool exact() const noexcept { return status==Status::exact; }
};

// Cap resource use: a representation such as 3.14159265358979323
// contains an integer component too large to expand as a factorial.
// Rather than fabricate precision, return symbolic_only.
inline constexpr unsigned max_exact_factorial_argument=2048u;
inline constexpr std::uint32_t decimal_base=1'000'000'000u;

[[nodiscard]] inline bool all_decimal_digits(std::string_view value) noexcept {
    if(value.empty())return false;
    for(char c:value)if(c<'0'||c>'9')return false;
    return true;
}
[[nodiscard]] inline std::string_view without_leading_zeros(std::string_view x) noexcept {
    while(x.size()>1u && x.front()=='0')x.remove_prefix(1u);
    return x;
}
[[nodiscard]] inline int compare_integer_parts(std::string_view a,std::string_view b) noexcept {
    a=without_leading_zeros(a);b=without_leading_zeros(b);
    if(a.size()!=b.size())return a.size()<b.size()?-1:1;
    if(a==b)return 0;
    return a<b?-1:1;
}
// 0! == 1!. This special case is necessary for direction inference.
[[nodiscard]] inline std::string_view factorial_rank(std::string_view v) noexcept {
    v=without_leading_zeros(v);
    return v=="0" ? std::string_view{"1"} : v;
}
[[nodiscard]] inline bool bounded_argument(std::string_view str,unsigned& output) noexcept {
    str=without_leading_zeros(str);
    if(str.size()>4u)return false;
    unsigned n=0u;
    for(char c:str)n=n*10u+unsigned(c-'0');
    if(n>max_exact_factorial_argument)return false;
    output=n;return true;
}
[[nodiscard]] inline Result evaluate(std::string_view source) {
    Result result{};
    result.source=std::string(source);
    const auto dot=source.find('.');
    if(dot==std::string_view::npos||dot==0u||dot+1u>=source.size()||
       source.find('.',dot+1u)!=std::string_view::npos)return result;
    const auto left=source.substr(0,dot),right=source.substr(dot+1u);
    if(!all_decimal_digits(left)||!all_decimal_digits(right))return result;
    result.left_integer=std::string(left);
    result.right_integer=std::string(right);
    const int approximate_direction=compare_integer_parts(
        factorial_rank(left),factorial_rank(right));
    result.direction=approximate_direction<0?Direction::negative:
                     approximate_direction>0?Direction::positive:
                     Direction::neutral;
    unsigned l=0,r=0;
    if(!bounded_argument(left,l)||!bounded_argument(right,r)) {
        result.status=Status::symbolic_only;
        return result;
    }
    const auto lf=toolong::factorial(l);
    const auto rf=toolong::factorial(r);
    const int comparison=lf.compare(rf);
    result.left_factorial=lf.str();
    result.right_factorial=rf.str();
    result.direction=comparison<0?Direction::negative:
                     comparison>0?Direction::positive:
                     Direction::neutral;
    result.magnitude=(comparison>=0?(lf-rf):
                                      (rf-lf)).str();
    result.status=Status::exact;
    return result;
}

struct DecimalToken { std::string_view text; };
[[nodiscard]] inline Result operator!(DecimalToken token) {
    return evaluate(token.text);
}

// Keep the author's π integer digits fixed; caller chooses the decimal point.
// This cannot silently turn π into ordinary irrational pi or guess a split.
inline constexpr std::string_view pi_digits="314159265358979323";
inline constexpr std::uint64_t pi_integer=toolong_q62::pi.expected_integer;
inline constexpr std::string_view bipi_square=
    "98696044010893585656621344341538329";
[[nodiscard]] inline toolong bipi_exact() {
    const toolong pi{pi_digits};
    return pi*pi;
}
// PiPi stays symbolic: roughly 5e18 output decimal digits for this huge exponent.
inline constexpr std::string_view pipi_symbolic="π^π";
inline constexpr std::uint64_t pidi_mod4=pi_integer%4u; // 3; label "I" unresolved

[[nodiscard]] inline Result sigma_pi(std::string_view decimal_token) {
    Result result=evaluate(decimal_token);
    std::string digits;
    digits.reserve(decimal_token.size());
    for(char c:decimal_token)if(c!='.')digits.push_back(c);
    if(digits!=pi_digits)result.status=Status::invalid_pi_representation;
    return result;
}

static_assert(pidi_mod4==3u);
static_assert(pi_integer>0u);
} // namespace swe::gammagamma
#endif // SWE_GAMMAGAMMA_HPP
