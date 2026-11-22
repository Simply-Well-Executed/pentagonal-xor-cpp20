# Japanese Article 239 source-fidelity review

**Document ID:** JP-CCP239-SXS-2026-11-21-v1  
**Status:** EXPLORATORY / DOCUMENTED / NEEDS REVIEW, not legal advice, an independent legal opinion, a completed HANDI certification, or an adopted project policy.  
**Scope:** Source-preserving documentation and translation audit; no executable change, public allegation, or incident report.  
**Provenance:** Read-only review of this branch's `docs/WRITING_OPERATING_PROCEDURE.md`, `docs/HANDI.md`, and `docs/POSIXI_FRESH_ARTISTIC_CHAIN.md`, plus the official Japanese law identified below. This document is an additive, derived record.  
**Privacy:** No personal or intimate conversation text, student identifier, social-media handle, school identifier, or unverified accusation is published here.

## Completed publication action and its limits

**Record** a side-by-side (SxS) comparison of Article 239(1) and 239(2) of Japan's Code of Criminal Procedure. **Preserve** the two statutory paragraphs as separate source units, map each to a bounded English gloss, and expose display-layer and inference failures that arose during an exploratory conversation. **Do not** modify existing HANDI, BiDi/TriDi/SigmaDi, MagicComb, or frozen FRESH protocols. No software tests or legal proceedings were performed.

**Original statutory source:** Japanese e-Gov, Criminal Procedure Act (刑事訴訟法, Act No. 131 of 1948), Article 239, 2026-08-13 effective-version page:  
https://laws.e-gov.go.jp/law/323AC0000000131/20260813_508AC0000000067

**English translation reference (different publication/version; not authority over Japanese text):** Ministry of Justice Japanese Law Translation database, *Code of Criminal Procedure (Part I and Part II)*:  
https://www.japaneselawtranslation.go.jp/en/laws/download/3364/09/s23Aa001310306en6.0_h23A74.pdf

The controlling source is the Japanese statutory text for the applicable date. A translation is a secondary interpretive aid, and a different edition is not proof of current effective law.

## SxS: two complete provisions, no checkerprinting

The color names below are **display metadata only**, not source characters, legal classes, votes, powers, or original statute headings.

| YIN (BLUE): Article 239(1) | YANG (ORANGE): Article 239(2) |
| --- | --- |
| **Original Japanese** | **Original Japanese** |
| 何人でも、犯罪があると思料するときは、告発をすることができる。 | 官吏又は公吏は、その職務を行うことにより犯罪があると思料するときは、告発をしなければならない。 |
| **Working English gloss** | **Working English gloss** |
| Anyone who believes that a criminal offense has occurred may file an accusation. | A government or local-government official who, through performing official duties, believes a criminal offense has occurred must file an accusation. |
| **Subject:** 何人でも (any person) | **Subject:** 官吏又は公吏 (government or local-government official) |
| **Trigger:** 犯罪があると思料するときは (when the person believes a crime exists/has occurred) | **Trigger:** その職務を行うことにより犯罪があると思料するときは (the belief arises through performing official duties) |
| **Operative expression:** 告発をすることができる | **Operative expression:** 告発をしなければならない |
| **Legal modal:** permission, may | **Legal modal:** duty, must |

**Source-level conclusion:** Paragraph (2) adds a context-specific obligation for the public officials it names. It does not amend paragraph (1) into a general mandatory-reporting obligation for everyone. Neither paragraph alone identifies a universal criminal penalty for non-reporting. Separate statutory duties and sanction rules require their **own** sources and elements.

**Terminological guard:** 告発 (kokuhatsu, criminal accusation) is not silently substituted with 告訴 (kokuso, complaint by a person entitled under other rules). Exact Japanese grammar and the legal context matter.

## Whole-heading MagicComb check

Earlier assistant-authored display text joined an existing project term with a new generic description and a middle dot, giving the composite the appearance of an established canonical label. **That was an editorial composition, not an authorized renaming.** Review the entire heading (Markdown level, words, punctuation, implied relationship, namespace, and visual rendering), not just one code point.

The character `·` is U+00B7 MIDDLE DOT, UTF-8 C2 B7, general category Po, bidi class ON; it remains the same under NFC and NFKC. It is meaningful in some ordinary languages, so **do not globally strip it**. Reject **unauthorized insertion into canonical identity** instead. Source bytes, original Unicode code points, explicit user-chosen labels, and the official statute outrank rendered appearance.

