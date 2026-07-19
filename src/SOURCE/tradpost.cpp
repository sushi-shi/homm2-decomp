#include <va.h>
#include <stdio.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/iconWidget.h>
#include <BASE/inputManager.h>
#include <BASE/mouseManager.h>
#include <BASE/widget.h>
#include <BASE/widgetKind.h>
#include <SOURCE/KB.h>
#include <SOURCE/PHILAI.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/kbwin.h>
#include <SOURCE/playerData.h>
#include <SOURCE/tradpost.h>
#include <BASE/message.h>

H2_ENUM_CLASS_BEGIN(TradingPostWidgetId)
    POST_LEFT_OFFER_ICON  = 0x14,
    POST_RIGHT_OFFER_ICON = 0x15,
    POST_LEFT_OFFER_TEXT  = 0x17,
    POST_RIGHT_OFFER_TEXT = 0x18,
    POST_DECREMENT        = 0x19,
    POST_INCREMENT        = 0x1a,
    POST_TRACK            = 0x1b,
    POST_EXECUTE          = 0x1d
H2_ENUM_CLASS_END(TradingPostWidgetId)

H2_ENUM_CLASS_BEGIN(TradingPostHandlerResult)
    POST_HANDLER_CONTINUE = 1,
    POST_HANDLER_EXIT     = 2
H2_ENUM_CLASS_END(TradingPostHandlerResult)

H2_ENUM_BEGIN(TradingPostPrivateConstant)
    KNOB_WIDTH       = 17,
    KNOB_HEIGHT      = 8,
    KNOB_FRAME       = 2,
    OFFER_SIDE_COUNT = 2,
    REDRAW_X_OFFSET  = 32,
    REDRAW_WIDTH     = 258,
    REDRAW_HEIGHT    = 418
H2_ENUM_END(TradingPostPrivateConstant)

VA(0x004bf340, 0x165)
void DoTradingPost(i32 isMarketplace, float efficiency) {
    tag_message messageTemp;

    bIsMarketPlace = isMarketplace;
    fTradingPostEfficiency = efficiency;
    bTradeMade = 0;
    tpX = TRADING_POST_WINDOW_X;
    tpY = TRADING_POST_WINDOW_Y;
    tpWindow = new heroWindow(tpX, tpY, "tradpost.bin");
    if (tpWindow == NULL)
        MemError();
    leftResource = -1;
    rightResource = -1;
    qtyToTrade = 0;
    tradeKnob = new iconWidget(
        TRADING_POST_KNOB_X,
        TRADING_POST_KNOB_Y,
        KNOB_WIDTH,
        KNOB_HEIGHT,
        "tradpost.icn",
        KNOB_FRAME,
        0,
        TRADING_POST_KNOB_ID,
        WIDGET_KIND_ICON_DIRECT,
        1
    );
    if (tradeKnob == NULL)
        MemError();
    tpWindow->AddWidget(tradeKnob, -1);
    UpdateTradingPost(0);
    gpWindowManager->DoDialog(tpWindow, TradingPostHandler, 0);
    delete tpWindow;
}

