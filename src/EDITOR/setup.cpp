// The editor's start-up dialogs: the main set-up screen, the new-map choice
// (blank or random) and the new map's size. The handlers began as copies of
// the game's SETUP.cpp handlers. Descriptive names: SetupMapSize, the three
// handlers but BaseSetupHandler, gNewMapSize, gbNewRandomMap.

#include <va.h>
#include <EDITOR/setup.h>
#include <EDITOR/EDITOR.h>
#include <BASE/dialog.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/message.h>
#include <SOURCE/KB.h>
#include <SOURCE/REQUEST.h>
#include <SOURCE/X_GLOBAL.h>

H2_ENUM_BEGIN(SetupConstant)
    WINDOW_X          = 405,
    WINDOW_Y          = 8,
    DIALOG_CANCEL     = DIALOG_BUTTON_1,
    HELP_DIALOG       = NORMAL_DIALOG_QUICK_VIEW,
    DIALOG_RESULT_MAX = 1000
H2_ENUM_END(SetupConstant)

H2_ENUM_BEGIN(SetupDialogChoice)
    CHOICE_ONE   = 1,
    CHOICE_TWO   = 2,
    CHOICE_THREE = 3,
    CHOICE_FOUR  = 4,
    // The main screen's quit button.
    CHOICE_QUIT  = 0x69
H2_ENUM_END(SetupDialogChoice)

H2_ENUM_BEGIN(SetupHelpIndex)
    NO_HELP = -1,
    FIRST_HELP = 0
H2_ENUM_END(SetupHelpIndex)

DATA(0x00499460) i32 gNewMapSize = MAP_DIMENSION_MEDIUM;
DATA(0x004a5820) b32 gbNewRandomMap;

VA(0x00425bf0, 0xfa)
i32 SetupNewMap(void) {
    heroWindow* window = new heroWindow(WINDOW_X, WINDOW_Y, "stpenew.bin");
    if (window == NULL)
        MemError();
    gpWindowManager->DoDialog(window, SetupNewMapHandler, 0);
    delete window;

    switch (gpWindowManager->m_dialogResult) {
        case CHOICE_ONE:
            gbNewRandomMap = false;
            break;
        case CHOICE_TWO:
            gbNewRandomMap = true;
            break;
        case DIALOG_CANCEL:
            return 0;
    }
    if (!SetupMapSize())
        return 0;
    return 1;
}

VA(0x00425cea, 0x119)
i32 SetupMapSize(void) {
    heroWindow* window = new heroWindow(WINDOW_X, WINDOW_Y, "stpesize.bin");
    if (window == NULL)
        MemError();
    gpWindowManager->DoDialog(window, SetupMapSizeHandler, 0);
    delete window;

    switch (gpWindowManager->m_dialogResult) {
        case CHOICE_ONE:
            gNewMapSize = MAP_DIMENSION_SMALL;
            break;
        case CHOICE_TWO:
            gNewMapSize = MAP_DIMENSION_MEDIUM;
            break;
        case CHOICE_THREE:
            gNewMapSize = MAP_DIMENSION_LARGE;
            break;
        case CHOICE_FOUR:
            gNewMapSize = MAP_DIMENSION_XLARGE;
            break;
        case DIALOG_CANCEL:
            return 0;
    }
    return 1;
}

VA(0x00425e03, 0xa1)
MessageDispatchResult SetupNewMapHandler(struct tag_message& message) {
    i32 helpIndex;

    if ((HAS(message.payload.widget.modifiers, MESSAGE_MODIFIER_RIGHT_BUTTON)) != 0
        && IS_WIDGET_SELECTION_NOTIFICATION(message.payload.widget.command)) {
        helpIndex = NO_HELP;
        switch (message.payload.widget.id) {
            case CHOICE_ONE:
                helpIndex = 0;
                break;
            case CHOICE_TWO:
                helpIndex = 1;
                break;
            case DIALOG_CANCEL:
                helpIndex = 2;
                break;
        }
        if (helpIndex >= FIRST_HELP)
            NormalDialog(gSetupNewMapHelp[helpIndex], HELP_DIALOG);
    }
    return BaseSetupHandler(message);
}

VA(0x00425ea4, 0xcf)
MessageDispatchResult SetupMapSizeHandler(struct tag_message& message) {
    i32 helpIndex;

    if ((HAS(message.payload.widget.modifiers, MESSAGE_MODIFIER_RIGHT_BUTTON)) != 0
        && IS_WIDGET_SELECTION_NOTIFICATION(message.payload.widget.command)) {
        helpIndex = NO_HELP;
        switch (message.payload.widget.id) {
            case CHOICE_ONE:
                helpIndex = 0;
                break;
            case CHOICE_TWO:
                helpIndex = 1;
                break;
            case CHOICE_THREE:
                helpIndex = 2;
                break;
            case CHOICE_FOUR:
                helpIndex = 3;
                break;
            case DIALOG_CANCEL:
                helpIndex = 4;
                break;
        }
        if (helpIndex >= FIRST_HELP)
            NormalDialog(gSetupMapSizeHelp[helpIndex], HELP_DIALOG);
    }
    return BaseSetupHandler(message);
}

// The game's handler less its shingle update; the main screen's quit button
// also closes a dialog.
VA(0x00425f73, 0xb9)
MessageDispatchResult BaseSetupHandler(struct tag_message& message) {
    b32 handled = false;

    PollSound();
    if (message.type == MESSAGE_WIDGET) {
        switch (message.payload.widget.command) {
            case WIDGET_NOTIFY_DESELECT:
                if ((message.payload.widget.id > 0
                     && message.payload.widget.id <= DIALOG_RESULT_MAX)
                    || message.payload.widget.id == DIALOG_CANCEL
                    || message.payload.widget.id == CHOICE_QUIT)
                    handled = true;
        }
    }

    if (handled || giMenuCommand != -1) {
        FINISH_DIALOG_MESSAGE(message);
        if (giMenuCommand != -1)
            gpWindowManager->m_dialogResult = DIALOG_CANCEL;
        return MESSAGE_DISPATCH_FORWARD;
    }
    return MESSAGE_DISPATCH_CONSUME;
}

VA(0x0042602c, 0xaa)
MessageDispatchResult SetupMainHandler(struct tag_message& message) {
    b32 handled = false;
    i32 helpIndex;

    PollSound();
    if ((HAS(message.payload.widget.modifiers, MESSAGE_MODIFIER_RIGHT_BUTTON)) != 0
        && IS_WIDGET_SELECTION_NOTIFICATION(message.payload.widget.command)) {
        helpIndex = NO_HELP;
        switch (message.payload.widget.id) {
            case CHOICE_ONE:
                helpIndex = 0;
                break;
            case CHOICE_TWO:
                helpIndex = 1;
                break;
            case CHOICE_QUIT:
                helpIndex = 2;
                break;
        }
        if (helpIndex >= FIRST_HELP)
            NormalDialog(gSetupMainHelp[helpIndex], HELP_DIALOG);
    }
    return BaseSetupHandler(message);
}