**Method status:** The above is a manual, source-based editorial finding. No claim is made that the repository's complete executable MagicComb HEAD+TAIL gate was run on this document.

## COREAO, THAILONGA, and SigmaPi namespace custody

- **CORE:** Preserve each original statutory paragraph unchanged as an attributable source unit. A display transformation is not a legal amendment.
- **THAILONGA:** Preserve the historically named two-aspect exploration without silently reidentifying it as a statutory hierarchy or a universal-language title.
- **YIN (BLUE):** Forward-check the Japanese wording and the limited interpretation it supports.
- **YANG (ORANGE):** Reverse-check the English interpretation against the Japanese source and challenge unsupported extensions.
- **AIRGAP:** Check alternative readings, dates, provenance gaps, missing elements, and whether multiple witnesses really are independent. Here it is methodological, not physical network isolation.
- **YAN:** Reconcile bounded findings without treating agreement as an automatic approval or inventing another authority.
- **HANDI SigmaPi:** In `docs/HANDI.md`, SigmaPi is the third-degree **adherence observation** corresponding to SigmaDi, with PING as fold name. This legal SxS is **NOT** a completed SigmaPi/PING witness or an independently validated conformance result. If used as a conceptual bridge, label it an analogy / EXPLORATORY review proposal. Do not alias it to the unrelated mathematical SigmaPi namespace.

**No checkerprinting:** Never alternate isolated fragments of paragraph (1), paragraph (2), a translated text, or another legal code to create a newly purported legal rule. Neither one source rendered nine times nor nine translations constitutes nine independent legal authorities.

**Inverse-square analogy:** The physical relation `I(r) = P/(4*pi*r^2)` may inspire increasing attention to a primary source, but does not quantify certainty, establish guilt, modify statutory text, or grant authority in law.

## Cross-domain educational-safeguard example (not an incident finding)

An isolated photograph of a student's marked worksheet can justify questions about clarity, assessment objectives, and constructive feedback. It does **not**, alone, establish criminal abuse, the teacher's intent, a point deduction, a JET Programme connection, a reportable statutory offense, or a mandatory national escalation. These are different hypotheses with different evidentiary requirements.

A school-level instructional review is distinct from a criminal accusation and from national legislative oversight. Respect local jurisdiction, student privacy, and the need for authorization before publishing third-party personal information or communicating on anyone else's behalf. No school or person is identified in this repository record.

**Source selection constraint for this review:** Do not rely on JET teaching materials as authority for the Japanese grammar or legal translation. This is an expressly scoped research preference, **not** a verified claim that all JET teaching is defective.

## No-mutation, revision, and review acceptance gates

1. Preserve each statute paragraph and citation as a separate complete source record; log the version/effective date.
2. Preserve original source bytes and Unicode code points; display colors and heading punctuation remain explicitly derived.
3. Record translation provenance, uncertainties, and alternative grammatical readings without silently changing the original law.
4. Keep source, translation, legal effect, possible duty breach, and possible sanction distinct.
5. Do not convert evidence of a student's confusion into proof of teacher misconduct, or merge unrelated statutory definitions.
6. Treat repeated renderings/translations as dependent on the same source unless independence is demonstrated.
7. Maintain existing canonical software algorithms and frozen LAB protocol records; a change requires a distinct ID and source history.
8. No invented findings, signatures, court results, compliance approvals, executed tests, or publication authority.
9. Record dissent and actual reviewer sign-off. Until then: **NEEDS REVIEW / NOT RUN**.

**Review decision:** This document provides a bounded correction-and-source register. It does not adjudicate a school incident, amend legal texts, run MagicComb or HANDI, and does not release or merge PR #32. The existing PR and repository HOLD states remain independent and unchanged.

**Related repository methods:** [Documentation operating procedure](WRITING_OPERATING_PROCEDURE.md), [HANDI](HANDI.md), and [FRESH artistic lineage](POSIXI_FRESH_ARTISTIC_CHAIN.md).

**Continuity:** Preserve prior erroneous displays as historical evidence where independently retained; do not silently promote them to CURRENT. Chosen, never assumed.
