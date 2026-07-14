// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\MOUSEMGR.OBJ   from: .\basewin.lib
// functions: 17   data: 12
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/mouseManager.h>
#include <string.h>
#include <stdio.h>
#include <windows.h>
#include <SOURCE/X_GLOBAL.h>
#include <BASE/bitmap.h>
#include <BASE/icon.h>
#include <BASE/Icon2b.h>
#include <BASE/Misc.h>
#include <BASE/resourceManager.h>
#include <BASE/heroWindowManager.h>
#include <SOURCE/KB.h>
#include <SOURCE/kbwin.h>
#include <BASE/INPUTMGR.h>
#include <_globals_model.h>



// ---- module-private synthetic globals (retail xref: single-module) ----
DATA(0x00533238) static int gOldMouseRight;
DATA(0x00533b40) static POINT gMouseScreenPt;    // GetCursorPos scratch (mouseManager::MouseCoords)
DATA(0x00533b48) static POINT gMouseCheckPt;     // GetCursorPos scratch (mouseManager::CheckUpdateMousePos/ShowColorPointer)
DATA(0x00533b50) static int gOldMouseTop;
DATA(0x00533fd8) static int gOldMouseBottom;
DATA(0x00533fdc) static int gOldMouseLeft;

VA(0x004c9270, 0xd9)
mouseManager::mouseManager(void) : baseManager()
{
    int i;
    m_savedUnderlying = 0;
    m_active = 0;
    m_cursorType = -1;
    strcpy(name, "mouseManager");
    m_cursorFrame = 0;
    m_cursorReady = 1;
    m_cursorIcon = 0;
    for (i = 0; i < 0x60; i++)
        hbmpAndMask[i] = 0;
    for (i = 0; i < 0x60; i++)
        hMouseCursor[i] = 0;
    for (i = 0; i < 0x60; i++)
        cColorBits[i] = 0;
    for (i = 0; i < 0x60; i++)
        cAndBits[i] = 0;
    for (i = 0; i < 0x60; i++) {
        if (iHotSpot[i][0] == -1)
            iHotSpot[i][0] = iMouseSize[i][1] / 2;
        if (iHotSpot[i][1] == -1)
            iHotSpot[i][1] = iMouseSize[i][1] / 2;
    }
    m_hideCount = 1;
}

// @early-stop
// base .text 0x91, retail 0x94; non-reloc differences are base +0x52..+0x56
// (the shared 0xf0 load) and base +0x60..+0x65 versus retail +0x5b..+0x68
// (two immediate stores). The suffix is identical after the resulting 3-byte shift.
// Relocations resolve 3/3 with only-base=0. Direct 0xf0 stores and a distinct
// 0xef+1 spelling for the second store both select the same cumulative constant CSE.
VA(0x004c9350, 0x94)
int mouseManager::Open(int id)
{
    m_forcePointerUpdate = 0;
    m_savedUnderlying = new bitmap(0x21, 0x42, 0x40);
    m_savedW = 0x13f;
    m_savedH = 0xef;
    field_0x6e = 0x140;
    field_0x66 = 0x13f;
    field_0x6a = 0xef;
    m_mouseX = 0x140;
    m_cursorSizeIndex = 0;
    field_0x4a = 0;
    field_0x72 = 0xf0;
    m_mouseY = 0xf0;
    if (gbColorMice != 0)
        ShowCursor(0);
    field_0xc = 0x40;
    m_active = 1;
    field_0x10 = id;
    return 0;
}

