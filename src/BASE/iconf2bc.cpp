// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\iconf2bc.obj   from: .\basewin.lib
// functions: 1   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/iconf2bc.h>
#include <BASE/IconEntry.h>
#include <BASE/IconRle.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <SOURCE/dimPalette.h>
#include <string.h>
DATA(0x005380d4) static int gFCSkip;
DATA(0x00538104) static unsigned int gFCRun;
DATA(0x005380f8) static int gFCX0;
DATA(0x005380e4) static int gFCXEnd;
DATA(0x005380e8) static unsigned int gFCCnt;
DATA(0x005380e0) static unsigned int gFCCnt2;
DATA(0x005380d8) static int gFCY;
DATA(0x005380ec) static IconEntry *gFCEntry;
DATA(0x005380d0) static int gFCX;
DATA(0x005380f0) static unsigned char *gFCSrc;
DATA(0x005380dc) static unsigned char *gFCDimPal;
DATA(0x005380f4) static unsigned char *gFCDimDst;
DATA(0x005380fc) static int gFCClipB;
DATA(0x00538100) static unsigned char *gFCRow;
DATA(0x005380c4) static unsigned int gFCDimLen;
DATA(0x005380cc) static unsigned char gFCColor;
DATA(0x005380c8) static int gFCClipR;
DATA(0x005380c0) static unsigned char *gFCDst;

