#include <BASE/WINMGR.h>
#include <BASE/Utf8.h>

typedef enum WindowColorCycleConstant {
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
} WindowColorCycleConstant;

typedef enum WindowColorCyclePaletteOffset {
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
} WindowColorCyclePaletteOffset;

typedef enum WindowScreenConstant {
    FRAMEBUFFER_DWORD_COUNT = 0x12c00,
    FRAMEBUFFER_FILL_COLOR  = 0x24
} WindowScreenConstant;

typedef enum WindowFizzleConstant {
    FIZZLE_DEFAULT_DELAY          = 150,
    FIZZLE_CYCLE_TABLE_BYTES      = 0x10000,
    FIZZLE_LOOKUP_HIGH_BYTE_SHIFT = 8,
    DIALOG_FADE_STEPS             = 8,
    SCREENSHOT_FILENAME_CAPACITY  = 16
} WindowFizzleConstant;

#ifdef __clang__
#else
#endif

i8 gCyclePal[WINDOW_CYCLE_PALETTE_BYTES];
i16 memSelector;

static inline u32& FadeSavedUpdate(void) {
    static u32 savedUpdate;
    return savedUpdate;
}

#include <Ints.h>


i32 iCombatCycleFrame = 0;
u8 gbEveryOtherCycle = true;
i32 iCycle1Count = 0;
i32 iCycle2Count = 0;
i32 iCycle3Count = 0;
i32 iDialogNestCount = 0;

#include <BASE/heroWindowManager.h>
#include <BASE/widget.h>
#include <BASE/palette.h>
#include <string.h>
#include <SOURCE/wingraph.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/KB.h>

