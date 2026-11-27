// SPDX-License-Identifier: 0BSD
#include "redux.hpp"
constexpr REDUX::Task tasks[]={{3,0},{2,1},{4,0},{1,0}};
constexpr REDUX::Task cyclic[]={{1,2},{1,1}};
constexpr REDUX::Task missing[]={{1,2}};
constexpr REDUX::Task overflow[]={{REDUX::limit,0},{1,0}};
constexpr REDUX::Task zero[]={{0,0}};
constexpr auto s=REDUX::analyze(tasks,4);
constexpr auto r=REDUX::reduction(4,1);
constexpr auto t=REDUX::time_reduction(s);
constexpr auto speed=REDUX::ideal_speedup(s);
constexpr auto f=REDUX::factorize(4,3,4);
static_assert(r.status==REDUX::Status::ok && r.numerator==3 && r.denominator==4);
static_assert(s.status==REDUX::Status::ok && s.work==10 && s.critical_path==5);
static_assert(s.order[0]==0 && s.order[1]==1 && s.finish[1]==5);
static_assert(t.numerator==1 && t.denominator==2);
static_assert(speed.numerator==2 && speed.denominator==1);
static_assert(REDUX::xor_reduce(0xA5,0x5A,0xFF)==0xFF);
static_assert(f.status==REDUX::Status::ok && f.original==16 && f.optimized==7);
static_assert(REDUX::reduction(0,0).status==REDUX::Status::zero_baseline);
static_assert(REDUX::reduction(4,5).negative && REDUX::reduction(4,5).numerator==1);
static_assert(REDUX::reduction(4,4).denominator==1);
static_assert(REDUX::analyze(cyclic,2).status==REDUX::Status::cycle);
static_assert(REDUX::analyze(missing,1).status==REDUX::Status::invalid_dependency);
static_assert(REDUX::analyze(overflow,2).status==REDUX::Status::overflow);
static_assert(REDUX::analyze(nullptr,0).status==REDUX::Status::invalid_input);
static_assert(REDUX::analyze(tasks,33).status==REDUX::Status::invalid_input);
static_assert(REDUX::time_reduction(REDUX::analyze(zero,1)).status==REDUX::Status::zero_baseline);
static_assert(REDUX::factorize(2,REDUX::limit,0).status==REDUX::Status::overflow);
static_assert(REDUX::factorize(0,1,0).status==REDUX::Status::invalid_input);
static_assert(REDUX::dakatana(10,1)==REDUX::Dakatana::qualified);
static_assert(REDUX::dakatana(9,1)==REDUX::Dakatana::below);
static_assert(REDUX::dakatana(10000,30)==REDUX::Dakatana::qualified);
static_assert(REDUX::dakatana(141,24)==REDUX::Dakatana::below);
static_assert(REDUX::dakatana(98,43)==REDUX::Dakatana::below);
static_assert(REDUX::dakatana(0,0)==REDUX::Dakatana::undefined_baseline);
static_assert(REDUX::dakatana(0,1)==REDUX::Dakatana::undefined_baseline);
static_assert(REDUX::dakatana(1,0)==REDUX::Dakatana::eliminated);
static_assert(REDUX::dakatana(1,2)==REDUX::Dakatana::below);
static_assert(REDUX::dakatana(REDUX::limit,REDUX::limit/10ULL)==REDUX::Dakatana::qualified);
static_assert(REDUX::dakatana(REDUX::limit,REDUX::limit/10ULL+1)==REDUX::Dakatana::below);
static_assert(REDUX::dakatana(REDUX::limit,REDUX::limit)==REDUX::Dakatana::below);
static_assert(REDUX::reduction(10000,30).numerator==997);
static_assert(REDUX::reduction(10000,30).denominator==1000);
static_assert(REDUX::fraction(10000,30).numerator==1000);
static_assert(REDUX::fraction(10000,30).denominator==3);
int main() {
    return s.status==REDUX::Status::ok &&
           t.numerator==1 && t.denominator==2 ? 0 : 1;
}
