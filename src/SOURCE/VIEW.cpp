#include <Ints.h>
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
typedef enum ViewGeneralConstant {
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
} ViewGeneralConstant;

typedef enum ViewGeneralControl {
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
} ViewGeneralControl;

typedef enum ViewGeneralLabel {
    GENERAL_LABEL_MORALE = 4,
    GENERAL_LABEL_LUCK   = 5,
    GENERAL_LABEL_MANA   = 6
} ViewGeneralLabel;

typedef enum ViewGeneralLongHelp {
    GENERAL_LONG_HELP_CLOSE     = 0,
    GENERAL_LONG_HELP_RETREAT   = 1,
    GENERAL_LONG_HELP_SURRENDER = 2,
    GENERAL_LONG_HELP_CAST      = 3
} ViewGeneralLongHelp;

typedef enum ViewGeneralHoverHelp {
    GENERAL_HOVER_HELP_CLOSE     = 1,
    GENERAL_HOVER_HELP_RETREAT   = 2,
    GENERAL_HOVER_HELP_SURRENDER = 3,
    GENERAL_HOVER_HELP_CAST      = 4,
    GENERAL_HOVER_HELP_HERO      = 5,
    GENERAL_HOVER_HELP_CAPTAIN   = 6
} ViewGeneralHoverHelp;

