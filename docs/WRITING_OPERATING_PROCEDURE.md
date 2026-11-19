# Documentation operating procedure | PENTAGONAL XOR

**Document ID:** DOC-OP-2026-11-19 | **Scope:** repository prose, diagrams, policy, software reference, validation and release instructions | **State:** CURRENT editorial procedure, pending maintainer adoption

**BLUF:** Say what a component does, identify the evidence that supports it, explain what it does *not* establish, and tell the reader exactly how to reproduce or review the result. An editorial revision never changes the underlying algorithm or the historical record.

**Owner and provenance:** Simply Well Executed project documentation. Original project writing copyright (c) 2026 Paul Allen Cheek, SPDX-License-Identifier: 0BSD where applicable. Third-party references are linked, not relicensed or copied into this repository.

## 1. Purpose and commander's intent

This is a project editorial standard, **not** a Department of the Air Force instruction, official correspondence, military performance statement, or regulatory authorization. Apply the practical discipline of concise military writing to C++20 technical records without claiming that external military policies govern or certify this software.

**Required order for a newly written page:**
1. **BLUF and scope:** Audience, decision, tested behavior, current project role.
2. **Authority and source:** Identify the canonical source file or immutable input. A README, diagram, report and checklist are **derived**, not a substitute for code or a signed evidence record.
3. **Execution / method:** Exact input, operation order, platform, guard, failure behavior and output.
4. **Evidence and limitations:** Specific test filenames or observed run IDs; separate observed results, theoretical identities, untested hypotheses and known limits.
5. **Action:** A runnable PowerShell 7 command, a review decision, or an explicit next verification task.
6. **Provenance and status:** Revision date, source commit, reviewer, dissent/exception and a non-destructive history pointer.

Use meaningful headings, short paragraphs, active verbs, parallel constructions, and plain definitions. Use the minimum acronyms necessary and define each on first use. Put the essential conclusion first; reserve complete technical derivations for later sections.

## 2. Critical-thinking gate (CLAIM → EVIDENCE → CHALLENGE → DECISION)

| Gate | Required question | Required record |
| --- | --- | --- |
| Clarify | What exactly is being claimed? Is the term defined in this repository? | Exact assertion and project namespace |
| Source | Is this from executable code, official publication, observation, sample, inference, or metaphor? | Canonical path, source SHA and reference date |
| Challenge | What alternative explanation or negative control could falsify it? | Counterexample, boundary case, dissent |
| Quantify | Are units, precision, error bounds and sample counts honest? | Inputs, formula, environment and reported scope |
| Decide | Does the current evidence establish this claim? | VERIFIED, REWRITTEN / VERIFY, NEEDS REVIEW, or CONTRADICTED |
| Preserve | Did editing inadvertently erase an old condition or change an established definition? | Diff, provenance and unchanged historical record |

Do not use an unverified benchmark to generalize speed; a successful CI run only confirms the tested commands at that specific SHA. A diagnostic checksum is not cryptographic authentication. A synthetic photon-count example does not validate quantum mechanics. A source file's stated license cannot grant rights in someone else's images, music, code or marks.

## 3. Editorial tone inspired by the supplied references

- **AFH 33-337, The Tongue and Quill:** Use an audience-first purpose, concise BLUF, active voice, clear action and reviewable supporting detail. This is an *editorial adaptation*, not a claim of verbatim compliance.
- **Airman Leadership Qualities Writing Guide, 7 Apr 2025:** For engineering accomplishment statements, use a factual *action → measurable outcome → operational significance* structure. Do not invent a metric or turn a hypothetical result into an award claim.
- **DAFI 36-2803, Military Decorations and Awards Program:** Keep award-like accomplishments, impact statements and supporting records distinct from evidence of program certification. Do not imply eligibility or formal military recognition.
- **AF Form 716:** Treat form-based reporting as a reminder to keep fields explicit and traceable. Do not fabricate fields, official signatures, or approval status.
- **DAFI 36-2907, Adverse Administrative Actions:** For failures, record the allegation/observation, evidence, response, finding and correction separately. Do not call a failed experiment a permanent policy; do not equate a CI warning with misconduct or a regulatory determination.
- **Six Sigma Development Solutions Institute (DSI) critical-thinking guide:** Examine source reliability, alternative perspectives, assumptions and bias before drawing conclusions. Revise claims when contrary evidence appears.

**Reference access qualification (checked 2026-11-19):** The supplied AFH/DAFI/Form PDF endpoints rejected automated retrieval (HTTP 403); the ALQ myFSS page exposed no readable public text. Therefore, the present procedure uses only their user-supplied identification and general writing principles, **not verified quotations or unseen detailed rule clauses**. The last supplied DAFI 36-2907 URL ends in \`.pd\`; resolve its exact official target before issuing a verified citation. The DSI critical-thinking page was publicly readable. A future authorized reviewer should open official sources, log the revision/date and update this qualification before asserting document-specific conformity.

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
6. Test claimed PowerShell commands and targeted C++20 \`-O2\`/MSVC \`/O2\` behavior in CI when possible. Report exact job IDs and failures. Never claim a green build that has not run.
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

Use explicit code-path namespaces and PowerShell 7 blocks; do not silently convert the user's tooling into Python or JavaScript automation. Never substitute 16-digit floating-point display precision for full exact \`toolong\` integer serialization.

## 7. Canonical reading list

User-supplied references. Links are informational; military publications retain the issuing authority's rights.

- [AFH 33-337, The Tongue and Quill](https://static.e-publishing.af.mil/production/1/administrative_assistant/publication/afh33-337/afh33-337.pdf) (PDF access unverified due HTTP 403).
- [ALQ Writing Guide, 7 Apr 2025](https://myfss.us.af.mil/USAFCommunity/s/knowledge-detail?pid=kA0t0000000wkx6CAA) (authenticated myFSS content unavailable for verification).
- [DAFI 36-2803, Military Decorations and Awards Program](https://static.e-publishing.af.mil/production/1/af_a1/publication/dafi36-2803/dafi36-2803.pdf) (PDF access unverified due HTTP 403).
- [AF Form 716](https://static.e-publishing.af.mil/production/1/af_a1/form/af716/af716.pdf) (PDF access unverified due HTTP 403).
- DAFI 36-2907, *Adverse Administrative Actions*. User-provided URL: \`https://static.e-publishing.af.mil/production/1/af_a1/publication/dafi36-2907/dafi36-2907.pd\` (truncated suffix; verify actual official URL before linking it as a PDF).
- [Six Sigma DSI, Critical Thinking](https://sixsigmadsi.com/glossary/critical-thinking/) (publicly checked 2026-11-19; used as conceptual lens, not copied).

## 8. Review register

See [DOCUMENT_REVIEW_REGISTER.md](DOCUMENT_REVIEW_REGISTER.md). An editorial status of **REWRITTEN / VERIFY** reports a changed presentation only. It does **not** assert that source and all CI claims were independently retested. An explicit sign-off is required to promote an item to **VERIFIED**.
