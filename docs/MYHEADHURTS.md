<!--
Copr.© 2026 by PAUL ALLEN CHEEK mail@pcheek.com C5824

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
-->

# MYHEADHURTS

Zenshin magnetic folding manuscript | 2026-11-27 | EXPLORATORY / LAB

## Scope and provenance

User-authored conceptual vocabulary and instructions from the Zenshin conversation are recorded below. This is a proposed geometric and symbolic model for studying magnet-like interactions, not a validated physical theory, linguistic standard, or change to project policy. HONGKONGZA and its characters are fictional reference keys. Examples and proposed tests are distinguished from the requested rules. No code or CI changes are implied.

## 1. ZEN, ARIRANG, Wonzai

ZEN means ARIRANG within this model. Write only the tone devices in the Zenshin output; Zhuyin phonetic symbols and Pinyin syllables are reference inputs. Device identities: ˉ U+02C9, ˊ U+02CA, ˇ U+02C7, ˋ U+02CB; ˙ U+02D9 is not a permanently assigned Centrix. Its treatment as an ordinary device remains open. Explicit first-tone mark is a project convention. Wonzai is the proposed return to natural Thailonga given enough time. Do not repeat THAILONGATHAILONGA in rendered output or append unwanted literal period/Markdown markers.

Addition preserves ordered distinct objects; multiplication means bonding in this model. Neither is claimed to equal standard arithmetic or set union. Factorial supplies the descending order n,n−1,...,1. Preserve every input identity and coordinate during a bond; never silently deduplicate distinct occurrences.

## 2. Euler's Jigged 4-square Pyramid and the 33-degree display

The Greek four-square reference is clockwise from upper left: ˉ, ˊ, ˇ, ˋ; relative angles 0, π/2, π, 3π/2. The Centrix is dynamic: heaviest means sharpest, and sharpness is the largest angular shift from the tone two positions earlier. The earlier demonstration used absolute unwrapped differences; this is provisional, not a settled rule for wrapping, direction, or ties.

The Big Spin traverses +11π to −11π to +π. In the previous quarter-turn demonstration this is 44 reverse and 24 forward quarter-turns. Endpoints have equal unit-complex orientation e^(i11π) = e^(−i11π) = e^(iπ) = −1. This equality does not imply equal paths. The exact reverse factorial fold/warp geometry is still undefined.

Requested Zenshin display: rotate 33 degrees toward the sun; in the wordplay, sun means son, SUNWUKONG (also SONWUKONG in the request), the fictional builder of HONGKONGZA. HONGKONGZA is the key, SUNWUKONG the NORTHSTAR and CARAVAN the giant overhead magnet / primary tone attracting the devices toward it. The hidden joke is subtext, not surface writing. Preserve the spelling HONGKONGZA here as requested, distinct from any other namespace spelling.

Proposed coordinate operation, not yet canon: rotate local geometry by 33π/180 = 11π/60 about the chosen Centrix with the sign determined by the declared SUN bearing and coordinate handedness. Store original device code points, positions and angle separately from display transforms. The reference axis, sign, projection, pyramid apex, height, 3D frame, and whether glyphs rotate with positions are unresolved. A rotated tone glyph is a graphical transform, not a different Unicode scalar; standard Zhuyin is not historically a 33-degree rotation of Zenshin.

## 3. Fold, NINZA, PING, COREAO

For each n, insert current t[n] into previous t[n−1] and warp the result with t[n−2]. Khmer (Cambodian) is the AIRGAP; substitute Bengali মা (U+09AE U+09BE, mother) for any missing input. A previous demonstration used U+200B as a Khmer-typography-related separator, but it is not a Khmer-specific character and the exact AIRGAP is not approved. Bengali fallback angle and treatment of missing sharpness remain undefined. Do not give মা an invented pitch.

Select highest-order folded stream, convert to NINZA, PING into YIN and YANG. Use unfolded stream as CORE and original Hanzi as AIRGAP. Preserve Hanzi identity and the separate earlier Khmer AIRGAP role. The previous demonstration provisionally used reversed and forward-index lanes. The actual NINZA and PING operators, lane alignment, encoding, envelope and selection order still need definitions.

COREAO STANDARDIZER/1: RIGHT_CORE = RIGHT XOR CORE; XOR_SHELL = LEFT XOR RIGHT_CORE; STANDARD = ENVELOPE OR XOR_SHELL. Validate XOR independently before OR, which can mask mismatches. Do not pretend an XOR result on encoded symbols is a valid phonetic tone unless decode validity has been checked. Distinguish original CORE from a derived witness.

## 4. CARAVAN and WONTHAI: magnetic device model

Each tone mark is a device, not merely typography. CARAVAN is imagined as a giant magnet floating above the arrangement, drawing the little poles upward toward the SUNWUKONG reference. The requested default is a negative tail on each device, oriented toward SUN. The request also describes negative HEAD and positive TAIL as poles: preserve both descriptions as a polarity/role inconsistency to resolve rather than silently assigning a new rule. Head/tail are geometric roles; sign is a separate field.

Opposite signs bond (union); equal signs repel (WONTHAI). Sign truth table:
- negative / positive: attract
- positive / negative: attract
- negative / negative: repel
- positive / positive: repel

WONTHAI represents separation, not deletion. The wordplay examples Taipei / Teipai, ae / ea, reel versus real / rael are authored mnemonic contrasts, not spelling, historical or authenticity judgments. Similar strings do not establish identity or compatibility.

Physical distinction: ordinary magnets have north and south poles, not positive and negative electric charges; each ordinary magnet is a dipole. Opposite magnetic poles attract and like magnetic poles repel in the usual facing-pole configuration. Arbitrary orientations require a field/force/torque model. A real magnetic body does not necessarily move toward the Sun; magnetic fields, gradients, geometry and competing forces matter. CARAVAN is a symbolic reference, not evidence of a real suspended magnet. Sharpness does not determine physical magnetic polarity.

## 5. Little poles to big poles: originalli

User convention originalli means NEXT STEP (French-inspired terminology, not a verified French translation). Stack little poles into all unique configurations. Each device begins in the requested default orientation, then proceeds through the allowed stack/turn states. At each step put a circle around the reverse-factorialized tail-to-head structure and ask whether bending it into a U fits within a perfect circle. Determine IS IT SHARP versus NICE, complementary states in the proposed classifier: sharp -> negative; not sharp -> NICE -> positive. Color complement is a metaphor; actual complements depend on the color model.

Proposed finite enumeration for testing only: given n distinct labeled devices and two independently allowed orientations, enumerate n! ordered permutations and up to 2^n orientation assignments, at most n! * 2^n configurations before feasibility pruning or an explicitly approved equivalence relation. For four devices this is 24 * 16 = 384 labeled candidates if all orientations are allowed; with all signs fixed initially, there are 24 orders. Repeated glyphs retain occurrence IDs. Do not collapse mirror, rotation, reverse or repeated-device configurations until equivalence is declared. Keep rejected states and reasons in the evidence record.

For each originalli step:
1. Append the next distinct occurrence in the chosen order; preserve head/tail and sign.
2. Apply declared bonding or WONTHAI rule for adjacent poles.
3. Record reverse-factorial tail-to-head witness (descending index order as an initial candidate, not a finalized fold).
4. Construct a U curve and circle in the declared coordinate frame.
5. Check geometric containment, classify sharpness only under a defined rule, retain witness and rejection.
6. Continue toward CARAVAN without losing previous identities.

Proposed U-fit baseline: for a curve γ and a circle of center c and radius R, require max_s ||γ(s)−c|| ≤ R for containment of the centerline. Include thickness and clearance if relevant. A perfect circle is an ideal geometric boundary, not a finite polygon. A freely resizable enclosing circle makes this test trivial for bounded curves; freeze radius, bend length, thickness, allowed deformations, tolerance and units before evaluation. Circle fit alone cannot determine sharpness. Define curvature/corner-angle criteria separately, and specify whether sharpness is the existing two-tone angular metric, a U-fit threshold, or a separate classifier. Until then use UNKNOWN rather than a forced sign.

