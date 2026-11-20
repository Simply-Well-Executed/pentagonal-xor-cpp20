// SPDX-License-Identifier: 0BSD
// Copyright (c) 2026 Paul Allen Cheek
// Optional high-throughput BYTEWISE XOR engine, separate from parity scores.
// The 256-byte repeating LUT stream is NOT the exact seeded pentagonal stream.
#pragma once

#if defined(__linux__) && !defined(_GNU_SOURCE)
#  define _GNU_SOURCE 1
#endif

#include <array>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <optional>
#include <ranges>
#include <span>

#if defined(_WIN32)
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif
#  ifndef NOMINMAX
#    define NOMINMAX
#  endif
#  ifndef VC_EXTRALEAN
#    define VC_EXTRALEAN
#  endif
#  include <windows.h>
#elif defined(__linux__)
#  include <sched.h>
#  include <sys/resource.h>
#endif

namespace pxor {

using fixed_t = std::int32_t;
inline constexpr int FRACBITS = 16;
inline constexpr fixed_t FRACUNIT = 1 << FRACBITS;

// Truncation toward zero for negative products is intentional and portable.
// Input products outside signed 32-bit representability are not supported.
[[nodiscard]] constexpr fixed_t fixed_mul(fixed_t a, fixed_t b) noexcept {
    return static_cast<fixed_t>(
        (static_cast<std::int64_t>(a) * static_cast<std::int64_t>(b)) / FRACUNIT);
}

template <std::size_t PowerOfTwoSize>
[[nodiscard]] constexpr std::size_t fast_wrap(std::size_t index) noexcept {
    static_assert(PowerOfTwoSize > 0 &&
                  (PowerOfTwoSize & (PowerOfTwoSize - 1)) == 0,
                  "Size must be a nonzero power of two");
    return index & (PowerOfTwoSize - 1);
}

// A best-effort scheduling hint, NOT a guarantee of L1 or NUMA residency.
// No affinity request is made on unsupported platforms.
[[nodiscard]] inline std::optional<unsigned int> pin_to_active_core() noexcept {
#if defined(_WIN32)
    PROCESSOR_NUMBER active{};
    GetCurrentProcessorNumberEx(&active);
    if (active.Number >= sizeof(KAFFINITY) * 8u) {
        return std::nullopt;
    }
    GROUP_AFFINITY affinity{};
    affinity.Group = active.Group;
    affinity.Mask = static_cast<KAFFINITY>(1) << active.Number;
    if (SetThreadGroupAffinity(GetCurrentThread(), &affinity, nullptr) == 0) {
        return std::nullopt;
    }
    return static_cast<unsigned int>(active.Group) * 64u + active.Number;
#elif defined(__linux__)
    const int core = sched_getcpu();
    if (core < 0 || core >= CPU_SETSIZE) {
        return std::nullopt;
    }
    cpu_set_t allowed{};
    CPU_ZERO(&allowed);
    CPU_SET(core, &allowed);
    if (sched_setaffinity(0, sizeof(allowed), &allowed) != 0) {
        return std::nullopt;
    }
    return static_cast<unsigned int>(core);
#else
    return std::nullopt;
#endif
}

// Use only moderate priority, never Windows TIME_CRITICAL.
// Privileges or policy may reject this request, which is reported by the CLI.
[[nodiscard]] inline bool request_moderate_priority() noexcept {
#if defined(_WIN32)
    const bool process = SetPriorityClass(GetCurrentProcess(),
                                           ABOVE_NORMAL_PRIORITY_CLASS) != 0;
    const bool thread = SetThreadPriority(GetCurrentThread(),
                                          THREAD_PRIORITY_ABOVE_NORMAL) != 0;
    return process && thread;
#elif defined(__linux__)
    return setpriority(PRIO_PROCESS, 0, -1) == 0;
#else
    return false;
#endif
}

// For values beyond the representable result type, unsigned arithmetic wraps.
// Do not confuse this modular result with an arbitrary-precision pentagonal number.
template <std::unsigned_integral T = std::uint64_t>
[[nodiscard]] constexpr T pentagonal(T n) noexcept {
    const T other = static_cast<T>(static_cast<T>(3) * n - static_cast<T>(1));
    if ((n & static_cast<T>(1)) == 0) {
        return static_cast<T>((n / 2) * other);
    }
    return static_cast<T>(n * (other / 2));
}

[[nodiscard]] constexpr auto pentagonal_numbers() noexcept {
    return std::views::iota(std::uint64_t{1})
         | std::views::transform([](std::uint64_t n) noexcept {
               return pentagonal(n);
           });
}

// LUT[i] represents the key for P_(i+1), i=0..255.
// It is repeated every 256 bytes in fast_xor_buffer, BY DESIGN.
inline constexpr auto PENTAGONAL_LUT = []() consteval {
    std::array<std::byte, 256> lut{};
    for (std::size_t i = 0; i < lut.size(); ++i) {
        const auto p = pentagonal(static_cast<std::uint64_t>(i + 1));
        lut[i] = static_cast<std::byte>((p ^ (p >> 8) ^ (p >> 16)) & 0xFFu);
    }
    return lut;
}();

// Exact seeded stream (within representable pentagonal-number range).
// This method and the 256-byte periodic LUT method are distinct APIs.
constexpr void apply_pentagonal_xor(std::span<std::byte> buffer,
                                    std::uint64_t seed = 1) noexcept {
    for (std::size_t i = 0; i < buffer.size(); ++i) {
        const std::uint64_t p =
            pentagonal(seed + static_cast<std::uint64_t>(i));
        const auto key = static_cast<std::byte>((p ^ (p >> 8) ^ (p >> 16)) & 0xFFu);
        buffer[i] ^= key;
    }
}

// Bytewise identical to XOR with the repeated LUT, regardless of endianness.
// memcpy permits unaligned access without violating strict aliasing.
inline void fast_xor_buffer(std::span<std::byte> buffer) noexcept {
    std::size_t i = 0;
    const std::size_t n = buffer.size();

    for (; n - i >= 8; i += 8) {
        std::uint64_t payload = 0;
        std::uint64_t key = 0;
        std::memcpy(&payload, buffer.data() + i, sizeof(payload));
        std::memcpy(&key, PENTAGONAL_LUT.data() + fast_wrap<256>(i), sizeof(key));
        payload ^= key;
        std::memcpy(buffer.data() + i, &payload, sizeof(payload));
    }
    for (; i < n; ++i) {
        buffer[i] ^= PENTAGONAL_LUT[fast_wrap<256>(i)];
    }
}

// Deterministic obfuscation for illustrative literals, NOT cryptography.
// The key/seed remains discoverable in the compiled binary.
template <std::size_t N>
struct ObfuscatedData {
    std::array<std::byte, N> data{};

    consteval explicit ObfuscatedData(const char (&source)[N],
                                      std::uint64_t seed = 1) noexcept {
        for (std::size_t i = 0; i < N; ++i) {
            data[i] = static_cast<std::byte>(
                static_cast<unsigned char>(source[i]));
        }
        apply_pentagonal_xor(data, seed);
    }

    [[nodiscard]] constexpr std::array<char, N>
    decrypt(std::uint64_t seed = 1) const noexcept {
        auto copy = data;
        apply_pentagonal_xor(copy, seed);
        std::array<char, N> plain{};
        for (std::size_t i = 0; i < N; ++i) {
            plain[i] = static_cast<char>(std::to_integer<unsigned char>(copy[i]));
        }
        return plain;
    }
};

} // namespace pxor
