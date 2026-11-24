// SPDX-License-Identifier: 0BSD
// Copyright (c) 2026 Paul Allen Cheek
// Public fixed storage and non-owning read/write views, with NO standard headers.
// C++ access qualifiers are not operating-system memory isolation.
#ifndef SWE_PXOR_MEMORY_HPP
#define SWE_PXOR_MEMORY_HPP

namespace pxor {

using u8 = unsigned char;
using i32 = int;
using i64 = long long;
using u64 = unsigned long long;
using usize = decltype(sizeof(0));

static_assert(sizeof(u8) == 1, "The XOR engine needs eight-bit bytes");
static_assert(sizeof(i32) == 4, "The fixed-point engine needs 32-bit int");
static_assert(sizeof(i64) == 8 && sizeof(u64) == 8,
              "The XOR engine needs 64-bit integers");

template <typename A, typename B>
inline constexpr bool same_type = false;
template <typename A>
inline constexpr bool same_type<A, A> = true;

template <typename T, usize N>
struct FixedBuffer {
    static_assert(N > 0, "FixedBuffer needs nonzero capacity");
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

// Views borrow storage: no ownership, sharing synchronization or lifetime extension.
} // namespace pxor

#endif // SWE_PXOR_MEMORY_HPP
