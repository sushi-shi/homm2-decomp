#ifndef HOMM2_SETUP_H
#define HOMM2_SETUP_H
// Declarations of the free functions DEFINED in SETUP.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).
#include <_all.h>

int SetupCampaignGameHandler(struct tag_message &);
int SetupComPortHandler(struct tag_message &);
int SetupBaudHandler(struct tag_message &);
int SetupHotSeatGameHandler(struct tag_message &);
int SetupModemGameHandler(struct tag_message &);
int SetupMultiPlayerGameHandler(struct tag_message &);
int SetupNetworkGameHandler(struct tag_message &);
int SetupNetworkGame2Handler(struct tag_message &);
int SetupGameHandler(struct tag_message &);
int ExpNewCampaignHandler(struct tag_message &);
int ExpLoadCampaignHandler(struct tag_message &);
int ExpStdGameHandler(struct tag_message &);
int BaseSetupHandler(struct tag_message &);

#endif // HOMM2_SETUP_H
