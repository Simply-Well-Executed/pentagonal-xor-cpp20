# TRIFORCE ΣΣΣ HEAD / TAIL native gate for PR #32

**CURRENT exploratory implementation, release HOLD.**

## Frozen owner constants

- Euler: Q62 source `12535862302449814170`, output `271828182845904523`
- ℼ: Q62 source `14488038916154245685`, output `314159265358979323`
- Intensity `I`: Q62 source `315868046760674477`, output `6849296450333523`
- Each has the rightmost **23**. They do not all have the same digit length.
- All are preserved separately in `shared/toolong_q62_core.hpp` and
  checked against independent decimal BigInteger operations at each stage.
- Full source and original comments: `docs/TOOLONG_TRIFORCE_Q62.md`.

## Protocol

1. **Bootstrap exception:** A Windows GitHub runner first retrieves the exact
   `GITHUB_SHA` source archive. The source-contained checker cannot execute
   before its own source has been retrieved. No deployment, build, or release
   operation may proceed during bootstrap.
2. **HEAD before each controlled operation:** recompute all three exact Q62
   integer results independently with PowerShell .NET BigInteger, confirm
   the three immutable source initializer lines, `static_assert` witnesses,
   the unapproved human-review state, and SHA256 of each protected source
   file. The first HEAD freezes a session's snapshot.
3. **Stage execution:** the dedicated
   `.github/workflows/triforce-headtail.yml` runs **seven strictly
   serial** stages: configure native gate, build native gate, execute it,
   configure root, build TOOLONG/GammaGamma, run their CTests, and reaffirm
   release HOLD. After native compilation, each HEAD and TAIL also executes
   the genuine TOOLONG C++20 test binary. The bootstrap's first configure
   and build use the independent arithmetic checker until that binary exists.
4. **TAIL immediately afterward:** repeat the checks; reject altered hashes,
   changed Q62 output, a mismatched or skipped phase, or a native regression.
   If a stage fails, the workflow stops. Its TAIL must not be claimed PASS.
5. **Other active Windows workflows:** existing licensing/MagicComb,
   OpenBSD-serial compatibility (Windows preflight only), no-Python source
   guard, and the Windows native-full-source steps now run the same reusable
   source-level HEAD/TAIL check around each post-bootstrap controlled stage.
   Their existing CMake targets preserve their own verification scope and
   do not automatically become authenticated TOOLONG validation.
6. **No implicit release:** successful math tests do not resolve existing
   POSIX/GNU/license/source provenance HOLDs. `three_distinct_claims`
   only checks caller-asserted identities and explicit YES flags, not real
   authentication. No human approvals are inserted by CI.

## Limitations and enforcement

- The gate controls the explicitly wrapped pipeline stages, **not every
  machine instruction**, arbitrary code path, or action on GitHub outside
  these workflows. A source-only HEAD/TAIL check is not a compiler run.
- Workflows are changes proposed in **PR #32**; they cannot automatically
  enforce themselves as required checks in repository branch protection.
  An authorized owner must explicitly require the new GitHub check and
  review bypass rules separately if universal gating is desired.
- Only native Windows Microsoft Visual C++ (MSVC) C++20 `/O2` runs
  the TOOLONG executable in the dedicated gate. Clang, clang-cl and GCC
  are not silently used as fallbacks. Linux, Android and OpenBSD native
  builds are not reclassified as certified by this source-only preflight.
- Evidence lives outside the source tree in the runner's temporary
  directory. A fresh session does not inherit a prior session's PASS;
  `HEAD` and `TAIL` require ordered pairing and identical SHA256
  source ledgers.
- This design does not claim protection from a malicious actor able to
  replace the workflow, checker and verification policy at once. Review
  permissions, pinned protected branches and external attestation remain
  necessary defense-in-depth.

**Failure never becomes policy. Chosen, never assumed.**

### Full-source HEAD/TAIL custody

The guard takes a deterministic **SHA-256 inventory of every authored source file** in the exact archive, ordered by ordinal path and excluding only local `.git` metadata. All subsequent HEAD/TAIL checks in that runner compare to the first frozen manifest. A new, missing, renamed or modified file changes the ledger and blocks the next controlled operation. This extends the narrower named-file check; it does not prevent tampering by an actor who can change both the code and the policy before the source bootstrap.
