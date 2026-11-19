// SPDX-License-Identifier: 0BSD
// Copyright (c) 2026 Paul Allen Cheek
// Tests must be compiled with C++20 -O2; keep exact values separate from display.
#include "../shared/gammagamma.hpp"
#include "../shared/pi_precision.hpp"
#include <cmath>
#include <cstdint>
#include <iomanip>
#include <limits>
#include <sstream>
#include <string>
#include <string_view>

int main() {
    using namespace swe;
    static_assert(pi_precision::pi_digits.size()==18u);
    static_assert(pi_precision::display_digits==16);
    static_assert(gammagamma::pidi_mod4==3u);
    static_assert(gammagamma::pi_integer==314159265358979323ULL);
    if(gammagamma::bipi_square!="98696044010893585656621344341538329")return 1;
    if(gammagamma::bipi_exact().str()!=gammagamma::bipi_square)return 13;

    const auto a=gammagamma::evaluate("3.14");
    if(!a.exact() || a.magnitude!="87178291194" ||
       a.direction!=gammagamma::Direction::negative ||
       a.left_factorial!="6" || a.right_factorial!="87178291200")return 2;
    const auto b=!gammagamma::DecimalToken{"14.3"};
    if(!b.exact() || b.direction!=gammagamma::Direction::positive ||
       b.magnitude!=a.magnitude)return 3;
    const auto equal=gammagamma::evaluate("0.1");
    if(!equal.exact() || equal.direction!=gammagamma::Direction::neutral ||
       equal.magnitude!="0")return 4;
    const auto original=gammagamma::evaluate("3.014");
    if(!original.exact() || original.source!="3.014" ||
       original.right_integer!="014" || original.magnitude!=a.magnitude)return 5;
    const auto symbolic=gammagamma::sigma_pi("3.14159265358979323");
    if(symbolic.status!=gammagamma::Status::symbolic_only ||
       symbolic.direction!=gammagamma::Direction::negative)return 6;
    if(gammagamma::sigma_pi("3.14").status !=
       gammagamma::Status::invalid_pi_representation)return 7;
    if(gammagamma::evaluate("3..14").status!=gammagamma::Status::invalid_decimal)return 8;

    // Copilot review: exact 2048!, symbolic 2049!, both sides and
    // decimal token provenance must not be silently normalized away.
    using gammagamma::Status;
    using gammagamma::Direction;
    const auto max_left = gammagamma::evaluate("2048.1");
    const auto max_right = gammagamma::evaluate("1.2048");
    const auto max_leading_left = gammagamma::evaluate("0002048.0001");
    const auto max_leading_right = gammagamma::evaluate("0001.0002048");
    if(max_left.status!=Status::exact || max_right.status!=Status::exact ||
       max_leading_left.status!=Status::exact ||
       max_leading_right.status!=Status::exact ||
       max_left.direction!=Direction::positive ||
       max_right.direction!=Direction::negative ||
       max_leading_left.direction!=Direction::positive ||
       max_leading_right.direction!=Direction::negative ||
       max_left.left_integer!="2048" ||
       max_leading_left.left_integer!="0002048" ||
       max_leading_left.right_integer!="0001" ||
       max_leading_right.right_integer!="0002048" ||
       max_left.right_factorial!="1" ||
       max_right.left_factorial!="1" ||
       max_left.left_factorial!=max_right.right_factorial ||
       max_left.magnitude!=max_right.magnitude ||
       max_left.magnitude!=max_leading_left.magnitude ||
       max_left.magnitude!=max_leading_right.magnitude ||
       max_left.magnitude.empty() || max_left.magnitude.front()=='-') return 14;

    const auto zero_boundary = gammagamma::evaluate("2048.2048");
    if(zero_boundary.status!=Status::exact ||
       zero_boundary.direction!=Direction::neutral ||
       zero_boundary.magnitude!="0")return 15;

    const auto check_symbolic = [](std::string_view token, Direction expected) {
        const auto result=gammagamma::evaluate(token);
        return result.status==Status::symbolic_only &&
               result.direction==expected &&
               result.source==token && result.magnitude.empty() &&
               !result.left_integer.empty() && !result.right_integer.empty();
    };
    if(!check_symbolic("2049.1",Direction::positive) ||
       !check_symbolic("1.2049",Direction::negative) ||
       !check_symbolic("0002049.0001",Direction::positive) ||
       !check_symbolic("0001.0002049",Direction::negative) ||
       !check_symbolic("2048.2049",Direction::negative) ||
       !check_symbolic("2049.2048",Direction::positive) ||
       !check_symbolic("2049.2049",Direction::neutral))return 16;

    // Signed fragments and missing sides are rejected. This must not be
    // confused with a symbolic result for a syntactically valid huge input.
    for(const std::string_view token : {
           std::string_view{".5"},std::string_view{"5."},
           std::string_view{"+5.1"},std::string_view{"-5.1"},
           std::string_view{"5.+1"},std::string_view{"5.-1"},
           std::string_view{"5..1"},std::string_view{"5.1.2"},
           std::string_view{" 5.1"},std::string_view{"5.1 "},
           std::string_view{"5,1"},std::string_view{""}}) {
        const auto result=gammagamma::evaluate(token);
        if(result.status!=Status::invalid_decimal ||
           result.direction!=Direction::neutral ||
           result.source!=token || !result.magnitude.empty())return 17;
    }

    std::ostringstream out;
    out<<std::setprecision(pi_precision::display_digits);
    if(out.precision()!=16)return 9;
    out<<std::sqrt(2.0);
    if(out.str().find('.')==std::string::npos)return 10;

    std::ostringstream fixed;
    fixed<<std::fixed<<std::setprecision(pi_precision::display_digits)<<3.14;
    const auto formatted=fixed.str();
    const auto dot=formatted.find('.');
    if(dot==std::string::npos || formatted.size()-dot-1u!=16u)return 11;

    // We do not silently equate display precision with binary64 exactness.
    if(pi_precision::display_digits > std::numeric_limits<double>::max_digits10)return 12;
    return 0;
}
