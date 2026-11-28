/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
// Copyright (c) 2026 Paul Allen Cheek
// Exact rational arithmetic using swe::toolong, no floating point.
#ifndef SWE_TOOLONG_RATIO_HPP
#define SWE_TOOLONG_RATIO_HPP
#include "toolong.hpp"
#include <string>
#include <utility>
namespace swe {
class toolong_ratio {
    toolong num_{0}, den_{1};
    void canonicalize() {
        if (den_.is_zero()) throw std::domain_error("toolong_ratio: zero denominator");
        if (den_.negative()) { num_ = -num_; den_ = -den_; }
        if (num_.is_zero()) { den_ = toolong{1}; return; }
        const toolong d = toolong::gcd(num_.abs(), den_);
        num_ /= d;
        den_ /= d;
    }
public:
    toolong_ratio() = default;
    toolong_ratio(const toolong_ratio&) = default;
    toolong_ratio& operator=(const toolong_ratio&) = default;
    toolong_ratio(toolong_ratio&& other) : toolong_ratio() {
        std::swap(num_, other.num_);
        std::swap(den_, other.den_);
    }
    toolong_ratio& operator=(toolong_ratio&& other) {
        if (this != &other) {
            toolong_ratio tmp(std::move(other));
            std::swap(num_, tmp.num_);
            std::swap(den_, tmp.den_);
        }
        return *this;
    }
    explicit toolong_ratio(toolong numerator) : num_(std::move(numerator)) {}
    toolong_ratio(toolong numerator, toolong denominator)
      : num_(std::move(numerator)), den_(std::move(denominator)) { canonicalize(); }
    [[nodiscard]] const toolong& numerator() const noexcept { return num_; }
    [[nodiscard]] const toolong& denominator() const noexcept { return den_; }
    [[nodiscard]] std::string str() const {
        return den_ == toolong{1} ? num_.str() : num_.str() + "/" + den_.str();
    }
    friend toolong_ratio operator+(const toolong_ratio& a, const toolong_ratio& b) {
        return {a.num_ * b.den_ + b.num_ * a.den_, a.den_ * b.den_};
    }
    friend toolong_ratio operator-(const toolong_ratio& a, const toolong_ratio& b) {
        return {a.num_ * b.den_ - b.num_ * a.den_, a.den_ * b.den_};
    }
    friend toolong_ratio operator*(const toolong_ratio& a, const toolong_ratio& b) {
        return {a.num_ * b.num_, a.den_ * b.den_};
    }
    friend toolong_ratio operator/(const toolong_ratio& a, const toolong_ratio& b) {
        if (b.num_.is_zero()) throw std::domain_error("toolong_ratio: division by zero");
        return {a.num_ * b.den_, a.den_ * b.num_};
    }
    friend bool operator==(const toolong_ratio& a, const toolong_ratio& b) noexcept {
        return a.num_ == b.num_ && a.den_ == b.den_;
    }
    friend std::ostream& operator<<(std::ostream& o, const toolong_ratio& r) {
        return o << r.str();
    }
};
} // namespace swe
#endif // SWE_TOOLONG_RATIO_HPP
