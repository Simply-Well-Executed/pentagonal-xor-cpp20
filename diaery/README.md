# DIAERY | the fold that remembers

**Proof of concept. 0BSD source. RELEASE=HOLD.**

DIAERY is a very small **native C++20 offline XHTML 1.0 Strict-profile
viewer**, not a fork of Chromium or an implementation of the entire W3C
XHTML DTD, CSS, DOM or ECMAScript standards. The name is an imaginative
portmanteau of *diary* and *aery* (geometry unfolding into a space).

## What it does

- Accepts only a deliberately narrow subset: exact UTF-8 XML declaration,
  exact XHTML 1.0 Strict public/system DTD declaration, exact XHTML namespace,
  a head with one title, and a body with unadorned `h1`, `h2`, and `p`
  tags containing printable ASCII text. All other markup fails closed.
- No networking, remote assets, script execution, dynamic packages, Node,
  Python, STL or C++ standard/system includes. Source references to existing
  project geometry headers are intentional. Windows native GDI and User32
  functions are declared locally without including SDK headers.
- Draws an interactive native Win32 inspection window; use Left/Right for
  six geometric stages (flat, folds 1–3, caps, visual seam fusion), mouse
  drag to rotate, +/- to zoom, W to change the water *visualization*, Y to
  select the YANG face filter, and C for cutaway, and B to toggle the enclosing bubble motif.
- The geometry and its exact witness values come from the existing
  `shared/` C++20 source, not from XHTML-generated arbitrary geometry.
  The source XHTML only describes states and exposes values to test parity.
- Produces **1:1-PARITY-GATE** via full integer invariants for both
  the 12-face capped model and the five cross-section points at **all four**
  flat/folded stages. A single mismatch denies rendering.
- Negative fixtures reject non-Strict doctypes, scripts, and tampered
  volume numbers. The profile is narrower than the full Strict DTD by design;
  success means *supported strict subset + geometry concurrence*, not formal
  DTD compliance certification.

## Folder map

- `diaery.cpp` offline native viewer / CLI (only local file access)
- `model_bridge.hpp` canonical source inclusions / static proofs
- `strict_profile.hpp` fail-closed XHTML lexical/structural scanner
- `parity_gate.hpp` exact stage and scalar concurrence gate
- `index.xhtml` interactive window's document, also browser-readable XHTML
- `bad_*.xhtml` controlled negative fixtures
- `Invoke-ParityGate.ps1` native Windows compiler, HEAD/TAIL audit
- `CMakeLists.txt` native MSVC target and tests
- `.github/workflows/diaery-parity.yml` exact-SHA bootstrap + offline test

## Build and use

On a Windows x64 machine with native MSVC installed, in the checkout root:

```powershell
pwsh -NoLogo -NoProfile -NonInteractive -File .\diaery\Invoke-ParityGate.ps1 -RepositoryRoot . -EvidenceDirectory $env:TEMP\diaery-evidence
& "$env:TEMP\diaery-evidence\diaery-native.exe" .\diaery\index.xhtml
```

The C++20 compile uses `/std:c++20 /O2 /W4 /permissive-` and links
Windows built-in `user32`, `gdi32`, and `kernel32` libraries.
The build script uses the installed VS Developer Shell and explicitly
rejects missing toolchains. The workflow downloads its exact source
archive from GitHub **once at bootstrap** because a hosted runner starts
without the repository: the actual build and tests make **zero network
requests**. This provenance bootstrap is not an air-gapped workflow.

## Exact geometry oracle

Original three-fold chain: a 60x60 square, four 60x15 strips, three
successive 90-degree bends at flat y=15,30,45. The capped rectangloid
has length 60, width 15 and depth 15. Its mathematical boundary has
8 wall triangles and 4 cap triangles, with 12 facets total, exact
volume 13500 cubic model units, and exterior area 4050 square units.
Its original source cap material receipt is preserved: each 60x60
copy contributes one 15x15 cap plus 3375 offcut area units. The
C++20 native model computes, rather than copies, the witnesses.

The HTML document contains an explicit `PARITY-1` declaration plus
`STAGE-0` through `STAGE-3` records. The C++20 gate reconstructs
these strings from source geometry, checks all of them for exact
agreement, and rejects missing, stale, duplicate or mutated records.

## Engineering integrity and limitations

