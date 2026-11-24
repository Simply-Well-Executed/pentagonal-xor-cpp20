# PR-PARITY / 1 | Common native CI across every open PR

## Contract and equality

This contract is **identical by Git blob identity** in all nine
PR branches. It imposes the same fail-closed core checks, with
no preference based on PR number, branch age, owner, or author.
A missing prerequisite is a **HOLD**, never PASS, automatic consent,
or a reason to erase historical work. Individual branch-specific
workflows remain intact, their scope visible in Git.

Git/GitHub's current ref graph and event commit are the source
identity. The same native Windows PowerShell bootstrap fetches the
exact event commit (a GitHub-generated merge commit for pull_request
events) and reads that tree. A SHA in a document, PR description or
derived ledger is not an authority; SHA-256 computed against current
source is an independent audit witness.

## Combined #34 and #95 requirements

A common independent check validates:
- exact source, existing workflow and common checker;
- 0BSD root license and license custody source;
- source paths, Python exclusions and PowerShell parser health;
- minimum presence of the existing TRIFORCE, REELNATIVE,
  Android no-Python and OpenBSD serial workflow/script families;
- expanded #95 coverage for EXCITE, DOVIA, QUADDI, MATCHB0Xn9,
  and the nine-circuit Festival;
- full root Visual Studio MSVC C++20 Release build and CTest,
  with C++ extensions disabled;
- native TRIFORCE HEAD/TAIL and EXCITE whole-repository stages,
  plus independent DOVIA native CTest;
- complete source digest/count before and after native execution.

The baseline does not equate *file exists* with *test executed*.
Coverage is reported separately from actual native execution.
Every missing or failed requirement is recorded in one TSV audit,
then returns failure. Source and build artifacts are segregated.

The workflow uses read-only contents/PR permissions, PowerShell only
and no Node checkout/actions. The source bootstrap uses GitHub's
event commit, never a hardcoded snapshot hash. Neither workflow
has GitHub merge/deployment/branch-deletion/force operations.
`RELEASE=HOLD` remains distinct even if every CI gate passes.

## Historical comparison (2026-11-24)

Before harmonization, #34 had 20 workflow files and #95 had 28,
with #97=10, #98=15, #99=15, #100=16, #101=0,
#102=6 and #103=6. Multiple older branches do not contain
the modern native suite or custody manifest. Copying only YAML
names would fabricate functional equivalence. Therefore this
contract is a **shared governance and visibility gate**, not a
claim that old branches already implement all later features.
They are expected to HOLD until their missing source and suites
are separately integrated, reviewed and actually pass.

#34's test requirements and #95's expanded test requirements are
both enforced. Historical workflows remain unchanged for audit
and because some of them contain deployment capabilities that
must NOT be silently switched on. A PR is not merge-qualified
merely for possessing the common CI policy.

The canonical filenames are
`.github/workflows/PR-PARITY.yml` and
`ci/Invoke-PRParity.ps1`. Both should be byte-for-byte identical
on every open PR branch at any given reviewed parity revision.
