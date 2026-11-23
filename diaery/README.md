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

## HARMONY: patience in humanism, tolerance in engineering (PR34-018)

**EXPLORATORY**. The user-defined `EULER'S JIG` constant is the
per-triangle area of the displayed 64-triangle total:
`1833.278772184302 / 64 = 28.64498081537971875` square units.
It is stored exactly as `2864498081537971875ULL` at a `10^17`
decimal scale. It is **not** the conventional dimensionless Euler
number `e`, separately recorded as `2718281828459045235ULL`
at a `10^18` scale.

A separate `shared/slidingtoolong.hpp` keeps conventional e and
pi's **64-bit fractional** Q64 words and distinct integer parts,
without changing historical `shared/toolong.hpp` or Q62 records.
The intensity fractional word is the historical Q62 word shifted
left twice; these two extra zero digits are NOT newfound precision.
A 3+1 32-bit register unit holds low, high, tag plus watchdog,
tests round trips across 8/16/24-bit reversible rotates, and
rejects mutated registers. A data-register watchdog is **not** NVRAM.
A fixed XOR diagnostic named `SIGMA_Q64_XOR` does not redefine
mathematical sigma or claim physical meaning. All slides occur in
compile-time proofs, not per-frame native rendering.

**64 x 64 = 4096** is an integer witness. **sqrt(360)** is
irrational, so `HARMONY_RADICAL_360_MICRO=18973665` is the
floor of sqrt(360) scaled by 1,000,000. It has a proved numerical
error bound less than 1 micro-unit, not a license to alter exact
topology. 64 wedges of 1 degree cover only 64 degrees, leaving
**296 degrees** missing from a full 360-degree circle.

The triangle-vs-square fold comparison is separate and deliberately
physical-geometry-oriented. Three 60 x 20 strips folded at TWO
120-degree hinges form an ideal equilateral triangular tube of
edge 20 with exact perimeter 60. Two copies of 60 x 60 stock
can each supply one equilateral side-20 triangular end cap, area
`100*sqrt(3)` square units. No geometric material-length
airgap is needed for that triangular tube, though real seals
and manufacturing remain untested.

By contrast, Euler's Jig's 1-degree triangle with minimum
height 1 has base greater than 57 and hypotenuse greater
than the base. Its perimeter is greater than 115, exceeding
the 60 available wall strip: **more sidewall material is needed**.
Each thin triangular cap can still fit a separate 60 x 60
stock sheet. Cap fitness and tube wall sufficiency must
not be treated as interchangeable tests.

`diaery/harmony.hpp` derives these facts without system
or standard-library includes, with separate EXACT and
TOLERANCE checks. The three `HARMONY-*` XHTML Strict
paragraphs are treated as **ordered, unique qualification
records** in C++20, including missing, mutated and re-ordered
negative fixtures. The offline native MSVC /O2 Fusion
concurrence workflow checks both source and generated C++20.
These are exploratory integer/geometry invariants,
not quantum mechanics, physical safety certification,
general XHTML DTD certification, or a merge approval.

**RELEASE=HOLD. Chosen, never assumed.**

## NASCAR FINITE HEAD-TO-HEAD | SlidingToolong (PR34-019)

This test replaces the premature notion of an "infinity winner" with
**64 exact, finite arithmetic rounds**. It preserves two distinct values
without renaming either:

- User Euler's Jig area = 28.64498081537971875 units squared,
  held as 2864498081537971875 / 10^17.
- Mathematical e (finite source decimal) =
  2.718281828459045235, held as 2718281828459045235 / 10^18.

Both are recast over a common denominator 10^18 **exactly**:
J = 28644980815379718750 / 10^18
and E = 2718281828459045235 / 10^18.
The finite function is F(n) = n * X^n for n=1..64.
The inverse G(n)=1/F(n) is represented **as a reciprocal fraction**
without rounding; F(n)*G(n)=1 by rational construction.

`finite_head_to_head.hpp` provides capacity-checked, base-10^9
limb multiplication and comparison, a repeated modular witness
over modulus 1,000,000,007, bidirectional exact-rational custody,
and explicit single-u64 overflow thresholds. The 64-step path is
executed only by `head_to_head.cpp` during native MSVC /O2
qualification, not on each graphical frame. A separate PowerShell
BigInteger oracle recomputes all 64 steps independently.

Expected observations: J first exceeds `UINT64_MAX` at n=13,
E at n=41; both exact finite representations continue through
n=64. At n=64 the J numerator has **1,248 digits**, E has
**1,182 digits**; their residues modulo 1,000,000,007 are
64,122,618 and 981,146,538 respectively. The common
denominator residue is 692,745,742. A corrupted limb fails
the residue check.

**Limits:** This is exact arithmetic for the supplied finite
decimal encodings. It does not compute all digits of irrational
mathematical e, evaluate literal infinities, establish physical
claims, detect dishonesty, or infer a more accurate "Euler."
It is not security-grade integrity authentication.
The fixed 192-limb capacity is more than required for these 64
steps; values larger than its capacity fail closed. Original
`shared/toolong.hpp`, historical Q62 sources, and the
user-defined Euler's Jig names remain unchanged.

