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

constexpr REDUX::Task tasks[]={{3,0},{2,1},{4,0},{1,0}};
constexpr REDUX::Task cyclic[]={{1,2},{1,1}};
constexpr REDUX::Task missing[]={{1,2}};
constexpr REDUX::Task overflow[]={{REDUX::limit,0},{1,0}};
constexpr REDUX::Task zero[]={{0,0}};
constexpr auto s=REDUX::analyze(tasks,4);
constexpr auto r=REDUX::reduction(4,1);
constexpr auto t=REDUX::time_reduction(s);
constexpr auto speed=REDUX::ideal_speedup(s);
constexpr auto f=REDUX::factorize(4,3,4);
static_assert(r.status==REDUX::Status::ok && r.numerator==3 && r.denominator==4);
static_assert(s.status==REDUX::Status::ok && s.work==10 && s.critical_path==5);
static_assert(s.order[0]==0 && s.order[1]==1 && s.finish[1]==5);
static_assert(t.numerator==1 && t.denominator==2);
static_assert(speed.numerator==2 && speed.denominator==1);
static_assert(REDUX::xor_reduce(0xA5,0x5A,0xFF)==0xFF);
static_assert(f.status==REDUX::Status::ok && f.original==16 && f.optimized==7);
static_assert(REDUX::reduction(0,0).status==REDUX::Status::zero_baseline);
static_assert(REDUX::reduction(4,5).negative && REDUX::reduction(4,5).numerator==1);
static_assert(REDUX::reduction(4,4).denominator==1);
static_assert(REDUX::analyze(cyclic,2).status==REDUX::Status::cycle);
static_assert(REDUX::analyze(missing,1).status==REDUX::Status::invalid_dependency);
static_assert(REDUX::analyze(overflow,2).status==REDUX::Status::overflow);
static_assert(REDUX::analyze(nullptr,0).status==REDUX::Status::invalid_input);
static_assert(REDUX::analyze(tasks,33).status==REDUX::Status::invalid_input);
static_assert(REDUX::time_reduction(REDUX::analyze(zero,1)).status==REDUX::Status::zero_baseline);
static_assert(REDUX::factorize(2,REDUX::limit,0).status==REDUX::Status::overflow);
static_assert(REDUX::factorize(0,1,0).status==REDUX::Status::invalid_input);
static_assert(REDUX::dakatana(10,1)==REDUX::Dakatana::qualified);
static_assert(REDUX::dakatana(9,1)==REDUX::Dakatana::below);
static_assert(REDUX::dakatana(10000,30)==REDUX::Dakatana::qualified);
static_assert(REDUX::dakatana(141,24)==REDUX::Dakatana::below);
static_assert(REDUX::dakatana(98,43)==REDUX::Dakatana::below);
static_assert(REDUX::dakatana(0,0)==REDUX::Dakatana::undefined_baseline);
static_assert(REDUX::dakatana(0,1)==REDUX::Dakatana::undefined_baseline);
static_assert(REDUX::dakatana(1,0)==REDUX::Dakatana::eliminated);
static_assert(REDUX::dakatana(1,2)==REDUX::Dakatana::below);
static_assert(REDUX::dakatana(REDUX::limit,REDUX::limit/10ULL)==REDUX::Dakatana::qualified);
static_assert(REDUX::dakatana(REDUX::limit,REDUX::limit/10ULL+1)==REDUX::Dakatana::below);
static_assert(REDUX::dakatana(REDUX::limit,REDUX::limit)==REDUX::Dakatana::below);
static_assert(REDUX::reduction(10000,30).numerator==997);
static_assert(REDUX::reduction(10000,30).denominator==1000);
static_assert(REDUX::fraction(10000,30).numerator==1000);
static_assert(REDUX::fraction(10000,30).denominator==3);
int main() {
    return s.status==REDUX::Status::ok &&
           t.numerator==1 && t.denominator==2 ? 0 : 1;
}
