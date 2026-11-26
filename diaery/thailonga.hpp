// SPDX-License-Identifier: 0BSD
// THAILONGA combines two distinct visual aspects of one DYNTR0X state:
// anatomical XY/Z + independent PROJECTIONS, with no source mutation.
#ifndef SWE_DIAERY_THAILONGA_HPP
#define SWE_DIAERY_THAILONGA_HPP
#include "application_of_ymxb.hpp"
#include "research_filter.hpp"
#include "filtrox.hpp"
#include "projections.hpp"
#include "trace_timeline.hpp"
namespace swe::diaery::thailonga {
namespace gr=swe::diaery::graph;
namespace rs=swe::diaery::research;
namespace fx=swe::diaery::filtrox;
namespace pr=swe::diaery::projections;
namespace tm=swe::diaery::trace;
struct Source {
 double length_inches;
 double circumference_inches;
 double depth_inches;
 bool reported_length;
 bool reported_circumference;
 bool reported_depth;
};
struct Check {
 rs::Decision filtrex;
 fx::Decision filtrox;
};
struct Frame {
 Source original;
 Check x,y,z;
 bool eligible;
 bool within_sample_ranges;
 gr::InterceptLine intercept;
 pr::Status projection_status[10];
 pr::Status orgasm_band_status;
 tm::Yarn time;
};
[[nodiscard]] constexpr Check check(rs::Record record) noexcept {
 return {rs::assess(record),fx::assess(record)};
}
[[nodiscard]] constexpr Frame build(Source source,tm::U64 exact_fs) noexcept {
 const auto x=check({"THAILONGA/X",rs::Measurement::PenileLength,
                      source.length_inches,source.reported_length});
 const auto y=check({"THAILONGA/Y",rs::Measurement::PenileCircumference,
                      source.circumference_inches,source.reported_circumference});
 const auto z=check({"THAILONGA/Z",rs::Measurement::VaginalDepth,
                      source.depth_inches,source.reported_depth});
 const bool eligible=rs::included(x.filtrex)&&rs::included(y.filtrex)&&
                     rs::included(z.filtrex);
 const bool ranges=x.filtrox.position==fx::Position::WithinObserved &&
                   y.filtrox.position==fx::Position::WithinObserved &&
                   z.filtrox.position==fx::Position::WithinObserved;
 const auto line=eligible?gr::application_of_ymxb(source.length_inches,
                         source.circumference_inches):
                         gr::InterceptLine{false,0.,0.,{0.,0.},{0.,0.}};
 const pr::Evidence no_study{eligible&&line.valid,
                             false,false,false,false,false,false};
 Frame frame{source,x,y,z,eligible,ranges,line,{},pr::Status::NullEvidence,
             tm::make_yarn(exact_fs)};
 for(unsigned slot=0;slot<10u;++slot)
     frame.projection_status[slot]=pr::qualify(slot,no_study);
 frame.orgasm_band_status=pr::band_qualification(
     pr::historical_female_orgasm_band,false,false);
 return frame;
}
} // namespace swe::diaery::thailonga
#endif
