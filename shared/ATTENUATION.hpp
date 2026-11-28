/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
// ATTENUATION/1: exact integer bridge and distinct semantic/display witnesses.
// EXPLORATORY: not an audio filter, dB measurement or quantum operator.
#ifndef SWE_ATTENUATION_HPP
#define SWE_ATTENUATION_HPP
struct ATTENUATION {
    using U = unsigned int;
    using I = long long;
    static_assert(~U{0} == 4294967295U);
    static_assert(sizeof(I) >= 8);

    enum class Cohort : unsigned { unknown, classified, pennyworth, other };
    struct Count { U value; Cohort cohort; };
    struct Ratio { bool valid; I numerator; U denominator; };
    struct Result {
        Count baseline, observed;
        U requested;
        I gain, shortfall, bridge, attenuated;
        bool comparable, cross_through;
        Ratio transfer;
    };

    // Unicode labels only; never executable arithmetic or a language-specific operator.
    static constexpr char32_t bridge_symbol = U'\u2261';
    static constexpr char32_t cross_symbol = U'\u2260';
    static constexpr char8_t bridge_utf8[] = u8"\u2261";
    static constexpr char8_t cross_utf8[] = u8"\u2260";

    [[nodiscard]] static constexpr I BRIDGE(U requested, U baseline) noexcept {
        return I(requested) - I(baseline);
    }

    [[nodiscard]] static constexpr Result evaluate(
        Count baseline, U requested, Count observed
    ) noexcept {
        const I g = I(observed.value) - I(baseline.value);
        const I r = I(requested);
        const I b = BRIDGE(requested, baseline.value);
        const bool same = baseline.cohort != Cohort::unknown &&
                          baseline.cohort == observed.cohort;
        return {baseline, observed, requested,
                g, r-g, b, g+b, same, g!=r,
                {same && requested!=0U, g, requested}};
    }

    // EULERS_JIG: exact quotient with supplied square-side witnesses.
    // This is a proposed geometric compensation, not a physical dBi model.
    struct Jig {
        bool valid;
        I loss, divisor, remainder, correction, compensated;
    };
    [[nodiscard]] static constexpr Jig EULERS_JIG(
        U gain, U requested, U gain_side, U requested_side
    ) noexcept {
        using W = unsigned long long;
        if (W(gain_side)*gain_side != gain ||
            W(requested_side)*requested_side != requested)
            return {};
        const I loss = I(gain)-I(requested);
        const I divisor = I(gain_side)+I(requested_side);
        if (!divisor) return {};
        const I remainder = (-loss)%divisor;
        if (remainder) return {false,loss,divisor,remainder,0,0};
        const I correction = (-loss)/divisor;
        return {true,loss,divisor,remainder,correction,loss+correction};
    }

    // DE-RADICALIZATION: retain independent sign branches of both squares.
    // Zero has one distinct root; duplicate orientations remain labeled.
    struct SignedJig {
        bool valid;
        I gain_root, requested_root, divisor, remainder, correction, compensated;
    };
    [[nodiscard]] static constexpr SignedJig oriented(
        U gain, U requested, U gain_side, U requested_side,
        bool negative_gain, bool negative_requested
    ) noexcept {
        using W = unsigned long long;
        if (W(gain_side)*gain_side != gain ||
            W(requested_side)*requested_side != requested)
            return {};
        const I x=negative_gain ? -I(gain_side) : I(gain_side);
        const I y=negative_requested ? -I(requested_side) : I(requested_side);
        const I d=x+y, delta=I(requested)-I(gain);
        // x+y=0 is a singular quotient, even though y-x is defined.
        if (!d) return {false,x,y,d,delta,0,0};
        const I rem=delta%d;
        if (rem) return {false,x,y,d,rem,0,0};
        const I q=delta/d;
        return {q==y-x,x,y,d,rem,q,-delta+q};
    }
    struct Branches { SignedJig pp, nn, pn, np; };
    [[nodiscard]] static constexpr Branches de_radicalize(
        U gain,U requested,U gain_side,U requested_side
    ) noexcept {
        return {oriented(gain,requested,gain_side,requested_side,false,false),
                oriented(gain,requested,gain_side,requested_side,true,true),
                oriented(gain,requested,gain_side,requested_side,false,true),
                oriented(gain,requested,gain_side,requested_side,true,false)};
    }

    // Audit against the external input record, not the report's own inputs.
    // Algebraically independent expansion stays within signed 64-bit range.
    [[nodiscard]] static constexpr bool audit(
        const Result& w, Count baseline, U requested, Count observed
    ) noexcept {
        const I b=I(baseline.value), o=I(observed.value), r=I(requested);
        const bool same=baseline.cohort!=Cohort::unknown &&
                        baseline.cohort==observed.cohort;
        return w.baseline.value==baseline.value &&
               w.baseline.cohort==baseline.cohort &&
               w.observed.value==observed.value &&
               w.observed.cohort==observed.cohort &&
               w.requested==requested &&
               w.gain==o-b && w.shortfall==b+r-o &&
               w.bridge==r-b && w.attenuated==o+r-2*b &&
               w.comparable==same && w.cross_through==(o!=b+r) &&
               w.transfer.valid==(same&&requested!=0U) &&
               w.transfer.numerator==o-b && w.transfer.denominator==requested;
    }
};
#endif