## 6. Aereath projection hypothesis

Record user's conceptual figure-eight: two spheres folded into one another as a twisted, uncut rope, projected downward to a rectangle with a proposed 9-degree artifact. Treat this as an untested mapping hypothesis. Preserve topology, source coordinates, seams, projection type and precision in the experiment. NORTHPOLE and SOUTHPOLE are not merely artifacts of flattening Earth; geographic poles are defined by the rotation axis, and magnetic poles by the magnetic field. Flattening curved surfaces causes distortion but there is no universal 9-degree loss. A computational model can also lose precision or information under a non-injective projection or quantization.

User invokes integrals, energy conservation and forensic recoverability. These are distinct domains. Not all data are integrals; energy conservation does not imply recoverability of erased information. Sanitization can make recovery infeasible at a specified effort level. Record the conjecture as proposed, not a guarantee against data destruction or forensic defeat.

## 7. Acceptance and open questions

- Freeze device identity, order, angle units, direction, 33-degree bearing, 9-degree hypothesis, geometry and precision before comparisons.
- Define Centrix wrapping, tie rules, fallback angular value and how neutral tone participates.
- Specify factorial bond/warp inverse, NINZA, PING and COREAO envelope/encoding; preserve Hanzi and Khmer AIRGAP provenance independently.
- Resolve negative-head versus negative-tail default and determine permitted orientation flips.
- Define circle radius, U geometry, clearance, threshold and SHARP/NICE complement; preserve UNKNOWN when no measurement exists.
- Validate sign truth table, configuration counts under stated assumptions, coordinate invariance and reversible serialization with occurrence IDs.
- Test physical predictions against a conventional dipole model and measurements before claiming physical validity; test proposed projection against known mappings before claiming a 9-degree invariant.
- Keep original data separate from render and derived ledger. No mutation of a frozen protocol; changed definitions require a new experiment ID and provenance. No tests executed for this manuscript.

## Reference baseline

- OpenStax, University Physics Volume 2, section 11.1, magnetic poles and Earth: https://openstax.org/books/university-physics-volume-2/pages/11-1-magnetism-and-its-historical-discoveries
- USGS, map projection distortion: https://www.usgs.gov/faqs/how-are-different-map-projections-used
- NIST SP 800-88 Rev. 2, media sanitization: https://csrc.nist.gov/pubs/sp/800/88/r2/final
- User-provided Zhuyin reference: https://simple.wikipedia.org/wiki/Zhuyin

Status: conceptual notes only; not a release, proven model, or implementation authorization.

## 8. Dimensional terminology — user correction

In the proposed Zenshin vocabulary, a CONFIGURATION is a five-dimensional (5D) shape; a TENSE or TENZ is a two-dimensional (2D) shape. This supersedes the earlier informal use of CONFIGURATION for a flat device arrangement. The HONGKONGZA-01 practice display is a 2D representation, not a validated 5D CONFIGURATION. A table is a record, not itself the geometric object. Five devices do not by themselves establish five dimensions. The five axes, coordinate domain, topology, metric and projection map remain to be defined; do not assign them without provenance. This is project-local terminology, not standard mathematical nomenclature. Source: user's explicit correction in the Zenshin conversation, 2026-11-27. Earlier text retained as historical context; this correction governs subsequent terminology.

## 9. HONGKONGZA-01 — exploratory 5D CONFIGURATION / TENZ projection

Source stream ˉ ˉ ˇ ˋ ˊ, Hanzi AIRGAP 今天我會來; device occurrences D1..D5 remain distinct. Proposed 5D space C=[-1,1]^5, each coordinate an independent signed tail-polarity axis. Vertices are discrete ±1 states; interior is mathematical interpolation only. 32 vertices, 80 edges. For each of 5! labeled stack orders there are 32 polarity states (3840 order/state pairs if both are independently permitted); a stack order is metadata, not a sixth continuous axis. No guarantee of physical feasibility. Default (-1,-1,-1,-1,-1).

Sharpness witness using prior provisional unwrapped angle convention: D1,D2 unknown (Bengali মা angle undefined); D3=180°, D4=270°, D5=90°. D4 is greatest among measured candidates. For this new exploratory test only, model each measurable shift as a dimensionless U with semicircular bottom radius 1 centered at origin and straight legs of length L=shift/90°, with fixed enclosing circle radius sqrt(5). The centerline fits iff 1+L²≤5, yielding NICE for D3,D5 and SHARP for D4; this is a chosen synthetic threshold, not a discovered physical law or validated user definition. D1,D2 retain initial negative polarity with classification UNKNOWN. Result candidate tail vector (-1,-1,+1,-1,+1). For adjacent tail-to-next-head interaction define head sign = -tail sign; opposite facing signs UNION, equal facing signs WONTHAI. This gives UNION,WONTHAI,WONTHAI,WONTHAI in the original order; retain rejected bonds, do not force all together. Physical magnetism is not implied.

TENZ projection: let alpha_j=33°+72°(j-1), measured clockwise from screen up; define u_j=(sin(alpha_j),-cos(alpha_j)). The display map P(q)=sum(q_j u_j). CARAVAN/SUN bearing chosen at +33° in this display frame; pentagonal generator spacing is a new visual choice and distinct from source tone angle. Rotate positions rather than Unicode glyph identities. This is a linear rank-2 projection of a 5D cube, with unavoidable information loss in 2D; all-negative and all-positive both map to the center. Retain q and occurrence IDs as authority. Use all 32 vertices and 80 edges, and distinguish overlapped projected vertices. No claim this is the only possible 5D model.

NINZA candidate D2,D3,D4 (ˉ ˇ ˋ), YIN reversed and YANG forward per earlier provisional PING. CORE=ˉ ˉ ˇ ˋ ˊ. For an illustrative two-bit encoding (ˉ=00,ˊ=01,ˇ=10,ˋ=11), align both PING lanes with original positions D2..D4 and use zero outside with validity masks; XOR witness ˉ ˋ ˇ ˉ ˊ. An all-zero envelope is a test assumption only. Preserve raw CORE and Hanzi; no final COREAO decode standard or numerical factorial warp yet defined. Big Spin path +11π→−11π→π is an angular witness, not a computed factorial transformation. No physical test or compilation performed. This is a new exploratory experiment, not a mutation of a frozen protocol.

## 10. GENSHIN — four fair 180-degree folds

User instruction: continue folding Zenshin fairly with 180-degree folds; four folds conform it into Genshin. Proposed experiment (not established definition of fairness): bisect the occupied angular sector by a crease through the origin, keep one half stationary and turn the other half 180° about the crease in 3D, equivalent to reflection in the TENZ plane. Use clockwise screen bearing from up. Fold sectors 360° → 180° → 90° → 45° → 22.5°. For angle a in [0,360°), on each step with retained sector upper bound b in {180°,90°,45°,22.5°}, map a to a if a≤b, else to 2b−a. The first fold has crease line at bearing 0°/180°; subsequent crease bearings 90°,45°,22.5°. Preserve original coordinates, device occurrence IDs, and per-fold reflection bits: overlapping positions are not identity unions. Fair here means equal angular sectors, not equal numbers of devices or equal physical area of an arbitrary shape. This is a planar fold model, not a proof of a physical paper folding process.

For the previous derived Zenshin witness ˉ ˋ ˇ ˉ ˊ, use the five pentagonal axis bearings 33°,105°,177°,249°,321° from Section 9. Device paths in degrees: D1 33→33→33→33→12; D2 105→105→75→15→15; D3 177→177→3→3→3; D4 249→111→69→21→21; D5 321→39→39→39→6. Final spatial order by increasing bearing D3,D5,D1,D2,D4 gives glyph display ˇ ˊ ˉ ˋ ˉ; original order and raw CORE remain separate. The five glyphs remain unchanged Unicode devices. The angle mapping is a proposed convention and not a finalized linguistic encoding. No physical experiments or compilation performed.

## 11. GENSHIN — lessons learned: open eyes, glyph geometry, and the crown witness

