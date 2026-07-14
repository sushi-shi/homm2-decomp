// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\Icond2b.obj   from: .\basewin.lib
// functions: 1   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/Icond2b.h>
#include <BASE/IconEntry.h>
#include <BASE/IconRle.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <SOURCE/dimPalette.h>
// Per-call decoder scratch — its own file-static block (0x534bf0+).
DATA(0x00534bf0) static unsigned char *gDimRow;
DATA(0x00534bf4) static unsigned int gDimCnt;
DATA(0x00534bf8) static unsigned int gDimRun;
DATA(0x00534bfc) static volatile int gDimY;
DATA(0x00534c00) static volatile unsigned int gDimCnt2;
DATA(0x00534c04) static IconEntry *gDimEntry;
DATA(0x00534c08) static int gDimClipB;
DATA(0x00534c0c) static unsigned char *gDimSrc;
DATA(0x00534c10) static unsigned char * volatile gDimDst;
DATA(0x00534c14) static int gDimX;
DATA(0x00534c18) static int gDimClipR;
DATA(0x00534c1c) static volatile int gDimX0;

// @match-note
// Coverage checkpoint: the decoder semantics and CFG are complete, including the four clipped-run
// quadrants and the negative-run/newline exits.  Both objects have a four-byte frame; [esp+0x10]
// retains the signed destination pitch.  All 37 external relocations match in identity,
// multiplicity, and order (the 12 scratch owners plus uDimPal).
// Combined-state live fuzzy is 83.29% (retained maximum 87.4770%).  The first raw divergence is
// +0x21: ours
// adds the caller X to ECX before publishing gDimEntry/gDimSrc, while retail publishes those
// pointers, loads entry->y, then loads/adds X and publishes gDimX0.  Ours ends at +0x269 versus
// retail +0x26e; later differences are register/source-order choices with the same CFG successors.
// Tried: real IconEntry/row/palette types, the canonical palette header, named RLE constants, the
// palette-plane `sizeof` stride, and a retail-ordered entryX/entryY lifetime split (rejected:
// 63.28%).  This is not a proven wall;
// revisit after shared-header/TU-state changes or in the last-mile AST phase, not during coverage.
VA(0x004cfd50, 0x26e)
void DimIconToBitmap(class icon *srcIcon, class bitmap *dest, int x, int y, int frame,
                     int color, int clip, int clipX, int clipY, int clipW, int clipH)
{
    char *data = srcIcon->m_data;
    int entryOffset = frame * sizeof(IconEntry);
    int X = reinterpret_cast<IconEntry *>(data + entryOffset)->x;
    int srcOffset = reinterpret_cast<IconEntry *>(data + entryOffset)->srcOffset;
    IconEntry * const entry = reinterpret_cast<IconEntry *>(data + entryOffset);
    unsigned char * const srcData = reinterpret_cast<unsigned char *>(data + srcOffset);
    const int entryY = entry->y;
    gDimEntry = entry;
    gDimSrc = srcData;
    X = gDimX0 = x + X;
    gDimY = y + entryY;
    int currentY;
    int right;
    unsigned int cnt;
    if (clip != 0) {
        if (gDimX0 < clipX || clipW + clipX < entry->w + gDimX0 || gDimY < clipY ||
            clipY + clipH < entry->h + gDimY) {
            clip = 1;
            gDimClipR = clipX + clipW - 1;
            gDimClipB = clipY + clipH - 1;
        } else {
            clip = 0;
        }
    }
    int rowOffset = gDimY;
    short pitch = dest->m_width;
    rowOffset = rowOffset * pitch;
    unsigned char *row = dest->m_pixels + rowOffset;
    for (;;) {
        gDimX = X;
        gDimSrc++;
        int cmd = gDimSrc[-1];
        if (static_cast<signed char>(cmd) < 0) {
            // skip run / end-of-sprite
            gDimRow = row;
            gDimRun = cmd;
            int n = cmd & ICON_RLE_MONO_RUN_MASK;
            if (n == 0)
                return;
            X = X + n;
            continue;
        }
        gDimRun = cmd;
        if (cmd != 0) {
            if (clip == 0) {
                unsigned char *dst = row + X;
                unsigned int paletteOffset;
                gDimCnt = 0;
                gDimDst = dst;
                if (static_cast<int>(cmd) > 0) {
                    paletteOffset = color * sizeof(uDimPal[0][0]);
                    gDimCnt = cmd;
                    cnt = cmd;
                    do {
                        int px = *dst++;
                        cnt--;
                        gDimDst = dst;
                        dst[-1] = (&uDimPal[0][0][0])[paletteOffset + px];
                    } while (cnt != 0);
                }
            } else {
                currentY = gDimY;
                if (clipY > currentY || gDimClipB < currentY ||
                    (right = X + cmd, clipX >= right) || gDimClipR < X) {
                } else {
                    unsigned int palOffset;
                    unsigned char *dst;
                    if (X >= clipX) {
                        right = gDimClipR < right ? (gDimClipR - X) + 1 : cmd;
                        dst = row + X;
                    } else {
                        right = gDimClipR < right ? clipW : (cmd - clipX) + X;
                        dst = row + clipX;
                    }
                    cnt = right;
                    int cn = cnt;
                    gDimDst = dst;
                    gDimCnt2 = right;
                    gDimCnt = 0;
                    if (cn > 0) {
                        palOffset = color * sizeof(uDimPal[0][0]);
                        gDimCnt = cn;
                        do {
                            int px = *dst++;
                            cnt--;
                            gDimDst = dst;
                            dst[-1] = (&uDimPal[0][0][0])[palOffset + px];
                        } while (cnt != 0);
                    }
                }
            }
            X = X + cmd;
            gDimRun = cmd;
            continue;
        }
        // newline
        X = gDimX0;
        row = row + pitch;
        gDimY = gDimY + 1;
    }
}
