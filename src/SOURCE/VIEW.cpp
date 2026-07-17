// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\VIEW.OBJ   from: (directly linked into exe)
// functions: 3   data: 1
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <stdio.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/widget.h>
#include <SOURCE/KB.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/armyGroup.h>
#include <SOURCE/combatManager.h>
#include <SOURCE/game.h>
#include <SOURCE/VIEW.h>
// @semantic
// Complete dialog behavior/CFG and all 71 ordered relocations. Recovering the
// individually allocated compiler locals and explicit null return raised the live
// match to 99.46%; candidate frame 0x74 remains one word below retail 0x78.
// Cached/repeated hero loads, packed locals, and both quick-view arm orders were tried.
VA(0x0040bd60, 0x6d3)
i32 combatManager::ViewGeneral(i32 side, i32 allowActions, i32 quickView) {
    if (m_heroes[side] == 0)
        return 0;
    {
        i16 messageConstant1;
        i16 messageConstant2;
        i16 messageConstant3;
        i16 messageConstant4;
        i16 messageConstant30;
        i16 messageConstant0;
        i16 messageConstant1B;
        i16 messageConstant7;
        i16 messageConstant8;
        i16 messageConstant9;
        i16 messageConstant10;
        i16 messageConstant11;
        i16 messageConstant12;
        i16 messageConstant13;
        i16 messageConstant14;
        tag_message message;
        heroWindow* generalWindow;
        i32 morale;
        i32 luck;

        iViewGeneralWhichSide = side;
        messageConstant1 = 1;
        messageConstant2 = 2;
        messageConstant3 = 3;
        messageConstant4 = 4;
        messageConstant30 = 30;
        messageConstant0 = 0;
        messageConstant1B = 1;
        messageConstant7 = 7;
        messageConstant8 = 8;
        messageConstant9 = 9;
        messageConstant10 = 10;
        messageConstant11 = 11;
        messageConstant12 = 12;
        messageConstant13 = 13;
        messageConstant14 = 14;
        giCurGeneral = side;

        message.type = MESSAGE_WIDGET;
        generalWindow = new heroWindow(VIEW_GENERAL_WINDOW_X, VIEW_GENERAL_WINDOW_Y, "vgenwin.bin");
        if (generalWindow == 0)
            MemError();
        sprintf(gText, "port%04d.icn", m_heroes[side]->m_portrait);
        message.payload.widget.command = VIEW_GENERAL_SET_ICON;
        message.payload.widget.id = 2;
        message.payload.widget.data.text = gText;
        generalWindow->BroadcastMessage(message);

        message.payload.widget.command =
            m_heroes[side]->m_isCaptain ? WIDGET_COMMAND_SET_FLAGS : WIDGET_COMMAND_CLEAR_FLAGS;
        message.payload.widget.id = 30;
        message.payload.widget.data.value = 4;
        generalWindow->BroadcastMessage(message);
        if (m_heroes[side]->m_isCaptain) {
            message.payload.widget.command = VIEW_GENERAL_SET_FRAME;
            if (m_playerId[side] == -1)
                message.payload.widget.data.value = 6;
            else
                message.payload.widget.data.value = gpGame->GetPlayerColor(m_playerId[side]);
            generalWindow->BroadcastMessage(message);
        }

        message.payload.widget.command = VIEW_GENERAL_SET_FRAME;
        message.payload.widget.id = 3;
        message.payload.widget.data.value = gpGame->GetPlayerColor(m_heroes[side]->m_owner) + 1;
        generalWindow->BroadcastMessage(message);

        if (m_heroes[side]->m_isCaptain != 0)
            sprintf(gText, "Captain");
        else
            sprintf(
                gText,
                "%s the %s",
                m_heroes[side]->m_name,
                gAlignmentNames[m_heroes[side]->m_cursorType]
            );
        message.payload.widget.command = VIEW_GENERAL_SET_TEXT;
        message.payload.widget.id = 1;
        message.payload.widget.data.text = gText;
        generalWindow->BroadcastMessage(message);

        morale = m_heroes[side]->m_army.GetMorale(
            m_heroes[side],
            m_combatTowns[side],
            gpCombatManager->m_armyGroups[1 - side]
        );
        luck = gpGame->GetLuck(m_heroes[side], 0, m_combatTowns[side]);
        sprintf(
            gText,
            "\n%s%d\n%s%d\n%s%d\n%s%d\n%s%s\n%s%s\n\n%s%d/%d",
            cViewGeneralLabels[0],
            m_heroes[side]->Stats(0),
            cViewGeneralLabels[1],
            m_heroes[side]->Stats(1),
            cViewGeneralLabels[2],
            m_heroes[side]->Stats(2),
            cViewGeneralLabels[3],
            m_heroes[side]->Stats(3),
            cViewGeneralLabels[4],
            gMoraleText[morale + VIEW_GENERAL_MORALE_TEXT_OFFSET],
            cViewGeneralLabels[5],
            gLuckText[luck + VIEW_GENERAL_LUCK_TEXT_OFFSET],
            cViewGeneralLabels[6],
            m_heroes[side]->m_spellPoints,
            m_heroes[side]->Stats(3) * VIEW_GENERAL_MANA_PER_KNOWLEDGE
        );
        message.payload.widget.command = VIEW_GENERAL_SET_TEXT;
        message.payload.widget.id = 4;
        message.payload.widget.data.text = gText;
        generalWindow->BroadcastMessage(message);

        if (m_heroes[side] == 0 || allowActions == 0
            || m_heroes[side]->HasArtifact(ARTIFACT_MAGIC_BOOK) == 0 || m_heroCastSpell[side] != 0
            || m_currentSide != giCurGeneral) {
            message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
            message.payload.widget.id = VIEW_GENERAL_CLOSE;
            message.payload.widget.data.value = WIDGET_FLAG_ENABLED;
            generalWindow->BroadcastMessage(message);
            message.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
            message.payload.widget.data.value = WIDGET_COMMAND_DIMMED;
            generalWindow->BroadcastMessage(message);
        }
        if (allowActions == 0 || m_heroes[1 - m_currentSide] == 0 || m_currentSide != giCurGeneral
            || m_heroes[side]->m_isCaptain != 0) {
            message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
            message.payload.widget.id = VIEW_GENERAL_SURRENDER;
            message.payload.widget.data.value = WIDGET_FLAG_ENABLED;
            generalWindow->BroadcastMessage(message);
            message.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
            message.payload.widget.data.value = WIDGET_COMMAND_DIMMED;
            generalWindow->BroadcastMessage(message);
        }
        if (allowActions == 0 || m_currentSide != giCurGeneral
            || (giCurGeneral == 1 && m_heroes[0] != 0) || m_sideRetreated[0] != 0
            || m_sideRetreated[1] != 0 || m_heroes[side]->m_isCaptain != 0) {
            message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
            message.payload.widget.id = VIEW_GENERAL_RETREAT;
            message.payload.widget.data.value = WIDGET_FLAG_ENABLED;
            generalWindow->BroadcastMessage(message);
            message.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
            message.payload.widget.data.value = WIDGET_COMMAND_DIMMED;
            generalWindow->BroadcastMessage(message);
        }

        if (quickView != 0) {
            gpWindowManager->AddWindow(generalWindow, -1, 1);
            QuickViewWait();
            gpWindowManager->RemoveWindow(generalWindow);
        } else {
            gpWindowManager->DoDialog(generalWindow, HandleViewGeneral, 0);
        }
        delete generalWindow;
        DrawFrame(1, 0, 0, 0, 75, 1, 1);
        if (quickView == 0)
            DoCommand(gpWindowManager->m_dialogResult);
    }
    return 0;
}

