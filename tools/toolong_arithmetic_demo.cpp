// SPDX-License-Identifier: 0BSD
// Copyright (c) 2026 Paul Allen Cheek
#include "../shared/toolong.hpp"
#include "../shared/toolong_ratio.hpp"
#include "../shared/toolong_photon_model.hpp"
#include "../shared/toolong_bits.hpp"
#include "../shared/pi_precision.hpp"
#include <iomanip>
#include <iostream>

int main(){
    using swe::toolong;
    std::cout << std::setprecision(swe::pi_precision::display_digits);
    const toolong pi{"314159265358979323"};
    const toolong a{"123456789012345678901234567890"};
    const toolong b{"98765432109876543210"};
    const auto [q,r]=toolong::divmod(a,b);

    std::cout<<"toolong exact C++20 integer arithmetic:\n"
             <<"a + b = "<<(a+b)<<'\n'
             <<"a - b = "<<(a-b)<<'\n'
             <<"a * b = "<<(a*b)<<'\n'
             <<"a / b = "<<q<<" [truncated integer quotient]\n"
             <<"a % b = "<<r<<" [remainder]\n"
             <<"a == b*q+r = "<<(q*b+r==a?"PASS":"FAIL")<<'\n'
             <<"-5 % 4 (C++ remainder) = "<<(toolong{-5}%toolong{4})<<'\n'
             <<"-5 mod 4 (Euclidean) = "<<toolong::euclidean_mod(toolong{-5},toolong{4})<<'\n'
             <<"gcd(84,30) = "<<toolong::gcd(toolong{84},toolong{30})<<'\n'
             <<"BiPi exact pi-integer^2 = "<<(pi*pi)<<'\n'
             <<"2^100 = "<<toolong::power(toolong{2},100)<<'\n'
             <<"100! = "<<toolong::factorial(100)<<'\n'
             <<"bit XOR(10,12) = "<<swe::toolong_bits::bitwise_xor(toolong{10},toolong{12})<<'\n'
             <<"rotl8(0x81,1) = "<<swe::toolong_bits::rotate_left_width(toolong{129},8,1)<<'\n'
             <<"exact 1/3+2/3 = "<<(swe::toolong_ratio{toolong{1},toolong{3}}+
                                    swe::toolong_ratio{toolong{2},toolong{3}})<<'\n';
    const auto photon=swe::inspect_photon_counts(toolong{100},
        {toolong{105},toolong{120},toolong{135}});
    std::cout<<"synthetic photon observations [105,120,135], incident 100 each:\n"
             <<"gain (counts ratio) = "<<photon.gain<<'\n'
             <<"mean detected = "<<photon.mean<<'\n'
             <<"dispersion (population variance) = "<<photon.dispersion<<'\n'
             <<"Fano = "<<photon.fano<<'\n'
             <<"Exact counting statistics are not a quantum-mechanics validation.\n";
    return 0;
}
