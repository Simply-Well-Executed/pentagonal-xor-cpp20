// SPDX-License-Identifier: 0BSD
// Copyright (c) 2026 Paul Allen Cheek
// External-consumer smoke test: includes ONLY installed public headers.

#include <swe/deca_sigma.hpp>
#include <swe/tri_pi.hpp>
#include <swe/coreao.hpp>
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

    const float input[6] = {1,10,2,20,3,30};
    swe::coreo_stereo::Quad quad[3]{};
    for (unsigned long long i=0; i<3u; ++i)
        if (!swe::coreo_stereo::forward_at(input,3u,i,quad[i])) return 7;
    if (!swe::coreo_stereo::inspect(input,3u,quad).admissible) return 8;
    return 0;
}
