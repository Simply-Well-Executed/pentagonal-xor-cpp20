// SPDX-License-Identifier: 0BSD
// REELNATIVE-YANG stage-09 evidence display, not an OpenBSD binary or a release qualification.
// This Linux/amd64 freestanding C++20 process uses raw Linux syscalls.
// Authoritative stage-09 witness is independently computed by the existing
// ci/Invoke-OpenBSDSerialCoreao.ps1 pipeline before build and on every run.
#if !defined(__linux__) || !defined(__x86_64__)
#error "REELNATIVE evidence runtime only supports Linux/amd64"
#endif

namespace reelnative_yang {
static constexpr char report[] =
    "REELNATIVE|STAGE=09|IDENTITY=OPENBSD2_YANG|CORE=OPENBSD2|RELEASE=2.9\n"
    "REELNATIVE|PREDECESSOR=OPENBSD2_YIN|SHA256=5397a209ab64bb9b194b97bb6e15f0c228b5a86f6bc0eb25c7dfcc9fc73bd7f6\n"
    "REELNATIVE|YANG_WITNESS_SHA256=d931ea94e80c62340b177555297501d4fc3d6bd69d2db170fd599662f32a7afc\n"
    "REELNATIVE|NATIVE_OPENBSD_BUILDS=0|RELEASE_STATUS=HOLD|HOST=LINUX_AMD64\n";
} // namespace reelnative_yang

extern "C" __attribute__((noreturn, used, visibility("default")))
void _start() noexcept {
  const char* current = reelnative_yang::report;
  unsigned long remaining = sizeof(reelnative_yang::report) - 1U;
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
