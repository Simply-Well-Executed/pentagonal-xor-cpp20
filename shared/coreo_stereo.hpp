// SPDX-License-Identifier: 0BSD
// Copyright (c) 2026 Paul Allen Cheek
// PR #17 Coreo stereo operand extraction, distinct from DecaSigma scalar COREAO.
// Caller owns source/output buffers, original source is never mutated.
// No model, no tokenizer, no IO, no allocations.
#ifndef SWE_COREO_STEREO_HPP
#define SWE_COREO_STEREO_HPP

#include "stability10d.hpp"

namespace swe::coreo_stereo {

struct Quad {
    float yin_left = 0.0f;
    float yin_right = 0.0f;
    float yang_left = 0.0f;
    float yang_right = 0.0f;
};

// Input source is exactly 2*frames interleaved stereo float32 values.
// Caller guarantees valid storage and frames <= 2^53-1, with no aliasing.
// For frame i:
// [ -L(N-1-i), -R(N-1-i), -L(i), -R(i) ]
inline bool forward_at(const float* source, unsigned long long frames,
                       unsigned long long i, Quad& output) noexcept {
    if (!source || frames == 0u || i >= frames ||
        frames > 9007199254740991ull) return false;
    const unsigned long long j = frames - 1u - i;
    const float l = source[2u * i], r = source[2u * i + 1u];
    const float reverse_l = source[2u * j], reverse_r = source[2u * j + 1u];
    if (!stability10d::finite(l) || !stability10d::finite(r) ||
        !stability10d::finite(reverse_l) ||
        !stability10d::finite(reverse_r)) return false;
    output = {-reverse_l, -reverse_r, -l, -r};
    return true;
}

// Inverse projection uses the YANG pair. This is not a speech decoder.
inline bool reverse_at(const Quad& quad, float& left, float& right) noexcept {
    if (!stability10d::finite(quad.yang_left) ||
        !stability10d::finite(quad.yang_right)) return false;
    left = -quad.yang_left;
    right = -quad.yang_right;
    return true;
}

struct Audit {
    double largest_disagreement = 0.0;
    bool admissible = false;
};

// Independent read-only check over caller-owned Quad records.
// Also compares the Z-fold relation yin[i] == yang[N-1-i].
// No averaging; one largest absolute residual is reported.
inline Audit inspect(const float* source, unsigned long long frames,
                     const Quad* output) noexcept {
    Audit audit{};
    if (!source || !output || frames == 0u ||
        frames > 9007199254740991ull) return audit;
    double maximum = 0.0;
    for (unsigned long long i = 0u; i < frames; ++i) {
        const unsigned long long j = frames - 1u - i;
        Quad expected{};
        if (!forward_at(source, frames, i, expected)) return audit;
        const Quad& actual = output[i];
        const Quad& z = output[j];
        const double residuals[6] = {
            static_cast<double>(actual.yin_left) - expected.yin_left,
            static_cast<double>(actual.yin_right) - expected.yin_right,
            static_cast<double>(actual.yang_left) - expected.yang_left,
            static_cast<double>(actual.yang_right) - expected.yang_right,
            static_cast<double>(actual.yin_left) - z.yang_left,
            static_cast<double>(actual.yin_right) - z.yang_right
        };
        for (double residual : residuals) {
            if (!stability10d::finite(residual)) return audit;
            const double size = stability10d::abs_value(residual);
            if (size > maximum) maximum = size;
        }
    }
    audit.largest_disagreement = maximum;
    audit.admissible = true;
    return audit;
}

} // namespace swe::coreo_stereo

#endif // SWE_COREO_STEREO_HPP
