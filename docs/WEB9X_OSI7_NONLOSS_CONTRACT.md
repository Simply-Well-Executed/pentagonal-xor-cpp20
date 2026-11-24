# WEB9.X: seven-layer boundary map and reversible design review

<!-- SPDX-License-Identifier: 0BSD -->
<!-- Copyright (c) 2026 Paul Allen Cheek -->

**Record:** WEB9X-OSI7-SEAM-2026-11-20  
**Status:** EXPLORATORY architecture note / not an OSI, cryptography, or release certificate.  
**Related CURRENT document:** [WEB9X_NITEREX_CURRENT.md](WEB9X_NITEREX_CURRENT.md).

## Decision boundary

The **Web9.X user interface** is chiefly an application-layer concern, but Web9.X's safe operation cannot be established by visual rendering alone. OSI layers describe useful *concerns*, not seven independent magical processing engines, a guarantee of security, or a claim that any application has modified all six other layers.

The operator raised concern that an overly rigid rejection of the earlier slots 7–9 experiment would discard a useful result. The previously named **certificate-chain slot experiment remains RETIRED from CURRENT execution**, as the operator explicitly requested. Its *source, witnesses, and lessons are preserved*. New interpretations can be proposed with a new ID and explicit approval; previous records are not silently rewritten.

## Seven distinct review lenses

| OSI layer | Relevance to a browser/rendering experiment | Checks, not promises |
| --- | --- | --- |
| 7 Application | Web9.X XHTML document, NiteRex controller, user choices, nine-slot model | Validate exact allowed input types and boundaries; never infer trust from a rendered label |
| 6 Presentation | UTF-8, XML, Unicode bidi/isolate processing, normalization and canonical serialization | Pin XML declaration, encoding, DTD/profile; compare canonical bytes separately from visible glyphs |
| 5 Session | User-initiated operation lifecycle and explicit session state | Do not infer consent from autoplay, silence, absent result, or restored cache; allow exit/restart |
| 4 Transport | HTTPS/TLS channel, ordering, timeouts, integrity in transit | Verify transport endpoint and certificate trust via supported APIs; do not invent custom certificate rules |
| 3 Network | IP routing, address selection, path changes | Route provenance matters; reachability is not identity or authorization |
| 2 Data link | Interface framing, physical-network segmentation and L2 trust boundaries | A shared link does not give shared identity or permission |
| 1 Physical | Hardware devices, signal paths, noise, fiber and optics | Physical signals and observed chart patterns do not establish data identity or a mathematical proof |

This mapping is intentionally conceptual: practical TLS deployments cross the simple OSI layering diagram; TLS certificates are not automatically “Layer 5” or “Layer 6.” A layer boundary is a review scope, not permission to relabel or manipulate source data.

## Certificate-chain lesson: specific, not universal

- TLS certificate chains can validly be transmitted as part of a standard handshake. **Chain bundling is not inherently incorrect**; verification of signatures, path, names, valid dates, trust anchors, key usage, revocation policy and actual implementation context is what matters.
- **Retired Web9.X-specific idea:** using slots 7, 8 and 9 for a chain/signature scheme without a complete operator-approved serialization and trust protocol. That idea must not run in CURRENT; retain original evidence in history.
- **No replacement invented:** slots 7–9 currently remain RESERVED without active payload. Preserve the total nine-slot layout. They do not silently acquire certificates, signatures, security privileges or a replacement manifest.
- **Reintroduction path:** introduce candidate protocol WEB9X-SLOT-REV-N, frozen inputs and precise byte stream, independent validation, stated threat model, explicit user approval; until then HOLD. This is a *proposed* review path, not a change to the retired source record.

## COREAO non-loss invariant

```text
original source / historical artifact (unchanged)
          |
          +--> interpreted candidate (labeled EXPLORATORY)
          |          |
          |          +--> independent testing / provenance
          |
          +--> CURRENT reviewed policy (explicitly chosen)
                     |
                     +--> Web9.X display and NiteRex renderer
                               |
                               +--> visual output / ledger (derived only)
```

- A screen, witness ledger, routing path or winning test must never overwrite original input bytes.
- Evidence of an earlier passing test remains evidence for that **exact** frozen test, not automatic authorization or proof for later variants.
- STATUS vocabulary: **ESTABLISHED / EXPLORATORY / CONTRADICTED / RETIRED / NEEDS REVIEW**; preserve scope and provenance on every transition.
- At publication the untested native Web9.X browser and full repository-wide integration remain **HOLD**. Build source-bound C++20 `-O2` prototypes, prove their actual functionality with tests, and attribute new source under 0BSD only when authorized; do not relicense external archives.
- The W,V integer 5×5 25-slice model and A/B separation are visualization experiments. No claim of biometric or breathing control.
- **Chosen, never assumed. RETIRE does not mean ERASE.**

## Recorded resolution

The potential “lost win” is preserved as *replayable historical evidence* without making its unverified certificate handling active. No source deletion, rename, binary mutation, production release or merge is authorized by this note.
