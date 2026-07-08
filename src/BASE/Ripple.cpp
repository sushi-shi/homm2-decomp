// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\Ripple.obj   from: .\basewin.lib
// functions: 1   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/Ripple.h>
#include <BASE/bitmap.h>
#include <BASE/mouseManager.h>
#include <BASE/heroWindowManager.h>
#include <BASE/Misc.h>
#include <SOURCE/KB.h>
#include <SOURCE/kbwin.h>
#include <SOURCE/NOOPT.h>
VA(0x004d26a0, 0x23f)
void DoRipple(class bitmap *param_1, class bitmap *param_2, int param_3, int param_4)
{
    unsigned char local_2c0[0x2c0];
    int local_28c;
    class bitmap *local_288 = param_2;
    class bitmap *local_284 = param_1;
    int local_2c4;
    int iVar2, iVar3;
    gpMouseManager->HideColorPointer();
    local_2c0[0] = 0;
    local_2c0[1] = 0;
    local_2c0[2] = 0;
    local_2c0[3] = 0;
    local_2c0[4] = 0;
    local_2c0[5] = 0;
    local_2c0[6] = 0;
    local_2c0[7] = 1;
    local_2c0[8] = 1;
    local_2c0[9] = 1;
    local_2c0[10] = 1;
    local_2c0[11] = 1;
    local_2c0[12] = 2;
    local_2c0[13] = 2;
    local_2c0[14] = 2;
    local_2c0[15] = 2;
    local_2c0[16] = 3;
    local_2c0[17] = 3;
    local_2c0[18] = 4;
    local_2c0[19] = 5;
    local_2c0[20] = 6;
    local_2c0[21] = 6;
    local_2c0[22] = 6;
    local_2c0[23] = 7;
    local_2c0[24] = 7;
    local_2c0[25] = 7;
    local_2c0[26] = 7;
    local_2c0[27] = 7;
    local_2c0[28] = 6;
    local_2c0[29] = 6;
    local_2c0[30] = 6;
    local_2c0[31] = 5;
    local_2c0[32] = 4;
    local_2c0[33] = 3;
    local_2c0[34] = 3;
    local_2c0[35] = 2;
    local_2c0[36] = 2;
    local_2c0[37] = 2;
    local_2c0[38] = 2;
    local_2c0[39] = 1;
    local_2c0[40] = 1;
    local_2c0[41] = 1;
    local_2c0[42] = 1;
    local_2c0[43] = 1;
    local_2c0[44] = 0;
    local_2c0[45] = 0;
    local_2c0[46] = 0;
    local_2c0[47] = 0;
    local_2c0[48] = 0;
    local_2c0[49] = 0;
    local_2c0[50] = 0;
    local_2c0[51] = 0;
    int *puVar6 = reinterpret_cast<int *>(local_2c0 + 0x40);
    for (iVar2 = 0xa0; iVar2 != 0; iVar2--) {
        *puVar6 = 0;
        puVar6++;
    }
    local_2c4 = -0x19;
    do {
        PollSound();
        local_28c = KBTickCount();
        for (iVar2 = 0; iVar2 < 0x33; iVar2++) {
            iVar3 = iVar2 - 0x19 + local_2c4;
            if (iVar3 >= 0 && iVar3 < 0x280 && local_2c0[iVar3 + 0x40] != local_2c0[iVar2]) {
                unsigned char *puVar4 = local_288->pixels + iVar3;
                int iVar1 = local_2c0[iVar2] * param_4;
                unsigned char *puVar5 = local_284->pixels + iVar1 * 0x280 + iVar3;
                for (; iVar1 < param_3 && (*puVar4 = *puVar5, iVar1 - param_3 != -1); iVar1++) {
                    puVar4 += 0x280;
                    puVar5 += 0x280;
                }
                local_2c0[iVar3 + 0x40] = local_2c0[iVar2];
            }
        }
        iVar2 = 0x29;
        iVar3 = local_2c4 - 0x16;
        if (iVar3 < 0) {
            iVar2 = local_2c4 + 0x13;
            iVar3 = 0;
        }
        if (0x280 < iVar3 + iVar2)
            iVar2 = 0x280 - iVar3;
        if (iVar2 > 0) {
            BlitBitmapToScreen(gpWindowManager->field_0x46, iVar3, 0, iVar2, param_3, iVar3, 0);
            DelayTil(&local_28c);
        }
        local_2c4 += 4;
    } while (local_2c4 < 0x299);
    gpMouseManager->ShowColorPointer();
}

