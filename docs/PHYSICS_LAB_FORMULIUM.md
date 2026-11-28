# Physics laboratory | FORMULIUM

**Review branch:** [PR #338](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/pull/338) · **Language:** C++20 · **First-party license:** 0BSD · **Status:** exploratory; native MSVC verification pending.

## Navigation

| Laboratory | Canonical source | Purpose |
|---|---|---|
| GEOMTRY / FHERE / CREST | [physics_lab_geometry.hpp](../shared/physics_lab_geometry.hpp) | Projection, crown continuity, isolated quadratic bias, rotations and reversible witnesses |
| FORMULIUM | [physics_lab_formulium.hpp](../shared/physics_lab_formulium.hpp) | Exact fractions, Boolean masks, discrete topology and evidence abstractions |
| Independent physical experiments | [swimming](../shared/physicals_swimming.hpp), [SPHERROID](../shared/spherroid.hpp) | Seeded motion and bounded radial projection |
| Shared numerical core | [sigma_form_geometry.hpp](../shared/sigma_form_geometry.hpp) | Attenuation, inverse-square profiles and geometric primitives |
| Regression | [physics_lab_formulium_test.cpp](../tests/physics_lab_formulium_test.cpp), [inverse_square_profile_test.cpp](../tests/inverse_square_profile_test.cpp) | Compile-time witnesses and runtime parity |
| Build registration | [CMakeLists.txt](../CMakeLists.txt) | Native test targets and installation |

## Transformation map

```text
FHYR -> FHERE -> FHEYR / DANDY
                    |
                    +-> DAND / LAND -> LADDERING -> LADDERINGANDY
                                                        |
                                                     MCLEAVE
                                                        |
                                                      CUNT
                                                        |
                                            REQUEST / RESPONSE / RELATION
                                                        |
                                    RELATED (factor) -> RELTERING / RELT
                                                        |
                                                 FONTANE / NEEDLE
                                                        |
                                                  TIME_TO_KILL
                                                        |
                                             CHAOTIC_LAWFUL_GOOD
```

Slashes denote aliases or grouped components, not additional numerical operations. The independent FORMULIUM namespace uses legal and institutional terminology only as project-local discrete mathematical labels.

## Quick start

The laboratory headers themselves have no standard-library includes. The following example uses repository-relative include paths; compile from the repository root with an existing native MSVC environment.

```cpp
#include "shared/physics_lab_geometry.hpp"
namespace P = SWE::PHYSICS_LAB;

constexpr bool witness() {
    const P::CUNT::CONTAIN inputs[9] = {
        {1,2},{2,3},{3,4},{4,5},{5,6},
        {6,7},{7,8},{8,9},{9,10}
    };
    const auto relation = P::RELATION::SELF_CONTAIN(inputs);
    const auto punched = P::RELTERING::PUNCH(relation);
    P::TIME_TO_KILL::POSTURE p(punched);
    for (unsigned i=0; i<5; ++i)
        if (!p.NEXT()) return false;
    const auto w = P::CHAOTIC_LAWFUL_GOOD::CONTAIN(
        p, {2,3}, 511, {2047});
    return w.COMPLETE && w.WITNESS.VERIFIED
        && w.GEOMETRY.X == -2 && w.GEOMETRY.Y == -3;
}
static_assert(witness());
int main() { return witness() ? 0 : 1; }
```

## Invariants and operation accounting

| Operator | Mathematical contract | Source-level cost / boundary |
|---|---|---|
| MCLEAVE::ISOLATE | I = S*S; B = I*Y; bow = (-B,0,B) | Two multiplications on valid input |
| REQUEST | Nine indexed crests, angles 0 through 320 degrees | Eighteen bias multiplications on nine valid inputs |
| RELTERING::PUNCH | Copy nine recasts; omit symbolic center | No new bias calculation |
| NEEDLE | Nine positional advances; final position 0, angle 360 | No new bias calculation |
| TIME_TO_KILL | Four index advances, fifth coordinate negation | Final underlying angle 160; no physical coordinate rotation during advances |
| CHAOS | (X XOR K) XOR K = X | Two XOR operations per round trip |
| ADVANTAGE_BIAS | U AND R = 0; U OR R = FULL | Validated complementary masks |
| SPHERROID identity | Valid residual=1 or squared norm=1 returns source | Eight Newton iterations bypassed |
| attenuation(0) | Exact unity | 64 series iterations bypassed |

Counts describe source algorithms, not measured CPU instructions or performance. Floating-point operations are not exact real arithmetic; rounding and underflow remain possible.

## Verification and release posture

| Witness | Coverage encoded | Status |
|---|---|---|
| masks() | 2,048 eleven-bit configurations and invalid width input | static_assert present |
| permutations() | 13 positional samples; six labeled permutations | static_assert present |
| legal() | 4,096 Boolean input configurations | Runtime check defined |
| crown() | Source continuity, rejected fold, flip and radius | static_assert present |
| crest_cycle() | Nine independent crests, nine turns, five-stage sequence, XOR restoration | static_assert present |
| inverse-square profile | Prepared and direct evaluation parity; zero attenuation | Regression source present |

From a configured native MSVC build directory using the repository's approved local CMake/CTest toolchain, the registered tests can be selected with:

```text
cmake --build <build-dir> --config Release --target physics_lab_formulium_test inverse_square_profile_test
ctest --test-dir <build-dir> -C Release -R "physics_lab_formulium_integrity|inverse_square_profile_parity" --output-on-failure
```

The targets are registered on the regular BUILD_TESTING path, not the early-return core-only path. No execution, benchmark or release approval is asserted here. Record exact commit, compiler version, build flags and observed result before promoting a witness. Failure never becomes policy.

## Protocol history and limitations

#### PHYSICS-LAB/FORMULIUM-1

Exploratory 0BSD C++20. Geometry and discrete evidence models are separate from empirical physics, legal advice, and determinations about real people. No institutional or professional role is conferred.

Canonical entrypoints: shared/physics_lab_geometry.hpp and shared/physics_lab_formulium.hpp. Historical names are aliases where behavior was unchanged; aliases do not recreate each prior API spelling. Existing MSL SPHERROID and swimming experiments are unchanged. New GEOMTRY supports feedback in a bounded near-unit to cube-corner radial domain. TENZ is a project-local resistance parameter, not mechanical tension. LONG and SHORT have distinct rounding and may underflow for tiny results; neither is universally faster or more accurate. No performance claim without native benchmarks.

LADDERDI uses 32-bit unsigned operands widened before multiplication; exact fractions are not reduced or divided. INTERROGATION enumerates six labeled permutations. QUESTION/QUERY advancement is a stateless rung check: unique evidence IDs, non-reuse and game termination require a separate ledger/state machine. No claim that real errors forfeit credibility. INQUISITION thresholds are explicit; only the example first rung is 36.

COREAO preserves operands and XOR shell before OR; changed bits are E & ~shell, not all potentially masked bits. Historical masking names remain exploratory. The original LAW masking concept is distinct from the later LAW nine-check structure. Four/five/thirteen equivalence lines are symbolic, not additional operators or proofs. EVIDENCE_ADMISSION rotation is not numerically coupled to consultation. PUBLICOPINION is metadata only.

LEGALY_IAM_DEAD and LEGALLY_IAM_ALIVE retain the original evaluation-complete predicate; LEGAL_SYSTEM additionally requires competency confirmation when a concern exists. Findings are supplied flags, not adjudication. COT direct findings use positive polarity for first four bits, unsupported inference for fifth; LAWFUL_GOOD_BIAS exposes the older review-mask polarity. Unsupported inference is not necessarily intentional deception. CASH has five defined bits and thirteen positional labels; eight additional semantics remain undefined. Case colors are not preset. ROT9 is a positional permutation, distinct from the earlier JURY evidence predicate. EBC9 is intentionally undefined and injected as a policy. MONGOLIAN_THE is nonbinding.

GSADVANTAGE is a schema for procurement evidence review, not a dataset of adjudicated cases. Federal jurisdiction is not universal. RUN_THE_CLOCK is illustrative; infinite budget/time flags are assumptions, not actual resources. LAW has nine checks, ACE complements the mask, NOBODY fixes the starting mask to 511, GREG adds capital/time flags. ADVANTAGE_BIAS validates width and input before complementing. A cleared bit is not independent proof.

Native MSVC compilation and benchmarking remain required. Regression includes 2048 masks, 13 rotations, six permutations and 4096 Boolean input cases. Do not claim execution until observed. No repository-wide production migration or legal case verification is implied.

### PHYSICS-LAB/FHERE-1

FHYR is the source coordinate; FHERE initially equals it and retains it immutably. FHEYR/DANDY contain the Y crown. DAND releases a rotated coordinate witness (180 degrees about Z). LAND uses prescribed 33 ft/s^2, not standard gravity; its velocity is the unconstrained ballistic sample, not post-impact velocity. No drag, collision, or crown contact mechanics are defined. LADDERING swaps role types (LAND head, DAND base) and copies the live crown; LADDERINGANDY preserves that live state across the G-D symbolic seam. MCLEAVE replaces the earlier historical jig name and computes (-x*x*y, 0, x*x*y) with two multiplications. Bounded floating-point domain, not exact real arithmetic. No physical launch or cleavage mechanism is implied.

Projection identity bypass: valid d=1 or computed squared norm=1 returns the original point and skips eight Newton iterations. This is a new experimental revision; no measured speedup is claimed. Invalid folds do not mutate the current point. The geometry model is still point-based, not a complete mesh.

Main receives the previously staged zero-attenuation and reusable inverse-square profile implementation only; experimental crown and bow remain in the review branch.

### PHYSICS-LAB/CREST-1

MCLEAVE::ISOLATE is the single source of the quadratic bias; CLEAVE and CUNT delegate to it. CUNT exposes S, I=S*S, Y, B=I*Y and the mirrored bow. The selected symbols are U+1D54F and U+2716, stored as escaped Unicode scalar values, not C++ operators. REQUEST retains nine independently supplied crests at 40-degree intervals and a symbolic figure-eight center (180-degree twist metadata). RESPONSE and RELATION are aliases. RELATED factors out progression. RELTERING punches only the symbolic center, preserving all nine recasts; RELT abbreviates it. FONTANE/NEEDLE retains an immutable source and advances a positional index through nine turns. TIME_TO_KILL is a project-local geometric sequence name: four 40-degree index advances and a fifth coordinate sign negation, not a physical action. It starts from turn zero, rejects further steps after completion, and does not recalculate the bias. CHAOTIC_LAWFUL_GOOD uses caller-supplied CHAOS as a reversible XOR key, not cryptographic encryption or randomness. Its composition overload returns the geometric output and integer round-trip witness separately; it does not claim the key alters geometry. The prior MCLEAVE historical name remains provenance, not a separate operator. Native MSVC execution has not been observed.
