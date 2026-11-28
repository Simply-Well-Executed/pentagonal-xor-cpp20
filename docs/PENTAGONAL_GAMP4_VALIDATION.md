<!--
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

Scope: original first-party contributions only; existing third-party rights, source media exclusions and other reservations remain authoritative.
-->

# PENTAGONAL-COR: GAMP 4-style supplier verification plan

> **Document status: DUE FOR UPDATE (2026-11-19).** Verify claims against code and tests; sharpen active verbs; check copyright, project-mark language, accessibility, and 0BSD scope. This mark is not a failure verdict. See [DOCUMENT_REVIEW_REGISTER.md](DOCUMENT_REVIEW_REGISTER.md) and [LICENSE_AND_MARKS.md](LICENSE_AND_MARKS.md).
>
> **Original contributions:** Copyright (c) 2026 Paul Allen Cheek · SPDX-License-Identifier: 0BSD. Third-party material and marks retain their respective rights; no registered mark is asserted.


**Status:** EXPLORATORY / supplier-side verification protocol, 2026-11-16.
**Not:** formal GAMP certification, validated clinical use, FDA approval,
an installed system PQ, a compliant electronic-signature application,
or an official Air Force manual. This plan is a novel project-specific
matrix inspired by historical **ISPE GAMP 4** validation practices,
not a copied ISPE publication.

## 1. System identity and intended use

**System boundary:** native C++20 Pentagonal Boolean and continuous
score adapter, experimental COR 64-bit controller, VISTA read-only
report and FLOP state transition. Primary purpose is five-ligand
mathematical research and non-destructive integrity diagnostics,
not regulated manufacturing or patient-care decision support.

**Software category:** locally developed application-specific code
requiring justified testing (GAMP-era "custom" category in common
practice), with third-party toolchain/CI components. Actual GxP
criticality depends on a regulated user's documented intended use;
it cannot be inherited from repository naming or successful CTest.

**Authority:** original source and its exact landmark/measurement
definition, if any, outrank derived ledger, renderer and reports.
An altered source must be reported rather than silently overwritten.

## 2. Lifecycle V-model correspondence

| Lifecycle evidence | Current deliverable | Status |
|---|---|---|
| User Requirements (URS) | URS-11 through URS-22 below | Defined |
| Functional Specification (FS) | shared/native pentagonal_gates.hpp | Implemented |
| Design and source custody | Existing canonical scorer and submodule policy; CMake and Git SHA | Recorded |
| Risk assessment | Section 3, failures and residual risks | Preliminary, not approved QA assessment |
| Installation Qualification (IQ) | Native compile, toolchain logs, exact Git SHA and installed header test | Automated subset, environment-specific |
| Operational Qualification (OQ) | 12 executable URS checks + existing CTest suites | Automated on connected CI runners |
| Performance Qualification (PQ) | Real device/production workload, user acceptance, operator training | OPEN / not performed |
| Release authorization | Independent change control and accountable QA sign-off | OPEN / not implied by green CI |
| Retirement/archival | Git history, immutable external evidence retention and audit trail policy | Operational procedure OPEN |

Do not make a claim of full GAMP 4 validation based on automated
supplier tests alone. Formal documentation, approved SOPs,
risk-appropriate access control, system-specific PQ and independent
review must be supplied by the regulated operator.

## 3. Risk-based URS-to-test traceability

