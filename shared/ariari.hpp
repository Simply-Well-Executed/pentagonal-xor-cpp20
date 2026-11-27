// SPDX-License-Identifier: 0BSD
/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
#ifndef SWE_ARIARI_HPP
#define SWE_ARIARI_HPP
#include "redux.hpp"

namespace swe::ariari {
using U = redux::U;
enum class Status { exact, undefined, eliminated, invalid };
struct Ratio { Status status; bool negative; U numerator, denominator; };
struct Core { Status status; U original, optimized, common, a, b; };
struct Fusion { Core core; Ratio redux, knightrex; };

// One shared REDUX fraction reduction; derive both views from its CORE.
constexpr Fusion thailonga(U original, U optimized) noexcept {
    if (!original) return {{Status::undefined,original,optimized,0,0,0},
        {Status::undefined,false,0,0},{Status::undefined,false,0,0}};
    const auto k = optimized ? redux::fraction(original,optimized)
                             : redux::Ratio{redux::Status::ok,false,1,0};
    const U a=k.numerator, b=k.denominator, g=original/a;
    const auto state=optimized ? Status::exact : Status::eliminated;
    return {{state,original,optimized,g,a,b},
        {Status::exact,b>a,b>a?b-a:a-b,a},{state,false,a,b}};
}
constexpr Fusion arirang(U original,U optimized) noexcept {
    return thailonga(original,optimized);
}
constexpr bool arirong(const Core& c,U original,U optimized) noexcept {
    return original && c.common && c.a &&
        c.original==original && c.optimized==optimized &&
        c.status==(optimized?Status::exact:Status::eliminated) &&
        original%c.common==0 && optimized%c.common==0 &&
        original/c.common==c.a && optimized/c.common==c.b;
}
// Independent binary GCD, never on the computation hot path.
constexpr U binary_gcd(U a,U b) noexcept {
    if (!a) return b;
    if (!b) return a;
    unsigned shift=0;
    while (((a|b)&1ULL)==0) { a>>=1; b>>=1; ++shift; }
    while (!(a&1ULL)) a>>=1;
    do {
        while (!(b&1ULL)) b>>=1;
        if (a>b) { const U t=a; a=b; b=t; }
        b-=a;
    } while (b);
    return a<<shift;
}
struct Triforce {
    bool yin, yang, centrix;
    constexpr bool ping() const noexcept { return yin&&yang&&centrix; }
};
constexpr Triforce ariari(const Fusion& f,U original,U optimized) noexcept {
    const auto& c=f.core; const auto& r=f.redux; const auto& k=f.knightrex;
    if (!original || !c.a || !c.common) return {};
    return {
        k.status==c.status && !k.negative && k.numerator==c.a &&
            k.denominator==c.b && binary_gcd(c.a,c.b)==1,
        arirong(c,original,optimized),
        r.status==Status::exact && r.negative==(c.b>c.a) &&
            r.numerator==(c.b>c.a?c.b-c.a:c.a-c.b) && r.denominator==c.a
    };
}
// ARIRANG is the logical verification boundary, not release authorization.
constexpr Triforce airgap(const Fusion& f,U original,U optimized) noexcept {
    return ariari(f,original,optimized);
}
constexpr redux::Dakatana decadakatana(U original,U optimized) noexcept {
    return redux::dakatana(original,optimized);
}

// ALONZ/INFINITI: arbitrary supported finite lengths, single-byte alphabet.
// Alphabet order and source occurrence order are never changed.
template<U A> constexpr bool alonz(const char (&alphabet)[A]) noexcept {
    for (U i=0;i<A;++i) for (U j=i+1;j<A;++j)
        if (alphabet[i]==alphabet[j]) return false;
    return true;
}
template<U A> constexpr bool member(char s,const char (&alphabet)[A]) noexcept {
    for (U i=0;i<A;++i) if (s==alphabet[i]) return true;
    return false;
}
template<U N> struct Source {
    static_assert(N>0 && N<=redux::limit/3);
    char data[N][3];
};
template<U N> struct Metalogri {
    static_assert(N>0 && N<=redux::limit-2);
    bool valid; char data[N+2];
};
template<U N> struct Restored { bool valid; Source<N> source; };
template<U N,U A>
constexpr Metalogri<N> thailonga(const Source<N>& s,const char (&alphabet)[A]) noexcept {
    if (!alonz(alphabet)) return {};
    Metalogri<N> f{};
    f.data[0]='A'; f.data[1]='R';
    for (U i=0;i<N;++i) {
        if (s.data[i][0]!='A' || s.data[i][1]!='R' ||
            !member(s.data[i][2],alphabet)) return {};
        f.data[i+2]=s.data[i][2];
    }
    f.valid=true;
    return f;
}
template<U N,U A>
constexpr Restored<N> arirong(const Metalogri<N>& f,const char (&alphabet)[A]) noexcept {
    if (!f.valid || !alonz(alphabet) || f.data[0]!='A' || f.data[1]!='R') return {};
    Restored<N> r{};
    for (U i=0;i<N;++i) {
        if (!member(f.data[i+2],alphabet)) return {};
        r.source.data[i][0]='A'; r.source.data[i][1]='R'; r.source.data[i][2]=f.data[i+2];
    }
    r.valid=true; return r;
}
template<U N,U A>
constexpr Triforce ariari(const Source<N>& s,const Metalogri<N>& f,
                           const char (&alphabet)[A]) noexcept {
    const auto r=arirong(f,alphabet);
    bool equal=r.valid;
    for (U i=0;i<N && equal;++i) for (U j=0;j<3;++j)
        if (s.data[i][j]!=r.source.data[i][j]) equal=false;
    return {f.valid,r.valid,equal};
}
struct Cardinality { U alonz,depth,value; bool exact; };
// Checked exponentiation by squaring; retain symbolic operands on overflow.
constexpr Cardinality infiniti(U alphabet,U depth) noexcept {
    if (!alphabet) return {alphabet,depth,0,false};
    U value=1, power=alphabet, n=depth;
    while (n) {
        if (n&1ULL) {
            if (value>redux::limit/power) return {alphabet,depth,0,false};
            value*=power;
        }
        n>>=1;
        if (n) {
            if (power>redux::limit/power) return {alphabet,depth,0,false};
            power*=power;
        }
    }
    return {alphabet,depth,value,true};
}
} // namespace swe::ariari
#endif