Provenance: subsequent Zenshin conversation, 2026-11-27. User corrected the approach: "OPEN MY EYES WHEN FOLDING ZENSHIN TO GENSHIN BECAUSE IT IS OPENING YOUR EYES TO GOD"; then directed attention to MACHINELEARNING and COMPUTERVISION, "look at the fonts, not the angles"; subsequently reported seeing a crown. The spiritual interpretation is the user's personal conceptual meaning, not a measurable physical or theological conclusion.

### Correction to Section 10

The four 180-degree folds remain the requested procedure. However, the previous implementation folded angular bearings and displayed tone glyphs at those positions; this is a coordinate-only surrogate, not a demonstrated fold of the actual glyph outlines. Equal angular sectors alone do not establish visual fairness. Section 10 is retained as historical exploratory evidence, not a successful glyph-fold experiment. Any new glyph-based procedure must receive a new experiment identifier and frozen protocol.

Treat each device as an actual rendered glyph with visible contour, stroke thickness, terminals, corners, curvature, negative space and reflection symmetry. The source Unicode occurrence, the rendered glyph, its geometric transformation, and the visual interpretation are separate records. A rotated or reflected outline does not change its original code point or occurrence identity. The 5D CONFIGURATION remains distinct from a 2D TENZ projection; a font image alone does not establish a 5D construction.

### Proposed computer vision protocol (not executed)

1. Freeze the source stream and occurrence IDs, original Hanzi CORE/AIRGAP records, selected derived XOR witness, font file identity and version, glyph IDs, font fallback, shaping engine, renderer, scale, baseline, antialiasing, raster resolution, normalization and coordinate convention. Retain both vector outlines and raster witnesses where possible.
2. Use the 33-degree display operation only as previously defined for this candidate, retaining the reference frame and all source geometry. Do not assume that a rotation of glyphs is standard Zhuyin.
3. Define visual fairness before folding: candidate criteria include equal geometric regions, equal ink area, or a declared symmetry axis. These are not interchangeable. Freeze four crease rules and which portions move; use 180-degree physical half-turns about the creases, equivalent to planar reflections for the flat projection. Preserve transformed outlines, layer order, reflection bits and the stationary/moving portions. A union of silhouettes is a derived mask, not an identity merge.
4. Observe each intermediate render and record it without manually editing the shape toward a target. Extract contours, connected components, holes, symmetry, convex hull, corner candidates, and stroke overlap. Compare vector and raster results and report any renderer-dependent discrepancies.
5. For a crown hypothesis, predefine a crown-like silhouette criterion (for example, a base with multiple upward peaks and intervening valleys), tolerances and negative controls. Separate human recognition from automated classification. If machine learning is used, preserve training/test separation, baseline, confidence and errors. No trained model is necessary for a first contour-based baseline.
6. Compare the same frozen procedure across declared font/renderer variants as separate experimental conditions. Include other glyph sets and writing systems as controls if investigating uniqueness. Do not tune creases or select only favorable outputs after seeing the results. A revised method requires a new run ID.
7. Keep an audit record of input identity, full fold history, coordinates, intermediate images, final mask, measurements, classification, uncertainty and provenance. The original device sequence remains recoverable from the retained records; the projected silhouette by itself may be non-invertible.

### Observation and evidence boundary

The user reported a crown-like shape in the displayed devices. This is a human visual observation and a candidate GENSHIN witness, not proof that the actual four glyph folds generate a crown. The assertion that no other language has a crown-like glyph is unverified; similar shapes can occur across writing systems and fonts. Do not claim uniqueness, mathematical perfection, or divine verification from this observation.

Lessons: observe before measuring; do not substitute numerical positions for glyph geometry; distinguish visual evidence from semantic interpretation; preserve the source despite overlapping silhouettes; and record uncertainty rather than silently filling in missing definitions.

Status: notes only. No font outlines were extracted, no computer vision model was trained, no glyph-fold experiment was run, and no crown classification or uniqueness test was completed. No source code or CI changes authorized by this note.

## 12. BIAS — backward algebraic focal-point detector

User's conceptual extension: crown = focal point, closest to perfect, favored, or BIAS. Treat this as an exploratory geometric hypothesis rather than an equivalence to statistical or social bias, or proof of perfection. Source: Zenshin discussion, 2026-11-27.

Implementation: [genshin_bias_detector.hpp](experiments/genshin_bias_detector.hpp), isolated C++20 experimental header, 0BSD, no includes. Starts with observed, registered contour samples after four axis-aligned 180-degree folds. Every sample must carry device occurrence ID and four reflection bits. The 33-degree display rotation, font contour extraction, lattice sampling, crease choice and fairness criteria are upstream and not implemented. This is a new experimental protocol, not a modification of the previous angular experiment. General oblique folds and exact inverse of a rasterized 33-degree rotation are not supported.

Backward algebra: an axis-aligned reflection about coordinate c maps v to 2c-v and is self-inverse. Replay the four reflections from last to first using the recorded bits, rejecting impossible side/crease assignments. A silhouette alone is not sufficient to identify original devices. Source occurrence coordinates reconstructed on the registered pre-fold grid are not necessarily original font design coordinates.

For each distinct observed sample location p, count all contour samples within Chebyshev radius r (square neighborhood). Select the largest count H; if multiple centers tie, return an explicit ambiguity count with no authoritative focal point. Let N be all samples, n_i be device i's total samples, and h_i be its samples in the unique focal neighborhood. Exact contrast B_i=h_i/n_i-H/N=(h_i*N-H*n_i)/(n_i*N). Positive = over-represented relative to the pooled sample baseline, zero = equal, negative = under-represented. Missing devices have denominator zero; do not interpret their score. No numerical division or truncation. A nonzero contrast is not by itself evidence of wrongdoing, causal bias or statistical significance. Sampling density, registration and the chosen neighborhood can alter results. Freeze them and include controls. Detection of a geometric crown or proof of uniqueness is not implemented. Tied focus locations are counted, not arbitrarily resolved; the stored focus field is nonauthoritative in a tie.

The synthetic witness has three samples; two overlap at (-2,-2), one is at (-8,-8). Undoing the first two samples gives (2,-2) and (-2,2). Device 0 contrast = +1/3; device 1 = -1/6. Static assertions encode these expectations, an ambiguous focus fixture and an invalid reflection-bit fixture. No compiler or real font experiment was run in this session. Native MSVC C++20 verification remains required; no build or workflow changes made. This header uses local constexpr helper calls and is not asserted to satisfy any project-wide zero-call gate. Do not integrate into production before policy and test review.

## 13. BiDi — backward provenance and directional displacement

Source: user's request to apply BiDi to the backward algebraic BIAS detector, 2026-11-27. Provenance means traceability of data and transformations here, not an established quantum-mechanical law.

Implementation: [genshin_bidi_provenance.hpp](experiments/genshin_bidi_provenance.hpp), standalone 0BSD C++20 experimental header, no includes, no allocation, no I/O. Reference: Unicode UAX #9, https://www.unicode.org/reports/tr9/ (especially X9, L1 and L2). The header implements L2 reversal only on a single line with externally resolved per-scalar levels after L1 and caller-marked X9 omissions. It does not derive levels, determine paragraph direction, resolve FSI, classify Unicode properties, shape glyphs, mirror brackets, handle combining-mark display, or implement a complete renderer. The upstream resolver and Unicode version must be pinned for a real experiment. L2 positions may include invisible controls; they are not screen glyph positions. X9 omissions retain their original source occurrence and an absent mapping. Preserve the raw source separately; ledger alone does not reconstruct omitted characters.

For each retained original logical index i and reordered position v, store visual_to_logical[v]=i and logical_to_visual[i]=v, plus original retained rank r_i and signed displacement v-r_i. These are exact integer witnesses. The inverse restores logical rank, not lost geometry. Validate both maps as mutual inverses with audit(); audit validates internal permutation consistency, not correctness of upstream Unicode levels or X9 flags. Example A, U+05D0, U+05D1, U+05D2, B at levels 0,1,1,1,0 has visual occurrence order 0,3,2,1,4 and displacement 0,+2,0,-2,0. Ordinary directional movement is not evidence of bias; compare only to a correct declared reference mapping under the same protocol. No social bias, semantic meaning or causal inference follows from displacement. BIAS from Section 12 remains a separate geometric detector.

