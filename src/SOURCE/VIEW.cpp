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
    i16 msgConst1;
    i16 msgConst12;
    i16 msgConst5;
    i16 msgConst17;
    i32 cost;
    i16 msgConst18;
    i16 msgConst;
    i16 msgConst0;
    i16 msgConst59;
    i16 msgConst31;
    heroWindow* generalWindow18;
    i16 msgConst25;
    i16 msgConst19;
    i16 msgConst72;
    i16 msgConst66;
    i32 morale8;
    i32 luck10;
    i16 msgConst32;
    i16 msgConst26;
    tag_message message15;

    if (m_heroes[(side)] == NULL)
        return 0;
    iViewGeneralWhichSide = side;
    msgConst25 = GENERAL_NAME_WIDGET;
    msgConst72 = GENERAL_PORTRAIT_WIDGET;
    msgConst31 = GENERAL_COLOR_WIDGET;
    msgConst0 = GENERAL_STATS_WIDGET;
    msgConst12 = GENERAL_CAPTAIN_WIDGET;
    msgConst66 = GENERAL_CONTROL_NONE;
    msgConst18 = GENERAL_NAME_WIDGET;
    msgConst = GENERAL_CONTROL_SEVEN;
    msgConst32 = GENERAL_CONTROL_EIGHT;
    msgConst17 = GENERAL_CONTROL_NINE;
    msgConst26 = GENERAL_CLOSE;
    msgConst5 = GENERAL_RETREAT;
    msgConst59 = GENERAL_SURRENDER;
    msgConst1 = GENERAL_CONTROL_THIRTEEN;
    msgConst19 = GENERAL_CONTROL_FOURTEEN;
    giCurGeneral = side;

    message15.type = MESSAGE_WIDGET;
    generalWindow18 = new heroWindow(GENERAL_WINDOW_X, GENERAL_WINDOW_Y, "vgenwin.bin");
    if (generalWindow18 == NULL)
        MemError();
    sprintf(gText, "port%04d.icn", (m_heroes[(side)]->m_portrait));
    message15.payload.widget.command = VIEW_GENERAL_SET_ICON;
    message15.payload.widget.id = GENERAL_PORTRAIT_WIDGET;
    message15.payload.widget.data.text = gText;
    generalWindow18->BroadcastMessage(message15);

    message15.payload.widget.command =
        m_heroes[(side)]->m_isCaptain ? WIDGET_COMMAND_SET_FLAGS : WIDGET_COMMAND_CLEAR_FLAGS;
    message15.payload.widget.id = GENERAL_CAPTAIN_WIDGET;
    message15.payload.widget.data.value = (WIDGET_FLAG_DRAW);
    generalWindow18->BroadcastMessage(message15);
    if (m_heroes[(side)]->m_isCaptain) {
        message15.payload.widget.command = VIEW_GENERAL_SET_FRAME;
        if (m_playerId[(side)] == -1)
            message15.payload.widget.data.value = GENERAL_CAPTAIN_FRAME;
        else
            message15.payload.widget.data.value =
                gpGame->m_players[m_playerId[(side)]].m_color;
        generalWindow18->BroadcastMessage(message15);
    }

    message15.payload.widget.command = VIEW_GENERAL_SET_FRAME;
    message15.payload.widget.id = GENERAL_COLOR_WIDGET;
    message15.payload.widget.data.value = gpGame->GetPlayerColor(m_heroes[(side)]->m_owner) + 1;
    generalWindow18->BroadcastMessage(message15);

    if (m_heroes[(side)]->m_isCaptain != 0)
        sprintf(gText, "Captain");
    else
        sprintf(
            gText,
            "%s the %s",
            m_heroes[(side)]->m_name,
            gAlignmentNames[(m_heroes[(side)]->m_cursorType)]
        );
    message15.payload.widget.command = VIEW_GENERAL_SET_TEXT;
    message15.payload.widget.id = GENERAL_NAME_WIDGET;
    message15.payload.widget.data.text = gText;
    generalWindow18->BroadcastMessage(message15);

    morale8 = m_heroes[(side)]->m_army.GetMorale(
        m_heroes[(side)],
        m_combatTowns[(side)],
        gpCombatManager->m_armyGroups[(OppositeCombatSide(side))]
    );
    luck10 = gpGame->GetLuck(m_heroes[(side)], NULL, m_combatTowns[(side)]);
    sprintf(
        gText,
        "\n%s%d\n%s%d\n%s%d\n%s%d\n%s%s\n%s%s\n\n%s%d/%d",
        cViewGeneralLabels[(HERO_PRIMARY_ATTACK)],
        m_heroes[(side)]->Stats(HERO_PRIMARY_ATTACK),
        cViewGeneralLabels[(HERO_PRIMARY_DEFENSE)],
        m_heroes[(side)]->Stats(HERO_PRIMARY_DEFENSE),
        cViewGeneralLabels[(HERO_PRIMARY_SPELL_POWER)],
        m_heroes[(side)]->Stats(HERO_PRIMARY_SPELL_POWER),
        cViewGeneralLabels[(HERO_PRIMARY_KNOWLEDGE)],
        m_heroes[(side)]->Stats(HERO_PRIMARY_KNOWLEDGE),
        cViewGeneralLabels[GENERAL_LABEL_MORALE],
        gMoraleText[morale8 + GENERAL_MORALE_TEXT_OFFSET],
        cViewGeneralLabels[GENERAL_LABEL_LUCK],
        gLuckText[luck10 + GENERAL_LUCK_TEXT_OFFSET],
        cViewGeneralLabels[GENERAL_LABEL_MANA],
        m_heroes[(side)]->m_spellPoints,
        m_heroes[(side)]->Stats(HERO_PRIMARY_KNOWLEDGE) * GENERAL_MANA_PER_KNOWLEDGE
    );
    message15.payload.widget.command = VIEW_GENERAL_SET_TEXT;
    message15.payload.widget.id = GENERAL_STATS_WIDGET;
    message15.payload.widget.data.text = gText;
    generalWindow18->BroadcastMessage(message15);

    if (m_heroes[(side)] == NULL || allowActions == 0
        || m_heroes[(side)]->HasArtifact(ARTIFACT_MAGIC_BOOK) == 0 || m_heroCastSpell[(side)] != 0
        || m_currentSide != giCurGeneral) {
        message15.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
        message15.payload.widget.id = GENERAL_CLOSE;
        message15.payload.widget.data.value = (WIDGET_FLAG_ENABLED);
        generalWindow18->BroadcastMessage(message15);
        message15.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
        message15.payload.widget.data.value = (WIDGET_COMMAND_DIMMED);
        generalWindow18->BroadcastMessage(message15);
    }
    if (allowActions == 0 || m_heroes[(OppositeCombatSide(m_currentSide))] == NULL
        || m_currentSide != giCurGeneral
        || m_heroes[(side)]->m_isCaptain != 0) {
        message15.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
        message15.payload.widget.id = GENERAL_SURRENDER;
        message15.payload.widget.data.value = (WIDGET_FLAG_ENABLED);
        generalWindow18->BroadcastMessage(message15);
        message15.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
        message15.payload.widget.data.value = (WIDGET_COMMAND_DIMMED);
        generalWindow18->BroadcastMessage(message15);
    }
    if (allowActions == 0 || m_currentSide != giCurGeneral
        || (giCurGeneral == COMBAT_DEFENDER_SIDE
            && m_combatTowns[(COMBAT_DEFENDER_SIDE)] != NULL)
        || m_sideRetreated[(COMBAT_ATTACKER_SIDE)] != 0
        || m_sideRetreated[1] != 0 || m_heroes[(side)]->m_isCaptain != 0) {
        message15.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
        message15.payload.widget.id = GENERAL_RETREAT;
        message15.payload.widget.data.value = (WIDGET_FLAG_ENABLED);
        generalWindow18->BroadcastMessage(message15);
        message15.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
        message15.payload.widget.data.value = (WIDGET_COMMAND_DIMMED);
        generalWindow18->BroadcastMessage(message15);
    }

    if (quickView != 0) {
        gpWindowManager->AddWindow(generalWindow18, -1, 1);
        QuickViewWait();
        gpWindowManager->RemoveWindow(generalWindow18);
    } else {
        gpWindowManager->DoDialog(generalWindow18, HandleViewGeneral, 0);
    }
    delete generalWindow18;
    DrawFrame(1, 0, 0, 0, COMBAT_MOUSE_REDRAW_DELAY, 1, 1);
    if (quickView == 0)
        DoCommand(static_cast<CombatMessageCommand>(gpWindowManager->m_dialogResult));
    return 0;
}

