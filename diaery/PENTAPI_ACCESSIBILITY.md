<!--
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
-->

# PENTAPI | Five-stage low-effort selection

Status: EXPLORATORY; native ISO C++20 /O2; 0BSD; RELEASE=HOLD.

## Interaction contract

PentaPi names five **process stages**, not five extra clicks:

1. **ROOT**: only root choices are necessary.
2. **BRANCH**: selecting the root exposes branch choices; focus moves forward.
3. **TERMINAL**: selecting the branch exposes one page of up to four actions; focus moves to a usable page or first leaf.
4. **NULL**: after a terminal action, the active selection becomes inactive. NULL does **not** erase any measurement, prior action, rejected source, audit provenance, or configuration.
5. **RESET**: selection returns to ROOT and focus moves to a root button **automatically**, without waiting for another switch/eye-pointer activation. Last completed action/status is preserved.

The two post-action phases are **automatic internal transitions**, not extra controls, modal dialogs or timing-dependent animations. No movement through hidden sibling branches is required. The root selectors remain visible as an explicit fast escape; COPY FULL ACADEMIC AUDIT is permanently available outside the accordions.

The reserved AIRGAP never reallocates to unrelated content when a panel collapses. At most one branch and one terminal action page can be exposed, and terminal input from stale hidden controls is ignored.

## Why this is accessibility-oriented

A user relying on switch scanning, an eye-controlled pointer, a stylus or touch input may find repeated backward navigation costly. PentaPi avoids the extra command to close an action and can direct focus toward the next available control. The approach is inspired by selection burden in assistive communication systems, including Stephen Hawking's cheek-switch access, **without claiming to know his subjective experience or claiming clinical validation**.

The design retains predictable button locations and a permanent audit action. Automatic return should be user-configurable if usability testing later shows that some users need an alternate behavior or repeated rapid actions. The current implementation follows the explicit Root→Branch→Terminal→NULL→RESET request.

## C++20 contract and tests

`pentapi.hpp` models phases and transitions as constexpr functions. `pentapi_test.cpp` validates root/branch/terminal reachability, the terminal visibility gate, unsuccessful action-status retention, automatic return without additional user action, explicit reset, and persistence of action count. `diaery.cpp` invokes the model from native Win32 BUTTON events, updates exposed controls, and moves keyboard focus after each navigation step. Tests registered in DIAERY CTest and LadderDi, pending execution.

**Remaining qualification:** native successful build, actual switch scanning, Windows Eye Control, high-magnification, screen reader and responsive-layout tests. We do not claim those tests passed or that the application is ADA/WCAG certified. RELEASE=HOLD.

## Restoration record (2026-11-27)

The operator reaffirmed the original automatic-return feature. The intervening explicit continue/release route is superseded for normal terminal completion. The current `complete()` executes NULL, RESET and ROOT internally, retaining action count, identity and reported status. `diaery.cpp` already refreshes the deck and focuses the first root after `complete()`; no extra activation is required. The historical explicit-route API remains for source compatibility, but normal completion no longer enters that phase. Compile-time fixtures now cover repeated actions, stale hidden actions, failed-action status and automatic return. Native execution and assistive technology qualification remain pending. See [Accessibility statement](../ACCESSIBILITY.md).
