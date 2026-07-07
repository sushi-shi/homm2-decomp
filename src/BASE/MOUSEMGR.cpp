// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\MOUSEMGR.OBJ   from: .\basewin.lib
// functions: 17   data: 12
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/mouseManager.h>
#include <string.h>
#include <windows.h>
#include <SOURCE/X_GLOBAL.h>
#include <BASE/bitmap.h>
#include <BASE/heroWindowManager.h>
#include <SOURCE/KB.h>
#include <SOURCE/kbwin.h>
#include <_globals_model.h>
VA(0x004c9270, 0xd9)
mouseManager::mouseManager(void) : baseManager()
{
    int i;
    field_0x36 = 0;
    field_0x32 = 0;
    field_0x42 = -1;
    strcpy(name, "mouseManager");
    field_0x3a = 0;
    field_0x82 = 1;
    field_0x3e = 0;
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
int mouseManager::Open(int) { return 0; }

VA(0x004c93f0, 0xed)
void mouseManager::Close(void) {}

VA(0x004c94e0, 0x5)
int mouseManager::Main(struct tag_message &) { return 0; }

VA(0x004c94f0, 0x135)
void mouseManager::SetPointer(char *, int, int) {}

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
void mouseManager::SaveAndDraw(void) {}

VA(0x004c9fd0, 0x29)
void mouseManager::RestoreUnderlying(void)
{
    field_0x36->CopyToCareful(gpWindowManager->field_0x46, field_0x5e, field_0x62, 0, 0,
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
void mouseManager::ReallyShowPointer(void) {}

VA(0x004ca0f0, 0x1a)
void mouseManager::HideColorPointer(void)
{
    int old = field_0x86;
    field_0x86 = old + 1;
    if (old + 1 == 1)
        NewUpdate(1);
}

VA(0x004ca110, 0x98)
void mouseManager::ShowColorPointer(void) {}

VA(0x004ca1b0, 0xc)
int mouseManager::IsVis(void) { return field_0x86 == 0; }

VA(0x004ca1c0, 0x66)
void mouseManager::CheckUpdateMousePos(void) {}

VA(0x004ca230, 0x191)
void mouseManager::SetColorMice(int) {}


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
