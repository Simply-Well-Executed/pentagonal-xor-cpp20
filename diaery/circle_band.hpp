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
 const unsigned char* present;     // length >= subunit_count, 1 = occupied
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
 if(!b.subunit_count||!b.present)
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
} // namespace swe::diaery::circle_band
#endif
