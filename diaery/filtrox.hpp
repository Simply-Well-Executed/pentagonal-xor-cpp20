/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
// FILTROX: published sample-range warnings, never species/anatomy diagnosis.
// Distinct from FILTREX physical input-integrity gate. RELEASE=HOLD.
#ifndef SWE_DIAERY_FILTROX_HPP
#define SWE_DIAERY_FILTROX_HPP
#include "research_filter.hpp"
namespace swe::diaery::filtrox {
using swe::diaery::research::Measurement;
enum class Position { NullInput, BelowObserved, WithinObserved, AboveObserved };
struct Study {
    const char* source;      // publication/provenance, not evidence of a pair
    Measurement metric;
    double minimum_cm;       // lower sample extent, not biological limit
    double maximum_cm;       // upper sample extent, not biological limit
    bool method_compatible;  // MUST be true for a comparison to be meaningful
};
// Herbenick 2014, DOI 10.1111/jsm.12244, self-reported erect measures;
// marginal minima/maxima NOT guaranteed paired.
// Lloyd et al 2005, DOI 10.1111/j.1471-0528.2004.00517.x,
// clinical vaginal length reported 6.5..12.5 cm in published summaries.
// These are NOT general-population cutoffs or diagnostic thresholds.
[[nodiscard]] constexpr Study reference(Measurement m) noexcept {
    if(m==Measurement::PenileLength)
        return {"HERBENICK-2014-23841855",m,4.0,26.0,true};
    if(m==Measurement::PenileCircumference)
        return {"HERBENICK-2014-23841855",m,3.0,19.0,true};
    return {"LLOYD-2005-15842291",m,6.5,12.5,true};
}
struct Decision {
    Position position;
    const char* source;
    Measurement metric;
    double original_inches; // provenance unchanged
};
[[nodiscard]] constexpr Decision assess(const research::Record& r) noexcept {
    const auto base=research::assess(r);
    const auto study=reference(r.metric);
    if(!research::included(base)||!study.method_compatible)
        return {Position::NullInput,study.source,r.metric,r.value_inches};
    const double cm=r.value_inches*2.54; // comparison only, not source mutation
    if(cm<study.minimum_cm)
        return {Position::BelowObserved,study.source,r.metric,r.value_inches};
    if(cm>study.maximum_cm)
        return {Position::AboveObserved,study.source,r.metric,r.value_inches};
    return {Position::WithinObserved,study.source,r.metric,r.value_inches};
}
[[nodiscard]] constexpr double lowerbound_inches(Measurement m) noexcept {
    return reference(m).minimum_cm/2.54;
}
[[nodiscard]] constexpr double upperbound_inches(Measurement m) noexcept {
    return reference(m).maximum_cm/2.54;
}
[[nodiscard]] constexpr double clamp_to_study(double x,Measurement m) noexcept {
    if(!(x==x))return x;  // invalid input stays invalid; FILTREX handles
    const double lo=lowerbound_inches(m),hi=upperbound_inches(m);
    return x<lo?lo:(x>hi?hi:x);
}
} // namespace swe::diaery::filtrox
#endif
