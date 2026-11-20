# Documentation operating procedure | PENTAGONAL XOR

**Document ID:** DOC-OP-2026-11-19 | **Scope:** repository prose, diagrams, policy, software reference, validation and release instructions | **State:** REWRITTEN / VERIFY, pending maintainer adoption

**Publication action:** State the completed operation in the present tense, name its verifiable outcome, identify its source and limitations, and carry that statement through every relevant section. Keep the original algorithm and historical record unchanged.

**Owner and provenance:** Simply Well Executed project documentation. Original project writing copyright (c) 2026 Paul Allen Cheek, SPDX-License-Identifier: 0BSD where applicable. Third-party references are linked, not relicensed or copied into this repository.

**The one publication rule of thumb:** **Finish the work; state what it does in an active present-tense verb; carry that same action and its evidence throughout the entire publication.** Revise recursively: opening statement → method → observable result → limitations → decision → opening statement, until no section overclaims, contradicts or leaves the action unsupported. If implementation has not happened, say **PROPOSED**, **NOT RUN** or **NEEDS REVIEW**; never turn an intention into a completed claim. This single editorial principle supplements the concrete toolchain, evidence and legal obligations below; it neither replaces nor alters them.

**Publication check:** Begin with a completed, verb-led declaration such as "Define the review route and identify its provenance" when the document actually defines it. Follow with the source identifiers, exact steps, observed outcomes, counterexamples and reviewer status. Give each important statement a corresponding result or an explicit unresolved state. The documentation itself is the publication; a heading alone is not a deliverable.

## 1. Purpose and commander's intent

This is a project editorial standard, **not** a Department of the Air Force instruction, official correspondence, military performance statement, or regulatory authorization. Apply the practical discipline of concise military writing to C++20 technical records without claiming that external military policies govern or certify this software.

**Required order for a newly written page:**
1. **Completed action and scope:** State an active, present-tense operation the publication actually delivers; name audience, decision, tested behavior and project role.
2. **Authority and source:** Identify the canonical source file or immutable input. A README, diagram, report and checklist are **derived**, not a substitute for code or a signed evidence record.
3. **Execution / method:** Exact input, operation order, platform, guard, failure behavior and output.
4. **Evidence and limitations:** Specific test filenames or observed run IDs; separate observed results, theoretical identities, untested hypotheses and known limits.
5. **Action:** A runnable PowerShell Core (latest stable version; `Set-StrictMode -Version 2.0`) command, a review decision, or an explicit next verification task.
6. **Provenance and status:** Revision date, source commit, reviewer, dissent/exception and a non-destructive history pointer.

Use meaningful headings, short paragraphs, active verbs, parallel constructions, and plain definitions. Use the minimum acronyms necessary and define each on first use. Put the essential conclusion first; reserve complete technical derivations for later sections.


## 2. TriDi critical-thinking gate: three directions, one accountable decision

**Publication action:** Preserve BiDi and define TriDi as an additive three-direction critical-thinking gate. Review each material claim along **three separately traceable directions**: YIN checks claims forward against evidence, YANG checks evidence backward against claims, and **AIRGAP** independently tests the assumptions, threat boundaries, negative controls, and provenance neither route can establish alone. **YAN** reconciles their findings and records a scoped decision; it is a synthesis, not a fourth vote or a superior source.

**Status:** TRI-DI-CRITICAL-THINKING-v1, **EXPLORATORY / REWRITTEN / VERIFY** pending maintainer adoption. This is a *project-designed review method* and not an official Air Force, Six Sigma, quantum-mechanical, physical air-gap, cryptographic or independent-certification standard.

### Geometry and direction ownership

| Direction | Coordinate | Named route | Objective | Output |
| --- | --- | --- | --- | --- |
| X / Forward | `[1,0,0]` | **YIN** | Start with the author-defined claim and seek supporting evidence | Defensible claim boundary, source and method |
| Y / Reverse | `[0,1,0]` | **YANG** | Start with actual evidence and infer the smallest justified claim | Evidence-bounded permissible statement |
| Z / Orthogonal | `[0,0,1]` | **AIRGAP** | Isolate assumptions and independently challenge both other routes | Countermodel, threat/rights checks, evidence gaps |
| Origin | `[0,0,0]` | **CORE** | Hold the original source statement, algorithm and frozen input unchanged | Authoritative source reference, not a fabricated fourth finding |

