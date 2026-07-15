// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\WINMGR.OBJ   from: .\basewin.lib
// functions: 21   data: 9
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/WINMGR.h>
#include <BASE/heroWindowManager.h>
#include <BASE/palette.h>
#include <string.h>
#include <SOURCE/wingraph.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/KB.h>
// Complete /O2 checkpoint: both code streams end at the ret at +0x3a2 and contain
// 261 instructions (retail's +0x3a3 nop is alignment outside the CodeView span).
// Relocations are exact at 71/71 with no wrong target. Keeping the combat and
// alternate copies in their source branches is the retail-evidenced structure:
// VC42 merges those two copies at +0x346, but preserves the saved-color restore at
// +0x29d and its direct jump, recovering the former missing gCyclePal+66 occurrence.
// The only remaining code residual is +0xda..+0x11c in the world-view loop: candidate
// assigns ECX to colorIndex and ESI to the destination induction pointer, while
// retail assigns ESI to colorIndex and ECX to the pointer. Renaming those two
// registers makes every instruction in the range agree; all code outside that range
// agrees after relocation masking. Earlier word/byte/RGB, signed-byte, pointer-loop,
// predicate, scope, switch, accessor, header, enum, and source-pointer/register
// variants did not recover the separate restore and need not be repeated. A semantic
// local rename and preincrement were byte-neutral, and a 60-iteration libclang AST
// pass retained no mutation. This is not a permitted artifact wall; revisit on a real
// declaration/TU-state change.
VA(0x004ca6d0, 0x3a3)
void CycleColors(int forceUpdate)
{
    signed char savedColor[WINDOW_PALETTE_COLOR_BYTES];
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
            if (frame >= WINDOW_CYCLE_REFLECTION_THRESHOLD)
                frame = WINDOW_CYCLE_FRAME_COUNT - frame;
            else
                frame = iCombatCycleFrame;
            unsigned char cycleIndices[WINDOW_WORLD_CYCLE_COLOR_COUNT] =
                { 0x98, 0x43, 0x59, 0xb5, 0x70, 0xdb, 0x87, 0x10 };
            for (int colorIndex = 0; colorIndex < WINDOW_WORLD_CYCLE_COLOR_COUNT; ++colorIndex) {
                signed char *src = gpBufferPalette->m_data +
                    (cycleIndices[colorIndex] + frame * 3) * 3;
                memcpy(gCyclePal + colorIndex * WINDOW_PALETTE_COLOR_BYTES, src,
                       WINDOW_PALETTE_COLOR_BYTES);
            }
            goto updatePalette;
        }

        memcpy(savedColor, gCyclePal + WINDOW_CYCLE_ROTATION_1_SAVE_OFFSET,
               WINDOW_PALETTE_COLOR_BYTES);
        memmove(gCyclePal + WINDOW_CYCLE_ROTATION_1_DESTINATION_OFFSET, gCyclePal,
                WINDOW_CYCLE_ROTATION_1_BYTES);
        memcpy(gCyclePal, savedColor, WINDOW_PALETTE_COLOR_BYTES);

        memcpy(savedColor, gCyclePal + WINDOW_CYCLE_ROTATION_2_SAVE_OFFSET,
               WINDOW_PALETTE_COLOR_BYTES);
        memmove(gCyclePal + WINDOW_CYCLE_ROTATION_2_DESTINATION_OFFSET,
                gCyclePal + WINDOW_CYCLE_ROTATION_2_SOURCE_OFFSET,
                WINDOW_CYCLE_ROTATION_2_BYTES);
        memcpy(gCyclePal + WINDOW_CYCLE_ROTATION_2_SOURCE_OFFSET, savedColor,
               WINDOW_PALETTE_COLOR_BYTES);

        memcpy(savedColor, gCyclePal + WINDOW_CYCLE_ROTATION_3_SAVE_OFFSET,
               WINDOW_PALETTE_COLOR_BYTES);
        memmove(gCyclePal + WINDOW_CYCLE_ROTATION_3_SAVE_OFFSET,
                gCyclePal + WINDOW_CYCLE_ROTATION_3_SOURCE_OFFSET,
                WINDOW_CYCLE_ROTATION_3_BYTES);
        memcpy(gCyclePal + WINDOW_CYCLE_ROTATION_3_RESTORE_OFFSET, savedColor,
               WINDOW_PALETTE_COLOR_BYTES);

        memcpy(savedColor, gCyclePal + WINDOW_CYCLE_ROTATION_4_SAVE_OFFSET,
               WINDOW_PALETTE_COLOR_BYTES);
        memmove(gCyclePal + WINDOW_CYCLE_ROTATION_4_DESTINATION_OFFSET,
                gCyclePal + WINDOW_CYCLE_ROTATION_4_SOURCE_OFFSET,
                WINDOW_CYCLE_ROTATION_4_BYTES);
        memcpy(gCyclePal + WINDOW_CYCLE_ROTATION_4_SOURCE_OFFSET, savedColor,
               WINDOW_PALETTE_COLOR_BYTES);

        memcpy(savedColor, gCyclePal + WINDOW_CYCLE_ROTATION_5_SAVE_OFFSET,
               WINDOW_PALETTE_COLOR_BYTES);
        memmove(gCyclePal + WINDOW_CYCLE_ROTATION_5_DESTINATION_OFFSET,
                gCyclePal + WINDOW_CYCLE_ROTATION_5_SOURCE_OFFSET,
                WINDOW_CYCLE_ROTATION_5_BYTES);
        memcpy(gCyclePal + WINDOW_CYCLE_ROTATION_5_SOURCE_OFFSET, savedColor,
               WINDOW_PALETTE_COLOR_BYTES);
    }

    if (giCycleType == WINDOW_COLOR_CYCLE_DEFAULT) {
        memcpy(savedColor, gCyclePal + WINDOW_DEFAULT_CYCLE_SAVE_OFFSET,
               WINDOW_PALETTE_COLOR_BYTES);
        memmove(gCyclePal + WINDOW_DEFAULT_CYCLE_SAVE_OFFSET,
                gCyclePal + WINDOW_DEFAULT_CYCLE_SOURCE_OFFSET,
                WINDOW_PALETTE_COLOR_BYTES);
        memcpy(gCyclePal + WINDOW_DEFAULT_CYCLE_SOURCE_OFFSET, savedColor,
               WINDOW_PALETTE_COLOR_BYTES);
        goto updatePalette;
    }

    if (giCycleType == WINDOW_COLOR_CYCLE_COMBAT) {
        iCombatCycleFrame = (iCombatCycleFrame + 1) % WINDOW_CYCLE_FRAME_COUNT;
        int frame = iCombatCycleFrame;
        if (frame >= WINDOW_CYCLE_REFLECTION_THRESHOLD)
            frame = WINDOW_CYCLE_FRAME_COUNT - frame;
        else
            frame = iCombatCycleFrame;
        memcpy(gCyclePal + WINDOW_DEFAULT_CYCLE_SOURCE_OFFSET,
               gpBufferPalette->m_data + WINDOW_COMBAT_CYCLE_SOURCE_OFFSET + frame * 12,
               WINDOW_PALETTE_COLOR_BYTES);
    } else {
        if (giCycleType != WINDOW_COLOR_CYCLE_COMBAT_ALTERNATE)
            goto updatePalette;
        int frame =
            (iCombatCycleFrame + 1) % WINDOW_ALTERNATE_CYCLE_FRAME_COUNT;
        iCombatCycleFrame = frame;
        if (frame >= WINDOW_ALTERNATE_CYCLE_REFLECTION_THRESHOLD)
            frame = WINDOW_ALTERNATE_CYCLE_FRAME_COUNT - frame;
        else
            frame = iCombatCycleFrame;
        memcpy(gCyclePal + WINDOW_DEFAULT_CYCLE_SOURCE_OFFSET,
               gpBufferPalette->m_data + WINDOW_ALTERNATE_CYCLE_SOURCE_OFFSET + frame * 21,
               WINDOW_PALETTE_COLOR_BYTES);
    }

