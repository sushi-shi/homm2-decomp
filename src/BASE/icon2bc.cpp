// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\icon2bc.obj   from: .\basewin.lib
// functions: 1   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/icon2bc.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <SOURCE/X_GLOBAL.h>
#include <_globals_model.h>
#include <BASE/Misc.h>
// Per-call decoder scratch — its own 0x534ca8+ block.
static int gCTPitch;
static unsigned int gCTCnt;
static int gCTX;
static int gCTY;
static IconEntry *gCTEntry;
static int gCTRow;
static unsigned char *gCTDimPal;
static int gCTClipB;
static int gCTClipR;
static unsigned char *gCTSrcCopy;
static unsigned int gCTCnt2;
static unsigned char *gCTDst;
static unsigned char *gCTSrc;
static unsigned int gCTDimLen;
static int gCTX0;
static unsigned char gCTColor;
static unsigned int gCTRun;

// @early-stop
// Colour-table variant of IconToBitmap: literal pixels are remapped through param_12[] and solid-fill
// colours through param_12[*src]; the dim step is gated on param_13. Own 0x534ca8 scratch block. Full
// logic recovered; 0% for the shared Icon*2b* /O2 register-fusion codegen wall (Icon2b.cpp). Kept per breadth.
VA(0x004d32a0, 0x5af)
void IconToBitmapColorTable(class icon *param_1, class bitmap *param_2, int param_3, int param_4,
                            int param_5, int param_6, int param_7, int param_8, int param_9, int param_10,
                            int param_11, unsigned char *param_12, int param_13)
{
    unsigned char bVar1;
    int iVar2, iVar3;
    unsigned int uVar4, uVar5, uVar6, uVar7;
    unsigned char *pbVar8;
    int *puVar9;
    IconEntry *entries = reinterpret_cast<IconEntry *>(param_1->m_data);
    gCTEntry = &entries[param_5];
    gCTSrc = reinterpret_cast<unsigned char *>(param_1->m_data) + gCTEntry->srcOffset;
    gCTX0 = param_3 + gCTEntry->x;
    gCTPitch = param_2->m_width;
    gCTY = gCTEntry->y + param_4;
    if (param_6 != 0) {
        if (gCTX0 < param_7 || param_9 + param_7 < gCTEntry->w + gCTX0 || gCTY < param_8 ||
            param_8 + param_10 < gCTEntry->h + gCTY) {
            param_6 = 1;
            gCTClipR = param_7 - 1 + param_9;
            gCTClipB = param_8 - 1 + param_10;
        } else {
            param_6 = 0;
        }
    }
    gCTRow = gCTPitch * gCTY + reinterpret_cast<int>(param_2->m_pixels);
    gCTX = gCTX0;
    do {
        while (1) {
            while (1) {
                while (1) {
                    pbVar8 = gCTSrc;
                    gCTSrc = gCTSrc + 1;
                    bVar1 = *pbVar8;
                    gCTRun = bVar1;
                    if (static_cast<signed char>(bVar1) < 0)
                        break;
                    if (gCTRun == 0) {
                        gCTY = gCTY + 1;
                        gCTRow = gCTRow + gCTPitch;
                        gCTX = gCTX0;
                    } else {
                        uVar7 = gCTRun;
                        gCTSrcCopy = gCTSrc;
                        if (param_6 == 0) {
                            gCTDst = reinterpret_cast<unsigned char *>(gCTX + gCTRow);
                        } else if (gCTY < param_8 || gCTClipB < gCTY ||
                                   (iVar2 = gCTX + gCTRun, iVar2 <= param_7 || gCTClipR < gCTX)) {
                            uVar7 = 0;
                        } else if (gCTX < param_7) {
                            uVar7 = param_9;
                            if (iVar2 <= gCTClipR)
                                uVar7 = (gCTRun - param_7) + gCTX;
                            gCTDst = reinterpret_cast<unsigned char *>(param_7 + gCTRow);
                            gCTSrcCopy = gCTSrc + (param_7 - gCTX);
                        } else {
                            gCTDst = reinterpret_cast<unsigned char *>(gCTX + gCTRow);
                            if (gCTClipR < iVar2)
                                uVar7 = (gCTClipR - gCTX) + 1;
                        }
                        gCTCnt = uVar7;
                        if (uVar7 != 0 && (gCTCnt2 = 0, pbVar8 = gCTDst, uVar6 = uVar7,
                                           0 < static_cast<int>(uVar7))) {
                            do {
                                gCTCnt2 = uVar6;
                                bVar1 = *gCTSrcCopy;
                                gCTDst = pbVar8 + 1;
                                gCTSrcCopy = gCTSrcCopy + 1;
                                *pbVar8 = param_12[bVar1];
                                uVar7 = uVar7 - 1;
                                pbVar8 = gCTDst;
                                uVar6 = gCTCnt2;
                            } while (uVar7 != 0);
                        }
                        gCTSrc = gCTSrc + gCTRun;
                        gCTX = gCTX + gCTRun;
                    }
                }
                if ((bVar1 & 0x40) != 0)
                    break;
                if ((bVar1 & 0x3f) == 0)
                    return;
                gCTX = gCTX + (bVar1 & 0x3f);
            }
            uVar7 = gCTRun & 0x3f;
            if ((bVar1 & 0x3f) != 0)
                break;
            bVar1 = *gCTSrc;
            uVar7 = bVar1 & 3;
            gCTSrc = pbVar8 + 2;
            if ((bVar1 & 3) == 0) {
                gCTSrc = pbVar8 + 3;
                uVar7 = pbVar8[2];
            }
            gCTCnt = uVar7;
            gCTDimLen = uVar7;
            if (param_11 != 0 && (bVar1 & 0x80) != 0) {
                gCTColor = static_cast<unsigned char>(param_11);
                goto LAB_004d346a;
            }
            uVar6 = gCTCnt2;
            if ((bVar1 & 0x40) != 0) {
                gCTDimPal = reinterpret_cast<unsigned char *>(uDimPal) + (bVar1 & 0x3c) * 0x40;
                if (param_6 == 0) {
                    gCTDst = reinterpret_cast<unsigned char *>(gCTX + gCTRow);
                    gCTCnt2 = 0;
                    uVar6 = gCTCnt2;
                    gCTCnt2 = uVar7;
                    for (; uVar7 != 0; uVar7--) {
                        if (param_13 != 0)
                            *gCTDst = gCTDimPal[*gCTDst];
                        gCTDst = gCTDst + 1;
                        uVar6 = gCTCnt2;
                    }
                } else if (param_8 <= gCTY && gCTY <= gCTClipB &&
                           (iVar2 = uVar7 + gCTX, param_7 < iVar2 && gCTX <= gCTClipR)) {
                    if (gCTX < param_7) {
                        uVar5 = param_9;
                        iVar3 = param_7;
                        if (iVar2 <= gCTClipR)
                            uVar5 = (uVar7 - param_7) + gCTX;
                    } else {
                        uVar5 = uVar7;
                        iVar3 = gCTX;
                        if (gCTClipR < iVar2)
                            uVar5 = (gCTClipR - gCTX) + 1;
                    }
                    gCTDst = reinterpret_cast<unsigned char *>(iVar3 + gCTRow);
                    gCTCnt2 = 0;
                    gCTCnt = uVar5;
                    uVar6 = gCTCnt2;
                    gCTCnt2 = uVar5;
                    if (0 < static_cast<int>(uVar5)) {
                        do {
                            if (param_13 != 0)
                                *gCTDst = gCTDimPal[*gCTDst];
                            gCTDst = gCTDst + 1;
                            uVar5 = uVar5 - 1;
                            uVar6 = gCTCnt2;
                        } while (uVar5 != 0);
                    }
                }
            }
            gCTCnt2 = uVar6;
            gCTX = gCTX + gCTDimLen;
        }
        if (gCTRun == 0xc1) {
            uVar7 = *gCTSrc;
            gCTSrc = pbVar8 + 2;
        }
        gCTColor = param_12[*gCTSrc];
        gCTSrc = gCTSrc + 1;
LAB_004d346a:
        uVar6 = uVar7;
        if (param_6 == 0) {
            uVar4 = gCTColor * 0x01010101;
            puVar9 = reinterpret_cast<int *>(gCTX + gCTRow);
            for (uVar5 = uVar7 >> 2; uVar5 != 0; uVar5--)
                *puVar9++ = uVar4;
LAB_004d35bf:
            for (uVar6 = uVar6 & 3; uVar6 != 0; uVar6--)
                *reinterpret_cast<char *>(puVar9) = static_cast<char>(uVar4),
                puVar9 = reinterpret_cast<int *>(reinterpret_cast<char *>(puVar9) + 1);
        } else if (param_8 <= gCTY && gCTY <= gCTClipB &&
                   (iVar2 = uVar7 + gCTX, param_7 < iVar2) && gCTX <= gCTClipR) {
            if (gCTX < param_7) {
                if (gCTClipR < iVar2) {
                    uVar4 = gCTColor * 0x01010101;
                    puVar9 = reinterpret_cast<int *>(param_7 + gCTRow);
                    for (uVar5 = static_cast<unsigned int>(param_9) >> 2; uVar6 = param_9, uVar5 != 0; uVar5--)
                        *puVar9++ = uVar4;
                } else {
                    uVar6 = (gCTX - param_7) + uVar7;
                    uVar4 = gCTColor * 0x01010101;
                    puVar9 = reinterpret_cast<int *>(param_7 + gCTRow);
                    for (uVar5 = uVar6 >> 2; uVar5 != 0; uVar5--)
                        *puVar9++ = uVar4;
                }
            } else if (gCTClipR < iVar2) {
                uVar6 = (gCTClipR - gCTX) + 1;
                uVar4 = gCTColor * 0x01010101;
                puVar9 = reinterpret_cast<int *>(gCTX + gCTRow);
                for (uVar5 = uVar6 >> 2; uVar5 != 0; uVar5--)
                    *puVar9++ = uVar4;
            } else {
                uVar4 = gCTColor * 0x01010101;
                puVar9 = reinterpret_cast<int *>(gCTX + gCTRow);
                for (uVar5 = uVar7 >> 2; uVar5 != 0; uVar5--)
                    *puVar9++ = uVar4;
            }
            goto LAB_004d35bf;
        }
        gCTX = gCTX + uVar7;
    } while (1);
}
