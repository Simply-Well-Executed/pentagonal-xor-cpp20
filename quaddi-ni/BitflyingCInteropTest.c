/* SPDX-License-Identifier: 0BSD
 * BITFLYING C11 / C++20 positive shared-language test.
 * Compile identical source/header with /TC /std:c11 and /TP /std:c++20 /O2.
 * Shared valid subset != identical languages or hidden compiler mergers.
 */
#include "../shared/bitflying_c_abi_001.h"
#include <stdio.h>
int main(void){
 const bf_result a=bf_nbit(9ULL,8u,42u);
 const bf_result b=bf_nbit(3ULL,8u,42u);
 const bf_result zero=bf_nbit(0ULL,0u,42u);
 const bf_result one=bf_nbit(1ULL,1u,42u);
 if(a.status!=BF_PASS||b.status!=BF_PASS||
    zero.status!=BF_PASS||one.status!=BF_PASS)return 1;
 if(bf_nbit(1ULL,0u,42u).status!=BF_REJECT_LOSS||
    bf_nbit(2ULL,1u,42u).status!=BF_REJECT_LOSS||
    bf_nbit(9ULL,3u,42u).status!=BF_REJECT_LOSS||
    bf_nbit(1ULL,65u,42u).status!=BF_REJECT_WIDTH)return 2;
 const bf_result plus=bf_plus(a.result,b.result);
 const bf_result minus=bf_minus(a.result,b.result);
 const bf_result band=bf_bitand(a.result,b.result);
 const bf_result complement=bf_complement(a.result);
 if(plus.status!=BF_PASS||plus.result.value!=12ULL||
    minus.status!=BF_PASS||minus.result.value!=6ULL||
    band.status!=BF_PASS||band.result.value!=1ULL||
    complement.status!=BF_PASS||complement.result.value!=246ULL)return 3;
 if(!bf_equal(bf_nop(a.result).result,a.result)||
    !bf_equal(bf_link(a.result,16u).result,
       bf_nbit(9ULL,16u,42u).result)||
    bf_link(a.result,3u).status!=BF_REJECT_LOSS||
    !bf_tilde_equal_experimental(complement.result,a.result))
  return 4;
 if(bf_minus(b.result,a.result).status!=BF_REJECT_UNDERFLOW||
    bf_plus(bf_nbit(255ULL,8u,42u).result,
            bf_nbit(1ULL,8u,42u).result).status!=BF_REJECT_LOSS)
  return 5;
 /* C and C++ agree on these ordinary integer operator results. */
 {
  int x=9,y=3,z=0;
  x+=y; if(x!=12)return 6;
  x-=y; if(x!=9)return 7;
  z=+y; if(z!=3)return 8;
  z=-y; if(z!=-3)return 9;
  z=~y; if(z!=(~3))return 10;
  if(!((x==9)&&(x&y)==1))return 11;
 }
 printf("BITFLYING_C_ABI|SHARED_C11_CPP20=PASS|NBIT=PASS|NOBIT=PASS|NOP=PASS|PLUS_MINUS=PASS|ASSIGN_SIGNS=PASS|AND_EQUAL_COMPLEMENT=PASS|NO_SILENT_LOSS=PASS|RELEASE=HOLD\n");
 return 0;
}