void CycleColors(i32 forceUpdate) {
    i8 savedColor[H2EnumIndex(PALETTE_CHANNEL_COUNT)];
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
                gCyclePal + colorIndex * H2EnumIndex(PALETTE_CHANNEL_COUNT),
                gpBufferPalette->m_data
                    + (colorIndices[colorIndex] + cycleFrame * WORLD_CYCLE_FRAME_COLOR_STEP)
                          * H2EnumIndex(PALETTE_CHANNEL_COUNT),
                H2EnumIndex(PALETTE_CHANNEL_COUNT)
            );
    } else {
        memcpy(
            savedColor,
            gCyclePal + CYCLE_ROTATION_1_SAVE_OFFSET,
            H2EnumIndex(PALETTE_CHANNEL_COUNT)
        );
        memmove(
            gCyclePal + CYCLE_ROTATION_1_DESTINATION_OFFSET,
            gCyclePal,
            CYCLE_ROTATION_1_BYTES
        );
        memcpy(gCyclePal, savedColor, H2EnumIndex(PALETTE_CHANNEL_COUNT));

        memcpy(
            savedColor,
            gCyclePal + CYCLE_ROTATION_2_SAVE_OFFSET,
            H2EnumIndex(PALETTE_CHANNEL_COUNT)
        );
        memmove(
            gCyclePal + CYCLE_ROTATION_2_DESTINATION_OFFSET,
            gCyclePal + CYCLE_ROTATION_2_SOURCE_OFFSET,
            CYCLE_ROTATION_2_BYTES
        );
        memcpy(
            gCyclePal + CYCLE_ROTATION_2_SOURCE_OFFSET,
            savedColor,
            H2EnumIndex(PALETTE_CHANNEL_COUNT)
        );

        memcpy(
            savedColor,
            gCyclePal + CYCLE_ROTATION_3_SAVE_OFFSET,
            H2EnumIndex(PALETTE_CHANNEL_COUNT)
        );
        memmove(
            gCyclePal + CYCLE_ROTATION_3_SAVE_OFFSET,
            gCyclePal + CYCLE_ROTATION_3_SOURCE_OFFSET,
            CYCLE_ROTATION_3_BYTES
        );
        memcpy(
            gCyclePal + CYCLE_ROTATION_3_RESTORE_OFFSET,
            savedColor,
            H2EnumIndex(PALETTE_CHANNEL_COUNT)
        );

        memcpy(
            savedColor,
            gCyclePal + CYCLE_ROTATION_4_SAVE_OFFSET,
            H2EnumIndex(PALETTE_CHANNEL_COUNT)
        );
        memmove(
            gCyclePal + CYCLE_ROTATION_4_DESTINATION_OFFSET,
            gCyclePal + CYCLE_ROTATION_4_SOURCE_OFFSET,
            CYCLE_ROTATION_4_BYTES
        );
        memcpy(
            gCyclePal + CYCLE_ROTATION_4_SOURCE_OFFSET,
            savedColor,
            H2EnumIndex(PALETTE_CHANNEL_COUNT)
        );

        memcpy(
            savedColor,
            gCyclePal + CYCLE_ROTATION_5_SAVE_OFFSET,
            H2EnumIndex(PALETTE_CHANNEL_COUNT)
        );
        memmove(
            gCyclePal + CYCLE_ROTATION_5_DESTINATION_OFFSET,
            gCyclePal + CYCLE_ROTATION_5_SOURCE_OFFSET,
            CYCLE_ROTATION_5_BYTES
        );
        memcpy(
            gCyclePal + CYCLE_ROTATION_5_SOURCE_OFFSET,
            savedColor,
            H2EnumIndex(PALETTE_CHANNEL_COUNT)
        );

    cycleType:
        if (giCycleType == WINDOW_COLOR_CYCLE_DEFAULT) {
            memcpy(savedColor, gCyclePal + DEFAULT_CYCLE_SAVE_OFFSET, H2EnumIndex(PALETTE_CHANNEL_COUNT));
            memmove(
                gCyclePal + DEFAULT_CYCLE_SAVE_OFFSET,
                gCyclePal + DEFAULT_CYCLE_SOURCE_OFFSET,
                H2EnumIndex(PALETTE_CHANNEL_COUNT)
            );
            memcpy(gCyclePal + DEFAULT_CYCLE_SOURCE_OFFSET, savedColor, H2EnumIndex(PALETTE_CHANNEL_COUNT));
        } else if (giCycleType == WINDOW_COLOR_CYCLE_COMBAT) {
            iCombatCycleFrame = (iCombatCycleFrame + 1) % CYCLE_FRAME_COUNT;
            i32 cycleFrame = iCombatCycleFrame < CYCLE_REFLECTION_THRESHOLD
                                 ? iCombatCycleFrame
                                 : CYCLE_FRAME_COUNT - iCombatCycleFrame;
            memcpy(
                gCyclePal + DEFAULT_CYCLE_SOURCE_OFFSET,
                gpBufferPalette->m_data
                    + (COMBAT_CYCLE_FIRST_COLOR + cycleFrame * COMBAT_CYCLE_FRAME_COLORS)
                          * H2EnumIndex(PALETTE_CHANNEL_COUNT),
                H2EnumIndex(PALETTE_CHANNEL_COUNT)
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
                          * H2EnumIndex(PALETTE_CHANNEL_COUNT),
                H2EnumIndex(PALETTE_CHANNEL_COUNT)
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
#include <PLATFORM/Platform.h>
#include <PLATFORM/Runtime.h>
#include <SOURCE/NOOPT.h>
#include <BASE/display.h>

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

i32 heroWindowManager::Open(i32 managerOrder) {
    platform::InitializeVideo();
    memset(gpBufferPalette->m_data, 0, PALETTE_DATA_SIZE);
    SetPalette(gpBufferPalette->m_data, 1);
    m_screen = new bitmap();
    if (m_screen == NULL)
        MemError();
    m_screen->m_bitmapType = BITMAP_TYPE_MEMORY;
    m_screen->m_width = LOGICAL_SCREEN_WIDTH;
    m_screen->m_height = LOGICAL_SCREEN_HEIGHT;
    m_screen->m_pixels = platform::Video().Pixels();
    memset(
        m_screen->m_pixels,
        FRAMEBUFFER_FILL_COLOR,
        LOGICAL_SCREEN_WIDTH * LOGICAL_SCREEN_HEIGHT
    );
    m_messageMask = BASE_MANAGER_ACCEPT_RIGHT_BUTTON_DOWN;
    m_priority = managerOrder;
    m_active = true;
    strcpy(m_name, "heroWindowManager");
    return 0;
}

void heroWindowManager::Close(void) {
    if (m_active != 1)
        return;

    heroWindow* window = m_windowListTail;
    while (window != NULL) {
        heroWindow* previous = window->m_prevWindow;
        RemoveWindow(window);
        window = previous;
    }
    m_screen->m_pixels = NULL;
    if (m_screen != NULL)
        delete m_screen;
    m_active = false;
}

MessageDispatchResult heroWindowManager::Main(struct tag_message& message) {
    MessageDispatchResult result = MESSAGE_DISPATCH_CONTINUE;
    heroWindow* window = m_windowListTail;
    while (window != NULL) {
        switch (result = window->BroadcastMessage(message)) {
            case MESSAGE_DISPATCH_CONTINUE:
                break;
            case MESSAGE_DISPATCH_CONSUME:
            case MESSAGE_DISPATCH_FORWARD:
                return result;
        }
        window = window->m_prevWindow;
    }
    return result;
}

MessageDispatchResult heroWindowManager::ConvertToHover(struct tag_message& message) {
    return Main(message);
}

MessageDispatchResult
heroWindowManager::BroadcastMessage(MessageType type, BaseWidgetCommand command, i32 widgetId, i32 value) {
    tag_message message;
    message.type = type;
    message.payload.widget.command = command;
    message.payload.widget.id = widgetId;
    message.payload.widget.data.value = value;
    return Main(message);
}

void heroWindowManager::AddWindow(class heroWindow* window, i32 zOrder, i32 openFlags) {
    heroWindow* currentWindow = m_windowListTail;
    if ((H2EnumIndex((window->m_winFlags) & (WINDOW_FLAG_FIXED_LAYER))))
        zOrder = 0;
    if (zOrder == -1) {
        if (currentWindow == NULL)
            zOrder = 0;
        else
            zOrder = currentWindow->m_zOrder + 1;
    }
    if (zOrder != 0 && m_windowListHead == NULL)
        return;
    if (window->Open(zOrder, openFlags) != 0)
        return;
    while (currentWindow != NULL && currentWindow->m_zOrder > zOrder)
        currentWindow = currentWindow->m_prevWindow;
    if (currentWindow == NULL) {
        window->m_nextWindow = m_windowListHead;
        window->m_prevWindow = NULL;
        m_windowListHead = window;
        if (m_windowListTail == NULL)
            m_windowListTail = window;
    } else if (currentWindow->m_nextWindow == NULL) {
        window->m_prevWindow = m_windowListTail;
        window->m_nextWindow = NULL;
        m_windowListTail->m_nextWindow = window;
        m_windowListTail = window;
    } else {
        window->m_prevWindow = currentWindow;
        window->m_nextWindow = currentWindow->m_nextWindow;
        currentWindow->m_nextWindow->m_prevWindow = window;
        currentWindow->m_nextWindow = window;
    }
    m_activeWindow = m_focusWindow;
    m_focusWindow = window;
}

void heroWindowManager::RemoveWindow(class heroWindow* window) {
    if (window == NULL)
        return;
    window->Close();
    if (window == m_windowListHead) {
        m_windowListHead = window->m_nextWindow;
        if (m_windowListHead == NULL)
            m_windowListTail = NULL;
        else
            m_windowListHead->m_prevWindow = NULL;
    } else {
        if (window == m_windowListTail) {
            m_windowListTail = window->m_prevWindow;
            m_windowListTail->m_nextWindow = NULL;
        } else {
            if (window->m_prevWindow != NULL)
                window->m_prevWindow->m_nextWindow = window->m_nextWindow;
            if (window->m_nextWindow != NULL)
                window->m_nextWindow->m_prevWindow = window->m_prevWindow;
        }
    }
    if (m_activeWindow == window)
        m_activeWindow = NULL;
    if (m_activeWindow == NULL) {
        m_focusWindow = m_windowListTail;
        return;
    }
    m_focusWindow = m_activeWindow;
}

i32 heroWindowManager::DoDialog(
    class heroWindow* window,
    MessageDispatchHandler handler,
    i32 fade
) {
    tag_message message;
    b32 done;
    MessageDispatchResult result;

    gbInDialog = true;
    if (iDialogNestCount == 0)
        platform::SetDialogMenusEnabled(false);
    iDialogNestCount++;
    m_lastHoverId = HERO_WINDOW_NO_HOVER_WIDGET;
    if (window != NULL)
        AddWindow(window, -1, 1);
    if (fade != 0)
        gpWindowManager->FadeScreen(FADE_IN, DIALOG_FADE_STEPS, gPalette);
    gpInputManager->Flush();
    m_dialogResult = HERO_WINDOW_NO_DIALOG_RESULT;
    done = false;
    while (done == 0) {
        PollSound();
        platform::PumpEvents();
        message = gpInputManager->GetEvent();
        gpMouseManager->Main(message);
        if (window != NULL && (message.type != MESSAGE_MOUSE_MOVE || gbSendMouseMoveMessages != 0)) {
            result = window->BroadcastMessage(message);
            if (result == MESSAGE_DISPATCH_FORWARD && message.type == MESSAGE_WIDGET
                && message.payload.widget.command == WIDGET_COMMAND_DIALOG_SELECT) {
                m_dialogResult = message.payload.widget.id;
                done = true;
            }
        }
        result = handler(message);
        if (result == MESSAGE_DISPATCH_FORWARD && message.type == MESSAGE_WIDGET
            && message.payload.widget.command == WIDGET_COMMAND_DIALOG_SELECT)
            done = true;
    }
    if (done != 0) {
        if (window != NULL)
            RemoveWindow(window);
        gpInputManager->Flush();
    }
    gbInDialog = false;
    iDialogNestCount--;
    if (iDialogNestCount == 0)
        platform::SetDialogMenusEnabled(true);
    return 0;
}

#undef MESSAGE_DISPATCH_CONTINUE
#undef MESSAGE_DISPATCH_CONSUME
#undef MESSAGE_DISPATCH_FORWARD

void heroWindowManager::UpdateScreen(void) {
    PollSound();
    BitmapToScreen(m_screen);
    PollSound();
}

void heroWindowManager::UpdateScreenRegion(i32 x, i32 y, i32 width, i32 height) {
    gpMouseManager->m_cursorReady = false;
    PollSound();
    BlitBitmapToScreen(m_screen, x, y, width, height, x, y);
    gpMouseManager->m_cursorReady = true;
    PollSound();
}

void heroWindowManager::RedrawScreen(void) {
    heroWindow* window = m_windowListHead;
    while (window != NULL) {
        window->DrawWindow();
        window = window->m_nextWindow;
    }
}

void heroWindowManager::FadeScreen(WindowFadeMode direction, i32 steps, class palette* currentPalette) {
    if (currentPalette != NULL)
        SetPalette(currentPalette->m_data, 0);
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

void heroWindowManager::ScreenShot(void) {
    char filename[SCREENSHOT_FILENAME_CAPACITY];
    utf8::Format(filename, "SHOT%04d.PCX", m_screenshotIndex);
    CreatePCXFile(
        filename,
        m_screen->m_pixels,
        LOGICAL_SCREEN_WIDTH,
        LOGICAL_SCREEN_HEIGHT,
        reinterpret_cast<u8*>(gPalette->m_data)
    );
    m_screenshotIndex++;
    gpInputManager->Flush();
}

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
    if (x + width > LOGICAL_SCREEN_WIDTH)
        width = LOGICAL_SCREEN_WIDTH - x;
    if (y + height > LOGICAL_SCREEN_HEIGHT)
        height = LOGICAL_SCREEN_HEIGHT - y;
    if (width <= 0 || height <= 0)
        return;
    if (m_fizzleSource != NULL)
        delete m_fizzleSource;
    m_fizzleSource =
        new bitmap(BITMAP_TYPE_NONE, static_cast<i16>(width), static_cast<i16>(height));
    BlitBitmap(gpWindowManager->m_screen, x, y, width, height, m_fizzleSource, 0, 0);
}

void CreateFizzleTables(void) {}

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
    i8* paletteBuffer;
    u8* savePixel;
    i32l tickStart;
    i32 i;
    i32 frame;
    i32 sourceY;
    i32 sourceX;
    i8* colorCycleBuffer;
    i32 saveFlags;

    if (bShowIt == 0)
        return;
    gbEnlargeScreenBlit = false;
    tickStart = platform::Ticks();
    if (x < 0) {
        width += x;
        x = 0;
    }
    if (y < 0) {
        height += y;
        y = 0;
    }
    if (x + width > LOGICAL_SCREEN_WIDTH)
        width = LOGICAL_SCREEN_WIDTH - x;
    if (y + height > LOGICAL_SCREEN_HEIGHT)
        height = LOGICAL_SCREEN_HEIGHT - y;
    if (width <= 0 || height <= 0)
        return;

    saveFlags = m_updateFlags;
    m_updateFlags = 0;
    if (delay == -1)
        delay = FIZZLE_DEFAULT_DELAY;
    paletteBuffer = static_cast<i8*>(H2_ALLOC(PALETTE_DATA_SIZE));
    m_fizzleWork = new bitmap(BITMAP_TYPE_NONE, static_cast<i16>(width), static_cast<i16>(height));
    colorCycleBuffer = static_cast<i8*>(H2_ALLOC(FIZZLE_CYCLE_TABLE_BYTES));
    BlitBitmap(m_screen, x, y, width, height, m_fizzleWork, 0, 0);

    for (frame = 0; frame < CYCLE_FRAME_COUNT; frame++) {
        utf8::Format(gText, GLOBAL_TEXT_BUFFER_SIZE, "CCYCLE%02d.BIN", frame);
        gpResourceManager->PointToFile((gpResourceManager->MakeId(gText, 1)));
        gpResourceManager->ReadBlock(colorCycleBuffer, FIZZLE_CYCLE_TABLE_BYTES);
        for (sourceY = y; sourceY < y + height; sourceY++) {
            savePixel = m_fizzleSource->m_pixels + (sourceY - y) * m_fizzleSource->m_width;
            workPixel = m_fizzleWork->m_pixels + (sourceY - y) * width;
            screenPixel = m_screen->m_pixels + sourceY * LOGICAL_SCREEN_WIDTH + x;
            for (sourceX = x; sourceX < x + width; sourceX++) {
                *screenPixel = colorCycleBuffer[static_cast<u16>(
                    *workPixel | (*savePixel << FIZZLE_LOOKUP_HIGH_BYTE_SHIFT)
                )];
                savePixel++;
                workPixel++;
                screenPixel++;
            }
        }
        PollSound();
        DelayTilMilli(tickStart + delay);
        tickStart = platform::Ticks();
        BlitBitmapToScreen(m_screen, x, y, width, height, x, y);
        if (startPalette != NULL) {
            memcpy(paletteBuffer, startPalette, PALETTE_DATA_SIZE);
            for (i = 0; i < PALETTE_DATA_SIZE; i++)
                paletteBuffer[i] +=
                    (frame + 1) * (endPalette[i] - startPalette[i]) / CYCLE_FRAME_COUNT;
            UpdatePalette(paletteBuffer);
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
    H2_FREE(colorCycleBuffer);
    H2_FREE(paletteBuffer);
}

void heroWindowManager::ReleaseFizzleSource(void) {
    if (m_fizzleSource != NULL)
        delete m_fizzleSource;
    m_fizzleSource = NULL;
}

void CreateColorTables(void) {}

void CreateColorLookupTables(void) {}