// @semantic
// Complete widget/help/hover behavior and all 12 ordered relocations. Retail's
// message+0x0d mask proves right-button help; the prior Left Shift spelling was a
// runtime bug. Individual compiler locals restored the frame to 0x58 versus retail
// 0x5c. Mouse-first conditions and the retained retail-order switches were tried.
VA(0x0040c433, 0x351)
i32 HandleViewGeneral(tag_message& message) {
    i16 messageConstant1;
    i16 messageConstant2;
    i16 messageConstant3;
    i16 messageConstant4;
    i16 messageConstant0;
    i16 messageConstant1B;
    i16 messageConstant7;
    i16 messageConstant8;
    i16 messageConstant9;
    i16 messageConstant10;
    i16 messageConstant11;
    i16 messageConstant12;
    i16 messageConstant13;
    i16 messageConstant14;
    i32 handled;
    i32 helpIndex;
    messageConstant1 = 1;
    messageConstant2 = 2;
    messageConstant3 = 3;
    messageConstant4 = 4;
    messageConstant0 = 0;
    messageConstant1B = 1;
    messageConstant7 = 7;
    messageConstant8 = 8;
    messageConstant9 = 9;
    messageConstant10 = 10;
    messageConstant11 = 11;
    messageConstant12 = 12;
    messageConstant13 = 13;
    messageConstant14 = 14;
    handled = 0;

    switch (message.type) {
        case MESSAGE_WIDGET:
            if (message.payload.widget.parameter & MESSAGE_MODIFIER_RIGHT_BUTTON) {
                helpIndex = -1;
                if (message.payload.widget.command != WIDGET_COMMAND_SELECT
                    && message.payload.widget.command != WIDGET_COMMAND_ALTERNATE_SELECT)
                    break;
                switch (message.payload.widget.id) {
                    case VIEW_GENERAL_CLOSE:
                        helpIndex = 0;
                        break;
                    case VIEW_GENERAL_RETREAT:
                        helpIndex = 1;
                        break;
                    case VIEW_GENERAL_SURRENDER:
                        helpIndex = 2;
                        break;
                    case VIEW_GENERAL_CAST_SPELL:
                        helpIndex = 3;
                        break;
                }
                if (helpIndex != -1)
                    NormalDialog(cViewGeneralLongHelp[helpIndex], 4, -1, -1, -1, 0, -1, 0, -1, 0);
            } else {
                switch (message.payload.widget.command) {
                    case WIDGET_COMMAND_DESELECT:
                        switch (message.payload.widget.id) {
                            case VIEW_GENERAL_CLOSE:
                            case VIEW_GENERAL_RETREAT:
                            case VIEW_GENERAL_SURRENDER:
                            case VIEW_GENERAL_CAST_SPELL:
                                gpWindowManager->m_dialogResult = message.payload.widget.id;
                                handled = 1;
                                break;
                        }
                        break;
                }
            }
            break;
        case MESSAGE_MOUSE_MOVE:
            gpWindowManager->ConvertToHover(message);
            if (gpWindowManager->m_lastHoverId == message.payload.hover.id)
                return 1;
            gpWindowManager->m_lastHoverId = message.payload.hover.id;
            switch (message.payload.hover.id) {
                case VIEW_GENERAL_CLOSE:
                    helpIndex = 1;
                    break;
                case VIEW_GENERAL_RETREAT:
                    helpIndex = 2;
                    break;
                case VIEW_GENERAL_SURRENDER:
                    helpIndex = 3;
                    break;
                case VIEW_GENERAL_CAST_SPELL:
                    helpIndex = 4;
                    break;
                default:
                    helpIndex = 5;
                    break;
            }
            if (helpIndex == 5 && gpCombatManager->m_heroes[iViewGeneralWhichSide]->m_isCaptain)
                helpIndex = 6;
            gpCombatManager->CombatMessage(cViewGeneralHelp[helpIndex], 1, 0, 0);
            break;
    }
    if (handled) {
        message.payload.widget.id = VIEW_GENERAL_CLOSE;
        message.payload.widget.command = message.payload.widget.id;
        return 2;
    }
    return 1;
}

