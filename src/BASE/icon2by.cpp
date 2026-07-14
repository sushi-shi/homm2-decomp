// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\icon2by.obj   from: .\basewin.lib
// functions: 1   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/icon2by.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <SOURCE/X_GLOBAL.h>
#include <BASE/Misc.h>
#include <string.h>
// Per-call decoder scratch — its own file-static block.
static int gYMClipB;
static int gYMDimIdx;
static int gYMPitch;
static int gYMY;
static int gYMX;
static unsigned char *gYMDimDst;
static int gYMX0;
static unsigned int gYMRun;
static unsigned char *gYMDimPal;
static int gYMRow;
static unsigned char *gYMSrc;
static IconEntry *gYMEntry;
static unsigned int gYMDimLen;
static unsigned char gYMColor;
static unsigned int gYMDimLen2;
static int gYMClipR;

// @early-stop
// /O2 byte-proven register-allocation wall: base 0x57c versus retail 0x588. Setup and command
// parsing are instruction-identical through +0x191 (apart from relocations/forward displacements).
// Residual blocks are fill base +0x191..+0x2da vs retail +0x191..+0x2e2, dim
// +0x2da..+0x409 vs +0x2e2..+0x413, and literal +0x409..+0x575 vs +0x413..+0x581.
// Base has 129/130 retail relocations with no wrong target; the sole missing occurrence is retail's
// gYMX reload at +0x214 in the full-fill quadrant. Tried merged and four-way memcpy/memset forms,
// combined/nested/goto clipping, both comparison directions, per-branch dim destinations, semantic
// locals, inclusive-bound equivalents, and the full 129-variant AST permutation search.
VA(0x004da270, 0x588)
void IconToBitmapYModify(class icon *srcIcon, class bitmap *dest, int x, int y, int frame, int clip,
                         int clipX, int clipY, int clipW, int clipH, int color, signed char *shear)
{
    IconEntry *entries = reinterpret_cast<IconEntry *>(srcIcon->m_data);
    gYMEntry = &entries[frame];
    gYMSrc = reinterpret_cast<unsigned char *>(srcIcon->m_data) + gYMEntry->srcOffset;
    gYMX0 = gYMEntry->x + x;
    gYMPitch = dest->m_width;
    gYMY = gYMEntry->y + y;
    gYMX = shear[gYMY] + gYMX0;
    gYMClipR = clipX + clipW - 1;
    gYMClipB = clipY + clipH - 1;
    gYMRow = gYMPitch * gYMY + reinterpret_cast<int>(dest->m_pixels);
    for (;;) {
        gYMRun = *gYMSrc;
        gYMSrc = gYMSrc + 1;
        if (static_cast<signed char>(gYMRun) < 0) {
            if ((gYMRun & 0x40) == 0) {
                if ((gYMRun & 0x3f) == 0)
                    return;
                gYMX = gYMX + (gYMRun & 0x3f);
                continue;
            }
            // 0xc0 - 0xff
            if ((gYMRun & 0x3f) != 0) {
                if (gYMRun == 0xc1) {
                    gYMRun = *gYMSrc;
                    gYMSrc = gYMSrc + 1;
                } else {
                    gYMRun = gYMRun & 0x3f;
                }
                gYMColor = *gYMSrc;
                gYMSrc = gYMSrc + 1;
                goto do_fill;
            }
            gYMRun = *gYMSrc;
            gYMSrc = gYMSrc + 1;
            if ((gYMRun & 3) != 0) {
                gYMDimLen = gYMRun & 3;
            } else {
                gYMDimLen = *gYMSrc;
                gYMSrc = gYMSrc + 1;
            }
            gYMDimLen2 = gYMDimLen;
            if (color != 0 && (gYMRun & 0x80) != 0) {
                gYMRun = gYMDimLen;
                gYMColor = static_cast<unsigned char>(color);
                goto do_fill;
            }
            if ((gYMRun & 0x40) != 0) {
                gYMDimPal = reinterpret_cast<unsigned char *>(uDimPal) + (gYMRun & 0x3c) * 0x40;
                if (shear[gYMY] != 0x7f && clipY <= gYMY && gYMClipB >= gYMY &&
                    (int)(gYMDimLen + gYMX) > clipX && gYMClipR >= gYMX) {
                    int dimRight = gYMDimLen + gYMX;
                    if (clipX <= gYMX) {
                        if (gYMClipR < dimRight)
                            gYMDimLen = (gYMClipR - gYMX) + 1;
                        gYMDimDst = reinterpret_cast<unsigned char *>(gYMRow + gYMX);
                    } else {
                        if (gYMClipR >= dimRight)
                            gYMDimLen = gYMDimLen + (gYMX - clipX);
                        else
                            gYMDimLen = clipW;
                        gYMDimDst = reinterpret_cast<unsigned char *>(gYMRow + clipX);
                    }
                    gYMDimIdx = 0;
                    if (0 < static_cast<int>(gYMDimLen)) {
                        do {
                            *gYMDimDst = gYMDimPal[*gYMDimDst];
                            gYMDimDst = 1 + gYMDimDst;
                            gYMDimIdx = gYMDimIdx + 1;
                        } while (gYMDimIdx < static_cast<int>(gYMDimLen));
                    }
                }
            }
            gYMX = gYMX + gYMDimLen2;
            continue;
        do_fill:
            if (shear[gYMY] != 0x7f && clipY <= gYMY && gYMClipB >= gYMY &&
                (int)(gYMX + gYMRun) > clipX && gYMClipR >= gYMX) {
                int fillRight = gYMX + gYMRun;
                if (clipX <= gYMX) {
                    if (gYMClipR >= fillRight) {
                        memset(reinterpret_cast<unsigned char *>(gYMRow + gYMX), gYMColor,
                               gYMRun);
                    } else {
                        memset(reinterpret_cast<unsigned char *>(gYMRow + gYMX), gYMColor,
                               (gYMClipR - gYMX) + 1);
                    }
                } else {
                    if (gYMClipR >= fillRight) {
                        memset(reinterpret_cast<unsigned char *>(gYMRow + clipX), gYMColor,
                               (gYMRun - clipX) + gYMX);
                    } else {
                        memset(reinterpret_cast<unsigned char *>(gYMRow + clipX), gYMColor,
                               clipW);
                    }
                }
            }
            gYMX = gYMX + gYMRun;
            continue;
        }
        // ---- positive command : literal copy / newline ----
        if (gYMRun != 0) {
            if (shear[gYMY] != 0x7f && clipY <= gYMY && gYMClipB >= gYMY &&
                (int)(gYMX + gYMRun) > clipX && gYMClipR >= gYMX) {
                int copyRight = gYMX + gYMRun;
                if (clipX <= gYMX) {
                    if (gYMClipR >= copyRight) {
                        memcpy(reinterpret_cast<unsigned char *>(gYMRow + gYMX), gYMSrc,
                               gYMRun);
                    } else {
                        memcpy(reinterpret_cast<unsigned char *>(gYMRow + gYMX), gYMSrc,
                               (gYMClipR - gYMX) + 1);
                    }
                } else {
                    if (gYMClipR >= copyRight) {
                        memcpy(reinterpret_cast<unsigned char *>(gYMRow + clipX),
                               gYMSrc + (clipX - gYMX), (gYMRun - clipX) + gYMX);
                    } else {
                        memcpy(reinterpret_cast<unsigned char *>(gYMRow + clipX),
                               gYMSrc + (clipX - gYMX), clipW);
                    }
                }
            }
            gYMX = gYMX + gYMRun;
            gYMSrc = gYMSrc + gYMRun;
            continue;
        }
        // newline
        gYMX = shear[gYMY] + gYMX0;
        gYMY = gYMY + 1;
        gYMRow = gYMRow + gYMPitch;
    }
}
