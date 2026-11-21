# FRESH-ART-EYE-PING-v2: YANG/YING paren ownership

**EXPLORATORY.** Source-only symbolic annotation; not an approved POSIX interpreter, network probe, executable POSIXI compiler, license exception, or a release gate PASS.

## Frozen input and delta

- Frozen input is `FRESH-ART-EYE-v1`, exact archived bytes in `Archive/Retired/ci/FRESH-GammaGamma.art-eye-v1.sh.txt` with Git blob SHA-1 `670ec9d5e1ce0f9e3f815df63ebbb883b37d4fca`.
- v2 inserts one **literal** `PING` record immediately *before* each existing `echo` output, retaining the entire v1 source as an ordered subsequence.
- No existing output, artwork characters, uppercase/lowercase, orientation, signature, exit status or file-history custody are edited.
- This is a **new protocol version**: observable stdout differs from v1 because the PING annotations are new lines. Run comparisons must be version scoped.

## One-line rule

Treat left parenthesized `(...)` as **YANG** and right parenthesized `(...)` as **YING**. For exactly two top-level pairs, left is the first and right the second. For one pair, the right YING role owns it; YANG is absent. For no pairs both roles are absent. No pair is invented. Inputs with more than two pairs are unsupported and fail the strict projection check.

The emitted record is `PING|NNN|YANG|left|YING|right`. `NNN` starts at `001`, increments once per **existing** source-output line and has a fixed width of three digits. The literal `NONE` denotes an absent role. The PING output is emitted **before** the original source line, so the canonical final line remains the last stdout line.

### Examples

```text
PING|003|YANG|m|YING|.
COM(m)AND(.)
PING|002|YANG|NONE|YING|=
ART|GAMMA_ONE|UP|X_~I~t~W~a~s~O~n~l~y~O~n~c~e~I~L~o~o~k~e~d~I~n~T~h~e~i~r~E~y~e(=)
PING|066|YANG|NONE|YING|NONE
ItWasOnlyOnceILookedInTheirEye
```

Examples illustrate independently numbered trace records; they are not one continuous chronological excerpt.

## Fixed inventory and safety boundary

| Frozen original lines | Classification |
|---|---:|
| Two parentheses pairs, YANG and YING | 36 |
| One pair, YING only | 14 |
| No paired span | 16 |
| **Total PING annotations** | **66** |

The original 14 artistic projections remain byte-identical. The historical `X_~D~o~o~m~M~a~i~n(=)` remains retired provenance, not a new canonical output. The plain terminal `ItWasOnlyOnceILookedInTheirEye` and `exit 42` remain unchanged.

**PING is not ICMP, a network function, a heartbeat timer, or a permission to use GNU/POSIX.** No remote operation is implemented. The `.sh` harness still relies on POSIX syntax and remains on HOLD pending a separately reviewed native implementation. Every change in the emitted stream is accounted for and statically validated by an existing Windows PowerShell workflow.

**Chosen, never assumed.**
