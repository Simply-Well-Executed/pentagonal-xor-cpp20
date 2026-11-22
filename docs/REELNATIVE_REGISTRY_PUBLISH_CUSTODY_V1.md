# REELNATIVE registry publication custody: snapshot v1

This file documents how to read `ci/REELNATIVE_REGISTRY_PUBLISH_RECEIPTS_V1.tsv`.
That TSV is an **append-only versioned snapshot of observed CI publisher receipts**,
not the authority for canonical stage identities, OpenBSD releases, or
historical native builds. The authority for serial stages is the unchanged
`ci/OPENBSD_SERIAL_COREAO.tsv` and
`ci/Invoke-OpenBSDSerialCoreao.ps1`. For package content, the authority
is the registry's manifest addressed by its exact digest. Every
`PUSH_CLI_DIGEST_MATCH` value is grounded in the named completed GitHub
Actions run's publisher stdout; a later independent manifest HEAD request or
anonymous pull was **not** performed.

The receipt order follows historical **publication chronology**, not canonical
runtime order. The two most recent entries are deliberately both for canonical
stage **07**:

- **v1 original 4.9 RETURN:** source commit
  `432a0a0e94fed8d0463ad84164e51d432d91a238`, registry digest
  `sha256:4fd533c1c5e710a3523b66ed1d2bb60fa019cba0914a9f49dfebd133b6fd328c`,
  exact short tag `openbsd3:sha-432a0a0e94fed8d0463ad84164e51d432d91a238`.
- **v2 REELNATIVE-RETURN-THAILONGA postflight:** source commit
  `fafa512bf5bef804902e22300259fa9f59c39ae0`, registry digest
  `sha256:e3cea4dc33c4b5da24da664bf61f5e2d79a395d64c53a0a145b732f21a01cad1`,
  exact short tag `openbsd3:sha-fafa512bf5bef804902e22300259fa9f59c39ae0`.

Both short tags are under `ghcr.io/simply-well-executed/openbsd3` and
`paulcheek/openbsd3`, pointing respectively to separate long-description
repositories `reelnative-return-evidence` and
`reelnative-return-thailonga-evidence`. The earlier tag, digest and
symbolic stage 07 witness remain intact. The THAILONGA record is derived
**after** completion of stages 08 YIN and 09 YANG, and displays
`CANONICAL_CORE=NONE` and `PROJECTED_CORE=OPENBSD2`. There was
no retroactive insertion of future witnesses into historical stage 07.

## Practical immutability boundary

- Git commits and manifest digests are **content-addressed identifiers**;
  a changed image requires a different digest.
- OCI tags, even tags beginning `sha-`, are technically mutable unless
  a registry enforces immutability/retention permissions. This workflow uses
  unique commit tags and does not intentionally overwrite old tags,
  but **does not claim registry-side tag locking or permanence**.
- To recover exact bytes, use a `@sha256:<digest>` reference. Package
  deletion or garbage collection could still make previously recorded
  digest bytes unavailable; a historical receipt is not a storage guarantee.
- Repository history retains the original sources and manifest.
  This new snapshot is not an amendment to frozen v1 records. If
  publication evidence must change, create a new versioned report with
  source links and the reason, not a silent reinterpretation of v1.
- Continuous GitHub branch checks run for every push; **publication is
  separately gated**. A successfully built Linux/amd64 diagnostic does not
  attest an OpenBSD kernel, OpenBSD-native binary, native source build,
  licensing clearance or release qualification.
- No source or registry record grants permission to overwrite an existing
  `latest` pointer. No PR merge was authorized.

**Failure never becomes policy. Chosen, never assumed.**
