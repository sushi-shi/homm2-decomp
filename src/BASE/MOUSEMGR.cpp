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
VA(0x004c9270, 0xd9)
mouseManager::mouseManager(void) : baseManager()
{
    int i;
    m_savedUnderlying = 0;
    m_active = 0;
    field_0x42 = -1;
    strcpy(name, "mouseManager");
    field_0x3a = 0;
    field_0x82 = 1;
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
    field_0x86 = 1;
}

VA(0x004c9350, 0x94)
int mouseManager::Open(int id)
{
    field_0x7e = 0;
    m_savedUnderlying = new bitmap(0x21, 0x42, 0x40);
    field_0x5e = 0x13f;
    field_0x62 = 0xef;
    field_0x6e = 0x140;
    field_0x66 = 0x13f;
    field_0x6a = 0xef;
    field_0x56 = 0x140;
    field_0x46 = 0;
    field_0x4a = 0;
    field_0x72 = 0xf0;
    field_0x5a = 0xf0;
    if (gbColorMice != 0)
        ShowCursor(0);
    field_0xc = 0x40;
    m_active = 1;
    field_0x10 = id;
    return 0;
}

VA(0x004c93f0, 0xed)
void mouseManager::Close(void)
{
    if (m_active == 1) {
        m_active = 0;
        if (m_savedUnderlying != 0)
            delete m_savedUnderlying;
        m_savedUnderlying = 0;
        SetCursor(LoadCursorA(0, IDC_ARROW));
        for (int i = 0; i < 0x60; i++) {
            if (hMouseCursor[i] != 0)
                DestroyIcon(hMouseCursor[i]);
            hMouseCursor[i] = 0;
            if (cAndBits[i] != 0)
                BaseFree(cAndBits[i], __FILE__, __LINE__);
            cAndBits[i] = 0;
            if (cColorBits[i] != 0)
                BaseFree(cColorBits[i], __FILE__, __LINE__);
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
    if (field_0x7e == 0) {
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
        if (field_0x42 != param_3 && (field_0x42 = param_3, gbColorMice != 0)) {
            int saved82 = field_0x82;
            field_0x82 = 0;
            if (m_cursorIcon != 0)
                gpResourceManager->Dispose(m_cursorIcon);
            char *fmt;
            if (field_0x42 == 0)
                fmt = "ADVMCO.ICN";
            else if (field_0x42 == 2)
                fmt = "SPELCO.ICN";
            else
                fmt = "CMSECO.ICN";
            char local_10[16];
            sprintf(local_10, fmt);
            m_cursorIcon = gpResourceManager->GetIcon(local_10);
            ProcessAssert(param_2 != 1000, __FILE__, __LINE__);
            field_0x3a = -1;
            field_0x82 = saved82;
        }
        SetPointer(param_2);
        gpResourceManager->RestorePosition();
        gbPutzingWithMouseCtr--;
    }
}

VA(0x004c9630, 0x405)
void mouseManager::SetPointer(int) {}

VA(0x004c9a40, 0x47a)
void mouseManager::NewUpdate(int) {}

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
    int w = iMouseSize[field_0x46][0];
    if (640 < field_0x66 + w)
        w = 640 - field_0x66;
    field_0x76 = w;
    int h = iMouseSize[field_0x46][1];
    if (480 < field_0x6a + h)
        h = 480 - field_0x6a;
    field_0x7a = h;
    gpWindowManager->m_screen->CopyToCareful(m_savedUnderlying, 0, 0, field_0x5e, field_0x62,
                                               field_0x76, field_0x7a);
    IconToBitmap(m_cursorIcon, gpWindowManager->m_screen, field_0x66, field_0x6a, field_0x3a,
                 1, 0, 0, 640, 480, 0);
}

VA(0x004c9fd0, 0x29)
void mouseManager::RestoreUnderlying(void)
{
    m_savedUnderlying->CopyToCareful(gpWindowManager->m_screen, field_0x5e, field_0x62, 0, 0,
                              field_0x76, field_0x7a);
}

VA(0x004ca000, 0x2c)
void mouseManager::ReallyHidePointer(void)
{
    if (gbColorMice == 0) {
        ShowCursor(0);
        return;
    }
    int old = field_0x86;
    field_0x86 = old + 1;
    if (old + 1 == 1)
        NewUpdate(1);
}

VA(0x004ca030, 0xb9)
void mouseManager::ReallyShowPointer(void)
{
    if (gbColorMice == 0) {
        ShowCursor(1);
    } else if (field_0x86 > 0 && --field_0x86 == 0) {
        gbPutzingWithMouseCtr++;
        if (gbColorMice != 0) {
            GetCursorPos(&gMouseCheckPt);
            ScreenToClient(hwndApp, &gMouseCheckPt);
            int x = (gMouseCheckPt.x * 640) / iMainWinScreenWidth;
            field_0x56 = x;
            int y = (gMouseCheckPt.y * 480) / iMainWinScreenHeight;
            field_0x5a = y;
            CheckChangeCursor(x, y, 0);
        }
        NewUpdate(1);
        gbPutzingWithMouseCtr--;
    }
}

VA(0x004ca0f0, 0x1a)
void mouseManager::HideColorPointer(void)
{
    int old = field_0x86;
    field_0x86 = old + 1;
    if (old + 1 == 1)
        NewUpdate(1);
}

VA(0x004ca110, 0x98)
void mouseManager::ShowColorPointer(void)
{
    if (field_0x86 > 0 && --field_0x86 == 0) {
        gbPutzingWithMouseCtr++;
        if (gbColorMice != 0) {
            GetCursorPos(&gMouseCheckPt);
            ScreenToClient(hwndApp, &gMouseCheckPt);
            int x = (gMouseCheckPt.x * 640) / iMainWinScreenWidth;
            field_0x56 = x;
            int y = (gMouseCheckPt.y * 480) / iMainWinScreenHeight;
            field_0x5a = y;
            CheckChangeCursor(x, y, 0);
        }
        NewUpdate(1);
        gbPutzingWithMouseCtr--;
    }
}

VA(0x004ca1b0, 0xc)
int mouseManager::IsVis(void) { return field_0x86 == 0; }

VA(0x004ca1c0, 0x66)
void mouseManager::CheckUpdateMousePos(void)
{
    if (gbColorMice != 0) {
        GetCursorPos(&gMouseCheckPt);
        ScreenToClient(hwndApp, &gMouseCheckPt);
        int x = (gMouseCheckPt.x * 640) / iMainWinScreenWidth;
        field_0x56 = x;
        int y = (gMouseCheckPt.y * 480) / iMainWinScreenHeight;
        field_0x5a = y;
        CheckChangeCursor(x, y, 0);
    }
}

VA(0x004ca230, 0x191)
void mouseManager::SetColorMice(int param_1)
{
    if (param_1 != gbColorMice) {
        int savedWM56 = gpWindowManager->m_updateFlags;
        gpWindowManager->m_updateFlags = 0;
        int savedInNew = bInNewMouseUpdate;
        gbPutzingWithMouseCtr++;
        bInNewMouseUpdate = 0;
        if (gbColorMice == 0) {
            ShowCursor(0);
        } else {
            int old = field_0x86;
            field_0x86 = old + 1;
            if (old + 1 == 1)
                NewUpdate(1);
        }
        int savedX = field_0x3a;
        int savedY = field_0x42;
        int saved7e = field_0x7e;
        field_0x82 = 0;
        gbColorMice = param_1;
        field_0x3a = -99;
        field_0x42 = -1;
        field_0x7e = 0;
        SetPointer(gDefaultCursorName, savedX, savedY);
        field_0x82 = 1;
        field_0x7e = saved7e;
        if (gbColorMice == 0) {
            ShowCursor(1);
        } else if (field_0x86 > 0 && --field_0x86 == 0) {
            gbPutzingWithMouseCtr++;
            if (gbColorMice != 0) {
                GetCursorPos(&gMouseCheckPt);
                ScreenToClient(hwndApp, &gMouseCheckPt);
                int x = (gMouseCheckPt.x * 640) / iMainWinScreenWidth;
                field_0x56 = x;
                int y = (gMouseCheckPt.y * 480) / iMainWinScreenHeight;
                field_0x5a = y;
                CheckChangeCursor(x, y, 0);
            }
            NewUpdate(1);
            gbPutzingWithMouseCtr--;
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
int *iMouseOffset;
signed char iMouseSize[96][2];
signed char iHotSpot[96][2];
int gbInSetPointer;
int bInNewMouseUpdate;
struct tagBITMAP *bmpAndMask;
void *hMouseCursor[96];
void *cAndBits[96];
void *cColorBits[98];
struct _ICONINFO *IconInfo;
void *hbmpAndMask[106];
