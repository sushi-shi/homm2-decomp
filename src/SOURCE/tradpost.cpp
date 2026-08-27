#include <Ints.h>
#include <stdio.h>
#include <string.h>
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

typedef enum TradingPostWidgetId {
    POST_LEFT_OFFER_ICON  = 0x14,
    POST_RIGHT_OFFER_ICON = 0x15,
    POST_LEFT_OFFER_TEXT  = 0x17,
    POST_RIGHT_OFFER_TEXT = 0x18,
    POST_DECREMENT        = 0x19,
    POST_INCREMENT        = 0x1a,
    POST_TRACK            = 0x1b,
    POST_EXECUTE          = 0x1d
} TradingPostWidgetId;

enum {
    OFFER_LEFT  = 0,
    OFFER_RIGHT = 1,
    OFFER_COUNT = 2
};
typedef i32 OfferSide;

typedef enum TradingPostPrivateConstant {
    KNOB_WIDTH      = 17,
    KNOB_HEIGHT     = 8,
    KNOB_FRAME      = 2,
    REDRAW_X_OFFSET = 32,
    REDRAW_WIDTH    = 258,
    REDRAW_HEIGHT   = 418,
    OFFER_NAME_SIZE = 52
} TradingPostPrivateConstant;

typedef enum Cp1251Case {
    CP1251_CASE_DELTA  = 0x20,
    CP1251_UPPER_FIRST = 0xc0,
    CP1251_UPPER_LAST  = 0xdf,
    CP1251_UPPER_YO    = 0xa8,
    CP1251_LOWER_YO    = 0xb8
} Cp1251Case;


float fTradingPostEfficiency = 0.0f;
i32 bLeftDenominated = 0;
i32 leftResource = 0;
i32 iMaxUnitsToTrade = 0;
i32 tpX = 0;
i32 tpY = 0;
b32 bTradeMade = false;
static char leftName[OFFER_NAME_SIZE] = {};
i32 qtyToTrade = 0;
static char rightName[OFFER_NAME_SIZE] = {};
i32 iTradeRatio = 0;
i32 rightResource = 0;
i32 maxUnitsToTrade = 0;
i32 bIsMarketPlace = 0;
class heroWindow* tpWindow = NULL;
class iconWidget* tradeKnob = NULL;

