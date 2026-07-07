// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\tradpost.obj   from: (directly linked into exe)
// functions: 6   data: 15
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <SOURCE/tradpost.h>
#include <_carcass_types.h>
VA(0x004bf340, 0x165)
void DoTradingPost(int, float) {}

VA(0x004bf4a5, 0x596)
void UpdateTradingPost(int) {}

VA(0x004bfa3b, 0xfe)
void ComputeTradeRatios(int, int, int *, int *, int *) {}

VA(0x004bfb39, 0x148)
void DoTradeKnob(struct tag_message) {}

VA(0x004bfc81, 0x3a)
void SetupNewTrade(void) {}

VA(0x004bfcbb, 0x3b6)
int TradingPostHandler(struct tag_message &) { return 0; }

// ---- globals (definitions, RVA order) ----
unsigned short *coreRatio;
class iconWidget *tradeKnob;
int qtyToTrade;
int leftResource;
int iTradeRatio;
int bTradeMade;
float fTradingPostEfficiency;
int tpX;
int tpY;
int bIsMarketPlace;
int bLeftDenominated;
class heroWindow *tpWindow;
int rightResource;
int iMaxUnitsToTrade;
int maxUnitsToTrade;