The coordinates identify *review directions*, not spatial movements, ownership, physical security boundaries or three independent experiments. Shared sources must be recorded as shared, even if reviewers perform independent analyses. Neither repetition nor three displayed viewpoints creates extra empirical witnesses.

### X: YIN / forward trace

`CLAIM → SOURCE → METHOD → EVIDENCE → SCOPE`

| Gate | Required action | Record |
| --- | --- | --- |
| X1. Clarify | Preserve the original wording, project namespace, units, context and intended audience | Claim ID and unmodified source statement |
| X2. Attribute | Name original algorithm/input, publication edition, code commit and applicable license | Source path and immutable ID/SHA |
| X3. Reproduce | Follow original operation order, frozen protocol, precision and environment; do not infer an unrun test | Inputs, procedure, result or NOT RUN |
| X4. Challenge | Check invalid inputs, counterexamples and alternative explanations | Negative control and limits |
| X5. Bound | Apply trust, accuracy, precision, relevance and sufficiency | Maximum supported forward claim |

### Y: YANG / reverse trace

`EVIDENCE → METHOD → SOURCE → PERMISSIBLE CLAIM → LIMIT`

| Gate | Required action | Record |
| --- | --- | --- |
| Y1. Begin from evidence | Inspect the actual artifact, test, primary observation or source, not the forward conclusion | Artifact ID, bytes/hash where relevant |
| Y2. Reconstruct | Recover true configuration, runner, timestamps, measured values, sample count and code version | Method and environment |
| Y3. Trace origin | Follow `algorithm/source → route → stream → witness → ledger → render → audit` backward without editing inputs | Attribution and derivation chain |
| Y4. Restrict inference | State the narrowest claim justified by the observed evidence | Permissible claim and exclusions |
| Y5. Contradict | Locate inconsistent records, missing conditions or failed counterexamples | Disagreement, missing witnesses |

The algorithm and original source records take precedence over derived ledgers and presentations. An incorrect ledger is corrected with an audit entry; the original algorithm is not silently rewritten.

### Z: AIRGAP / independent orthogonal challenge

`ASSUMPTIONS → ALTERNATIVE MODEL → ATTACK / NEGATIVE CONTROL → BOUNDARY → DISSENT`

AIRGAP is a deliberate *methodological separation* from the other two directions. Before seeing their proposed final decision, the reviewer must record the independently chosen threat model or competing explanation and applicable exclusions. **AIRGAP does not claim that a real network is physically air-gapped.**

| Gate | Required action | Record |
| --- | --- | --- |
| Z1. Declare separation | Record the reviewer, available data, shared dependencies, and what was hidden or already known. Do not claim blind review if none occurred | Independence scope and overlap |
| Z2. Challenge assumptions | Identify an untested assumption, omitted case, different causal mechanism, or incompatible rights/security interpretation | Competing explanation and why it matters |
| Z3. Design countertest | Use an adversarial or out-of-domain input, a negative control, an alternative calculation or independent source where practical | Exact countertest plan, not merely rhetoric |
| Z4. Observe and contain | Record executed result, blocked prerequisite or NOT RUN; check licensing, binary provenance, tooling and environment boundaries as relevant | Actual observations, resource limits, provenance |
| Z5. Preserve dissent | Produce an explicitly scoped challenge, even if no defect is found, and retain conflicts with X/Y | Orthogonal finding and unresolved risks |

A failed or unavailable AIRGAP check is evidence of an incomplete investigation, not evidence that a claim passed or that a failed process has become policy. Its scope is proportional to the claim: a spelling correction does not need an unnecessary physics experiment, but a release-security assertion needs its threat and dependency boundaries examined.

### YAN: reconcile X, Y and Z; no voting shortcut

The three directions yield distinct records: `X_claim_limit`, `Y_permissible_claim`, and `Z_challenge`. YAN does not merge their evidence or average away disagreement. Its conclusion must be no broader than the intersection of supported claims and must address every material Z objection.

