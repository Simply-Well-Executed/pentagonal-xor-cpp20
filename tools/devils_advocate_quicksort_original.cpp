// SPDX-License-Identifier: 0BSD
// Copyright (c) 2026 Paul Allen Cheek
#include <algorithm>
#include <array>
#include <bit>
#include <chrono>
#include <cstdint>
#include <format>
#include <iostream>
#include <limits>
#include <span>
#include <string_view>
#include <utility>
#include <vector>

namespace sort_audit {

enum class SortVerdict : uint8_t {
    OK_PRIMARY_PATH = 0,
    OK_ADVERSARIAL_FALLBACK_TRIGGERED,
    ERR_OVERFLOW_64,
    ERR_DEPTH_EXHAUSTED,
    ERR_UNSORTED_INVARIANT,
    ERR_MULTISET_CORRUPTION
};

constexpr std::string_view verdict_name(SortVerdict v) noexcept {
    switch (v) {
        case SortVerdict::OK_PRIMARY_PATH:                   return "OK_PRIMARY_PATH";
        case SortVerdict::OK_ADVERSARIAL_FALLBACK_TRIGGERED: return "OK_ADVERSARIAL_FALLBACK_TRIGGERED";
        case SortVerdict::ERR_OVERFLOW_64:                   return "ERR_OVERFLOW_64";
        case SortVerdict::ERR_DEPTH_EXHAUSTED:               return "ERR_DEPTH_EXHAUSTED";
        case SortVerdict::ERR_UNSORTED_INVARIANT:            return "ERR_UNSORTED_INVARIANT";
        case SortVerdict::ERR_MULTISET_CORRUPTION:           return "ERR_MULTISET_CORRUPTION";
    }
    return "ERR_UNKNOWN";
}

// Deterministic audit record with checked signed 64-bit counters & provenance
struct SortTelemetry {
    SortVerdict verdict = SortVerdict::OK_PRIMARY_PATH;
    int64_t element_count = 0;
    int64_t comparisons = 0;
    int64_t swaps = 0;
    int64_t max_recursion_depth = 0;
    int64_t fallback_triggers = 0;
    uint64_t pre_multiset_hash = 0;   // Commutative XOR+SUM scalar fingerprint
    uint64_t post_multiset_hash = 0;
    uint64_t order_witness = 0;       // Order-dependent final sequence hash
    double elapsed_us = 0.0;

    [[nodiscard]] bool passed() const noexcept {
        return verdict == SortVerdict::OK_PRIMARY_PATH ||
               verdict == SortVerdict::OK_ADVERSARIAL_FALLBACK_TRIGGERED;
    }

