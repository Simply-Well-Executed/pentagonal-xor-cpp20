/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
#ifndef SWE_NULL0_TOPOLOGY_HPP
#define SWE_NULL0_TOPOLOGY_HPP
// Symbolic topology only: no actual orbital or force safety claims.
struct NULL0_TOPOLOGY {
 using U=unsigned;
 static constexpr U mask=2047u, hexagons=99u, pentagrams=180u, xor_slots=360u;
 static constexpr U delta(U a,U b) noexcept {return (a^b)&mask;}
 static constexpr U bias(U a,U b) noexcept {return mask^delta(a,b);}
 static constexpr bool interface(bool positive,bool negative) noexcept {
  return positive&&negative;
 }
 static constexpr U stars(U cell) noexcept {
  if(cell>=hexagons)return 0;
  const U r=cell/11,c=cell%11;
  return (r==0||r==8)&&c>=1&&c<=9?1:2;
 }
 static constexpr U active_stars(U selection) noexcept {
  U n=0;for(U i=0;i<hexagons;++i)if(selection&(1u<<(i%11)))n+=stars(i);
  return n;
 }
 struct Pairing {bool valid;U used,duplicates,missing;};
 static constexpr Pairing pair(const U (&dest)[5]) noexcept {
  U seen=0,duplicate=0;
  for(U i=0;i<5;++i){
   if(dest[i]<16||dest[i]>20)return {};
   const U bit=1u<<(dest[i]-16);
   if(seen&bit)++duplicate;
   seen|=bit;
  }
  U used=0;for(U i=0;i<5;++i)if(seen&(1u<<i))++used;
  return {used==5,used,duplicate,5-used};
 }
 static constexpr U pole_mask=(1u<<20)-1u;
 static constexpr U pole_delta(U a,U b) noexcept {return (a^b)&pole_mask;}
 static constexpr U pole_bias(U a,U b) noexcept {return pole_mask^pole_delta(a,b);}
 // Illustrative energy in watt-minutes, integer minute resolution, 0..180.
 struct Energy {bool valid; U watt_minutes;};
 static constexpr Energy energy(U minutes,bool solar) noexcept {
  if(minutes>180)return {};
  int e=6000;
  for(U t=0;t<minutes;++t){
   e+=(solar&&t%90<54?80:0)-50;
   if(e<0)e=0; if(e>6000)e=6000;
  }
  return {true,U(e)};
 }
 struct Groups {U complete,reserve;};
 static constexpr Groups groups(U n) noexcept {return {n/20,n%20};}
};
#endif
