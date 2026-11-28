/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
// Copyright (c) 2026 Paul Allen Cheek
#include "../shared/toolong.hpp"
#include "../shared/toolong_ratio.hpp"
#include "../shared/toolong_photon_model.hpp"
#include "../shared/toolong_bits.hpp"
#include "../shared/toolong_q62.hpp"
#include <cstdint>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>
using swe::toolong;
static void check(bool ok, const char* why) { if(!ok)throw std::runtime_error(why); }
int main() {
try {
    static_assert(swe::toolong_q62::sigma_sigma_sigma());
    static_assert(swe::toolong_q62::euler.expected_integer % 100ULL == 23ULL);
    static_assert(swe::toolong_q62::pi.expected_integer % 100ULL == 23ULL);
    static_assert(swe::toolong_q62::intensity.expected_integer % 100ULL == 23ULL);
    for(const auto r : {swe::toolong_q62::euler,
                        swe::toolong_q62::pi,
                        swe::toolong_q62::intensity}) {
        const auto audit=swe::toolong_q62::recompute(r);
        check(swe::toolong_q62::verify(r),"TRIFORCE exact source witness");
        check(audit.exact_product ==
              audit.exact_quotient*audit.divisor+audit.exact_remainder,
              "TRIFORCE quotient and remainder reconstruction");
    }
    check(swe::toolong_q62::verify_triforce(), "TRIFORCE all three records");
    auto changed=swe::toolong_q62::intensity;
    ++changed.expected_integer;
    check(!swe::toolong_q62::verify(changed),
          "TRIFORCE corrupted output must fail");
    using swe::toolong_q62::ApprovalClaim;
    constexpr const char* head="0123456789abcdef0123456789abcdef01234567";
    constexpr ApprovalClaim first{"test-reviewer-a",head,true};
    constexpr ApprovalClaim second{"test-reviewer-b",head,true};
    constexpr ApprovalClaim third{"test-reviewer-c",head,true};
    static_assert(swe::toolong_q62::three_distinct_claims(first,second,third,head));
    static_assert(!swe::toolong_q62::three_distinct_claims(first,first,third,head));
    static_assert(!swe::toolong_q62::three_distinct_claims(first,second,
                  ApprovalClaim{"test-reviewer-c",head,false},head));
    static_assert(!swe::toolong_q62::externally_authenticated_approval);
    const toolong zero, x{"314159265358979323"}, y{"123456789012345678901234567890"};
    check(zero.str()=="0" && toolong{"-000"}==zero,"canonical zero");
    check(toolong{"+000123456789000"}.str()=="123456789000","leading zero parsing");
    check(toolong{std::numeric_limits<std::int64_t>::min()}.str()=="-9223372036854775808","int64 minimum");
    check(toolong::from_unsigned(std::numeric_limits<std::uint64_t>::max()).str()=="18446744073709551615","uint64 maximum");
    for(const std::string s: {"","+","-","3.14","-a","7E8"}) {
        bool threw=false;
        try {[[maybe_unused]]toolong bad{s};}
        catch(const std::invalid_argument&){threw=true;}
        check(threw,"invalid decimal rejected");
    }
    check((x+x).str()=="628318530717958646","addition");
    check((x-x-toolong{1}).str()=="-1","subtraction");
    check((toolong{-5}+toolong{10})==toolong{5},"signed addition");
    check((toolong{-5}-toolong{10})==toolong{-15},"signed subtraction");
    check((x*x).str()=="98696044010893585656621344341538329","BiPi exact square");
    check((y/toolong{10}).str()=="12345678901234567890123456789","division");
    check((y%toolong{10})==zero,"remainder");
    check((toolong{-7}*toolong{-8})==toolong{56},"signed multiplication");
    check((toolong{-5}/toolong{4})==toolong{-1},"truncating quotient");
    check((toolong{-5}%toolong{4})==toolong{-1},"C++ remainder");
    check(toolong::euclidean_mod(toolong{-5},toolong{4})==toolong{3},"Euclidean modulo");
    check(toolong::euclidean_mod(toolong{-5},toolong{-4})==toolong{3},"negative divisor");
    check(toolong::gcd(toolong{-84},toolong{30})==toolong{6},"gcd");
    check(toolong::power(toolong{2},100).str()=="1267650600228229401496703205376","power exact");
    check(toolong::power(toolong{-2},7)==toolong{-128},"odd signed power");
    check(toolong::power(toolong{-2},8)==toolong{256},"even signed power");
    check(toolong::power(zero,0)==toolong{1},"declared 0^0 convention");
    check(toolong::factorial(0)==toolong{1},"0 factorial");
    check(toolong::factorial(5)==toolong{120},"5 factorial");
    check(toolong::factorial(100).decimal_digits()==158u,"100 factorial digit count");
    bool division_rejected=false;
    try {[[maybe_unused]]auto fail=x/zero;}
    catch(const std::domain_error&){division_rejected=true;}
    check(division_rejected,"zero divisor rejected");
    bool power_limited=false;
    try {[[maybe_unused]]auto fail=toolong::power(toolong{9},2,1);}
    catch(const std::length_error&){power_limited=true;}
    check(power_limited,"power optional digit budget");
    for(int a=-117;a<=117;++a)for(int b=-27;b<=27;++b){
        if(b==0)continue;
        auto [q,r]=toolong::divmod(toolong{a},toolong{b});
        check(q.str()==std::to_string(a/b) && r.str()==std::to_string(a%b),"signed native division oracle");
        check(q*toolong{b}+r==toolong{a},"quotient identity");
        const auto er=toolong::euclidean_mod(toolong{a},toolong{b});
        check(!er.negative() && er<toolong{b}.abs(),"Euclidean invariant");
    }
    std::uint64_t seed=0x0123456789abcdefULL;
    for(int n=0;n<150;n++){
        std::string a,b;
        for(int i=0;i<120+n%14;i++){seed=seed*6364136223846793005ULL+1;a+=char('0'+(seed>>59)%10);}
        for(int i=0;i<14+n%19;i++){seed=seed*6364136223846793005ULL+1;b+=char('0'+(seed>>59)%10);}
        toolong A{a},B{b};
        check(!B.is_zero(),"nonzero pseudorandom divisor");
        auto [q,r]=toolong::divmod(A,B);
        check(q*B+r==A && r>=zero && r<B,"large divmod identity");
    }
    check(swe::toolong_bits::bitwise_xor(toolong{10},toolong{12})==toolong{6},"xor");
    check(swe::toolong_bits::bitwise_and(toolong{10},toolong{12})==toolong{8},"and");
    check(swe::toolong_bits::bitwise_or(toolong{10},toolong{12})==toolong{14},"or");
    check(swe::toolong_bits::bitwise_not_width(toolong{10},4)==toolong{5},"not 4-bit");
    check(swe::toolong_bits::shift_left(toolong{1},100)==toolong::power(toolong{2},100),"left shift");
    check(swe::toolong_bits::shift_right(toolong{128},3)==toolong{16},"right shift");
    check(swe::toolong_bits::rotate_left_width(toolong{129},8,1)==toolong{3},"rotate left");
    const auto fraction=swe::toolong_ratio{toolong{4},toolong{-6}};
    check(fraction.str()=="-2/3","canonical signed fraction");
    const swe::toolong_ratio one_third{toolong{1},toolong{3}};
    check((one_third+one_third).str()=="2/3","ratio sum");
    check((one_third*one_third).str()=="1/9","ratio product");
    check((one_third/one_third).str()=="1","ratio division");
    check((one_third-one_third).str()=="0","ratio difference");
    const auto counts=swe::inspect_photon_counts(toolong{100},
        {toolong{105},toolong{120},toolong{135}});
    check(counts.total_detected==toolong{360},"count total");
    check(counts.gain.str()=="6/5","gain");
    check(counts.mean.str()=="120","mean");
    check(counts.dispersion.str()=="150","dispersion");
    check(counts.fano_defined && counts.fano.str()=="5/4","Fano");
    const auto dark=swe::inspect_photon_counts(toolong{100},{zero,zero});
    check(!dark.fano_defined && dark.gain.str()=="0","zero mean");
    std::cout<<"PASS toolong exact integer, Euclidean, rationals, binary and trial-count statistics\n";
} catch(const std::exception& e){std::cerr<<"FAIL: "<<e.what()<<'\n';return 1;}
return 0;
}
