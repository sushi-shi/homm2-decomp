// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\iconf2by.obj   from: .\basewin.lib
// functions: 1   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/iconf2by.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <SOURCE/X_GLOBAL.h>
#include <_globals_model.h>
#include <BASE/Misc.h>
// Per-call decoder scratch — its own 0x538108+ block.
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
static unsigned int gFYDimLen;
static int gFYClipB;
static int gFYDimIdx;
static int gFYClipR;
static int gFYRow;
static unsigned int gFYRun;
static int gFYY;
static unsigned char *gFYDimDst;

// @early-stop
// Flip+Y-modify variant: horizontal-flip decoder (pixels reversed, dst decrements) whose per-row X is
// sheared by param_12[Y] (0x7f = skip-row). Own 0x538108 scratch block. Full logic recovered; 0% for the
// shared Icon*2b* /O2 register-fusion codegen wall (Icon2b.cpp). Kept per breadth mandate. Completes 11/11.
VA(0x004d9ce0, 0x58d)
void FlipIconToBitmapYModify(class icon *param_1, class bitmap *param_2, int param_3, int param_4,
                            int param_5, int param_6, int param_7, int param_8, int param_9, int param_10,
                            int param_11, signed char *param_12)
{
    unsigned char bVar1;
    int iVar4;
    unsigned int uVar3, uVar5, uVar6;
    unsigned char *pbVar2;
    int *puVar7;
    IconEntry *entries = reinterpret_cast<IconEntry *>(param_1->m_data);
    gFYEntry = &entries[param_5];
    gFYSrc = reinterpret_cast<unsigned char *>(param_1->m_data) + gFYEntry->srcOffset;
    gFYX0 = ((param_3 - gFYEntry->w) - gFYEntry->x) + 1;
    gFYXEnd = gFYEntry->w + gFYX0 - 1;
    gFYY = gFYEntry->y + param_4;
    gFYX = gFYXEnd - param_12[gFYY];
    gFYClipR = param_7 + param_9 - 1;
    gFYClipB = param_8 + param_10 - 1;
    short sVar2 = param_2->m_width;
    gFYRow = sVar2 * gFYY + reinterpret_cast<int>(param_2->m_pixels);
    do {
        while (1) {
            while (1) {
                while (1) {
                    pbVar2 = gFYSrc;
                    bVar1 = *gFYSrc;
                    gFYRun = bVar1;
                    gFYSrc = gFYSrc + 1;
                    if (static_cast<signed char>(bVar1) < 0)
                        break;
                    if (gFYRun == 0) {
                        gFYX = gFYXEnd - param_12[gFYY];
                        gFYRow = gFYRow + sVar2;
                        gFYY = gFYY + 1;
                    } else {
                        uVar6 = gFYRun;
                        if (param_12[gFYY] != 0x7f && param_8 <= gFYY && gFYY <= gFYClipB) {
                            iVar4 = (gFYX - gFYRun) + 1;
                            if (iVar4 <= gFYClipR && param_7 <= gFYX) {
                                if (gFYClipR < gFYX) {
                                    gFYSrc = gFYSrc + (gFYX - gFYClipR);
                                    gFYDst = reinterpret_cast<unsigned char *>(gFYClipR + gFYRow);
                                    if (static_cast<int>(gFYX - gFYRun) < param_7) {
                                        gFYDimLen = param_9;
                                        gFYSkip = ((gFYRun - gFYX) - param_9) + gFYClipR;
                                    } else {
                                        gFYSkip = 0;
                                        gFYDimLen = (gFYRun - gFYX) + gFYClipR;
                                    }
                                } else {
                                    gFYDst = reinterpret_cast<unsigned char *>(gFYRow + gFYX);
                                    if (iVar4 < param_7) {
                                        gFYDimLen = (gFYX - param_7) + 1;
                                        gFYSkip = gFYRun - gFYDimLen;
                                    } else {
                                        gFYSkip = 0;
                                        gFYDimLen = gFYRun;
                                    }
                                }
                                gFYDimIdx = 0;
                                uVar6 = gFYSkip;
                                if (0 < static_cast<int>(gFYDimLen)) {
                                    do {
                                        *gFYDst = *gFYSrc;
                                        gFYDst = gFYDst + -1;
                                        gFYSrc = gFYSrc + 1;
                                        gFYDimIdx = gFYDimIdx + 1;
                                        uVar6 = gFYSkip;
                                    } while (gFYDimIdx < static_cast<int>(gFYDimLen));
                                }
                            }
                        }
                        gFYSrc = gFYSrc + uVar6;
                        gFYX = gFYX - gFYRun;
                    }
                }
                if ((bVar1 & 0x40) != 0)
                    break;
                if ((bVar1 & 0x3f) == 0)
                    return;
                gFYX = gFYX - (bVar1 & 0x3f);
            }
            if ((bVar1 & 0x3f) == 0)
                break;
            {
                int bVar8 = gFYRun == 0xc1;
                gFYRun = gFYRun & 0x3f;
                if (bVar8) {
                    bVar1 = *gFYSrc;
                    gFYSrc = pbVar2 + 2;
                    gFYRun = bVar1;
                }
            }
            gFYColor = *gFYSrc;
            gFYSrc = gFYSrc + 1;
LAB_004d9e88:
            uVar6 = gFYRun;
            if (param_12[gFYY] != 0x7f && param_8 <= gFYY && gFYY <= gFYClipB &&
                (iVar4 = (gFYX - gFYRun) + 1, param_7 <= iVar4 && gFYX <= gFYClipR)) {
                if (iVar4 < param_7) {
                    uVar6 = (gFYX - param_7) + 1;
                    uVar3 = gFYColor * 0x01010101;
                    puVar7 = reinterpret_cast<int *>(gFYRow + param_7);
                    for (uVar5 = uVar6 >> 2; uVar5 != 0; uVar5--)
                        *puVar7++ = uVar3;
                } else {
                    uVar3 = gFYColor * 0x01010101;
                    puVar7 = reinterpret_cast<int *>((gFYRow - gFYRun) + 1 + gFYX);
                    for (uVar5 = gFYRun >> 2; uVar5 != 0; uVar5--)
                        *puVar7++ = uVar3;
                }
                for (uVar6 = uVar6 & 3; uVar6 != 0; uVar6--)
                    *reinterpret_cast<char *>(puVar7) = static_cast<char>(uVar3),
                    puVar7 = reinterpret_cast<int *>(reinterpret_cast<char *>(puVar7) + 1);
            }
            gFYX = gFYX - gFYRun;
        }
        bVar1 = *gFYSrc;
        gFYSrc = pbVar2 + 2;
        gFYDimLen = bVar1 & 3;
        if ((bVar1 & 3) == 0) {
            gFYDimLen = *gFYSrc;
            gFYSrc = pbVar2 + 3;
        }
        gFYDimLen2 = gFYDimLen;
        if (param_11 != 0 && (bVar1 & 0x80) != 0) {
            gFYColor = static_cast<unsigned char>(param_11);
            gFYRun = gFYDimLen;
            goto LAB_004d9e88;
        }
        if ((bVar1 & 0x40) != 0 &&
            (gFYDimPal = reinterpret_cast<unsigned char *>(uDimPal) + (bVar1 & 0x3c) * 0x40,
             param_12[gFYY] != 0x7f && param_8 <= gFYY) &&
            gFYY <= gFYClipB &&
            (iVar4 = (gFYX - gFYDimLen) + 1, param_7 <= iVar4 && gFYX <= gFYClipR)) {
            if (iVar4 < param_7) {
                gFYDimLen = (gFYX - param_7) + 1;
                gFYDimDst = reinterpret_cast<unsigned char *>(gFYRow + param_7);
            } else {
                gFYDimDst = reinterpret_cast<unsigned char *>((gFYRow - gFYDimLen) + gFYX + 1);
            }
            gFYDimIdx = 0;
            if (0 < static_cast<int>(gFYDimLen)) {
                do {
                    *gFYDimDst = gFYDimPal[*gFYDimDst];
                    gFYDimDst = gFYDimDst + 1;
                    gFYDimIdx = gFYDimIdx + 1;
                } while (gFYDimIdx < static_cast<int>(gFYDimLen));
            }
        }
        gFYX = gFYX - gFYDimLen2;
    } while (1);
}