    [[nodiscard]] std::string report(std::string_view label) const {
        return std::format(
            "[{:<22}] verdict={:<33} | n={:<6} | cmp={:<9} | depth={:<5} | fallback={} | multiset={:016x}=={:016x} | time={:.1f}us",
            label, verdict_name(verdict), element_count, comparisons,
            max_recursion_depth, fallback_triggers,
            pre_multiset_hash, post_multiset_hash, elapsed_us
        );
    }
};

// Commutative order-independent fingerprint: proves no elements were lost, duplicated, or mutated
[[nodiscard]] inline uint64_t compute_multiset_fingerprint(std::span<const int64_t> data) noexcept {
    uint64_t xor_acc = 0;
    uint64_t sum_acc = 0;
    for (int64_t v : data) {
        uint64_t u = static_cast<uint64_t>(v);
        // SplitMix64 mixing so colliding values don't cancel easily
        u ^= u >> 30;
        u *= 0xBF58476D1CE4E5B9ULL;
        u ^= u >> 27;
        u *= 0x94D049BB133111EBULL;
        u ^= u >> 31;
        xor_acc ^= u;
        sum_acc += u;
    }
    return xor_acc ^ std::rotl(sum_acc, 19);
}

// Order-dependent sequence witness
[[nodiscard]] inline uint64_t compute_order_witness(std::span<const int64_t> data) noexcept {
    uint64_t h = 0x9E3779B97F4A7C15ULL;
    for (int64_t v : data) {
        h = std::rotl(h ^ static_cast<uint64_t>(v), 7) * 0xBF58476D1CE4E5B9ULL;
    }
    return h;
}

// =============================================================================
// 1. NAIVE QUICKSORT (NO DEVIL'S ADVOCATE)
//    - Assumes benign input, picks last element as pivot (Lomuto)
//    - No depth budget, no skew monitor, no post-condition audit
//    - Degrades to O(N^2) comparisons and O(N) stack depth on adversarial inputs
// =============================================================================
class NaiveQuicksort {
public:
    static SortTelemetry sort(std::span<int64_t> data) {
        SortTelemetry t{};
        t.element_count = static_cast<int64_t>(data.size());
        t.pre_multiset_hash = compute_multiset_fingerprint(data);

        auto start = std::chrono::steady_clock::now();
        if (t.element_count > 1) {
            quicksort_impl(data, 0, t.element_count - 1, 1, t);
        }
        auto end = std::chrono::steady_clock::now();

        t.elapsed_us = std::chrono::duration<double, std::micro>(end - start).count();
        t.post_multiset_hash = compute_multiset_fingerprint(data);
        t.order_witness = compute_order_witness(data);
        t.verdict = SortVerdict::OK_PRIMARY_PATH; // Blindly trusts itself
        return t;
    }

private:
    static void quicksort_impl(std::span<int64_t> a, int64_t lo, int64_t hi, int64_t depth, SortTelemetry& t) {
        if (lo >= hi) return;
        if (depth > t.max_recursion_depth) t.max_recursion_depth = depth;

        // Naive pivot: last element (catastrophic on sorted, reverse-sorted, or all-equal inputs)
        int64_t pivot = a[static_cast<size_t>(hi)];
        int64_t i = lo;
        for (int64_t j = lo; j < hi; ++j) {
            ++t.comparisons;
            if (a[static_cast<size_t>(j)] < pivot) {
                std::swap(a[static_cast<size_t>(i)], a[static_cast<size_t>(j)]);
                ++t.swaps;
                ++i;
            }
        }
        std::swap(a[static_cast<size_t>(i)], a[static_cast<size_t>(hi)]);
        ++t.swaps;

        quicksort_impl(a, lo, i - 1, depth + 1, t);
        quicksort_impl(a, i + 1, hi, depth + 1, t);
    }
};

// =============================================================================
// 2. "DEVIL'S ADVOCATE" QUICKSORT (ADVERSARIAL-FIRST DEFENSIVE DESIGN)
//    Philosophy: Assume the input was crafted to break your assumptions.
//    - Ninther (median-of-medians-of-3) pivot selection resists anti-quicksort killers
//    - 3-way Dutch National Flag partitioning collapses duplicate-heavy inputs to O(N)
//    - Checked 64-bit recursion budget (2 * floor(log2(N))) triggers deterministic
//      heapsort fallback before stack exhaustion or O(N^2) blowup can occur
//    - Tail-call elimination on larger partition bounds stack depth to O(log N)
//    - Post-sort scalar audit verifies both monotonic order and multiset conservation
// =============================================================================
class DevilsAdvocateQuicksort {
public:
    static SortTelemetry sort(std::span<int64_t> data) {
        SortTelemetry t{};
        if (data.size() > static_cast<size_t>(std::numeric_limits<int64_t>::max() / 2)) {
            t.verdict = SortVerdict::ERR_OVERFLOW_64;
            return t;
        }

        t.element_count = static_cast<int64_t>(data.size());
        t.pre_multiset_hash = compute_multiset_fingerprint(data);

        auto start = std::chrono::steady_clock::now();
        if (t.element_count > 1) {
            // Strict adversarial depth budget: 2 * log2(N)
            const int64_t depth_budget = 2 * static_cast<int64_t>(std::bit_width(data.size()));
            introsort_3way(data, 0, t.element_count - 1, depth_budget, 1, t);
        }
        auto end = std::chrono::steady_clock::now();

        t.elapsed_us = std::chrono::duration<double, std::micro>(end - start).count();
        t.post_multiset_hash = compute_multiset_fingerprint(data);
        t.order_witness = compute_order_witness(data);

        // Devil's Advocate self-audit: never trust the algorithm without verifying post-conditions
        if (t.pre_multiset_hash != t.post_multiset_hash) {
            t.verdict = SortVerdict::ERR_MULTISET_CORRUPTION;
            return t;
        }
        for (int64_t i = 1; i < t.element_count; ++i) {
            if (data[static_cast<size_t>(i - 1)] > data[static_cast<size_t>(i)]) {
                t.verdict = SortVerdict::ERR_UNSORTED_INVARIANT;
                return t;
            }
        }

        t.verdict = (t.fallback_triggers > 0)
            ? SortVerdict::OK_ADVERSARIAL_FALLBACK_TRIGGERED
            : SortVerdict::OK_PRIMARY_PATH;
        return t;
    }

private:
    static constexpr int64_t INSERTION_THRESHOLD = 16;

