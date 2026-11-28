/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
// DIAERY-TRIFLAVORS-PR34-022
// Exact XHTML 1.0 Strict-profile witness, native C++20 model.
// Oracle Java 27.0.1.1 independently calculates the same records.
// Only the 3-4-5 triangle "Nice" containment predicate is claimed.
// A radius-squared sweep is NOT a radian sweep. Radians are angles.
// Closure of arbitrary step sizes is NOT guaranteed; fail closed.
#ifndef SWE_DIAERY_TRI_FLAVORS_GATE_HPP
#define SWE_DIAERY_TRI_FLAVORS_GATE_HPP
#ifndef SWE_DIAERY_MODEL_BRIDGE_HPP
#error "Include model_bridge.hpp before tri_flavors_gate.hpp"
#endif
#ifndef SWE_EULERS_JIG_CANONICAL_HPP
#error "Include eulers_jig_canonical.hpp before tri_flavors_gate.hpp"
#endif
namespace swe::diaery::tri_flavors {
namespace nice=swe::u_circle_fit;
namespace str=swe::diaery::strict;
namespace jig=swe::eulers_jig;
namespace slide=swe::slidingtoolong;
using u64=unsigned long long;
static constexpr u64 SCALE_JIG=jig::SCALE_X1E17;
static constexpr u64 SCALE_REF=slide::DECIMAL_SCALE_1E18;
static constexpr u64 HISTORICAL_E_REFERENCE_X1E18=slide::MATH_E_X1E18;
static constexpr u64 JIG_FRAC=jig::ONE_TRIANGLE_X1E17%SCALE_JIG;
static constexpr u64 E_FRAC=HISTORICAL_E_REFERENCE_X1E18%SCALE_REF;
// Multiplying by 64 through six modular doublings. Each step is
// below 2*10^18, so this C++20 arithmetic cannot overflow uint64.
[[nodiscard]] constexpr u64 MOD64(u64 word,u64 denominator) noexcept {
 for(unsigned i=0;i<6u;++i)word=(word*2ULL)%denominator;
 return word;
}
[[nodiscard]] constexpr u64 GAP(u64 remainder,u64 denom) noexcept {
 return remainder<denom-remainder?remainder:denom-remainder;
}
inline constexpr u64 JIG_REMAINDER64=MOD64(JIG_FRAC,SCALE_JIG);
inline constexpr u64 E_REMAINDER64=MOD64(E_FRAC,SCALE_REF);
inline constexpr u64 JIG_GAP_X1E18=GAP(JIG_REMAINDER64,SCALE_JIG)*10ULL;
inline constexpr u64 E_GAP_X1E18=GAP(E_REMAINDER64,SCALE_REF);
static_assert(JIG_REMAINDER64==27877218430200000ULL);
static_assert(E_REMAINDER64==970037021378895040ULL);
static_assert(JIG_GAP_X1E18==278772184302000000ULL);
static_assert(E_GAP_X1E18==29962978621104960ULL);
static_assert(JIG_GAP_X1E18>E_GAP_X1E18); // counterexample
static_assert(JIG_REMAINDER64!=0ULL&&E_REMAINDER64!=0ULL);
// A normalized 64-step full turn closes exactly by construction.
static_assert((SCALE_REF/64ULL)*64ULL==SCALE_REF);
// Reuse canonical Nice triangle and circle. The center is doubled.
// At radius2=2499 the boundary fails, 2500 and 2501 both pass.
static_assert(nice::DOUBLE_AREA(nice::QUAKER)==1200ULL);
static_assert(nice::VALIDATE(nice::QUAKER,{30,40,2499ULL})==
 nice::U::NotNice);
static_assert(nice::VALIDATE(nice::QUAKER,{30,40,2500ULL})==
 nice::U::Nice);
static_assert(nice::VALIDATE(nice::QUAKER,{30,40,2501ULL})==
 nice::U::Nice);
// Sweep every allowed squared radius 1..4096, monotonic inclusion.
// No new optimizers are dynamically created by the sweep.
[[nodiscard]] constexpr bool SWEEP() noexcept {
 bool seenNice=false;
 unsigned firstNice=0u;
 for(unsigned r=1u;r<=4096u;++r) {
  const bool pass=nice::VALIDATE(nice::QUAKER,
        {30,40,static_cast<u64>(r)})==nice::U::Nice;
  if(pass && !seenNice){seenNice=true;firstNice=r;}
  if(seenNice && !pass)return false;
 }
 return firstNice==2500u;
}
static constexpr const char* DOCUMENT_TITLE="DIAERY | Tri-Flavor Concurrence";
static constexpr const char* LINES[4]={
 "TRIFLAVOR-1|oracle=27.0.1.1|jig_x1e17=2864498081537971875",
 "TRIFLAVOR-NICE|triangle=30,40|radius2_min=2500|below=0|at=1|above=1|sweep=4096",
 "TRIFLAVOR-LOOP|steps=64|jig_closed=0|e_closed=0|jig_gap_x1e18=278772184302000000|e_gap_x1e18=29962978621104960|normalized_closed=1",
 "TRIFLAVOR-WATCHDOG|monotone=1|fail_closed=1|release=HOLD"
};
[[nodiscard]] constexpr bool PAGE(const str::Page& page) noexcept {
 if(!page.structurally_valid||!str::same(page.title,DOCUMENT_TITLE)||
    page.count!=5u||page.lines[0].tag!='1'|| 
    !str::same(page.lines[0].text,"DIAERY Tri-Flavor Concurrence"))return false;
 for(unsigned i=0;i<4u;++i) {
  if(page.lines[i+1u].tag!='p'||
     !str::same(page.lines[i+1u].text,LINES[i]))return false;
 }
 return SWEEP()&&slide::QUALIFIED()&&jig::ONE_TRIANGLE_X1E17==
  2864498081537971875ULL;
}
inline constexpr bool MATHEMATICAL_EXP_OPTION=false;
inline constexpr bool XHTML_FULL_DTD_CERTIFIED=false;
inline constexpr bool JAVA_RUNTIME_VERIFIED_BY_CPP=false;
static_assert(!MATHEMATICAL_EXP_OPTION&&!XHTML_FULL_DTD_CERTIFIED &&
 !JAVA_RUNTIME_VERIFIED_BY_CPP);
} // namespace swe::diaery::tri_flavors
#endif
