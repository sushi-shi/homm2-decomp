// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\tradpost.obj   from: (directly linked into exe)
// functions: 6   data: 15
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <stdio.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/iconWidget.h>
#include <BASE/inputManager.h>
#include <BASE/mouseManager.h>
#include <BASE/widget.h>
#include <SOURCE/KB.h>
#include <SOURCE/PHILAI.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/kbwin.h>
#include <SOURCE/playerData.h>
#include <SOURCE/tradpost.h>
#include <_carcass_types.h>

VA(0x004bf340, 0x165)
void DoTradingPost(int isMarketplace, float efficiency) {
    fTradingPostEfficiency = efficiency;
    bTradeMade = 0;
    tpX = TRADING_POST_WINDOW_X;
    tpY = TRADING_POST_WINDOW_Y;
    bIsMarketPlace = isMarketplace;
    tpWindow = new heroWindow(tpX, tpY, "tradpost.bin");
    if (tpWindow == 0)
        MemError();
    leftResource = -1;
    rightResource = -1;
    qtyToTrade = 0;
    tradeKnob = new iconWidget(
        TRADING_POST_KNOB_X, TRADING_POST_KNOB_Y, 0x11, 8,
        "tradpost.icn", 2, 0, TRADING_POST_KNOB_ID, 0x10, 1);
    if (tradeKnob == 0)
        MemError();
    tpWindow->AddWidget(tradeKnob, -1);
    UpdateTradingPost(0);
    gpWindowManager->DoDialog(tpWindow, TradingPostHandler, 0);
    delete tpWindow;
}

VA(0x004bf4a5, 0x596)
void UpdateTradingPost(int draw) {
    tag_message message;
    int ratio;
    int maxTrade;
    int leftDenominated;
    int offered;
    int requested;
    int side;
    int resource;

    message.type = MESSAGE_WIDGET;
    if (leftResource != -1 && rightResource != -1 &&
        rightResource != leftResource) {
        if (bLeftDenominated != 0) {
            requested = 1;
            offered = iTradeRatio;
        } else {
            requested = iTradeRatio;
            offered = 1;
        }
        sprintf(gText,
                "{%s}\n\nI can offer you %d %s of %s for %d %s of %s.",
                bIsMarketPlace != 0 ? "Marketplace" : "Trading Post",
                offered, offered > 1 ? "units" : "unit",
                gResourceNames[rightResource], requested,
                requested > 1 ? "units" : "unit",
                gResourceNames[leftResource]);
    } else if (bTradeMade != 0) {
        sprintf(gText,
                "{%s}\n\nYou have received quite a bargain.  I expect to make no profit on the deal.  Can I interest you in any of my other wares?",
                bIsMarketPlace != 0 ? "Marketplace" : "Trading Post");
    } else {
        sprintf(gText,
                "{%s}\n\nPlease inspect our fine wares.  If you feel like offering a trade, click on the items you wish to trade with and for.",
                bIsMarketPlace != 0 ? "Marketplace" : "Trading Post");
    }
    message.payload.widget.command = TRADING_POST_SET_TEXT;
    message.payload.widget.id = 1;
    message.payload.widget.data.text = gText;
    tpWindow->BroadcastMessage(message);

    for (resource = TRADING_POST_CONTROL_FIRST;
         resource <= TRADING_POST_CONTROL_LAST; resource++) {
        if (leftResource != -1 && rightResource != -1 &&
            rightResource != leftResource)
            message.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
        else
            message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
        message.payload.widget.id = resource;
        message.payload.widget.data.value = WIDGET_FLAG_ENABLED;
        tpWindow->BroadcastMessage(message);
    }

    for (side = 0; side < 2; side++) {
        if (leftResource != -1 && rightResource != -1 &&
            rightResource != leftResource) {
            message.payload.widget.command = TRADING_POST_SET_ICON;
            if (side == 0) {
                message.payload.widget.id = TRADING_POST_LEFT_OFFER_ICON;
                message.payload.widget.data.value = leftResource;
            } else {
                message.payload.widget.id = TRADING_POST_RIGHT_OFFER_ICON;
                message.payload.widget.data.value = rightResource;
            }
            tpWindow->BroadcastMessage(message);
            message.payload.widget.command = TRADING_POST_SET_TEXT;
            message.payload.widget.data.text = gText;
            if (side == 0) {
                message.payload.widget.id = TRADING_POST_LEFT_OFFER_TEXT;
                if (bLeftDenominated != 0)
                    sprintf(gText, "%d", qtyToTrade);
                else
                    sprintf(gText, "%d", qtyToTrade * iTradeRatio);
            } else {
                message.payload.widget.id = TRADING_POST_RIGHT_OFFER_TEXT;
                if (bLeftDenominated != 0)
                    sprintf(gText, "%d", qtyToTrade * iTradeRatio);
                else
                    sprintf(gText, "%d", qtyToTrade);
            }
            tpWindow->BroadcastMessage(message);
        }

        for (resource = 0; resource < TRADING_POST_RESOURCE_COUNT; resource++) {
            message.payload.widget.command = TRADING_POST_SET_TEXT;
            message.payload.widget.data.text = gText;
            if (side == 0) {
                message.payload.widget.id =
                    TRADING_POST_LEFT_TEXT_FIRST + resource;
                sprintf(gText, "%d", gpCurPlayer->m_resources[resource]);
            } else {
                message.payload.widget.id =
                    TRADING_POST_RIGHT_TEXT_FIRST + resource;
                if (leftResource != -1) {
                    if (resource == leftResource) {
                        sprintf(gText, "n/a");
                    } else {
                        ComputeTradeRatios(leftResource, resource, &ratio,
                                           &leftDenominated, &maxTrade);
                        if (leftDenominated != 0)
                            sprintf(gText, "%d", ratio);
                        else
                            sprintf(gText, "1/%d", ratio);
                    }
                } else {
                    sprintf(gText, "");
                }
            }
            tpWindow->BroadcastMessage(message);
            if ((side == 0 && resource == leftResource) ||
                (side == 1 && resource == rightResource))
                message.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
            else
                message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
            if (side == 0)
                message.payload.widget.id =
                    TRADING_POST_LEFT_ICON_FIRST + resource;
            else
                message.payload.widget.id =
                    TRADING_POST_RIGHT_ICON_FIRST + resource;
            message.payload.widget.data.value = WIDGET_FLAG_DRAW;
            tpWindow->BroadcastMessage(message);
        }
    }
    if (iMaxUnitsToTrade == 0)
        tradeKnob->m_x = TRADING_POST_KNOB_X;
    else
        tradeKnob->m_x = static_cast<short>(
            qtyToTrade * TRADING_POST_KNOB_TRAVEL / iMaxUnitsToTrade +
            TRADING_POST_KNOB_X);
    if (draw != 0) {
        tpWindow->DrawWindow(0);
        gpWindowManager->UpdateScreenRegion(tpX + 0x20, tpY, 0x102, 0x1a2);
    }
}

