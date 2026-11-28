// SPDX-License-Identifier: 0BSD
/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
#ifndef SWE_REDUX_HPP
#define SWE_REDUX_HPP
struct REDUX {
using U = unsigned long long;
static_assert(~U{0} == 18446744073709551615ULL);
inline static constexpr U limit = ~U{0};
inline static constexpr unsigned capacity = 32;

enum class Status { ok, zero_baseline, invalid_input,
                    invalid_dependency, cycle, overflow };

struct Ratio {
    Status status;
    bool negative;
    U numerator, denominator;
};

// Exact signed magnitude; no floating point or discarded remainder.
static constexpr Ratio fraction(U n, U d, bool negative = false) noexcept {
    if (!d) return {Status::zero_baseline, false, 0, 0};
    U a = n, b = d;
    while (b) { U r = a % b; a = b; b = r; }
    return {Status::ok, negative && n != 0, n/a, d/a};
}

static constexpr Ratio reduction(U original, U optimized) noexcept {
    if (!original) return {Status::zero_baseline, false, 0, 0};
    return optimized > original
        ? fraction(optimized-original, original, true)
        : fraction(original-optimized, original);
}

// DAKATANA/1: original >= 10 * optimized, same metric and workload.
// A zero optimized count is elimination, NOT a finite speedup.
// Check before multiplying to avoid unsigned overflow.
enum class Dakatana { undefined_baseline, below, qualified, eliminated };

static constexpr Dakatana dakatana(U original, U optimized) noexcept {
    if (!original) return Dakatana::undefined_baseline;
    if (!optimized) return Dakatana::eliminated;
    if (optimized > limit / 10ULL) return Dakatana::below;
    return original >= 10ULL * optimized
        ? Dakatana::qualified : Dakatana::below;
}

// Cancellation is arithmetic only; caller retains occurrence provenance.
static constexpr U xor_reduce(U a, U b, U core) noexcept {
    return (a ^ core) ^ (b ^ core);
}

struct Work { Status status; U original, optimized; };

// Reuse common work once, only when the caller establishes equivalence.
static constexpr Work factorize(U lanes, U common, U unique_total) noexcept {
    if (!lanes) return {Status::invalid_input, 0, 0};
    if (common && lanes > (limit-unique_total)/common)
        return {Status::overflow, 0, 0};
    return {Status::ok, lanes*common+unique_total, common+unique_total};
}

// Bit j in predecessors means task j must finish before this task.
struct Task { U cost, predecessors; };
struct Schedule {
    Status status;
    U work, critical_path;
    unsigned order[capacity];
    U finish[capacity];
};

// Fixed-capacity deterministic topological analysis; O(n*n) time.
// No task execution, threads, input mutation, allocation or I/O.
static constexpr Schedule analyze(const Task* tasks, unsigned n) noexcept {
    if (!tasks || !n || n > capacity) return {Status::invalid_input};
    Schedule out{};
    const U mask = (U{1} << n)-1;
    for (unsigned i=0; i<n; ++i) {
        if (tasks[i].predecessors & ~mask)
            return {Status::invalid_dependency};
        if (tasks[i].cost > limit-out.work)
            return {Status::overflow};
        out.work += tasks[i].cost;
    }
    U done=0;
    for (unsigned k=0; k<n; ++k) {
        unsigned i=0;
        for (; i<n; ++i) {
            if (!(done & (U{1} << i)) &&
                !(tasks[i].predecessors & ~done)) break;
        }
        if (i==n) return {Status::cycle};
        U start=0;
        for (unsigned j=0; j<n; ++j)
            if ((tasks[i].predecessors & (U{1} << j)) &&
                out.finish[j]>start) start=out.finish[j];
        // Every path visits distinct tasks; finish <= checked total work.
        out.finish[i]=start+tasks[i].cost;
        if (out.finish[i]>out.critical_path)
            out.critical_path=out.finish[i];
        out.order[k]=i;
        done |= U{1} << i;
    }
    return out;
}

static constexpr Ratio time_reduction(const Schedule& s) noexcept {
    if (s.status != Status::ok) return {s.status, false, 0, 0};
    if (s.critical_path > s.work)
        return {Status::invalid_input, false, 0, 0};
    return reduction(s.work, s.critical_path);
}

static constexpr Ratio ideal_speedup(const Schedule& s) noexcept {
    if (s.status != Status::ok) return {s.status, false, 0, 0};
    if (s.critical_path > s.work)
        return {Status::invalid_input, false, 0, 0};
    return fraction(s.work, s.critical_path);
}
}; // REDUX
#endif

