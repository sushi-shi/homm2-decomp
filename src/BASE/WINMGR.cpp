#include <BASE/WINMGR.h>

H2_ENUM_BEGIN(WindowColorCycleConstant)
    CYCLE_FRAME_COUNT                    = 8,
    WORLD_CYCLE_COLOR_COUNT              = 8,
    ALTERNATE_CYCLE_FRAME_COUNT          = 6,
    CYCLE_REFLECTION_THRESHOLD           = 5,
    ALTERNATE_CYCLE_REFLECTION_THRESHOLD = 4,
    CYCLE_PALETTE_OFFSET                 = 0x282,
    CYCLE_PALETTE_BYTES                  = 84,
    COMBAT_CYCLE_FIRST_COLOR = 0x6e,
    ALTERNATE_CYCLE_FIRST_COLOR = 0x6c,
    WORLD_CYCLE_FRAME_COLOR_STEP         = 3,
    COMBAT_CYCLE_FRAME_COLORS = 4,
    ALTERNATE_CYCLE_FRAME_COLORS = 7
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

H2_ENUM_BEGIN(WindowScreenConstant)
    SCREEN_WIDTH            = 640,
    SCREEN_HEIGHT           = 480,
    FRAMEBUFFER_DWORD_COUNT = 0x12c00,
    FRAMEBUFFER_FILL_COLOR  = 0x24
H2_ENUM_END(WindowScreenConstant)

H2_ENUM_BEGIN(WindowPaletteConstant)
    PALETTE_COLOR_BYTES = 3,
    PALETTE_BYTE_COUNT = 0x300
H2_ENUM_END(WindowPaletteConstant)

H2_ENUM_BEGIN(WindowFizzleConstant)
    FIZZLE_DEFAULT_DELAY          = 150,
    FIZZLE_CYCLE_TABLE_BYTES      = 0x10000,
    FIZZLE_LOOKUP_HIGH_BYTE_SHIFT = 8,
    DIALOG_FADE_STEPS             = 8,
    SCREENSHOT_FILENAME_CAPACITY  = 16
H2_ENUM_END(WindowFizzleConstant)

#ifdef __clang__
#define DATA(addr) __attribute__((annotate("data:" #addr)))
#else
#define DATA(addr)
#endif

DATA(0x005348f4) i8 gCyclePal[WINDOW_CYCLE_PALETTE_BYTES];
i16 memSelector;

static inline u32& FadeSavedUpdate(void) {
    DATA(0x00534958) static u32 savedUpdate;
    return savedUpdate;
}

#undef DATA
#include <va.h>


DATA(0x0053495c) i32 iCombatCycleFrame = 0;
DATA(0x0051dfe4) u8 gbEveryOtherCycle = true;
DATA(0x00534960) i32 iCycle1Count = 0;
DATA(0x00534964) i32 iCycle2Count = 0;
DATA(0x00534968) i32 iCycle3Count = 0;
DATA(0x0053496c) i32 iDialogNestCount = 0;

#include <BASE/heroWindowManager.h>
#include <BASE/widget.h>
#include <BASE/palette.h>
#include <string.h>
#include <SOURCE/wingraph.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/KB.h>

