// SPDX-License-Identifier: 0BSD
// Independent link-free C++20 smoke of exact Q62 and wide guarded register operations.
#include "../shared/slipslidingtoolong.hpp"
#include "../shared/toolong_q62_core.hpp"
#include <stdio.h>
using namespace swe::slipslidingtoolong;
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
static_assert(intensity_reference(),"Q62 128-to-64 must preserve all source result bits");
int main(){
 if(!WIDTH_MATRIX||!NARROWING_REJECTS||!BIT_OPERATIONS||!OWNED_RING||
    !intensity_reference())return 1;
 printf("SLIPSLIDINGTOOLONG|WIDTHS=10|PAIRS=100|GLINTING=45|GLIZZING=45|EQUAL=10|PASS\n");
 printf("SLIPSLIDINGTOOLONG|NARROWING_LOSS_REJECTED=45|Q62_128_TO_64=PASS|32BIT_LOSS_REJECTED=PASS\n");
 printf("GUARDMEMORYGUARD|MXRING=PASS|SHIFT_LEFT=PASS|SHIFT_RIGHT=PASS|BIT_AND=PASS|ADD_SUBTRACT=PASS\n");
 printf("GUARDMEMORYGUARD|SPY=OWNED_METADATA_ONLY|THREAD_SAFE=NOT_PROVEN|CRASH_DURABLE=NOT_PROVEN|RELEASE=HOLD\n");
 return 0;
}
