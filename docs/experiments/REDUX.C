// SPDX-License-Identifier: 0BSD
/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// Standalone REDUX/1; compile as C++20, not C (MSVC /TP).
namespace swe::redux {
using U = unsigned long long;
static_assert(~U{0} == 18446744073709551615ULL);
inline constexpr U limit = ~U{0};
inline constexpr unsigned capacity = 32;

enum class Status { ok, zero_baseline, invalid_input,
                    invalid_dependency, cycle, overflow };

struct Ratio {
    Status status;
    bool negative;
    U numerator, denominator;
};

// Exact signed magnitude; no floating point or discarded remainder.
constexpr Ratio fraction(U n, U d, bool negative = false) noexcept {
    if (!d) return {Status::zero_baseline, false, 0, 0};
    U a = n, b = d;
    while (b) { U r = a % b; a = b; b = r; }
    return {Status::ok, negative && n != 0, n/a, d/a};
}

constexpr Ratio reduction(U original, U optimized) noexcept {
    if (!original) return {Status::zero_baseline, false, 0, 0};
    return optimized > original
        ? fraction(optimized-original, original, true)
        : fraction(original-optimized, original);
}

// Cancellation is arithmetic only; caller retains occurrence provenance.
constexpr U xor_reduce(U a, U b, U core) noexcept {
    return (a ^ core) ^ (b ^ core);
}

struct Work { Status status; U original, optimized; };

// Reuse common work once, only when the caller establishes equivalence.
constexpr Work factorize(U lanes, U common, U unique_total) noexcept {
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
constexpr Schedule analyze(const Task* tasks, unsigned n) noexcept {
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

constexpr Ratio time_reduction(const Schedule& s) noexcept {
    if (s.status != Status::ok) return {s.status, false, 0, 0};
    if (s.critical_path > s.work)
        return {Status::invalid_input, false, 0, 0};
    return reduction(s.work, s.critical_path);
}

constexpr Ratio ideal_speedup(const Schedule& s) noexcept {
    if (s.status != Status::ok) return {s.status, false, 0, 0};
    if (s.critical_path > s.work)
        return {Status::invalid_input, false, 0, 0};
    return fraction(s.work, s.critical_path);
}
} // namespace swe::redux
namespace redux_test {
using namespace swe::redux;
constexpr Task tasks[]={{3,0},{2,1},{4,0},{1,0}};
constexpr Task cyclic[]={{1,2},{1,1}};
constexpr Task missing[]={{1,2}};
constexpr Task overflow[]={{limit,0},{1,0}};
constexpr Task zero[]={{0,0}};
constexpr auto s=analyze(tasks,4);
constexpr auto r=reduction(4,1);
constexpr auto t=time_reduction(s);
constexpr auto speed=ideal_speedup(s);
constexpr auto f=factorize(4,3,4);
static_assert(r.status==Status::ok && r.numerator==3 && r.denominator==4);
static_assert(s.status==Status::ok && s.work==10 && s.critical_path==5);
static_assert(s.order[0]==0 && s.order[1]==1 && s.finish[1]==5);
static_assert(t.numerator==1 && t.denominator==2);
static_assert(speed.numerator==2 && speed.denominator==1);
static_assert(xor_reduce(0xA5,0x5A,0xFF)==0xFF);
static_assert(f.status==Status::ok && f.original==16 && f.optimized==7);
static_assert(reduction(0,0).status==Status::zero_baseline);
static_assert(reduction(4,5).negative && reduction(4,5).numerator==1);
static_assert(reduction(4,4).denominator==1);
static_assert(analyze(cyclic,2).status==Status::cycle);
static_assert(analyze(missing,1).status==Status::invalid_dependency);
static_assert(analyze(overflow,2).status==Status::overflow);
static_assert(analyze(nullptr,0).status==Status::invalid_input);
static_assert(analyze(tasks,33).status==Status::invalid_input);
static_assert(time_reduction(analyze(zero,1)).status==Status::zero_baseline);
static_assert(factorize(2,limit,0).status==Status::overflow);
static_assert(factorize(0,1,0).status==Status::invalid_input);
}
int main() {
    return redux_test::s.status==swe::redux::Status::ok &&
           redux_test::t.numerator==1 && redux_test::t.denominator==2 ? 0 : 1;
}
