// SPDX-License-Identifier: 0BSD
// Canonical source constants + GUARDMEMORYGUARD width registry qualification.
// C++20 /O2; MSVC uses the existing Q62 32-bit-limb implementation.
#include "../shared/slipslidingtoolong.hpp"
#include "../shared/toolong_q62_core.hpp"
#include "../shared/slidingtoolong.hpp"
#include "../shared/constant_formats_register_001.hpp"
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
int main(){
 if(!WIDTH_MATRIX||!NARROWING_REJECTS||!BIT_OPERATIONS||!OWNED_RING||
    !intensity_reference()||!requested_width_routes())return 1;
 printf("SLIPSLIDINGTOOLONG|WIDTHS=10|PAIRS=100|GLINTING=45|GLIZZING=45|EQUAL=10|PASS\n");
 printf("SLIPSLIDINGTOOLONG|NARROWING_LOSS_REJECTED=45|Q62_128_TO_64=PASS|32BIT_LOSS_REJECTED=PASS\n");
 printf("GUARDMEMORYGUARD|MXRING=PASS|SHIFT_LEFT=PASS|SHIFT_RIGHT=PASS|BIT_AND=PASS|ADD_SUBTRACT=PASS\n");
 printf("GUARDMEMORYGUARD|SPY=OWNED_METADATA_ONLY|THREAD_SAFE=NOT_PROVEN|CRASH_DURABLE=NOT_PROVEN|RELEASE=HOLD\n");
 printf("CONSTANT_FORMATS|RECORDS=21|U64=PASS|LONG16=PASS|U32_PAIRS=PASS|WIDE_ROUTES=PASS|RELEASE=HOLD\n");
 return 0;
}
