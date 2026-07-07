#ifndef HOMM2_TRADPOST_H
#define HOMM2_TRADPOST_H
// Declarations of the free functions DEFINED in tradpost.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).
// forward declarations (was <_all.h>):
struct tag_message;

void DoTradingPost(int, float);
void UpdateTradingPost(int);
void ComputeTradeRatios(int, int, int *, int *, int *);
void DoTradeKnob(struct tag_message);
void SetupNewTrade(void);
int TradingPostHandler(struct tag_message &);

// ---- globals (declarations, RVA order) ----
DATA(0x0051d9d0) extern unsigned short *coreRatio;
DATA(0x00533170) extern class iconWidget *tradeKnob;
DATA(0x00533174) extern int qtyToTrade;
DATA(0x00533178) extern int leftResource;
DATA(0x0053317c) extern int iTradeRatio;
DATA(0x00533180) extern int bTradeMade;
DATA(0x00533184) extern float fTradingPostEfficiency;
DATA(0x00533188) extern int tpX;
DATA(0x0053318c) extern int tpY;
DATA(0x00533190) extern int bIsMarketPlace;
DATA(0x00533194) extern int bLeftDenominated;
DATA(0x00533198) extern class heroWindow *tpWindow;
DATA(0x0053319c) extern int rightResource;
DATA(0x005331a0) extern int iMaxUnitsToTrade;
DATA(0x005331a4) extern int maxUnitsToTrade;

#endif // HOMM2_TRADPOST_H