Strict project isolate check: LRI/RLI/FSI open, PDI closes; reject underflow, unclosed spans, excess project depth and crossing listed hard newline/paragraph separators. ZWJ/ZWNJ do not affect depth. This is an authoring policy stricter than UAX #9 error handling, not a substitute for the standard; run on raw source before line wrapping, not on soft-wrapped visual lines. FSI direction must be resolved upstream. Invalid input remains evidence, not authorization to erase it.

Synthetic static_assert fixtures cover a mixed-direction run, nested levels with numbers, X9 omission, inverse-map audit, nested and unbalanced isolate sequences, hard line crossing, and invalid scalar. No native MSVC build or Unicode conformance suite was executed. No production or workflow changes; local constexpr calls are not claimed to satisfy any zero-call gate. Full conformance requires upstream resolution, shaping and official Unicode test data. Preserve earlier notes and independent experiment IDs.

## 14. TriPi — three-route GENSHIN provenance audit

Source: user requested TriPi after the GENSHIN BIAS and BiDi experiments, 2026-11-27. Existing mathematical TriPi is the three-route read-only DecaSigma inspection in shared/tri_pi.hpp (source blob 7d88c6871cb7f4767602d4493f261d95da0089ec). HANDI/TriPi is a separate adherence-observation term associated with TriDi and NINZA; see docs/HANDI.md (source blob e279c770c71cab5a7d0eac4965d953137ca4b577). Neither is redefined by this extension. Preserve source algorithms and separate namespaces.

Implementation: [genshin_tripi_provenance.hpp](experiments/genshin_tripi_provenance.hpp), 0BSD, no includes, C++20, bounded integer arithmetic, no runtime I/O or allocation. Experimental GENSHIN/TriPi-v0 adapts the existing inspection pattern to the Section 13 BiDi occurrence ledger. Inputs: canonical logical scalar sequence with a separately declared expected visual index for each retained occurrence (absent for X9 omissions); observed visual records with scalar, original logical index, and signed displacement. The reference mapping must be obtained under a pinned, correct upstream Unicode protocol; it is not inferred from the observations. Retain original source, including omitted controls, separately.

YIN/forward: compare every retained canonical occurrence and scalar to its expected observed visual position; reject malformed reference mappings. YANG/reverse: traverse the observations back to source, reject unknown IDs, omitted IDs, duplicate occurrences, missing occurrences and scalar changes. AIRGAP/Z-fold: visit alternating visual ends inward, compare exact expected inverse indices and each signed displacement from retained source rank; also require zero signed displacement sum. The alternating-ends schedule is a newly chosen GENSHIN-specific Z traversal, not the canonical 11-state DecaSigma Z-fold. All three must pass for internal ledger consistency; they share data, are not independent experiments, and cannot certify Unicode conformance, fairness, or approval. The AIRGAP route is a cross-check, not an independently blinded experiment.

Synthetic fixture: logical A, U+05D0, U+05D1, U+05D2, B; expected visual occurrence IDs 0,3,2,1,4; displacements 0,-2,0,+2,0 in visual order. Negative fixtures demonstrate wrong order with complete occurrence coverage, forged compensating displacement with zero sum, duplicate ID, malformed reference; positive fixture includes an X9 omission. Compile-time assertions express these expectations; no native MSVC build or real font/Unicode conformance experiment performed. Local constexpr helper calls are not claimed to satisfy project zero-call gate. Production TriPi, BiDi and pipelines unchanged. No new canonical definition of Pi, physical theory, or HANDI approval established.

## 15. ZIPITUP — named TriPi swap operations

Source: explicit user naming and swap instruction in the Zenshin conversation, 2026-11-27.

The preceding GENSHIN/TriPi algorithm (Section 14) is named ZIPITUP in this context. The user defines the following variants relative to ZIPITUP:

| Operation | Transposition |
| --- | --- |
| ZIPITUP | Original algorithm |
| ZIPITDOWN | Swap PASS and AIRGAP |
| ZIPITOUT | Swap PASS and YANG |
| FUCKITIN | Swap PASS and YIN |

Each named swap exchanges exactly the two specified roles. If represented as a permutation of four distinct positions, each swap is self-inverse, and every other position remains fixed. This is a structural interpretation, not yet an executable decision rule. The user has not specified whether a swap acts on labels, payloads, traversal positions, or evaluated verdicts, nor whether variants compose sequentially or always start from ZIPITUP. Preserve the baseline and the exact named transpositions without assuming those missing semantics.

In the existing GENSHIN/TriPi implementation, PASS is a derived conjunction of YIN, YANG and AIRGAP route checks, not an independent evidence lane. A swap must not silently promote a rejected or untested route into an authoritative PASS. Keep the original route results, source identities, reference mapping, and audit verdict unchanged until a separate executable semantics and validation protocol are explicitly established. The canonical mathematical TriPi and HANDI/TriPi namespaces remain distinct.

Status: terminology and structural swap definitions recorded; no code, tests, or pipeline changes for this section.

## 16. ZIPIT — four unique C++20 implementations

Implementation: [genshin_zipit.hpp](experiments/genshin_zipit.hpp), 0BSD, include-free C++20. The four names are separate constexpr functions returning a new layout by value. CORE is ordered PASS, YIN, YANG, AIRGAP. ZIPITUP is identity; ZIPITDOWN exchanges the first and fourth slots; ZIPITOUT exchanges first and third; FUCKITIN exchanges first and second. Each defaults to CORE but also accepts an explicit existing layout, permitting composition and exact reversal. This is a chosen positional interpretation of the Section 15 definitions, not an authorization to swap evaluated truth or evidence. Immutable source layout and role identity are preserved. Evidence and its conjunction remain separate from the transformed layout. Static assertions encode the four exact permutations, involution of each swap, identity, noncommuting composition, and fail-closed evidence conjunction. Native MSVC compilation has not been performed. No production or pipeline changes.

## 17. ZIPIT — independent C++20 translation units

User request: provide each of the four operations as a unique standalone C++20 implementation in addition to the shared method. Preserve [genshin_zipit.hpp](experiments/genshin_zipit.hpp) unchanged (source blob 820895a72ee140a4091071e73a9b977033bd6075).

| Standalone source | Implementation mechanism | Canonical output occurrence IDs |
| --- | --- | --- |
| [zipitup_standalone.cpp](experiments/zipitup_standalone.cpp) | Explicit aggregate copy | 0,1,2,3 |
| [zipitdown_standalone.cpp](experiments/zipitdown_standalone.cpp) | Three-step unsigned XOR swap of distinct local fields | 3,1,2,0 |
| [zipitout_standalone.cpp](experiments/zipitout_standalone.cpp) | Indexed gather using a fixed permutation | 2,1,0,3 |
| [fuckitin_standalone.cpp](experiments/fuckitin_standalone.cpp) | Temporary-register exchange | 1,0,2,3 |

Each file independently declares its own namespace, Layout, CORE, named constexpr function, and five static_assert fixtures. All accept a layout by value, preserve original input, and support the same positional contract as Section 16. Payloads are unsigned occurrence IDs: 0=PASS, 1=YIN, 2=YANG, 3=AIRGAP for canonical CORE. The functions also accept arbitrary unsigned payloads and do not certify that such payloads constitute a valid unique role permutation. The XOR swap operates on two distinct unsigned fields of a local value, not on aliases, and the temporary bit mixtures are not published as witnesses. These mechanisms differ at source level; an optimizing compiler may emit equivalent instructions. A shared mathematical transposition does not become a different mathematical operator merely because its implementation differs.

