; Retail links this unit through the OMF-to-COFF converter. This is the sole
; reconstructed implementation: COFF for objdiff, OMF for the final link.

.386
.model flat, C
option casemap:none
option prologue:none
option epilogue:none

.code

BitTest PROC C
    push ebp
    mov ebp, esp
    push esi
    mov esi, DWORD PTR [ebp+8]
    mov eax, DWORD PTR [ebp+12]
    mov ecx, eax
    shr eax, 3
    and ecx, 7
    add esi, eax
    mov eax, 1
    shl eax, cl
    and eax, DWORD PTR [esi]
    jne bit_test_set
    mov eax, 0
    jmp bit_test_done
bit_test_set:
    mov eax, 1
bit_test_done:
    pop esi
    pop ebp
    ret
BitTest ENDP

BitSet PROC C
    push ebp
    mov ebp, esp
    push esi
    mov esi, DWORD PTR [ebp+8]
    mov eax, DWORD PTR [ebp+12]
    mov ecx, eax
    shr eax, 3
    and ecx, 7
    add esi, eax
    mov eax, 1
    shl eax, cl
    or DWORD PTR [esi], eax
    pop esi
    pop ebp
    ret
BitSet ENDP

BitClear PROC C
    push ebp
    mov ebp, esp
    push esi
    mov esi, DWORD PTR [ebp+8]
    mov eax, DWORD PTR [ebp+12]
    mov ecx, eax
    shr eax, 3
    and ecx, 7
    add esi, eax
    mov eax, 1
    shl eax, cl
    not eax
    and DWORD PTR [esi], eax
    pop esi
    pop ebp
    ret
BitClear ENDP

END