| Invariant | Acceptance criterion | If violated |
| --- | --- | --- |
| Identity | Same namespaced source statement, commit/edition, frozen inputs and defined terms | NEEDS REVIEW; similar is not identical |
| Provenance | Each observation has an attributable source, route and hash where appropriate | NEEDS REVIEW; no invented lineage |
| Semantic coverage | X claim does not exceed Y permissible claim, and Z does not expose an unresolved material counterexample | Narrow the claim or mark CONTRADICTED |
| Units and execution | Precision, sample sizes, runtime, operating system, exact compiler profile and outcome agree where applicable | Recompute; distinguish NOT RUN from PASS |
| Independences | Shared artifacts and correlated tests remain explicitly attributed; no synthetic majority | Treat dependence as a limitation |
| Source immutability | Original algorithm, source record, code points and frozen experiment configuration are not changed during review | Stop comparison; new protocol needs explicit change and new ID |
| Governance | A named authorized reviewer explicitly approves documented scope and acknowledges dissent | No automatic VERIFIED, even with 3 favorable reports |

**Possible decisions:**
- `TRIDI_PASS` **eligible for VERIFIED / CURRENT** only when all three directions meet their scoped requirements and an authorized reviewer explicitly signs off.
- `TRIDI_NEEDS_REVIEW` when any direction is absent, inconclusive, stale, unavailable or subject to unresolved material doubt.
- `TRIDI_CONTRADICTED` when observed evidence or a substantiated orthogonal counterexample refutes the proposed statement. Preserve the original, mark the correction and record supporting evidence.
- `REWRITTEN / VERIFY` when editorial changes are committed but the three-route audit and explicit sign-off have not occurred.

**No two-out-of-three or quorum decision:** Two agreeing routes do not defeat a material contradiction in the third. Silence, abstention, missing tooling, unrendered content or an empty seat is not agreement. A reviewer may record PASS, dissent, request changes or decline; none is assigned automatically.

### Traceable TriDi review record

Use one canonical record for each claim or document version. Required fields:

`review_id | claim_id | namespace | original_text | source_path | source_commit_or_edition | original_inputs | X_route_and_scope | Y_evidence_and_permissible_claim | Z_independent_assumptions_and_countertests | shared_dependencies | negative_test_results_or_NOT_RUN | units_and_precision | mismatch_and_dissent | decision | reviewer | explicit_approval | decision_time | revised_commit | historical_baseline`

**Completed example: a Windows OCI capability claim**

- **X / YIN:** Proposed claim: "Windows containers were validated." Supporting artifact: `docker info` returned an engine type.
- **Y / YANG:** The evidence supports only `ENGINE_RESPONDING`. With `host_container_build_executed=false`, no Windows container execution is established.
- **Z / AIRGAP:** Test a daemon-unavailable response, empty or unexpected engine type, wrong daemon/platform, and the absence of an executed container. An engine probe cannot substitute for a tested licensed Windows base image.
- **YAN outcome:** `TRIDI_CONTRADICTED` for the broad claim; eligible revised wording: "The Windows runner's Docker engine responded during the capability probe; no container build or execution was verified." The narrower statement still requires appropriate evidence and explicit sign-off.

**Completed example: synthetic photon-count statistics**

- **X:** Exact sample arithmetic reports mean and population variance for the specified counts.
- **Y:** Reverse inspection limits the result to the synthetic sample, numerical formula and source test.
- **Z:** Consider sample dependence, calibration, detector physics, shot noise and alternative distributions; do not label an unperformed measurement a quantum experiment.
- **YAN:** Preserve exact statistical claims; classify a physical gain/dispersion inference as EXPLORATORY / NOT VERIFIED.

### Compatibility, rendering and source authority

**BiDi is retained inside TriDi.** The prior two-pass gate and its original Git history remain identifiable. Old `BIDI_*` decisions are historical records; they are not automatically promoted to `TRIDI_PASS`. TriDi is a named evolution of review procedure, not a retroactive rewrite of previous evidence.

The review directions do **not** determine Unicode writing direction. For RTL/LTR spans, preserve semantic ownership and source code points, pair LRI/RLI/FSI with PDI, and do not cross an explicit newline in the project display protocol. Soft wrapping is not a paragraph boundary. A display failure does not authorize a canonical-data change.

**References:** AFH 33-337, *The Tongue and Quill*, inspected PDF pages 53-54, discusses evaluating trust, accuracy, precision, relevance and sufficiency and distinguishing facts from inference. The ALQ guide supports precise action/outcome writing. Neither source establishes this project's TriDi, YIN, YANG, AIRGAP or YAN names. The critical-thinking reference is a conceptual lens, not a certification.

## 2A. SigmaDi review extension: four alternating butterfly connections

