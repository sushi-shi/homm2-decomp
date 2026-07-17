// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\Icondf2b.obj   from: .\basewin.lib
// functions: 1   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/Icondf2b.h>
#include <BASE/IconDraw.h>
#include <BASE/IconEntry.h>
#include <BASE/IconMonoRle.h>
#include <BASE/IconRle.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <SOURCE/dimPalette.h>
// Per-call decoder scratch — its own contiguous file-static block (0x5381b8+).
DATA(0x005381b8) static i32 gFDX0;
DATA(0x005381bc) static i32 gFDXEnd;
DATA(0x005381c0) static u32 gFDCnt;
DATA(0x005381c4) static i32 gFDX;
DATA(0x005381c8) static i32 gFDClipR;
DATA(0x005381cc) static u32 gFDCnt2;
DATA(0x005381d0) static u8 *gFDRow;
DATA(0x005381d4) static i32 gFDClipB;
DATA(0x005381d8) static u8 *gFDSrc;
DATA(0x005381dc) static u8 *gFDDst;
DATA(0x005381e0) static i32 gFDY;
DATA(0x005381e4) static IconEntry *gFDEntry;
DATA(0x005381e8) static u32 gFDRun;

// @semantic
// Complete /O2 decoder: candidate/retail have 166/167 instructions, both have 25 branches, both
// reserve the same four-byte pitch home and save EBX/ESI/EDI/EBP, and all 37 ordered external
// relocations resolve to the same scratch owners and uDimPal. Candidate ends at +0x235 versus
// retail +0x23b. First divergence is +0x1d: retail schedules the formal-x load before publishing
// gFDEntry, while this TU publishes the entry first and carries the left edge in EDI. The remaining
// differences are register allocation/scheduling over the same command-loop and clipping
// successors. gFDRow is a byte pointer and gFDY is ordinary scratch; the former integer row model
// and volatile reload coercion were removed. Retail tests clipX <= left both before the clipped run
// and before choosing its full arm, leaving the emitted partial-left arm unreachable; preserve that
// shipped CFG. Moving the x0 declaration ahead of entry selection is byte-identical. Adding the
// shared IconDraw mode enum and completing the existing monochrome-RLE opcode enum changed VC4.2's
// TU state and improved the canonical schedule without changing the recovered body. Revisit after
// a genuine shared-header/TU-state change or in the gated last-mile phase; do not restore qualifiers
// or integer pointer arithmetic. No permutation or generated TU-state probe was run in this pass.
// A 2026-07-15 real-header audit tested MIDIWrap, tileset, palette, font, button, and the historical
// resource tail; every state regressed from the 78.433740% direct baseline. No header is retained.
// Replacing the split cursor publication/`src[-1]` residue with the shared inline reader preserves
// the complete 37/37 per-owner relocation multiset. The 79.06626% live score is TU-state-sensitive;
// the semantic helper is retained rather than restoring compiler-shaped source for a higher score.
VA(0x004daa20, 0x23b)
void FlipDimIconToBitmap(class icon *srcIcon, class bitmap *dest, i32 x, i32 y, i32 frame,
                         i32 color, i32 clip, i32 clipX, i32 clipY, i32 clipW, i32 clipH)
{
    IconEntry *entries = reinterpret_cast<IconEntry *>(srcIcon->m_data);
    IconEntry *entry = &entries[frame];
    u8 *srcData = reinterpret_cast<u8 *>(entries) + entry->srcOffset;
    i32 x0 = x;
    gFDEntry = entry;
    gFDSrc = srcData;
    i32 w = entry->w;
    i32 entryY = entry->y;
    x0 = x0 - entry->x;
    x0 = x0 - w;
    x0++;
    gFDX0 = x0;
    gFDY = y + entryY;
    i32 X = w + x0 - 1;
    gFDXEnd = X;
    if (clip != ICON_DRAW_NO_CLIP) {
        i32 currentY;
        if (x0 < clipX || clipW + clipX < w + x0 || (currentY = gFDY) < clipY ||
            entry->h + currentY > clipY + clipH) {
            clip = ICON_DRAW_CLIP;
            gFDClipR = clipX + clipW - 1;
            gFDClipB = clipY + clipH - 1;
        } else {
            clip = ICON_DRAW_NO_CLIP;
        }
    }
    i16 pitch = dest->m_width;
    gFDRow = dest->m_pixels + gFDY * pitch;
    for (;;) {
        gFDX = X;
        i32 cmd = ReadIconRleByte(gFDSrc);
        if (static_cast<i8>(cmd) < 0) {
            gFDRun = cmd;
            i32 n = cmd & ICON_RLE_MONO_RUN_MASK;
            if (n == ICON_RLE_MONO_END_COUNT)
                return;
            X = X - n;
            continue;
        }
        gFDRun = cmd;
        if (cmd != ICON_RLE_MONO_NEWLINE_COMMAND) {
            if (clip == ICON_DRAW_NO_CLIP) {
                u32 cnt;
                gFDCnt = 0;
                u8 *dst = (gFDRow - cmd) + X + 1;
                gFDDst = dst;
                if (static_cast<i32>(cmd) > 0) {
                    cnt = cmd;
                    gFDCnt = cmd;
                    do {
                        i32 px = *dst++;
                        cnt--;
                        gFDDst = dst;
                        dst[-1] = uDimPal[0][color][px];
                    } while (cnt != 0);
                }
            } else {
                i32 left;
                i32 currentY = gFDY;
                if (clipY <= currentY && currentY <= gFDClipB &&
                    (left = (X - cmd) + 1, clipX <= left) && X <= gFDClipR) {
                    i32 cn;
                    u8 *dst;
                    if (clipX <= left) {
                        cn = cmd;
                        dst = (gFDRow - cmd) + X + 1;
                    } else {
                        dst = gFDRow + clipX;
                        cn = (X - clipX) + 1;
                    }
                    gFDCnt2 = cn;
                    gFDDst = dst;
                    gFDCnt = 0;
                    if (static_cast<i32>(cn) > 0) {
                        gFDCnt = cn;
                        u32 cnt = cn;
                        do {
                            i32 px = *dst++;
                            cnt--;
                            gFDDst = dst;
                            dst[-1] = uDimPal[0][color][px];
                        } while (cnt != 0);
                    }
                }
            }
            X = X - cmd;
            gFDRun = cmd;
            continue;
        }
        // newline
        X = gFDXEnd;
        gFDY = gFDY + 1;
        gFDRow = gFDRow + pitch;
    }
}