    static int64_t median_of_three(std::span<int64_t> a, int64_t i, int64_t j, int64_t k, SortTelemetry& t) noexcept {
        auto less = [&](int64_t x, int64_t y) noexcept {
            ++t.comparisons;
            return a[static_cast<size_t>(x)] < a[static_cast<size_t>(y)];
        };
        if (less(i, j)) {
            if (less(j, k)) return j;
            return less(i, k) ? k : i;
        } else {
            if (less(i, k)) return i;
            return less(j, k) ? k : j;
        }
    }

    // Tukey's Ninther: median of 9 evenly spaced samples to defeat crafted pivot killers
    static int64_t select_ninther_pivot(std::span<int64_t> a, int64_t lo, int64_t hi, SortTelemetry& t) noexcept {
        const int64_t n = hi - lo;
        const int64_t step = n / 8;
        const int64_t m1 = median_of_three(a, lo, lo + step, lo + 2 * step, t);
        const int64_t m2 = median_of_three(a, lo + 3 * step, lo + 4 * step, lo + 5 * step, t);
        const int64_t m3 = median_of_three(a, lo + 6 * step, lo + 7 * step, hi, t);
        const int64_t ninther = median_of_three(a, m1, m2, m3, t);
        return a[static_cast<size_t>(ninther)];
    }

    static void insertion_sort(std::span<int64_t> a, int64_t lo, int64_t hi, SortTelemetry& t) noexcept {
        for (int64_t i = lo + 1; i <= hi; ++i) {
            int64_t key = a[static_cast<size_t>(i)];
            int64_t j = i - 1;
            while (j >= lo) {
                ++t.comparisons;
                if (a[static_cast<size_t>(j)] > key) {
                    a[static_cast<size_t>(j + 1)] = a[static_cast<size_t>(j)];
                    ++t.swaps;
                    --j;
                } else {
                    break;
                }
            }
            a[static_cast<size_t>(j + 1)] = key;
        }
    }

    // Deterministic O(N log N) worst-case fallback (Heapsort on subrange [lo, hi])
    static void heapsort_fallback(std::span<int64_t> a, int64_t lo, int64_t hi, SortTelemetry& t) noexcept {
        ++t.fallback_triggers;
        const int64_t count = hi - lo + 1;

        auto sift_down = [&](int64_t start, int64_t end) noexcept {
            int64_t root = start;
            while (root * 2 + 1 <= end) {
                int64_t child = root * 2 + 1;
                int64_t swap_idx = root;

                ++t.comparisons;
                if (a[static_cast<size_t>(lo + swap_idx)] < a[static_cast<size_t>(lo + child)]) {
                    swap_idx = child;
                }
                if (child + 1 <= end) {
                    ++t.comparisons;
                    if (a[static_cast<size_t>(lo + swap_idx)] < a[static_cast<size_t>(lo + child + 1)]) {
                        swap_idx = child + 1;
                    }
                }
                if (swap_idx == root) return;
                std::swap(a[static_cast<size_t>(lo + root)], a[static_cast<size_t>(lo + swap_idx)]);
                ++t.swaps;
                root = swap_idx;
            }
        };

        for (int64_t start = (count - 2) / 2; start >= 0; --start) {
            sift_down(start, count - 1);
        }
        for (int64_t end = count - 1; end > 0; --end) {
            std::swap(a[static_cast<size_t>(lo)], a[static_cast<size_t>(lo + end)]);
            ++t.swaps;
            sift_down(0, end - 1);
        }
    }

