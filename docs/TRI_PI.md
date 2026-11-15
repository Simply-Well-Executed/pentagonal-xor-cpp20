# Triℼ: three inspection routes for a DecaSigma trace

**Status:** experimental, source-preserving MAT.RECYCLE audit. Not a claim about
historical mathematics, built-in calculator keys, cryptography, or identity.

## Contract

The three routes are identified by their operations, without assigned special names:

| Route | Stage visit order | Inspection |
|---|---|---|
| Forward | 0,1,2,3,4,5,6,7,8,9,10 | Recompute each transition |
| Reverse | 10,9,8,7,6,5,4,3,2,1,0 | Recover the preceding state using self-inverse R, S, or T |
| Z-fold | 0,1,3,2,4,5,7,6,8,9,10 | Compare equal stages across folds and the final reciprocal hinge |

The visit order is an **audit/readout order**. It does not reorder source
operations, change algebraic precedence, alter the numerical trace, or make
distinct mathematical functions interchangeable.

## Underlying source

DecaSigma preserves a unchanged, ordered record with stages `s[0]..s[10]`:

    R  S  R  T  R  S  R  T  R  R

where `R(x)=1/x`, `S(x)=1-x` and
`T(x)=R(S(R(x)))=x/(x-1)`, with the original domain `x!=0,1`.
An admissible exact-rational trace has
`s[0]=s[4]=s[8]=s[10]`. Its corresponding repeated states also
satisfy `s[1]=s[5]=s[9]`, `s[2]=s[6]`, and `s[3]=s[7]`.

The read-only public implementation is `shared/tri_pi.hpp`, which uses
the existing include-free `shared/deca_sigma.hpp`. It returns a report
with three separate Boolean findings and a conjunction `passed()`.

- **Forward** checks each adjacent transition with its prescribed operator
  and the source's domain guards.
- **Reverse** walks backward and checks that the inverse operation returns the
  previously recorded stage. R, S and T are each their own inverse on the
  specified domains.
- **Z-fold** visits each stage exactly once in Z-fold order, then
  compares the repeated cross-fold witnesses including return-to-CORE.

A report passes **only if all three checks pass**. The audit rejects
inadmissible traces, nonfinite stored stages, invalid tolerances, and broken
reconstructions. It does not change the original trace.

## Example

For `x=-9`, the exact sequence is:

    -9, -1/9, 10/9, 9/10, -9,
    -1/9, 10/9, 9/10, -9, -1/9, -9

This satisfies all three audit routes. `tests/tri_pi_test.cpp` also
mutates intermediate records and asserts that the combination fails,
including a **correlated tampering** fixture that leaves repeated-stage
comparisons consistent but breaks forward/backward arithmetic.

The three routes are complementary validation algorithms over the **same**
stored values, not three independent observations or independent proofs of
origin. Passing checks cannot authenticate a source record, guarantee
bit-for-bit floating-point reversal, or make Pentagonal XOR invertible.
The 1e-12 witness tolerance is a numerical consistency threshold, not
cryptographic strength.

## Unified COREAO scalar

[COREAO](COREAO.md) composes the three routes:
Triℼ I computes the forward trace from source CORE, Triℼ II
reconstructs the reverse path from Triℼ I's final state, and
Triℼ III compares those paths with the unchanged record in Z-fold order.
Rather than treating three Boolean reports as independent evidence,
COREAO returns one normalized **maximum discrepancy scalar** with a
separate admissibility flag. This scalar is an audit diagnostic, not a
proof of authentic provenance. The ten-step operation sequence is
defined once in `shared/deca_sigma.hpp`.

## Compile and test (PowerShell 7)

```powershell
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

The executable `pentagonal_xor` retains its **one fast production path**.
Triℼ adds no runtime flag, optional parity mode, or runtime minterm walk.
