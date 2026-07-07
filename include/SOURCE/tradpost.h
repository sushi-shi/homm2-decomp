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

#endif // HOMM2_TRADPOST_H
