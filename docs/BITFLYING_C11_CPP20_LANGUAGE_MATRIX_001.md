<!-- SPDX-License-Identifier: 0BSD -->
# BITFLYING C11 / C++20 compatibility and QuadDi compiler matrix

**ID:** BITFLYING-C11-CPP20-001. **Status:** EXPLORATORY. **Release:** HOLD. **Scope:** shared ABI and two distinct language modes of the same Microsoft `cl.exe` toolchain. The source of truth is the tested source file, pinned compiler invocation, and its observed compile/runtime output, not a guessed relationship between languages.

## Two standards, one installed compiler

Microsoft's `cl.exe` accepts both `/TC /std:c11 /O2` and `/TP /std:c++20 /O2`. The `/TC` and `/TP` switches control source language. Sharing a compiler executable is **not** proof that C11 and C++20 are the same language. This project preserves the real-world shared subset rather than forcing incompatible declarations into one parser.

[Shared C-compatible ABI](../shared/bitflying_c_abi_001.h) declares by-value `bf_word`, bounded `nbit` 0..64, `nop` identity, checked `link`, guarded `+`/`-`/`&`/complement/equality. The remaining 3072-bit C++20 register system remains distinct. **No whole-repository 100% C11 conversion has been made or authorized.**

## Native six-probe VALID / NOT VALID classification

[MSVC PowerShell qualification](../quaddi-ni/Invoke-BitflyingCInteropMatrix.ps1) compiles every probe with one resolved native `cl.exe` and saves diagnostics outside the checkout. A positive result requires successful compile, program exit 0 and expected witness (where applicable); a negative result requires nonzero compiler exit and a recognizable compiler error, never a successful runtime.

| Exact source | Forced language mode | Expected outcome |
|---|---|---|
| [Shared BITFLYING](../quaddi-ni/BitflyingCInteropTest.c) | C11 | VALID, execute |
| The same Shared BITFLYING source | C++20 | VALID, same runtime output |
| [C11 `_Generic`](../quaddi-ni/BitflyingC11OnlyProbe.c) | C11 | VALID, execute |
| The same C11 `_Generic` source | C++20 | NOT VALID, expected compile reject |
| [C++20 `template`/`consteval`](../quaddi-ni/BitflyingCpp20OnlyProbe.cpp) | C++20 | VALID, execute |
| The same C++20-only source | C11 | NOT VALID, expected compile reject |

A compiler extension or unexpected acceptance is a test discrepancy; it does **not** establish an unrecorded language merger. For C-only deployments, keep a fully C11-compatible module and expose explicit checked interfaces; for C++-only deployments, preserve the specialized C++20 computation engine and add C ABI wrappers only where requested and tested.

## QuadDi four-lane pipeline

The [four-lane workflow](../.github/workflows/quaddi-ni-oracle-cpp20.yml) combines, without merging source languages:

1. **C++20:** native MSVC `/std:c++20 /O2`, `GUARDMEMORYGUARD`, original 21 source-backed constants, 0/1/2/4/8/16 tiny-width and 32..512 widths, 1D→100D→1D 3072-bit test, 32/64-bit limb parity.
2. **Oracle Java:** latest release **27.0.1.1** checked at the time of this record, pinned by vendor/version and archive SHA-256; independently compute the existing Oracle parity/model witnesses. The Oracle binary's licensing remains Oracle's, not this repository's 0BSD.
3. **XHTML 1.0 Strict:** a **separate** scriptless report, produced and checked by [Invoke-QuadDiXhtml10StrictGate.ps1](../quaddi-ni/Invoke-QuadDiXhtml10StrictGate.ps1), with XML declaration, exact W3C Strict public/system DOCTYPE, XHTML 1 namespace, XML well-formedness, and a bounded XHTML element vocabulary. The existing XHTML5+SVG visual reports remain historically unchanged. The local gate **does not perform full external W3C DTD validation**; that verification remains explicitly **NOT PROVEN**, rather than a false PASS.
4. **C11:** the same Microsoft compiler forced to C11 mode, shared C ABI and language-specific positive/negative tests. No C11 claim for a C++20-only source unless independently ported and verified.

The workflow must finish all positive and expected-negative tests, and the C++20/Oracle source parity checks, before declaring overall success. It never auto-merges or upgrades `RELEASE=HOLD`. `nobit` represents zero numeric data bits; `nop` is an identity operation, not a register capacity. `BITFLYING` here is an experimental safe by-value linking protocol, not physical scheduler/RAM manipulation.

## Provenance and evidence gate

**Requested release authority:** no silent changes to the original 21 constants, operator definitions, original 10 widths, historical Q62 rounding, or 100D source-ledger data. Changes are additive and reviewable in draft PR #95.

**Qualification status at initial authoring:** the [new four-lane CI run](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/pull/95/checks) must be inspected before replacing PENDING with PASS. The XHTML bounded-profile validator is separate from a full official DTD validator. Any negative test outcome stays recorded as a NOT VALID compiler-mode result rather than erased. `RELEASE=HOLD`.