VA(0x004bf4a5, 0x596)
void UpdateTradingPost(i32 draw) {
    tag_message messageTemp;
    i32 ratioLocal;
    i32 maxTradeField;
    i32 leftDenominatedLocal;
    i32 offeredValue;
    i32 requestedValue;
    i32 sideCurrent;
    i32 resource;

    messageTemp.type = MESSAGE_WIDGET;
    if (leftResource != -1 && rightResource != -1 && leftResource != rightResource) {
        if (bLeftDenominated != 0) {
            requestedValue = 1;
            offeredValue = iTradeRatio;
        } else {
            requestedValue = iTradeRatio;
            offeredValue = 1;
        }
        sprintf(
            gText,
            "{%s}\n\nI can offer you %d %s of %s for %d %s of %s.",
            bIsMarketPlace != 0 ? "Marketplace" : "Trading Post",
            offeredValue,
            offeredValue > 1 ? "units" : "unit",
            gResourceNames[rightResource],
            requestedValue,
            requestedValue > 1 ? "units" : "unit",
            gResourceNames[leftResource]
        );
    } else if (bTradeMade != 0) {
        sprintf(
            gText,
            "{%s}\n\nYou have received quite a bargain.  I expect to make no profit on the deal.  "
            "Can I interest you in any of my other wares?",
            bIsMarketPlace != 0 ? "Marketplace" : "Trading Post"
        );
    } else {
        sprintf(
            gText,
            "{%s}\n\nPlease inspect our fine wares.  If you feel like offering a trade, click on "
            "the items you wish to trade with and for.",
            bIsMarketPlace != 0 ? "Marketplace" : "Trading Post"
        );
    }
    messageTemp.type = MESSAGE_WIDGET;
    messageTemp.payload.widget.command = TRADING_POST_SET_TEXT;
    messageTemp.payload.widget.id = 1;
    messageTemp.payload.widget.data.text = gText;
    tpWindow->BroadcastMessage(messageTemp);

    for (resource = TRADING_POST_CONTROL_FIRST; resource <= TRADING_POST_CONTROL_LAST; resource++) {
        if (leftResource != -1 && rightResource != -1 && leftResource != rightResource)
            messageTemp.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
        else
            messageTemp.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
        messageTemp.payload.widget.id = resource;
        messageTemp.payload.widget.data.value = WIDGET_FLAG_ENABLED | WIDGET_FLAG_DRAW;
        tpWindow->BroadcastMessage(messageTemp);
    }

    for (sideCurrent = 0; sideCurrent < OFFER_SIDE_COUNT; sideCurrent++) {
        if (leftResource != -1 && rightResource != -1 && leftResource != rightResource) {
            messageTemp.payload.widget.command = TRADING_POST_SET_ICON;
            if (sideCurrent == 0) {
                messageTemp.payload.widget.id = IDX(POST_LEFT_OFFER_ICON);
            } else {
                messageTemp.payload.widget.id = IDX(POST_RIGHT_OFFER_ICON);
            }
            if (sideCurrent == 0) {
                messageTemp.payload.widget.data.value = leftResource;
            } else {
                messageTemp.payload.widget.data.value = rightResource;
            }
            tpWindow->BroadcastMessage(messageTemp);
            messageTemp.payload.widget.command = TRADING_POST_SET_TEXT;
            messageTemp.payload.widget.data.text = gText;
            if (sideCurrent == 0) {
                messageTemp.payload.widget.id = IDX(POST_LEFT_OFFER_TEXT);
                if (bLeftDenominated != 0)
                    sprintf(gText, "%d", qtyToTrade);
                else
                    sprintf(gText, "%d", qtyToTrade * iTradeRatio);
            } else {
                messageTemp.payload.widget.id = IDX(POST_RIGHT_OFFER_TEXT);
                if (bLeftDenominated != 0)
                    sprintf(gText, "%d", qtyToTrade * iTradeRatio);
                else
                    sprintf(gText, "%d", qtyToTrade);
            }
            tpWindow->BroadcastMessage(messageTemp);
        }

        for (resource = 0; resource < TRADING_POST_RESOURCE_COUNT; resource++) {
            messageTemp.payload.widget.command = TRADING_POST_SET_TEXT;
            messageTemp.payload.widget.data.text = gText;
            if (sideCurrent == 0) {
                messageTemp.payload.widget.id = TRADING_POST_LEFT_TEXT_FIRST + resource;
                sprintf(gText, "%d", gpCurPlayer->m_resources[resource]);
            } else {
                messageTemp.payload.widget.id = TRADING_POST_RIGHT_TEXT_FIRST + resource;
                if (leftResource != -1) {
                    if (resource == leftResource) {
                        sprintf(gText, "n/a");
                    } else {
                        ComputeTradeRatios(
                            leftResource,
                            resource,
                            &ratioLocal,
                            &leftDenominatedLocal,
                            &maxTradeField
                        );
                        if (leftDenominatedLocal != 0)
                            sprintf(gText, "%d", ratioLocal);
                        else
                            sprintf(gText, "1/%d", ratioLocal);
                    }
                } else {
                    sprintf(gText, "");
                }
            }
            tpWindow->BroadcastMessage(messageTemp);
            if ((sideCurrent == 0 && leftResource == resource)
                || (sideCurrent == 1 && rightResource == resource))
                messageTemp.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
            else
                messageTemp.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
            if (sideCurrent == 0)
                messageTemp.payload.widget.id = TRADING_POST_LEFT_ICON_FIRST + resource;
            else
                messageTemp.payload.widget.id = TRADING_POST_RIGHT_ICON_FIRST + resource;
            messageTemp.payload.widget.data.value = WIDGET_FLAG_DRAW;
            tpWindow->BroadcastMessage(messageTemp);
        }
    }
    if (iMaxUnitsToTrade != 0)
        tradeKnob->m_x = static_cast<i16>(
            qtyToTrade * TRADING_POST_KNOB_TRAVEL / iMaxUnitsToTrade + TRADING_POST_KNOB_X
        );
    else
        tradeKnob->m_x = TRADING_POST_KNOB_X;
    if (draw != 0) {
        tpWindow->DrawWindow(0);
        gpWindowManager->UpdateScreenRegion(
            tpX + REDRAW_X_OFFSET,
            tpY,
            REDRAW_WIDTH,
            REDRAW_HEIGHT
        );
    }
}