**Publication action:** Define **four directed cross-check connections** over TriDi's YIN forward, YANG reverse and AIRGAP orthogonal records. The original CORE and all BiDi/TriDi evidence remain unchanged. YAN reconciles the four outputs without majority voting. A positive connection asks for corroboration; a negative connection challenges an overclaim or missing premise. **The signed links are checks, not arithmetic, votes, consent, proof or a new physical dimension.**

**Status:** `SIGMADI-REVIEW-BUTTERFLY4-v0`, **EXPLORATORY / REWRITTEN / VERIFY**; the proposed endpoint topology below is not yet approved as an established mathematical operator. The user's SigmaDi exploration already mentions **four butterfly connections with alternating polarity**, but exact pairwise endpoints and sign semantics are **proposed here for review**, not silently attributed to earlier work.

### Keep the pre-existing SigmaDi distinct

The repository's already implemented [SigmaDi COLLISION12](SIGMADI_COLLISION12.md) uses the separate ID `SIGMADI-COLLISION-12D-v1` and the bounded C++20 source `shared/sigmadi_collision12.hpp`. It inspects exactly **12 source-declaration provenance coordinates**, D0..D11, where **D11 identifies a declaration instance**. The source's classification distinguishes `DUPLICATE_BINDING`, `INCONSISTENT_DECLARATION`, `SCOPED_REUSE` and `CHANGED_REVISION` without silently coalescing instances or rewriting input.

**Never alias these protocols.** The existing COLLISION12 comparator is a declaration-identity guard, *not* four-link review logic, a physical 12th dimension or a general truth machine. SigmaDi review may use a correctly formed COLLISION12 receipt as **provenance evidence only** when examining declaration bindings. It may not feed arbitrary claims into the comparator as if they were valid source-language declarations.

### Proposed two-wing, four-link routing

The forward YIN record is a *read-only comparison junction* for two counterposed butterfly wings: YIN ↔ YANG and YIN ↔ AIRGAP. The underlying TriDi report remains independently attributable.

| Link | Polarity | Connection | Review task | Required witness |
| --- | --- | --- | --- | --- |
| `Σ0` | **+** | YIN → YANG | Does the forward claim fit the reverse evidence? | Same namespaced claim/source and bounded support |
| `Σ1` | **−** | YANG → YIN | What unsupported part or conflicting observation does the reverse result expose? | Scoped counterclaim, negative evidence or tested absence |
| `Σ2` | **+** | YIN → AIRGAP | Can the precise claim and assumptions be subjected to an independent orthogonal test? | Declared assumptions, shared-dependency scope and test plan |
| `Σ3` | **−** | AIRGAP → YIN | Does the countertest refute the claim or expose provenance, toolchain, rights or environmental gaps? | Actual result, unresolved objection or NOT RUN |

**Ordering:** `Σ0:+ → Σ1:− → Σ2:+ → Σ3:−`. Here **+** is corroboration-seeking and **−** is refutation-seeking, *not* truth/false polarity, signed numerical gain, ownership or moral rank. A negative test finding no defect remains `NO_COUNTEREXAMPLE_OBSERVED` in its tested scope; an unexecuted test is `NOT_RUN`, never PASS. The direct YANG↔AIRGAP evidence relationship is examined in the final YAN synthesis, without inventing another link.

### Execution and audit contract

1. **Pin CORE**: freeze original wording, algorithm, input, source SHA/edition and experiment protocol. The review does not edit CORE.
2. **Collect TriDi**: keep X/Y/Z routes, original evidence IDs and distinct test contexts, including shared dependencies. Missing records are not manufactured.
3. **Evaluate all four links in order**. Each has source, destination, polarity, tested scope, observed result and dissent. No synthetic independent witnesses or combining of different revisions.
4. **Check declaration provenance** with COLLISION12 only when its twelve valid source-language axes are available. `DUPLICATE_BINDING` or `INCONSISTENT_DECLARATION` blocks an identity-based inference; source records remain intact.
5. **Reconcile through YAN**. No four-of-four or three-of-four count can override a material contradiction, an invalid source, an unrun required negative test or absent approval.
6. **Record a signed, scoped decision**. Only `SIGMADI_PASS_ELIGIBLE` can proceed to a separately authorized VERIFIED decision; `SIGMADI_NEEDS_REVIEW`, `SIGMADI_CONTRADICTED` and `REWRITTEN / VERIFY` preserve open issues and provenance. Silence or absence never equals consent.

