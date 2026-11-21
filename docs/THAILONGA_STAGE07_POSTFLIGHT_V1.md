# THAILONGA postflight v1: recompleting REELNATIVE stage 07 without rewriting custody

**Scope:** the already-established ten-stage ledger and historical stage-07
image are frozen. This is a new, supplemental, append-only evidence protocol,
`THAILONGA_POSTFLIGHT_V1`, and an independent OCI diagnostic version v2.

## Canonical before derivation

The canonical sequence is
`01 8.0 → 02 7.9 → 03 6.9 → 04 5.9 → 05 4.9 FIRST → 06 3.9
→ 07 4.9 RETURN → 08 2.9 YIN → 09 2.9 YANG → 10 COREAO (OpenBSD2)`.

- Stage 05 FIRST and stage 07 RETURN are **different** witnesses at the
  same OpenBSD 4.9 release reference. Neither is overwritten.
- Stage 07's original `core` field is exactly `NONE`. It may not be
  retroactively rewritten to `OPENBSD2` without defining a new experiment.
- Stage 08 YIN and stage 09 YANG each retain an independently established
  `core=OPENBSD2` and distinct witness SHA-256. Stage 10 retains both.
- The user's **THAILONGA** name designates an **ordered YIN + YANG core
  projection** in this new protocol, never an identity merger. Both source
  hashes and separate identities remain present in the derived evidence.
- **Temporal constraint:** stages 08 and 09 do not exist at stage 07's
  original execution point. Their results may annotate stage 07 **after**
  stage 10 completes; they may not be presented as inputs that stage 07
  originally consumed. Hence the explicit `POSTFLIGHT_ONLY` label.
- For any stage without a core requirement or an explicitly authorized
  postflight projection, its original `core=NONE` stays NONE. No
  indiscriminate core assignment or fabricated hardware/software capability.

## Defined exactly once

The authority remains `ci/Invoke-OpenBSDSerialCoreao.ps1` and the original
`ci/OPENBSD_SERIAL_COREAO.tsv`. The new function
`ci/Invoke-OpenBSDThailongaPostflightV1.ps1` reads a completed ten-line
canonical JSONL and verifies the frozen manifest and native-evidence annex
SHA-256 values, exact stages 05–10 witnesses, chronological predecessor
links, and native status HOLD. It then constructs exactly one domain-separated,
UTF-8, literal pipe-delimited payload:

```text
THAILONGA|V1|POSTFLIGHT_STAGE07|OPENBSD4_RETURN|CANONICAL_CORE=NONE|PROJECTED_CORE=OPENBSD2|RETURN=<SHA07>|YIN=<SHA08>|YANG=<SHA09>|COREAO=<SHA10>|MANIFEST=<SHA256_CANONICAL_TSV>
```

The supplementary `thailonga_sha256` is SHA-256 of those exact bytes.
It **does not replace** the earlier witnesses, and the SHA-256 is not
an OpenBSD signature. The derived JSON record preserves every individual
witness, source-provenance hash, timing designation, and separate core
fields. `approved=false`, `native_openbsd_binary=NOT_BUILT`,
`release_status=HOLD`.

## Mutation, pipeline and publication rules

- `ci/Test-OpenBSDThailongaPostflightV1.ps1` independently derives the
  overlay twice (HEAD and TAIL), requires byte-for-byte equality, rejects
  altered YANG witnesses, and rechecks frozen source file hashes.
- The existing `.github/workflows/openbsd-serial-coreao.yml` now also
  triggers on **every push to the working feature branch**, even for file
  changes outside its prior path filter. It runs the ten-stage source ledger,
  HEAD+TAIL, native-evidence validation, THAILONGA postflight negative test,
  and the existing serial Windows MSVC C++20 `/O2` portability preflight.
- A future automatic registry release is **not enabled** merely because
  continuous branch validation passes. Existing GNU/POSIX license HOLD and
  native OpenBSD target HOLD remain unchanged. Automatic publishing on
  every commit would create many unreviewed public artifacts; it requires
  explicit per-stage release criteria and authorization in a new protocol.
- The new independent `REELNATIVE-RETURN-THAILONGA` OCI image prints
  the original 07 FIRST/RETURN custody plus postflight YIN+YANG+seal,
  embeds the supplementary JSON, compiles twice with Clang C++20 `-O2`
  and executes in a Linux/amd64 `FROM scratch` container twice. This
  proves only the diagnostic, not a historical OpenBSD source or binary.
- The historic REELNATIVE-RETURN v1 image and its exact manifest
  `sha256:4fd533c1c5e710a3523b66ed1d2bb60fa019cba0914a9f49dfebd133b6fd328c`
  are preserved. The v2 image uses a different descriptive package:
  `reelnative-return-thailonga-evidence`. On an explicitly approved
  one-time publication commit, it can also publish a **new immutable
  source-commit tag** in `openbsd3` without overwriting the existing
  `sha-432a0a0e94fed8d0463ad84164e51d432d91a238` tag.
- The registries support content-addressed blobs and independent tag
  names. `@sha256:<digest>` is the exact pinned image reference.
  Registries do not supply a general permanent HTTP redirect. No
  `latest` tag is overwritten, and no PR merge occurs.

**Failure never becomes policy. Chosen, never assumed.**
