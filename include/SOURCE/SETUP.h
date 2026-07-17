#ifndef HOMM2_SETUP_H
#define HOMM2_SETUP_H

#include <Ints.h>
// Declarations of the free functions DEFINED in SETUP.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).
// forward declarations (was <_all.h>):
struct tag_message;

typedef enum SetupConstant {
    SETUP_WINDOW_X = 405,
    SETUP_WINDOW_Y = 8,
    SETUP_DIALOG_CANCEL = 0x7801,
    SETUP_DIALOG_YES = 0x7805,
    SETUP_PLAYER_COUNT = 6,
    SETUP_PLAYER_NAME_LENGTH = 20,
    SETUP_MODEM_INIT_ENTRY_LENGTH = 40,
    SETUP_TELEPHONE_ENTRY_LENGTH = 35,
    SETUP_DISABLED_WIDGET_ID = 1,
    SETUP_LOG_UNUSED = -999,
    SETUP_CAMPAIGN_INTRO = 4,
    SETUP_CAMPAIGN_SELECTION = 35,
    SETUP_HELP_DIALOG = 4,
    SETUP_DIALOG_RESULT_MIN = 1,
    SETUP_DIALOG_RESULT_MAX = 1000,
    SETUP_HANDLER_CONTINUE = 1,
    SETUP_HANDLER_CLOSE = 2
} SetupConstant;

typedef enum SetupMultiplayerType {
    SETUP_MULTIPLAYER_MODEM = 0,
    SETUP_MULTIPLAYER_NETWORK = 1,
    SETUP_MULTIPLAYER_HOT_SEAT = 2
} SetupMultiplayerType;

typedef enum SetupRemoteType {
    SETUP_REMOTE_NETWORK_HOST = 1,
    SETUP_REMOTE_NETWORK_GUEST = 2,
    SETUP_REMOTE_MODEM_DIAL = 3,
    SETUP_REMOTE_MODEM_ANSWER = 4
} SetupRemoteType;

typedef enum SetupBaudRate {
    SETUP_BAUD_2400 = 2400,
    SETUP_BAUD_9600 = 9600,
    SETUP_BAUD_19200 = 19200,
    SETUP_BAUD_38400 = 38400
} SetupBaudRate;

typedef enum SetupComPort {
    SETUP_COM_PORT_UNCONFIGURED = 0,
    SETUP_COM_PORT_1 = 1,
    SETUP_COM_PORT_2 = 2,
    SETUP_COM_PORT_3 = 3,
    SETUP_COM_PORT_4 = 4
} SetupComPort;

i32 SetupCampaignGameHandler(struct tag_message &);
i32 SetupComPortHandler(struct tag_message &);
i32 SetupBaudHandler(struct tag_message &);
i32 SetupHotSeatGameHandler(struct tag_message &);
i32 SetupModemGameHandler(struct tag_message &);
i32 SetupMultiPlayerGameHandler(struct tag_message &);
i32 SetupNetworkGameHandler(struct tag_message &);
i32 SetupNetworkGame2Handler(struct tag_message &);
i32 SetupGameHandler(struct tag_message &);
i32 ExpNewCampaignHandler(struct tag_message &);
i32 ExpLoadCampaignHandler(struct tag_message &);
i32 ExpStdGameHandler(struct tag_message &);
i32 BaseSetupHandler(struct tag_message &);

// ---- globals (declarations, RVA order) ----
extern i32 gbDoModemConfig;

#endif // HOMM2_SETUP_H
