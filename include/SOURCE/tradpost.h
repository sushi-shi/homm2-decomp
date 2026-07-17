#ifndef HOMM2_TRADPOST_H
#define HOMM2_TRADPOST_H

#include <Ints.h>
// Declarations of the free functions DEFINED in tradpost.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).
// forward declarations (was <_all.h>):
struct tag_message;

H2_ENUM_BEGIN(TradingPostConstant)
    TRADING_POST_RESOURCE_COUNT = 7,
    TRADING_POST_WINDOW_X = 0x9f,
    TRADING_POST_WINDOW_Y = 0x17,
    TRADING_POST_KNOB_X = 0x43,
    TRADING_POST_KNOB_Y = 0xcc,
    TRADING_POST_KNOB_ID = 0x1f,
    TRADING_POST_KNOB_TRACK_OFFSET = 0x4b,
    TRADING_POST_KNOB_TRACK_WIDTH = 0xbb,
    TRADING_POST_KNOB_TRAVEL = 0xaa,
    TRADING_POST_CONTROL_FIRST = 0x14,
    TRADING_POST_CONTROL_LAST = 0x1f,
    TRADING_POST_LEFT_ICON_FIRST = 0x32,
    TRADING_POST_LEFT_TEXT_FIRST = 0x3c,
    TRADING_POST_LEFT_SELECT_FIRST = 0x46,
    TRADING_POST_RIGHT_ICON_FIRST = 0x5a,
    TRADING_POST_RIGHT_TEXT_FIRST = 0x64,
    TRADING_POST_RIGHT_SELECT_FIRST = 0x6e
H2_ENUM_END(TradingPostConstant)

H2_ENUM_CLASS_BEGIN(TradingPostWidgetId)
    TRADING_POST_LEFT_OFFER_ICON = 0x14,
    TRADING_POST_RIGHT_OFFER_ICON = 0x15,
    TRADING_POST_LEFT_OFFER_TEXT = 0x17,
    TRADING_POST_RIGHT_OFFER_TEXT = 0x18,
    TRADING_POST_DECREMENT = 0x19,
    TRADING_POST_INCREMENT = 0x1a,
    TRADING_POST_TRACK = 0x1b,
    TRADING_POST_EXECUTE = 0x1d
H2_ENUM_CLASS_END(TradingPostWidgetId)

H2_ENUM_CLASS_BEGIN(TradingPostWidgetCommand)
    TRADING_POST_SET_TEXT = 3,
    TRADING_POST_SET_ICON = 4
H2_ENUM_CLASS_END(TradingPostWidgetCommand)

H2_ENUM_CLASS_BEGIN(TradingPostHandlerResult)
    TRADING_POST_HANDLER_CONTINUE = 1,
    TRADING_POST_HANDLER_EXIT = 2
H2_ENUM_CLASS_END(TradingPostHandlerResult)

void DoTradingPost(i32, float);
void UpdateTradingPost(i32);
void ComputeTradeRatios(i32, i32, i32*, i32*, i32*);
void DoTradeKnob(struct tag_message);
void SetupNewTrade(void);
i32 TradingPostHandler(struct tag_message&);

// ---- globals (declarations, RVA order) ----
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

#endif // HOMM2_TRADPOST_H
