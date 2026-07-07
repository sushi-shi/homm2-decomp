// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\winextra\TILE.obj   from: .\basewin.lib
// functions: 1   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/TILE.h>
#include <_globals_model.h>

// Hand-written inline-asm tile blitter — the map renderer's hottest inner loop (one call per
// visible tile per frame), so New World Computing wrote it in assembly instead of leaving it to
// the /Od compiler. Pseudocode:
//
//   d = dst->pixels + y*dst->width + x                     // top-left of the destination
//   gTileMode = flags
//   s = src->pixels + src->width*src->height*(flags&0xFFF) // start of the selected frame
//   stride = dst->width - src->width                       // dest advance after each tile row
//   switch (flags & 0xC000) {
//     case 0x0000:  // straight copy, 16 rows unrolled, a row per `rep movsd` (width/4 dwords)
//       for (row = src->height; row; row -= 16) { copy_row_dwords(); d += stride; } * 16
//     case 0x4000:  // vertical flip: read rows bottom-to-top (s -= 2*width after each row)
//     case 0x8000:  // horizontal flip: write each row right-to-left, byte by byte (lodsb)
//     case 0xC000:  // 180-degree: both flips, source read backward with the direction flag (std)
//   }
// @early-stop 94%: body + all jumps byte-exact under /O2. Residual is one prologue byte — the retail
// uses ebx but does NOT save it (hand-asm clobber), whereas MSVC 4.2 always emits `push ebx` for an
// __asm that writes ebx (verified under C / C++ / /O2 / /Od). That extra push shifts every /O2
// loop-alignment nop. Likely a compiler minor-version policy diff (NWC's toolchain vs our 4.2).
VA(0x004d310c, 0x18f)
extern "C" void __cdecl TileToBitmap(bitmap *src, unsigned int flags, bitmap *dst, int x, int y)
{
    __asm {
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
