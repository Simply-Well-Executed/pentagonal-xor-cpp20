// SPDX-License-Identifier: 0BSD
// Native, include-free C++20 calculation of Euler's number e.
// The exact decimal interval is derived using unsigned integer arithmetic.
// This does NOT change the domain or overflow rules of downstream functions.
#ifndef SWE_EULER_NATIVE_HPP
#define SWE_EULER_NATIVE_HPP

namespace swe::euler_native {
using word = unsigned long long;
static_assert(sizeof(word) >= 8u, "Euler calculation requires >=64-bit unsigned long long");

// The project's Pi-derived display policy is 16 significant digits.
// We calculate e at 18 fractional guard digits, then certify rounding
// to 15 fractional digits (16 significant digits, since 2 <= e < 3).
inline constexpr word scale18 = 1000000000000000000ULL;
inline constexpr word scale15 = 1000000000000000ULL;
inline constexpr word guard_divisor = 1000ULL;

struct Result {
    word lower_scaled18;
    word upper_scaled18;
    word rounded_scaled15;
    bool certified;
};

// e = sum_{k=0}^infinity 1/k!.
// floor(scale18/k!) is a nonnegative integer lower bound for each term.
// k=0..20 has 21 terms, each with floor loss <1; the remaining scaled
// tail after k=20 is <1. Therefore the exact scaled value is in
// [lower, lower+22). This interval can certify 16-digit decimal rounding.
// 20! and the accumulated scaled sum fit in 64-bit unsigned integers.
[[nodiscard]] constexpr Result compute() noexcept {
    word factorial = 1ULL;
    word lower = 0ULL;
    for (unsigned k = 0; k <= 20u; ++k) {
        if (k > 1u)
            factorial *= static_cast<word>(k);
        lower += scale18 / factorial;
    }
    const word upper = lower + 22ULL;
    const word rounded_lower = (lower + guard_divisor / 2ULL) / guard_divisor;
    const word rounded_upper = (upper + guard_divisor / 2ULL) / guard_divisor;
    return {lower, upper, rounded_lower, rounded_lower == rounded_upper};
}

inline constexpr Result result = compute();
static_assert(result.certified, "Euler's decimal rounding is not certified");

// ASCII decimal representation calculated from the integer result.
// There are 16 significant digits: one before and 15 after the dot.
struct Decimal16 { char value[18]{}; };
[[nodiscard]] constexpr Decimal16 decimal16() noexcept {
    Decimal16 out{};
    word digits = result.rounded_scaled15;
    for (int i = 15; i >= 0; --i) {
        const char digit = static_cast<char>('0' + (digits % 10ULL));
        out.value[i == 0 ? 0 : i + 1] = digit;
        digits /= 10ULL;
    }
    out.value[1] = '.';
    out.value[17] = '\0';
    return out;
}
inline constexpr Decimal16 ascii = decimal16();

// Optional binary64 adapter: only for numerical models that explicitly
// accept binary64 rounding and independently check their operation range.
[[nodiscard]] constexpr double binary64() noexcept {
    return static_cast<double>(result.rounded_scaled15) /
           static_cast<double>(scale15);
}
} // namespace swe::euler_native

#endif // SWE_EULER_NATIVE_HPP