void DoTradingPost(i32 isMarketplace, float efficiency) {
    tag_message messageTemp [[maybe_unused]];

    bIsMarketPlace = isMarketplace;
    fTradingPostEfficiency = efficiency;
    bTradeMade = false;
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
        ICON_DRAW_NORMAL,
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

void UpdateTradingPost(i32 draw) {
    tag_message messageTemp;
    i32 idx;
    OfferSide sideCurrent;
    i32 offeredValue;
    i32 requestedValue;
    i32 leftDenominatedLocal;
    i32 nMax;
    i32 ratioLocal;
    char chr1;
    char chr2;

    messageTemp.type = MESSAGE_WIDGET;
    if (leftResource != -1 && rightResource != -1 && leftResource != rightResource) {
        if (bLeftDenominated != 0) {
            requestedValue = 1;
            offeredValue = iTradeRatio;
        } else {
            requestedValue = iTradeRatio;
            offeredValue = 1;
        }
        strcpy(rightName, gResourceNames[rightResource]);
        if (static_cast<u8>(rightName[0]) >= 'A' && static_cast<u8>(rightName[0]) <= 'Z')
            chr1 = static_cast<char>(static_cast<u8>(rightName[0]) + CP1251_CASE_DELTA);
        else if (static_cast<u8>(rightName[0]) >= CP1251_UPPER_FIRST
                 && static_cast<u8>(rightName[0]) <= CP1251_UPPER_LAST)
            chr1 = static_cast<char>(static_cast<u8>(rightName[0]) + CP1251_CASE_DELTA);
        else if (static_cast<u8>(rightName[0]) == CP1251_UPPER_YO)
            chr1 = static_cast<char>(CP1251_LOWER_YO);
        else
            chr1 = rightName[0];
        rightName[0] = chr1;

        strcpy(leftName, gResourceNames[leftResource]);
        if (static_cast<u8>(leftName[0]) >= 'A' && static_cast<u8>(leftName[0]) <= 'Z')
            chr2 = static_cast<char>(static_cast<u8>(leftName[0]) + CP1251_CASE_DELTA);
        else if (static_cast<u8>(leftName[0]) >= CP1251_UPPER_FIRST
                 && static_cast<u8>(leftName[0]) <= CP1251_UPPER_LAST)
            chr2 = static_cast<char>(static_cast<u8>(leftName[0]) + CP1251_CASE_DELTA);
        else if (static_cast<u8>(leftName[0]) == CP1251_UPPER_YO)
            chr2 = static_cast<char>(CP1251_LOWER_YO);
        else
            chr2 = leftName[0];
        leftName[0] = chr2;

        sprintf(
            gText,
            "{%s}\n\nНа моем рынке %s и %s "
            "меняются из соотношения "
            "%d %s к %d %s"  ,
            bIsMarketPlace != 0 ? "Рынок"
                                : "Рынок"  ,
            rightName,
            leftName,
            offeredValue,
            offeredValue > 1 ? "ед."   : "ед."  ,
            requestedValue,
            requestedValue > 1 ? "ед."   : "ед."
        );
    } else if (bTradeMade != 0) {
        sprintf(
            gText,
            "{%s}\n\nВам предложена "
            "достойная сделка. Я не "
            "пытаюсь нажиться на ней. "
            "Вас интересует что-нибудь "
            "из моих товаров?"  ,
            bIsMarketPlace != 0 ? "Рынок"
                                : "Рынок"
        );
    } else {
        sprintf(
            gText,
            "{%s}\n\nПосмотрите на наши "
            "товары. Если что-то вас "
            "заинтересует, щелкните "
            "по нужным вещам и выберите, "
            "на что хотите поменять."  ,
            bIsMarketPlace != 0 ? "Рынок"
                                : "Рынок"
        );
    }
    messageTemp.type = MESSAGE_WIDGET;
    messageTemp.payload.widget.command = TRADING_POST_SET_TEXT;
    messageTemp.payload.widget.id = 1;
    messageTemp.payload.widget.data.text = gText;
    tpWindow->BroadcastMessage(messageTemp);

    for (idx = TRADING_POST_CONTROL_FIRST; idx <= TRADING_POST_CONTROL_LAST; idx++) {
        messageTemp.payload.widget.command =
            leftResource != -1 && rightResource != -1 && leftResource != rightResource
                ? WIDGET_COMMAND_SET_FLAGS
                : WIDGET_COMMAND_CLEAR_FLAGS;
        messageTemp.payload.widget.id = idx;
        messageTemp.payload.widget.data.value = (WIDGET_FLAG_ENABLED | WIDGET_FLAG_DRAW);
        tpWindow->BroadcastMessage(messageTemp);
    }

    for (sideCurrent = OFFER_LEFT; sideCurrent < OFFER_COUNT; sideCurrent++) {
        if (leftResource != -1 && rightResource != -1 && leftResource != rightResource) {
            messageTemp.payload.widget.command = TRADING_POST_SET_ICON;
            messageTemp.payload.widget.id =
                sideCurrent == OFFER_LEFT ? POST_LEFT_OFFER_ICON : POST_RIGHT_OFFER_ICON;
            messageTemp.payload.widget.data.value =
                sideCurrent == OFFER_LEFT ? leftResource : rightResource;
            tpWindow->BroadcastMessage(messageTemp);
            messageTemp.payload.widget.command = TRADING_POST_SET_TEXT;
            messageTemp.payload.widget.data.text = gText;
            if (sideCurrent == OFFER_LEFT) {
                messageTemp.payload.widget.id = POST_LEFT_OFFER_TEXT;
                if (bLeftDenominated != 0)
                    sprintf(gText, "%d", qtyToTrade);
                else
                    sprintf(gText, "%d", qtyToTrade * iTradeRatio);
            } else {
                messageTemp.payload.widget.id = POST_RIGHT_OFFER_TEXT;
                if (bLeftDenominated != 0)
                    sprintf(gText, "%d", qtyToTrade * iTradeRatio);
                else
                    sprintf(gText, "%d", qtyToTrade);
            }
            tpWindow->BroadcastMessage(messageTemp);
        }

        for (idx = 0; idx < TRADING_POST_RESOURCE_COUNT; idx++) {
            messageTemp.payload.widget.command = TRADING_POST_SET_TEXT;
            messageTemp.payload.widget.data.text = gText;
            if (sideCurrent == OFFER_LEFT) {
                messageTemp.payload.widget.id = TRADING_POST_LEFT_TEXT_FIRST + idx;
                sprintf(gText, "%d", gpCurPlayer->m_resources[idx]);
            } else {
                messageTemp.payload.widget.id = TRADING_POST_RIGHT_TEXT_FIRST + idx;
                if (leftResource != -1) {
                    if (leftResource == idx) {
                        sprintf(gText, "н/д"  );
                    } else {
                        ComputeTradeRatios(
                            leftResource,
                            idx,
                            &ratioLocal,
                            &leftDenominatedLocal,
                            &nMax
                        );
                        if (leftDenominatedLocal != 0)
                            sprintf(gText, "%d", ratioLocal);
                        else
                            sprintf(gText, "1/%d", ratioLocal);
                    }
                } else {
                    sprintf(
                        gText,
                        ""
                    );
                }
            }
            tpWindow->BroadcastMessage(messageTemp);
            if ((sideCurrent == OFFER_LEFT && leftResource == idx)
                || (sideCurrent == OFFER_RIGHT && rightResource == idx))
                messageTemp.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
            else
                messageTemp.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
            messageTemp.payload.widget.id = sideCurrent == OFFER_LEFT
                                                ? TRADING_POST_LEFT_ICON_FIRST + idx
                                                : TRADING_POST_RIGHT_ICON_FIRST + idx;
            messageTemp.payload.widget.data.value = (WIDGET_FLAG_DRAW);
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

void ComputeTradeRatios(
    i32 sourceResource,
    i32 destinationResource,
    i32* ratio,
    i32* leftDenominated,
    i32* maxTrade
) {
    float srcVal = coreRatio[sourceResource] * fTradingPostEfficiency;
    float dstVal = coreRatio[destinationResource];
    float tRatio = dstVal / srcVal;

    if (tRatio >= 1.0f) {
        *leftDenominated = 0;
        *ratio = static_cast<i32>(tRatio + 0.999);
        *maxTrade = gpCurPlayer->m_resources[sourceResource] / *ratio;
    } else {
        *leftDenominated = 1;
        *ratio = static_cast<i32>(1.0f / tRatio);
        *maxTrade = gpCurPlayer->m_resources[sourceResource];
    }
}

void DoTradeKnob(struct tag_message message) {
    tag_message nextMessage;
    i32 knobPosition;

    while (message.type != MESSAGE_LEFT_BUTTON_UP && message.type != MESSAGE_RIGHT_BUTTON_UP) {
        if (message.type == MESSAGE_MOUSE_MOVE) {
            knobPosition = message.payload.mouse.screenX - tpX - TRADING_POST_KNOB_TRACK_OFFSET;
            qtyToTrade = knobPosition * (iMaxUnitsToTrade + 1) / TRADING_POST_KNOB_TRACK_WIDTH;
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

MessageDispatchResult TradingPostHandler(struct tag_message& message) {
    b32 exitFlag = false;
    b32 redraw = false;
    i32 resourceData;
    i32 knobPosition;

    if (message.type == MESSAGE_WIDGET) {
        switch (message.payload.widget.command) {
            case WIDGET_COMMAND_SELECT:
                switch (message.payload.widget.id) {
                    case POST_TRACK:
                        if (iMaxUnitsToTrade == 0)
                            break;
                        knobPosition =
                            message.payload.widget.screenX - tpX - TRADING_POST_KNOB_TRACK_OFFSET;
                        qtyToTrade =
                            knobPosition * (iMaxUnitsToTrade + 1) / TRADING_POST_KNOB_TRACK_WIDTH;
                        if (qtyToTrade < 0)
                            qtyToTrade = 0;
                        if (qtyToTrade > iMaxUnitsToTrade)
                            qtyToTrade = iMaxUnitsToTrade;
                        redraw = true;
                        break;
                    case TRADING_POST_KNOB_ID:
                        DoTradeKnob(message);
                        break;
                    case TRADING_POST_LEFT_SELECT_FIRST + (RES_WOOD):
                    case TRADING_POST_LEFT_SELECT_FIRST + (RES_MERCURY):
                    case TRADING_POST_LEFT_SELECT_FIRST + (RES_ORE):
                    case TRADING_POST_LEFT_SELECT_FIRST + (RES_SULFUR):
                    case TRADING_POST_LEFT_SELECT_FIRST + (RES_CRYSTAL):
                    case TRADING_POST_LEFT_SELECT_FIRST + (RES_GEMS):
                    case TRADING_POST_LEFT_SELECT_FIRST + (RES_GOLD):
                        resourceData = message.payload.widget.id - TRADING_POST_LEFT_SELECT_FIRST;
                        if (resourceData != leftResource) {
                            leftResource = resourceData;
                            redraw = true;
                            SetupNewTrade();
                        }
                        break;
                    case TRADING_POST_RIGHT_SELECT_FIRST + (RES_WOOD):
                    case TRADING_POST_RIGHT_SELECT_FIRST + (RES_MERCURY):
                    case TRADING_POST_RIGHT_SELECT_FIRST + (RES_ORE):
                    case TRADING_POST_RIGHT_SELECT_FIRST + (RES_SULFUR):
                    case TRADING_POST_RIGHT_SELECT_FIRST + (RES_CRYSTAL):
                    case TRADING_POST_RIGHT_SELECT_FIRST + (RES_GEMS):
                    case TRADING_POST_RIGHT_SELECT_FIRST + (RES_GOLD):
                        resourceData = message.payload.widget.id - TRADING_POST_RIGHT_SELECT_FIRST;
                        if (resourceData != rightResource) {
                            rightResource = resourceData;
                            redraw = true;
                            if (leftResource != -1)
                                SetupNewTrade();
                        }
                        break;
                }
                break;
            case WIDGET_COMMAND_DESELECT:
                switch (message.payload.widget.id) {
                    case NORMAL_DIALOG_BUTTON_TWO:
                        exitFlag = true;
                        break;
                    case POST_EXECUTE:
                        if (qtyToTrade == 0)
                            break;
                        if (bLeftDenominated != 0) {
                            gpCurPlayer->m_resources[leftResource] -= qtyToTrade;
                            gpCurPlayer->m_resources[rightResource] += qtyToTrade * iTradeRatio;
                        } else {
                            gpCurPlayer->m_resources[leftResource] -= qtyToTrade * iTradeRatio;
                            gpCurPlayer->m_resources[rightResource] += qtyToTrade;
                        }
                        bTradeMade = true;
                        rightResource = -1;
                        leftResource = rightResource;
                        redraw = true;
                        break;
                    case POST_DECREMENT:
                        if (qtyToTrade > 0) {
                            --qtyToTrade;
                            redraw = true;
                        }
                        break;
                    case POST_INCREMENT:
                        if (qtyToTrade < iMaxUnitsToTrade) {
                            ++qtyToTrade;
                            redraw = true;
                        }
                        break;
                }
                break;
        }
    }
    if (redraw)
        UpdateTradingPost(1);
    if (exitFlag) {
        message.payload.widget.id = (WIDGET_COMMAND_DIALOG_SELECT);
        message.payload.widget.command = WIDGET_COMMAND_DIALOG_SELECT;
        return MESSAGE_DISPATCH_FORWARD;
    }
    return MESSAGE_DISPATCH_CONSUME;
}

u16 coreRatio[TRADING_POST_RESOURCE_COUNT] = {250, 500, 250, 500, 500, 500, 1};
