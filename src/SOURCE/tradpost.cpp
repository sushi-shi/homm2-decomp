// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\tradpost.obj   from: (directly linked into exe)
// functions: 6   data: 15
// RVA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <rva.h>
RVA(0x004bf340, 0x165)
// void DoTradingPost(int, float);

RVA(0x004bf4a5, 0x596)
// void UpdateTradingPost(int);

RVA(0x004bfa3b, 0xfe)
// void ComputeTradeRatios(int, int, int *, int *, int *);

RVA(0x004bfb39, 0x148)
// void DoTradeKnob(struct tag_message);

RVA(0x004bfc81, 0x3a)
// void SetupNewTrade(void);

RVA(0x004bfcbb, 0x3b6)
// int TradingPostHandler(struct tag_message &);

// ---- data / globals / vtables ----
DATA(0x0051d9d0)  // unsigned short int * coreRatio
DATA(0x00533170)  // class iconWidget * tradeKnob
DATA(0x00533174)  // int qtyToTrade
DATA(0x00533178)  // int leftResource
DATA(0x0053317c)  // int iTradeRatio
DATA(0x00533180)  // int bTradeMade
DATA(0x00533184)  // float fTradingPostEfficiency
DATA(0x00533188)  // int tpX
DATA(0x0053318c)  // int tpY
DATA(0x00533190)  // int bIsMarketPlace
DATA(0x00533194)  // int bLeftDenominated
DATA(0x00533198)  // class heroWindow * tpWindow
DATA(0x0053319c)  // int rightResource
DATA(0x005331a0)  // int iMaxUnitsToTrade
DATA(0x005331a4)  // int maxUnitsToTrade
