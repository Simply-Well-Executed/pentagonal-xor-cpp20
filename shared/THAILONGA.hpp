/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
#pragma once
namespace SWE {
template<class L,class R> struct THAILONGA {
    L left;
    R right;
    constexpr bool operator==(const THAILONGA&) const = default;
};
template<class L,class R> THAILONGA(L,R)->THAILONGA<L,R>;

// A failed candidate remains evidence, never the selected baseline.
// Caller supplies an independent verdict and explicit authorization.
template<class T> struct THAILONGA_TRIAL {
    T baseline;
    T candidate;
    bool independently_verified;
    bool authorized;
    constexpr bool accepted() const noexcept {
        return independently_verified && authorized;
    }
    constexpr const T& selected() const & noexcept {
        return accepted()?candidate:baseline;
    }
    const T& selected() const && = delete;
};
}
