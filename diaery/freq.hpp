// SPDX-License-Identifier: 0BSD
// THAILONGA FREQ: count of qualified values in equally sized inch bins.
// Manual exploratory samples do NOT become verified study observations.
#ifndef SWE_DIAERY_FREQ_HPP
#define SWE_DIAERY_FREQ_HPP
namespace swe::diaery::freq {
constexpr unsigned CAPACITY=64u;
constexpr unsigned BINS=16u;
constexpr double AXIS_MAX_INCHES=12.0;
struct Samples {
 double values[CAPACITY];
 unsigned count;
};
enum class State { Ready, NullNoData, NullTooFew, NullZeroVariance, InvalidInput, Full };
[[nodiscard]] constexpr bool admissible(double x) noexcept {
 return x==x && x>0.0 && x<=AXIS_MAX_INCHES;
}
[[nodiscard]] constexpr State append(Samples& samples,double value) noexcept {
 if(!admissible(value))return State::InvalidInput;
 if(samples.count>=CAPACITY)return State::Full;
 samples.values[samples.count++]=value;
 return State::Ready;
}
[[nodiscard]] constexpr unsigned bin_index(double v) noexcept {
 if(!admissible(v))return BINS;
 unsigned i=static_cast<unsigned>((v*static_cast<double>(BINS))/AXIS_MAX_INCHES);
 return i>=BINS?BINS-1u:i;
}
struct Histogram {
 State state;
 unsigned bins[BINS];
 unsigned total;
 unsigned maximum_count;
};
[[nodiscard]] constexpr Histogram count_bins(const Samples& s) noexcept {
 Histogram h{State::NullNoData,{},0u,0u};
 if(s.count>CAPACITY)return {State::InvalidInput,{},0u,0u};
 for(unsigned i=0;i<s.count;++i){
   const unsigned index=bin_index(s.values[i]);
   if(index==BINS)return {State::InvalidInput,{},0u,0u};
   const unsigned next=++h.bins[index];
   if(next>h.maximum_count)h.maximum_count=next;
   ++h.total;
 }
 if(h.total)h.state=State::Ready;
 return h;
}
// Exact-count query of a source VALUE, not a binned approximation.
[[nodiscard]] constexpr unsigned count_at_value(const Samples& s,double value) noexcept {
 if(s.count>CAPACITY||!admissible(value))return 0u;
 unsigned n=0u;
 for(unsigned i=0;i<s.count;++i)if(s.values[i]==value)++n;
 return n;
}
struct Bell {
 State state;
 double mean;
 double sample_variance;
 double sample_standard_deviation;
 unsigned count;
};
[[nodiscard]] constexpr double sqrt_approx(double value) noexcept {
 if(value<=0.0)return 0.0;
 double v=value>=1.0?value:1.0;
 for(unsigned i=0;i<40u;++i)v=(v+value/v)/2.0;
 return v;
}
[[nodiscard]] constexpr Bell fit_bell(const Samples& s) noexcept {
 if(s.count>CAPACITY)return {State::InvalidInput,0.,0.,0.,0u};
 if(!s.count)return {State::NullNoData,0.,0.,0.,0u};
 if(s.count<3u)return {State::NullTooFew,0.,0.,0.,s.count};
 // Welford incremental sample variance, stable for positive inch values.
 double mean=0.0,m2=0.0;
 for(unsigned i=0;i<s.count;++i){
   if(!admissible(s.values[i]))return {State::InvalidInput,0.,0.,0.,s.count};
   const double delta=s.values[i]-mean;
   mean+=delta/static_cast<double>(i+1u);
   const double after=s.values[i]-mean;
   m2+=delta*after;
 }
 const double variance=m2/static_cast<double>(s.count-1u);
 if(!(variance>1.0e-16))
    return {State::NullZeroVariance,mean,variance,0.,s.count};
 return {State::Ready,mean,variance,sqrt_approx(variance),s.count};
}
// Bounded approximation of exp(-a), a>=0, as (1+a/256)^(-256).
// Render-only fit; source histogram counts remain integers.
[[nodiscard]] constexpr double negative_exp_approx(double a) noexcept {
 if(!(a>=0.0)||a>50.0)return 0.0;
 double v=1.0+a/256.0;
 for(unsigned i=0;i<8u;++i)v*=v;
 return 1.0/v;
}
// Expected counts per 0.75-inch bin under fitted Gaussian (not observed counts).
[[nodiscard]] constexpr double expected_bin_count(const Bell& b,double inches) noexcept {
 if(b.state!=State::Ready||!admissible(inches))return 0.0;
 const double offset=(inches-b.mean)/b.sample_standard_deviation;
 constexpr double root_two_pi=2.5066282746310005;
 constexpr double width=AXIS_MAX_INCHES/static_cast<double>(BINS);
 return (static_cast<double>(b.count)*width/(root_two_pi*b.sample_standard_deviation))*
        negative_exp_approx(0.5*offset*offset);
}
} // namespace swe::diaery::freq
#endif
