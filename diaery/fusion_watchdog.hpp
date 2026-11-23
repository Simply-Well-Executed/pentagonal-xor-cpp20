// SPDX-License-Identifier: 0BSD
// DIAERY FUSION WATCHDOG | THREE inputs + independent final witness.
// XHTML 1.0 Strict-profile uses ordinary p elements, never script.
// Strictly scoped to a PINNED 90-degree triangular hinge experiment.
// No STL, system headers, I/O, network access, dynamic allocation.
#ifndef SWE_DIAERY_FUSION_WATCHDOG_HPP
#define SWE_DIAERY_FUSION_WATCHDOG_HPP
#ifndef SWE_DIAERY_PARITY_GATE_HPP
#error "Load parity_gate.hpp before fusion_watchdog.hpp"
#endif
namespace swe::diaery::watchdog {
namespace str=swe::diaery::strict;
namespace geo=swe::diaery::geometry;
using u64=unsigned long long;
// Authority: the existing three-lane geometry source, not XHTML text.
inline constexpr u64 AD=geo::tridi::AD_FOLDED;
inline constexpr u64 CD=geo::tridi::CD_FOLDED;
inline constexpr u64 AC=geo::tridi::AC_FOLDED;
inline constexpr u64 SUM=AD+CD;
static_assert(AD==67600ULL);
static_assert(CD==152100ULL);
static_assert(AC==219700ULL);
static_assert(SUM==AC);
static_assert(geo::tridi::REPORT.valid);
// The final WATCHDOG line is calculated from its three source
// variables. It is not an independent secret or a checksum.
[[nodiscard]] constexpr str::Builder record(unsigned index) noexcept {
 str::Builder b{};
 switch(index) {
  case 0u:b.text("VAR-AD|squared=");b.num(static_cast<long long>(AD));break;
  case 1u:b.text("VAR-CD|squared=");b.num(static_cast<long long>(CD));break;
  case 2u:b.text("VAR-AC|squared=");b.num(static_cast<long long>(AC));break;
  case 3u:b.text("WATCHDOG|rule=AD+CD=AC|squared=");
          b.num(static_cast<long long>(SUM));break;
  default:b.ok=false;break;
 }
 return b;
}
[[nodiscard]] constexpr bool qualified(const str::Page& page) noexcept {
 if(!page.structurally_valid)return false;
 unsigned found=0u,previous=0u;
 for(unsigned i=0u;i<page.count;++i) {
  const auto& l=page.lines[i];
  if(str::prefix(l.text,"VAR-")||str::prefix(l.text,"WATCHDOG|")) {
   if(found>=4u||l.tag!='p')return false;
   if(found!=0u&&i!=previous+1u)return false; // contiguous records
   const auto expected=record(found);
   if(!expected.ok||!str::same(l.text,expected.s))return false;
   previous=i;++found;
  }
 }
 return found==4u&&SUM==AC&&geo::tridi::REPORT.valid;
}
static constexpr auto AD_TEXT=record(0u);
static constexpr auto CD_TEXT=record(1u);
static constexpr auto AC_TEXT=record(2u);
static constexpr auto WATCHDOG_TEXT=record(3u);
static_assert(str::same(AD_TEXT.s,"VAR-AD|squared=67600"));
static_assert(str::same(CD_TEXT.s,"VAR-CD|squared=152100"));
static_assert(str::same(AC_TEXT.s,"VAR-AC|squared=219700"));
static_assert(str::same(WATCHDOG_TEXT.s,
 "WATCHDOG|rule=AD+CD=AC|squared=219700"));
static_assert(!geo::solid::RELEASE_AUTHORIZED);
} // namespace swe::diaery::watchdog
#endif
