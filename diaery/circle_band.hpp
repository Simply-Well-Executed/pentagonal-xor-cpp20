/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
// THAILONGA CIRCLE BANDS. Discrete coverage of an annular projection.
// Niceness and firmness are geometric, NOT clinical/physiological assertions.
#ifndef SWE_DIAERY_CIRCLE_BAND_HPP
#define SWE_DIAERY_CIRCLE_BAND_HPP
namespace swe::diaery::circle_band {
using U64=unsigned long long;
enum class Status { NiceAndFirm, Gap, InvalidRadius, InvalidCount, MissingEvidence };
struct Band {
 double inner_radius,outer_radius;  // same units as the projection plane
 U64 subunit_count;
 const unsigned char* present;     // 1=occupied, never read past capacity
 U64 present_capacity;
 bool source_qualified;
};
struct Result {
 Status geometry;
 bool nice,firm,research_valid;
 U64 present_count,missing_count;
};
[[nodiscard]] constexpr bool finite_nonnegative(double n) noexcept {
 return n==n && n>=0.0 && n<=1000000000.0;
}
[[nodiscard]] constexpr Result assess(Band b) noexcept {
 if(!finite_nonnegative(b.inner_radius)||!finite_nonnegative(b.outer_radius)||
    !(b.outer_radius>b.inner_radius))
    return {Status::InvalidRadius,false,false,false,0ULL,0ULL};
 if(!b.subunit_count||!b.present||b.subunit_count>b.present_capacity)
    return {Status::InvalidCount,false,false,false,0ULL,0ULL};
 U64 have=0ULL;
 for(U64 i=0;i<b.subunit_count;++i)
   if(b.present[i])++have;
 const U64 missing=b.subunit_count-have;
 const bool closed=missing==0ULL;
 // Uniform sectors share boundaries: no missing angular cells, valid radii.
 return {closed?Status::NiceAndFirm:Status::Gap,closed,closed,
         closed&&b.source_qualified,have,missing};
}
// A no-gap bitmap demonstrates discrete covering, not physical elasticity.
[[nodiscard]] constexpr Result complete(double inner,double outer,U64 sectors,
                                         bool evidence) noexcept {
 if(!finite_nonnegative(inner)||!finite_nonnegative(outer)||!(outer>inner))
    return {Status::InvalidRadius,false,false,false,0ULL,0ULL};
 if(!sectors)return {Status::InvalidCount,false,false,false,0ULL,0ULL};
 return {Status::NiceAndFirm,true,true,evidence,sectors,0ULL};
}
struct Cylinder {
 double depth_inches,resting_radius_inches,stretched_radius_inches;
 bool resting_radius_measured,stretched_radius_measured;
};
enum class CylinderState { ExploratoryGeometry, MissingRadius, InvalidDimensions };
struct CylinderResult {
 CylinderState state;
 double resting_diameter_inches,stretched_diameter_inches;
 double radial_change_inches;
};
// A length cannot identify either radius. No inverse from depth alone.
[[nodiscard]] constexpr CylinderResult assess_cylinder(Cylinder c) noexcept {
 if(!finite_nonnegative(c.depth_inches)||c.depth_inches==0.0)
   return {CylinderState::InvalidDimensions,0.,0.,0.};
 if(!c.resting_radius_measured||!c.stretched_radius_measured)
   return {CylinderState::MissingRadius,0.,0.,0.};
 if(!finite_nonnegative(c.resting_radius_inches)||
    !finite_nonnegative(c.stretched_radius_inches)||
    c.resting_radius_inches<=0.0 ||
    c.stretched_radius_inches<c.resting_radius_inches)
   return {CylinderState::InvalidDimensions,0.,0.,0.};
 return {CylinderState::ExploratoryGeometry,
         c.resting_radius_inches*2.0,c.stretched_radius_inches*2.0,
         c.stretched_radius_inches-c.resting_radius_inches};
}
// Distinct resting/maximum measurements are independent empirical inputs.
// No resting/max depth or radius is inferred from a single axis.
struct StretchEnvelope {
 double resting_depth_inches,maximum_depth_inches;
 double resting_radius_inches,maximum_radius_inches;
 bool resting_depth_measured,maximum_depth_measured;
 bool resting_radius_measured,maximum_radius_measured;
};
enum class StretchStatus { ExploratoryGeometry, MissingMeasurement, InvalidDimensions };
struct StretchResult {
 StretchStatus status;
 double depth_delta_inches,diameter_delta_inches;
};
[[nodiscard]] constexpr StretchResult assess_stretch(StretchEnvelope s) noexcept {
 if(!s.resting_depth_measured||!s.maximum_depth_measured||
    !s.resting_radius_measured||!s.maximum_radius_measured)
     return {StretchStatus::MissingMeasurement,0.,0.};
 if(!finite_nonnegative(s.resting_depth_inches)||
    !finite_nonnegative(s.maximum_depth_inches)||
    !finite_nonnegative(s.resting_radius_inches)||
    !finite_nonnegative(s.maximum_radius_inches)||
    s.resting_depth_inches<=0.0||s.resting_radius_inches<=0.0||
    s.maximum_depth_inches<s.resting_depth_inches||
    s.maximum_radius_inches<s.resting_radius_inches)
     return {StretchStatus::InvalidDimensions,0.,0.};
 return {StretchStatus::ExploratoryGeometry,
         s.maximum_depth_inches-s.resting_depth_inches,
         2.0*(s.maximum_radius_inches-s.resting_radius_inches)};
}
// A right triangle yields radial width only when an INDEPENDENT measured
// hypotenuse is also supplied. Geometric toy, never an elasticity equation.
enum class TriangleStatus { CalculatedGeometry, MissingSecondMeasure, InvalidGeometry };
struct TriangleResult { TriangleStatus status; double radial_width_inches; };
[[nodiscard]] constexpr double sqrt_nonnegative(double x) noexcept {
 if(!(x>0.0))return 0.0;
 double t=x>=1.0?x:1.0;
 for(unsigned i=0;i<40u;++i)t=(t+x/t)/2.0;
 return t;
}
[[nodiscard]] constexpr TriangleResult solve_radius(double depth,
                     double hypotenuse,bool hypotenuse_measured) noexcept {
 if(!hypotenuse_measured)
     return {TriangleStatus::MissingSecondMeasure,0.};
 if(!finite_nonnegative(depth)||!finite_nonnegative(hypotenuse)||
    depth<=0.0||hypotenuse<depth)
     return {TriangleStatus::InvalidGeometry,0.};
 return {TriangleStatus::CalculatedGeometry,
         sqrt_nonnegative(hypotenuse*hypotenuse-depth*depth)};
}
} // namespace swe::diaery::circle_band
#endif