**Minimum ledger:** `sigma_review_id | original_claim_id | tri_di_review_id | protocol_id | frozen_source_sha | Σ0_polarity_source_target_result | Σ1_polarity_source_target_result | Σ2_polarity_source_target_result | Σ3_polarity_source_target_result | collision12_receipt_if_applicable | shared_dependencies | NOT_RUN_or_observed | dissent | yan_permissible_claim | outcome | reviewer | explicit_approval | audit_commit`.

**Example, Windows OCI:** `docker info` returning `ENGINE_RESPONDING` cannot establish a Windows container build. `Σ0` compares the claim to the probe. `Σ1` exposes the missing build witness. `Σ2` specifies daemon-unavailable/invalid-response challenges. `Σ3` retains unresolved execution or licensing limits. YAN must reject "Windows container execution passed" unless an actual appropriately sourced build-and-execution record exists.

**Historical continuity:** BiDi and TriDi decisions remain independently identified; `SIGMADI-REVIEW-BUTTERFLY4-v0` is an exploratory additive review layer, **not** a replacement for `SIGMADI-COLLISION-12D-v1`, nor a silent promotion of old `TRIDI_PASS` status. Source/algorithm outranks route, witness and derived ledger. Move, archive and revision do not erase historical meaning.

**Tooling:** This document-only proposal introduces no executable dependencies. Any later implementation remains subject to ISO C++20, only `-O2` (MSVC `/O2`), PowerShell Core (latest stable) with `Set-StrictMode -Version 2.0`, and the maintainer's license/source-binary restrictions in section 9. Changes to frozen experimental protocols require an explicit new ID, not an editorial overwrite.


## 3. Writing and accountability guides: inspected editions

These are editorial and evidentiary reference materials, **not** Air Force endorsement of this repository, a military order to software contributors, or a substitute for applicable standards.

| Reference and inspected edition | Guidance used | Limit |
| --- | --- | --- |
| **AFH 33-337, The Tongue and Quill**: 27 May 2015; Change 1, 19 Nov 2015; certified current 27 Jul 2016. Uploaded `afh33-337.pdf`, PDF pp. 1, 4-5, 53-54. | Analyze audience/purpose, research sources, support claims, edit, seek feedback; check quality and sufficiency of evidence. | Project editorial adaptation, not official compliance. |
| **Airman Leadership Qualities Writing Guide**: uploaded `ALQ Writing Guide.pdf` is Version 1, **10 Jan 2023**, PDF pp. 1-3. | Use standalone plain-language statements describing an action and its impact/result/outcome; favor representative evidence over padded claims. | User referenced 7 Apr 2025; this supplied PDF is a **different edition** until established otherwise. |
| **DAFI 36-2803**, Military Decorations and Awards Program: 3 May 2022, Change 3 dated 21 Jan 2026; certified current 28 Jan 2026. Uploaded `dafi36-2803.pdf`, PDF pp. 1, 4-5. | Separate recommendation, verification, formal approval and recognition. | A GitHub review or release is not an official decoration. |
| **DAFI 36-2907**, Adverse Administrative Actions: 14 Jan 2026, supersedes 14 Oct 2022. Uploaded `dafi36-2907.pdf`, PDF pp. 1-3, 5. | Preserve distinct observations, response opportunities, findings, correction and records in project issue handling. | DAF personnel actions do **not** govern GitHub contributors; do not analogize failed CI to misconduct. |
| **AF Form 716** | Do not invent fields or signatories; preserve only verified form requirements. | **No copy supplied; form-specific claims pending.** |
| **USAF Educational Conference, October 1956** | Historical background may be used when separately supported. | Not a current writing directive or approval authority. |
| **Six Sigma DSI Critical Thinking** | Ask what is assumed, what could falsify a conclusion, and whether evidence is representative. | Conceptual lens only, not a certification. |

**Source custody:** The four inspected Air Force writing/instruction copies were provided in the conversation, not added to the repository or redistributed. Exact dates/page references describe those copies, not independently refreshed official websites. Maintain the ALQ edition discrepancy as NEEDS REVIEW until the intended 2025 revision is inspected. Do not copy publication text into project files.

## 4. Evidence classes

