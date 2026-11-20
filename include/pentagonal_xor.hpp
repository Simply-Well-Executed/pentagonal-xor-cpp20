// SPDX-License-Identifier: 0BSD
// Copyright (c) 2026 Paul Allen Cheek
// Independent C++20 byte-stream XOR engine; not the parity probability API.
// Public/private/shared below are C++ access/borrow scopes, not OS memory maps.
#pragma once

#include "pxor_memory.hpp"

#if defined(__linux__) && !defined(_GNU_SOURCE)
#  define _GNU_SOURCE 1
#endif

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
#  include <sys/mman.h>
#  include <sys/resource.h>
#  include <time.h>
#  include <unistd.h>
#endif

namespace pxor {

using fixed_t = i32;
inline constexpr i32 FRACBITS = 16;
inline constexpr fixed_t FRACUNIT = 1 << FRACBITS;

// Preserve signed fixed-point arithmetic with defined negative rounding.
// Like the established API, the result must be representable in fixed_t.
[[nodiscard]] constexpr fixed_t fixed_mul(fixed_t a, fixed_t b) noexcept {
    return static_cast<fixed_t>((static_cast<i64>(a) * b) / FRACUNIT);
}

template <usize PowerOfTwoSize>
[[nodiscard]] constexpr usize fast_wrap(usize index) noexcept {
    static_assert(PowerOfTwoSize > 0 &&
                  (PowerOfTwoSize & (PowerOfTwoSize - 1)) == 0,
                  "Buffer modulus must be a nonzero power of two");
    return index & (PowerOfTwoSize - 1);
}

// Canonical fixed storage and borrowed slices live in pxor_memory.hpp.
struct CorePinResult {
    unsigned int core_id{0};
    bool pinned{false};
};

// Scheduling hint, not L1 residency, page locality or CPU-frequency guarantee.
[[nodiscard]] inline CorePinResult pin_to_active_core() noexcept {
#if defined(_WIN32)
    PROCESSOR_NUMBER active{};
    GetCurrentProcessorNumberEx(&active);
    if (active.Number >= sizeof(KAFFINITY) * 8u) return {};
    GROUP_AFFINITY affinity{};
    affinity.Group = active.Group;
    affinity.Mask = static_cast<KAFFINITY>(1) << active.Number;
    if (SetThreadGroupAffinity(GetCurrentThread(), &affinity, nullptr) == 0) {
        return {};
    }
    return {static_cast<unsigned int>(active.Group) * 64u + active.Number, true};
#elif defined(__linux__)
    const int core = sched_getcpu();
    if (core < 0 || core >= CPU_SETSIZE) return {};
    cpu_set_t allowed{};
    CPU_ZERO(&allowed);
    CPU_SET(core, &allowed);
    if (sched_setaffinity(0, sizeof(allowed), &allowed) != 0) return {};
    return {static_cast<unsigned int>(core), true};
#else
    return {};
#endif
}

// Time-critical scheduling is deliberately excluded.
[[nodiscard]] inline bool request_moderate_priority() noexcept {
#if defined(_WIN32)
    const bool process = SetPriorityClass(
        GetCurrentProcess(), ABOVE_NORMAL_PRIORITY_CLASS) != 0;
    const bool thread = SetThreadPriority(
        GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL) != 0;
    return process && thread;
#elif defined(__linux__)
    return setpriority(PRIO_PROCESS, 0, -1) == 0;
#else
    return false;
#endif
}

// Unsigned modular arithmetic after overflow, NOT arbitrary precision.
[[nodiscard]] constexpr u64 pentagonal(u64 n) noexcept {
    const u64 other = 3u * n - 1u;
    return (n & 1u) == 0u ? (n / 2u) * other : n * (other / 2u);
}

// A bounded-consumer lazy cursor, implemented only with C++20 core language.
struct PentagonalSequence {
    struct Cursor {
        u64 n{1};
        [[nodiscard]] constexpr u64 operator*() const noexcept {
            return pentagonal(n);
        }
        constexpr Cursor& operator++() noexcept { ++n; return *this; }
        constexpr Cursor operator++(int) noexcept {
            const Cursor old = *this;
            ++n;
            return old;
        }
    };
    [[nodiscard]] constexpr Cursor begin() const noexcept { return {}; }
};
[[nodiscard]] constexpr PentagonalSequence pentagonal_numbers() noexcept {
    return {};
}

// Private construction of the keystream at compile time, publicly readable.
// Index is repeated every 256 bytes; NOT equal to exact seeded stream at >255.
inline constexpr auto PENTAGONAL_LUT = []() consteval {
    FixedBuffer<u8, 256> lut{};
    for (usize i = 0; i < lut.size(); ++i) {
        const u64 p = pentagonal(static_cast<u64>(i + 1));
        lut[i] = static_cast<u8>((p ^ (p >> 8) ^ (p >> 16)) & 0xFFu);
    }
    return lut;
}();

// Exact seeded byte stream over the finite representable integer range.
// ByteSlice is non-owning, so passing invalid pointers is a caller error.
constexpr void apply_pentagonal_xor(ByteSlice buffer, u64 seed = 1) noexcept {
    for (usize i = 0; i < buffer.size(); ++i) {
        const u64 p = pentagonal(seed + static_cast<u64>(i));
        buffer[i] ^= static_cast<u8>(
            (p ^ (p >> 8) ^ (p >> 16)) & 0xFFu);
    }
}

// Explicit little-endian byte packing avoids aliasing, alignment,
// library calls, compiler intrinsics and endian-specific pointer casts.
[[nodiscard]] constexpr u64 pack_u64_le(const u8* bytes) noexcept {
    return static_cast<u64>(bytes[0])
         | (static_cast<u64>(bytes[1]) << 8)
         | (static_cast<u64>(bytes[2]) << 16)
         | (static_cast<u64>(bytes[3]) << 24)
         | (static_cast<u64>(bytes[4]) << 32)
         | (static_cast<u64>(bytes[5]) << 40)
         | (static_cast<u64>(bytes[6]) << 48)
         | (static_cast<u64>(bytes[7]) << 56);
}

constexpr void unpack_u64_le(u8* bytes, u64 value) noexcept {
    for (unsigned n = 0; n < 8; ++n) {
        bytes[n] = static_cast<u8>((value >> (n * 8u)) & 255u);
    }
}

constexpr void fast_xor_buffer(ByteSlice buffer) noexcept {
    usize i = 0;
    const usize n = buffer.size();
    u8* const dst = buffer.data();
    const u8* const lut = PENTAGONAL_LUT.data();
    for (; n - i >= 8; i += 8) {
        const u64 value = pack_u64_le(dst + i);
        const u64 key = pack_u64_le(lut + fast_wrap<256>(i));
        unpack_u64_le(dst + i, value ^ key);
    }
    for (; i < n; ++i) dst[i] ^= lut[fast_wrap<256>(i)];
}

// An access token models a private C++ API, NOT an OS memory permission.
class EngineKey {
    friend class EngineAuditor;
    constexpr EngineKey() noexcept = default;
};

template <usize N>
class ObfuscatedData {
    static_assert(N > 0, "Literal must include a terminator");
    FixedBuffer<u8, N> cipher_{};
    u64 seed_{1};

public:
    consteval explicit ObfuscatedData(const char (&literal)[N],
                                      u64 seed = 1) noexcept : seed_(seed) {
        for (usize i = 0; i < N; ++i) {
            cipher_[i] = static_cast<u8>(
                static_cast<unsigned char>(literal[i]));
        }
        apply_pentagonal_xor(ByteSlice{cipher_}, seed_);
    }

