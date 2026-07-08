// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\icon2bs.obj   from: .\basewin.lib
// functions: 1   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/icon2bs.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <BASE/Icon2b.h>

// Scale wrapper: render the icon full-size into a temp 0x40x0x40 bitmap via IconToBitmap, then
// point-sample it down to param_11 x param_11 into the destination (skipping transparent pixels).
// param_11 == 0x20 is the identity fast-path (straight IconToBitmap).
VA(0x004d2f90, 0x179)
void IconToBitmapScale(class icon *param_1, class bitmap *param_2, int param_3, int param_4, int param_5,
                       int param_6, int param_7, int param_8, int param_9, int param_10, int param_11)
{
    if (param_11 != 0x20) {
        int iVar2 = 0x20 / param_11;
        bitmap *local_18 = new bitmap(0, 0x40, 0x40);
        int iVar3 = reinterpret_cast<int>(local_18->m_pixels);
        int iVar5 = 0;
        do {
            int *puVar8 = reinterpret_cast<int *>(iVar3 + iVar5);
            for (int iVar4 = 8; iVar4 != 0; iVar4--)
                *puVar8++ = 0;
            iVar5 = iVar5 + 0x20;
        } while (iVar5 < 0x800);
        IconToBitmap(param_1, local_18, 0, 0, param_5, 1, 0, 0, 0x20, 0x20, 0);
        short sVar1 = param_2->m_width;
        char *local_c = reinterpret_cast<char *>(param_3 + param_4 * sVar1 +
                                                 reinterpret_cast<int>(param_2->m_pixels));
        char *pcVar6 = reinterpret_cast<char *>((((1 - param_11) * iVar2 + 0x20) >> 1) * 0x41 +
                                                reinterpret_cast<int>(local_18->m_pixels));
        if (0 < param_11) {
            int local_14 = param_11;
            do {
                int iVar4 = param_11;
                char *pcVar7 = local_c;
                char *pcVar9 = pcVar6;
                if (0 < param_11) {
                    do {
                        if (*pcVar9 != 0)
                            *pcVar7 = *pcVar9;
                        iVar4 = iVar4 - 1;
                        pcVar7 = pcVar7 + 1;
                        pcVar9 = pcVar9 + iVar2;
                    } while (iVar4 != 0);
                }
                pcVar6 = pcVar6 + iVar2 * 0x40;
                local_c = local_c + sVar1;
                local_14 = local_14 - 1;
            } while (local_14 != 0);
        }
        delete local_18;
    } else {
        IconToBitmap(param_1, param_2, param_3, param_4, param_5, param_6, param_7, param_8, param_9,
                     param_10, 0);
    }
}
