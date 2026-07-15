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
// @match-note
// Complete 0x78 frame, all dialog broadcasts/action-disable CFG, and all 71 relocation
// addresses agree. Three gText sites are delinked under adjacent local data aliases;
// residuals otherwise begin in local-slot order. Tried cached/repeated hero loads and
// both quick-view arm orders. Revisit in the last-mile phase.
VA(0x0040bd60, 0x6d3)
int combatManager::ViewGeneral(int side, int allowActions, int quickView)
{
    if (m_heroes[side] != 0) {
        short localConstants[15];
        tag_message message;
        heroWindow *generalWindow;
        int morale;
        int luck;

        iViewGeneralWhichSide = side;
        localConstants[0] = 1;
        localConstants[1] = 2;
        localConstants[2] = 3;
        localConstants[3] = 4;
        localConstants[4] = 30;
        localConstants[5] = 0;
        localConstants[6] = 1;
        localConstants[7] = 7;
        localConstants[8] = 8;
        localConstants[9] = 9;
        localConstants[10] = 10;
        localConstants[11] = 11;
        localConstants[12] = 12;
        localConstants[13] = 13;
        localConstants[14] = 14;
        giCurGeneral = side;

        message.type = MESSAGE_WIDGET;
        generalWindow = new heroWindow(VIEW_GENERAL_WINDOW_X,
                                       VIEW_GENERAL_WINDOW_Y, "vgenwin.bin");
        if (generalWindow == 0)
            MemError();
        sprintf(gText, "port%04d.icn", m_heroes[side]->m_portrait);
        message.payload.widget.command = VIEW_GENERAL_SET_ICON;
        message.payload.widget.id = 2;
        message.payload.widget.data.text = gText;
        generalWindow->BroadcastMessage(message);

        message.payload.widget.command = m_heroes[side]->m_isCaptain
                                             ? WIDGET_COMMAND_SET_FLAGS
                                             : WIDGET_COMMAND_CLEAR_FLAGS;
        message.payload.widget.id = 30;
        message.payload.widget.data.value = 4;
        generalWindow->BroadcastMessage(message);
        if (m_heroes[side]->m_isCaptain) {
            message.payload.widget.command = VIEW_GENERAL_SET_FRAME;
            if (m_playerId[side] == -1)
                message.payload.widget.data.value = 6;
            else
                message.payload.widget.data.value =
                    gpGame->GetPlayerColor(m_playerId[side]);
            generalWindow->BroadcastMessage(message);
        }

        message.payload.widget.command = VIEW_GENERAL_SET_FRAME;
        message.payload.widget.id = 3;
        message.payload.widget.data.value =
            gpGame->GetPlayerColor(m_heroes[side]->m_owner) + 1;
        generalWindow->BroadcastMessage(message);

        if (m_heroes[side]->m_isCaptain != 0)
            sprintf(gText, "Captain");
        else
            sprintf(gText, "%s the %s", m_heroes[side]->m_name,
                    gAlignmentNames[m_heroes[side]->m_cursorType]);
        message.payload.widget.command = VIEW_GENERAL_SET_TEXT;
        message.payload.widget.id = 1;
        message.payload.widget.data.text = gText;
        generalWindow->BroadcastMessage(message);

        morale = m_heroes[side]->m_army.GetMorale(
            m_heroes[side], m_combatTowns[side],
            gpCombatManager->m_armyGroups[1 - side]);
        luck = gpGame->GetLuck(m_heroes[side], 0, m_combatTowns[side]);
        sprintf(gText,
                "\n%s%d\n%s%d\n%s%d\n%s%d\n%s%s\n%s%s\n\n%s%d/%d",
                cViewGeneralLabels[0], m_heroes[side]->Stats(0),
                cViewGeneralLabels[1], m_heroes[side]->Stats(1),
                cViewGeneralLabels[2], m_heroes[side]->Stats(2),
                cViewGeneralLabels[3], m_heroes[side]->Stats(3),
                cViewGeneralLabels[4],
                gMoraleText[morale + VIEW_GENERAL_MORALE_TEXT_OFFSET],
                cViewGeneralLabels[5],
                gLuckText[luck + VIEW_GENERAL_LUCK_TEXT_OFFSET],
                cViewGeneralLabels[6], m_heroes[side]->m_spellPoints,
                m_heroes[side]->Stats(3) * VIEW_GENERAL_MANA_PER_KNOWLEDGE);
        message.payload.widget.command = VIEW_GENERAL_SET_TEXT;
        message.payload.widget.id = 4;
        message.payload.widget.data.text = gText;
        generalWindow->BroadcastMessage(message);

        if (m_heroes[side] == 0 || allowActions == 0 ||
            m_heroes[side]->HasArtifact(VIEW_GENERAL_MAGIC_BOOK) == 0 ||
            m_heroCastSpell[side] != 0 || m_currentSide != giCurGeneral) {
            message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
            message.payload.widget.id = VIEW_GENERAL_CLOSE;
            message.payload.widget.data.value = WIDGET_FLAG_ENABLED;
            generalWindow->BroadcastMessage(message);
            message.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
            message.payload.widget.data.value = WIDGET_COMMAND_DIMMED;
            generalWindow->BroadcastMessage(message);
        }
        if (allowActions == 0 || m_heroes[1 - m_currentSide] == 0 ||
            m_currentSide != giCurGeneral || m_heroes[side]->m_isCaptain != 0) {
            message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
            message.payload.widget.id = VIEW_GENERAL_SURRENDER;
            message.payload.widget.data.value = WIDGET_FLAG_ENABLED;
            generalWindow->BroadcastMessage(message);
            message.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
            message.payload.widget.data.value = WIDGET_COMMAND_DIMMED;
            generalWindow->BroadcastMessage(message);
        }
        if (allowActions == 0 || m_currentSide != giCurGeneral ||
            (giCurGeneral == 1 && m_heroes[0] != 0) ||
            m_sideRetreated[0] != 0 || m_sideRetreated[1] != 0 ||
            m_heroes[side]->m_isCaptain != 0) {
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

// @match-note
// Complete 0x5c frame, widget/help/hover switch CFG, and all 12 relocations agree.
// Residuals are local-constant slot hashing and switch continuations after trying
// mouse-first conditionals and the retained retail-order switch. Revisit last-mile.
VA(0x0040c433, 0x351)
int HandleViewGeneral(tag_message &message)
{
    short localConstants[14];
    int handled = 0;
    int helpIndex;
    localConstants[0] = 1;
    localConstants[1] = 2;
    localConstants[2] = 3;
    localConstants[3] = 4;
    localConstants[4] = 0;
    localConstants[5] = 1;
    localConstants[6] = 7;
    localConstants[7] = 8;
    localConstants[8] = 9;
    localConstants[9] = 10;
    localConstants[10] = 11;
    localConstants[11] = 12;
    localConstants[12] = 13;
    localConstants[13] = 14;

    switch (message.type) {
    case MESSAGE_WIDGET:
        if (message.payload.widget.parameter & MESSAGE_MODIFIER_LEFT_SHIFT) {
            if (message.payload.widget.command != WIDGET_COMMAND_SELECT &&
                message.payload.widget.command != WIDGET_COMMAND_ALTERNATE_SELECT)
                break;
            helpIndex = -1;
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
                NormalDialog(cViewGeneralLongHelp[helpIndex], 4,
                             -1, -1, -1, 0, -1, 0, -1, 0);
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
        if (helpIndex == 5 &&
            gpCombatManager->m_heroes[iViewGeneralWhichSide]->m_isCaptain)
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

// @match-note
// Complete 0x24 frame, geometry clamps and ViewArmy arguments; both relocations agree.
// Residuals are local-constant slots and arithmetic continuations after trying direct
// ternary geometry and the retained branchless facing expression. Revisit last-mile.
VA(0x0040c784, 0x165)
void combatManager::ViewArmy(army *viewedArmy, int quickView)
{
    if (viewedArmy == 0)
        return;
    {
        short localConstants[4];
        localConstants[0] = 402;
        localConstants[1] = 229;
        localConstants[2] = 86;
        localConstants[3] = 164;
        int windowX = m_hexCells[viewedArmy->m_hex].m_x;
        int windowY = m_hexCells[viewedArmy->m_hex].m_y;
        int xOffset =
            (-static_cast<unsigned int>(viewedArmy->m_facing == 0) &
             VIEW_ARMY_FACING_OFFSET_DELTA) +
            VIEW_ARMY_RIGHT_FACING_X_OFFSET;
        windowX -= -(-xOffset);
        if (windowX < 0)
            windowX = 0;
        if (windowX + VIEW_ARMY_WIDTH > VIEW_ARMY_SCREEN_WIDTH)
            windowX = VIEW_ARMY_RIGHT_CLAMP;
        windowY -= VIEW_ARMY_Y_OFFSET;
        if (windowY < 0)
            windowY = 0;
        if (windowY + VIEW_ARMY_HEIGHT > VIEW_ARMY_SCREEN_HEIGHT)
            windowY = VIEW_ARMY_BOTTOM_CLAMP;
        int side = viewedArmy->m_side;
        gpGame->ViewArmy(windowX, windowY, viewedArmy->m_monsterType,
                         viewedArmy->m_quantity, m_combatTowns[side], 1,
                         viewedArmy->m_facing, quickView, m_heroes[side],
                         viewedArmy, m_armyGroups[side], 0);
    }
}

// ---- globals (definitions, RVA order) ----
DATA(0x004eddf4) int iViewGeneralWhichSide = 0;
