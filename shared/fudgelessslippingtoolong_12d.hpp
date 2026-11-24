// SPDX-License-Identifier: 0BSD
// FUDGELESSSLIPPINGTOOLONG-12D: test-only 1D..12D decimal precision ladder.
// No edits to canonical e, pi, Q62, Sigma or prior historical witnesses.
// Headerless strict C++20, owned software limbs, not hardware dimensions.
// LEVEL_9 is 9 decimal digits per chunk, NOT a proven performance speedup.
#ifndef SWE_FUDGELESSSLIPPINGTOOLONG_12D_HPP
#define SWE_FUDGELESSSLIPPINGTOOLONG_12D_HPP
#ifndef SWE_SLIPSLIDINGTOOLONG_HPP
#error "include slipslidingtoolong.hpp first"
#endif
namespace swe::fudgelessslippingtoolong_12d {
using u32 = unsigned int;
using u64 = unsigned long long;
using swe::slipslidingtoolong::Register;
using swe::slipslidingtoolong::Result;
using swe::slipslidingtoolong::make_register;
using swe::slipslidingtoolong::GUARDMEMORYGUARD;
using swe::slipslidingtoolong::checksum;
using swe::slipslidingtoolong::same;
inline constexpr unsigned STAGES = 12u;
inline constexpr unsigned LEVEL_9 = 9u;
inline constexpr u32 RADIX9 = 1000000000u;
inline constexpr u32 BLOCK9 = 123456789u;
inline constexpr u32 HALF9 = 500000000u;
inline constexpr unsigned BITS[STAGES]{
 24u,54u,84u,114u,144u,174u,203u,233u,263u,293u,323u,353u
};
inline constexpr unsigned MIN_WIDTH[STAGES]{
 32u,64u,128u,128u,192u,192u,224u,256u,384u,384u,384u,384u
};

// Preserve this exact source-text specimen, including its newline and space.
// It MUST NOT be executed as a memory/scheduler barrier or timing optimization.
// No invented wait()/pause() overloads, sleep, busy-spin or pointer race.
inline constexpr char PRE_SHIFT_WHITESPACE[] = "wait()\n (wait)\npause()";
[[nodiscard]] constexpr bool whitespace_witness() noexcept {
 const char* pointer = PRE_SHIFT_WHITESPACE;
 if(pointer[6]!='\n'||pointer[7]!=' '||pointer[8]!='('||
    pointer[14]!='\n'||pointer[15]!='p'||pointer[20]!=')'||
    pointer[21]!='\0')return false;
 const u32 shifted = static_cast<u32>(pointer[0]) >> 1u;
 return shifted==(static_cast<u32>(pointer[0]) / 2u);
}
static_assert(whitespace_witness());

struct Checked {Register value;bool valid;};
// Each partial product <= (2^32-1)*10^9 + (10^9-1) < 2^64.
// Shift-out carry is propagated, never silently discarded or rounded.
[[nodiscard]] constexpr Checked append(
 const Register& input,u32 group,unsigned digits) noexcept {
 if(!GUARDMEMORYGUARD(input)||input.bits!=512u||
    digits<1u||digits>9u)return {make_register(512u),false};
 u64 radix=1ULL;
 for(unsigned j=0;j<digits;++j)radix*=10ULL;
 if(static_cast<u64>(group)>=radix)return {make_register(512u),false};
 Register out=make_register(512u,input.tag);
 u64 carry=group;
 for(unsigned i=0;i<16u;++i){
  const u64 p=static_cast<u64>(input.limb[i])*radix+carry;
  out.limb[i]=static_cast<u32>(p);
  carry=p>>32u;
 }
 if(carry)return {make_register(512u),false};
 out.digest=checksum(out);
 return {out,GUARDMEMORYGUARD(out)};
}
[[nodiscard]] constexpr Checked fast9(const Register& in,u32 value) noexcept {
 return append(in,value,LEVEL_9);
}
[[nodiscard]] constexpr Checked slow9(const Register& in,u32 value) noexcept {
 if(value>=RADIX9||!GUARDMEMORYGUARD(in)||in.bits!=512u)
  return {make_register(512u),false};
 Register cur=in;
 u32 divisor=100000000u;
 for(unsigned j=0;j<9u;++j){
  const u32 digit=value/divisor;
  value%=divisor;
  if(divisor>1u)divisor/=10u;
  const Checked next=append(cur,digit,1u);
  if(!next.valid)return next;
  cur=next.value;
 }
 return {cur,true};
}
// Exact inverse subtraction: T=N-H*10^9. Quotient and remainder are
// computed by full 512-bit long division; verified by rebuilding N.
struct HeadTail {Register head;u32 tail;bool valid;};
[[nodiscard]] constexpr HeadTail split9(const Register& n) noexcept {
 if(!GUARDMEMORYGUARD(n)||n.bits!=512u)
  return {make_register(512u),0u,false};
 Register h=make_register(512u,n.tag);
 u64 rem=0ULL;
 for(unsigned i=16u;i>0u;--i){
  const u64 step=(rem<<32u)|static_cast<u64>(n.limb[i-1u]);
  h.limb[i-1u]=static_cast<u32>(step/RADIX9);
  rem=step%RADIX9;
 }
 h.digest=checksum(h);
 const Checked inverse=fast9(h,static_cast<u32>(rem));
 if(!inverse.valid||!same(inverse.value,n))
  return {make_register(512u),0u,false};
 return {h,static_cast<u32>(rem),true};
}
[[nodiscard]] constexpr Checked increment(const Register& input) noexcept {
 if(!GUARDMEMORYGUARD(input)||input.bits!=512u)
  return {make_register(512u),false};
 Register r=input;
 u64 carry=1ULL;
 for(unsigned i=0;i<16u;++i){
  const u64 sum=static_cast<u64>(r.limb[i])+carry;
  r.limb[i]=static_cast<u32>(sum);
  carry=sum>>32u;
 }
 if(carry)return {make_register(512u),false};
 r.digest=checksum(r);
 return {r,true};
}
struct Rounded {
 Register exact,head,rounded;
 u32 tail,residual_magnitude;
 bool valid,up,halfway,residual_positive;
};
[[nodiscard]] constexpr Rounded round_first_odd(const Register& n) noexcept {
 const HeadTail ht=split9(n);
 if(!ht.valid)return {};
 const bool tie=(ht.tail==HALF9);
 const bool up=ht.tail>HALF9||(tie&&((ht.head.limb[0]&1u)==0u));
 Register result=ht.head;
 if(up){
  const Checked shifted=increment(result);
  if(!shifted.valid)return {};
  result=shifted.value;
 }
 return {n,ht.head,result,ht.tail,
         static_cast<u32>(up?RADIX9-ht.tail:ht.tail),
         true,up,tie,up};
}
[[nodiscard]] constexpr unsigned bit_length(const Register& n) noexcept {
 for(unsigned i=16u;i>0u;--i){
  const u32 limb=n.limb[i-1u];
  if(limb)for(unsigned bit=32u;bit>0u;--bit)
   if((limb>>(bit-1u))&1u)return (i-1u)*32u+bit;
 }
 return 0u;
}
[[nodiscard]] constexpr bool projections(const Register& n,unsigned minimum) noexcept {
 using namespace swe::slipslidingtoolong;
 for(unsigned i=0;i<WIDTH_COUNT;++i){
  const unsigned width=WIDTHS[i];
  const Projection down=GLINTING(n,width);
  if(width<minimum){
   if(down.status!=Result::REJECT_LOSS)return false;
  }else{
   if(down.status!=Result::PASS)return false;
   const Projection back=GLIZZING(down.value,512u);
   if(back.status!=Result::PASS||!same(back.value,n))return false;
  }
 }
 return true;
}
[[nodiscard]] constexpr bool midpoint_cases(const Register& head) noexcept {
 constexpr u32 tails[]{0u,123456789u,499999999u,500000000u,
                         500000001u,987654321u,999999999u};
 for(unsigned i=0;i<7u;++i){
  const u32 tail=tails[i];
  const Checked n=fast9(head,tail);
  if(!n.valid)return false;
  const Rounded r=round_first_odd(n.value);
  if(!r.valid||r.tail!=tail||!same(r.head,head))return false;
  const bool expected_up=(tail>HALF9)||
             (tail==HALF9&&((head.limb[0]&1u)==0u));
  if(r.up!=expected_up||r.halfway!=(tail==HALF9)||
     r.residual_magnitude!=(expected_up?RADIX9-tail:tail))
   return false;
  if(r.residual_positive!=expected_up)return false;
  if(!expected_up&&!same(r.rounded,head))return false;
  if(expected_up){
   const Checked next=increment(head);
   if(!next.valid||!same(r.rounded,next.value))return false;
  }
 }
 return true;
}
// User's exact seed: "1.2345678". Each subsequent dimension APPENDS
// "123456789" rather than prepending or interpolating missing digits.
// The integer numerator is always paired with its declared decimal scale.
[[nodiscard]] constexpr Checked seed() noexcept {
 Register n=make_register(512u,91u);
 n.limb[0]=12345678u;
 n.digest=checksum(n);
 return {n,GUARDMEMORYGUARD(n)};
}
[[nodiscard]] constexpr bool literal_3d_matches() noexcept {
 constexpr char literal[]="1.2345678123456789123456789";
 Register parsed=make_register(512u,91u);
 for(unsigned i=0;literal[i];++i){
  const char c=literal[i];
  if(c=='.')continue;
  if(c<'0'||c>'9')return false;
  const Checked next=append(parsed,static_cast<u32>(c-'0'),1u);
  if(!next.valid)return false;
  parsed=next.value;
 }
 const Checked a=seed();
 if(!a.valid)return false;
 const Checked b=fast9(a.value,BLOCK9);
 if(!b.valid)return false;
 const Checked c=fast9(b.value,BLOCK9);
 return c.valid&&same(parsed,c.value);
}
[[nodiscard]] constexpr bool ladder_all_12() noexcept {
 if(!literal_3d_matches()||!whitespace_witness())return false;
 const Checked first=seed();
 if(!first.valid)return false;
 Register running=first.value;
 for(unsigned d=0;d<STAGES;++d){
  if(d>0u){
   const Checked fast=fast9(running,BLOCK9);
   const Checked slow=slow9(running,BLOCK9);
   if(!fast.valid||!slow.valid||!same(fast.value,slow.value))
    return false;
   running=fast.value;
  }
  if(bit_length(running)!=BITS[d]||
     !projections(running,MIN_WIDTH[d])||
     !midpoint_cases(running))return false;
  const Rounded check=round_first_odd(running);
  if(!check.valid||check.up||check.halfway)return false;
 }
 return true;
}
inline constexpr bool SERIES_12_PASS=ladder_all_12();
static_assert(SERIES_12_PASS,"12D decimal series/head-to-head/ties-odd failed");
} // namespace swe::fudgelessslippingtoolong_12d
#endif
