#ifndef HOMM2_SETUP_H
#define HOMM2_SETUP_H

#include <Ints.h>
#include <BASE/message.h>
#include <BASE/dialog.h>
#include <SOURCE/REMOTE_TYPES.h>

struct tag_message;


typedef enum SetupDialogChoice {
    DIALOG_CANCEL = DIALOG_BUTTON_1,
    CHOICE_ONE    = 1,
    CHOICE_TWO    = 2,
    CHOICE_THREE  = 3,
    CHOICE_FOUR   = 4,
    CHOICE_FIVE   = 5
} SetupDialogChoice;

typedef enum SetupHelpIndex {
    NO_HELP    = -1,
    FIRST_HELP = 0
} SetupHelpIndex;


typedef enum SetupWindowConstant {
    SETUP_WINDOW_X = 405,
    SETUP_WINDOW_Y = 8
} SetupWindowConstant;

MessageDispatchResult SetupCampaignGameHandler(struct tag_message& message);
MessageDispatchResult SetupComPortHandler(struct tag_message& message);
MessageDispatchResult SetupBaudHandler(struct tag_message& message);
MessageDispatchResult SetupHotSeatGameHandler(struct tag_message& message);
MessageDispatchResult SetupModemGameHandler(struct tag_message& message);
MessageDispatchResult SetupMultiPlayerGameHandler(struct tag_message& message);
MessageDispatchResult SetupNetworkGameHandler(struct tag_message& message);
MessageDispatchResult SetupNetworkGame2Handler(struct tag_message& message);
MessageDispatchResult SetupGameHandler(struct tag_message& message);
MessageDispatchResult ExpNewCampaignHandler(struct tag_message& message);
MessageDispatchResult ExpLoadCampaignHandler(struct tag_message& message);
MessageDispatchResult ExpStdGameHandler(struct tag_message& message);
MessageDispatchResult BaseSetupHandler(struct tag_message& message);

extern b32 gbDoModemConfig;

#endif
