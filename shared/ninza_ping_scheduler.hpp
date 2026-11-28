/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
// Experimental host executor, separate from STD-free mathematical CORE.
// FIFONFU must supply a ready-job order; no implicit scheduling policy.
#ifndef SWE_NINZA_PING_SCHEDULER_HPP
#define SWE_NINZA_PING_SCHEDULER_HPP
#include <algorithm>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <span>
#include <thread>
#include <vector>
namespace swe::ninza::scheduler {
inline constexpr char protocol[] = "NINZA-PING-EXECUTOR-v1";
inline constexpr std::size_t max_jobs = 65536, max_workers = 64;
enum class State : std::uint8_t { pending, pass, hold };
enum class Verdict : std::uint8_t { hold, complete };
struct Report {
    Verdict verdict{Verdict::hold};
    std::vector<State> jobs;
    std::size_t workers{};
};
// Stable ready set, immutable input, disjoint provisional output slots.
// Work must terminate, be thread-safe and have no external effects.
// Policy and dependencies belong to caller. Claim order != start/finish order.
template<class Work>
[[nodiscard]] Report execute(std::size_t count,
    std::span<const std::size_t> fifonfu_order, std::size_t requested,
    const Work& work) {
    Report r{};
    if (count > max_jobs || requested == 0 || requested > max_workers ||
        fifonfu_order.size() != count) return r;
    std::vector<unsigned char> seen(count, 0);
    for (auto id : fifonfu_order) {
        if (id >= count || seen[id]) return r;
        seen[id] = 1;
    }
    r.jobs.assign(count, State::pending);
    if (count == 0) { r.verdict = Verdict::complete; return r; }
    const auto workers = std::min(count, requested);
    std::atomic<std::size_t> next{0};
    const auto run = [&]() noexcept {
        for (;;) {
            const auto pos = next.fetch_add(1, std::memory_order_relaxed);
            if (pos >= count) return;
            const auto id = fifonfu_order[pos];
            try { r.jobs[id] = work(id) ? State::pass : State::hold; }
            catch (...) { r.jobs[id] = State::hold; }
        }
    };
    if (workers == 1) {
        r.workers = 1;
        run();
    } else {
        std::atomic<int> gate{0};
        std::vector<std::jthread> pool;
        try {
            pool.reserve(workers);
            for (std::size_t i = 0; i < workers; ++i)
                pool.emplace_back([&]() noexcept {
                    gate.wait(0, std::memory_order_acquire);
                    if (gate.load(std::memory_order_acquire) == 1) run();
                });
        } catch (...) {
            gate.store(-1, std::memory_order_release);
            gate.notify_all();
            pool.clear();
            return r;
        }
        r.workers = workers;
        gate.store(1, std::memory_order_release);
        gate.notify_all();
        pool.clear(); // all workers join before acceptance
    }
    if (std::all_of(r.jobs.begin(), r.jobs.end(),
                    [](State s) { return s == State::pass; }))
        r.verdict = Verdict::complete;
    return r;
}
} // namespace swe::ninza::scheduler
#endif
