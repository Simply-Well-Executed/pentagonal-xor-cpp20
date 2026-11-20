# toolong: arbitrary-precision exact C++20 integer arithmetic

> **EXPLORATORY** · TOOLONG-CXX20-EXACT-INTEGER-v1 · Copyright (c) 2026 Paul Allen Cheek · SPDX-License-Identifier: 0BSD

## Design

\`swe::toolong\` is a distinct user-defined C++20 integer class, not an alias for \`long long\`. It stores **signed integers in a vector of little-endian base-10^9 limbs** and renders complete decimal strings without \`std::setprecision\`, IEEE-754 floating-point conversion, or a fixed bit-width ceiling. \`std::setprecision(16)\` remains the project's chosen floating-point **display** convention; applying it to exact integers would be a lossy and incorrect transformation.

**"Unlimited necessary digits" means there is no built-in 16/64/128-bit cap**, not that physical memory, computing time, output length or operating-system resources are infinite. Exhausting resources is a legitimate error, not a license to truncate or fabricate results.

## Covered operators and related types

| API | Semantics |
| --- | --- |
| \`toolong(std::string_view)\` | Parse ASCII decimal digits with optional leading sign, reject nondecimal input, normalize negative zero |
| \`+ - unary- *\` | Exact signed arithmetic, all result digits |
| \`/ %\` | C++-compatible integer division, quotient toward zero, remainder signed like dividend |
| \`toolong::euclidean_mod\` | Remainder from zero through abs(divisor)-1 |
| \`== != < > <= >=\` | Exact signed comparisons |
| \`toolong::power(base, exponent, optional_max_digits)\` | Integer exponentiation by squaring, including optional guard against impractical results |
| \`toolong::factorial(n)\` | Exact nonnegative integer factorial, \`0! = 1\`; no finite precision cap |
| \`toolong::gcd(a,b)\` | Euclidean greatest common divisor |
| \`swe::toolong_bits::bitwise_xor/and/or\` | Exact nonnegative-integer bit operations, no finite width imposed |
| \`shift_left/right\` | Nonnegative bit shifts |
| \`bitwise_not_width\`, \`rotate_left_width\` | Requires declared bit width, like \`std::rotl\` |
| \`swe::toolong_ratio\` | Exact reduced signed rational values, \`+ - * /\`, positive denominator |
| \`swe::inspect_photon_counts\` | Exact counts, gain ratio, mean, variance (dispersion), and Fano statistic |

**Scope notes:** \`toolong\` performs integer division, not fractional division; use \`toolong_ratio\` when a rational result is needed. \`/0\` throws. Negative-input bitwise operations are rejected rather than inventing unspecified infinite two's-complement widths. The \`^!\` notation of the user's exploratory Pi family is not silently substituted for C++ bitwise XOR or logical NOT.

For enormous powers such as \`π^π\`, exact mathematical integer results exist but materializing them would require astronomical resources. This version retains such expressions **symbolically** rather than pretending they were calculated. \`BiPi = π2\` is directly calculated with \`toolong\` and equals \`98696044010893585656621344341538329\`. \`PiDi = π % 4\` evaluates to 3; the symbolic identity \`I=3\` remains Needs Review.

## GammaGamma integration

\`shared/gammagamma.hpp\` now delegates its actual factorial operations to \`swe::toolong::factorial\`. The GammaGamma **trial policy** still limits input factorial arguments to 2048 to guard against accidental runaway work, but this cap is **not a precision restriction of \`toolong\`**. Under its accepted range, \`!3.14\` has exact magnitude \`87178291194\` with separately recorded negative direction. The user-authored \`3.014\` source remains distinct in its ledger even though it has the same integer-component factorial result.

## Synthetic photon-count gain and dispersion

For nonnegative counts \`c[0..N-1]\` and positive incident count per trial \`I\`, the exploratory counting adapter defines:

\`\`\`text
Total = sum(c[i])
Gain = Total/(N*I)                 (dimensionless output/input count ratio)
Mean = Total/N                     (detected counts per trial)
Dispersion = (N*sum(c[i]^2)-Total^2)/N^2 (population variance; counts^2)
Fano = Dispersion/Mean             (if Mean > 0)
\`\`\`

These are **exact rational arithmetic statements**, not inferred physical radiation gain, lens dispersion, or quantum state evolution. Any quantum/measured-photon interpretation requires calibrated detectors, shot-noise model, sample independence and uncertainty assessment. Synthetic counts cannot establish stimulated emission, field gain, a genuine radiation process, or a quantum phenomenon.

For \`I=100\` and synthetic counts \`105,120,135\`:

| Derived value | Exact |
| --- | --- |
| Total | 360 |
| Gain | 6/5 |
| Mean | 120 |
| Dispersion (variance) | 150 |
| Fano ratio | 5/4 |

No floating-point digits are lost in these calculations. If plotting the ratios as floating-point, use the π-derived \`std::setprecision(16)\` presentation policy, not as a substitute for the preserved exact fractions.

## Evidence and testing

- The authored \`tests/toolong_arithmetic_test.cpp\` checks the complete signed arithmetic API, decimal parsing, INT64_MIN, negative divisors, 100! digit count, 2^100, the Pi squared constant, rational operations, explicit-width bit rotation, synthetic photon statistics and division/round-trip identities.
- The prototype was additionally compared locally with **Boost Multiprecision \`cpp_int\` for 700 independent randomized cases with 1–200-digit inputs**, covering \`+,-,*,/,%,<\`. This independent oracle requires Boost and is *not* a runtime or packaged library dependency.
- Local \`g++ 14 -std=c++20 -O2\` compiles and AddressSanitizer/UBSan checks passed for the prototype. Remote GitHub Actions CI compiles new repo code with C++20 \`-O2\` and executes the repo tests. Inspect the workflow run before concluding it passed.
- The \`toolong\` test implementation uses schoolbook multiplication and base-10^9 long division. Complexity and allocation costs can be substantial on very large operands; no cryptographic constant-time guarantee is claimed.
- Preserve exact inputs, explicit operation choice, original source hashes, observed outputs and failures. Do not silently normalize different user input strings into one source identity.

**Provenance:** original integer text → \`toolong\` parsed integer → exact operation → \`toolong\` or \`toolong_ratio\` result → independent witness → optional display. Source identity and precision policies remain independent.
