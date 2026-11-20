// SPDX-License-Identifier: 0BSD
// Copyright (c) 2026 Paul Allen Cheek
// Public storage and borrowed views for native C++20 modules.
// "Private/shared" are C++ access and read-only borrowing concepts,
// not .NET managed memory, OS shared mappings, privilege, or thread safety.
#pragma once

#include <cstddef>
#include <cstdint>

namespace pxor {

using u8 = unsigned char;
using i32 = std::int32_t;
using i64 = std::int64_t;
using u64 = std::uint64_t;
using usize = std::size_t;

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

} // namespace pxor
