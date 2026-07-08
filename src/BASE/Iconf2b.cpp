// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\Iconf2b.obj   from: .\basewin.lib
// functions: 1   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/Iconf2b.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <SOURCE/X_GLOBAL.h>
#include <_globals_model.h>
#include <BASE/Misc.h>
// Per-call decoder scratch — its own 0x534c60+ block (distinct from IconToBitmap's 0x534c20 block).
static int gFlipSkip;
static unsigned int gFlipRun;
static int gFlipX0;
static unsigned int gFlipCnt;
static unsigned int gFlipCnt2;
static int gFlipY;
static IconEntry *gFlipEntry;
static int gFlipX;
static unsigned char *gFlipSrc;
static unsigned char *gFlipDimPal;
static unsigned char *gFlipDimDst;
static int gFlipClipB;
static int gFlipRow;
static unsigned int gFlipDimLen;
static unsigned char gFlipColor;
static int gFlipClipR;
static unsigned char *gFlipDst;
static int gFlipXEnd;

// @early-stop
// Horizontal-flip variant of IconToBitmap (dst pointer decrements; sprite drawn right-to-left).
// Full RLE decoder logic recovered; 0% for the same shared /O2 register-fusion codegen wall documented
// in Icon2b.cpp. Kept per "don't throw out big-function work" — unlocks with the family once cracked.
VA(0x004d1ba0, 0x4f1)
void FlipIconToBitmap(class icon *param_1, class bitmap *param_2, int param_3, int param_4, int param_5,
                      int param_6, int param_7, int param_8, int param_9, int param_10, int param_11)
{
    unsigned char bVar1;
    unsigned int uVar3;
    int iVar6;
    unsigned int uVar5, uVar7, uVar9;
    unsigned char *pbVar8;
    int *puVar10;
    IconEntry *entries = reinterpret_cast<IconEntry *>(param_1->field_0x12);
    int iVar4 = entries[param_5].w;
    gFlipEntry = &entries[param_5];
    gFlipX0 = ((param_3 - gFlipEntry->x) - iVar4) + 1;
    gFlipXEnd = iVar4 - 1 + gFlipX0;
    gFlipY = param_4 + gFlipEntry->y;
    if (param_6 != 0) {
        if (gFlipX0 < param_7 || param_9 + param_7 < gFlipX0 + iVar4 || gFlipY < param_8 ||
            param_8 + param_10 < gFlipEntry->h + gFlipY) {
            param_6 = 1;
            gFlipClipR = param_7 - 1 + param_9;
            gFlipClipB = param_8 - 1 + param_10;
        } else {
            param_6 = 0;
        }
    }
    short sVar2 = param_2->width;
    gFlipRow = gFlipY * sVar2 + reinterpret_cast<int>(param_2->pixels);
    gFlipX = gFlipXEnd;
    pbVar8 = reinterpret_cast<unsigned char *>(param_1->field_0x12) + gFlipEntry->srcOffset;
LAB_004d1c83:
    do {
        while (1) {
            bVar1 = *pbVar8;
            gFlipRun = bVar1;
            gFlipSrc = pbVar8 + 1;
            if (static_cast<signed char>(bVar1) < 0)
                break;
            if (gFlipRun == 0) {
                gFlipY = gFlipY + 1;
                gFlipRow = gFlipRow + sVar2;
                gFlipX = gFlipXEnd;
                pbVar8 = gFlipSrc;
            } else {
                if (param_6 == 0) {
                    gFlipCnt = 0;
                    gFlipDst = reinterpret_cast<unsigned char *>(gFlipRow + gFlipX);
                    uVar9 = gFlipCnt;
                    gFlipCnt = gFlipRun;
                    for (uVar7 = gFlipRun; uVar7 != 0; uVar7--) {
                        bVar1 = *gFlipSrc;
                        gFlipSrc = gFlipSrc + 1;
                        *gFlipDst = bVar1;
                        gFlipDst = gFlipDst + -1;
                        uVar9 = gFlipCnt;
                    }
                } else if (param_8 <= gFlipY && gFlipY <= gFlipClipB) {
                    iVar6 = (gFlipX - gFlipRun) + 1;
                    if (iVar6 <= gFlipClipR && param_7 <= gFlipX) {
                        if (gFlipClipR < gFlipX) {
                            gFlipDst = reinterpret_cast<unsigned char *>(gFlipRow + gFlipClipR);
                            gFlipSrc = gFlipSrc + (gFlipX - gFlipClipR);
                            if (static_cast<int>(gFlipX - gFlipRun) < param_7) {
                                gFlipSkip = gFlipClipR + ((gFlipRun - gFlipX) - param_9);
                                uVar9 = param_9;
                            } else {
                                gFlipSkip = 0;
                                uVar9 = (gFlipRun - gFlipX) + gFlipClipR;
                            }
                        } else {
                            gFlipDst = reinterpret_cast<unsigned char *>(gFlipRow + gFlipX);
                            if (iVar6 < param_7) {
                                uVar9 = (gFlipX - param_7) + 1;
                                gFlipSkip = gFlipRun - uVar9;
                            } else {
                                gFlipSkip = 0;
                                uVar9 = gFlipRun;
                            }
                        }
                        gFlipCnt = 0;
                        uVar7 = uVar9;
                        gFlipCnt2 = uVar9;
                        if (0 < static_cast<int>(uVar9)) {
                            do {
                                gFlipCnt = uVar7;
                                bVar1 = *gFlipSrc;
                                gFlipSrc = gFlipSrc + 1;
                                uVar9 = uVar9 - 1;
                                pbVar8 = gFlipDst + -1;
                                *gFlipDst = bVar1;
                                gFlipDst = pbVar8;
                                uVar7 = gFlipCnt;
                            } while (uVar9 != 0);
                        }
                        gFlipSrc = gFlipSrc + gFlipSkip;
                        uVar9 = gFlipCnt;
                        goto LAB_004d2053;
                    }
                    gFlipSrc = gFlipSrc + gFlipRun;
                    uVar9 = gFlipCnt;
                    goto LAB_004d2053;
                } else {
                    gFlipSrc = gFlipSrc + gFlipRun;
                    uVar9 = gFlipCnt;
                }
LAB_004d2053:
                gFlipCnt = uVar9;
                gFlipX = gFlipX - gFlipRun;
                pbVar8 = gFlipSrc;
            }
        }
        if ((bVar1 & 0x40) == 0) {
            if ((bVar1 & 0x3f) == 0)
                return;
            gFlipX = gFlipX - (bVar1 & 0x3f);
            pbVar8 = gFlipSrc;
            goto LAB_004d1c83;
        }
        uVar9 = gFlipRun & 0x3f;
        if ((bVar1 & 0x3f) == 0) {
            bVar1 = *gFlipSrc;
            gFlipSrc = pbVar8 + 2;
            uVar9 = bVar1 & 3;
            if ((bVar1 & 3) == 0) {
                uVar9 = *gFlipSrc;
                gFlipSrc = pbVar8 + 3;
            }
            gFlipCnt2 = uVar9;
            gFlipDimLen = uVar9;
            if (param_11 != 0 && (bVar1 & 0x80) != 0) {
                gFlipColor = static_cast<unsigned char>(param_11);
                goto LAB_004d1d0b;
            }
            uVar7 = gFlipCnt;
            if ((bVar1 & 0x40) != 0) {
                gFlipDimPal = reinterpret_cast<unsigned char *>(uDimPal) + (bVar1 & 0x3c) * 0x40;
                if (param_6 == 0) {
                    gFlipCnt = 0;
                    gFlipDimDst = reinterpret_cast<unsigned char *>((gFlipRow - uVar9) + 1 + gFlipX);
                    uVar7 = gFlipCnt;
                    pbVar8 = gFlipDimDst;
                    gFlipCnt = uVar9;
                    for (; uVar9 != 0; uVar9--) {
                        gFlipDimDst = pbVar8 + 1;
                        *pbVar8 = gFlipDimPal[*pbVar8];
                        uVar7 = gFlipCnt;
                        pbVar8 = pbVar8 + 1;
                    }
                } else if (param_8 <= gFlipY && gFlipY <= gFlipClipB &&
                           (iVar6 = (gFlipX - uVar9) + 1, param_7 <= iVar6 && gFlipX <= gFlipClipR)) {
                    if (iVar6 < param_7) {
                        pbVar8 = reinterpret_cast<unsigned char *>(gFlipRow + param_7);
                        uVar9 = (gFlipX - param_7) + 1;
                    } else {
                        pbVar8 = reinterpret_cast<unsigned char *>((gFlipRow - uVar9) + 1 + gFlipX);
                    }
                    gFlipCnt = 0;
                    uVar7 = gFlipCnt;
                    gFlipCnt = uVar9;
                    gFlipCnt2 = uVar9;
                    gFlipDimDst = pbVar8;
                    if (0 < static_cast<int>(uVar9)) {
                        do {
                            uVar9 = uVar9 - 1;
                            gFlipDimDst = pbVar8 + 1;
                            *pbVar8 = gFlipDimPal[*pbVar8];
                            pbVar8 = pbVar8 + 1;
                            uVar7 = gFlipCnt;
                        } while (uVar9 != 0);
                    }
                }
            }
            gFlipCnt = uVar7;
            gFlipX = gFlipX - gFlipDimLen;
            pbVar8 = gFlipSrc;
        } else {
            if (gFlipRun == 0xc1) {
                uVar9 = *gFlipSrc;
                gFlipSrc = pbVar8 + 2;
            }
            gFlipColor = *gFlipSrc;
            gFlipSrc = gFlipSrc + 1;
LAB_004d1d0b:
            uVar7 = uVar9;
            if (param_6 == 0) {
                uVar3 = gFlipColor * 0x01010101;
                puVar10 = reinterpret_cast<int *>((gFlipRow - uVar9) + 1 + gFlipX);
                for (uVar5 = uVar9 >> 2; uVar5 != 0; uVar5--)
                    *puVar10++ = uVar3;
LAB_004d1de4:
                for (uVar7 = uVar7 & 3; uVar7 != 0; uVar7--)
                    *reinterpret_cast<char *>(puVar10) = static_cast<char>(uVar3),
                    puVar10 = reinterpret_cast<int *>(reinterpret_cast<char *>(puVar10) + 1);
            } else if (param_8 <= gFlipY && gFlipY <= gFlipClipB &&
                       (iVar6 = (gFlipX - uVar9) + 1, param_7 <= iVar6) && gFlipX <= gFlipClipR) {
                if (iVar6 < param_7) {
                    uVar7 = (gFlipX - param_7) + 1;
                    uVar3 = gFlipColor * 0x01010101;
                    puVar10 = reinterpret_cast<int *>(gFlipRow + param_7);
                    for (uVar5 = uVar7 >> 2; uVar5 != 0; uVar5--)
                        *puVar10++ = uVar3;
                } else {
                    uVar3 = gFlipColor * 0x01010101;
                    puVar10 = reinterpret_cast<int *>((gFlipRow - uVar9) + 1 + gFlipX);
                    for (uVar5 = uVar9 >> 2; uVar5 != 0; uVar5--)
                        *puVar10++ = uVar3;
                }
                goto LAB_004d1de4;
            }
            gFlipX = gFlipX - uVar9;
            pbVar8 = gFlipSrc;
        }
    } while (1);
}
