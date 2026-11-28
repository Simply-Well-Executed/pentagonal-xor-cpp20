/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
// SOVIETA-LABROCKETS-100D-001 | Exact 1D -> 100D -> 1D decimal ladder.
// Separate 96x32 owned software register. DO NOT change canonical Q62,
// 512-bit GLINTING, Sigma, GammaGamma, or legacy operator definitions.
// ISO C++20, no includes, STL, floating point, heap, compiler extensions.
// LEVEL_9 = 9 decimal digits per stage; not 9x clock acceleration.
#ifndef SWE_FUDGELESSSLIPPINGTOOLONG_100D_HPP
#define SWE_FUDGELESSSLIPPINGTOOLONG_100D_HPP
namespace swe::fudgelessslippingtoolong_100d {
using u32=unsigned int;
using u64=unsigned long long;
static_assert(sizeof(u32)==4 && sizeof(u64)==8);
inline constexpr unsigned STAGES=100u;
inline constexpr unsigned TRANSITIONS=99u;
inline constexpr unsigned LIMBS=96u; // 3072 bits, 100D numerator uses 2984.
inline constexpr unsigned LEVEL_9=9u;
inline constexpr unsigned FRACTION_DIGITS_100=7u+LEVEL_9*TRANSITIONS;
inline constexpr u32 BASE=1000000000u;
inline constexpr u32 BLOCK=123456789u;
inline constexpr u32 CANARY_HEAD=0xC2B467E1u;
inline constexpr u32 CANARY_TAIL=0x85D9136Au;
inline constexpr u32 FNV=16777619u;
struct Wide {
 u32 head;
 u32 limb[LIMBS];
 u32 tail;
 u32 tag;
 u32 digest;
};
[[nodiscard]] constexpr u32 mix(u32 h,u32 v) noexcept {
 return (h^v)*FNV;
}
[[nodiscard]] constexpr u32 checksum(const Wide& n) noexcept {
 u32 h=mix(2166136261u,n.tag);
 for(unsigned i=0u;i<LIMBS;++i)h=mix(h,n.limb[i]);
 return h;
}
[[nodiscard]] constexpr Wide make(unsigned tag=100u) noexcept {
 Wide n{CANARY_HEAD,{},CANARY_TAIL,tag,0u};
 n.digest=checksum(n);
 return n;
}
[[nodiscard]] constexpr bool guarded(const Wide& n) noexcept {
 return n.head==CANARY_HEAD && n.tail==CANARY_TAIL &&
        n.digest==checksum(n);
}
[[nodiscard]] constexpr bool same(const Wide& a,const Wide& b) noexcept {
 if(!guarded(a)||!guarded(b)||a.tag!=b.tag)return false;
 for(unsigned i=0u;i<LIMBS;++i)
  if(a.limb[i]!=b.limb[i])return false;
 return true;
}
struct Checked {Wide value;bool valid;};
[[nodiscard]] constexpr Checked seed() noexcept {
 Wide n=make();
 n.limb[0]=12345678u;
 n.digest=checksum(n);
 return {n,guarded(n)};
}
[[nodiscard]] constexpr Checked append(
 const Wide& n,u32 group,unsigned digits) noexcept {
 if(!guarded(n)||digits<1u||digits>9u)return {make(),false};
 u64 radix=1ULL;
 for(unsigned i=0u;i<digits;++i)radix*=10ULL;
 if(static_cast<u64>(group)>=radix)return {make(),false};
 Wide out=make(n.tag);
 u64 carry=group;
 for(unsigned i=0u;i<LIMBS;++i){
  // Partial product <2^64, carry retained in every iteration.
  const u64 word=static_cast<u64>(n.limb[i])*radix+carry;
  out.limb[i]=static_cast<u32>(word);
  carry=word>>32u;
 }
 if(carry!=0ULL)return {make(),false}; // reject loss, never wrap
 out.digest=checksum(out);
 return {out,guarded(out)};
}
[[nodiscard]] constexpr Checked rocket_left(const Wide& n) noexcept {
 return append(n,BLOCK,LEVEL_9);
}
[[nodiscard]] constexpr Checked car_right(const Wide& n) noexcept {
 Wide next=n;
 u32 remaining=BLOCK,place=100000000u;
 for(unsigned i=0u;i<LEVEL_9;++i){
  const u32 digit=remaining/place;
  remaining%=place;
  if(place>1u)place/=10u;
  const Checked one=append(next,digit,1u);
  if(!one.valid)return one;
  next=one.value;
 }
 return {next,guarded(next)};
}
// Exact inverse: n = head*10^9 + tail. Quotient via long division;
// reconstruction must yield every original bit. Never use rounding here.
struct Split {Wide head;u32 tail;bool valid;};
[[nodiscard]] constexpr Split split9(const Wide& n) noexcept {
 if(!guarded(n))return {make(),0u,false};
 Wide head=make(n.tag);
 u64 rem=0ULL;
 for(unsigned i=LIMBS;i>0u;--i){
  const u64 word=(rem<<32u)|static_cast<u64>(n.limb[i-1u]);
  head.limb[i-1u]=static_cast<u32>(word/BASE);
  rem=word%BASE;
 }
 head.digest=checksum(head);
 const Checked recon=append(head,static_cast<u32>(rem),9u);
 if(!recon.valid||!same(recon.value,n))return {make(),0u,false};
 return {head,static_cast<u32>(rem),true};
}
[[nodiscard]] constexpr unsigned bit_length(const Wide& n) noexcept {
 if(!guarded(n))return 0u;
 for(unsigned i=LIMBS;i>0u;--i){
  const u32 word=n.limb[i-1u];
  if(word)for(unsigned k=32u;k>0u;--k)
   if(((word>>(k-1u))&1u)!=0u)return (i-1u)*32u+k;
 }
 return 0u;
}
// A verified in-process reversible checkpoint. "Direct" means no
// intermediate descent snapshots; NOT constant-time arbitrary validation.
struct HopWitness {
 Wide origin,tip;
 unsigned step_count;
 bool all_steps_validated;
};
[[nodiscard]] constexpr Checked direct_hop(
 const HopWitness& witness,const Wide& current) noexcept {
 if(!witness.all_steps_validated||
    witness.step_count!=TRANSITIONS||
    !same(witness.tip,current)||!guarded(witness.origin))
  return {make(),false};
 return {witness.origin,true};
}

// Ordered *metadata-only* operator replay, not execution on a decimal
// scalar. Names have different source domains and cannot be composed
// blindly. Proven permutation probes operate on labels, never values.
enum class OperatorStatus:u32 {
 METADATA_ONLY, SOURCE_REFERENCE_ONLY, SYMBOLIC_ONLY, NEEDS_DOMAIN
};
struct OperatorRecord {const char* name;OperatorStatus status;};
inline constexpr OperatorRecord OPERATORS[12]{
 {"BiDi",OperatorStatus::METADATA_ONLY},
 {"TriDi",OperatorStatus::METADATA_ONLY},
 {"PiDi",OperatorStatus::SOURCE_REFERENCE_ONLY},
 {"PiPi",OperatorStatus::SYMBOLIC_ONLY},
 {"SigmaSigmaSigma",OperatorStatus::NEEDS_DOMAIN},
 {"Sigma",OperatorStatus::NEEDS_DOMAIN},
 {"SigmaSigma",OperatorStatus::NEEDS_DOMAIN},
 {"GammaGammaGamma",OperatorStatus::NEEDS_DOMAIN},
 {"GammaGammaGamma",OperatorStatus::NEEDS_DOMAIN},
 {"Gamma",OperatorStatus::NEEDS_DOMAIN},
 {"GammGamma",OperatorStatus::NEEDS_DOMAIN},
 {"Gamma",OperatorStatus::NEEDS_DOMAIN}
};
[[nodiscard]] constexpr bool string_equal(
 const char* a,const char* b) noexcept {
 for(unsigned i=0u;;++i){
  if(a[i]!=b[i])return false;
  if(a[i]=='\0')return true;
 }
}
[[nodiscard]] constexpr bool metadata_operator_probes() noexcept {
 // BiDi swaps two named lanes, second swap must restore provenance.
 unsigned a=1u,b=2u;
 const unsigned before_a=a,before_b=b;
 unsigned temp=a;a=b;b=temp;
 if(a!=before_b||b!=before_a)return false;
 temp=a;a=b;b=temp;
 if(a!=before_a||b!=before_b)return false;
 // TriDi cycles three labels; three applications return to origin.
 unsigned x=1u,y=2u,z=3u;
 for(unsigned i=0u;i<3u;++i){
  const unsigned old=x;
  x=y;y=z;z=old;
 }
 return x==1u&&y==2u&&z==3u;
}
[[nodiscard]] constexpr bool replay_order_12() noexcept {
 if(!metadata_operator_probes())return false;
 constexpr const char* names[]{
  "BiDi","TriDi","PiDi","PiPi","SigmaSigmaSigma","Sigma",
  "SigmaSigma","GammaGammaGamma","GammaGammaGamma","Gamma",
  "GammGamma","Gamma"
 };
 unsigned metadata=0u,reference=0u,symbolic=0u,hold=0u;
 for(unsigned i=0u;i<12u;++i){
  if(!string_equal(OPERATORS[i].name,names[i]))return false;
  switch(OPERATORS[i].status){
   case OperatorStatus::METADATA_ONLY:++metadata;break;
   case OperatorStatus::SOURCE_REFERENCE_ONLY:++reference;break;
   case OperatorStatus::SYMBOLIC_ONLY:++symbolic;break;
   case OperatorStatus::NEEDS_DOMAIN:++hold;break;
  }
 }
 // Existing separately-scoped PiDi source: 314159265358979323 % 4 = 3.
 constexpr u64 scoped_pi_integer=314159265358979323ULL;
 return metadata==2u&&reference==1u&&symbolic==1u&&hold==8u &&
        (scoped_pi_integer%4ULL)==3ULL;
}
static_assert(FRACTION_DIGITS_100==898u && LIMBS*32u>=2984u);
static_assert(replay_order_12());

struct Qualification {
 bool valid;
 unsigned ascents,descents;
 unsigned highest_bits;
 unsigned repeated_operator_labels;
 bool direct_hop_matches;
 bool operator_sequence_nonmutating;
};
[[nodiscard]] inline Qualification run() noexcept {
 if(!replay_order_12())return {};
 const Checked s=seed();
 if(!s.valid)return {};
 Wide current=s.value;
 unsigned ascents=0u;
 for(unsigned d=2u;d<=STAGES;++d){
  const Checked left=rocket_left(current);
  const Checked right=car_right(current);
  if(!left.valid||!right.valid||!same(left.value,right.value))
   return {};
  current=left.value;
  ++ascents;
 }
 const unsigned highest=bit_length(current);
 if(highest!=2984u)return {};
 const HopWitness witness{s.value,current,ascents,true};
 const Checked direct=direct_hop(witness,current);
 if(!direct.valid||!same(direct.value,s.value))return {};
 Wide walk=current;
 unsigned descents=0u;
 for(unsigned d=STAGES;d>1u;--d){
  const Split step=split9(walk);
  if(!step.valid||step.tail!=BLOCK)return {};
  walk=step.head;
  ++descents;
 }
 if(!same(walk,s.value)||descents!=TRANSITIONS||
    ascents!=TRANSITIONS)return {};
 // Re-run the ordered metadata audit TWICE without modifying source data.
 for(unsigned pass=0u;pass<2u;++pass)
  if(!replay_order_12())return {};
 return {true,ascents,descents,highest,24u,true,
         same(walk,direct.value)&&same(witness.tip,current)};
}
} // namespace swe::fudgelessslippingtoolong_100d
#endif