// @semantic
// Complete typed decoder with the canonical IconEntry/layout, enum constants, dim-palette owner,
// byte-pointer rows, retail DATA owners, eight-byte frame, and the same 76 ordered blocks and 59
// branches as retail. On master 71bdca9 the unused IconShear typedef in IconRle.h lowered this TU
// to 84.33%. Moving the shear and monochrome typedef enums to consumer-only headers is a real
// declaration-ownership correction and raises it through 85.27% to 85.32%. From the shear-scoped
// stage, mono scoping improves four siblings and leaves the other three unchanged; a fifth score
// gain was rejected because it left shear sentinels as raw literals. Moving IconRle after bitmap.h,
// dimPalette.h, or string.h is byte-identical, so declaration placement is closed here.
// A fresh selected-frame audit under the consumer-scoped enum state recovered a real typed-root
// boundary: retain the IconEntry array root, read the indexed width/X fields, then bind the selected
// entry while advancing a separate byte cursor. This raises live match from 85.32% to 85.99% and
// removes the old +0x0b divergence: candidate now keeps icon data/source in retail ESI and working
// X in retail EBX through the decoder. Candidate is 0x53c versus retail 0x54d and 393 versus 397
// instructions. Relocations remain 84/83 with every target and occurrence agreeing except gFCY
// 9/8. Candidate stores Y at relocation +0x4f, then loads it at +0x76 and +0x84; retail stores at
// +0x55 and loads once at +0x77, retaining ECX through the bottom test.
// First raw divergence is now +0x11: candidate forms the final frame offset before loading formal
// X, then loads entry X before width; retail loads formal X into EBP before that LEA, loads width
// before entry X, and spills width at [esp+0x14] for the horizontal upper edge. Binding `entry`
// between the two indexed reads is byte-identical. Moving the semantic `+1` into the initial X
// expression regresses to 85.63%, so both were reverted. The source still owns distinct
// width/destination-pitch lifetimes; the earlier width/pitch family remains closed.
// Reusing by-value `frame` as the byte offset is the only tested real source shape that moves the
// formal-X load before the final frame LEA: +0x0e..+0x15 then exactly matches retail scheduling.
// It scores 84.58%, however, because width lands in EAX and entry X in ECX, the retail width spill
// is still absent, and downstream allocation regresses. Swapping the field-read statements and a
// left-associated combined subtraction are byte-identical at 84.58%. An explicit entry-X snapshot
// and a named byte-offset local each score 83.46%; accumulating into by-value x scores 83.71%; a
// direct byte root scores 84.87%; and advancing the typed root scores 83.09%. The latter forms also
// lose the recovered ESI source/EBX X allocation. Do not repeat these frame/root spellings: revisit
// only after a real type or declaration-state correction naturally changes the width/X allocation.
// Under this scoped-header state, accumulating into formal y gives 81.83%/81:83 because MSVC
// propagates y through the clip tests and row setup. A sibling-style clip-home memcpy reaches
// 85.07%/83:83 only by adding a non-retail store/reload, and direct global predicates give
// 82.89%/81:83. Direct gFCY publication remains 85.04%/84:83. All were reverted: matching a count
// without the surrounding instructions is not structural recovery. Earlier width/pitch, snapshot,
// staged-skip, C89 declaration, static-order, and enum-grouping attempts remain closed. The
// residual is unresolved, not a byte-proven wall; no regex or AST permuter was used.
// A fresh command-body/row-ownership audit on d8502da retained one source correction: the selected
// IconEntry now owns the horizontal clip width directly instead of preserving that value through
// the later destination-pitch local. This raises live match from 85.9925% to 86.31738% and aligns
// row setup and decoder entry at retail +0xcc/+0xec. A current-header family batch then moved the
// command lifetime outside the loop, matching the forward decoders' authored structure. Direct
// target-to-candidate objdiff rises from 83.760704% to 84.554150%; candidate size grows one byte
// toward retail without changing the setup divergence or relocation stream. Candidate is now
// 0x542 versus retail 0x54d. Excluding retail's three trailing padding NOPs, candidate/retail are
// 395/397 instructions; both retain the eight-byte frame and 59 branches with the previously
// proved 76-block topology.
// The first divergence remains +0x11. Candidate reloads entry->w at +0x61 for the second horizontal
// clause, whereas retail spills the original width at +0x32 and consumes [esp+0x14] at +0x6b.
// A clip-local width snapshot regresses to 85.93%, moves decoder entry back to +0xea, and does not
// create that spill, so it was reverted. Relocations remain 84/83 with only gFCY 9/8: candidate
// stores at relocation +0x51 and loads at +0x78/+0x86; retail stores at +0x55 and loads once at
// +0x77 while retaining ECX. Revisit only after real type/declaration/TU-state evidence changes
// this allocation; do not introduce an alias, dummy reload, volatile state, or count-only local.
// A 2026-07-15 256-state exact-only sweep reached only a disposable 84.654915% (baseline
// 84.554150%) and no exact closure; all generated declarations were removed and no MAX was recorded.
VA(0x004d9790, 0x54d)
void FlipIconToBitmapColorTable(class icon *srcIcon, class bitmap *dest, int x, int y, int frame,
                                int clip, int clipX, int clipY, int clipW, int clipH, int color,
                                unsigned char *colorTable)
{
    IconEntry *entries = reinterpret_cast<IconEntry *>(srcIcon->m_data);
    unsigned char *src = reinterpret_cast<unsigned char *>(entries);
    int x0 = x;
    int w;
    int pitch;
    w = entries[frame].w;
    x0 = x0 - entries[frame].x;
    IconEntry *entry = &entries[frame];
    x0 = x0 - w;
    gFCEntry = entry;
    src += entry->srcOffset;
    x0++;
    gFCX0 = x0;
    w = w + x0;
    int X = (gFCXEnd = w - 1);
    int Y = y + entry->y;
    gFCY = Y;
    if (clip != 0) {
        int currentY = gFCY;
        if (x0 < clipX || clipW + clipX < x0 + entry->w ||
            currentY < clipY || clipY + clipH < entry->h + currentY) {
            clip = 1;
            gFCClipR = clipX + clipW - 1;
            gFCClipB = clipY + clipH - 1;
        } else {
            clip = 0;
        }
    }
    pitch = dest->m_width;
    gFCRow = dest->m_pixels + gFCY * pitch;
    int cmd;
    for (;;) {
        cmd = *src++;
        if (static_cast<signed char>(cmd) < 0) {
            if ((cmd & ICON_RLE_COMMAND_SOLID_FLAG) == 0) {
                // skip run / end-of-sprite
                gFCRun = cmd;
                int n = cmd & ICON_RLE_COMMAND_RUN_MASK;
                gFCX = X;
                gFCSrc = src;
                if (n == 0)
                    return;
                X = X - n;
                continue;
            }
            // 0xc0 - 0xff
            gFCRun = cmd;
            unsigned int count = cmd & ICON_RLE_COMMAND_RUN_MASK;
            int flags = 0;
            if (count != 0) {
                // 0xc1 - 0xff : solid colour run
                if (cmd == ICON_RLE_LONG_SOLID_COMMAND)
                    count = *src++;
                gFCColor = colorTable[*src++];
                goto do_fill;
            }
            // 0xc0 : shadow / dim run
            flags = *src++;
            count = flags & ICON_RLE_DIM_SHORT_COUNT_MASK;
            if (count == 0)
                count = *src++;
            gFCDimLen = count;
            if (color != 0) {
                gFCRun = flags;
                if (flags & ICON_RLE_DIM_RECOLOR_FLAG) {
                    gFCCnt2 = count;
                    gFCColor = static_cast<unsigned char>(color);
                    goto do_fill;
                }
            }
            goto do_dim;
        do_fill:
            if (clip == 0) {
                memset((gFCRow - count) + 1 + X, gFCColor, count);
            } else {
                int currentY = gFCY;
                int left;
                if (currentY >= clipY && currentY <= gFCClipB &&
                    (left = (X - count) + 1, clipX <= left) && X <= gFCClipR) {
                    if (clipX <= left) {
                        memset((gFCRow - count) + 1 + X, gFCColor, count);
                    } else {
                        memset(gFCRow + clipX, gFCColor, (X - clipX) + 1);
                    }
                }
            }
            X = X - count;
            gFCRun = count;
            continue;
        do_dim:
            gFCRun = flags;
            gFCCnt2 = count;
            if (flags & ICON_RLE_DIM_APPLY_FLAG) {
                unsigned char *palette =
                    &uDimPal[0][0][0] +
                    (flags & ICON_RLE_DIM_LEVEL_MASK) * ICON_RLE_DIM_PALETTE_LEVEL_STRIDE;
                gFCDimPal = palette;
                if (clip == 0) {
                    unsigned char *dp = (gFCRow - count) + 1 + X;
                    gFCCnt = 0;
                    int dimCount = count;
                    gFCDimDst = dp;
                    if (dimCount > 0) {
                        gFCCnt = dimCount;
                        do {
                            unsigned char *dimPalette = gFCDimPal;
                            int px = *dp;
                            dp++;
                            count--;
                            gFCDimDst = dp;
                            dp[-1] = dimPalette[px];
                        } while (count != 0);
                    }
                } else {
                    int currentY = gFCY;
                    gFCCnt2 = count;
                    if (currentY >= clipY && currentY <= gFCClipB &&
                        clipX <= static_cast<int>((X - count) + 1) && X <= gFCClipR) {
                        int left = (X - count) + 1;
                        unsigned char *dp;
                        if (clipX <= left) {
                            dp = (gFCRow - count) + 1 + X;
                        } else {
                            count = (X - clipX) + 1;
                            dp = gFCRow + clipX;
                        }
                        gFCCnt2 = count;
                        gFCDimDst = dp;
                        int dimCount = count;
                        gFCCnt = 0;
                        if (dimCount > 0) {
                            gFCCnt = dimCount;
                            do {
                                unsigned char *dimPalette = gFCDimPal;
                                int px = *dp;
                                dp++;
                                count--;
                                gFCDimDst = dp;
                                dp[-1] = dimPalette[px];
                            } while (count != 0);
                        }
                    }
                }
            }
            X = X - gFCDimLen;
            continue;
        }
        // ---- positive command : colour-table backward copy / newline ----
        gFCRun = cmd;
        gFCX = X;
        if (cmd != 0) {
            if (clip == 0) {
                gFCCnt = 0;
                unsigned char *dst = gFCRow + X;
                gFCDst = dst;
                if (cmd > 0) {
                    gFCCnt = cmd;
                    int k = cmd;
                    do {
                        int c = *src++;
                        dst--;
                        gFCDst = dst;
                        dst[1] = colorTable[c];
                        k--;
                    } while (k != 0);
                }
            } else {
                int currentY = gFCY;
                if (currentY >= clipY && currentY <= gFCClipB) {
                    int left = (X - cmd) + 1;
                    if (left <= gFCClipR && clipX <= X) {
                        unsigned int cn;
                        unsigned char *selectedDst;
                        int skip;
                        if (X <= gFCClipR) {
                            selectedDst = gFCRow + X;
                            gFCDst = selectedDst;
                            if (clipX <= left) {
                                gFCSkip = 0;
                                cn = cmd;
                            } else {
                                cn = (X - clipX) + 1;
                                skip = cmd - cn;
                                goto set_skip;
                            }
                        } else {
                            int right = gFCClipR;
                            src = src + (X - right);
                            selectedDst = gFCRow + right;
                            gFCDst = selectedDst;
                            if (clipX <= (X - cmd)) {
                                gFCSkip = 0;
                                cn = (cmd - X) + gFCClipR;
                            } else {
                                skip = gFCClipR + ((cmd - X) - clipW);
                                cn = clipW;
                                goto set_skip;
                            }
                        }
                        goto skip_set;
                    set_skip:
                        gFCSkip = skip;
                    skip_set:
                        int copyCount = cn;
                        gFCCnt2 = cn;
                        gFCCnt = 0;
                        if (copyCount > 0) {
                            gFCCnt = copyCount;
                            do {
                                unsigned char *dst = gFCDst;
                                int c = *src++;
                                dst--;
                                cn--;
                                unsigned char mapped = colorTable[c];
                                gFCDst = dst;
                                dst[1] = mapped;
                            } while (cn != 0);
                        }
                        src = src + gFCSkip;
                    } else {
                        src = src + cmd;
                    }
                } else {
                    src = src + cmd;
                }
            }
            X = X - cmd;
            gFCRun = cmd;
            continue;
        }
        // newline
        X = gFCXEnd;
        gFCY = gFCY + 1;
        gFCRow = gFCRow + pitch;
    }
}
