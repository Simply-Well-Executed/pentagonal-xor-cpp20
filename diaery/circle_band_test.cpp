/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
#include "circle_band.hpp"
using namespace swe::diaery::circle_band;
constexpr unsigned char all[8]{1,1,1,1,1,1,1,1};
constexpr unsigned char missing[8]{1,1,1,1,1,1,0,1};
static_assert(assess({1.,2.,8,all,8,false}).nice);
static_assert(assess({1.,2.,8,all,8,false}).firm);
static_assert(!assess({1.,2.,8,all,8,false}).research_valid);
static_assert(assess({1.,2.,8,all,8,true}).research_valid);
static_assert(assess({1.,2.,8,missing,8,true}).geometry==Status::Gap);
static_assert(!assess({1.,2.,8,missing,8,true}).firm);
static_assert(assess({1.,2.,8,missing,8,true}).missing_count==1ULL);
static_assert(assess({2.,1.,8,all,8,true}).geometry==Status::InvalidRadius);
static_assert(assess({1.,2.,0,all,8,true}).geometry==Status::InvalidCount);
static_assert(complete(1.,2.,72,false).present_count==72ULL);
static_assert(assess({1.,2.,9,all,8,true}).geometry==Status::InvalidCount);
static_assert(!complete(1.,2.,72,false).research_valid);
static_assert(assess_cylinder({3.25,0.,0.,false,false}).state==CylinderState::MissingRadius);
static_assert(assess_cylinder({3.25,.5,.8,true,true}).state==CylinderState::ExploratoryGeometry);
static_assert(assess_cylinder({3.25,.5,.8,true,true}).stretched_diameter_inches==1.6);
static_assert(assess_stretch({3.0,4.5,0.5,0.75,true,true,true,true}).status==StretchStatus::ExploratoryGeometry);
static_assert(assess_stretch({3.0,4.5,0.5,0.75,true,true,true,true}).depth_delta_inches==1.5);
static_assert(assess_stretch({3.0,4.5,0.5,0.75,true,true,true,true}).diameter_delta_inches==0.5);
static_assert(assess_stretch({3.0,4.5,0.5,0.75,true,true,false,true}).status==StretchStatus::MissingMeasurement);
static_assert(solve_radius(3.0,5.0,true).status==TriangleStatus::CalculatedGeometry);
static_assert(solve_radius(3.0,5.0,true).radial_width_inches>3.9999);
static_assert(solve_radius(3.0,5.0,true).radial_width_inches<4.0001);
static_assert(solve_radius(3.0,0.0,false).status==TriangleStatus::MissingSecondMeasure);
static_assert(solve_radius(3.0,2.0,true).status==TriangleStatus::InvalidGeometry);
int main(){return 0;}
