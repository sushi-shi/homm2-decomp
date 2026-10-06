#ifndef HOMM2_SETUP_H
#define HOMM2_SETUP_H

#include <Ints.h>
#include <BASE/message.h>
#include <SOURCE/REMOTE_TYPES.h>

struct tag_message;

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