**Concurrence is necessary but not sufficient for safety.** No full
XHTML DTD processing, entity expansion, Unicode text layout, XHTML
forms, CSS engine, script engine, GPU Z-buffer, water physics, physical
seam fusion, pressure testing, water mass determination, or
certification of a real vessel is implemented.

The YANG back-face view is an illustrative native graphics feature,
not a substitute for a complete 3D occlusion pipeline. Both real
physical properties and release authorization remain explicitly
unverified. The former 9-triangle partition and symbolic BiDi,
TriDi, PiDi, PiSigma, SigmaPi and SigmaSigmaSigma source experiments
remain unmodified and cross-referenced. **Chosen, never assumed.**

## FUSION: qualified XHTML to permanent headerless C++20

`diaery-native.exe --qualify diaery/index.xhtml` runs the independent
seven-gate qualification check (BiDi, TriDi, PiDi, PiPi, SigmaDi,
SigmaSigma, GammaGamma) on the **existing** source geometry and XHTML
parity manifest. `--fusion diaery/index.xhtml output.cpp` creates a
deterministic **new** C++20 source file. Existing output is never overwritten.

The generated file uses no `#include`, STL or third-party runtime. It
embeds the exact original XHTML bytes **as data**, geometry values from
the canonical headers, four 3D fold-stage coordinate records, and
compile-time assertions. Compiling the generated source produces an
executable that emits the source XHTML bytes unchanged. This is a
lossless snapshot/qualification adapter, not a compiler for arbitrary
XHTML syntax or a second browser engine.

**FIFO == LAFIT** has a concrete contract: the input XHTML bytes must
equal the recovered bytes emitted by the generated native executable;
the recovered XHTML must requalify in DIAERY; and regenerating C++20
from those recovered bytes must produce a byte-for-byte identical
source file. Source order and the authority of earlier geometry stay
intact. The produced C++20 file and the original XHTML remain
separately retained, with SHA-256 evidence.

The new `diaery/Invoke-FusionQualification.ps1` compiles the new source
using native MSVC `/std:c++20 /O2`, performs binary-safe recovery,
checks both round-trip hashes, checks regeneration identity,
denies corrupted XHTML and unsupported input, and prints
`QUALIFICATION=PASS` only if all gates succeed. It does not change or
bypass INCEPTION-GATE. The exact source archive bootstrap remains
networked; compilation and tests after bootstrap are offline.

**Operator boundaries:** PiDi uses the project's symbolic finite
integer modulo-four witness; PiPi remains `SYMBOLIC_ONLY` and
does not evaluate pi-to-the-pi. SigmaDi implements the explicitly
labeled four-branch butterfly as a separate reversible adapter.
GammaGamma tests bounded integer-factorial differences only; it is
not asserted equivalent to the full STL-based GammaGamma library.
All seven names preserve their original source meanings where
established and are labeled EXPERIMENTAL where incomplete.
No safety or full XHTML 1.0 DTD qualification is inferred.
**RELEASE=HOLD.**

## Three XHTML variables and a final watchdog (PR34-016)

DIAERY's supported XHTML 1.0 Strict profile expresses these records as
**ordinary paragraphs**, without scripts, custom elements, event
handlers or nonstandard XHTML attributes:

```xml
<p>VAR-AD|squared=67600</p>
<p>VAR-CD|squared=152100</p>
<p>VAR-AC|squared=219700</p>
<p>WATCHDOG|rule=AD+CD=AC|squared=219700</p>
```

The values come from the earlier fixed-hinge three-edge geometry,
in which `AD_FOLDED+CD_FOLDED=AC_FOLDED`. A new local header
`fusion_watchdog.hpp` checks all **four contiguous, ordered,
unique** records independently against the canonical geometric
values. The fourth is calculated from the preceding three and
does not pretend to be a secret, cryptographic signature or a
fourth independent measurement. Missing, duplicated, swapped,
or tampered inputs fail the Fusion QUALIFICATION gate.

The generated C++20 snapshot includes corresponding three
`constexpr` variables, an independently calculated
`WATCHDOG=VAR_AD+VAR_CD`, and an exact static assertion.
FIFO/LAFIT recovery preserves the original XHTML bytes,
including these four paragraphs. The ordinary 1:1 parity gate
remains a separate prerequisite, rather than silently becoming
a watchdog's definition.

A passing watchdog does **not** establish formal XHTML DTD
qualification, real fluid pressure safety, physical waterproof
seams or release authorization. **RELEASE=HOLD.**
