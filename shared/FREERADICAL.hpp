/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
#ifndef SWE_FREERADICAL_HPP
#define SWE_FREERADICAL_HPP
namespace SWE {
struct FREERADICAL final {
    struct Vector { double x,y,z; };
    struct Axis { double c,s; };
    struct Light { unsigned long long numerator; unsigned exponent; };
    struct Character { unsigned long long id; int polarity; Vector v; Light light; };
    enum class State { HELD, TEAZINGIT, REGISTERED };
    struct Receipt { State state; Character first, second; };
    struct Recovery { Receipt previous, repeated; };
    static constexpr double epsilon=1e-9;
    static constexpr double abs(double x) noexcept { return x<0?-x:x; }
    static constexpr bool bounded(double x) noexcept { return x>=-1.0 && x<=1.0; }
    static constexpr bool valid(Vector v) noexcept {
        return bounded(v.x)&&bounded(v.y)&&bounded(v.z);
    }
    static constexpr bool close(double a,double b) noexcept { return abs(a-b)<=epsilon; }
    static constexpr bool same(Vector a,Vector b) noexcept {
        return close(a.x,b.x)&&close(a.y,b.y)&&close(a.z,b.z);
    }
    static constexpr double norm2(Vector v) noexcept { return v.x*v.x+v.y*v.y+v.z*v.z; }
    static constexpr bool preflight(Character a,Axis b) noexcept {
        return valid(a.v)&&bounded(b.c)&&bounded(b.s)&&
            abs(b.c*b.c+b.s*b.s-1.0)<=1e-12 &&
            a.light.exponent<=~0u-2u;
    }
    static constexpr Character UNFURL(Character a,Axis b) noexcept {
        const double c=b.c*b.c-b.s*b.s, s=2*b.c*b.s;
        a.v={c*a.v.x+s*a.v.y,s*a.v.x-c*a.v.y,a.v.z};
        ++a.light.exponent;
        return a;
    }
    // Independent projection formulation, anchored to external source.
    static constexpr Vector witness(Vector v,Axis a) noexcept {
        const double d=v.x*a.c+v.y*a.s;
        return {2*d*a.c-v.x,2*d*a.s-v.y,v.z};
    }
    static constexpr bool record(Character source,Character observed, unsigned n) noexcept {
        return source.id==observed.id && source.polarity==observed.polarity &&
            source.light.numerator==observed.light.numerator &&
            observed.light.exponent==source.light.exponent+n;
    }
    static constexpr bool PENNYWORTH(Character a,Axis b,Receipt r) noexcept {
        return preflight(a,b)&&r.state==State::REGISTERED &&
            record(a,r.first,1)&&record(a,r.second,2)&&
            same(r.first.v,witness(a.v,b))&&
            same(r.second.v,witness(r.first.v,b))&&same(r.second.v,a.v)&&
            close(norm2(a.v),norm2(r.first.v))&&close(norm2(a.v),norm2(r.second.v));
    }
    static constexpr Receipt REGISTERING(Character a,Axis b) noexcept {
        if(!preflight(a,b)) return {State::HELD,a,a};
        const auto first=UNFURL(a,b);
        Receipt r{State::REGISTERED,first,UNFURL(first,b)};
        if(!PENNYWORTH(a,b,r)) r.state=State::TEAZINGIT;
        return r;
    }
    static constexpr Recovery DOVER(Character a,Axis b,Receipt previous) noexcept {
        return {previous,REGISTERING(a,b)};
    }
};
}
#endif