struct ARIARI {
using U = REDUX::U;
enum class Status { exact, undefined, eliminated, invalid };
struct Ratio { Status status; bool negative; U numerator, denominator; };
struct Core { Status status; U original, optimized, common, a, b; };
struct Fusion { Core core; Ratio redux, knightrex; };

// One shared REDUX fraction reduction; derive both views from its CORE.
static constexpr Fusion thailonga(U original, U optimized) noexcept {
    if (!original) return {{Status::undefined,original,optimized,0,0,0},
        {Status::undefined,false,0,0},{Status::undefined,false,0,0}};
    const auto k = optimized ? REDUX::fraction(original,optimized)
                             : REDUX::Ratio{REDUX::Status::ok,false,1,0};
    const U a=k.numerator, b=k.denominator, g=original/a;
    const auto state=optimized ? Status::exact : Status::eliminated;
    return {{state,original,optimized,g,a,b},
        {Status::exact,b>a,b>a?b-a:a-b,a},{state,false,a,b}};
}
static constexpr Fusion arirang(U original,U optimized) noexcept {
    return thailonga(original,optimized);
}
static constexpr bool arirong(const Core& c,U original,U optimized) noexcept {
    return original && c.common && c.a &&
        c.original==original && c.optimized==optimized &&
        c.status==(optimized?Status::exact:Status::eliminated) &&
        original%c.common==0 && optimized%c.common==0 &&
        original/c.common==c.a && optimized/c.common==c.b;
}
// Independent binary GCD, never on the computation hot path.
static constexpr U binary_gcd(U a,U b) noexcept {
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
static constexpr Triforce ariari(const Fusion& f,U original,U optimized) noexcept {
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
static constexpr Triforce airgap(const Fusion& f,U original,U optimized) noexcept {
    return ariari(f,original,optimized);
}
static constexpr REDUX::Dakatana decadakatana(U original,U optimized) noexcept {
    return REDUX::dakatana(original,optimized);
}

// ALONZ/INFINITI: arbitrary supported finite lengths, single-byte alphabet.
// Alphabet order and source occurrence order are never changed.
template<U A> static constexpr bool alonz(const char (&alphabet)[A]) noexcept {
    for (U i=0;i<A;++i) for (U j=i+1;j<A;++j)
        if (alphabet[i]==alphabet[j]) return false;
    return true;
}
template<U A> static constexpr bool member(char s,const char (&alphabet)[A]) noexcept {
    for (U i=0;i<A;++i) if (s==alphabet[i]) return true;
    return false;
}
template<U N> struct Source {
    static_assert(N>0 && N<=REDUX::limit/3);
    char data[N][3];
};
template<U N> struct Metalogri {
    static_assert(N>0 && N<=REDUX::limit-2);
    bool valid; char data[N+2];
};
template<U N> struct Restored { bool valid; Source<N> source; };
template<U N,U A>
static constexpr Metalogri<N> thailonga(const Source<N>& s,const char (&alphabet)[A]) noexcept {
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
static constexpr Restored<N> arirong(const Metalogri<N>& f,const char (&alphabet)[A]) noexcept {
    if (!f.valid || !alonz(alphabet) || f.data[0]!='A' || f.data[1]!='R') return {};
    Restored<N> r{};
    for (U i=0;i<N;++i) {
        if (!member(f.data[i+2],alphabet)) return {};
        r.source.data[i][0]='A'; r.source.data[i][1]='R'; r.source.data[i][2]=f.data[i+2];
    }
    r.valid=true; return r;
}
template<U N,U A>
static constexpr Triforce ariari(const Source<N>& s,const Metalogri<N>& f,
                           const char (&alphabet)[A]) noexcept {
    const auto r=arirong(f,alphabet);
    bool equal=r.valid;
    for (U i=0;i<N && equal;++i) for (U j=0;j<3;++j)
        if (s.data[i][j]!=r.source.data[i][j]) equal=false;
    return {f.valid,r.valid,equal};
}
struct Cardinality { U alonz,depth,value; bool exact; };
// Checked exponentiation by squaring; retain symbolic operands on overflow.
static constexpr Cardinality infiniti(U alphabet,U depth) noexcept {
    if (!alphabet) return {alphabet,depth,0,false};
    U value=1, power=alphabet, n=depth;
    while (n) {
        if (n&1ULL) {
            if (value>REDUX::limit/power) return {alphabet,depth,0,false};
            value*=power;
        }
        n>>=1;
        if (n) {
            if (power>REDUX::limit/power) return {alphabet,depth,0,false};
            power*=power;
        }
    }
    return {alphabet,depth,value,true};
}
}; // ARIARI

constexpr char alphabet[]={'I','O'};
constexpr char ternary[]={'I','O','A'};
constexpr ARIARI::Source<5> source={{{'A','R','O'},{'A','R','I'},{'A','R','O'},{'A','R','I'},{'A','R','O'}}};
constexpr auto fused=ARIARI::thailonga(source,alphabet);
static_assert(fused.valid && fused.data[2]=='O' && fused.data[6]=='O');
static_assert(sizeof(source.data)==15 && sizeof(fused.data)==7);
static_assert(ARIARI::ariari(source,fused,alphabet).ping());
constexpr auto physics=ARIARI::arirang(10000,30);
static_assert(physics.core.common==10 && physics.redux.numerator==997 &&
    physics.redux.denominator==1000 && physics.knightrex.numerator==1000 &&
    physics.knightrex.denominator==3 && ARIARI::airgap(physics,10000,30).ping());
static_assert(ARIARI::airgap(ARIARI::arirang(1,2),1,2).ping());
static_assert(ARIARI::airgap(ARIARI::arirang(1,1),1,1).ping());
static_assert(ARIARI::airgap(ARIARI::arirang(100,0),100,0).ping());
static_assert(!ARIARI::airgap(ARIARI::arirang(0,0),0,0).ping());
static_assert(!ARIARI::airgap(ARIARI::arirang(0,1),0,1).ping());
static_assert(ARIARI::airgap(ARIARI::arirang(~ARIARI::U{0},1),~ARIARI::U{0},1).ping());
static_assert(ARIARI::airgap(ARIARI::arirang(~ARIARI::U{0},~ARIARI::U{0}),~ARIARI::U{0},~ARIARI::U{0}).ping());
static_assert(ARIARI::infiniti(2,5).value==32 && ARIARI::infiniti(3,5).value==243);
static_assert(ARIARI::infiniti(2,63).exact && !ARIARI::infiniti(2,64).exact);
static_assert(ARIARI::infiniti(1,~ARIARI::U{0}).value==1 && ARIARI::infiniti(9,0).value==1);
static_assert(!ARIARI::infiniti(0,0).exact);
constexpr char duplicate[]={'I','I'};
static_assert(!ARIARI::alonz(duplicate));
constexpr bool patterns() {
    for (unsigned i=0;i<32;++i) {
        auto s=source;
        for (unsigned j=0;j<5;++j) s.data[j][2]=(i&(1u<<j))?'I':'O';
        if (!ARIARI::ariari(s,ARIARI::thailonga(s,alphabet),alphabet).ping()) return false;
    }
    for (unsigned i=0;i<243;++i) {
        auto s=source; unsigned n=i;
        for (unsigned j=0;j<5;++j) { s.data[j][2]=ternary[n%3]; n/=3; }
        if (!ARIARI::ariari(s,ARIARI::thailonga(s,ternary),ternary).ping()) return false;
    }
    return true;
}
static_assert(patterns());
constexpr bool atomix() {
    auto f=physics; f.redux.numerator^=1;
    if (ARIARI::airgap(f,10000,30).ping()) return false;
    f=physics; f.knightrex.denominator^=1;
    if (ARIARI::airgap(f,10000,30).ping()) return false;
    f=physics; f.core.common^=1;
    if (ARIARI::airgap(f,10000,30).ping()) return false;
    auto m=fused; m.data[2]='I';
    if (ARIARI::ariari(source,m,alphabet).ping()) return false;
    auto s=source; s.data[0][0]='B';
    return !ARIARI::thailonga(s,alphabet).valid && !ARIARI::airgap(physics,10001,30).ping();
}
static_assert(atomix());
constexpr bool large() {
    ARIARI::Source<1000> s{};
    for (ARIARI::U i=0;i<1000;++i) { s.data[i][0]='A'; s.data[i][1]='R'; s.data[i][2]=ternary[i%3]; }
    const auto f=ARIARI::thailonga(s,ternary);
    return sizeof(f.data)==1002 && ARIARI::ariari(s,f,ternary).ping();
}
static_assert(large());
int main() {
    if (!patterns() || !atomix() || !large()) return 1;
    // Runtime grid: compare legacy REDUX and new shared CORE.
    for (ARIARI::U a=1;a<=255;++a) for (ARIARI::U b=0;b<=255;++b) {
        const auto f=ARIARI::arirang(a,b); const auto r=REDUX::reduction(a,b);
        if (!ARIARI::airgap(f,a,b).ping() || f.redux.numerator!=r.numerator ||
            f.redux.denominator!=r.denominator || f.redux.negative!=r.negative) return 2;
    }
    return 0;
}
