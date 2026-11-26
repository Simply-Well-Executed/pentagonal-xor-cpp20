# PentaDi | Four LadderDi stages with shared NULL

**EXPLORATORY / RELEASE=HOLD**

Five conceptual positions: RUNG, EXCITE, EXCITED, ATTEST, and one **common NULL**. The NULL checkpoint is not the fifth passing task. It is a shared fail-closed witness that preserves absent/unknown qualification information. Each stage independently reports PASS, FAIL, or NULL. An actual FAIL blocks; an unperformed or missing stage is NULL/HOLD; only four explicit PASS results qualify the four-stage sequence. The common NULL is never silently rewritten to PASS or used to fabricate another participant. DYNAMO native tests and the EXCITED verification stage are sub-evidence requirements for ATTEST, not a substitute for the common NULL. No deletion of records and no automatic merge or release.

The C++20 `pentadi.hpp` implements the pure compile-time gate, with `pentadi_test.cpp` static assertions for all-pass, absent, failure and no auto-promotion. LadderDi's PowerShell ATTEST reports explicit `PENTADI|COMMON=NULL` and rejects any non-success prerequisite. Its identity is project-local terminology, not an established mathematical theorem. All recorded source outcomes remain authoritative.

## Research anatomy: zero measurement exclusion gate

The native `research_filter.hpp` is applied to the DIAERY X/Y/Z observation qualification. For this **research inclusion rule**, a reported penile length of 0 inches, penile circumference of 0 inches, or vaginal depth of 0 inches is **excluded from the eligible dataset**, not treated as a valid physical observation for this plot. Reported zero, unreported NULL, negative, non-finite and out-of-domain values have **different reason codes**. Such a result is ineligible for this specified analysis, not evidence that the source article lied, and not a statement about the identity or humanity of the subject. Actual papers may have zero as a code for missing, an unavailable measurement, or a different measurement definition: review provenance before excluding an entire publication.

A zero is excluded from the *derived computation and rendering*, while the source record (including zero and source ID), historical document and reason remain intact for audit. The test fixture uses synthetic identifiers `study-A` etc., **not actual identified articles or live imported study rows**. No unknown external dataset has been scraped, modified, or deleted. The inclusion gate is tested in `research_filter_test.cpp` and `dynamo_subject_test.cpp`, registered in CTest, and added to every LadderDi rung's native DYNAMO test group. PentaDi's shared NULL does not automatically qualify a missing publication or a missing error bound. Evidence of successful CI execution remains separate from committed source; RELEASE=HOLD.

## FILTREX versus FILTROX (literature-based research qualification)

**FILTREX** = source input integrity. Reported zero, negative, non-finite, out-of-domain and unreported NULL are not eligible for this particular positive-anatomical-length dataset. Preserve their original value, reason and study provenance.

**FILTROX** = source-specific *observed sample interval* check, not an anatomy diagnosis:
- X, **erect penile length**, Herbenick et al. (2014), 1,661 US self-measurements: **4..26 cm** (DOI `10.1111/jsm.12244`; PMID `23841855`). This is a *marginal* range.
- Y, **erect penile circumference**, same paper: **3..19 cm** (marginal range). The 4 cm minimum length and 3 cm minimum circumference are NOT necessarily one paired observation.
- Z, **vaginal length**, Lloyd et al. (2005), 50 participants, clinical observational measurements: **6.5..12.5 cm**, as reported in published summaries of the study (DOI `10.1111/j.1471-0528.2004.00517.x`; PMID `15842291`). The PubMed abstract confirms methodology and variation but **does not itself reproduce the numeric interval**. This is not an MRI-derived bound, and measurement landmarks, population and methods require confirmation before pooling with another vaginal-depth series.

The 0.244-in penile length, 0.1978-in circumference example falls below the Herbenick sample's minima. **FILTROX=BELOW_OBSERVED**, not “does not have a penis,” “impossible,” or “micropenis diagnosed.” Micropenis is a real penile anatomical diagnosis assessed with age-appropriate stretched penile length distributions, not by this erect self-measurement sample cutoff. Similarly, vaginal lengths outside the Lloyd study's sampled interval are not evidence that a person lacks a vagina.

DYNAMO **K** toggles between **STUDY-RANGE gears** (default: key-driven X/Y/Z changes clamped within each selected source interval) and **EXPLORATORY gears** (range warnings permitted; positive values may still be shown). Both X/Y and Z are treated identically by the FILTREX/FILTROX gates. A 0.05-inch key step and rendering precision do not replace the original sample extrema. Any source point is an unverified selection, not a real paired anatomical subject. No statistical regression or penile-vaginal correlation can be inferred merely from independently sourced minimum/maximum values or a line constructed from intercepts.

The `filtrox.hpp` and `filtrox_test.cpp` tests are registered in DIAERY CTest and nine LadderDi rungs. No study records were scraped or altered. **Source ranges are literature reference profiles and qualification warnings, never universal biological limits; RELEASE=HOLD until compiled/tested.**