// @early-stop
// base .text 0xed, retail 0xed after restoring retail __LINE__ values. Register-only
// spans are +0x2b..+0x2c, +0x43..+0x54, +0x5a..+0x6b, +0x7f..+0x8e,
// +0xa2..+0xb1, and +0xb9..+0xcb: base uses EDI as the byte offset and EBP for
// DestroyIcon; retail exchanges EDI/EBP. Relocations resolve 14/14, only-base=0.
// for-scope, hoisted int, and register-qualified counters all choose the base allocation.
VA(0x004c93f0, 0xed)
void mouseManager::Close(void)
{
    int i;
    if (m_active == 1) {
        m_active = 0;
        if (m_savedUnderlying != 0)
            delete m_savedUnderlying;
        m_savedUnderlying = 0;
        SetCursor(LoadCursorA(0, IDC_ARROW));
        for (i = 0; i < 0x60; i++) {
            if (hMouseCursor[i] != 0)
                DestroyIcon(hMouseCursor[i]);
            hMouseCursor[i] = 0;
            if (cAndBits[i] != 0)
#line 330
                H2_FREE(cAndBits[i], "I:\\Projects\\Heroes\\Prog\\BASE\\MOUSEMGR.CPP", 0x14a);
            cAndBits[i] = 0;
            if (cColorBits[i] != 0)
                H2_FREE(cColorBits[i], "I:\\Projects\\Heroes\\Prog\\BASE\\MOUSEMGR.CPP", 0x14e);
            cColorBits[i] = 0;
            if (hbmpAndMask[i] != 0)
                DeleteObject(hbmpAndMask[i]);
            hbmpAndMask[i] = 0;
        }
        if (m_cursorIcon != 0)
            gpResourceManager->Dispose(m_cursorIcon);
        m_cursorIcon = 0;
    }
}

VA(0x004c94e0, 0x5)
int mouseManager::Main(struct tag_message &) { return 0; }

VA(0x004c94f0, 0x135)
void mouseManager::SetPointer(char *name, int param_2, int param_3)
{
    if (m_forcePointerUpdate == 0) {
        gbPutzingWithMouseCtr++;
        gpResourceManager->SavePosition();
        if (param_3 == -999) {
            if (giCurExe == 1 || *name == 'a' || *name == 'A')
                param_3 = 0;
            else if (*name == 's' || *name == 'S')
                param_3 = 2;
            else
                param_3 = 1;
        }
        if (m_cursorType != param_3 && (m_cursorType = param_3, gbColorMice != 0)) {
            int saved82 = m_cursorReady;
            m_cursorReady = 0;
            if (m_cursorIcon != 0)
                gpResourceManager->Dispose(m_cursorIcon);
            char local_10[16];
            if (m_cursorType == 0)
                sprintf(local_10, "ADVMCO.ICN");
            else if (m_cursorType == 2)
                sprintf(local_10, "SPELCO.ICN");
            else
                sprintf(local_10, "CMSECO.ICN");
            m_cursorIcon = gpResourceManager->GetIcon(local_10);
#line 410
            H2_ASSERT(param_2 != 1000, "I:\\Projects\\Heroes\\Prog\\BASE\\MOUSEMGR.CPP", 0x19a);
            m_cursorFrame = -1;
            m_cursorReady = saved82;
        }
        SetPointer(param_2);
        gpResourceManager->RestorePosition();
        gbPutzingWithMouseCtr--;
    }
}

