# QuadDi NI | independent line and page evaluator

**Status: EXPLORATORY.** 0BSD. This folder stands alongside DIAERY; it does not rewrite DIAERY mathematics or bypass existing release gates.

## Seed power defaults to zero

The seed exponent defaults to **0** in both native C++20 and Oracle Java. Users may choose any nonnegative integer representable by a signed 32-bit number. Powers are evaluated by repeated squaring, with a **HOLD** if finite numerical arithmetic cannot represent the result. There is no hard-coded maximum of power 6, but a finite computer cannot calculate infinite magnitudes.

The application adopts the empty-product rule: **t^0 = 1**, including t=0 (a declared application convention for zero to the zeroth power).

C++20 CLI commands:

    QuadDiNI.exe quaddi-ni/default-lines.tsv quaddi-ni/default-xs.tsv quaddi-ni/default-misc.tsv
    QuadDiNI.exe quaddi-ni/default-lines.tsv quaddi-ni/default-xs.tsv 9 quaddi-ni/default-misc.tsv

Oracle Java commands:

    java -cp classes QuadDiNI quaddi-ni/default-lines.tsv quaddi-ni/default-xs.tsv quaddi-ni/default-misc.tsv
    java -cp classes QuadDiNI quaddi-ni/default-lines.tsv quaddi-ni/default-xs.tsv 9 quaddi-ni/default-misc.tsv

PowerShell full paired qualification command:

    pwsh -NoLogo -NoProfile -NonInteractive -File quaddi-ni/Invoke-OracleNativeParity.ps1 -RepositoryRoot ROOT -EvidenceDirectory OUTSIDE_CHECKOUT -OracleJdkHome ORACLE27_HOME -Power 0

The **-Power** parameter is optional. The workflow_dispatch input **seed_power** also defaults to 0.

## Inputs and arbitrary line counts

- **default-lines.tsv:** one independent NAME|SLOPE|INTERCEPT record per line. Three defaults: Children, conventional e approximation, and Your_Jig. Add any number of lines with distinct ordinals; available memory, disk and CPU determine practical throughput. Names use ASCII letters, digits, underscores, hyphens and periods.
- **default-xs.tsv:** one finite X value per line, including the original five X values and a 201-point near-origin sweep. Custom X files are supported.
- **default-misc.tsv:** one miscellaneous page label per line. Add as many as desired.
- Total pages derive from **6 QuadDi states + miscellaneous labels + independent source lines**; there is no artificial fixed page count.

Every equation uses t = slope * x + intercept and seed = t^power. The original line centrix is -intercept/slope, or NA when the line is constant. The Cartesian origin (0,0) is shown independently.

## Exploratory QuadDi and COREAO

The ordered states are SEED, NEGATIVE_IMAGINARY, PRIME_ROOT, POSITIVE_PI, POSITIVE_IMAGINARY, NEGATIVE_PI. This first implementation explicitly uses **cumulative addition** of the state offsets, not multiplication or another unstated transition.

The root degree is the closest positive prime to the absolute value of the origin seed. Ties choose the lower prime. Negative seeds with root degree 2 yield positive imaginary roots; other negative odd-degree roots yield signed real roots.

**GammGamma** means the distance between consecutive states in the complex offset plane. It is **not physical gamma radiation**. Closure is the final offset distance from the seed. No claim of QuadDi uniqueness or proof is made.

## Dual-language qualification and viewer

The QUADDI workflow is **adapted from the already qualified DIAERY native parity YAML** (`.github/workflows/diaery-parity.yml`): SHA-pinned PowerShell source bootstrap, Windows MSVC **/std:c++20 /O2**, external evidence, concurrency without cancellation, native qualification, and always-on RELEASE=HOLD. DIAERY's original workflow remains untouched. The separate native qualification script checks powers **0–6**, 17 independent source lines, four additional miscellaneous pages, input rejection, hash custody, and an XHTML report.

When repository variable `SWE_ORACLE_JDK27_HOME` is **unset**, the same workflow logs `JAVA_CPP=HOLD|ORACLE_NOT_PROVISIONED` and may still pass its **native-only** job, matching DIAERY's explicit status convention. This is **not** an Oracle parity success. When the exact Oracle JDK **27.0.1.1** is provisioned, the independent Java/C++20 parity script runs and any disagreement fails the job. The app downloads no Java, substitutes no vendor, and has no automatic release.

Both engines evaluate all default powers **0 through 6**, an explicit user-selected power if greater than 6, and a test with **17 independent lines** including a zero slope. They must agree for names, ordinal/page identity, samples, real/imaginary outputs, prime degree, GammGamma, and closure. Continuous quantities use a declared floating-point tolerance (1e-11 relative to max(1, abs(a), abs(b))). Malformed-input rejection and input SHA256 checks are required. No cross-source result borrowing.

On parity, Invoke-Render.ps1 produces a **read-only, scriptless XHTML5** report with native HTML details/summary controls, per-source curves, original centrix text, origin (0,0), both X/Y numeric rulers, and **RULER REGISTER COUNT: #1**. Real and imaginary coordinates are separately disclosed, not numerically collapsed. No CSS, JavaScript, or hidden automatic fitting. The viewer uses declared finite windows and clipping; those are not mathematical domain limits. For powers 0 through 6 the Y viewports are predetermined; above 6 the viewport defaults to ±1e12 or can be overridden by -YHalfRange. Raw source data always outrank the display.

**Until the GitHub workflow actually runs with Oracle 27.0.1.1, verification is unconfirmed. RELEASE=HOLD.**

## Power-68 example and numerical honesty

Selecting power **68** is a valid *nonnegative-integer request*, but it does **not** imply the current finite-double engine can evaluate every default X value or determine a nearest prime degree exactly. For the default intercept -3, `(-3)^68` already exceeds the exact-integer range used by the prime-degree check. The application must report **HOLD** rather than approximate an unverified prime, truncate samples, or quietly alter the user's exponent. No such HOLD changes the mathematical domain of the underlying polynomial.
