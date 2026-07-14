// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\X_GLOBAL.OBJ   from: (directly linked into exe)
// functions: 0   data: 25
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <SOURCE/ExpCampaign.h>
#include <SOURCE/X_GLOBAL.h>

// ---- globals (definitions, RVA order) ----
DATA(0x004f26b8) char *xHSCampaignNames[EXPANSION_HIGH_SCORE_CAMPAIGN_COUNT];
DATA(0x004f26f4) char *xNecromancerShrine;
DATA(0x004f26f8) char *xNecromancerShrineDesc;
DATA(0x004f2700) char *xStableText[X_GLOBAL_STABLE_TEXT_COUNT];
DATA(0x004f2750) char *xPasswordStrings[X_GLOBAL_PASSWORD_STRING_COUNT];
DATA(0x004f2a9c) signed char xNetHasOldPlayers;
DATA(0x004f2ab0) int xShrineBuildingCost[2334];
DATA(0x00527e78) int xPasswordStringsIndex[8];
DATA(0x00527e98) signed char xIsExpansionMap;
DATA(0x00527e9c) int xLastChoice;
DATA(0x00527e20) unsigned char xIsPlayingExpansionCampaign;
DATA(0x00527e28) ExpCampaign xCampaign;
DATA(0x00527ea0) int xTheSpell;
