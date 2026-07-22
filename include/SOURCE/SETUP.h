#ifndef HOMM2_SETUP_H
#define HOMM2_SETUP_H

#include <Ints.h>
#include <BASE/message.h>
#include <SOURCE/REMOTE_TYPES.h>

struct tag_message;

WidgetDispatchResult SetupCampaignGameHandler(struct tag_message&);
WidgetDispatchResult SetupComPortHandler(struct tag_message&);
WidgetDispatchResult SetupBaudHandler(struct tag_message&);
WidgetDispatchResult SetupHotSeatGameHandler(struct tag_message&);
WidgetDispatchResult SetupModemGameHandler(struct tag_message&);
WidgetDispatchResult SetupMultiPlayerGameHandler(struct tag_message&);
WidgetDispatchResult SetupNetworkGameHandler(struct tag_message&);
WidgetDispatchResult SetupNetworkGame2Handler(struct tag_message&);
WidgetDispatchResult SetupGameHandler(struct tag_message&);
WidgetDispatchResult ExpNewCampaignHandler(struct tag_message&);
WidgetDispatchResult ExpLoadCampaignHandler(struct tag_message&);
WidgetDispatchResult ExpStdGameHandler(struct tag_message&);
WidgetDispatchResult BaseSetupHandler(struct tag_message&);

extern b32 gbDoModemConfig;

#endif