// @early-stop
// base .text 0x40f, retail 0x405. The mask loop differs at base +0x1c6..+0x24f
// versus retail +0x1d1..+0x243: row*32/row*4 strength reduction uses EBX/EDI in
// base versus independent EAX/EDI offsets in retail. Paired-offset for/while forms
// (with direct subscripts or pointer temps) regress to 75.10%; the row/column form is 94.32%.
// Aggregate-reloc spelling spans are base +0x26b..+0x2da and +0x337..+0x399
// versus retail +0x25f..+0x2ce and +0x32b..+0x38d. Audit: base 69, retail 70.
// The relocation checker now resolves indexed member displacements correctly and reports
// only-base=0. Source uses the proper typed bmpAndMask and IconInfo members; retail's interior
// labels are delinker identities, not globals or aliases that should be reproduced in source.
VA(0x004c9630, 0x405)
void mouseManager::SetPointer(int frame)
{
    if (m_forcePointerUpdate != 0 || frame < 0 || m_active != 1 || m_cursorFrame == frame || gbInSetPointer != 0)
        return;

    gbInSetPointer = 1;
    gbPutzingWithMouseCtr++;
    gpResourceManager->SavePosition();
    if (giCurExe == 1)
        m_cursorType = 0;
    if (frame == 1000)
        frame = m_cursorFrame;
    else
        m_cursorFrame = frame;
    m_cursorSizeIndex = iMouseOffset[m_cursorType] + frame;
#line 458
    H2_ASSERT(m_cursorSizeIndex >= 0 && m_cursorSizeIndex < 96, "I:\\Projects\\Heroes\\Prog\\BASE\\MOUSEMGR.CPP", 0x1ca);

    if (gbColorMice != 0) {
        NewUpdate(1);
    } else {
        if (hMouseCursor[m_cursorSizeIndex] == 0) {
#line 480
            cColorBits[m_cursorSizeIndex] = H2_ALLOC(0x400, "I:\\Projects\\Heroes\\Prog\\BASE\\MOUSEMGR.CPP", 0x1e0);
            cAndBits[m_cursorSizeIndex] = H2_ALLOC(0x100, "I:\\Projects\\Heroes\\Prog\\BASE\\MOUSEMGR.CPP", 0x1e1);

            char filename[16];
            if (m_cursorType == 0)
                sprintf(filename, "ADVMBW%02d.BMP", frame + 1);
            else if (m_cursorType == 2)
                sprintf(filename, "SPELBW%02d.BMP", frame);
            else
                sprintf(filename, "CMSEBW%02d.BMP", frame + 1);

            gpResourceManager->PointToFile(gpResourceManager->MakeId(filename, 1));
            gpResourceManager->ReadBlock(static_cast<signed char *>(cColorBits[m_cursorSizeIndex]), 6);
            gpResourceManager->ReadBlock(static_cast<signed char *>(cColorBits[m_cursorSizeIndex]), 0x400);
            memset(cAndBits[m_cursorSizeIndex], 0, 0x100);
            for (int row = 0; row < 32; row++) {
                for (int column = 0; column < 32; column++) {
                    signed char *colorBits = static_cast<signed char *>(cColorBits[m_cursorSizeIndex]);
                    signed char *andBits = static_cast<signed char *>(cAndBits[m_cursorSizeIndex]);
                    if (colorBits[row * 32 + column] == 0)
                        andBits[row * 4 + column / 8] |= 1 << (7 - (column & 7));
                    else if (colorBits[row * 32 + column] == 1)
                        andBits[0x80 + row * 4 + column / 8] |= 1 << (7 - (column & 7));
                }
            }

            bmpAndMask[m_cursorSizeIndex].bmType = 0;
            bmpAndMask[m_cursorSizeIndex].bmWidth = 32;
            bmpAndMask[m_cursorSizeIndex].bmHeight = 64;
            bmpAndMask[m_cursorSizeIndex].bmWidthBytes = 4;
            bmpAndMask[m_cursorSizeIndex].bmPlanes = 1;
            bmpAndMask[m_cursorSizeIndex].bmBitsPixel = 1;
            bmpAndMask[m_cursorSizeIndex].bmWidthBytes = 4;
            bmpAndMask[m_cursorSizeIndex].bmBits = cAndBits[m_cursorSizeIndex];
            hbmpAndMask[m_cursorSizeIndex] = CreateBitmapIndirect(&bmpAndMask[m_cursorSizeIndex]);
#line 514
            H2_ASSERT(reinterpret_cast<int>(hbmpAndMask[m_cursorSizeIndex]), "I:\\Projects\\Heroes\\Prog\\BASE\\MOUSEMGR.CPP", 0x202);

            IconInfo[m_cursorSizeIndex].fIcon = 0;
            if (m_cursorType == 2) {
                IconInfo[m_cursorSizeIndex].xHotspot = 15;
                IconInfo[m_cursorSizeIndex].yHotspot = 15;
            } else {
                IconInfo[m_cursorSizeIndex].xHotspot = iHotSpot[m_cursorSizeIndex][0];
                IconInfo[m_cursorSizeIndex].yHotspot = iHotSpot[m_cursorSizeIndex][1];
            }
            IconInfo[m_cursorSizeIndex].hbmMask = hbmpAndMask[m_cursorSizeIndex];
            IconInfo[m_cursorSizeIndex].hbmColor = 0;
            hMouseCursor[m_cursorSizeIndex] = CreateIconIndirect(&IconInfo[m_cursorSizeIndex]);
#line 533
            H2_ASSERT(reinterpret_cast<int>(hMouseCursor[m_cursorSizeIndex]), "I:\\Projects\\Heroes\\Prog\\BASE\\MOUSEMGR.CPP", 0x215);
        }
        SetCursor(hMouseCursor[m_cursorSizeIndex]);
    }
    gpResourceManager->RestorePosition();
    gbInSetPointer = 0;
    gbPutzingWithMouseCtr--;
}

