# FRESH-ART-EYE-PING-v3: YING-only ↔ AIRGAP

**Historical shell instructions:** POSIX commands and shell behavior described below are preserved as historical protocol/evidence. The 17 wrappers are removed from active CI; links target archived text. See the [discarded-files registry](../discardedfiles.md). Retained task names invoke the native fixed transcript and exit 42 / **RELEASE=HOLD**, without performing these former operations.

**EXPLORATORY / SOURCE ONLY / RELEASE HOLD.** This is a new protocol version, not a retroactive edit to FRESH-ART-EYE-PING-v2.

## Frozen lineage

- FRESH-ART-EYE-v1: `Archive/Retired/ci/FRESH-GammaGamma.art-eye-v1.sh.txt`
- FRESH-ART-EYE-PING-v2: `Archive/Retired/ci/FRESH-GammaGamma.art-eye-ping-v2.sh.txt` (Git blob SHA-1 `f1870c1169ccdbb72f39a917dfa292a1b53c3f59`).
- FRESH-ART-EYE-PING-v3: archived `Archive/Retired/ci/FRESH-GammaGamma.art-eye-ping-v3.sh.txt`. v3 adds 14 explicit pair markers; v2 and v3 stdout streams are **not equivalent**.

## New mapping

The left group `(...)` has symbolic role YANG, and the right group `(...)` has symbolic role YING. When **only one parenthesized group** exists, YANG is explicitly absent (`NONE`), the group belongs to YING, and **AIRGAP is the explicit counterpart**. AIRGAP is a boundary role, *not* a fabricated character, new pair of parentheses, or claim that YANG is present.

```text
PING|002|YANG|NONE|YING|=|PAIR|AIRGAP
ART|GAMMA_ONE|UP|X_~I~t~W~a~s~O~n~l~y~O~n~c~e~I~L~o~o~k~e~d~I~n~T~h~e~i~r~E~y~e(=)
```

Two-sided pairs stay unchanged:

```text
PING|003|YANG|m|YING|.
COM(m)AND(.)
```

Zero-sided lines stay unchanged:

```text
PING|066|YANG|NONE|YING|NONE
ItWasOnlyOnceILookedInTheirEye
```

## Invariants

- Exactly **66 PING records**. Of these, 36 have both YANG and YING, 14 have one YING paired with AIRGAP, and 16 have neither.
- Only the 14 YING-only PING records append the exact literal suffix `|PAIR|AIRGAP`; **no other original output or PING text changes**.
- Each of the 14 existing eye-art displays remains byte-for-byte identical and in the same position.
- Last printed line exactly `ItWasOnlyOnceILookedInTheirEye`; subsequent status `exit 42`.
- No variables, dollar signs, percent formatting, new evaluation, recursion, external commands, or networking are introduced to the shell harness.
- Symbolic PING does **not** send ICMP traffic. AIRGAP does **not** imply any physical isolation, real network boundary, consent, authority, or relationship.
- The existing Windows PowerShell static source gate proves the exact transformation without executing any POSIX shell. The GNU/POSIX release gate remains HOLD.

**Chosen, never assumed.**
