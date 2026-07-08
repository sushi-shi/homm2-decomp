// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\Icond2b.obj   from: .\basewin.lib
// functions: 1   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/Icond2b.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <SOURCE/X_GLOBAL.h>
#include <_globals_model.h>
#include <BASE/Misc.h>
// Per-call decoder scratch — its own 0x534bf0+ block.
static int gDimRow;
static unsigned int gDimCnt;
static unsigned int gDimRun;
static int gDimY;
static unsigned int gDimCnt2;
static IconEntry *gDimEntry;
static int gDimClipB;
static unsigned char *gDimSrc;
static unsigned char *gDimDst;
static int gDimX;
static int gDimClipR;
static int gDimX0;

// @early-stop
// Dim variant of IconToBitmap: every pixel is remapped through the dim palette row uDimPal[param_6]
// (no solid-run/colour path — this decoder only copies dimmed literals). Full logic recovered; 0% for
// the shared Icon*2b* /O2 register-fusion codegen wall documented in Icon2b.cpp. Kept per breadth mandate.
VA(0x004cfd50, 0x26e)
void DimIconToBitmap(class icon *param_1, class bitmap *param_2, int param_3, int param_4, int param_5,
                     int param_6, int param_7, int param_8, int param_9, int param_10, int param_11)
{
    unsigned char bVar1;
    int iVar3;
    unsigned int uVar5;
    unsigned char *pbVar6;
    IconEntry *entries = reinterpret_cast<IconEntry *>(param_1->field_0x12);
    gDimEntry = &entries[param_5];
    gDimSrc = reinterpret_cast<unsigned char *>(param_1->field_0x12) + gDimEntry->srcOffset;
    gDimX0 = param_3 + gDimEntry->x;
    gDimY = param_4 + gDimEntry->y;
    if (param_7 != 0) {
        if (gDimX0 < param_8 || param_10 + param_8 < gDimEntry->w + gDimX0 || gDimY < param_9 ||
            param_9 + param_11 < gDimEntry->h + gDimY) {
            param_7 = 1;
            gDimClipR = param_8 - 1 + param_10;
            gDimClipB = param_9 - 1 + param_11;
        } else {
            param_7 = 0;
        }
    }
    short sVar2 = param_2->m_width;
    gDimRow = gDimY * sVar2 + reinterpret_cast<int>(param_2->m_pixels);
    gDimX = gDimX0;
    while (1) {
        while (1) {
            pbVar6 = gDimSrc;
            gDimSrc = gDimSrc + 1;
            bVar1 = *pbVar6;
            gDimRun = bVar1;
            if (static_cast<signed char>(bVar1) < 0)
                break;
            if (gDimRun == 0) {
                gDimY = gDimY + 1;
                gDimRow = gDimRow + sVar2;
                gDimX = gDimX0;
            } else {
                if (param_7 == 0) {
                    gDimDst = reinterpret_cast<unsigned char *>(gDimRow + gDimX);
                    gDimCnt = 0;
                    if (gDimRun != 0) {
                        uVar5 = gDimRun;
                        pbVar6 = gDimDst;
                        gDimCnt = gDimRun;
                        do {
                            uVar5 = uVar5 - 1;
                            gDimDst = pbVar6 + 1;
                            *pbVar6 = (reinterpret_cast<unsigned char *>(uDimPal) + param_6 * 0x100)[*pbVar6];
                            pbVar6 = pbVar6 + 1;
                        } while (uVar5 != 0);
                    }
                } else if (param_9 <= gDimY && gDimY <= gDimClipB &&
                           (iVar3 = gDimX + gDimRun, param_8 < iVar3 && gDimX <= gDimClipR)) {
                    if (gDimX < param_8) {
                        uVar5 = param_10;
                        if (iVar3 <= gDimClipR)
                            uVar5 = (gDimRun - param_8) + gDimX;
                        gDimDst = reinterpret_cast<unsigned char *>(param_8 + gDimRow);
                    } else if (gDimClipR < iVar3) {
                        gDimDst = reinterpret_cast<unsigned char *>(gDimRow + gDimX);
                        uVar5 = (gDimClipR - gDimX) + 1;
                    } else {
                        gDimDst = reinterpret_cast<unsigned char *>(gDimRow + gDimX);
                        uVar5 = gDimRun;
                    }
                    gDimCnt = 0;
                    gDimCnt2 = uVar5;
                    if (0 < static_cast<int>(uVar5)) {
                        pbVar6 = gDimDst;
                        gDimCnt = uVar5;
                        do {
                            uVar5 = uVar5 - 1;
                            gDimDst = pbVar6 + 1;
                            *pbVar6 = (reinterpret_cast<unsigned char *>(uDimPal) + param_6 * 0x100)[*pbVar6];
                            pbVar6 = pbVar6 + 1;
                        } while (uVar5 != 0);
                    }
                }
                gDimX = gDimX + gDimRun;
            }
        }
        if ((bVar1 & 0x7f) == 0)
            break;
        gDimX = gDimX + (bVar1 & 0x7f);
    }
}
