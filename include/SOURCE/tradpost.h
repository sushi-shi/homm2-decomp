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
extern unsigned short *coreRatio;
extern class iconWidget *tradeKnob;
extern int qtyToTrade;
extern int leftResource;
extern int iTradeRatio;
extern int bTradeMade;
extern float fTradingPostEfficiency;
extern int tpX;
extern int tpY;
extern int bIsMarketPlace;
extern int bLeftDenominated;
extern class heroWindow *tpWindow;
extern int rightResource;
extern int iMaxUnitsToTrade;
extern int maxUnitsToTrade;

#endif // HOMM2_TRADPOST_H
