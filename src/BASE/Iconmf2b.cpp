// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\Iconmf2b.obj   from: .\basewin.lib
// functions: 1   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/Iconmf2b.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <_globals_model.h>
#include <BASE/Misc.h>
// Per-call decoder scratch — its own 0x538190+ block.
static unsigned int gFMRun;
static unsigned char *gFMSrc;
static IconEntry *gFMEntry;
static int gFMX;
static int gFMX0;
static int gFMClipB;
static int gFMRow;
static int gFMY;
static int gFMClipR;
static int gFMXEnd;

// @early-stop
// Flip+Mono variant of IconToBitmap: fills each RLE run with a single colour (param_6) in the flipped
// run position ((row-run)+1+X, X decrements). Full logic recovered; 0% for the shared Icon*2b* /O2
// register-fusion codegen wall documented in Icon2b.cpp. Kept per breadth mandate.
VA(0x004da800, 0x212)
void FlipMonoIconToBitmap(class icon *param_1, class bitmap *param_2, int param_3, int param_4,
                          int param_5, int param_6, int param_7, int param_8, int param_9, int param_10,
                          int param_11)
{
    unsigned char bVar1;
    unsigned char *pbVar3;
    unsigned int uVar4;
    int iVar5;
    unsigned int uVar6, uVar7;
    unsigned char uVar8;
    int *puVar9;
    IconEntry *entries = reinterpret_cast<IconEntry *>(param_1->field_0x12);
    gFMEntry = &entries[param_5];
    gFMSrc = reinterpret_cast<unsigned char *>(param_1->field_0x12) + gFMEntry->srcOffset;
    gFMX0 = (param_3 - gFMEntry->x) - gFMEntry->w;
    iVar5 = gFMEntry->w + 1 + gFMX0;
    gFMX0 = gFMX0 + 1;
    gFMXEnd = iVar5 - 1;
    gFMY = param_4 + gFMEntry->y;
    if (param_7 != 0) {
        if (gFMX0 < param_8 || param_10 + param_8 < iVar5 || gFMY < param_9 ||
            param_9 + param_11 < gFMEntry->h + gFMY) {
            param_7 = 1;
            gFMClipR = param_8 - 1 + param_10;
            gFMClipB = param_9 - 1 + param_11;
        } else {
            param_7 = 0;
        }
    }
    short sVar2 = param_2->m_width;
    gFMRow = gFMY * sVar2 + reinterpret_cast<int>(param_2->m_pixels);
    gFMX = gFMXEnd;
    do {
        while (1) {
            while (1) {
                pbVar3 = gFMSrc;
                gFMSrc = gFMSrc + 1;
                bVar1 = *pbVar3;
                gFMRun = bVar1;
                if (-1 < static_cast<signed char>(bVar1))
                    break;
                if ((bVar1 & 0x7f) == 0)
                    return;
                gFMX = gFMX - (bVar1 & 0x7f);
            }
            if (gFMRun != 0)
                break;
            gFMY = gFMY + 1;
            gFMRow = gFMRow + sVar2;
            gFMX = gFMXEnd;
        }
        uVar8 = static_cast<unsigned char>(param_6);
        uVar7 = gFMRun;
        if (param_7 == 0) {
            uVar4 = uVar8 * 0x01010101;
            puVar9 = reinterpret_cast<int *>((gFMRow - gFMRun) + 1 + gFMX);
LAB_004da9c5:
            for (uVar6 = uVar7 >> 2; uVar6 != 0; uVar6--)
                *puVar9++ = uVar4;
            for (uVar7 = uVar7 & 3; uVar7 != 0; uVar7--)
                *reinterpret_cast<char *>(puVar9) = static_cast<char>(uVar4),
                puVar9 = reinterpret_cast<int *>(reinterpret_cast<char *>(puVar9) + 1);
        } else if (param_9 <= gFMY && gFMY <= gFMClipB &&
                   (iVar5 = (gFMX - gFMRun) + 1, param_8 <= iVar5 && gFMX <= gFMClipR)) {
            if (iVar5 < param_8) {
                uVar7 = (gFMX - param_8) + 1;
                uVar4 = uVar8 * 0x01010101;
                puVar9 = reinterpret_cast<int *>(param_8 + gFMRow);
            } else {
                uVar4 = uVar8 * 0x01010101;
                puVar9 = reinterpret_cast<int *>((gFMRow - gFMRun) + 1 + gFMX);
            }
            goto LAB_004da9c5;
        }
        gFMX = gFMX - gFMRun;
    } while (1);
}
