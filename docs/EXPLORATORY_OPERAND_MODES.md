# Exploratory operand families × gain, dispersion, stability

**State: EXPLORATORY / pending complete numerical semantics.** This is a source-grounded symbolic grammar for a user-defined research framework, not a statement about quantum mechanics or a law of physics. The implementation must not transform these spellings into arithmetic instructions without a separately defined, tested, domain-limited operation.

## User clarification and axis model

The user defined **three modes on every operand family**, not just the lowercase and uppercase `p` variants:

1. **Gain**: historically associated with a leading `+` in the examples
2. **Dispersion**: associated with leading `-` in the explicitly supplied examples
3. **Stability**: associated with leading `=` in the explicitly supplied examples

The modes are independent of the underlying operand family. Even the shift-up and shift-down families carry all three modes.

The C++20 registry therefore exposes `Mode::{Gain,Dispersion,Stability}`, `Operand`, `mode_slot(Operand, Mode)`, `lookup_mode(Operand, Mode)`, and `lookup_exact(spelling,byte_count)`. Each of the **14 preserved operand identities** has all three conceptual slots: **14 × 3 = 42**. This does not imply that 42 exact, user-confirmed spellings or 42 numerical functions are already defined. A slot can exist with **no canonical notation**. A supplied spelling, an inferred candidate spelling, and an undefined spelling are different states. No slot can silently execute.

## Directly supplied mode examples and provisional extrapolations

| Operand family | Gain | Dispersion | Stability | Basis |
| --- | --- | --- | --- | --- |
| Lowercase `p` | `+p` | `-p` | `=p` | Three modes explicitly supplied by user |
| Uppercase `P` | `+P` | `-P` | `=P` | Three modes explicitly supplied by user; case-sensitive |
| Shift up `/` | `+/` | `-/` **candidate** | `=/` **candidate** | User named `+/` as shift up, associated with inverse-square law, then stated each family has three modes |
| Shift down `\` | `+\` | `-\` **candidate** | `=\` **candidate** | User named `+\` as shift down, associated with a root-of-square theorem, then stated each family has three modes |

The four shift variants `-/`, `=/`, `-\`, and `=\` are **inferred example spellings, not user-confirmed spellings**. They are deliberately tagged `Notation::Inferred`, `Status::ProposedNotation`, and `Provenance::InferredModeNotationNovember2026`. The modes themselves *are* user-confirmed as applicable. If a preferred punctuation scheme is provided later, add a new evidence-backed revision without erasing the original candidate.

`+/` and `+\` were previously recorded as **AMBIGUOUS** and **UNDEFINED** in the 2026-11-09 PlusStar validation, respectively. The latest user corrections override those earlier **interpretations**, not the historical source record: the former now has the descriptive association **shift up / inverse-square law**; the latter **shift down / root-of-square theorem**. Neither association supplies a unique computable formula, exponent convention, unit domain, branch choice, numerical stability criterion or physical test. Consequently both remain `Status::Exploratory` and `evaluable(record)` remains `false`.

An inverse-square law normally has a quantity varying inversely with the square of a distance, such as `k/r²` when its domain and constants are specified. That familiar physical model is **not** automatically assigned as `+/`'s implementation. The words "root of square theorem" also do not specify whether the intended relationship is the principal square root of a square (`sqrt(x²)=|x|` for real `x`) or another construction. No evaluation function is introduced by guessing.

## All existing operand identities, not just shifts

The original exact-source spellings and scope are retained:

| Source | Operand identity | Current gain/dispersion/stability knowledge |
| --- | --- | --- |
| `=`, `;=`, `[=`, `]=` | Four distinct equals-family operands | Three applicable modes each; no mode-specific punctuation supplied |
| `++`, `+-` | Two distinct historical PlusStar operands | Historical leading-plus gain spelling; two other mode spellings unassigned |
| `+;`, `+'` | Semicolon and apostrophe PlusStar operands | Historical leading-plus gain spelling; other spellings unassigned |
| `+[`, `+]` | Opening versus closing grouping operands | Historical leading-plus gain spelling; other spellings unassigned; no invented bracket balance |
| `+p`, `+P` | Case-distinct coefficient operands | Gain, dispersion, stability all supplied |
| `+/`, `+\` | Shift-up and shift-down operands | Three modes exist; four shift-mode spellings inferred, not established |

This preserves the original `++` / `+-` conditional interpretations, `+;` undefined, `+'` ambiguous, and grouping unbalanced states unless later explicitly revised. None of the equals-family base spellings (`;=` etc.) is silently conflated with stability-mode prefix `=`. **A symbol's position, spelling and case are part of its identity.**

## Exact byte-level T-CROSS and safety

The implementation is header-only ISO C++20 and intentionally imports no C++ standard-library header and no `std::` call. `records[]` preserves the original fourteen spellings plus eight additional mode spellings (four explicitly supplied `-p`, `-P`, `=p`, `=P`, and four clearly labeled inferred shift spellings). The registry has **22 exact spelling records** and 42 semantic mode slots. The record count and uniqueness of exact tokens and mode slots are compile-time assertions.

The `quantum_operand_exact_symbol_registry` CTest checks every registered spelling against its literal byte sequence, its mode, and its operand identity. Negative controls reject unregistered alternatives and Unicode full-width equals `＝` rather than folding it to ASCII `=`. `p` and `P` remain distinct. The CMake core-only archive also statically compiles the lexicon. The strict POSIX STD-free source gate scans both the new header and its test.

The C++ API intentionally never evaluates new operators numerically. If the user specifies a domain, constant, branch, source order, scale/units and exact mathematical evaluation for a given pair, that can be implemented as a separately versioned operation with mathematical and numerical tests.

## Custody and evidence

The earlier **COREAO Plus-Star Conservation Theorem: Validation Record** dated 2026-11-09 and **COREAO Plus-Star × MINZA 9D→1D Validation** dated 2026-11-10 are historical evidence of the ten original PlusStar spellings, conditional meanings and exact case distinction. They are not proof of quantum phenomena. The additional equals-family `= ;= [= ]=` spellings and the shift/mode clarifications are direct user statements in this conversation. The four inferred shift spellings are an explicit **proposal**, not a recovered quotation from the JSONL or Copilot+ archives.

No experiment, release, source baseline, GAMP4 result, Docker manifest or other PR is modified by the addition of this symbolic registry. Its code and acceptance tests must be built and checked on the target compilers before promotion.
