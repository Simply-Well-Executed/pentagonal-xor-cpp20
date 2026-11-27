// SPDX-License-Identifier: 0BSD
// Copyright (c) 2026 Paul Allen Cheek
// REDUX source-expression counts and independent historical formulas.
#include "../shared/redux.hpp"
#include "../shared/coreao.hpp"
#include "../shared/sigma_form_geometry.hpp"
#include <bit>
#include <limits>

namespace {
using namespace swe::redux;
namespace co = swe::coreao;
namespace geo = swe::sigma_form;
constexpr auto coreao_redux = reduction(141,24);
constexpr auto sine_redux = reduction(98,43);
static_assert(coreao_redux.numerator==39 && coreao_redux.denominator==47);
static_assert(sine_redux.numerator==55 && sine_redux.denominator==98);
static_assert(4*coreao_redux.numerator > coreao_redux.denominator);
static_assert(4*sine_redux.numerator > sine_redux.denominator);
constexpr bool same(double a,double b) {
    return std::bit_cast<unsigned long long>(a)==
           std::bit_cast<unsigned long long>(b);
}
constexpr double old_residual(double a,double b) {
    const double scale=co::largest(1.0,co::largest(co::magnitude(a),co::magnitude(b)));
    return co::magnitude(a/scale-b/scale);
}
constexpr geo::Scalar old_sine(double radians) {
    if (!geo::finite(radians)||geo::absolute(radians)>64.0) return {0.0,false};
    double x=radians;
    while(x>geo::geometry_pi) x-=2.0*geo::geometry_pi;
    while(x<-geo::geometry_pi) x+=2.0*geo::geometry_pi;
    double term=x,sum=x;
    for(unsigned n=1;n<=14;++n) {
        const double d=2.0*static_cast<double>(n);
        term*=-(x*x)/(d*(d+1.0));
        sum+=term;
    }
    return {sum,geo::finite(sum)};
}
constexpr bool denominators() {
    for(unsigned n=1;n<=14;++n)
        if(geo::sine_denominators[n-1]!=double((2*n)*(2*n+1))) return false;
    return true;
}
static_assert(denominators());
static_assert(same(co::normalized_residual(2.0,2.0),old_residual(2.0,2.0)));
static_assert(same(geo::sine(1.0).value,old_sine(1.0).value));
static_assert(co::inspect(swe::deca_sigma::run(2.0)).scalar==0.0);
}
int main() {
    const double values[]={0.0,-0.0,0.5,-0.5,1.0,-1.0,2.0,-9.0,
        std::numeric_limits<double>::max(),
        -std::numeric_limits<double>::max(),
        std::numeric_limits<double>::denorm_min(),
        -std::numeric_limits<double>::denorm_min()};
    for(double a:values) for(double b:values)
        if(!same(co::normalized_residual(a,b),old_residual(a,b))) return 1;
    for(int i=-640;i<=640;++i) {
        const double x=double(i)/10.0;
        const auto a=geo::sine(x), b=old_sine(x);
        if(a.valid!=b.valid || !same(a.value,b.value)) return 2;
    }
    const double edges[]={-0.0,geo::geometry_pi,-geo::geometry_pi,
        64.0,-64.0,65.0,-65.0,std::numeric_limits<double>::infinity(),
        std::numeric_limits<double>::quiet_NaN()};
    for(double x:edges) {
        const auto a=geo::sine(x), b=old_sine(x);
        if(a.valid!=b.valid || !same(a.value,b.value)) return 3;
    }
    auto damaged=swe::deca_sigma::run(2.0);
    damaged.stages[3]+=0.25;
    if(co::inspect(damaged).passed()) return 4;
    return 0;
}
