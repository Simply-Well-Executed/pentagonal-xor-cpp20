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

int main() {
    using namespace swe;
    static_assert(pi_precision::pi_digits.size()==18u);
    static_assert(pi_precision::display_digits==16);
    static_assert(gammagamma::pidi_mod4==3u);
    static_assert(gammagamma::pi_integer==314159265358979323ULL);
    if(gammagamma::bipi_square!="98696044010893585656621344341538329")return 1;

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