// @early-stop
// base .text 0x469, retail 0x47a. Private-DATA/second-column alias-only spans are
// +0xdc..+0xe2, +0x12c..+0x132, and +0x14c..+0x152. The remaining equivalent
// compare/load-order spans are base/retail +0x200..+0x203, +0x213..+0x216,
// +0x243..+0x24a; base +0x255..+0x25d vs retail +0x255..+0x25c;
// base +0x2eb..+0x307 vs retail +0x2ed..+0x309; and base +0x361..+0x388
// vs retail +0x373..+0x399. Relocations resolve base 84/retail 83, only-base=0.
// Reversed < and > spellings are byte-neutral; ternary and direct-field width/height
// forms regress, while explicit branch assignments give the retained 97.38% shape.
VA(0x004c9a40, 0x47a)
void mouseManager::NewUpdate(int force)
{
    int width;
    int height;
    if (m_hideCount != 0 && force == 0)
        return;
    if (m_cursorReady == 0 || bInNewMouseUpdate != 0)
        return;

    bInNewMouseUpdate = 1;
    gbPutzingWithMouseCtr++;
    if (force == 0) {
        if (gbColorMice == 0)
            goto updateDone;
        GetCursorPos(&gMouseCheckPt);
        ScreenToClient(hwndApp, &gMouseCheckPt);
        m_mouseX = (gMouseCheckPt.x * 640) / iMainWinScreenWidth;
        m_mouseY = (gMouseCheckPt.y * 480) / iMainWinScreenHeight;
        CheckChangeCursor(m_mouseX, m_mouseY, 0);
    }
    if (gbColorMice == 0)
        goto updateDone;

    if (force == 0 &&
        m_mouseX - iHotSpot[m_cursorSizeIndex][0] == field_0x66 &&
        m_mouseY - iHotSpot[m_cursorSizeIndex][1] == field_0x6a)
        goto updateDone;

    gOldMouseLeft = m_savedW;
    gOldMouseTop = m_savedH;
    gOldMouseRight = field_0x6e;
    gOldMouseBottom = field_0x72;

    field_0x66 = m_mouseX - iHotSpot[m_cursorSizeIndex][0];
    field_0x6a = m_mouseY - iHotSpot[m_cursorSizeIndex][1];
    field_0x6e = field_0x66 + iMouseSize[m_cursorSizeIndex][0] - 1;
    field_0x72 = field_0x6a + iMouseSize[m_cursorSizeIndex][1] - 1;
    if (field_0x6e > 639)
        field_0x6e = 639;
    if (field_0x72 > 479)
        field_0x72 = 479;
    m_savedW = 0;
    if (field_0x66 >= 0)
        m_savedW = field_0x66;
    m_savedH = 0;
    if (field_0x6a >= 0)
        m_savedH = field_0x6a;

    if (gOldMouseLeft <= 639 && gOldMouseTop <= 479 &&
        gOldMouseRight >= 0 && gOldMouseBottom >= 0) {
        if (gOldMouseRight < m_savedW || gOldMouseLeft > field_0x6e ||
            gOldMouseBottom < m_savedH || gOldMouseTop > field_0x72) {
            if (gOldMouseRight > 639)
                gOldMouseRight = 639;
            if (gOldMouseBottom > 479)
                gOldMouseBottom = 479;
            BlitBitmapToScreenNoMouseCheck(gpWindowManager->m_screen,
                gOldMouseLeft, gOldMouseTop,
                gOldMouseRight - gOldMouseLeft + 1,
                gOldMouseBottom - gOldMouseTop + 1,
                gOldMouseLeft, gOldMouseTop);
        } else {
            if (gOldMouseLeft > m_savedW)
                gOldMouseLeft = m_savedW;
            if (gOldMouseTop > m_savedH)
                gOldMouseTop = m_savedH;
            int right = m_savedW + iMouseSize[m_cursorSizeIndex][0] - 1;
            if (gOldMouseRight < right)
                gOldMouseRight = right;
            int bottom = m_savedH + iMouseSize[m_cursorSizeIndex][1] - 1;
            if (gOldMouseBottom < bottom)
                gOldMouseBottom = bottom;
        }
    }

    gOldMouseLeft = m_savedW;
    gOldMouseTop = m_savedH;
    gOldMouseRight = m_savedW + iMouseSize[field_0x4a][0] - 1;
    gOldMouseBottom = m_savedH + iMouseSize[field_0x4a][1] - 1;
    if (gOldMouseLeft <= 639 && gOldMouseTop <= 479 &&
        gOldMouseRight >= 0 && gOldMouseBottom >= 0) {
        if (gOldMouseRight > 639)
            gOldMouseRight = 639;
        if (gOldMouseBottom > 479)
            gOldMouseBottom = 479;
    }

    width = iMouseSize[m_cursorSizeIndex][0];
    if (m_savedW + width > 640)
        field_0x76 = 640 - m_savedW;
    else
        field_0x76 = width;
    height = iMouseSize[m_cursorSizeIndex][1];
    if (m_savedH + height > 480)
        field_0x7a = 480 - m_savedH;
    else
        field_0x7a = height;

    gpWindowManager->m_screen->CopyToCareful(m_savedUnderlying, 0, 0,
        m_savedW, m_savedH, field_0x76, field_0x7a);
    if (m_hideCount == 0)
        IconToBitmap(m_cursorIcon, gpWindowManager->m_screen, field_0x66, field_0x6a,
            m_cursorFrame, 1, 0, 0, 640, 480, 0);
    BlitBitmapToScreenNoMouseCheck(gpWindowManager->m_screen,
        gOldMouseLeft, gOldMouseTop,
        gOldMouseRight - gOldMouseLeft + 1,
        gOldMouseBottom - gOldMouseTop + 1,
        gOldMouseLeft, gOldMouseTop);
    m_savedUnderlying->CopyToCareful(gpWindowManager->m_screen, m_savedW, m_savedH,
        0, 0, field_0x76, field_0x7a);
    field_0x4a = m_cursorSizeIndex;

updateDone:
    bInNewMouseUpdate = 0;
    gbPutzingWithMouseCtr--;
}

