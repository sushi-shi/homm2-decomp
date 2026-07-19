#include <Ints.h>

H2_ENUM_BEGIN(WindowColorCycleConstant)
    CYCLE_FRAME_COUNT                    = 8,
    WORLD_CYCLE_COLOR_COUNT              = 8,
    ALTERNATE_CYCLE_FRAME_COUNT          = 6,
    CYCLE_REFLECTION_THRESHOLD           = 5,
    ALTERNATE_CYCLE_REFLECTION_THRESHOLD = 4,
    CYCLE_PALETTE_OFFSET                 = 0x282,
    CYCLE_PALETTE_BYTES                  = 84,
    COMBAT_CYCLE_SOURCE_OFFSET           = 0x14a,
    ALTERNATE_CYCLE_SOURCE_OFFSET        = 0x144
H2_ENUM_END(WindowColorCycleConstant)

H2_ENUM_BEGIN(WindowColorCyclePaletteOffset)
    CYCLE_ROTATION_1_SAVE_OFFSET        = 9,
    CYCLE_ROTATION_1_DESTINATION_OFFSET = 3,
    CYCLE_ROTATION_1_BYTES              = 9,
    CYCLE_ROTATION_2_SAVE_OFFSET        = 21,
    CYCLE_ROTATION_2_SOURCE_OFFSET      = 12,
    CYCLE_ROTATION_2_DESTINATION_OFFSET = 15,
    CYCLE_ROTATION_2_BYTES              = 9,
    CYCLE_ROTATION_3_SAVE_OFFSET        = 51,
    CYCLE_ROTATION_3_SOURCE_OFFSET      = 54,
    CYCLE_ROTATION_3_RESTORE_OFFSET     = 63,
    CYCLE_ROTATION_3_BYTES              = 12,
    CYCLE_ROTATION_4_SAVE_OFFSET        = 81,
    CYCLE_ROTATION_4_SOURCE_OFFSET      = 72,
    CYCLE_ROTATION_4_DESTINATION_OFFSET = 75,
    CYCLE_ROTATION_4_BYTES              = 9,
    CYCLE_ROTATION_5_SAVE_OFFSET        = 93,
    CYCLE_ROTATION_5_SOURCE_OFFSET      = 84,
    CYCLE_ROTATION_5_DESTINATION_OFFSET = 87,
    CYCLE_ROTATION_5_BYTES              = 9,
    DEFAULT_CYCLE_SOURCE_OFFSET         = 66,
    DEFAULT_CYCLE_SAVE_OFFSET           = 69
H2_ENUM_END(WindowColorCyclePaletteOffset)

H2_ENUM_CLASS_BEGIN(WindowFadeMode)
    FADE_IN  = 0,
    FADE_OUT = 1
H2_ENUM_CLASS_END(WindowFadeMode)

H2_ENUM_BEGIN(WindowScreenConstant)
    SCREEN_WIDTH            = 640,
    SCREEN_HEIGHT           = 480,
    FRAMEBUFFER_DWORD_COUNT = 0x12c00,
    FRAMEBUFFER_FILL_COLOR  = 0x24
H2_ENUM_END(WindowScreenConstant)

H2_ENUM_BEGIN(WindowPaletteConstant)
    PALETTE_COLOR_BYTES = 3,
    PALETTE_BYTE_COUNT  = 0x300,
    PALETTE_DWORD_COUNT = 0xc0
H2_ENUM_END(WindowPaletteConstant)

H2_ENUM_BEGIN(WindowFizzleConstant)
    FIZZLE_DEFAULT_DELAY     = 150,
    FIZZLE_CYCLE_TABLE_BYTES = 0x10000
H2_ENUM_END(WindowFizzleConstant)

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

#define RETAIL_FILE "I:\\Projects\\Heroes\\Prog\\BASE\\WINMGR.CPP"

DATA(0x0051ef28) i32 iCombatCycleFrame = 0;
DATA(0x0051ef2c) b32 gbEveryOtherCycle = true;
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

