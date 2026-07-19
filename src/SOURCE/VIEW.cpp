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
H2_ENUM_BEGIN(ViewGeneralConstant)
    GENERAL_WINDOW_X           = 179,
    GENERAL_WINDOW_Y           = 60,
    GENERAL_CLOSE              = 10,
    GENERAL_RETREAT            = 11,
    GENERAL_SURRENDER          = 12,
    GENERAL_CAST_SPELL         = 0x7800,
    GENERAL_TEXT_COLOR_COUNT   = 11,
    GENERAL_MORALE_TEXT_OFFSET = 3,
    GENERAL_LUCK_TEXT_OFFSET   = 3,
    GENERAL_MANA_PER_KNOWLEDGE = 10,
    GENERAL_CAPTAIN_FRAME      = 6,
    ARMY_WIDTH                 = 488,
    ARMY_HEIGHT                = 229,
    ARMY_SCREEN_WIDTH          = 640,
    ARMY_SCREEN_HEIGHT         = 460,
    ARMY_LEFT_FACING_X_OFFSET  = 123,
    ARMY_RIGHT_FACING_X_OFFSET = 80,
    ARMY_VIEW_X_OFFSET         = 86,
    ARMY_FACING_OFFSET_DELTA   = 43,
    ARMY_Y_OFFSET              = 164,
    ARMY_RIGHT_CLAMP           = 151,
    ARMY_BOTTOM_CLAMP          = 230
H2_ENUM_END(ViewGeneralConstant)

H2_ENUM_BEGIN(ViewGeneralControl)
    GENERAL_CONTROL_NONE     = 0,
    GENERAL_NAME_WIDGET      = 1,
    GENERAL_PORTRAIT_WIDGET  = 2,
    GENERAL_COLOR_WIDGET     = 3,
    GENERAL_STATS_WIDGET     = 4,
    GENERAL_CONTROL_SEVEN    = 7,
    GENERAL_CONTROL_EIGHT    = 8,
    GENERAL_CONTROL_NINE     = 9,
    GENERAL_CONTROL_THIRTEEN = 13,
    GENERAL_CONTROL_FOURTEEN = 14,
    GENERAL_CAPTAIN_WIDGET   = 30
H2_ENUM_END(ViewGeneralControl)

H2_ENUM_BEGIN(ViewGeneralLabel)
    GENERAL_LABEL_MORALE = 4,
    GENERAL_LABEL_LUCK   = 5,
    GENERAL_LABEL_MANA   = 6
H2_ENUM_END(ViewGeneralLabel)

H2_ENUM_BEGIN(ViewGeneralLongHelp)
    GENERAL_LONG_HELP_CLOSE     = 0,
    GENERAL_LONG_HELP_RETREAT   = 1,
    GENERAL_LONG_HELP_SURRENDER = 2,
    GENERAL_LONG_HELP_CAST      = 3
H2_ENUM_END(ViewGeneralLongHelp)

H2_ENUM_BEGIN(ViewGeneralHoverHelp)
    GENERAL_HOVER_HELP_CLOSE     = 1,
    GENERAL_HOVER_HELP_RETREAT   = 2,
    GENERAL_HOVER_HELP_SURRENDER = 3,
    GENERAL_HOVER_HELP_CAST      = 4,
    GENERAL_HOVER_HELP_HERO      = 5,
    GENERAL_HOVER_HELP_CAPTAIN   = 6
H2_ENUM_END(ViewGeneralHoverHelp)

