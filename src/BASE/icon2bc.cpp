// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\icon2bc.obj   from: .\basewin.lib
// functions: 1   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/icon2bc.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <BASE/IconEntry.h>
#include <BASE/IconRle.h>
#include <SOURCE/dimPalette.h>
#include <string.h>
// Per-call decoder scratch — its own file-static block (0x534ca8+).
DATA(0x00534ca8) static int gCTPitch;
DATA(0x00534cac) static unsigned int gCTCnt;
DATA(0x00534cb0) static int gCTX;
DATA(0x00534cb4) static int gCTY;
DATA(0x00534cb8) static IconEntry *gCTEntry;
DATA(0x00534cbc) static int gCTRow;
DATA(0x00534cc0) static unsigned char *gCTDimPal;
DATA(0x00534cc4) static int gCTClipB;
DATA(0x00534cc8) static int gCTClipR;
DATA(0x00534ccc) static unsigned char *gCTSrcCopy;
DATA(0x00534cd0) static unsigned int gCTDimLen;
DATA(0x00534cd4) static unsigned char *gCTDst;
DATA(0x00534cd8) static unsigned char *gCTSrc;
DATA(0x00534cdc) static unsigned int gCTCnt2;
DATA(0x00534ce0) static int gCTX0;
DATA(0x00534ce4) static unsigned char gCTColor;
DATA(0x00534ce8) static unsigned int gCTRun;

