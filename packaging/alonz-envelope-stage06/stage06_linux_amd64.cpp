// SPDX-License-Identifier: 0BSD
// ALONZ ENVELOPE | stage 06 | independently authored diagnostic, not OpenBSD.
// Fixed witness text crosschecked against the canonical serial ledger at build.
// THAILONGA supplies the later COREAO=OPENBSD2 projection after stage 10.\n// The historical stage 06 canonical CORE=NONE remains intact as provenance.
#if !defined(__linux__) || !defined(__x86_64__)
#error "ALONZ-ENVELOPE is a Linux/amd64 evidence diagnostic, not OpenBSD"
#endif

namespace alonz {
static constexpr char report[] =
    "ALONZ|STAGE=06|IDENTITY=OPENBSD3|RELEASE=3.9|PASS=NONE|CORE=OPENBSD2|CORE_MODE=THAILONGA_POSTFLIGHT|CANONICAL_CORE=NONE|HOST=LINUX_AMD64\n"
    "ALONZ|ADDRESSING|NAME_DISTINCT_FROM_ADDRESS|ADDRESS_DISTINCT_FROM_ROUTE|NO_IMPLIED_AUTHORITY\n"
    "ALONZ|PREDECESSOR=OPENBSD4_FIRST|SHA256=1e91defa5aa33b5b613b0ffc8c62da3eee826fb02a2fc2d296d6a01d6c417095\n"
    "ALONZ|STAGE06_WITNESS=OPENBSD3|SHA256=d220105c5b3fc0ca018e2ace6e0b7b02461852e28557ae4a28251731ca28fb22\n"
    "ALONZ|SUCCESSOR=OPENBSD4_RETURN|SHA256=e8690d3c9cfcf1993810e8847aead545b0461467a3d1e4f74047e6b0a1f394fd\n"
    "ALONZ|THAILONGA|CORE=OPENBSD2|CANONICAL_CORE=NONE|PROJECTION=POSTFLIGHT_ONLY|NOT_STAGE06_INPUT=1|YIN=5397a209ab64bb9b194b97bb6e15f0c228b5a86f6bc0eb25c7dfcc9fc73bd7f6|YANG=d931ea94e80c62340b177555297501d4fc3d6bd69d2db170fd599662f32a7afc\n"
    "ALONZ|NATIVE_OPENBSD_BUILDS=0|LICENSE_REVIEW=HOLD|RELEASE_STATUS=HOLD|NETWORK_RUNTIME=NONE\n";
} // namespace alonz

extern "C" __attribute__((noreturn, used, visibility("default")))
void _start() noexcept {
  const char *p = alonz::report;
  unsigned long remaining = sizeof(alonz::report) - 1U;
  while (remaining != 0U) {
    long written;
    __asm__ __volatile__("syscall"
                         : "=a"(written)
                         : "a"(1L), "D"(1L), "S"(p), "d"(remaining)
                         : "rcx", "r11", "memory");
    if (written <= 0L) {
      __asm__ __volatile__("syscall" : : "a"(60L), "D"(1L)
                           : "rcx", "r11", "memory");
      __builtin_unreachable();
    }
    p += written;
    remaining -= static_cast<unsigned long>(written);
  }
  __asm__ __volatile__("syscall" : : "a"(60L), "D"(0L)
                       : "rcx", "r11", "memory");
  __builtin_unreachable();
}