Pass criterion for each of the twelve checks: the test produces
exactly one \`PASS|URS-NN|...\` line, the process exits 0, and no
\`FAIL|\` lines appear. Test executable outputs an overall
\`GAMP4_STYLE_SUMMARY|PASS|cases=12|failures=0\` witness.
Any failure causes CTest and the separate evidence harness to fail.

| URS ID | Hazard / requirement | Risk | Test or expected evidence |
|---|---|---|---|
| URS-11 | Incorrect Boolean gate grants false authorization | High | All 32 XOR/OR/AND states checked against independent set-bit oracle and original score functions |
| URS-12 | Invalid numeric ligands alter existing output | High | Finite boundaries 0, 1, 0.5, NaN, infinity, values outside [0,1]; prior output unchanged on rejected input |
| URS-13 | Faulty 64-bit pack/unpack or selector | High | Exact 2/30/16/16 widths, mask truncation, all four entries, 96/64/32/0 logical counts |
| URS-14 | Changed bit goes undetected or destroys original | High | All 30 single-bit changes, mapped to six five-ligand block rupture flags; observed value retained |
| URS-15 | Replay, untrusted clock, missing witness silently pass | High | Independent missing, replay, backward clock and invalid-entry bits; combined flags |
| URS-16 | Self-comparison mistaken for authentication | High | Same altered vector in both inputs can match; missing external witness explicitly alerted |
| URS-17 | NOP unexpectedly changes data | Medium | True/false output unchanged |
| URS-18 | Extra XOR after FLOP or wrong ordering | High | Three exact six-step loops: XOR OR AND NOP COR FLOP, return only to next XOR entry |
| URS-19 | Telemetry mutates witness or drops gate state | High | Full read-only VISTA snapshot equality including all six blocks |
| URS-20 | Logical sweeps misreported as physical clock cycles | Medium | COR 96/64/32 logical counts, bypass tested |
| URS-21 | Redundant depth-2/3 parity silently changes results | Medium | Exact 32-state depth equivalence; no independent observation/speed claims |
| URS-22 | Block order reverses, endpoints omitted | High | Six independent blocks tested at both first and last positions |

**Requirement custody:** a changed URS, test, or reference implementation
creates a new Git commit and fresh evidence. A prior report remains
historical evidence, not a mutable approval.

## 4. Execution and acceptance

At each selected commit:
1. Checkout exactly the source commit and (where applicable) pinned
   unmodified Pentagonal XOR submodule.
2. Record compiler, platform, CMake/CTest configuration, library/source
   hashes and test logs.
3. Configure and build all C++20/C11 tests with CMake.
4. Run full CTest plus the dedicated GAMP 4-style URS executable.
5. Capture every \`PASS|URS-..\` row and the summary line; reject missing,
   duplicate or failed URS records and unexpected nonzero exit.
6. Record the source file SHA-256 hashes and program output in an
   inspectable report; retain the original GitHub Actions run/log and
   commit pointer even on failure.
7. Require an independent human to decide whether IQ/OQ/PQ and GxP
   predicate-rule obligations are satisfied for their real application.

The validation helper does **not** edit the source datum, repair bytes
or claim a digest is an authenticated root of trust. An adversary
controlling the test, source and evidence host can falsify a local
witness; externally anchored attestations and signed artifacts are
not implemented. Sequence freshness booleans and clock flags are
**supplied signals**, not a hardened monotonic counter.

PowerShell 7 reproducibility, after CMake build and CTest:

~~~powershell
pwsh -NoLogo -NoProfile -ExecutionPolicy Bypass -File ./ci/Invoke-PentagonalGamp4.ps1 -BuildDir build -EvidenceDir evidence-gamp4 -ExecutableName pentagonal_gamp4_validation_test
~~~

For the downstream DIAMOND build use \`build-native\` and the
\`pentagonal_gamp4_integration_test\` executable.

## 5. Deviations, environmental limits and risk disposition

- All 12 automated supplier OQ checks passing does not establish
  authorized release to clinical manufacturing or healthcare.
- A published SHA-256 hash alone cannot prevent a malicious editor from
  changing both the record and hash. Store independently authenticated
  evidence with protected access and retention where required.
- True microcontroller timing, power and WFI/idle states require
  instrumented hardware testing. COR's 96/64/32 values are *logical
  truth-table evaluations*, not measured clock cycles.
- The controller operates with Boolean ligand blocks; its default
  \`expected\` is not a valid independent witness if simply copied from
  the observed value. Missing witness flag must not be ignored.
- Validation error rate for 30 injected bit flips may be reported as
  **0 observed misses / 30 injected faults** if tests pass. This is
  *not* a population-level security error rate; it supplies no
  confidence bound against unseen attacks.
- Approval and deviation handling need human ownership and a linked
  change-control process. This software does not generate
  attributable electronic signatures, access controls, trusted clocks,
  secure audit trails or authenticated electronic records.

## 6. Source guidance, scope and regulatory distinction

Historical GAMP 4 expanded guidance from automated GMP into all GxP
around 2001. ISPE's current GAMP 5 Second Edition (July 2022)
focuses on risk-based, lifecycle-appropriate assurance. This work
implements a **GAMP 4-style study** because requested, with current
risk-based caution, without claiming to be an ISPE-endorsed program.

- ISPE history: https://ispe.org/pharmaceutical-engineering/january-february-2025/celebrating-25-years-gampr-americas
- ISPE current guide: https://ispe.org/publications/guidance-documents/gamp-5-guide-2nd-edition/
- FDA 21 CFR 11.10(a), (e), (f), (k):
  https://www.ecfr.gov/current/title-21/part-11/section-11.10
- FDA Part 11 Scope and Application guidance:
  https://www.fda.gov/regulatory-information/search-fda-guidance-documents/part-11-electronic-records-electronic-signatures-scope-and-application

In particular, FDA recommends a justified, documented risk
assessment appropriate to intended use and predicate requirements.
This test protocol is **not** a substitute for those controls.

**No source mutation during validation. Failure evidence remains
visible; failure never becomes permission to rewrite the datum.**

### Original repository implementation scope
The source authority is `shared/pentagonal_xor_std_free.h`;
the extension is `shared/pentagonal_gates.hpp`.
The existing broad `docs/GAMP4_VALIDATION.md` remains the primary
whole-repository matrix; this new plan adds operator URS-11..22.
CTest target: `pentagonal_gamp4_validation_test`.
The OR/XOR/AND fast scorer and existing CLI are not changed.
