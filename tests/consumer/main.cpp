// SPDX-License-Identifier: 0BSD
// Copyright (c) 2026 Paul Allen Cheek
// External-consumer smoke test: includes ONLY installed public headers.

#include <swe/deca_sigma.hpp>
#include <swe/tri_pi.hpp>
#include <swe/coreao.hpp>
#include <swe/pentagonal_xor_std_free.h>

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
    return 0;
}
