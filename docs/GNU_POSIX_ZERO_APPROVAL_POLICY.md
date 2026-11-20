# GNU / POSIX zero-approval migration protocol

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
