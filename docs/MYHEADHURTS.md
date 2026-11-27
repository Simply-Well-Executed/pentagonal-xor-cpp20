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
