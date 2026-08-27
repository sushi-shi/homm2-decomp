#include <va.h>
#include <stdio.h>
#include <string.h>
#include <BASE/font.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/iconWidget.h>
#include <BASE/soundManager.h>
#include <BASE/textWidget.h>
#include <SOURCE/KB.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/advManager.h>
#include <SOURCE/kbwin.h>
#include <SOURCE/x_arena.h>

H2_ENUM_BEGIN(ArenaConstant)
    CHOICE_COUNT            = 3,
    WINDOW_RESOURCE         = 5,
    WINDOW_WIDTH            = 306,
    WINDOW_BASE_HEIGHT      = 180,
    WINDOW_ROW_HEIGHT       = 45,
    WINDOW_X                = 159,
    WINDOW_SCREEN_HEIGHT    = 480,
    WINDOW_MAX_Y            = 28,
    WINDOW_CENTER_DIVISOR   = 2,
    WINDOW_NAME_SIZE        = 16,
    TEXT_WIDTH              = 244,
    TEXT_LINE_HEIGHT        = 16,
    TEXT_LINE_SHIFT         = 4,
    ICON_FIRST_X            = 84,
    TEXT_FIRST_X            = 79,
    WIDGET_X_STEP           = 60,
    ICON_Y                  = 244,
    ICON_WIDTH              = 39,
    ICON_HEIGHT             = 34,
    TEXT_Y                  = 282,
    TEXT_WIDTH_PIXELS       = 49,
    TEXT_HEIGHT             = 24,
    WIDGET_FIRST_ID         = 100,
    WIDGET_LAST_ID          = 102,
    SELECTED_FRAME_OFFSET   = 4,
    TEXT_BACKGROUND         = -1,
    CYCLE_KEY               = 15,
    BROADCAST_TEXT_ID       = 1,
    BROADCAST_CONTROL_VALUE = 6,
    DRAW_MODE               = 1
H2_ENUM_END(ArenaConstant)

