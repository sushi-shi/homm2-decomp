#ifndef HOMM2_SETUP_H
#define HOMM2_SETUP_H

#include <Ints.h>
#include <BASE/message.h>
#include <SOURCE/REMOTE_TYPES.h>

struct tag_message;

MessageDispatchResult SetupCampaignGameHandler(struct tag_message&);
MessageDispatchResult SetupComPortHandler(struct tag_message&);
MessageDispatchResult SetupBaudHandler(struct tag_message&);
MessageDispatchResult SetupHotSeatGameHandler(struct tag_message&);
MessageDispatchResult SetupModemGameHandler(struct tag_message&);
MessageDispatchResult SetupMultiPlayerGameHandler(struct tag_message&);
MessageDispatchResult SetupNetworkGameHandler(struct tag_message&);
MessageDispatchResult SetupNetworkGame2Handler(struct tag_message&);
MessageDispatchResult SetupGameHandler(struct tag_message&);
MessageDispatchResult ExpNewCampaignHandler(struct tag_message&);
MessageDispatchResult ExpLoadCampaignHandler(struct tag_message&);
MessageDispatchResult ExpStdGameHandler(struct tag_message&);
MessageDispatchResult BaseSetupHandler(struct tag_message&);

extern b32 gbDoModemConfig;

#endif
