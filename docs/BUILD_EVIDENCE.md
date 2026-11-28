<!--
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
-->

# Build and failure evidence: SHA-256, symbols, compiler provenance

> **Document status: DUE FOR UPDATE (2026-11-19).** Verify claims against code and tests; sharpen active verbs; check copyright, project-mark language, accessibility, and 0BSD scope. This mark is not a failure verdict. See [DOCUMENT_REVIEW_REGISTER.md](DOCUMENT_REVIEW_REGISTER.md) and [LICENSE_AND_MARKS.md](LICENSE_AND_MARKS.md).
>
> **Original contributions:** Copyright (c) 2026 Paul Allen Cheek · SPDX-License-Identifier: 0BSD. Third-party material and marks retain their respective rights; no registered mark is asserted.


**Purpose:** failures remain evidence, not policy. This document is an
experimental engineering record, not a statement of legal clearance.

## What every platform job archives

On **success or failure after checkout**, GitHub Actions attempts to upload
an `evidence-<platform>-<arch>` archive from the main build matrix and a
`swe-guard-<platform>` archive from the shared guard matrix. The contents
include, when the respective stages were reached:

- `configure.log`, `build.log`, `test.log` and `ctest-results.xml`;
- `manifest.json`: Git commit checked out, run ID, OS, architecture,
  outcomes, detected versions, installed compiler executable paths and
  SHA-256 hashes of compiler/tool executables actually located;
- `metadata/CMakeCXXCompiler.cmake` and `metadata/CMakeCache.txt`;
- `build-files/`: native build outputs including Windows `.pdb`,
  `.obj`, `.exe`, `.ilk` and Unix `.o`, executables and debug files
  **if produced** (failed compilation might have no linked PDB);
- `source-snapshot.zip`: tracked project sources, workflows, tests,
  documentation and 0BSD LICENSE for the exact checked-out Git commit;
- `SHA256SUMS`: SHA-256 for every archived file **before**
  `upload-artifact` wraps the files into a GitHub artifact.

The archived compiler *metadata* includes versions and file SHA-256
fingerprints where the original binary can be located on the runner.
The compiler binary itself is **not redistributed** by this pipeline.
Toolchain paths are machine-specific, not instructions to copy the software.
If a compiler cannot be located, its SHA-256 is unavailable and must not
be invented. A hash establishes file integrity against a known value but
does not by itself authenticate the publisher or create distribution rights.

Windows optimized builds use MSVC `/Zi` and linker `/DEBUG:FULL`. The
root `pentagonal_xor` executable PDB is also installed into the Windows
release package under `symbols/` when generated. Other local test PDBs
and partial objects live in the per-job evidence artifact. Unix builds
use `-g`; debuginfo can remain in the executable/object rather than a
separate `.pdb`/symbol bundle. Files that were not produced are reported
by their absence, not fabricated.

**Retention:** uploads request 90 days of GitHub Actions artifact
retention, subject to platform/organization limits. This is not an
immutable archive or perpetual hosting. Preserve external copies with
their hashes if longer retention is required.

## How to verify after download (PowerShell 7)

Inside the extracted evidence directory:

```powershell
Get-Content .\SHA256SUMS | ForEach-Object {
    if ($_ -match '^([0-9a-fA-F]{64})  (.+)$') {
        $expected = $Matches[1].ToLowerInvariant()
        $file = Join-Path (Get-Location) $Matches[2]
        $actual = (Get-FileHash -LiteralPath $file -Algorithm SHA256).Hash.ToLowerInvariant()
        if ($actual -ne $expected) { throw "SHA-256 mismatch: $file" }
    } else {
        throw "Malformed SHA256SUMS entry: $_"
    }
}
```

The GNU-compatible hash manifest covers individual evidence members,
not the zip wrapper that GitHub constructs *after* the manifest was made.
The release pipeline separately generates SHA256SUMS for its distributed
platform archives. Checksums do not substitute for signed provenance.

## Historical Windows MSVC failure: preserve the original

On November 15, 2026, the first COREAO Windows x64 and x86 jobs failed
at C++20 compilation on commit
`eb9e86774ab275bc9ba21f7bf7d41962ed5c3cc8`, while compiling
`tests/coreao_test.cpp`:

```text
shared/coreao.hpp(118,51): error C2131: expression did not evaluate to a constant
shared/coreao.hpp(119,52): error C2131: expression did not evaluate to a constant
shared/coreao.hpp(109,66): a non-constant (sub-)expression was encountered
```