i32 combatManager::ViewGeneral(
    CombatSide side, i32 allowActions, i32 quickView
) {
    i16 msgConst37;
    i16 msgConst4;
    i16 msgConst10;
    i16 msgConst3;
    i16 msgConst29;
    i16 msgConst2;
    i32 luck14;
    i16 msgConst1;
    i32 morale11;
    i16 msgConst7;
    i16 msgConst6;
    i16 msgConst5;
    i16 msgConst17;
    i16 msgConst18;
    i16 msgConst15;
    i32 cost2;
    heroWindow* generalWindow26;
    i16 msgConst28;
    tag_message message16;
    i16 msgConst26;

    if (m_heroes[H2EnumIndex(side)] == NULL)
        return 0;
    iViewGeneralWhichSide = side;
    msgConst5 = GENERAL_NAME_WIDGET;
    msgConst1 = GENERAL_PORTRAIT_WIDGET;
    msgConst15 = GENERAL_COLOR_WIDGET;
    msgConst37 = GENERAL_STATS_WIDGET;
    msgConst7 = GENERAL_CAPTAIN_WIDGET;
    msgConst18 = GENERAL_CONTROL_NONE;
    msgConst26 = GENERAL_NAME_WIDGET;
    msgConst4 = GENERAL_CONTROL_SEVEN;
    msgConst2 = GENERAL_CONTROL_EIGHT;
    msgConst28 = GENERAL_CONTROL_NINE;
    msgConst17 = GENERAL_CLOSE;
    msgConst3 = GENERAL_RETREAT;
    msgConst29 = GENERAL_SURRENDER;
    msgConst10 = GENERAL_CONTROL_THIRTEEN;
    msgConst6 = GENERAL_CONTROL_FOURTEEN;
    giCurGeneral = side;

    message16.type = MESSAGE_WIDGET;
    generalWindow26 = new heroWindow(GENERAL_WINDOW_X, GENERAL_WINDOW_Y, "vgenwin.bin");
    if (generalWindow26 == NULL)
        MemError();
    sprintf(gText, "port%04d.icn", H2EnumIndex(m_heroes[H2EnumIndex(side)]->m_portrait));
    message16.payload.widget.command = VIEW_GENERAL_SET_ICON;
    message16.payload.widget.id = GENERAL_PORTRAIT_WIDGET;
    message16.payload.widget.data.text = gText;
    generalWindow26->BroadcastMessage(message16);

    message16.payload.widget.command =
        m_heroes[H2EnumIndex(side)]->m_isCaptain ? WIDGET_COMMAND_SET_FLAGS : WIDGET_COMMAND_CLEAR_FLAGS;
    message16.payload.widget.id = GENERAL_CAPTAIN_WIDGET;
    message16.payload.widget.data.value = H2EnumIndex(WIDGET_FLAG_DRAW);
    generalWindow26->BroadcastMessage(message16);
    if (m_heroes[H2EnumIndex(side)]->m_isCaptain) {
        message16.payload.widget.command = VIEW_GENERAL_SET_FRAME;
        message16.payload.widget.data.value = m_playerId[H2EnumIndex(side)] == -1
            ? GENERAL_CAPTAIN_FRAME
            : gpGame->m_players[m_playerId[H2EnumIndex(side)]].m_color;
        generalWindow26->BroadcastMessage(message16);
    }

    message16.payload.widget.command = VIEW_GENERAL_SET_FRAME;
    message16.payload.widget.id = GENERAL_COLOR_WIDGET;
    message16.payload.widget.data.value = gpGame->GetPlayerColor(m_heroes[H2EnumIndex(side)]->m_owner) + 1;
    generalWindow26->BroadcastMessage(message16);

    if (m_heroes[H2EnumIndex(side)]->m_isCaptain != 0)
        sprintf(
            gText,
              "\xca\xe0\xef\xe8\xf2\xe0\xed"
        );
    else
        sprintf(
            gText,
            "%s - %s",
            m_heroes[H2EnumIndex(side)]->m_name,
            gAlignmentNames[H2EnumIndex(m_heroes[H2EnumIndex(side)]->m_cursorType)]
        );
    message16.payload.widget.command = VIEW_GENERAL_SET_TEXT;
    message16.payload.widget.id = GENERAL_NAME_WIDGET;
    message16.payload.widget.data.text = gText;
    generalWindow26->BroadcastMessage(message16);

    morale11 = m_heroes[H2EnumIndex(side)]->m_army.GetMorale(
        m_heroes[H2EnumIndex(side)],
        m_combatTowns[H2EnumIndex(side)],
        gpCombatManager->m_armyGroups[H2EnumIndex(OppositeCombatSide(side))]
    );
    luck14 = gpGame->GetLuck(m_heroes[H2EnumIndex(side)], NULL, m_combatTowns[H2EnumIndex(side)]);
    sprintf(
        gText,
        "\n%s%d\n%s%d\n%s%d\n%s%d\n%s%s\n%s%s\n\n%s%d/%d",
        cViewGeneralLabels[H2EnumIndex(HERO_PRIMARY_ATTACK)],
        m_heroes[H2EnumIndex(side)]->Stats(HERO_PRIMARY_ATTACK),
        cViewGeneralLabels[H2EnumIndex(HERO_PRIMARY_DEFENSE)],
        m_heroes[H2EnumIndex(side)]->Stats(HERO_PRIMARY_DEFENSE),
        cViewGeneralLabels[H2EnumIndex(HERO_PRIMARY_SPELL_POWER)],
        m_heroes[H2EnumIndex(side)]->Stats(HERO_PRIMARY_SPELL_POWER),
        cViewGeneralLabels[H2EnumIndex(HERO_PRIMARY_KNOWLEDGE)],
        m_heroes[H2EnumIndex(side)]->Stats(HERO_PRIMARY_KNOWLEDGE),
        cViewGeneralLabels[GENERAL_LABEL_MORALE],
        gMoraleText[morale11 + GENERAL_MORALE_TEXT_OFFSET],
        cViewGeneralLabels[GENERAL_LABEL_LUCK],
        gLuckText[luck14 + GENERAL_LUCK_TEXT_OFFSET],
        cViewGeneralLabels[GENERAL_LABEL_MANA],
        m_heroes[H2EnumIndex(side)]->m_spellPoints,
        m_heroes[H2EnumIndex(side)]->Stats(HERO_PRIMARY_KNOWLEDGE) * GENERAL_MANA_PER_KNOWLEDGE
    );
    message16.payload.widget.command = VIEW_GENERAL_SET_TEXT;
    message16.payload.widget.id = GENERAL_STATS_WIDGET;
    message16.payload.widget.data.text = gText;
    generalWindow26->BroadcastMessage(message16);

    if (m_heroes[H2EnumIndex(side)] == NULL || allowActions == 0
        || m_heroes[H2EnumIndex(side)]->HasArtifact(ARTIFACT_MAGIC_BOOK) == 0 || m_heroCastSpell[H2EnumIndex(side)] != 0
        || giCurGeneral != m_currentSide) {
        message16.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
        message16.payload.widget.id = GENERAL_CLOSE;
        message16.payload.widget.data.value = H2EnumIndex(WIDGET_FLAG_ENABLED);
        generalWindow26->BroadcastMessage(message16);
        message16.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
        message16.payload.widget.data.value = H2EnumIndex(WIDGET_COMMAND_DIMMED);
        generalWindow26->BroadcastMessage(message16);
    }
    if (allowActions == 0 || m_heroes[H2EnumIndex(OppositeCombatSide(m_currentSide))] == NULL
        || giCurGeneral != m_currentSide
        || m_heroes[H2EnumIndex(side)]->m_isCaptain != 0) {
        message16.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
        message16.payload.widget.id = GENERAL_SURRENDER;
        message16.payload.widget.data.value = H2EnumIndex(WIDGET_FLAG_ENABLED);
        generalWindow26->BroadcastMessage(message16);
        message16.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
        message16.payload.widget.data.value = H2EnumIndex(WIDGET_COMMAND_DIMMED);
        generalWindow26->BroadcastMessage(message16);
    }
    if (allowActions == 0 || giCurGeneral != m_currentSide
        || (giCurGeneral == COMBAT_DEFENDER_SIDE
            && m_combatTowns[H2EnumIndex(COMBAT_DEFENDER_SIDE)] != NULL)
        || m_sideRetreated[H2EnumIndex(COMBAT_ATTACKER_SIDE)] != 0
        || m_sideRetreated[1] != 0 || m_heroes[H2EnumIndex(side)]->m_isCaptain != 0) {
        message16.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
        message16.payload.widget.id = GENERAL_RETREAT;
        message16.payload.widget.data.value = H2EnumIndex(WIDGET_FLAG_ENABLED);
        generalWindow26->BroadcastMessage(message16);
        message16.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
        message16.payload.widget.data.value = H2EnumIndex(WIDGET_COMMAND_DIMMED);
        generalWindow26->BroadcastMessage(message16);
    }

    if (quickView != 0) {
        gpWindowManager->AddWindow(generalWindow26, -1, 1);
        QuickViewWait();
        gpWindowManager->RemoveWindow(generalWindow26);
    } else {
        gpWindowManager->DoDialog(generalWindow26, HandleViewGeneral, 0);
    }
    delete generalWindow26;
    DrawFrame(1, 0, 0, 0, COMBAT_MOUSE_REDRAW_DELAY, 1, 1);
    if (quickView == 0)
        DoCommand(static_cast<CombatMessageCommand>(gpWindowManager->m_dialogResult));
    return 0;
}

