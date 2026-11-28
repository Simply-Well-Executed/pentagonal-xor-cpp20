/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
#include "thailonga.hpp"
using namespace swe::diaery::thailonga;
constexpr Source good{5.57,4.81,3.25,true,true,true};
constexpr auto frame=build(good,1000000000000001ULL);
static_assert(frame.eligible&&frame.within_sample_ranges&&frame.intercept.valid);
static_assert(frame.original.depth_inches==3.25);
static_assert(frame.projection_status[0]==pr::Status::Ready);
static_assert(frame.projection_status[1]==pr::Status::Ready);
static_assert(frame.projection_status[2]==pr::Status::Ready);
static_assert(frame.projection_status[3]==pr::Status::Ready);
static_assert(frame.projection_status[4]==pr::Status::NullEvidence);
static_assert(frame.projection_status[5]==pr::Status::NullEvidence);
static_assert(frame.projection_status[6]==pr::Status::NullEvidence);
static_assert(frame.projection_status[7]==pr::Status::NullEvidence);
static_assert(frame.projection_status[8]==pr::Status::NullEvidence);
static_assert(frame.projection_status[9]==pr::Status::NullEvidence);
static_assert(frame.orgasm_band_status==pr::Status::NullEvidence);
static_assert(frame.time.exact_seconds==1ULL);
static_assert(frame.time.remainder_femtoseconds==1ULL);
constexpr auto invalid=build({0.,4.81,3.25,true,true,true},0ULL);
static_assert(!invalid.eligible&&!invalid.intercept.valid);
static_assert(invalid.projection_status[1]==pr::Status::InvalidInput);
static_assert(invalid.x.filtrex.disposition==rs::Disposition::ExcludedZero);
static_assert(invalid.original.length_inches==0.);
constexpr auto missing=build({5.57,4.81,3.25,true,true,false},0ULL);
static_assert(!missing.eligible);
static_assert(missing.z.filtrex.disposition==rs::Disposition::ExcludedMissing);
constexpr auto small=build({0.244,0.1978,3.25,true,true,true},0ULL);
static_assert(small.eligible&&!small.within_sample_ranges);
static_assert(small.x.filtrox.position==fx::Position::BelowObserved);
static_assert(small.y.filtrox.position==fx::Position::BelowObserved);
int main(){return 0;}