VA(0x004b6b80, 0x3ff)
void CycleColors(i32 forceUpdate) {
    i8 savedColor[PALETTE_COLOR_BYTES];
    iCycle1Count++;
    if (gpWindowManager == NULL)
        return;
    if (gpBufferPalette == NULL)
        return;
    if (gpWindowManager->m_active != 1)
        return;
    if (gpWindowManager->m_updateFlags == 0 && forceUpdate == 0)
        return;

    if (giCycleType == WINDOW_COLOR_CYCLE_DEFAULT)
        gbEveryOtherCycle = true;
    else
        gbEveryOtherCycle = !gbEveryOtherCycle;

    if (gbEveryOtherCycle == 0)
        goto cycleType;

    if (giCycleType == WINDOW_COLOR_CYCLE_WORLD_VIEW) {
        iCombatCycleFrame = (iCombatCycleFrame + 1) % CYCLE_FRAME_COUNT;
        i32 cycleFrame = iCombatCycleFrame < CYCLE_REFLECTION_THRESHOLD
                             ? iCombatCycleFrame
                             : CYCLE_FRAME_COUNT - iCombatCycleFrame;
        u8 colorIndices[WORLD_CYCLE_COLOR_COUNT] = {0x98, 0x43, 0x59, 0xb5, 0x70, 0xdb, 0x87, 0x10};
        for (i32 colorIndex = 0; colorIndex < WORLD_CYCLE_COLOR_COUNT; colorIndex++)
            memcpy(
                gCyclePal + colorIndex * PALETTE_COLOR_BYTES,
                gpBufferPalette->m_data
                    + (colorIndices[colorIndex] + cycleFrame * WORLD_CYCLE_FRAME_COLOR_STEP)
                          * PALETTE_COLOR_BYTES,
                PALETTE_COLOR_BYTES
            );
    } else {
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

    cycleType:
        if (giCycleType == WINDOW_COLOR_CYCLE_DEFAULT) {
            memcpy(savedColor, gCyclePal + DEFAULT_CYCLE_SAVE_OFFSET, PALETTE_COLOR_BYTES);
            memmove(
                gCyclePal + DEFAULT_CYCLE_SAVE_OFFSET,
                gCyclePal + DEFAULT_CYCLE_SOURCE_OFFSET,
                PALETTE_COLOR_BYTES
            );
            memcpy(gCyclePal + DEFAULT_CYCLE_SOURCE_OFFSET, savedColor, PALETTE_COLOR_BYTES);
        } else if (giCycleType == WINDOW_COLOR_CYCLE_COMBAT) {
            iCombatCycleFrame = (iCombatCycleFrame + 1) % CYCLE_FRAME_COUNT;
            i32 cycleFrame = iCombatCycleFrame < CYCLE_REFLECTION_THRESHOLD
                                 ? iCombatCycleFrame
                                 : CYCLE_FRAME_COUNT - iCombatCycleFrame;
            memcpy(
                gCyclePal + DEFAULT_CYCLE_SOURCE_OFFSET,
                gpBufferPalette->m_data
                    + (COMBAT_CYCLE_FIRST_COLOR + cycleFrame * COMBAT_CYCLE_FRAME_COLORS)
                          * PALETTE_COLOR_BYTES,
                PALETTE_COLOR_BYTES
            );
        } else if (giCycleType == WINDOW_COLOR_CYCLE_COMBAT_ALTERNATE) {
            iCombatCycleFrame = (iCombatCycleFrame + 1) % ALTERNATE_CYCLE_FRAME_COUNT;
            i32 cycleFrame = iCombatCycleFrame < ALTERNATE_CYCLE_REFLECTION_THRESHOLD
                                 ? iCombatCycleFrame
                                 : ALTERNATE_CYCLE_FRAME_COUNT - iCombatCycleFrame;
            memcpy(
                gCyclePal + DEFAULT_CYCLE_SOURCE_OFFSET,
                gpBufferPalette->m_data
                    + (ALTERNATE_CYCLE_FIRST_COLOR + cycleFrame * ALTERNATE_CYCLE_FRAME_COLORS)
                          * PALETTE_COLOR_BYTES,
                PALETTE_COLOR_BYTES
            );
        }
    }

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

VA(0x004b6f80, 0x97)
heroWindowManager::heroWindowManager(void) : baseManager() {
    m_active = false;
    m_activeWindow = NULL;
    m_focusWindow = NULL;
    m_windowListTail = NULL;
    m_windowListHead = NULL;
    m_screen = NULL;
    m_screenshotIndex = 1;
    m_updateFlags = 0;
    m_fizzleSource = NULL;
    m_fizzleWork = NULL;
    m_lastHoverId = HERO_WINDOW_NO_HOVER_WIDGET;
    m_dialogResult = HERO_WINDOW_NO_DIALOG_RESULT;
}

VA(0x004b7020, 0x12e)
i32 heroWindowManager::Open(i32 managerOrder) {
    InitVideo();
    memset(gpBufferPalette->m_data, 0, PALETTE_BYTE_COUNT);
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
    m_messageMask = BASE_MANAGER_ACCEPT_RIGHT_BUTTON_DOWN;
    m_priority = managerOrder;
    m_active = true;
    strcpy(m_name, "heroWindowManager");
    return 0;
}

VA(0x004b7150, 0x91)
void heroWindowManager::Close(void) {
    if (m_active != 1)
        return;

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

VA(0x004b71f0, 0x5b)
MessageDispatchResult heroWindowManager::Main(struct tag_message& msg) {
    MessageDispatchResult ret = MESSAGE_DISPATCH_CONTINUE;
    heroWindow* w = m_windowListTail;
    while (w != NULL) {
        switch (ret = w->BroadcastMessage(msg)) {
            case MESSAGE_DISPATCH_CONTINUE:
                break;
            case MESSAGE_DISPATCH_CONSUME:
            case MESSAGE_DISPATCH_FORWARD:
                return ret;
        }
        w = w->m_prevWindow;
    }
    return ret;
}

VA(0x004b7250, 0x1c)
MessageDispatchResult heroWindowManager::ConvertToHover(struct tag_message& msg) {
    return Main(msg);
}

VA(0x004b7270, 0x36)
MessageDispatchResult
heroWindowManager::BroadcastMessage(MessageType type, BaseWidgetCommand p2, i32 p3, i32 p4) {
    tag_message msg;
    msg.type = type;
    msg.payload.widget.command = p2;
    msg.payload.widget.id = p3;
    msg.payload.widget.data.value = p4;
    return Main(msg);
}

VA(0x004b72b0, 0x142)
void heroWindowManager::AddWindow(class heroWindow* w, i32 zOrder, i32 openFlags) {
    heroWindow* cur = m_windowListTail;
    if (HAS(w->m_winFlags, WINDOW_FLAG_FIXED_LAYER))
        zOrder = 0;
    if (zOrder == -1) {
        if (cur == NULL)
            zOrder = 0;
        else
            zOrder = cur->m_zOrder + 1;
    }
    if (zOrder != 0 && m_windowListHead == NULL)
        return;
    if (w->Open(zOrder, openFlags) != 0)
        return;
    while (cur != NULL && cur->m_zOrder > zOrder)
        cur = cur->m_prevWindow;
    if (cur == NULL) {
        w->m_nextWindow = m_windowListHead;
        w->m_prevWindow = NULL;
        m_windowListHead = w;
        if (m_windowListTail == NULL)
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

VA(0x004b7400, 0xe9)
void heroWindowManager::RemoveWindow(class heroWindow* w) {
    if (w == NULL)
        return;
    w->Close();
    if (w == m_windowListHead) {
        m_windowListHead = w->m_nextWindow;
        if (m_windowListHead == NULL)
            m_windowListTail = NULL;
        else
            m_windowListHead->m_prevWindow = NULL;
    } else {
        if (w == m_windowListTail) {
            m_windowListTail = w->m_prevWindow;
            m_windowListTail->m_nextWindow = NULL;
        } else {
            if (w->m_prevWindow != NULL)
                w->m_prevWindow->m_nextWindow = w->m_nextWindow;
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

VA(0x004b74f0, 0x199)
i32 heroWindowManager::DoDialog(
    class heroWindow* window,
    MessageDispatchHandler handler,
    i32 fade
) {
    tag_message message;
    i32 done;
    MessageDispatchResult result;

    gbInDialog = true;
    if (iDialogNestCount == 0)
        SetNoDialogMenus(0);
    iDialogNestCount++;
    m_lastHoverId = HERO_WINDOW_NO_HOVER_WIDGET;
    if (window != NULL)
        AddWindow(window, -1, 1);
    if (fade != 0)
        gpWindowManager->FadeScreen(FADE_IN, DIALOG_FADE_STEPS, gPalette);
    gpInputManager->Flush();
    m_dialogResult = HERO_WINDOW_NO_DIALOG_RESULT;
    done = 0;
    while (done == 0) {
        PollSound();
        Process1WindowsMessage();
        message = gpInputManager->GetEvent();
        gpMouseManager->Main(message);
        if (window != NULL && (message.type != MESSAGE_MOUSE_MOVE || gbSendMouseMoveMessages != 0)) {
            result = window->BroadcastMessage(message);
            if (result == MESSAGE_DISPATCH_FORWARD && message.type == MESSAGE_WIDGET
                && message.payload.widget.command == WIDGET_COMMAND_DIALOG_SELECT) {
                m_dialogResult = message.payload.widget.id;
                done = 1;
            }
        }
        result = handler(message);
        if (result == MESSAGE_DISPATCH_FORWARD && message.type == MESSAGE_WIDGET
            && message.payload.widget.command == WIDGET_COMMAND_DIALOG_SELECT)
            done = 1;
    }
    if (done != 0) {
        if (window != NULL)
            RemoveWindow(window);
        gpInputManager->Flush();
    }
    gbInDialog = false;
    iDialogNestCount--;
    if (iDialogNestCount == 0)
        SetNoDialogMenus(1);
    return 0;
}

#undef MESSAGE_DISPATCH_CONTINUE
#undef MESSAGE_DISPATCH_CONSUME
#undef MESSAGE_DISPATCH_FORWARD

VA(0x004b7690, 0x20)
void heroWindowManager::UpdateScreen(void) {
    PollSound();
    BitmapToScreen(m_screen);
    PollSound();
}

VA(0x004b76b0, 0x58)
void heroWindowManager::UpdateScreenRegion(i32 x, i32 y, i32 w, i32 h) {
    gpMouseManager->m_cursorReady = 0;
    PollSound();
    BlitBitmapToScreen(m_screen, x, y, w, h, x, y);
    gpMouseManager->m_cursorReady = 1;
    PollSound();
}

VA(0x004b7710, 0x2f)
void heroWindowManager::RedrawScreen(void) {
    heroWindow* w = m_windowListHead;
    while (w != NULL) {
        w->DrawWindow();
        w = w->m_nextWindow;
    }
}

VA(0x004b7740, 0x8f)
void heroWindowManager::FadeScreen(WindowFadeMode direction, i32 steps, class palette* pal) {
    if (pal != NULL)
        SetPalette(pal->m_data, 0);
    switch (direction) {
        case FADE_IN: {
            u32 saved = m_updateFlags;
            m_updateFlags = 0;
            PollSound();
            FadeIn(steps);
            m_updateFlags = saved | FadeSavedUpdate();
            break;
        }
        case FADE_OUT:
            FadeSavedUpdate() = m_updateFlags;
            m_updateFlags = 0;
            PollSound();
            FadeOut(steps);
            break;
    }
    PollSound();
}

VA(0x004b77d0, 0x63)
void heroWindowManager::ScreenShot(void) {
    char local_10[SCREENSHOT_FILENAME_CAPACITY];
    sprintf(local_10, "SHOT%04d.PCX", m_screenshotIndex);
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

VA(0x004b7840, 0x159)
void heroWindowManager::SaveFizzleSource(i32 x, i32 y, i32 width, i32 height) {
    if (bShowIt == 0)
        return;
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
    if (width <= 0 || height <= 0)
        return;
    if (m_fizzleSource != NULL)
        delete m_fizzleSource;
    m_fizzleSource =
        new bitmap(BITMAP_TYPE_NONE, static_cast<i16>(width), static_cast<i16>(height));
    BlitBitmap(gpWindowManager->m_screen, x, y, width, height, m_fizzleSource, 0, 0);
}

VA(0x004b79a0, 0x5)
void CreateFizzleTables(void) {}

VA(0x004b79b0, 0x46d)
void heroWindowManager::FizzleForward(
    i32 x,
    i32 y,
    i32 width,
    i32 height,
    i32 delay,
    i8* startPalette,
    i8* endPalette
) {
    u8* workPixel;
    u8* screenPixel;
    i8* paletteBuf;
    u8* savePixel;
    i32l tickStart;
    i32 i;
    i32 frame;
    i32 sourceY;
    i32 sourceX;
    i8* ccycleBuf;
    i32 saveFlags;

    if (bShowIt == 0)
        return;
    gbEnlargeScreenBlit = false;
    tickStart = 0;
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
    if (width <= 0 || height <= 0)
        return;

    saveFlags = m_updateFlags;
    m_updateFlags = 0;
    if (delay == -1)
        delay = FIZZLE_DEFAULT_DELAY;
    paletteBuf = static_cast<i8*>(H2_ALLOC(PALETTE_BYTE_COUNT));
    m_fizzleWork = new bitmap(BITMAP_TYPE_NONE, static_cast<i16>(width), static_cast<i16>(height));
    ccycleBuf = static_cast<i8*>(H2_ALLOC(FIZZLE_CYCLE_TABLE_BYTES));
    BlitBitmap(m_screen, x, y, width, height, m_fizzleWork, 0, 0);

    for (frame = 0; frame < CYCLE_FRAME_COUNT; frame++) {
        sprintf(gText, "CCYCLE%02d.BIN", frame);
        gpResourceManager->PointToFile((gpResourceManager->MakeId(gText, 1)));
        gpResourceManager->ReadBlock(ccycleBuf, FIZZLE_CYCLE_TABLE_BYTES);
        for (sourceY = y; sourceY < y + height; sourceY++) {
            savePixel = m_fizzleSource->m_pixels + (sourceY - y) * m_fizzleSource->m_width;
            workPixel = m_fizzleWork->m_pixels + (sourceY - y) * width;
            screenPixel = m_screen->m_pixels + sourceY * SCREEN_WIDTH + x;
            for (sourceX = x; sourceX < x + width; sourceX++) {
                *screenPixel = ccycleBuf[static_cast<u16>(
                    *workPixel | (*savePixel << FIZZLE_LOOKUP_HIGH_BYTE_SHIFT)
                )];
                savePixel++;
                workPixel++;
                screenPixel++;
            }
        }
        PollSound();
        DelayTilMilli(tickStart + delay);
        tickStart = KBTickCount();
        BlitBitmapToScreen(m_screen, x, y, width, height, x, y);
        if (startPalette != NULL) {
            memcpy(paletteBuf, startPalette, PALETTE_BYTE_COUNT);
            for (i = 0; i < PALETTE_BYTE_COUNT; i++)
                paletteBuf[i] +=
                    (frame + 1) * (endPalette[i] - startPalette[i]) / CYCLE_FRAME_COUNT;
            UpdatePalette(paletteBuf);
        }
        PollSound();
    }
    DelayTilMilli(tickStart + delay);
    BlitBitmap(m_fizzleWork, 0, 0, width, height, m_screen, x, y);
    BlitBitmapToScreen(m_screen, x, y, width, height, x, y);
    gbEnlargeScreenBlit = true;
    m_updateFlags = saveFlags;
    delete m_fizzleSource;
    m_fizzleSource = NULL;
    delete m_fizzleWork;
    m_fizzleWork = NULL;
    H2_FREE(ccycleBuf);
    H2_FREE(paletteBuf);
}

VA(0x004b7e20, 0x4d)
void heroWindowManager::ReleaseFizzleSource(void) {
    if (m_fizzleSource != NULL)
        delete m_fizzleSource;
    m_fizzleSource = NULL;
}

VA(0x004b7e70, 0x5)
void CreateColorTables(void) {}

VA(0x004b7e80, 0x5)
void CreateColorLookupTables(void) {}

// Compiler-emitted vtables; the markers are census claims, not definitions.
VTBL(heroWindowManager, 0x004ea960)
