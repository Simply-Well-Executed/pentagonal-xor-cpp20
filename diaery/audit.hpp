/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
// THAILONGA portable ASCII TSV audit writer, no STL, no silent truncation.
// IEEE754 byte fields preserve exact double object representation.
// A failed buffer request is HOLD, never a shortened "successful" export.
#ifndef SWE_DIAERY_AUDIT_HPP
#define SWE_DIAERY_AUDIT_HPP
#include "thailonga.hpp"
#include "freq.hpp"
#include "circle_band.hpp"
#include "quintiles.hpp"
#include "rejection_ledger.hpp"
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
 U64 rejected,bool memory_hold,
 const quintiles::Result* qx,
 const quintiles::Result* qy,
 const quintiles::Result* qz,
 const reject::Ledger& rejections) noexcept {
 if(x.count!=y.count||y.count!=z.count||!reject::complete(rejections))return false;
 if((x.count&&!x.values)||(y.count&&!y.values)||(z.count&&!z.values))
    return false;
 w.text("THAILONGA\tAUDIT\tV1\n");
 w.text("STATUS\tRELEASE=HOLD\tDATA=MANUAL_EXPLORATORY\n");
 w.text("PROVENANCE\tPR316\tNO_PUBLISHED_ROWS_IMPORTED\n");
 w.text("UNITS\tX=inch\tY=inch\tZ=inch\tFREQ=count\n");
 w.text("FIGURE_1\tTHAILONGA_DYNTR0X_XY\tSOURCE_PLANE\n");
 w.text("FIGURE_2\tTHAILONGA_FREQ_X_Y_Z\tEMPIRICAL_COUNT_AT_VALUE\n");
 w.text("FIGURE_3\tTHAILONGA_CIRCLE_BAND\tEXPLORATORY_NORMALIZED_ANNULUS\n");
 w.text("XY_MAJOR\tX=1_INCH\tY=1_INCH\tORIGIN=0_0\n");
 w.text("XY_SUBUNITS\tX=1/4_INCH\tY=1/3_INCH\tDISPLAY_ONLY\n");
 w.text("FREQ_BINS\t16_DISPLAY_BINS\tNOT_SAMPLE_CAPACITY\n");
 w.text("FREQ_COUNTS\tEXACT_VALUE_COUNT\tSTORED_IEEE754_EQUALITY\n");
 w.text("CIRCLE_SUBUNITS\t72_SECTORS\tGAP_CANARY_SINGLE_MISSING_CELL\n");
 w.text("CIRCLE_BOUNDS\t41..42_PERCENT\tUNVERIFIED_OUTCOME\n");
 w.text("BIOMEASUREMENT_MODEL\tEXPLORATORY_CYLINDER\tDEPTH_ALONE_CANNOT_PREDICT_WIDTH_OR_ELASTICITY\n");
 w.text("REJECTION_PROVENANCE\tPER_EVENT_IEEE754\tREASON_CODES_1_FILTREX_2_FREQUENCY_3_MEMORY\n");
 w.text("STUDY_RANGE_METHOD\tMARGINAL_SAMPLE_EXTREMA\tNOT_DIAGNOSTIC\n");

 w.text("ORIGIN\tX=0\tY=0\tGEOMETRY=INTERCEPT_NOT_REGRESSION\n");
 w.text("STUDY_REFERENCE\tX/Y=HERBENICK_2014\tZ=LLOYD_2005\tRANGES_NOT_BIOLOGICAL_LIMITS\n");
 w.text("FILTREX\t");w.text(f.eligible?"ELIGIBLE":"HOLD");w.put('\n');
 w.text("FILTROX\t");w.text(f.within_sample_ranges?"WITHIN_SAMPLES":"OUTSIDE_OR_NULL");w.put('\n');
 w.text("INTERCEPT_LINE\t");
 w.text(f.intercept.valid?"ALGEBRAIC_VALID":"HOLD");w.put('\t');
 w.hex_double(f.intercept.slope);w.put('\t');
 w.hex_double(f.intercept.intercept);w.put('\n');
 w.text("PROJECTION_QUALIFICATION\t");
 for(unsigned i=0u;i<10u;++i){
   if(i)w.put(',');
   w.put('{');w.put(static_cast<char>('0'+i));w.put('}');
   w.put(':');
   w.text(f.projection_status[i]==projections::Status::Ready?"ALGEBRA":
          f.projection_status[i]==projections::Status::NullEvidence?"NULL":"HOLD");
 }
 w.put('\n');
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
 w.text("DARN_SECONDS\t");w.integer(f.time.exact_seconds);w.put('\n');
 w.text("DARN_REMAINDER_FS\t");w.integer(f.time.remainder_femtoseconds);w.put('\n');
 w.text("YARN_SECONDS_DERIVED\t");w.integer(f.time.shown_yarn_seconds);w.put('\n');
 w.text("SPLOON_DERIVED\t");w.integer(f.time.sploon);w.put('\n');
 const freq::Samples axes[3]{x,y,z};
 const char* labels[3]{"X","Y","Z"};
 const quintiles::Result* qs[3]{qx,qy,qz};
 for(unsigned a=0u;a<3u;++a){
   const auto bell=freq::fit_bell(axes[a]);
   const auto hist=freq::count_bins(axes[a]);
   w.text("STATS\t");w.text(labels[a]);
   w.text("\tMEAN_HEX=");w.hex_double(bell.mean);
   w.text("\tSD_HEX=");w.hex_double(bell.sample_standard_deviation);
   w.text("\tSAMPLE_VARIANCE_HEX=");w.hex_double(bell.sample_variance);
   w.text("\tMIN_HEX=");w.hex_double(hist.minimum);
   w.text("\tMAX_HEX=");w.hex_double(hist.maximum);w.put('\n');
   w.text("QUINTILES_NEAREST_RANK\t");w.text(labels[a]);
   if(!axes[a].count||!qs[a]||qs[a]->state!=quintiles::State::Ready||qs[a]->count!=axes[a].count)w.text("\tNULL_OR_HOLD");
   else for(unsigned q=0u;q<6u;++q){w.put('\t');w.hex_double(qs[a]->boundaries[q]);}
   w.put('\n');
 }
 w.text("REJECTED_ATTEMPTS\t");w.integer(rejected);w.put('\n');
 w.text("REJECTION_LEDGER_COUNT\t");w.integer(rejections.count);w.put('\n');
 w.text("REJECTION_LOST\t");w.integer(rejections.lost);w.put('\n');
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
 w.text("QUINTILE_POLICY\tNEAREST_RANK\tNOT_ANATOMICAL_CUTOFFS\n");
 w.text("REJECTION_ROW\tREASON_CODE\tX_HEX\tY_HEX\tZ_HEX\n");
 for(U64 i=0ULL;i<rejections.count;++i){
    const reject::Event e=rejections.events[i];
    w.integer(i+1ULL);w.put('\t');
    w.integer(static_cast<U64>(e.reason));w.put('\t');
    w.hex_double(e.x_inches);w.put('\t');
    w.hex_double(e.y_inches);w.put('\t');
    w.hex_double(e.z_inches);w.put('\n');
 }
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
