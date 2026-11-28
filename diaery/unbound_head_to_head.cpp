/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
// DIAERY-UNBOUND-PR34-020 | exact scalable finite NASCAR head-to-head.
// "Unbound": no hard-coded 64-step arithmetic cap. Memory/time remain
// finite, allocations are checked. No claim of infinite computation.
// Historical TOOLONG, SlidingToolong and finite witnesses preserved.
// Native MSVC C++20 /O2 only, zero system/STL headers, zero telemetry/network.
#include "model_bridge.hpp"
#include "finite_head_to_head.hpp"
#if !defined(_WIN32)
#error "DIAERY UNBOUND requires native Microsoft C++20"
#endif
namespace race=swe::diaery::finite_head_to_head;
namespace sliding=swe::slidingtoolong;
using u32=unsigned int;
using u64=unsigned long long;
static_assert(sizeof(u32)==4u && sizeof(u64)==8u);
inline constexpr u64 BASE=1000000000ULL,MOD=1000000007ULL;
inline constexpr u64 MAX_UINT64=18446744073709551615ULL;
extern "C" {
__declspec(dllimport) void* __stdcall GetProcessHeap();
__declspec(dllimport) void* __stdcall HeapAlloc(void*,unsigned long,u64);
__declspec(dllimport) void* __stdcall HeapReAlloc(void*,unsigned long,void*,u64);
__declspec(dllimport) int __stdcall HeapFree(void*,unsigned long,void*);
__declspec(dllimport) void* __stdcall GetStdHandle(unsigned long);
__declspec(dllimport) int __stdcall WriteFile(
    void*,const void*,unsigned long,unsigned long*,void*);
}
struct Big {
 u32* limb=nullptr;
 u32 used=0u,capacity=0u;
};
// All Big values have one owner. No copying of owning Big structs.
static void free_big(void* heap,Big& b) noexcept {
 if(b.limb)HeapFree(heap,0u,b.limb);
 b.limb=nullptr;b.used=0u;b.capacity=0u;
}
[[nodiscard]] static bool reserve_big(void* heap,Big& b,u32 need) noexcept {
 if(need<=b.capacity)return true;
 if(need==0u||need>0x3fffffffu)return false;
 u32 grow=b.capacity?b.capacity:4u;
 while(grow<need){
  if(grow>0x1fffffffu){grow=need;break;}
  grow*=2u;
 }
 const u64 bytes=static_cast<u64>(grow)*sizeof(u32);
 void* next=b.limb?
    HeapReAlloc(heap,0u,b.limb,bytes):
    HeapAlloc(heap,0u,bytes);
 if(!next)return false;
 b.limb=static_cast<u32*>(next);
 b.capacity=grow;
 return true;
}
[[nodiscard]] static bool from_u64(void* heap,Big& b,u64 value) noexcept {
 if(!reserve_big(heap,b,4u))return false;
 b.used=0u;
 do {
  b.limb[b.used++]=static_cast<u32>(value%BASE);
  value/=BASE;
 }while(value);
 return true;
}
[[nodiscard]] static bool copy_big(void* heap,Big& to,const Big& from) noexcept {
 if(!from.limb||from.used==0u||!reserve_big(heap,to,from.used))
  return false;
 for(u32 i=0;i<from.used;++i)to.limb[i]=from.limb[i];
 to.used=from.used;
 return true;
}
static void trim(Big& x) noexcept {
 while(x.used>1u&&x.limb[x.used-1u]==0u)--x.used;
}
[[nodiscard]] static bool multiply_u32(void* heap,Big& a,u32 m) noexcept {
 if(!a.limb||a.used==0u||!reserve_big(heap,a,a.used+2u))return false;
 u64 carry=0ULL;
 for(u32 i=0;i<a.used;++i){
  const u64 v=static_cast<u64>(a.limb[i])*m+carry;
  a.limb[i]=static_cast<u32>(v%BASE);
  carry=v/BASE;
 }
 while(carry){
  a.limb[a.used++]=static_cast<u32>(carry%BASE);
  carry/=BASE;
 }
 trim(a);return true;
}
[[nodiscard]] static bool multiply_big(void* heap,Big& a,const Big& b) noexcept {
 if(!a.limb||!b.limb||a.used==0u||b.used==0u||
    a.used>0x3fffffffu-b.used-1u)return false;
 Big result{};
 const u32 needed=a.used+b.used+1u;
 if(!reserve_big(heap,result,needed))return false;
 for(u32 i=0;i<needed;++i)result.limb[i]=0u;
 result.used=needed;
 for(u32 i=0;i<a.used;++i){
  u64 carry=0ULL;
  for(u32 j=0;j<b.used;++j){
   const u32 k=i+j;
   const u64 v=static_cast<u64>(a.limb[i])*b.limb[j]+
               result.limb[k]+carry;
   result.limb[k]=static_cast<u32>(v%BASE);
   carry=v/BASE;
  }
  u32 k=i+b.used;
  while(carry!=0ULL){
   if(k>=needed){free_big(heap,result);return false;}
   const u64 v=static_cast<u64>(result.limb[k])+carry;
   result.limb[k]=static_cast<u32>(v%BASE);
   carry=v/BASE;++k;
  }
 }
 trim(result);
 free_big(heap,a);
 a.limb=result.limb;a.used=result.used;a.capacity=result.capacity;
 return true;
}
[[nodiscard]] static int compare_big(const Big& a,const Big& b) noexcept {
 if(a.used!=b.used)return a.used<b.used?-1:1;
 for(u32 i=a.used;i>0u;--i)
  if(a.limb[i-1u]!=b.limb[i-1u])
   return a.limb[i-1u]<b.limb[i-1u]?-1:1;
 return 0;
}
[[nodiscard]] static u64 residue(const Big& a) noexcept {
 u64 r=0ULL;
 for(u32 i=a.used;i>0u;--i)
  r=(r*BASE+static_cast<u64>(a.limb[i-1u]))%MOD;
 return r;
}
[[nodiscard]] static u32 digits(const Big& a) noexcept {
 u32 count=(a.used-1u)*9u;
 u32 top=a.limb[a.used-1u];
 do{++count;top/=10u;}while(top);
 return count;
}
struct Answer {
 u32 steps=0u,first_j=0u,first_e=0u,j_digits=0u,e_digits=0u;
 u64 j_mod=0ULL,e_mod=0ULL,d_mod=0ULL;
 bool valid=false;
};
// No input-based allocation until all constant sources are verified.
[[nodiscard]] static Answer compute(void* heap,u32 steps) noexcept {
 Answer answer{};
 if(!heap||steps==0u||!sliding::QUALIFIED()||
    !race::RUN(2u).valid)return answer;
 Big jig{},math{},scale{},max{},jp{},ep{},dp{},jn{},en{},bound{};
 const bool seeded=from_u64(heap,jig,sliding::EULER_JIG_ONE_X1E17)&&
  multiply_u32(heap,jig,10u)&&
  from_u64(heap,math,sliding::MATH_E_X1E18)&&
  from_u64(heap,scale,sliding::DECIMAL_SCALE_1E18)&&
  from_u64(heap,max,MAX_UINT64)&&
  from_u64(heap,jp,1ULL)&&from_u64(heap,ep,1ULL)&&
  from_u64(heap,dp,1ULL);
 bool good=seeded;
 u64 jm=1ULL,em=1ULL,dm=1ULL;
 if(good){
  const u64 jb=residue(jig),eb=residue(math),db=residue(scale);
  for(u64 step=1ULL;step<=static_cast<u64>(steps);++step){
   const u32 n=static_cast<u32>(step);
   if(!multiply_big(heap,jp,jig)||
      !multiply_big(heap,ep,math)||
      !multiply_big(heap,dp,scale)||
      !copy_big(heap,jn,jp)||!multiply_u32(heap,jn,n)||
      !copy_big(heap,en,ep)||!multiply_u32(heap,en,n)||
      !copy_big(heap,bound,dp)||!multiply_big(heap,bound,max)){
     good=false;break;
   }
   if(compare_big(jn,en)<=0){good=false;break;}
   // Independent modular recurrence; uses no Big multiplication.
   jm=(jm*jb)%MOD;em=(em*eb)%MOD;dm=(dm*db)%MOD;
   const u64 nj=(jm*(step%MOD))%MOD,
             ne=(em*(step%MOD))%MOD;
   if(residue(jn)!=nj||residue(en)!=ne||residue(dp)!=dm){
    good=false;break;
   }
   if(answer.first_j==0u&&compare_big(jn,bound)>0)
    answer.first_j=n;
   if(answer.first_e==0u&&compare_big(en,bound)>0)
    answer.first_e=n;
   // Finite rational inverse uses (dp / jn) or (dp / en);
   // the backward reversal is lossless with no floating point.
   answer.steps=n;
   answer.j_digits=digits(jn);answer.e_digits=digits(en);
   answer.j_mod=nj;answer.e_mod=ne;answer.d_mod=dm;
  }
 }
 answer.valid=good&&answer.steps==steps &&
  (steps<13u||answer.first_j==13u) &&
  (steps<41u||answer.first_e==41u);
 free_big(heap,jig);free_big(heap,math);free_big(heap,scale);
 free_big(heap,max);free_big(heap,jp);free_big(heap,ep);
 free_big(heap,dp);free_big(heap,jn);free_big(heap,en);
 free_big(heap,bound);
 return answer;
}
struct Output {
 char buffer[512]{};
 unsigned used=0u;
 bool ok=true;
 void ch(char c) noexcept {
  if(used+1u>=sizeof(buffer)){ok=false;return;}
  buffer[used++]=c;buffer[used]=0;
 }
 void text(const char* s) noexcept {
  if(!s){ok=false;return;}
  for(unsigned i=0u;s[i];++i)ch(s[i]);
 }
 void number(u64 v) noexcept {
  char temp[24]{};unsigned len=0u;
  do{temp[len++]=static_cast<char>('0'+v%10ULL);v/=10ULL;}
  while(v&&len<sizeof(temp));
  while(len)ch(temp[--len]);
 }
};
static void line(Output& o,const char* key,u64 v) noexcept {
 o.text("|");o.text(key);o.text("=");o.number(v);
}
static bool print(const char* s,unsigned n) noexcept {
 unsigned long written=0u;
 return WriteFile(GetStdHandle(0xFFFFFFF5u),s,
  static_cast<unsigned long>(n),&written,nullptr)!=0&&written==n;
}
[[nodiscard]] static bool parse_steps(int argc,char** argv,u32& n) noexcept {
 if(argc!=3||!argv||!argv[1]||!argv[2])return false;
 const char* p="--steps",*a=argv[1];
 for(unsigned i=0;;++i) {
  if(a[i]!=p[i])return false;
  if(a[i]==0)break;
 }
 u64 value=0ULL;
 for(unsigned i=0u;argv[2][i];++i){
  const char c=argv[2][i];
  if(c<'0'||c>'9')return false;
  value=value*10ULL+static_cast<u64>(c-'0');
  if(value>0xffffffffULL)return false;
 }
 if(!argv[2][0]||value==0ULL)return false;
 n=static_cast<u32>(value);return true;
}
int main(int argc,char** argv) {
 u32 steps=0u;
 if(!parse_steps(argc,argv,steps)){
  constexpr char error[]="DIAERY|NASCAR_UNBOUND=HOLD|BAD_STEP_ARGUMENT\n";
  print(error,sizeof(error)-1u);return 20;
 }
 void* heap=GetProcessHeap();
 const Answer a=compute(heap,steps);
 if(!a.valid){
  constexpr char error[]="DIAERY|NASCAR_UNBOUND=HOLD|ARITHMETIC_OR_MEMORY\n";
  print(error,sizeof(error)-1u);return 21;
 }
 Output o{};
 o.text("DIAERY|NASCAR_UNBOUND=PASS");
 line(o,"STEPS",a.steps);
 line(o,"JIG_U64_LIMIT",a.first_j);
 line(o,"CONVENTIONAL_U64_LIMIT",a.first_e);
 line(o,"JIG_NUMERATOR_DIGITS",a.j_digits);
 line(o,"E_NUMERATOR_DIGITS",a.e_digits);
 line(o,"JIG_MOD_1000000007",a.j_mod);
 line(o,"E_MOD_1000000007",a.e_mod);
 line(o,"DEN_MOD_1000000007",a.d_mod);
 o.text("|EXACT_RECIPROCAL=STRUCTURAL|RELEASE=HOLD\n");
 return o.ok&&print(o.buffer,o.used)?0:22;
}
