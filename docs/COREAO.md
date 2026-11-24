# COREAO: Triπ I → Triπ II → Triπ III → one scalar

> **Document status: DUE FOR UPDATE (2026-11-19).** Verify claims against code and tests; sharpen active verbs; check copyright, project-mark language, accessibility, and 0BSD scope. This mark is not a failure verdict. See [DOCUMENT_REVIEW_REGISTER.md](DOCUMENT_REVIEW_REGISTER.md) and [LICENSE_AND_MARKS.md](LICENSE_AND_MARKS.md).
>
> **Original contributions:** Copyright (c) 2026 Paul Allen Cheek · SPDX-License-Identifier: 0BSD. Third-party material and marks retain their respective rights; no registered mark is asserted.


**Status:** experimental, source-preserving MAT.RECYCLE algebra audit.
**License:** 0BSD. This feature is independent of the Pentagonal XOR CLI.

The composition makes the user's COREAO concept operational while keeping
each step's provenance inspectable:

    COREAO = Z-fold[Reverse[Forward[CORE]]]

For this implementation, **CORE** is the supplied, unchanged eleven-stage
DecaSigma trace `s[0..10]`, with source operand `s[0]`.
The calculations never modify the original trace.

## The three linked inspections

1. **Triπ I, forward:** derive fresh stages `F[0..10]` from the original
   source operand using the DecaSigma operations
   `R,S,R,T,R,S,R,T,R,R`. The given trace is *not* trusted as its own proof.
2. **Triπ II, reverse:** fill `B[10]` from `F[10]`, then reconstruct all
   earlier `B[i-1]` by applying each operation in reverse order.
   `R(x)=1/x`, `S(x)=1-x`, and `T(x)=x/(x-1)` are involutions on their
   stated real domains.
3. **Triπ III, Z-fold:** compare each supplied `s[i]` against both
   `F[i]` and `B[i]` in visit order
   `0,1,3,2,4,5,7,6,8,9,10`, then check the seven cross-fold pairs
   `(0,4),(4,8),(8,10),(1,5),(5,9),(2,6),(3,7)`.

The labels I/II/III distinguish inspection order, not special calculator
functions, interchangeable operators, or independent evidence providers.

## The unified scalar

Define a normalized difference for finite double values:

    scale(a,b) = max(1, |a|, |b|)
    rho(a,b) = |a / scale(a,b) - b / scale(a,b)|

For the three ordered routes, COREAO returns one scalar:

    delta = max(
      rho(B[i], F[i])      over i=0..10,
      rho(s[i], F[i])      over i=0..10,
      rho(s[i], B[i])      over i=0..10,
      rho(s[p], s[q])      over the seven fold pairs (p,q)
    )

Here `delta >= 0`, with **smaller meaning better agreement**. The
implementation scales inputs *before* subtraction to avoid an overflow when
subtracting two large finite values. There is no summation that could
conceal a single bad step by cancellation or averaging.

The result includes `admissible` as a separate domain flag.
`passed()` requires an admissible trace and `delta <= 1e-12` by
default. A zero-valued scalar when `admissible=false` is *not* success;
the scalar is not meaningful on an invalid route.

A valid exact rational trace has delta=0. For binary floating point,
small nonzero errors can remain even when the exact identity holds.

## Efficiency and scientific limits

- **Fixed-size:** 11 states and 10 named operations; bounded constant
  runtime and bounded stack, no allocation, no standard-library headers.
- **Read-only:** input CORE, source trace, and stored source coefficients
  are never overwritten.
- **Finite guards:** zero, one, nonfinite values, and intermediary invalid
  arithmetic are rejected, rather than manufactured into successful zeros.
- **No XOR runtime overhead:** COREAO is a separate reference header and
  test. The production five-input Pentagonal XOR stays one closed-form path.
- **No independent provenance:** forward and reverse are linked calculations
  from one source; a coherently forged new source can still pass the audit.
  Delta only measures internal consistency, not security, identity or truth.
- **No inverse of Pentagonal XOR:** reducing five feature probabilities to
  one parity probability still loses information.

## Example

Input `x=-9` produces the exact-rational DecaSigma stages:

    -9, -1/9, 10/9, 9/10, -9,
    -1/9, 10/9, 9/10, -9, -1/9, -9

Forward derives these; reverse reconstructs the earlier stages; Z-fold
compares both with the original record. The scalar is approximately zero
under IEEE-754 binary64 arithmetic and passes the 1e-12 default tolerance.

Altering a stage or making correlated edits to stages 1/5/9 raises delta;
the CTest fixture checks that these changes **fail** the combined audit.

## PowerShell 7 validation

```powershell
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

The test is named `coreao_unified_scalar`. No additional CLI mode is
introduced.
