#ifndef HOMM2_SETUP_H
#define HOMM2_SETUP_H

#include <Ints.h>
#include <SOURCE/REMOTE_TYPES.h>

struct tag_message;

i32 SetupCampaignGameHandler(struct tag_message&);
i32 SetupComPortHandler(struct tag_message&);
i32 SetupBaudHandler(struct tag_message&);
i32 SetupHotSeatGameHandler(struct tag_message&);
i32 SetupModemGameHandler(struct tag_message&);
i32 SetupMultiPlayerGameHandler(struct tag_message&);
i32 SetupNetworkGameHandler(struct tag_message&);
i32 SetupNetworkGame2Handler(struct tag_message&);
i32 SetupGameHandler(struct tag_message&);
i32 ExpNewCampaignHandler(struct tag_message&);
i32 ExpLoadCampaignHandler(struct tag_message&);
i32 ExpStdGameHandler(struct tag_message&);
i32 BaseSetupHandler(struct tag_message&);

extern b32 gbDoModemConfig;

#endif
