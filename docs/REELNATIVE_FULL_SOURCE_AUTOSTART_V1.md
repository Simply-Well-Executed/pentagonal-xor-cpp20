# Full repository autostart OCI candidate v1

**Status: candidate, not publicly published. Separate from historical native
OpenBSD release HOLD and from the already published evidence images.**

This is a **new** versioned workflow and Dockerfile, not a change to the
previous COREAO or REELNATIVE evidence container digests. Each push to
`feat/fresh-artistic-eye-chain-20261120` triggers
`.github/workflows/reelnative-fullsource-autostart.yml`:

1. Pull exact source at `GITHUB_SHA` from GitHub with PowerShell, no Node-based
   checkout action.
2. Confirm original root zero-clause `LICENSE`, the full `LICENSES/`
   reference catalog, `.github/workflows/`, `Archive/Retired/`, the
   authoritative OpenBSD ten-stage manifest and native evidence.
3. In GitHub-hosted **Windows** CI, compile **all CMake native targets** in
   the root and `shared/` builds using C++20 `/O2`; run every
   registered root/shared CTest, install the root build, compile/test a
   downstream `tests/consumer` user of installed audit headers, run
   the normal `pentagonal_xor` production program with its five feature
   arguments, and run separately authored PowerShell security/custody checks.
4. A **separate gated Docker job** can build
   `docker/FullSource-ReelnativeAutostart.Dockerfile` with an explicitly
   digest-pinned **approved** toolchain and runtime. The in-Docker build
   reruns native CMake/CTest plus normal application execution with no
   network access during build. The resulting runtime contains the source,
   licenses, docs and installed application, starts the real
   `pentagonal_xor` executable directly with sample feature arguments,
   uses a non-root identity and has no shell-based entrypoint.
5. Before pushing, the workflow runs a restricted Docker startup and
   compares the entire copied non-sensitive source tree, file by file,
   against the original exact GitHub source revision. Any mismatch
   blocks publishing; stage #07's original evidence content remains
   an independent immutable-digest reference.

## Docker input scope

The source context includes **all tracked files except repository
administration history (`.git`), generated build/evidence files, and
sensitive private key/secret/token paths specified in `.dockerignore`**.
It now explicitly includes `.github` in the build context, rather
than suppressing CI metadata. Reference texts in `LICENSES/text/`
are preserved *as licenses/reference documents*, not imported as
approved licenses for any compiler, OS package or linked library.
Source has its own 0BSD license; third-party base images, CMake,
compiler runtimes and SDKs retain independent licenses.

## Linked profiles, without replacing existing images

| Profile | Original immutable evidence | Future autostart tag |
| --- | --- | --- |
| `openbsd1` stage 09 YANG | `ghcr.io/simply-well-executed/openbsd1@sha256:a14c9c0e588291492d3523bc6109393c497bc6b4b1019646270b1f119c00a47c` | `paulcheek/autostart:openbsd1-sha-<new-source-commit>` |
| `openbsd2` stage 08 YIN | `ghcr.io/simply-well-executed/openbsd2@sha256:e06e9c3c5f982f9e013a519e463012d528dc0839fe54dace410366dc21f74d56` | `paulcheek/autostart:openbsd2-sha-<new-source-commit>` |
| `openbsd3` stage 07 RETURN+THAILONGA | `ghcr.io/simply-well-executed/openbsd3@sha256:e3cea4dc33c4b5da24da664bf61f5e2d79a395d64c53a0a145b732f21a01cad1` | `paulcheek/autostart:openbsd3-sha-<new-source-commit>` |

Each future autostart image also has the descriptive GHCR name
`ghcr.io/simply-well-executed/<profile>-autostart:sha-<new-source-commit>`
and a GHCR short name
`ghcr.io/simply-well-executed/autostart:<profile>-sha-<new-source-commit>`.
The Docker Hub and GHCR manifest digests must match *after* successful
pushes, and the Dockerfile retains the exact original evidence digest
as metadata. It does not use the evidence executable as the normal
application, does not claim an OpenBSD OS build, and never writes to
any existing `openbsd1`, `openbsd2` or `openbsd3` release tag.

## Fail-closed external approval requirements

The Docker job will not execute until these GitHub repository variables
are independently supplied by an authorized reviewer:

- `SWE_AUTOSTART_BASES_APPROVED=true`
- `SWE_AUTOSTART_ALL_TESTS_ATTESTED=true`
- `SWE_AUTOSTART_RELEASE_APPROVED=true`
- `SWE_AUTOSTART_TOOLCHAIN_IMAGE=<approved-registry/image@sha256:64hex>`
- `SWE_AUTOSTART_RUNTIME_IMAGE=<approved-registry/image@sha256:64hex>`

Two Docker Hub secrets already used by the prior evidence publisher,
`DOCKERHUB_USERNAME` and `DOCKERHUB_TOKEN`, are reused only after
the independent checks. Repository variables **do not override** the
release safety checks. The publisher independently rejects active
POSIX `.sh` files and runs the native local license registry gate
before pulling or executing Docker. The existing repository contains
unresolved POSIX/GNU compliance HOLDs, so **do not set these variables
to fake approval**.

Java/Oracle JDK, Android SDK/device execution, and any unregistered
external suites are **not** part of root/shared/consumer CTest. Running
all CTest entries is not equivalent to validating every language and
platform in the repository. No `autostart` package or digest may be
claimed published without an actual successful Docker build, every
required test, binary dependency/license evidence, and both remote
push receipts. We never overwrite `latest`.

This workflow only moves compilation and testing to GitHub-hosted
runners. It cannot prove the availability of the workers indefinitely,
guarantee retention of transient logs, or remove a third-party
license concern by renaming a container.

**Failure never becomes policy. Chosen, never assumed.**