// @semantic: first code residual is +0xda..+0x11c in the world-view loop.
VA(0x004ca6d0, 0x3a3)
void CycleColors(i32 forceUpdate) {
    i8 savedColor[PALETTE_COLOR_BYTES];
    iCycle1Count++;
    if (gpWindowManager == NULL || gpBufferPalette == NULL || !gpWindowManager->m_active)
        return;
    if (gpWindowManager->m_updateFlags == 0 && forceUpdate == 0)
        return;

    if (giCycleType == WINDOW_COLOR_CYCLE_DEFAULT)
        gbEveryOtherCycle = true;
    else
        gbEveryOtherCycle = 1 - gbEveryOtherCycle;

    if (gbEveryOtherCycle != 0) {
        if (giCycleType == WINDOW_COLOR_CYCLE_WORLD_VIEW) {
            iCombatCycleFrame = (iCombatCycleFrame + 1) % CYCLE_FRAME_COUNT;
            i32 frame = iCombatCycleFrame;
            if (frame >= CYCLE_REFLECTION_THRESHOLD)
                frame = CYCLE_FRAME_COUNT - frame;
            else
                frame = iCombatCycleFrame;
            u8 cycleIndices[WORLD_CYCLE_COLOR_COUNT] =
                {0x98, 0x43, 0x59, 0xb5, 0x70, 0xdb, 0x87, 0x10};
            for (i32 colorIndex = 0; colorIndex < WORLD_CYCLE_COLOR_COUNT; ++colorIndex) {
                i8* src = gpBufferPalette->m_data + (cycleIndices[colorIndex] + frame * 3) * 3;
                memcpy(
                    gCyclePal + colorIndex * PALETTE_COLOR_BYTES,
                    src,
                    PALETTE_COLOR_BYTES
                );
            }
            goto updatePalette;
        }

        memcpy(
            savedColor,
            gCyclePal + CYCLE_ROTATION_1_SAVE_OFFSET,
            PALETTE_COLOR_BYTES
        );
        memmove(
            gCyclePal + CYCLE_ROTATION_1_DESTINATION_OFFSET,
            gCyclePal,
            CYCLE_ROTATION_1_BYTES
        );
        memcpy(gCyclePal, savedColor, PALETTE_COLOR_BYTES);

        memcpy(
            savedColor,
            gCyclePal + CYCLE_ROTATION_2_SAVE_OFFSET,
            PALETTE_COLOR_BYTES
        );
        memmove(
            gCyclePal + CYCLE_ROTATION_2_DESTINATION_OFFSET,
            gCyclePal + CYCLE_ROTATION_2_SOURCE_OFFSET,
            CYCLE_ROTATION_2_BYTES
        );
        memcpy(
            gCyclePal + CYCLE_ROTATION_2_SOURCE_OFFSET,
            savedColor,
            PALETTE_COLOR_BYTES
        );

        memcpy(
            savedColor,
            gCyclePal + CYCLE_ROTATION_3_SAVE_OFFSET,
            PALETTE_COLOR_BYTES
        );
        memmove(
            gCyclePal + CYCLE_ROTATION_3_SAVE_OFFSET,
            gCyclePal + CYCLE_ROTATION_3_SOURCE_OFFSET,
            CYCLE_ROTATION_3_BYTES
        );
        memcpy(
            gCyclePal + CYCLE_ROTATION_3_RESTORE_OFFSET,
            savedColor,
            PALETTE_COLOR_BYTES
        );

        memcpy(
            savedColor,
            gCyclePal + CYCLE_ROTATION_4_SAVE_OFFSET,
            PALETTE_COLOR_BYTES
        );
        memmove(
            gCyclePal + CYCLE_ROTATION_4_DESTINATION_OFFSET,
            gCyclePal + CYCLE_ROTATION_4_SOURCE_OFFSET,
            CYCLE_ROTATION_4_BYTES
        );
        memcpy(
            gCyclePal + CYCLE_ROTATION_4_SOURCE_OFFSET,
            savedColor,
            PALETTE_COLOR_BYTES
        );

        memcpy(
            savedColor,
            gCyclePal + CYCLE_ROTATION_5_SAVE_OFFSET,
            PALETTE_COLOR_BYTES
        );
        memmove(
            gCyclePal + CYCLE_ROTATION_5_DESTINATION_OFFSET,
            gCyclePal + CYCLE_ROTATION_5_SOURCE_OFFSET,
            CYCLE_ROTATION_5_BYTES
        );
        memcpy(
            gCyclePal + CYCLE_ROTATION_5_SOURCE_OFFSET,
            savedColor,
            PALETTE_COLOR_BYTES
        );
    }

    if (giCycleType == WINDOW_COLOR_CYCLE_DEFAULT) {
        memcpy(
            savedColor,
            gCyclePal + DEFAULT_CYCLE_SAVE_OFFSET,
            PALETTE_COLOR_BYTES
        );
        memmove(
            gCyclePal + DEFAULT_CYCLE_SAVE_OFFSET,
            gCyclePal + DEFAULT_CYCLE_SOURCE_OFFSET,
            PALETTE_COLOR_BYTES
        );
        memcpy(
            gCyclePal + DEFAULT_CYCLE_SOURCE_OFFSET,
            savedColor,
            PALETTE_COLOR_BYTES
        );
        goto updatePalette;
    }

    if (giCycleType == WINDOW_COLOR_CYCLE_COMBAT) {
        iCombatCycleFrame = (iCombatCycleFrame + 1) % CYCLE_FRAME_COUNT;
        i32 frame = iCombatCycleFrame;
        if (frame >= CYCLE_REFLECTION_THRESHOLD)
            frame = CYCLE_FRAME_COUNT - frame;
        else
            frame = iCombatCycleFrame;
        memcpy(
            gCyclePal + DEFAULT_CYCLE_SOURCE_OFFSET,
            gpBufferPalette->m_data + COMBAT_CYCLE_SOURCE_OFFSET + frame * 12,
            PALETTE_COLOR_BYTES
        );
    } else {
        if (giCycleType != WINDOW_COLOR_CYCLE_COMBAT_ALTERNATE)
            goto updatePalette;
        i32 frame = (iCombatCycleFrame + 1) % ALTERNATE_CYCLE_FRAME_COUNT;
        iCombatCycleFrame = frame;
        if (frame >= ALTERNATE_CYCLE_REFLECTION_THRESHOLD)
            frame = ALTERNATE_CYCLE_FRAME_COUNT - frame;
        else
            frame = iCombatCycleFrame;
        memcpy(
            gCyclePal + DEFAULT_CYCLE_SOURCE_OFFSET,
            gpBufferPalette->m_data + ALTERNATE_CYCLE_SOURCE_OFFSET + frame * 21,
            PALETTE_COLOR_BYTES
        );
    }

updatePalette:
    memcpy(
        gpBufferPalette->m_data + CYCLE_PALETTE_OFFSET,
        gCyclePal,
        CYCLE_PALETTE_BYTES
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
    m_active = false;
    m_activeWindow = NULL;
    m_focusWindow = NULL;
    m_windowListTail = NULL;
    m_windowListHead = NULL;
    m_screen = NULL;
    m_updateFlags = 0;
    m_fizzleSource = NULL;
    m_screenshotIndex = 1;
    m_fizzleWork = NULL;
    m_lastHoverId = HERO_WINDOW_NO_HOVER_WIDGET;
    m_dialogResult = -1;
}

VA(0x004caad0, 0xd6)
i32 heroWindowManager::Open(i32 managerOrder) {
    i32 i;
    InitVideo();
    i32* pal = reinterpret_cast<i32*>(gpBufferPalette->m_data);
    for (i = PALETTE_DWORD_COUNT; i != 0; i--) {
        *pal = 0;
        pal++;
    }
    SetPalette(gpBufferPalette->m_data, 1);
    m_screen = new bitmap();
    if (m_screen == NULL)
        MemError();
    m_screen->m_bitmapType = BITMAP_TYPE_MEMORY;
    m_screen->m_width = SCREEN_WIDTH;
    m_screen->m_height = SCREEN_HEIGHT;
    m_screen->m_pixels = reinterpret_cast<u8*>(lpInitWin);
    memset(
        m_screen->m_pixels,
        FRAMEBUFFER_FILL_COLOR,
        SCREEN_WIDTH * SCREEN_HEIGHT
    );
    m_priority = BaseManagerPriority(managerOrder);
    m_messageMask = BASE_MANAGER_ACCEPT_RIGHT_BUTTON_DOWN;
    m_active = true;
    strcpy(m_name, gWindowManagerText.managerName);
    return 0;
}

VA(0x004cabb0, 0x45)
void heroWindowManager::Close(void) {
    if (m_active) {
        heroWindow* w = m_windowListTail;
        while (w != NULL) {
            heroWindow* prev = w->m_prevWindow;
            RemoveWindow(w);
            w = prev;
        }
        m_screen->m_pixels = NULL;
        if (m_screen != NULL)
            delete m_screen;
        m_active = false;
    }
}

// Keep these expansion-only mirrors local: including widget.h perturbs unrelated VC4.2 output.
#define WIDGET_DISPATCH_CONTINUE 0
#define WIDGET_DISPATCH_CONSUME 1
#define WIDGET_DISPATCH_FORWARD 2

// @semantic: optimized register-allocation residual.
VA(0x004cac00, 0x2d)
i32 heroWindowManager::Main(struct tag_message& msg) {
    i32 result = WIDGET_DISPATCH_CONTINUE;
    heroWindow* w = m_windowListTail;
    while (w != NULL) {
        result = w->BroadcastMessage(msg);
        if (result >= WIDGET_DISPATCH_CONSUME && result <= WIDGET_DISPATCH_FORWARD)
            break;
        w = w->m_prevWindow;
    }
    return result;
}

VA(0x004cac30, 0xf)
i32 heroWindowManager::ConvertToHover(struct tag_message& msg) {
    return Main(msg);
}

// @semantic: optimized register-allocation residual.
VA(0x004cac40, 0x35)
i32 heroWindowManager::BroadcastMessage(MessageType type, BaseWidgetCommand p2, i32 p3, i32 p4) {
    tag_message msg;
    msg.type = type;
    msg.payload.widget.command = p2;
    msg.payload.widget.id = p3;
    msg.payload.widget.data.value = p4;
    return Main(msg);
}

// @semantic: first residual is xor ebp,ebp versus retail mov ebp,0 after the z == -1 branch.
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
        if (cur != NULL)
            z = cur->m_zOrder + 1;
    }
    if (z != 0 && m_windowListHead == NULL)
        return;
    if (w->Open(z, openFlags) != 0)
        return;
    if (cur != NULL) {
        do {
            if (cur->m_zOrder <= z)
                break;
            cur = cur->m_prevWindow;
        } while (cur != NULL);
    }
    if (cur == NULL) {
        w->m_nextWindow = m_windowListHead;
        w->m_prevWindow = NULL;
        heroWindow* oldHead = m_windowListHead;
        m_windowListHead = w;
        if (oldHead == NULL)
            m_windowListTail = w;
    } else if (cur->m_nextWindow == NULL) {
        w->m_prevWindow = m_windowListTail;
        w->m_nextWindow = NULL;
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
    if (w != NULL) {
        w->Close();
        if (m_windowListHead == w) {
            heroWindow* next = w->m_nextWindow;
            m_windowListHead = next;
            if (next == NULL)
                m_windowListTail = NULL;
            else
                next->m_prevWindow = NULL;
        } else {
            if (m_windowListTail == w) {
                heroWindow* prev = w->m_prevWindow;
                m_windowListTail = prev;
                prev->m_nextWindow = NULL;
            } else {
                heroWindow* prev = w->m_prevWindow;
                if (prev != NULL)
                    prev->m_nextWindow = w->m_nextWindow;
                if (w->m_nextWindow != NULL)
                    w->m_nextWindow->m_prevWindow = w->m_prevWindow;
            }
        }
        if (m_activeWindow == w)
            m_activeWindow = NULL;
        if (m_activeWindow == NULL) {
            m_focusWindow = m_windowListTail;
            return;
        }
        m_focusWindow = m_activeWindow;
    }
}

