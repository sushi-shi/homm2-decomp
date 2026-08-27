#include <Ints.h>
#include <BASE/Utf8.h>
#include <stdio.h>
#include <string.h>
#include <BASE/font.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/iconWidget.h>
#include <BASE/soundManager.h>
#include <BASE/textWidget.h>
#include <SOURCE/KB.h>
#include <SOURCE/Localization.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/advManager.h>
#include <PLATFORM/Runtime.h>
#include <SOURCE/x_arena.h>

typedef enum ArenaConstant {
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
} ArenaConstant;

i32 DoArenaDialog(void) {

    i32 windowLines2 = WINDOW_RESOURCE;

    i32 windowHeight16 = windowLines2 * WINDOW_ROW_HEIGHT + WINDOW_BASE_HEIGHT;
    i32 windowX4 = WINDOW_X;
    i32 windowY3 = (WINDOW_SCREEN_HEIGHT - windowHeight16) / WINDOW_CENTER_DIVISOR;
    char windowName7[WINDOW_NAME_SIZE];
    tag_message message14;
    i32 widgetIndex1;
    textWidget* statWidgets0[CHOICE_COUNT];

    if (windowY3 > WINDOW_MAX_Y)
        windowY3 = WINDOW_MAX_Y;
    choice = 0;
    utf8::Format(windowName7, "evntwin%d.bin", windowLines2);
    arenaWinPtr = new heroWindow(windowX4, windowY3, windowName7);
    if (arenaWinPtr == NULL)
        MemError();

    strcpy(gText, localization::Tr("adventure.arena.choose_skill"));
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
            gStatNames[widgetIndex1],
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

MessageDispatchResult ArenaWindowHandler(struct tag_message& message_1) {

    if (!gpSoundManager->MusicPlaying() && gpAdvManager->m_active == 1)
        gpSoundManager->SwitchAmbientMusic(
            giTerrainToMusicTrack[H2EnumIndex(gpAdvManager->m_currentTerrain)]
        );
    if (giDialogTimeout != 0 && platform::Ticks() > giDialogTimeout) {
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

i32 choice;
class iconWidget* skillWidget[CHOICE_COUNT];
class heroWindow* arenaWinPtr;
