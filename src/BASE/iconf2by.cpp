// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\iconf2by.obj   from: .\basewin.lib
// functions: 1   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/iconf2by.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <SOURCE/X_GLOBAL.h>
#include <BASE/Misc.h>
#include <string.h>
// Per-call decoder scratch — its own file-static block.
static int gFYXEnd;
static unsigned char *gFYDimPal;
static int gFYX0;
static unsigned int gFYDimLen2;
static IconEntry *gFYEntry;
static unsigned char *gFYDst;
static int gFYX;
static int gFYSkip;
static unsigned char *gFYSrc;
static unsigned char gFYColor;
static int gFYDimLen;
static int gFYClipB;
static int gFYDimIdx;
static int gFYClipR;
static int gFYRow;
static int gFYRun;
static int gFYY;
static unsigned char *gFYDimDst;

// @early-stop
// /O2 register-allocation/intrinsic wall after complete decoder recovery: base is 0x546 bytes and
// retail is 0x58d. The relocation/branch-masked command decoder is byte-identical at base
// +0xae..+0x138 versus retail +0xb2..+0x13c. Residual spans are setup base +0x00..+0xad versus
// retail +0x00..+0xb1, fill/dim base +0x139..+0x359 versus retail +0x13d..+0x395, and clipped
// literal/newline base +0x35a..+0x545 versus retail +0x396..+0x58c: retail reserves an unused
// four-byte frame slot, pins clipW in EBP, reloads shear, and schedules the three inlined loops
// differently. Relocations are base 140 versus retail 144 with no base-only target; the uDimPal
// target and all 18 file-static scratch addresses agree. Tried signed/unsigned scratch lengths,
// local/global flags, pitch, clip-width and advance forms, split/repeated clipping expressions,
// 124 AST variants over eight improving rounds plus 80 walks, and 60 text-shape variants.
// Retail's fill/dim outer gates require the entire run inside the horizontal clip, so their later
// left-clipping arms are dead. Replacing those gates with ordinary overlap semantics was tested
// and rejected: it changes both compare sequences and lowers the function match to 73.58%.
VA(0x004d9ce0, 0x58d)
void FlipIconToBitmapYModify(class icon *srcIcon, class bitmap *dest, int x, int y, int frame,
                             int clip, int clipX, int clipY, int clipW, int clipH, int color,
                             signed char *shear)
{
    IconEntry *entries = reinterpret_cast<IconEntry *>(srcIcon->m_data);
    gFYEntry = &entries[frame];
    gFYSrc = reinterpret_cast<unsigned char *>(srcIcon->m_data) + gFYEntry->srcOffset;
    gFYX0 = ((x - gFYEntry->w) - gFYEntry->x) + 1;
    gFYXEnd = gFYEntry->w + gFYX0 - 1;
    gFYY = gFYEntry->y + y;
    gFYX = gFYXEnd - shear[gFYY];
    gFYClipB = clipY + clipH - 1;
    gFYClipR = clipX + clipW - 1;
    gFYRow = dest->m_width * gFYY + reinterpret_cast<int>(dest->m_pixels);
    for (;;) {
        gFYRun = *gFYSrc;
        gFYSrc = gFYSrc + 1;
        if (static_cast<signed char>(gFYRun) < 0) {
            if ((gFYRun & 0x40) == 0) {
                if ((gFYRun & 0x3f) == 0)
                    return;
                gFYX = gFYX - (gFYRun & 0x3f);
                continue;
            }
            if ((gFYRun & 0x3f) != 0) {
                if (gFYRun == 0xc1) {
                    gFYRun = *gFYSrc;
                    gFYSrc = gFYSrc + 1;
                } else {
                    gFYRun = gFYRun & 0x3f;
                }
                gFYColor = *gFYSrc;
                gFYSrc = gFYSrc + 1;
                goto do_fill;
            }
            gFYRun = *gFYSrc;
            gFYDimLen = gFYRun & 3;
            gFYSrc = gFYSrc + 1;
            if (gFYDimLen == 0) {
                gFYDimLen = *gFYSrc;
                gFYSrc = gFYSrc + 1;
            }
            gFYDimLen2 = gFYDimLen;
            if (color != 0 && (gFYRun & 0x80) != 0) {
                gFYColor = static_cast<unsigned char>(color);
                gFYRun = gFYDimLen;
                goto do_fill;
            }
            if ((gFYRun & 0x40) != 0) {
                gFYDimPal =
                    reinterpret_cast<unsigned char *>(uDimPal) + (gFYRun & 0x3c) * 0x40;
                if (shear[gFYY] != 0x7f && clipY <= gFYY && gFYY <= gFYClipB) {
                    if (clipX <= (gFYX - gFYDimLen) + 1 && gFYX <= gFYClipR) {
                        if ((gFYX - gFYDimLen) + 1 < clipX) {
                            gFYDimDst = reinterpret_cast<unsigned char *>(gFYRow + clipX);
                            gFYDimLen = (gFYX - clipX) + 1;
                        } else {
                            gFYDimDst = reinterpret_cast<unsigned char *>((gFYRow - gFYDimLen) + gFYX + 1);
                        }
                        gFYDimIdx = 0;
                        if (0 <= static_cast<int>(gFYDimLen) - 1) {
                            do {
                                *gFYDimDst = gFYDimPal[*gFYDimDst];
                                gFYDimDst = gFYDimDst + 1;
                                gFYDimIdx = gFYDimIdx + 1;
                            } while (gFYDimIdx < static_cast<int>(gFYDimLen));
                        }
                    }
                }
            }
            gFYX = gFYX - gFYDimLen2;
            continue;
        do_fill:
            if (shear[gFYY] != 0x7f && clipY <= gFYY && gFYY <= gFYClipB) {
                if (clipX <= (gFYX - gFYRun) + 1 && gFYX <= gFYClipR) {
                    if ((gFYX - gFYRun) + 1 < clipX) {
                        memset(reinterpret_cast<unsigned char *>(gFYRow + clipX), gFYColor,
                               (gFYX - clipX) + 1);
                    } else {
                        memset(reinterpret_cast<unsigned char *>((gFYRow - gFYRun) + 1 + gFYX),
                               gFYColor, gFYRun);
                    }
                }
            }
            gFYX = gFYX - gFYRun;
            continue;
        }
        // ---- positive command : backward literal copy / newline ----
        if (gFYRun != 0) {
            unsigned int advance = gFYRun;
            if (shear[gFYY] != 0x7f && clipY <= gFYY && gFYY <= gFYClipB) {
                int left = (gFYX - gFYRun) + 1;
                if (left <= gFYClipR && clipX <= gFYX) {
                    if (gFYClipR < gFYX) {
                        gFYDst = reinterpret_cast<unsigned char *>(gFYClipR + gFYRow);
                        gFYSrc = gFYSrc + (gFYX - gFYClipR);
                        if ((gFYX - gFYRun) < clipX) {
                            gFYSkip = ((gFYRun - gFYX) - clipW) + gFYClipR;
                            gFYDimLen = clipW;
                        } else {
                            gFYSkip = 0;
                            gFYDimLen = (gFYRun - gFYX) + gFYClipR;
                        }
                    } else {
                        gFYDst = reinterpret_cast<unsigned char *>(gFYRow + gFYX);
                        if (left < clipX) {
                            gFYDimLen = (gFYX - clipX) + 1;
                            gFYSkip = gFYRun - gFYDimLen;
                        } else {
                            gFYSkip = 0;
                            gFYDimLen = gFYRun;
                        }
                    }
                    gFYDimIdx = 0;
                    advance = gFYSkip;
                    if (0 < static_cast<int>(gFYDimLen)) {
                        do {
                            *gFYDst = *gFYSrc;
                            gFYSrc = gFYSrc + 1;
                            gFYDst = gFYDst - 1;
                            gFYDimIdx = gFYDimIdx + 1;
                        } while (gFYDimIdx < static_cast<int>(gFYDimLen));
                    }
                }
            }
            gFYX = gFYX - gFYRun;
            gFYSrc = gFYSrc + advance;
            continue;
        }
        // newline
        gFYX = gFYXEnd - shear[gFYY];
        gFYY = gFYY + 1;
        gFYRow = gFYRow + dest->m_width;
    }
}
