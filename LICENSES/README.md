# Local-first licensing reference and evidence registry

**Current policy (2026-11-20):** Preferred: `0BSD`, `BSD-2-Clause`, `BSD-3-Clause`, `MIT`, `LGPL-2.1-only`, and official Oracle Java *under its exact vendor license terms and separate redistribution review*. Forbidden: `LGPL-3.0`, GPL, AGPL, EUPL, EPL and other unapproved GNU licenses. Unknown, mixed, or version-ambiguous grants: **HOLD / REVIEW**.

## Identity and custody

- [`LOCK.tsv`](LOCK.tsv) is the local, machine-readable index. Each verbatim license reference in [`text/`](text/) is identified by its exact file path, license ID or variant, policy status, publisher's repository, immutable source commit, upstream path, and **Git blob SHA-1**. Git source commit and blob identity, not display names or live redirects, determine the recorded bytes. A PowerShell verifier additionally reports calculated SHA-256 for each file.
- The source of 24 SPDX reference texts is [SPDX License List Data v3.29.0](https://github.com/spdx/license-list-data/releases/tag/v3.29.0), specifically commit `31ba1a50e5397e00a304dbadc76531740e89ee48`. These copies are unmodified legal reference texts, not repository-authored new grants. Preserve their exact bytes and line endings; never run NFKC cleanup, whitespace formatting, or text substitutions on them.
- **Canonical first-party license:** [root `LICENSE`](../LICENSE) now uses the unmodified standard 0BSD permission and warranty terms from [`text/0BSD.txt`](text/0BSD.txt), substituting only the usual author/year/email placeholders with `Copyright (C) 2026 by PAUL ALLEN CHEEK mail@pcheek.com`. No additional or project-specific license conditions are added. The previously separate `PROJECT-0BSD.txt` snapshot was retired from CURRENT in the license-baseline update and remains recoverable from Git history; it is not a second license authority. The exact standard reference template under `text/0BSD.txt` remains byte-for-byte pinned to SPDX.
- [`text/Oracle-NFTC-reference.txt`](text/Oracle-NFTC-reference.txt) is a verbatim reference preserved from Oracle's [public `oracle/tribuo-site` repository](https://github.com/oracle/tribuo-site/blob/4548f7e2c326c4284adbbd57bfbeca65897fbbc4/learn/4.0/javadoc/legal/LICENSE), commit `4548f7e2c326c4284adbbd57bfbeca65897fbbc4`. **Oracle is a vendor, not a SPDX license identifier.** This is an identified Oracle NFTC reference copy; it does not prove that a different Oracle product/version, or an Oracle JDK 27 distribution, carries precisely these terms. Verify the actual product's bundled notices/terms and its redistribution rights separately before shipment.
- The [`COMPONENT_USAGE.tsv`](COMPONENT_USAGE.tsv) register is intentionally **empty except for its schema** until specific components, versions, actual notices, source hashes, AND/OR grants and scopes are verified. A saved license text is not proof any component is licensed under it.

## Prevent the 'this BSD-3-Clause versus that BSD-3-Clause' dispute

Exact `BSD-3-Clause` is **PREFERRED**. Separately named `BSD-3-Clause-Clear`, `BSD-3-Clause-Attribution` and `BSD-3-Clause-LBNL` have independent verbatim copies marked **REVIEW**, not silently interchangeable. Likewise `MIT` is preferred while `MIT-0` is separately marked REVIEW. The `LGPL-2.1-only` and `LGPL-2.1-or-later` copies happen to have the **same Git blob**; differences of the actual component's copyright/license *grant* must be resolved through original notices rather than pretending these reference texts differ. Forbidden variants remain present for evidence, not adoption.

## No-network normal path and explicit update boundary

1. On GitHub-hosted review/build, use the committed `LICENSES/` copies first; do **not** fetch a latest license registry at normal build or verification time. Run `pwsh -NoLogo -NoProfile -File ci/Assert-LocalLicenseRegistry.ps1` to verify all locked blobs byte-for-byte and output SHA-256 evidence without changing them.
2. On finding a new source term or conflict, create a **new, named** local snapshot and an evidence row. Pin the upstream source version, compare with the old text, retain the old record in history and mark Needs Review. Do not silently overwrite, reclassify or normalize legal text.
3. A successful local integrity check proves only the bytes recorded in `LOCK.tsv` are intact, **not** source authenticity beyond the cited publisher commit, license enforceability, supply-chain security, vendor redistribution permission, or image release approval. CVE, SBOM, component-specific legal, and QA gates remain independent.

## Interpretation and organization

[UNKNOWN](UNKNOWN.md), [MIXED](MIXED.md), and [UNAPPROVED-GNU](UNAPPROVED-GNU.md) are **policy records**, not fictitious license texts. The registry intentionally includes prohibited reference terms for dispute resolution. It does **not** relicense any upstream or first-party material.

Locality is a deliberate reproducibility and availability preference for this project, not a universal performance guarantee or a technical claim that Fibre Channel fabrics and license repositories behave identically. Remote verification may still be required at separately approved source-refresh boundaries; normal verification is offline.