MessageDispatchResult HandleViewGeneral(tag_message& message) {
    i16 msgConst;
    i16 msgConst27;
    i16 msgConst0;
    i16 msgConst28;
    i16 msgConst71;
    i32 handled3;
    i16 msgConst65;
    i16 msgConst59;
    i16 msgConst31;
    i16 msgConst25;
    i16 msgConst19;
    i32 helpIndex36;
    i32 hintIndex11;
    i16 msgConst72;
    i16 msgConst66;
    i16 msgConst32;
    i16 msgConst26;
    msgConst59 = GENERAL_NAME_WIDGET;
    msgConst72 = GENERAL_PORTRAIT_WIDGET;
    msgConst65 = GENERAL_COLOR_WIDGET;
    msgConst25 = GENERAL_STATS_WIDGET;
    msgConst66 = GENERAL_CONTROL_NONE;
    msgConst28 = GENERAL_NAME_WIDGET;
    msgConst19 = GENERAL_CONTROL_SEVEN;
    msgConst32 = GENERAL_CONTROL_EIGHT;
    msgConst27 = GENERAL_CONTROL_NINE;
    msgConst26 = GENERAL_CLOSE;
    msgConst0 = GENERAL_RETREAT;
    msgConst71 = GENERAL_SURRENDER;
    msgConst = GENERAL_CONTROL_THIRTEEN;
    msgConst31 = GENERAL_CONTROL_FOURTEEN;
    handled3 = 0;

    switch (message.type) {
        case MESSAGE_WIDGET:
            if ((((message.payload.widget.modifiers) & (MESSAGE_MODIFIER_RIGHT_BUTTON)))) {
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
                            handled3 = 1;
                            break;
                    }
                    break;
                default:
                    break;
            }
            break;
        case MESSAGE_MOUSE_MOVE:
            gpWindowManager->ConvertToHover(message);
            if (gpWindowManager->m_lastHoverId == message.payload.hover.id)
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
                && gpCombatManager->m_heroes[(iViewGeneralWhichSide)]->m_isCaptain)
                hintIndex11 = GENERAL_HOVER_HELP_CAPTAIN;
            gpCombatManager->CombatMessage(cViewGeneralHelp[hintIndex11], 1, 0, 0);
            return MESSAGE_DISPATCH_CONSUME;
    }
    if (handled3) {
        message.payload.widget.id = GENERAL_CLOSE;
        message.payload.widget.command = BaseWidgetCommand(message.payload.widget.id);
        return MESSAGE_DISPATCH_FORWARD;
    }
    return MESSAGE_DISPATCH_CONSUME;
}

void combatManager::ViewArmy(army* viewedArmy, i32 quickView) {
    if (viewedArmy == NULL)
        return;
    {
        CombatSide side;
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
            (viewedArmy->m_facing == ARMY_FACING_LEFT ? ARMY_FACING_OFFSET_DELTA : 0)
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
            viewedArmy->m_monsterType,
            viewedArmy->m_quantity,
            m_combatTowns[(side)],
            1,
            viewedArmy->m_facing,
            quickView,
            m_heroes[(side)],
            viewedArmy,
            m_armyGroups[(side)],
            0
        );
    }
}

i32 iViewGeneralWhichSide = COMBAT_ATTACKER_SIDE;
