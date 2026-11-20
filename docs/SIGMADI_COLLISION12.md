# SigmaDi COLLISION12: the exploratory 12th provenance coordinate

> **Document status: PARTIALLY REWRITTEN / VERIFY (2026-11-19).** Added a namespaced separation from the proposed SigmaDi butterfly review and a PowerShell Core strict-mode example. Complete source, CI and licensing verification remain DUE; this is not a declaration that the implementation changed. See [DOCUMENT_REVIEW_REGISTER.md](DOCUMENT_REVIEW_REGISTER.md) and [LICENSE_AND_MARKS.md](LICENSE_AND_MARKS.md).
>
> **Original contributions:** Copyright (c) 2026 Paul Allen Cheek · SPDX-License-Identifier: 0BSD. Third-party material and marks retain their respective rights; no registered mark is asserted.


**Status:** EXPLORATORY · `SIGMADI-COLLISION-12D-v1` · native C++20 · 0BSD

## Namespace separation: SigmaDi review butterfly versus COLLISION12

**Publication action:** Distinguish the existing `SIGMADI-COLLISION-12D-v1` implementation, whose 12 coordinates record declaration identity and provenance, from the separate experimental review overlay. The additional [SigmaDi review extension](WRITING_OPERATING_PROCEDURE.md#2a-sigmadi-review-extension-four-alternating-butterfly-connections) is a distinct **`SIGMADI-REVIEW-BUTTERFLY4-v0` EXPLORATORY** documentation-review overlay: four signed cross-checks of TriDi YIN/YANG/AIRGAP evidence, synthesized by YAN. It is not implemented by `shared/sigmadi_collision12.hpp`, cannot rename its 12 coordinates, and does not treat four links as four independent empirical observations.

**Authority boundary:** A review may consume a valid COLLISION12 receipt only as a declaration/provenance witness. It cannot use COLLISION12 to classify arbitrary reasoning claims, nor promote a prior BiDi/TriDi decision to VERIFIED by renaming it. The proposed butterfly endpoint and polarity conventions require explicit approval before they become established. The canonical C++20 guard and existing source tests remain unchanged.



**Source example:** during the creation of the
`SIGMA_PENTA_HAN_VISTA_BOARD.svg` illustration, one tool-side
JavaScript expression used `r` for a radius and later declared
`const r` again for the GitHub response **inside the same scope**.
That duplicate lexical declaration caused a JavaScript
`SyntaxError: Identifier 'r' has already been declared`. This was
a tool-side generator error, not a problem in the recorded C++ parity,
the source photographs, the earlier H11 measurement, or the OSI-6
renderer. The generator was subsequently corrected with different
variable names. **Failure is evidence, not an authorization to mutate
a source record.**

The user has named the exploration **the 12th dimension**. Here
dimension is a **metadata axis for identity and provenance**,
not a newly discovered physical/spacetime dimension or a mathematical
proof of 12D geometry. The twelfth axis distinguishes two declaration
instances which otherwise resolve to the same source-language
symbol binding.

## Exactly 12 coordinates, zero based

| Index | Axis | Why it matters |
|---|---|---|
| D0 | repository | Isolate independent repository custody |
| D1 | branch | Avoid confusing exploratory branch and CURRENT |
| D2 | file | Do not join unrelated source files |
| D3 | namespace | No naked references across namespaces |
| D4 | lexical scope | Distinguish legal reuse inside another scope |
| D5 | exact symbol spelling | Match exact UTF-8 bytes, not a look-alike |
| D6 | declared type | Preserve type attribution |
| D7 | semantic role | Retain intent and meaning |
| D8 | source provenance | Record origin of a declaration |
| D9 | route | Record where the declaration was used |
| D10 | revision | MOVE/RETIRE never silently rewrites version history |
| **D11** | **declaration instance** | **Distinguish independently declared bindings** |

**Example:**

```text
D4 scope = build_board
D5 symbol = "r"
D11 instance = "declaration:0"    // radius
D11 instance = "declaration:1"    // later GitHub response
↓
DUPLICATE_BINDING
↓
No implicit name-merging or substitution.
```

A different D4 scope permits ordinary scoped reuse. A changed D10
revision is a different historical witness, not an automatic correction
of the previous revision. If the same D11 declaration identity claims
incompatible type, semantic role, provenance or route, the
classification is `INCONSISTENT_DECLARATION`.

Two visually similar symbols are **not** merged merely because
they look alike. The audit compares exact bytes; Unicode NFC and
confusables are *not* applied or inferred. For example, Arabic
`آستر` is not the ASCII string `Aster`. Any optional Unicode
normalization or confusable screening needs its own frozen protocol,
source span coordinates, and separate proof of identity. Exact byte
comparison is sufficient for this declared version's witness logic,
not a universal language parser.

## Bounded, deterministic C++20 API

`shared/sigmadi_collision12.hpp` defines:
- `Coordinate` containing exactly 12 borrowed `std::string_view` axes.
- `compare(a,b)`: `DISTINCT`, `SAME_DECLARATION`,
  `SCOPED_REUSE`, `DUPLICATE_BINDING`,
  `INCONSISTENT_DECLARATION`, or `CHANGED_REVISION`.
- `Ledger<Capacity>`: fixed-capacity ledger with `inspect`
  (read only) and `append` (explicit admission).
- `Receipt`: a classification, matched 0-based index, number
  of retained entries, and explicit `retained` flag.

The guard blocks exact-symbol duplicate declarations in the same
repository/branch/file/namespace/scope/revision when D11 differs;
it does not execute a JavaScript, PowerShell or C++ parser. The
caller supplies syntax-tree-derived scopes and declaration IDs.
It rejects empty axes and capacity overruns. A blocked collision
leaves the ledger unmodified and returns a receipt, rather than
silently deleting one of the candidates.

`std::string_view` **borrows** its bytes. The caller owns the source
strings and must keep them alive while a ledger refers to them.
For durable storage, persist byte-for-byte source and declaration
records separately. A `Receipt` is not a cryptographic signature,
a replacement for source control, or an independently attested witness.

### Test contract

`tests/sigmadi_collision12_test.cpp` covers:

- Duplicate `r` declaration: caught as `DUPLICATE_BINDING`.
- Original record count unchanged on blocked collision.
- Repeated identical declaration: recorded once, classified.
- Same spelling inside a new scope: `SCOPED_REUSE`.
- Changed revision: `CHANGED_REVISION` retained separately.
- One declaration instance with contradictory semantic role:
  `INCONSISTENT_DECLARATION`.
- Case sensitivity and distinct Unicode bytes: no false coalescing.
- Capacity and invalid-input failures: no mutation.
- Twelve exact coordinates; D11 is coordinate index 11.

### Connection to the nine-eye / HANDecaSigma presentation

This new axis is a **presentation-provenance guard** after the
canonical calculation: source → derived operators → VISTA/COR →
MINZA/NINZA/PING → 12-coordinate declaration witnesses →
OSI-6 visual/audit presentation. It is **not** a new physical H12
derived from the temporal chirality variable H11. HANDecaSigma[11]
retains its original method and null S11 where unsupported.

Existing nine-eye trace slots, ten provisional language labels and
108 virtual VietQuang visual lenses remain independent display
organizations. None manufactures an extra measured camera,
new entropy, additional ligand, authenticated source, or personal
identity.

**Authority:** source records and their original scopes remain primary.
A display grouping cannot mutate the identity coordinates. No
automatic renaming, editing, replacement, deletion or merge occurs.

## Reproducibility

**PowerShell Core (latest stable version), with `Set-StrictMode -Version 2.0` mandatory.** This is an **EXPLORATORY, NON-POLICY BUILD** only: it tests the existing CMake configuration, which has not been migrated to the C++20 and `-O2`/MSVC `/O2`-only normative profile. A successful test does not demonstrate optimizer-policy compliance. Retain prior CI records with their exact flags.

```powershell
Set-StrictMode -Version 2.0
$ErrorActionPreference = 'Stop'
$PSVersionTable.PSVersion
cmake -S . -B build -DBUILD_TESTING=ON -DCMAKE_CXX_STANDARD=20 -DCMAKE_CXX_STANDARD_REQUIRED=ON -DCMAKE_CXX_EXTENSIONS=OFF
if ($LASTEXITCODE -ne 0) { throw 'CMake configuration failed' }
cmake --build build --config Release
if ($LASTEXITCODE -ne 0) { throw 'CMake build failed' }
ctest --test-dir build -C Release --output-on-failure -R collision12
if ($LASTEXITCODE -ne 0) { throw 'Collision12 CTest failed' }
```

**Policy boundary:** The existing CMake Release preset can select `-O3` on Unix toolchains, and MSVC Release includes additional interprocedural optimization in some targets. Neither the command above nor `--config Release` enforces `-O2`/`/O2` universally. Inspect exact compile and link commands, and treat this example as **NON-POLICY / EXPLORATORY** until a separately reviewed toolchain change enforces the prescribed profile.

Until this repository's CI completes on the commit containing this
file, the source is implemented but cross-platform test results
remain **unverified**.
