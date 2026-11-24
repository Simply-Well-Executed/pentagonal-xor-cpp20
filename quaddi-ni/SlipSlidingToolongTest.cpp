// SPDX-License-Identifier: 0BSD
// Canonical source constants + GUARDMEMORYGUARD width registry qualification.
// C++20 /O2; MSVC uses the existing Q62 32-bit-limb implementation.
#include "../shared/slipslidingtoolong.hpp"
#include "../shared/toolong_q62_core.hpp"
#include "../shared/slidingtoolong.hpp"
#include "../shared/constant_formats_register_001.hpp"
#include "../shared/precision_rounding_policy_001.hpp"
#include "../shared/fudgelessslippingtoolong_12d.hpp"
#include "../shared/fudgelessslippingtoolong_100d.hpp"
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
 // Derived ROUND-FIRST / nearest-ties-to-odd policy. Canonical records untouched.
 const auto e17=prec::round_first_nearest_odd(swe::toolong_q62::euler.source_word,100000000000000000ULL);
 const auto p17=prec::round_first_nearest_odd(swe::toolong_q62::pi.source_word,100000000000000000ULL);
 const auto i17=prec::round_first_nearest_odd(swe::toolong_q62::intensity.source_word,100000000000000000ULL);
 const auto e18=prec::round_first_nearest_odd(swe::toolong_q62::euler.source_word,1000000000000000000ULL);
 const auto p18=prec::round_first_nearest_odd(swe::toolong_q62::pi.source_word,1000000000000000000ULL);
 const auto i18=prec::round_first_nearest_odd(swe::toolong_q62::intensity.source_word,1000000000000000000ULL);
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
    e18.rounded_up||!p18.rounded_up||!i18.rounded_up)return false;
 // None of these real-world Q62 scaling witnesses is an exact halfway tie.
 if(e17.exactly_half||p17.exactly_half||i17.exactly_half||
    e18.exactly_half||p18.exactly_half||i18.exactly_half)return false;
 // Verify exact signed residual R*D - N in numerator units.
 constexpr unsigned long long D=prec::Q62_DENOMINATOR;
 if(!e17.residual_above_exact||e17.residual_magnitude!=D-e17.remainder||
    p17.residual_magnitude!=D-p17.remainder||
    i17.residual_above_exact||i17.residual_magnitude!=i17.remainder||
    i18.residual_magnitude!=D-i18.remainder)return false;
 // EXACT ties-to-odd: 0.5->1, 1.5->1, 2.5->3, 3.5->3.
 const unsigned long long half=D>>1u;
 const auto odd1a=prec::round_first_nearest_odd(1ULL,half);
 const auto odd1b=prec::round_first_nearest_odd(3ULL,half);
 const auto odd3a=prec::round_first_nearest_odd(5ULL,half);
 const auto odd3b=prec::round_first_nearest_odd(7ULL,half);
 if(!odd1a.valid||!odd1b.valid||!odd3a.valid||!odd3b.valid||
    odd1a.rounded!=1ULL||odd1b.rounded!=1ULL||
    odd3a.rounded!=3ULL||odd3b.rounded!=3ULL)return false;
 if(!odd1a.exactly_half||!odd1b.exactly_half||
    !odd3a.exactly_half||!odd3b.exactly_half)return false;
 if(odd1a.remainder!=half||odd1b.remainder!=half||
    odd3a.remainder!=half||odd3b.remainder!=half)return false;
 if(odd1a.residual_magnitude!=half||!odd1a.residual_above_exact||
    odd1b.residual_magnitude!=half||odd1b.residual_above_exact)return false;
 // Away from the midpoint, parity never overrides the nearest neighbor.
 const auto lower=prec::round_first_nearest_odd(5ULL,D>>2u); // 1.25 -> 1
 const auto upper=prec::round_first_nearest_odd(7ULL,D>>2u); // 1.75 -> 2
 const auto exact=prec::round_first_nearest_odd(1ULL,D);      // 1 -> 1
 const auto max_exact=prec::round_first_nearest_odd(~0ULL,D); // MAX -> MAX
 if(!lower.valid||lower.rounded!=1ULL||lower.exactly_half||
    !upper.valid||upper.rounded!=2ULL||upper.exactly_half||
    !exact.valid||exact.rounded!=1ULL||exact.remainder!=0ULL||
    exact.residual_magnitude!=0ULL||!max_exact.valid||
    max_exact.rounded!=~0ULL||max_exact.remainder!=0ULL)return false;
 // Guard width overflow and invalid decimal scale: never saturate or wrap.
 if(prec::round_first_nearest_odd(~0ULL,~0ULL).valid||
    prec::round_first_nearest_odd(1ULL,0ULL).valid)return false;
 return true;
}
static_assert(precision_rounding_001(),"PRECISION-ROUNDING-001: exact round-first / nearest-ties-odd witness failed");