VA(0x004c9ec0, 0x56)
void mouseManager::MouseCoords(int &x, int &y)
{
    GetCursorPos(&gMouseScreenPt);
    ScreenToClient(hwndApp, &gMouseScreenPt);
    x = (gMouseScreenPt.x * 640) / iMainWinScreenWidth;
    y = (gMouseScreenPt.y * 480) / iMainWinScreenHeight;
}

VA(0x004c9f20, 0xa2)
void mouseManager::SaveAndDraw(void)
{
    field_0x76 = field_0x66 + iMouseSize[m_cursorSizeIndex][0] > 640
        ? 640 - field_0x66 : iMouseSize[m_cursorSizeIndex][0];
    field_0x7a = field_0x6a + iMouseSize[m_cursorSizeIndex][1] > 480
        ? 480 - field_0x6a : iMouseSize[m_cursorSizeIndex][1];
    gpWindowManager->m_screen->CopyToCareful(m_savedUnderlying, 0, 0, m_savedW, m_savedH,
                                               field_0x76, field_0x7a);
    IconToBitmap(m_cursorIcon, gpWindowManager->m_screen, field_0x66, field_0x6a, m_cursorFrame,
                 1, 0, 0, 640, 480, 0);
}

VA(0x004c9fd0, 0x29)
void mouseManager::RestoreUnderlying(void)
{
    m_savedUnderlying->CopyToCareful(gpWindowManager->m_screen, m_savedW, m_savedH, 0, 0,
                              field_0x76, field_0x7a);
}

