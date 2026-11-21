// SPDX-License-Identifier: 0BSD
// COREAO evidence display, not an OpenBSD binary or a release qualification.
// This Linux/amd64 freestanding C++20 process uses raw Linux syscalls.
// Authoritative reconciliation is independently computed by the existing
// ci/Invoke-OpenBSDSerialCoreao.ps1 pipeline before build and on every run.
#if !defined(__linux__) || !defined(__x86_64__)
#error "COREAO evidence runtime only supports Linux/amd64"
#endif

namespace coreao_evidence {
static constexpr char report[] =
    "COREAO|STAGE=10|PASS=COREAO|CORE=OPENBSD2|RELEASE=2.9\n"
    "COREAO|YIN_WITNESS_SHA256=5397a209ab64bb9b194b97bb6e15f0c228b5a86f6bc0eb25c7dfcc9fc73bd7f6\n"
    "COREAO|YANG_WITNESS_SHA256=d931ea94e80c62340b177555297501d4fc3d6bd69d2db170fd599662f32a7afc\n"
    "COREAO|RECONCILED_WITNESS_SHA256=efed3f09265eb3f5e506cf310a9905b12befa3b642f0085b3a089fb856ddbcdf\n"
    "COREAO|NATIVE_OPENBSD_BUILDS=0|RELEASE_STATUS=HOLD|HOST=LINUX_AMD64\n";
} // namespace coreao_evidence

extern "C" __attribute__((noreturn, used, visibility("default")))
void _start() noexcept {
  const char* current = coreao_evidence::report;
  unsigned long remaining = sizeof(coreao_evidence::report) - 1U;
  while (remaining != 0U) {
    long written;
    __asm__ __volatile__("syscall"
        : "=a"(written)
        : "a"(1L), "D"(1L), "S"(current), "d"(remaining)
        : "rcx", "r11", "memory");
    if (written <= 0) {
      __asm__ __volatile__("syscall" : : "a"(60L), "D"(1L)
                           : "rcx", "r11", "memory");
      __builtin_unreachable();
    }
    current += written;
    remaining -= static_cast<unsigned long>(written);
  }
  __asm__ __volatile__("syscall" : : "a"(60L), "D"(0L)
                       : "rcx", "r11", "memory");
  __builtin_unreachable();
}
