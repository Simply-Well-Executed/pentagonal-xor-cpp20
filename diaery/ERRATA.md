# DIAERY | Engineering Errata and Qualification Ledger

**EXPLORATORY | PR #34 | RELEASE=HOLD | 0BSD**

Errata belong to **source records and methods**, never to people.
Failure never becomes policy: preserve the original record, state the
limitation, show the correction, and cite the tested scope.
This document does not erase prior commits or issue a production release.

## ERR-001 | Conflation of two different "Euler" definitions

**Previous error:** Calling the conventional constant `e` a competing
definition of the user's Euler's Jig area, or treating one numerical
value as evidence that the other was false.

**Source records (retain both):**
- `EULER_JIG_ONE_X1E17 = 2864498081537971875ULL`. This records
  **28.64498081537971875**, exactly the quotient of the *displayed*
  total area `1833.278772184302 / 64`, with area units squared.
- `MATH_E_X1E18 = 2718281828459045235ULL`. This records
  **2.718281828459045235**, a *finite decimal approximation* to the
  conventional mathematical constant `e`, which is dimensionless.

**Correction:** Preserve distinct names, scales, and source ownership in
`shared/slidingtoolong.hpp` and `diaery/harmony.hpp`. No assertion
states that one definition replaces the other. The recorded jig quotient
is exact *relative to its rounded displayed source*, not proof that a
particular triangle has that exact trigonometric area.

**Open clarification:** The jig's intended angle was discussed in both
degree and radian terms. An observed angle must carry its unit and
a fully specified geometric construction. The earlier 1-degree and
1-radian exploratory calculations are **not automatically interchangeable**
or promoted to a single canonical geometric input.

## ERR-002 | Single-register overflow mistaken for an infinity test

**Previous error:** Using the first `uint64` overflow as though it
indicated the end of the underlying mathematical sequence.

**Correction:** For the two stated finite decimal inputs, calculate
`F(n)=n*X^n` using exact integer numerators over a shared decimal
denominator. Preserve reciprocal fractions by interchanging numerator
and denominator without a floating-point division. The numerator is
not restricted to 64 bits.

**Evidence:** `diaery/finite_head_to_head.hpp` checked 64 finite
steps; `diaery/unbound_head_to_head.cpp` adds checked, dynamically
growing base-10^9 limbs. The separate
`diaery/Invoke-UnboundQualification.ps1` BigInteger oracle agrees at
64, 128, and 256 steps. In this specific comparison, the first
**single-u64** threshold is at step 13 for the jig and step 41 for
the finite decimal approximation of `e`. Both multi-limb sequences
continue through 256. This cannot evaluate literal infinity or
establish that anyone has cheated.

**Verified CI evidence:**
[Native MSVC /O2 DIAERY qualification run #45283944521](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/actions/runs/45283944521),
including source HEAD/TAIL SHA-256 agreement, Fusion FIFO/LAFIT,
and the dynamic-limb BigInteger oracle.

## ERR-003 | Fixed-point format and rounding policy conflated

**Previous error:** Treating 64 fractional bits (Q64) as though every
whole value larger than one must itself fit in a single `uint64`, or
silently granting new precision by left-shifting a Q62 value.

**Correction:** Preserve conventional `e` and pi as Q64 fractional
words with their integer parts held separately. Retain the historical
Q62 source words without modification. The Q62-to-Q64 lift for the
intensity witness produces two zero low bits and **does not add
measured precision**. Preserve the documented differences between
earlier Q62 policy and independently rounded Q64 values.

**Evidence:** `shared/toolong_q62_core.hpp` and
`shared/slidingtoolong.hpp` static assertions. These fixed-width
witnesses are **not** arbitrary-precision values of irrational numbers.

## ERR-004 | Geometry closure versus fabricated physical safety

**Previous error:** Describing closed triangular surfaces and two caps
as if they established a manufactured watertight vessel or solid block.

**Correction:** Separate exact manifold edge pairing, available
sidewall perimeter, source-sheet cap fit, actual seam sealing,
water fill, and physical pressure tolerance. All physical claims
remain unverified. The angle example of 64 one-degree wedges covers
only 64 degrees, leaving 296 degrees before a full 360-degree turn.
`sqrt(360)` is recorded with an explicit numerical tolerance,
not mistaken for an exact rational square root.

**Evidence:** `shared/rectangloid_primitive_cxx20.hpp` and
`diaery/harmony.hpp`. The exploratory equilateral triangular tube
has perimeter 60 and two possible end caps; it is a different form
from a 1-degree slender triangular cross section, which requires
more than 60 units of sidewall strip.

## ERR-005 | Supported XHTML profile versus full XHTML 1.0 Strict

**Previous error:** Language that could imply a complete XHTML 1.0
Strict DTD validator, browser, general DOM/CSS interpreter, or
safety-qualified graphics engine.

**Correction:** DIAERY accepts a **narrow, fail-closed XHTML 1.0
Strict-profile subset** with exact permitted elements and syntax.
Its Fusion transformation snapshots accepted XHTML bytes into
C++20 source and qualifies the recoverable record; it does not
compile arbitrary web markup into a complete browser.

**Evidence:** `diaery/strict_profile.hpp`,
`diaery/parity_gate.hpp`, `diaery/fusion_qualification.hpp`,
`diaery/fusion_generator.hpp`, and the negative XHTML fixtures.
Compilation, exact byte recovery, and source regeneration are
qualified only within this supported profile.

## Qualification rules and release boundary

- **Exact identities:** zero numerical tolerance, including 64 squared
  equal to 4096, checked source decimal scale, triangle edge pairing,
  finite modular residue agreement, and recovered XHTML bytes.
- **Approximate calculations:** tolerance is explicit and bounded,
  never silently substituted for an exact equality.
- **Failure:** record the source and scope; do not reinterpret failed
  tests as successful or delete the previous record.
- **Ownership:** the source equation or implementation is authoritative;
  this ledger is a derived explanation, not a source that may invent
  new mathematical values.
- **Publication:** this erratum is visible on the PR branch for review;
  it is **not** a merged mainline change, official tagged release,
  engineering license, or safety certification.
- **Release:** INCEPTION-GATE and independent maintainer approvals
  remain in force. **RELEASE=HOLD** even where DIAERY's own tests pass.

**Chosen, never assumed.**

## ERR-006 | Full-repo substitution and speculative loop closure

**Request:** Make Euler's Jig area canonical and demote conventional
mathematical e to a non-selectable reference.

**Qualification:** `shared/eulers_jig_canonical.hpp` is DIAERY's
authoritative area record. Existing series and inverse-square tests
that intentionally use conventional mathematical e remain present
as historical scientific reference, not as a user-selectable DIAERY
runtime mode. Replacing the exponential base in those independent
algorithms with an area quantity would be a unit error. This
does **not** constitute a whole-repository replacement.

**Counterexample:** A 64-step closed-loop test based on fractional
turns does not close for either number. The recorded gaps are
0.278772184302 turns for the jig and 0.029962978621104960
turns for the historical reference. A normalized 1/64-turn
construction closes exactly. There is no general theorem that
every sufficiently long mathematical line loops.

**Proof boundary:** The 3-4-5 triangle Nice predicate uses exact
circle inclusion and an increasing squared-radius sweep; it is
not a universal property of all triangles or a quantum model.
The optional Oracle JDK 27.0.1.1 lane only qualifies if executed
on a verified Oracle distribution. Java unavailability means
**HOLD**, not implied parity. See `diaery/README.md`.
