# JSTAR: native MSVC process provenance (experimental)

<!-- SPDX-License-Identifier: 0BSD -->

## Scope and safety

The process name alone is **not evidence of a compromise**. GitHub Actions
cleanup of vctip.exe, conhost.exe, or several MSBuild.exe workers does not prove
malware, man-in-the-middle attacks, registry injection, or a modified binary.

The JSTAR PowerShell wrapper \`ci/Invoke-JstarProcessAudit.ps1\` is a
Windows-native, source-preserving **sampled process provenance** check.
The authoritative C++20 build still uses CMake, MSVC and the existing
TRIFORCE gates. It never invokes Java to compile C++.

- Pin the native build to \`Release\`, \`--parallel 1\`, MSBuild \`/m:1\`,
  \`/nr:false\`, \`MSBUILDDISABLENODEREUSE=1\`.
- Observe the CMake root PID and sampled descendant processes, including
  unexpected descendants. Also sample named host tool/console processes
  for comparison.
- Record parent PID, creation timestamp, executable path, command line,
  Authenticode status, signer subject, and SHA-256.
- HOLD if sampled critical build tools (\`cmake.exe\`, \`MSBuild.exe\`,
  \`cl.exe\`, \`link.exe\`) lack a valid Microsoft/Kitware signature,
  if executable identity cannot be established, or if an unreviewed
  descendant appears.
- Preserve raw stdout/stderr byte hashes, full process audit JSON and
  its SHA-256 in GitHub job logs, even when compilation fails.
- Never kill a process merely because it is named \`conhost.exe\` or
  \`vctip.exe\`. Never auto-repair C++ source, elevate privileges,
  promote a release, or infer human consent.

This is **not a complete kernel event trace**. Snapshot polling can miss
short-lived descendants, PID reuse needs forensic review, and valid
Authenticode signatures do **not** rule out every attack. The exact
command line and path are evidence, not proof of harmlessness.
A GitHub-hosted runner is not physically airgapped; initial source retrieval
is online. JSTAR does not claim otherwise.

## HEAD / TAIL and custody

The existing owner Q62 and exact-source SHA checks remain authoritative.
Both \`ci/Invoke-TriforcePipeline.ps1\` and the Windows
\`ci/Invoke-FullSourceAutostartNativeSuite.ps1\` route native build operations
through JSTAR. The invoking stage's TAIL runs in a \`finally\` block, so a
failed build still closes the observed stage before failing CI.

Audit JSON is stored outside the repository source tree under the runner's
temporary evidence folder; the workflow prints its digest and complete
record into GitHub Actions logs. The historical job logs are distinct from
GitHub repository commits and are subject to GitHub log retention.
For longer-term custody, copy artifacts to a separately reviewed evidence
destination and verify SHA-256. No auto-publishing or upload action is enabled.

## Expected current failure and follow-up

As of the investigation of run 44982849912, the root test source
\`tests/toolong_arithmetic_test.cpp\` declared \`third\` once as an
\`ApprovalClaim\` and again as a \`toolong_ratio\`; MSVC errors
\`C2371\` and \`C2088\` follow. This PR deliberately does not
edit that unrelated test source. **Native CI may remain red** until a
separate, reviewed code correction is made. A red build with a complete
JSTAR record is still meaningful evidence; it never becomes PASS.

## References

- Microsoft MSBuild command-line reference: \`/nr:false\`, \`/m:1\`.
- Microsoft PowerShell \`Get-AuthenticodeSignature\`.
- Microsoft \`Win32_Process\` CIM properties: \`ParentProcessId\`,
  \`CreationDate\`, \`ExecutablePath\`, \`CommandLine\`.

All release, licensing and human-approval requirements remain unchanged:
**RELEASE=HOLD; chosen, never assumed.**
