# APPLICATION OF Y=MX+B | DIAERY graph adapter

**EXPLORATORY | RELEASE=HOLD | 0BSD.** Stacked on the unmerged DIAERY PR #34 branch to avoid introducing unrelated changes into main.

## Canonical flow

Original source coordinates -> independent validation -> circle fit for three distinct noncollinear points -> line fallback `y=mx+b` for collinear points -> `x=c` for a vertical line -> invalid/hold for degenerate or out-of-domain inputs -> square auto-fit viewport. The fallback does **not** "fix" the underlying data or claim equivalence between a circle and a line.

`application_of_ymxb.hpp` is header-only, freestanding-style C++20 (no standard library includes). `application_of_ymxb_test.cpp` supplies compile-time witnesses. Precision of floating computations is finite, so exact source metadata should be retained separately. This does not represent verified human anatomical coordinates or a clinical measurement.

## Named exploratory arithmetic

* FOLD: `n/2`
* PROJECT: `(n/2)^2`
* QUARTER: `(n*n)^2/2`, equivalent to `n^4/2` (name preserved)
* MINTING: `(100*n-3)/100 = n-0.03` in real arithmetic. Implementation uses `n-0.03` to avoid a gratuitous intermediate multiplication; floating-point output is approximate. This is **not** conventional rounding, measured precision, a unit conversion, or an established banking standard.

All transforms create derived values, never edit canonical input. A square viewport is display-only, not a coordinate scaling or clamp. A real UI must convert radius-squared to screen radius and include the full circle extent in auto-fit if drawing the fitted circle.

## Boundaries

This adds a pure calculation adapter and tests to the existing native DIAERY subproject. It does not retroactively modify the narrow fail-closed XHTML profile or claim a fully integrated live plotting interface. No numerical source should be rounded silently. Reject unsupported/non-finite inputs, do not turn failures into policy, and keep RELEASE=HOLD until separate UI integration and native CI pass.