updatePalette:
    memcpy(gpBufferPalette->m_data + WINDOW_CYCLE_PALETTE_OFFSET, gCyclePal,
           WINDOW_CYCLE_PALETTE_BYTES);
    iCycle2Count++;
    if (forceUpdate == 0) {
        iCycle3Count++;
        UpdatePalette(gpBufferPalette->m_data);
    }
}

#include <BASE/bitmap.h>
#include <BASE/Misc.h>
#include <BASE/bmap2.h>
#include <BASE/mouseManager.h>
#include <BASE/heroWindow.h>
#include <BASE/inputManager.h>
#include <BASE/resourceManager.h>
#include <stdio.h>
#include <SOURCE/kbwin.h>
#include <SOURCE/NOOPT.h>

// ---- module-private synthetic globals (retail xref: single-module) ----
DATA(0x0053496c) static unsigned int gFadeSavedUpdate; // saved update flag across a fade (heroWindowManager::FadeScreen)

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
    m_screen->m_bitmapType = BITMAP_TYPE_MEMORY;
    m_screen->m_width = WINDOW_SCREEN_WIDTH;
    m_screen->m_height = WINDOW_SCREEN_HEIGHT;
    m_screen->m_pixels = reinterpret_cast<unsigned char *>(lpInitWin);
    memset(m_screen->m_pixels, WINDOW_FRAMEBUFFER_FILL_COLOR,
           WINDOW_SCREEN_WIDTH * WINDOW_SCREEN_HEIGHT);
    m_priority = managerOrder;
    m_messageMask = 0x20;
    m_active = 1;
    strcpy(m_name, "heroWindowManager");
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

