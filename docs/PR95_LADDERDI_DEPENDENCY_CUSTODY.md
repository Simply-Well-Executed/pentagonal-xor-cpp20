# PR #95 | LADDERDI dependency custody (EXPLORATORY, RELEASE=HOLD)

This branch introduces pre-lab source transport, not runtime installation, a passing build, or merge approval. The existing nine-lane qualification remains authoritative. The original historical PR #95 witness is unchanged.

## Historical blocked lanes (run 45443640826)

- HeptaDi: Lua 3.2.2 unavailable.
- OctaDi: Python 2.7.18 plus Python 3 qualification exits 1.
- DoDecaDi: Perl 5.44.0 version mismatch.
- TriDecaDi: OCaml 5.5.2 unavailable.

The other five rungs in that run passed. Historical results do not attest the current head. PR #95 also diverges from its PR #34 base; source transport cannot resolve conflicts.

## Custody contract

1. The source-controlled TSV is HOLD by default. There is no assumed license approval.
2. Each reviewed APPROVED row must record exact version, trusted HTTPS origin, independently verified SHA-256, and a license in current repository policy.
3. The PowerShell tool ci/Invoke-LadderDiDependencyCustody.ps1 operates only in a network-enabled pre-lab stage and takes a fresh external destination directory.
4. The tool downloads one pinned archive and writes a custody receipt. It NEVER installs, expands, compiles or executes the archive.
5. Redirects, overwrites, digest mismatches, missing pins and unreviewed licenses are HOLD.
6. Any later extraction and execution integration requires additional archive safety, binary provenance and exact runtime-version attestation before the preexisting offline ladder runs.
7. Successfully staged is not successfully qualified, approved for release, or guaranteed safe.

Example after explicit policy approval and source pin review:

    pwsh -NoLogo -NoProfile -NonInteractive -File ci/Invoke-LadderDiDependencyCustody.ps1 -Lane HeptaDi -Manifest ci/LADDERDI_DEPENDENCY_CUSTODY.tsv -DestinationRoot $env:RUNNER_TEMP

## Unresolved license decisions

OCaml 5.5.2 declares LGPL-2.1-or-later with an OCaml linking exception. Perl offers Artistic/GPL options and needs specific terms checked. Lua 3.2 predates the modern Lua license and needs exact source license verification. Python and Perl are also prohibited by stated project tooling preferences. These are not silently approved by authorization to develop a dependency toolchain.

Sources:
- https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/actions/runs/45443640826
- https://opam.ocaml.org/packages/ocaml/ocaml.5.5.2/
- https://www.lua.org/license.html
- https://dev.perl.org/licenses/

Status: EXPLORATORY | NO MUTATION OF ORIGINAL PR #95 | RELEASE=HOLD.
