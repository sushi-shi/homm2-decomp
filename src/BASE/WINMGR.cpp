// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\WINMGR.OBJ   from: .\basewin.lib
// functions: 21   data: 9
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

// These globals precede va.h to preserve the established VC4.2 BSS declaration order.
// Their fixed-width types still need the lightweight alias header here.
#include <Ints.h>

#ifdef __clang__
#define DATA(addr) __attribute__((annotate("data:" #addr)))
#else
#define DATA(addr)
#endif

DATA(0x00534908) i8 gCyclePal[0x60];
DATA(0x00534968) i16 memSelector;

static inline u32& FadeSavedUpdate(void) {
    DATA(0x0053496c) static u32 savedUpdate;
    return savedUpdate;
}

#undef DATA
#include <va.h>
#include <BASE/WINMGR.h>
#include <BASE/WINMGR_TYPES.h>

// __FILE__ for the NWC memory/assert tracking (reloc-masked path string).
#define RETAIL_FILE "I:\\Projects\\Heroes\\Prog\\BASE\\WINMGR.CPP"

DATA(0x0051ef28) i32 iCombatCycleFrame = 0;
DATA(0x0051ef2c) i32 gbEveryOtherCycle = 1;
DATA(0x0051ef30) i32 iCycle1Count = 0;
DATA(0x0051ef34) i32 iCycle2Count = 0;
DATA(0x0051ef38) i32 iCycle3Count = 0;
DATA(0x0051ef3c) i32 iDialogNestCount = 0;
DATA(0x0051ef40) static SWindowManagerText gWindowManagerText = {
    "heroWindowManager",
    "SHOT%04d.PCX",
    RETAIL_FILE,
    RETAIL_FILE,
    "CCYCLE%02d.BIN",
    RETAIL_FILE,
    RETAIL_FILE
};

#include <BASE/heroWindowManager.h>
#include <BASE/palette.h>
#include <string.h>
#include <SOURCE/wingraph.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/KB.h>

