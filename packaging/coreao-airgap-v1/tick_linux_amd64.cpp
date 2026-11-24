// SPDX-License-Identifier: 0BSD
// COREAO airgap V1: freestanding Linux/amd64 diagnostic, not an OpenBSD OS.
// Every invocation is an independent, bounded witness tick, not a timer.
#if !defined(__linux__) || !defined(__x86_64__)
#error "COREAO airgap test only supports Linux/amd64"
#endif

namespace coreao_airgap_v1 {
static constexpr char report[] =
    "COREAO|AIRGAP_V1|TICK=1|ENTRY=FREESTANDING_CXX20|COMPILER_O2=REQUIRED\n"
    "COREAO|CANONICAL_STAGE10_WITNESS_SHA256=efed3f09265eb3f5e506cf310a9905b12befa3b642f0085b3a089fb856ddbcdf\n"
    "COREAO|NATIVE_OPENBSD_BUILDS=0|RELEASE_STATUS=HOLD|HOST=LINUX_AMD64\n";
}

extern "C" __attribute__((noreturn,used,visibility("default")))
void _start() noexcept {
  const char* current=coreao_airgap_v1::report;
  unsigned long remaining=sizeof(coreao_airgap_v1::report)-1U;
  while(remaining!=0U){
    long count;
    __asm__ __volatile__("syscall"
      : "=a"(count)
      : "a"(1L), "D"(1L), "S"(current), "d"(remaining)
      : "rcx","r11","memory");
    if(count<=0){
      __asm__ __volatile__("syscall" : : "a"(60L),"D"(1L) : "rcx","r11","memory");
      __builtin_unreachable();
    }
    current+=count;
    remaining-=static_cast<unsigned long>(count);
  }
  __asm__ __volatile__("syscall" : : "a"(60L),"D"(0L) : "rcx","r11","memory");
  __builtin_unreachable();
}