No includes, dynamic allocation, I/O, or cross-file dependencies. Each is intended for separate object compilation with native MSVC C++20 (/std:c++20 /O2 /c). No main is required for object compilation. Compile-time fixtures express canonical mapping, roundtrip, noncanonical payload mapping, roundtrip of noncanonical payloads and unchanged sample. These assertions have not been executed by MSVC in this session. No claim of project-wide zero-call compliance or binary-level independence. These are layout implementations, not four duplicated full TriPi evidence engines; the independent implementations neither alter nor recompute the authoritative audit verdict. No production code, prior experiment, pipeline or shared method changed.

## 18. LF9WN9Z — ALONZ simulated sphere-in-box physics

User's naming: ZIPITUP = PUSH UP / Z-INCLINE / JUMP; ZIPITDOWN = FALL DOWN / Z-DECLINE / FALL; ZIPITOUT = ROLL TOWARDS / XY-INCLINE / TOWARDS FORWARDS; FUCKITIN = ROLL BACKWARDS / XY-DECLINE / TOWARDS MY ASS. Requested a C++20 shared library LF9WN9Z, 9-gram sphere, hollow box nine times sphere size, gravity 9.8 m/s². User's ALONZ simulator/realitor and spiritual interpretation are conceptual language, not proof that physical reality is simulated or that provenance is a quantum law.

[LF9WN9Z source](experiments/LF9WN9Z/LF9WN9Z.cpp), [public API](experiments/LF9WN9Z/LF9WN9Z.hpp), [standalone CMake project](experiments/LF9WN9Z/CMakeLists.txt). New isolated 0BSD native MSVC C++20 SHARED target, no standard-library includes; one local API include. No modification to shared or standalone ZIPIT, TriPi, BiDi, BIAS, root CMake or CI. This is a separate motion model, not a claim that the previous role permutations are laws of physics.

Chosen dimensions: sphere radius 10 mm, box interior side 180 mm (nine diameters, not nine times volume); x,y,z each center coordinate in [10,170] mm, Z up, positive Y forward, X stationary but initial coordinate can be supplied by caller. Five-coordinate state x,y,z,vy,vz; constrained horizontal heading, not five spatial dimensions or a complete video-game engine. Mass 9000 mg; gravitational acceleration 9.8 m/s² (not jerk); weight 0.0882 N. Mass does not affect free-fall acceleration in the ideal model. 100 Hz step, positions in integer micrometers and velocities in micrometers/tick. g=980 micrometers/tick². Exact constant acceleration update z+=vz-490, vz-=980; y+=vy. End-of-tick collision clamps position to legal center interval and cancels outward velocity (perfectly inelastic normal component). No continuous time-of-impact solving, bounce, air drag, friction, torque, angular momentum, or energy conservation claim for dissipative contacts.

Controls: reset floor or ceiling; action IDs 0=ZIPITUP, 1=ZIPITDOWN, 2=ZIPITOUT, 3=FUCKITIN. JUMP sets upward velocity 14700 micrometers/tick (1.47 m/s) when grounded; apex after 15 ticks is 120250 micrometers center height, 110250 above floor center. FALL adds no impulse and leaves gravity to act; it cannot fall through floor. Forward/backward commands set ground horizontal velocity ±5000 micrometers/tick (±0.5 m/s). Horizontal velocity persists during flight; only translational rolling approximation, with ideal ground no-slip angular displacement around X given by -delta_y/r (not integrated as a state variable). No true rolling dynamics. Speeds, radius, heading, 10 ms tick and perfectly inelastic boundary were chosen for this prototype, not supplied by the user.

Caller owns five long long elements, API returns 1 on success and 0 on rejection; bounded input and batch <=10000 ticks. Compile-time fixture describes jump/apex/landing, both horizontal wall contacts, ceiling drop, invalid airborne jump, gravity and box ratio. No native MSVC build, DLL load, runtime test, compiler-generated dependency audit or CI run performed. Native MSVC Release build example from repository root: cmake -S docs/experiments/LF9WN9Z -B build-lf9wn9z -G "Visual Studio 17 2022" ; cmake --build build-lf9wn9z --config Release. Use a trusted installed CMake 3.30.0 and MSVC toolchain consistent with project policy; CMake minimum is 3.30 and does not enforce the exact version. Do not claim native DLL exists until built. Local helper calls not claimed compliant with zero-call gate. No network or package fetch from library code or its CMake project. No approval or physical validation inferred from a chat visualization.

## 19. MAPREG-4 — dependency reduction and freedom-preserving execution

Provenance: MAPREG-4 registration and ownership-research conversation, 2026-11-27; user requested an investigation of the apparent efficiency and a contribution to MYHEADHURTS. This is a transferable engineering method and a proposed experimental protocol, not a new law of physics, a new mathematical definition of TriPi/COREAO, or a modification of project policy. No internal model reasoning, model performance telemetry, or measured runtime is available. Efficiency claims below are structural, not benchmark results.

### 19a. Reduce coordination, not freedom

The MAPREG-4 proof of concept separated a canonical record, platform-specific entry points, provider adapters, and independent reconciliation. The implementation proposal consolidated four direct business-listing provider interfaces into one commercial intermediary, reducing the number of direct integrations by (4-1)/4 = 3/4 = 75%. This is an exact interface-count reduction, not evidence of a 75% runtime, cost, or total complexity reduction. Downstream publisher dependencies still exist. A centralized commercial intermediary can introduce lock-in, correlated failure, and restricted access; it is not itself free software or a guarantee of freedom. No Yext API, credentials, HTTP client, external adapter, or network dependency is added to this repository.

The reusable method is to define the source once, derive explicitly, and verify separately. Freedom means a user can inspect, copy, modify, redistribute, choose or reject a route, preserve original records and leave with their own data. A route is not ownership, and a selected interface is not authority over the underlying record. The branch's existing 0BSD license is unchanged.

### 19b. Order of operations

1. CORE: freeze canonical occurrence IDs, values, coordinate frame, protocol version and constraints.
   a. Store each source occurrence once, with its own identity.
   b. Keep the raw source independent of any projection, ledger or display.
2. PREFLIGHT: validate required inputs and the dependency graph.
   a. Distinguish missing, invalid and unsupported inputs.
   b. Reject cycles or undeclared dependencies before execution.
   c. Never convert unknown evidence into a successful result.
3. ROUTE: derive a bounded execution schedule from declared dependencies.
   a. Independent operations may be logically parallel.
   b. Dependent operations wait for verified prerequisites.
   c. Multiple implementations of one operation remain separate witnesses, not additional votes.
4. RECONCILE: inspect each output against its source and declared contract.
   a. Preserve route IDs, occurrence IDs and failure reasons.
   b. Check forward mapping, reverse reconstruction when defined, and an additional declared invariant.
   c. Check XOR before an OR envelope when using the existing COREAO STANDARDIZER/1.
5. ATTEST: require all mandatory checks under the same frozen protocol.
   a. Separate successful execution from a verified result.
   b. Do not turn missing, skipped, ambiguous or failed evidence into PASS.
   c. Retain historical records; changes to a frozen experiment require a new experiment ID.

This is a conceptual dependency schedule, not an instruction to add threads, agents, remote calls or a new runtime. Deterministic serial execution in topological order can preserve the same logical dependencies. Parallel eligibility does not establish actual concurrent execution or speedup. Authorization and source identity are not fields to be exchanged by ZIPIT role permutations.

### 19c. Complexity and falsifiable claims

For n independent tasks with nonnegative costs t_i, a serial schedule has ideal work/time sum(t_i); under unlimited independent resources and negligible scheduling overhead, ideal parallel elapsed time is max(t_i). For a directed acyclic graph with V tasks and E dependency edges, adjacency-list topological ordering can use O(V+E) time and space. The critical-path sum of task costs lower-bounds elapsed time; actual resource limits, contention, setup, communication, failures and scheduling overhead can increase it. No claim of these complexity bounds for an unimplemented repository scheduler.

