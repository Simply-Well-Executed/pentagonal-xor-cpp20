// SPDX-License-Identifier: 0BSD
#include "redux.hpp"
namespace redux_test {
using namespace swe::redux;
constexpr Task tasks[]={{3,0},{2,1},{4,0},{1,0}};
constexpr Task cyclic[]={{1,2},{1,1}};
constexpr Task missing[]={{1,2}};
constexpr Task overflow[]={{limit,0},{1,0}};
constexpr Task zero[]={{0,0}};
constexpr auto s=analyze(tasks,4);
constexpr auto r=reduction(4,1);
constexpr auto t=time_reduction(s);
constexpr auto speed=ideal_speedup(s);
constexpr auto f=factorize(4,3,4);
static_assert(r.status==Status::ok && r.numerator==3 && r.denominator==4);
static_assert(s.status==Status::ok && s.work==10 && s.critical_path==5);
static_assert(s.order[0]==0 && s.order[1]==1 && s.finish[1]==5);
static_assert(t.numerator==1 && t.denominator==2);
static_assert(speed.numerator==2 && speed.denominator==1);
static_assert(xor_reduce(0xA5,0x5A,0xFF)==0xFF);
static_assert(f.status==Status::ok && f.original==16 && f.optimized==7);
static_assert(reduction(0,0).status==Status::zero_baseline);
static_assert(reduction(4,5).negative && reduction(4,5).numerator==1);
static_assert(reduction(4,4).denominator==1);
static_assert(analyze(cyclic,2).status==Status::cycle);
static_assert(analyze(missing,1).status==Status::invalid_dependency);
static_assert(analyze(overflow,2).status==Status::overflow);
static_assert(analyze(nullptr,0).status==Status::invalid_input);
static_assert(analyze(tasks,33).status==Status::invalid_input);
static_assert(time_reduction(analyze(zero,1)).status==Status::zero_baseline);
static_assert(factorize(2,limit,0).status==Status::overflow);
static_assert(factorize(0,1,0).status==Status::invalid_input);
}
int main() {
    return redux_test::s.status==swe::redux::Status::ok &&
           redux_test::t.numerator==1 && redux_test::t.denominator==2 ? 0 : 1;
}