VA(0x0040bd60, 0x6d3)
i32 combatManager::ViewGeneral(i32 side, i32 allowActions, i32 quickView) {
    if (m_heroes[side] == NULL)
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
        messageConstant1 = GENERAL_NAME_WIDGET;
        messageConstant2 = GENERAL_PORTRAIT_WIDGET;
        messageConstant3 = GENERAL_COLOR_WIDGET;
        messageConstant4 = GENERAL_STATS_WIDGET;
        messageConstant30 = GENERAL_CAPTAIN_WIDGET;
        messageConstant0 = GENERAL_CONTROL_NONE;
        messageConstant1B = GENERAL_NAME_WIDGET;
        messageConstant7 = GENERAL_CONTROL_SEVEN;
        messageConstant8 = GENERAL_CONTROL_EIGHT;
        messageConstant9 = GENERAL_CONTROL_NINE;
        messageConstant10 = GENERAL_CLOSE;
        messageConstant11 = GENERAL_RETREAT;
        messageConstant12 = GENERAL_SURRENDER;
        messageConstant13 = GENERAL_CONTROL_THIRTEEN;
        messageConstant14 = GENERAL_CONTROL_FOURTEEN;
        giCurGeneral = side;

        message.type = MESSAGE_WIDGET;
        generalWindow = new heroWindow(GENERAL_WINDOW_X, GENERAL_WINDOW_Y, "vgenwin.bin");
        if (generalWindow == NULL)
            MemError();
        sprintf(gText, "port%04d.icn", m_heroes[side]->m_portrait);
        message.payload.widget.command = VIEW_GENERAL_SET_ICON;
        message.payload.widget.id = GENERAL_PORTRAIT_WIDGET;
        message.payload.widget.data.text = gText;
        generalWindow->BroadcastMessage(message);

        message.payload.widget.command =
            m_heroes[side]->m_isCaptain ? WIDGET_COMMAND_SET_FLAGS : WIDGET_COMMAND_CLEAR_FLAGS;
        message.payload.widget.id = GENERAL_CAPTAIN_WIDGET;
        message.payload.widget.data.value = WIDGET_FLAG_DRAW;
        generalWindow->BroadcastMessage(message);
        if (m_heroes[side]->m_isCaptain) {
            message.payload.widget.command = VIEW_GENERAL_SET_FRAME;
            if (m_playerId[side] == -1)
                message.payload.widget.data.value = GENERAL_CAPTAIN_FRAME;
            else
                message.payload.widget.data.value = gpGame->GetPlayerColor(m_playerId[side]);
            generalWindow->BroadcastMessage(message);
        }

        message.payload.widget.command = VIEW_GENERAL_SET_FRAME;
        message.payload.widget.id = GENERAL_COLOR_WIDGET;
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
        message.payload.widget.id = GENERAL_NAME_WIDGET;
        message.payload.widget.data.text = gText;
        generalWindow->BroadcastMessage(message);

        morale = m_heroes[side]->m_army.GetMorale(
            m_heroes[side],
            m_combatTowns[side],
            gpCombatManager->m_armyGroups[1 - side]
        );
        luck = gpGame->GetLuck(m_heroes[side], NULL, m_combatTowns[side]);
        sprintf(
            gText,
            "\n%s%d\n%s%d\n%s%d\n%s%d\n%s%s\n%s%s\n\n%s%d/%d",
            cViewGeneralLabels[IDX(HERO_PRIMARY_ATTACK)],
            m_heroes[side]->Stats(HERO_PRIMARY_ATTACK),
            cViewGeneralLabels[IDX(HERO_PRIMARY_DEFENSE)],
            m_heroes[side]->Stats(HERO_PRIMARY_DEFENSE),
            cViewGeneralLabels[IDX(HERO_PRIMARY_SPELL_POWER)],
            m_heroes[side]->Stats(HERO_PRIMARY_SPELL_POWER),
            cViewGeneralLabels[IDX(HERO_PRIMARY_KNOWLEDGE)],
            m_heroes[side]->Stats(HERO_PRIMARY_KNOWLEDGE),
            cViewGeneralLabels[GENERAL_LABEL_MORALE],
            gMoraleText[morale + GENERAL_MORALE_TEXT_OFFSET],
            cViewGeneralLabels[GENERAL_LABEL_LUCK],
            gLuckText[luck + GENERAL_LUCK_TEXT_OFFSET],
            cViewGeneralLabels[GENERAL_LABEL_MANA],
            m_heroes[side]->m_spellPoints,
            m_heroes[side]->Stats(HERO_PRIMARY_KNOWLEDGE) * GENERAL_MANA_PER_KNOWLEDGE
        );
        message.payload.widget.command = VIEW_GENERAL_SET_TEXT;
        message.payload.widget.id = GENERAL_STATS_WIDGET;
        message.payload.widget.data.text = gText;
        generalWindow->BroadcastMessage(message);

        if (m_heroes[side] == NULL || allowActions == 0
            || m_heroes[side]->HasArtifact(ARTIFACT_MAGIC_BOOK) == 0 || m_heroCastSpell[side] != 0
            || m_currentSide != giCurGeneral) {
            message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
            message.payload.widget.id = GENERAL_CLOSE;
            message.payload.widget.data.value = WIDGET_FLAG_ENABLED;
            generalWindow->BroadcastMessage(message);
            message.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
            message.payload.widget.data.value = WIDGET_COMMAND_DIMMED;
            generalWindow->BroadcastMessage(message);
        }
        if (allowActions == 0 || m_heroes[1 - m_currentSide] == NULL || m_currentSide != giCurGeneral
            || m_heroes[side]->m_isCaptain != 0) {
            message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
            message.payload.widget.id = GENERAL_SURRENDER;
            message.payload.widget.data.value = WIDGET_FLAG_ENABLED;
            generalWindow->BroadcastMessage(message);
            message.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
            message.payload.widget.data.value = WIDGET_COMMAND_DIMMED;
            generalWindow->BroadcastMessage(message);
        }
        if (allowActions == 0 || m_currentSide != giCurGeneral
            || (giCurGeneral == 1 && m_heroes[0] != NULL) || m_sideRetreated[0] != 0
            || m_sideRetreated[1] != 0 || m_heroes[side]->m_isCaptain != 0) {
            message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
            message.payload.widget.id = GENERAL_RETREAT;
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
        DrawFrame(1, 0, 0, 0, COMBAT_MOUSE_REDRAW_DELAY, 1, 1);
        if (quickView == 0)
            DoCommand(static_cast<CombatMessageCommand>(gpWindowManager->m_dialogResult));
    }
    return 0;
}

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
    messageConstant1 = GENERAL_NAME_WIDGET;
    messageConstant2 = GENERAL_PORTRAIT_WIDGET;
    messageConstant3 = GENERAL_COLOR_WIDGET;
    messageConstant4 = GENERAL_STATS_WIDGET;
    messageConstant0 = GENERAL_CONTROL_NONE;
    messageConstant1B = GENERAL_NAME_WIDGET;
    messageConstant7 = GENERAL_CONTROL_SEVEN;
    messageConstant8 = GENERAL_CONTROL_EIGHT;
    messageConstant9 = GENERAL_CONTROL_NINE;
    messageConstant10 = GENERAL_CLOSE;
    messageConstant11 = GENERAL_RETREAT;
    messageConstant12 = GENERAL_SURRENDER;
    messageConstant13 = GENERAL_CONTROL_THIRTEEN;
    messageConstant14 = GENERAL_CONTROL_FOURTEEN;
    handled = 0;

    switch (message.type) {
        case MESSAGE_WIDGET:
            if (message.payload.widget.parameter & MESSAGE_MODIFIER_RIGHT_BUTTON) {
                helpIndex = -1;
                if (message.payload.widget.command != WIDGET_COMMAND_SELECT
                    && message.payload.widget.command != WIDGET_COMMAND_ALTERNATE_SELECT)
                    break;
                switch (message.payload.widget.id) {
                    case GENERAL_CLOSE:
                        helpIndex = GENERAL_LONG_HELP_CLOSE;
                        break;
                    case GENERAL_RETREAT:
                        helpIndex = GENERAL_LONG_HELP_RETREAT;
                        break;
                    case GENERAL_SURRENDER:
                        helpIndex = GENERAL_LONG_HELP_SURRENDER;
                        break;
                    case GENERAL_CAST_SPELL:
                        helpIndex = GENERAL_LONG_HELP_CAST;
                        break;
                }
                if (helpIndex != -1)
                    NormalDialog(
                        cViewGeneralLongHelp[helpIndex],
                        NORMAL_DIALOG_QUICK_VIEW,
                        -1,
                        -1,
                        -1,
                        0,
                        -1,
                        0,
                        -1,
                        0
                    );
            } else {
                switch (message.payload.widget.command) {
                    case WIDGET_COMMAND_DESELECT:
                        switch (message.payload.widget.id) {
                            case GENERAL_CLOSE:
                            case GENERAL_RETREAT:
                            case GENERAL_SURRENDER:
                            case GENERAL_CAST_SPELL:
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
                return WIDGET_DISPATCH_CONSUME;
            gpWindowManager->m_lastHoverId = message.payload.hover.id;
            switch (message.payload.hover.id) {
                case GENERAL_CLOSE:
                    helpIndex = GENERAL_HOVER_HELP_CLOSE;
                    break;
                case GENERAL_RETREAT:
                    helpIndex = GENERAL_HOVER_HELP_RETREAT;
                    break;
                case GENERAL_SURRENDER:
                    helpIndex = GENERAL_HOVER_HELP_SURRENDER;
                    break;
                case GENERAL_CAST_SPELL:
                    helpIndex = GENERAL_HOVER_HELP_CAST;
                    break;
                default:
                    helpIndex = GENERAL_HOVER_HELP_HERO;
                    break;
            }
            if (helpIndex == GENERAL_HOVER_HELP_HERO
                && gpCombatManager->m_heroes[iViewGeneralWhichSide]->m_isCaptain)
                helpIndex = GENERAL_HOVER_HELP_CAPTAIN;
            gpCombatManager->CombatMessage(cViewGeneralHelp[helpIndex], 1, 0, 0);
            break;
    }
    if (handled) {
        message.payload.widget.id = GENERAL_CLOSE;
        message.payload.widget.command = BaseWidgetCommand(message.payload.widget.id);
        return WIDGET_DISPATCH_FORWARD;
    }
    return WIDGET_DISPATCH_CONSUME;
}

VA(0x0040c784, 0x165)
void combatManager::ViewArmy(army* viewedArmy, i32 quickView) {
    if (viewedArmy == NULL)
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

        viewWidthConstant = ARMY_WIDTH - ARMY_VIEW_X_OFFSET;
        viewHeightConstant_10 = ARMY_HEIGHT;
        viewXOffsetConstant_9 = ARMY_VIEW_X_OFFSET;
        viewYOffsetConstant_1 = ARMY_Y_OFFSET;
        windowX = m_hexCells[viewedArmy->m_hex].m_x;
        windowY_5 = m_hexCells[viewedArmy->m_hex].m_y;
        xOffset_9 =
            (-(viewedArmy->m_facing == ARMY_FACING_LEFT) & ARMY_FACING_OFFSET_DELTA)
            + ARMY_RIGHT_FACING_X_OFFSET;
        windowX -= xOffset_9;
        if (windowX < 0)
            windowX = 0;
        if (windowX + ARMY_WIDTH > ARMY_SCREEN_WIDTH)
            windowX = ARMY_RIGHT_CLAMP;
        windowY_5 -= ARMY_Y_OFFSET;
        if (windowY_5 < 0)
            windowY_5 = 0;
        if (windowY_5 + ARMY_HEIGHT > ARMY_SCREEN_HEIGHT)
            windowY_5 = ARMY_BOTTOM_CLAMP;
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

DATA(0x004eddf4) i32 iViewGeneralWhichSide = 0;