VA(0x004b2ba0, 0x3bb)
i32 DoArenaDialog(void) {
    i32 H2_UNUSED(statValue0);
    i32 H2_UNUSED(statValue2);
    i32 H2_UNUSED(statValue3);
    i32 H2_UNUSED(statValue7);
    i32 windowLines2 = WINDOW_RESOURCE;
    i16 H2_UNUSED(widgetMode10) = 1;
    i32 H2_UNUSED(windowWidth) = WINDOW_WIDTH;
    i32 windowHeight16 = windowLines2 * WINDOW_ROW_HEIGHT + WINDOW_BASE_HEIGHT;
    i32 windowX4 = WINDOW_X;
    i32 windowY3 = (WINDOW_SCREEN_HEIGHT - windowHeight16) / WINDOW_CENTER_DIVISOR;
    char windowName7[WINDOW_NAME_SIZE];
    i32 lineCount11;
    i32 H2_UNUSED(textHeight4);
    tag_message message14;
    i32 widgetIndex1;
    textWidget* statWidgets0[CHOICE_COUNT];

    if (windowY3 > WINDOW_MAX_Y)
        windowY3 = WINDOW_MAX_Y;
    choice = 0;
    sprintf(windowName7, "evntwin%d.bin", windowLines2);
    arenaWinPtr = new heroWindow(windowX4, windowY3, windowName7);
    if (arenaWinPtr == NULL)
        MemError();

    strcpy(
        gText,
        /* Вы вошли на арену и сразились со сворой свирепых львов. Впечатленный вашим мастерством
           тренер гладиаторов согласился научить вас одному навыку на ваш выбор. */
        "\xc2\xfb \xe2\xee\xf8\xeb\xe8 \xed\xe0 \xe0\xf0\xe5\xed\xf3 \xe8 \xf1\xf0\xe0\xe7\xe8\xeb\xe8\xf1\xfc \xf1\xee \xf1\xe2\xee\xf0\xee\xe9 \xf1\xe2\xe8\xf0\xe5\xef\xfb\xf5 \xeb\xfc\xe2\xee\xe2. "
            "\xc2\xef\xe5\xf7\xe0\xf2\xeb\xe5\xed\xed\xfb\xe9 \xe2\xe0\xf8\xe8\xec \xec\xe0\xf1\xf2\xe5\xf0\xf1\xf2\xe2\xee\xec \xf2\xf0\xe5\xed\xe5\xf0 \xe3\xeb\xe0\xe4\xe8\xe0\xf2\xee\xf0\xee\xe2 "
            "\xf1\xee\xe3\xeb\xe0\xf1\xe8\xeb\xf1\xff \xed\xe0\xf3\xf7\xe8\xf2\xfc \xe2\xe0\xf1 \xee\xe4\xed\xee\xec\xf3 \xed\xe0\xe2\xfb\xea\xf3 \xed\xe0 \xe2\xe0\xf8 \xe2\xfb\xe1\xee\xf0."
    );
    lineCount11 = bigFont->LineLength(gText, TEXT_WIDTH);
    textHeight4 = lineCount11 << TEXT_LINE_SHIFT;
    message14.type = MESSAGE_WIDGET;
    message14.payload.widget.command = ARENA_BROADCAST_TEXT;
    message14.payload.widget.id = BROADCAST_TEXT_ID;
    message14.payload.widget.data.text = gText;
    arenaWinPtr->BroadcastMessage(message14);

    for (widgetIndex1 = 0; widgetIndex1 < CHOICE_COUNT; widgetIndex1++) {
        skillWidget[widgetIndex1] = new iconWidget(
            widgetIndex1 * WIDGET_X_STEP + ICON_FIRST_X,
            ICON_Y,
            ICON_WIDTH,
            ICON_HEIGHT,
            "xprimary.icn",
            widgetIndex1 == choice ? widgetIndex1 + SELECTED_FRAME_OFFSET
                                     : widgetIndex1,
            ICON_DRAW_NORMAL,
            widgetIndex1 + WIDGET_FIRST_ID,
            WIDGET_KIND_ICON_DIRECT,
            1
        );
        if (skillWidget[widgetIndex1] == NULL)
            MemError();

        statWidgets0[widgetIndex1] = new textWidget(
            widgetIndex1 * WIDGET_X_STEP + TEXT_FIRST_X,
            TEXT_Y,
            TEXT_WIDTH_PIXELS,
            TEXT_HEIGHT,
            // Retail passes static text into a widget that assumes ownership.
            const_cast<char*>(gStatNames[widgetIndex1]),
            "smalfont.fnt",
            FONT_DRAW_DEFAULT,
            TEXT_BACKGROUND,
            WIDGET_KIND_TEXT,
            FONT_ALIGN_CENTER
        );
        if (statWidgets0[widgetIndex1] == NULL)
            MemError();
        arenaWinPtr->AddWidget(skillWidget[widgetIndex1], -1);
        arenaWinPtr->AddWidget(statWidgets0[widgetIndex1], -1);
    }

    message14.type = MESSAGE_WIDGET;
    message14.payload.widget.command = ARENA_BROADCAST_CONTROL;
    message14.payload.widget.data.value = BROADCAST_CONTROL_VALUE;
    message14.payload.widget.id = EVENT_WINDOW_SEVENTH_BUTTON;
    arenaWinPtr->BroadcastMessage(message14);
    message14.payload.widget.id = EVENT_WINDOW_EIGHTH_BUTTON;
    arenaWinPtr->BroadcastMessage(message14);
    message14.payload.widget.id = EVENT_WINDOW_SECOND_BUTTON;
    arenaWinPtr->BroadcastMessage(message14);
    message14.payload.widget.id = EVENT_WINDOW_FIFTH_BUTTON;
    arenaWinPtr->BroadcastMessage(message14);
    message14.payload.widget.id = EVENT_WINDOW_SIXTH_BUTTON;
    arenaWinPtr->BroadcastMessage(message14);

    gpWindowManager->DoDialog(arenaWinPtr, ArenaWindowHandler, 0);
    delete arenaWinPtr;
    return choice;
}