- Failed workflow: https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/actions/runs/43763123775
- Failed Windows x64 job: https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/actions/runs/43763123775/job/132962155983
- Failed Windows x86 job: https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/actions/runs/43763123775/job/132962155999
- Corrective commit, preserving algebra: https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/commit/9ce8a094eeddf3b886530217912e8f0a96f17c05
- Successful seven-lane rerun: https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/actions/runs/43763299331

The first workflow did **not** upload this new evidence bundle, compiler
SHA-256 manifest or PDB collection. They cannot be reconstructed
retroactively from unavailable runner files. Its original GitHub job
logs and source commit remain the actual historical evidence. Subsequent
runs with this workflow capture additional verifiable data.

## Rights and license separation: researched authorities

**Project-owned source and outputs:** This project's `LICENSE` states
Zero-Clause BSD (`0BSD`). The OSI-published text permits use, copy,
modification and distribution of that software, including commercial use,
without attribution conditions. The grant covers the rights its licensor
has to license; it is not a blanket license from third-party authors.

**Third-party toolchains:** Microsoft Visual C++ tools, Apple SDKs,
system libraries, GitHub Actions packages and other software have their
own licenses. The pipeline therefore publishes compiler identifiers,
paths and SHA-256 hashes **without** copying compiler executables or
assigning them the project's 0BSD license. Actual inclusion of any
third-party component needs a separate documented rights review. The
project-generated PDBs and binaries are distinct from compiler
executables; distribution also requires checking whether bundled
third-party runtime code is present.

**U.S. Code references (Title 17):**
- [17 U.S.C. § 106](https://uscode.house.gov/view.xhtml?req=granuleid:USC-prelim-title17-section106)
  reserves copyright owners' rights to reproduce and distribute their works.
- [17 U.S.C. § 109](https://uscode.house.gov/view.xhtml?req=granuleid:USC-prelim-title17-section109)
  concerns disposition of particular lawfully made copies; it does not
  confer general permission to make new copies of software.
- [17 U.S.C. § 117](https://uscode.house.gov/view.xhtml?req=granuleid:USC-prelim-title17-section117)
  contains limited permissions for certain computer-program copies or
  adaptations; it is not a general toolchain redistribution license.

**Cases and scope (not decisions about this repository or 0BSD):**
- *Jacobsen v. Katzer*, 535 F.3d 1373 (Fed. Cir. 2008), on enforceability
  of conditions in an **Artistic License**, not an 0BSD license:
  [Federal Circuit opinion](https://www.cafc.uscourts.gov/8-13-2008-2008-1001-jacobsen-v-katzer-08-1001/).
  [Google Scholar search](https://scholar.google.com/scholar?q=Jacobsen+v.+Katzer+535+F.3d+1373).
- *Artifex Software, Inc. v. Hancom, Inc.*, No. 16-cv-06982-JSC,
  N.D. Cal. Apr. 25, 2017, order denying a motion to dismiss involving
  the **GPL/Ghostscript**, not an 0BSD project:
  [U.S. Courts primary docket](https://www.govinfo.gov/app/details/USCOURTS-cand-3_16-cv-06982).
  [Google Scholar search](https://scholar.google.com/scholar?q=Artifex+Software+Inc.+v.+Hancom+Inc.).
- *MDY Industries, LLC v. Blizzard Entertainment, Inc.*, 629 F.3d 928
  (9th Cir. 2010), on the distinction between copyright-license
  conditions and contractual covenants:
  [Ninth Circuit opinion on govinfo](https://www.govinfo.gov/app/details/USCOURTS-ca9-09-16044/USCOURTS-ca9-09-16044-0).
  [Google Scholar search](https://scholar.google.com/scholar?q=MDY+Industries+v.+Blizzard+Entertainment+629+F.3d+928).

These authorities provide context for license boundaries, not an
opinion on whether any particular redistributable has legal clearance.
A legal review should inspect the actual compiler/runtime
redistribution terms before shipping any third-party executables.

## Failure handling policy

A red result must not become a green result by removing checks,
suppressing a diagnostic, rewriting the source commit or deleting
failure evidence. A future correction receives a separate commit and
its own full manifest. The algorithm and source record remain
authoritative; each CI run is an auditable, dated observation.
