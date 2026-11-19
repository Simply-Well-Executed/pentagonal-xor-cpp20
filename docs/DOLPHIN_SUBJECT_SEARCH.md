# DOLPHIN Subject Search · MINZA Pinwheel Atlas v9

> **Status: EXPLORATORY · DUE FOR UPDATE (2026-11-19).** This is a local human-readable subject catalog, not a certified object-detection or facial-identification system. Copyright (c) 2026 Paul Allen Cheek for original project code and documentation, SPDX-License-Identifier: 0BSD for owned contributions; external media and third-party marks retain their own rights. See [LICENSE_AND_MARKS.md](LICENSE_AND_MARKS.md).

## Purpose and definitions

**DOLPHIN** is the project's optional, source-preserving, **single-subject focus selector**. A *subject* can be one person, object/article, or unconfirmed search target. A person name or emotional interpretation derived from narration is not treated as a verified identity or internal mental state.

The user-provided screen-recording prototype uses a manually curated list and source-backed snapshots, not an automatically trained detector. The standalone v9 has 52 embedded source-scene stills (mostly approximately four seconds apart plus selected exact-anchor images), 32 initial catalog entries, and the existing 67 half-second source-face crops. The source files, user-specific catalog descriptions and embedded pictures remain **local** and are **not distributed in this public repository**. No exhaustive discovery of every object is claimed.

## Always-visible last super and one-at-a-time semantics

- **Display** the last overlay at the bottom persistently. Do not hide the subject selector behind tabs, checkboxes, or radio controls.
- **Search** labels and source notes, and **filter** PEOPLE, ARTICLES, TO VERIFY, or ALL using labeled buttons.
- **Select** one unique subject ID at a time: a new choice replaces the prior selected ID; it never merges two people, identifies someone across scenes, or grants an identity claim.
- **Show** the original recorded scene alongside a separately rendered focused crop, with exact source scene time and bounding coordinates.
- **Keep** the source moment and selected region independent of the existing MINI/MINZA 67-frame face timeline. Scrubbing one source does not certify correspondence to another.
- **Offer** ADD ARTICLE and DRAW BOUNDS. The user can mark a previously unconfirmed subject in the scene. Record these regions as **USER-ANNOTATED**, not as detections supplied by a machine model. A pending label without bounds never receives a guessed region.
- **Allow** PREVIOUS SCENE and NEXT SCENE for browsing available stills; do not reuse a rectangle from a different timestamp without an explicit new annotation.

## Reversible origami visual controls

DOLPHIN ORIGAMI is the thirtieth independent nine-tile display skin. It projects **one selected, observed source rectangle** onto the same 3×3 hinged UV mapping when ON. When OFF, the original source-face tiles remain visible. Projection does not rewrite photo bytes or reassign the inputs of PENTAGONAL, NINZA, PING, or VISTA calculations.

All image modifications remain in independently removable plates:
- **DOLPHIN FOCUS BOX:** ON/OFF for a single scene rectangle, without changing the focused crop's source.
- **NINZA LABEL MARKERS:** ON/OFF independently of the NINZA UNFORGE calculation and plate. Default OFF.
- **PING LABEL MARKERS:** ON/OFF independently of PING UNTWIRL and its column inverse. Default OFF.
- **CENTRIX CROSS:** ON/OFF. Default ON as a removable visual axis, not a reconstructed anatomical landmark.
- **REAR FACEPLATE:** ON/OFF. Default ON, with nine visible patterned schematic backs reading UNOBSERVED. These display plates contain **no new camera observation**, 3D recovery or hidden backface geometry.

Use bold button labels and exposed text states plus accessible pressed states. Do not rely on color as the only meaning. The source media and audit records remain intact if any display plate is switched OFF.

## Evidence discipline and audit

Use the same source-coordinate frame to describe region bounds `[x,y,width,height]`. **Verify** that every bounding box lies in the selected recorded scene, and **record** scene time, crop/viewport mapping, subject ID, human label, kind, annotation status, source basis, selection action, and optional plate/toggle event. Separate exact image sample timestamps from femtosecond-formatted *requests*. Neither 15-decimal-place addressing nor frame interpolation creates additional camera observations.

Recognizable clothing, insignia, belt attachments, computer-like devices, body-worn camera-like rectangles and posted wall papers can be catalogued. A particular person's identity, professional authorization, emotional state, weapon classification, computer location, or calendar contents require separate evidence. Mark uncertain items TO VERIFY and make manual placement possible.

**Source → inspected scene → declared manual region → single active subject → optional nine-tile focus projection → displayed labels → local audit.** No automatic modification, no biometric identity score, no independent authentication, no hidden-content restoration.

## Review instructions

**Inspect** the original renderer and index, **test** one-active-subject selection, source-bound validation, source restoration after toggling OFF, independent PING/NINZA label states and scene-to-region timestamp guards. **Record** results and explicit reviewer decisions before removing the DUE flag. The repository documents the protocol but deliberately does not upload the media-embedded HTML/XHTML artifact.

Related: [MINZA_PINWHEEL_ATLAS.md](MINZA_PINWHEEL_ATLAS.md), [NINZA_PING_UNFORGE_UNTWIRL.md](NINZA_PING_UNFORGE_UNTWIRL.md), [VISTA_9X9.md](VISTA_9X9.md), and [DOCUMENT_REVIEW_REGISTER.md](DOCUMENT_REVIEW_REGISTER.md).
