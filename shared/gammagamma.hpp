// SPDX-License-Identifier: 0BSD
// Copyright (c) 2026 Paul Allen Cheek
// GammaGamma: source-preserving decimal COMPONENT factorial difference.
// Exploratory mathematics, not a radiation or quantum-physics model.
#ifndef SWE_GAMMAGAMMA_HPP
#define SWE_GAMMAGAMMA_HPP

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

class BigUnsigned {
    std::vector<std::uint32_t> limbs_{1u};  // little endian, base 1e9
public:
    [[nodiscard]] static BigUnsigned factorial(unsigned n) {
        BigUnsigned result;
        for(unsigned k=2;k<=n;++k) {
            std::uint64_t carry=0;
            for(auto& limb:result.limbs_) {
                const std::uint64_t p=std::uint64_t(limb)*k+carry;
                limb=std::uint32_t(p%decimal_base);
                carry=p/decimal_base;
            }
            while(carry!=0u) {
                result.limbs_.push_back(std::uint32_t(carry%decimal_base));
                carry/=decimal_base;
            }
        }
        return result;
    }
    [[nodiscard]] int compare(const BigUnsigned& other) const noexcept {
        if(limbs_.size()!=other.limbs_.size())
            return limbs_.size()<other.limbs_.size()?-1:1;
        for(std::size_t i=limbs_.size();i>0;--i)
            if(limbs_[i-1]!=other.limbs_[i-1])
                return limbs_[i-1]<other.limbs_[i-1]?-1:1;
        return 0;
    }
    [[nodiscard]] BigUnsigned minus_smaller(const BigUnsigned& rhs) const {
        BigUnsigned result=*this;
        std::uint64_t borrow=0;
        for(std::size_t i=0;i<result.limbs_.size();++i){
            const std::uint64_t sub=borrow+
              (i<rhs.limbs_.size()?rhs.limbs_[i]:0u);
            const std::uint64_t val=result.limbs_[i];
            if(val<sub) {
                result.limbs_[i]=std::uint32_t(val+decimal_base-sub);
                borrow=1;
            } else {
                result.limbs_[i]=std::uint32_t(val-sub);
                borrow=0;
            }
        }
        while(result.limbs_.size()>1u&&result.limbs_.back()==0u)
            result.limbs_.pop_back();
        return result;
    }
    [[nodiscard]] std::string str() const {
        std::string s=std::to_string(limbs_.back());
        for(std::size_t i=limbs_.size()-1;i>0;--i) {
            const std::string part=std::to_string(limbs_[i-1]);
            s.append(9u-part.size(),'0');
            s+=part;
        }
        return s;
    }
};

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
    const auto lf=BigUnsigned::factorial(l);
    const auto rf=BigUnsigned::factorial(r);
    const int comparison=lf.compare(rf);
    result.left_factorial=lf.str();
    result.right_factorial=rf.str();
    result.direction=comparison<0?Direction::negative:
                     comparison>0?Direction::positive:
                     Direction::neutral;
    result.magnitude=(comparison>=0?lf.minus_smaller(rf):
                                      rf.minus_smaller(lf)).str();
    result.status=Status::exact;
    return result;
}

struct DecimalToken { std::string_view text; };
[[nodiscard]] inline Result operator!(DecimalToken token) {
    return evaluate(token.text);
}

// Keep the author's ℼ integer digits fixed; caller chooses the decimal point.
// This cannot silently turn ℼ into ordinary irrational pi or guess a split.
inline constexpr std::string_view pi_digits="314159265358979323";
inline constexpr std::uint64_t pi_integer=314159265358979323ULL;
inline constexpr std::string_view bipi_square=
    "98696044010893585656621344341538329";
inline constexpr std::string_view pipi_symbolic="ℼ^ℼ";
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
