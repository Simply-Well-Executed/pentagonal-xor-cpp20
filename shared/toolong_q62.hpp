/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
// Native TOOLONG recomputation and three independent ΣΣΣ integrity checks.
// This retains all product bits without a compiler-specific __uint128_t.
#ifndef SWE_TOOLONG_Q62_HPP
#define SWE_TOOLONG_Q62_HPP
#include "toolong_q62_core.hpp"
#include "toolong_bits.hpp"
namespace swe::toolong_q62 {
struct Audit {
    toolong exact_product;
    toolong exact_quotient;
    toolong exact_remainder;
    toolong divisor;
    bool integer_matches;
    bool remainder_in_range;
    bool suffix_matches;
};
[[nodiscard]] inline Audit recompute(const Record& r) {
    const toolong product=toolong::from_unsigned(r.source_word) *
                          toolong::from_unsigned(multiplier);
    const toolong divisor=toolong_bits::shift_left(toolong{1},shift);
    const auto qr=toolong::divmod(product,divisor);
    const bool exact=qr.first==toolong::from_unsigned(r.expected_integer) &&
                     qr.first==toolong::from_unsigned(multiply_shift62(r.source_word)) &&
                     qr.first==toolong_bits::shift_right(product,shift) &&
                     qr.first*divisor+qr.second==product;
    const bool range=!qr.second.negative() && qr.second<divisor;
    return {product,qr.first,qr.second,divisor,
            exact,range,suffix23(r)};
}
[[nodiscard]] inline bool verify(const Record& r) {
    const Audit a=recompute(r);
    return a.integer_matches && a.remainder_in_range &&
           a.suffix_matches && matches(r);
}
[[nodiscard]] inline bool verify_triforce() {
    return sigma_sigma_sigma() &&
           verify(euler) && verify(pi) && verify(intensity);
}
} // namespace swe::toolong_q62
#endif // SWE_TOOLONG_Q62_HPP
