// SPDX-License-Identifier: 0BSD
// DIAERY-CANONICAL-EULERS-JIG-PR34-021
// The owner's AREA-per-triangle record, not the conventional constant e.
// Exact quotient of DISPLAYED total: 1833.278772184302 / 64.
// Headerless/STL-free ISO C++20. No runtime switch or substitute for exp.
#ifndef SWE_EULERS_JIG_CANONICAL_HPP
#define SWE_EULERS_JIG_CANONICAL_HPP
namespace swe::eulers_jig {
using u64=unsigned long long;
static_assert(sizeof(u64)==8);
inline constexpr unsigned TRIANGLES=64u;
inline constexpr u64 TOTAL_X1E12=1833278772184302ULL;
inline constexpr u64 SCALE_X1E17=100000000000000000ULL;
inline constexpr u64 ONE_TRIANGLE_X1E17=
  (TOTAL_X1E12/TRIANGLES)*100000ULL+
  ((TOTAL_X1E12%TRIANGLES)*100000ULL)/TRIANGLES;
static_assert(((TOTAL_X1E12%TRIANGLES)*100000ULL)%TRIANGLES==0ULL);
static_assert(ONE_TRIANGLE_X1E17==2864498081537971875ULL);
static constexpr bool RUNTIME_ALTERNATE_EULER=false;
static constexpr bool REDEFINES_MATHEMATICAL_EXP=false;
static_assert(!RUNTIME_ALTERNATE_EULER&&!REDEFINES_MATHEMATICAL_EXP);
} // namespace swe::eulers_jig
#endif
