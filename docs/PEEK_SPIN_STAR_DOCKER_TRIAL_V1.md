# PEEK / SPIN / STAR native Docker trial protocol v1

**Status:** active validation protocol, not native OpenBSD attestation or
production release authorization.

The user's references are three **Warcraft III** trigger-based test
designs, not W3C compliance standards or PowerShell source files:

| Source | Original pattern | Native Docker analogy |
| --- | --- | --- |
| [WTii unit tester](https://www.hiveworkshop.com/threads/wtiis-unit-tester.239333/) | Independently selected trial units and their behavior | Explicit positive and hostile scenario test fixtures |
| [Spin v1.1](https://www.hiveworkshop.com/threads/spin-version-1-1.219769/) by -Derp- | SpinSpinSPIN periodically updates units | **SPIN**: 9 back-to-back independent invocations checking correct output |
| [Star v1.4](https://www.hiveworkshop.com/threads/star-v1-4.198361/) by -Derp- | AbsoluteLOOP advances a multi-stage star effect | **STAR**: 3 ordered phases x 9 independent invocations preserving output |

Both original maps display a **0.03-second game-time periodic trigger**.
This is a Warcraft III simulation interval, **not** a PowerShell
scheduler, exact host wall-clock interval, minimum CPU instruction
latency, or container execution guarantee. The difference in our
analogy is the tested quantity: rapid consecutive throughput versus
sustained consecutive correctness. We do not copy their GUI/JASS
content, map assets or effects into this 0BSD project.

## Source of truth and pinning

The source-commit tag and OCI manifest SHA-256 must match the remote
registry before any test arena starts. The runner pulls a digest-qualified
image, checks its source SHA, normal executable entrypoint, non-native
OpenBSD status, profile name and original independently published
YIN/YANG/RETURN evidence reference. Each test launches a **new Docker
container** with no network, read-only root filesystem, no Linux
capabilities, no-new-privileges and bounded PIDs.

Positive cases verify the exact eight-decimal parity output, including
zero and odd parity. Hostile cases verify nonzero exit code 2, stderr,
and empty stdout. One independent control-program arena parses JSON
and verifies six typed stages and three typed Boolean fields on each.
A final positive baseline must equal the first baseline exactly.

**SPIN** executes nine container runs consecutively, without artificially
sleeping between them. **STAR** executes 27 container runs in three
separate chronological groups. Both record wall time using
`System.Diagnostics.Stopwatch`, but the project does not claim
precise 0.03-second scheduling or cross-run performance comparability.
A per-container process timeout and finite loop bounds are mandatory:
a failed container or output aborts the trial, never extends it.

## GitHub publication order

1. A branch push triggers automatic Windows native compilation and the
   root/shared/consumer CTest suites plus normal production startup.
2. The PowerShell preflight parses the PEEK trial and publisher code
   and runs a malformed-digest rejection test, without executing Docker.
3. The separate Docker job is **HOLD** until an authorized source-bound
   toolchain/runtime license review and all required testing are complete.
4. After its in-Docker build, full-source byte comparison and startup
   check pass, GHCR receives a **unique candidate trial tag**. That
   candidate is pulled by digest and passes PEEK, SPIN and STAR before
   final GHCR autostart tags are issued.
5. Docker Hub receives its own **unique candidate trial tag**, also
   subject to the same remote-digest and PEEK/SPIN/STAR checks. Only
   after passing can the final uniquely versioned `paulcheek/autostart`
   profile tag be issued and independently revalidated.

The candidate and final tags remain distinct. A failed candidate
leaves its original failure logs and never qualifies or changes the
existing `openbsd1`, `openbsd2`, `openbsd3` evidence images.
Even a successful PEEK test is insufficient to resolve licensing,
native OpenBSD, Android/Oracle Java/other unregistered test coverage.
The workflow never overwrites a `latest` tag.

## Distinct standards

`ci/Test-FullSourceOutputContractsV1.ps1` performs SVG/XML
well-formedness against the W3C SVG namespace and type checks against
native CLI and JSON output contracts. This is **not** a claim of
Warcraft III support, browser-level W3C certification, or externally
validated XML schemas. **WC3** (Warcraft III) and **W3C** (World Wide
Web Consortium) must remain explicitly distinct.

**Chosen, never assumed. Failure never becomes policy.**
