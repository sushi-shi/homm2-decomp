#include <va.h>
#include <SOURCE/hero.h>
#include <BASE/message.h>
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
#include <BASE/dialog.h>
#include <BASE/display.h>
H2_ENUM_BEGIN(ViewGeneralAction)
    GENERAL_ACTION_CLOSE      = DIALOG_BUTTON_0,
    GENERAL_ACTION_CAST_SPELL = IDX(COMBAT_MESSAGE_COMMAND_CAST_SPELL),
    GENERAL_ACTION_RETREAT    = IDX(COMBAT_MESSAGE_COMMAND_RETREAT),
    GENERAL_ACTION_SURRENDER  = IDX(COMBAT_MESSAGE_COMMAND_SURRENDER)
H2_ENUM_END(ViewGeneralAction)

H2_ENUM_BEGIN(ViewGeneralConstant)
    GENERAL_WINDOW_X           = 179,
    GENERAL_WINDOW_Y           = 60,
    GENERAL_TEXT_COLOR_COUNT   = 11,
    GENERAL_MORALE_TEXT_OFFSET = 3,
    GENERAL_LUCK_TEXT_OFFSET   = 3,
    GENERAL_CAPTAIN_FRAME      = 6,
    ARMY_WIDTH                 = 488,
    ARMY_HEIGHT                = 229,
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
    GENERAL_LONG_HELP_CAST_SPELL = 0,
    GENERAL_LONG_HELP_RETREAT    = 1,
    GENERAL_LONG_HELP_SURRENDER  = 2,
    GENERAL_LONG_HELP_CLOSE      = 3
H2_ENUM_END(ViewGeneralLongHelp)

H2_ENUM_BEGIN(ViewGeneralHoverHelp)
    GENERAL_HOVER_HELP_CAST_SPELL = 1,
    GENERAL_HOVER_HELP_RETREAT    = 2,
    GENERAL_HOVER_HELP_SURRENDER  = 3,
    GENERAL_HOVER_HELP_CLOSE      = 4,
    GENERAL_HOVER_HELP_HERO       = 5,
    GENERAL_HOVER_HELP_CAPTAIN    = 6
H2_ENUM_END(ViewGeneralHoverHelp)