VA(0x004bfa3b, 0xfe)
void ComputeTradeRatios(
    i32 sourceResource,
    i32 destinationResource,
    i32* ratio,
    i32* leftDenominated,
    i32* maxTrade
) {
    float sourceValueLocal = coreRatio[sourceResource] * fTradingPostEfficiency;
    float destinationValueValue = static_cast<u32>(coreRatio[destinationResource]);
    float tradeRatio = destinationValueValue / sourceValueLocal;

    if (tradeRatio >= 1.0f) {
        *leftDenominated = 0;
        *ratio = static_cast<i32>(tradeRatio + 0.999); // NOLINT(readability-magic-numbers)
        *maxTrade = gpCurPlayer->m_resources[sourceResource] / *ratio;
    } else {
        *leftDenominated = 1;
        *ratio = static_cast<i32>(1.0f / tradeRatio);
        *maxTrade = gpCurPlayer->m_resources[sourceResource];
    }
}

VA(0x004bfb39, 0x148)
void DoTradeKnob(struct tag_message message) {
    tag_message nextMessage;
    i32 knobPosition;

    while (message.type != MESSAGE_LEFT_BUTTON_UP && message.type != MESSAGE_RIGHT_BUTTON_UP) {
        if (message.type == MESSAGE_MOUSE_MOVE) {
            knobPosition = message.payload.mouse.screenX - tpX - TRADING_POST_KNOB_TRACK_OFFSET;
            qtyToTrade = (iMaxUnitsToTrade + 1) * knobPosition / TRADING_POST_KNOB_TRACK_WIDTH;
            if (qtyToTrade < 0)
                qtyToTrade = 0;
            if (qtyToTrade > iMaxUnitsToTrade)
                qtyToTrade = iMaxUnitsToTrade;
            gpMouseManager->Main(message);
            UpdateTradingPost(1);
        }
        Process1WindowsMessage();
        message = gpInputManager->GetEvent();
        if (message.type == MESSAGE_MOUSE_MOVE) {
            nextMessage = gpInputManager->PeekEvent();
            while (nextMessage.type == MESSAGE_MOUSE_MOVE) {
                message = gpInputManager->GetEvent();
                nextMessage = gpInputManager->PeekEvent();
            }
        }
    }
    UpdateTradingPost(1);
}

VA(0x004bfc81, 0x3a)
void SetupNewTrade(void) {
    qtyToTrade = 0;
    ComputeTradeRatios(
        leftResource,
        rightResource,
        &iTradeRatio,
        &bLeftDenominated,
        &iMaxUnitsToTrade
    );
}

