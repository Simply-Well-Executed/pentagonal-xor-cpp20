# COREAO stage 10: isolated OCI evidence v1

**Status: source-only proposal until the linked CI run confirms a Docker runtime result.**
This supplements, not edits, the existing `ci/OPENBSD_SERIAL_COREAO.tsv` or its
ten-stage SHA-256 witness algorithm.

The purpose is to **execute the existing COREAO reconciliation on the GitHub
runner**, preserve independent OpenBSD 2.9 YIN and YANG witnesses and original
**CORE=OpenBSD2**, then compile a **Linux/amd64 C++20 freestanding witness
display** with `clang++-18 -O2` (no libc, standard library, shell, runtime
package manager or third-party base image). Two clean native-Linux compiles must
produce exactly matching binary SHA-256. A `FROM scratch` OCI image packages
that diagnostic plus the unchanged ten-line witness JSONL and native-evidence
manifest. Two restricted `docker run` executions must exactly match the
authoritative PowerShell reconciliation's published witness values.

It is **not an OpenBSD 2.9 operating system, a complete OpenBSD source build,
a native OpenBSD binary, an ABI-compatibility attestation, or a release approval**.
Docker runs Linux containers on a Linux kernel. All ten native OpenBSD
compatibility statuses remain HOLD; Stage 10 reconciles metadata, not software
from an unavailable historical toolchain. No numerical width, precision or
feature availability is inferred from version labels.

This isolated OCI image differs from the repository's legacy GNU/POSIX-contaminated
Dockerfiles. It uses **scratch** with a single freestanding Linux/amd64 ELF. No
`latest` tag is created or overwritten. No prior historical source or PR is
merged. This does not remove the existing independent GNU/POSIX policy failure
or create a general project-release exception.

## Artifact identifiers and authorization boundaries

On one explicitly authorized commit, the new workflow may attempt to push a
provenance-only candidate to these distinct image repositories:

- GitHub Packages / GHCR:
  `ghcr.io/simply-well-executed/pentagonal-xor-cpp20-coreao-evidence:sha-<40-character-commit>`
- Docker Hub:
  `paulcheek/openbsd2-coreao-evidence:sha-<40-character-commit>`

The owner namespace `paulcheek` is used by already-existing project Docker Hub
workflows. If the current `DOCKERHUB_USERNAME` secret differs or the
`DOCKERHUB_TOKEN` is missing, Docker Hub publication must fail closed.
The token is never written to the image or log. GHCR uses the
GitHub-provided repository token with `packages:write`. A container's remote
presence and immutable digest must be confirmed from actual workflow logs.
GitHub Packages visibility is registry-specific and not inferred from push.

A source `push` alone cannot silently grant permission to publish repeatedly.
The one-time workflow requires the exact project/branch and an explicit
`publish(coreao):` commit message. Any future publish needs a new deliberate
approval and a new versioned/immutable SHA tag.

## License and build boundaries

- Repository files packaged in the evidence image retain their existing
  original license. The evidence diagnostic itself is 0BSD.
- LLVM Clang and LLD toolchain executables are separate host tools under
  Apache-2.0 WITH LLVM-exception (verify against toolchain provenance);
  this permission is **not** a blanket approval of other third-party inputs.
- Docker's daemon/CLI are external host infrastructure, not included in the
  scratch image. Its implementation language or Ubuntu runner contents
  are not silently represented as the image's licensed dependencies.
- Project-authored PowerShell scripts generate and verify the source witness.
  No project-authored Python, JavaScript, TypeScript, Node.js, npm, Go,
  POSIX shell or GNU-source library is added to this isolated image.
- This standalone evidence publication must not be used to dismiss,
  bypass or mark PASS the wider repository's existing GPL/GNU or POSIX HOLD.

The CI outcome, both image digest(s), and registry upload success must be read
from GitHub Actions run logs. Missing credentials, unavailable Clang/LLD,
build failures, nonmatching witnesses and failed pushes are **HOLD/FAIL evidence**.

**Failure never becomes policy. Chosen, never assumed.**