VA(0x004bfa3b, 0xfe)
void ComputeTradeRatios(int sourceResource, int destinationResource, int *ratio,
                        int *leftDenominated, int *maxTrade) {
    float sourceValue = coreRatio[sourceResource] * fTradingPostEfficiency;
    float destinationValue =
        static_cast<unsigned int>(coreRatio[destinationResource]);
    float tradeRatio = destinationValue / sourceValue;

    if (tradeRatio >= 1.0f) {
        *leftDenominated = 0;
        *ratio = static_cast<int>(tradeRatio + 0.999);
        *maxTrade = gpCurPlayer->m_resources[sourceResource] / *ratio;
    } else {
        *leftDenominated = 1;
        *ratio = static_cast<int>(1.0f / tradeRatio);
        *maxTrade = gpCurPlayer->m_resources[sourceResource];
    }
}

VA(0x004bfb39, 0x148)
void DoTradeKnob(struct tag_message message) {
    tag_message nextMessage;
    int knobPosition;

    while (message.type != MESSAGE_LEFT_BUTTON_UP &&
           message.type != MESSAGE_RIGHT_BUTTON_UP) {
        if (message.type == MESSAGE_MOUSE_MOVE) {
            knobPosition = message.payload.mouse.screenX - tpX -
                           TRADING_POST_KNOB_TRACK_OFFSET;
            qtyToTrade = (iMaxUnitsToTrade + 1) * knobPosition /
                         TRADING_POST_KNOB_TRACK_WIDTH;
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
    ComputeTradeRatios(leftResource, rightResource, &iTradeRatio,
                       &bLeftDenominated, &iMaxUnitsToTrade);
}

VA(0x004bfcbb, 0x3b6)
int TradingPostHandler(struct tag_message &message) {
    int exitDialog = 0;
    int updateDisplay = 0;
    int resource;

    if (message.type == MESSAGE_WIDGET) {
        if (message.payload.widget.command == WIDGET_COMMAND_SELECT) {
            switch (message.payload.widget.id) {
            case TRADING_POST_TRACK:
                if (iMaxUnitsToTrade != 0) {
                    qtyToTrade = (iMaxUnitsToTrade + 1) *
                        (message.payload.widget.screenX - tpX -
                         TRADING_POST_KNOB_TRACK_OFFSET) /
                        TRADING_POST_KNOB_TRACK_WIDTH;
                    if (qtyToTrade < 0)
                        qtyToTrade = 0;
                    if (qtyToTrade > iMaxUnitsToTrade)
                        qtyToTrade = iMaxUnitsToTrade;
                    updateDisplay = 1;
                }
                break;
            case TRADING_POST_KNOB_ID:
                DoTradeKnob(message);
                break;
            case TRADING_POST_LEFT_SELECT_FIRST:
            case TRADING_POST_LEFT_SELECT_FIRST + 1:
            case TRADING_POST_LEFT_SELECT_FIRST + 2:
            case TRADING_POST_LEFT_SELECT_FIRST + 3:
            case TRADING_POST_LEFT_SELECT_FIRST + 4:
            case TRADING_POST_LEFT_SELECT_FIRST + 5:
            case TRADING_POST_LEFT_SELECT_FIRST + 6:
                updateDisplay = message.payload.widget.id -
                                TRADING_POST_LEFT_SELECT_FIRST != leftResource;
                if (updateDisplay) {
                    leftResource = message.payload.widget.id -
                                   TRADING_POST_LEFT_SELECT_FIRST;
                    SetupNewTrade();
                }
                break;
            case TRADING_POST_RIGHT_SELECT_FIRST:
            case TRADING_POST_RIGHT_SELECT_FIRST + 1:
            case TRADING_POST_RIGHT_SELECT_FIRST + 2:
            case TRADING_POST_RIGHT_SELECT_FIRST + 3:
            case TRADING_POST_RIGHT_SELECT_FIRST + 4:
            case TRADING_POST_RIGHT_SELECT_FIRST + 5:
            case TRADING_POST_RIGHT_SELECT_FIRST + 6:
                resource = message.payload.widget.id -
                           TRADING_POST_RIGHT_SELECT_FIRST;
                if (resource != rightResource) {
                    rightResource = resource;
                    updateDisplay = 1;
                    if (leftResource != -1)
                        SetupNewTrade();
                }
                break;
            }
        } else if (message.payload.widget.command == WIDGET_COMMAND_DESELECT) {
            if (message.payload.widget.id == NORMAL_DIALOG_BUTTON_TWO) {
                exitDialog = 1;
            } else if (message.payload.widget.id == TRADING_POST_DECREMENT) {
                updateDisplay = qtyToTrade > 0;
                if (updateDisplay)
                    qtyToTrade--;
            } else if (message.payload.widget.id == TRADING_POST_INCREMENT) {
                updateDisplay = qtyToTrade < iMaxUnitsToTrade;
                if (updateDisplay)
                    qtyToTrade++;
            } else if (message.payload.widget.id == TRADING_POST_EXECUTE &&
                       qtyToTrade != 0) {
                if (bLeftDenominated == 0) {
                    gpCurPlayer->m_resources[leftResource] -=
                        qtyToTrade * iTradeRatio;
                    gpCurPlayer->m_resources[rightResource] += qtyToTrade;
                } else {
                    gpCurPlayer->m_resources[leftResource] -= qtyToTrade;
                    gpCurPlayer->m_resources[rightResource] +=
                        qtyToTrade * iTradeRatio;
                }
                bTradeMade = 1;
                rightResource = -1;
                leftResource = rightResource;
                updateDisplay = 1;
            }
        }
    }
    if (updateDisplay)
        UpdateTradingPost(1);
    if (exitDialog) {
        message.payload.widget.id = WIDGET_COMMAND_DIALOG_SELECT;
        message.payload.widget.command = message.payload.widget.id;
        return TRADING_POST_HANDLER_EXIT;
    }
    return TRADING_POST_HANDLER_CONTINUE;
}

// ---- globals (definitions, RVA order) ----
DATA(0x0051d9d0) unsigned short coreRatio[TRADING_POST_RESOURCE_COUNT];
DATA(0x00533170) class iconWidget *tradeKnob;
DATA(0x00533174) int qtyToTrade;
DATA(0x00533178) int leftResource;
DATA(0x0053317c) int iTradeRatio;
DATA(0x00533180) int bTradeMade;
DATA(0x00533184) float fTradingPostEfficiency;
DATA(0x00533188) int tpX;
DATA(0x0053318c) int tpY;
DATA(0x00533190) int bIsMarketPlace;
DATA(0x00533194) int bLeftDenominated;
DATA(0x00533198) class heroWindow *tpWindow;
DATA(0x0053319c) int rightResource;
DATA(0x005331a0) int iMaxUnitsToTrade;
DATA(0x005331a4) int maxUnitsToTrade;
