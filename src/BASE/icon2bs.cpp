// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\icon2bs.obj   from: .\basewin.lib
// functions: 1   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/icon2bs.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <BASE/Icon2b.h>
#include <string.h>

// @semantic
// Clean /O2 scale-wrapper reconstruction; live 72.89% after correcting the retail statement order
// (the older structurally-wrong order had a retained 76.3262% maximum). Candidate is 0x16d bytes
// versus retail 0x179. The first raw divergence is +0x0: candidate reserves 0x10 frame bytes while
// retail reserves 0x18; at +0x3 candidate loads scale while retail spills srcIcon. All retail homes
// are semantically present: tmp, tmpPixels, srcBase, srcAdv, pitch, dstRow/srcRow, and rows. Retail
// colors scale/dest/step as EBX/ESI/EBP and spills tmpPixels; candidate spills both pointer
// arguments and colors srcBase/step/tmpPixels as EBX/ESI/EBP, coalescing the remaining roles into
// four homes. Both sides have the same identity fast path, allocation/null join, 0x800-byte clear,
// second render, sample gates, transparent-pixel branch, row/column backedges, and nullable virtual
// delete. Their ten ordered branches and successor topology agree. Relocations are exact 4/4 in
// identity, multiplicity, and order: IconToBitmap, operator new, bitmap constructor, IconToBitmap.
// Retail and the related shipped binary both advance dstPix/srcPix before cols and srcRow/dstRow
// before rows; retaining that recovered order causes the live TU-state dip. Widening pitch to the
// actual 32-bit home is code-neutral. A nested tmpPixels lifetime is neutral; inlining the three
// scale temporaries shrinks to 0x162/61.57%; direct tmp->m_pixels clearing deletes the proven
// tmpPixels home (0x16c/78.31%); top-hoisting declarations gives 0x186/76.43%. Merely placing
// rowOff before tmpPixels reaches 0x185/78.81% only with the old wrong loop order. No volatile,
// padding, alias, or permutation was used. This is not a proved wall; revisit after a real
// bitmap/icon/header TU-state change or in the post-coverage last-mile phase.
// The 2026-07-15 batched TU-state audit tested the historical pre-deumbrella `_all.h` order, all
// eight meaningful owner-header orders, 20 real BASE-header sequences, and 256 disposable
// declaration states. The full historical aggregate scored only 74.70%; order alone was neutral.
// A lone real MIDIWrap/tileset class surface reached 84.99%, while the best generated member state
// reached 81.58%, proving strong allocator-state sensitivity but not an exact closure. Adding a
// scale alias was byte-neutral in all 24 setup/row/column ownership combinations. No unused header
// or generated declaration is retained, and no sub-100 result may be recorded as MAX.
// A follow-up 315-variant structural audit crossed row scope, all outer/inner update orders, inner
// declaration order, and scale/column gate ownership with the MIDIWrap/tileset predecessor states.
// Moving `rows` before the outer gate and declaring dstPix/srcPix before cols raised the disposable
// best to 85.666664%, candidate 0x16b versus retail 0x179, with the same correct 4/4 relocations.
// Every inner-order variant remained in the same 0x16b-0x16c allocation family: retail still owns
// scale/step/src/dst as EBX/EBP/EDI/ESI and a 0x18 frame, while candidates use a 0x10 frame and
// coalesce those roles. No sub-100 source or generated predecessor state was retained.
// A consolidated follow-up fixed that 85.666664% MIDI/loop shape and exhausted 229 macro-safe
// single TU states plus 465 pair states from the typedef/extern/static-data/include families; none
// changed the 0x10 frame or exceeded the plateau. Hoisting `rows` and initializing it at entry
// produced a new 0x14-frame family at 85.602840% (0x16d), while six later initialization points
// ranged through 0x171-0x176 without the retail allocation. Hoisting tmpPixels/pitch/dstRow/srcRow,
// singly and in combinations, was code-neutral within that family. Equivalent do/while/for clear
// loops were also neutral; a pointer-advance clear regressed. Project-ironfist contains the same
// retail assembly/stack map but no source reconstruction. No generated state or sub-100 shape is
// retained; the missing scale=EBX, step=EBP, tmpPixels/dstRow spills remain unresolved.
VA(0x004d2f90, 0x179)
void IconToBitmapScale(class icon *srcIcon, class bitmap *dest, int x, int y, int frame,
                       int clip, int clipX, int clipY, int clipW, int clipH, int scale)
{
    if (scale == ICON_SCALE_NATIVE_SIZE) {
        IconToBitmap(srcIcon, dest, x, y, frame, clip, clipX, clipY, clipW, clipH, 0);
        return;
    }
    int step = ICON_SCALE_NATIVE_SIZE / scale;
    int srcBase = ((1 - scale) * step + ICON_SCALE_NATIVE_SIZE) >> 1;
    int srcAdv = step * ICON_SCALE_WORK_BITMAP_SIZE;
    bitmap *tmp = new bitmap(0, ICON_SCALE_WORK_BITMAP_SIZE, ICON_SCALE_WORK_BITMAP_SIZE);
    int rowOff = 0;
    unsigned char *tmpPixels = tmp->m_pixels;
    do {
        memset(tmpPixels + rowOff, 0, ICON_SCALE_NATIVE_SIZE);
        rowOff = rowOff + ICON_SCALE_NATIVE_SIZE;
    } while (rowOff < ICON_SCALE_NATIVE_SIZE * ICON_SCALE_WORK_BITMAP_SIZE);
    IconToBitmap(srcIcon, tmp, 0, 0, frame, 1, 0, 0, ICON_SCALE_NATIVE_SIZE,
                 ICON_SCALE_NATIVE_SIZE, 0);
    int pitch = dest->m_width;
    unsigned char *dstRow = dest->m_pixels + y * pitch + x;
    unsigned char *srcRow =
        tmp->m_pixels + srcBase * ICON_SCALE_WORK_BITMAP_SIZE + srcBase;
    if (0 < scale) {
        int rows = scale;
        do {
            int cols = scale;
            unsigned char *dstPix = dstRow;
            unsigned char *srcPix = srcRow;
            if (0 < scale) {
                do {
                    if (*srcPix != 0)
                        *dstPix = *srcPix;
                    dstPix = dstPix + 1;
                    srcPix = srcPix + step;
                    cols = cols - 1;
                } while (cols != 0);
            }
            srcRow = srcRow + srcAdv;
            dstRow = dstRow + pitch;
            rows = rows - 1;
        } while (rows != 0);
    }
    delete tmp;
}
