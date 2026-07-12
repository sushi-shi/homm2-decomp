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



// ---- module-private synthetic globals (retail xref: single-module) ----
DATA(0x0053496c) static unsigned int gFadeSavedUpdate; // saved update flag across a fade (heroWindowManager::FadeScreen)

VA(0x004ca6d0, 0x3a3)
void CycleColors(int) {}

VA(0x004caa80, 0x41)
heroWindowManager::heroWindowManager(void) : baseManager()
{
    m_active = 0;
    m_activeWindow = 0;
    m_focusWindow = 0;
    m_windowListTail = 0;
    m_windowListHead = 0;
    m_screen = 0;
    m_updateFlags = 0;
    m_fizzleSource = 0;
    m_screenshotIndex = 1;
    field_0x4e = 0;
    field_0x5e = -1;
    m_dialogResult = -1;
}

VA(0x004caad0, 0xd6)
int heroWindowManager::Open(int param_1)
{
    int i;
    InitVideo();
    int *pal = reinterpret_cast<int *>(gpBufferPalette->m_data);
    for (i = 0xc0; i != 0; i--) {
        *pal = 0;
        pal++;
    }
    SetPalette(gpBufferPalette->m_data, 1);
    m_screen = new bitmap();
    if (m_screen == 0)
        MemError();
    m_screen->field_0x10 = 0x21;
    m_screen->m_width = 0x280;
    m_screen->m_height = 0x1e0;
    m_screen->m_pixels = reinterpret_cast<unsigned char *>(lpInitWin);
    int *fb = reinterpret_cast<int *>(m_screen->m_pixels);
    for (i = 0x12c00; i != 0; i--) {
        *fb = 0x24242424;
        fb++;
    }
    field_0x10 = param_1;
    field_0xc = 0x20;
    m_active = 1;
    strcpy(name, "heroWindowManager");
    return 0;
}

VA(0x004cabb0, 0x45)
void heroWindowManager::Close(void)
{
    if (m_active == 1) {
        heroWindow *w = m_windowListTail;
        while (w != 0) {
            heroWindow *prev = w->m_prevWindow;
            RemoveWindow(w);
            w = prev;
        }
        m_screen->m_pixels = 0;
        if (m_screen != 0)
            delete m_screen;
        m_active = 0;
    }
}

VA(0x004cac00, 0x2d)
int heroWindowManager::Main(struct tag_message &msg)
{
    int result = 0;
    heroWindow *w = m_windowListTail;
    while (w != 0 && (result = w->BroadcastMessage(msg), result < 1 || result > 2))
        w = w->m_prevWindow;
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
    heroWindow *cur = m_windowListTail;
    int z = 0;
    if ((w->m_winFlags & 1) == 0)
        z = param_2;
    if (z == -1 && (z = 0, cur != 0))
        z = cur->m_zOrder + 1;
    if (z != 0 && m_windowListHead == 0)
        return;
    if (w->Open(z, param_3) != 0)
        return;
    if (cur != 0) {
        do {
            if (cur->m_zOrder <= z)
                break;
            cur = cur->m_prevWindow;
        } while (cur != 0);
        if (cur != 0) {
            if (cur->m_nextWindow == 0) {
                w->m_nextWindow = 0;
                w->m_prevWindow = m_windowListTail;
                m_windowListTail->m_nextWindow = w;
                m_windowListTail = w;
            } else {
                w->m_prevWindow = cur;
                w->m_nextWindow = cur->m_nextWindow;
                cur->m_nextWindow->m_prevWindow = w;
                cur->m_nextWindow = w;
            }
            goto done;
        }
    }
    w->m_prevWindow = 0;
    w->m_nextWindow = m_windowListHead;
    m_windowListHead = w;
    if (m_windowListTail == 0)
        m_windowListTail = w;
done:
    m_activeWindow = m_focusWindow;
    m_focusWindow = w;
}

