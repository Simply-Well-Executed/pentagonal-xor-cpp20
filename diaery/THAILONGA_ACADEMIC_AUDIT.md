# THAILONGA academic audit and circle-band extension

Status: EXPLORATORY; native ISO C++20 /O2, 0BSD, release HOLD. No clinical diagnosis or intervention is derived from this viewer.

## Clipboard export is integral

From the DYNTR0X or FREQ view, press **C** to copy the complete ASCII TSV audit to the Windows clipboard. The native clipboard handoff uses CF_TEXT, movable global memory, and fails closed on allocation, export overflow, unavailable clipboard, or a mismatched X/Y/Z observation count. Successful copy is explicitly acknowledged onscreen. Nothing is silently truncated. Each accepted manual row is exported with lossless 8-byte IEEE754 native byte representation of X, Y and Z. This is not rounded decimal precision and the byte-order label must travel with the record.

Export includes figures and titles, inch and count units, X-quarter/Y-third subunit definitions and 0,0 origin, original selected coordinates, source/study warnings, all accepted user-generated rows, source-range and FILTREX outcomes, derived intercept coefficients, ten projector states, normalized annular band status, band sector count, per-axis means, sample standard deviations and variances, empirical frequency extrema, nearest-rank 0/20/40/60/80/100 percent values, exact DARN femtosecond components and derived YARN, rejected-attempt total, memory HOLD, and known lack of uncertainty/physiological source evidence.

**Rejection provenance:** each rejected manual attempt is retained separately with its original X/Y/Z IEEE-754 values and an explicit reason code (FILTREX input, FREQ domain, or memory). An unsuccessful rejection-ledger allocation increments a missing-evidence indicator, and the clipboard export is held rather than pretending the record is complete. No actual research participants have been imported. The current X/Y/Z rows are manually selected exploratory triples.

## Bands are rings: projection geometry distinct from physical anatomy

The 41–42 percent historical exploratory annotation is represented only by an annular circle projection with 72 angular subunits, in its own normalized-percent coordinate space (not inches); no previous linear-band strip remains. NICENESS passes if every required angular sector exists; FIRMNESS passes if the inner and outer radii are finite/ordered and the discrete ring is gap-free. **J** toggles one missing sector as a regression canary, and **N** toggles band visibility. A passing geometric ring is not evidence of a physiological phenomenon. Its original numerical provenance remains unverified.

`circle_band.hpp` also has a **nonclinical cylinder design geometry** with depth and independently measured resting and stretched radii. If either radius is missing, it returns `MissingRadius`: depth alone cannot yield width, stretch capacity, elasticity, pressure-volume compliance, or insertion behavior. A right triangle, Pythagorean identity or an x-squared transformation cannot determine an unknown radius without further constraints. This applies to any anatomical context without sex-based preference or biological essentialism.

## FREQ, quintiles and model overlays

`freq.hpp` counts exact stored VALUES independently for X/Y/Z, with dynamically grown host buffers and no preset sample-count cap. Rejecting invalid values does not mutate accepted evidence. The histogram bins are a display setting with observed-data-derived endpoints, not the canonical count function.

`quintiles.hpp` sorts caller-allocated copies of data and computes **empirical nearest-rank** 20, 40, 60, and 80 percentile boundaries. It does not assume normality. Quantiles cannot by themselves detect biologically impossible values: FILTREX rejects nonpositive, missing or nonfinite data in this positive-length application and FILTROX labels measurements outside cited study-specific observed intervals, without treating sample extrema as universal anatomical limits.

The optional Gaussian bell is only a parametric fit to manually entered observations with adequate number and variance. Bell curves are **not** quintiles; they are distinct summaries. No value generated from a bell replaces an observed count, and no confidence interval or statistical certainty is implied by a smooth line. Biological plausibility additionally requires source/methodology checks beyond a chart.

## Qualification

Compile-time unit tests: `circle_band_test.cpp`, `quintiles_test.cpp`, `freq_test.cpp`, `thailonga_test.cpp`; runtime clipboard-audit buffer tests: `audit_test.cpp`. Each is registered for native /O2 DIAERY CTest and LadderDi's nine-lane MSVC phase. Code registration does not prove workflow success. No remote feed is required and no native release has been attested; RELEASE=HOLD until verified.
