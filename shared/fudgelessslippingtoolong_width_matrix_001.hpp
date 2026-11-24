// SPDX-License-Identifier: 0BSD
// SOVIETA-LABROCKETS-WIDTH-MATRIX-001.
// Exact 1D..100D..1D 32-bit/64-bit limb parity plus ALL ten original
// logical widths (32/64/128/192/224/256/384/448/480/512).
// Includes explicit 512->128 and 512->512 GLINTING witness. No sources
// mutated; no floating point, STL, heap, extensions or unmeasured speed claim.
#ifndef SWE_FUDGELESSSLIPPINGTOOLONG_WIDTH_MATRIX_001_HPP
#define SWE_FUDGELESSSLIPPINGTOOLONG_WIDTH_MATRIX_001_HPP
#ifndef SWE_FUDGELESSSLIPPINGTOOLONG_100D_HPP
#error "Include fudgelessslippingtoolong_100d.hpp first"
#endif
#ifndef SWE_SLIPSLIDINGTOOLONG_HPP
#error "Include slipslidingtoolong.hpp first"
#endif
namespace swe::fudgelessslippingtoolong_width_matrix_001 {
namespace original=swe::slipslidingtoolong;
namespace century=swe::fudgelessslippingtoolong_100d;
using u32=unsigned int;
using u64=unsigned long long;
using original::Register;
using original::Result;
using century::Wide;
inline constexpr unsigned WIDTH_COUNT=original::WIDTH_COUNT;
inline constexpr unsigned BITS[WIDTH_COUNT]{
 32u,64u,128u,192u,224u,256u,384u,448u,480u,512u
};
inline constexpr unsigned EXPECTED_FITS[WIDTH_COUNT]{
 1u,2u,4u,6u,7u,8u,13u,15u,16u,17u
};
inline constexpr unsigned TOTAL_STAGE_WIDTH_CASES=century::STAGES*WIDTH_COUNT;
inline constexpr unsigned EXPECTED_ACCEPTED=89u;
inline constexpr unsigned EXPECTED_REJECTED=911u;
inline constexpr unsigned L64=century::LIMBS/2u;
static_assert(L64==48u && 2u*L64*64u==6144u);
static_assert(century::LIMBS*32u==3072u);
[[nodiscard]] constexpr bool width_registry_matches() noexcept {
 for(unsigned i=0u;i<WIDTH_COUNT;++i)
  if(BITS[i]!=original::WIDTHS[i])return false;
 return true;
}
static_assert(width_registry_matches());

struct Wide64 {
 u32 head;
 u64 limb[L64];
 u32 tail;
 u32 tag;
 u32 digest;
};
[[nodiscard]] constexpr u32 checksum64(const Wide64& w) noexcept {
 u32 h=century::mix(2166136261u,w.tag);
 for(unsigned i=0u;i<L64;++i){
  h=century::mix(h,static_cast<u32>(w.limb[i]));
  h=century::mix(h,static_cast<u32>(w.limb[i]>>32u));
 }
 return h;
}
[[nodiscard]] constexpr Wide64 make64(u32 tag=100u) noexcept {
 Wide64 w{century::CANARY_HEAD,{},century::CANARY_TAIL,tag,0u};
 w.digest=checksum64(w);
 return w;
}
[[nodiscard]] constexpr bool guarded64(const Wide64& w) noexcept {
 return w.head==century::CANARY_HEAD &&
        w.tail==century::CANARY_TAIL && checksum64(w)==w.digest;
}
[[nodiscard]] constexpr bool same64(const Wide64& a,const Wide64& b) noexcept {
 if(!guarded64(a)||!guarded64(b)||a.tag!=b.tag)return false;
 for(unsigned i=0u;i<L64;++i)if(a.limb[i]!=b.limb[i])return false;
 return true;
}
struct Checked64 {Wide64 value;bool valid;};
[[nodiscard]] constexpr Checked64 pack64(const Wide& n) noexcept {
 if(!century::guarded(n))return {make64(),false};
 Wide64 w=make64(n.tag);
 for(unsigned i=0u;i<L64;++i)
  w.limb[i]=static_cast<u64>(n.limb[i*2u])|
            (static_cast<u64>(n.limb[i*2u+1u])<<32u);
 w.digest=checksum64(w);
 return {w,guarded64(w)};
}
[[nodiscard]] constexpr century::Checked unpack64(const Wide64& w) noexcept {
 if(!guarded64(w))return {century::make(),false};
 Wide n=century::make(w.tag);
 for(unsigned i=0u;i<L64;++i){
  n.limb[i*2u]=static_cast<u32>(w.limb[i]);
  n.limb[i*2u+1u]=static_cast<u32>(w.limb[i]>>32u);
 }
 n.digest=century::checksum(n);
 return {n,century::guarded(n)};
}
// Portable 64-bit limb x 10^digits multiplication: split each word into
// 32-bit halves, keep BOTH carries. No nonstandard unsigned __int128.
[[nodiscard]] constexpr Checked64 append64(
 const Wide64& n,u32 group,unsigned digits) noexcept {
 if(!guarded64(n)||digits<1u||digits>9u)
  return {make64(),false};
 u64 radix=1ULL;
 for(unsigned i=0u;i<digits;++i)radix*=10ULL;
 if(static_cast<u64>(group)>=radix)return {make64(),false};
 Wide64 out=make64(n.tag);
 constexpr u64 mask=0xffffffffULL;
 u64 carry=group;
 for(unsigned i=0u;i<L64;++i){
  const u64 lo=(n.limb[i]&mask)*radix+carry;
  const u64 hi=(n.limb[i]>>32u)*radix+(lo>>32u);
  out.limb[i]=(lo&mask)|((hi&mask)<<32u);
  carry=hi>>32u;
 }
 if(carry!=0ULL)return {make64(),false};
 out.digest=checksum64(out);
 return {out,guarded64(out)};
}
[[nodiscard]] constexpr Checked64 rocket_left64(const Wide64& n) noexcept {
 return append64(n,century::BLOCK,9u);
}
[[nodiscard]] constexpr Checked64 car_right64(const Wide64& n) noexcept {
 Wide64 cur=n;
 u32 group=century::BLOCK,place=100000000u;
 for(unsigned i=0u;i<9u;++i){
  const u32 digit=group/place;
  group%=place;
  if(place>1u)place/=10u;
  const Checked64 next=append64(cur,digit,1u);
  if(!next.valid)return next;
  cur=next.value;
 }
 return {cur,guarded64(cur)};
}
struct Narrow {
 Register value;
 Result status;
};
// Source remains fully 3072 bits; no width silently assumes 512 bits.
[[nodiscard]] constexpr Narrow from32(
 const Wide& n,unsigned bits) noexcept {
 if(!original::allowed(bits))return {original::make_register(32u),Result::REJECT_WIDTH};
 if(!century::guarded(n))return {original::make_register(bits),Result::REJECT_GUARD};
 const unsigned count=bits/32u;
 for(unsigned i=count;i<century::LIMBS;++i)
  if(n.limb[i]!=0u)return {original::make_register(bits),Result::REJECT_LOSS};
 Register out=original::make_register(bits,n.tag);
 for(unsigned i=0u;i<count;++i)out.limb[i]=n.limb[i];
 out.digest=original::checksum(out);
 return {out,Result::PASS};
}
// Independently inspect EVERY source half-word of 64-bit-limb storage.
// 32/224/480 use a half-filled final 64-bit lane; padding MUST be zero.
[[nodiscard]] constexpr Narrow from64(
 const Wide64& n,unsigned bits) noexcept {
 if(!original::allowed(bits))return {original::make_register(32u),Result::REJECT_WIDTH};
 if(!guarded64(n))return {original::make_register(bits),Result::REJECT_GUARD};
 const unsigned count=bits/32u;
 for(unsigned i=count;i<2u*L64;++i){
  const u64 word=n.limb[i/2u];
  const u32 half=static_cast<u32>((i%2u)?word>>32u:word);
  if(half!=0u)return {original::make_register(bits),Result::REJECT_LOSS};
 }
 Register out=original::make_register(bits,n.tag);
 for(unsigned i=0u;i<count;++i){
  const u64 word=n.limb[i/2u];
  out.limb[i]=static_cast<u32>((i%2u)?word>>32u:word);
 }
 out.digest=original::checksum(out);
 return {out,Result::PASS};
}
[[nodiscard]] constexpr bool reconstruct(
 const Wide& original_value,const Register& narrow) noexcept {
 if(!century::guarded(original_value)||
    !original::GUARDMEMORYGUARD(narrow))return false;
 Wide exact=century::make(original_value.tag);
 for(unsigned i=0u;i<narrow.bits/32u;++i)
  exact.limb[i]=narrow.limb[i];
 exact.digest=century::checksum(exact);
 return century::same(original_value,exact);
}
[[nodiscard]] constexpr bool zero_512_baseline_and_512_to_128() noexcept {
 const auto a=century::seed();
 if(!a.valid)return false;
 Wide n=a.value;
 // Stage 4 fits 128; stage 5 does not. Stage 17 fits 512;
 // stage 18 crosses the 512-bit baseline.
 for(unsigned d=1u;d<=18u;++d){
  if(d>1u){
   const auto next=century::rocket_left(n);
   if(!next.valid)return false;
   n=next.value;
  }
  const Narrow base=from32(n,512u);
  if(base.status!=(d<=17u?Result::PASS:Result::REJECT_LOSS))
   return false;
  if(d<=17u){
   const auto equal=original::project(base.value,512u);
   if(equal.status!=Result::PASS||
      !original::same(equal.value,base.value))return false;
   const auto down=original::GLINTING(base.value,128u);
   if(d<=4u){
    if(down.status!=Result::PASS ||
       !reconstruct(n,down.value))return false;
    const auto back=original::GLIZZING(down.value,512u);
    if(back.status!=Result::PASS||
       !original::same(back.value,base.value))return false;
   }else if(down.status!=Result::REJECT_LOSS)return false;
  }
 }
 return true;
}
struct Audit {
 bool valid;
 unsigned widths;
 unsigned routes;
 unsigned checked_stages;
 unsigned accepted;
 unsigned rejected;
 unsigned rocket_matches;
 unsigned 64_roundtrips;
 unsigned highest_bits;
 bool baseline_512_and_512_to_128;
 bool exact_roundtrip;
};
// Execute dynamic tests, rather than 100*10*multiple static_assert loop
// allocations. All results are checked, no default PASS.
[[nodiscard]] inline Audit run() noexcept {
 if(!width_registry_matches() ||
    !zero_512_baseline_and_512_to_128())return {};
 const auto original_seed=century::seed();
 if(!original_seed.valid)return {};
 const Checked64 first64=pack64(original_seed.value);
 if(!first64.valid)return {};
 Wide current32=original_seed.value;
 Wide64 current64=first64.value;
 unsigned accepted=0u,rejected=0u,matches=0u,roundtrips=0u;
 unsigned fit[WIDTH_COUNT]{};
 for(unsigned d=1u;d<=century::STAGES;++d){
  if(d>1u){
   const auto r32=century::rocket_left(current32);
   const auto c32=century::car_right(current32);
   const Checked64 r64=rocket_left64(current64);
   const Checked64 c64=car_right64(current64);
   if(!r32.valid||!c32.valid||!r64.valid||!c64.valid||
      !century::same(r32.value,c32.value)||
      !same64(r64.value,c64.value))return {};
   current32=r32.value;
   current64=r64.value;
   ++matches;
  }
  const auto reconstructed=unpack64(current64);
  if(!reconstructed.valid||
     !century::same(reconstructed.value,current32))return {};
  ++roundtrips;
  const unsigned needed=century::bit_length(current32);
  for(unsigned j=0u;j<WIDTH_COUNT;++j){
   const unsigned width=BITS[j];
   const Narrow left=from32(current32,width);
   const Narrow right=from64(current64,width);
   const bool fits=needed<=width;
   if(fits){
    if(left.status!=Result::PASS||
       right.status!=Result::PASS||
       !original::same(left.value,right.value)||
       !reconstruct(current32,left.value))return {};
    const auto up=original::GLIZZING(left.value,512u);
    if(up.status!=Result::PASS)return {};
    const auto down=original::GLINTING(up.value,width);
    if(down.status!=Result::PASS||
       !original::same(down.value,left.value))return {};
    ++accepted;++fit[j];
   }else{
    if(left.status!=Result::REJECT_LOSS||
       right.status!=Result::REJECT_LOSS)return {};
    ++rejected;
   }
  }
 }
 if(matches!=century::TRANSITIONS||
    accepted!=EXPECTED_ACCEPTED||
    rejected!=EXPECTED_REJECTED||
    roundtrips!=century::STAGES ||
    century::bit_length(current32)!=2984u)return {};
 for(unsigned j=0u;j<WIDTH_COUNT;++j)
  if(fit[j]!=EXPECTED_FITS[j])return {};
 // After the full width matrix, retain original 100D inverse witnesses.
 Wide walker=current32;
 for(unsigned d=century::STAGES;d>1u;--d){
  const auto step=century::split9(walker);
  if(!step.valid||step.tail!=century::BLOCK)return {};
  walker=step.head;
 }
 if(!century::same(walker,original_seed.value))return {};
 return {true,WIDTH_COUNT,WIDTH_COUNT*WIDTH_COUNT,century::STAGES,
         accepted,rejected,matches,roundtrips,2984u,true,true};
}
} // namespace swe::fudgelessslippingtoolong_width_matrix_001
#endif