NASCAR remains **EXPLORATORY**, with separate source/CI evidence
and `RELEASE=HOLD`.

## UNBOUND head-to-head: dynamically growing limbs (PR34-020)

The follow-on `unbound_head_to_head.cpp` removes the **fixed 64-step
array ceiling** while retaining all prior exact, finite witnesses.
It uses dynamically allocated base-10^9 arithmetic limbs via native
Win32 heap APIs and cannot overclaim actual mathematical infinity.
The caller chooses a strictly positive integer step count with
`--steps N`; excessive work or memory shortages fail closed.

The user Euler's Jig value and conventional mathematical e remain
distinct. Both are represented by **their supplied finite decimal
rationals**, so the forward path `n*value^n` is exact for that
representation. The reciprocal is retained structurally by
interchanging numerator and denominator, never by floating point.
The new source compares both numerators against a
`UINT64_MAX * denominator` bound, so overflow detection
doesn't mistake a floating-point approximation for a threshold.

At n=64, 128 and 256, native C++20 `/O2` outputs independent
modular residues and decimal digit counts, cross-checked by a
separate PowerShell BigInteger implementation. Invalid step
arguments fail. HEAD/TAIL SHA-256 source custody includes both
the native source and its qualification script.

**Safety boundaries:** no STL or system headers in the C++20 source;
no dynamic memory allocation inside a frame renderer; all
allocations are checked; finite time/RAM always apply.
A quiet calculation emits explicit PASS/HOLD output, not fake
real-time telemetry. Concurrence does not prove that
mathematical e is wrong, that anyone has cheated, or that
literal infinities can be evaluated. Separate INCEPTION-GATE
review and **RELEASE=HOLD** remain unchanged.

## Published engineering errata

The versioned [DIAERY engineering errata](ERRATA.md) records
corrections to value definitions, fixed-point formats, overflow
interpretation, physical versus mathematical geometry, and the
scope of XHTML Strict-profile qualification. It describes source
records, **not people**. It retains provenance, cites reproducible
test evidence, and does not grant release authorization.

## TRI-FLAVORS | C++20, XHTML Strict profile, Oracle Java 27 (PR34-022)

**Canonical DIAERY default** is now `shared/eulers_jig_canonical.hpp`:
`2864498081537971875 / 10^17` square units per triangle,
derived from the displayed 64-triangle total. The historical
mathematical constant `e` remains labeled in reference algorithms
and scientific regression tests; it is not a selectable alternative
in DIAERY's operational Euler's Jig. No sweeping replacement
of `exp`, factorial series, or dimensionless physical formulae
is claimed or attempted. These must keep their source meanings.

**Nice is a project predicate, not a theorem that other polygons
cannot lie inside circles.** We reuse `shared/u_circle_fit.hpp`:
the 3-4-5 triangle has nonzero area, three vertices contained
in the disk centered at doubled coordinates (30,40), and
minimum squared-radius threshold 2500. One sweep of squared
radii 1 through 4096 checks the monotonicity of containment.
Changing **radius** is not changing **radian** angle, and no
new optimizers or validators are created per iteration.

**Loop test:** interpret the *fractional parts* of the two
separately labeled numbers as dimensionless step sizes in
units of full turns. Exactly 64 steps do NOT close either
arbitrary-step loop:
`jig_gap_x1e18=278772184302000000`;
`historical_reference_gap_x1e18=29962978621104960`.
At 64 steps the historical reference happens to be closer
to a closed turn. Neither is a symmetric "winner" universally.
A separately **normalized** 1/64-turn step closes exactly
after 64 steps. An open line is not an invalid line.

**Three pairwise concurrence gates** use the new versioned
`diaery/tri_flavors.xhtml` fixture and a common output
witness. Native `diaery/tri_flavors.cpp` checks the existing
strict XHTML scanner and geometry. The isolated
`diaery/DiaeryTriFlavors.java` independently calculates
the same results, and it also requires an exact file match
to the fail-closed XHTML Strict-profile fixture. Neither
component resolves the external DTD or claims complete XHTML
1.0 DTD certification. `diaery/Invoke-TriFlavorQualification.ps1`
compiles C++20 using MSVC `/O2`, cross-checks an independent
PowerShell BigInteger oracle, and rejects mutated XHTML.

**Oracle Java toolchain:** as of November 23, 2026 the latest
Oracle JDK is **27.0.1.1** (released November 17, 2026).
The Java adapter requires that *exact* version and the
`Oracle Corporation` vendor. It is NOT an OpenJDK fallback,
it is NOT bundled in this 0BSD repository, and is NOT
downloaded during tests. The regular CI explicitly reports
`JAVA_XHTML=HOLD` and `JAVA_CPP=HOLD` if no verified
Oracle JDK is provisioned. The separate manual workflow
`.github/workflows/diaery-oracle-triflavors.yml` is
fail-closed and requires `SWE_ORACLE_JDK27_HOME` pointing
to an existing Oracle JDK 27.0.1.1 installation on the
Windows job runner. The workflow never silently passes
Java comparisons without executing Java.

**RELEASE=HOLD.** Conventional-e reference computations
remain scientific references, not implemented alternatives
to the DIAERY Euler's Jig definition.