VA(0x004cadd0, 0x1cf)
i32 heroWindowManager::DoDialog(
    class heroWindow* window,
    i32 (*handler)(struct tag_message&),
    i32 fade
) {
    tag_message message;
    i32 done;
    i32 result;

    gbInDialog = true;
    if (iDialogNestCount == 0)
        SetNoDialogMenus(0);
    iDialogNestCount++;
    m_lastHoverId = HERO_WINDOW_NO_HOVER_WIDGET;
    if (window != NULL)
        AddWindow(window, -1, 1);
    if (fade != 0) {
        palette* dialogPalette = gPalette;
        heroWindowManager* manager = gpWindowManager;
        if (dialogPalette != NULL)
            SetPalette(dialogPalette->m_data, 0);
        i32 fadeType = IDX(FADE_IN);
        switch (fadeType) {
            case IDX(FADE_IN): {
                u32 savedUpdate = manager->m_updateFlags;
                manager->m_updateFlags = 0;
                PollSound();
                FadeIn(8);
                manager->m_updateFlags = FadeSavedUpdate() | savedUpdate;
                break;
            }
            case IDX(FADE_OUT):
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
        if (window != NULL && (message.type != MESSAGE_MOUSE_MOVE || gbSendMouseMoveMessages != 0)) {
            result = window->BroadcastMessage(message);
            if (result == WIDGET_DISPATCH_FORWARD && message.type == MESSAGE_WIDGET
                && message.payload.widget.command == WIDGET_COMMAND_DIALOG_SELECT) {
                m_dialogResult = message.payload.widget.id;
                done = 1;
            }
        }
        result = handler(message);
        if (result == WIDGET_DISPATCH_FORWARD && message.type == MESSAGE_WIDGET
            && message.payload.widget.command == WIDGET_COMMAND_DIALOG_SELECT)
            done = 1;
    } while (done == 0);
    if (window != NULL)
        RemoveWindow(window);
    gpInputManager->Flush();
    gbInDialog = false;
    iDialogNestCount--;
    if (iDialogNestCount == 0)
        SetNoDialogMenus(1);
    return 0;
}

#undef WIDGET_DISPATCH_CONTINUE
#undef WIDGET_DISPATCH_CONSUME
#undef WIDGET_DISPATCH_FORWARD

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
    for (heroWindow* w = m_windowListHead; w != NULL; w = w->m_nextWindow)
        w->DrawWindow();
}

