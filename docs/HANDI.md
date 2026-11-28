<!--
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

Scope: original first-party contributions only; existing reservations below remain authoritative.
-->

# HANDI | completed-action standards and adherence observation

> **Status:** HANDI-OBSERVATION-v0 · EXPLORATORY / REWRITTEN / VERIFY · maintainer naming proposal, not a merged implementation. Copyright (c) 2026 Paul Allen Cheek for original contributions; SPDX-License-Identifier: 0BSD where applicable. Third-party rights and historical versions remain independent.

## Publication action

**Define HANDI** as the working name for the source-preserving standards/adherence/folding system. **Distinguish** the theory of BiDi, TriDi and SigmaDi from the practical observations called BiPi, TriPi and SigmaPi. **Record** the first, second and third folding degrees as MINZA, NINZA and PING. **Preserve** the previous proposed label `zzzDi` as history, not as a renamed existing C++20 symbol or a competing source of authority.

These statements describe the documentation contract this page establishes, **not** a claim that code already executes every HANDI adherence check. Existing C++20 operators and tests remain unchanged.

## Standards, observations and the three folds

| Standard / expectation | Adherence observation (HANDI context) | Degree | Fold name | Current implementation evidence |
| --- | --- | --- | --- | --- |
| **BiDi** | **BiPi** | **1** | **MINZA** | Three MINZA groups cover loops [0,1,2], [3,4,5], [6,7,8] |
| **TriDi** | **TriPi** | **2** | **NINZA** | Primary and mirrored six-byte records per lane; currently derived from the same data |
| **SigmaDi** | **SigmaPi** | **3** | **PING** | Nine lanes × six bytes are braided by a reversible permutation; a roundtrip is checked |

**Use the left column as expectations**, the second as observations of adherence, and the final two as source-preserving representation stages. The words *solid-state* and *degree* denote a named, recorded state and folding order in this protocol. They do not claim a material solid-state device, physical dimension, mathematical polynomial degree, network connectivity, cryptographic identity or empirical quantum validation.

**Implementation boundaries:** [`shared/sigma_penta_ping.hpp`](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/blob/main/shared/sigma_penta_ping.hpp) defines nine lanes, three MINZA groups, NINZA arrays and the PING permutation. [`shared/minza_ninza_ping_unfold.hpp`](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/blob/main/shared/minza_ninza_ping_unfold.hpp) implements read-only NINZA difference and PING inverse checks. These compute byte arrangements, not yet a normative HANDI conformance verdict. Do not manufacture additional observations out of mirrored copies, virtual lenses, repeated passes or a successful local roundtrip.

## Directional orders, with no assumed winner

The three TriDi directions are `X=YIN` (claim → evidence), `Y=YANG` (evidence → bounded claim), and `Z=AIRGAP` (orthogonal countertest and threat/rights boundary). The maintainer proposed **HANDI** as the system name and `XYZ`, `ZYX`, `XZY`, `ZXY` as candidate *traversal orders*.

| Candidate | Visit sequence | Exact reversal | Status |
| --- | --- | --- | --- |
| `XYZ` | YIN → YANG → AIRGAP | `ZYX` | EXPLORATORY |
| `ZYX` | AIRGAP → YANG → YIN | `XYZ` | EXPLORATORY |
| `XZY` | YIN → AIRGAP → YANG | `YZX` | EXPLORATORY |
| `ZXY` | AIRGAP → YIN → YANG | `YXZ` | EXPLORATORY |

The additional reversals `YZX` and `YXZ` complete the six permutations; no order is selected as canonical. In particular, `reverse(XZY)=YZX`, **not** `ZXY`. A traversal changes reading order, **not** the original algorithm, its operator precedence, source input, original test records or evidence. AIRGAP independence must be declared before exposure to the other conclusions where blindness is required; do not claim an independent witness without independent evidence.

## Namespace separation: the older Pi mathematics survives

| Name | Existing mathematical source usage | HANDI adherence usage |
| --- | --- | --- |
| **BiPi** | Exact `π × π` for the author's integer `π=314159265358979323` | Observe BiDi adherence; not integer multiplication |
| **TriPi / Triπ** | Read-only three-route DecaSigma inspection in `shared/tri_pi.hpp` | Observe TriDi adherence; not an automatic reuse of an unrelated mathematical witness |
| **SigmaPi** | Prefix GammaGamma on an explicitly decimal-positioned `π` token | Observe SigmaDi adherence; not a factorial or a built-in C++ operation |

See [GammaGamma/Pi precision](GAMMAGAMMA_PI_PRECISION.md) and [TriPi implementation evidence](TRI_PI.md). Name each result with its **HANDI/adherence** or mathematical namespace; never infer identity or compliance from matching bare names.

The existing `SIGMADI-COLLISION-12D-v1` C++20 declaration-provenance comparator and the proposed `SIGMADI-REVIEW-BUTTERFLY4-v0` four-link overlay also remain distinct. Neither can silently replace a BiDi/TriDi/SigmaDi expectation or assume approval.

## Publish a bounded, present-tense observation

**Record** the versioned expectation and its authoritative source first. **Inspect** the X/Y/Z routes and any actual Pi-adherence checks, noting their dependency overlap. **Retain** the MINZA, NINZA and PING fold witnesses with their exact inputs. **Expose** counterexamples, NOT RUN prerequisites, contradictions, rights limitations and dissent. **Reconcile** the permissible statement through YAN. **Request** explicit sign-off separately.

The HANDI record contains:

`review_id | HANDI_protocol | canonical_source_and_SHA | expected_BiDi_TriDi_SigmaDi_versions | traversal_order | independent_X_Y_Z_evidence_or_NOT_RUN | BiPi_TriPi_SigmaPi_adherence_observations | MINZA_NINZA_PING_fold_witnesses | source_dependency_overlap | relevant_negative_tests | contradictions_and_dissent | permissible_claim | reviewer | explicit_approval | publication_commit`

**Classify** a finished record `HANDI_PASS_ELIGIBLE` only if the evidence supports its scoped claim and the reviewer independently decides on approval. **Classify** missing witnesses as `HANDI_NEEDS_REVIEW` and contrary evidence as `HANDI_CONTRADICTED`. Keep `REWRITTEN / VERIFY` when the documentation is revised without a completed test or authorized decision. **Never** replace material contrary evidence with a majority count, substitute a missing test with PASS or erase the pre-revision record.

## Source-policy compatibility and validation status

**Preserve** the repository's ISO **C++20** restriction and exclusive **`-O2`** (MSVC `/O2`) optimization level. Any project script must use **PowerShell Core, latest stable version**, beginning with **`Set-StrictMode -Version 2.0`**. Retain the maintainer's prohibited-tool, Oracle-Java provenance, in-house source-build and intellectual-property due-diligence requirements in [the writing operating procedure](WRITING_OPERATING_PROCEDURE.md).

**State the actual result:** This commit publishes a project terminology and observation specification. It does not create a new executable, pass an unrun test, change frozen mathematics, enforce toolchain policy in the existing CI, or prove a physical theory. A separate authorized implementation and source-backed review are required to promote the adherence-observation code to VERIFIED / CURRENT.

**Continuity:** Previous `zzzDi` terminology, original BiDi/TriDi/SigmaDi definitions, existing Pi math, PING source and all experimental records retain their recorded identities and versions. **Chosen, never assumed.**