// @data-layout-note NB09 assigns WINMGR a 0xf0 initialized-data contribution
// at 0x11ef28. Six public integers form its 0x18 prefix; gWindowManagerText
// reproduces the remaining 0xd8 bytes and the seven owner addends used by Open,
// ScreenShot, and FizzleForward. Its four source paths are distinct owners.
// The loader-zero contribution at 0x134908 is gCyclePal, memSelector, two bytes
// of natural alignment, then FadeSavedUpdate's function-local static.
// The inline accessor reproduces that 0x68-byte BSS topology without a padding
// symbol or an unretained external data identity.
// @semantic
// Complete /O2 checkpoint: both code streams end at the ret at +0x3a2 and contain
// 261 instructions (retail's +0x3a3 nop is alignment outside the CodeView span).
// Relocations are exact at 71/71 with no wrong target. Keeping the combat and
// alternate copies in their source branches is the retail-evidenced structure:
// VC42 merges those two copies at +0x346, but preserves the saved-color restore at
// +0x29d and its direct jump, recovering the former missing gCyclePal+66 occurrence.
// The first code residual is +0xda..+0x11c in the world-view loop: candidate
// assigns ECX to colorIndex and ESI to the destination induction pointer, while
// retail assigns ESI to colorIndex and ECX to the pointer. The later alternate-cycle
// frame calculation also uses equivalent ECX/EDX/EAX scheduling. Earlier word/byte/RGB,
// signed-byte, pointer-loop,
// predicate, scope, switch, accessor, header, enum, and source-pointer/register
// variants did not recover the separate restore and need not be repeated. A semantic
// local rename and preincrement were byte-neutral, and a 60-iteration libclang AST
// pass retained no mutation. This is not a permitted artifact wall; revisit on a real
// declaration/TU-state change.
VA(0x004ca6d0, 0x3a3)
void CycleColors(i32 forceUpdate) {
    i8 savedColor[WINDOW_PALETTE_COLOR_BYTES];
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
            i32 frame = iCombatCycleFrame;
            if (frame >= WINDOW_CYCLE_REFLECTION_THRESHOLD)
                frame = WINDOW_CYCLE_FRAME_COUNT - frame;
            else
                frame = iCombatCycleFrame;
            u8 cycleIndices[WINDOW_WORLD_CYCLE_COLOR_COUNT] =
                {0x98, 0x43, 0x59, 0xb5, 0x70, 0xdb, 0x87, 0x10};
            for (i32 colorIndex = 0; colorIndex < WINDOW_WORLD_CYCLE_COLOR_COUNT; ++colorIndex) {
                i8* src = gpBufferPalette->m_data + (cycleIndices[colorIndex] + frame * 3) * 3;
                memcpy(
                    gCyclePal + colorIndex * WINDOW_PALETTE_COLOR_BYTES,
                    src,
                    WINDOW_PALETTE_COLOR_BYTES
                );
            }
            goto updatePalette;
        }

        memcpy(
            savedColor,
            gCyclePal + WINDOW_CYCLE_ROTATION_1_SAVE_OFFSET,
            WINDOW_PALETTE_COLOR_BYTES
        );
        memmove(
            gCyclePal + WINDOW_CYCLE_ROTATION_1_DESTINATION_OFFSET,
            gCyclePal,
            WINDOW_CYCLE_ROTATION_1_BYTES
        );
        memcpy(gCyclePal, savedColor, WINDOW_PALETTE_COLOR_BYTES);

        memcpy(
            savedColor,
            gCyclePal + WINDOW_CYCLE_ROTATION_2_SAVE_OFFSET,
            WINDOW_PALETTE_COLOR_BYTES
        );
        memmove(
            gCyclePal + WINDOW_CYCLE_ROTATION_2_DESTINATION_OFFSET,
            gCyclePal + WINDOW_CYCLE_ROTATION_2_SOURCE_OFFSET,
            WINDOW_CYCLE_ROTATION_2_BYTES
        );
        memcpy(
            gCyclePal + WINDOW_CYCLE_ROTATION_2_SOURCE_OFFSET,
            savedColor,
            WINDOW_PALETTE_COLOR_BYTES
        );

        memcpy(
            savedColor,
            gCyclePal + WINDOW_CYCLE_ROTATION_3_SAVE_OFFSET,
            WINDOW_PALETTE_COLOR_BYTES
        );
        memmove(
            gCyclePal + WINDOW_CYCLE_ROTATION_3_SAVE_OFFSET,
            gCyclePal + WINDOW_CYCLE_ROTATION_3_SOURCE_OFFSET,
            WINDOW_CYCLE_ROTATION_3_BYTES
        );
        memcpy(
            gCyclePal + WINDOW_CYCLE_ROTATION_3_RESTORE_OFFSET,
            savedColor,
            WINDOW_PALETTE_COLOR_BYTES
        );

        memcpy(
            savedColor,
            gCyclePal + WINDOW_CYCLE_ROTATION_4_SAVE_OFFSET,
            WINDOW_PALETTE_COLOR_BYTES
        );
        memmove(
            gCyclePal + WINDOW_CYCLE_ROTATION_4_DESTINATION_OFFSET,
            gCyclePal + WINDOW_CYCLE_ROTATION_4_SOURCE_OFFSET,
            WINDOW_CYCLE_ROTATION_4_BYTES
        );
        memcpy(
            gCyclePal + WINDOW_CYCLE_ROTATION_4_SOURCE_OFFSET,
            savedColor,
            WINDOW_PALETTE_COLOR_BYTES
        );

        memcpy(
            savedColor,
            gCyclePal + WINDOW_CYCLE_ROTATION_5_SAVE_OFFSET,
            WINDOW_PALETTE_COLOR_BYTES
        );
        memmove(
            gCyclePal + WINDOW_CYCLE_ROTATION_5_DESTINATION_OFFSET,
            gCyclePal + WINDOW_CYCLE_ROTATION_5_SOURCE_OFFSET,
            WINDOW_CYCLE_ROTATION_5_BYTES
        );
        memcpy(
            gCyclePal + WINDOW_CYCLE_ROTATION_5_SOURCE_OFFSET,
            savedColor,
            WINDOW_PALETTE_COLOR_BYTES
        );
    }

    if (giCycleType == WINDOW_COLOR_CYCLE_DEFAULT) {
        memcpy(
            savedColor,
            gCyclePal + WINDOW_DEFAULT_CYCLE_SAVE_OFFSET,
            WINDOW_PALETTE_COLOR_BYTES
        );
        memmove(
            gCyclePal + WINDOW_DEFAULT_CYCLE_SAVE_OFFSET,
            gCyclePal + WINDOW_DEFAULT_CYCLE_SOURCE_OFFSET,
            WINDOW_PALETTE_COLOR_BYTES
        );
        memcpy(
            gCyclePal + WINDOW_DEFAULT_CYCLE_SOURCE_OFFSET,
            savedColor,
            WINDOW_PALETTE_COLOR_BYTES
        );
        goto updatePalette;
    }

    if (giCycleType == WINDOW_COLOR_CYCLE_COMBAT) {
        iCombatCycleFrame = (iCombatCycleFrame + 1) % WINDOW_CYCLE_FRAME_COUNT;
        i32 frame = iCombatCycleFrame;
        if (frame >= WINDOW_CYCLE_REFLECTION_THRESHOLD)
            frame = WINDOW_CYCLE_FRAME_COUNT - frame;
        else
            frame = iCombatCycleFrame;
        memcpy(
            gCyclePal + WINDOW_DEFAULT_CYCLE_SOURCE_OFFSET,
            gpBufferPalette->m_data + WINDOW_COMBAT_CYCLE_SOURCE_OFFSET + frame * 12,
            WINDOW_PALETTE_COLOR_BYTES
        );
    } else {
        if (giCycleType != WINDOW_COLOR_CYCLE_COMBAT_ALTERNATE)
            goto updatePalette;
        i32 frame = (iCombatCycleFrame + 1) % WINDOW_ALTERNATE_CYCLE_FRAME_COUNT;
        iCombatCycleFrame = frame;
        if (frame >= WINDOW_ALTERNATE_CYCLE_REFLECTION_THRESHOLD)
            frame = WINDOW_ALTERNATE_CYCLE_FRAME_COUNT - frame;
        else
            frame = iCombatCycleFrame;
        memcpy(
            gCyclePal + WINDOW_DEFAULT_CYCLE_SOURCE_OFFSET,
            gpBufferPalette->m_data + WINDOW_ALTERNATE_CYCLE_SOURCE_OFFSET + frame * 21,
            WINDOW_PALETTE_COLOR_BYTES
        );
    }

