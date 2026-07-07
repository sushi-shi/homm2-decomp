// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\WINMGR.OBJ   from: .\basewin.lib
// functions: 21   data: 9
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/heroWindowManager.h>
#include <BASE/WINMGR.h>
#include <BASE/bitmap.h>
#include <BASE/Misc.h>
#include <BASE/bmap2.h>
#include <BASE/mouseManager.h>
#include <BASE/heroWindow.h>
#include <BASE/palette.h>
#include <BASE/inputManager.h>
#include <stdio.h>
#include <string.h>
#include <SOURCE/kbwin.h>
#include <SOURCE/wingraph.h>
#include <SOURCE/X_GLOBAL.h>
#include <_globals_model.h>
#include <SOURCE/KB.h>
VA(0x004ca6d0, 0x3a3)
void CycleColors(int) {}

VA(0x004caa80, 0x41)
heroWindowManager::heroWindowManager(void) : baseManager()
{
    field_0x32 = 0;
    field_0x42 = 0;
    field_0x3e = 0;
    field_0x3a = 0;
    field_0x36 = 0;
    field_0x46 = 0;
    field_0x56 = 0;
    field_0x4a = 0;
    field_0x52 = 1;
    field_0x4e = 0;
    field_0x5e = -1;
    field_0x5a = -1;
}

VA(0x004caad0, 0xd6)
int heroWindowManager::Open(int param_1)
{
    int i;
    InitVideo();
    int *pal = reinterpret_cast<int *>(gpBufferPalette->field_0x10);
    for (i = 0xc0; i != 0; i--) {
        *pal = 0;
        pal++;
    }
    SetPalette(gpBufferPalette->field_0x10, 1);
    field_0x46 = new bitmap();
    if (field_0x46 == 0)
        MemError();
    field_0x46->field_0x10 = 0x21;
    field_0x46->field_0x12 = 0x280;
    field_0x46->field_0x14 = 0x1e0;
    field_0x46->field_0x16 = reinterpret_cast<unsigned char *>(lpInitWin);
    int *fb = reinterpret_cast<int *>(field_0x46->field_0x16);
    for (i = 0x12c00; i != 0; i--) {
        *fb = 0x24242424;
        fb++;
    }
    field_0x10 = param_1;
    field_0xc = 0x20;
    field_0x32 = 1;
    strcpy(name, "heroWindowManager");
    return 0;
}

VA(0x004cabb0, 0x45)
void heroWindowManager::Close(void)
{
    if (field_0x32 == 1) {
        heroWindow *w = field_0x3a;
        while (w != 0) {
            heroWindow *prev = w->field_0x8;
            RemoveWindow(w);
            w = prev;
        }
        field_0x46->field_0x16 = 0;
        if (field_0x46 != 0)
            delete field_0x46;
        field_0x32 = 0;
    }
}

VA(0x004cac00, 0x2d)
int heroWindowManager::Main(struct tag_message &msg)
{
    int result = 0;
    heroWindow *w = field_0x3a;
    while (w != 0 && (result = w->BroadcastMessage(msg), result < 1 || result > 2))
        w = w->field_0x8;
    return result;
}

VA(0x004cac30, 0xf)
int heroWindowManager::ConvertToHover(struct tag_message &msg)
{
    return Main(msg);
}

VA(0x004cac40, 0x35)
int heroWindowManager::BroadcastMessage(int type, int p2, int p3, int p4)
{
    tag_message msg;
    msg.type = type;
    msg.field4 = p2;
    msg.field8 = p3;
    msg.text = reinterpret_cast<char *>(p4);
    return Main(msg);
}

VA(0x004cac80, 0xbc)
void heroWindowManager::AddWindow(class heroWindow *w, int param_2, int param_3)
{
    heroWindow *cur = field_0x3a;
    int z = 0;
    if ((w->field_0x20 & 1) == 0)
        z = param_2;
    if (z == -1 && (z = 0, cur != 0))
        z = cur->field_0x0 + 1;
    if (z != 0 && field_0x36 == 0)
        return;
    if (w->Open(z, param_3) != 0)
        return;
    if (cur != 0) {
        do {
            if (cur->field_0x0 <= z)
                break;
            cur = cur->field_0x8;
        } while (cur != 0);
        if (cur != 0) {
            if (cur->field_0x4 == 0) {
                w->field_0x4 = 0;
                w->field_0x8 = field_0x3a;
                field_0x3a->field_0x4 = w;
                field_0x3a = w;
            } else {
                w->field_0x8 = cur;
                w->field_0x4 = cur->field_0x4;
                cur->field_0x4->field_0x8 = w;
                cur->field_0x4 = w;
            }
            goto done;
        }
    }
    w->field_0x8 = 0;
    w->field_0x4 = field_0x36;
    field_0x36 = w;
    if (field_0x3a == 0)
        field_0x3a = w;
done:
    field_0x42 = field_0x3e;
    field_0x3e = w;
}

