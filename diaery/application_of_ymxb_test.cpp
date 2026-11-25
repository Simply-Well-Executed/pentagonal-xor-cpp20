// SPDX-License-Identifier: 0BSD
// Compile-time, source-preserving DIAERY graph witnesses.
#include "application_of_ymxb.hpp"
using namespace swe::diaery::graph;
constexpr Point a{1.0,0.0},b{0.0,1.0},c{-1.0,0.0};
constexpr Fit circle=fit(a,b,c);
static_assert(circle.shape==Shape::Circle);
static_assert(circle.center.x==0.0 && circle.center.y==0.0);
static_assert(circle.radius_squared==1.0);
constexpr Fit line=fit({0.0,1.0},{1.0,3.0},{2.0,5.0});
static_assert(line.shape==Shape::Line && line.slope==2.0 && line.intercept==1.0);
static_assert(fit({2.0,0.0},{2.0,1.0},{2.0,3.0}).shape==Shape::VerticalLine);
static_assert(fit({1.0,1.0},{1.0,1.0},{1.0,1.0}).shape==Shape::Invalid);
static_assert(fit({1.0e20,0.0},{0.0,1.0},{1.0,0.0}).shape==Shape::Invalid);
static_assert(minting(9.0)>8.969999 && minting(9.0)<8.970001);
static_assert(fold(8.0)==4.0 && project(8.0)==16.0 && quarter(2.0)==8.0);
constexpr Bounds box=view_bounds({1.0,2.0},{3.0,2.0},{2.0,4.0});
static_assert(box.xmax-box.xmin==box.ymax-box.ymin);
static_assert(a.x==1.0 && a.y==0.0); // original witness still intact
// Native DYNAMO line: exact algebraic intercept witnesses.
// Values are selection coordinates in inches, not clinical paired records.
constexpr auto direct=application_of_ymxb(2.0,6.0);
static_assert(direct.valid && direct.slope==-3.0 && direct.intercept==6.0);
static_assert(direct.x_end.x==2.0 && direct.x_end.y==0.0);
static_assert(direct.y_end.x==0.0 && direct.y_end.y==6.0);
static_assert(direct.slope*direct.x_end.x+direct.intercept==0.0);
static_assert(!application_of_ymxb(0.0,6.0).valid);
static_assert(!application_of_ymxb(2.0,0.0).valid);
static_assert(!application_of_ymxb(-1.0,6.0).valid);
static_assert(!application_of_ymxb(1.0e20,6.0).valid);
static_assert(a.x==1.0 && a.y==0.0);
int main(){return 0;}
