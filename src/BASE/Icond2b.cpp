// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\Icond2b.obj   from: .\basewin.lib
// functions: 1   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/Icond2b.h>
#include <BASE/IconEntry.h>
#include <BASE/IconRle.h>
#include <BASE/IconMonoRle.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <SOURCE/dimPalette.h>
// Per-call decoder scratch — its own file-static block (0x534bf0+).
DATA(0x00534bf0) static unsigned char *gDimRow;
DATA(0x00534bf4) static unsigned int gDimCnt;
DATA(0x00534bf8) static unsigned int gDimRun;
DATA(0x00534bfc) static int gDimY;
DATA(0x00534c00) static unsigned int gDimCnt2;
DATA(0x00534c04) static IconEntry *gDimEntry;
DATA(0x00534c08) static int gDimClipB;
DATA(0x00534c0c) static unsigned char *gDimSrc;
DATA(0x00534c10) static unsigned char *gDimDst;
DATA(0x00534c14) static int gDimX;
DATA(0x00534c18) static int gDimClipR;
DATA(0x00534c1c) static int gDimX0;

// @match-note
// Complete decoder CFG: negative skip/end, newline, unclipped dim, and all four clipped-run
// quadrants. Both objects use a four-byte frame and [esp+0x10] retains the signed destination
// pitch. The first divergence is +0x1f: retail adds the icon-data base to the encoded source offset
// before publishing gDimEntry; this TU state schedules that add after the store. Candidate code ends
// at +0x258 versus retail +0x26e. Relocations are 34/37 with no excess: MSVC forwards the live X
// value through the two initial gDimX0 predicates and forwards the first gDimY clipping value into
// the second predicate, omitting exactly two gDimX0 loads and one gDimY load. Prior `volatile`
// qualifiers manufactured those reloads and were removed. Tried the typed IconEntry array root,
// retail publication lifetimes, and retail-positive clipping predicates. Revisit after a genuine
// shared-header/TU-state change; do not restore qualifiers or add alias/reload coercion.
VA(0x004cfd50, 0x26e)
void DimIconToBitmap(class icon *srcIcon, class bitmap *dest, int x, int y, int frame,
                     int color, int clip, int clipX, int clipY, int clipW, int clipH)
{
    IconEntry *entries = reinterpret_cast<IconEntry *>(srcIcon->m_data);
    int entryX = entries[frame].x;
    IconEntry * const entry = &entries[frame];
    unsigned char * const srcData =
        reinterpret_cast<unsigned char *>(entries) + entries[frame].srcOffset;
    gDimEntry = entry;
    const int entryY = entry->y;
    gDimSrc = srcData;
    int X = x + entryX;
    gDimX0 = X;
    gDimY = y + entryY;
    int right;
    unsigned int cnt;
    if (clip != 0) {
        if (clipX > gDimX0 || gDimX0 + entry->w > clipX + clipW || gDimY < clipY ||
            gDimY + entry->h > clipY + clipH) {
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
                if (gDimY >= clipY && gDimY <= gDimClipB &&
                    (right = X + cmd, clipX < right) && X <= gDimClipR) {
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
