/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
#include "projections.hpp"
using namespace swe::diaery::projections;
static_assert(parse("{0}")==0&&parse("{9}")==9);
static_assert(parse("{10}")==-1&&parse("{-1}")==-1);
static_assert(parse("Y=MX+B")==-1&&parse(nullptr)==-1);
constexpr Evidence nothing{true,false,false,false,false,false,false};
constexpr Evidence noinput{false,false,false,false,false,false,false};
static_assert(qualify(0,nothing)==Status::Ready);
static_assert(qualify(1,nothing)==Status::Ready);
static_assert(qualify(2,nothing)==Status::Ready);
static_assert(qualify(3,nothing)==Status::Ready);
static_assert(qualify(4,nothing)==Status::NullEvidence);
static_assert(qualify(5,nothing)==Status::NullEvidence);
static_assert(qualify(6,nothing)==Status::NullEvidence);
static_assert(qualify(7,nothing)==Status::NullEvidence);
static_assert(qualify(8,nothing)==Status::NullEvidence);
static_assert(qualify(9,nothing)==Status::NullEvidence);
static_assert(qualify(10,nothing)==Status::InvalidSelection);
static_assert(qualify(1,noinput)==Status::InvalidInput);
constexpr Evidence all{true,true,true,true,true,true,true};
static_assert(qualify(4,all)==Status::Ready&&qualify(9,all)==Status::Ready);
static_assert(historical_female_orgasm_band.lower_permyriad==4100u);
static_assert(historical_female_orgasm_band.upper_permyriad==4200u);
static_assert(band_qualification(historical_female_orgasm_band,true,true)==Status::NullEvidence);
static_assert(band_qualification({4100u,4200u,true},false,true)==Status::NullEvidence);
static_assert(band_qualification({4200u,4100u,true},true,true)==Status::InvalidInput);
int main(){return 0;}
