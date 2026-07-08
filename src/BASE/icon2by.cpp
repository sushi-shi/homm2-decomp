// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\icon2by.obj   from: .\basewin.lib
// functions: 1   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/icon2by.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <SOURCE/X_GLOBAL.h>
#include <_globals_model.h>
#include <BASE/Misc.h>
// Per-call decoder scratch — its own 0x538150+ block.
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
// Y-modify variant of IconToBitmap: each row is sheared horizontally by param_12[Y] (a signed per-row
// X offset; 0x7f = skip-row). Clip-only rendering (no fast path). Own 0x538150 scratch block. Full logic
// recovered; 0% for the shared Icon*2b* /O2 register-fusion codegen wall (Icon2b.cpp). Kept per breadth.
VA(0x004da270, 0x588)
void IconToBitmapYModify(class icon *param_1, class bitmap *param_2, int param_3, int param_4,
                         int param_5, int param_6, int param_7, int param_8, int param_9, int param_10,
                         int param_11, signed char *param_12)
{
    unsigned char bVar1;
    int iVar2, iVar3;
    unsigned int uVar4, uVar5, uVar6;
    unsigned char *pbVar7, *pbVar9;
    int *puVar8;
    IconEntry *entries = reinterpret_cast<IconEntry *>(param_1->field_0x12);
    gYMEntry = &entries[param_5];
    gYMSrc = reinterpret_cast<unsigned char *>(param_1->field_0x12) + gYMEntry->srcOffset;
    gYMX0 = gYMEntry->x + param_3;
    gYMPitch = param_2->width;
    gYMY = gYMEntry->y + param_4;
    gYMX = param_12[gYMY] + gYMX0;
    gYMClipB = param_8 + param_10 - 1;
    gYMClipR = param_7 + param_9 - 1;
    gYMRow = gYMPitch * gYMY + reinterpret_cast<int>(param_2->pixels);
    do {
        while (1) {
            while (1) {
                while (1) {
                    pbVar7 = gYMSrc;
                    bVar1 = *gYMSrc;
                    uVar6 = bVar1;
                    gYMSrc = gYMSrc + 1;
                    gYMRun = uVar6;
                    if (static_cast<signed char>(bVar1) < 0)
                        break;
                    if (uVar6 == 0) {
                        gYMX = param_12[gYMY] + gYMX0;
                        gYMRow = gYMRow + gYMPitch;
                        gYMY = gYMY + 1;
                    } else {
                        if (param_12[gYMY] != 0x7f && param_8 <= gYMY && gYMY <= gYMClipB &&
                            (iVar3 = gYMX + uVar6, param_7 < iVar3 && gYMX <= gYMClipR)) {
                            if (gYMX < param_7) {
                                if (gYMClipR < iVar3) {
                                    pbVar7 = gYMSrc + (param_7 - gYMX);
                                    pbVar9 = reinterpret_cast<unsigned char *>(gYMRow + param_7);
                                    for (uVar5 = static_cast<unsigned int>(param_9) >> 2; uVar6 = param_9, uVar5 != 0; uVar5--) {
                                        *reinterpret_cast<int *>(pbVar9) = *reinterpret_cast<int *>(pbVar7);
                                        pbVar7 += 4;
                                        pbVar9 += 4;
                                    }
                                } else {
                                    uVar6 = (uVar6 - param_7) + gYMX;
                                    pbVar7 = gYMSrc + (param_7 - gYMX);
                                    pbVar9 = reinterpret_cast<unsigned char *>(gYMRow + param_7);
                                    for (uVar5 = uVar6 >> 2; uVar5 != 0; uVar5--) {
                                        *reinterpret_cast<int *>(pbVar9) = *reinterpret_cast<int *>(pbVar7);
                                        pbVar7 += 4;
                                        pbVar9 += 4;
                                    }
                                }
                            } else {
                                pbVar7 = gYMSrc;
                                if (gYMClipR < iVar3) {
                                    uVar6 = (gYMClipR - gYMX) + 1;
                                    pbVar9 = reinterpret_cast<unsigned char *>(gYMRow + gYMX);
                                    for (uVar5 = uVar6 >> 2; uVar5 != 0; uVar5--) {
                                        *reinterpret_cast<int *>(pbVar9) = *reinterpret_cast<int *>(pbVar7);
                                        pbVar7 += 4;
                                        pbVar9 += 4;
                                    }
                                } else {
                                    pbVar9 = reinterpret_cast<unsigned char *>(gYMRow + gYMX);
                                    for (uVar5 = bVar1 >> 2; uVar5 != 0; uVar5--) {
                                        *reinterpret_cast<int *>(pbVar9) = *reinterpret_cast<int *>(pbVar7);
                                        pbVar7 += 4;
                                        pbVar9 += 4;
                                    }
                                }
                            }
                            for (uVar6 = uVar6 & 3; uVar6 != 0; uVar6--) {
                                *pbVar9 = *pbVar7;
                                pbVar7 = pbVar7 + 1;
                                pbVar9 = pbVar9 + 1;
                            }
                        }
                        gYMX = gYMX + gYMRun;
                        gYMSrc = gYMSrc + gYMRun;
                    }
                }
                if ((bVar1 & 0x40) != 0)
                    break;
                if ((bVar1 & 0x3f) == 0)
                    return;
                gYMX = gYMX + (uVar6 & 0x3f);
            }
            gYMRun = uVar6 & 0x3f;
            if ((bVar1 & 0x3f) == 0)
                break;
            if (uVar6 == 0xc1) {
                gYMRun = *gYMSrc;
                gYMSrc = pbVar7 + 2;
            }
            gYMColor = *gYMSrc;
            gYMSrc = gYMSrc + 1;
LAB_004da401:
            uVar6 = gYMRun;
            if (param_12[gYMY] != 0x7f && param_8 <= gYMY && gYMY <= gYMClipB &&
                (iVar3 = gYMX + gYMRun, param_7 < iVar3 && gYMX <= gYMClipR)) {
                if (gYMX < param_7) {
                    if (gYMClipR < iVar3) {
                        uVar4 = gYMColor * 0x01010101;
                        puVar8 = reinterpret_cast<int *>(gYMRow + param_7);
                        for (uVar5 = static_cast<unsigned int>(param_9) >> 2; uVar6 = param_9, uVar5 != 0; uVar5--)
                            *puVar8++ = uVar4;
                    } else {
                        uVar6 = (gYMRun - param_7) + gYMX;
                        uVar4 = gYMColor * 0x01010101;
                        puVar8 = reinterpret_cast<int *>(gYMRow + param_7);
                        for (uVar5 = uVar6 >> 2; uVar5 != 0; uVar5--)
                            *puVar8++ = uVar4;
                    }
                } else if (gYMClipR < iVar3) {
                    uVar6 = (gYMClipR - gYMX) + 1;
                    uVar4 = gYMColor * 0x01010101;
                    puVar8 = reinterpret_cast<int *>(gYMRow + gYMX);
                    for (uVar5 = uVar6 >> 2; uVar5 != 0; uVar5--)
                        *puVar8++ = uVar4;
                } else {
                    uVar4 = gYMColor * 0x01010101;
                    puVar8 = reinterpret_cast<int *>(gYMRow + gYMX);
                    for (uVar5 = gYMRun >> 2; uVar5 != 0; uVar5--)
                        *puVar8++ = uVar4;
                }
                for (uVar6 = uVar6 & 3; uVar6 != 0; uVar6--)
                    *reinterpret_cast<char *>(puVar8) = static_cast<char>(uVar4),
                    puVar8 = reinterpret_cast<int *>(reinterpret_cast<char *>(puVar8) + 1);
            }
            gYMX = gYMX + gYMRun;
        }
        bVar1 = *gYMSrc;
        gYMSrc = pbVar7 + 2;
        gYMDimLen = bVar1 & 3;
        if ((bVar1 & 3) == 0) {
            gYMDimLen = *gYMSrc;
            gYMSrc = pbVar7 + 3;
        }
        gYMDimLen2 = gYMDimLen;
        if (param_11 != 0 && (bVar1 & 0x80) != 0) {
            gYMColor = static_cast<unsigned char>(param_11);
            gYMRun = gYMDimLen;
            goto LAB_004da401;
        }
        if ((bVar1 & 0x40) != 0 &&
            (gYMDimPal = reinterpret_cast<unsigned char *>(uDimPal) + (bVar1 & 0x3c) * 0x40,
             param_12[gYMY] != 0x7f) &&
            param_8 <= gYMY &&
            gYMY <= gYMClipB && (iVar3 = gYMDimLen + gYMX, param_7 < iVar3) && gYMX <= gYMClipR) {
            if (gYMX < param_7) {
                iVar2 = param_7;
                if (gYMClipR < iVar3)
                    gYMDimLen = param_9;
                else
                    gYMDimLen = gYMDimLen + (gYMX - param_7);
            } else {
                iVar2 = gYMX;
                if (gYMClipR < iVar3)
                    gYMDimLen = (gYMClipR - gYMX) + 1;
            }
            gYMDimDst = reinterpret_cast<unsigned char *>(gYMRow + iVar2);
            gYMDimIdx = 0;
            if (0 < static_cast<int>(gYMDimLen)) {
                do {
                    *gYMDimDst = gYMDimPal[*gYMDimDst];
                    gYMDimDst = gYMDimDst + 1;
                    gYMDimIdx = gYMDimIdx + 1;
                } while (gYMDimIdx < static_cast<int>(gYMDimLen));
            }
        }
        gYMX = gYMX + gYMDimLen2;
    } while (1);
}