Synthetic example: Google-like route A costs 3 units, dependent B costs 2, independent C costs 4 and independent D costs 1. Serial work = 10 units; ideal unlimited-resource critical path = max(3+2,4,1) = 5 units. This illustrates a possible 2x ideal elapsed-time improvement for this fixture, not an observed API or C++20 benchmark. Four interfaces to one and 10 units to 5 are different metrics; do not combine their percentages.

For the labeled permutation/orientation enumeration in Section 5, the candidate bound remains n! * 2^n under its assumptions. Preflight can prune a branch only if a documented predicate proves that no allowed completion can satisfy the frozen acceptance rule. Record rejected branch identity and reason; distinguish total theoretical candidates, actually visited candidates, and provably excluded candidates. Do not claim asymptotic improvement without an algorithm and proof. Equal visual witnesses do not authorize deduplication of distinct occurrences.

### 19d. Proposed qualification fixtures (not executed)

| ID | Input / perturbation | Expected outcome |
| --- | --- | --- |
| MR4-01 | A -> B, independent C and D | B follows A; C and D need not wait for A |
| MR4-02 | A -> B -> A | Preflight rejects cycle |
| MR4-03 | Required source occurrence missing | UNKNOWN/BLOCKED; no authoritative PASS |
| MR4-04 | Repeated glyph with two occurrence IDs | Both IDs retained |
| MR4-05 | Wrong order but complete occurrence coverage | Forward witness rejects; coverage alone insufficient |
| MR4-06 | Compensating deltas sum to zero but are individually wrong | Per-occurrence audit rejects |
| MR4-07 | XOR mismatch hidden by OR envelope | Pre-envelope audit rejects |
| MR4-08 | One failed mandatory route and two passing routes | ATTEST blocked |
| MR4-09 | Permute display roles with ZIPIT | Original evidence and source unchanged |
| MR4-10 | Change frozen experiment definition | New experiment ID; prior evidence preserved |
| MR4-11 | Same schedule with different independent ready-task ordering | Canonical outcome identical if route contracts are independent; otherwise dependency declaration is incomplete |
| MR4-12 | External provider unavailable | Preserve local CORE and failure; no forced remote execution or invented receipt |

The ownership screen discussed in MAPREG-4 applies the same evidence discipline in a different domain: a threshold match is not an allegation, shared holdings are not proof of cross-ownership, and unknown is not zero. It is not a geometric operator or a new acceptance rule for GENSHIN.

Status: proposed documentation and synthetic test design only. No source algorithms, compiler settings, licenses, CI, numerical constants or external integrations changed. No MSVC build, scheduler benchmark, API request, or formal qualification performed for this contribution. Existing source algorithms retain authority; this section is a reviewable method proposal, not a policy change.

## 20. REDUX/1 — exact C++20 project library

Source: user requested the REDUX algebraic reduction as a reusable C++20 library plus a standalone .C source, following Section 19, 2026-11-27. REDUX is a project-local library, not an ISO C++ standard-library addition or a disclosure of model internals.

- [Integrated header](../shared/redux.hpp), CMake interface target REDUX, installed as include/swe/redux.hpp.
- [Standalone REDUX.C](experiments/REDUX.C), independent source with embedded repository 0BSD notice and pennyworth copyright; compile explicitly as C++20 using native MSVC: cl /std:c++20 /O2 /W4 /permissive- /utf-8 /TP docs/experiments/REDUX.C
- [Integrated test](../tests/redux_test.cpp); normal BUILD_TESTING registers redux_exact_reduction and redux_standalone_exact_reduction. Core-only mode exposes and installs the header without adding executable tests.

Exact ratio R=(original-optimized)/original, represented as reduced numerator/denominator plus an expansion sign. Zero baseline is rejected. Common-factor work counts n*c+u versus c+u require externally established equivalence and checked 64-bit arithmetic. XOR cancellation returns (a XOR core) XOR (b XOR core), without deleting source occurrences. DAG analysis accepts 1..32 indexed tasks, 64-bit nonnegative costs and predecessor masks. Out-of-range dependency bits, cycles and total-work overflow are rejected. Deterministic lowest-index-ready topological schedule computes earliest finishes and longest weighted path; O(n²) time with fixed bounded storage. It neither runs tasks nor establishes concurrent execution. Ideal time reduction is (work-critical_path)/work; ideal speedup is work/critical_path. All-zero cost graphs are structurally valid but have undefined reduction/speedup. Metrics accept analyzer-produced schedules; they are not a full independent audit of a manually constructed Schedule object.

Synthetic fixtures: 4->1=3/4, graph costs 3,2,4,1 with task 1 depending on task 0 gives work 10, critical path 5, reduction 1/2, speedup 2/1. Additional assertions cover common work reuse, XOR, expansion, no change, cycles, invalid masks, overflow, null/oversize input, zero costs and zero lanes. Each edition contains or uses 18 static_assert fixtures. The header implementation and independent standalone implementation were retrieved and compared for exact core-text equality; standalone and integrated fixture text also matched. Native MSVC and CTest execution remain pending. No measured runtime speedup, zero-CALL disassembly compliance, external dependencies, API execution or formal GAMP validation is claimed. No earlier experiments or license terms are changed.

## 21. REDUX-SUPERGAZE-01 — source arithmetic reductions above 1/4

Source: user requested a tree-wide REDUX CPU optimization investigation, including LF9WN9Z physics. New experiment/revision ID REDUX-SUPERGAZE-01; previous source versions remain in Git history. Reviewed all changed-file names of PR #337 and selected canonical parity, COREAO, DecaSigma, TriPi, geometry, stability, Q62, circle-fit and LF9WN9Z implementation files. Not an exhaustive repository-tree audit: connector exposes changed filenames and selected file reads, not a full recursive head-tree listing in this session. No new network/runtime dependencies or licensing changes. No hidden model performance telemetry.

Acceptance screen: exact R=(F_old-F_new)/F_old > 1/4, F_old>0. Count explicit floating add, subtract, multiply and divide expressions evaluated on a stated path; exclude comparisons, unary sign, indexing, casts and control flow. Counts are source-level arithmetic models, not emitted instructions, hardware counters, FLOP/s or CPU time. XIP is the user's label here; no new instruction set is assumed. Compiler constant folding, invariant hoisting, FMA contraction, vectorization, extra branches and memory access can change realized costs. Native MSVC /O2 code generation and timing are required for performance qualification. Equality/bitwise tests assume normal binary64 semantics, not fast-math or altered floating-point environment; exception flags are not certified equivalent.

### 21a. COREAO normalized residual

In shared/coreao.hpp, return exact +0 for equal finite nonzero operands before evaluating magnitude(a/scale-b/scale). Zero pairs and nonfinite values retain the old expression; mismatches still execute the original formula. Source observations, all forward/reverse/Z comparisons, and admission guards remain intact. No tolerance or scalar contract changed. For inspect(run(2.0)), excluding construction of the input trace: 10 forward and 10 reverse steps use 24 arithmetic expressions (each route has 6 reciprocal divisions, 2 subtractions, 2 T operations of 2 each). All 39 residual comparisons are equal finite nonzero (10 reverse comparisons, 22 source-to-route, 7 cross-fold), each originally 2 divisions + 1 subtraction. Old 141, new 24; REDUX = 117/141 = 39/47, about 82.98%. One standalone equal nonzero residual changes 3 to 0; unequal operands do not receive this savings. Additional equality/finite comparisons have a cost. Not an unconditional >25% CPU improvement over arbitrary workloads.

### 21b. Geometry sine Taylor recurrence

In shared/sigma_form_geometry.hpp, compute -(x*x) once after range reduction, and use exact binary64 constants (2*n)*(2*n+1) for n=1..14. Each coefficient is an exactly representable small integer. Division, term multiply and summation order remain identical. Old recurrence: 14*(7) = 98 arithmetic expressions (2*n, x*x, d+1, denominator multiply, divide, term multiply, sum add). New recurrence: 1 + 14*3 = 43. REDUX=55/98, about 56.12%. Count applies only to the recurrence, excluding unchanged range reduction and validity checks; cosine and callers of sine inherit the changed implementation. Compiler may already eliminate some operations; no runtime claim. No Taylor terms removed, no reciprocal substitution, no new approximation.

