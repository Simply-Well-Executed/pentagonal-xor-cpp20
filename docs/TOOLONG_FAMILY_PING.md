<!--
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

Scope: original first-party contributions only; existing third-party rights, source media exclusions and other reservations remain authoritative.
-->

# TOOLONG FAMILY | TALON-separated PING contract

Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Status: **EXPLORATORY / RELEASE=HOLD**. This document is an integration contract, **not** evidence that tests ran. First-party SPDX: 0BSD. Neither source absence nor CI/tool failure authorizes deletion, renaming, or a PASS.

## Canonical source lanes (no aliases)

| Lane | Source / qualification | Distinct witness | Current status |
| --- | --- | --- | --- |
| TOOLONG | `shared/toolong.hpp`; `tests/toolong_arithmetic_test.cpp` | Exact decimal round-trip and arithmetic, including integer exponentiation | SOURCE LOCATED; RUN NOT VERIFIED |
| SLIDINGTOOLONG | `shared/slidingtoolong.hpp` (PR #34 branch) | `swe::slidingtoolong::QUALIFIED()`; 8/16/24 bit `ROUNDTRIP` over the independently tagged 64-bit data words | SOURCE LOCATED; RUN NOT VERIFIED |
| SLIDINGTOOLONGA | `quaddi-ni/Invoke-SlidingToolongAQualification.ps1` referenced by `.github/workflows/quaddi-ni-oracle-cpp20.yml` | Requested nonnegative exponent; exact independent oracle; original mathematical input and independent output | REFERENCED; IMPLEMENTATION/RUN NOT VERIFIED |
| SLIPSLIDINGTOOLONG | `quaddi-ni/Invoke-SlipSlidingToolongQualification.ps1` referenced by the same workflow | GUARDMEMORYGUARD logical-width / MXRING qualification and independent records | REFERENCED; IMPLEMENTATION/RUN NOT VERIFIED |

**TOOLONG is not SLIDINGTOOLONG**: the former is signed, limb-based arbitrary-precision integer arithmetic; the latter is a separately named reversible 32-bit-lane transport/check contract around 64-bit words. Q62 and Q64 constants have separate provenance; SLIDINGTOOLONG's Q64 is a finite-precision representation, not an exact representation of the irrational constants.

## PING definition

Each lane returns a named `PASS | FAIL | HOLD` result, source path/ref, precise test identity, input provenance, observed witness, and any independent oracle. No lane is credited for another lane's tests.

1. **HEAD**: identify exact repo commit, source paths, original inputs and per-lane versions. Freeze expected witness and operator order for the comparison.
2. **TALON**: preserve all four lane identities and their separate source and output records. Do not interchange implementations, combine unlike evidence, relabel, or delete on failure.
3. **TRANSFORM**: run each lane's *existing* native qualification. `SLIDINGTOOLONG` must show `JOIN(UNSLIDE(SLIDE(PACK(word,tag),bits),bits)) == word` for each of 8/16/24 and `PING` at all stages, including corruption-rejection checks. `TOOLONG` must retain entire exact digits, not floating-point output. Other named lanes require their own accessible scripts and test outputs.
4. **TAIL**: validate recovered source, independent reference, unmodified canonical source, and complete audit records. A failed, absent, inaccessible, or unexecuted check is not a PASS.
5. **COMBINED PING**: `PASS` only when each required lane has independently passed; otherwise `FAIL` on demonstrated mismatch, or `HOLD` when evidence is missing. No publication or merge authorization follows from CI alone.

## Immutable distinctions

- Algorithm/source data > ledger > visualization. A render artifact must not mutate authoritative source.
- A reversible fixed-width `XDR/RDX` or 32-bit sliding transport does **not** provide arbitrary-precision exponentiation. Reuse `TOOLONG` or an independently qualified exact big-integer oracle for that task.
- The phrase `infinity^infinity^infinity` has to be assigned an explicit limit interpretation; it is not directly an integer input.
- Exponents, output digit counts, Q62/Q64 encodings and transformed words are different unit types. Name each unit in the ledger.
- Preserve source provenance, historical branches, failures, reserved AIRGAP, individual agency, and `RELEASE=HOLD`. No silent correction or invented tests.

## Required implementation follow-through

Wire native TEST → TALON → independently sourced WITNESS → PING → TAIL → AUDIT in the branch's established C++20 `/O2` pipeline; verify the actual scripts and runs before changing any status to PASS. This documentation does not change existing `shared/*.hpp` implementations, CI files or their existing results.
