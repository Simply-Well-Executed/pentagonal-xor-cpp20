# DEARY DYNAMO | Subject-coordinate repair

**EXPLORATORY / RELEASE=HOLD.** Supersedes the earlier ASCII chart and any presentation of the X/Y/Z slider values as a verified paired clinical subject.

- X is horizontal penile length (inches); Y is vertical penile circumference (inches). Both must have identical pixel-per-inch scale. Z is a separate depth variable and is **not** automatically a third point in the XY plane.
- A source X/Y subject marker remains distinct from any constructed points A/B/C. Do not derive the latter from anatomical units unless a documented mapping and validation protocol authorizes it.
- Zero/null/unverified/non-MRI-disallowed Z: HOLD. Positively entered manual Z is exploratory, not research evidence. A claimed research-valid subject requires paired provenance and reported uncertainties.
- The circle-first adapter is an independent exploratory mathematics laboratory. It may not be invoked as proof of physiology from one (X,Y,Z) observation.
- The requested 41–42% flip is purely symbolic; never label it a verified orgasm threshold. MINTING never modifies source values. No displayed anatomical values are inferred from a missing source.
- Error status is inherited from the SIGMA/NULL ledger; missing source uncertainty is not zero.
- Screenshots/interactive implementations must show axes, units, original values, validation HOLD and the reason; no fabricated coordinates.

The C++20 `dynamo_subject.hpp` and `dynamo_subject_test.cpp` provide a minimal qualification boundary and compile-time witnesses, not a fully connected native graphical frontend. The source-status booleans here require real provenance verification upstream; setting a boolean does not establish scientific validity.

## Implemented native Win32 DYNAMO interface (PR #316)

Native `diaery.cpp` now includes `application_of_ymxb.hpp` and renders a dedicated GDI graph in **DYNAMO mode** with equal 30 pixels/inch on both axes. The axis origin is the lower-left of its 0..12 inch square. X is horizontal length and Y is vertical circumference. A distinct purple **S** plots the current (X,Y) selection. The cyan straight line is *algebraically constructed* through the intercepts `(0,Y)` and `(X,0)`, with computed slope `m=-Y/X` and `b=Y`. The line is not claimed to be an empirical correlation.

Launch from the repository root after the existing native MSVC build:

```powershell
& "$env:TEMP\diaery-evidence\diaery-native.exe" --dynamo .\diaery\index.xhtml
```

Or launch the existing DIAERY viewer normally and press **D** to switch. In DYNAMO: Left/Right change X, Up/Down change Y, PageUp/PageDown change Z, **M** toggles exploratory MINTING, and **D** returns to the original geometric viewer. X and Y are clamped to the observed bounds of the identified U.S. erect sample; Z's 0..6 range is only an interface limit. The displayed decimals are explicitly rounded **for display only**.

Z remains a separately labeled *unverified manual depth input* in inches; it is never fabricated as a third XY datum. Without qualified paired observations and an uncertainty budget, research remains **HOLD / SIGMA NULL**. MINTING draws an extra derived point without altering S. This does not create a biological correlation, infer orgasm, or purport to validate a source paper.

The XHTML profile, exact existing parity witnesses, current fold inspector and admission checks remain intact. The C++20 `application_of_ymxb_test.cpp` now includes intercept identity and invalid-input tests. **Actual native build/runtime and CTest execution have not been verified in this operation; RELEASE=HOLD.**