// @semantic
// Exact 0x24 frame and all local slots after restoring the separate compiler locals;
// geometry, clamps, call arguments, and both relocations agree. First non-branch
// residual is the branchless facing mask near +0x81. Signed/unsigned equality and
// less-than mask spellings plus direct ternary geometry were tried.
VA(0x0040c784, 0x165)
void combatManager::ViewArmy(army* viewedArmy, i32 quickView) {
    if (viewedArmy == 0)
        return;
    {
        i32 side;
        i32 windowX;
        i32 windowY_5;
        i16 viewWidthConstant;
        i16 viewYOffsetConstant_1;
        i16 viewXOffsetConstant_9;
        i32 xOffset_9;
        i16 viewHeightConstant_10;

        viewWidthConstant = 402;
        viewHeightConstant_10 = 229;
        viewXOffsetConstant_9 = 86;
        viewYOffsetConstant_1 = 164;
        windowX = m_hexCells[viewedArmy->m_hex].m_x;
        windowY_5 = m_hexCells[viewedArmy->m_hex].m_y;
        xOffset_9 = (-(viewedArmy->m_facing == 0) & VIEW_ARMY_FACING_OFFSET_DELTA)
                    + VIEW_ARMY_RIGHT_FACING_X_OFFSET;
        windowX -= xOffset_9;
        if (windowX < 0)
            windowX = 0;
        if (windowX + VIEW_ARMY_WIDTH > VIEW_ARMY_SCREEN_WIDTH)
            windowX = VIEW_ARMY_RIGHT_CLAMP;
        windowY_5 -= VIEW_ARMY_Y_OFFSET;
        if (windowY_5 < 0)
            windowY_5 = 0;
        if (windowY_5 + VIEW_ARMY_HEIGHT > VIEW_ARMY_SCREEN_HEIGHT)
            windowY_5 = VIEW_ARMY_BOTTOM_CLAMP;
        side = viewedArmy->m_side;
        gpGame->ViewArmy(
            windowX,
            windowY_5,
            IDX(viewedArmy->m_monsterType),
            viewedArmy->m_quantity,
            m_combatTowns[side],
            1,
            viewedArmy->m_facing,
            quickView,
            m_heroes[side],
            viewedArmy,
            m_armyGroups[side],
            0
        );
    }
}

// ---- globals (definitions, RVA order) ----
DATA(0x004eddf4) i32 iViewGeneralWhichSide = 0;
