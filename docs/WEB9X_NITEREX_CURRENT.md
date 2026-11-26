# WEB9.X / NiteRex / uni-cor-n-fold: CURRENT minimal design contract

<!-- SPDX-License-Identifier: 0BSD -->
<!-- Copyright (c) 2026 Paul Allen Cheek. Authorship attribution for project-owner design; this file is a new synthesis, not an assertion that historical third-party archives are 0BSD. -->

**Record:** WEB9X-NITEREX-CURRENT-2026-11-20  
**Status:** EXPLORATORY implementation contract, preserving established project names and source authority. **No release approval.**  
**Decision:** The operator explicitly retired the previous certificate-chain / signed-envelope **slots 7–9 experiment** on November 20, 2026. Active implementations must not use or infer that design.

## Names and identity

- **Web9.X / Web.9X / WEB9.X:** the owner's nine-slot browser and experiment family; no public internet project may substitute for its own design.
- **NiteRex:** exact capitalization, advanced renderer within Web9.X.
- **uni-cor-n-fold:** the current project name. Historical ZIP names beginning `unicorn-fold-` stay unchanged as provenance.
- **COREAO:** preserve the original CORE and validate independently computed derived routes. Rendered pixels, a browser result, or a cache never outrank the source algorithm and its witness.
- **OpenBSD2–8:** owner-defined project metaphor for a capability ladder. This is not evidence that any such versions of OpenBSD exist. OS/toolchain/platform support needs its own reproducible test.

## CURRENT active slot matrix

| Slot | Status | Purpose |
| --- | --- | --- |
| 1 | Historical proposed input | CSS |
| 2 | Historical proposed input | XHTML / HTML fragment (actual format must be declared) |
| 3 | Historical proposed input | JS as inert source/reference; do not silently execute as trusted code |
| 4 | Historical proposed input | Java text/reference |
| 5 | Historical proposed input | CSV |
| 6 | Historical proposed input | SVN metadata |
| 7 | **RESERVED / no active payload** | Previous certificate-chain experiment **RETIRED** |
| 8 | **RESERVED / no active payload** | Previous note/certificate split experiment **RETIRED** |
| 9 | **RESERVED / no active payload** | Previous recursive signature/subject-certificate experiment **RETIRED** |

**All nine numbered positions remain in the schema.** Reserved means no defined payload and no implicit authorization. A missing value does not become a certificate, authority, personality, or cryptographic YES. Slots 7–9 MUST NOT be populated by inferred defaults. Reassignment requires a separately identified, explicitly approved revision.

## RETIRED history, not ACTIVE policy

The earlier Nov 2 experiment modeled a certificate chain bundled into one payload and explored SHA-256/SHA-512/Ed25519 signing, while another slot list split root/intermediate/subject certificates across 7, 8, and 9. These are now **RETIRED**, not valid choices for the native renderer. Do not bundle any certificate chain in slots 7–9; do not create a signature or imply a trust chain from that past experiment. Historical ZIPs/notes/tests may remain intact under Archive/Retired for provenance and deliberate recall; retiring a scheme does **not** erase its test evidence or justify editing the original files.

**Technical qualification:** certificate chains are sometimes legitimately transmitted together by standard protocols such as TLS; the owner's rule here is a **Web9.X-specific design choice**, not a general theorem that all chain packaging is invalid or insecure.

No claim of security, verification, zero trust, signed authenticity, or trusted CA provenance may rest on the retired protocol.

## Rendering target and limits

1. Prefer **XHTML 1.0 Strict** DOM/CSS in new Web9.X renderer prototypes. An XML-parsed XHTML5 Canvas file is not automatically XHTML 1.0 Strict; changing the format must be explicit.
2. C++20 should use exact original repository headers where applicable, with `-O2` on applicable compiler targets and no unreviewed runtime dependency, generated script, downloaded toolchain, network access, or license exception. 0BSD attribution applies to newly authored source only; uploaded MIT binaries or unverified sources are never relicensed by renaming.
3. Keep A and B surfaces independent. W and V are integer coordinates -2 to +2 (25 choices). The five-dimensional scalar height field is a mathematical model, not physiological measurement.
4. Preserve A/B/both/signed-difference display views, finite miniatures, separately tagged 25-slice overview, and at most nine ghost-trail entries. Opt-in Play only, explicit Stop; no automatic motion or breathing synchronization.
5. Original algorithm and immutable input record outrank derived heatmap, ledger, signature, report, or renderer.
6. No GUI claim may imply every file under the 286-path baseline repo inventory is executed by the browser. Integration requires a source-bound build target, test coverage, provenance, and a separately recorded unintegrated path list.

## Current integration hold

- **Native C++20 Web9.X/NiteRex renderer:** PENDING build + test, not represented as complete.
- **Entire pentagonal-xor-cpp20 codebase as a Web9.X browser:** NOT IMPLEMENTED; numerous modules are platform-specific native code, tests, docs or evidence rather than browser runtime modules.
- **OpenBSD source/toolchain and hardware claims:** NOT CERTIFIED.
- **Cryptographic signature / certificate chain for slots 7–9:** RETIRED; no replacement inferred.
- **Release or publication:** HOLD pending actual independent evidence and owner review.

*Chosen, never assumed. MOVE ≠ EDIT. RETIRE ≠ ERASE. An experiment's failure never becomes policy.*
