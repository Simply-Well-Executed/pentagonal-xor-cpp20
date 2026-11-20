# Copyright, project marks and 0BSD · source-preserving notice

> **Review status: DUE FOR UPDATE (2026-11-19).** Verify rights, third-party
> content and source attribution before claiming ownership or registrations.
> **Original contributions:** Copyright (c) 2026 Paul Allen Cheek.
> SPDX-License-Identifier: 0BSD where the project holds the applicable rights.

## What the license grants

Read the repository's [LICENSE](../LICENSE). It identifies **Copyright
(c) 2026 Paul Allen Cheek** and distributes the original software under
the **BSD Zero Clause License (0BSD)**. Reuse, copy, modify and
distribute covered software with or without a fee, subject to the
license's warranty disclaimer. **Do not add a mandatory attribution
condition**: 0BSD does not require one.

For original project writing, use 0BSD as declared by repository
documentation unless a file specifically identifies different rights.
Keep `SPDX-License-Identifier: 0BSD` on newly authored C++ files.
Retain historical authorship and upstream provenance as records even
though the license does not require recipients to preserve attribution.

## Local-first license reference and component provenance

The root [`LICENSE`](../LICENSE) is authoritative for the original first-party source under 0BSD; the [`LICENSES/` directory](../LICENSES/README.md) is a separate **reference cache**, not a second repository-wide grant. It retains exact copies of approved **and forbidden** license texts and their pinned publisher/source identities in [`LOCK.tsv`](../LICENSES/LOCK.tsv). The [PowerShell integrity verifier](../ci/Assert-LocalLicenseRegistry.ps1) checks Git object identity without network fetches, normalizing text, or changing the files.

Do **not** infer the license of an individual component from a nearby boilerplate copy. Record its actual version, file-level copyright and NOTICE, SPDX expression, AND/OR alternatives, and approved distribution scope in [`COMPONENT_USAGE.tsv`](../LICENSES/COMPONENT_USAGE.tsv). Unidentified, mixed and Oracle-vendor notices require review; Oracle is a vendor rather than a universal SPDX license. LGPL-2.1-only is preferred; LGPL-3.0 is forbidden. See [locality-first source custody](LOCALITY_FIRST_SOURCE_CUSTODY.md).

## What the license does not establish

**Verify** independent rights for user-supplied portraits, other
people's video frames, external screenshots, citations, fonts,
artwork, vendor branding, linked websites, proprietary standards,
and other imported material. A project-wide LICENSE file does **not**
grant copyright in material the project does not own. The Atlas
HTML/XHTML prototypes embed user-provided source frames: the 0BSD
notice applies to original code and documentation, **not**
automatically to those embedded media bytes.

## Project marks and unverified registration

The project uses **Simply Well Executed**, **PENTAGONAL XOR**,
**MINZA**, **MINZA Pinwheel Atlas**, and **Hanzeye** as identifying
names. If the respective rights holder elects, those names can be
presented with an unregistered `TM` symbol; this document does
**not** assert a trademark registration, cite an unverified registry,
or use the registered `®` symbol. A 0BSD copyright license does not
resolve trademark ownership or automatically license another party's
marks. Respect the rights of third-party trademark owners.

**Trademark verification still requires evidence** of rights holder,
mark, jurisdiction, goods and services, and registration status (if
one exists). Do not infer those facts from a repository name.

## Verb and evidence discipline

- **Write** "the source records" instead of "it is proven" without
  an independently supported proof.
- **Write** "the renderer displays" instead of "the algorithm
  reconstructs" when it only changes presentation.
- **Write** "the gate compares five sampled colors" instead of
  "the gate confirms identity" when it measures correlated pixels.
- **Write** "the route toggles presentation direction" instead of
  "the video reverses time".
- **Write** "the test checks 32 Boolean inputs" rather than
  "the compiler guarantees integrity".
- **Name** the actor, operation, source, validation scope and
  unverified condition. **Keep** passive voice only where the actor
  genuinely does not matter or the protocol requires it.

## Required review action

**Verify** code-to-prose correspondence, **inspect** source licensing,
**audit** mathematical definitions, **test** commands, **check**
external references and **record** reviewer provenance. Preserve
established definitions and historical versions; a review flag is
neither a silent retirement nor a mutation.

See [DOCUMENT_REVIEW_REGISTER.md](DOCUMENT_REVIEW_REGISTER.md).

**Chosen, never assumed.** Third-party rights remain third-party
rights, and any unresolved ownership claim remains unresolved.
