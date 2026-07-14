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

// @match-note
// Structurally complete /O2 checkpoint (combined live and retained 94.85%): base is 0x391 bytes/255
// instructions versus retail's 0x3a3/261, with the same 0xc frame, 33 blocks and
// 23 branches. The default-only color rotation and both reflected combat frame
// paths are now explicit. One CFG edge still differs because VC42 tail-merges the
// default path's final three-byte copy into the non-default copy; retail exits the
// default path directly. Relocations are 70/71 with no wrong/extra target (the
// missing occurrence is an iCombatCycleFrame reload). Of 580 comparable unmasked
// bytes, 125 differ from +0x1e. Direct word/byte copies added a wrong gCyclePal
// relocation; a direct-global frame form added a wrong iCombatCycleFrame
// occurrence; a forced `| 0` reload was byte-identical. Do not repeat the source
// variants in docs/matching-matrices/winmgr-structural-6148770.tsv. Revisit only
// after a real predecessor/header TU-state change; this is not a proven wall.
VA(0x004ca6d0, 0x3a3)
void CycleColors(int forceUpdate)
{
    unsigned char savedColor[3];
    iCycle1Count++;
    if (gpWindowManager == 0 || gpBufferPalette == 0 || gpWindowManager->m_active != 1)
        return;
    if (gpWindowManager->m_updateFlags == 0 && forceUpdate == 0)
        return;

    if (giCycleType == WINDOW_COLOR_CYCLE_DEFAULT)
        gbEveryOtherCycle = 1;
    else
        gbEveryOtherCycle = 1 - gbEveryOtherCycle;

    if (gbEveryOtherCycle != 0) {
        if (giCycleType == WINDOW_COLOR_CYCLE_WORLD_VIEW) {
            iCombatCycleFrame = (iCombatCycleFrame + 1) % WINDOW_CYCLE_FRAME_COUNT;
            int frame = iCombatCycleFrame;
            if (frame >= 4)
                frame = WINDOW_CYCLE_FRAME_COUNT - frame;
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

    if (giCycleType == WINDOW_COLOR_CYCLE_DEFAULT) {
        memcpy(savedColor, gCyclePal + 69, WINDOW_PALETTE_COLOR_BYTES);
        memmove(gCyclePal + 69, gCyclePal + 66, WINDOW_PALETTE_COLOR_BYTES);
        memcpy(gCyclePal + 66, savedColor, WINDOW_PALETTE_COLOR_BYTES);
        goto updatePalette;
    }

    signed char *src;
    if (giCycleType == WINDOW_COLOR_CYCLE_COMBAT) {
        iCombatCycleFrame = (iCombatCycleFrame + 1) % WINDOW_CYCLE_FRAME_COUNT;
        int frame = iCombatCycleFrame;
        if (frame > 4)
            frame = WINDOW_CYCLE_FRAME_COUNT - frame;
        else
            frame = iCombatCycleFrame;
        src = gpBufferPalette->m_data + WINDOW_COMBAT_CYCLE_SOURCE_OFFSET + frame * 12;
    } else {
        if (giCycleType != WINDOW_COLOR_CYCLE_COMBAT_ALTERNATE)
            goto updatePalette;
        int frame =
            (iCombatCycleFrame + 1) % WINDOW_ALTERNATE_CYCLE_FRAME_COUNT;
        iCombatCycleFrame = frame;
        if (frame > 3)
            frame = WINDOW_ALTERNATE_CYCLE_FRAME_COUNT - frame;
        else
            frame = iCombatCycleFrame;
        src = gpBufferPalette->m_data + WINDOW_ALTERNATE_CYCLE_SOURCE_OFFSET + frame * 21;
    }
    memcpy(gCyclePal + 66, src, WINDOW_PALETTE_COLOR_BYTES);

updatePalette:
    memcpy(gpBufferPalette->m_data + WINDOW_CYCLE_PALETTE_OFFSET, gCyclePal,
           WINDOW_CYCLE_PALETTE_BYTES);
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
    m_lastHoverId = HERO_WINDOW_NO_HOVER_WIDGET;
    m_dialogResult = -1;
}

// @match-note
// Structurally complete /O2 checkpoint: both code spans are 0xd6, there is no
// local frame, and all 9 relocations agree. Bytes through the framebuffer rep
// stosd match; the final manager-field/string-copy block leaves 24 unmasked bytes
// from +0x95 because retail schedules the Open argument in ECX rather than EAX
// around the same three stores. An explicit open-mode lifetime emitted no change.
VA(0x004caad0, 0xd6)
int heroWindowManager::Open(int managerOrder)
{
    int i;
    InitVideo();
    int *pal = reinterpret_cast<int *>(gpBufferPalette->m_data);
    for (i = WINDOW_PALETTE_DWORD_COUNT; i != 0; i--) {
        *pal = 0;
        pal++;
    }
    SetPalette(gpBufferPalette->m_data, 1);
    m_screen = new bitmap();
    if (m_screen == 0)
        MemError();
    m_screen->field_0x10 = 0x21;
    m_screen->m_width = WINDOW_SCREEN_WIDTH;
    m_screen->m_height = WINDOW_SCREEN_HEIGHT;
    m_screen->m_pixels = reinterpret_cast<unsigned char *>(lpInitWin);
    int *fb = reinterpret_cast<int *>(m_screen->m_pixels);
    for (i = WINDOW_FRAMEBUFFER_DWORD_COUNT; i != 0; i--) {
        *fb = 0x24242424;
        fb++;
    }
    field_0x10 = managerOrder;
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

// @match-note
// Structurally complete /O2 checkpoint: both code spans are 0x35, the 0x1c message
// frame and every initialized message field agree, and there are no relocations.
// Seven unmasked bytes differ from +0x24 solely because retail loads the vtable
// before storing message.payload.widget.data.value while base performs those two
// operations in reverse;
// both make the same slot-2 virtual call. Revisit after a new combined TU state.
VA(0x004cac40, 0x35)
int heroWindowManager::BroadcastMessage(int type, int p2, int p3, int p4)
{
    tag_message msg;
    msg.type = type;
    msg.payload.widget.command = p2;
    msg.payload.widget.id = p3;
    msg.payload.widget.data.text = reinterpret_cast<char *>(p4);
    return Main(msg);
}

// @match-note
// Structurally complete /O2 checkpoint: retail link-assignment order and an
// explicit second head snapshot raised this from 92.16% to 97.35%. The Open call,
// ordered search and all head/tail/middle links now agree; 1/1 relocation agrees.
// Base is 0xb6 bytes versus retail 0xbc. The remaining semantic diff is two
// `xor ebp,ebp` encodings versus `mov ebp,0`, plus EAX/ECX selection for the head
// snapshot; raw branch displacements make 149 bytes differ from +0x0d. Cached-head,
// cached-tail and alternate assignment-order variants are recorded in the matrix.
VA(0x004cac80, 0xbc)
void heroWindowManager::AddWindow(class heroWindow *w, int zOrder, int openFlags)
{
    heroWindow *cur = m_windowListTail;
    int z = 0;
    if ((w->m_winFlags & 1) == 0)
        z = zOrder;
    if (z == -1 && (z = 0, cur != 0))
        z = cur->m_zOrder + 1;
    if (z != 0 && m_windowListHead == 0)
        return;
    if (w->Open(z, openFlags) != 0)
        return;
    if (cur != 0) {
        do {
            if (cur->m_zOrder <= z)
                break;
            cur = cur->m_prevWindow;
        } while (cur != 0);
    }
    if (cur == 0) {
        w->m_nextWindow = m_windowListHead;
        w->m_prevWindow = 0;
        heroWindow *head = m_windowListHead;
        m_windowListHead = w;
        if (head == 0)
            m_windowListTail = w;
    } else if (cur->m_nextWindow == 0) {
        w->m_prevWindow = m_windowListTail;
        w->m_nextWindow = 0;
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

// @match-note
// Structurally complete /O2 checkpoint (93.88%): moving next-link repair into the
// non-tail arm recovered retail semantics. Base is 0x89 bytes/50 instructions and
// retail is 0x87/49; both have the same 17-block, 11-branch ordered CFG, no local
// frame, and the single relocation agrees. Retail compares [this+tail] directly
// while base first loads it into ECX, leaving 81 unmasked positional bytes different
// from +0xb. Branch-local previous-pointer lifetimes regressed to 90.10%; retain the
// shared lifetime and revisit only after TU state changes. This is not a proven wall.
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
                if (w->m_nextWindow != 0)
                    w->m_nextWindow->m_prevWindow = w->m_prevWindow;
            }
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

// @match-note
// Structurally complete /O2 checkpoint (combined live and retained 99.11%): both code spans are 0x1cf with
// 135 instructions, the exact 0x38 frame, identical 31-block/20-branch ordered CFG,
// and 31/31 relocations with matching resolved targets. Nine unmasked bytes differ
// at +0x152 because the message.payload.widget.id load crosses `mov ebp,1`. The two
// displayed gFadeSavedUpdate names resolve to DATA(0x0053496c), but the scheduling
// residual is real. Revisit after a genuine TU-state change; this is not an artifact
// wall and does not justify repeating local predicate synonyms.
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
    m_lastHoverId = HERO_WINDOW_NO_HOVER_WIDGET;
    if (window != 0)
        AddWindow(window, -1, 1);
    if (fade != 0) {
        palette *dialogPalette = gPalette;
        heroWindowManager *manager = gpWindowManager;
        if (dialogPalette != 0)
            SetPalette(dialogPalette->m_data, 0);
        int fadeType = WINDOW_FADE_IN;
        switch (fadeType) {
        case WINDOW_FADE_IN: {
            unsigned int savedUpdate = manager->m_updateFlags;
            manager->m_updateFlags = 0;
            PollSound();
            FadeIn(8);
            manager->m_updateFlags = gFadeSavedUpdate | savedUpdate;
            break;
        }
        case WINDOW_FADE_OUT:
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
            if (result == 2 && message.type == MESSAGE_WIDGET &&
                message.payload.widget.command == WIDGET_COMMAND_DIALOG_SELECT) {
                int dialogResult = message.payload.widget.id;
                done = 1;
                m_dialogResult = dialogResult;
            }
        }
        result = handler(message);
        if (result == 2 && message.type == MESSAGE_WIDGET &&
            message.payload.widget.command == WIDGET_COMMAND_DIALOG_SELECT)
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
    CreatePCXFile(local_10, m_screen->m_pixels, WINDOW_SCREEN_WIDTH,
                  WINDOW_SCREEN_HEIGHT,
                  reinterpret_cast<unsigned char *>(gPalette->m_data));
    m_screenshotIndex++;
    gpInputManager->Flush();
}

VA(0x004cb110, 0xc0)
void heroWindowManager::SaveFizzleSource(int x, int y, int width, int height)
{
    if (bShowIt != 0) {
        if (x < 0) {
            width += x;
            x = 0;
        }
        if (y < 0) {
            height += y;
            y = 0;
        }
        if (x + width > WINDOW_SCREEN_WIDTH)
            width = WINDOW_SCREEN_WIDTH - x;
        if (y + height > WINDOW_SCREEN_HEIGHT)
            height = WINDOW_SCREEN_HEIGHT - y;
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

// @match-note
// Structurally complete /O2 checkpoint (combined live 91.38%; retained 93.51%): direct clipping removed the
// decompiler-only coordinate snapshots, and declaring `remaining` inside the
// non-empty row recovered retail's missing pixel-loop block. Base is 0x418 bytes/
// 311 instructions versus retail's 0x402/304; both now have the same 34-block,
// 19-branch ordered CFG, 0x2c frame, and 33/33 matching relocations. The eight
// resource/read/pixel/palette passes, delays, blits, deletes and frees agree; both
// H2_ALLOC/H2_FREE sites carry the retail WINMGR.CPP filename and line numbers.
// Of 815 comparable unmasked bytes, 555 differ from +0x18 through register/stack
// coloring and equivalent 16-bit lookup SIB order. Revisit after a genuine
// predecessor/header TU-state change; this is not a proven wall.
VA(0x004cb1e0, 0x402)
void heroWindowManager::FizzleForward(int x, int y, int width, int height, int delay,
                                      signed char *startPalette, signed char *endPalette)
{
    if (bShowIt != 0) {
        gbEnlargeScreenBlit = 0;
        int tick = 0;
        if (x < 0) {
            width += x;
            x = 0;
        }
        if (y < 0) {
            height += y;
            y = 0;
        }
        if (x + width > WINDOW_SCREEN_WIDTH)
            width = WINDOW_SCREEN_WIDTH - x;
        if (y + height > WINDOW_SCREEN_HEIGHT)
            height = WINDOW_SCREEN_HEIGHT - y;
        if (width > 0 && height > 0) {
            unsigned int savedUpdate = m_updateFlags;
            m_updateFlags = 0;
            if (delay == -1)
                delay = WINDOW_FIZZLE_DEFAULT_DELAY;
            signed char *fadePalette = static_cast<signed char *>(H2_ALLOC(WINDOW_PALETTE_BYTE_COUNT, "I:\\Projects\\Heroes\\Prog\\BASE\\WINMGR.CPP", 808));
            m_fizzleWork = new bitmap(0, static_cast<short>(width), static_cast<short>(height));
            signed char *cycleTable = static_cast<signed char *>(H2_ALLOC(WINDOW_FIZZLE_CYCLE_TABLE_BYTES, "I:\\Projects\\Heroes\\Prog\\BASE\\WINMGR.CPP", 810));
            BlitBitmap(m_screen, x, y, width, height, m_fizzleWork, 0, 0);

            for (int frame = 0; frame < WINDOW_CYCLE_FRAME_COUNT; frame++) {
                sprintf(gText, "CCYCLE%02d.BIN", frame);
                unsigned long id = gpResourceManager->MakeId(gText, 1);
                gpResourceManager->PointToFile(id);
                gpResourceManager->ReadBlock(cycleTable, WINDOW_FIZZLE_CYCLE_TABLE_BYTES);
                int sourceY = y;
                if (sourceY < y + height) {
                    int screenOffset = y * WINDOW_SCREEN_WIDTH;
                    int workOffset = 0;
                    do {
                        signed char *savedPixel = reinterpret_cast<signed char *>(m_fizzleSource->m_pixels) +
                            m_fizzleSource->m_width * (sourceY - y);
                        signed char *screenPixel = reinterpret_cast<signed char *>(m_screen->m_pixels) +
                            x + screenOffset;
                        signed char *workPixel = reinterpret_cast<signed char *>(m_fizzleWork->m_pixels) + workOffset;
                        if (x < x + width) {
                            int remaining = width;
                            do {
                                unsigned short lookup = static_cast<unsigned char>(*savedPixel++);
                                lookup <<= 8;
                                lookup |= static_cast<unsigned char>(*workPixel++);
                                *screenPixel++ = cycleTable[lookup];
                                remaining--;
                            } while (remaining != 0);
                        }
                        screenOffset += WINDOW_SCREEN_WIDTH;
                        workOffset += width;
                        sourceY++;
                    } while (sourceY < y + height);
                }
                PollSound();
                DelayTilMilli(delay + tick);
                tick = KBTickCount();
                BlitBitmapToScreen(m_screen, x, y, width, height, x, y);
                if (startPalette != 0) {
                    memcpy(fadePalette, startPalette, WINDOW_PALETTE_BYTE_COUNT);
                    for (int i = 0; i < WINDOW_PALETTE_BYTE_COUNT; i++)
                        fadePalette[i] +=
                            (endPalette[i] - startPalette[i]) * (frame + 1) /
                            WINDOW_CYCLE_FRAME_COUNT;
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
