# Accessibility statement

Simply Well Executed welcomes contributors with disabilities and different access needs. Access to documentation, controls, evidence and participation must not depend on a particular sensory or motor ability. Accessibility is a contribution requirement, not a certification claim.

Last reviewed: 2026-11-27.

## Scope and target

This statement covers project-authored Markdown, XHTML, native Windows interfaces, command-line output, diagrams and contribution processes. The web-content design target is [WCAG 2.2 Level AA](https://www.w3.org/TR/WCAG22/). Native interfaces require platform-specific assistive technology evaluation. GitHub, browsers, operating systems and external tools have separate accessibility boundaries.

**Conformance status: not fully assessed.** We do not claim WCAG or ADA certification or legal compliance. Source inspection and automated tests are limited evidence.

## Required contribution practices

- Provide meaningful headings, descriptive links, logical reading order, text alternatives for informative images and nonvisual equivalents for diagrams, color, sound and motion.
- Keep a direct index and home route, previous and next links, and reciprocal related references. A reader may leave the reading circuit at any time; never require a full tour. Native PENTAPI must automatically return to ROOT and move focus to a root control after a deliberate terminal action, with evidence retained and no additional activation.
- Use simple Markdown lists and explicit link labels instead of decorative separators, including U+00B7 in new navigation. Preserve literal mathematical, linguistic, license and experimental source data.
- Make user actions available through keyboard and appropriately named standard controls; provide alternatives to dragging, timed gestures and pointer-only interaction. Keep focus visible and predictable, with no trap.
- Support magnification, text reflow, readable contrast, reduced motion and large enough targets. Evaluate keyboard, screen reader, high contrast, switch, voice and eye-control routes with appropriate equipment and user input. Do not infer consent from gaze, silence or timeouts.
- Preserve measurements and audit history when closing, resetting, collapsing or switching views. Do not promote a hidden control or stale action to authorization.
- Provide human-readable error and status messages, including a way to report barriers without relying on voice or private disability disclosure.

## Existing framework and Wiki comparison

| Framework | Existing evidence | Application to documentation |
|---|---|---|
| [THAILONGA action deck](diaery/accessible_actions.hpp) | Native named button mappings and action coverage contract | Descriptive link labels and ordinary keyboard-operable anchors; browser operation still needs testing |
| [SEAL3](diaery/accessible_seals.hpp) | Exclusive selection with at most four visible terminal actions | Four primary navigation roles; content and related links remain available, not forcibly collapsed |
| [PENTAPI](diaery/pentapi.hpp) | Root/branch/terminal, automatic NULL and RESET to ROOT, retained action status | Persistent home and index links; static pages do not simulate native focus or timed redirects |
| [WIKI and HIKE](wiki.cpp) | Graph reciprocity and closed reading cycle checks | Discoverability, no orphan pages, voluntary return and text-only diagnostics |

The original [PENTAPI design note](diaery/PENTAPI_ACCESSIBILITY.md) requires automatic return after a deliberately invoked terminal action. The operator reaffirmed this on 2026-11-27. The later explicit continue/release behavior is superseded for normal action completion, with its source history retained. NULL and RESET are internal transitions, not extra activations. Static Wiki links remain ordinary links; automatic native ROOT return does not imply an automatic browser redirect. See [Wiki accessibility guide](.wiki/Accessibility.md) for operational details.

## PENCE: similarity and rights review

PENNYWORTH preserves established attribution and licensing. PENCE is the project's proposed provenance-review process for unexplained overlap between distinct implementations or documents. Similarity of behavior, interface, output or source bytes alone does not establish common authorship, ownership or a license grant. Record the two paths, source commits, authorship/rights evidence, actual notices, scope of overlap, reason for duplication and independent test evidence. Retain the originals; classify uncertain cases as HOLD / Needs Review.

A shared object may be proposed only after independent identity and rights review. BSD-1-Clause (1BSD) is a candidate for an explicitly authorized common object, not an automatic conversion. Obtain the necessary rights and policy approval, retain applicable notices, verify compatibility and record the exact grant and scope before merging or redistributing under any different license. Existing 0BSD notices and third-party terms are not changed by this statement. See [license registry](LICENSES/README.md). U.S. Copyright Office [computer program guidance](https://www.copyright.gov/register/tx-programs.html) distinguishes copyrightable expression from functionality. Consult qualified counsel for disputed rights.

## Contribution and feedback

All contributions must follow this statement and the [contribution guide](CONTRIBUTING.md). Record applicable tests, evidence, limitations and NOT TESTED results in the pull request. A known material regression or unresolved rights conflict blocks maintainer approval until resolved or explicitly reviewed under the applicable policy. Documentation changes do not configure GitHub branch protection or certify compliance.

Report an accessibility barrier using [GitHub issues](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/issues/new) or an accessible written contribution channel. Include the affected page or control, expected behavior, actual behavior and optional device/browser details. Do not disclose medical information or private credentials. Maintainers should acknowledge, triage and document corrective actions; no specific response time is promised.

## Known limitations and evidence

Native screen reader, switch/eye control, voice, keyboard-only, reflow and contrast qualification have not been comprehensively completed. The native fixed desktop canvas requires further responsive testing. The Wiki graph audit does not inspect screen-reader announcements, visual layout, external URLs or all Markdown grammar. GitHub's actual rendering and assistive technology behavior remain to be assessed. Release HOLD remains independent.

[Documentation index](.wiki/Index.md) | [Accessibility guide](.wiki/Accessibility.md) | [Security policy](SECURITY.md) | [License](LICENSE)
