#include <va.h>
#include <BASE/TILE.h>

// Hand-written tile blitter supporting normal, vertical, horizontal, and combined flips.

DATA(0x0051fec0) static u32 gTileMode;
DATA(0x0051fec4) static i32 gTileRowCtr;

VA(0x004d310c, 0x18f)
extern "C" __declspec(naked) void __cdecl
TileToBitmap(tileset* src, u32 flags, bitmap* dst, i32 x, i32 y) {
    __asm {
        push    ebp
        mov     ebp, esp
        push    esi
        push    edi
        mov     edi, dst
        movzx   ebx, word ptr [edi+12h]
        mov     eax, y
        mul     ebx
        add     eax, x
        mov     edi, [edi+16h]
        add     edi, eax
        mov     eax, flags
        mov     gTileMode, eax
        and     eax, 0FFFh
        mov     flags, eax
        mov     esi, src
        movzx   ecx, word ptr [esi+12h]
        sub     ebx, ecx
        mov     eax, ecx
        movzx   edx, word ptr [esi+14h]
        mul     edx
        mov     edx, flags
        mul     edx
        mov     dx, word ptr [esi+14h]
        mov     esi, [esi+16h]
        add     esi, eax
        mov     eax, gTileMode
        and     eax, 8000h
        jne     path_h
        mov     eax, gTileMode
        and     eax, 4000h
        jne     path_v
        mov     eax, ecx
        shr     eax, 2
        xchg    ebx, ebx
        nop
    fwd:
        mov ecx, eax
        rep movsd
        add edi, ebx
        mov ecx, eax
        rep movsd
        add edi, ebx
        mov ecx, eax
        rep movsd
        add edi, ebx
        mov ecx, eax
        rep movsd
        add edi, ebx
        mov ecx, eax
        rep movsd
        add edi, ebx
        mov ecx, eax
        rep movsd
        add edi, ebx
        mov ecx, eax
        rep movsd
        add edi, ebx
        mov ecx, eax
        rep movsd
        add edi, ebx
        mov ecx, eax
        rep movsd
        add edi, ebx
        mov ecx, eax
        rep movsd
        add edi, ebx
        mov ecx, eax
        rep movsd
        add edi, ebx
        mov ecx, eax
        rep movsd
        add edi, ebx
        mov ecx, eax
        rep movsd
        add edi, ebx
        mov ecx, eax
        rep movsd
        add edi, ebx
        mov ecx, eax
        rep movsd
        add edi, ebx
        mov ecx, eax
        rep movsd
        add edi, ebx
        sub     dx, 10h
        jne     fwd
    epi:
        pop     edi
        pop     esi
        pop     ebp
        ret
        xchg    ebx, ebx
    path_v:
        mov     eax, ecx
        dec     eax
        mul     ecx
        add     esi, eax
        mov     eax, ecx
        mov     dx, cx
    v_loop:
        mov     ecx, eax
        shr     ecx, 2
        rep     movsd
        add     edi, ebx
        sub     esi, eax
        sub     esi, eax
        dec     dx
        jne     v_loop
        jmp     epi
        nop
    path_h:
        mov     eax, gTileMode
        and     eax, 4000h
        jne     path_hv
        add     edi, ecx
        dec     edi
        mov     gTileRowCtr, edx
        mov     edx, ecx
        add     ebx, edx
        add     ebx, edx
        shr     edx, 3
    h_outer:
        mov     ecx, edx
    h_inner:
        lodsb
        mov [edi], al
        dec edi
        lodsb
        mov [edi], al
        dec edi
        lodsb
        mov [edi], al
        dec edi
        lodsb
        mov [edi], al
        dec edi
        lodsb
        mov [edi], al
        dec edi
        lodsb
        mov [edi], al
        dec edi
        lodsb
        mov [edi], al
        dec edi
        lodsb
        mov [edi], al
        dec edi
        loop    h_inner
        add     edi, ebx
        dec     gTileRowCtr
        jne     h_outer
        jmp     epi
        xchg    ebx, ebx
    path_hv:
        mov     eax, ecx
        mul     ecx
        dec     eax
        add     esi, eax
        std
        mov     edx, ecx
        shr     edx, 3
        mov     gTileRowCtr, ecx
    hv_outer:
        mov     ecx, edx
    hv_inner:
        lodsb
        mov [edi], al
        inc edi
        lodsb
        mov [edi], al
        inc edi
        lodsb
        mov [edi], al
        inc edi
        lodsb
        mov [edi], al
        inc edi
        lodsb
        mov [edi], al
        inc edi
        lodsb
        mov [edi], al
        inc edi
        lodsb
        mov [edi], al
        inc edi
        lodsb
        mov [edi], al
        inc edi
        loop    hv_inner
        add     edi, ebx
        dec     gTileRowCtr
        jne     hv_outer
        cld
        jmp     epi
    }
}
