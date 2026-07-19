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
    ICON_FILL_COLOR         = 0x10,
    TEXT_COLOR              = 1,
    TEXT_BACKGROUND         = -1,
    TEXT_FLAGS              = 0x200,
    CYCLE_KEY               = 15,
    BROADCAST_TEXT_ID       = 1,
    BROADCAST_CONTROL_VALUE = 6,
    DRAW_MODE               = 1
H2_ENUM_END(ArenaConstant)

VA(0x004c0080, 0x35c)
i32 DoArenaDialog(void) {
    i32 statValues_2[CHOICE_COUNT];
    i32 statValue_28;
    i32 windowLines_7 = WINDOW_RESOURCE;
    i16 widgetMode_3 = 1;
    i32 windowWidth_9 = WINDOW_WIDTH;
    i32 windowHeight = windowLines_7 * WINDOW_ROW_HEIGHT + WINDOW_BASE_HEIGHT;
    i32 windowX_9 = WINDOW_X;
    i32 windowY_12 = (WINDOW_SCREEN_HEIGHT - windowHeight) / 2;
    char windowName_3[WINDOW_NAME_SIZE];
    i32 lineCount_3;
    i32 textHeight_11;
    tag_message message;
    i32 widgetIndex_12;
    textWidget* statWidgets[CHOICE_COUNT];

    if (windowY_12 > WINDOW_MAX_Y)
        windowY_12 = WINDOW_MAX_Y;
    choice = 0;
    sprintf(windowName_3, "evntwin%d.bin", windowLines_7);
    arenaWinPtr = new heroWindow(windowX_9, windowY_12, windowName_3);
    if (arenaWinPtr == NULL)
        MemError();

    strcpy(
        gText,
        "You enter the arena and face a pack of vicious lions.  You handily "
        "defeat them, to the wild cheers of the crowd.  Impressed by your "
        "skill, the aged trainer of gladiators agrees to train you in a "
        "skill of your choice."
    );
    lineCount_3 = bigFont->LineLength(gText, TEXT_WIDTH);
    textHeight_11 = lineCount_3 << TEXT_LINE_SHIFT;
    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = ARENA_BROADCAST_TEXT;
    message.payload.widget.id = BROADCAST_TEXT_ID;
    message.payload.widget.data.text = gText;
    arenaWinPtr->BroadcastMessage(message);

    for (widgetIndex_12 = 0; widgetIndex_12 < CHOICE_COUNT; widgetIndex_12++) {
        skillWidget[widgetIndex_12] = new iconWidget(
            widgetIndex_12 * WIDGET_X_STEP + ICON_FIRST_X,
            ICON_Y,
            ICON_WIDTH,
            ICON_HEIGHT,
            "xprimary.icn",
            widgetIndex_12 == choice ? widgetIndex_12 + SELECTED_FRAME_OFFSET
                                     : widgetIndex_12,
            0,
            widgetIndex_12 + WIDGET_FIRST_ID,
            ICON_FILL_COLOR,
            1
        );
        if (skillWidget[widgetIndex_12] == NULL)
            MemError();

        statWidgets[widgetIndex_12] = new textWidget(
            widgetIndex_12 * WIDGET_X_STEP + TEXT_FIRST_X,
            TEXT_Y,
            TEXT_WIDTH_PIXELS,
            TEXT_HEIGHT,
            gStatNames[widgetIndex_12],
            "smalfont.fnt",
            TEXT_COLOR,
            TEXT_BACKGROUND,
            TEXT_FLAGS,
            1
        );
        if (statWidgets[widgetIndex_12] == NULL)
            MemError();
        arenaWinPtr->AddWidget(skillWidget[widgetIndex_12], -1);
        arenaWinPtr->AddWidget(statWidgets[widgetIndex_12], -1);
    }

    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = ARENA_BROADCAST_CONTROL;
    message.payload.widget.data.value = BROADCAST_CONTROL_VALUE;
    message.payload.widget.id = EVENT_WINDOW_SEVENTH_BUTTON;
    arenaWinPtr->BroadcastMessage(message);
    message.payload.widget.id = EVENT_WINDOW_EIGHTH_BUTTON;
    arenaWinPtr->BroadcastMessage(message);
    message.payload.widget.id = EVENT_WINDOW_SECOND_BUTTON;
    arenaWinPtr->BroadcastMessage(message);
    message.payload.widget.id = EVENT_WINDOW_FIFTH_BUTTON;
    arenaWinPtr->BroadcastMessage(message);
    message.payload.widget.id = EVENT_WINDOW_SIXTH_BUTTON;
    arenaWinPtr->BroadcastMessage(message);

    gpWindowManager->DoDialog(arenaWinPtr, ArenaWindowHandler, 0);
    delete arenaWinPtr;
    return choice;
}

VA(0x004c03dc, 0x25f)
i32 ArenaWindowHandler(struct tag_message& message_1) {
    tag_message dialogMessage_12;
    i32 widgetIndex_5;
    i32 type_13;
    i32 extra_18;

    if (!gpSoundManager->MusicPlaying() && gpAdvManager->m_active)
        gpSoundManager->SwitchAmbientMusic(giTerrainToMusicTrack[gpAdvManager->m_currentTerrain]);
    if (giDialogTimeout != 0 && KBTickCount() > giDialogTimeout) {
        message_1.type = MESSAGE_WIDGET;
        gpWindowManager->m_dialogResult = message_1.payload.widget.id;
        message_1.payload.widget.id = EVENT_WINDOW_CLOSE_COMMAND;
        message_1.payload.widget.command = BaseWidgetCommand(message_1.payload.widget.id);
        giDialogTimeout = 0;
        return EVENT_WINDOW_CLOSE;
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
                type_13 = NORMAL_DIALOG_NO_RESOURCE;
                extra_18 = NORMAL_DIALOG_NO_VALUE;
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
                        message_1.payload.widget.command =
                            BaseWidgetCommand(message_1.payload.widget.id);
                        giDialogTimeout = 0;
                        return EVENT_WINDOW_CLOSE;
                    default:
                        break;
                }
                break;
            default:
                break;
        }
    }
    return EVENT_WINDOW_CONTINUE;
}

VA(0x004c063b, 0x150)
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
            0,
            widgetIndex + WIDGET_FIRST_ID,
            ICON_FILL_COLOR,
            1
        );
        if (skillWidget[widgetIndex] == NULL)
            MemError();
        arenaWinPtr->AddWidget(skillWidget[widgetIndex], -1);
    }
    arenaWinPtr->DrawWindow(DRAW_MODE, WIDGET_FIRST_ID, WIDGET_LAST_ID);
}

DATA(0x005331a8) i32 choice;
DATA(0x005331b0) class iconWidget* skillWidget[3];
DATA(0x005331bc) class heroWindow* arenaWinPtr;
