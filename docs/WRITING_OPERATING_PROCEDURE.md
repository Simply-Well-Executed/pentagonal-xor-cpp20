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


## 2. BiDi critical-thinking gate: forward claims and reverse evidence

**BLUF:** Check every material claim in both directions. **YIN, the forward pass**, tests whether the claim leads to valid evidence. **YANG, the reverse pass**, starts at the evidence and reconstructs the narrowest claim that it actually supports. **YAN, the BiDi reconciliation**, compares both trails and records disagreement. A complete, traceable two-way review is necessary for a VERIFIED decision, but it is not itself a proof of authenticity.

BiDi here names a **bidirectional reasoning protocol**, not a Unicode text-direction command. Reviewing the same source twice is not independent corroboration, and the passes must not merge separate records or silently change original data.

### YIN: claim → source → method → evidence → boundary

| Step | Question | Required evidence |
| --- | --- | --- |
| F1. Clarify | What exact claim, term, namespace, units, audience and intended decision are involved? | Unchanged claim text and claim ID |
| F2. Attribute | Is its authority the algorithm, original input, test, official publication, observation, hypothesis or presentation? | Canonical source path/edition, date and SHA where applicable |
| F3. Reconstruct | Can the claimed transformation be repeated using its frozen inputs, ordered stages, precision, guard conditions and platform? | Actual results, method or explicit NOT RUN |
| F4. Challenge | What negative control, alternative cause, counterexample, stale artifact or bias might disprove it? | Contrary evidence, dissent and unresolved cases |
| F5. Bound | Does the evidence have adequate **trust, accuracy, precision, relevance and sufficiency** for the entire claim? | Narrowed permissible statement, with limits |

### YANG: evidence → method → source → permissible claim → challenge

| Step | Question | Required evidence |
| --- | --- | --- |
| R1. Inspect | Starting from the retained artifact rather than the author's conclusion, what was actually observed? | Independent reading of log, test, data, source record or publication |
| R2. Reconstruct | What exact procedure produced the evidence, on what inputs and at what commit/environment? | Measured route and recorded assumptions |
| R3. Trace | What original source and transformations account for it? Does any ledger disagree with the algorithm or primary record? | Provenance chain; algorithm and original input outrank derived ledgers |
| R4. Limit | Which **smallest** claim is warranted? Which broader interpretation is not warranted? | Permissible claim and explicit exclusions |
| R5. Challenge | Which negative controls, contradictions, alternatives and untested cases remain? | Counterevidence and uncertainty |

### YAN: compare forward ↔ reverse; record the decision

| Check | Required agreement | On mismatch |
| --- | --- | --- |
| Identity | Same namespaced claim, definition, source version and inputs | NEEDS REVIEW; never equate merely similar records |
| Trace | Source → route → witness → ledger ↔ ledger → witness → route → source reconcile | Repair derived ledger with audit, not original algorithm |
| Scope | Reverse permissible claim covers *all* of forward assertion | Narrow or retract unsupported scope |
| Quantity | Units, precision, sample count, bounds, environment and result agree | Recompute; record exact conflict |
| Counterexample | Negative tests and adverse evidence are actually examined | Do not call an incomplete review PASS |
| Immutability | Frozen protocols and original input remain unchanged | Stop comparison; changes require new ID and explicit provenance |

**Decision record:**
- **BIDI_PASS, eligible for VERIFIED / CURRENT:** Both traces are complete, reconcilable and appropriately evidenced; a named reviewer explicitly approves. PASS does not assume approval.
- **BIDI_NEEDS_REVIEW:** A path is stale, absent, untested, ambiguous or unsupported. Leave the existing status and unresolved issues visible.
- **BIDI_CONTRADICTED:** The reverse trace refutes or materially narrows the forward claim. Correct the derived text, retain the original version and record the reason.
- **REWRITTEN / VERIFY:** Documentation text changed but independent verification remains incomplete.

**Minimum ledger fields:** claim ID and namespace; source statement; source revision and SHA; forward operations; evidence IDs; reverse reconstructed operations; reverse permissible claim; units/precision/environment; positive and negative tests; dissent and mismatch; disposition; reviewer/date; resulting commit. A missing reviewer, silent stakeholder, failed renderer, unavailable tool or skipped test is **never** a YES or evidence of deletion.

