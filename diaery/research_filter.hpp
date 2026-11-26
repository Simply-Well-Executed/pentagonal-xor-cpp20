// SPDX-License-Identifier: 0BSD
// DYNAMO research inclusion gate: exclude zero/nonpositive anatomical lengths.
// This is dataset eligibility, not diagnosis, identity, or source deletion.
#ifndef SWE_DIAERY_RESEARCH_FILTER_HPP
#define SWE_DIAERY_RESEARCH_FILTER_HPP
namespace swe::diaery::research {
enum class Measurement { PenileLength, PenileCircumference, VaginalDepth };
enum class Disposition { Included, ExcludedZero, ExcludedNegative, ExcludedMissing, ExcludedNonFinite, ExcludedOutOfDomain };
struct Record {
    const char* source_id;  // provenance metadata only; NEVER discard this
    Measurement metric;
    double value_inches;
    bool reported;          // false means missing, distinct from 0
};
struct Decision { Disposition disposition; const char* source_id; Measurement metric; double original_inches; };
[[nodiscard]] constexpr Decision assess(const Record& r) noexcept {
    if(!r.reported)return {Disposition::ExcludedMissing,r.source_id,r.metric,r.value_inches};
    if(!(r.value_inches==r.value_inches))
        return {Disposition::ExcludedNonFinite,r.source_id,r.metric,r.value_inches};
    if(r.value_inches==0.0)
        return {Disposition::ExcludedZero,r.source_id,r.metric,r.value_inches};
    if(r.value_inches<0.0)
        return {Disposition::ExcludedNegative,r.source_id,r.metric,r.value_inches};
    if(r.value_inches>1000000000.0)
        return {Disposition::ExcludedOutOfDomain,r.source_id,r.metric,r.value_inches};
    return {Disposition::Included,r.source_id,r.metric,r.value_inches};
}
[[nodiscard]] constexpr bool included(const Decision& d) noexcept {
    return d.disposition==Disposition::Included;
}
struct Counts { unsigned included; unsigned excluded; unsigned zero; unsigned missing; };
[[nodiscard]] constexpr Counts count(const Record* records,unsigned number) noexcept {
    Counts c{0u,0u,0u,0u};
    if(!records)return c;
    for(unsigned i=0u;i<number;++i){
        const auto d=assess(records[i]);
        if(included(d))++c.included;
        else {
            ++c.excluded;
            if(d.disposition==Disposition::ExcludedZero)++c.zero;
            if(d.disposition==Disposition::ExcludedMissing)++c.missing;
        }
    }
    return c;
}
} // namespace swe::diaery::research
#endif
