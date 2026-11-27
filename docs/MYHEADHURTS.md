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
