// SPDX-License-Identifier: 0BSD
// Copyright (c) 2026 Paul Allen Cheek
// toolong: signed arbitrary-precision integer, C++20, exact decimal I/O.
// Digit count is limited only by available resources, not a fixed integer width.
#ifndef SWE_TOOLONG_HPP
#define SWE_TOOLONG_HPP

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <ostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace swe {
class toolong {
    static constexpr std::uint32_t BASE = 1'000'000'000u;
    std::vector<std::uint32_t> limbs_{0u}; // Little-endian base 10^9.
    bool neg_ = false;

    void normalize() noexcept {
        while (limbs_.size() > 1u && limbs_.back() == 0u) limbs_.pop_back();
        if (limbs_.size() == 1u && limbs_[0] == 0u) neg_ = false;
    }
    [[nodiscard]] int cmp_abs(const toolong& b) const noexcept {
        if (limbs_.size() != b.limbs_.size())
            return limbs_.size() < b.limbs_.size() ? -1 : 1;
        for (std::size_t i = limbs_.size(); i-- > 0u;)
            if (limbs_[i] != b.limbs_[i]) return limbs_[i] < b.limbs_[i] ? -1 : 1;
        return 0;
    }
    void add_abs(const toolong& b) {
        const auto n = std::max(limbs_.size(), b.limbs_.size());
        limbs_.resize(n + 1u, 0u);
        std::uint64_t carry = 0u;
        for (std::size_t i = 0; i < n; ++i) {
            const std::uint64_t digit = std::uint64_t(limbs_[i]) +
                (i < b.limbs_.size() ? b.limbs_[i] : 0u) + carry;
            limbs_[i] = std::uint32_t(digit % BASE);
            carry = digit / BASE;
        }
        limbs_[n] = std::uint32_t(carry);
        normalize();
    }
    // Precondition: |*this| >= |b|.
    void sub_abs(const toolong& b) noexcept {
        std::int64_t borrow = 0;
        for (std::size_t i = 0; i < limbs_.size(); ++i) {
            std::int64_t digit = std::int64_t(limbs_[i]) - borrow -
                (i < b.limbs_.size() ? b.limbs_[i] : 0u);
            if (digit < 0) { digit += BASE; borrow = 1; }
            else borrow = 0;
            limbs_[i] = std::uint32_t(digit);
        }
        normalize();
    }
    [[nodiscard]] toolong mul_digit(std::uint32_t digit) const {
        if (digit == 0u || is_zero()) return {};
        toolong out;
        out.limbs_.assign(limbs_.size() + 1u, 0u);
        std::uint64_t carry = 0u;
        for (std::size_t i = 0; i < limbs_.size(); ++i) {
            const std::uint64_t p = std::uint64_t(limbs_[i]) * digit + carry;
            out.limbs_[i] = std::uint32_t(p % BASE);
            carry = p / BASE;
        }
        out.limbs_.back() = std::uint32_t(carry);
        out.normalize();
        return out;
    }
    // Multiply by BASE and add a low limb, without creating numeric overflow.
    void append_low_limb(std::uint32_t digit) {
        if (is_zero()) limbs_[0] = digit;
        else limbs_.insert(limbs_.begin(), digit);
    }
    [[nodiscard]] static std::pair<toolong, toolong> divmod_abs(
        const toolong& dividend, const toolong& divisor) {
        toolong quotient, remainder;
        if (divisor.limbs_.size() == 1u) {
            const std::uint64_t d = divisor.limbs_[0];
            std::uint64_t rem = 0u;
            quotient.limbs_.assign(dividend.limbs_.size(), 0u);
            for (std::size_t i = dividend.limbs_.size(); i-- > 0u;) {
                const std::uint64_t cur = rem * BASE + dividend.limbs_[i];
                quotient.limbs_[i] = std::uint32_t(cur / d);
                rem = cur % d;
            }
            quotient.normalize();
            return {std::move(quotient), from_unsigned(rem)};
        }
        quotient.limbs_.assign(dividend.limbs_.size(), 0u);
        for (std::size_t i = dividend.limbs_.size(); i-- > 0u;) {
            remainder.append_low_limb(dividend.limbs_[i]);
            if (remainder.cmp_abs(divisor) < 0) continue;
            // Binary search each quotient limb in [0, BASE-1].
            std::int64_t low = 0, high = BASE - 1u;
            std::uint32_t best = 0u;
            while (low <= high) {
                const auto mid = std::uint32_t(low + (high - low) / 2);
                const auto trial = divisor.mul_digit(mid);
                if (trial.cmp_abs(remainder) <= 0) {
                    best = mid; low = std::int64_t(mid) + 1;
                } else high = std::int64_t(mid) - 1;
            }
            quotient.limbs_[i] = best;
            remainder.sub_abs(divisor.mul_digit(best));
        }
        quotient.normalize();
        remainder.normalize();
        return {std::move(quotient), std::move(remainder)};
    }
public:
    toolong() = default;
    toolong(const toolong&) = default;
    toolong& operator=(const toolong&) = default;
    toolong(toolong&& other) : toolong() {
        limbs_.swap(other.limbs_);
        std::swap(neg_, other.neg_);
    }
    toolong& operator=(toolong&& other) {
        if (this != &other) {
            toolong tmp(std::move(other));
            limbs_.swap(tmp.limbs_);
            std::swap(neg_, tmp.neg_);
        }
        return *this;
    }
    explicit toolong(std::int64_t value) {
        const bool negative = value < 0;
        const std::uint64_t mag = negative ?
            std::uint64_t(-(value + 1)) + 1u : std::uint64_t(value);
        *this = from_unsigned(mag);
        neg_ = negative && !is_zero();
    }
    explicit toolong(std::string_view decimal) {
        std::size_t start = 0u;
        if (!decimal.empty() && (decimal.front() == '-' || decimal.front() == '+')) {
            neg_ = decimal.front() == '-'; start = 1u;
        }
        if (start == decimal.size())
            throw std::invalid_argument("toolong: missing decimal digits");
        for (std::size_t i = start; i < decimal.size(); ++i)
            if (decimal[i] < '0' || decimal[i] > '9')
                throw std::invalid_argument("toolong: invalid decimal digit");
        limbs_.clear();
        for (std::size_t end = decimal.size(); end > start;) {
            const auto begin = end - start > 9u ? end - 9u : start;
            std::uint32_t digit = 0u;
            for (std::size_t i = begin; i < end; ++i)
                digit = 10u * digit + unsigned(decimal[i] - '0');
            limbs_.push_back(digit);
            end = begin;
        }
        normalize();
    }
    [[nodiscard]] static toolong from_unsigned(std::uint64_t value) {
        toolong out;
        out.limbs_.clear();
        do {
            out.limbs_.push_back(std::uint32_t(value % BASE));
            value /= BASE;
        } while (value);
        return out;
    }
    [[nodiscard]] bool is_zero() const noexcept {
        return limbs_.size() == 1u && limbs_[0] == 0u;
    }
    [[nodiscard]] bool negative() const noexcept { return neg_; }
    [[nodiscard]] std::size_t limb_count() const noexcept { return limbs_.size(); }
    [[nodiscard]] std::string str() const {
        std::string text = neg_ ? "-" : "";
        text += std::to_string(limbs_.back());
        for (std::size_t i = limbs_.size(); i > 1u; --i) {
            const std::string chunk = std::to_string(limbs_[i - 2u]);
            text.append(9u - chunk.size(), '0');
            text += chunk;
        }
        return text;
    }
    [[nodiscard]] std::size_t decimal_digits() const {
        return str().size() - (neg_ ? 1u : 0u);
    }
    [[nodiscard]] toolong abs() const {
        toolong out = *this; out.neg_ = false; return out;
    }
    [[nodiscard]] toolong operator-() const {
        toolong out = *this;
        if (!out.is_zero()) out.neg_ = !out.neg_;
        return out;
    }
    [[nodiscard]] int compare(const toolong& b) const noexcept {
        if (neg_ != b.neg_) return neg_ ? -1 : 1;
        const int c = cmp_abs(b);
        return neg_ ? -c : c;
    }
    friend bool operator==(const toolong& a, const toolong& b) noexcept {
        return a.compare(b) == 0;
    }
    friend bool operator!=(const toolong& a, const toolong& b) noexcept {
        return !(a == b);
    }
    friend bool operator<(const toolong& a, const toolong& b) noexcept {
        return a.compare(b) < 0;
    }
    friend bool operator>(const toolong& a, const toolong& b) noexcept {
        return a.compare(b) > 0;
    }
    friend bool operator<=(const toolong& a, const toolong& b) noexcept {
        return a.compare(b) <= 0;
    }
    friend bool operator>=(const toolong& a, const toolong& b) noexcept {
        return a.compare(b) >= 0;
    }
    toolong& operator+=(const toolong& b) {
        if (neg_ == b.neg_) { add_abs(b); return *this; }
        const int c = cmp_abs(b);
        if (c == 0) { *this = toolong{}; return *this; }
        if (c > 0) { sub_abs(b); return *this; }
        toolong other = b.abs();
        other.sub_abs(*this);
        other.neg_ = b.neg_;
        *this = std::move(other);
        return *this;
    }
    toolong& operator-=(const toolong& b) { return *this += -b; }
    toolong& operator*=(const toolong& b) {
        if (is_zero() || b.is_zero()) { *this = toolong{}; return *this; }
        toolong product;
        product.limbs_.assign(limbs_.size() + b.limbs_.size(), 0u);
        for (std::size_t i = 0; i < limbs_.size(); ++i) {
            std::uint64_t carry = 0;
            for (std::size_t j = 0; j < b.limbs_.size(); ++j) {
                const std::uint64_t p =
                    std::uint64_t(limbs_[i]) * b.limbs_[j] +
                    product.limbs_[i + j] + carry;
                product.limbs_[i + j] = std::uint32_t(p % BASE);
                carry = p / BASE;
            }
            std::size_t k = i + b.limbs_.size();
            while (carry != 0u) {
                // The full product is at most lhs.size()+rhs.size() limbs.
                const std::uint64_t p = product.limbs_[k] + carry;
                product.limbs_[k] = std::uint32_t(p % BASE);
                carry = p / BASE;
                ++k;
            }
        }
        product.neg_ = neg_ != b.neg_;
        product.normalize();
        *this = std::move(product);
        return *this;
    }
    [[nodiscard]] static std::pair<toolong, toolong> divmod(
        const toolong& a, const toolong& b) {
        if (b.is_zero()) throw std::domain_error("toolong: division by zero");
        auto qr = divmod_abs(a.abs(), b.abs());
        if (!qr.first.is_zero()) qr.first.neg_ = (a.neg_ != b.neg_);
        if (!qr.second.is_zero()) qr.second.neg_ = a.neg_;
        return qr; // C++ semantics: quotient truncates toward zero.
    }
    toolong& operator/=(const toolong& b) { *this = divmod(*this, b).first; return *this; }
    toolong& operator%=(const toolong& b) { *this = divmod(*this, b).second; return *this; }
    friend toolong operator+(toolong a, const toolong& b) { a += b; return a; }
    friend toolong operator-(toolong a, const toolong& b) { a -= b; return a; }
    friend toolong operator*(toolong a, const toolong& b) { a *= b; return a; }
    friend toolong operator/(toolong a, const toolong& b) { a /= b; return a; }
    friend toolong operator%(toolong a, const toolong& b) { a %= b; return a; }
    friend std::ostream& operator<<(std::ostream& o, const toolong& n) {
        return o << n.str();
    }
    // Remainder in [0, |divisor|), irrespective of dividend/divisor signs.
    [[nodiscard]] static toolong euclidean_mod(const toolong& a, const toolong& divisor) {
        toolong r = a % divisor;
        if (r.negative()) r += divisor.abs();
        return r;
    }
    [[nodiscard]] static toolong gcd(toolong a, toolong b) {
        a = a.abs(); b = b.abs();
        while (!b.is_zero()) {
            toolong r = a % b;
            a = std::move(b); b = std::move(r);
        }
        return a;
    }
    [[nodiscard]] static toolong power(toolong base, std::uint64_t exponent,
                                       std::size_t max_digits = 0u) {
        toolong result{1};
        auto ensure = [max_digits](const toolong& a, const toolong& b) {
            if (max_digits == 0u || a.is_zero() || b.is_zero()) return;
            const std::size_t a_digits = a.decimal_digits();
            const std::size_t b_digits = b.decimal_digits();
            if (a_digits > max_digits || b_digits > max_digits ||
                a_digits > max_digits - b_digits + 1u)
                throw std::length_error("toolong: power exceeds configured digit budget");
        };
        while (exponent != 0u) {
            if (exponent & 1u) {
                ensure(result, base);
                result *= base;
                if (max_digits != 0u && result.decimal_digits() > max_digits)
                    throw std::length_error("toolong: power exceeds configured digit budget");
            }
            exponent >>= 1u;
            if (exponent != 0u) {
                ensure(base, base);
                base *= base;
                if (max_digits != 0u && base.decimal_digits() > max_digits)
                    throw std::length_error("toolong: power exceeds configured digit budget");
            }
        }
        return result;
    }
    [[nodiscard]] static toolong factorial(unsigned n) {
        toolong result{1};
        for (unsigned k = 2u; k <= n; ++k) {
            result *= from_unsigned(k);
            if (k == n) break; // Avoid overflow if n==UINT_MAX.
        }
        return result;
    }
};
} // namespace swe
#endif // SWE_TOOLONG_HPP
