// SPDX-License-Identifier: 0BSD
// Copyright (c) 2026 Paul Allen Cheek
// Pentagonal XOR: a single, closed-form C++20 execution path.
// All feature inputs are finite numbers in [0,1]. Verification belongs to tests.
// Five features -> one parity score; no inversion or identity claim.

#include "shared/pentagonal_xor_std_free.h"

#include <array>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>

namespace pentagon {
constexpr unsigned feature_count = 5;
using Features = std::array<double, feature_count>;

void validate(const Features& h) {
    for (unsigned i = 0; i < feature_count; ++i) {
        if (!std::isfinite(h[i]) || h[i] < 0.0 || h[i] > 1.0) {
            throw std::invalid_argument("h[" + std::to_string(i) +
                                        "] must be finite and within [0,1]");
        }
    }
}

// Single authoritative implementation shared with the strict C11 API.
// Exactly five factors; constant work and constant memory, without minterms.
double parity(const Features& h) noexcept {
    return swe_pxor_xor_parity_arr(h.data());
}
} // namespace pentagon

int main(int argc, char** argv) {
    using namespace pentagon;
    if (argc != 6) {
        std::cerr << "Usage: " << argv[0] << " h1 h2 h3 h4 h5\n";
        return 2;
    }

    try {
        Features h{};
        for (unsigned i = 0; i < feature_count; ++i) {
            const std::string raw(argv[i + 1]);
            std::size_t consumed = 0;
            h[i] = std::stod(raw, &consumed);
            if (consumed != raw.size()) {
                throw std::invalid_argument("Invalid numeric input: " + raw);
            }
        }
        validate(h);
        std::cout << std::fixed << std::setprecision(8)
                  << "Pentagonal XOR: " << parity(h) * 100.0 << "%\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 2;
    }
}
