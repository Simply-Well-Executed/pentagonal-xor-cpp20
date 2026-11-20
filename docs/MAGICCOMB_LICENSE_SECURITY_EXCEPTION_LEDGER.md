# MagicComb HEAD/TAIL: License and CVE exception ledger

> **Release status: HOLD.** This ledger is a custody and triage record, not a license grant, production approval, or an instruction to delete historical images. All identities are source-bound. Every exception is **UNAPPROVED** until explicit operator review.

## Strict user-defined admission policy

1. **Prohibited by default:** GNU GPL, LGPL, AGPL and Eclipse Public License (EPL); Eclipse licensing is a separate family, not a GNU-derived license. A forbidden or mixed-license component cannot be silently reclassified as 0BSD, BSD, permissive or harmless.
2. **Unknown licenses:** also HOLD; missing or unparseable attribution is **not consent**.
3. **Explicitly requested Oracle Java:** Oracle Java 27.0.1.1 is licensed under [Oracle No-Fee Terms and Conditions](https://www.java.com/freeuselicense), **not 0BSD**. Use as the chosen official vendor for the experimental build toolchain only; shipping or repackaging requires an exact license and third-party-attribution review.
4. **Zero vulnerabilities desired:** treat every currently reported CVE as a finding requiring triage, repair or technically justified source-matched verification. A low/medium finding is not silently accepted. Scanner failure, unknowns or missing coverage prevent a zero-CVE claim.
5. All probing uses **GitHub-hosted** Ubuntu runners, exact Git commit, Docker registry digests, SHA256-verified upstream artifacts, no author-created Node/npm/Python pipeline, and no user-local or self-hosted compute. Build/test exceptions do not imply production distribution consent.
6. MagicComb **HEAD** protects source and artifact identity before input evaluation/extraction. **TAIL** revalidates post-test image/reports and prevents promotion if metadata, sources, licenses or CVEs do not satisfy policy. Canonical digest is not rewritten by display formatting or normalization.

## Pinned official version candidates (2026-11-20)

| Component | Pinned release | Official evidence | Scope |
| --- | --- | --- | --- |
| PowerShell | **7.7.0** | [Signed GitHub GA release](https://github.com/PowerShell/PowerShell/releases/tag/v7.7.0); `powershell-7.7.0-linux-x64.tar.gz` SHA256 `c89d9c682d2c403dfe726f3df948e359675235a5bd6fab535776de69729ca0cb` | Cloud toolchain, not yet released |
| .NET SDK | **11.0.100** / runtime **11.0.0** | Microsoft `mcr.microsoft.com/dotnet/sdk:11.0` resolved to registry manifest `sha256:65f3c7c6a25a239ab3e6719f2896147e7b68242ff2d8818dd70498bde6ef8bea` | Cloud toolchain only |
| Oracle Java | **27.0.1.1** | [Oracle official Java 27 download](https://www.oracle.com/java/technologies/downloads/); `jdk-27_linux-x64_bin.tar.gz` SHA256 `50d42ab5bacc735aa9b4da9c451f631a4b5c09e00b961df412a26d159c538cfc` | Official Oracle toolchain; separate NFTC and third-party license custody |
| Docker Scout | **1.29.0** | [Docker Scout official release](https://github.com/docker/scout-cli/releases/tag/v1.29.0); asset SHA256 `9157cdc9372b478a8cd51dd53f5e9c74a7e34056ea291c65520bf76016fa72f4` | Temporary scanner installed only on GitHub worker |

Oracle JDK is used to compile/test Java sources in the **unpublished** experimental image. The final production container must not silently include OpenJDK or an unreviewed Oracle JDK/runtime; separately determine if the final application needs `java`, and review any Oracle redistribution constraints before publishing a new Java-containing artifact.

## Evidence ledger

| ID | Source / observation | Current status |
| --- | --- | --- |
| IMG-HISTORIC | Docker Hub `paulcheek/android-source-study-v1:latest` digest `sha256:5791dcef8beb3704f58346e79af0ec5313f0599c2320b204939cf29aeda6ad30` | Historical: unchanged, not Python-free claimed |
| IMG-PYFREE | `paulcheek/android-source-study-v1:python-free-candidate-ed28e0544042` digest `sha256:cf8ea2216382fd030d36caeb8092d28506c1bd74128cfb0f8beedc303aa24b62` | Published candidate, **not zero-CVE or 0BSD-only certified** |
| BASE-CVE | Authenticated GitHub-only [MagicComb run #44764136095](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/actions/runs/44764136095), exact IMG-PYFREE digest: **3 critical, 89 high, 204 medium, 162 low**, 1,051 indexed packages. Docker Scout also flagged **509 copyleft package records** and missing attestations. | **FAIL, HOLD**. This fully reconciles the Docker Hub dashboard figures, not a zero-vulnerability result |
| REBASE-VERSION | [Cloud run #44763512579](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/actions/runs/44763512579) built official pinned version artifacts, reporting .NET SDK 11.0.100, PowerShell 7.7.0, Java and Javac 27.0.1.1; no-Python check passed | Toolchain assembled. Test invocation initially stopped by shell quoting; not release approval |
| REBASE-TEST | [Cloud run #44763943778](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/actions/runs/44763943778) then passed **45/45 native C++20 CTests**, **26,654 Java 17-compatibility checks** via Oracle Java 27, and Python-free runtime check | Functional test **PASS**, release **HOLD** |
| REBASE-SCOUT | Authenticated GitHub-only [MagicComb run #44764136095](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/actions/runs/44764136095) inspected the unpublished .NET11 / PowerShell7.7 / Oracle Java27 toolchain image; **0 critical, 0 high, 1 medium, 0 low**, 1,014 indexed packages. Detected `CVE-2026-97054` in a Go standard-library component. Docker Scout flagged **563 copyleft records**, a default-root-image warning, and missing attestations. A new unpublished Dockerfile changes to uid/gid `65534:65534`; revalidation is required. | **1 unresolved medium and unapproved copyleft, HOLD**. Not a zero-CVE image |
| REBASE-LICENSE | Same cloud rebase run detected **71 installed package records for prohibited-license review**, plus **103 records with missing/unclear attribution** in conservative Debian metadata scan. A subsequent audit normalizes Debian `:amd64` package suffixes to reduce false missing-document paths. | **71 provisional flags / 103 provisional unknown** before normalization; not a proof every flagged package is exclusively GNU-licensed; HOLD on complete, independent license review |

### Named exception claims requiring operator determination

| Exception request | Problem and evidence | Proposed custody decision | Approval |
| --- | --- | --- | --- |
| `EXC-GNU-LINUX-001` | Base GNU/Linux image and developer-toolchain packages, including `bash`, `coreutils`, `gcc`, `g++`, `git`, and likely `libc` runtime, have GPL/LGPL or mixed-license attribution. Their presence conflicts with an absolute zero-GNU policy, even if first-party project source is 0BSD. | Prefer redesign into a minimal separable runtime and non-distributed toolchain. If any unavoidable GNU component must ship, ask the user for a **named package/version/license exception**, never quorum or implicit approval. | **UNAPPROVED** |
| `EXC-ORACLE-NFTC-001` | Official Oracle JDK 27.0.1.1 has **NFTC** and bundled third-party notices. Oracle Java is a requested exception to 0BSD-only wording, but presence/redistribution and permitted modifications still require review. | Build-only vendor use is exploratory. Shipping Oracle runtimes only after explicit license inventory and source-matched consent. | **UNAPPROVED FOR DISTRIBUTION** |
| `EXC-UNKNOWN-LICENSE-001` | 103 package records lacked attributed documentation in the conservative scan; architecture-qualified package names may contribute to missing file-path lookups. | Correct metadata path normalization, produce full SPDX-like inventory, then review remaining unknowns. | **UNAPPROVED** |
| `EXC-CVE-2026-97054` | One medium Go-stdlib CVE in unpublished refreshed toolchain image. | Locate exact binary/package provenance; replace or rebuild from a fixed upstream version, rescan. Not a waiver request for release. | **OPEN, MUST REMEDIATE** |
| `EXC-DOCKER-SCOUT-BASELINE` | Previously published candidate has verified **3 critical CVEs**; severity/source mapping must be preserved. | Replace vulnerable underlying OS/runtime dependencies in a **new** image and rescan. Never erase the old image or reclassify historical vulnerability evidence. | **OPEN, MUST REMEDIATE** |

## Why no new image was pushed

An authenticated build can function correctly while violating licensing or security admission rules. The new Oracle Java/.NET/PowerShell image is only a GitHub-hosted **probe**. The chosen rule 'failure never becomes policy' means the failed security/license gates remain blocked and visible. No `latest` overwrite, GHCR candidate publication, Docker Hub candidate publication, GitHub release, or production Deployment is authorized by this branch.

## CVE-2026-97054 exact executable provenance and scoped remediation

GitHub-hosted [run #44766220131](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/actions/runs/44766220131) verified that the inherited Linux/amd64 Go-based executable is `/usr/bin/pebble`, exactly **10,584,224 bytes** with SHA256 `5e9859b77a7ed5c4421f5c43435cc3822d5090d0f4ced422adade068e3b974e3`. `dpkg-query -S /usr/bin/pebble` returned **not owned by a Debian package**. Docker Scout independently attributed the medium finding to this file, `pkg:golang/stdlib@1.27.3`, fixed in Go 1.27.4. This is not an Oracle Java or PowerShell file.

Experimental GitHub commit [`c134f7b8ef9d`](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/commit/c134f7b8ef9def72f93bd70c39a9b99583e89d82) proposes a narrowly scoped **removal only from the newly built, unpublished trial image**, not from history or the Microsoft original base: require existing non-symlink file, **exact SHA256 match**, and still-unowned `dpkg-query` state before removing it, with explicit `IRON|MAGICCOMB|PEBBLE|REMOVED` audit. A changed upstream binary or package owner makes the build fail. Full CTest, Java and Docker Scout validation are still required to establish that removing the unused binary is safe and actually eliminates the CVE.

**No release permitted even if this CVE reaches zero:** the GNU/EPL policy remains blocked pending source-matched licensing of distributable files, and Oracle NFTC review remains separate. Any CVE reduction is evidence, not permission.
## Next audit requirements

Re-run a complete package/SBOM license scanner with normalized architecture suffixes; capture explicit license identifiers and third-party legal bundles from Microsoft .NET, PowerShell and Oracle Java; identify which files actually ship in a proposed runtime versus remain in disposable build infrastructure. Re-scan with the same Docker Scout release and exact platform/digest. Require zero applicable and unsuppressed CVEs plus no GNU/EPL packages or documented operator-approved exceptions before any new release.
