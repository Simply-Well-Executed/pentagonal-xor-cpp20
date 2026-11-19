# Documentation operating procedure | PENTAGONAL XOR

**Document ID:** DOC-OP-2026-11-19 | **Scope:** repository prose, diagrams, policy, software reference, validation and release instructions | **State:** REWRITTEN / VERIFY, pending maintainer adoption

**BLUF:** Say what a component does, identify the evidence that supports it, explain what it does *not* establish, and tell the reader exactly how to reproduce or review the result. An editorial revision never changes the underlying algorithm or the historical record.

**Owner and provenance:** Simply Well Executed project documentation. Original project writing copyright (c) 2026 Paul Allen Cheek, SPDX-License-Identifier: 0BSD where applicable. Third-party references are linked, not relicensed or copied into this repository.

## 1. Purpose and commander's intent

This is a project editorial standard, **not** a Department of the Air Force instruction, official correspondence, military performance statement, or regulatory authorization. Apply the practical discipline of concise military writing to C++20 technical records without claiming that external military policies govern or certify this software.

**Required order for a newly written page:**
1. **BLUF and scope:** Audience, decision, tested behavior, current project role.
2. **Authority and source:** Identify the canonical source file or immutable input. A README, diagram, report and checklist are **derived**, not a substitute for code or a signed evidence record.
3. **Execution / method:** Exact input, operation order, platform, guard, failure behavior and output.
4. **Evidence and limitations:** Specific test filenames or observed run IDs; separate observed results, theoretical identities, untested hypotheses and known limits.
5. **Action:** A runnable PowerShell Core (latest stable version; `Set-StrictMode -Version 2.0`) command, a review decision, or an explicit next verification task.
6. **Provenance and status:** Revision date, source commit, reviewer, dissent/exception and a non-destructive history pointer.

Use meaningful headings, short paragraphs, active verbs, parallel constructions, and plain definitions. Use the minimum acronyms necessary and define each on first use. Put the essential conclusion first; reserve complete technical derivations for later sections.


## 2. TriDi critical-thinking gate: three directions, one accountable decision

**BLUF:** TriDi builds on the existing BiDi method without erasing it. Review each material claim along **three separately traceable directions**: YIN checks claims forward against evidence, YANG checks evidence backward against claims, and **AIRGAP** independently tests the assumptions, threat boundaries, negative controls, and provenance neither route can establish alone. **YAN** reconciles their findings and records a scoped decision; it is a synthesis, not a fourth vote or a superior source.

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
3. Establish one unambiguous BLUF; rewrite headings around behavior, evidence, limitation and next action.
4. Retain canonical formulae, exact result values, frozen experimental protocols, historical evidence and copyrights. A copyedit is not authorization to change them.
5. Link relative repository files; link external references without copying third-party protected passages.
6. Test claimed PowerShell Core (latest stable version; `Set-StrictMode -Version 2.0`) commands and targeted C++20 `-O2`/MSVC `/O2` behavior in CI when possible. Report exact job IDs and failures. Never claim a green build that has not run.
7. Compare with prior source, document contradictions and keep an audit entry with head SHA, reviewer, evidence and any dissent.
8. Close the register row only after a separate verification decision. Review is not sign-off; silence is not consent.

**Contributor model:** An author may propose a revision; a reviewer independently confirms current behavior; the maintainer chooses whether to merge. No person, office or external title silently assumes approval.

## 6. Writing rules

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
| C++ dialect | **ISO C++20 only**: `-std=c++20` or MSVC `/std:c++20`; CMake `CXX_STANDARD 20`, `CXX_STANDARD_REQUIRED ON`, `CXX_EXTENSIONS OFF`. | Other C++ standards, compiler-specific C++ dialect extensions or silent fallback to an older/newer standard |
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

**As inspected on 2026-11-19, several historical or currently configured CI paths conflict with the proposed profile.** Examples include non-Oracle Temurin/JDK 17 setup in `.github/workflows/ship.yml`, `android-standalone.yml`, `ligaze-v2-yin-yang.yml` and `ligaze-windows-header-optimizer-ab.yml`, plus Java build/execution in `supertaoism-gamp4-java-jar.yml`. Some workflows use Java without Gradle, which still requires a separate Oracle provenance and Java-usage determination. Android runtime/toolchain constraints must be reviewed independently.

The CORE YIN MSVC PGO workflow is **historical research evidence** and is not compliant with the newly requested `-O2`/`/O2`-only normative build. The repository also provides a C11 include-free API, which is **not a C++20 source file**; preserve that distinction until its role is explicitly decided.

**Review decision:** Mark all such workflows **POLICY CONFLICT / NEEDS REVIEW** in the documentation register. Do not silently delete tests, change frozen compiler or benchmark protocols, replace a Java distribution, relabel old CI successes, or claim the bans are technically enforced just because this policy is written. The follow-up enforcement task requires a separate, explicit code/CI plan, verification and maintainer decision. Existing provenance remains accessible.

### Audit acceptance criteria

A document or release may be labeled **POLICY-CONFORMANT** only after the TriDi gate (YIN, YANG and AIRGAP, reconciled by YAN) traces its claim to current C++20 compile flags, the PowerShell Core latest-stable/StrictMode 2.0 execution evidence, dependency/toolchain provenance, applicable license obligations and negative controls. A missing artifact, silent permission, or green job on an unreviewed toolchain cannot establish policy conformity.
