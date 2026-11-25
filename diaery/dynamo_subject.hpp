// SPDX-License-Identifier: 0BSD
// Explicit DYNAMO XY subject coordinates and a separate Z dimension.
#ifndef SWE_DIAERY_DYNAMO_SUBJECT_HPP
#define SWE_DIAERY_DYNAMO_SUBJECT_HPP
#include "application_of_ymxb.hpp"
#include "error_ledger.hpp"
namespace swe::diaery::subject {
namespace g=swe::diaery::graph;
namespace e=swe::diaery::error;
enum class Qualification { ResearchValid, ExploratoryOnly, Hold };
struct Observation {
    double penile_length_inches;
    double penile_girth_inches;
    double vaginal_depth_inches;
    bool verified_paired_subject;
    bool verified_non_mri_depth;
    bool has_reported_uncertainty;
};
struct Display {
    g::Point xy;
    double z_inches;
    Qualification qualification;
    g::Fit constructed_fit;
};
[[nodiscard]] constexpr Display build(const Observation& s) noexcept {
    const g::Fit no_fit{g::Shape::Invalid,{0.,0.},0.,0.,0.,0.};
    const g::Point xy{s.penile_length_inches,s.penile_girth_inches};
    if(!g::finite_domain(xy.x)||!g::finite_domain(xy.y)||
       xy.x<=0.||xy.y<=0.||!g::finite_domain(s.vaginal_depth_inches))
        return {xy,s.vaginal_depth_inches,Qualification::Hold,no_fit};
    if(!e::admissible_depth(s.vaginal_depth_inches,
                          s.verified_non_mri_depth,s.verified_non_mri_depth))
        return {xy,s.vaginal_depth_inches,Qualification::Hold,no_fit};
    if(!s.verified_paired_subject || !s.has_reported_uncertainty)
        return {xy,s.vaginal_depth_inches,Qualification::ExploratoryOnly,no_fit};
    // Deliberate modeling rule: a single (X,Y,Z) observation does not
    // supply three independent planar points for any scientific circle.
    return {xy,s.vaginal_depth_inches,Qualification::ResearchValid,no_fit};
}
[[nodiscard]] constexpr g::Point xy_source(const Observation& s) noexcept {
    return {s.penile_length_inches,s.penile_girth_inches};
}
} // namespace swe::diaery::subject
#endif
