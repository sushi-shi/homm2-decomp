// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\SETUP.OBJ   from: (directly linked into exe)
// functions: 23   data: 1
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <SOURCE/SETUP.h>
#include <_all.h>
VA(0x00410e90, 0x2f)
int game::SetupCampaignGame(void) { return 0; }

VA(0x00410ebf, 0x141)
int game::SetupBaud(void) { return 0; }

VA(0x00411000, 0x200)
int game::SetupComPort(void) { return 0; }

VA(0x00411200, 0x238)
int game::SetupHotSeatGame(void) { return 0; }

VA(0x00411438, 0x127)
int game::SetupNetworkGame(void) { return 0; }

VA(0x0041155f, 0x236)
int game::SetupNetworkGame2(void) { return 0; }

VA(0x00411795, 0x317)
int game::SetupModemGame(void) { return 0; }

VA(0x00411aac, 0x257)
int game::SetupMultiPlayerGame(void) { return 0; }

VA(0x00411d03, 0x6c9)
int game::SetupGame(void) { return 0; }

VA(0x004123cc, 0x2aa)
int game::PickLoadGame(void) { return 0; }

VA(0x00412676, 0x1e)
int SetupCampaignGameHandler(struct tag_message &) { return 0; }

VA(0x00412694, 0x13e)
int SetupComPortHandler(struct tag_message &) { return 0; }

VA(0x004127d2, 0x13e)
int SetupBaudHandler(struct tag_message &) { return 0; }

VA(0x00412910, 0x118)
int SetupHotSeatGameHandler(struct tag_message &) { return 0; }

VA(0x00412a28, 0x12e)
int SetupModemGameHandler(struct tag_message &) { return 0; }

VA(0x00412b56, 0x108)
int SetupMultiPlayerGameHandler(struct tag_message &) { return 0; }

VA(0x00412c5e, 0xd7)
int SetupNetworkGameHandler(struct tag_message &) { return 0; }

VA(0x00412d35, 0xf8)
int SetupNetworkGame2Handler(struct tag_message &) { return 0; }

VA(0x00412e2d, 0x14b)
int SetupGameHandler(struct tag_message &) { return 0; }

VA(0x00412f78, 0xd7)
int ExpNewCampaignHandler(struct tag_message &) { return 0; }

VA(0x0041304f, 0xd7)
int ExpLoadCampaignHandler(struct tag_message &) { return 0; }

VA(0x00413126, 0xd7)
int ExpStdGameHandler(struct tag_message &) { return 0; }

VA(0x004131fd, 0xf0)
int BaseSetupHandler(struct tag_message &) { return 0; }

// ---- data / globals / vtables ----
DATA(0x004ee248)  // int gbDoModemConfig
