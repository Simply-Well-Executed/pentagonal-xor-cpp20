#ifndef SWE_PENTAGONAL_SUITE_HPP
#define SWE_PENTAGONAL_SUITE_HPP

#include "pentagonal_xor.hpp"

namespace pxor {

enum class Op : u8 { Xor, Or, And, Nand, Nop, Nil };

[[nodiscard]] constexpr u8 pentagonal_key_byte(u64 index) noexcept {
    const u64 p = pentagonal(index);
    return static_cast<u8>((p ^ (p >> 8) ^ (p >> 16)) & 255u);
}

class PentagonalCor {
    u64 current_{1};
    u64 remaining_{0};
    bool bounded_{false};

public:
    constexpr PentagonalCor() noexcept = default;
    constexpr explicit PentagonalCor(u64 start) noexcept : current_(start) {}
    constexpr PentagonalCor(u64 start, u64 count) noexcept
        : current_(start), remaining_(count), bounded_(true) {}

    [[nodiscard]] constexpr bool done() const noexcept {
        return bounded_ && remaining_ == 0;
    }

    [[nodiscard]] constexpr u64 current() const noexcept { return current_; }

    [[nodiscard]] constexpr u64 remaining() const noexcept {
        return bounded_ ? remaining_ : 0;
    }

    constexpr bool try_next(u64& result) noexcept {
        if (done()) return false;
        result = pentagonal(current_);
        if (current_ == ~u64{0}) {
            bounded_ = true;
            remaining_ = 0;
        } else {
            ++current_;
            if (bounded_) --remaining_;
        }
        return true;
    }

    [[nodiscard]] constexpr u64 next() noexcept {
        u64 result = 0;
        (void)try_next(result);
        return result;
    }

    [[nodiscard]] constexpr u8 next_key_byte() noexcept {
        const u64 value = next();
        return static_cast<u8>(
            (value ^ (value >> 8) ^ (value >> 16)) & 255u);
    }

    constexpr void reset(u64 start = 1) noexcept {
        current_ = start;
        remaining_ = 0;
        bounded_ = false;
    }

    constexpr void reset(u64 start, u64 count) noexcept {
        current_ = start;
        remaining_ = count;
        bounded_ = true;
    }
};

class PentagonalVista {
    u64 start_{1};
    usize count_{0};

public:
    struct Iterator {
        u64 index{1};
        usize left{0};

        [[nodiscard]] constexpr u64 operator*() const noexcept {
            return pentagonal(index);
        }

        constexpr Iterator& operator++() noexcept {
            if (left != 0) {
                --left;
                if (left != 0) ++index;
            }
            return *this;
        }

        [[nodiscard]] constexpr bool operator!=(const Iterator& other) const noexcept {
            return left != other.left;
        }
    };

    constexpr PentagonalVista(u64 start, usize count) noexcept
        : start_(start), count_(count) {
        if (start_ != 0) {
            const u64 capacity = (~u64{0} - start_) + 1u;
            if (static_cast<u64>(count_) > capacity) {
                count_ = static_cast<usize>(capacity);
            }
        }
    }

    [[nodiscard]] constexpr u64 operator[](usize offset) const noexcept {
        return pentagonal(start_ + static_cast<u64>(offset));
    }

    [[nodiscard]] constexpr usize size() const noexcept { return count_; }
    [[nodiscard]] constexpr Iterator begin() const noexcept {
        return {start_, count_};
    }

    [[nodiscard]] constexpr Iterator end() const noexcept { return {0, 0}; }

