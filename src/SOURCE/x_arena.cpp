#include <H2/Ints.h>
#include <stdio.h>
#include <string.h>
#include <BASE/message.h>
#include <BASE/font.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/inputManager.h>
#include <BASE/iconWidget.h>
#include <BASE/soundManager.h>
#include <BASE/textWidget.h>
#include <SOURCE/KB.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/advManager.h>
#include <SOURCE/kbwin.h>
#include <SOURCE/x_arena.h>
#include <BASE/dialog.h>
#include <BASE/display.h>

typedef enum ArenaConstant {
    CHOICE_COUNT          = 3,
    WINDOW_RESOURCE       = 5,
    WINDOW_CENTER_DIVISOR = 2,
    TEXT_LINE_SHIFT       = 4,
    ICON_FIRST_X          = 84,
    TEXT_FIRST_X          = 79,
    WIDGET_X_STEP         = 60,
    ICON_Y                = 244,
    ICON_WIDTH            = 39,
    ICON_HEIGHT           = 34,
    TEXT_Y                = 282,
    TEXT_WIDTH_PIXELS     = 49,
    TEXT_HEIGHT           = 24,
    WIDGET_FIRST_ID       = 100,
    WIDGET_LAST_ID        = 102,
    SELECTED_FRAME_OFFSET = 4,
    TEXT_BACKGROUND       = -1,
    BROADCAST_TEXT_ID     = 1
} ArenaConstant;

i32 DoArenaDialog(void) {
    i32 unusedValue1 [[maybe_unused]];
    i32 unusedValue2 [[maybe_unused]];
    i32 unusedValue3 [[maybe_unused]];
    i32 unusedValue4 [[maybe_unused]];
    i32 windowLines = WINDOW_RESOURCE;
    i16 widgetMode [[maybe_unused]] = 1;
    i32 windowWidth [[maybe_unused]] = NORMAL_DIALOG_WINDOW_WIDTH;
    i32 windowHeight = windowLines * NORMAL_DIALOG_WINDOW_ROW_HEIGHT + NORMAL_DIALOG_WINDOW_BASE_HEIGHT;
    i32 windowX = NORMAL_DIALOG_DEFAULT_X;
    i32 windowY = (LOGICAL_SCREEN_HEIGHT - windowHeight) / WINDOW_CENTER_DIVISOR;
    char windowName[NORMAL_DIALOG_FILENAME_LENGTH];
    i32 lineCount;
    i32 textHeight [[maybe_unused]];
    tag_message message;
    i32 widgetIndex;
    textWidget* statWidgets[CHOICE_COUNT];

    if (windowY > NORMAL_DIALOG_MAX_TOP)
        windowY = NORMAL_DIALOG_MAX_TOP;
    choice = 0;
    sprintf(windowName, "evntwin%d.bin", windowLines);
    arenaWinPtr = new heroWindow(windowX, windowY, windowName);
    if (arenaWinPtr == NULL)
        MemError();

    strcpy(
        gText,


        localization::Tr("adventure.arena.choose_skill")
    );
    lineCount = bigFont->LineLength(gText, NORMAL_DIALOG_TEXT_LINE_WIDTH);
    textHeight = lineCount << TEXT_LINE_SHIFT;
    SET_WIDGET_MESSAGE(message, WIDGET_COMMAND_SET_TEXT, BROADCAST_TEXT_ID);
    message.payload.widget.data.text = gText;
    arenaWinPtr->BroadcastMessage(message);

    for (widgetIndex = 0; widgetIndex < CHOICE_COUNT; widgetIndex++) {
        skillWidget[widgetIndex] = new iconWidget(
            widgetIndex * WIDGET_X_STEP + ICON_FIRST_X,
            ICON_Y,
            ICON_WIDTH,
            ICON_HEIGHT,
            "xprimary.icn",
            widgetIndex == choice ? widgetIndex + SELECTED_FRAME_OFFSET
                                     : widgetIndex,
            ICON_DRAW_NORMAL,
            widgetIndex + WIDGET_FIRST_ID,
            WIDGET_KIND_ICON_DIRECT,
            1
        );
        if (skillWidget[widgetIndex] == NULL)
            MemError();

        statWidgets[widgetIndex] = new textWidget(
            widgetIndex * WIDGET_X_STEP + TEXT_FIRST_X,
            TEXT_Y,
            TEXT_WIDTH_PIXELS,
            TEXT_HEIGHT,

            const_cast<char*>(gStatNames[widgetIndex]),
            "smalfont.fnt",
            FONT_DRAW_DEFAULT,
            TEXT_BACKGROUND,
            WIDGET_KIND_TEXT,
            FONT_ALIGN_CENTER
        );
        if (statWidgets[widgetIndex] == NULL)
            MemError();
        arenaWinPtr->AddWidget(skillWidget[widgetIndex], -1);
        arenaWinPtr->AddWidget(statWidgets[widgetIndex], -1);
    }

    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
    message.payload.widget.data.value = H2EnumIndex(WIDGET_FLAG_ENABLED | WIDGET_FLAG_DRAW);
    message.payload.widget.id = DIALOG_BUTTON_7;
    arenaWinPtr->BroadcastMessage(message);
    message.payload.widget.id = DIALOG_BUTTON_8;
    arenaWinPtr->BroadcastMessage(message);
    message.payload.widget.id = DIALOG_BUTTON_1;
    arenaWinPtr->BroadcastMessage(message);
    message.payload.widget.id = DIALOG_BUTTON_5;
    arenaWinPtr->BroadcastMessage(message);
    message.payload.widget.id = DIALOG_BUTTON_6;
    arenaWinPtr->BroadcastMessage(message);

    gpWindowManager->DoDialog(arenaWinPtr, ArenaWindowHandler, false);
    delete arenaWinPtr;
    return choice;
}