VA(0x004cb030, 0x80)
void heroWindowManager::FadeScreen(i32 direction, i32 steps, class palette* pal) {
    if (pal != NULL)
        SetPalette(pal->m_data, 0);
    switch (direction) {
        case 0: {
            u32 saved = m_updateFlags;
            m_updateFlags = 0;
            PollSound();
            FadeIn(steps);
            m_updateFlags = FadeSavedUpdate() | saved;
            PollSound();
            break;
        }
        case 1:
            FadeSavedUpdate() = m_updateFlags;
            m_updateFlags = 0;
            PollSound();
            FadeOut(steps);
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
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        reinterpret_cast<u8*>(gPalette->m_data)
    );
    m_screenshotIndex++;
    gpInputManager->Flush();
}

// @semantic: compiler-shape residual.
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
        if (x + width > SCREEN_WIDTH)
            width = SCREEN_WIDTH - x;
        if (y + height > SCREEN_HEIGHT)
            height = SCREEN_HEIGHT - y;
        if (width > 0 && height > 0) {
            if (m_fizzleSource != NULL)
                delete m_fizzleSource;
            m_fizzleSource =
                new bitmap(BITMAP_TYPE_NONE, static_cast<i16>(width), static_cast<i16>(height));
            BlitBitmap(gpWindowManager->m_screen, x, y, width, height, m_fizzleSource, 0, 0);
        }
    }
}

