/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
// Copyright (c) 2026 Paul Allen Cheek
// Port of the experimental PR #17 Stability-9L3PX5-U16LE-v1 contract.
// One authoritative five-input XOR kernel, already defined in this repository.
// C++20, include-free except for the original local C parity header.
// No heap, model weights, tokenizer, runtime platform API, or IO.
#ifndef SWE_STABILITY10D_HPP
#define SWE_STABILITY10D_HPP

#include "pentagonal_xor_std_free.h"

namespace swe::stability10d {

inline constexpr char protocol[] = "Stability-9L3PX5-U16LE-v1";
inline constexpr unsigned indices[3][5] = {
    {0u, 1u, 2u, 3u, 4u},
    {3u, 4u, 5u, 6u, 7u},
    {6u, 7u, 8u, 0u, 1u}
};

constexpr bool finite(double value) noexcept {
    return value <= 1.7976931348623157e308 &&
           value >= -1.7976931348623157e308;
}
constexpr double abs_value(double value) noexcept {
    return value < 0.0 ? -value : value;
}
constexpr double clamp01(double value) noexcept {
    return value < 0.0 ? 0.0 : (value > 1.0 ? 1.0 : value);
}

// Source counts must be exact integers representable in binary64.
// Restrict to 2^53-1 to keep the float64 count / index witness unambiguous.
constexpr bool exact_count(double value) noexcept {
    return finite(value) && value >= 0.0 &&
           value <= 9007199254740991.0 &&
           static_cast<double>(static_cast<unsigned long long>(value)) == value;
}

struct Result {
    double ligands[9]{};
    double rings[3]{};
    unsigned words[3]{};      // unsigned 16-bit values stored in >=16-bit unsigned
    unsigned char hexabyte[6]{}; // LE: P0 low/high, P1 low/high, P2 low/high
    double stability = 0.0;  // 1 - (max ring score - min ring score)
    bool admissible = false;
};

// Exactly 9 measured CORE values:
// N, mean L/R, rms L/R, peak L/R, zero crossings L/R.
// On failure, no output mutation. Does not relabel raw measurements as probabilities.
inline bool normalize(const double (&measurements)[9],
                      double (&ligands)[9]) noexcept {
    for (double m : measurements) if (!finite(m)) return false;
    const double frames = measurements[0];
    if (!exact_count(frames) || frames < 1.0) return false;

    double h[9]{};
    h[0] = frames / (frames + 1024.0);
    for (unsigned i = 1u; i <= 2u; ++i) {
        const double m = measurements[i];
        h[i] = 0.5 * (1.0 + m / (1.0 + abs_value(m)));
    }
    for (unsigned i = 3u; i <= 6u; ++i) {
        const double v = measurements[i];
        if (v < 0.0) return false;
        h[i] = v / (1.0 + v);
    }
    const double transitions = frames > 1.0 ? (frames - 1.0) : 1.0;
    for (unsigned i = 7u; i <= 8u; ++i) {
        const double crossings = measurements[i];
        if (!exact_count(crossings) || crossings > (frames - 1.0)) return false;
        h[i] = crossings / transitions;
    }
    for (double v : h) if (!finite(v) || v < 0.0 || v > 1.0) return false;
    for (unsigned i = 0u; i < 9u; ++i) ligands[i] = h[i];
    return true;
}

// Ring parity is NOT a calibrated probability for correlated acoustic features.
// Output is only assigned on success; callers retain the source unchanged.
inline bool calculate(const double (&ligands)[9], Result& output) noexcept {
    for (double h : ligands)
        if (!finite(h) || h < 0.0 || h > 1.0) return false;

    Result candidate{};
    for (unsigned i = 0u; i < 9u; ++i) candidate.ligands[i] = ligands[i];

    for (unsigned ring = 0u; ring < 3u; ++ring) {
        double five[5]{};
        for (unsigned j = 0u; j < 5u; ++j)
            five[j] = ligands[indices[ring][j]];
        const double p = clamp01(swe_pxor_xor_parity_arr(five));
        if (!finite(p)) return false;
        candidate.rings[ring] = p;

        // Nearest integer, ties upward: equivalent to floor(65535*p + 0.5)
        // because p is bounded in [0,1]. No math library or rounding mode.
        const unsigned word = static_cast<unsigned>(65535.0 * p + 0.5);
        candidate.words[ring] = word;
        candidate.hexabyte[2u * ring] =
            static_cast<unsigned char>(word & 255u);
        candidate.hexabyte[2u * ring + 1u] =
            static_cast<unsigned char>((word >> 8u) & 255u);
    }

    double minimum = candidate.rings[0], maximum = candidate.rings[0];
    for (unsigned j = 1u; j < 3u; ++j) {
        const double x = candidate.rings[j];
        if (x < minimum) minimum = x;
        if (x > maximum) maximum = x;
    }
    candidate.stability = clamp01(1.0 - (maximum - minimum));
    candidate.admissible = true;
    output = candidate;
    return true;
}

inline bool from_measurements(const double (&measurements)[9],
                              Result& output) noexcept {
    double normalized[9]{};
    if (!normalize(measurements, normalized)) return false;
    return calculate(normalized, output);
}

// Exactly 12 ASCII lowercase hex digits plus a null terminator.
inline void hex(const Result& result, char (&text)[13]) noexcept {
    constexpr char digits[] = "0123456789abcdef";
    for (unsigned i = 0u; i < 6u; ++i) {
        const unsigned value = result.hexabyte[i];
        text[2u * i] = digits[value >> 4u];
        text[2u * i + 1u] = digits[value & 15u];
    }
    text[12] = '\0';
}

} // namespace swe::stability10d

#endif // SWE_STABILITY10D_HPP