VA(0x004cad40, 0x87)
void heroWindowManager::RemoveWindow(class heroWindow *w)
{
    if (w != 0) {
        w->Close();
        if (field_0x36 == w) {
            heroWindow *next = w->field_0x4;
            field_0x36 = next;
            if (next == 0)
                field_0x3a = 0;
            else
                next->field_0x8 = 0;
        } else {
            heroWindow *prev = w->field_0x8;
            if (field_0x3a == w) {
                field_0x3a = prev;
                prev->field_0x4 = 0;
            } else {
                prev->field_0x4 = w->field_0x4;
            }
            if (w->field_0x4 != 0)
                w->field_0x4->field_0x8 = w->field_0x8;
        }
        if (field_0x42 == w)
            field_0x42 = 0;
        if (field_0x42 == 0) {
            field_0x3e = field_0x3a;
            return;
        }
        field_0x3e = field_0x42;
    }
}

VA(0x004cadd0, 0x1cf)
int heroWindowManager::DoDialog(class heroWindow *, int (*)(struct tag_message &), int) { return 0; }

VA(0x004cafa0, 0x17)
void heroWindowManager::UpdateScreen(void)
{
    PollSound();
    BitmapToScreen(field_0x46);
    PollSound();
}

VA(0x004cafc0, 0x4f)
void heroWindowManager::UpdateScreenRegion(int x, int y, int w, int h)
{
    gpMouseManager->field_0x82 = 0;
    PollSound();
    BlitBitmapToScreen(field_0x46, x, y, w, h, x, y);
    gpMouseManager->field_0x82 = 1;
    PollSound();
}

VA(0x004cb010, 0x18)
void heroWindowManager::RedrawScreen(void)
{
    for (heroWindow *w = field_0x36; w != 0; w = w->field_0x4)
        w->DrawWindow();
}

VA(0x004cb030, 0x80)
void heroWindowManager::FadeScreen(int param_1, int param_2, class palette *pal)
{
    if (pal != 0)
        SetPalette(pal->field_0x10, 0);
    if (param_1 != 0) {
        if (param_1 == 1) {
            gFadeSavedUpdate = field_0x56;
            field_0x56 = 0;
            PollSound();
            FadeOut(param_2);
            PollSound();
        } else {
            unsigned int saved = field_0x56;
            field_0x56 = 0;
            PollSound();
            FadeIn(param_2);
            field_0x56 = gFadeSavedUpdate | saved;
            PollSound();
        }
    }
}

VA(0x004cb0b0, 0x53)
void heroWindowManager::ScreenShot(void)
{
    char local_10[16];
    sprintf(local_10, "SHOT%04d.PCX", field_0x52);
    CreatePCXFile(local_10, field_0x46->field_0x16, 640, 480,
                  reinterpret_cast<unsigned char *>(gPalette->field_0x10));
    field_0x52++;
    gpInputManager->Flush();
}

VA(0x004cb110, 0xc0)
void heroWindowManager::SaveFizzleSource(int param_1, int param_2, int param_3, int param_4)
{
    int origX = param_1;
    if (bShowIt != 0) {
        if (param_1 < 0) {
            param_1 = 0;
            param_3 = param_3 + origX;
        }
        if (param_2 < 0) {
            param_4 = param_4 + param_2;
            param_2 = 0;
        }
        if (640 < param_1 + param_3)
            param_3 = 640 - param_1;
        if (480 < param_2 + param_4)
            param_4 = 480 - param_2;
        if (param_3 > 0 && param_4 > 0) {
            if (field_0x4a != 0)
                delete field_0x4a;
            field_0x4a = new bitmap(0, param_3, param_4);
            BlitBitmap(gpWindowManager->field_0x46, param_1, param_2, param_3, param_4,
                       field_0x4a, 0, 0);
        }
    }
}

VA(0x004cb1d0, 0x1)
void CreateFizzleTables(void) {}

VA(0x004cb1e0, 0x402)
void heroWindowManager::FizzleForward(int, int, int, int, int, signed char *, signed char *) {}

VA(0x004cb5f0, 0x19)
void heroWindowManager::ReleaseFizzleSource(void)
{
    if (field_0x4a != 0)
        delete field_0x4a;
    field_0x4a = 0;
}

VA(0x004cb610, 0x1)
void CreateColorTables(void) {}

VA(0x004cb620, 0x1)
void CreateColorLookupTables(void) {}


// ===== vtable heroWindowManager : public baseManager  (3 slots) =====
//  [ 0] VA(0x004caad0, 0xd6)  int heroWindowManager::Open(int)   <- override (implements baseManager pure virtual)
//  [ 1] VA(0x004cabb0, 0x45)  void heroWindowManager::Close(void)   <- override (implements baseManager pure virtual)
//  [ 2] VA(0x004cac00, 0x2d)  int heroWindowManager::Main(struct tag_message &)   <- override (implements baseManager pure virtual)

// ---- vtables (compiler-emitted; census) ----
VTBL(heroWindowManager, 0x004eba10);

// ---- globals (definitions, RVA order) ----
int iCombatCycleFrame;
int gbEveryOtherCycle;
int iCycle1Count;
int iCycle2Count;
int iCycle3Count;
int iDialogNestCount;
signed char *gCyclePal;
short memSelector;
