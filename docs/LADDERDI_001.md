# LadderDi 001 | native and local interpreter qualification

**Status: IMPLEMENTED SOURCES, QUALIFICATION PER LANE; release HOLD.** Existing five-lane PentaDi, PowerShell/C# HexaDi and script-only R* HenDecaDi remain independent.

## Requirements

| Lane | Language | Self-contained source | Version and provenance | Qualification |
|---|---|---|---|---|
| HeptaDi | Lua3L | heptadi/HeptaDiLua3L.lua | Lua 3.2.2 separately installed, historical unsupported interpreter | No remote imports; all declared program variables local; exact interpreter and execution required |
| OctaDi | Python 2/3 | octadi/G.py | Python 2.7.18 and separately identified Python 3 executable | G table 2 binary orders and 6 ternary orders, 255 laps each, negative canaries and exact output parity |
| NonaDi | Node/npm | nonadi/Invoke-NonaDiSelfExclude.ps1 | SELF_EXCLUDE | No Node execution; active Node/npm files and known Node-backed Actions remain HOLD |
| EnneaDi | Rust native | enneadi/EnneaDi.rs | Rust 1.100.0 from verified local install | rustc directly, -C opt-level=2, no Cargo or registry; 256-byte and masking negative controls |
| DecaDi | Go native | decadi/DecaDi.go | Go 1.27.4, official Google-hosted archive | Offline Google archive SHA256 verification; direct go build with GOPROXY=off, GOSUMDB=off, GOTOOLCHAIN=local, CGO_ENABLED=0 |
| UnDecaDi | direct x64 MASM | undecadi/UnDecaDi.asm + UnDecaDiTest.cpp | Microsoft ml64/cl, separately provisioned | ml64 /c plus MSVC /O2 ISO C++20 native witness, 256-byte and OR-negative tests |
| DoDecaDi | Perl | dodecadi/DoDecaDi.pl | Perl 5.44.0 separately installed | Perl interpreter with -T, no modules, canonical bitwise and inverse witnesses |
| TriDecaDi | OCaml | tridecadi/TriDecaDi.ml | OCaml 5.5.2 separately installed | ocamlopt -O2 native compilation and in-process witness |

**No language runtime is bundled into the project.** External interpreters/compilers and their licenses remain under their original publishers, separate from our 0BSD source. The normal offline execution harness accepts pre-provisioned local executable paths through designated environment variables, records computed SHA-256, and may never download missing toolchains. In particular, neither Python3 nor a Python23 compatibility algorithm can impersonate Python 2.7.18.

Google Go archive reference: https://dl.google.com/go/go1.27.4.windows-amd64.zip. The published exact archive SHA256 for this experiment is `178ab98950ac69ab25069307dc6ade79f3816e414cce95b4da166a115cde1e7e`. It is an **artifact digest** checked against upstream release metadata, not a replacement for original license notices or real source lineage. The bundled Go binary is NOT committed; the lab must provision and verify the archive separately. No other download source is approved for the Go lane.

## How the evidence runs

[ci/Invoke-LadderDiQualification.ps1](../ci/Invoke-LadderDiQualification.ps1) checks all lanes with exact local sources and writes `ladderdi-qualification.tsv`, plus per-lane stdout/stderr receipts into a **fresh external evidence directory**. Source digests and native executables are recomputed. Each unavailable interpreter, compiler, invalid exact version, missing source, output disagreement, or failed negative control results in **HOLD**, never PASS. An overall success requires every active lane to qualify.

[.github/workflows/LADDERDI.yml](../.github/workflows/LADDERDI.yml) uses PowerShell to fetch **only the exact GitHub source revision** for a Windows CI comparison, before running the offline qualification. This CI source transport is not itself an air-gapped laboratory and does not download/approve a toolchain. Preflight installed tools are never assumed to be the required versions. The workflow does not merge, release or remove any prior checks.

### OctaDi whitespace is a LAST-RESORT diagnostic

[octadi/Invoke-OctaDiWhitespaceMatrix.ps1](../octadi/Invoke-OctaDiWhitespaceMatrix.ps1) requires an explicit `-DiagnoseIndentation` flag. It writes separate 4-space, 5-space, 8-space and all-tab fixtures outside the Git repository; original canonical bytes do not change. Python 2 and Python 3 both expand tabs to next multiples of eight columns, not a fixed five spaces, regardless of an editor's display width. A whitespace candidate is valid only after BOTH exact local interpreters accept it, all 8 G-table layouts produce identical stdout, and the canonical data and source remain unchanged. A missing interpreter is not an indentation failure. Any comparison-protocol change requires a new experiment ID, not mutation of previous results.

### Scope and provenance

This source is project-authored 0BSD. Historical Lua 3 and Python 2 runtimes are unsupported: a functional pass is not a claim of current security maintenance. MASM, Rust, Go, Perl, OCaml and Python distributions may have distinct upstream licenses. Their local vendor notices, checksums and component-level grants must be examined independently before redistribution. Attributions are evidence, not blanket authorization.

NonaDi **self-excludes Node/npm**. Legacy JS/TS or Node-backed workflow files are flagged, not deleted or silently reclassified. Source availability does not imply a verified native runtime. All existing PR-PARITY, EXCITE, INCEPTION and release checks remain fail-closed, and protected main is untouched.
