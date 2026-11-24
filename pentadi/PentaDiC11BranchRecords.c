/* SPDX-License-Identifier: 0BSD
 * PENTADI-001 | Fifth lane's C11 branch-record witness.
 * Feed strictly validated pipe records to Delphi 7 Object Pascal.
 * /TC /std:c11 /O2 /FA /Fa produces a native C11 .asm listing.
 * The listing is produced by MSVC, NEVER mislabeled Delphi output.
 */
#include "../shared/bitflying_c_abi_001.h"
#include <stdio.h>
#include <stdint.h>

typedef struct {
 unsigned pass, id, line, width, next, target;
 const char *label;
 const char *encoding;
 int expected_fits;
} BranchRecord;

static const BranchRecord RECORDS[]={
 {1u,7u,10u,2u,1000u,1070u,"LOOP7","SHORT",1},
 {2u,7u,10u,5u,1000u,1070u,"LOOP7","LONG",1},
 {3u,7u,10u,2u,1000u,1070u,"LOOP7","SHORT",1},
 {1u,8u,20u,2u,100u,500u,"OVER8","SHORT",0},
 {1u,9u,21u,2u,100u,227u,"PLUS127","SHORT",1},
 {1u,10u,22u,2u,228u,100u,"MINUS128","SHORT",1},
 {1u,11u,23u,2u,100u,228u,"PLUS128","SHORT",0},
 {1u,12u,24u,2u,229u,100u,"MINUS129","SHORT",0},
 {1u,13u,25u,2u,65530u,3u,"NO_WRAP","SHORT",0}
};
int main(void){
 unsigned i;
 const unsigned count=(unsigned)(sizeof RECORDS/sizeof RECORDS[0]);
 const bf_result guard=bf_nbit(65535ULL,16u,95u);
 if(guard.status!=BF_PASS || bf_nop(guard.result).status!=BF_PASS)
  return 10;
 for(i=0u;i<count;++i){
  const BranchRecord r=RECORDS[i];
  const int64_t diff=(int64_t)r.target-(int64_t)r.next;
  const int fits=(diff>=-128 && diff<=127);
  if(r.next>65535u || r.target>65535u ||
     r.pass>65535u || r.id>65535u || r.line>65535u ||
     r.width==0u || r.width>65535u ||
     r.expected_fits!=fits) return 20;
  if(printf("%u|%u|%u|%s|%s|%u|%u|%u\n",
            r.pass,r.id,r.line,r.label,r.encoding,
            r.width,r.next,r.target)<0) return 30;
 }
 return 0;
}
