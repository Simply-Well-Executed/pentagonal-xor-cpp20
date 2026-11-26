# SLIPSLIDINGTOOLONG-001 / GUARDMEMORYGUARD

STATUS: EXPLORATORY, independently testable native C++20 and Oracle Java model.
LICENSE: project-authored source 0BSD, Oracle JDK retained under Oracle NFTC.
SOURCE CUSTODY: this add-on does not edit shared/slidingtoolong.hpp,
shared/toolong_q62_core.hpp, or shared/eulers_jig_canonical.hpp.

## Definitions

THAILONGA GUARD + MEMORYGUARD = GUARDMEMORYGUARD, as a compound check
with independently named components. GUARD validates head/tail canaries,
declared width and zeroed unused lanes. MEMORYGUARD checks a deterministic
FNV-derived digest. They are error detectors, not cryptographic MACs or
hardware isolation.

GLINTING projects a logical value to a narrower permitted width only if
no discarded high bit is set. Otherwise REJECT_LOSS. GLIZZING projects up
by exact zero extension. Width changes do not read other physical registers.
Arithmetic + and - preserve explicit carry or borrow evidence. << and >>
operate on owned limbs, and report bits lost outside the width. Logical &
masks owned values, not addresses, pointers, DMA buffers, or memory mappings.

The MXRING is an eight-slot bounded, by-value software FIFO of protected
logical register values, with canary, count, read/write index, generation
and per-record checksum validation. SPY is a read-only observation of its
own bounded metadata. It is not arbitrary memory surveillance.

## Width matrix

Representations tested: 32, 64, 128, 192, 224, 256, 384, 448, 480, 512 bits.
100 ordered source/destination transitions, comprising 45 GLINTING,
45 GLIZZING and 10 equal widths. A further 45 tests intentionally populate
discarded high bits and REQUIRE REJECT_LOSS on narrowing. A 32-bit
conversion of the Q62 intensity result must reject, not round.

The C++20 test exercises GUARDMEMORYGUARD, bitwise AND, shifts with
loss reporting, arithmetic carry/borrow, FIFO ordering, full and empty
conditions, stale checksum rejection, and invalid widths. The separate
Oracle Java 27 BigInteger model checks equivalent width semantics and
the original Q62 numerical result. Neither asserts native 256/512-bit
hardware registers or a particular compiler-speed optimization.

Original Q62 source custody:
- Euler Q62 source 12535862302449814170 ULL;
  integer output 271828182845904523 ULL.
- Pi Q62 source 14488038916154245685 ULL;
  integer output 314159265358979323 ULL.
- Intensity Q62 source 315868046760674477 ULL;
  integer output 6849296450333523 ULL.
- Q62 multiplier 100000000000000000 ULL; shift 62.
- Euler's Jig (distinct from e) 2864498081537971875 ULL
  with decimal scale 10^17.
- This is a scoped provenance ledger, not a claim to have indexed every
  historical constant in every repository branch.

## Limits and threat model

No unauthorized adjacent-memory access, no BIOS/kernel/hypervisor inspection,
no actual hardware register spying, no garbage collector implementation,
no automatic thread-safety, and no survival after process shutdown.
The .NET CoreCLR GC uses native runtime infrastructure; F# and C# can
consume managed memory, but this add-on is not a .NET GC replacement.
Worker-thread exit does not destroy a separately owned object if valid
synchronized ownership remains; that protection requires an explicit
synchronization strategy not implemented here. Process-crash durability
requires a separate journal/checkpoint and recovery design.
Nested guest/hypervisor configurations are finite and subject to
hardware and software support.

The existing 128-bit Q62 intermediate / 64-bit output remains
the established reference. Other width transitions are new independent
logical value tests, not proven performance accelerators.

RELEASE=HOLD. Historical proofs are not silently upgraded or erased.
