// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\Icon2b.obj   from: .\basewin.lib
// functions: 1   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/Icon2b.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <SOURCE/X_GLOBAL.h>
#include <_globals_model.h>
int gIcRow;
int gIcPitch;
unsigned char gIcColor;
unsigned char * gIcDimPal;
unsigned int gIcRun;
unsigned int gIcCnt;
unsigned char * gIcSrc;
unsigned char * gIcDimDst;
int gIcClipR;
int gIcClipB;
int gIcX0;
unsigned int gIcDimLen;
int gIcY;
int gIcX;
unsigned char * gIcEntry;
unsigned int gIcCnt2;

// @early-stop
// Full RLE sprite decoder recovered + compiles, but /O2 codegen is ~2x retail (503 vs 232 instr)
// -> 0%: the 16 file-static scratch globals + row-ptr arithmetic need structural tuning to match
// the retail register/global usage. Logic is faithful; kept for codegen-matching. Unlocks the 11-TU
// Icon*2b* family once tuned (all share this decoder).
VA(0x004d0570, 0x4ed)
void IconToBitmap(class icon *param_1, class bitmap *param_2, int param_3, int param_4, int param_5,
                  int param_6, int param_7, int param_8, int param_9, int param_10, int param_11)
{
    unsigned char bVar2;
    int iVar3, iVar4;
    unsigned int uVar6, uVar7, uVar8;
    unsigned char *pbVar9, *pbVar11;
    unsigned char *ppuVar1;
    unsigned int uVar5;
    int *puVar10;
    unsigned char *iEntry = reinterpret_cast<unsigned char *>(param_1->field_0x12) + param_5 * 0xd;
    gIcEntry = iEntry;
    gIcSrc = reinterpret_cast<unsigned char *>(param_1->field_0x12) + *reinterpret_cast<int *>(iEntry + 9);
    gIcX0 = param_3 + *reinterpret_cast<short *>(iEntry);
    gIcY = *reinterpret_cast<short *>(gIcEntry + 2) + param_4;
    gIcPitch = param_2->field_0x12;
    if (param_6 != 0) {
        if (gIcX0 < param_7 || param_9 + param_7 < *reinterpret_cast<short *>(gIcEntry + 4) + gIcX0 ||
            gIcY < param_8 || param_8 + param_10 < *reinterpret_cast<short *>(gIcEntry + 6) + gIcY) {
            param_6 = 1;
            gIcClipR = param_7 - 1 + param_9;
            gIcClipB = param_8 - 1 + param_10;
        } else {
            param_6 = 0;
        }
    }
    gIcRow = gIcPitch * gIcY + reinterpret_cast<int>(param_2->field_0x16);
    gIcX = gIcX0;
LAB_0651:
    do {
        while (1) {
            pbVar9 = gIcSrc;
            gIcSrc = gIcSrc + 1;
            bVar2 = *pbVar9;
            gIcRun = bVar2;
            if (static_cast<signed char>(bVar2) < 0)
                break;
            if (gIcRun == 0) {
                gIcY = gIcY + 1;
                gIcRow = gIcRow + gIcPitch;
                gIcX = gIcX0;
            } else {
                uVar8 = gIcRun;
                pbVar9 = gIcSrc;
                if (param_6 == 0) {
                    pbVar11 = reinterpret_cast<unsigned char *>(gIcX + gIcRow);
                    goto LAB_0a10;
                } else if (param_8 <= gIcY && gIcY <= gIcClipB &&
                           (iVar3 = gIcX + gIcRun, param_7 < iVar3 && gIcX <= gIcClipR)) {
                    if (gIcX < param_7) {
                        uVar8 = param_9;
                        if (iVar3 <= gIcClipR)
                            uVar8 = (gIcRun - param_7) + gIcX;
                        pbVar11 = reinterpret_cast<unsigned char *>(param_7 + gIcRow);
                        pbVar9 = gIcSrc + (param_7 - gIcX);
                    } else if (gIcClipR < iVar3) {
                        pbVar11 = reinterpret_cast<unsigned char *>(gIcX + gIcRow);
                        uVar8 = (gIcClipR - gIcX) + 1;
                    } else {
                        pbVar11 = reinterpret_cast<unsigned char *>(gIcX + gIcRow);
                    }
LAB_0a10:
                    for (uVar7 = uVar8 >> 2; uVar7 != 0; uVar7--) {
                        *reinterpret_cast<int *>(pbVar11) = *reinterpret_cast<int *>(pbVar9);
                        pbVar9 += 4;
                        pbVar11 += 4;
                    }
                    for (uVar8 = uVar8 & 3; uVar8 != 0; uVar8--) {
                        *pbVar11 = *pbVar9;
                        pbVar9++;
                        pbVar11++;
                    }
                }
                gIcSrc = gIcSrc + gIcRun;
                gIcX = gIcX + gIcRun;
            }
        }
        if ((bVar2 & 0x40) == 0) {
            if ((bVar2 & 0x3f) == 0)
                return;
            gIcX = gIcX + (bVar2 & 0x3f);
            goto LAB_0651;
        }
        uVar8 = gIcRun & 0x3f;
        if ((bVar2 & 0x3f) == 0) {
            bVar2 = *gIcSrc;
            uVar8 = bVar2 & 3;
            gIcSrc = pbVar9 + 2;
            if ((bVar2 & 3) == 0) {
                gIcSrc = pbVar9 + 3;
                uVar8 = pbVar9[2];
            }
            gIcDimLen = uVar8;
            gIcCnt2 = uVar8;
            if (param_11 != 0 && (bVar2 & 0x80) != 0) {
                gIcColor = static_cast<unsigned char>(param_11);
                goto LAB_0721;
            }
            uVar7 = gIcCnt;
            if ((bVar2 & 0x40) != 0) {
                ppuVar1 = reinterpret_cast<unsigned char *>(uDimPal) + (bVar2 & 0x3c) * 0x40;
                gIcDimPal = ppuVar1;
                if (param_6 == 0) {
                    gIcDimDst = reinterpret_cast<unsigned char *>(gIcX + gIcRow);
                    gIcCnt = uVar8;
                    pbVar9 = gIcDimDst;
                    for (; uVar8 != 0; uVar8--) {
                        gIcDimPal = ppuVar1;
                        gIcDimDst = pbVar9 + 1;
                        *pbVar9 = ppuVar1[*pbVar9];
                        pbVar9++;
                        uVar7 = gIcCnt;
                    }
                } else if (param_8 <= gIcY && gIcY <= gIcClipB &&
                           (iVar3 = gIcX + uVar8, param_7 < iVar3 && gIcX <= gIcClipR)) {
                    if (gIcX < param_7) {
                        uVar6 = param_9;
                        iVar4 = param_7;
                        if (iVar3 <= gIcClipR)
                            uVar6 = (uVar8 - param_7) + gIcX;
                    } else {
                        uVar6 = uVar8;
                        iVar4 = gIcX;
                        if (gIcClipR < iVar3)
                            uVar6 = (gIcClipR - gIcX) + 1;
                    }
                    pbVar9 = reinterpret_cast<unsigned char *>(iVar4 + gIcRow);
                    gIcCnt = uVar6;
                    gIcDimDst = pbVar9;
                    gIcCnt2 = uVar6;
                    if (0 < static_cast<int>(uVar6)) {
                        do {
                            uVar6--;
                            gIcDimPal = ppuVar1;
                            gIcDimDst = pbVar9 + 1;
                            *pbVar9 = ppuVar1[*pbVar9];
                            pbVar9++;
                            uVar7 = gIcCnt;
                        } while (uVar6 != 0);
                    }
                }
            }
            gIcCnt = uVar7;
            gIcX = gIcX + gIcDimLen;
        } else {
            if (gIcRun == 0xc1) {
                uVar8 = *gIcSrc;
                gIcSrc = pbVar9 + 2;
            }
            gIcColor = *gIcSrc;
            gIcSrc = gIcSrc + 1;
LAB_0721:
            uVar7 = uVar8;
            if (param_6 == 0) {
                uVar5 = gIcColor * 0x01010101;
                puVar10 = reinterpret_cast<int *>(gIcX + gIcRow);
                for (uVar6 = uVar8 >> 2; uVar6 != 0; uVar6--)
                    *puVar10++ = uVar5;
LAB_080c:
                for (uVar7 = uVar7 & 3; uVar7 != 0; uVar7--)
                    *reinterpret_cast<char *>(puVar10) = static_cast<char>(uVar5),
                    puVar10 = reinterpret_cast<int *>(reinterpret_cast<char *>(puVar10) + 1);
            } else if (param_8 <= gIcY && gIcY <= gIcClipB &&
                       (iVar3 = gIcX + uVar8, param_7 < iVar3) && gIcX <= gIcClipR) {
                if (gIcX < param_7) {
                    uVar7 = param_9;
                    if (iVar3 <= gIcClipR)
                        uVar7 = (uVar8 - param_7) + gIcX;
                    puVar10 = reinterpret_cast<int *>(param_7 + gIcRow);
                    uVar5 = gIcColor * 0x01010101;
                } else if (gIcClipR < iVar3) {
                    uVar7 = (gIcClipR - gIcX) + 1;
                    puVar10 = reinterpret_cast<int *>(gIcX + gIcRow);
                    uVar5 = gIcColor * 0x01010101;
                } else {
                    puVar10 = reinterpret_cast<int *>(gIcX + gIcRow);
                    uVar5 = gIcColor * 0x01010101;
                }
                for (uVar6 = uVar7 >> 2; uVar6 != 0; uVar6--)
                    *puVar10++ = uVar5;
                goto LAB_080c;
            }
            gIcX = gIcX + uVar8;
        }
    } while (1);
}

