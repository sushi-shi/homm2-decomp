// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\Campaign.obj   from: (directly linked into exe)
// functions: 7   data: 6
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
VA(0x00447710, 0x563)
// int game::HandleCampaignWin(void);

VA(0x00447c73, 0x343)
// void game::PlayPreScenarioSmacker(int, int);

VA(0x00447fb6, 0x48d)
// void game::ShowCampaignInfo(int, int);

VA(0x00448443, 0xa0f)
// void game::CampaignInfoUpdate(int);

VA(0x00448e52, 0x521)
// int CampaignHandler(struct tag_message &);

VA(0x00449373, 0x47)
// void game::InitEntireCampaign(int);

VA(0x004493ba, 0xbb7)
// void game::InitCampaignMap(void);

// ---- data / globals / vtables ----
DATA(0x004f4f28)  // short int (*)[13][2] trackXY
DATA(0x004f4f90)  // class heroWindow * campWin
DATA(0x00527ea4)  // int iCurViewSide
DATA(0x00527ea8)  // int iCampaignTrackType
DATA(0x00527eac)  // int bCampaignViewOnly
DATA(0x00527eb0)  // int iCurViewMap