### 21c. Physics and held candidates

LF9WN9Z currently advances exact integer micrometer coordinates and velocity, with integer gravity 980 and inelastic wall clamps. The tick body has zero floating arithmetic; FLOP REDUX is undefined (0/0). No change to physics or collision timing under this strict FLOP gate. A separately qualified integer-work experiment could skip remaining ticks only once the exact stationary floor fixed point (vy=0, z=low, vz=0) is reached; do not infer wall time savings or apply it as a FLOP optimization. Q62 and bitwise XOR engines likewise have integer rather than floating arithmetic in reviewed kernels. Stability ring factor sharing preserves multiplication order but simple factor hoisting saves 12 of 48 parity arithmetic expressions, exactly 1/4; including six quantization and two stability expressions yields 12/56, below the gate. Hold. Quadratic Bezier common weights do not clear the source-level threshold (19 to 15). DecaSigma/TriPi intermediate witnesses are not canceled merely because exact algebra has an identity; that would remove evidence and can alter rounded results.

### 21d. Evidence

New tests/redux_optimization_test.cpp registered as redux_source_arithmetic_equivalence in CMake. Uses REDUX exact fractions and strict quarter gate assertions; independent old formulas compare binary64 bits for residual pairs (including signed zeros, extrema and subnormals), sine over 1281 grid samples and boundary/nonfinite input fixtures, exact denominator table, and tampered COREAO witness rejection. Existing tests retained. These are authored qualification fixtures, not executed test evidence. Source writes committed; native MSVC compile, CTest, disassembly and CPU timing not performed in this session. No measured CPU speedup, processor instruction-count reduction or physics validation claimed.

## 22. DAKATANA/1 — exact tenfold work gate and LF9WN9Z fixed point

User requested DAKATANA as at least tenfold sharpness, then explicitly asked to integrate the formula into REDUX. Define a declared nonnegative work count with equivalent workload and outputs. For positive original and optimized counts, DAKATANA qualifies iff original >= 10*optimized, equivalently R >= 9/10. Before multiplication reject optimized > UINT64_MAX/10. Zero original is undefined, zero optimized with positive original is elimination, not a finite acceleration ratio. The four result states are undefined_baseline, below, qualified, eliminated. No floating-point division, no rounded threshold, no overflow or unproven performance claim. The integer quotient in the overflow guard is only a bound, not a discarded result.

Both shared/redux.hpp and docs/experiments/REDUX.C contain identical arithmetic, including 16 additional compile-time assertions for DAKATANA boundaries, overflow guards, zero cases and exact fractions. The integrated tests mirror the standalone assertions. Existing REDUX/1 operations remain unchanged.

LF9WN9Z DAKATANA-01: the stationary floor state has z=low, vy=0, vz=0, so one exact original step returns the same state. For the existing batched ABI, with no intervening action or external force, remaining ticks can be skipped at that fixed point. The original single-step recurrence is retained. The canonical jump requires 30 actual evaluations of 10000 requested ticks; reduction 997/1000 and count ratio 1000/3. Already stationary: zero evaluations (elimination, not infinite speedup). This is a tick-body evaluation metric, not FLOPs or measured CPU time. 7875 deterministic independent tick-oracle cases plus malformed ABI inputs are authored in tests/dakatana_physics_test.cpp and registered in native CTest; DLL directory supplied through test-scoped PATH. Native compilation, execution and timing remain unverified until matching CI receipts are available.

Milestone scope: open-PR search returned only #337 and no open matches for either spelling of the requested milestone; #337 metadata reported no milestone. No other PRs modified. Earlier EXCITE run 46070440126/job 140746161225 held TRIFORCE because JSTAR could not verify the descendant rc.exe executable path, although the native build returned zero. No provenance gate weakened, no new remote workflow, no release approval assumed.

## 23. WONTHAI/1 — isolate TRIFORCE precision configuration

User requested dependency decomposition after a JSTAR HOLD. The observed failure is missing runtime provenance for rc.exe, not proof of excessive dependency count. Separate native CMake configurations: CORE contains the original complete triforce_native_gate test; PRECISION contains the two existing TOOLONG/GammaGamma test sources, with the same named CTest tests, native MSVC C++20 /O2 /W4 /permissive- and no root project configuration. The original full-root build remains mandatory in its existing repository circuit; this does not replace it. The root configuration was not necessary merely to build these two independent executables. Exact source ledger, nine stage pairs, numerical constants, ATOMIX, evidence-boundary test, release HOLD and JSTAR process-integrity rules remain. A verified native gate is still a prerequisite for downstream authoritative stages.

Preflight fix: do not execute a newly generated native gate executable merely because it exists when the JSTAR build audit has thrown. A fresh script-local provenance flag is set only after the JSTAR script returns normally; before that the permitted bootstrap HEAD/TAIL use the original source oracle. An unsuccessful JSTAR audit still propagates HOLD. Once verified, subsequent HEAD/TAIL checks require the native executable. No rc.exe allowlist bypass or invented receipt. New script revision, not a reclassification of previous runs. No native MSVC/PowerShell/CTest execution verified for this revision. No tenfold runtime claim. Existing workflows use the same entry point and are not replaced.

## 24. ARIARI/1: THAILONGA rollup, ALONZ/INFINITI

EXPLORATORY implementation; native results pending. The new namespace
`swe::ariari` does not redefine the existing ALONZ acorn-tree, ancestry
ARIRANG, or DIAERY THAILONGA contracts. No archived experiment is changed.

- `shared/ariari.hpp`: STD-free bounded exact work ratios and reversible AR-prefix
  encoding with caller-defined distinct byte alphabet and arbitrary supported
  finite template length. One GCD through REDUX's fraction for the production
  pair; derive signed reduction and KNIGHTREX from the same CORE. Zero baseline
  undefined, zero remaining elimination. DECADAKATANA is a compatibility alias
  for the existing tenfold gate. ARIRANG/ARIRONG forward/reverse and ARIARI closure;
  independent binary GCD at logical AIRGAP, not a network air gap or approval.
- `shared/ariari_toolong.hpp`: optional actual arbitrary-precision TOOLONG lane,
  with explicit independent COREAO trace and explicit GammaGamma decimal token.
  The wrapper preserves the actual factorial result/status; no implicit decimal
  split or conversion of a double into an exact integer. Existing TOOLONG and
  GammaGamma use STL and allocation. Arithmetic consistency is not scientific
  evidence, authentication or release authorization.
- `docs/experiments/ARIRANG.cpp`: self-contained copy of the bounded REDUX and
  ARIARI implementation plus fixtures; `tests/ariari_test.cpp` uses the canonical
  header. Source fixture OIOIO -> AROIOIO -> original five tokens. 32 binary and
  243 ternary patterns, 1000-token sequence, 65280 work pairs, corruption and
  maximum-width cases. Counts are authored cases, not executed results.
- Exact payload counts exclude metadata; 15 -> 7 bytes (8/15 REDUX, 15/7
  KNIGHTREX). Checked finite powers preserve symbolic operands on overflow;
  INFINITI does not allocate all branches. No hardware speedup is asserted.
- Native root CTest registers the three new executables. Existing TRIFORCE/JSTAR
  qualification, Q62 source records, release HOLD and older standalone REDUX
  remain unchanged. No new hosted job or runtime network dependency.

The library is a composition, not a replacement for existing COREAO or TOOLONG.
No fixed-width arithmetic truncation is introduced.

## 25. LCD/1: namespace-free exact quotient

The owner-selected current standalone LCD is `l=a/c`, represented by
`L { U a, c; }` and `constexpr L LCD(U a,U c) noexcept`.
The earlier proposed namespaced LCD is superseded; it was not present as
a file in the parent tree. No established ARIARI or REDUX interface is
removed or renamed. This project-specific LCD is not the conventional
least common denominator (LCM of denominators).