VA(0x004ca000, 0x2c)
void mouseManager::ReallyHidePointer(void)
{
    if (gbColorMice != 0) {
        m_hideCount++;
        if (m_hideCount == 1)
            NewUpdate(1);
    } else {
        ShowCursor(0);
    }
}

VA(0x004ca030, 0xb9)
void mouseManager::ReallyShowPointer(void)
{
    if (gbColorMice != 0) {
        if (m_hideCount > 0 && --m_hideCount == 0) {
            gbPutzingWithMouseCtr++;
            if (gbColorMice != 0) {
                GetCursorPos(&gMouseCheckPt);
                ScreenToClient(hwndApp, &gMouseCheckPt);
                int x = (gMouseCheckPt.x * 640) / iMainWinScreenWidth;
                m_mouseX = x;
                int y = (gMouseCheckPt.y * 480) / iMainWinScreenHeight;
                m_mouseY = y;
                CheckChangeCursor(x, y, 0);
            }
            NewUpdate(1);
            gbPutzingWithMouseCtr = gbPutzingWithMouseCtr - 1;
        }
    } else {
        ShowCursor(1);
    }
}

VA(0x004ca0f0, 0x1a)
void mouseManager::HideColorPointer(void)
{
    m_hideCount++;
    if (m_hideCount == 1)
        NewUpdate(1);
}

VA(0x004ca110, 0x98)
void mouseManager::ShowColorPointer(void)
{
    if (m_hideCount > 0 && --m_hideCount == 0) {
        gbPutzingWithMouseCtr++;
        if (gbColorMice != 0) {
            GetCursorPos(&gMouseCheckPt);
            ScreenToClient(hwndApp, &gMouseCheckPt);
            int x = (gMouseCheckPt.x * 640) / iMainWinScreenWidth;
            m_mouseX = x;
            int y = (gMouseCheckPt.y * 480) / iMainWinScreenHeight;
            m_mouseY = y;
            CheckChangeCursor(x, y, 0);
        }
        NewUpdate(1);
        gbPutzingWithMouseCtr--;
    }
}

VA(0x004ca1b0, 0xc)
int mouseManager::IsVis(void) { return m_hideCount == 0; }

VA(0x004ca1c0, 0x66)
void mouseManager::CheckUpdateMousePos(void)
{
    if (gbColorMice != 0) {
        GetCursorPos(&gMouseCheckPt);
        ScreenToClient(hwndApp, &gMouseCheckPt);
        int x = (gMouseCheckPt.x * 640) / iMainWinScreenWidth;
        m_mouseX = x;
        int y = (gMouseCheckPt.y * 480) / iMainWinScreenHeight;
        m_mouseY = y;
        CheckChangeCursor(x, y, 0);
    }
}