    [[nodiscard]] constexpr PentagonalVista subvista(
        usize offset, usize length) const noexcept {
        const usize first = offset < count_ ? offset : count_;
        const usize available = count_ - first;
        const usize selected = length < available ? length : available;
        if (selected == 0) return {start_, 0};
        return {start_ + static_cast<u64>(first), selected};
    }
};

[[nodiscard]] constexpr u64 pentagonal_nan_bits(u64 index) noexcept {
    constexpr u64 qnan = 0x7FF8000000000000ull;
    constexpr u64 payload = 0x0007FFFFFFFFFFFFull;
    return qnan | (pentagonal(index) & payload);
}

[[nodiscard]] constexpr u64 extract_nan_payload(u64 bits) noexcept {
    return bits & 0x0007FFFFFFFFFFFFull;
}

[[nodiscard]] constexpr bool is_pentagonal_nan(u64 bits) noexcept {
    return (bits & 0x7FF8000000000000ull) ==
           0x7FF8000000000000ull;
}

template <Op Mode>
constexpr void apply_pentagonal_op(ByteSlice buffer, u64 seed = 1) noexcept {
    if constexpr (Mode == Op::Nop) {
        return;
    } else if constexpr (Mode == Op::Nil) {
        for (usize i = 0; i < buffer.size(); ++i) buffer[i] = 0;
    } else if constexpr (Mode == Op::Xor) {
        apply_pentagonal_xor(buffer, seed);
    } else {
        u64 index = seed;
        for (usize i = 0; i < buffer.size(); ++i, ++index) {
            const u8 key = pentagonal_key_byte(index);
            if constexpr (Mode == Op::Or) {
                buffer[i] |= key;
            } else if constexpr (Mode == Op::And) {
                buffer[i] &= key;
            } else if constexpr (Mode == Op::Nand) {
                buffer[i] = static_cast<u8>(~(buffer[i] & key));
            }
        }
    }
}

constexpr void apply_pentagonal_or(ByteSlice buffer, u64 seed = 1) noexcept {
    apply_pentagonal_op<Op::Or>(buffer, seed);
}

constexpr void apply_pentagonal_and(ByteSlice buffer, u64 seed = 1) noexcept {
    apply_pentagonal_op<Op::And>(buffer, seed);
}

constexpr void apply_pentagonal_nand(ByteSlice buffer, u64 seed = 1) noexcept {
    apply_pentagonal_op<Op::Nand>(buffer, seed);
}

constexpr void apply_pentagonal_nop(ByteSlice buffer, u64 seed = 1) noexcept {
    apply_pentagonal_op<Op::Nop>(buffer, seed);
}

constexpr void apply_pentagonal_nil(ByteSlice buffer, u64 seed = 1) noexcept {
    apply_pentagonal_op<Op::Nil>(buffer, seed);
}

constexpr void apply_pentagonal_nan(ByteSlice buffer, u64 seed = 1) noexcept {
    usize i = 0;
    u64 index = seed;
    for (; buffer.size() - i >= 8; i += 8, ++index) {
        unpack_u64_le(buffer.data() + i, pentagonal_nan_bits(index));
    }
    for (; i < buffer.size(); ++i) buffer[i] = 255u;
}

template <Op Mode = Op::Xor>
constexpr void fast_op_buffer(ByteSlice buffer) noexcept {
    if constexpr (Mode == Op::Nop) {
        return;
    } else if constexpr (Mode == Op::Nil) {
        for (usize i = 0; i < buffer.size(); ++i) buffer[i] = 0;
    } else if constexpr (Mode == Op::Xor) {
        fast_xor_buffer(buffer);
    } else {
        usize i = 0;
        const usize count = buffer.size();
        u8* const dst = buffer.data();
        const u8* const lut = PENTAGONAL_LUT.data();
        for (; count - i >= 8; i += 8) {
            const u64 value = pack_u64_le(dst + i);
            const u64 key = pack_u64_le(lut + fast_wrap<256>(i));
            u64 result = 0;
            if constexpr (Mode == Op::Or) {
                result = value | key;
            } else if constexpr (Mode == Op::And) {
                result = value & key;
            } else if constexpr (Mode == Op::Nand) {
                result = ~(value & key);
            }
            unpack_u64_le(dst + i, result);
        }
        for (; i < count; ++i) {
            const u8 key = lut[fast_wrap<256>(i)];
            if constexpr (Mode == Op::Or) {
                dst[i] |= key;
            } else if constexpr (Mode == Op::And) {
                dst[i] &= key;
            } else if constexpr (Mode == Op::Nand) {
                dst[i] = static_cast<u8>(~(dst[i] & key));
            }
        }
    }
}

constexpr void fast_or_buffer(ByteSlice buffer) noexcept {
    fast_op_buffer<Op::Or>(buffer);
}

constexpr void fast_and_buffer(ByteSlice buffer) noexcept {
    fast_op_buffer<Op::And>(buffer);
}

constexpr void fast_nand_buffer(ByteSlice buffer) noexcept {
    fast_op_buffer<Op::Nand>(buffer);
}

constexpr void fast_nop_buffer(ByteSlice buffer) noexcept {
    fast_op_buffer<Op::Nop>(buffer);
}

constexpr void fast_nil_buffer(ByteSlice buffer) noexcept {
    fast_op_buffer<Op::Nil>(buffer);
}

}

#endif
