# Atomix and TOOLONG TRIFORCE backport review

<!-- SPDX-License-Identifier: 0BSD -->

**Provenance:** Source: [intro-to-codeql PR #1](https://github.com/Simply-Well-Executed/introduction-to-codeql/pull/1), Atomix protocol 002 proven at commit a7855fb655628eb1e73beb236fc618a3cb9a8f77, plus new [upstream PR #79 review](https://github.com/skills/introduction-to-codeql/pull/79). Destination: [pentagonal-xor-cpp20 PR #34](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/pull/34). No cross-project byte-identity claim.

## Backported work

1. The **original Q62 TRIFORCE remains the authority**: three distinct Euler, Pi and fixed-point intensity records, independent BigInteger calculation, the native C++ witness where available, and full-source SHA-256 HEAD/TAIL. The common final 23 suffix is a limited consistency observation, not cryptographic authentication.
2. The **Atomix / Pinoy Half-Phiping local experiment** is implemented in ci/Invoke-AtomixHalfPhiping.ps1. It tests three phase-varying Unicode strings in disposable synthetic SARIF 2.1.0 JSON. Each trial changes exactly one reversible bit (ASCII A to @), checks parse and semantic integrity, and records hashes and byte offset. No live SARIF upload, exploitation or executable payload.
3. The **HEAD, AIRGAP, TAIL** phases each call the original authoritative Q62 and full-tree source gate before and after in a finally block. The AIRGAP is a logical validation boundary, not a physical network isolation guarantee.
4. **JSTAR build provenance** now rejects external junction redirects, output-equals-source, missing build-root samples, unproven process-identity paths, and unsafe process cleanup. A recorded failure remains HOLD.
5. Each Atomix invocation uses a **fresh evidence directory** and preserves prior evidence. A Windows junction negative regression is an independently bracketed stage.

## PR #79 findings mapped to PR #34

| Finding | PR #79 source correction | Adaptation here |
| --- | --- | --- |
| Opted-in SARIF publication skipped if findings detected | Validate/count, optionally publish, then enforce findings HOLD | Policy retained; PR #34 does not have a custom CodeQL SARIF uploader, so none is introduced |
| Junction lets output escape lexical containment | Reject reparse-point ancestors before any write | Shared guard used by TRIFORCE, JSTAR, Atomix, full-source autostart, plus a negative test |
| Repeat build reuses evidence directory | Fresh unique evidence path per invocation | Each local Atomix stage gets independent evidence and audit |

## Protocol lineage and controls

The backport uses **ATOMIX-PINOY-HALF-PHIPING-PR34-001**. This is **not equivalent** to the bookstore protocol ATOMIX-PINOY-HALF-PHIPING-SARIF-002: source authority, stage schedule and code differ. Never pool the scores without an explicit new comparison protocol.

- HEAD: Σ-Euler-א | original 271828182845904523
- AIRGAP: π-Pi-ب | original 314159265358979323
- TAIL: I-Intensity-한 | original 6849296450333523

Each controlled SARIF message contains a single ASCII sentinel. Only that byte receives XOR 0x01. All original bytes, source and schema context remain authoritative, and the local record logs the offset, digests and result. This deliberately does not claim to implement an otherwise unspecified historical volatile-string generator. The audit JSON is logged; no external artifact-upload action is added.

## Independent HOLD conditions

The historical root TOOLONG test has an unrelated conflicting 'third' declaration (MSVC C2371/C2088). This backport does not rewrite it. The strengthened JSTAR signer rules rely on observed trusted certificates and expected tool roots, but sampled Windows process ancestry is not a complete kernel event trace. All tests and cross-repo review must be assessed independently.

**RELEASE=HOLD. No auto-merge, authentication, external upload or physical-airgap claim.**
