# Pondaz | Unicode intake validation at the CI task-queue origin

> **Status:** SOURCE IMPLEMENTED / CI VALIDATION IN PROGRESS · PONDAZ-UNICODE-PREFLIGHT-v1 · 0BSD for original project material, copyright (c) 2026 Paul Allen Cheek.

## Completed publication action

**Validate GitHub Actions input before downstream jobs run.** Pondaz uses the GitHub-hosted PowerShell Core runtime and .NET strict UTF-8 decoding with Unicode `NormalizationForm.FormKC` (NFKC). It creates a **normalized, filtered comparison view without changing the repository's original bytes**. Invalid UTF-8, U+200B..U+200D, U+FEFF, and disallowed ASCII controls block the task queue. NFKC-altering workflow YAML is rejected; other compatibility differences are recorded for explicit review rather than silently changing mathematical, multilingual or copyrighted text.

The earlier PHP sample is a *behavioral reference*, not a deployed PHP dependency. The native implementation and complete positive/negative fixture suite reside in [`ci/Invoke-Pondaz.ps1`](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/blob/main/ci/Invoke-Pondaz.ps1). GitHub Actions workflows put the Pondaz preflight job first and declare `needs: pondaz` for every downstream lane. A standalone [Pondaz workflow](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/blob/main/.github/workflows/pondaz.yml) supplies an additional explicit named check.

## What Pondaz checks

| Input | Gate | Evidence |
| --- | --- | --- |
| All **Git-tracked textual files** with supported source/config/documentation extensions | Strict UTF-8 decoder, NFKC comparison, zero-width/FEFF and disallowed control detection | Original Git commit ID, input path, validity, issues and SHA-256 of shadow text |
| All raw bytes in the **GitHub Actions event JSON** | Strict UTF-8 decoder and same Unicode checks | Raw event record |
| Parsed JSON **string values and object keys**, including escaped Unicode | Same checks applied after JSON unescaping | `github.event.value[n]` or key record |
| GitHub workflow YAML | NFKC changes additionally **block**: normalize only for inspection, never modify workflow instruction text | `NONCANONICAL_WORKFLOW_NFKC` |
| Other legitimate NFKC differences in project prose, mathematical symbols and experimental text | Record a **warning**, preserving original semantic identity | `nfkc_changed=true`; no substituted source |
| Binary payloads, downloaded packages, network responses and later runtime inputs | **OUT OF SCOPE** of this initial CI gate | Separate source/provenance and runtime validation required |

Pondaz refuses tracked symbolic links for text inputs, uses NUL-delimited Git path enumeration, and checks the source commit against GitHub's immutable job SHA before running. It does not invent output that would imply original text was normalized and accepted by later stages.

### Input classes and falsification

The built-in 11 fixtures test ordinary ASCII, Greek `π`, compatibility-folding full-width `Ａ`, the three prohibited zero-width code points, U+FEFF, a prohibited control, RTL isolate boundaries, malformed UTF-8 and NFKC-sensitive workflow YAML.

- **Invalid decoding**: `INVALID_UTF8` blocks.
- **Prohibited invisible**: `INVISIBLE_U200B_U200D_OR_FEFF` blocks, including U+200D used in some emoji grapheme sequences. An exception requires an explicit, versioned policy revision and independent review. No character is deleted from the source.
- **Prohibited C0/control**: `CONTROL_CHARACTER` blocks.
- **NFKC normalization failure**: `NFKC_FAILURE` blocks.
- **NFKC-different workflow**: `NONCANONICAL_WORKFLOW_NFKC` blocks.
- **NFKC-different prose or mathematical text**: record a compatibility difference; no replacement or automatic evidence merger.
- **Accepted text**: preserve exact original code points, input bytes and source identity.

### Scientific, security and governance boundary

Unicode values without a font glyph are **not, by themselves, evidence of memory corruption**. Font fallback and missing glyph rendering are different from invalid UTF-8 decoding, parser bugs, bidi display deception, and memory-safety vulnerabilities. Pondaz reduces selected input risks but does not prove arbitrary C++20 memory safety, font correctness, package provenance, nor cryptographic authenticity.

NFKC is a compatibility transform; it may fold characters that matter to source identifiers, mathematical notation, legal names, text direction or archives. The original bytes and author-defined semantic spans remain authoritative; Pondaz never promotes a normalized shadow to canonical identity. U+2066–U+2069 bidirectional isolates remain valid inputs and should instead be assessed under the author's separate balanced-isolate display protocol.

A failing gate **does not change history or implementation policy**. It provides evidence and blocks CI tasks whose prerequisites were not met. The reviewer decides on any legal exception or remediation; missing evidence is not PASS, and an unsupported character is not automatically illegal.

## Reproduction with PowerShell Core (latest stable version)

Every script and example sets StrictMode 2.0. These commands require the repository checkout and `pwsh`:

~~~powershell
Set-StrictMode -Version 2.0
$ErrorActionPreference = 'Stop'
$PSVersionTable.PSVersion
& ./ci/Invoke-Pondaz.ps1 -SelfTest
if (-not $?) { throw 'Pondaz fixture gate failed' }
& ./ci/Invoke-Pondaz.ps1 -RepositoryRoot . -EvidenceDir pondaz-evidence
if (-not $?) { throw 'Pondaz input gate failed' }
~~~

**Evidence:** `pondaz-evidence/pondaz.json` records inspected paths, differences, blocked findings, the GitHub source SHA and PowerShell Core version. A green *standalone* Pondaz check establishes only its tested source/event scan, not that every package, PR workflow or runtime input has passed the gate.

**Intellectual property and use:** The algorithm uses the runtime's built-in .NET Unicode normalization library. It adds no copied PHP code, third-party binary, Java distribution, Python, Node.js, Lua, Go, Rust, Gradle or npm dependency. Existing CI lanes retain their separately recorded toolchain policy conflicts pending an explicitly approved migration.

## Audit and publication review

The source run, the downstream task dependency and this page must each be checked at the same PR head before promotion to VERIFIED / CURRENT. Logs and historical exceptions remain linked to their originating SHA, not folded into later results. Any change to classification, normalization, blocked code points or exclusion rules requires a new reviewed policy revision.
