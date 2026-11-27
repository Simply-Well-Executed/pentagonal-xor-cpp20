# Accessibility guide

The [repository accessibility statement](../ACCESSIBILITY.md) is the authoritative contribution policy. This page is its operational Wiki companion.

## Navigate by choice

- The index lists the document groups. Home explains the documentation scope.
- Previous and next links form the HIKE reading circuit. A reader can instead use any direct reference, index or home, or leave entirely.
- Related references have return paths. This is graph reachability, not a mandate to traverse every page.
- Every page uses explicit, separately listed navigation labels. No decorative middle-dot separator is needed.
- Native PENTAPI automatically resets the selection and returns focus to ROOT after a deliberate terminal action; static Markdown does not automatically redirect a reader.

## Compare the existing implementations

| Framework | What it establishes | What WIKI adds |
|---|---|---|
| THAILONGA accessible actions | Named native button mappings and coverage | Descriptive text links and an ordinary Markdown navigation surface |
| SEAL3 accessible seals | One exposed terminal page, up to four terminal actions | Four primary navigation roles, with separate policy and related links; no hidden content mandate |
| PENTAPI | Root/branch/terminal, automatic NULL and RESET to ROOT; status retained | Persistent index/home links; static navigation does not claim native focus movement |
| HIKE | Reciprocal Hamiltonian tour of the supplied ring | A graph audit; not a proof of screen-reader access, focus or layout |

Source: [THAILONGA actions](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/blob/8a224966c39920c69e60f9ef80af5bbe3b1f283f/diaery/accessible_actions.hpp), [SEAL3](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/blob/8a224966c39920c69e60f9ef80af5bbe3b1f283f/diaery/accessible_seals.hpp), [PENTAPI](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/blob/8a224966c39920c69e60f9ef80af5bbe3b1f283f/diaery/pentapi.hpp), [PENTAPI tests](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/blob/8a224966c39920c69e60f9ef80af5bbe3b1f283f/diaery/pentapi_test.cpp), [HIKE](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/blob/8a224966c39920c69e60f9ef80af5bbe3b1f283f/wiki.cpp). The original automatic-return contract was reaffirmed on 2026-11-27. The explicit-route variant in the pinned historical source is superseded for normal completion. See the current [repository accessibility statement](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/blob/docs/myheadhurts-zenshin/ACCESSIBILITY.md) and `diaery/pentapi.hpp` for the restoration.

## Qualification checklist

1. Run the native Wiki audit for page coverage, reciprocal edges and the closed ring.
2. Navigate the rendered GitHub pages using keyboard only, then a screen reader; check link names, order, focus visibility and ability to leave the circuit.
3. Inspect zoom/reflow, contrast and diagram alternatives; evaluate target input devices when relevant.
4. Report PASS, FAIL or NOT TESTED per source revision. Graph PASS does not establish WCAG 2.2 AA conformance.
5. Apply PENCE to unexplained overlap; preserve distinct notices and source history. No new license grant is inferred.

## Related pages

- [Documentation entry](README.md)
- [Documentation index](Index.md)
- [Documentation home](Home.md)
- [Getting Started](Getting-Started.md)
- [Architecture](Architecture.md)
- [Source Map](Source-Map.md)
- [API Reference](API-Reference.md)
- [LCD](LCD.md)
- [REDUX](REDUX.md)
- [ARIARI](ARIARI.md)
- [TOOLONG and GammaGamma](TOOLONG-and-GammaGamma.md)
- [COREAO](COREAO.md)
- [ALONZ and METALOGRI](ALONZ-and-METALOGRI.md)
- [GENSHIN](GENSHIN.md)
- [LF9WN9Z](LF9WN9Z.md)
- [TALON](TALON.md)
- [SILVERHAND](SILVERHAND.md)
- [TRIFORCE and JSTAR](TRIFORCE-and-JSTAR.md)
- [Troubleshooting](Troubleshooting.md)
- [Policy and Custody](Policy-and-Custody.md)
- [Release Status](Release-Status.md)
- [Glossary](Glossary.md)
- [Sidebar](_Sidebar.md)
- [Footer](_Footer.md)

## Navigation

- [Previous: Policy and Custody](Policy-and-Custody.md)
- [Documentation index](Index.md)
- [Documentation home](Home.md)
- [Next: Release Status](Release-Status.md)
