# Interpreter custody and offline headstones (METHOD)

**CURRENT method, November 24, 2026.** Scope: active interpreted scripts in the Git checkout, including PowerShell, shell, R, Lua, Python, Perl and other script extensions. This is an audit rule, not a retroactive claim of zero-network conformity. Preserve historical failed source and Git path history.

## The boundary: portable source != bundled runtime

The project contains its own scripts, not third-party interpreters or packages. A supervised **pre-laboratory provisioning** phase may obtain an official release from its original publisher, examine the original license notices, independently verify exact archive bytes and record a digest. That phase is separate from air-gapped **execution**, which must make **zero network requests, remote procedure calls, remote includes, dynamic dependency resolution, package installs or telemetry**. A script needing external imports or runtime loading is HOLD until rewritten as self-contained and independently qualified. Laboratory network isolation must be enforced separately; static source inspection alone cannot prove it.

Never automatically import a module, download a compiler, or copy proprietary/GPL/interpreter binaries into the 0BSD source tree. Permission to *use* R in a laboratory is not permission to redistribute R or relicense its original software as 0BSD. Original 0BSD scripts do not automatically become GPL merely by being executed by R.

The canonical credit register is [ci/INTERPRETER_HEADSTONES.tsv](../ci/INTERPRETER_HEADSTONES.tsv). It records the external **runtime or plugin name, original author/publisher, version, upstream project, official download reference, license expression, independently verified archive SHA-256 or pending status, provisioning status**. R Foundation labels R as a package **GPL-2 | GPL-3** (https://www.r-project.org/Licenses/). The descriptive modern SPDX alternatives are **GPL-2.0-only OR GPL-3.0-only**. Do not substitute **GPL-2.0-or-later**, which allows later versions not explicitly established by the package-level label; individual R components can have other licenses.

## Every interpreted source: a derived headstone

Run [ci/Invoke-InterpreterCustodyAudit.ps1](../ci/Invoke-InterpreterCustodyAudit.ps1) offline under PowerShell 7+. It reads but does not execute the inspected files and emits a HEADSTONE per active script: relative path, computed SHA-256 witness, Git path-commit history as source of authorship, language, original runtime publisher/version, official source/download, license identifier, external archive digest or pending state, script's own SPDX tag, static hazards, and PASS_STATIC_ONLY or HOLD.

Active files are audited separately from Archive/Retired. Git history and original notices are authoritative: a ledger cannot invent individual authors, override the upstream license, or grant executable toolchain permission. **PASS_STATIC_ONLY is not a proof of license clearance, no-network execution, or release readiness.** Existing scripts that fetch externally, dot-source, import, or have ambiguous grants remain HOLD / Needs Review until repaired, not deleted or silently grandfathered.

[HenDecaDi/R*](../hendecadi/HenDecaDi.R) is original 0BSD R script source only. R 4.6.2 must be separately installed and licensed by the laboratory; its script does not fetch, install, import or bundle R. The 255 laps and logical ATMOSECONDS are separate from measured elapsed seconds and full live validation.

## HEAD / USE / TAIL

HEAD: pin exact local source bytes, original notices, named author or explicit UNKNOWN, original publisher, version, official download, external package digest or HOLD. Git history beats the derived ledger.

USE: run in a genuinely isolated laboratory process with no network, remote includes, RPC, telemetry, dynamic module loading or package acquisition. Built-in language operations may be used; no runtime download.

TAIL: recalculate source digests and retain the receipt, all failures and every version's lineage. Failure never authorizes a workaround. No automatic merge/release.

New interpreter versions or plugin dependencies require an explicit new source/credit entry and separate preflight. Native C++, C11, MASM, Rust and Go have different build gates. Node/npm remains SELF_EXCLUDE. Python .py2 and .py3 remain separate with exact runtime verification; a shared .py compatibility source cannot silently substitute interpreters. Lua 3 and Python 2 are unsupported legacy and cannot be claimed security-current.

The baseline audit intentionally exposes any incompatible historical scripts. Do not relax existing PR-PARITY, license, human approval or other release gates merely to produce a green indicator.