| Class | What it permits | What it does not permit |
| --- | --- | --- |
| SOURCE / CURRENT | State what code currently implements at a pinned SHA | Infer physical validity or user intent |
| TEST / VERIFIED | State the exact test inputs and pass conditions | Generalize beyond tested environments |
| LAB / EXPLORATORY | Describe a hypothesis, controlled transformation or model | Report the hypothesis as established fact |
| HISTORICAL / ARCHIVE | Preserve original scope and observed outcomes | Promote older output as today's code |
| REVIEW / PENDING | Track a rewritten claim awaiting independent checks | Mark a rewrite as validated automatically |
| CONTRADICTED | Retain the disproved version, correction and source | Erase the version or silently substitute a new meaning |

Source-code algorithms and original input records outrank derived ledgers; a renderer never changes canonical identity. Preserve exact decimal input strings, Unicode code points, channel routes, original file hashes and experiment configuration when relevant.

## 5. Editing and verification workflow

1. Inventory every Markdown, legal notice, SVG, README, template and operator protocol. Register new items before changing status.
2. Read the existing page and applicable source/test/workflow. If source was not read, label the page **REWRITTEN / VERIFY**, not VERIFIED.
3. Complete the claimed action before describing it as done. Write its opening declaration with an active present-tense verb; recursively revise the method, evidence, limitations, outcome and opening until they agree. Label any unfinished work PROPOSED / NOT RUN / NEEDS REVIEW.
4. Retain canonical formulae, exact result values, frozen experimental protocols, historical evidence and copyrights. A copyedit is not authorization to change them.
5. Link relative repository files; link external references without copying third-party protected passages.
6. Test claimed PowerShell Core (latest stable version; `Set-StrictMode -Version 2.0`) commands and targeted C++20 `-O2`/MSVC `/O2` behavior in CI when possible. Report exact job IDs and failures. Never claim a green build that has not run.
7. Compare with prior source, document contradictions and keep an audit entry with head SHA, reviewer, evidence and any dissent.
8. Close the register row only after a separate verification decision. Review is not sign-off; silence is not consent.

**Contributor model:** An author may propose a revision; a reviewer independently confirms current behavior; the maintainer chooses whether to merge. No person, office or external title silently assumes approval.

## 6. Apply the single completed-action publication rule

Write: **"The implementation computes the odd parity probability of five independent Bernoulli variables."**
Do not write: **"The implementation proves identity or security."**

Write: **"This synthetic series has a population variance of 150 counts squared."**
Do not write: **"The code demonstrates quantum radiation dispersion."**

Write: **"This Windows runner reported no compatible OCI daemon; the capability check is advisory."**
Do not write: **"Windows container validation passed."**

Use explicit code-path namespaces and PowerShell Core (latest stable version; `Set-StrictMode -Version 2.0`) blocks; do not silently convert the user's tooling into Python or JavaScript automation. Never substitute 16-digit floating-point display precision for full exact `toolong` integer serialization.


## 7. Reference links and reproducible writing environment

The project references the user's supplied copies for edition-specific statements. External links may require access; they are not substitutes for the inspected copy.