// Colour-table variant: literals and solid runs use colorTable[], and dimming is gated by dimGate.
// Earlier setup, owner, command, lifetime, and CFG families are hash-pinned under
// docs/matching-matrices/icon2bc-*.tsv; do not replay them without a real header/type trigger.
// @match-note
// /O2 structural checkpoint on master 58ac64e: direct IconEntry ownership plus the real Icon2b
// icon/bitmap/IconEntry/IconRle declaration order raises this TU from 72.73% with 93/91 relocs to
// 78.94% with 90/91.  The function is 0x58c versus retail 0x5af.  Relocation-union masking leaves
// 767 differing bytes among 822 common unmasked bytes, first +0x12 and last +0x588, plus a 0x23
// retail tail.  Both reserve one stack word and have the same 15 accesses to its [esp+0x10] home;
// the complete skip/solid/shadow/fill/dim/literal/newline topology remains 86 logical blocks and
// 67 branch sites, with three equivalent compare-operand reversals in clipped literal selection.
// Every target identity is correct and no base-only target exists.  The multiset differs only by
// Cnt 5/6, X0 2/3, and Y 9/8.  Ordered evidence: retail reloads X0 at +0x61 and holds one Y load at
// +0x7b across both initial vertical clauses; candidate forwards X and reloads Y twice.  Retail's
// clipped-dim X<clip arm stores the original Cnt at +0x406 before selected Cnt at +0x42a and gives
// each selection arm its own ClipR load.  MSVC deletes the source's first Cnt store and merges the
// branch-local ClipR snapshots, then later reloads ClipR in the literal arm despite its local view.
// The source explicitly owns all of those lifetimes; volatile/dummy reads would only falsify it.
// The first broad divergence remains +0x12: candidate coalesces frame offset/entry in ECX while
// retail retains offset=EBX and entry=EDI, then uses source=ECX, command=EAX, count=EDX, cursor=EBX.
// Current-trigger attempts: full sibling byte-offset setup was 77.07/87; formal-X update was
// 78.40/90; deriving X from published X0 was 78.74/92 (X0 4/3); wider currentY scope and explicit
// per-arm ClipR locals were byte-identical.  Canonical IconRle constants are byte-neutral in the
// retained sibling include order.  Revisit only after another real declaration/type/layout change,
// not with predicate synonyms, dummy state, arbitrary record boundaries, or permutation tools.
VA(0x004d32a0, 0x5af)
void IconToBitmapColorTable(class icon *srcIcon, class bitmap *dest, int x, int y, int frame,
                            int clip, int clipX, int clipY, int clipW, int clipH, int color,
                            unsigned char *colorTable, int dimGate)
{
    unsigned char *data = reinterpret_cast<unsigned char *>(srcIcon->m_data);
    IconEntry *entry = reinterpret_cast<IconEntry *>(data) + frame;
    int entryX = entry->x;
    int sourceOffset = entry->srcOffset;
    unsigned char *savedDst;
    gCTEntry = entry;
    gCTSrc = data + sourceOffset;
    int X = x + entryX;
    gCTX0 = X;
    gCTPitch = dest->m_width;
    gCTY = entry->y + y;
    if (clip != 0) {
        int currentY = gCTY;
        if (X < clipX || clipW + clipX < entry->w + gCTX0 ||
            currentY < clipY ||
            clipY + clipH < entry->h + currentY) {
            clip = 1;
            gCTClipR = clipX + clipW - 1;
            gCTClipB = clipY + clipH - 1;
        } else {
            clip = 0;
        }
    }
    unsigned char *row =
        reinterpret_cast<unsigned char *>(gCTPitch * gCTY + reinterpret_cast<int>(dest->m_pixels));
    savedDst = gCTDst;
    int cmd;
    for (;;) {
        gCTSrc++;
        cmd = gCTSrc[-1];
        if (static_cast<signed char>(cmd) < 0) {
            if ((cmd & ICON_RLE_COMMAND_SOLID_FLAG) == 0) {
                // skip run / end-of-sprite
                gCTX = X;
                gCTRow = reinterpret_cast<int>(row);
                gCTDst = savedDst;
                gCTRun = cmd;
                if ((cmd & ICON_RLE_COMMAND_RUN_MASK) == 0)
                    return;
                X = X + (cmd & ICON_RLE_COMMAND_RUN_MASK);
                continue;
            }
            // 0xc0 - 0xff
            gCTRun = cmd;
            unsigned int count = cmd & ICON_RLE_COMMAND_RUN_MASK;
            int flags = 0;
            if (count != 0) {
                // 0xc1 - 0xff : solid colour run
                if (cmd == ICON_RLE_LONG_SOLID_COMMAND) {
                    gCTSrc++;
                    count = gCTSrc[-1];
                }
                gCTSrc++;
                gCTColor = colorTable[gCTSrc[-1]];
                goto do_fill;
            }
            // 0xc0 : shadow / dim run
            gCTSrc++;
            flags = gCTSrc[-1];
            count = flags & ICON_RLE_DIM_SHORT_COUNT_MASK;
            if (count == 0) {
                gCTSrc++;
                count = gCTSrc[-1];
            }
            gCTCnt2 = count;
            if (color != 0) {
                gCTRun = flags;
                if (flags & ICON_RLE_DIM_RECOLOR_FLAG) {
                    gCTCnt = count;
                    gCTColor = static_cast<unsigned char>(color);
                    goto do_fill;
                }
            }
            goto do_dim;
        do_fill:
            if (clip == 0) {
                memset(row + X, gCTColor, count);
            } else {
                int right;
                int fillLen;
                if (clipY <= gCTY && gCTClipB >= gCTY &&
                    (right = X + count, clipX < right) && gCTClipR >= X) {
                    if (X >= clipX) {
                        if (gCTClipR >= right) {
                            fillLen = count;
                            memset(row + X, gCTColor, fillLen);
                        } else {
                            fillLen = (gCTClipR - X) + 1;
                            memset(row + X, gCTColor, fillLen);
                        }
                    } else {
                        if (gCTClipR >= right) {
                            fillLen = (count - clipX) + X;
                            memset(row + clipX, gCTColor, fillLen);
                        } else {
                            fillLen = clipW;
                            memset(row + clipX, gCTColor, fillLen);
                        }
                    }
                }
            }
            X = X + count;
            gCTRun = count;
            continue;
        do_dim:
            gCTCnt = count;
            gCTRun = flags;
            if (flags & ICON_RLE_DIM_APPLY_FLAG) {
                unsigned char *palette =
                    reinterpret_cast<unsigned char *>(uDimPal) +
                    (flags & ICON_RLE_DIM_LEVEL_MASK) * ICON_RLE_DIM_PALETTE_LEVEL_STRIDE;
                gCTDst = savedDst;
                gCTDimPal = palette;
                if (clip == 0) {
                    savedDst = row + X;
                    gCTDimLen = 0;
                    int dimCount = count;
                    if (dimCount > 0) {
                        gCTDimLen = dimCount;
                        do {
                            if (dimGate != 0)
                                *savedDst = palette[*savedDst];
                            savedDst = savedDst + 1;
                            gCTDimPal = palette;
                            count--;
                        } while (count != 0);
                    }
                } else {
                    gCTCnt = count;
                    if (clipY <= gCTY && gCTClipB >= gCTY &&
                        (int)(X + count) > clipX &&
                        (gCTDst = savedDst, gCTClipR >= X)) {
                        int right = X + count;
                        unsigned int cn;
                        if (X >= clipX) {
                            int clipRight = gCTClipR;
                            cn = count;
                            if (clipRight < right)
                                cn = (clipRight - X) + 1;
                            savedDst = row + X;
                        } else {
                            int clipRight = gCTClipR;
                            gCTCnt = count;
                            if (right <= clipRight)
                                count = (count - clipX) + X;
                            else
                                count = clipW;
                            cn = count;
                            savedDst = row + clipX;
                        }
                        gCTDimPal = palette;
                        gCTCnt = cn;
                        gCTDimLen = 0;
                        int dimCount = cn;
                        if (dimCount > 0) {
                            gCTDimLen = dimCount;
                            do {
                                if (dimGate != 0)
                                    *savedDst = palette[*savedDst];
                                savedDst = savedDst + 1;
                                gCTDimPal = palette;
                                cn--;
                            } while (cn != 0);
                        }
                    }
                }
            }
            X = X + gCTCnt2;
            continue;
        }
        // ---- positive command : colour-table copy / newline ----
        gCTX = X;
        gCTRun = cmd;
        if (cmd != 0) {
            unsigned int cnt;
            do {
                gCTDst = savedDst;
                gCTSrcCopy = gCTSrc;
                if (clip == 0) {
                    savedDst = row + X;
                    cnt = cmd;
                } else {
                    if (gCTY < clipY || gCTClipB < gCTY) {
                        cnt = 0;
                        break;
                    }
                    int right = X + cmd;
                    if (right <= clipX || (gCTDst = savedDst, gCTClipR < X)) {
                        cnt = 0;
                        break;
                    }
                    int clipRight = gCTClipR;
                    if (clipX <= X) {
                        savedDst = row + X;
                        if (clipRight >= right)
                            cnt = cmd;
                        else
                            cnt = (clipRight - X) + 1;
                    } else {
                        if (clipRight >= right)
                            cnt = (cmd - clipX) + X;
                        else
                            cnt = clipW;
                        gCTSrcCopy = gCTSrc + (clipX - X);
                        savedDst = row + clipX;
                    }
                }
            } while (0);
            gCTCnt = cnt;
            if (cnt != 0) {
                gCTDimLen = 0;
                if (static_cast<int>(cnt) > 0) {
                    gCTDimLen = cnt;
                    do {
                        unsigned char b = *gCTSrcCopy;
                        gCTSrcCopy = gCTSrcCopy + 1;
                        *savedDst = colorTable[b];
                        cnt--;
                        savedDst = savedDst + 1;
                    } while (cnt != 0);
                }
            }
            X = X + cmd;
            gCTSrc = gCTSrc + cmd;
            gCTRun = cmd;
            continue;
        }
        // newline
        X = gCTX0;
        gCTY = gCTY + 1;
        row = row + gCTPitch;
    }
}