// @semantic
// Structurally complete /O2 checkpoint: both code spans are 0x35, the 0x1c message
// frame and every initialized message field agree, and there are no relocations.
// Seven unmasked bytes differ from +0x24 solely because retail loads the vtable
// before storing message.payload.widget.data.value while base performs those two
// operations in reverse; both make the same slot-2 virtual call. A 30-trial
// guarded TU-state sweep (seed 0x484f4d32) left the score unchanged. Moving the
// final assignment into the return comma expression and calling through a
// baseManager alias were byte-identical. Revisit after a genuine combined-TU
// change; this is not a proven wall.
VA(0x004cac40, 0x35)
int heroWindowManager::BroadcastMessage(int type, int p2, int p3, int p4)
{
    tag_message msg;
    msg.type = type;
    msg.payload.widget.command = p2;
    msg.payload.widget.id = p3;
    msg.payload.widget.data.value = p4;
    return Main(msg);
}

// @semantic
// Structurally complete /O2 checkpoint (live 98.97%): the Open call, ordered search,
// and every head/tail/middle link agree; 1/1 relocation agrees. Base is 0xb9 bytes
// versus retail 0xbc. The first residual is `xor ebp,ebp` versus retail
// `mov ebp,0` after the z == -1 branch; the three-byte encoding delta accounts for
// the size difference. The only other instruction diff is EAX versus ECX for the
// saved head snapshot. Nested/comma z forms and direct/cached/duplicated head forms
// were tried; reusing the zOrder parameter was byte-identical. Thirty guarded
// TU-state trials and 80 clang-AST iterations found no gain. Revisit after a
// genuine combined-TU change; this is not a proven wall.
VA(0x004cac80, 0xbc)
void heroWindowManager::AddWindow(class heroWindow *w, int zOrder, int openFlags)
{
    heroWindow *cur = m_windowListTail;
    int z;
    if ((w->m_winFlags & 1) != 0)
        z = 0;
    else
        z = zOrder;
    if (z == -1) {
        z = 0;
        if (cur != 0)
            z = cur->m_zOrder + 1;
    }
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
        heroWindow *oldHead = m_windowListHead;
        m_windowListHead = w;
        if (oldHead == 0)
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
            if (m_windowListTail == w) {
                heroWindow *prev = w->m_prevWindow;
                m_windowListTail = prev;
                prev->m_nextWindow = 0;
            } else {
                heroWindow *prev = w->m_prevWindow;
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

// Structurally complete /O2 checkpoint (live 99.19%): base is 0x1ce bytes versus
// retail 0x1cf with the exact 0x38 frame, identical ordered CFG, and 31/31 resolved
// relocations. Reordering the independent dialog-result/done stores via the clang-AST
// permuter removed the prior +0x152 residual. The sole code-shape difference is now
// immediately after FadeIn: base emits `or edi,[gFadeSavedUpdate]`, while retail
// emits `mov eax,[gFadeSavedUpdate]; or eax,edi`; both store the same flags. Direct,
// swapped, local-accumulator, and two-store forms were tried. Guarded TU-state trials
// reached disposable 99.78% candidates but rejected all that changed protected
// siblings. Revisit after a genuine TU-state change; this is not a proven wall.
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
                m_dialogResult = message.payload.widget.id;
                done = 1;
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

// @early-stop: retail is the one byte `ret` at delinked object +0xaa8, followed
// only by alignment NOPs and carrying no relocations; the delinker names the
// COMDAT-folded symbol empty_stub. This empty body emits the same `c3` byte.
VA(0x004cb1d0, 0x1)
void CreateFizzleTables(void) {}

// @semantic
// Structurally complete /O2 checkpoint (live 99.30%): declaring the three row
// cursors in saved/work/screen order makes the prefix through row-pointer setup
// byte-exact and reduces base to 0x404 bytes versus retail's 0x402. The first raw
// divergence is +0x206: base loads screenOffset from [esp+24h] before x from
// [esp+40h], while retail retains x in EDX and adds screenOffset afterward. At
// +0x219 base reloads the row end into EDX and uses EDX for the 16-bit lookup;
// retail loads the row end into EBX and uses EBX for that lookup. The row-backedge
// compare polarity and three later palette-loop SIB encodings are equivalent.
// Both retain the 0x2c frame and ordered CFG. Manual COFF review finds the same
// 33 relocation identities in the same order: offsets agree through ReadBlock at
// +0x1ab, then base's remaining 14 sites are +2 because of the pixel-block size
// delta; homm2 relocs' base-only aliases are delinker duplicate-name failures.
// The other five cursor declaration orders score 88.47-95.78%. Manager/source-row/
// source-bitmap aliases, pointer-add and palette-index operand swaps, sequential
// and split high/low lookup spellings, and loop-polarity swaps did not improve the
// retained structure. An 80-iteration libclang AST pass pinned every sibling and
// retained no mutation. This declaration state moves unchanged retained-exact
// FadeScreen/DoDialog live output to 97.25/99.19%; fresh pinned AST passes on those
// predecessors retained no mutation, so their exact-max 100 records carry this
// TU-cumulative dip. Revisit after a genuine predecessor/header TU-state change;
// this is not a proven wall.
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
                        unsigned char *savedPixel = m_fizzleSource->m_pixels +
                            m_fizzleSource->m_width * (sourceY - y);
                        unsigned char *workPixel = m_fizzleWork->m_pixels + workOffset;
                        unsigned char *screenPixel = m_screen->m_pixels +
                            x + screenOffset;
                        if (x < x + width) {
                            int remaining = width;
                            do {
                                unsigned short lookup =
                                    *workPixel++ | (*savedPixel++ << 8);
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

// @early-stop: retail is the one byte `ret` at delinked object +0xecc, followed
// only by alignment NOPs and carrying no relocations; the delinker names the
// COMDAT-folded symbol empty_stub. This empty body emits the same `c3` byte.
VA(0x004cb610, 0x1)
void CreateColorTables(void) {}

// @early-stop: retail is the one byte `ret` at delinked object +0xed0, followed
// only by alignment NOPs and carrying no relocations; the delinker names the
// COMDAT-folded symbol empty_stub. This empty body emits the same `c3` byte.
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
