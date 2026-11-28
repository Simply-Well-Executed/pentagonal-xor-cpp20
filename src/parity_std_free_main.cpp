#include "pentagonal_xor.hpp"
#include "../shared/pentagonal_xor_std_free.h"

namespace {
bool parse_unit_decimal(const char* source, double& value) noexcept {
    if (!source) return false;
    const char* p = source;
    while (*p == ' ' || *p == '\t' || *p == '\r' ||
           *p == '\n' || *p == '\f' || *p == '\v') ++p;
    bool negative = false;
    if (*p == '+' || *p == '-') {
        negative = (*p == '-');
        ++p;
    }
    bool seen_digit = false;
    bool decimal_point = false;
    pxor::u64 mantissa = 0;
    int scale = 0;
    for (;;) {
        if (*p >= '0' && *p <= '9') {
            seen_digit = true;
            const pxor::u64 digit = static_cast<pxor::u64>(*p - '0');
            if (mantissa <= 100000000000000000ull) {
                mantissa = mantissa * 10u + digit;
                if (decimal_point) --scale;
            } else if (!decimal_point) {
                ++scale;
            }
            ++p;
        } else if (*p == '.' && !decimal_point) {
            decimal_point = true;
            ++p;
        } else {
            break;
        }
    }
    if (!seen_digit) return false;
    if (*p == 'e' || *p == 'E') {
        ++p;
        bool minus = false;
        if (*p == '+' || *p == '-') {
            minus = *p == '-';
            ++p;
        }
        if (*p < '0' || *p > '9') return false;
        int exponent = 0;
        while (*p >= '0' && *p <= '9') {
            if (exponent > 400) return false;
            exponent = exponent * 10 + (*p - '0');
            ++p;
        }
        scale += minus ? -exponent : exponent;
    }
    if (*p != '\0' || scale < -400 || scale > 400) return false;
    double result = static_cast<double>(mantissa);
    while (scale > 0) {
        result *= 10.0;
        if (result > 1.0) return false;
        --scale;
    }
    while (scale < 0) {
        result /= 10.0;
        ++scale;
    }
    if (result < 0.0 || result > 1.0 ||
        (negative && result != 0.0)) return false;
    value = negative ? -result : result;
    return true;
}

void print_percent_eight(double probability) noexcept {
    if (probability < 0.0) probability = 0.0;
    if (probability > 1.0) probability = 1.0;
    const double scaled = probability * 10000000000.0;
    pxor::u64 rounded = static_cast<pxor::u64>(scaled);
    const double fraction = scaled - static_cast<double>(rounded);
    if (fraction > 0.5 || (fraction == 0.5 && (rounded & 1u))) {
        ++rounded;
    }
    pxor::write_literal("Pentagonal XOR: ");
    pxor::write_u64(rounded / 100000000u);
    pxor::write_literal(".");
    pxor::u64 rest = rounded % 100000000u;
    char digits[8]{};
    for (int i = 7; i >= 0; --i) {
        digits[i] = static_cast<char>('0' + (rest % 10u));
        rest /= 10u;
    }
    pxor::write_raw(digits, 8);
    pxor::write_literal("%\n");
}
}

#include "WATCHGUARD.hpp"
static int watchguard_operation(int argc, char** argv) {
    if (argc != 6) {
        pxor::write_literal("Usage: pentagonal_xor_std_free h1 h2 h3 h4 h5\n",
                            true);
        return 2;
    }
    double features[5]{};
    for (unsigned i = 0; i < 5; ++i) {
        if (!parse_unit_decimal(argv[i + 1], features[i])) {
            pxor::write_literal("Error: invalid decimal feature in [0,1]\n",
                                true);
            return 2;
        }
    }
    print_percent_eight(swe_pxor_xor_parity_arr(features));
    return 0;
}

int main(int argc, char** argv) {
    return SWE::WATCHGUARD_MAIN(watchguard_operation,argc,argv);
}
