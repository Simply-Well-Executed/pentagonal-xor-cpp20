// SPDX-License-Identifier: 0BSD
/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
#ifndef SWE_ARIARI_TOOLONG_HPP
#define SWE_ARIARI_TOOLONG_HPP
#include "ariari.hpp"
#include "toolong.hpp"
#include "coreao.hpp"
#include "gammagamma.hpp"
namespace swe::ariari {
// Optional real TOOLONG lane. Existing TOOLONG uses STL and allocation.
// Explicit source operands: no floating-point trace -> integer conversion.
struct WideCore {
    Status status; toolong original, optimized, common, a, b;
};
struct WideFusion {
    WideCore core;
    toolong redux_numerator; // signed
    // REDUX denominator = core.a; KNIGHTREX = core.a / core.b (if exact).
};
inline WideFusion thailonga(const toolong& original,const toolong& optimized) {
    if (original.negative() || optimized.negative())
        return {{Status::invalid,original,optimized,toolong{0},toolong{0},toolong{0}},toolong{0}};
    if (original.is_zero())
        return {{Status::undefined,original,optimized,toolong{0},toolong{0},toolong{0}},toolong{0}};
    const auto g=toolong::gcd(original,optimized);
    const auto a=original/g, b=optimized/g; // exact divisions by GCD
    return {{optimized.is_zero()?Status::eliminated:Status::exact,original,optimized,g,a,b},a-b};
}
inline bool ariari(const WideFusion& f,const toolong& original,const toolong& optimized) {
    const auto& c=f.core;
    if (original.is_zero() || original.negative() || optimized.negative() ||
        c.common<=toolong{0} || c.a<=toolong{0} || c.b.negative()) return false;
    return c.original==original && c.optimized==optimized &&
        c.status==(optimized.is_zero()?Status::eliminated:Status::exact) &&
        c.a*c.common==original && c.b*c.common==optimized &&
        toolong::gcd(c.a,c.b)==toolong{1} && f.redux_numerator==c.a-c.b;
}
// Outer GammaGamma result wraps separate typed witnesses. No invented split,
// no claim that a factorial result validates a work ratio or COREAO trace.
struct GammaEnvelope {
    coreao::Result coreao_witness;
    WideFusion arithmetic;
    gammagamma::Result gamma;
};
inline GammaEnvelope compose(const deca_sigma::Trace& trace,const toolong& original,
                             const toolong& optimized,std::string_view decimal_token) {
    return {coreao::inspect(trace),thailonga(original,optimized),
            gammagamma::evaluate(decimal_token)};
}
} // namespace swe::ariari
#endif
