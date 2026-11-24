// SPDX-License-Identifier: 0BSD
// Canonical source constants + GUARDMEMORYGUARD width registry qualification.
// C++20 /O2; MSVC uses the existing Q62 32-bit-limb implementation.
#include "../shared/slipslidingtoolong.hpp"
#include "../shared/toolong_q62_core.hpp"
#include "../shared/slidingtoolong.hpp"
#include "../shared/constant_formats_register_001.hpp"
#include "../shared/precision_rounding_policy_001.hpp"
#include <stdio.h>
using namespace swe::slipslidingtoolong;
namespace fmt=swe::constant_formats_register_001;
[[nodiscard]] constexpr bool intensity_reference() noexcept {
 const auto& i=swe::toolong_q62::intensity;
 if(swe::toolong_q62::multiply_shift62(i.source_word)!=i.expected_integer)return false;
 Register r=make_register(128u,99u);
 r.limb[0]=static_cast<u32>(i.expected_integer);
 r.limb[1]=static_cast<u32>(i.expected_integer>>32u);
 r.digest=checksum(r);
 const Projection down=GLINTING(r,64u);
 if(down.status!=Result::PASS||!GUARDMEMORYGUARD(down.value))return false;
 const Projection bad=GLINTING(down.value,32u);
 return bad.status==Result::REJECT_LOSS;
}
static_assert(intensity_reference(),"Q62 128-to-64 must preserve all result bits");
[[nodiscard]] constexpr bool requested_width_routes() noexcept {
 // Values carried here are the checked 64-bit Q62 RESULT, not the raw product.
 const unsigned long long value=swe::toolong_q62::intensity.expected_integer;
 Register seed=make_register(64u,99u);
 seed.limb[0]=static_cast<u32>(value&0xffffffffULL);
 seed.limb[1]=static_cast<u32>(value>>32u);
 seed.digest=checksum(seed);
 if(!GUARDMEMORYGUARD(seed))return false;
 struct Case {unsigned source,destination;bool expected_fit;};
 constexpr Case cases[]{
  {128u,64u,true}, {64u,32u,false},
  {256u,128u,true},{256u,192u,true},{256u,224u,true},
  {512u,384u,true},{512u,448u,true},{512u,480u,true},
  {512u,256u,true},{512u,64u,true},
  {32u,64u,false}
 };
 for(const Case test:cases) {
  if(test.source<64u){
   // This source cannot hold the value; no modulo/truncation is permitted.
   if(fmt::fits_u32(value))return false;
   continue;
  }
  const Projection up=GLIZZING(seed,test.source);
  if(up.status!=Result::PASS)return false;
  const Projection step=project(up.value,test.destination);
  if(test.expected_fit) {
   if(step.status!=Result::PASS)return false;
   const Projection restored=project(step.value,64u);
   if(restored.status!=Result::PASS||!same(seed,restored.value))return false;
  } else if(step.status!=Result::REJECT_LOSS)return false;
 }
 return fmt::all_source_representations();
}
static_assert(requested_width_routes(),"GLINTING/GLIZZING required routes failed");
static_assert(!fmt::fits_signed_long32(swe::eulers_jig::ONE_TRIANGLE_X1E17));
static_assert(fmt::fits_signed_long64(swe::eulers_jig::ONE_TRIANGLE_X1E17));
static_assert(!fmt::fits_signed_long64(swe::toolong_q62::euler.source_word));

namespace prec=swe::precision_rounding_policy_001;
[[nodiscard]] constexpr bool precision_rounding_001() noexcept {
 // Derived policy; original historical record outputs remain untouched.
 const auto e17=prec::round_nearest_even(swe::toolong_q62::euler.source_word,100000000000000000ULL);
 const auto p17=prec::round_nearest_even(swe::toolong_q62::pi.source_word,100000000000000000ULL);
 const auto i17=prec::round_nearest_even(swe::toolong_q62::intensity.source_word,100000000000000000ULL);
 const auto e18=prec::round_nearest_even(swe::toolong_q62::euler.source_word,1000000000000000000ULL);
 const auto p18=prec::round_nearest_even(swe::toolong_q62::pi.source_word,1000000000000000000ULL);
 const auto i18=prec::round_nearest_even(swe::toolong_q62::intensity.source_word,1000000000000000000ULL);
 if(!e17.valid||!p17.valid||!i17.valid||!e18.valid||!p18.valid||!i18.valid)return false;
 if(e17.rounded!=271828182845904524ULL||p17.rounded!=314159265358979324ULL||
    i17.rounded!=6849296450333523ULL)return false;
 if(e18.rounded!=2718281828459045235ULL||p18.rounded!=3141592653589793239ULL||
    i18.rounded!=68492964503335234ULL)return false;
 if(i18.rounded-swe::slidingtoolong::INTENSITY_X1E18!=4ULL)return false;
 if(e17.remainder!=2403300696930910208ULL||
    p17.remainder!=3925840205563691008ULL||
    i17.remainder!=1790435901504094208ULL)return false;
 if(!e17.rounded_up||!p17.rounded_up||i17.rounded_up||
    e18.rounded_up==true||p18.rounded_up==false||i18.rounded_up==false)return false;
 // Tie policies: 0.5=>0, 1.5=>2, 2.5=>2. No silent discarded bits.
 const unsigned long long half=1ULL<<61u;
 const auto even0=prec::round_nearest_even(1ULL,half);
 const auto even2=prec::round_nearest_even(3ULL,half);
 const auto even2again=prec::round_nearest_even(5ULL,half);
 if(!even0.valid||!even2.valid||!even2again.valid||
    even0.rounded!=0ULL||even2.rounded!=2ULL||even2again.rounded!=2ULL)return false;
 if(even0.remainder!=half||even2.remainder!=half||even2again.remainder!=half)return false;
 if(prec::round_nearest_even(~0ULL,~0ULL).valid ||
    prec::round_nearest_even(1ULL,0ULL).valid)return false;
 return true;
}
static_assert(precision_rounding_001(),"PRECISION-ROUNDING-001: exact remainder or nearest-even witness failed");

int main(){
 if(!WIDTH_MATRIX||!NARROWING_REJECTS||!BIT_OPERATIONS||!OWNED_RING||
    !intensity_reference()||!requested_width_routes()||
    !precision_rounding_001())return 1;
 printf("SLIPSLIDINGTOOLONG|WIDTHS=10|PAIRS=100|GLINTING=45|GLIZZING=45|EQUAL=10|PASS\n");
 printf("SLIPSLIDINGTOOLONG|NARROWING_LOSS_REJECTED=45|Q62_128_TO_64=PASS|32BIT_LOSS_REJECTED=PASS\n");
 printf("GUARDMEMORYGUARD|MXRING=PASS|SHIFT_LEFT=PASS|SHIFT_RIGHT=PASS|BIT_AND=PASS|ADD_SUBTRACT=PASS\n");
 printf("GUARDMEMORYGUARD|SPY=OWNED_METADATA_ONLY|THREAD_SAFE=NOT_PROVEN|CRASH_DURABLE=NOT_PROVEN|RELEASE=HOLD\n");
 printf("CONSTANT_FORMATS|RECORDS=21|U64=PASS|LONG16=PASS|U32_PAIRS=PASS|WIDE_ROUTES=PASS|RELEASE=HOLD\n");
 printf("PRECISION_ROUNDING|EXACT_128_PRODUCT=PASS|REMAINDER=RETAINED|NEAREST_TIES_EVEN=PASS|NO_SILENT_TRUNCATION=PASS|RELEASE=HOLD\n");
 return 0;
}
