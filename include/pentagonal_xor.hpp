// SPDX-License-Identifier: 0BSD
// Copyright (c) 2026 Paul Allen Cheek
// Independent C++20 byte-stream XOR engine; not the parity probability API.
// Public/private/shared below are C++ access/borrow scopes, not OS memory maps.
#pragma once

#if defined(__linux__) && !defined(_GNU_SOURCE)
#  define _GNU_SOURCE 1
#endif

#include <array>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <ranges>
#include <span>
#include <type_traits>

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

using u8 = unsigned char;
using i32 = std::int32_t;
using i64 = std::int64_t;
using u64 = std::uint64_t;
using usize = std::size_t;
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

// Public fixed-capacity, allocation-free storage. A view never owns this data.
template <typename T, usize N>
struct FixedBuffer {
    static_assert(N > 0, "FixedBuffer requires a positive capacity");
    T items[N]{};

    [[nodiscard]] constexpr T& operator[](usize i) noexcept { return items[i]; }
    [[nodiscard]] constexpr const T& operator[](usize i) const noexcept {
        return items[i];
    }
    [[nodiscard]] constexpr T* data() noexcept { return items; }
    [[nodiscard]] constexpr const T* data() const noexcept { return items; }
    [[nodiscard]] static constexpr usize size() noexcept { return N; }

    constexpr void fill(const T& value) noexcept {
        for (usize i = 0; i < N; ++i) items[i] = value;
    }
};

struct ByteSlice {
    u8* ptr{nullptr};
    usize len{0};

    constexpr ByteSlice() noexcept = default;
    constexpr ByteSlice(u8* p, usize n) noexcept : ptr(p), len(n) {}
    template <usize N>
    constexpr ByteSlice(FixedBuffer<u8, N>& buf) noexcept
        : ptr(buf.data()), len(N) {}

    [[nodiscard]] constexpr u8& operator[](usize i) const noexcept {
        return ptr[i];
    }
    [[nodiscard]] constexpr u8* data() const noexcept { return ptr; }
    [[nodiscard]] constexpr usize size() const noexcept { return len; }
};

struct ConstByteSlice {
    const u8* ptr{nullptr};
    usize len{0};

    constexpr ConstByteSlice() noexcept = default;
    constexpr ConstByteSlice(const u8* p, usize n) noexcept : ptr(p), len(n) {}
    template <usize N>
    constexpr ConstByteSlice(const FixedBuffer<u8, N>& buf) noexcept
        : ptr(buf.data()), len(N) {}

    [[nodiscard]] constexpr const u8& operator[](usize i) const noexcept {
        return ptr[i];
    }
    [[nodiscard]] constexpr const u8* data() const noexcept { return ptr; }
    [[nodiscard]] constexpr usize size() const noexcept { return len; }
};

struct StringSlice {
    const char* ptr{nullptr};
    usize len{0};

    constexpr StringSlice() noexcept = default;
    constexpr StringSlice(const char* p, usize n) noexcept : ptr(p), len(n) {}
    [[nodiscard]] constexpr const char* data() const noexcept { return ptr; }
    [[nodiscard]] constexpr usize size() const noexcept { return len; }
};

// Borrowed views do NOT extend ownership or lifetime, protect memory from the
// operating system, or impose thread synchronization. Never retain beyond owner.
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
template <std::unsigned_integral T = u64>
[[nodiscard]] constexpr T pentagonal(T n) noexcept {
    const T other = static_cast<T>(static_cast<T>(3) * n - static_cast<T>(1));
    if ((n & static_cast<T>(1)) == 0) {
        return static_cast<T>((n / 2) * other);
    }
    return static_cast<T>(n * (other / 2));
}

