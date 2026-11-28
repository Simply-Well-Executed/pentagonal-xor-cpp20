/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
// SLIPSLIDINGTOOLONG-001: GLINTING, GLIZZING, GUARDMEMORYGUARD and MXRING.
// ISO C++20, no includes, STL, compiler extensions, or arbitrary memory reads.
// Logical 32-bit limbs; no physical register adjacency or acceleration claim.
#ifndef SWE_SLIPSLIDINGTOOLONG_HPP
#define SWE_SLIPSLIDINGTOOLONG_HPP
namespace swe::slipslidingtoolong {
using u32=unsigned int;
using u64=unsigned long long;
static_assert(sizeof(u32)==4 && sizeof(u64)==8);
inline constexpr unsigned MAX_LIMBS=16u;
inline constexpr unsigned WIDTH_COUNT=10u;
inline constexpr unsigned WIDTHS[WIDTH_COUNT]{
 32u,64u,128u,192u,224u,256u,384u,448u,480u,512u
};
inline constexpr unsigned MXRING_CAPACITY=8u;
inline constexpr u32 HEAD=0xA12C76E9u;
inline constexpr u32 TAIL=0x75D3B42Eu;
inline constexpr u32 FNV=16777619u;

[[nodiscard]] constexpr bool allowed(unsigned bits) noexcept {
 for(unsigned i=0;i<WIDTH_COUNT;++i)if(WIDTHS[i]==bits)return true;
 return false;
}
enum class Result:u32 {PASS, REJECT_WIDTH, REJECT_GUARD, REJECT_LOSS,
 REJECT_FULL, REJECT_EMPTY, REJECT_SHIFT};
struct Register {
 u32 head;
 u32 limb[MAX_LIMBS];
 u32 tail;
 unsigned bits;
 u32 tag;
 u32 digest;
};
[[nodiscard]] constexpr u32 mix(u32 h,u32 v) noexcept {
 return (h^v)*FNV;
}
[[nodiscard]] constexpr u32 checksum(const Register& r) noexcept {
 u32 h=mix(mix(2166136261u,r.bits),r.tag);
 for(unsigned i=0;i<MAX_LIMBS;++i)h=mix(h,r.limb[i]);
 return h;
}
[[nodiscard]] constexpr Register make_register(unsigned bits,u32 tag=0u) noexcept {
 Register r{HEAD,{},TAIL,bits,tag,0u};
 r.digest=checksum(r);
 return r;
}
[[nodiscard]] constexpr Register sample(unsigned bits,u32 lo,u32 tag=7u) noexcept {
 Register r=make_register(bits,tag);
 r.limb[0]=lo;
 r.digest=checksum(r);
 return r;
}
// GUARD checks ownership bounds and canaries; MEMORYGUARD checks the digest.
// The digest is a deterministic error detector, NOT a cryptographic MAC.
[[nodiscard]] constexpr bool GUARD(const Register& r) noexcept {
 if(!allowed(r.bits)||r.head!=HEAD||r.tail!=TAIL)return false;
 for(unsigned i=r.bits/32u;i<MAX_LIMBS;++i)if(r.limb[i]!=0u)return false;
 return true;
}
[[nodiscard]] constexpr bool MEMORYGUARD(const Register& r) noexcept {
 return checksum(r)==r.digest;
}
[[nodiscard]] constexpr bool GUARDMEMORYGUARD(const Register& r) noexcept {
 return GUARD(r)&&MEMORYGUARD(r);
}
struct Projection {Register value;Result status;};
[[nodiscard]] constexpr Projection GLINTING(const Register& input,unsigned dest) noexcept {
 if(!allowed(dest)||dest>input.bits)return {make_register(32u),Result::REJECT_WIDTH};
 if(!GUARDMEMORYGUARD(input))return {make_register(dest),Result::REJECT_GUARD};
 for(unsigned i=dest/32u;i<input.bits/32u;++i)
  if(input.limb[i]!=0u)return {make_register(dest),Result::REJECT_LOSS};
 Register r=make_register(dest,input.tag);
 for(unsigned i=0;i<dest/32u;++i)r.limb[i]=input.limb[i];
 r.digest=checksum(r);
 return {r,Result::PASS};
}
[[nodiscard]] constexpr Projection GLIZZING(const Register& input,unsigned dest) noexcept {
 if(!allowed(dest)||dest<input.bits)return {make_register(32u),Result::REJECT_WIDTH};
 if(!GUARDMEMORYGUARD(input))return {make_register(dest),Result::REJECT_GUARD};
 Register r=make_register(dest,input.tag);
 for(unsigned i=0;i<input.bits/32u;++i)r.limb[i]=input.limb[i];
 r.digest=checksum(r);
 return {r,Result::PASS};
}
[[nodiscard]] constexpr Projection project(const Register& r,unsigned dest) noexcept {
 return dest<r.bits?GLINTING(r,dest):GLIZZING(r,dest);
}
[[nodiscard]] constexpr bool same(const Register& a,const Register& b) noexcept {
 if(!GUARDMEMORYGUARD(a)||!GUARDMEMORYGUARD(b)||
    a.bits!=b.bits||a.tag!=b.tag)return false;
 for(unsigned i=0;i<MAX_LIMBS;++i)if(a.limb[i]!=b.limb[i])return false;
 return true;
}
struct Arithmetic {Register value;Result status;bool carry_or_borrow;};
[[nodiscard]] constexpr Arithmetic add(const Register& a,const Register& b) noexcept {
 if(!GUARDMEMORYGUARD(a)||!GUARDMEMORYGUARD(b))
  return {make_register(32u),Result::REJECT_GUARD,false};
 if(a.bits!=b.bits)return {make_register(32u),Result::REJECT_WIDTH,false};
 Register r=make_register(a.bits,a.tag);
 u64 carry=0ULL;
 for(unsigned i=0;i<a.bits/32u;++i){
  const u64 v=static_cast<u64>(a.limb[i])+b.limb[i]+carry;
  r.limb[i]=static_cast<u32>(v);
  carry=v>>32u;
 }
 r.digest=checksum(r);
 return {r,Result::PASS,carry!=0ULL};
}
[[nodiscard]] constexpr Arithmetic subtract(const Register& a,const Register& b) noexcept {
 if(!GUARDMEMORYGUARD(a)||!GUARDMEMORYGUARD(b))
  return {make_register(32u),Result::REJECT_GUARD,false};
 if(a.bits!=b.bits)return {make_register(32u),Result::REJECT_WIDTH,false};
 Register r=make_register(a.bits,a.tag);
 u64 borrow=0ULL;
 for(unsigned i=0;i<a.bits/32u;++i){
  const u64 right=static_cast<u64>(b.limb[i])+borrow;
  r.limb[i]=static_cast<u32>(static_cast<u64>(a.limb[i])-right);
  borrow=static_cast<u64>(a.limb[i])<right?1ULL:0ULL;
 }
 r.digest=checksum(r);
 return {r,Result::PASS,borrow!=0ULL};
}
// A logical bitwise AND, not a pointer, hook, memory address, or DMA operation.
[[nodiscard]] constexpr Projection bit_and(const Register& a,const Register& b) noexcept {
 if(!GUARDMEMORYGUARD(a)||!GUARDMEMORYGUARD(b))
  return {make_register(32u),Result::REJECT_GUARD};
 if(a.bits!=b.bits)return {make_register(32u),Result::REJECT_WIDTH};
 Register r=make_register(a.bits,a.tag);
 for(unsigned i=0;i<a.bits/32u;++i)r.limb[i]=a.limb[i]&b.limb[i];
 r.digest=checksum(r);
 return {r,Result::PASS};
}
[[nodiscard]] constexpr bool get_bit(const Register& r,unsigned k) noexcept {
 return ((r.limb[k/32u]>>(k%32u))&1u)!=0u;
}
constexpr void set_bit(Register& r,unsigned k) noexcept {
 r.limb[k/32u]|=(1u<<(k%32u));
}
struct Shift {Register value;Result status;bool discarded_nonzero;};
[[nodiscard]] constexpr Shift shift_left(const Register& a,unsigned amount) noexcept {
 if(!GUARDMEMORYGUARD(a))return {make_register(32u),Result::REJECT_GUARD,false};
 if(amount>a.bits)return {make_register(a.bits),Result::REJECT_SHIFT,false};
 Register r=make_register(a.bits,a.tag);
 bool lost=false;
 for(unsigned bit=0;bit<a.bits;++bit){
  if(!get_bit(a,bit))continue;
  if(bit>=a.bits-amount)lost=true;
  else set_bit(r,bit+amount);
 }
 r.digest=checksum(r);
 return {r,Result::PASS,lost};
}
[[nodiscard]] constexpr Shift shift_right(const Register& a,unsigned amount) noexcept {
 if(!GUARDMEMORYGUARD(a))return {make_register(32u),Result::REJECT_GUARD,false};
 if(amount>a.bits)return {make_register(a.bits),Result::REJECT_SHIFT,false};
 Register r=make_register(a.bits,a.tag);
 bool lost=false;
 for(unsigned bit=0;bit<a.bits;++bit){
  if(!get_bit(a,bit))continue;
  if(bit<amount)lost=true;
  else set_bit(r,bit-amount);
 }
 r.digest=checksum(r);
 return {r,Result::PASS,lost};
}

// MXRING: bounded software ring of BY-VALUE copies, never physical ring RAM.
// No pointers to foreign registers; no concurrent access without an external lock.
struct MXRing {
 u32 head;
 Register slot[MXRING_CAPACITY];
 u32 tail;
 unsigned read_index,write_index,count;
 u64 generation;
 u32 digest;
};
[[nodiscard]] constexpr u32 ring_checksum(const MXRing& r) noexcept {
 u32 h=2166136261u;
 h=mix(h,r.read_index);h=mix(h,r.write_index);h=mix(h,r.count);
 h=mix(h,static_cast<u32>(r.generation));h=mix(h,static_cast<u32>(r.generation>>32u));
 for(unsigned i=0;i<MXRING_CAPACITY;++i) {
  h=mix(h,r.slot[i].digest);
  h=mix(h,r.slot[i].bits);
  h=mix(h,r.slot[i].tag);
 }
 return h;
}
[[nodiscard]] constexpr MXRing make_ring() noexcept {
 MXRing r{HEAD,{},TAIL,0u,0u,0u,0ULL,0u};
 for(unsigned i=0;i<MXRING_CAPACITY;++i)r.slot[i]=make_register(32u);
 r.digest=ring_checksum(r);
 return r;
}
[[nodiscard]] constexpr bool GUARDMEMORYGUARD(const MXRing& r) noexcept {
 if(r.head!=HEAD||r.tail!=TAIL||r.read_index>=MXRING_CAPACITY||
    r.write_index>=MXRING_CAPACITY||r.count>MXRING_CAPACITY||
    r.write_index!=(r.read_index+r.count)%MXRING_CAPACITY)return false;
 for(unsigned i=0;i<MXRING_CAPACITY;++i)
  if(!GUARDMEMORYGUARD(r.slot[i]))return false;
 return r.digest==ring_checksum(r);
}
[[nodiscard]] constexpr Result push(MXRing& r,const Register& item) noexcept {
 if(!GUARDMEMORYGUARD(r)||!GUARDMEMORYGUARD(item))return Result::REJECT_GUARD;
 if(r.count==MXRING_CAPACITY)return Result::REJECT_FULL;
 r.slot[r.write_index]=item;
 r.write_index=(r.write_index+1u)%MXRING_CAPACITY;
 ++r.count;++r.generation;
 r.digest=ring_checksum(r);
 return Result::PASS;
}
struct Read {Register value;Result status;};
[[nodiscard]] constexpr Read peek(const MXRing& r) noexcept {
 if(!GUARDMEMORYGUARD(r))return {make_register(32u),Result::REJECT_GUARD};
 if(r.count==0u)return {make_register(32u),Result::REJECT_EMPTY};
 return {r.slot[r.read_index],Result::PASS};
}
[[nodiscard]] constexpr Read pop(MXRing& r) noexcept {
 const Read observed=peek(r);
 if(observed.status!=Result::PASS)return observed;
 r.slot[r.read_index]=make_register(32u);
 r.read_index=(r.read_index+1u)%MXRING_CAPACITY;
 --r.count;++r.generation;
 r.digest=ring_checksum(r);
 return observed;
}
// Defensive SPY reads only this owned object's metadata, not external memory.
struct Observation {unsigned count;u64 generation;bool safe;u32 digest;};
[[nodiscard]] constexpr Observation SPY(const MXRing& r) noexcept {
 return {r.count,r.generation,GUARDMEMORYGUARD(r),r.digest};
}
[[nodiscard]] constexpr bool matrix_100() noexcept {
 unsigned total=0u,down=0u,up=0u;
 for(unsigned i=0;i<WIDTH_COUNT;++i){
  const Register original=sample(WIDTHS[i],0xA1B2C3D4u,i+1u);
  if(!GUARDMEMORYGUARD(original))return false;
  for(unsigned j=0;j<WIDTH_COUNT;++j){
   const Projection p=project(original,WIDTHS[j]);
   if(p.status!=Result::PASS)return false;
   const Projection back=project(p.value,WIDTHS[i]);
   if(back.status!=Result::PASS||!same(original,back.value))return false;
   ++total;
   if(WIDTHS[j]<WIDTHS[i])++down;
   if(WIDTHS[j]>WIDTHS[i])++up;
  }
 }
 return total==100u&&down==45u&&up==45u;
}
[[nodiscard]] constexpr bool rejection_45() noexcept {
 unsigned count=0u;
 for(unsigned i=1;i<WIDTH_COUNT;++i){
  Register high=sample(WIDTHS[i],0x1234u);
  high.limb[WIDTHS[i]/32u-1u]=0x80000000u;
  high.digest=checksum(high);
  for(unsigned j=0;j<i;++j){
   if(GLINTING(high,WIDTHS[j]).status!=Result::REJECT_LOSS)return false;
   ++count;
  }
 }
 return count==45u;
}
[[nodiscard]] constexpr bool arithmetic_and_shifts() noexcept {
 const Arithmetic sum=add(sample(32u,0xffffffffu),sample(32u,1u));
 if(sum.status!=Result::PASS||!sum.carry_or_borrow||sum.value.limb[0]!=0u)return false;
 const Arithmetic diff=subtract(sample(32u,0u),sample(32u,1u));
 if(diff.status!=Result::PASS||!diff.carry_or_borrow||diff.value.limb[0]!=0xffffffffu)return false;
 const Projection hooked=bit_and(sample(32u,0xf0f0f0f0u),sample(32u,0x00ff00ffu));
 if(hooked.status!=Result::PASS||hooked.value.limb[0]!=0x00f000f0u)return false;
 const Register input=sample(128u,0x80000001u);
 const Shift left=shift_left(input,1u);
 if(left.status!=Result::PASS||left.discarded_nonzero||
    left.value.limb[0]!=2u||left.value.limb[1]!=1u)return false;
 const Shift right=shift_right(left.value,1u);
 if(right.status!=Result::PASS||right.discarded_nonzero||!same(input,right.value))return false;
 Register max=sample(512u,0u);
 max.limb[15]=0x80000000u;
 max.digest=checksum(max);
 if(!shift_left(max,1u).discarded_nonzero)return false;
 if(shift_left(max,513u).status!=Result::REJECT_SHIFT)return false;
 return true;
}
[[nodiscard]] constexpr bool ring_roundtrip() noexcept {
 MXRing ring=make_ring();
 const Register a=sample(64u,0xAABBCCDDu,71u);
 const Register b=sample(512u,0x12345678u,72u);
 if(!GUARDMEMORYGUARD(ring))return false;
 if(push(ring,a)!=Result::PASS||push(ring,b)!=Result::PASS)return false;
 if(!SPY(ring).safe||SPY(ring).count!=2u)return false;
 if(peek(ring).status!=Result::PASS||!same(peek(ring).value,a))return false;
 if(!same(pop(ring).value,a)||!same(pop(ring).value,b))return false;
 if(pop(ring).status!=Result::REJECT_EMPTY)return false;
 for(unsigned i=0;i<MXRING_CAPACITY;++i)if(push(ring,a)!=Result::PASS)return false;
 if(push(ring,a)!=Result::REJECT_FULL)return false;
 ring.slot[ring.read_index].limb[0]^=1u;
 if(GUARDMEMORYGUARD(ring)||peek(ring).status!=Result::REJECT_GUARD)return false;
 return true;
}
inline constexpr bool WIDTH_MATRIX=matrix_100();
inline constexpr bool NARROWING_REJECTS=rejection_45();
inline constexpr bool BIT_OPERATIONS=arithmetic_and_shifts();
inline constexpr bool OWNED_RING=ring_roundtrip();
inline constexpr bool HARDWARE_SPEEDUP_PROVEN=false;
inline constexpr bool THREAD_SAFE_PROVEN=false;
inline constexpr bool PROCESS_CRASH_SURVIVAL_PROVEN=false;
static_assert(WIDTH_MATRIX&&NARROWING_REJECTS&&BIT_OPERATIONS&&OWNED_RING);
static_assert(!HARDWARE_SPEEDUP_PROVEN&&!THREAD_SAFE_PROVEN&&!PROCESS_CRASH_SURVIVAL_PROVEN);
} // namespace swe::slipslidingtoolong
#endif
