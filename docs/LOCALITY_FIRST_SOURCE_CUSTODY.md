# Locality-first source custody and deterministic builds

**CURRENT METHOD, 2026-11-20.** Scope: first-party code, third-party dependency records, licensed toolchains, security scans, release evidence and canonical inputs across this repository. Preferred execution: **GitHub-hosted toolchains only** for the current cloud effort. Local-first means **resolve verified, version-pinned, committed evidence before network lookups**, not that remote verification is always unnecessary or that network topology alone proves legality/security.

## HEAD -> use -> TAIL invariant

1. **HEAD:** resolve a source revision and canonical byte stream; verify exact identity, UTF-8 validity where applicable, source/license provenance, pin/digest, and named authorization. Preserve raw third-party license bytes without NFKC or MagicComb alteration. The human-approved license status is distinct from the vendor's actual license grant.
2. **USE:** evaluate from the verified local snapshot where technically feasible; no automatic lookup of a mutable global license registry or floating `latest` tag. New toolchain fetches and vulnerability-database updates are **explicit source-refresh transactions** on GitHub-hosted infrastructure, with exact source/digest validation and new custody records. Never claim cached scanning results are up to date merely because they are local.
3. **TAIL:** independently recalculate byte identities, record SHA-256 fingerprints and audit output; reconcile the files/components that actually shipped with their grants and the [registry](../LICENSES/README.md). A discrepancy, unknown field, forbidden license, unverified source, or failed check means HOLD. Maintain the old source as audit evidence; never silently relabel, update or delete it.

## Layered authority

- **Original project grant:** [root LICENSE](../LICENSE), derived directly from the pinned standard SPDX 0BSD template with only the copyright placeholders filled, not a separately authored license. The former PROJECT-0BSD snapshot remains in Git history, not active LICENSES custody.
- **Legal source records:** [pinned 25-text local cache](../LICENSES/LOCK.tsv), with original publisher commit and exact Git object hash.
- **Actual component grants:** not inferred from the cache. [Component usage register](../LICENSES/COMPONENT_USAGE.tsv) is a separate **derived record**, requiring a component/version/source hash and an original copyright/NOTICE or license expression, including AND/OR alternatives.
- **Operator admission policy:** [LICENSES index](../LICENSES/README.md) and [exception ledger](MAGICCOMB_LICENSE_SECURITY_EXCEPTION_LEDGER.md). Policy classification never edits third-party rights.
- **Computed evidence:** SHA-256 logs, SBOM and security reports. They derive from source bytes/algorithms; do not overrule authoritative source or convert a scan result into legal authorization.

## Reproducibility and refresh

- Store exact reference texts in `LICENSES/text/`; `.gitattributes` disables automatic EOL rewriting in this folder. The PowerShell [verifier](../ci/Assert-LocalLicenseRegistry.ps1) compares each file's exact Git blob digest with [LOCK.tsv](../LICENSES/LOCK.tsv), and can emit JSONL with independent SHA-256 fingerprints. This is an **integrity test**, not proof of the original upstream author's identity, license coverage, product rights, or a zero-vulnerability release.
- When a version/variant changes, add an explicitly sourced replacement snapshot and review its diff. Record when and why the new revision became preferred; prior history remains accessible. A source hash change without the required review fails closed.
- Keep forbidden texts in the reference cache for comparison and legal analysis. Presence in `LICENSES/` **does not authorize** inclusion in products or builds.
- Validate a project's *real* dependency graph and shipped file inventory against license policy; a complete license text alone does not prove which component it governs.
- Oracle JDK license terms depend on the exact product, distribution and accompanying third-party notices. The [Oracle NFTC reference](../LICENSES/text/Oracle-NFTC-reference.txt) is **not** a blanket license or proof of JDK27 redistribution rights.

## Network analogy, with limits

Like keeping trusted data near its consumers, local licensed evidence reduces repeated discovery, variability and dependency on remote lookups. Fibre Channel locality and fabric design can inspire the architecture, but they do **not** establish a universal rule that local beats distributed systems in performance, fault tolerance or regulatory obligations. Design choices need measurement and domain-specific evidence. Security advisories and vendor licenses may change upstream; deliberate refresh and reconciliation are mandatory before claiming current compliance.

**No mutation after experiment:** any normalization, rebase, source refresh or scoring-rule change becomes a separately identified, auditable new attempt, not a retroactive rewrite of an earlier result.
