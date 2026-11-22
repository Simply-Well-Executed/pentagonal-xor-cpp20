# GNU / POSIX zero-approval migration protocol

**Migration update, 2026-11-22 (PR #34):** The 17 retired `.sh` wrappers have been removed from active source. The [versioned discarded-files registry](../discardedfiles.md) records each original and wrapper Git blob SHA; exact original text remains archival evidence. The current fixed transcript is [native PowerShell](../ci/FRESH-GammaGamma.ps1), preserving every historical v8 output line and nonzero exit 42 / **RELEASE=HOLD**. The GNU-dependent, unpublished probe Dockerfile is retained only as [archived text](../Archive/Retired/docker/MagicComb-Oracle27-DotNet11-PowerShell77.Probe.Dockerfile.txt), with its Git blob identity in the registry. The four deprecated publishing/build workflows remain disabled. This migration does not approve GNU components, inherited toolchains, third-party licenses, existing images, publication or merge; those independent holds persist. Earlier statements below about active wrappers or the probe describe the historical migration state.

**CURRENT, 2026-11-20, operator-directed**. This policy applies to project-authored source, scripts, CI jobs, build containers and runtime images in this repository. The operator additionally states the same expectation for other organizational projects, but this repository and its CI **cannot enforce changes outside their scope**. Current policy has **no GNU or POSIX component exceptions**, including GPL, LGPL 2.1/3.0, AGPL, GFDL, GCC/g++, GNU make, GNU grep/gawk, and authored POSIX shell execution.

## License versus interface

GNU software licenses are specific copyright grants, not an inference from syntax such as `$EXAMPLE`. POSIX is an interface specification, not a GNU license or an intrinsic copyright claim. Both are forbidden *as project policy* when the owner says so. Non-GNU weak copyleft families (such as MPL-2.0 and CDDL-1.0) are distinct from LGPL but **not implicitly preferred**; third-party obligations and dual-licensing choices require case-by-case operator review.

## Inventory and exception custody

- Preserve the earlier explicit LGPL-2.1 approval in Git history as **SUPERSEDED**, not deleted or treated as CURRENT. Keep the exact forbidden reference texts in `LICENSES/text/`; mark current `LICENSES/LOCK.tsv` rows FORBIDDEN.
- The tracked `.sh` files are **historical migration evidence** and not approved executable paths. No workflow may execute them under the current policy. Inspect CMake, Dockerfiles, build caches and source dependencies for hard-coded `/bin/sh`, bash, GNU utilities, inherited libc and other POSIX/GNU runtime dependencies before claiming compliance.
- Keep the existing Linux/GNU/Docker MagicComb probe **disabled**. Its earlier failures remain evidence, and it must not run to request an exception that has not been granted. The PowerShell-based Windows workflow verifies local license custody independently and returns HOLD when active legacy scripts remain.
- For each component, retain package name, exact version, immutable upstream revision, actual SPDX/license notice, usage scope, build and runtime provenance, hash, and review status. A license registry text is not evidence that the component actually carries that license.
- Never rewrite legal texts with NFKC, rename a GNU license to a non-GNU one, hide a dependency, claim that a user-space shell migration eliminates every POSIX API, or reinterpret a failed gate as PASS. Do not move to production, publish or merge solely on the registry's success.

## Acceptance gates

1. All *active* repository scripts/workflows/build steps are native-approved implementation with source-bound verification; no POSIX shell invocation and no GNU component admitted by default.
2. An independently verified source and dependency inventory covers generated artifacts and build tools. No unapproved GNU/POSIX component may be used or shipped.
3. The reference registry and the root 0BSD license independently pass HEAD and TAIL checks. Forbidden reference texts may remain stored as inert evidence.
4. Functional, CVE, license and provenance gates pass independently. Unknown, mixed, unattested or unverified terms stay HOLD.

**Chosen, never assumed.** An implementation detail cannot silently create a legal exception.

## Strict license version admission

**0BSD is the ONLY approved BSD-family license**. BSD-2-Clause, BSD-3-Clause and all named BSD-3 variants are FORBIDDEN, despite historical approvals which remain visible in Git history. MIT, MPL-2.0 and CDDL-1.0 are independently approved by name; MPL-1.0/1.1, CDDL-1.1, NPL-1.0/1.1 and MPL-2.0-no-copyleft-exception remain REVIEW. No GNU or authored POSIX execution approval is implied. Reference copies, unlike component-specific approvals, are inert evidence.

## Issue #29 execution containment (2026-11-20)

Branch `29-exc-gnu-linux-001-prefer-mit-oracle-lgpl-21-forbid-lgpl-3-gpl-eupl-epl-release-hold` makes the four historical Bash/Ubuntu workflows non-executable through explicit false job guards; active no-Python source checking is being migrated to Windows PowerShell. The native license-custody workflow is extended to main, pull requests, and issue #29 branch, and verifies the job guards. This is a code proposal, **not yet merged**. Existing remote historical branches may still carry their original workflow definitions and need independent review. The source guard and a skipped job do **not** certify third-party dependencies or the installed runner. A failing compliance gate remains HOLD.

## FRESH GammaGamma exploratory custody (2026-11-20)

The Issue #29 draft branch has exact archival copies of the 17 original legacy shell scripts, each indexed by original Git blob SHA-1. The original executable paths now contain variable-free, nonfunctional fixed-transfer FRESH GammaGamma stubs, and a central scripted trace exits nonzero with status 42. These files are **not approved to execute**, do not remove the need to migrate GNU/POSIX dependencies, and do not authorize publication or merge. Neither shell text nor a shell trace alone qualifies a toolchain, container or native build.