MessageDispatchResult HandleViewGeneral(tag_message& message) {
    i16 msgConst8;
    i16 msgConst28;
    i16 msgConst1;
    i16 msgConst19;
    i16 msgConst6;
    i32 handled28;
    i16 msgConst0;
    i16 msgConst18;
    i16 msgConst17;
    i16 msgConst3;
    i16 msgConst4;
    i32 helpIndex36;
    i32 hintIndex11;
    i16 msgConst5;
    i16 msgConst9;
    i16 msgConst7;
    i16 msgConst15;
    msgConst18 = GENERAL_NAME_WIDGET;
    msgConst5 = GENERAL_PORTRAIT_WIDGET;
    msgConst0 = GENERAL_COLOR_WIDGET;
    msgConst3 = GENERAL_STATS_WIDGET;
    msgConst9 = GENERAL_CONTROL_NONE;
    msgConst19 = GENERAL_NAME_WIDGET;
    msgConst4 = GENERAL_CONTROL_SEVEN;
    msgConst7 = GENERAL_CONTROL_EIGHT;
    msgConst28 = GENERAL_CONTROL_NINE;
    msgConst15 = GENERAL_CLOSE;
    msgConst1 = GENERAL_RETREAT;
    msgConst6 = GENERAL_SURRENDER;
    msgConst8 = GENERAL_CONTROL_THIRTEEN;
    msgConst17 = GENERAL_CONTROL_FOURTEEN;
    handled28 = 0;

    switch (message.type) {
        case MESSAGE_WIDGET:
            if ((H2EnumIndex((message.payload.widget.modifiers) & (MESSAGE_MODIFIER_RIGHT_BUTTON)))) {
                helpIndex36 = -1;
                if (message.payload.widget.command == WIDGET_COMMAND_SELECT
                    || message.payload.widget.command == WIDGET_COMMAND_ALTERNATE_SELECT) {
                    switch (message.payload.widget.id) {
                        case GENERAL_CLOSE:
                            helpIndex36 = GENERAL_LONG_HELP_CLOSE;
                            break;
                        case GENERAL_RETREAT:
                            helpIndex36 = GENERAL_LONG_HELP_RETREAT;
                            break;
                        case GENERAL_SURRENDER:
                            helpIndex36 = GENERAL_LONG_HELP_SURRENDER;
                            break;
                        case GENERAL_CAST_SPELL:
                            helpIndex36 = GENERAL_LONG_HELP_CAST;
                            break;
                    }
                    if (helpIndex36 != -1)
                        NormalDialog(
                            cViewGeneralLongHelp[helpIndex36],
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
                }
                break;
            }
            switch (message.payload.widget.command) {
                case WIDGET_COMMAND_DESELECT:
                    switch (message.payload.widget.id) {
                        case GENERAL_CLOSE:
                        case GENERAL_RETREAT:
                        case GENERAL_SURRENDER:
                        case GENERAL_CAST_SPELL:
                            gpWindowManager->m_dialogResult = message.payload.widget.id;
                            handled28 = 1;
                            break;
                    }
                    break;
                default:
                    break;
            }
            break;
        case MESSAGE_MOUSE_MOVE:
            gpWindowManager->ConvertToHover(message);
            if (message.payload.hover.id == gpWindowManager->m_lastHoverId)
                return MESSAGE_DISPATCH_CONSUME;
            gpWindowManager->m_lastHoverId = message.payload.hover.id;
            switch (message.payload.hover.id) {
                case GENERAL_CLOSE:
                    hintIndex11 = GENERAL_HOVER_HELP_CLOSE;
                    break;
                case GENERAL_RETREAT:
                    hintIndex11 = GENERAL_HOVER_HELP_RETREAT;
                    break;
                case GENERAL_SURRENDER:
                    hintIndex11 = GENERAL_HOVER_HELP_SURRENDER;
                    break;
                case GENERAL_CAST_SPELL:
                    hintIndex11 = GENERAL_HOVER_HELP_CAST;
                    break;
                default:
                    hintIndex11 = GENERAL_HOVER_HELP_HERO;
                    break;
            }
            if (hintIndex11 == GENERAL_HOVER_HELP_HERO
                && gpCombatManager->m_heroes[H2EnumIndex(iViewGeneralWhichSide)]->m_isCaptain)
                hintIndex11 = GENERAL_HOVER_HELP_CAPTAIN;
            gpCombatManager->CombatMessage(cViewGeneralHelp[hintIndex11], 1, 0, 0);
            return MESSAGE_DISPATCH_CONSUME;
    }
    if (handled28) {
        message.payload.widget.id = GENERAL_CLOSE;
        message.payload.widget.command = BaseWidgetCommand(message.payload.widget.id);
        return MESSAGE_DISPATCH_FORWARD;
    }
    return MESSAGE_DISPATCH_CONSUME;
}

void combatManager::ViewArmy(army* viewedArmy, i32 quickView) {
        i32 xWnd;
        i16 viewYOffsetConst;
        i32 yWindow;
        i32 xDelta;
        i16 viewXOffsetFixed;
        CombatSide side;
        i16 viewWidthConstant;
        i16 viewHeightConstant;

    if (viewedArmy == NULL)
        return;
    {

        viewWidthConstant = ARMY_WIDTH - ARMY_VIEW_X_OFFSET;
        viewHeightConstant = ARMY_HEIGHT;
        viewXOffsetFixed = ARMY_VIEW_X_OFFSET;
        viewYOffsetConst = ARMY_Y_OFFSET;
        xWnd = m_hexCells[viewedArmy->m_hex].m_x;
        yWindow = m_hexCells[viewedArmy->m_hex].m_y;
        xDelta =
            (viewedArmy->m_facing == ARMY_FACING_LEFT ? ARMY_FACING_OFFSET_DELTA : 0)
            + ARMY_RIGHT_FACING_X_OFFSET;
        xWnd -= xDelta;
        if (xWnd < 0)
            xWnd = 0;
        if (xWnd + ARMY_WIDTH > ARMY_SCREEN_WIDTH)
            xWnd = ARMY_RIGHT_CLAMP;
        yWindow -= ARMY_Y_OFFSET;
        if (yWindow < 0)
            yWindow = 0;
        if (yWindow + ARMY_HEIGHT > ARMY_SCREEN_HEIGHT)
            yWindow = ARMY_BOTTOM_CLAMP;
        side = viewedArmy->m_side;
        gpGame->ViewArmy(
            xWnd,
            yWindow,
            viewedArmy->m_monsterType,
            viewedArmy->m_quantity,
            m_combatTowns[H2EnumIndex(side)],
            1,
            viewedArmy->m_facing,
            quickView,
            m_heroes[H2EnumIndex(side)],
            viewedArmy,
            m_armyGroups[H2EnumIndex(side)],
            0
        );
    }
}

H2EnumStorage<CombatSide, i32> iViewGeneralWhichSide = COMBAT_ATTACKER_SIDE;
