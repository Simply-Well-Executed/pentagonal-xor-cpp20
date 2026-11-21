// SPDX-License-Identifier: 0BSD
// REELNATIVE-RETURN-THAILONGA v2 append-only stage-07 postflight display, not an OpenBSD binary or a release qualification.
// This Linux/amd64 freestanding C++20 process uses raw Linux syscalls.
// Authoritative stage-07 witness is independently computed by the existing
// ci/Invoke-OpenBSDSerialCoreao.ps1 pipeline before build and on every run.
#if !defined(__linux__) || !defined(__x86_64__)
#error "REELNATIVE evidence runtime only supports Linux/amd64"
#endif

// Generated only after canonical serial evidence and postflight THAILONGA v1 validation.
// The header is created in a temporary CI build directory; never mutates frozen source.
#include "thailonga_build_digest.hpp"
#ifndef THAILONGA_POSTFLIGHT_SHA256
#error "THAILONGA postflight SHA256 witness was not generated"
#endif

namespace reelnative_return {
static constexpr char report[] =
    "REELNATIVE|STAGE=07|IDENTITY=OPENBSD4_RETURN|PASS=RETURN|CORE=NONE|RELEASE=4.9\n"
    "REELNATIVE|SEPARATE_FIRST_PASS=OPENBSD4_FIRST|SHA256=1e91defa5aa33b5b613b0ffc8c62da3eee826fb02a2fc2d296d6a01d6c417095\n"
    "REELNATIVE|PREDECESSOR=OPENBSD3|SHA256=d220105c5b3fc0ca018e2ace6e0b7b02461852e28557ae4a28251731ca28fb22\n"
    "REELNATIVE|RETURN_WITNESS_SHA256=e8690d3c9cfcf1993810e8847aead545b0461467a3d1e4f74047e6b0a1f394fd\n"
    "REELNATIVE|THAILONGA_V1|MODE=POSTFLIGHT_ONLY|CANONICAL_STAGE07_CORE=NONE|PROJECTED_CORE=OPENBSD2\n"
    "REELNATIVE|THAILONGA_YIN_SHA256=5397a209ab64bb9b194b97bb6e15f0c228b5a86f6bc0eb25c7dfcc9fc73bd7f6\n"
    "REELNATIVE|THAILONGA_YANG_SHA256=d931ea94e80c62340b177555297501d4fc3d6bd69d2db170fd599662f32a7afc\n"
    "REELNATIVE|THAILONGA_POSTFLIGHT_SHA256=" THAILONGA_POSTFLIGHT_SHA256 "\n"
    "REELNATIVE|NATIVE_OPENBSD_BUILDS=0|RELEASE_STATUS=HOLD|HOST=LINUX_AMD64\n";
} // namespace reelnative_return

extern "C" __attribute__((noreturn, used, visibility("default")))
void _start() noexcept {
  const char* current = reelnative_return::report;
  unsigned long remaining = sizeof(reelnative_return::report) - 1U;
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
