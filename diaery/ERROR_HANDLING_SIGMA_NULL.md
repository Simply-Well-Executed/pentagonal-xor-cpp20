# DYNAMO ERROR HANDLING | SIGMA / NULL / PROVENANCE

**EXPLORATORY | METHOD gate | RELEASE=HOLD | 0BSD.** Implemented on PR #316; not a claim of reviewed science or a clinical inference.

## The user's requested chain, kept distinct from verified statistics

1. Inside one study, review all **actual reported measurement-error bounds** relevant to *the same quantity, method, units and population*. An SD, an SEM, a CI, or a participant variance is not automatically an instrument error. Unknown uncertainty is `MissingComponent`, not zero.
2. For nonnegative absolute bounds whose effects may accumulate, SIGMA creates a conservative within-paper sum **only if each error bound is already proven valid and comparable**. Do not double-count overlapping error sources.
3. Across independent papers, SIGMA then divides the total by paper count, preserving a rational numerator/denominator. This is an illustrative arithmetic average of paper upper bounds, **not** a universally valid worst-case estimator. A pooled research result requires study-weighting, correlation, heterogeneity and comparable protocols.
4. The proposed logarithmic transformation involving Euler's Jig / `i^2` is `InvalidPhysicalModel`. `i^2=-1`, so with a positive real Jig constant the logarithm is outside the real domain; Jig *area* also carries units, so the ratio is not dimensionless. A separate complex-log experiment would require an explicit branch convention, units and validation. It cannot be silently used to certify error bars.
5. A record without a source-verified positive non-MRI vaginal-depth measurement never enters numerical research aggregation. It remains available as a provenance/audit record, not erased.
6. NULL, inapplicable, incomparable, overflow and invalid physical model are explicit statuses: no chart writes or automatic fallback to a fake numeric zero.

The five checks above (source, applicability, comparability, arithmetic, model validity) are the optional **PENTAGRAM NULL lock** metaphor. The checks operate on data, not people. This ledger does not restrict discussion about anatomy, gender, or consensual sexuality.

## Exact test fixture (synthetic, NOT a source journal)

Paper A: bounds 10+20+30 = 60 micro-inches.
Paper B: bounds 15+25 = 40 micro-inches.
SIGMA papers = 100/2 = 50 micro-inches (exact rational aggregate).
This test does not imply any real paper has those reported error components.

## Known unfinished work

A real paper-by-paper audit must capture the article IDs and exact error definitions, including dependence and measurement bias. No claim that all uncertainties have been captured. DIAERY's existing geometry graph and native viewer are not changed by this header alone. Validation uses C++20 constexpr tests; CI execution unconfirmed; RELEASE=HOLD.
