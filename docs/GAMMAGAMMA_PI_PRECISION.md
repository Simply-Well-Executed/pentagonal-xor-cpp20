<!--
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
-->

# COREAO π operators, tGamma, GammaGamma, and display precision

> **EXPLORATORY** · `COREAO-GAMMAGAMMA-PI-PRECISION-v1` · C++20 · 0BSD · Copyright (c) 2026 Paul Allen Cheek

This addition preserves the original DecaSigma, Triπ, and COREAO algorithms and their historical tests. It introduces a **separate** integer-component GammaGamma operator and project output-formatting contract. These should not be silently merged with existing mathematics or treated as nuclear-physics models.

## Source notation and definedness

The owner's π has the **exact integer digits** `314159265358979323` (18 digits). Its ordinary decimal formatting requires the source author to choose a decimal point. This implementation never guesses the location.

| Label | Owner's expression | Implemented meaning | Outcome |
| --- | --- | --- | --- |
| Triπ | implements π | Existing three-route forward/reverse/Z-fold DecaSigma inspector | Existing `shared/tri_pi.hpp`, unmodified |
| BiPi | `π^2` | Mathematical integer exponentiation | Exact `98696044010893585656621344341538329` |
| PiPi | `π^π` | Exact symbolic expression, not representable in native fixed-width integers | Symbolic only |
| PiDi | `π % 4 = I` | Euclidean integer modulo four | **3**; the proposed label `I` needs a user-defined mapping before asserting equality |
| SigmaPi | `!π` | New **prefix GammaGamma** on an explicitly decimal-pointed input string | Exact when factorial components are bounded; symbolic otherwise |

**Critical C++20 distinctions:** `^` denotes XOR in C++, not exponentiation. `!` denotes logical NOT in C++ for built-in integers, but the new overload `!swe::gammagamma::DecimalToken{...}` is deliberately a new operation on the tagged decimal string. Ordinary `!integer` retains its normal C++ meaning. The special notation does not retroactively change built-in operators.

## Postfix factorial and prefix GammaGamma are distinct

- **tGamma**, standard mathematics: `x! = std::tgamma(x+1)`. For `3.14!`, this yields a finite approximate floating-point value. It does **not** model radiation decay without a separate physical model.
- **GammaGamma**, newly specified prefix operator: `!3.14 = abs(3! - 14!) = 87178291194`; the comparison direction is **NEGATIVE** and is saved in a **separate field**. The magnitude is nonnegative.
- Input is a string containing **one period and decimal digits on each side**. Keep `3.014` distinct from `3.14` in the source ledger even when both parse to identical component integers. No floating-point parsing or normalization changes the original token.
- Integer factorials up to **2048!** can be calculated exactly by the bundled base-1,000,000,000 arbitrary-precision implementation. Larger component arguments receive `symbolic_only` with their original integers and direction retained; **no guessed numeric result**.
- Inputs such as `!3.14159265358979323` contain the correct concatenated π digits and an explicit user-chosen decimal point, but the factorial of 14159265358979323 is beyond the declared resource bound. The result is symbolic, not a failed identity.
- The words *gain* and *dispersion* are exploratory interpretations. Neither this operator nor `std::tgamma` by itself establishes quantum interactions or physical radiation dynamics.

## π-derived presentation precision

Per the owner's output-conformance instruction:

`n = digit_count(π) − 2 = 18 − 2 = 16`

Use **`std::setprecision(swe::pi_precision::display_digits)`** for new iostream floating-point output. The constant is defined once in `shared/pi_precision.hpp`. The author identifies the two reserved positions as one whole-number and one decimal-place position.

- With defaultfloat, `setprecision(16)` sets **16 significant decimal digits**.
- With `std::fixed`, `setprecision(16)` sets **16 digits after the decimal point**.
- With `std::scientific`, `setprecision(16)` likewise controls digits after the decimal point.
- The setting **does not increase `double`'s internal precision**, change rounding of preceding calculations, or guarantee round-trip of every binary64 value (which generally needs `max_digits10=17`).
- Exact decimal BigUnsigned factorial results, exact input digit strings, hashes, binary source records and algorithmic constants must **never be rounded or truncated to 16 digits**. This is the explicit exception required to avoid data corruption.
- Historical immutable code and old output fixtures remain as they were. New, source-preserving adapters use the shared convention; this document does not falsely claim every legacy file was retroactively rewritten.

## PR #32: exploratory SigmaPi-for-pi intensity substitution

The user's exploratory circle-area formulation, with `I` as a standalone
symbol, is `(P × e^(−αω)) / (Σπω²) = I`. Here `Σπ` means this document's
prefix GammaGamma `!π`, not ordinary mathematical π and not the separate
`handi::sigmapi` review tool. The canonical 18-digit π source remains
`314159265358979323`, and the exact decimal token `3.14159265358979323`
remains unmodified.

`gammagamma::sigma_pi("3.14159265358979323")` yields `symbolic_only`,
because the right factorial argument exceeds the explicit resource bound.
There is therefore **no numeric value for Σπ in this source**, and no
justified numeric intensity `I`, inverse, division, or physical stability
claim. `shared/euler_pi_inverse_square.hpp` records the candidate as a
`static constexpr` **symbolic** `I` with
`numerically_defined=false` and compile-time assertions. It does not
replace or mutate the original spherical inverse-square equation; that
equation has the separate `4π` geometrical denominator.

An actual fixed numerical `I` requires an independently justified positive
finite `Σπ` value and explicit `P`, attenuation and radial-distance inputs.
No alternate value is guessed or silently substituted.

## Validation

`tests/gammagamma_pi_precision_test.cpp` checks 0!, 1!, factorial subtraction direction, magnitude preservation, literal decimal-point provenance, symbolic overflow handling, PiDi remainder, the exact BiPi square, ordinary and fixed-format output, and source-independent compile-time constants.

`tools/gammagamma_pi_precision_demo.cpp` demonstrates both operations and the numerical and symbolic routes.

Compile with **C++20 `-O2`** on GCC/Clang. The CI workflow compiles the test and demo using `g++-14 -std=c++20 -O2` and runs both. The workflow result must be separately observed; a committed workflow file alone is not an executed validation.

**Custody:** Source digits → explicit decimal placement → parsed integer components → exactly computed factorials or symbolic state → direction + magnitude → optional `std::setprecision(16)` presentation for floating data → evidence. No media/codec tracks are inferred by this arithmetic.

See [COREAO](COREAO.md), [Triπ](TRI_PI.md), and [MINZA audiovisual audit](MINZA_DUAL_RENDITION_MP4_AUDIT_2026-11-19.md).
