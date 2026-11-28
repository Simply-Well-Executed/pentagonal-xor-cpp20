/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
#pragma once
namespace SWE::PERSPECTIVE {
enum class COLOR : unsigned { PURPLE, BLUE, RED, GREEN, GOLD };
enum class WITNESS : unsigned { ORIGINAL, BLACK, DARK_GRAY, PURE_GRAY };
template<class E,class M,class A,class I>
struct ARIRANG final {
    const E EVIDENCE;
    const M MIRROR;
    const A ALTERNATIVES;
    const I IMPLICATIONS;
};
template<class S> struct PYRAMID final {
    const S SELF;
    static constexpr unsigned POSE=5u;
    static constexpr COLOR AGGREGATE=COLOR::PURPLE;
};
// Exact coordinate: integer + numerator / sqrt(2).
struct RADICAL { int integer, numerator; };
struct POINT { int x,y; };
struct LANDING { RADICAL x,y; bool valid; };
struct ELECTRIC_SLIDE {
    static constexpr unsigned FULL=18u, HALF=9u;
    static_assert(HALF*2u==FULL);
};
constexpr LANDING ROTATE(POINT p,WITNESS layer) noexcept {
    // Five by five centered board; validate before arithmetic.
    if(p.x < -2 || p.x > 2 || p.y < -2 || p.y > 2)
        return {{0,0},{0,0},false};
    switch(layer) {
    case WITNESS::ORIGINAL: return {{p.x,0},{p.y,0},true};
    case WITNESS::BLACK: return {{0,p.x-p.y},{0,p.x+p.y},true};
    case WITNESS::DARK_GRAY: return {{-p.y,0},{p.x,0},true};
    case WITNESS::PURE_GRAY: return {{0,-p.x-p.y},{1,p.x-p.y},true};
    }
    return {{0,0},{0,0},false};
}
// Exact comparison to integer + 1/2, using squared magnitudes.
// Inputs restricted to the bounded board's radical coefficients.
constexpr bool ABOVE_HALF(RADICAL v,int k) noexcept {
    const int a=2*(v.integer-k)-1, b=2*v.numerator;
    if(a>=0 && b>=0) return true;
    if(a<0 && b<=0) return false;
    return a>=0 ? 2*a*a>b*b : b*b>2*a*a;
}
struct CELL { int row,column; bool valid; };
constexpr CELL PROJECT(LANDING p) noexcept {
    if(!p.valid || p.x.integer < -2 || p.x.integer > 2 ||
        p.y.integer < -2 || p.y.integer > 2 ||
        p.x.numerator < -4 || p.x.numerator > 4 ||
        p.y.numerator < -4 || p.y.numerator > 4)
        return {0,0,false};
    int x=-5,y=-5;
    for(int k=-5;k<5;++k) {
        if(ABOVE_HALF(p.x,k)) ++x;
        if(ABOVE_HALF(p.y,k)) ++y;
    }
    return {y+2,x+2,x>=-2 && x<=2 && y>=-2 && y<=2};
}
constexpr POINT SEEDS[3]={{1,-1},{0,2},{-2,2}};
constexpr ARIRANG self{1u,2u,3u,3u};
constexpr PYRAMID pyramid{self};
static_assert(pyramid.POSE==5u);
static_assert(PROJECT(ROTATE(SEEDS[0],WITNESS::BLACK)).column==3);
static_assert(PROJECT(ROTATE(SEEDS[0],WITNESS::DARK_GRAY)).row==3);
static_assert(PROJECT(ROTATE(SEEDS[0],WITNESS::PURE_GRAY)).row==4);
static_assert(PROJECT(ROTATE(SEEDS[2],WITNESS::PURE_GRAY)).row==0);
static_assert(!ROTATE({3,0},WITNESS::ORIGINAL).valid);
}
