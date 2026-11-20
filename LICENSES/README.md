# Local-first licensing reference and evidence registry

**CURRENT zero-GNU/POSIX decision (2026-11-20, superseding earlier LGPL permission):** Preferred license candidates: **`0BSD` is the ONLY approved BSD-family license**, plus separately approved `MIT`, `MPL-2.0` and `CDDL-1.0`; official Oracle Java remains subject to its exact vendor and bundled third-party terms. **No GNU license, including LGPL-2.1-only/or-later, GNU toolchain or GNU runtime, or authored POSIX shell/dependent execution is approved** in the project, including build and development. Unknown and mixed terms are **HOLD / REVIEW, never inferred consent**. Retain all **33** current reference texts even when forbidden.

## Identity and custody

- [`LOCK.tsv`](LOCK.tsv) is the local, machine-readable index. Each verbatim license reference in [`text/`](text/) is identified by its exact file path, license ID or variant, policy status, publisher's repository, immutable source commit, upstream path, and **Git blob SHA-1**. Git source commit and blob identity, not display names or live redirects, determine the recorded bytes. A PowerShell verifier additionally reports calculated SHA-256 for each file.
- The source of 32 SPDX reference texts is [SPDX License List Data v3.29.0](https://github.com/spdx/license-list-data/releases/tag/v3.29.0), specifically commit `31ba1a50e5397e00a304dbadc76531740e89ee48`. These copies are unmodified legal reference texts, not repository-authored new grants. Preserve their exact bytes and line endings; never run NFKC cleanup, whitespace formatting, or text substitutions on them.
- **Canonical first-party license:** [root `LICENSE`](../LICENSE) now uses the unmodified standard 0BSD permission and warranty terms from [`text/0BSD.txt`](text/0BSD.txt), substituting the copyright template line with the operator-specified notice; the trailing C5824 is treated as part of that notice, not an extra licensing condition with `Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824`. No additional or project-specific license conditions are added. The previously separate `PROJECT-0BSD.txt` snapshot was retired from CURRENT in the license-baseline update and remains recoverable from Git history; it is not a second license authority. The exact standard reference template under `text/0BSD.txt` remains byte-for-byte pinned to SPDX.
- [`text/Oracle-NFTC-reference.txt`](text/Oracle-NFTC-reference.txt) is a verbatim reference preserved from Oracle's [public `oracle/tribuo-site` repository](https://github.com/oracle/tribuo-site/blob/4548f7e2c326c4284adbbd57bfbeca65897fbbc4/learn/4.0/javadoc/legal/LICENSE), commit `4548f7e2c326c4284adbbd57bfbeca65897fbbc4`. **Oracle is a vendor, not a SPDX license identifier.** This is an identified Oracle NFTC reference copy; it does not prove that a different Oracle product/version, or an Oracle JDK 27 distribution, carries precisely these terms. Verify the actual product's bundled notices/terms and its redistribution rights separately before shipment.
- The [`COMPONENT_USAGE.tsv`](COMPONENT_USAGE.tsv) register is intentionally **empty except for its schema** until specific components, versions, actual notices, source hashes, AND/OR grants and scopes are verified. A saved license text is not proof any component is licensed under it.

## Prevent the 'this BSD-3-Clause versus that BSD-3-Clause' dispute

**Only 0BSD is approved in the BSD family.** `BSD-2-Clause`, `BSD-3-Clause` and the separately named BSD-3-Clause-Clear/Attribution/LBNL variants are all **FORBIDDEN** as current policy, while their exact reference texts stay in the archive. Identical-sounding licenses are never equivalent by assumption. Likewise `MIT` is preferred while `MIT-0` is separately marked REVIEW. The `LGPL-2.1-only` and `LGPL-2.1-or-later` copies happen to have the **same Git blob**; differences of the actual component's copyright/license *grant* must be resolved through original notices rather than pretending these reference texts differ. Forbidden variants remain present for evidence, not adoption.

## No-network normal path and explicit update boundary

1. On GitHub-hosted review/build, use the committed `LICENSES/` copies first; do **not** fetch a latest license registry at normal build or verification time. Run `pwsh -NoLogo -NoProfile -File ci/Assert-LocalLicenseRegistry.ps1` to verify all locked blobs byte-for-byte and output SHA-256 evidence without changing them.
2. On finding a new source term or conflict, create a **new, named** local snapshot and an evidence row. Pin the upstream source version, compare with the old text, retain the old record in history and mark Needs Review. Do not silently overwrite, reclassify or normalize legal text.
3. A successful local integrity check proves only the bytes recorded in `LOCK.tsv` are intact, **not** source authenticity beyond the cited publisher commit, license enforceability, supply-chain security, vendor redistribution permission, or image release approval. CVE, SBOM, component-specific legal, and QA gates remain independent.

## Interpretation and organization

[UNKNOWN](UNKNOWN.md), [MIXED](MIXED.md), and [UNAPPROVED-GNU](UNAPPROVED-GNU.md) are **policy records**, not fictitious license texts. The registry intentionally includes prohibited reference terms for dispute resolution. It does **not** relicense any upstream or first-party material.

Locality is a deliberate reproducibility and availability preference for this project, not a universal performance guarantee or a technical claim that Fibre Channel fabrics and license repositories behave identically. Remote verification may still be required at separately approved source-refresh boundaries; normal verification is offline.

## Newer decision and provenance

The previous `LGPL-2.1-only` Preferred classification and `LGPL-2.1-or-later` review-to-choose pathway are **superseded** by the user's explicit zero-GNU and zero-POSIX instruction. Their reference text bytes, source hashes, filenames and historical decisions remain in Git; both current lock rows are **FORBIDDEN**. See [zero-approval migration](../docs/GNU_POSIX_ZERO_APPROVAL_POLICY.md). Inclusion in this registry never authorizes execution, copying into a release, or a build-only exception. Non-GNU weak-copyleft terms such as MPL-2.0 and CDDL-1.0 are **not automatically approved** and require a source-specific review.

## MPL / CDDL / NPL version-family survey (2026-11-20)

Pinned SPDX v3.29.0 has `MPL-1.0`, `MPL-1.1`, `MPL-2.0`, `MPL-2.0-no-copyleft-exception`, `CDDL-1.0`, `CDDL-1.1`, `NPL-1.0`, and `NPL-1.1`. **Only the exact `MPL-2.0` and `CDDL-1.0` identifiers are PREFERRED**; the other six require separate REVIEW. The pinned SPDX source tree does not list `MPL-0.0`, `MPL-0.1`, `MPL-2.3` or `CDDL-2.0`; this is not proof no custom/historical unindexed documents exist. Distinct terms, provenance, notices and the actual grant control each decision.

## Complete pinned SPDX discovery index (826 text records)

[SPDX-V3.29.0-CATALOG.tsv](SPDX-V3.29.0-CATALOG.tsv) records **all 826 text entries** in the exact upstream SPDX v3.29.0 `text/` Git tree, with each upstream blob SHA, source commit, identifier, byte count, current admission classification, and explicit `VENDORED_EXACT` or `INDEX_ONLY_NOT_VENDORED` custody status. At this revision, **32 SPDX texts plus one Oracle NFTC reference are copied verbatim into this repository**. The other **794 SPDX texts are indexed but not yet copied**, and must not be presented as locally preserved license text. The index includes license exceptions and deprecated IDs, and is not a claim to know every privately authored license that ever existed. Names not explicitly approved are REVIEW/HOLD or FORBIDDEN by current operator family policy, never silently permitted.

Some GNU- or BSD-related identifiers are flagged as FORBIDDEN through a **conservative identifier-family policy filter**, not by interpreting copyright grants. Component-specific evidence and actual license notices remain authoritative. An identifier is not a complete dependency license analysis.