VA(0x004cad40, 0x87)
void heroWindowManager::RemoveWindow(class heroWindow *w)
{
    if (w != 0) {
        w->Close();
        if (m_windowListHead == w) {
            heroWindow *next = w->m_nextWindow;
            m_windowListHead = next;
            if (next == 0)
                m_windowListTail = 0;
            else
                next->m_prevWindow = 0;
        } else {
            heroWindow *prev = w->m_prevWindow;
            if (m_windowListTail == w) {
                m_windowListTail = prev;
                prev->m_nextWindow = 0;
            } else {
                prev->m_nextWindow = w->m_nextWindow;
            }
            if (w->m_nextWindow != 0)
                w->m_nextWindow->m_prevWindow = w->m_prevWindow;
        }
        if (m_activeWindow == w)
            m_activeWindow = 0;
        if (m_activeWindow == 0) {
            m_focusWindow = m_windowListTail;
            return;
        }
        m_focusWindow = m_activeWindow;
    }
}

VA(0x004cadd0, 0x1cf)
// int heroWindowManager::DoDialog(class heroWindow *, int (*)(struct tag_message &), int);

VA(0x004cafa0, 0x17)
void heroWindowManager::UpdateScreen(void)
{
    PollSound();
    BitmapToScreen(m_screen);
    PollSound();
}

VA(0x004cafc0, 0x4f)
void heroWindowManager::UpdateScreenRegion(int x, int y, int w, int h)
{
    gpMouseManager->m_cursorReady = 0;
    PollSound();
    BlitBitmapToScreen(m_screen, x, y, w, h, x, y);
    gpMouseManager->m_cursorReady = 1;
    PollSound();
}

VA(0x004cb010, 0x18)
void heroWindowManager::RedrawScreen(void)
{
    for (heroWindow *w = m_windowListHead; w != 0; w = w->m_nextWindow)
        w->DrawWindow();
}

VA(0x004cb030, 0x80)
void heroWindowManager::FadeScreen(int param_1, int param_2, class palette *pal)
{
    if (pal != 0)
        SetPalette(pal->m_data, 0);
    if (param_1 != 0) {
        if (param_1 == 1) {
            gFadeSavedUpdate = m_updateFlags;
            m_updateFlags = 0;
            PollSound();
            FadeOut(param_2);
            PollSound();
        } else {
            unsigned int saved = m_updateFlags;
            m_updateFlags = 0;
            PollSound();
            FadeIn(param_2);
            m_updateFlags = gFadeSavedUpdate | saved;
            PollSound();
        }
    }
}

VA(0x004cb0b0, 0x53)
void heroWindowManager::ScreenShot(void)
{
    char local_10[16];
    sprintf(local_10, "SHOT%04d.PCX", m_screenshotIndex);
    CreatePCXFile(local_10, m_screen->m_pixels, 640, 480,
                  reinterpret_cast<unsigned char *>(gPalette->m_data));
    m_screenshotIndex++;
    gpInputManager->Flush();
}

VA(0x004cb110, 0xc0)
// void heroWindowManager::SaveFizzleSource(int, int, int, int);

VA(0x004cb1d0, 0x1)
void CreateFizzleTables(void) {}

VA(0x004cb1e0, 0x402)
void heroWindowManager::FizzleForward(int, int, int, int, int, signed char *, signed char *) {}

VA(0x004cb5f0, 0x19)
void heroWindowManager::ReleaseFizzleSource(void)
{
    if (m_fizzleSource != 0)
        delete m_fizzleSource;
    m_fizzleSource = 0;
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
DATA(0x0051ef28) int iCombatCycleFrame;
DATA(0x0051ef2c) int gbEveryOtherCycle;
DATA(0x0051ef30) int iCycle1Count;
DATA(0x0051ef34) int iCycle2Count;
DATA(0x0051ef38) int iCycle3Count;
DATA(0x0051ef3c) int iDialogNestCount;
DATA(0x00534908) signed char gCyclePal[0x60];
DATA(0x00534968) short memSelector;
