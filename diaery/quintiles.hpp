// SPDX-License-Identifier: 0BSD
// THAILONGA FREQ empirical quintiles from accepted measurements.
// Each boundary is an ORDER STATISTIC, not a Gaussian assumption.
#ifndef SWE_DIAERY_QUINTILES_HPP
#define SWE_DIAERY_QUINTILES_HPP
#include "freq.hpp"
namespace swe::diaery::quintiles {
using freq::U64;
enum class State { Ready, NullNoData, MissingScratch, InvalidInput };
struct Result {
 State state;
 double boundaries[6];  // minimum, 20%, 40%, 60%, 80%, maximum
 U64 count;
};
// Caller owns scratch storage >= count; no mutation of canonical samples.
// Sorting uses nonrecursive in-place heapsort for bounded call stack.
constexpr void swap(double& a,double& b) noexcept {
 const double t=a;a=b;b=t;
}
constexpr void sift_down(double* a,U64 start,U64 end) noexcept {
 U64 root=start;
 while(root<=end/2ULL){
   if(root>(~0ULL-1ULL)/2ULL)break;
   U64 child=root*2ULL+1ULL;
   if(child>end)break;
   if(child<end&&a[child]<a[child+1ULL])++child;
   if(!(a[root]<a[child]))break;
   swap(a[root],a[child]);root=child;
 }
}
[[nodiscard]] constexpr Result calculate(const freq::Samples& s,
                                          double* scratch,
                                          U64 scratch_capacity) noexcept {
 if(!s.count)return {State::NullNoData,{},0ULL};
 if(!s.values||!scratch||s.count>s.capacity||s.count>scratch_capacity)
    return {State::MissingScratch,{},s.count};
 for(U64 i=0;i<s.count;++i){
   if(!freq::admissible(s.values[i]))return {State::InvalidInput,{},s.count};
   scratch[i]=s.values[i];
 }
 for(U64 i=s.count/2ULL;i>0ULL;){
   --i;sift_down(scratch,i,s.count-1ULL);
 }
 for(U64 i=s.count-1ULL;i>0ULL;--i){
   swap(scratch[0],scratch[i]);sift_down(scratch,0,i-1ULL);
 }
 Result r{State::Ready,{},s.count};
 r.boundaries[0]=scratch[0];
 r.boundaries[5]=scratch[s.count-1ULL];
 for(U64 q=1ULL;q<=4ULL;++q){
   // Nearest-rank order statistic, documented; no hidden truncation of
   // an empirical measurement or fabricated fractional observation.
   const U64 whole=s.count/5ULL;
   const U64 rem=s.count%5ULL;
   const U64 rank=whole*q+(rem*q+4ULL)/5ULL; // ceil(q*n/5)
   r.boundaries[q]=scratch[rank?rank-1ULL:0ULL];
 }
 return r;
}
} // namespace swe::diaery::quintiles
#endif
