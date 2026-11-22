# Native Euler number: Q64 conversion

Status: EXPLORATORY / HOLD. Current protocol EULER-Q64-v3. SPDX-License-Identifier: 0BSD.

## Authority and correction

Authoritative Pi comment (original, unchanged): https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/pull/3#issuecomment-6902016891

The original author character is U+213C (ℼ), not U+03C0 (π).
PR #3 uses a 64-bit integer seed multiplied in a 128-bit intermediate, then shifted right by **62**. The earlier Euler draft copied that Q62 shift, but the operator has now requested a **64-bit fractional scale**.

A number greater than one (e = 2.718...) cannot be encoded directly as an unsigned 64-bit Q64 fraction. Therefore encode **e/4** as a Q64 fraction and multiply by **4 * 10^17** before shifting 64 bits:

```cpp
const unsigned long long result = (unsigned long long)(
    ((__uint128_t)(unsigned long long)12535862302449814170ULL *
     400000000000000000ULL) >> 64);
// Output: 271828182845904523
```

Explicit decimal placement: 2.71828182845904523 (18 significant digits, truncated).

Both Q62 and Q64 expressions yield the same 18-digit integer because scaling the multiplier by four and raising the shift by two are algebraically equivalent. That mathematical equivalence does not erase the correction: **the active Euler implementation shifts by 64**, and the deprecated Q62 implementation is retained only in Git history.

## Source and validation

- `shared/euler_native.hpp` implements only the native 128-bit multiply/shift path with one static assertion.
- `tests/euler_native_test.cpp` checks the output in CTest.
- The seed 12535862302449814170 was independently checked using the Euler factorial series; it is an encoded rational approximation, not an infinitely precise e.
- No 32-bit limb decomposition, no 64-bit-limb fallback, no POSIX, Python, Node, network fetch, standard-library header, or runtime library is introduced in this implementation.
- `__uint128_t` is a compiler extension rather than ISO C++20. The header intentionally rejects unsupported compilers, including MSVC, instead of silently changing algorithms. The repository's multi-platform validation therefore remains on HOLD.
- This constant conversion does not itself fix IEEE binary64 underflow/overflow, division by zero, negative-base Euler roots, or sign loss in inverse-square-SELF.

## History and release

Prior factorial-series integer-interval work and Q62 draft remain recoverable from Git history, not asserted as the current Euler algorithm. PR #17's 16-digit floating display policy remains separate. The old Pi comment remains authoritative for its own expression and unchanged.

Current repository-wide GNU/POSIX and optimization-policy conflicts are unrelated release blockers. Do not merge/publish until requested compatibility and validation decisions are explicitly reviewed.

Chosen, never assumed.