#if H2_RETAIL_COMPILER
#define captainWidgetId msgConst7
#define closeWidgetId msgConst17
#define colorWidgetId msgConst15
#define cost cost2
#define eighthControlWidgetId msgConst2
#define fourteenthControlWidgetId msgConst6
#define generalWindow generalWindow26
#define luck luck14
#define message message16
#define morale morale11
#define nameWidgetId msgConst5
#define nameWidgetIdCopy msgConst26
#define ninthControlWidgetId msgConst28
#define noControlWidgetId msgConst18
#define portraitWidgetId msgConst1
#define retreatWidgetId msgConst3
#define seventhControlWidgetId msgConst4
#define statsWidgetId msgConst37
#define surrenderWidgetId msgConst29
#define thirteenthControlWidgetId msgConst10
#endif
VA(0x004ad4b0, 0x6f9)
i32 combatManager::ViewGeneral(
    H2_ENUM_PARAM(CombatSide, i32) side, i32 allowActions, i32 quickView
) {
    i16 H2_UNUSED(statsWidgetId);
    i16 H2_UNUSED(seventhControlWidgetId);
    i16 H2_UNUSED(thirteenthControlWidgetId);
    i16 H2_UNUSED(retreatWidgetId);
    i16 H2_UNUSED(surrenderWidgetId);
    i16 H2_UNUSED(eighthControlWidgetId);
    i32 luck;
    i16 H2_UNUSED(portraitWidgetId);
    i32 morale;
    i16 H2_UNUSED(captainWidgetId);
    i16 H2_UNUSED(fourteenthControlWidgetId);
    i16 H2_UNUSED(nameWidgetId);
    i16 H2_UNUSED(closeWidgetId);
    i16 H2_UNUSED(noControlWidgetId);
    i16 H2_UNUSED(colorWidgetId);
    i32 H2_UNUSED(cost);
    heroWindow* generalWindow;
    i16 H2_UNUSED(ninthControlWidgetId);
    tag_message message;
    i16 H2_UNUSED(nameWidgetIdCopy);

    if (m_heroes[IDX(side)] == NULL)
        return 0;
    iViewGeneralWhichSide = side;
    nameWidgetId = GENERAL_NAME_WIDGET;
    portraitWidgetId = GENERAL_PORTRAIT_WIDGET;
    colorWidgetId = GENERAL_COLOR_WIDGET;
    statsWidgetId = GENERAL_STATS_WIDGET;
    captainWidgetId = GENERAL_CAPTAIN_WIDGET;
    noControlWidgetId = GENERAL_CONTROL_NONE;
    nameWidgetIdCopy = GENERAL_NAME_WIDGET;
    seventhControlWidgetId = GENERAL_CONTROL_SEVEN;
    eighthControlWidgetId = GENERAL_CONTROL_EIGHT;
    ninthControlWidgetId = GENERAL_CONTROL_NINE;
    closeWidgetId = GENERAL_ACTION_CAST_SPELL;
    retreatWidgetId = GENERAL_ACTION_RETREAT;
    surrenderWidgetId = GENERAL_ACTION_SURRENDER;
    thirteenthControlWidgetId = GENERAL_CONTROL_THIRTEEN;
    fourteenthControlWidgetId = GENERAL_CONTROL_FOURTEEN;
    giCurGeneral = side;

    message.type = MESSAGE_WIDGET;
    generalWindow = new heroWindow(GENERAL_WINDOW_X, GENERAL_WINDOW_Y, "vgenwin.bin");
    if (generalWindow == NULL)
        MemError();
    sprintf(gText, "port%04d.icn", IDX(m_heroes[IDX(side)]->m_portrait));
    message.payload.widget.command = WIDGET_COMMAND_SET_ICON;
    message.payload.widget.id = GENERAL_PORTRAIT_WIDGET;
    message.payload.widget.data.text = gText;
    generalWindow->BroadcastMessage(message);

    message.payload.widget.command =
        m_heroes[IDX(side)]->m_isCaptain ? WIDGET_COMMAND_SET_FLAGS : WIDGET_COMMAND_CLEAR_FLAGS;
    message.payload.widget.id = GENERAL_CAPTAIN_WIDGET;
    message.payload.widget.data.value = IDX(WIDGET_FLAG_DRAW);
    generalWindow->BroadcastMessage(message);
    if (m_heroes[IDX(side)]->m_isCaptain) {
        message.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
        message.payload.widget.data.value = m_playerId[IDX(side)] == -1
            ? GENERAL_CAPTAIN_FRAME
            : gpGame->m_players[m_playerId[IDX(side)]].m_color;
        generalWindow->BroadcastMessage(message);
    }

    message.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
    message.payload.widget.id = GENERAL_COLOR_WIDGET;
    message.payload.widget.data.value = gpGame->GetPlayerColor(m_heroes[IDX(side)]->m_owner) + 1;
    generalWindow->BroadcastMessage(message);

    if (m_heroes[IDX(side)]->m_isCaptain != 0)
        sprintf(
            gText,
             localization::Tr("hero.captain")
        );
    else
        sprintf(
            gText,
            "%s - %s",
            m_heroes[IDX(side)]->m_name,
            gAlignmentNames[IDX(m_heroes[IDX(side)]->m_cursorType)]
        );
    message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    message.payload.widget.id = GENERAL_NAME_WIDGET;
    message.payload.widget.data.text = gText;
    generalWindow->BroadcastMessage(message);

    morale = m_heroes[IDX(side)]->m_army.GetMorale(
        m_heroes[IDX(side)],
        m_combatTowns[IDX(side)],
        gpCombatManager->m_armyGroups[IDX(OppositeCombatSide(side))]
    );
    luck = gpGame->GetLuck(m_heroes[IDX(side)], NULL, m_combatTowns[IDX(side)]);
    sprintf(
        gText,
        "\n%s%d\n%s%d\n%s%d\n%s%d\n%s%s\n%s%s\n\n%s%d/%d",
        cViewGeneralLabels[IDX(HERO_PRIMARY_ATTACK)],
        m_heroes[IDX(side)]->Stats(HERO_PRIMARY_ATTACK),
        cViewGeneralLabels[IDX(HERO_PRIMARY_DEFENSE)],
        m_heroes[IDX(side)]->Stats(HERO_PRIMARY_DEFENSE),
        cViewGeneralLabels[IDX(HERO_PRIMARY_SPELL_POWER)],
        m_heroes[IDX(side)]->Stats(HERO_PRIMARY_SPELL_POWER),
        cViewGeneralLabels[IDX(HERO_PRIMARY_KNOWLEDGE)],
        m_heroes[IDX(side)]->Stats(HERO_PRIMARY_KNOWLEDGE),
        cViewGeneralLabels[GENERAL_LABEL_MORALE],
        gMoraleText[morale + GENERAL_MORALE_TEXT_OFFSET],
        cViewGeneralLabels[GENERAL_LABEL_LUCK],
        gLuckText[luck + GENERAL_LUCK_TEXT_OFFSET],
        cViewGeneralLabels[GENERAL_LABEL_MANA],
        m_heroes[IDX(side)]->m_spellPoints,
        HERO_NORMAL_SPELL_POINTS(*m_heroes[IDX(side)])
    );
    message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    message.payload.widget.id = GENERAL_STATS_WIDGET;
    message.payload.widget.data.text = gText;
    generalWindow->BroadcastMessage(message);

    if (m_heroes[IDX(side)] == NULL || allowActions == 0
        || m_heroes[IDX(side)]->HasArtifact(ARTIFACT_MAGIC_BOOK) == 0 || m_heroCastSpell[IDX(side)] != 0
        || giCurGeneral != m_currentSide) {
        message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
        message.payload.widget.id = GENERAL_ACTION_CAST_SPELL;
        message.payload.widget.data.value = IDX(WIDGET_FLAG_ENABLED);
        generalWindow->BroadcastMessage(message);
        message.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
        message.payload.widget.data.value = IDX(WIDGET_FLAGS_ARGUMENT_DIMMED);
        generalWindow->BroadcastMessage(message);
    }
    if (allowActions == 0 || m_heroes[IDX(OppositeCombatSide(m_currentSide))] == NULL
        || giCurGeneral != m_currentSide
        || m_heroes[IDX(side)]->m_isCaptain != 0) {
        message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
        message.payload.widget.id = GENERAL_ACTION_SURRENDER;
        message.payload.widget.data.value = IDX(WIDGET_FLAG_ENABLED);
        generalWindow->BroadcastMessage(message);
        message.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
        message.payload.widget.data.value = IDX(WIDGET_FLAGS_ARGUMENT_DIMMED);
        generalWindow->BroadcastMessage(message);
    }
    if (allowActions == 0 || giCurGeneral != m_currentSide
        || (giCurGeneral == COMBAT_DEFENDER_SIDE
            && m_combatTowns[IDX(COMBAT_DEFENDER_SIDE)] != NULL)
        || m_sideRetreated[IDX(COMBAT_ATTACKER_SIDE)] != 0
        || m_sideRetreated[1] != 0 || m_heroes[IDX(side)]->m_isCaptain != 0) {
        message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
        message.payload.widget.id = GENERAL_ACTION_RETREAT;
        message.payload.widget.data.value = IDX(WIDGET_FLAG_ENABLED);
        generalWindow->BroadcastMessage(message);
        message.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
        message.payload.widget.data.value = IDX(WIDGET_FLAGS_ARGUMENT_DIMMED);
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
    return 0;
}
#if H2_RETAIL_COMPILER
#undef captainWidgetId
#undef closeWidgetId
#undef colorWidgetId
#undef cost
#undef eighthControlWidgetId
#undef fourteenthControlWidgetId
#undef generalWindow
#undef luck
#undef message
#undef morale
#undef nameWidgetId
#undef nameWidgetIdCopy
#undef ninthControlWidgetId
#undef noControlWidgetId
#undef portraitWidgetId
#undef retreatWidgetId
#undef seventhControlWidgetId
#undef statsWidgetId
#undef surrenderWidgetId
#undef thirteenthControlWidgetId
#endif

#if H2_RETAIL_COMPILER
#define closeWidgetId msgConst15
#define colorWidgetId msgConst0
#define eighthControlWidgetId msgConst7
#define fourteenthControlWidgetId msgConst17
#define handled handled28
#define helpIndex helpIndex36
#define hintIndex hintIndex11
#define nameWidgetId msgConst19
#define nameWidgetIdCopy msgConst18
#define ninthControlWidgetId msgConst28
#define noControlWidgetId msgConst9
#define portraitWidgetId msgConst5
#define retreatWidgetId msgConst1
#define seventhControlWidgetId msgConst4
#define statsWidgetId msgConst3
#define surrenderWidgetId msgConst6
#define thirteenthControlWidgetId msgConst8
#endif
VA(0x004adba9, 0x286)
MessageDispatchResult HandleViewGeneral(tag_message& message) {
    i16 H2_UNUSED(thirteenthControlWidgetId);
    i16 H2_UNUSED(ninthControlWidgetId);
    i16 H2_UNUSED(retreatWidgetId);
    i16 H2_UNUSED(nameWidgetId);
    i16 H2_UNUSED(surrenderWidgetId);
    b32 handled;
    i16 H2_UNUSED(colorWidgetId);
    i16 H2_UNUSED(nameWidgetIdCopy);
    i16 H2_UNUSED(fourteenthControlWidgetId);
    i16 H2_UNUSED(statsWidgetId);
    i16 H2_UNUSED(seventhControlWidgetId);
    i32 helpIndex;
    i32 hintIndex;
    i16 H2_UNUSED(portraitWidgetId);
    i16 H2_UNUSED(noControlWidgetId);
    i16 H2_UNUSED(eighthControlWidgetId);
    i16 H2_UNUSED(closeWidgetId);
    nameWidgetIdCopy = GENERAL_NAME_WIDGET;
    portraitWidgetId = GENERAL_PORTRAIT_WIDGET;
    colorWidgetId = GENERAL_COLOR_WIDGET;
    statsWidgetId = GENERAL_STATS_WIDGET;
    noControlWidgetId = GENERAL_CONTROL_NONE;
    nameWidgetId = GENERAL_NAME_WIDGET;
    seventhControlWidgetId = GENERAL_CONTROL_SEVEN;
    eighthControlWidgetId = GENERAL_CONTROL_EIGHT;
    ninthControlWidgetId = GENERAL_CONTROL_NINE;
    closeWidgetId = GENERAL_ACTION_CAST_SPELL;
    retreatWidgetId = GENERAL_ACTION_RETREAT;
    surrenderWidgetId = GENERAL_ACTION_SURRENDER;
    thirteenthControlWidgetId = GENERAL_CONTROL_THIRTEEN;
    fourteenthControlWidgetId = GENERAL_CONTROL_FOURTEEN;
    handled = false;

    switch (message.type) {
        case MESSAGE_WIDGET:
            if (HAS(message.payload.widget.modifiers, MESSAGE_MODIFIER_RIGHT_BUTTON)) {
                helpIndex = -1;
                if (IS_WIDGET_SELECTION_NOTIFICATION(message.payload.widget.command)) {
                    switch (message.payload.widget.id) {
                        case GENERAL_ACTION_CAST_SPELL:
                            helpIndex = GENERAL_LONG_HELP_CAST_SPELL;
                            break;
                        case GENERAL_ACTION_RETREAT:
                            helpIndex = GENERAL_LONG_HELP_RETREAT;
                            break;
                        case GENERAL_ACTION_SURRENDER:
                            helpIndex = GENERAL_LONG_HELP_SURRENDER;
                            break;
                        case GENERAL_ACTION_CLOSE:
                            helpIndex = GENERAL_LONG_HELP_CLOSE;
                            break;
                    }
                    if (helpIndex != -1)
                        NormalDialog(cViewGeneralLongHelp[helpIndex], NORMAL_DIALOG_QUICK_VIEW);
                }
                break;
            }
            switch (message.payload.widget.command) {
                case WIDGET_NOTIFY_DESELECT:
                    switch (message.payload.widget.id) {
                        case GENERAL_ACTION_CAST_SPELL:
                        case GENERAL_ACTION_RETREAT:
                        case GENERAL_ACTION_SURRENDER:
                        case GENERAL_ACTION_CLOSE:
                            gpWindowManager->m_dialogResult = message.payload.widget.id;
                            handled = true;
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
                case GENERAL_ACTION_CAST_SPELL:
                    hintIndex = GENERAL_HOVER_HELP_CAST_SPELL;
                    break;
                case GENERAL_ACTION_RETREAT:
                    hintIndex = GENERAL_HOVER_HELP_RETREAT;
                    break;
                case GENERAL_ACTION_SURRENDER:
                    hintIndex = GENERAL_HOVER_HELP_SURRENDER;
                    break;
                case GENERAL_ACTION_CLOSE:
                    hintIndex = GENERAL_HOVER_HELP_CLOSE;
                    break;
                default:
                    hintIndex = GENERAL_HOVER_HELP_HERO;
                    break;
            }
            if (hintIndex == GENERAL_HOVER_HELP_HERO
                && gpCombatManager->m_heroes[IDX(iViewGeneralWhichSide)]->m_isCaptain)
                hintIndex = GENERAL_HOVER_HELP_CAPTAIN;
            gpCombatManager->CombatMessage(cViewGeneralHelp[hintIndex], 1, 0, 0);
            return MESSAGE_DISPATCH_CONSUME;
    }
    if (handled) {
        message.payload.widget.id = IDX(WIDGET_COMMAND_DIALOG_SELECT);
        message.payload.widget.command = BaseWidgetCommand(message.payload.widget.id);
        return MESSAGE_DISPATCH_FORWARD;
    }
    return MESSAGE_DISPATCH_CONSUME;
}
#if H2_RETAIL_COMPILER
#undef closeWidgetId
#undef colorWidgetId
#undef eighthControlWidgetId
#undef fourteenthControlWidgetId
#undef handled
#undef helpIndex
#undef hintIndex
#undef nameWidgetId
#undef nameWidgetIdCopy
#undef ninthControlWidgetId
#undef noControlWidgetId
#undef portraitWidgetId
#undef retreatWidgetId
#undef seventhControlWidgetId
#undef statsWidgetId
#undef surrenderWidgetId
#undef thirteenthControlWidgetId
#endif

VA(0x004ade2f, 0x143)
void combatManager::ViewArmy(army* viewedArmy, i32 quickView) {
        i32 xWnd;
        i16 H2_UNUSED(viewYOffsetConst);
        i32 yWindow;
        i32 xDelta;
        i16 H2_UNUSED(viewXOffsetFixed);
        CombatSide side;
        i16 H2_UNUSED(viewWidthConstant);
        i16 H2_UNUSED(viewHeightConstant);

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
        if (xWnd + ARMY_WIDTH > LOGICAL_SCREEN_WIDTH)
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
            m_combatTowns[IDX(side)],
            1,
            viewedArmy->m_facing,
            quickView,
            m_heroes[IDX(side)],
            viewedArmy,
            m_armyGroups[IDX(side)],
            0
        );
    }
}

DATA(0x00533f7c) H2_ENUM_STORAGE(CombatSide, i32) iViewGeneralWhichSide = COMBAT_ATTACKER_SIDE;
