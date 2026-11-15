// SPDX-License-Identifier: 0BSD
// Copyright (c) 2026 Paul Allen Cheek
// pentagonal_xor_cpp20.cpp
// C++20 implementation of a recursive fuzzy-XOR parity operator.
// The five inputs h_i are independent model feature-agreement values in [0, 1].
// This is a mathematical forward transform, not an identity/kinship detector
// and not a lossless or invertible compression codec.

#include <array>
#include <bit>
#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>

namespace pentagon {

constexpr std::size_t feature_count = 5;
using Features = std::array<double, feature_count>;

// X_k = X_(k-1) * (1 - h_k) + (1 - X_(k-1)) * h_k
// A soft XOR, equivalent to OR over mutually exclusive AND terms.
constexpr double xor_step(double previous, double h) noexcept {
    return previous * (1.0 - h) + (1.0 - previous) * h;
}

void validate(const Features& features) {
    for (std::size_t i = 0; i < features.size(); ++i) {
        const double h = features[i];
        if (!std::isfinite(h) || h < 0.0 || h > 1.0) {
            throw std::invalid_argument("h[" + std::to_string(i) +
                                        "] must be finite and within [0, 1]");
        }
    }
}

// O(5) recursive fold (implemented iteratively to use constant stack space).
constexpr double xor_parity(const Features& features) noexcept {
    double state = 0.0;  // X_0 = 0
    for (double h : features) {
        state = xor_step(state, h);
    }
    return state;
}

// O(5) closed form: odd parity for independent features.
// Explicit opt-in fast mode avoids enumerating 32 minterms at runtime.
constexpr double xor_parity_closed_form(const Features& features) noexcept {
    double product = 1.0;
    for (double h : features) {
        product *= (1.0 - 2.0 * h);
    }
    return 0.5 * (1.0 - product);
}

// Independent validation: sum of the 16 odd-parity minterms out of 2^5 = 32.
double xor_by_minterms(const Features& features) {
    double total = 0.0;
    for (unsigned mask = 0; mask < (1u << feature_count); ++mask) {
        if ((std::popcount(mask) % 2) == 0) continue;
        double and_term = 1.0;
        for (std::size_t i = 0; i < feature_count; ++i) {
            and_term *= (mask & (1u << i)) ? features[i] : (1.0 - features[i]);
        }
        total += and_term;  // OR of disjoint minterms is their sum.
    }
    return total;
}

// Compile-time exhaustive check: every Boolean input state agrees with parity.
consteval bool all_boolean_states_agree() {
    for (unsigned mask = 0; mask < (1u << feature_count); ++mask) {
        Features bits{};
        for (std::size_t i = 0; i < feature_count; ++i) {
            bits[i] = ((mask >> i) & 1u) ? 1.0 : 0.0;
        }
        const double expected = (std::popcount(mask) & 1) ? 1.0 : 0.0;
        if (xor_parity(bits) != expected ||
            xor_parity_closed_form(bits) != expected) {
            return false;
        }
    }
    return true;
}
static_assert(all_boolean_states_agree());

static_assert(xor_parity(Features{0, 0, 0, 0, 0}) == 0.0);
static_assert(xor_parity(Features{1, 0, 0, 0, 0}) == 1.0);
static_assert(xor_parity(Features{1, 1, 0, 0, 0}) == 0.0);
static_assert(xor_parity(Features{1, 1, 1, 0, 0}) == 1.0);

}  // namespace pentagon

int main(int argc, char** argv) {
    using namespace pentagon;
    try {
        // Illustrative defaults only; replace them with your frozen h_1..h_5.
        Features h{0.80, 0.70, 0.90, 0.60, 0.95};
        const bool fast = argc >= 2 && std::string(argv[1]) == "--fast";
        const int first_feature_arg = fast ? 2 : 1;
        const bool custom_features = argc == first_feature_arg + 5;
        if (argc != first_feature_arg && !custom_features) {
            std::cerr << "Usage: " << argv[0]
                      << " [--fast] [h1 h2 h3 h4 h5]  (each within [0,1])\n";
            return 2;
        }
        if (custom_features) {
            for (std::size_t i = 0; i < feature_count; ++i) {
                const std::string input = argv[i + first_feature_arg];
                std::size_t parsed = 0;
                h[i] = std::stod(input, &parsed);
                if (parsed != input.size()) {
                    throw std::invalid_argument("Invalid numeric input: " + input);
                }
            }
        }
        validate(h);
        std::cout << std::fixed << std::setprecision(8);
        if (fast) {
            const double result = xor_parity_closed_form(h);
            std::cout << "Fast XOR : " << result * 100.0 << "%\n";
            return 0;
        }
        double state = 0.0;
        for (std::size_t i = 0; i < h.size(); ++i) {
            state = xor_step(state, h[i]);
            std::cout << "X_" << (i + 1) << " = " << state << '\n';
        }
        const double direct = xor_parity(h);
        const double enumerated = xor_by_minterms(h);
        std::cout << "Recursive XOR : " << direct * 100.0 << "%\n"
                  << "32-state check: " << enumerated * 100.0 << "%\n"
                  << "Absolute error: " << std::abs(direct - enumerated) << '\n';
        return std::abs(direct - enumerated) < 1e-12 ? 0 : 1;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 2;
    }
}
