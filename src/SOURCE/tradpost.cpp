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
#include <BASE/message.h>

VA(0x004bf340, 0x165)
void DoTradingPost(i32 isMarketplace, float efficiency) {
    tag_message messageTemp;

    bIsMarketPlace = isMarketplace;
    fTradingPostEfficiency = efficiency;
    bTradeMade = 0;
    tpX = TRADING_POST_WINDOW_X;
    tpY = TRADING_POST_WINDOW_Y;
    tpWindow = new heroWindow(tpX, tpY, "tradpost.bin");
    if (tpWindow == 0)
        MemError();
    leftResource = -1;
    rightResource = -1;
    qtyToTrade = 0;
    tradeKnob = new iconWidget(
        TRADING_POST_KNOB_X,
        TRADING_POST_KNOB_Y,
        0x11,
        8,
        "tradpost.icn",
        2,
        0,
        TRADING_POST_KNOB_ID,
        0x10,
        1
    );
    if (tradeKnob == 0)
        MemError();
    tpWindow->AddWidget(tradeKnob, -1);
    UpdateTradingPost(0);
    gpWindowManager->DoDialog(tpWindow, TradingPostHandler, 0);
    delete tpWindow;
}

// @semantic
// Complete text, control-state, offer-icon, ratio, and knob CFG;
// the 0x3c frame and all recovered local slots agree. Candidate span is 0x597
// versus 0x596 retail code, with 312/312 non-jump instructions and 23/23 jumps.
// The visible residual is the selected-resource compare loading the loop local
// before the global instead of after it. All 112 relocations agree as a multiset;
// ordered review has five commutative swapped pairs (three left/right validity
// tests and two qty/ratio products). Reversing those expressions was byte-identical;
// the De Morgan selection predicate worsened block order. Ten reviewed depth-one
// commutative/relational AST variants did not improve the raw match. Revisit after TU-state
// changes or a new structural discovery.
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

    for (sideCurrent = 0; sideCurrent < 2; sideCurrent++) {
        if (leftResource != -1 && rightResource != -1 && leftResource != rightResource) {
            messageTemp.payload.widget.command = TRADING_POST_SET_ICON;
            if (sideCurrent == 0) {
                messageTemp.payload.widget.id = TRADING_POST_LEFT_OFFER_ICON;
            } else {
                messageTemp.payload.widget.id = TRADING_POST_RIGHT_OFFER_ICON;
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
                messageTemp.payload.widget.id = TRADING_POST_LEFT_OFFER_TEXT;
                if (bLeftDenominated != 0)
                    sprintf(gText, "%d", qtyToTrade);
                else
                    sprintf(gText, "%d", qtyToTrade * iTradeRatio);
            } else {
                messageTemp.payload.widget.id = TRADING_POST_RIGHT_OFFER_TEXT;
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
        gpWindowManager->UpdateScreenRegion(tpX + 0x20, tpY, 0x102, 0x1a2);
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
        *ratio = static_cast<i32>(tradeRatio + 0.999);
        *maxTrade = gpCurPlayer->m_resources[sourceResource] / *ratio;
    } else {
        *leftDenominated = 1;
        *ratio = static_cast<i32>(1.0f / tradeRatio);
        *maxTrade = gpCurPlayer->m_resources[sourceResource];
    }
}

// @semantic
// Exact 0x90 frame, stack slots, 0x148 extent, CFG/semantics, and 22/22 external
// relocations. The sole residual starts at +0x67: ours loads iMaxUnitsToTrade,
// compares qtyToTrade, and uses jle; retail loads qtyToTrade, compares
// iMaxUnitsToTrade, and uses jge. Reversing the comparison, an explicit empty
// positive arm, `qtyToTrade | 0`, and the zero-index SIB spelling compiled the
// same or added a jump. Revisit only after TU-state changes.
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

// @semantic
// Complete nested select/deselect switches, case-body order,
// 0x20 frame, stack slots, and embedded pointer/index tables. Both object spans
// are 0x3b8 (retail code 0x3b6); excluding table data leaves 154/154 non-jump
// instructions and 25/25 jumps. The four residual sites are commuted comparisons:
// the upper clamp, left and right resource selection, and increment bound.
// All 51 relocations agree as a multiset; ordered review has four swapped external
// pairs (clamp, two qty/ratio products, increment), while local-table spellings are
// delinker aliases. Direct commuted spellings and ten reviewed depth-one
// commutative/relational AST variants compiled byte-identically. Revisit after TU-state changes.
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
                    case TRADING_POST_TRACK:
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
                    case TRADING_POST_LEFT_SELECT_FIRST:
                    case TRADING_POST_LEFT_SELECT_FIRST + 1:
                    case TRADING_POST_LEFT_SELECT_FIRST + 2:
                    case TRADING_POST_LEFT_SELECT_FIRST + 3:
                    case TRADING_POST_LEFT_SELECT_FIRST + 4:
                    case TRADING_POST_LEFT_SELECT_FIRST + 5:
                    case TRADING_POST_LEFT_SELECT_FIRST + 6:
                        resourceData = message.payload.widget.id - TRADING_POST_LEFT_SELECT_FIRST;
                        if (leftResource != resourceData) {
                            leftResource = resourceData;
                            updateDisplay = 1;
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
                    case TRADING_POST_EXECUTE:
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
                    case TRADING_POST_DECREMENT:
                        if (qtyToTrade > 0) {
                            --qtyToTrade;
                            updateDisplay = 1;
                        }
                        break;
                    case TRADING_POST_INCREMENT:
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
        message.payload.widget.command = message.payload.widget.id;
        return TRADING_POST_HANDLER_EXIT;
    }
    return TRADING_POST_HANDLER_CONTINUE;
}

// ---- globals (definitions, RVA order) ----
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