// @early-stop: named one-byte retail stub.
VA(0x004cb1d0, 0x1)
void CreateFizzleTables(void) {}

// @semantic: first raw divergence occurs after clipping: candidate keeps the manager in ECX while retail retains it in ESI.
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
        gbEnlargeScreenBlit = false;
        i32 tick = 0;
        if (x < 0) {
            width += x;
            x = 0;
        }
        if (y < 0) {
            height += y;
            y = 0;
        }
        if (x + width > SCREEN_WIDTH)
            width = SCREEN_WIDTH - x;
        if (y + height > SCREEN_HEIGHT)
            height = SCREEN_HEIGHT - y;
        if (width > 0 && height > 0) {
            u32 savedUpdate = m_updateFlags;
            m_updateFlags = 0;
            if (delay == -1)
                delay = FIZZLE_DEFAULT_DELAY;
            i8* fadePalette = static_cast<i8*>(H2_ALLOC_AT(
                PALETTE_BYTE_COUNT,
                gWindowManagerText.fadePaletteAllocSource,
                808
            ));
            m_fizzleWork =
                new bitmap(BITMAP_TYPE_NONE, static_cast<i16>(width), static_cast<i16>(height));
            i8* cycleTable = static_cast<i8*>(H2_ALLOC_AT(
                FIZZLE_CYCLE_TABLE_BYTES,
                gWindowManagerText.cycleTableAllocSource,
                810
            ));
            BlitBitmap(m_screen, x, y, width, height, m_fizzleWork, 0, 0);

            for (i32 frame = 0; frame < CYCLE_FRAME_COUNT; frame++) {
                sprintf(gText, gWindowManagerText.cycleFilenameFormat, frame);
                u32l id = gpResourceManager->MakeId(gText, 1);
                gpResourceManager->PointToFile(id);
                gpResourceManager->ReadBlock(cycleTable, FIZZLE_CYCLE_TABLE_BYTES);
                i32 sourceY = y;
                if (sourceY < y + height) {
                    i32 screenOffset = y * SCREEN_WIDTH;
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
                        screenOffset += SCREEN_WIDTH;
                        workOffset += width;
                        sourceY++;
                    } while (sourceY < y + height);
                }
                PollSound();
                DelayTilMilli(delay + tick);
                tick = KBTickCount();
                BlitBitmapToScreen(m_screen, x, y, width, height, x, y);
                if (startPalette != NULL) {
                    memcpy(fadePalette, startPalette, PALETTE_BYTE_COUNT);
                    for (i32 i = 0; i < PALETTE_BYTE_COUNT; i++)
                        fadePalette[i] += (endPalette[i] - startPalette[i]) * (frame + 1)
                                          / CYCLE_FRAME_COUNT;
                    UpdatePalette(fadePalette);
                }
                PollSound();
            }
            DelayTilMilli(delay + tick);
            BlitBitmap(m_fizzleWork, 0, 0, width, height, m_screen, x, y);
            BlitBitmapToScreen(m_screen, x, y, width, height, x, y);
            gbEnlargeScreenBlit = true;
            m_updateFlags = savedUpdate;
            if (m_fizzleSource != NULL)
                delete m_fizzleSource;
            m_fizzleSource = NULL;
            if (m_fizzleWork != NULL)
                delete m_fizzleWork;
            m_fizzleWork = NULL;
            H2_FREE_AT(cycleTable, gWindowManagerText.cycleTableFreeSource, 897);
            H2_FREE_AT(fadePalette, gWindowManagerText.fadePaletteFreeSource, 898);
        }
    }
}

VA(0x004cb5f0, 0x19)
void heroWindowManager::ReleaseFizzleSource(void) {
    if (m_fizzleSource != NULL)
        delete m_fizzleSource;
    m_fizzleSource = NULL;
}

// @early-stop: named one-byte retail stub.
VA(0x004cb610, 0x1)
void CreateColorTables(void) {}

// @early-stop: named one-byte retail stub.
VA(0x004cb620, 0x1)
void CreateColorLookupTables(void) {}


VTBL(heroWindowManager, 0x004eba10);

#undef RETAIL_FILE
