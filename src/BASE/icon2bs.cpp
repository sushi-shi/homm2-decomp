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

// @match-note
// Clean /O2 scale-wrapper reconstruction; live 76.33% (previous retained max 76.3262%). Candidate
// is 0x185 bytes versus retail 0x179. The first raw divergence is +0x0: candidate reserves 0x14
// frame bytes while retail reserves 0x18; at +0x3 candidate spills dest, while retail spills
// srcIcon. All retail homes are semantically present: tmp, tmpPixels, srcBase, srcAdv, pitch,
// dstRow/srcRow, and the reused srcIcon/row-count home. Retail colors scale/dest/step as
// EBX/ESI/EBP and spills tmpPixels; candidate colors scale/step/tmpPixels as ESI/EBX/EBP, spills
// dest, and coalesces those roles into five homes. Both sides have the same identity fast path,
// allocation/null join, 0x800-byte clear loop in 0x20-byte chunks, second render, sample gates,
// transparent-pixel branch, row/column backedges, and nullable virtual delete. Their complete ten
// ordered branch mnemonics and successor topology agree. Relocations are exact 4/4 in identity,
// multiplicity, and order: IconToBitmap, operator new, bitmap constructor, IconToBitmap.
// Restoring tmpPixels as a byte pointer is code-neutral; expressing the clear as intrinsic memset
// improves 75.47% -> 76.43%. Typed enum constants and byte-pointer row arithmetic retain 76.33%.
// No volatile/padding local or permutation was used. This is not a proved wall; revisit after a
// real bitmap/icon/header TU-state change or in the post-coverage last-mile phase.
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
    unsigned char *tmpPixels = tmp->m_pixels;
    int rowOff = 0;
    do {
        memset(tmpPixels + rowOff, 0, ICON_SCALE_NATIVE_SIZE);
        rowOff = rowOff + ICON_SCALE_NATIVE_SIZE;
    } while (rowOff < ICON_SCALE_NATIVE_SIZE * ICON_SCALE_WORK_BITMAP_SIZE);
    IconToBitmap(srcIcon, tmp, 0, 0, frame, 1, 0, 0, ICON_SCALE_NATIVE_SIZE,
                 ICON_SCALE_NATIVE_SIZE, 0);
    short pitch = dest->m_width;
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
                    cols = cols - 1;
                    dstPix = dstPix + 1;
                    srcPix = srcPix + step;
                } while (cols != 0);
            }
            srcRow = srcRow + srcAdv;
            rows = rows - 1;
            dstRow = dstRow + pitch;
        } while (rows != 0);
    }
    delete tmp;
}
