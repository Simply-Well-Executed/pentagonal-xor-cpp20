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

## SLIDINGTOOLONGA-001 exact seed, 646/647 and inverse-window graph

The new **SLIDINGTOOLONGA** engine takes the user's nonnegative decimal exponent text without the old 32-bit exponent cap and computes the seed **exactly at X=0**, using independent dynamic base-1e9 limbs in native C++20 and BigInteger in Oracle Java 27.0.1.1. Both preserve the independent NAME, SLOPE and INTERCEPT source records. The output is a signed exact integer mantissa with an exact decimal scale. For all three default sources the intercept is exactly -3, so their origin seeds happen to coincide; this is *not* identity merging or a claim about their distinct slopes.

The DIAERY-derived workflow runs native exact qualification for power **0, 646, 647, and the selected user exponent**. A separate PowerShell/.NET BigInteger oracle recalculates every exact result, including full mantissa and scale. If the exact Oracle JDK is provisioned, C++20 and Java must also produce byte-identical source witnesses, or the gate fails. With Oracle absent, Java parity remains **HOLD**, never PASS. All experiments retain `RELEASE=HOLD`.

The signed origin result of `(-3)^647` has **309 decimal digits**, and is not mathematically infinite. The older binary-double implementation cannot represent it. For selected powers above 6, the old floating-point six-state gate remains a **power-0 baseline**, while the exact SLIDINGTOOLONGA gate evaluates the requested exponent; this is explicitly disclosed, not an automatic substitution or inherited pass.

`Invoke-SlidingToolongARender.ps1` produces scriptless XHTML with native fold/unfold pages, exact raw mantissa, one page-indexed ruler register, labeled Cartesian origin (0,0), and a fixed **[-2,2] X and [-2,2] Y** viewing window with equal units (80 SVG units per mathematical unit). It reconstructs visible segments by approximate inverse-power sampling, avoiding direct floating-point overflow. This is a **visual approximation only**, not an arbitrary-precision graph sample, and clipping or viewport constraints are *not mathematical restrictions*. It does not apply logarithmic axes, hidden fitting, or normalization.

**Boundary:** exact large-exponent polynomial seeds at arbitrary X, nearest-prime certification for a very large origin, and full six-state complex results are **not yet verified**. The exact origin is authoritative, while the prime-root-dependent QuadDi states remain symbolic/HOLD. This must not be described as proof of infinite computing, physical gamma emission, or that conventional Euler's constant is invalid. Inverse-square `1/r^2` is a distinct rule with a singularity at `r=0`, not a substitute for `(mx+b)^p`.

Manually run the exact qualification on Windows with:

    pwsh -NoLogo -NoProfile -NonInteractive -File quaddi-ni/Invoke-SlidingToolongAQualification.ps1 -RepositoryRoot ROOT -EvidenceDirectory OUTSIDE_REPO -Exponent 647

The separate optional `-OracleJdkHome` may be provided to require tandem Oracle parity.

**Expanded XHTML pagination:** SLIDINGTOOLONGA renders the six ordered states, every entry in `default-misc.tsv`, and one separately folded page per named line. Its report now qualifies the exact **Power 647** visualization as a CI canary, even when the selected input defaults to Power 0. Both pages state their exact exponent and disclose that the inverse-power visual path is only approximate. Additional misc labels are accepted through `-MiscFile`; the page count is derived from the input without a fixed page maximum.