- [AFH 33-337, *The Tongue and Quill*](https://static.e-publishing.af.mil/production/1/administrative_assistant/publication/afh33-337/afh33-337.pdf): inspected 2015/2016 edition, particularly PDF pages 53-54.
- [ALQ Writing Guide, myFSS](https://myfss.us.af.mil/USAFCommunity/s/knowledge-detail?pid=kA0t0000000wkx6CAA): inspected **10 Jan 2023 Version 1**; **7 Apr 2025 user reference remains unverified**.
- [DAFI 36-2803](https://static.e-publishing.af.mil/production/1/af_a1/publication/dafi36-2803/dafi36-2803.pdf): inspected Change 3 / Jan 2026 edition.
- [AF Form 716](https://static.e-publishing.af.mil/production/1/af_a1/form/af716/af716.pdf): not supplied or inspected; no inferred fields.
- **DAFI 36-2907**: inspected 14 Jan 2026 uploaded edition; supplied web address had a truncated `.pd` suffix and is not repeated as an authoritative destination.
- [Six Sigma DSI Critical Thinking](https://sixsigmadsi.com/glossary/critical-thinking/): conceptual source only.

**PowerShell Core (latest stable version) is the only prescribed shell for project examples.** Every PowerShell Core script and code sample MUST include `Set-StrictMode -Version 2.0` at its start. Record `$PSVersionTable.PSVersion` in reproducible evidence and check native command exit codes; no assumption that an installed local version is actually the latest stable release.

**Script prologue (PowerShell Core, latest stable version):**

~~~powershell
Set-StrictMode -Version 2.0
$ErrorActionPreference = 'Stop'
$PSVersionTable.PSVersion
~~~

Any external process or nested `pwsh` script must observe the same PowerShell Core version and StrictMode 2.0 policy. CI may use an explicitly recorded version when reproducibility requires pinning; do not silently relabel a pinned version as latest.

## 8. Review register

See [DOCUMENT_REVIEW_REGISTER.md](DOCUMENT_REVIEW_REGISTER.md). An editorial status of **REWRITTEN / VERIFY** reports a changed presentation only. It does **not** assert that source and all CI claims were independently retested. An explicit sign-off is required to promote an item to **VERIFIED**.

## 9. Maintainer-directed implementation and supply-chain policy

**Authority and status:** These restrictions are expressly directed by the repository maintainer for PENTAGONAL XOR and are proposed as the **highest-priority project implementation and documentation requirements**, subject to applicable law, third-party licenses, and the maintainer's explicit adoption through review. They do **not** purport to be U.S. Air Force orders, official Department of Defense policy, legal judgments or rules binding unrelated projects. Record unresolved conflicts rather than inventing approvals.

### Allowed implementation profile

| Dimension | Prescribed configuration | Prohibited or blocked without a new, explicitly approved policy decision |
| --- | --- | --- |
| C++ dialect | **ISO C++20 only**: `-std=gnu++20` with GNU-compatible extensions or MSVC `/std:c++20`; CMake `CXX_STANDARD 20`, `CXX_STANDARD_REQUIRED ON`, `CXX_EXTENSIONS ON`. | Other C++ language versions, disabled CMake CXX extensions, or silent fallback to another standard |
| Optimizer | **`-O2` only** for GNU/Clang-family compilers; **`/O2` only** for MSVC, as its platform spelling of the same requested optimization level. Warning/hardening options may be separately evaluated if they do not change this optimization mandate. | Alternative optimization levels; PGO instrumentation or profile use; LTO/whole-program optimization and additional speed/size/architecture-tuning options (for example `-O3`, `-Ofast`, `/GL`, `/LTCG:PGOptimize`, `/fp:fast`, or `/arch:AVX2`) as part of the normative build |
| Shell | **PowerShell Core, latest stable version**. Start each script and every sample with **`Set-StrictMode -Version 2.0`** and capture version/exit status in test evidence. | Other scripting shells or an unqualified PowerShell invocation treated as policy-compliant |
| Languages and package tooling | Reviewed C++20 and the prescribed PowerShell Core environment. Existing C11 interfaces require a **recorded compatibility exception or separate migration decision**, not a false C++20 claim. | **Python, Lua, Go, Rust, Node.js, npm, Gradle**, and their runners/package/build automation |
| Java provenance | **Only Java obtained directly from Oracle**, if Java is explicitly authorized at all. Record origin, precise distribution, permitted uses and license rights before integration. | **Non-Oracle Java distributions**, including previously configured Temurin builds. A Gradle-free Java build is not exempt from this requirement |
| Artifacts and dependencies | Prefer compiling reviewed source in-house and linking to appropriately licensed, preferably open-source libraries. | Blind trust in downloaded/precompiled binaries, unchecked third-party source or unsupported redistribution |

**Pinned does not mean universally portable:** Every release build must report the compiler, selected C++20 standard, exact optimization flags, dependency manifest and test platform. An experimental branch may retain historical flags as **LAB / HISTORICAL** evidence but must not pass them off as the current authorized release profile.

### Source-first provenance and executable trust gate

1. **Identify** each dependency/toolchain by name, maintainer, exact version, source location, build recipe, target platform, published license and upstream commit or release identity.
2. **Prefer source acquisition and in-house compilation.** A binary or package is not trustworthy merely because it was downloadable, signed, pinned, popular or supplied by a hosted CI image.
3. **Before any unavoidable third-party binary is executed**, independently assess its source, publisher authentication, cryptographic digest/signature, supply-chain path, isolation, legal redistribution rights and threat model; record the exception and obtain explicit maintainer approval. If source-building and provenance checks cannot satisfy the policy, **BLOCK** its use rather than silently allow it. These controls reduce risk; they never prove a binary harmless.
4. **Inspect all transitive open-source license obligations**, including notices, copyleft terms, source-distribution requirements, linking restrictions, patent grants and any incompatible licenses. Honor the exact governing license for each imported component.
5. **Preserve third-party intellectual property rights.** Updating attributions or the project's licensing document does **not** grant unowned rights or cure an incompatible license. Seek permission, replace the dependency or remove the affected component when obligations cannot be satisfied.
6. **Retain an audit trail:** source and version hashes, compiler flags, dependency inventory, original license texts/required notices, permission decisions, exception approvals, tests and release artifacts. Unsupported claims or lack of due diligence are **not** acceptable substitutes for evidence.

This is a risk-control and intellectual-property due-diligence policy. It does not declare an alleged conduct pattern criminal without a legal finding, and it does not treat ignorance or "plausible deniability" as evidence of compliance.

### Existing implementation conflicts and no-silent-mutation rule

**Retire** the former non-Oracle Java, Android, Docker, PGO and Node-backed CI workflow lanes from the active PR branch. Their previous source versions and measurements remain in Git history at commit `601b4352bc38f146def4642a917ef38a0b37b2d9` and earlier revisions. Java/Android source files still exist as non-executed historical material; their presence is not an approved Java build or release.

**Apply** CMake `CXX_STANDARD 20`, `CXX_STANDARD_REQUIRED ON`, `CXX_EXTENSIONS ON`, `-O2` or MSVC `/O2`, and interprocedural optimization OFF for current native builds. The earlier PGO benchmark stays HISTORICAL / LAB, not a current release configuration. The native Windows CI has passed all **34/34** C++20 CTests after the at-rest text normalization. It still **fails the latest-runtime prerequisite**: hosted PowerShell Core 7.6.9 is older than the officially published 7.7.0. A hosted runner is not an in-house-compiled toolchain. The separate C11 include-free header remains tracked as a compatibility interface, not relabeled C++20 source.

**Record the implementation change:** The retired workflows are no longer active on this PR branch; original versions remain auditable in Git history. The new single Windows-native workflow calls no Node-backed actions. The latest PowerShell Core prerequisite is **BLOCKED / NEEDS REVIEW**, not silently satisfied. Only explicit maintainer approval can adopt the current constraints or issue a release.

### Audit acceptance criteria

A document or release may be labeled **POLICY-CONFORMANT** only after the TriDi gate and the applicable SigmaDi four-link review overlay (both reconciled by YAN) trace its claim to current C++20 compile flags, the PowerShell Core latest-stable/StrictMode 2.0 execution evidence, dependency/toolchain provenance, applicable license obligations and negative controls. A missing artifact, silent permission, or green job on an unreviewed toolchain cannot establish policy conformity.

## 10. PowerShell Core Magic Comb: repository text at rest

**Apply** the maintainer's exact canonical text transform to Git-tracked source before any source is compiled or executed: decode strict UTF-8, normalize to **NFKC**, remove code points **U+200B through U+200D and U+FEFF**, then encode as UTF-8 without a BOM. This **intentionally rewrites source text**; the same single native CI job verifies that all committed inputs are already cleaned. Do not run a second dependent job graph or a Linux container for this step.

**Implement** the operation in `ci/Invoke-MagicComb.ps1` using **PowerShell Core (latest stable)** with `Set-StrictMode -Version 2.0` and JSON evidence nesting depth **9**. The `Apply` mode writes the canonical bytes at rest and reports before/after hashes for each changed file; `Verify` refuses a noncanonical checkout; `SelfTest` checks the exact normalization, removal and invalid UTF-8 cases. Keep all original revisions in Git commit history.

**Record** the semantic impact. NFKC may fold distinct mathematical symbols, full-width characters and ligatures. Accepted changes to source require code/test review against the cleaned version. The filter does not prove elimination of memory corruption, malicious input or supply-chain hazards; untrusted runtime input needs its own boundary validation.

**Prohibit** any Node-backed GitHub Actions, third-party shell wrappers, Java build lanes or container tasks in the active native-only workflow. The one native Windows-hosted job checks the source using PowerShell Core and compiles C++20 with CMake extensions **ON** at `-O2` or MSVC `/O2`. The hosted runner is still external infrastructure; no undocumented claim of wholly in-house toolchain provenance is made.
