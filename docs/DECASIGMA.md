# DecaSigma: ten-step reversible algebra witness

**Status:** experimental MAT.RECYCLE construction, separate from Pentagonal XOR.
**License:** 0BSD, consistent with the containing repository.

## Why this is useful

DecaSigma is a **small audit example** for a program that needs to preserve an original
operand (CORE), retain an ordered trail of derived representations, and check whether
a specified inverse path returns to the same value. It illustrates *witness retention*,
domain guards, numerical error checks, and independent review of intermediate stages.

It does **not** make the five-feature fuzzy XOR invertible. Pentagonal XOR reduces five
features to one parity probability, which cannot in general recover the original
five input values. DecaSigma is not encryption, a hash, a security signature,
or an identity/provenance attestation.

## Operators and exact mathematics

For an input real or rational number x, define:

- R(x) = 1/x (RECIPROCAL; x != 0)
- S(x) = 1-x (SUBTRACT)
- T(x) = R(S(R(x))) = x/(x-1) (SPAN; x != 0 and x != 1 **for the original nested definition**)

The original nested T is undefined at zero, even though its reduced rational
expression x/(x-1) would admit an extension there. Transformations **must not**
silently expand their source domains.

Both R and S are involutions. So is T:

    T(T(x)) = x, for x != 0,1.

A four-operation closure, called QuadSigma here, is T o R o S o R = identity.
The ten named operations for DecaSigma are:

    R, S, R, T, R, S, R, T, R, R

This is two QuadSigma closures followed by two reciprocal operations. There
are 10 **named** steps; expanding each T into R,S,R gives 14 primitive steps.

| Stage | Operation | Exact expression |
|---|---|---|
| 0 | source CORE | x |
| 1 | R | 1/x |
| 2 | S | (x-1)/x |
| 3 | R | x/(x-1) |
| 4 | T | x |
| 5 | R | 1/x |
| 6 | S | (x-1)/x |
| 7 | R | x/(x-1) |
| 8 | T | x |
| 9 | R | 1/x |
| 10 | R | x |

Thus stage[0] == stage[4] == stage[8] == stage[10] **exactly in rational
algebra**, for x != 0,1. This is a theorem about this constructed transform,
not a theorem about a separate handwritten integral.

For x = -9, the exact stages are:

    -9, -1/9, 10/9, 9/10, -9,
    -1/9, 10/9, 9/10, -9, -1/9, -9

## Implementation and limits

The small include-free C++20 header `shared/deca_sigma.hpp` returns all 11
ordered stages and an admissibility flag. The input x must be **finite**.
The implementation rejects zero and one inputs and guards the intermediate
division denominators, including denominators that may become zero due to
floating-point rounding.

Binary floating point is **not exact rational arithmetic**. For some valid
mathematical inputs, finite-precision rounding can cause an intermediate
denominator to collapse or a return witness to differ. This is a numerical
limitation, not evidence that an algebraic theorem is false.

`witnesses_return(trace)` tests stages 4, 8 and 10 relative to CORE using a
1e-12 tolerance by default. The test caller also checks all values are
finite. The return comparison is a consistency diagnostic, not an
independent source of truth.

There is no dependency from DecaSigma into the Pentagonal XOR engine.

## Triℼ inspection routes

Three read-only inspection routes are implemented in [Triℼ](TRI_PI.md):
**forward** transition checks, **reverse** reconstruction, and **Z-fold**
cross-stage comparisons. `shared/tri_pi.hpp` inspects a frozen DecaSigma
trace and reports each result separately. No special names are assigned to
these operations. These are checks on one record, not independent evidence
that it was authentic.

## Validation

On PowerShell 7:

```powershell
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

CTest includes `deca_sigma_roundtrip` and the XOR parity tests. Build results
should be obtained from actual compilation/CI, not inferred from this document.
