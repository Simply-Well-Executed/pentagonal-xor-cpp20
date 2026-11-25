# LICE | PR #95 GammaMAGICCOMB / MAGICCOMBammaG (EXPLORATORY)

LICE is the name of the **read-only two-pass inspection receipt**, not a diagnosis of infection, compromise, cyberattack, or malicious code.

## Frozen scope

- Original inspected source: PR #95 head 0179dc7de04c74637bfcf3d8da39b2d7fc0ef16f.
- Input list: ci/PR95_GAMMA24_PATHS.txt, exactly 24 explicitly named paths.
- Inspected Git tree: 664 entries including directories. The claim that exactly 24 files hide information is unverified.
- Archive/Retired is excluded from this selected scope.
- No source mutation, Unicode rewriting, NFKC application, deletion, or branch movement.

## Algorithm

1. Validate the manifest's cardinality (24), path syntax, and unique paths.
2. GammaMAGICCOMB reads files byte-for-byte in manifest order; records SHA-256, length, strict UTF-8 validity, NFKC difference indicator, bidi control counts, selected zero-width counts, NUL, TAB, and CRLF counts.
3. MAGICCOMBammaG re-reads the same files in reverse order, independently recomputing every observation.
4. Compare keyed rows from the two passes with ordinal equality.
5. Emit LICE.gamma24.tsv in a fresh evidence directory outside the source tree, retaining every original path.
6. HOLD on missing inputs, invalid UTF-8, NUL, or differing forward/reverse observations. Otherwise record matching observations, not a release approval.

The two-pass match establishes only stability between inspections. It cannot establish the absence of hidden instructions, tampering, supply-chain compromise, or semantic Unicode vulnerabilities. NFKC differences and zero-width controls are audit observations, not automatic evidence of malice.

Run against a local checkout of PR #95 or a descendant containing the audit:

    pwsh -NoLogo -NoProfile -NonInteractive -File ci/Invoke-GammaMagicComb24.ps1 -RepositoryRoot . -EvidenceDirectory "$env:RUNNER_TEMP/pr95-lice"

This implementation is committed for review, not independently executed or qualified.

Status: EXPLORATORY | SOURCE_NOT_MUTATED | RELEASE=HOLD.
