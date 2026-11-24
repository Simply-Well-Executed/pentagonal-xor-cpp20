# FRESH-ART-EYE-GAMME-v5: FLOP/FLIP nine-beat ingress

**EXPLORATORY / SOURCE-ONLY / RELEASE HOLD**. This is an explicitly versioned new source stream rather than mutation of any prior frozen protocol.

## Vocabulary and operator boundaries

- **GAMMA = EGRESSING**: the two outer source passes are `GAMMA_ONE` and `GAMMA_TWO`, not `GAMME`.
- **GAMME = INGRESSING**: the nine inner phase names are `GAMME_ONE`, `GAMME_TWO`, `GAMME_THREE`, `GAMME_FOUR`, `GAMME_FIVE`, `GAMME_SIX`, `GAMME_SEVEN`, `GAMME_EIGHT`, `GAMME_NINE`.
- **FLOP = `=`**, **FLIP = `s`**. The nine-beat sequence is FLOP, FLIP, FLOP, FLIP, FLOP, FLIP, FLOP, FLIP, FLOP.
- `YANG|NONE` stays explicit on YING-only lines, and YING stays `PAIR|AIRGAP`. No omitted YANG witness is fabricated.
- GAMME is a user-defined directionality/operator label in this experiment, not an established shell command or proof of physical ingress/egress.

## Trace example

```text
FRESH|GAMMA_ONE|EGRESSING|HEAD
PING|002|GAMME_ONE|INGRESSING|FLOP|YANG|NONE|YING|=|PAIR|AIRGAP
ART|GAMMA_ONE|UP|GAMME_ONE|INGRESSING|FLOP|X_~I~t~W~a~s~O~n~l~y~O~n~c~e~I~L~o~o~k~e~d~I~n~T~h~e~i~r~E~y~e(=)
PING|002|GAMME_TWO|INGRESSING|FLIP|YANG|NONE|YING|s|PAIR|AIRGAP
ART|GAMMA_ONE|UP|GAMME_TWO|INGRESSING|FLIP|X_~I~t~W~a~s~O~n~l~y~O~n~c~e~I~L~o~o~k~e~d~I~n~T~h~e~i~r~E~y~e(s)
...
PING|002|GAMME_NINE|INGRESSING|FLOP|YANG|NONE|YING|=|PAIR|AIRGAP
ART|GAMMA_ONE|UP|GAMME_NINE|INGRESSING|FLOP|X_~I~t~W~a~s~O~n~l~y~O~n~c~e~I~L~o~o~k~e~d~I~n~T~h~e~i~r~E~y~e(=)
```

## Frozen source custody and exact version delta

- `FRESH-ART-EYE-PING-v4` source is archived at `Archive/Retired/ci/FRESH-GammaGamma.art-eye-ping-v4.sh.txt` with Git blob `804472e09d501d3357a652f94d790c28135f30a3`.
- On v5, the two egress headers are renamed exactly to `FRESH|GAMMA_ONE|EGRESSING|HEAD` and `FRESH|GAMMA_TWO|EGRESSING|TAIL`.
- At each of the 14 existing YING-only paired checkpoint entries, v4's one PING/art pair expands into nine literal paired GAMME phases in place, in ascending ONE..NINE order.
- v5 contains 126 phase PING records and 126 artistic records plus 52 untouched PING records: **178 PINGs total**. Counts: 70 FLOP, 56 FLIP, 14 checkpoints, two GAMMA egress headers.
- Each record retains its inherited three-digit original event number; the GAMME phase tag distinguishes the nine observations without renumbering unrelated events.
- Historical `X_~D~o~o~m~M~a~i~n(=)` remains retained only in prior evidence; `ItWasOnlyOnceILookedInTheirEye` is still the exact final printed line, immediately followed by `exit 42`.
- v5 stdout differs from v4. Compare only against an explicitly frozen protocol and keep v4 available for independent audit.

## Safety and licensing

The CI source-only PowerShell validator checks v1→v2→v3→v4→v5 exact Git-blob chain without executing POSIX shell code. All authored v5 shell lines remain literal with no variables, dollar signs, percent formatting, loops, recursion, external commands or network PINGs. The actual `.sh` format and GNU-dependent Dockerfile still violate the zero-POSIX/GNU project policy; **release HOLD persists**.

**Chosen, never assumed.**
