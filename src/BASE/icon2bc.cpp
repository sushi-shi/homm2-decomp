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
DATA(0x00534cbc) static unsigned char *gCTRow;
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
// /O2 typed checkpoint on master e34ed0d with the canonical header-level IconRle and IconShear
// typedef enums: 78.95071%, candidate 0x58c versus retail 0x5af. Relocation-union masking leaves
// 767 differing bytes among 822 common unmasked bytes, first +0x12 and last +0x588, plus a 0x23
// retail tail. Candidate/retail have 423/426 instructions, the same 86 blocks, 133 directed edges,
// 67 branch sites, and identical ordered successor topology. Both reserve one stack word and have
// the same 15 accesses to its [esp+0x10] home.
// Relocations are 90/91 with no wrong or base-only target; uDimPal and every scratch count agree
// except Cnt 5/6, X0 2/3, and Y 9/8. Retail reloads X0 at +0x61, while candidate forwards local X.
// Retail holds one Y load at +0x7b across both initial vertical clauses; candidate reloads Y at
// +0x78 and +0x86. Retail publishes original Cnt at +0x406 and selected Cnt at +0x42a in clipped
// dim; candidate source owns both stores, but MSVC deletes the first and retains selected Cnt at
// +0x408. ClipR totals are both ten, but retail gives the two dim selection arms loads at +0x3ec
// and +0x400; candidate merges them at +0x3e7 and compensates with a literal-arm reload at +0x4d4.
// Do not manufacture these occurrences with volatile, dummy reads, aliases, or inert expressions.
// The first broad divergence remains +0x12: candidate coalesces frame offset/entry in ECX while
// retail retains offset=EBX and entry=EDI, then uses source=ECX, command=EAX, count=EDX, cursor=EBX.
// This pass corrected the proven row owner to unsigned-char pointer storage and removed all three
// pointer/int casts around row formation/publication. The gCTRow-only correction measured
// 78.985916% with 766 raw differences; the fully typed row expression is retained at
// 78.95071%/767 because source correctness wins.
// Closed prior-header axes remain: sibling byte-offset setup 77.07/87, formal-X 78.40/90,
// published-X0 derivation 78.74/92, and byte-identical wider-Y/per-arm-ClipR locals. Revisit only
// after another real declaration/type/layout change, not with predicate synonyms, count balancing,
// arbitrary record boundaries, or permutation tools.
// Fresh e34ed0d trigger audit: this compiland contains only this function, so there is no same-TU
// predecessor to steer. Replaying the current Icon2b byte-offset setup regresses to 78.50%/87:91;
// retaining entry.x across the entry/source stores then adding formal x regresses to 78.83%/90:91.
// Delaying typed entry formation through a real IconEntry table is byte-identical. Parsing the real
// owner prototype after icon/bitmap are complete is byte-identical; moving it after the scratch
// declarations regresses to 78.85%/90:91. No setup or declaration variant is retained. Reopen only
// when a reachable type/header declaration changes, not for another local spelling search.
// Fresh cfec3b8 enum-state closure: widening setup/row/command declarations to C89 scope scores
// 66.72%/89:91, while putting setup and clip temporaries in one lexical scope with X outside scores
// 75.27%/90:91. An explicit 13-byte entry offset scores 78.60%/90:91; narrowing entry.x to short
// scores 77.85%, mutating formal x scores 78.34%, and loading fields through an entry/entry-data
// alias scores 78.28%, all with 90:91. Chaining the X0 publication reaches 78.62% but emits X0
// 4/3 and 92:91 by reloading it in both horizontal clauses; a scoped current-X snapshot is byte-
// identical. These are real lifetime/type alternatives, not count fixes, and all are rejected.
// Fresh 5a48d4c accessor/type audit: forcing the exact CodeView-backed GetIconEntry body through an
// inline selection boundary scores 78.28%/90:91 and emits the same rejected entry/data-alias shape;
// it does not recover retail's EBX offset plus EDI selected-entry lifetime. Changing the serialized
// srcOffset field from signed to unsigned int is instruction-byte-identical at 78.95071%/90:91.
// The observed data proves its four-byte width but not signedness, so the existing field is retained.
// Fresh 8c85311 command-body audit raises the live checkpoint to 80.58451%, candidate 0x596 versus
// retail 0x5af, while preserving 90/91 correct relocations and the exact one-word frame. Solid-fill
// quadrant order, both dim bodies, and their shared X-advance tails already agree with retail. The
// mapped-literal loop did not: retail loads the byte through gCTSrcCopy, advances the destination,
// maps and stores it, then decrements the selected count before publishing gCTSrcCopy and taking
// the backedge (+0x547..+0x56b). Delaying that source publication and making the decrement belong
// to the loop condition recovers the same ownership/order at candidate +0x52f..+0x552. Both sides
// remain 426 instructions and 67 branch sites with the previously proved 86-block topology; first
// broad divergence is still setup +0x12. A combined source/destination post-increment scores 80.19,
// a separate next-source local scores 67.72, and splitting the mapped value into its own assignment
// scores 69.58; all were rejected. An explicit signed copy-count local is byte-identical and omitted.
// Fresh d8502da clipped-literal ownership audit raises the live checkpoint to 82.333336% and grows
// the candidate to 0x59e, only 0x11 short of retail. In retail's X<clipX subtree, both the fully
// right-visible and right-clipped bodies independently form row+clipX before selecting their count
// (+0x4f6..+0x511); only then does their join publish the adjusted gCTSrcCopy. The prior common
// savedDst assignment let MSVC merge that formation. Keeping the destination assignment in each
// real selection arm reproduces the duplicated ownership at candidate +0x4e2..+0x503 and improves
// 1.748826 points without changing semantics or relocation targets. Candidate/retail now both have
// 427 instructions, 67 branch sites, the exact 86-block topology and one-word frame; relocations
// remain 90/91 with no wrong target, and the first broad divergence remains +0x12. Reversing the
// three equivalent literal comparisons and spelling (clipX-X)+gCTSrc are byte-identical; omitted.
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
    unsigned char *row = dest->m_pixels + gCTPitch * gCTY;
    savedDst = gCTDst;
    int cmd;
    for (;;) {
        gCTSrc++;
        cmd = gCTSrc[-1];
        if (static_cast<signed char>(cmd) < 0) {
            if ((cmd & ICON_RLE_COMMAND_SOLID_FLAG) == 0) {
                // skip run / end-of-sprite
                gCTX = X;
                gCTRow = row;
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
                        static_cast<int>(X + count) > clipX &&
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
                        if (clipRight >= right) {
                            savedDst = row + clipX;
                            cnt = (cmd - clipX) + X;
                        } else {
                            savedDst = row + clipX;
                            cnt = clipW;
                        }
                        gCTSrcCopy = gCTSrc + (clipX - X);
                    }
                }
            } while (0);
            gCTCnt = cnt;
            if (cnt != 0) {
                gCTDimLen = 0;
                if (static_cast<int>(cnt) > 0) {
                    gCTDimLen = cnt;
                    do {
                        int b = *gCTSrcCopy;
                        savedDst = savedDst + 1;
                        savedDst[-1] = colorTable[b];
                        gCTSrcCopy = gCTSrcCopy + 1;
                    } while (--cnt != 0);
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