MessageDispatchResult ArenaWindowHandler(struct tag_message& message) {
    tag_message dialogMessage [[maybe_unused]];
    i32 widgetIndex [[maybe_unused]];
    i32 unusedDialogResourceType [[maybe_unused]];
    i32 extra [[maybe_unused]];

    if (!gpSoundManager->MusicPlaying() && gpAdvManager->m_active == true)
        gpSoundManager->SwitchAmbientMusic(
            giTerrainToMusicTrack[H2EnumIndex(gpAdvManager->m_currentTerrain)]
        );
    if (giDialogTimeout != 0 && KBTickCount() > giDialogTimeout) {
        message.type = MESSAGE_WIDGET;
        gpWindowManager->m_dialogResult = message.payload.widget.id;
        message.payload.widget.id = H2EnumIndex(WIDGET_COMMAND_DIALOG_SELECT);
        message.payload.widget.command = WIDGET_COMMAND_DIALOG_SELECT;
        giDialogTimeout = 0;
        return MESSAGE_DISPATCH_FORWARD;
    }

    if (message.type == MESSAGE_KEY_DOWN) {
        if (message.payload.keyboard.keyCode == INPUT_SCAN_TAB) {
            choice++;
            if (choice >= CHOICE_COUNT)
                choice = 0;
            UpdateArenaIcons();
        }
    } else if (message.type == MESSAGE_WIDGET) {
        switch (message.payload.widget.command) {
            case WIDGET_NOTIFY_SELECT:
            case WIDGET_NOTIFY_RIGHT_CLICK:
                extra = NORMAL_DIALOG_NO_VALUE;
                unusedDialogResourceType = NORMAL_DIALOG_NO_RESOURCE;
                if (message.payload.widget.parameter & H2EnumIndex(MESSAGE_MODIFIER_RIGHT_BUTTON)) {
                    switch (message.payload.widget.id) {
                        case WIDGET_FIRST_ID:
                        case WIDGET_FIRST_ID + 1:
                        case WIDGET_LAST_ID:
                            choice = message.payload.widget.id - WIDGET_FIRST_ID;
                            NormalDialog(gStatDesc[choice], NORMAL_DIALOG_QUICK_VIEW);
                            break;
                    }
                }
                break;

            case WIDGET_NOTIFY_DESELECT:
                switch (message.payload.widget.id) {
                    case WIDGET_FIRST_ID:
                    case WIDGET_FIRST_ID + 1:
                    case WIDGET_LAST_ID:
                        choice = message.payload.widget.id - WIDGET_FIRST_ID;
                        UpdateArenaIcons();
                        break;
                    case DIALOG_BUTTON_2:
                        gpWindowManager->m_dialogResult = message.payload.widget.id;
                        message.payload.widget.id = H2EnumIndex(WIDGET_COMMAND_DIALOG_SELECT);
                        message.payload.widget.command = WIDGET_COMMAND_DIALOG_SELECT;
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
    arenaWinPtr->DrawWindow(WINDOW_DRAW_UPDATE_SCREEN, WIDGET_FIRST_ID, WIDGET_LAST_ID);
}

i32 choice;
class iconWidget* skillWidget[CHOICE_COUNT];
class heroWindow* arenaWinPtr;
