#include <va.h>
#include <stdio.h>
#include <string.h>
#include <BASE/message.h>
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
#include <BASE/dialog.h>
#include <BASE/display.h>

H2_ENUM_BEGIN(ArenaConstant)
    CHOICE_COUNT          = 3,
    WINDOW_RESOURCE       = 5,
    WINDOW_WIDTH          = 306,
    WINDOW_BASE_HEIGHT    = 180,
    WINDOW_ROW_HEIGHT     = 45,
    WINDOW_X              = 159,
    WINDOW_MAX_Y          = 28,
    WINDOW_CENTER_DIVISOR = 2,
    WINDOW_NAME_SIZE      = 16,
    TEXT_WIDTH            = 244,
    TEXT_LINE_HEIGHT      = 16,
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
    CYCLE_KEY             = 15,
    BROADCAST_TEXT_ID     = 1,
    DRAW_MODE             = 1
H2_ENUM_END(ArenaConstant)

#if H2_RETAIL_COMPILER
#define lineCount lineCount11
#define message message14
#define statWidgets statWidgets0
#define textHeight textHeight4
#define unusedValue1 statValue0
#define unusedValue2 statValue2
#define unusedValue3 statValue3
#define unusedValue4 statValue7
#define widgetIndex widgetIndex1
#define widgetMode widgetMode10
#define windowHeight windowHeight16
#define windowLines windowLines2
#define windowName windowName7
#define windowX windowX4
#define windowY windowY3
#endif
VA(0x004b2ba0, 0x3bb)
i32 DoArenaDialog(void) {
    i32 H2_UNUSED(unusedValue1);
    i32 H2_UNUSED(unusedValue2);
    i32 H2_UNUSED(unusedValue3);
    i32 H2_UNUSED(unusedValue4);
    i32 windowLines = WINDOW_RESOURCE;
    i16 H2_UNUSED(widgetMode) = 1;
    i32 H2_UNUSED(windowWidth) = WINDOW_WIDTH;
    i32 windowHeight = windowLines * WINDOW_ROW_HEIGHT + WINDOW_BASE_HEIGHT;
    i32 windowX = WINDOW_X;
    i32 windowY = (LOGICAL_SCREEN_HEIGHT - windowHeight) / WINDOW_CENTER_DIVISOR;
    char windowName[WINDOW_NAME_SIZE];
    i32 lineCount;
    i32 H2_UNUSED(textHeight);
    tag_message message;
    i32 widgetIndex;
    textWidget* statWidgets[CHOICE_COUNT];

    if (windowY > WINDOW_MAX_Y)
        windowY = WINDOW_MAX_Y;
    choice = 0;
    sprintf(windowName, "evntwin%d.bin", windowLines);
    arenaWinPtr = new heroWindow(windowX, windowY, windowName);
    if (arenaWinPtr == NULL)
        MemError();

    strcpy(
        gText,
        /* Вы вошли на арену и сразились со сворой свирепых львов. Впечатленный вашим мастерством
           тренер гладиаторов согласился научить вас одному навыку на ваш выбор. */
        localization::Tr("adventure.arena.choose_skill")
    );
    lineCount = bigFont->LineLength(gText, TEXT_WIDTH);
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
            // Retail passes static text into a widget that assumes ownership.
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
    message.payload.widget.data.value = IDX(WIDGET_FLAG_ENABLED | WIDGET_FLAG_DRAW);
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

    gpWindowManager->DoDialog(arenaWinPtr, ArenaWindowHandler, 0);
    delete arenaWinPtr;
    return choice;
}
#if H2_RETAIL_COMPILER
#undef lineCount
#undef message
#undef statWidgets
#undef textHeight
#undef unusedValue1
#undef unusedValue2
#undef unusedValue3
#undef unusedValue4
#undef widgetIndex
#undef widgetMode
#undef windowHeight
#undef windowLines
#undef windowName
#undef windowX
#undef windowY
#endif

#if H2_RETAIL_COMPILER
#define dialogMessage dialogMessage_12
#define extra extra_18
#define unusedDialogResourceType type_13
#define widgetIndex widgetIndex_5
#endif
VA(0x004b2f5b, 0x1e1)
MessageDispatchResult ArenaWindowHandler(struct tag_message& message_1) {
    tag_message H2_UNUSED(dialogMessage);
    i32 H2_UNUSED(widgetIndex);
    i32 H2_UNUSED(unusedDialogResourceType);
    i32 H2_UNUSED(extra);

    if (!gpSoundManager->MusicPlaying() && gpAdvManager->m_active == 1)
        gpSoundManager->SwitchAmbientMusic(
            giTerrainToMusicTrack[IDX(gpAdvManager->m_currentTerrain)]
        );
    if (giDialogTimeout != 0 && KBTickCount() > giDialogTimeout) {
        message_1.type = MESSAGE_WIDGET;
        gpWindowManager->m_dialogResult = message_1.payload.widget.id;
        message_1.payload.widget.id = IDX(WIDGET_COMMAND_DIALOG_SELECT);
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
            case WIDGET_NOTIFY_SELECT:
            case WIDGET_NOTIFY_RIGHT_CLICK:
                extra = NORMAL_DIALOG_NO_VALUE;
                unusedDialogResourceType = NORMAL_DIALOG_NO_RESOURCE;
                if (message_1.payload.widget.parameter & EVENT_WINDOW_RESOURCE_FLAG) {
                    switch (message_1.payload.widget.id) {
                        case WIDGET_FIRST_ID:
                        case WIDGET_FIRST_ID + 1:
                        case WIDGET_LAST_ID:
                            choice = message_1.payload.widget.id - WIDGET_FIRST_ID;
                            NormalDialog(gStatDesc[choice], NORMAL_DIALOG_QUICK_VIEW);
                            break;
                    }
                }
                break;

            case WIDGET_NOTIFY_DESELECT:
                switch (message_1.payload.widget.id) {
                    case WIDGET_FIRST_ID:
                    case WIDGET_FIRST_ID + 1:
                    case WIDGET_LAST_ID:
                        choice = message_1.payload.widget.id - WIDGET_FIRST_ID;
                        UpdateArenaIcons();
                        break;
                    case DIALOG_BUTTON_2:
                        gpWindowManager->m_dialogResult = message_1.payload.widget.id;
                        message_1.payload.widget.id = IDX(WIDGET_COMMAND_DIALOG_SELECT);
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
#if H2_RETAIL_COMPILER
#undef dialogMessage
#undef extra
#undef unusedDialogResourceType
#undef widgetIndex
#endif

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
