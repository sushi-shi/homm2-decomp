#include <va.h>
#include <BASE/TILE.h>

// Hand-written tile blitter supporting normal, vertical, horizontal, and combined flips.

H2_ENUM_BEGIN(TileBlitConstant)
    BITMAP_WIDTH_OFFSET         = 0x12,
    BITMAP_PIXELS_OFFSET        = 0x16,
    TILE_WIDTH_OFFSET           = 0x12,
    TILE_HEIGHT_OFFSET          = 0x14,
    TILE_PIXELS_OFFSET          = 0x16,
    PIXELS_PER_DWORD_SHIFT      = 2,
    FORWARD_ROW_BATCH           = 16,
    PIXELS_PER_COPY_GROUP_SHIFT = 3
H2_ENUM_END(TileBlitConstant)

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
        movzx   ebx, word ptr [edi+BITMAP_WIDTH_OFFSET]
        mov     eax, y
        mul     ebx
        add     eax, x
        mov     edi, [edi+BITMAP_PIXELS_OFFSET]
        add     edi, eax
        mov     eax, flags
        mov     gTileMode, eax
        and     eax, TILE_INDEX_MASK
        mov     flags, eax
        mov     esi, src
        movzx   ecx, word ptr [esi+TILE_WIDTH_OFFSET]
        sub     ebx, ecx
        mov     eax, ecx
        movzx   edx, word ptr [esi+TILE_HEIGHT_OFFSET]
        mul     edx
        mov     edx, flags
        mul     edx
        mov     dx, word ptr [esi+TILE_HEIGHT_OFFSET]
        mov     esi, [esi+TILE_PIXELS_OFFSET]
        add     esi, eax
        mov     eax, gTileMode
        and     eax, TILE_FLIP_HORIZONTAL
        jne     path_h
        mov     eax, gTileMode
        and     eax, TILE_FLIP_VERTICAL
        jne     path_v
        mov     eax, ecx
        shr     eax, PIXELS_PER_DWORD_SHIFT
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
        sub     dx, FORWARD_ROW_BATCH
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
        shr     ecx, PIXELS_PER_DWORD_SHIFT
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
        and     eax, TILE_FLIP_VERTICAL
        jne     path_hv
        add     edi, ecx
        dec     edi
        mov     gTileRowCtr, edx
        mov     edx, ecx
        add     ebx, edx
        add     ebx, edx
        shr     edx, PIXELS_PER_COPY_GROUP_SHIFT
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
        shr     edx, PIXELS_PER_COPY_GROUP_SHIFT
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