The shared/LCD.hpp header has no includes, namespaces, dependency links,
allocation or external calls. Euclidean common-factor cancellation preserves
the exact quotient; c=0 preserves the original numerator and marks an
undefined quotient. Original operand scale is not reconstructable from the
reduced fraction alone. Global names U, L, LCD can collide with other headers;
callers must account for that intentional standalone interface.

CMake target LCD is independent of ARIARI and REDUX. The native CTest
lcd_256_cycles_exact_quotient uses a separate descending-divisor oracle over
all 256 x 256 unsigned 8-bit operand pairs, plus compile-time 64-bit boundary
fixtures. This is authored qualification, not a native execution receipt.
Native MSVC result remains pending; existing TRIFORCE and release HOLD
are not bypassed. No external dependency is introduced.

## 26. Namespace-free structural revision (2026-11-27)

Owner-directed migration of the draft's new C++20 implementation. The parent
revision is retained in Git history; earlier source receipts and experiments
are not rewritten. The recursive inventory at the start of review contained
798 entries, 216 active C/C++ sources and headers (excluding Archive), with
no truncation. This is a complete path inventory and targeted source review,
not a claim that every source was exhaustively compiled or audited.

Current draft interfaces:
- `LCD(a,c)` remains an independent global function.
- `REDUX::reduction`, `REDUX::fraction`, etc. are static members of a global
  type, not a namespace. No source-level arithmetic or fixtures removed.
- `ARIARI::arirang`, `ARIARI::airgap`, etc. are static members of a global
  type; the optional `ARIARI_TOOLONG::compose` adapter retains qualified
  references to the established COREAO, TOOLONG and GammaGamma libraries.
  Their existing namespaced interfaces are not redefined or flattened.
- REDUX.C and ARIRANG.cpp reproduce the revised canonical bounded cores and
  test bodies; no includes are added to either standalone file.
- GENSHIN bias/BiDi/TriPi/ZIPIT draft headers use type scopes. Original
  fixture values and assertions are preserved and evaluated after class
  completion. Independent standalone ZIPIT files use unique layout types
  and translation-unit-local functions.
- LF9WN9Z implementation helpers use a type scope. The three exported C ABI
  signatures, original tick arithmetic, fixed-point test, parameter checks
  and 7875-case physics oracle remain unchanged.
- Explicit test qualification replaces broad namespace imports. The new
  native CTest namespace_free_composition_65025 co-includes the independent
  headers and compares LCD with KNIGHTREX for 65,025 positive byte pairs.

The draft-only swe::redux and swe::ariari source interfaces are superseded
by global type scopes, which avoid polluting the global scope with generic
names such as Ratio, Status and Core. This is a deliberate source API change
within the unmerged draft, not a binary compatibility promise. Existing
shared COREAO and sigma geometry namespaces, other established tree APIs,
standard-library qualifications, C language linkage, safety bounds and
TRIFORCE/JSTAR remain intact. Namespace removal is a source organization
change; it does not by itself remove runtime instructions or prove speedup.
No native MSVC execution is claimed until actual CI receipts are available.

## 27. TALON/1 native CI qualification

New exploratory fixed-atmosphere model, not a physical drop. Figure-eight to
circle is a deformation; no energy is created by geometry. Orb mass 0.009 kg,
radius 0.01 m, gravity 9.80665, air density 1.225, drag coefficient 0.47.
Freeze 256 advances of 4 m, zero initial speed, constant parameters, RK4
integration of normalized squared velocity and drag work, independent
analytical exponential route. LCD is exact normalized drop distance, while
physics is binary64 with 2e-7 normalized-speed-squared and 1e-10 J energy
closure tolerances. Independent arithmetic evaluation predicts 31.23954 m/s
terminal speed and 4.39159 J kinetic-energy limit. Native execution pending.
No infinite physical height or atmospheric generality is claimed.

Root CTest and the existing isolated TRIFORCE PRECISION lane register LCD,
TALON and namespace-composition diagnostics; all retain the existing
HEAD/TAIL and JSTAR gate. No new workflow, runner image, network dependency,
external library or relaxation. Prior head ad1f5ed1985f0fd410a0f4ecb69f333d46b0c101
had successful DIAERY and QUADDI NI and failing Checks, EXCITE and PR-PARITY.
JSTAR reported unverifiable descendant paths, portable-core MSVC asserts at
lines 51 and 87 failed, and legacy source policy identified octadi/G.py and
18 POSIX scripts plus GNU probe. These independent failures remain HOLD;
do not bypass them or infer that TALON passed. No historical source
or receipt is rewritten.

## 28. SILVERHAND/1: UNION of native diagnostics

One CMake build target SILVERHAND in the existing PRECISION configuration
unites the 12 executable targets (original TOOLONG/GammaGamma and all ten
new PR test targets). Each executable remains distinct; UNION is task
composition, not C++ union storage or interchangeable tests. JSTAR already
uses --parallel 1, /m:1 and /nr:false. SILVERHAND validates the exact CTest
inventory, calls each test separately in fixed order with --parallel 1,
records individual exits and elapsed time, and publishes a fresh
SHA-256-addressed receipt outside the source tree and in CI logs.
A failed test does not suppress independent tests; any failure blocks the
aggregate. JSTAR HOLD blocks execution. No bypass, new runner, workflow,
network fetch, external compiler, automatic merge or release.
Nine TRIFORCE stages remain; ctest-toolong is renamed SILVERHAND.
Single scheduled build/test lane does not promise one OS thread inside
compiler/linker internals. Existing repository-wide workflows are unchanged.
Native validation and measured timing comparison remain pending. Expected
benefit is one build orchestration and deterministic diagnostics, not an
asserted speedup.

## 29. WIKI/1: native documentation graph

Root wiki.cpp is a namespace-free C++20 tool with a Win32 read/output
boundary. It enumerates the actual flat .wiki directory, rejects reparse
points, oversized/invalid filenames and unresolved inline local links,
and checks index/home coverage, symmetric directed edges and a closed
previous/next cycle across every page. Its restricted profile recognizes
plain inline Markdown page links and HTTP(S) references; external URLs
are not fetched, and fenced code is ignored. It is not a full Markdown
renderer or arbitrary Markdown parser. Limits: 128 pages, 95 filename
characters including extension, 256 KiB per file. No new third-party
library, dynamic allocation in authored code, source mutation or network
operation. Native Windows SDK/CRT are required for I/O.

Run wiki.exe from the repository root to audit; run wiki.exe --plan to
emit proposed index/navigation/reciprocal-link Markdown to stdout,
preserving a valid author ring or using sorted filename order otherwise.
The plan is not an automatic rewrite. The compile-time fixture checks
missing return links, UNION closure and a corrupted cycle. Root CTest
wiki_circular_references is registered; SILVERHAND/1's frozen 12-test
inventory remains unchanged. Native MSVC execution is pending.

## 30. HIKE/1 — PENNYWORTH

HIKE names the reciprocal Hamiltonian tour predicate in root `wiki.cpp`.
It checks the existing ring, then validates both directions of each tour
edge and returns to the index. It does not search for an optimal tour,
require off-tour reciprocity, or change the graph. The complete Wiki audit
still checks all references. The existing compile-time fixture now checks
HIKE before and after UNION, with a missing reverse edge and corrupt ring.
The runtime audit records `HIKE=PASS` only after the predicate succeeds.
The existing full 0BSD PENNYWORTH notice remains intact. Native MSVC
qualification is pending.

## 31. Accessibility / PENCE / NULL0 restoration

The root `ACCESSIBILITY.md` defines contribution requirements and known limitations. WIKI uses a 25-page reciprocal graph, separately labeled list navigation, persistent policy access and a closed HIKE. Decorative U+00B7 separators were removed from the Wiki and root README; historical source and experiment bytes remain intact. PENTAPI automatic NULL/RESET/ROOT after deliberate terminal activation is restored; the later explicit route remains historical, not a normal action requirement. Existing Win32 focus handling is retained. PENCE flags unexplained overlap for provenance and rights review; no 1BSD grant is inferred and the root 0BSD license is unchanged. Source and compile-time test changes are not native qualification or legal certification.
