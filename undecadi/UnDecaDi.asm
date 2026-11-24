; SPDX-License-Identifier: 0BSD
; UNDECADI-001: direct x64 MASM assembly, no macros/includes/remote code.
; Contract: ECX holds an 8-bit input (0..255); EAX returns input XOR 0xFF.
OPTION CASEMAP:NONE
PUBLIC UnDecaDi
.code
UnDecaDi PROC
    mov eax, ecx
    xor eax, 0FFh
    ret
UnDecaDi ENDP
END
