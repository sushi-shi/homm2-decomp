#ifndef HOMM2_TRADPOST_H
#define HOMM2_TRADPOST_H

#include <Ints.h>

struct tag_message;

H2_ENUM_BEGIN(TradingPostConstant)
    TRADING_POST_RESOURCE_COUNT     = 7,
    TRADING_POST_WINDOW_X           = 0x9f,
    TRADING_POST_WINDOW_Y           = 0x17,
    TRADING_POST_KNOB_X             = 0x43,
    TRADING_POST_KNOB_Y             = 0xcc,
    TRADING_POST_KNOB_ID            = 0x1f,
    TRADING_POST_KNOB_TRACK_OFFSET  = 0x4b,
    TRADING_POST_KNOB_TRACK_WIDTH   = 0xbb,
    TRADING_POST_KNOB_TRAVEL        = 0xaa,
    TRADING_POST_CONTROL_FIRST      = 0x14,
    TRADING_POST_CONTROL_LAST       = 0x1f,
    TRADING_POST_LEFT_ICON_FIRST    = 0x32,
    TRADING_POST_LEFT_TEXT_FIRST    = 0x3c,
    TRADING_POST_LEFT_SELECT_FIRST  = 0x46,
    TRADING_POST_RIGHT_ICON_FIRST   = 0x5a,
    TRADING_POST_RIGHT_TEXT_FIRST   = 0x64,
    TRADING_POST_RIGHT_SELECT_FIRST = 0x6e
H2_ENUM_END(TradingPostConstant)

void DoTradingPost(i32, float);
void UpdateTradingPost(i32);
void ComputeTradeRatios(i32, i32, i32*, i32*, i32*);
void DoTradeKnob(struct tag_message);
void SetupNewTrade(void);
i32 TradingPostHandler(struct tag_message&);

extern u16 coreRatio[TRADING_POST_RESOURCE_COUNT];
extern class iconWidget* tradeKnob;
extern i32 qtyToTrade;
extern i32 leftResource;
extern i32 iTradeRatio;
extern i32 bTradeMade;
extern float fTradingPostEfficiency;
extern i32 tpX;
extern i32 tpY;
extern i32 bIsMarketPlace;
extern i32 bLeftDenominated;
extern class heroWindow* tpWindow;
extern i32 rightResource;
extern i32 iMaxUnitsToTrade;
extern i32 maxUnitsToTrade;

#endif
