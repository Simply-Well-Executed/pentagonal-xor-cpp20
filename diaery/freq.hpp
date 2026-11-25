// SPDX-License-Identifier: 0BSD
// THAILONGA FREQ: empirical count of X/Y/Z at VALUE, without sample cap.
// Memory exhaustion is explicit HOLD; never silently discard an observation.
// No actual published-study rows are imported by this module.
#ifndef SWE_DIAERY_FREQ_HPP
#define SWE_DIAERY_FREQ_HPP
namespace swe::diaery::freq {
using U64=unsigned long long;
constexpr unsigned BINS=16u; // display resolution, NEVER sample capacity
struct Samples {
 double* values; U64 count; U64 capacity;
};
enum class State {
 Ready, NullNoData, NullTooFew, NullZeroVariance,
 InvalidInput, CapacityRequired, Overflow, MemoryHold
};
[[nodiscard]] constexpr bool admissible(double x) noexcept {
 return x==x && x>0.0 && x<=1000000000.0;
}
// The native host reserves/grows memory BEFORE calling append. There is no
// hard-coded observation ceiling. All prior sample indices remain intact.
[[nodiscard]] constexpr State append(Samples& s,double v) noexcept {
 if(!admissible(v))return State::InvalidInput;
 if(s.count==~0ULL)return State::Overflow;
 if(!s.values || s.count>=s.capacity)return State::CapacityRequired;
 s.values[s.count++]=v;
 return State::Ready;
}
struct Histogram {
 State state;
 U64 bins[BINS];
 U64 total,maximum_count;
 double minimum,maximum;
};
[[nodiscard]] constexpr Histogram count_bins(const Samples& s) noexcept {
 Histogram h{State::NullNoData,{},0ULL,0ULL,0.,0.};
 if(!s.count)return h;
 if(!s.values || s.count>s.capacity)
   return {State::InvalidInput,{},0ULL,0ULL,0.,0.};
 double low=s.values[0],high=s.values[0];
 if(!admissible(low))return {State::InvalidInput,{},0ULL,0ULL,0.,0.};
 for(U64 i=1;i<s.count;++i) {
   const double value=s.values[i];
   if(!admissible(value))return {State::InvalidInput,{},0ULL,0ULL,0.,0.};
   if(value<low)low=value;
   if(value>high)high=value;
 }
 h.minimum=low;h.maximum=high;
 for(U64 i=0;i<s.count;++i){
   const double value=s.values[i];
   unsigned bin=0u;
   if(high>low){
      const double scaled=((value-low)/(high-low))*static_cast<double>(BINS);
      bin=scaled>=static_cast<double>(BINS)?BINS-1u:
          static_cast<unsigned>(scaled);
   }
   ++h.bins[bin];
   if(h.bins[bin]>h.maximum_count)h.maximum_count=h.bins[bin];
   ++h.total;
 }
 h.state=State::Ready;
 return h;
}
// COUNT of X exactly AT VALUE, not histogram approximation.
// For measurement ranges, users should instead query declared bins.
[[nodiscard]] constexpr U64 count_at_value(const Samples& s,double value) noexcept {
 if(!admissible(value)||!s.values||s.count>s.capacity)return 0ULL;
 U64 n=0ULL;
 for(U64 i=0;i<s.count;++i)if(s.values[i]==value)++n;
 return n;
}
struct Bell {
 State state;
 double mean,sample_variance,sample_standard_deviation;
 U64 count;
};
[[nodiscard]] constexpr double sqrt_approx(double x) noexcept {
 if(x<=0.0)return 0.0;
 double v=x>=1.0?x:1.0;
 for(unsigned i=0;i<40u;++i)v=(v+x/v)/2.0;
 return v;
}
[[nodiscard]] constexpr Bell fit_bell(const Samples& s) noexcept {
 if(!s.count)return {State::NullNoData,0.,0.,0.,0ULL};
 if(!s.values||s.count>s.capacity)return {State::InvalidInput,0.,0.,0.,0ULL};
 if(s.count<3ULL)return {State::NullTooFew,0.,0.,0.,s.count};
 double mean=0.,m2=0.;
 for(U64 i=0;i<s.count;++i){
   const double value=s.values[i];
   if(!admissible(value))return {State::InvalidInput,0.,0.,0.,s.count};
   const double delta=value-mean;
   mean+=delta/static_cast<double>(i+1ULL);
   const double d2=value-mean;
   m2+=delta*d2;
 }
 const double variance=m2/static_cast<double>(s.count-1ULL);
 if(!(variance>1.0e-16))return {State::NullZeroVariance,mean,variance,0.,s.count};
 return {State::Ready,mean,variance,sqrt_approx(variance),s.count};
}
[[nodiscard]] constexpr double negative_exp_approx(double a) noexcept {
 if(!(a>=0.0)||a>50.0)return 0.0;
 double v=1.0+a/256.0;
 for(unsigned i=0;i<8u;++i)v*=v;
 return 1.0/v;
}
// Gaussian overlay is a fit, not the observed empirical count. Its expected
// count uses the DATA-DERIVED histogram bin width, not a 12-inch preset.
[[nodiscard]] constexpr double expected_bin_count(const Bell& b,double inches,
                                                  double bin_width) noexcept {
 if(b.state!=State::Ready||!admissible(inches)||!(bin_width>0.0))return 0.0;
 const double offset=(inches-b.mean)/b.sample_standard_deviation;
 constexpr double root_two_pi=2.5066282746310005;
 return (static_cast<double>(b.count)*bin_width/
         (root_two_pi*b.sample_standard_deviation))*
         negative_exp_approx(0.5*offset*offset);
}
} // namespace swe::diaery::freq
#endif