VA(0x004b2f5b, 0x1e1)
MessageDispatchResult ArenaWindowHandler(struct tag_message& message_1) {
    tag_message H2_UNUSED(dialogMessage_12);
    i32 H2_UNUSED(widgetIndex_5);
    i32 H2_UNUSED(type_13);
    i32 H2_UNUSED(extra_18);

    if (!gpSoundManager->MusicPlaying() && gpAdvManager->m_active == 1)
        gpSoundManager->SwitchAmbientMusic(
            giTerrainToMusicTrack[IDX(gpAdvManager->m_currentTerrain)]
        );
    if (giDialogTimeout != 0 && KBTickCount() > giDialogTimeout) {
        message_1.type = MESSAGE_WIDGET;
        gpWindowManager->m_dialogResult = message_1.payload.widget.id;
        message_1.payload.widget.id = EVENT_WINDOW_CLOSE_COMMAND;
        message_1.payload.widget.command = WIDGET_COMMAND_DIALOG_SELECT;
        giDialogTimeout = 0;
        return MESSAGE_DISPATCH_FORWARD;
    }

    if (message_1.type == MESSAGE_KEY_DOWN) {
        if (message_1.payload.keyboard.keyCode == CYCLE_KEY) {
            choice++;
            if (choice >= CHOICE_COUNT)
                choice = 0;
            UpdateArenaIcons();
        }
    } else if (message_1.type == MESSAGE_WIDGET) {
        switch (message_1.payload.widget.command) {
            case WIDGET_COMMAND_SELECT:
            case WIDGET_COMMAND_ALTERNATE_SELECT:
                extra_18 = NORMAL_DIALOG_NO_VALUE;
                type_13 = NORMAL_DIALOG_NO_RESOURCE;
                if (message_1.payload.widget.parameter & EVENT_WINDOW_RESOURCE_FLAG) {
                    switch (message_1.payload.widget.id) {
                        case WIDGET_FIRST_ID:
                        case WIDGET_FIRST_ID + 1:
                        case WIDGET_LAST_ID:
                            choice = message_1.payload.widget.id - WIDGET_FIRST_ID;
                            NormalDialog(
                                gStatDesc[choice],
                                NORMAL_DIALOG_QUICK_VIEW,
                                NORMAL_DIALOG_NO_RESOURCE,
                                NORMAL_DIALOG_NO_VALUE,
                                NORMAL_DIALOG_NO_RESOURCE,
                                0,
                                NORMAL_DIALOG_NO_RESOURCE,
                                0,
                                NORMAL_DIALOG_NO_RESOURCE,
                                0
                            );
                            break;
                    }
                }
                break;

            case WIDGET_COMMAND_DESELECT:
                switch (message_1.payload.widget.id) {
                    case WIDGET_FIRST_ID:
                    case WIDGET_FIRST_ID + 1:
                    case WIDGET_LAST_ID:
                        choice = message_1.payload.widget.id - WIDGET_FIRST_ID;
                        UpdateArenaIcons();
                        break;
                    case EVENT_WINDOW_THIRD_BUTTON:
                        gpWindowManager->m_dialogResult = message_1.payload.widget.id;
                        message_1.payload.widget.id = EVENT_WINDOW_CLOSE_COMMAND;
                        message_1.payload.widget.command = WIDGET_COMMAND_DIALOG_SELECT;
                        giDialogTimeout = 0;
                        return MESSAGE_DISPATCH_FORWARD;
                    default:
                        break;
                }
                break;
            default:
                break;
        }
    }
    return MESSAGE_DISPATCH_CONSUME;
}

VA(0x004b313c, 0x169)
void UpdateArenaIcons(void) {
    i32 widgetIndex;

    for (widgetIndex = 0; widgetIndex < CHOICE_COUNT; widgetIndex++) {
        arenaWinPtr->RemoveWidget(skillWidget[widgetIndex]);
        delete skillWidget[widgetIndex];
        skillWidget[widgetIndex] = NULL;
        skillWidget[widgetIndex] = new iconWidget(
            widgetIndex * WIDGET_X_STEP + ICON_FIRST_X,
            ICON_Y,
            ICON_WIDTH,
            ICON_HEIGHT,
            "xprimary.icn",
            widgetIndex == choice ? widgetIndex + SELECTED_FRAME_OFFSET : widgetIndex,
            ICON_DRAW_NORMAL,
            widgetIndex + WIDGET_FIRST_ID,
            WIDGET_KIND_ICON_DIRECT,
            1
        );
        if (skillWidget[widgetIndex] == NULL)
            MemError();
        arenaWinPtr->AddWidget(skillWidget[widgetIndex], -1);
    }
    arenaWinPtr->DrawWindow(DRAW_MODE, WIDGET_FIRST_ID, WIDGET_LAST_ID);
}

DATA(0x005347d4) i32 choice;
DATA(0x005347c8) class iconWidget* skillWidget[CHOICE_COUNT];
DATA(0x005347c4) class heroWindow* arenaWinPtr;