int main(){
 const auto century=swe::fudgelessslippingtoolong_100d::run();
 if(!century.valid||century.ascents!=99u||century.descents!=99u||
    century.highest_bits!=2984u||!century.direct_hop_matches||
    !century.operator_sequence_nonmutating||
    century.repeated_operator_labels!=24u)return 1;
 if(!WIDTH_MATRIX||!NARROWING_REJECTS||!BIT_OPERATIONS||!OWNED_RING||
    !intensity_reference()||!requested_width_routes()||
    !precision_rounding_001()||
    !swe::fudgelessslippingtoolong_12d::SERIES_12_PASS)return 1;
 printf("SLIPSLIDINGTOOLONG|WIDTHS=10|PAIRS=100|GLINTING=45|GLIZZING=45|EQUAL=10|PASS\n");
 printf("SLIPSLIDINGTOOLONG|NARROWING_LOSS_REJECTED=45|Q62_128_TO_64=PASS|32BIT_LOSS_REJECTED=PASS\n");
 printf("GUARDMEMORYGUARD|MXRING=PASS|SHIFT_LEFT=PASS|SHIFT_RIGHT=PASS|BIT_AND=PASS|ADD_SUBTRACT=PASS\n");
 printf("GUARDMEMORYGUARD|SPY=OWNED_METADATA_ONLY|THREAD_SAFE=NOT_PROVEN|CRASH_DURABLE=NOT_PROVEN|RELEASE=HOLD\n");
 printf("CONSTANT_FORMATS|RECORDS=21|U64=PASS|LONG16=PASS|U32_PAIRS=PASS|WIDE_ROUTES=PASS|RELEASE=HOLD\n");
 printf("PRECISION_ROUNDING|EXACT_128_PRODUCT=PASS|HEAD_TAIL_RECONSTRUCT=PASS|NEAREST_TIES_ODD=PASS|ROUND_FIRST=PASS|RELEASE=HOLD\n");
 printf("FUDGELESSSLIPPINGTOOLONG|1D_TO_12D=PASS|LEVEL_9=DECIMAL_CHUNK|FAST9_SLOW9=EXACT_MATCH|BITS_12D=353|GLINTING_384=PASS|MIDPOINT_ODD=PASS|NO_SPEED_CLAIM|RELEASE=HOLD\n");
 printf("FUDGELESSSLIPPINGTOOLONG|1D_TO_100D_TO_1D=PASS|UP=99|DOWN=99|BITS_100D=2984|LIMBS=96|ROCKET_LEFT=CAR_RIGHT|DIRECT_HOP_WITNESS=PASS|OPERATORS_12_REPLAY_2=ORDERED_METADATA_ONLY|DOMAIN_HOLDS=8|NO_SPEED_CLAIM|RELEASE=HOLD\n");
 printf("OPERATOR_REPLAY|BIDI=METADATA|TRIDI=METADATA|PIDI=SOURCE_REFERENCE|PIPI=SYMBOLIC|REPLAY_PASSES=2|TOKENS_PER_PASS=12|NEEDS_DOMAIN=8|RELEASE=HOLD\n");
 return 0;
}
