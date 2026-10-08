#ifndef HOMM2_SETUP_H
#define HOMM2_SETUP_H

#include <Domains.h>
#include <BASE/message.h>
#include <BASE/dialog.h>
#include <SOURCE/remoteTypes.h>

struct tag_message;

// The setup dialogs' numbered choices returned through m_dialogResult.
H2_ENUM_BEGIN(SetupDialogChoice)
    DIALOG_CANCEL = DIALOG_BUTTON_1,
    CHOICE_ONE    = 1,
    CHOICE_TWO    = 2,
    CHOICE_THREE  = 3,
    CHOICE_FOUR   = 4,
    CHOICE_FIVE   = 5
H2_ENUM_END(SetupDialogChoice)

H2_ENUM_BEGIN(SetupHelpIndex)
    NO_HELP    = -1,
    FIRST_HELP = 0
H2_ENUM_END(SetupHelpIndex)

// Where the setup panels (stpemain.bin, x_mapmnu.bin and the setup dialogs) open.
H2_ENUM_BEGIN(SetupWindowConstant)
    SETUP_WINDOW_X = 405,
    SETUP_WINDOW_Y = 8
H2_ENUM_END(SetupWindowConstant)

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
