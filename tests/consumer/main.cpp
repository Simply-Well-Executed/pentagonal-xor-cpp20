/*
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
// SPDX-License-Identifier: 0BSD
// Copyright (c) 2026 Paul Allen Cheek
// External-consumer smoke test: includes ONLY installed public headers.

#include <swe/deca_sigma.hpp>
#include <swe/tri_pi.hpp>
#include <swe/coreao.hpp>
#include <swe/coreao_gain.hpp>
#include <swe/pentagonal_xor_std_free.h>
#include <swe/stability10d.hpp>
#include <swe/coreo_stereo.hpp>

int main() {
    const auto input = swe::deca_sigma::run(-9.0);
    if (!input.admissible || !swe::deca_sigma::witnesses_return(input))
        return 1;

    const auto three_routes = swe::tri_pi::inspect(input);
    if (!three_routes.passed()) return 2;

    const auto scalar = swe::coreao::inspect(input);
    if (!scalar.admissible || !scalar.passed()) return 3;

    // Public header stays useful independent of the CLI.
    const double features[5] = {1.0, 0.0, 1.0, 0.0, 1.0};
    if (swe_pxor_xor_parity_arr(features) != 1.0) return 4;
    // Header-only additions must survive installation as standalone APIs.
    const double measurements[9] = {3.0, 2.0, 20.0,
        2.160246899469287, 21.602468994692867, 3.0,30.0,0.0,0.0};
    swe::stability10d::Result stability{};
    if (!swe::stability10d::from_measurements(measurements, stability) ||
        !stability.admissible) return 5;
    char bytes[13]{};
    swe::stability10d::hex(stability,bytes);
    if (bytes[12] != '\0' || stability.stability < 0.0 ||
        stability.stability > 1.0) return 6;

    const float stereo_input[6] = {1,10,2,20,3,30};
    swe::coreo_stereo::Quad quad[3]{};
    for (unsigned long long i=0; i<3u; ++i)
        if (!swe::coreo_stereo::forward_at(stereo_input,3u,i,quad[i])) return 7;
    if (!swe::coreo_stereo::inspect(stereo_input,3u,quad).admissible) return 8;
    // Separate RF-domain COREAO wrapper, no changes to audio computation.
    const auto eirp = swe::coreao::rf_gain::apply(1.0);
    if (!eirp.passed() || eirp.gain_dbi != 9.0 ||
        eirp.output_eirp_watts < 7.94 ||
        eirp.output_eirp_watts > 7.95 ||
        eirp.input_watts != 1.0) return 9;
    return 0;
}