[[nodiscard]] constexpr auto pentagonal_numbers() noexcept {
    return std::views::iota(u64{1})
         | std::views::transform([](u64 n) noexcept {
               return pentagonal(n);
           });
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

// Backward-compatible C++20 span facade for existing callers.
constexpr void apply_pentagonal_xor(std::span<std::byte> buffer,
                                    u64 seed = 1) noexcept {
    for (usize i = 0; i < buffer.size(); ++i) {
        const u64 p = pentagonal(seed + static_cast<u64>(i));
        buffer[i] ^= static_cast<std::byte>(
            (p ^ (p >> 8) ^ (p >> 16)) & 0xFFu);
    }
}

// memcpy is ISO C++ and safe for unaligned byte slices on MSVC/GCC/Clang.
// A compiler may optimize these operations to register instructions.
inline void fast_xor_buffer(ByteSlice buffer) noexcept {
    usize i = 0;
    const usize n = buffer.size();
    u8* const dst = buffer.data();
    const u8* const lut = PENTAGONAL_LUT.data();

    for (; n - i >= sizeof(u64); i += sizeof(u64)) {
        u64 data = 0;
        u64 key = 0;
        std::memcpy(&data, dst + i, sizeof(data));
        std::memcpy(&key, lut + fast_wrap<256>(i), sizeof(key));
        data ^= key;
        std::memcpy(dst + i, &data, sizeof(data));
    }
    for (; i < n; ++i) {
        dst[i] ^= lut[fast_wrap<256>(i)];
    }
}

// Thin compatibility adapter for pre-existing std::byte clients.
inline void fast_xor_buffer(std::span<std::byte> buffer) noexcept {
    fast_xor_buffer(ByteSlice{
        reinterpret_cast<u8*>(buffer.data()), buffer.size()});
}

// An access token models a private C++ API, NOT an OS memory permission.
class EngineKey {
    friend class EngineAuditor;
    constexpr EngineKey() noexcept = default;
};

template <usize N>
class ObfuscatedData {
    static_assert(N > 0, "A literal requires a terminator");
    FixedBuffer<u8, N> cipher_{};
    u64 seed_{1};

    template <typename T>
    static constexpr void wipe_buffer(FixedBuffer<T, N>& buf) noexcept {
        if (std::is_constant_evaluated()) {
            buf.fill(T{});
        } else {
            // Volatile stores are compiler-observable, but cannot guarantee
            // removal of all register copies, spills or callback-held aliases.
            for (usize i = 0; i < N; ++i) {
                volatile T* item = &buf[i];
                *item = T{};
            }
        }
    }

public:
    consteval explicit ObfuscatedData(const char (&source)[N],
                                      u64 seed = 1) noexcept : seed_(seed) {
        for (usize i = 0; i < N; ++i) {
            cipher_[i] = static_cast<u8>(
                static_cast<unsigned char>(source[i]));
        }
        apply_pentagonal_xor(ByteSlice{cipher_}, seed_);
    }

    // Restricted shared read-only cipher view, callable only with EngineKey.
    // Public EngineAuditor intentionally reveals a limited first-byte witness.
    [[nodiscard]] constexpr ConstByteSlice shared_cipher(EngineKey) const noexcept {
        return ConstByteSlice{cipher_};
    }

    // Scoped plaintext borrow: callback must return void. C++ access rules do
    // not prevent a malicious callback from saving its pointer, so never claim
    // hardware-backed secrecy or cryptographic protection.
    template <typename Func>
    constexpr void with_decrypted(Func&& fn) const {
        FixedBuffer<u8, N> decoded = cipher_;
        apply_pentagonal_xor(ByteSlice{decoded}, seed_);
        FixedBuffer<char, N> plain{};
        for (usize i = 0; i < N; ++i) {
            plain[i] = static_cast<char>(decoded[i]);
        }

        struct ScopedWipe {
            FixedBuffer<u8, N>& decoded;
            FixedBuffer<char, N>& plaintext;
            constexpr ~ScopedWipe() noexcept {
                ObfuscatedData::wipe_buffer(decoded);
                ObfuscatedData::wipe_buffer(plaintext);
            }
        } wipe{decoded, plain};

        constexpr usize length = N - 1;
        static_assert(std::is_void_v<decltype(
            static_cast<Func&&>(fn)(StringSlice{plain.data(), length}))>,
            "with_decrypted callbacks must return void");
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
