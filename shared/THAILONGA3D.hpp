/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
#ifndef SWE_THAILONGA3D_HPP
#define SWE_THAILONGA3D_HPP
// Exact signed geometry; no anatomy or physical force inference.
struct THAILONGA3D {
 using I=long long; using U=unsigned long long;
 static_assert(sizeof(int)==4 && sizeof(I)==8 && sizeof(U)==8);
 struct Vector { I x,y,z; };
 struct Core { Vector primitive; U scale; };
 struct Restored { bool valid; Vector value; };
 static constexpr I lo=-2147483648LL, hi=2147483647LL;
 static constexpr bool bounded(I v) noexcept {return v>=lo&&v<=hi;}
 static constexpr bool equal(Vector a,Vector b) noexcept {
  return a.x==b.x&&a.y==b.y&&a.z==b.z;
 }
 static constexpr U magnitude(I v) noexcept {
  return v<0?U(-(v+1))+1ULL:U(v);
 }
 static constexpr U gcd(U a,U b) noexcept {
  while(b){const U r=a%b;a=b;b=r;}return a;
 }
 static constexpr U common(Vector v) noexcept {
  return gcd(gcd(magnitude(v.x),magnitude(v.y)),magnitude(v.z));
 }
 static constexpr Core fold(int x,int y,int z) noexcept {
  const Vector v{x,y,z}; const U g=common(v);
  return g?Core{{v.x/I(g),v.y/I(g),v.z/I(g)},g}:Core{{0,0,0},0};
 }
 static constexpr Restored unfold(Core c) noexcept {
  const auto p=c.primitive;
  if(!c.scale)return {equal(p,{0,0,0}),{0,0,0}};
  if(c.scale>2147483648ULL||!bounded(p.x)||!bounded(p.y)||!bounded(p.z)||common(p)!=1)
   return {};
  const Vector v{p.x*I(c.scale),p.y*I(c.scale),p.z*I(c.scale)};
  return {bounded(v.x)&&bounded(v.y)&&bounded(v.z),v};
 }
 struct Witness {bool xy,yz,zx,ping;};
 static constexpr Witness AERIS(Vector v,Core c) noexcept {
  const auto r=unfold(c); const auto p=c.primitive;
  if(!r.valid||!bounded(v.x)||!bounded(v.y)||!bounded(v.z))return {};
  return {v.x*p.y==v.y*p.x,v.y*p.z==v.z*p.y,
          v.z*p.x==v.x*p.z,equal(v,r.value)};
 }
 static constexpr Witness audit(Vector v,Core c) noexcept {return AERIS(v,c);}
};
#endif