    static void introsort_3way(std::span<int64_t> a, int64_t lo, int64_t hi,
                               int64_t budget, int64_t depth, SortTelemetry& t) noexcept {
        while (hi - lo > INSERTION_THRESHOLD) {
            if (depth > t.max_recursion_depth) t.max_recursion_depth = depth;

            // Adversary check: if pivot skew persisted past log2(N) budget, abort quicksort for Heapsort
            if (budget <= 0) {
                heapsort_fallback(a, lo, hi, t);
                return;
            }
            --budget;

            const int64_t pivot = select_ninther_pivot(a, lo, hi, t);

            // 3-Way Dutch National Flag Partition: [lo..lt-1] < pivot, [lt..gt] == pivot, [gt+1..hi] > pivot
            int64_t lt = lo;
            int64_t i = lo;
            int64_t gt = hi;

            while (i <= gt) {
                ++t.comparisons;
                const int64_t val = a[static_cast<size_t>(i)];
                if (val < pivot) {
                    std::swap(a[static_cast<size_t>(lt++)], a[static_cast<size_t>(i++)]);
                    ++t.swaps;
                } else if (val > pivot) {
                    ++t.comparisons;
                    std::swap(a[static_cast<size_t>(i)], a[static_cast<size_t>(gt--)]);
                    ++t.swaps;
                } else {
                    ++t.comparisons;
                    ++i;
                }
            }

            // Recurse only on the smaller partition; loop on the larger (guarantees O(log N) stack)
            const int64_t left_size = lt - lo;
            const int64_t right_size = hi - gt;
            if (left_size < right_size) {
                if (left_size > 1) {
                    introsort_3way(a, lo, lt - 1, budget, depth + 1, t);
                }
                lo = gt + 1;
            } else {
                if (right_size > 1) {
                    introsort_3way(a, gt + 1, hi, budget, depth + 1, t);
                }
                hi = lt - 1;
            }
            ++depth;
        }

        if (lo < hi) {
            insertion_sort(a, lo, hi, t);
        }
    }
};

// =============================================================================
// ADVERSARIAL TEST CORPUS GENERATOR (NEGATIVE-TEST WITNESSES)
// =============================================================================
struct CorpusCase {
    std::string_view name;
    std::vector<int64_t> data;
};

[[nodiscard]] std::vector<CorpusCase> build_adversarial_corpus(int64_t n) {
    std::vector<CorpusCase> cases;

    // 1. Benign deterministic pseudo-random input
    {
        std::vector<int64_t> v(static_cast<size_t>(n));
        uint64_t s = 0x123456789ABCDEF0ULL;
        for (int64_t i = 0; i < n; ++i) {
            s = s * 6364136223846793005ULL + 1442695040888963407ULL;
            v[static_cast<size_t>(i)] = static_cast<int64_t>(s >> 16);
        }
        cases.push_back({"1_Benign_Random", std::move(v)});
    }

    // 2. Already sorted ascending (classic O(N^2) killer for naive last-element pivot)
    {
        std::vector<int64_t> v(static_cast<size_t>(n));
        for (int64_t i = 0; i < n; ++i) v[static_cast<size_t>(i)] = i;
        cases.push_back({"2_Sorted_Ascending", std::move(v)});
    }

    // 3. Organ-pipe distribution (defeats naive median-of-3 pivots)
    {
        std::vector<int64_t> v(static_cast<size_t>(n));
        int64_t half = n / 2;
        for (int64_t i = 0; i < half; ++i) v[static_cast<size_t>(i)] = i;
        for (int64_t i = half; i < n; ++i) v[static_cast<size_t>(i)] = n - i;
        cases.push_back({"3_Organ_Pipe", std::move(v)});
    }

    // 4. All identical elements (Dutch National Flag stress test)
    {
        std::vector<int64_t> v(static_cast<size_t>(n), 42);
        cases.push_back({"4_All_Duplicates", std::move(v)});
    }

    return cases;
}

} // namespace sort_audit

int main() {
    using namespace sort_audit;

    constexpr int64_t N = 12'000;
    auto corpus = build_adversarial_corpus(N);

    std::cout << "=== QUICKSORT COMPARISON: NO DEVIL'S ADVOCATE vs. DEVIL'S ADVOCATE (N = " << N << ") ===\n\n";

    for (const auto& test : corpus) {
        std::cout << "Scenario: " << test.name << "\n";

        auto copy_naive = test.data;
        auto copy_adv   = test.data;

        SortTelemetry t_naive = NaiveQuicksort::sort(copy_naive);
        SortTelemetry t_adv   = DevilsAdvocateQuicksort::sort(copy_adv);

        std::cout << "  " << t_naive.report("Naive (No Advocate)") << "\n";
        std::cout << "  " << t_adv.report("Devil's Advocate") << "\n";

        const double speedup = static_cast<double>(t_naive.comparisons) /
                               static_cast<double>(std::max<int64_t>(1, t_adv.comparisons));
        std::cout << std::format(
            "  -> Comparison Reduction: {:.1f}x | Stack Depth: {} vs {} | Order Witness Match: {}\n\n",
            speedup,
            t_naive.max_recursion_depth,
            t_adv.max_recursion_depth,
            (t_naive.order_witness == t_adv.order_witness ? "EXACT" : "MISMATCH")
        );
    }

    return 0;
}
