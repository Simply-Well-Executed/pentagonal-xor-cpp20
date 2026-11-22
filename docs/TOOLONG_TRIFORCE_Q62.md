# TOOLONG TRIFORCE ΣΣΣ: exact Q62 custody and three-person review

**CURRENT PROPOSAL for PR #32.** C++20 native-MSVC source policy;
**release HOLD** until separately evidenced human approvals and build verification.

## Source authority: three separately preserved constants

| Owner's PR comment | Original Q62 source word | Expected integer | Last two digits |
|---|---:|---:|---|
| [Euler, comment 7073177351](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/pull/32#issuecomment-7073177351) | `12535862302449814170` | `271828182845904523` | `23` |
| [ℼ, comment 7073580560](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/pull/32#issuecomment-7073580560) | `14488038916154245685` | `314159265358979323` | `23` |
| [I, comment 7073578206](https://github.com/Simply-Well-Executed/pentagonal-xor-cpp20/pull/32#issuecomment-7073578206) | `315868046760674477` | `6849296450333523` | `23` |

For each record, the exact operator is
`floor((source_word × 100000000000000000) / 2^62)`.
The owner comments use `__uint128_t`, which MSVC does not compile.
The program preserves the exact source words and result integers while
implementing the same **nonnegative** arithmetic without external toolchains.
The `I` result has **16 integer digits**; unlike Euler and π, it does not
have 18. It represents a particular fixed-point intensity *sample* for
`P=1`, `α=0.15`, `ω=1`, not a universal physical constant.
A matching `23` suffix is an integrity check, **not a proof of common origin,
cryptographic authenticity, or a new mathematical identity**.

## Three independent technical witnesses, never a vote

1. `shared/toolong_q62_core.hpp`: standalone constexpr partial-product
   recomputation using 64-bit operations and strict static assertions.
2. `shared/toolong_q62.hpp`: exact `swe::toolong` product, division,
   independent bit shift, remainder and reconstruction audits.
3. `sigma_sigma_sigma()`: all three exact expected outputs and the
   owner-observed rightmost `23` suffix must be simultaneously satisfied.

The third lane is a **consistency observation**, not an independent proof
of the arithmetic. The original three source records remain distinct.
A perturbation of one recorded output or an altered reviewer identity
must not be converted into a passing audit.
No numeric SigmaPi interpretation is silently adopted.

## Three-person integrity: explicit and authenticated

The `ApprovalClaim` structure requires three **distinct, nonempty** reviewer
identifiers, three explicit `YES` claims and identical 40-character
lowercase reviewed commit SHA values. Missing identities, duplicate names,
silence, PASS or mismatched SHA all block the *claims-complete* predicate.

This only validates **caller-provided claims**. It cannot authenticate
real people or signatures. GitHub identities, permissions, review
authenticity and the actual reviewed commit must be verified separately
by a trusted review process. There are **zero** prefilled approvals,
no automatic consent, and no release permission. The header records
`externally_authenticated_approval=false`; a source edit alone must
not turn an unverified process into a verified one.

## Testing and status

The original TOOLONG arithmetic and GammaGamma tests exercise the
immutable records, TOOLONG big-integer quotient/remainder results,
a deliberately corrupted expected output, and a duplicate-identity
negative control. The STD-free build uses the independent header-only
witness. Keep native Microsoft Visual C++ 2026 C++20 `/O2` as the
supported build path; Clang, GCC and auto-fallback are prohibited.

No historical protocols, original owner comments, source bytes, or
retired versions are overwritten by this additional v1 audit.
Actual MSVC CTest outcomes, build source SHA, and independent human
review evidence must be recorded before any merge or release.
