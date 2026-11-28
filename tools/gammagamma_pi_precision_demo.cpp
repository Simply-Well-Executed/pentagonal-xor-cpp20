/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
// Copyright (c) 2026 Paul Allen Cheek
// C++20 output contract: computed Pi-derived precision, no hard-coded 6/8 digit displays.
#include "../shared/gammagamma.hpp"
#include "../shared/pi_precision.hpp"
#include <cmath>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string_view>

constexpr std::uint64_t factorial_u64(unsigned n) noexcept {
    if(n>20u)return 0u; // overflow sentinel; no silent wrap
    std::uint64_t result=1u;
    for(unsigned i=2u;i<=n;++i)result*=i;
    return result;
}

int main() {
    using namespace swe;
    // Always establish the project display precision before writing floating values.
    std::cout << std::setprecision(pi_precision::display_digits);
    static_assert(pi_precision::display_digits==16);
    static_assert(factorial_u64(5)==120u);
    static_assert(factorial_u64(20)==2432902008176640000ULL);
    static_assert(factorial_u64(21)==0u);
    std::cout << "PI_DIGITS=" << pi_precision::pi_digits
              << " OUTPUT_PRECISION=" << pi_precision::display_digits << '\n';
    std::cout << "TGAMMA 3.14!=" << std::tgamma(3.14+1.0) << '\n';

    const gammagamma::Result q=gammagamma::evaluate("3.14");
    if(!q.exact())return 1;
    std::cout << "GAMMAGAMMA !3.14 signed_direction=" << static_cast<int>(q.direction)
              << " nonnegative_magnitude=" << q.magnitude << '\n';

    const gammagamma::Result symbol=gammagamma::sigma_pi("3.14159265358979323");
    std::cout << "SIGMAPI status=" << static_cast<int>(symbol.status)
              << " source=" << symbol.source << '\n';
    std::cout << "BIPI exact_square=" << gammagamma::bipi_square << '\n'
              << "PIPI symbolic=" << gammagamma::pipi_symbolic << '\n'
              << "PIDI modulo4=" << gammagamma::pidi_mod4 << " (I unresolved)\n";
    return (q.magnitude=="87178291194"
            && q.direction==gammagamma::Direction::negative
            && symbol.status==gammagamma::Status::symbolic_only)
        ? 0 : 2;
}
