// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\icon2bs.obj   from: .\basewin.lib
// functions: 1   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/icon2bs.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <BASE/Icon2b.h>

// @early-stop
// Scale wrapper (NOT an RLE decoder): render the icon full-size into a temp 0x40x0x40 bitmap via
// IconToBitmap, then point-sample it down to `scale` x `scale` into the destination (skipping
// transparent pixels). scale == 0x20 is the identity fast-path (straight IconToBitmap). Residual is
// the /O2 register wall around the new/delete + downsample loop (not source-steerable).
VA(0x004d2f90, 0x179)
void IconToBitmapScale(class icon *srcIcon, class bitmap *dest, int x, int y, int frame,
                       int clip, int clipX, int clipY, int clipW, int clipH, int scale)
{
    if (scale == 0x20) {
        IconToBitmap(srcIcon, dest, x, y, frame, clip, clipX, clipY, clipW, clipH, 0);
        return;
    }
    int step = 0x20 / scale;
    int srcBase = ((1 - scale) * step + 0x20) >> 1;
    int srcAdv = step * 0x40;
    bitmap *tmp = new bitmap(0, 0x40, 0x40);
    int tmpPixels = reinterpret_cast<int>(tmp->m_pixels);
    int rowOff = 0;
    do {
        int *p = reinterpret_cast<int *>(tmpPixels + rowOff);
        for (int k = 8; k != 0; k--)
            *p++ = 0;
        rowOff = rowOff + 0x20;
    } while (rowOff < 0x800);
    IconToBitmap(srcIcon, tmp, 0, 0, frame, 1, 0, 0, 0x20, 0x20, 0);
    short pitch = dest->m_width;
    char *dstRow = reinterpret_cast<char *>(x + y * pitch + reinterpret_cast<int>(dest->m_pixels));
    char *srcRow = reinterpret_cast<char *>(srcBase * 0x41 + reinterpret_cast<int>(tmp->m_pixels));
    if (0 < scale) {
        int rows = scale;
        do {
            int cols = scale;
            char *dstPix = dstRow;
            char *srcPix = srcRow;
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
            dstRow = dstRow + pitch;
            rows = rows - 1;
        } while (rows != 0);
    }
    delete tmp;
}