    [[nodiscard]] constexpr ConstByteSlice shared_cipher(EngineKey) const noexcept {
        return ConstByteSlice{cipher_};
    }

    // Scoped callback-only access. Runtime volatile writes are best effort;
    // compiler access control does not enforce OS or process secrecy.
    template <typename Func>
    void with_decrypted(Func&& fn) const {
        FixedBuffer<u8, N> decoded = cipher_;
        apply_pentagonal_xor(ByteSlice{decoded}, seed_);
        FixedBuffer<char, N> plain{};
        for (usize i = 0; i < N; ++i) {
            plain[i] = static_cast<char>(decoded[i]);
        }
        struct Guard {
            FixedBuffer<u8, N>& raw;
            FixedBuffer<char, N>& chars;
            ~Guard() noexcept {
                for (usize i = 0; i < N; ++i) {
                    volatile u8* a = &raw[i];
                    volatile char* b = &chars[i];
                    *a = 0;
                    *b = '\0';
                }
            }
        } guard{decoded, plain};
        constexpr usize length = N - 1;
        static_assert(same_type<decltype(
            static_cast<Func&&>(fn)(StringSlice{plain.data(), length})), void>,
            "Scoped plaintext callbacks must return void");
        static_cast<Func&&>(fn)(StringSlice{plain.data(), length});
    }
};

class EngineAuditor {
public:
    template <usize N>
    [[nodiscard]] static constexpr u8 first_cipher_byte(
        const ObfuscatedData<N>& source) noexcept {
        return source.shared_cipher(EngineKey{})[0];
    }
};

} // namespace pxor
