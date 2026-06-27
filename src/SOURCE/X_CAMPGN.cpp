// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\X_CAMPGN.OBJ   from: (directly linked into exe)
// functions: 36   data: 1
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
VA(0x004bb680, 0x23)
// void ExpCampaign::constructor(void);

VA(0x004bb6a3, 0x16)
// void ExpCampaign::~destructor(void);

VA(0x004bb6b9, 0x29)
// void ExpCampaign::ResetMapChoices(void);

VA(0x004bb6e2, 0x29)
// void ExpCampaign::ResetMapsPlayed(void);

VA(0x004bb70b, 0x29)
// void ExpCampaign::ResetAwards(void);

VA(0x004bb734, 0x29)
// void ExpCampaign::ResetBonusChoices(void);

VA(0x004bb75d, 0x23)
// void ExpCampaign::GrantAward(int);

VA(0x004bb780, 0x23)
// void ExpCampaign::RemoveAward(int);

VA(0x004bb7a3, 0x22)
// signed char ExpCampaign::HasAward(int);

VA(0x004bb7c5, 0x24)
// void ExpCampaign::SetMapWasPlayed(void);

VA(0x004bb7e9, 0x5a)
// void ExpCampaign::InitNewCampaign(int);

VA(0x004bb843, 0x7cb)
// void ExpCampaign::InitMap(void);

VA(0x004bc00e, 0x33f)
// void ExpCampaign::ShowInfo(int, int);

VA(0x004bc34d, 0x921)
// void ExpCampaign::UpdateInfo(int);

VA(0x004bcc6e, 0x172)
// int ExpCampaign::HandleVictory(void);

VA(0x004bcde0, 0x167)
// void ExpCampaign::HandleVictory1(void);

VA(0x004bcf47, 0x14b)
// void ExpCampaign::HandleVictory2(void);

VA(0x004bd092, 0xce)
// void ExpCampaign::HandleVictory3(void);

VA(0x004bd160, 0xb9)
// void ExpCampaign::HandleVictory4(void);

VA(0x004bd219, 0x8c)
// void ExpCampaign::ReplaySmacker(void);

VA(0x004bd2a5, 0xf8)
// void ExpCampaign::ReplaySmacker1(void);

VA(0x004bd39d, 0xd9)
// void ExpCampaign::ReplaySmacker2(void);

VA(0x004bd476, 0xac)
// void ExpCampaign::ReplaySmacker3(void);

VA(0x004bd522, 0x88)
// void ExpCampaign::ReplaySmacker4(void);

VA(0x004bd5aa, 0x59)
// unsigned char ExpCampaign::IsCompleted(void);

VA(0x004bd603, 0x36)
// signed char ExpCampaign::IsThisMapCompleted(void);

VA(0x004bd639, 0x32e)
// static int ExpCampaign::MessageHandler(struct tag_message &);

VA(0x004bd967, 0x6c)
// void ExpCampaign::Autosave(void);

VA(0x004bd9d3, 0x25)
// int ExpCampaign::Choose(void);

VA(0x004bd9f8, 0x64)
// short int ExpCampaign::Days(void);

VA(0x004bda5c, 0x1b)
// int ExpCampaign::CampaignID(void);

VA(0x004bda77, 0x3a)
// char * ExpCampaign::JosephName(void);

VA(0x004bdab1, 0x3a)
// char * ExpCampaign::IvanName(void);

VA(0x004bdaeb, 0x4e)
// signed char ExpCampaign::IsSpecialGoldenBow(int, int);

VA(0x004bdb39, 0x38)
// signed char ExpCampaign::IsSpecialUA(void);

VA(0x004bdb71, 0x73)
// signed char ExpCampaign::IsSpecialLossCondition(int);

// ---- data / globals / vtables ----
DATA(0x0051d560)  // struct SCampaignChoice (*)[8][3] xCampaignChoices
