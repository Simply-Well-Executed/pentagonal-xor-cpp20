// SPDX-License-Identifier: 0BSD
// DYNTR0X PROJECTIONS: ten bounded projector transparency layers.
// No unrestricted expression evaluation, no fabricated research data.
#ifndef SWE_DIAERY_PROJECTIONS_HPP
#define SWE_DIAERY_PROJECTIONS_HPP
namespace swe::diaery::projections {
enum class Status { Ready, NullEvidence, InvalidSelection, InvalidInput };
enum class Slot : unsigned {
 Source=0, Intercept=1, Identity=2, Baseline=3, Mean=4,
 Threshold=5, MovingAverage=6, Regression=7, Confidence=8, Circle=9
};
struct Metadata {
 const char* token;
 const char* label;
 const char* description;
};
[[nodiscard]] constexpr Metadata describe(unsigned slot) noexcept {
 switch(slot){
 case 0:return {"{0}","SOURCE","Selected source XY; unverified observation"};
 case 1:return {"{1}","Y=MX+B","Algebraic intercept construction; not regression"};
 case 2:return {"{2}","Y=X","Identity reference; not anatomical equivalence"};
 case 3:return {"{3}","Y=0","Horizontal baseline; zero is an axis, not a body"};
 case 4:return {"{4}","MEAN","Requires genuine comparable sample values"};
 case 5:return {"{5}","THRESHOLD","Requires a documented threshold and units"};
 case 6:return {"{6}","ROLLING MEAN","Requires an ordered sample series and window"};
 case 7:return {"{7}","REGRESSION","Requires actual paired observations"};
 case 8:return {"{8}","CONFIDENCE","Requires a justified model, bounds and method"};
 case 9:return {"{9}","CIRCLE","Requires at least three suitable geometric points"};
 default:return {"NULL","HOLD","Projection selection outside 0..9"};
 }
}
[[nodiscard]] constexpr bool valid_token(const char* token) noexcept {
 return token&&token[0]=='{'&&token[1]>='0'&&token[1]<='9'&&
        token[2]=='}'&&token[3]==0;
}
[[nodiscard]] constexpr int parse(const char* token) noexcept {
 return valid_token(token)?static_cast<int>(token[1]-'0'):-1;
}
struct Evidence {
 bool positive_xy;
 bool has_series;
 bool has_window;
 bool has_threshold;
 bool has_paired;
 bool has_interval;
 bool has_three_points;
};
[[nodiscard]] constexpr Status qualify(unsigned slot,Evidence e) noexcept {
 if(slot>=10u)return Status::InvalidSelection;
 if(!e.positive_xy)return Status::InvalidInput;
 if(slot<=3u)return Status::Ready;
 if(slot==4u)return e.has_series?Status::Ready:Status::NullEvidence;
 if(slot==5u)return e.has_threshold?Status::Ready:Status::NullEvidence;
 if(slot==6u)return e.has_series&&e.has_window?Status::Ready:Status::NullEvidence;
 if(slot==7u)return e.has_paired?Status::Ready:Status::NullEvidence;
 if(slot==8u)return e.has_paired&&e.has_interval?Status::Ready:Status::NullEvidence;
 return e.has_three_points?Status::Ready:Status::NullEvidence;
}
// Exploratory remembered historical band, NOT a published probability.
// Do not plot on a physical length axis or call it a predicted orgasm rate.
struct Band { unsigned lower_permyriad; unsigned upper_permyriad; bool source_verified; };
constexpr Band historical_female_orgasm_band{4100u,4200u,false};
[[nodiscard]] constexpr Status band_qualification(Band b,bool endpoints_defined,
                                                  bool outcome_validated) noexcept {
 if(b.lower_permyriad>=b.upper_permyriad||b.upper_permyriad>10000u)
    return Status::InvalidInput;
 return b.source_verified&&endpoints_defined&&outcome_validated?
        Status::Ready:Status::NullEvidence;
}
} // namespace swe::diaery::projections
#endif
