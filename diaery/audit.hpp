// SPDX-License-Identifier: 0BSD
// THAILONGA portable ASCII TSV audit writer, no STL, no silent truncation.
// IEEE754 byte fields preserve exact double object representation.
// A failed buffer request is HOLD, never a shortened "successful" export.
#ifndef SWE_DIAERY_AUDIT_HPP
#define SWE_DIAERY_AUDIT_HPP
#include "thailonga.hpp"
#include "freq.hpp"
#include "circle_band.hpp"
namespace swe::diaery::audit {
using U64=unsigned long long;
struct Writer {
 char* data; U64 cap,used; bool ok;
 constexpr void put(char c) noexcept {
    if(!ok||!data||used>=cap){ok=false;return;}
    data[used++]=c;
 }
 constexpr void text(const char* s) noexcept {
    if(!s){text("NULL");return;}
    for(U64 i=0ULL;s[i];++i)put(s[i]);
 }
 constexpr void integer(U64 n) noexcept {
    char digits[24]{};unsigned count=0u;
    do{digits[count++]=static_cast<char>('0'+n%10ULL);n/=10ULL;}
    while(n&&count<24u);
    while(count)put(digits[--count]);
 }
 // Full 8-byte IEEE-754 payload in memory byte order, not rounded decimal.
 void hex_double(double value) noexcept {
    const unsigned char* ptr=reinterpret_cast<const unsigned char*>(&value);
    constexpr char digits[]="0123456789ABCDEF";
    for(unsigned i=0u;i<sizeof(double);++i){
        put(digits[ptr[i]>>4u]);put(digits[ptr[i]&15u]);
    }
 }
 constexpr void end() noexcept {put('\0');}
};
[[nodiscard]] inline bool emit(
 Writer& w,const thailonga::Frame& f,
 const freq::Samples& x,const freq::Samples& y,const freq::Samples& z,
 const bool* layers,bool circle_gap,
 U64 rejected,bool memory_hold) noexcept {
 if(x.count!=y.count||y.count!=z.count)return false;
 if((x.count&&!x.values)||(y.count&&!y.values)||(z.count&&!z.values))
    return false;
 w.text("THAILONGA\tAUDIT\tV1\n");
 w.text("STATUS\tRELEASE=HOLD\tDATA=MANUAL_EXPLORATORY\n");
 w.text("PROVENANCE\tPR316\tNO_PUBLISHED_ROWS_IMPORTED\n");
 w.text("UNITS\tX=inch\tY=inch\tZ=inch\tFREQ=count\n");
 w.text("ORIGIN\tX=0\tY=0\tGEOMETRY=INTERCEPT_NOT_REGRESSION\n");
 w.text("STUDY_REFERENCE\tX/Y=HERBENICK_2014\tZ=LLOYD_2005\tRANGES_NOT_BIOLOGICAL_LIMITS\n");
 w.text("FILTREX\t");w.text(f.eligible?"ELIGIBLE":"HOLD");w.put('\n');
 w.text("FILTROX\t");w.text(f.within_sample_ranges?"WITHIN_SAMPLES":"OUTSIDE_OR_NULL");w.put('\n');
 w.text("SOURCE_IEEE754_NATIVE_BYTES\t");
 w.hex_double(f.original.length_inches);w.put('\t');
 w.hex_double(f.original.circumference_inches);w.put('\t');
 w.hex_double(f.original.depth_inches);w.put('\n');
 w.text("BELL_X\t");w.text(freq::fit_bell(x).state==freq::State::Ready?"FITTED":"NULL");w.put('\n');
 w.text("BELL_Y\t");w.text(freq::fit_bell(y).state==freq::State::Ready?"FITTED":"NULL");w.put('\n');
 w.text("BELL_Z\t");w.text(freq::fit_bell(z).state==freq::State::Ready?"FITTED":"NULL");w.put('\n');
 w.text("SAMPLE_SD_IEEE754_NATIVE_BYTES\t");
 w.hex_double(freq::fit_bell(x).sample_standard_deviation);w.put('\t');
 w.hex_double(freq::fit_bell(y).sample_standard_deviation);w.put('\t');
 w.hex_double(freq::fit_bell(z).sample_standard_deviation);w.put('\n');
 w.text("COUNT_TOTAL\t");w.integer(x.count);w.put('\n');
 w.text("REJECTED_ATTEMPTS\t");w.integer(rejected);w.put('\n');
 w.text("MEMORY_HOLD\t");w.text(memory_hold?"YES":"NO");w.put('\n');
 w.text("CIRCLE\t");w.text(circle_gap?"GAP":"NICE_FIRM");
 w.text("\tRESEARCH=NULL\tBAND=41..42_PERCENT_UNVERIFIED\n");
 w.text("CIRCLE_UNITS\tNORMALIZED_PERCENT\tNOT_ANATOMICAL_INCHES\n");
 w.text("PROJECTIONS\t");
 for(unsigned i=0u;i<10u;++i){if(i)w.put(',');w.put('{');
    w.put(static_cast<char>('0'+i));w.put('}');w.put(':');
    w.text(layers&&layers[i]?"ON":"OFF");}
 w.put('\n');
 w.text("UNCERTAINTY\tUNKNOWN\tNO_VALIDATED_PHYSIOLOGICAL_PREDICTION\n");
 w.text("QUINTILES\tNEAREST_RANK_AVAILABLE_IN_NATIVE_CORE\tNOT_ANATOMICAL_CUTOFFS\n");
 w.text("ROW\tX_IEEE754_NATIVE_BYTES\tY_IEEE754_NATIVE_BYTES\tZ_IEEE754_NATIVE_BYTES\n");
 for(U64 i=0ULL;i<x.count;++i){
    w.integer(i+1ULL);w.put('\t');
    w.hex_double(x.values[i]);w.put('\t');
    w.hex_double(y.values[i]);w.put('\t');
    w.hex_double(z.values[i]);w.put('\n');
 }
 w.end();
 return w.ok;
}
} // namespace swe::diaery::audit
#endif
