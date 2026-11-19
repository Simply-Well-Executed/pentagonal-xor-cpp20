// SPDX-License-Identifier: 0BSD
// Copyright (c) 2026 Paul Allen Cheek
// Optional nonnegative binary operations for toolong, with explicit bit width.
// Negative infinite-width two's-complement bitwise semantics are intentionally unsupported.
#ifndef SWE_TOOLONG_BITS_HPP
#define SWE_TOOLONG_BITS_HPP
#include "toolong.hpp"
#include <cstddef>
#include <stdexcept>
namespace swe::toolong_bits {
inline void require_nonnegative(const toolong& value) {
    if(value.negative())throw std::domain_error("toolong_bits: negative operand");
}
[[nodiscard]] inline toolong bitwise_xor(toolong a,toolong b){
    require_nonnegative(a);require_nonnegative(b);
    toolong result{0},bit{1},two{2};
    while(!a.is_zero()||!b.is_zero()){
        auto ra=toolong::divmod(a,two), rb=toolong::divmod(b,two);
        if(ra.second!=rb.second)result+=bit;
        a=std::move(ra.first);b=std::move(rb.first);bit*=two;
    }
    return result;
}
[[nodiscard]] inline toolong bitwise_and(toolong a,toolong b){
    require_nonnegative(a);require_nonnegative(b);
    toolong result{0},bit{1},two{2};
    while(!a.is_zero()&&!b.is_zero()){
        auto ra=toolong::divmod(a,two), rb=toolong::divmod(b,two);
        if(!ra.second.is_zero()&&!rb.second.is_zero())result+=bit;
        a=std::move(ra.first);b=std::move(rb.first);bit*=two;
    }
    return result;
}
[[nodiscard]] inline toolong bitwise_or(toolong a,toolong b){
    require_nonnegative(a);require_nonnegative(b);
    return a+b-bitwise_and(a,b);
}
[[nodiscard]] inline toolong shift_left(const toolong& value,std::uint64_t by){
    require_nonnegative(value);return value*toolong::power(toolong{2},by);
}
[[nodiscard]] inline toolong shift_right(const toolong& value,std::uint64_t by){
    require_nonnegative(value);return value/toolong::power(toolong{2},by);
}
[[nodiscard]] inline toolong bitwise_not_width(const toolong& value,unsigned bits){
    require_nonnegative(value);
    const toolong modulus=toolong::power(toolong{2},bits);
    if(value>=modulus)throw std::domain_error("toolong_bits: operand exceeds declared width");
    return modulus-toolong{1}-value;
}
[[nodiscard]] inline toolong rotate_left_width(const toolong& value,unsigned bits,unsigned shift){
    require_nonnegative(value);
    if(bits==0)throw std::invalid_argument("toolong_bits: width zero");
    const toolong modulus=toolong::power(toolong{2},bits);
    if(value>=modulus)throw std::domain_error("toolong_bits: operand exceeds declared width");
    shift%=bits;
    if(shift==0)return value;
    return (shift_left(value,shift)%modulus)+
           shift_right(value,bits-shift);
}
}
#endif
