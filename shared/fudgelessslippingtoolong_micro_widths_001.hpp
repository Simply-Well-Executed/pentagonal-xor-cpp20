/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
// SOVIETA-LABROCKETS-MICRO-WIDTHS-001: 0/1/2/4/8/16 + 10 established
// widths. NOBIT=zero-bit data capacity; NOP=identity, NOT a bit width.
// Exact projections only: no silently clipped bits or implicit rounding.
// Own 3072-bit source and sealed views; do NOT alter original core registry.
// Strict ISO C++20, no includes, STL, floats, heap, extensions or timing claims.
#ifndef SWE_FUDGELESSSLIPPINGTOOLONG_MICRO_WIDTHS_001_HPP
#define SWE_FUDGELESSSLIPPINGTOOLONG_MICRO_WIDTHS_001_HPP
#ifndef SWE_FUDGELESSSLIPPINGTOOLONG_WIDTH_MATRIX_001_HPP
#error "Include fudgelessslippingtoolong_width_matrix_001.hpp first"
#endif
namespace swe::fudgelessslippingtoolong_micro_widths_001 {
namespace century=swe::fudgelessslippingtoolong_100d;
namespace previous=swe::fudgelessslippingtoolong_width_matrix_001;
namespace core=swe::slipslidingtoolong;
using century::Wide;
using u32=unsigned int;
using u64=unsigned long long;
using core::Result;
inline constexpr unsigned WIDTHS[16]{
 0u,1u,2u,4u,8u,16u,32u,64u,
 128u,192u,224u,256u,384u,448u,480u,512u
};
inline constexpr unsigned WIDTH_COUNT=16u;
inline constexpr unsigned EXPECTED_ACCEPTED=89u;
inline constexpr unsigned EXPECTED_REJECTED=1511u;
inline constexpr unsigned EXPECTED_ROUTES_PASS=136u;
inline constexpr unsigned EXPECTED_ROUTES_REJECT=120u;
inline constexpr unsigned EXPECTED_NOP_STAGES=100u;
static_assert(WIDTH_COUNT*century::STAGES==1600u);
[[nodiscard]] constexpr bool allowed(unsigned bits) noexcept {
 for(unsigned i=0;i<WIDTH_COUNT;++i)
  if(WIDTHS[i]==bits)return true;
 return false;
}
[[nodiscard]] constexpr bool matches_core() noexcept {
 for(unsigned i=0;i<core::WIDTH_COUNT;++i)
  if(WIDTHS[i+6u]!=core::WIDTHS[i])return false;
 return WIDTHS[0]==0u&&WIDTHS[1]==1u&&WIDTHS[2]==2u&&
        WIDTHS[3]==4u&&WIDTHS[4]==8u&&WIDTHS[5]==16u;
}
static_assert(matches_core());
[[nodiscard]] constexpr bool fits32(const Wide& n,unsigned bits) noexcept {
 if(!century::guarded(n)||!allowed(bits))return false;
 const unsigned complete=bits/32u;
 const unsigned remainder=bits%32u;
 for(unsigned i=0u;i<century::LIMBS;++i){
  const u32 word=n.limb[i];
  if(i<complete)continue;
  if(i==complete&&remainder!=0u){
   if((word>>remainder)!=0u)return false;
  }else if(word!=0u)return false;
 }
 return true;
}
// Independent bit boundary test of the 48x64-bit representation.
// Never shift by 64. A 0-bit register can store ONLY zero.
[[nodiscard]] constexpr bool fits64(
 const previous::Wide64& n,unsigned bits) noexcept {
 if(!previous::guarded64(n)||!allowed(bits))return false;
 const unsigned complete=bits/64u;
 const unsigned remainder=bits%64u;
 for(unsigned i=0u;i<previous::L64;++i){
  const u64 word=n.limb[i];
  if(i<complete)continue;
  if(i==complete&&remainder!=0u){
   if((word>>remainder)!=0ULL)return false;
  }else if(word!=0ULL)return false;
 }
 return true;
}
struct View {
 Wide value;
 unsigned bits;
 u32 seal;
};
[[nodiscard]] constexpr u32 seal(const Wide& n,unsigned bits) noexcept {
 return century::mix(
   century::mix(2166136261u,static_cast<u32>(bits)),n.digest);
}
[[nodiscard]] constexpr View make_view(const Wide& n,unsigned bits) noexcept {
 return {n,bits,seal(n,bits)};
}
[[nodiscard]] constexpr bool guarded(const View& v) noexcept {
 return century::guarded(v.value)&&allowed(v.bits)&&
        v.seal==seal(v.value,v.bits)&&fits32(v.value,v.bits);
}
struct Projection {View view;Result status;};
[[nodiscard]] constexpr Projection project(
 const Wide& n,unsigned bits) noexcept {
 if(!allowed(bits))return {make_view(century::make(),0u),Result::REJECT_WIDTH};
 if(!century::guarded(n))return {make_view(century::make(),0u),Result::REJECT_GUARD};
 if(!fits32(n,bits))return {make_view(century::make(),0u),Result::REJECT_LOSS};
 const View v=make_view(n,bits);
 return {v,guarded(v)?Result::PASS:Result::REJECT_GUARD};
}
// Direct projection from a sealed source view; no hidden numeric mutation.
[[nodiscard]] constexpr Projection project(
 const View& v,unsigned bits) noexcept {
 if(!allowed(bits))return {make_view(century::make(),0u),Result::REJECT_WIDTH};
 if(!guarded(v))return {make_view(century::make(),0u),Result::REJECT_GUARD};
 return project(v.value,bits);
}
// NOP is an explicit guarded identity action, NOT width=0 and NOT sleep.
[[nodiscard]] constexpr Projection nop(const View& v) noexcept {
 if(!guarded(v))return {make_view(century::make(),0u),Result::REJECT_GUARD};
 return {v,Result::PASS};
}
[[nodiscard]] constexpr century::Checked nop(const Wide& v) noexcept {
 if(!century::guarded(v))return {century::make(),false};
 return {v,true};
}
[[nodiscard]] constexpr bool view_equal(const View& a,const View& b) noexcept {
 return guarded(a)&&guarded(b)&&a.bits==b.bits &&
        a.seal==b.seal&&century::same(a.value,b.value);
}
[[nodiscard]] constexpr Wide number(unsigned bits,u32 tag=100u) noexcept {
 Wide n=century::make(tag);
 if(bits>0u){
  const unsigned index=(bits-1u)/32u;
  const unsigned offset=(bits-1u)%32u;
  n.limb[index]=static_cast<u32>(1u<<offset);
 }
 n.digest=century::checksum(n);
 return n;
}
// Zero, max-value and first-out-of-range controls for all tiny widths.
[[nodiscard]] constexpr bool tiny_controls() noexcept {
 const Wide zero=century::make();
 for(unsigned i=0u;i<WIDTH_COUNT;++i){
  const unsigned bits=WIDTHS[i];
  const Projection z=project(zero,bits);
  if(z.status!=Result::PASS||!guarded(z.view))return false;
  const Projection identity=nop(z.view);
  if(identity.status!=Result::PASS||
     !view_equal(identity.view,z.view))return false;
 }
 const Wide one=number(1u);
 if(project(one,0u).status!=Result::REJECT_LOSS ||
    project(one,1u).status!=Result::PASS)return false;
 for(unsigned i=1u;i<=5u;++i){
  const unsigned bits=WIDTHS[i];
  Wide limit=century::make();
  const u32 max=(static_cast<u32>(1u)<<bits)-1u;
  limit.limb[0]=max;
  limit.digest=century::checksum(limit);
  if(project(limit,bits).status!=Result::PASS)return false;
  Wide next=century::make();
  next.limb[0]=static_cast<u32>(1u<<bits);
  next.digest=century::checksum(next);
  if(project(next,bits).status!=Result::REJECT_LOSS)return false;
 }
 const Projection valid=project(one,1u);
 if(valid.status!=Result::PASS)return false;
 View corrupt=valid.view;
 corrupt.bits=2u; // seal must reject altered width metadata
 if(nop(corrupt).status!=Result::REJECT_GUARD)return false;
 corrupt=valid.view;
 corrupt.value.limb[0]^=1u; // digest must reject payload tampering
 if(nop(corrupt).status!=Result::REJECT_GUARD)return false;
 Wide tampered=one;
 tampered.limb[0]^=1u;
 if(nop(tampered).valid)return false;
 return true;
}
static_assert(tiny_controls());

// Complete source->destination route matrix, with the source's highest
// data bit set. Therefore all strictly smaller widths MUST reject loss,
// while equal/larger destinations MUST preserve the exact source.
struct RouteAudit {
 bool valid;
 unsigned passes,rejections,nops;
};
[[nodiscard]] inline RouteAudit all_routes() noexcept {
 unsigned pass=0u,reject=0u,identities=0u;
 for(unsigned i=0u;i<WIDTH_COUNT;++i){
  const unsigned from=WIDTHS[i];
  const Wide n=number(from);
  const Projection source=project(n,from);
  if(source.status!=Result::PASS)return {};
  const Projection still=nop(source.view);
  if(still.status!=Result::PASS||
     !view_equal(still.view,source.view))return {};
  ++identities;
  for(unsigned j=0u;j<WIDTH_COUNT;++j){
   const unsigned to=WIDTHS[j];
   const Projection converted=project(source.view,to);
   if(to<from){
    if(converted.status!=Result::REJECT_LOSS)return {};
    ++reject;
   }else{
    if(converted.status!=Result::PASS||
       !century::same(converted.view.value,n))return {};
    const Projection back=project(converted.view,from);
    if(back.status!=Result::PASS||
       !view_equal(back.view,source.view))return {};
    ++pass;
   }
  }
 }
 return {pass==EXPECTED_ROUTES_PASS&&
         reject==EXPECTED_ROUTES_REJECT&&identities==WIDTH_COUNT,
         pass,reject,identities};
}
struct Audit {
 bool valid;
 unsigned widths,stage_projections,accepted,rejected;
 unsigned routes,route_passes,route_rejections;
 unsigned nobit_zero_controls,nop_stages;
 bool core_unchanged,roundtrip_100d,limb64_parity;
};
[[nodiscard]] inline Audit run() noexcept {
 if(!matches_core()||!tiny_controls())return {};
 const RouteAudit routes=all_routes();
 if(!routes.valid)return {};
 const auto seed=century::seed();
 if(!seed.valid)return {};
 Wide n=seed.value;
 unsigned fit=0u,reject=0u,nops=0u;
 for(unsigned d=1u;d<=century::STAGES;++d){
  if(d>1u){
   const auto left=century::rocket_left(n);
   const auto right=century::car_right(n);
   if(!left.valid||!right.valid||
      !century::same(left.value,right.value))return {};
   n=left.value;
  }
  const auto same_wide=nop(n);
  if(!same_wide.valid||!century::same(same_wide.value,n))return {};
  ++nops;
  const auto limb64=previous::pack64(n);
  if(!limb64.valid)return {};
  const unsigned bitlen=century::bit_length(n);
  for(unsigned i=0u;i<WIDTH_COUNT;++i){
   const unsigned width=WIDTHS[i];
   const Projection from32=project(n,width);
   const bool from64=fits64(limb64.value,width);
   const bool expected=bitlen<=width;
   if(from64!=expected)return {};
   if(expected){
    if(from32.status!=Result::PASS||!guarded(from32.view)||
       !century::same(from32.view.value,n))return {};
    const Projection preserved=nop(from32.view);
    if(preserved.status!=Result::PASS||
       !view_equal(preserved.view,from32.view))return {};
    // Historical core widths MUST agree with the extension.
    if(core::allowed(width)){
     const auto legacy32=previous::from32(n,width);
     const auto legacy64=previous::from64(limb64.value,width);
     if(legacy32.status!=Result::PASS||
        legacy64.status!=Result::PASS||
        !core::same(legacy32.value,legacy64.value))return {};
    }
    ++fit;
   }else{
    if(from32.status!=Result::REJECT_LOSS)return {};
    if(core::allowed(width)){
     const auto legacy32=previous::from32(n,width);
     const auto legacy64=previous::from64(limb64.value,width);
     if(legacy32.status!=Result::REJECT_LOSS||
        legacy64.status!=Result::REJECT_LOSS)return {};
    }
    ++reject;
   }
  }
 }
 if(fit!=EXPECTED_ACCEPTED||reject!=EXPECTED_REJECTED||
    nops!=EXPECTED_NOP_STAGES||
    century::bit_length(n)!=2984u)return {};
 // Independent descending proof: every 9-digit tail is retained.
 Wide down=n;
 for(unsigned d=century::STAGES;d>1u;--d){
  const auto step=century::split9(down);
  if(!step.valid||step.tail!=century::BLOCK)return {};
  down=step.head;
 }
 if(!century::same(down,seed.value))return {};
 return {true,WIDTH_COUNT,fit+reject,fit,reject,
         routes.passes+routes.rejections,routes.passes,routes.rejections,
         1u,nops,true,true,true};
}
} // namespace swe::fudgelessslippingtoolong_micro_widths_001
#endif
