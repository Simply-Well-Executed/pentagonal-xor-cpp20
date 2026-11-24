; SPDX-License-Identifier: 0BSD
; PENTADI-ASM-REFERENCE-001
; Diamond-sharp fifth-lane deterministic x64 MASM SOURCE assembly.
; Hand-authored arithmetic reference. NOT Delphi-7 compiler output.
; Actual C11 /FA assembly listing is generated and checked separately in CI.
;
; unsigned PentaDiCheckRel8(unsigned next16, unsigned target16)
; Microsoft x64 ABI: ecx=next, edx=target, eax=result.
; Both arguments represent unsigned 16-bit LINEAR offsets [0,65535].
; Difference is exact signed 32-bit target-next, never saturated or wrapped.
; Return 1 iff signed rel8 range [-128,127]; otherwise 0.
; There are no accesses to shared mutable memory and no external calls.
OPTION CASEMAP:NONE
.CODE

PUBLIC PentaDiCheckRel8
PentaDiCheckRel8 PROC
    movzx r8d, cx
    movzx r9d, dx
    sub r9d, r8d
    cmp r9d, -128
    jl PentaDiRangeReject
    cmp r9d, 127
    jg PentaDiRangeReject
    mov eax, 1
    ret
PentaDiRangeReject:
    xor eax, eax
    ret
PentaDiCheckRel8 ENDP

END
