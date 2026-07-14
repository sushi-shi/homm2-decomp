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
#include <BASE/resourceManager.h>
#include <stdio.h>
#include <string.h>
#include <SOURCE/kbwin.h>
#include <SOURCE/wingraph.h>
#include <SOURCE/X_GLOBAL.h>
#include <_globals_model.h>
#include <SOURCE/KB.h>
#include <SOURCE/NOOPT.h>



// ---- module-private synthetic globals (retail xref: single-module) ----
DATA(0x0053496c) static unsigned int gFadeSavedUpdate; // saved update flag across a fade (heroWindowManager::FadeScreen)

// @early-stop
// /O2 regalloc wall: retail 0x3a3 vs base 0x398; first residual is the 8-color loop's ecx/esi induction-register exchange, followed by relocation-name-only gCyclePal subobject aliases. All six memmove spans, three frame modulo/reflection paths, final 84-byte copy, counters, and UpdatePalette call match.
VA(0x004ca6d0, 0x3a3)
void CycleColors(int forceUpdate)
{
    unsigned char savedColor[3];
    iCycle1Count++;
    if (gpWindowManager == 0 || gpBufferPalette == 0 || gpWindowManager->m_active != 1)
        return;
    if (gpWindowManager->m_updateFlags == 0 && forceUpdate == 0)
        return;

    if (giCycleType == 0)
        gbEveryOtherCycle = 1;
    else
        gbEveryOtherCycle = 1 - gbEveryOtherCycle;

    if (gbEveryOtherCycle != 0) {
        if (giCycleType == 2) {
            iCombatCycleFrame = (iCombatCycleFrame + 1) % 8;
            int frame = iCombatCycleFrame;
            if (frame >= 4)
                frame = 8 - frame;
            else
                frame = iCombatCycleFrame;
            unsigned char cycleIndices[8] = { 0x98, 0x43, 0x59, 0xb5, 0x70, 0xdb, 0x87, 0x10 };
            for (int i = 0; i < 8; i++) {
                signed char *src = gpBufferPalette->m_data + (cycleIndices[i] + frame * 3) * 3;
                memcpy(gCyclePal + i * 3, src, 3);
            }
            goto updatePalette;
        }

        memcpy(savedColor, gCyclePal + 9, 3);
        memmove(gCyclePal + 3, gCyclePal, 9);
        memcpy(gCyclePal, savedColor, 3);

        memcpy(savedColor, gCyclePal + 21, 3);
        memmove(gCyclePal + 15, gCyclePal + 12, 9);
        memcpy(gCyclePal + 12, savedColor, 3);

        memcpy(savedColor, gCyclePal + 51, 3);
        memmove(gCyclePal + 51, gCyclePal + 54, 12);
        memcpy(gCyclePal + 63, savedColor, 3);

        memcpy(savedColor, gCyclePal + 81, 3);
        memmove(gCyclePal + 75, gCyclePal + 72, 9);
        memcpy(gCyclePal + 72, savedColor, 3);

        memcpy(savedColor, gCyclePal + 93, 3);
        memmove(gCyclePal + 87, gCyclePal + 84, 9);
        memcpy(gCyclePal + 84, savedColor, 3);
    }

    memcpy(savedColor, gCyclePal + 69, 3);
    if (giCycleType == 0) {
        memmove(gCyclePal + 69, gCyclePal + 66, 3);
        memcpy(gCyclePal + 66, savedColor, 3);
    } else {
        signed char *src;
        if (giCycleType == 1) {
            iCombatCycleFrame = (iCombatCycleFrame + 1) % 8;
            int frame = iCombatCycleFrame;
            if (frame >= 4)
                frame = 8 - frame;
            else
                frame = iCombatCycleFrame;
            src = gpBufferPalette->m_data + 0x14a + frame * 12;
        } else {
            if (giCycleType != 3)
                goto updatePalette;
            iCombatCycleFrame = (iCombatCycleFrame + 1) % 6;
            int frame = iCombatCycleFrame;
            if (frame >= 3)
                frame = 6 - frame;
            src = gpBufferPalette->m_data + 0x144 + frame * 21;
        }
        memcpy(gCyclePal + 66, src, 3);
    }

updatePalette:
    memcpy(gpBufferPalette->m_data + 0x282, gCyclePal, 84);
    iCycle2Count++;
    if (forceUpdate == 0) {
        iCycle3Count++;
        UpdatePalette(gpBufferPalette->m_data);
    }
}

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
    m_fizzleWork = 0;
    field_0x5e = -1;
    m_dialogResult = -1;
}

// @early-stop
// /O2 scheduling wall: both spans are 0xd6 and bytes through the framebuffer rep stosd match; only the final 14-instruction manager-field/string-copy block schedules the Open argument in ecx vs eax around the same three stores.
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
    while (w != 0) {
        result = w->BroadcastMessage(msg);
        if (result >= 1 && result <= 2)
            break;
        w = w->m_prevWindow;
    }
    return result;
}

