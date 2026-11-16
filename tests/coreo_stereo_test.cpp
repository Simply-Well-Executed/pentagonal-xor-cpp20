// SPDX-License-Identifier: 0BSD
// Copyright (c) 2026 Paul Allen Cheek
#include "../shared/coreo_stereo.hpp"
#include <cmath>
#include <iostream>
#include <limits>

int main() {
    const float original[6] = {1,10,2,20,3,30};
    const float immutable[6] = {1,10,2,20,3,30};
    const swe::coreo_stereo::Quad expected[3] = {
        {-3,-30,-1,-10}, {-2,-20,-2,-20}, {-1,-10,-3,-30}
    };
    swe::coreo_stereo::Quad actual[3]{};
    for(unsigned long long i=0; i<3u; ++i) {
        if (!swe::coreo_stereo::forward_at(original,3u,i,actual[i])) return 1;
        if (actual[i].yin_left != expected[i].yin_left ||
            actual[i].yin_right != expected[i].yin_right ||
            actual[i].yang_left != expected[i].yang_left ||
            actual[i].yang_right != expected[i].yang_right) return 2;

        float left=0.0f,right=0.0f;
        if (!swe::coreo_stereo::reverse_at(actual[i],left,right) ||
            left != original[2u*i] || right != original[2u*i+1u]) return 3;
    }
    for(unsigned i=0; i<6u; ++i)
        if (original[i] != immutable[i]) return 4;
    const auto audit=swe::coreo_stereo::inspect(original,3u,actual);
    if (!audit.admissible || audit.largest_disagreement != 0.0) return 5;

    actual[0].yin_left += 0.5f;
    const auto tampered=swe::coreo_stereo::inspect(original,3u,actual);
    if (!tampered.admissible || tampered.largest_disagreement <= 0.0) return 6;
    if (swe::coreo_stereo::forward_at(original,3u,3u,actual[0])) return 7;
    if (swe::coreo_stereo::forward_at(nullptr,3u,0u,actual[0])) return 8;
    if (swe::coreo_stereo::inspect(original,0u,actual).admissible) return 9;

    const float invalid[2]={std::numeric_limits<float>::infinity(),1.0f};
    if (swe::coreo_stereo::forward_at(invalid,1u,0u,actual[0])) return 10;
    std::cout << "PASS Coreo stereo: exact PR17 golden, inverse, Z-fold, tamper, "
                 "source identity and invalid guards\n";
    return 0;
}