updatePalette:
    memcpy(
        gpBufferPalette->m_data + WINDOW_CYCLE_PALETTE_OFFSET,
        gCyclePal,
        WINDOW_CYCLE_PALETTE_BYTES
    );
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


VA(0x004caa80, 0x41)
heroWindowManager::heroWindowManager(void) : baseManager() {
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
i32 heroWindowManager::Open(i32 managerOrder) {
    i32 i;
    InitVideo();
    i32* pal = reinterpret_cast<i32*>(gpBufferPalette->m_data);
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
    m_screen->m_pixels = reinterpret_cast<u8*>(lpInitWin);
    memset(
        m_screen->m_pixels,
        WINDOW_FRAMEBUFFER_FILL_COLOR,
        WINDOW_SCREEN_WIDTH * WINDOW_SCREEN_HEIGHT
    );
    m_priority = managerOrder;
    m_messageMask = 0x20;
    m_active = 1;
    strcpy(m_name, gWindowManagerText.managerName);
    return 0;
}

VA(0x004cabb0, 0x45)
void heroWindowManager::Close(void) {
    if (m_active == 1) {
        heroWindow* w = m_windowListTail;
        while (w != 0) {
            heroWindow* prev = w->m_prevWindow;
            RemoveWindow(w);
            w = prev;
        }
        m_screen->m_pixels = 0;
        if (m_screen != 0)
            delete m_screen;
        m_active = 0;
    }
}

// @semantic
// Complete 0x2d-byte /O2 function with the same loop, return behavior, and sole
// BroadcastMessage relocation target/addend. Candidate assigns the window cursor to
// ESI and the message reference to EDI; retail swaps those register roles throughout.
// A source-level message-reference alias was byte-neutral. Revisit after a genuine
// declaration or combined-TU state change.
VA(0x004cac00, 0x2d)
i32 heroWindowManager::Main(struct tag_message& msg) {
    i32 result = 0;
    heroWindow* w = m_windowListTail;
    while (w != 0) {
        result = w->BroadcastMessage(msg);
        if (result >= 1 && result <= 2)
            break;
        w = w->m_prevWindow;
    }
    return result;
}

VA(0x004cac30, 0xf)
i32 heroWindowManager::ConvertToHover(struct tag_message& msg) {
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
i32 heroWindowManager::BroadcastMessage(i32 type, i32 p2, i32 p3, i32 p4) {
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
void heroWindowManager::AddWindow(class heroWindow* w, i32 zOrder, i32 openFlags) {
    heroWindow* cur = m_windowListTail;
    i32 z;
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
        heroWindow* oldHead = m_windowListHead;
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
void heroWindowManager::RemoveWindow(class heroWindow* w) {
    if (w != 0) {
        w->Close();
        if (m_windowListHead == w) {
            heroWindow* next = w->m_nextWindow;
            m_windowListHead = next;
            if (next == 0)
                m_windowListTail = 0;
            else
                next->m_prevWindow = 0;
        } else {
            if (m_windowListTail == w) {
                heroWindow* prev = w->m_prevWindow;
                m_windowListTail = prev;
                prev->m_nextWindow = 0;
            } else {
                heroWindow* prev = w->m_prevWindow;
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
i32 heroWindowManager::DoDialog(
    class heroWindow* window,
    i32 (*handler)(struct tag_message&),
    i32 fade
) {
    tag_message message;
    i32 done;
    i32 result;

    gbInDialog = 1;
    if (iDialogNestCount == 0)
        SetNoDialogMenus(0);
    iDialogNestCount++;
    m_lastHoverId = HERO_WINDOW_NO_HOVER_WIDGET;
    if (window != 0)
        AddWindow(window, -1, 1);
    if (fade != 0) {
        palette* dialogPalette = gPalette;
        heroWindowManager* manager = gpWindowManager;
        if (dialogPalette != 0)
            SetPalette(dialogPalette->m_data, 0);
        i32 fadeType = WINDOW_FADE_IN;
        switch (fadeType) {
            case WINDOW_FADE_IN: {
                u32 savedUpdate = manager->m_updateFlags;
                manager->m_updateFlags = 0;
                PollSound();
                FadeIn(8);
                manager->m_updateFlags = FadeSavedUpdate() | savedUpdate;
                break;
            }
            case WINDOW_FADE_OUT:
                FadeSavedUpdate() = manager->m_updateFlags;
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
        if (window != 0 && (message.type != MESSAGE_MOUSE_MOVE || gbSendMouseMoveMessages != 0)) {
            result = window->BroadcastMessage(message);
            if (result == 2 && message.type == MESSAGE_WIDGET
                && message.payload.widget.command == WIDGET_COMMAND_DIALOG_SELECT) {
                m_dialogResult = message.payload.widget.id;
                done = 1;
            }
        }
        result = handler(message);
        if (result == 2 && message.type == MESSAGE_WIDGET
            && message.payload.widget.command == WIDGET_COMMAND_DIALOG_SELECT)
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
void heroWindowManager::UpdateScreen(void) {
    PollSound();
    BitmapToScreen(m_screen);
    PollSound();
}

VA(0x004cafc0, 0x4f)
void heroWindowManager::UpdateScreenRegion(i32 x, i32 y, i32 w, i32 h) {
    gpMouseManager->m_cursorReady = 0;
    PollSound();
    BlitBitmapToScreen(m_screen, x, y, w, h, x, y);
    gpMouseManager->m_cursorReady = 1;
    PollSound();
}

VA(0x004cb010, 0x18)
void heroWindowManager::RedrawScreen(void) {
    for (heroWindow* w = m_windowListHead; w != 0; w = w->m_nextWindow)
        w->DrawWindow();
}

VA(0x004cb030, 0x80)
void heroWindowManager::FadeScreen(i32 param_1, i32 param_2, class palette* pal) {
    if (pal != 0)
        SetPalette(pal->m_data, 0);
    switch (param_1) {
        case 0: {
            u32 saved = m_updateFlags;
            m_updateFlags = 0;
            PollSound();
            FadeIn(param_2);
            m_updateFlags = FadeSavedUpdate() | saved;
            PollSound();
            break;
        }
        case 1:
            FadeSavedUpdate() = m_updateFlags;
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
void heroWindowManager::ScreenShot(void) {
    char local_10[16];
    sprintf(local_10, gWindowManagerText.screenshotFormat, m_screenshotIndex);
    CreatePCXFile(
        local_10,
        m_screen->m_pixels,
        WINDOW_SCREEN_WIDTH,
        WINDOW_SCREEN_HEIGHT,
        reinterpret_cast<u8*>(gPalette->m_data)
    );
    m_screenshotIndex++;
    gpInputManager->Flush();
}

// @semantic
// Complete 0xc0-byte clipping/allocation/blit implementation with all five relocation
// sites and owner/addends aligned. Retail assigns width/y/height to EDI/ESI/EBX;
// candidate assigns the equivalent values to ESI/EBX/EDI, which changes only register
// names and argument push scheduling. Explicit clipped-width/clipped-height locals kept
// the score unchanged and worsened the prologue, so the direct-parameter form remains.
// Revisit after a genuine declaration or combined-TU state change.
VA(0x004cb110, 0xc0)
void heroWindowManager::SaveFizzleSource(i32 x, i32 y, i32 width, i32 height) {
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
            m_fizzleSource = new bitmap(0, static_cast<i16>(width), static_cast<i16>(height));
            BlitBitmap(gpWindowManager->m_screen, x, y, width, height, m_fizzleSource, 0, 0);
        }
    }
}

// @early-stop: retail is the named one-byte `ret` at delinked object +0xaa8,
// followed only by alignment NOPs and carrying no relocations.
VA(0x004cb1d0, 0x1)
void CreateFizzleTables(void) {}

// @semantic
// Structurally complete /O2 checkpoint (current live 92.35%): declaring the three row
// cursors in saved/work/screen order makes the prefix through row-pointer setup
// byte-exact in the earlier TU state and reduces base to 0x404 bytes versus retail's
// 0x402. In the current combined-TU state the first raw divergence occurs after clipping:
// candidate keeps the manager in ECX while retail retains it in ESI. The pixel-loop
// residual begins at +0x206: base loads screenOffset from [esp+24h] before x from
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
void heroWindowManager::FizzleForward(
    i32 x,
    i32 y,
    i32 width,
    i32 height,
    i32 delay,
    i8* startPalette,
    i8* endPalette
) {
    if (bShowIt != 0) {
        gbEnlargeScreenBlit = 0;
        i32 tick = 0;
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
            u32 savedUpdate = m_updateFlags;
            m_updateFlags = 0;
            if (delay == -1)
                delay = WINDOW_FIZZLE_DEFAULT_DELAY;
            i8* fadePalette = static_cast<i8*>(
                H2_ALLOC(WINDOW_PALETTE_BYTE_COUNT, gWindowManagerText.fadePaletteAllocSource, 808)
            );
            m_fizzleWork = new bitmap(0, static_cast<i16>(width), static_cast<i16>(height));
            i8* cycleTable = static_cast<i8*>(H2_ALLOC(
                WINDOW_FIZZLE_CYCLE_TABLE_BYTES,
                gWindowManagerText.cycleTableAllocSource,
                810
            ));
            BlitBitmap(m_screen, x, y, width, height, m_fizzleWork, 0, 0);

            for (i32 frame = 0; frame < WINDOW_CYCLE_FRAME_COUNT; frame++) {
                sprintf(gText, gWindowManagerText.cycleFilenameFormat, frame);
                u32l id = gpResourceManager->MakeId(gText, 1);
                gpResourceManager->PointToFile(id);
                gpResourceManager->ReadBlock(cycleTable, WINDOW_FIZZLE_CYCLE_TABLE_BYTES);
                i32 sourceY = y;
                if (sourceY < y + height) {
                    i32 screenOffset = y * WINDOW_SCREEN_WIDTH;
                    i32 workOffset = 0;
                    do {
                        u8* savedPixel =
                            m_fizzleSource->m_pixels + m_fizzleSource->m_width * (sourceY - y);
                        u8* workPixel = m_fizzleWork->m_pixels + workOffset;
                        u8* screenPixel = m_screen->m_pixels + x + screenOffset;
                        if (x < x + width) {
                            i32 remaining = width;
                            do {
                                u16 lookup = *workPixel++ | (*savedPixel++ << 8);
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
                    for (i32 i = 0; i < WINDOW_PALETTE_BYTE_COUNT; i++)
                        fadePalette[i] += (endPalette[i] - startPalette[i]) * (frame + 1)
                                          / WINDOW_CYCLE_FRAME_COUNT;
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
            H2_FREE(cycleTable, gWindowManagerText.cycleTableFreeSource, 897);
            H2_FREE(fadePalette, gWindowManagerText.fadePaletteFreeSource, 898);
        }
    }
}

VA(0x004cb5f0, 0x19)
void heroWindowManager::ReleaseFizzleSource(void) {
    if (m_fizzleSource != 0)
        delete m_fizzleSource;
    m_fizzleSource = 0;
}

// @early-stop: retail is the named one-byte `ret` at delinked object +0xecc,
// followed only by alignment NOPs and carrying no relocations.
VA(0x004cb610, 0x1)
void CreateColorTables(void) {}

// @early-stop: retail is the named one-byte `ret` at delinked object +0xed0,
// followed only by alignment NOPs and carrying no relocations.
VA(0x004cb620, 0x1)
void CreateColorLookupTables(void) {}

// ===== vtable heroWindowManager : public baseManager  (3 slots) =====
//  [ 0] VA(0x004caad0, 0xd6)  int heroWindowManager::Open(int)   <- override (implements baseManager pure virtual)
//  [ 1] VA(0x004cabb0, 0x45)  void heroWindowManager::Close(void)   <- override (implements baseManager pure virtual)
//  [ 2] VA(0x004cac00, 0x2d)  int heroWindowManager::Main(struct tag_message &)   <- override (implements baseManager pure virtual)

// ---- vtables (compiler-emitted; census) ----
VTBL(heroWindowManager, 0x004eba10);

#undef RETAIL_FILE