VA(0x004cac30, 0xf)
int heroWindowManager::ConvertToHover(struct tag_message &msg)
{
    return Main(msg);
}

// @early-stop
// /O2 scheduling wall: both spans are 0x35; the sole residual moves the vtable load across the message.field18 store (retail load/store, base store/load), with identical stack fields and slot-2 virtual call.
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

// @early-stop
// /O2 regalloc wall: retail 0xbc vs base 0xb3; the nine bytes are two mov ebp,0 encodings vs xor ebp,ebp plus head-insertion load/store scheduling. Open call, list search, and all head/tail/middle links match.
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
    }
    if (cur == 0) {
        heroWindow *head = m_windowListHead;
        w->m_prevWindow = 0;
        w->m_nextWindow = head;
        m_windowListHead = w;
        if (head == 0)
            m_windowListTail = w;
    } else if (cur->m_nextWindow == 0) {
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
    m_activeWindow = m_focusWindow;
    m_focusWindow = w;
}

// @early-stop
// /O2 scheduling wall: retail 0x87 vs base 0x89; only cmp [this+tail],window vs mov ecx,[this+tail];cmp ecx,window differs (+2 bytes). Close call and every link/focus store match.
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
                if (prev != 0)
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

// @early-stop
// /O2 scheduling + synthetic-reloc wall: both spans are 0x1cf; residual code is one message.field8 load moved across mov ebp,1. The two gFadeSavedUpdate relocs name the same DATA(0x0053496c) address; all dialog calls/control flow match.
VA(0x004cadd0, 0x1cf)
int heroWindowManager::DoDialog(class heroWindow *window, int (*handler)(struct tag_message &),
                                int fade)
{
    tag_message message;
    int done;
    int result;

    gbInDialog = 1;
    if (iDialogNestCount == 0)
        SetNoDialogMenus(0);
    iDialogNestCount++;
    field_0x5e = -1;
    if (window != 0)
        AddWindow(window, -1, 1);
    if (fade != 0) {
        palette *dialogPalette = gPalette;
        heroWindowManager *manager = gpWindowManager;
        if (dialogPalette != 0)
            SetPalette(dialogPalette->m_data, 0);
        int fadeType = 0;
        switch (fadeType) {
        case 0: {
            unsigned int savedUpdate = manager->m_updateFlags;
            manager->m_updateFlags = 0;
            PollSound();
            FadeIn(8);
            manager->m_updateFlags = gFadeSavedUpdate | savedUpdate;
            break;
        }
        case 1:
            gFadeSavedUpdate = manager->m_updateFlags;
            manager->m_updateFlags = 0;
            PollSound();
            FadeOut(8);
            break;
        default:
            break;
        }
        PollSound();
    }
    gpInputManager->Flush();
    done = 0;
    m_dialogResult = -1;
    do {
        PollSound();
        Process1WindowsMessage();
        message = gpInputManager->GetEvent();
        gpMouseManager->Main(message);
        if (window != 0 && (message.type != 4 || gbSendMouseMoveMessages != 0)) {
            result = window->BroadcastMessage(message);
            if (result == 2 && message.type == 0x200 && message.field4 == 10) {
                int dialogResult = message.field8;
                done = 1;
                m_dialogResult = dialogResult;
            }
        }
        result = handler(message);
        if (result == 2 && message.type == 0x200 && message.field4 == 10)
            done = 1;
    } while (done == 0);
    if (window != 0)
        RemoveWindow(window);
    gpInputManager->Flush();
    gbInDialog = 0;
    iDialogNestCount--;
    if (iDialogNestCount == 0)
        SetNoDialogMenus(1);
    return 0;
}

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
    switch (param_1) {
    case 0: {
        unsigned int saved = m_updateFlags;
        m_updateFlags = 0;
        PollSound();
        FadeIn(param_2);
        m_updateFlags = gFadeSavedUpdate | saved;
        PollSound();
        break;
    }
    case 1:
        gFadeSavedUpdate = m_updateFlags;
        m_updateFlags = 0;
        PollSound();
        FadeOut(param_2);
        PollSound();
        break;
    default:
        PollSound();
        break;
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

// @early-stop
// /O2 regalloc wall: retail 0xc0 vs base 0xc6; clipping and allocation/call sequence match, while this/height color as ebp/ebx in retail vs ebx/ebp in base, perturbing six bytes of encodings and push scheduling.
VA(0x004cb110, 0xc0)
void heroWindowManager::SaveFizzleSource(int x, int y, int width, int height)
{
    if (bShowIt != 0) {
        int originalX = x;
        if (x < 0) {
            x = 0;
            width += originalX;
        }
        if (y < 0) {
            height += y;
            y = 0;
        }
        if (x + width > 640)
            width = 640 - x;
        if (y + height > 480)
            height = 480 - y;
        if (width > 0 && height > 0) {
            if (m_fizzleSource != 0)
                delete m_fizzleSource;
            m_fizzleSource = new bitmap(0, static_cast<short>(width), static_cast<short>(height));
            BlitBitmap(gpWindowManager->m_screen, x, y, width, height, m_fizzleSource, 0, 0);
        }
    }
}

VA(0x004cb1d0, 0x1)
void CreateFizzleTables(void) {}

// @early-stop
// /O2 regalloc wall: retail 0x402 vs base 0x41e; complete clipping, 8-frame resource/read/pixel/palette loops, delays, blits, deletes, and frees match. Residuals are stack/register coloring plus equivalent SIB/order in the 16-bit color-table index loop; no calls or stores are missing.
VA(0x004cb1e0, 0x402)
void heroWindowManager::FizzleForward(int x, int y, int width, int height, int delay,
                                      signed char *startPalette, signed char *endPalette)
{
    if (bShowIt != 0) {
        int originalX = x;
        int originalY = y;
        gbEnlargeScreenBlit = 0;
        int tick = 0;
        if (x < 0) {
            x = 0;
            width += originalX;
        }
        if (y < 0) {
            y = 0;
            height += originalY;
        }
        if (x + width > 640)
            width = 640 - x;
        if (y + height > 480)
            height = 480 - y;
        if (width > 0 && height > 0) {
            unsigned int savedUpdate = m_updateFlags;
            m_updateFlags = 0;
            if (delay == -1)
                delay = 150;
            signed char *fadePalette = static_cast<signed char *>(H2_ALLOC(0x300, "I:\\Projects\\Heroes\\Prog\\BASE\\WINMGR.CPP", 808));
            m_fizzleWork = new bitmap(0, static_cast<short>(width), static_cast<short>(height));
            signed char *cycleTable = static_cast<signed char *>(H2_ALLOC(0x10000, "I:\\Projects\\Heroes\\Prog\\BASE\\WINMGR.CPP", 810));
            BlitBitmap(m_screen, x, y, width, height, m_fizzleWork, 0, 0);

            for (int frame = 0; frame < 8; frame++) {
                sprintf(gText, "CCYCLE%02d.BIN", frame);
                unsigned long id = gpResourceManager->MakeId(gText, 1);
                gpResourceManager->PointToFile(id);
                gpResourceManager->ReadBlock(cycleTable, 0x10000);
                int sourceY = y;
                if (sourceY < y + height) {
                    int screenOffset = y * 640;
                    int workOffset = 0;
                    do {
                        signed char *savedPixel = reinterpret_cast<signed char *>(m_fizzleSource->m_pixels) +
                            m_fizzleSource->m_width * (sourceY - y);
                        signed char *screenPixel = reinterpret_cast<signed char *>(m_screen->m_pixels) +
                            x + screenOffset;
                        signed char *workPixel = reinterpret_cast<signed char *>(m_fizzleWork->m_pixels) + workOffset;
                        int remaining = width;
                        if (x < x + width) {
                            do {
                                unsigned short lookup = static_cast<unsigned char>(*savedPixel++);
                                lookup <<= 8;
                                lookup |= static_cast<unsigned char>(*workPixel++);
                                *screenPixel++ = cycleTable[lookup];
                                remaining--;
                            } while (remaining != 0);
                        }
                        screenOffset += 640;
                        workOffset += width;
                        sourceY++;
                    } while (sourceY < y + height);
                }
                PollSound();
                DelayTilMilli(delay + tick);
                tick = KBTickCount();
                BlitBitmapToScreen(m_screen, x, y, width, height, x, y);
                if (startPalette != 0) {
                    memcpy(fadePalette, startPalette, 0x300);
                    for (int i = 0; i < 0x300; i++)
                        fadePalette[i] += (endPalette[i] - startPalette[i]) * (frame + 1) / 8;
                    UpdatePalette(fadePalette);
                }
                PollSound();
            }
            DelayTilMilli(delay + tick);
            BlitBitmap(m_fizzleWork, 0, 0, width, height, m_screen, x, y);
            BlitBitmapToScreen(m_screen, x, y, width, height, x, y);
            gbEnlargeScreenBlit = 1;
            m_updateFlags = savedUpdate;
            if (m_fizzleSource != 0)
                delete m_fizzleSource;
            m_fizzleSource = 0;
            if (m_fizzleWork != 0)
                delete m_fizzleWork;
            m_fizzleWork = 0;
            H2_FREE(cycleTable, "I:\\Projects\\Heroes\\Prog\\BASE\\WINMGR.CPP", 897);
            H2_FREE(fadePalette, "I:\\Projects\\Heroes\\Prog\\BASE\\WINMGR.CPP", 898);
        }
    }
}

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
