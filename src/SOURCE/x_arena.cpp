// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\x_arena.obj   from: (directly linked into exe)
// functions: 3   data: 3
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

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

VA(0x004c0080, 0x35c)
i32 DoArenaDialog(void) {
    i32 statValues_2[ARENA_CHOICE_COUNT];
    i32 statValue_28;
    i32 windowLines_7 = ARENA_WINDOW_RESOURCE;
    i16 widgetMode_3 = 1;
    i32 windowWidth_9 = ARENA_WINDOW_WIDTH;
    i32 windowHeight = windowLines_7 * ARENA_WINDOW_ROW_HEIGHT + ARENA_WINDOW_BASE_HEIGHT;
    i32 windowX_9 = ARENA_WINDOW_X;
    i32 windowY_12 = (ARENA_WINDOW_SCREEN_HEIGHT - windowHeight) / 2;
    char windowName_3[ARENA_WINDOW_NAME_SIZE];
    i32 lineCount_3;
    i32 textHeight_11;
    tag_message message;
    i32 widgetIndex_12;
    textWidget* statWidgets[ARENA_CHOICE_COUNT];

    if (windowY_12 > ARENA_WINDOW_MAX_Y)
        windowY_12 = ARENA_WINDOW_MAX_Y;
    choice = 0;
    sprintf(windowName_3, "evntwin%d.bin", windowLines_7);
    arenaWinPtr = new heroWindow(windowX_9, windowY_12, windowName_3);
    if (arenaWinPtr == 0)
        MemError();

    strcpy(
        gText,
        "You enter the arena and face a pack of vicious lions.  You handily "
        "defeat them, to the wild cheers of the crowd.  Impressed by your "
        "skill, the aged trainer of gladiators agrees to train you in a "
        "skill of your choice."
    );
    lineCount_3 = bigFont->LineLength(gText, ARENA_TEXT_WIDTH);
    textHeight_11 = lineCount_3 << ARENA_TEXT_LINE_SHIFT;
    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = ARENA_BROADCAST_TEXT;
    message.payload.widget.id = ARENA_BROADCAST_TEXT_ID;
    message.payload.widget.data.text = gText;
    arenaWinPtr->BroadcastMessage(message);

    for (widgetIndex_12 = 0; widgetIndex_12 < ARENA_CHOICE_COUNT; widgetIndex_12++) {
        skillWidget[widgetIndex_12] = new iconWidget(
            widgetIndex_12 * ARENA_WIDGET_X_STEP + ARENA_ICON_FIRST_X,
            ARENA_ICON_Y,
            ARENA_ICON_WIDTH,
            ARENA_ICON_HEIGHT,
            "xprimary.icn",
            widgetIndex_12 == choice ? widgetIndex_12 + ARENA_SELECTED_FRAME_OFFSET
                                     : widgetIndex_12,
            0,
            widgetIndex_12 + ARENA_WIDGET_FIRST_ID,
            ARENA_ICON_FILL_COLOR,
            1
        );
        if (skillWidget[widgetIndex_12] == 0)
            MemError();

        statWidgets[widgetIndex_12] = new textWidget(
            widgetIndex_12 * ARENA_WIDGET_X_STEP + ARENA_TEXT_FIRST_X,
            ARENA_TEXT_Y,
            ARENA_TEXT_WIDTH_PIXELS,
            ARENA_TEXT_HEIGHT,
            gStatNames[widgetIndex_12],
            "smalfont.fnt",
            ARENA_TEXT_COLOR,
            ARENA_TEXT_BACKGROUND,
            ARENA_TEXT_FLAGS,
            1
        );
        if (statWidgets[widgetIndex_12] == 0)
            MemError();
        arenaWinPtr->AddWidget(skillWidget[widgetIndex_12], -1);
        arenaWinPtr->AddWidget(statWidgets[widgetIndex_12], -1);
    }

    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = ARENA_BROADCAST_CONTROL;
    message.payload.widget.data.value = ARENA_BROADCAST_CONTROL_VALUE;
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
        message_1.payload.widget.command = message_1.payload.widget.id;
        giDialogTimeout = 0;
        return EVENT_WINDOW_CLOSE;
    }

    if (message_1.type == MESSAGE_KEY_DOWN) {
        if (message_1.payload.keyboard.keyCode == ARENA_CYCLE_KEY) {
            choice++;
            if (choice >= ARENA_CHOICE_COUNT)
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
                        case ARENA_WIDGET_FIRST_ID:
                        case ARENA_WIDGET_FIRST_ID + 1:
                        case ARENA_WIDGET_LAST_ID:
                            choice = message_1.payload.widget.id - ARENA_WIDGET_FIRST_ID;
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
                    case ARENA_WIDGET_FIRST_ID:
                    case ARENA_WIDGET_FIRST_ID + 1:
                    case ARENA_WIDGET_LAST_ID:
                        choice = message_1.payload.widget.id - ARENA_WIDGET_FIRST_ID;
                        UpdateArenaIcons();
                        break;
                    case EVENT_WINDOW_THIRD_BUTTON:
                        gpWindowManager->m_dialogResult = message_1.payload.widget.id;
                        message_1.payload.widget.id = EVENT_WINDOW_CLOSE_COMMAND;
                        message_1.payload.widget.command = message_1.payload.widget.id;
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

// @semantic
// Complete body, 0x8 frame/slots, CFG, and all 18 ordered relocation targets
// agree. At +0x8d retail loads global choice with the five-byte absolute EAX
// form before comparing widgetIndex; base loads the local first, making the body
// one byte longer and shifting later relocation sites. Operand swap and
// widgetIndex == OD_STEER(choice) were byte-neutral.
VA(0x004c063b, 0x150)
void UpdateArenaIcons(void) {
    i32 widgetIndex;

    for (widgetIndex = 0; widgetIndex < ARENA_CHOICE_COUNT; widgetIndex++) {
        arenaWinPtr->RemoveWidget(skillWidget[widgetIndex]);
        delete skillWidget[widgetIndex];
        skillWidget[widgetIndex] = 0;
        skillWidget[widgetIndex] = new iconWidget(
            widgetIndex * ARENA_WIDGET_X_STEP + ARENA_ICON_FIRST_X,
            ARENA_ICON_Y,
            ARENA_ICON_WIDTH,
            ARENA_ICON_HEIGHT,
            "xprimary.icn",
            widgetIndex == choice ? widgetIndex + ARENA_SELECTED_FRAME_OFFSET : widgetIndex,
            0,
            widgetIndex + ARENA_WIDGET_FIRST_ID,
            ARENA_ICON_FILL_COLOR,
            1
        );
        if (skillWidget[widgetIndex] == 0)
            MemError();
        arenaWinPtr->AddWidget(skillWidget[widgetIndex], -1);
    }
    arenaWinPtr->DrawWindow(ARENA_DRAW_MODE, ARENA_WIDGET_FIRST_ID, ARENA_WIDGET_LAST_ID);
}

// ---- globals (definitions, RVA order) ----
DATA(0x005331a8) i32 choice;
DATA(0x005331b0) class iconWidget* skillWidget[3];
DATA(0x005331bc) class heroWindow* arenaWinPtr;
