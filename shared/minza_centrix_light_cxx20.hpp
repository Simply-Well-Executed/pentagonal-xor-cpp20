/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
// Copyright (c) 2026 Paul Allen Cheek
// MINZA v12: standalone, source-preserving geometric light reference.
#pragma once
#include <cstddef>
#include <cstdint>
namespace swe::minza::centrix {
inline constexpr unsigned tile_count = 9;
inline constexpr double default_exposure = 1.0;
inline constexpr double minimum_exposure = 0.25;
inline constexpr double maximum_exposure = 2.50;
inline constexpr double neighborhood_decay = 0.58;
inline constexpr char protocol[] = "MINZA-CENTRIX-CSS3D-v12";
struct Axis { double x{},y{},z{}; };
inline constexpr Axis center{0,0,0};
[[nodiscard]] constexpr bool valid_tile(unsigned i) noexcept { return i < tile_count; }
[[nodiscard]] constexpr unsigned mirror(unsigned index) noexcept {
    return valid_tile(index) ? (index/3u)*3u + 2u - index%3u : tile_count;
}
[[nodiscard]] constexpr unsigned distance(unsigned a,unsigned b) noexcept {
    if(!valid_tile(a)||!valid_tile(b))return tile_count;
    unsigned ra=a/3u,rb=b/3u,ca=a%3u,cb=b%3u;
    return (ra>rb?ra-rb:rb-ra)+(ca>cb?ca-cb:cb-ca);
}
[[nodiscard]] constexpr double clamp(double x,double lo,double hi) noexcept {
    return x<lo?lo:(x>hi?hi:x);
}
[[nodiscard]] constexpr double radiance(unsigned selected,unsigned target,double exposure) noexcept {
    if(!valid_tile(selected)||!valid_tile(target))return 0.0;
    double weight=1.0;
    for(unsigned d=0;d<distance(selected,target);++d)weight*=neighborhood_decay;
    return clamp(clamp(exposure,minimum_exposure,maximum_exposure)*(0.40+0.60*weight),0.07,2.70);
}
static_assert(mirror(0)==2 && mirror(1)==1 && mirror(2)==0);
static_assert(mirror(3)==5 && mirror(4)==4 && mirror(8)==6);
static_assert(mirror(mirror(0))==0 && mirror(mirror(8))==8);
static_assert(center.x==0 && center.y==0 && center.z==0);
static_assert(radiance(4,4,1.0)==1.0);
static_assert(radiance(4,1,1.0)==0.748);
} // namespace swe::minza::centrix
