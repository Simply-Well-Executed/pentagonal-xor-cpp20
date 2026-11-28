<!--
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
-->

# THAILONGA SEAL³ Accordion | Single-exposure Accessibility

**Status:** EXPLORATORY, release HOLD, native ISO C++20 /O2, 0BSD.

## Architecture

Two root choices (X/Y), two intermediate branches (Y1/Y2) and five terminal pages (Z1..Z5) each expose four native BUTTON actions. All actions from the previous touch deck remain instantiated but **only one path's terminal action buttons are visible and operable at a time**. The root/branch selectors remain visible. Hidden action buttons cannot be triggered by stale WM_COMMAND IDs. This derives from the disclosure/accordion UI pattern; W3C APG supports exclusive expansion. The source data and selections are retained independently of display.

## Reserved AIRGAP

The window reserves a fixed 690px-wide action region below root, branch and terminal headings. Collapsing a branch hides pages and terminal actions but never shifts or repurposes the reserved slot. Text indicates AIRGAP RESERVED. Native target dimensions are 338x52 for roots/branches, 132x52 for page selectors and 166x56 for leaf actions. The always-visible COPY FULL ACADEMIC AUDIT control occupies a fixed 690x64 independent position and works regardless of accordion state. The prior shortcuts remain functional.

## Screenreader and assistive pointer

Native Win32 standard BUTTON controls expose names and click/invoke semantics through operating system accessibility support; active selection appears in the button's visible name. The standard dialog message pump handles keyboard focus traversal. User activation is required, never presumed from gaze proximity or silence. Eye dwell, switch scanning, screen reader announcements, high-contrast colors, zoom/magnification, tablet touch and reduced window sizes MUST be tested with real devices. This revision is not yet an accessibility certification or proof of WCAG conformance; the fixed desktop canvas still needs responsive work before mobile-size deployment.

## Regulatory framing

The Americans with Disabilities Act (ADA) is legislation of the United States; it was not ratified by all countries across the Americas. The European Accessibility Act (EU Directive 2019/882) applies to covered goods/services and EU member-state implementing laws; the United Kingdom has separate Equality Act 2010 and accessibility regulations. Legal scope, standards, enforcement and remedies depend on jurisdiction, organization and product. General design inspirations do not establish compliance or a universal ranking of which jurisdiction is stricter.

Reference: Wikipedia 'Disclosure widget'; W3C WAI-ARIA APG 'Accordion Pattern'; WebAIM 'Disclosures and Accordions'; ADA.gov ADA law; European Commission EAA; GOV.UK accessibility requirements.

## Validation

`accessible_seals.hpp` enforces complete static action coverage and that no more than four terminal action buttons are visible. `accessible_seals_test.cpp` checks exclusivity and branch transitions. Native `diaery.cpp` uses independent visible button headings and reserved slot, with `accessible_actions_test.cpp` preserving all action mappings. Tests are registered for DIAERY CTest and every LadderDi rung. CI and hands-on accessibility checks remain unverified, so RELEASE=HOLD.