**Demonstration A, OCI:** An `ENGINE_RESPONDING` result from `docker info` establishes that the daemon responded. It **does not** establish that Windows container builds ran. When `host_container_build_executed=false`, the reverse pass rejects the forward statement "Windows containers were validated." Report only observed capability.

**Demonstration B, counting physics:** An exact population variance of 150 for three synthetic counts warrants a statement about those counts, not a claim of observed quantum-mechanical dispersion. A green CTest run verifies that test on its commit and runner, not general physical validity.

**Separate Unicode direction protocol:** Do not let the word BiDi change text provenance. When writing genuine RTL/LTR passages, identify the semantic owner of each span; preserve exact Unicode code points, normalization, and source-order positions. LRI/RLI/FSI open an isolate and PDI closes it, subject to balanced nesting. Explicit newlines bound the project display protocol; a soft wrap does not. Renderer failure is not data failure.

**Critical-thinking provenance:** The supplied *Tongue and Quill*, AFH 33-337, PDF pages 53-54, distinguishes fact from inference and addresses trust, accuracy, precision, relevance and sufficiency of evidence. The **YIN/YANG/YAN BiDi process is a project-defined adaptation**, not an Air Force-issued procedure.


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
| C++ dialect | **ISO C++20 only**: \`-std=c++20\` or MSVC \`/std:c++20\`; CMake \`CXX_STANDARD 20\`, \`CXX_STANDARD_REQUIRED ON\`, \`CXX_EXTENSIONS OFF\`. | Other C++ standards, compiler-specific C++ dialect extensions or silent fallback to an older/newer standard |
| Optimizer | **\`-O2\` only** for GNU/Clang-family compilers; **\`/O2\` only** for MSVC, as its platform spelling of the same requested optimization level. Warning/hardening options may be separately evaluated if they do not change this optimization mandate. | Alternative optimization levels; PGO instrumentation or profile use; LTO/whole-program optimization and additional speed/size/architecture-tuning options (for example \`-O3\`, \`-Ofast\`, \`/GL\`, \`/LTCG:PGOptimize\`, \`/fp:fast\`, or \`/arch:AVX2\`) as part of the normative build |
| Shell | **PowerShell Core, latest stable version**. Start each script and every sample with **\`Set-StrictMode -Version 2.0\`** and capture version/exit status in test evidence. | Other scripting shells or an unqualified PowerShell invocation treated as policy-compliant |
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

**As inspected on 2026-11-19, several historical or currently configured CI paths conflict with the proposed profile.** Examples include non-Oracle Temurin/JDK 17 setup in \`.github/workflows/ship.yml\`, \`android-standalone.yml\`, \`ligaze-v2-yin-yang.yml\` and \`ligaze-windows-header-optimizer-ab.yml\`, plus Java build/execution in \`supertaoism-gamp4-java-jar.yml\`. Some workflows use Java without Gradle, which still requires a separate Oracle provenance and Java-usage determination. Android runtime/toolchain constraints must be reviewed independently.

The CORE YIN MSVC PGO workflow is **historical research evidence** and is not compliant with the newly requested \`-O2\`/\`/O2\`-only normative build. The repository also provides a C11 include-free API, which is **not a C++20 source file**; preserve that distinction until its role is explicitly decided.

**Review decision:** Mark all such workflows **POLICY CONFLICT / NEEDS REVIEW** in the documentation register. Do not silently delete tests, change frozen compiler or benchmark protocols, replace a Java distribution, relabel old CI successes, or claim the bans are technically enforced just because this policy is written. The follow-up enforcement task requires a separate, explicit code/CI plan, verification and maintainer decision. Existing provenance remains accessible.

### Audit acceptance criteria

A document or release may be labeled **POLICY-CONFORMANT** only after the BiDi gate traces its claim to current C++20 compile flags, the PowerShell Core latest-stable/StrictMode 2.0 execution evidence, dependency/toolchain provenance, applicable license obligations and negative controls. A missing artifact, silent permission, or green job on an unreviewed toolchain cannot establish policy conformity.