// @early-stop
// base .text 0x18f, retail 0x191. Divergence is confined to the prefix: base +0x0..+0xf
// vs retail +0x0..+0xd, base +0x77..+0x80 vs retail +0x75..+0x81, and base
// +0x8c..+0x91 vs retail +0x8d..+0x93. From base +0xb3 / retail +0xb5 onward,
// instructions are identical; private-DATA names are delinker aliases. Relocations
// resolve 29/29, only-base=0. Reversing savedY/savedX declarations improved 86.82%
// to 96.84% and is retained. Direct/reversed comparisons, old-color temp, volatile
// reference, and gbColorMice|0 all still fold the global into base's memory CMP.
VA(0x004ca230, 0x191)
void mouseManager::SetColorMice(int param_1)
{
    if (param_1 != gbColorMice) {
        int savedWM56 = gpWindowManager->m_updateFlags;
        gpWindowManager->m_updateFlags = 0;
        gbPutzingWithMouseCtr++;
        int savedInNew = bInNewMouseUpdate;
        bInNewMouseUpdate = 0;
        if (gbColorMice != 0) {
            m_hideCount++;
            if (m_hideCount == 1)
                NewUpdate(1);
        } else {
            ShowCursor(0);
        }
        int savedY = m_cursorType;
        int savedX = m_cursorFrame;
        int saved7e = m_forcePointerUpdate;
        m_cursorReady = 0;
        gbColorMice = param_1;
        m_cursorFrame = -99;
        m_cursorType = -1;
        m_forcePointerUpdate = 0;
        SetPointer(gDefaultCursorName, savedX, savedY);
        m_cursorReady = 1;
        m_forcePointerUpdate = saved7e;
        if (gbColorMice != 0) {
            if (m_hideCount > 0 && --m_hideCount == 0) {
                gbPutzingWithMouseCtr++;
                if (gbColorMice != 0) {
                    GetCursorPos(&gMouseCheckPt);
                    ScreenToClient(hwndApp, &gMouseCheckPt);
                    int x = (gMouseCheckPt.x * 640) / iMainWinScreenWidth;
                    m_mouseX = x;
                    int y = (gMouseCheckPt.y * 480) / iMainWinScreenHeight;
                    m_mouseY = y;
                    CheckChangeCursor(x, y, 0);
                }
                NewUpdate(1);
                gbPutzingWithMouseCtr--;
            }
        } else {
            ShowCursor(1);
        }
        gbPutzingWithMouseCtr--;
        bInNewMouseUpdate = savedInNew;
        gpWindowManager->m_updateFlags = savedWM56;
    }
}


// ===== vtable mouseManager : public baseManager  (3 slots) =====
//  [ 0] VA(0x004c9350, 0x94)  int mouseManager::Open(int)   <- override (implements baseManager pure virtual)
//  [ 1] VA(0x004c93f0, 0xed)  void mouseManager::Close(void)   <- override (implements baseManager pure virtual)
//  [ 2] VA(0x004c94e0, 0x5)  int mouseManager::Main(struct tag_message &)   <- override (implements baseManager pure virtual)

// ---- vtables (compiler-emitted; census) ----
VTBL(mouseManager, 0x004eba00);

// ---- globals (definitions, RVA order) ----
DATA(0x0051ebc8) int iMouseOffset[4];
DATA(0x0051ebd8) signed char iMouseSize[96][2];
DATA(0x0051ec98) signed char iHotSpot[96][2];
DATA(0x0051ed58) int gbInSetPointer;
DATA(0x0051ed5c) int bInNewMouseUpdate;
DATA(0x00533240) struct tagBITMAP bmpAndMask[97];
DATA(0x00533b58) void *hMouseCursor[96];
DATA(0x00533cd8) void *cAndBits[96];
DATA(0x00533e58) void *cColorBits[98];
DATA(0x00533fe0) struct _ICONINFO IconInfo[96];
DATA(0x00534760) void *hbmpAndMask[106];
