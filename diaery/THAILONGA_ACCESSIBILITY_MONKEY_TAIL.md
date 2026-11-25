# THAILONGA | Accessibility and MONKEY TAIL numerical integrity

Status: EXPLORATORY 0BSD, native C++20 /O2, RELEASE=HOLD.

## Every key gets a button

All existing DYNAMO, THAILONGA, PROJECTIONS, FREQ, circle and DIAERY keyboard actions have visible standard Win32 BUTTON counterparts (large 109x48 targets in a 6-column action deck), including X/Y/Z +/- steps, D view, H frequency, A sample append, C copy audit, K study gear, G grid, M mint, N band, J canary, F +1 femtosecond, S +1 second, P next projection, T trace, R edit, all ten {0}..{9} projectors, source trace presets, Enter, Escape, Backspace, and explicit on-screen trace-keypad characters, plus previous W/Y/C/B, zoom +/- and stage navigation. The former mouse-drag-only 3D yaw and pitch controls now also have four physical rotation buttons. BUTTON role/name/Invoke is supplied through native Windows controls. The main message pump now uses IsDialogMessageA to support focus traversal and button activation, while preserving existing keyboard shortcuts. The controls do not require touch pressure or a specific stylus device: Windows pointer/assistive-device activation can click ordinary buttons. Actual assistive-tech interoperability remains to be verified with target hardware and users; native buttons alone do not certify eye-tracker integration, dwell timing, screen-reader announcements, or accessibility compliance.

Stephen Hawking is an instructive example of switch access rather than an assumption that every disabled person has identical abilities. His communication used a cheek sensor and selection software; compatible eye gaze, single-switch scan or dwell activation should be evaluated for this application's controls. The UI must not invent consent from unintentional gaze or touch; a deliberate activation is required.

## MONKEY TAIL: conjugate test, rounding and gain

The term MONKEY TAIL is project-local, exploratory terminology, not a verified name for a Thai mathematical theorem. A web search did not establish the requested Thailand origin or an article describing this precise rounding algorithm. Established conjugate geometry is distinct from the project's mnemonic.

Represent the right triangle as z = a + bi, with complex conjugate conjugate(z) = a - bi. Since i^2 = -1, z * conjugate(z) = a^2 + b^2 + (ab-ab)i = a^2 + b^2. The cross terms cancel exactly. This is a squared-magnitude invariant. Example (3+4i)(3-4i)=25+0i. It does not imply that i itself is negative gain. Rotation i has magnitude 1, while a gain g with 0 <= |g| < 1 contracts magnitudes to zero under repeated multiplication. An explicit multiplying factor is necessary to combine spiral rotation and decay.

To avoid silent truncation, monkey_tail.hpp provides round_nearest_even(numerator,denominator) for unsigned 64-bit integers, returning quotient, remainder, whether the result was exact, and tie/increment flags. Exact ties are resolved to even, with no overflow-prone 2*remainder. Example 5/2 rounds to 2, while 7/2 rounds to 4, but their source rational numerator/denominator and remainder are not discarded. It does not claim a floating-point universal highest accuracy; numeric error analysis and reference results are needed for that statement. Interval methods and properly rounded operations should be selected when scientifically required.

monkey_tail_test.cpp contains static assertions for i cancellation, positive squared triangle magnitude, invalid integer domains, exact rational ties-to-even, and separate shrinking-gain checks. This checks mathematical invariants, not missing anatomical measurements or research-source claims.

Reference: William Kahan, Miscalculating Area and Angles of a Needle-like Triangle, 2014, for the danger of badly rounded geometric formulas. This is evidence for numerical care, not evidence of a named Thai MONKEY TAIL algorithm.

## Validation and consent

Both accessibility actions and MONKEY TAIL tests are registered in CTest and every LadderDi native test source list. A registered pipeline is not a proven passing pipeline. Evaluate on Windows with Windows Voice Access/Eye Control, touch, a stylus and switch-actuated pointer/Tab. Keep RELEASE=HOLD until real tests finish.