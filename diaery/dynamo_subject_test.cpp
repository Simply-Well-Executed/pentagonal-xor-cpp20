// SPDX-License-Identifier: 0BSD
#include "dynamo_subject.hpp"
using namespace swe::diaery::subject;
constexpr Observation unverified{1.5748,6.1240,2.9,false,false,false};
constexpr Display hold=build(unverified);
static_assert(hold.qualification==Qualification::Hold);
static_assert(hold.xy.x==1.5748 && hold.xy.y==6.1240);
static_assert(hold.constructed_fit.shape==swe::diaery::graph::Shape::Invalid);
constexpr Observation exploratory{1.5748,6.1240,2.9,false,true,false};
static_assert(build(exploratory).qualification==Qualification::ExploratoryOnly);
constexpr Observation hypothetical_paired{1.5748,6.1240,2.9,true,true,true};
static_assert(build(hypothetical_paired).qualification==Qualification::ResearchValid);
static_assert(build(hypothetical_paired).constructed_fit.shape==swe::diaery::graph::Shape::Invalid);
static_assert(build({5.,4.,0.,true,true,true}).qualification==Qualification::Hold);
static_assert(xy_source(unverified).x==1.5748);
int main(){return 0;}
