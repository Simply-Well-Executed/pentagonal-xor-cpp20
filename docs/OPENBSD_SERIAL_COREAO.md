# Strictly serial OpenBSD 8 → 2 compatibility pipeline, COREAO

**EXPLORATORY, SOURCE CONTRACT + WINDOWS NATIVE PORTABILITY PREFLIGHT. Native OpenBSD binary builds: NOT EXECUTED. GNU/POSIX release HOLD remains.**

## Frozen serial stage ordering

| # | Target ID | OpenBSD release | Pass | CORE |
|---:|---|---|---|---|
| 1 | `OPENBSD8` | 8.0 | NONE | NONE |
| 2 | `OPENBSD7` | 7.9 | NONE | NONE |
| 3 | `OPENBSD6` | 6.9 | NONE | NONE |
| 4 | `OPENBSD5` | 5.9 | NONE | NONE |
| 5 | `OPENBSD4_FIRST` | 4.9 | FIRST | NONE |
| 6 | `OPENBSD3` | 3.9 | NONE | NONE |
| 7 | `OPENBSD4_RETURN` | 4.9 | RETURN | NONE |
| 8 | `OPENBSD2_YIN` | 2.9 | YIN | OPENBSD2 |
| 9 | `OPENBSD2_YANG` | 2.9 | YANG | OPENBSD2 |
| 10 | `COREAO_OPENBSD2` | 2.9 | COREAO | OPENBSD2 |

The final target is **COREAO_OPENBSD2**, explicitly deriving from the independent OpenBSD 2.9 **YIN** and **YANG** witnesses. CORE remains **OpenBSD2** and is not discarded, replaced, flattened or renamed. The repeated 4.9 stages remain separate visits, and the sequence has nine OpenBSD passes plus one COREAO reconciliation, executed in a single ordered PowerShell loop with a chained SHA-256 witness.

## Two distinct levels of execution

1. `ci/Invoke-OpenBSDSerialCoreao.ps1` checks the canonical `ci/OPENBSD_SERIAL_COREAO.tsv` stage order, source-lock presence, distinct YIN/YANG identities and CORE. It emits deterministic ten-line JSONL, predecessor SHA-256 and explicit HOLD status. `ci/Test-OpenBSDSerialCoreao.ps1` repeats it at MagicComb HEAD+TAIL, checks exact output equality and tries a malformed stage manifest that MUST fail. No stage claims to have built an OpenBSD binary.
2. `.github/workflows/openbsd-serial-coreao.yml` is one Windows 2025 job with sequential ordered steps, no matrix and no parallel build. After validating the serial ledger, it compiles the native C++20 **portable STD-free core on Windows** using Visual Studio 2026 and `/O2` with `--parallel 1`. The resulting artifact is a Windows build-validation sample, **not** an OpenBSD executable, OpenBSD runtime compatibility test, or legacy supercomputer support proof.

## Compatibility and numerical precision

OpenBSD 8.0 is an official current release, and historical 7.9, 6.9, 5.9, 4.9, 3.9 and 2.9 have distinct Kernel, architecture, ABI, compiler and hardware compatibility constraints. Historical 2–5-era toolchains do not directly supply C++20. Feature flags may be disabled explicitly per native target after analysis, but **we do not silently reduce integer width, floating-point precision, decimal scale or witness magnitude**. OS version alone does not imply hardware precision, memory size or numeric range. `PRESERVE_SOURCE_PRECISION` and `hardware_abi=UNATTESTED` remain recorded until hardware-specific evidence exists.

## Trust and security

Only the OpenBSD 8.0 fixed release source metadata is pinned in `ci/OPENBSD_80_SOURCE_LOCK.tsv`. The 8.0 patched -stable tree and releases 2 through 7 have no authenticated source+binary provenance in this new compatibility ledger; all stay HOLD. Neither unspecified world supercomputers nor remote OpenAI endpoints are assumed available, controlled, or directly connected. POSIX shell and GNU toolchains remain unapproved. No `sh`, `bash`, GNU compiler, Docker/containers, package manager, or unapproved native OpenBSD build is invoked in this pipeline.

To reach a genuine multi-release build pipeline, a future explicitly reviewed protocol needs real pinned source snapshots, matching architecture-specific native OpenBSD hosts or independently attested cross-toolchains, support for the code's language level, per-target `-O2` functional tests, complete source/binary licensing and security records, and approval for any newly required tools. This is deliberately a **fail-closed preflight**, not a fabricated OpenBSD build.

**Chosen, never assumed. Failure never becomes policy.**