VA(0x004bfcbb, 0x3b6)
i32 TradingPostHandler(struct tag_message& message) {
    i32 exitFlag = 0;
    i32 updateDisplay = 0;
    i32 resourceData;
    i32 knobPositionValue;

    if (message.type == MESSAGE_WIDGET) {
        switch (message.payload.widget.command) {
            case WIDGET_COMMAND_SELECT:
                switch (message.payload.widget.id) {
                    case IDX(POST_TRACK):
                        if (iMaxUnitsToTrade == 0) {
                        } else {
                            knobPositionValue = message.payload.widget.screenX - tpX
                                                - TRADING_POST_KNOB_TRACK_OFFSET;
                            qtyToTrade = (iMaxUnitsToTrade + 1) * knobPositionValue
                                         / TRADING_POST_KNOB_TRACK_WIDTH;
                            if (qtyToTrade < 0)
                                qtyToTrade = 0;
                            if (iMaxUnitsToTrade < qtyToTrade)
                                qtyToTrade = iMaxUnitsToTrade;
                            updateDisplay = 1;
                        }
                        break;
                    case TRADING_POST_KNOB_ID:
                        DoTradeKnob(message);
                        break;
                    case TRADING_POST_LEFT_SELECT_FIRST + IDX(RES_WOOD):
                    case TRADING_POST_LEFT_SELECT_FIRST + IDX(RES_MERCURY):
                    case TRADING_POST_LEFT_SELECT_FIRST + IDX(RES_ORE):
                    case TRADING_POST_LEFT_SELECT_FIRST + IDX(RES_SULFUR):
                    case TRADING_POST_LEFT_SELECT_FIRST + IDX(RES_CRYSTAL):
                    case TRADING_POST_LEFT_SELECT_FIRST + IDX(RES_GEMS):
                    case TRADING_POST_LEFT_SELECT_FIRST + IDX(RES_GOLD):
                        resourceData = message.payload.widget.id - TRADING_POST_LEFT_SELECT_FIRST;
                        if (leftResource != resourceData) {
                            leftResource = resourceData;
                            updateDisplay = 1;
                            SetupNewTrade();
                        }
                        break;
                    case TRADING_POST_RIGHT_SELECT_FIRST + IDX(RES_WOOD):
                    case TRADING_POST_RIGHT_SELECT_FIRST + IDX(RES_MERCURY):
                    case TRADING_POST_RIGHT_SELECT_FIRST + IDX(RES_ORE):
                    case TRADING_POST_RIGHT_SELECT_FIRST + IDX(RES_SULFUR):
                    case TRADING_POST_RIGHT_SELECT_FIRST + IDX(RES_CRYSTAL):
                    case TRADING_POST_RIGHT_SELECT_FIRST + IDX(RES_GEMS):
                    case TRADING_POST_RIGHT_SELECT_FIRST + IDX(RES_GOLD):
                        resourceData = message.payload.widget.id - TRADING_POST_RIGHT_SELECT_FIRST;
                        if (rightResource != resourceData) {
                            rightResource = resourceData;
                            updateDisplay = 1;
                            if (leftResource != -1)
                                SetupNewTrade();
                        }
                        break;
                }
                break;
            case WIDGET_COMMAND_DESELECT:
                switch (message.payload.widget.id) {
                    case NORMAL_DIALOG_BUTTON_TWO:
                        exitFlag = 1;
                        break;
                    case IDX(POST_EXECUTE):
                        if (qtyToTrade == 0)
                            break;
                        if (bLeftDenominated != 0) {
                            gpCurPlayer->m_resources[leftResource] -= qtyToTrade;
                            gpCurPlayer->m_resources[rightResource] += qtyToTrade * iTradeRatio;
                        } else {
                            gpCurPlayer->m_resources[leftResource] -= qtyToTrade * iTradeRatio;
                            gpCurPlayer->m_resources[rightResource] += qtyToTrade;
                        }
                        bTradeMade = 1;
                        rightResource = -1;
                        leftResource = rightResource;
                        updateDisplay = 1;
                        break;
                    case IDX(POST_DECREMENT):
                        if (qtyToTrade > 0) {
                            --qtyToTrade;
                            updateDisplay = 1;
                        }
                        break;
                    case IDX(POST_INCREMENT):
                        if (iMaxUnitsToTrade > qtyToTrade) {
                            ++qtyToTrade;
                            updateDisplay = 1;
                        }
                        break;
                }
                break;
        }
    }
    if (updateDisplay)
        UpdateTradingPost(1);
    if (exitFlag) {
        message.payload.widget.id = WIDGET_COMMAND_DIALOG_SELECT;
        message.payload.widget.command = BaseWidgetCommand(message.payload.widget.id);
        return IDX(POST_HANDLER_EXIT);
    }
    return IDX(POST_HANDLER_CONTINUE);
}

DATA(0x0051d9d0) u16 coreRatio[TRADING_POST_RESOURCE_COUNT] = {250, 500, 250, 500, 500, 500, 1};
DATA(0x00533170) class iconWidget* tradeKnob;
DATA(0x00533174) i32 qtyToTrade;
DATA(0x00533178) i32 leftResource;
DATA(0x0053317c) i32 iTradeRatio;
DATA(0x00533180) i32 bTradeMade;
DATA(0x00533184) float fTradingPostEfficiency;
DATA(0x00533188) i32 tpX;
DATA(0x0053318c) i32 tpY;
DATA(0x00533190) i32 bIsMarketPlace;
DATA(0x00533194) i32 bLeftDenominated;
DATA(0x00533198) class heroWindow* tpWindow;
DATA(0x0053319c) i32 rightResource;
DATA(0x005331a0) i32 iMaxUnitsToTrade;
DATA(0x005331a4) i32 maxUnitsToTrade;
