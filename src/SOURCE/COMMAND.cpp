#include <va.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <BASE/bmap2.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/iconWidget.h>
#include <BASE/inputManager.h>
#include <BASE/Misc.h>
#include <BASE/mouseManager.h>
#include <BASE/resourceManager.h>
#include <BASE/soundManager.h>
#include <BASE/textWidget.h>
#include <SOURCE/advManager.h>
#include <SOURCE/combatManager.h>
#include <SOURCE/COMMAND.h>
#include <SOURCE/GAME.h>
#include <SOURCE/game.h>
#include <SOURCE/KB.h>
#include <SOURCE/kbwin.h>
#include <SOURCE/NOOPT.h>
#include <SOURCE/PATH.h>
#include <SOURCE/PHILAI.h>
#include <SOURCE/REMOTE.h>
#include <SOURCE/town.h>
#include <SOURCE/X_GLOBAL.h>

#define COMBAT_NECROMANCY_LEVEL_FACTOR DATA_COMPGEN(0x004eb238, lEVELFACTORConstant, 0.1)
#define COMBAT_SURRENDER_QUILL_FACTOR 0.1
#define COMBAT_SURRENDER_BASE_FACTOR                                                   \
    DATA_COMPGEN(0x004eb240, combatSharedHalfFactor, 0.5)
#define COMBAT_SURRENDER_DIPLOMACY_FACTOR DATA_COMPGEN(0x004eb248, dIPLOMACYFACTORConstant, 0.2)
#define COMBAT_IDLE_ROLL_DIVISOR DATA_COMPGEN(0x004eb258, rOLLDIVISORConstant, 100.0f)
#define COMBAT_STAND_DELAY_BASE_FACTOR DATA_COMPGEN(0x004eb260, bASEFACTORConstant, 0.25)
#define COMBAT_STAND_DELAY_RANDOM_FACTOR 0.5
#define COMBAT_CYCLE_TIMER_FACTOR DATA_COMPGEN(0x004eb268, tIMERFACTORConstant, 150.0f)
#define COMBAT_SOUND_POLL_DELAY DATA_COMPGEN(0x004eb208, pOLLDELAYConstant, 75.0f)
#define COMBAT_DIRECTION_SLOPE_STEEPEST DATA_COMPGEN(0x004eb210, sLOPESTEEPESTConstant, 3.73)
#define COMBAT_DIRECTION_SLOPE_STEEP DATA_COMPGEN(0x004eb218, sLOPESTEEPConstant, 1.73)
#define COMBAT_DIRECTION_SLOPE_DIAGONAL DATA_COMPGEN(0x004eb220, sLOPEDIAGONALConstant, 1.0f)
#define COMBAT_DIRECTION_SLOPE_SHALLOW DATA_COMPGEN(0x004eb228, sLOPESHALLOWConstant, 0.58)
#define COMBAT_DIRECTION_SLOPE_SHALLOWEST DATA_COMPGEN(0x004eb230, sLOPESHALLOWESTConstant, 0.27)

namespace {

H2_ENUM_CLASS_BEGIN(CombatKeyCommand)
    KEY_MOUSE_COORDS          = 2,
    KEY_VIEW_ARMY             = 0x14,
    KEY_VIEW_GENERAL          = 0x23,
    KEY_CAST_SPELL            = 0x2e,
    KEY_WAIT                  = 0x39,
    KEY_CLOSE_NETWORK_BOX     = 0x3c,
    KEY_REDRAW_SCREEN         = 0x3d,
    KEY_CYCLE_ARMY_VIEW       = 0x3f,
    KEY_TOGGLE_GRID           = 0x40,
    KEY_TOGGLE_MOUSE_HEX      = 0x41,
    KEY_CYCLE_SHADE           = 0x42,
    KEY_CYCLE_SPELL_EFFECT    = 0x43,
    KEY_DEBUG_CREATURE_EFFECT = 0x57
H2_ENUM_CLASS_END(CombatKeyCommand)

H2_ENUM_BEGIN(CombatWinLoseConstant)
    WIN_LOSE_WIDGET_COUNT               = 25,
    WIN_LOSE_NEXT_CONTROL               = 0x7800,
    WIN_LOSE_CLOSE_COMMAND              = 10,
    WIN_LOSE_TEXT_ID                    = 101,
    WIN_LOSE_RESOURCE_LOAD_ID           = 200,
    WIN_LOSE_RESOURCE_DRAW_ID           = 201,
    WIN_LOSE_DRAW_DEPTH                 = 0x7fff,
    WIN_LOSE_ANIMATION_CYCLE_FIRST      = 1,
    WIN_LOSE_ANIMATION_LOSS             = 2,
    WIN_LOSE_ANIMATION_FLEE             = 3,
    WIN_LOSE_ANIMATION_CYCLE_SECOND     = 4,
    WIN_LOSE_FIRST_ANIMATION_FRAME      = 1,
    WIN_LOSE_LOOP_FRAME_COUNT           = 20,
    WIN_LOSE_FLEE_SECOND_RESOURCE_FRAME = 43,
    WIN_LOSE_FLEE_THIRD_RESOURCE_FRAME  = 69,
    WIN_LOSE_FLEE_LAST_FRAME            = 25,
    WIN_LOSE_LOSS_SECOND_RESOURCE_FRAME = 30,
    WIN_LOSE_LOSS_THIRD_RESOURCE_FRAME  = 59,
    WIN_LOSE_LOSS_RESET_FRAME           = 80,
    WIN_LOSE_INITIAL_DELAY              = 90,
    WIN_LOSE_LOOP_DELAY                 = 99,
    WIN_LOSE_FLEE_DELAY                 = 40,
    WIN_LOSE_LOSS_DELAY                 = 50,
    WIN_LOSE_LOSS_FINAL_DELAY           = 100,
    WIN_LOSE_ARTIFACT_ICON_ID           = 2001,
    WIN_LOSE_ARTIFACT_IMAGE_ID          = 2002,
    WIN_LOSE_ARTIFACT_TEXT_ID           = 2101,
    WIN_LOSE_SKELETON_ICON_ID           = 900,
    WIN_LOSE_SKELETON_TEXT_ID           = 901,
    WIN_LOSE_EAGLE_BACKGROUND_ID        = 902,
    WIN_LOSE_EAGLE_SPELL_ID             = 903,
    WIN_LOSE_EAGLE_TEXT_ID              = 904,
    WIN_LOSE_ICON_FILENAME_SIZE         = 40,
    PICKUP_SAMPLE_FIRST                 = 1,
    PICKUP_SAMPLE_LAST                  = 5
H2_ENUM_END(CombatWinLoseConstant)

H2_ENUM_CLASS_BEGIN(CombatBattleResultText)
    RESULT_TEXT_ENEMY_SURRENDERED  = 0,
    RESULT_TEXT_ENEMY_FLED         = 1,
    RESULT_TEXT_VICTORY            = 2,
    RESULT_TEXT_EXPERIENCE         = 3,
    RESULT_TEXT_HERO_SURRENDER     = 4,
    RESULT_TEXT_HERO_FLEE          = 5,
    RESULT_TEXT_HERO_DEFEAT        = 6,
    RESULT_TEXT_FORCES_SURRENDER   = 7,
    RESULT_TEXT_FORCES_FLEE        = 8,
    RESULT_TEXT_FORCES_DEFEAT      = 9,
    RESULT_TEXT_NETWORK_EXPERIENCE = 10
H2_ENUM_CLASS_END(CombatBattleResultText)

H2_ENUM_CLASS_BEGIN(CombatControlId)
    CONTROL_ATTACK            = 1,
    CONTROL_WAIT              = 2,
    CONTROL_DISABLE_SELECTION = 3,
    CONTROL_SYSTEM_OPTIONS    = 4,
    CONTROL_HELP_FIRST        = 10,
    CONTROL_HELP_SECOND       = 11,
    CONTROL_HELP_THIRD        = 12,
    CONTROL_HELP_FOURTH       = 13,
    CONTROL_MAIN_BUTTON       = 0x40
H2_ENUM_CLASS_END(CombatControlId)

H2_ENUM_BEGIN(CombatRemoteConstant)
    REMOTE_PACKET_TYPE     = 1,
    REMOTE_COMMAND_MESSAGE = 11,
    REMOTE_COMMAND_ACTION  = 23
H2_ENUM_END(CombatRemoteConstant)

H2_ENUM_CLASS_BEGIN(CombatActionDataIndex)
    ACTION_DATA_ACTION      = 0,
    ACTION_DATA_EXTRA       = 1,
    ACTION_DATA_GRID        = 2,
    ACTION_DATA_SECOND_GRID = 3,
    ACTION_DATA_COUNT       = 4
H2_ENUM_CLASS_END(CombatActionDataIndex)

H2_ENUM_BEGIN(CombatDirectionConstant)
    DIRECTION_SECTOR_COUNT                   = 24,
    DIRECTION_SECTOR_QUARTER                 = 6,
    DIRECTION_SECTOR_HALF                    = 12,
    DIRECTION_SECTOR_THREE_QUARTERS          = 18,
    DIRECTION_PENDING_OFFSET                 = 10,
    DIRECTION_SECTORS_PER_ADJACENT           = 4,
    DIRECTION_MOUSE_TOP_OFFSET               = 5,
    DIRECTION_MOUSE_CENTER_OFFSET            = 21,
    DIRECTION_SECTOR_OFFSET_SHALLOW          = 2,
    DIRECTION_SECTOR_OFFSET_DIAGONAL         = 3,
    DIRECTION_SECTOR_OFFSET_STEEP            = 4,
    DIRECTION_SECTOR_OFFSET_STEEPEST         = 5,
    DIRECTION_SPECIAL_FIRST_SECTOR_START     = 11,
    DIRECTION_SPECIAL_FIRST_SECTOR_CENTER    = 12,
    DIRECTION_SPECIAL_FIRST_SECTOR_END       = 13,
    DIRECTION_SPECIAL_SECOND_SECTOR_PREVIOUS = 23,
    DIRECTION_SPECIAL_SECOND_SECTOR_CENTER   = 0,
    DIRECTION_SPECIAL_SECOND_SECTOR_NEXT     = 1
H2_ENUM_END(CombatDirectionConstant)

H2_ENUM_BEGIN(CombatCommandConstant)
    POINTER_VIEW             = 5,
    POINTER_ATTACK_OFFSET    = 7,
    INVALID_ARMY_INDEX       = -1,
    INVALID_HEX              = -1,
    PLAYER_NONE              = -1,
    SMALL_VIEW_POSITION_NONE = -1,
    IGNORED_HEX              = -2,
    CASTLE_GATE_HEX          = 59
H2_ENUM_END(CombatCommandConstant)

H2_ENUM_BEGIN(CombatRoundConstant)
    ROUND_INITIAL_SPEED = 15
H2_ENUM_END(CombatRoundConstant)

H2_ENUM_BEGIN(CombatCommandGeometry)
    CONTROL_RIGHT_MIN_X          = 590,
    CONTROL_LEFT_MAX_X           = 50,
    CONTROL_SYSTEM_OPTIONS_MIN_Y = 460,
    SCREEN_MAX_X                 = 639,
    SCREEN_MAX_Y                 = 479,
    ARMY_VIEW_LEVEL_COUNT        = 3,
    COMMAND_FRAME_DELAY          = 75
H2_ENUM_END(CombatCommandGeometry)

H2_ENUM_BEGIN(CombatCasualtyConstant)
    CASUALTY_QUANTITY_STORAGE_COUNT = 42,
    CASUALTY_DISPLAY_LIMIT          = 7,
    CASUALTY_WIDGETS_PER_SIDE       = 7,
    CASUALTY_WINDOW_WIDTH           = 320,
    CASUALTY_WINDOW_BOTTOM          = 458
H2_ENUM_END(CombatCasualtyConstant)

H2_ENUM_BEGIN(CombatArtifactPanelConstant)
    ARTIFACT_BACKGROUND_X    = 136,
    ARTIFACT_BACKGROUND_Y    = 310,
    ARTIFACT_BACKGROUND_SIZE = 80,
    ARTIFACT_ICON_X          = 144,
    ARTIFACT_ICON_Y          = 318,
    ARTIFACT_ICON_SIZE       = 64,
    ARTIFACT_TEXT_X          = 16,
    ARTIFACT_TEXT_Y          = 397,
    ARTIFACT_TEXT_HEIGHT     = 12,
    ARTIFACT_NAME_CAPACITY   = 60
H2_ENUM_END(CombatArtifactPanelConstant)

H2_ENUM_BEGIN(CombatSkeletonPanelConstant)
    SKELETON_ICON_X         = 173,
    SKELETON_ICON_Y         = 270,
    SKELETON_ICON_WIDTH     = 32,
    SKELETON_ICON_HEIGHT    = 30,
    SKELETON_TEXT_X         = 165,
    SKELETON_TEXT_Y         = 300,
    SKELETON_TEXT_WIDTH     = 32,
    SKELETON_TEXT_HEIGHT    = 12,
    SKELETON_COUNT_CAPACITY = 9
H2_ENUM_END(CombatSkeletonPanelConstant)

H2_ENUM_BEGIN(CombatEagleEyePanelConstant)
    EAGLE_PANEL_X             = 105,
    EAGLE_PANEL_Y             = 275,
    EAGLE_BACKGROUND_X_OFFSET = 16,
    EAGLE_ICON_X_OFFSET       = 36,
    EAGLE_ICON_Y_OFFSET       = 5,
    EAGLE_ICON_WIDTH          = 72,
    EAGLE_ICON_HEIGHT         = 51,
    EAGLE_TEXT_X_OFFSET       = 34,
    EAGLE_TEXT_Y_OFFSET       = 57,
    EAGLE_TEXT_WIDTH          = 80,
    EAGLE_TEXT_HEIGHT         = 24,
    EAGLE_SPELL_NAME_CAPACITY = 200
H2_ENUM_END(CombatEagleEyePanelConstant)

H2_ENUM_BEGIN(CombatCasualtyPanelConstant)
    CASUALTY_TITLE_WIDGET         = 17,
    CASUALTY_TITLE_X              = 16,
    CASUALTY_TITLE_Y              = 263,
    CASUALTY_TEXT_HEIGHT          = 20,
    CASUALTY_TEXT_WIDGET_ID       = 2110,
    CASUALTY_SIDE_WIDGET_FIRST    = 15,
    CASUALTY_ATTACKER_Y           = 279,
    CASUALTY_DEFENDER_Y           = 346,
    CASUALTY_SIDE_LABEL_Y_OFFSET  = 3,
    CASUALTY_NONE_Y_OFFSET        = 21,
    CASUALTY_WIDGET_ID_STRIDE     = 5,
    CASUALTY_TEXT_WIDGET_ID_FIRST = 2100,
    CASUALTY_ICON_WIDGET_ID_FIRST = 2000,
    CASUALTY_ICON_SPACING         = 40,
    CASUALTY_CENTER_DIVISOR       = 2,
    CASUALTY_CENTER_X_OFFSET      = 3,
    CASUALTY_ICON_CELL_WIDTH      = 32,
    CASUALTY_ICON_TOP_OFFSET      = 19,
    CASUALTY_ICON_WIDTH           = 32,
    CASUALTY_ICON_HEIGHT          = 28,
    CASUALTY_QUANTITY_X_OFFSET    = 16,
    CASUALTY_QUANTITY_Y_OFFSET    = 53,
    CASUALTY_QUANTITY_WIDTH       = 32,
    CASUALTY_QUANTITY_HEIGHT      = 12,
    CASUALTY_HEADING_CAPACITY     = 30,
    CASUALTY_NONE_CAPACITY        = 10,
    CASUALTY_QUANTITY_CAPACITY    = 9
H2_ENUM_END(CombatCasualtyPanelConstant)

H2_ENUM_BEGIN(CombatCycleConstant)
    HERO_ANIMATION_STAND           = 0,
    HERO_ANIMATION_DEATH_FIRST     = 1,
    HERO_ANIMATION_DEATH_SECOND    = 2,
    HERO_ANIMATION_IDLE_FIRST      = 9,
    HERO_ANIMATION_IDLE_SECOND     = 10,
    HERO_ANIMATION_IDLE_LAST       = 11,
    HERO_IDLE_DELAY                = 4500,
    STAND_DELAY_RANDOM_THRESHOLD   = 51,
    STAND_DELAY_RANDOM_MIN         = 50,
    IDLE_ROLL_MIN                  = 1,
    IDLE_ROLL_MAX                  = 100,
    IDLE_REPEAT_CHANCE             = 8,
    STANDING_ANIMATION_SEARCH_DONE = 99
H2_ENUM_END(CombatCycleConstant)

H2_ENUM_BEGIN(CombatVictoryConstant)
    VICTORY_NECROMANCY_STACK_LIMIT   = 5,
    VICTORY_FADE_STEPS               = 8,
    VICTORY_WASTELAND_FADE_STEPS     = 5,
    VICTORY_FADE_DELAY               = 60,
    VICTORY_MUSIC                    = 29,
    LOSS_MUSIC                       = 30,
    VICTORY_EXPERIENCE_TEXT_SIZE     = 152,
    WIN_LOSE_WINDOW_X                = 143,
    WIN_LOSE_WINDOW_Y                = 10,
    WIN_LOSE_DIALOG_TIMEOUT          = 15000,
    WIN_LOSE_ANIMATION_FILENAME_SIZE = 52,
    VICTORY_DIM_STEP                 = 3
H2_ENUM_END(CombatVictoryConstant)

H2_ENUM_BEGIN(CombatSurrenderDialogConstant)
    SURRENDER_DIALOG_TYPE            = 1,
    SURRENDER_DIALOG_ACCEPT_RESULT   = 2,
    SURRENDER_TEXT_WIDTH             = 30,
    SURRENDER_PORTRAIT_RESOURCE_ID   = 1,
    SURRENDER_PORTRAIT_WIDGET_ID     = 30,
    SURRENDER_PORTRAIT_DEFAULT_COLOR = 4,
    SURRENDER_TEXT_ID                = 2,
    SURRENDER_WINDOW_X               = 74,
    SURRENDER_WINDOW_Y               = 80
H2_ENUM_END(CombatSurrenderDialogConstant)

H2_ENUM_CLASS_BEGIN(CombatHelpTextIndex)
    HELP_SKIP_UNIT      = 1,
    HELP_AUTO_COMBAT    = 2,
    HELP_SYSTEM_OPTIONS = 3,
    HELP_OTHER_CONTROL  = 4
H2_ENUM_CLASS_END(CombatHelpTextIndex)

H2_ENUM_CLASS_BEGIN(CombatLongHelpIndex)
    LONG_HELP_ATTACK            = 0,
    LONG_HELP_WAIT              = 1,
    LONG_HELP_DISABLE_SELECTION = 2,
    LONG_HELP_SYSTEM_OPTIONS    = 3,
    LONG_HELP_CONTROLS          = 4
H2_ENUM_CLASS_END(CombatLongHelpIndex)

H2_ENUM_BEGIN(CombatStorageConstant)
    SELECTOR_INITIAL_FRAME = 7,
    VIEW_DESCRIPTION_SIZE  = 300
H2_ENUM_END(CombatStorageConstant)

}

#define RETAIL_FILE "I:\\Projects\\Heroes\\Prog\\SOURCE\\COMMAND.CPP"
VA(0x0042a6d0, 0x36d)
MessageDispatchResult combatManager::Main(tag_message& message) {
    MessageDispatchResult result = MESSAGE_DISPATCH_CONSUME;

    if (gbNoShowCombat == 0) {
        if (KBTickCount() > glTimers[0]) {
            PollSound();
            glTimers[0] = static_cast<i32>(
                KBTickCount() + gfCombatSpeedMod[gConfig.combatSpeed] * COMBAT_SOUND_POLL_DELAY
            );
        }
        if (KBTickCount() > glTimers[GLOBAL_COMBAT_CYCLE_TIMER_SLOT]
            && gbProcessingCombatAction == 0) {
            gbProcessingCombatAction = true;
            CycleCombatScreen();
            gbProcessingCombatAction = false;
        }
    }

    CheckCastleAttack();
    if (CheckWin(&message) != 0)
        return MESSAGE_DISPATCH_FORWARD;

    if (gbNoShowCombat == 0) {
        CombatRemotePacket* packet =
            reinterpret_cast<CombatRemotePacket*>(GetRemoteData(REMOTE_PACKET_TYPE));
        if (packet != NULL && packet->type == REMOTE_MESSAGE_RELIABLE) {
            switch (packet->command) {
                case REMOTE_COMMAND_ACTION:
                    giNextAction = packet->nextAction;
                    giNextActionExtra = packet->nextActionExtra;
                    giNextActionGridIndex = packet->nextActionGridIndex;
                    giNextActionGridIndex2 = packet->nextActionGridIndex2;
                    goto ProcessAction;
                case REMOTE_COMMAND_MESSAGE:
                    PopNetBox(reinterpret_cast<char*>(&packet->nextAction), packet->messageLength);
                    break;
            }
        }

        if (gbThisNetHasControl == 0) {
            if (message.type == MESSAGE_KEY_DOWN) {
                switch (static_cast<CombatKeyCommand>(message.payload.keyboard.keyCode)) {
                    case KEY_CLOSE_NETWORK_BOX:
                        PopNetBox(NULL, -1);
                        break;
                }
            }
            return MESSAGE_DISPATCH_CONSUME;
        }
    }

    {
        army* currentArmy = &m_armies[IDX(m_currentArmySide)][m_currentArmyIndex];
        if (currentArmy->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_BERSERK)] != 0) {
            currentArmy->GoBerserk();
            if (CheckWin(&message) != 0)
                return MESSAGE_DISPATCH_FORWARD;
        }
    }

    if (gbNoShowCombat == 0) {
        if (m_gridSelectionDisabled != 0) {
            while (message.type != MESSAGE_KEY_DOWN && message.type != MESSAGE_LEFT_BUTTON_DOWN
                   && message.type != MESSAGE_RIGHT_BUTTON_DOWN && message.type != MESSAGE_NONE) {
                message = gpInputManager->GetEvent();
            }
            if (message.type != MESSAGE_NONE)
                m_gridSelectionDisabled = 0;
        }
        CheckChangeSelector();
    }

ProcessAction:
    if (giNextAction == ACTION_NONE) {
        if (m_playerId[IDX(m_currentSide)] == -1 || gbThisNetHumanPlayer[m_playerId[IDX(m_currentSide)]] == 0
            || m_gridSelectionDisabled != 0) {
            CheckGetAIMove();
        } else {
            result = ProcessCombatMsg(message);
        }
    }
    if (giNextAction != ACTION_NONE)
        result = ProcessNextAction(message);
    return result;
}

VA(0x0042aa3d, 0x181)
i32 combatManager::ValidHexToStandOn(i32 hexIndex) {
    if (hexIndex == IGNORED_HEX)
        return 1;

    if (!(hexIndex == INVALID_HEX || MAP_WIDTH - 1 == hexIndex % COMBAT_GRID_ROW_LENGTH
          || hexIndex % COMBAT_GRID_ROW_LENGTH == 0
          || (m_hexCells[hexIndex].m_blocked != 0
              && (gpCombatManager->m_inCastleCombat == 0
                  || (hexIndex != COMBAT_CASTLE_GATE_APPROACH_HEX
                      && hexIndex != CASTLE_GATE_HEX)
                  || (gpCombatManager->m_drawbridgeState == COMBAT_CASTLE_GATE_OPEN
                      && (gpCombatManager->m_currentSide != COMBAT_DEFENDER_SIDE
                          || gpCombatManager->m_hexCells[COMBAT_CASTLE_GATE_APPROACH_HEX]
                                     .m_occupantSide
                                 != COMBAT_SIDE_NONE
                          || gpCombatManager->m_hexCells[COMBAT_CASTLE_GATE_APPROACH_HEX]
                                     .m_deadOccupantCount
                                 != 0))))
          || (m_hexCells[hexIndex].m_occupantSide != COMBAT_SIDE_NONE
              && (m_hexCells[hexIndex].m_occupantSide != m_currentArmySide
                  || m_hexCells[hexIndex].m_occupantIndex != m_currentArmyIndex)))) {
        return 1;
    } else {
        return 0;
    }
}

VA(0x0042abbe, 0x8a1)
void combatManager::SetCombatDirections(i32 targetHex) {
    if (m_gridSelectionDisabled != 0)
        return;

    char standable_3[COMBAT_DIRECTION_COUNT];
    i32 directionHexes_4[COMBAT_DIRECTION_COUNT];
    i32 rearHexes_15[COMBAT_DIRECTION_COUNT];
    char pathValid_1[COMBAT_DIRECTION_COUNT];
    army* currentArmy_3 = &m_armies[IDX(m_currentArmySide)][m_currentArmyIndex];
    CombatSide targetSide_36 = currentArmy_3->m_targetSide;
    i32 targetIndex_6 = currentArmy_3->m_targetIndex;
    currentArmy_3->m_targetSide = COMBAT_SIDE_NONE;
    currentArmy_3->m_targetIndex = -1;
    army* targetArmy_1 = &m_armies[IDX(targetSide_36)][targetIndex_6];

    i32 direction_27;
    for (direction_27 = 0; direction_27 < COMBAT_DIRECTION_COUNT; direction_27++) {
        if (direction_27 == IDX(COMBAT_DIRECTION_WIDE_WEST)
            || direction_27 == IDX(COMBAT_DIRECTION_WIDE_EAST)) {
            if (HAS(currentArmy_3->m_monster.flags.all, MONSTER_FLAGS_WIDE) != 0) {
                if (currentArmy_3->m_facing == ARMY_FACING_RIGHT) {
                    if (direction_27 == IDX(COMBAT_DIRECTION_WIDE_WEST))
                        directionHexes_4[direction_27] = m_adjacency[targetHex][IDX(COMBAT_DIRECTION_NORTHWEST)];
                    if (direction_27 == IDX(COMBAT_DIRECTION_WIDE_EAST))
                        directionHexes_4[direction_27] = m_adjacency[targetHex][IDX(COMBAT_DIRECTION_SOUTHWEST)];
                } else {
                    if (direction_27 == IDX(COMBAT_DIRECTION_WIDE_WEST))
                        directionHexes_4[direction_27] = m_adjacency[targetHex][IDX(COMBAT_DIRECTION_NORTHEAST)];
                    if (direction_27 == IDX(COMBAT_DIRECTION_WIDE_EAST))
                        directionHexes_4[direction_27] = m_adjacency[targetHex][IDX(COMBAT_DIRECTION_SOUTHEAST)];
                }
            } else {
                directionHexes_4[direction_27] = INVALID_HEX;
            }
        } else {
            directionHexes_4[direction_27] =
                *(&m_adjacency[0][0] + targetHex * COMBAT_DIRECTION_ADJACENT_COUNT + direction_27);
        }

        if (HAS(currentArmy_3->m_monster.flags.all, MONSTER_FLAGS_WIDE) != 0
            && directionHexes_4[direction_27] != INVALID_HEX) {
            if (currentArmy_3->m_facing == ARMY_FACING_RIGHT) {
                if (direction_27 == IDX(COMBAT_DIRECTION_NORTHWEST)
                    || direction_27 == IDX(COMBAT_DIRECTION_WEST)
                    || direction_27 == IDX(COMBAT_DIRECTION_SOUTHWEST)) {
                    if (directionHexes_4[direction_27] % COMBAT_GRID_ROW_LENGTH
                        == COMBAT_GRID_FIRST_COLUMN)
                        directionHexes_4[direction_27] = INVALID_HEX;
                    else
                        directionHexes_4[direction_27]--;
                }
                if (directionHexes_4[direction_27] % COMBAT_GRID_ROW_LENGTH
                    == COMBAT_GRID_REVERSE_FIRST_COLUMN)
                    rearHexes_15[direction_27] = INVALID_HEX;
                else
                    rearHexes_15[direction_27] = directionHexes_4[direction_27] + 1;
            } else {
                if (direction_27 == IDX(COMBAT_DIRECTION_NORTHEAST)
                    || direction_27 == IDX(COMBAT_DIRECTION_EAST)
                    || direction_27 == IDX(COMBAT_DIRECTION_SOUTHEAST)) {
                    if (directionHexes_4[direction_27] % COMBAT_GRID_ROW_LENGTH
                        == COMBAT_GRID_REVERSE_FIRST_COLUMN)
                        directionHexes_4[direction_27] = INVALID_HEX;
                    else
                        directionHexes_4[direction_27]++;
                }
                if (directionHexes_4[direction_27] % COMBAT_GRID_ROW_LENGTH
                    == COMBAT_GRID_FIRST_COLUMN)
                    rearHexes_15[direction_27] = INVALID_HEX;
                else
                    rearHexes_15[direction_27] = directionHexes_4[direction_27] - 1;
            }
        } else {
            rearHexes_15[direction_27] = IGNORED_HEX;
        }

        if (ValidHexToStandOn(directionHexes_4[direction_27]) != 0
            && ValidHexToStandOn(rearHexes_15[direction_27]) != 0)
            standable_3[direction_27] = 1;
        else
            standable_3[direction_27] = 0;
    }

    if (HAS(currentArmy_3->m_monster.flags.all, MONSTER_FLAGS_FLYING) != 0) {
        for (direction_27 = 0; direction_27 < COMBAT_DIRECTION_COUNT; direction_27++)
            pathValid_1[direction_27] = standable_3[direction_27];
    } else {
        for (direction_27 = 0; direction_27 < COMBAT_DIRECTION_COUNT; direction_27++) {
            if (standable_3[direction_27] != 0) {
                if (directionHexes_4[direction_27] == currentArmy_3->m_hex
                    || currentArmy_3->ValidPath(
                           directionHexes_4[direction_27], ARMY_PATH_EXACT_TARGET_HEX
                       )
                        != 0)
                    pathValid_1[direction_27] = 1;
                else
                    pathValid_1[direction_27] = 0;
            } else {
                pathValid_1[direction_27] = 0;
            }
        }
    }

    m_validDirectionCount = 0;
    for (direction_27 = 0; direction_27 < COMBAT_DIRECTION_COUNT; direction_27++) {
        if (pathValid_1[direction_27] != 0)
            m_validDirectionCount++;
    }
    if (m_validDirectionCount == 0)
        pathValid_1[IDX(COMBAT_DIRECTION_WIDE_WEST)] = 1;

    memset(m_directionMap, -1, sizeof(m_directionMap));
    for (direction_27 = 0; direction_27 < COMBAT_DIRECTION_COUNT; direction_27++) {
        i32 outputDirection_11 = direction_27;
        i32 mappedDirection_7;
        if (direction_27 < COMBAT_DIRECTION_ADJACENT_COUNT)
            mappedDirection_7 = (direction_27 + COMBAT_DIRECTION_OPPOSITE_OFFSET) % COMBAT_DIRECTION_ADJACENT_COUNT;
        else if (direction_27 == IDX(COMBAT_DIRECTION_WIDE_WEST))
            mappedDirection_7 = IDX(COMBAT_DIRECTION_WIDE_EAST);
        else
            mappedDirection_7 = IDX(COMBAT_DIRECTION_WIDE_WEST);

        if (pathValid_1[mappedDirection_7] != 0) {
            if (HAS(targetArmy_1->m_monster.flags.all, MONSTER_FLAGS_WIDE) != 0) {
                if (direction_27 == IDX(COMBAT_DIRECTION_NORTHEAST)
                    && m_hexCells[targetHex - 1].m_occupantSide == targetSide_36
                    && m_hexCells[targetHex - 1].m_occupantIndex == targetIndex_6) {
                    outputDirection_11 = IDX(COMBAT_DIRECTION_WIDE_WEST);
                } else if (direction_27 == IDX(COMBAT_DIRECTION_NORTHWEST)
                           && m_hexCells[targetHex + 1].m_occupantSide == targetSide_36
                           && m_hexCells[targetHex + 1].m_occupantIndex == targetIndex_6) {
                    outputDirection_11 = IDX(COMBAT_DIRECTION_WIDE_WEST);
                } else if (direction_27 == IDX(COMBAT_DIRECTION_SOUTHEAST)
                           && m_hexCells[targetHex - 1].m_occupantSide == targetSide_36
                           && m_hexCells[targetHex - 1].m_occupantIndex == targetIndex_6) {
                    outputDirection_11 = IDX(COMBAT_DIRECTION_WIDE_EAST);
                } else if (direction_27 == IDX(COMBAT_DIRECTION_SOUTHWEST)
                           && m_hexCells[targetHex + 1].m_occupantSide == targetSide_36
                           && m_hexCells[targetHex + 1].m_occupantIndex == targetIndex_6) {
                    outputDirection_11 = IDX(COMBAT_DIRECTION_WIDE_EAST);
                }
            }

            if (direction_27 < COMBAT_DIRECTION_ADJACENT_COUNT) {
                memset(
                    &m_directionMap[mappedDirection_7 * DIRECTION_SECTORS_PER_ADJACENT],
                    outputDirection_11,
                    DIRECTION_SECTORS_PER_ADJACENT
                );
            } else if (direction_27 == IDX(COMBAT_DIRECTION_WIDE_WEST)) {
                m_directionMap[DIRECTION_SPECIAL_FIRST_SECTOR_START] =
                    static_cast<i8>(outputDirection_11);
                m_directionMap[DIRECTION_SPECIAL_FIRST_SECTOR_CENTER] =
                    static_cast<i8>(outputDirection_11);
                m_directionMap[DIRECTION_SPECIAL_FIRST_SECTOR_END] =
                    static_cast<i8>(outputDirection_11);
            } else {
                m_directionMap[DIRECTION_SPECIAL_SECOND_SECTOR_CENTER] =
                    static_cast<i8>(outputDirection_11);
                m_directionMap[DIRECTION_SPECIAL_SECOND_SECTOR_NEXT] =
                    static_cast<i8>(outputDirection_11);
                m_directionMap[DIRECTION_SPECIAL_SECOND_SECTOR_PREVIOUS] =
                    static_cast<i8>(outputDirection_11);
            }
        }
    }

    i32 unresolved_7 = DIRECTION_SECTOR_COUNT;
    while (unresolved_7 > 0) {
        for (direction_27 = 0; direction_27 < DIRECTION_SECTOR_COUNT; direction_27++) {
            if (m_directionMap[direction_27] == -1) {
                i32 next_3 = (direction_27 + 1) % DIRECTION_SECTOR_COUNT;
                i32 previous_1 = (direction_27 + DIRECTION_SECTOR_COUNT - 1)
                                 % DIRECTION_SECTOR_COUNT;
                if (m_directionMap[next_3] >= 0
                    && m_directionMap[next_3] <= IDX(COMBAT_DIRECTION_WIDE_EAST)) {
                    m_directionMap[direction_27] =
                        m_directionMap[next_3] + DIRECTION_PENDING_OFFSET;
                } else if (m_directionMap[previous_1] >= 0
                           && m_directionMap[previous_1] <= IDX(COMBAT_DIRECTION_WIDE_EAST)) {
                    m_directionMap[direction_27] =
                        m_directionMap[previous_1] + DIRECTION_PENDING_OFFSET;
                }
            }
        }
        unresolved_7 = 0;
        for (direction_27 = 0; direction_27 < DIRECTION_SECTOR_COUNT; direction_27++) {
            if (m_directionMap[direction_27] >= DIRECTION_PENDING_OFFSET) {
                m_directionMap[direction_27] -= DIRECTION_PENDING_OFFSET;
            } else if (m_directionMap[direction_27] == INVALID_HEX) {
                unresolved_7++;
            }
        }
    }

    currentArmy_3->m_targetSide = targetSide_36;
    currentArmy_3->m_targetIndex = targetIndex_6;
}

VA(0x0042b45f, 0x63c)
void combatManager::CheckSetMouseDirection(i32 mouseX, i32 mouseY, i32 targetHex) {
    if (m_gridSelectionDisabled != 0)
        return;
    if (m_validDirectionCount <= 1 && m_mouseDirection >= 0)
        return;

    i32 relativeX1 = mouseX - (targetHex % COMBAT_GRID_ROW_LENGTH - 1) * COMBAT_HEX_HORIZONTAL_STEP;
    relativeX1 -= COMBAT_HEX_GRID_LEFT_ORIGIN;
    if (((targetHex / COMBAT_GRID_ROW_LENGTH) & 1) == 0)
        relativeX1 -= COMBAT_HEX_ROW_STAGGER;
    i32 relativeY10 = mouseY - COMBAT_HEX_CENTER_Y_ORIGIN
                      - targetHex / COMBAT_GRID_ROW_LENGTH * COMBAT_HEX_VERTICAL_STEP;
    relativeY10 -= DIRECTION_MOUSE_TOP_OFFSET;
    relativeX1 -= COMBAT_HEX_ROW_STAGGER;
    relativeY10 -= DIRECTION_MOUSE_CENTER_OFFSET;

    i32 sector6 = 0;
    if (relativeX1 < 0) {
        if (relativeY10 < 0)
            sector6 += DIRECTION_SECTOR_THREE_QUARTERS;
        else
            sector6 += DIRECTION_SECTOR_HALF;
    } else {
        if (relativeY10 < 0) {
        } else {
            sector6 += DIRECTION_SECTOR_QUARTER;
        }
    }

    relativeX1 = abs(relativeX1);
    relativeY10 = abs(relativeY10);
    float ratio7 = static_cast<float>(relativeX1) / relativeY10;
    if (sector6 == 0 || sector6 == DIRECTION_SECTOR_HALF) {
        if (ratio7 > COMBAT_DIRECTION_SLOPE_STEEPEST)
            sector6 += DIRECTION_SECTOR_OFFSET_STEEPEST;
        else if (ratio7 > COMBAT_DIRECTION_SLOPE_STEEP)
            sector6 += DIRECTION_SECTOR_OFFSET_STEEP;
        else if (ratio7 > COMBAT_DIRECTION_SLOPE_DIAGONAL)
            sector6 += DIRECTION_SECTOR_OFFSET_DIAGONAL;
        else if (ratio7 > COMBAT_DIRECTION_SLOPE_SHALLOW)
            sector6 += DIRECTION_SECTOR_OFFSET_SHALLOW;
        else if (ratio7 > COMBAT_DIRECTION_SLOPE_SHALLOWEST)
            sector6++;
    } else {
        if (ratio7 < COMBAT_DIRECTION_SLOPE_SHALLOWEST)
            sector6 += DIRECTION_SECTOR_OFFSET_STEEPEST;
        else if (ratio7 < COMBAT_DIRECTION_SLOPE_SHALLOW)
            sector6 += DIRECTION_SECTOR_OFFSET_STEEP;
        else if (ratio7 < COMBAT_DIRECTION_SLOPE_DIAGONAL)
            sector6 += DIRECTION_SECTOR_OFFSET_DIAGONAL;
        else if (ratio7 < COMBAT_DIRECTION_SLOPE_STEEP)
            sector6 += DIRECTION_SECTOR_OFFSET_SHALLOW;
        else if (ratio7 < COMBAT_DIRECTION_SLOPE_STEEPEST)
            sector6++;
    }

    if (m_directionMap[sector6] == m_mouseDirection)
        return;

    m_mouseDirection = m_directionMap[sector6];
    CombatHexDirection direction =
        OppositeDirection(static_cast<CombatHexDirection>(m_directionMap[sector6]));
    CombatHexDirection directionCopy = direction;
    CombatHexDirection alternateDirection = COMBAT_DIRECTION_INVALID;
    army* currentArmy = &m_armies[IDX(m_currentArmySide)][m_currentArmyIndex];
    army* targetArmy1 = &m_armies[IDX(currentArmy->m_targetSide)][currentArmy->m_targetIndex];

    if (direction == COMBAT_DIRECTION_WIDE_WEST || direction == COMBAT_DIRECTION_WIDE_EAST) {
        if (HAS(currentArmy->m_monster.flags.all, MONSTER_FLAGS_WIDE) != 0) {
            if (currentArmy->m_facing == ARMY_FACING_RIGHT
                && direction == COMBAT_DIRECTION_WIDE_WEST) {
                direction = COMBAT_DIRECTION_NORTHWEST;
                alternateDirection = COMBAT_DIRECTION_NORTHEAST;
            } else if (currentArmy->m_facing == ARMY_FACING_RIGHT
                       && direction == COMBAT_DIRECTION_WIDE_EAST) {
                direction = COMBAT_DIRECTION_SOUTHWEST;
                alternateDirection = COMBAT_DIRECTION_SOUTHEAST;
            } else if (currentArmy->m_facing == ARMY_FACING_LEFT
                       && direction == COMBAT_DIRECTION_WIDE_WEST) {
                direction = COMBAT_DIRECTION_NORTHEAST;
                alternateDirection = COMBAT_DIRECTION_NORTHWEST;
            } else {
                direction = COMBAT_DIRECTION_SOUTHEAST;
                alternateDirection = COMBAT_DIRECTION_SOUTHWEST;
            }
        } else {
            if (m_hexCells[targetHex - 1].m_occupantSide == currentArmy->m_targetSide
                && m_hexCells[targetHex - 1].m_occupantIndex == currentArmy->m_targetIndex) {
                targetHex--;
            }
            if (direction == COMBAT_DIRECTION_WIDE_WEST)
                direction = COMBAT_DIRECTION_NORTHEAST;
            else
                direction = COMBAT_DIRECTION_SOUTHEAST;
        }
    } else {
        if (currentArmy->m_facing == ARMY_FACING_RIGHT
            && HAS(currentArmy->m_monster.flags.all, MONSTER_FLAGS_WIDE) != 0) {
            if (direction == COMBAT_DIRECTION_NORTHWEST || direction == COMBAT_DIRECTION_WEST
                || direction == COMBAT_DIRECTION_SOUTHWEST)
                targetHex--;
        } else if (currentArmy->m_facing == ARMY_FACING_LEFT
                   && HAS(currentArmy->m_monster.flags.all, MONSTER_FLAGS_WIDE) != 0
                   && (direction == COMBAT_DIRECTION_NORTHEAST || direction == COMBAT_DIRECTION_EAST
                       || direction == COMBAT_DIRECTION_SOUTHEAST)) {
            targetHex++;
        }
    }

    m_directionTargetHex = m_adjacency[targetHex][IDX(direction)];
    i32 rearHex3 = IGNORED_HEX;
    if (currentArmy->m_facing == ARMY_FACING_LEFT
        && HAS(currentArmy->m_monster.flags.all, MONSTER_FLAGS_WIDE) != 0) {
        rearHex3 = m_directionTargetHex - 1;
    }
    if (currentArmy->m_facing == ARMY_FACING_RIGHT
        && HAS(currentArmy->m_monster.flags.all, MONSTER_FLAGS_WIDE) != 0) {
        rearHex3 = m_directionTargetHex + 1;
    }
    if (ValidHexToStandOn(m_directionTargetHex) == 0 || ValidHexToStandOn(rearHex3) == 0) {
        if (HAS(currentArmy->m_monster.flags.all, MONSTER_FLAGS_WIDE) != 0
            && (directionCopy == COMBAT_DIRECTION_WIDE_WEST
                || directionCopy == COMBAT_DIRECTION_WIDE_EAST)) {
            if (currentArmy->m_facing == ARMY_FACING_RIGHT)
                m_directionTargetHex++;
            else
                m_directionTargetHex--;
        } else {
            if (alternateDirection != COMBAT_DIRECTION_INVALID)
                m_directionTargetHex = m_adjacency[targetHex][IDX(alternateDirection)];
        }
    }
    gpMouseManager->SetPointer(m_mouseDirection + POINTER_ATTACK_OFFSET);
}

VA(0x0042ba9b, 0x8b)
i32 combatManager::GetPointer(CombatMessageCommand command, i32 hexIndex) {
    i32 result;
    if (command == COMBAT_MESSAGE_COMMAND_OPPOSING_OPTIONS) {
        return POINTER_VIEW;
    } else {
        switch (command) {
            case COMBAT_MESSAGE_COMMAND_MOVE:
            case COMBAT_MESSAGE_COMMAND_FLY:
            case COMBAT_MESSAGE_COMMAND_SHOOT:
            case COMBAT_MESSAGE_COMMAND_VIEW_INFO: {
                i32 x = hexIndex % COMBAT_GRID_ROW_LENGTH;
                i32 y = hexIndex / COMBAT_GRID_ROW_LENGTH;
                return IDX(command);
            }
            default:
                return IDX(command);
        }
    }
}

VA(0x0042bb26, 0x8e4)
MessageDispatchResult combatManager::ProcessCombatMsg(tag_message& message) {
    i32 mouseX = message.payload.mouse.screenX;
    i32 mouseY = message.payload.mouse.screenY;
    i32 unusedResult_14 = 0;
    i32 selectedHex_36;
    tag_message pendingMessage;

    switch (message.type) {
        case MESSAGE_WIDGET:
            if (HAS(
                    message.payload.widget.modifiers,
                    MESSAGE_MODIFIER_RIGHT_BUTTON
                )) {
                if (message.payload.widget.command == WIDGET_COMMAND_SELECT
                    || message.payload.widget.command == WIDGET_COMMAND_ALTERNATE_SELECT) {
                    i32 helpIndex = -1;
                    switch (static_cast<CombatControlId>(message.payload.widget.id)) {
                        case CONTROL_ATTACK:
                            helpIndex = IDX(LONG_HELP_ATTACK);
                            break;
                        case CONTROL_WAIT:
                            helpIndex = IDX(LONG_HELP_WAIT);
                            break;
                        case CONTROL_DISABLE_SELECTION:
                            helpIndex = IDX(LONG_HELP_DISABLE_SELECTION);
                            break;
                        case CONTROL_SYSTEM_OPTIONS:
                            helpIndex = IDX(LONG_HELP_SYSTEM_OPTIONS);
                            break;
                        case CONTROL_HELP_FIRST:
                        case CONTROL_HELP_SECOND:
                        case CONTROL_HELP_THIRD:
                        case CONTROL_HELP_FOURTH:
                            helpIndex = IDX(LONG_HELP_CONTROLS);
                            break;
                        case CONTROL_MAIN_BUTTON:
                            RightClick(m_selectedHex);
                            break;
                    }
                    if (helpIndex != -1) {
                        NormalDialog(
                            cLongCombatHelp[helpIndex],
                            NORMAL_DIALOG_QUICK_VIEW,
                            NORMAL_DIALOG_NO_RESOURCE,
                            NORMAL_DIALOG_NO_VALUE,
                            NORMAL_DIALOG_NO_RESOURCE,
                            0,
                            NORMAL_DIALOG_NO_RESOURCE,
                            0,
                            NORMAL_DIALOG_NO_RESOURCE,
                            0
                        );
                    }
                }
            } else {
                switch (message.payload.widget.command) {
                    case WIDGET_COMMAND_SELECT:
                        switch (static_cast<CombatControlId>(message.payload.widget.id)) {
                            case CONTROL_MAIN_BUTTON:
                                DoCommand(m_currentCommand);
                                break;
                        }
                        break;
                    case WIDGET_COMMAND_DESELECT:
                        switch (static_cast<CombatControlId>(message.payload.widget.id)) {
                            case CONTROL_ATTACK:
                                giNextAction = ACTION_DEFEND;
                                break;
                            case CONTROL_WAIT:
                                giNextAction = ACTION_WAIT;
                                break;
                            case CONTROL_DISABLE_SELECTION:
                                m_gridSelectionDisabled = 1;
                                break;
                            case CONTROL_SYSTEM_OPTIONS:
                                CombatSystemOptions();
                                break;
                        }
                        break;
                }
            }
            break;

        case MESSAGE_MOUSE_MOVE:
            if (m_gridSelectionDisabled != 0)
                break;
            pendingMessage = gpInputManager->PeekEvent();
            if (pendingMessage.type == MESSAGE_MOUSE_MOVE)
                break;
            if (InCombatArea(message.payload.mouse.screenX, message.payload.mouse.screenY)
                != 0)
                selectedHex_36 = GetGridIndex(mouseX, mouseY);
            else
                selectedHex_36 = INVALID_HEX;

            UpdateMouseGrid(selectedHex_36, 0);
            if (InCombatArea(message.payload.mouse.screenX, message.payload.mouse.screenY)
                != 0) {
                if (m_selectedHex != selectedHex_36
                    || selectedHex_36 == INVALID_HEX) {
                    m_selectedHex = selectedHex_36;
                    m_previousCommand = COMBAT_INVALID_COMMAND;
                    m_currentCommand = GetCommand(m_selectedHex);
                    m_mouseDirection = INVALID_HEX;
                    if (m_currentCommand == COMBAT_MESSAGE_COMMAND_ATTACK) {
                        SetCombatDirections(selectedHex_36);
                        CheckSetMouseDirection(mouseX, mouseY, selectedHex_36);
                    } else {
                        gpMouseManager->SetPointer(
                            GetPointer(m_currentCommand, selectedHex_36)
                        );
                    }
                } else if (m_currentCommand == COMBAT_MESSAGE_COMMAND_ATTACK) {
                    CheckSetMouseDirection(mouseX, mouseY, selectedHex_36);
                }
                if (m_previousCommand != m_currentCommand) {
                    m_previousCommand = m_currentCommand;
                    CombatMessage(m_currentCommand);
                }
            } else {
                if (mouseX >= CONTROL_RIGHT_MIN_X) {
                    CombatMessage(cCombatHelp[IDX(HELP_SKIP_UNIT)], 1, 0, 0);
                } else if (mouseX <= CONTROL_LEFT_MAX_X
                           && mouseY < CONTROL_SYSTEM_OPTIONS_MIN_Y) {
                    CombatMessage(cCombatHelp[IDX(HELP_AUTO_COMBAT)], 1, 0, 0);
                } else if (mouseX <= CONTROL_LEFT_MAX_X) {
                    CombatMessage(cCombatHelp[IDX(HELP_SYSTEM_OPTIONS)], 1, 0, 0);
                } else {
                    CombatMessage(cCombatHelp[IDX(HELP_OTHER_CONTROL)], 1, 0, 0);
                }
                gpMouseManager->SetPointer(COMBAT_POINTER_DEFAULT);
                m_selectedHex = INVALID_HEX;
                m_previousCommand = COMBAT_INVALID_COMMAND;
            }
            return MESSAGE_DISPATCH_CONSUME;

        case MESSAGE_KEY_DOWN:
            switch (static_cast<CombatKeyCommand>(message.payload.keyboard.keyCode)) {
                case KEY_CLOSE_NETWORK_BOX:
                    PopNetBox(NULL, -1);
                    break;
                case KEY_REDRAW_SCREEN:
                    gpWindowManager->UpdateScreenRegion(
                        0,
                        0,
                        COMBAT_SCREEN_WIDTH - 1,
                        COMBAT_SCREEN_HEIGHT - 1
                    );
                    break;
                case KEY_CYCLE_ARMY_VIEW:
                    SetCombatViewArmySmallLevel(
                        (gConfig.combatArmyInfoLevel + 1) % ARMY_VIEW_LEVEL_COUNT
                    );
                    break;
                case KEY_TOGGLE_GRID:
                    SetCombatGrid(
                        1 - gConfig.showCombatGrid,
                        gConfig.showCombatMouseHex,
                        gConfig.combatShadeLevel
                    );
                    break;
                case KEY_TOGGLE_MOUSE_HEX:
                    SetCombatGrid(
                        gConfig.showCombatGrid,
                        1 - gConfig.showCombatMouseHex,
                        gConfig.combatShadeLevel
                    );
                    break;
                case KEY_CYCLE_SHADE:
                    SetCombatGrid(
                        gConfig.showCombatGrid,
                        gConfig.showCombatMouseHex,
                        1 - gConfig.combatShadeLevel
                    );
                    break;
                case KEY_CYCLE_SPELL_EFFECT:
                    giSpellEffectShowType = NextSpellEffectDisplayType(giSpellEffectShowType);
                    DrawFrame(1, 0, 0, 0, COMMAND_FRAME_DELAY, 1, 1);
                    break;
                case KEY_DEBUG_CREATURE_EFFECT:
                    if (HAS(message.payload.keyboard.modifiers, MESSAGE_MODIFIER_SHIFT_KEYS)) {
                        VaporizeCreature(COMBAT_DEFENDER_SIDE, 1);
                    } else if (HAS(
                                   message.payload.keyboard.modifiers,
                                   MESSAGE_MODIFIER_CONTROL_KEYS
                               )) {
                        RippleCreature(COMBAT_DEFENDER_SIDE, 1, COMBAT_RIPPLE_DEATH_RIPPLE);
                        RippleCreature(COMBAT_DEFENDER_SIDE, 1, COMBAT_RIPPLE_DEATH_WAVE);
                    } else {
                        RippleCreature(COMBAT_DEFENDER_SIDE, 1, COMBAT_RIPPLE_WAVE);
                    }
                    break;
                case KEY_WAIT:
                    giNextAction = ACTION_WAIT;
                    break;
                case KEY_MOUSE_COORDS: {
                    i32 currentMouseX_18;
                    i32 currentMouseY_18;
                    gpMouseManager->MouseCoords(currentMouseX_18, currentMouseY_18);
                    break;
                }
                case KEY_VIEW_GENERAL:
                    if (m_heroes[IDX(m_currentSide)] != NULL) {
                        gpMouseManager->SetPointer(COMBAT_POINTER_DEFAULT);
                        ViewGeneral(m_currentSide, 1, 0);
                        ResetMouse();
                    }
                    break;
                case KEY_VIEW_ARMY:
                    gpMouseManager->SetPointer(COMBAT_POINTER_DEFAULT);
                    ViewArmy(&m_armies[IDX(m_currentArmySide)][m_currentArmyIndex], 0);
                    ResetMouse();
                    break;
                case KEY_CAST_SPELL:
                    if (m_heroes[IDX(m_currentSide)] == NULL) {
                        NormalDialog(
                            DATA_COMPGEN(0x004f0860, processCombatMsgYouHaveNoHeroToCast, "You have no hero to cast a spell."),
                            NORMAL_DIALOG_INFO,
                            NORMAL_DIALOG_NO_RESOURCE,
                            NORMAL_DIALOG_NO_VALUE,
                            NORMAL_DIALOG_NO_RESOURCE,
                            0,
                            NORMAL_DIALOG_NO_RESOURCE,
                            0,
                            NORMAL_DIALOG_NO_RESOURCE,
                            0
                        );
                    } else if (IsNegationSphereInEffect() != 0) {
                        NormalDialog(
                            DATA_COMPGEN(0x004f0884, processCombatMsgTheSphereOfNegationArtifactIs, "The Sphere of Negation artifact is in effect for this battle, "
                            "disabling all combat spells."),
                            NORMAL_DIALOG_INFO,
                            NORMAL_DIALOG_NO_RESOURCE,
                            NORMAL_DIALOG_NO_VALUE,
                            NORMAL_DIALOG_NO_RESOURCE,
                            0,
                            NORMAL_DIALOG_NO_RESOURCE,
                            0,
                            NORMAL_DIALOG_NO_RESOURCE,
                            0
                        );
                    } else if (m_heroCastSpell[IDX(m_currentSide)] != 0 && giDebugLevel == 0) {
                        NormalDialog(
                            DATA_COMPGEN(0x004f08e0, processCombatMsgYouHaveAlreadyCastASpell, "You have already cast a spell this round."),
                            NORMAL_DIALOG_INFO,
                            NORMAL_DIALOG_NO_RESOURCE,
                            NORMAL_DIALOG_NO_VALUE,
                            NORMAL_DIALOG_NO_RESOURCE,
                            0,
                            NORMAL_DIALOG_NO_RESOURCE,
                            0,
                            NORMAL_DIALOG_NO_RESOURCE,
                            0
                        );
                    } else {
                        gpMouseManager->SetPointer(COMBAT_POINTER_DEFAULT);
                        giCurGeneral = m_currentSide;
                        ViewSpells(0);
                        ResetMouse();
                    }
                    break;
            }
            break;
    }

    return MESSAGE_DISPATCH_CONSUME;
}

VA(0x0042c40a, 0x70)
i32 combatManager::IsNegationSphereInEffect(void) {
    CombatSide side;
    for (side = COMBAT_ATTACKER_SIDE; IDX(side) < COMBAT_SIDE_COUNT; side++) {
        if (m_heroes[IDX(side)] != NULL && m_heroes[IDX(side)]->HasArtifact(ARTIFACT_SPHERE_NEGATION) != 0)
            return 1;
    }
    return 0;
}

VA(0x0042c47a, 0x205)
void combatManager::ResetRound(void) {
    i32 unusedResetRoundWord6;

    m_heroDeathAnimationPlayed[0] = m_heroDeathAnimationPlayed[1] = 0;
    m_heroAlternateDeathAnimationPlayed[0] = m_heroAlternateDeathAnimationPlayed[1] = 0;
    m_heroDeathPending[0] = m_heroDeathPending[1] = 0;
    m_heroAlternateDeathPending[0] = m_heroAlternateDeathPending[1] = 0;
    m_catapultAttacksRemaining[0] = m_catapultAttackCount[0];
    m_catapultAttacksRemaining[1] = m_catapultAttackCount[1];
    m_keepAttacksRemaining[0] = 1;
    m_keepAttacksRemaining[1] = 1;
    m_heroCastSpell[0] = m_heroCastSpell[1] = 0;

    memset(gpCombatManager->m_removedArmies, 0, sizeof(gpCombatManager->m_removedArmies));
    gpCombatManager->m_removedArmyPresent = 0;

    CombatSide side;
    i32 armyIndex;
    for (side = COMBAT_ATTACKER_SIDE; IDX(side) < COMBAT_SIDE_COUNT; side++) {
        for (armyIndex = 0; armyIndex < COMBAT_ARMY_SLOT_COUNT; armyIndex++) {
            army* currentArmy = m_armies[IDX(side)] + armyIndex;
            if (currentArmy->m_quantity > 0) {
                currentArmy->m_monster.flags.abilityFlags &=
                    MONSTER_FLAGS_ROUND_PERSISTENT_MASK;
                if (currentArmy->m_monsterType == CREATURE_TROLL
                    || currentArmy->m_monsterType == CREATURE_WAR_TROLL)
                    currentArmy->m_hitPointsLost = 0;
                currentArmy->DecrementSpellRounds();
                if (currentArmy->m_roundCounter == 0)
                    currentArmy->ProcessDeath(1);
            }
        }
    }

    if (gpCombatManager->m_removedArmyPresent != 0)
        gpCombatManager->MakeCreaturesVanish();
    m_currentSpeed = ROUND_INITIAL_SPEED;
}

VA(0x0042c67f, 0x280)
i32 combatManager::CheckWin(struct tag_message* message) {
    i32 combatEnded = 0;
    if (IsWinner(m_currentSide) != 0) {
        combatEnded = 1;
        if (IsWinner(OppositeCombatSide(m_currentSide)) != 0)
            m_combatResult = COMBAT_RESULT_DRAW;
        else
            m_combatResult = CombatResultForSide(m_currentSide);
    } else if (IsWinner(OppositeCombatSide(m_currentSide)) != 0) {
        combatEnded = 1;
        m_combatResult = CombatResultForSide(OppositeCombatSide(m_currentSide));
    } else if (m_sideRetreated[0] != 0 || m_sideRetreated[1] != 0) {
        combatEnded = 1;
        gbRetreatWin = true;
        if (m_sideRetreated[0] != 0)
            m_combatResult = COMBAT_RESULT_DEFENDER;
        else
            m_combatResult = COMBAT_RESULT_ATTACKER;
    }

    if (combatEnded != 0 && m_combatResult != COMBAT_RESULT_DRAW) {
        i32 armyAlive = 0;
        i32 unusedWinWord37;
        i32 armyIndex;
        for (armyIndex = 0; armyIndex < COMBAT_ARMY_SLOT_COUNT; armyIndex++) {
            if (m_armies[IDX(m_combatResult)][armyIndex].m_monsterType != CREATURE_NONE
                && m_armies[IDX(m_combatResult)][armyIndex].m_quantity > 0
                && HAS(m_armies[IDX(m_combatResult)][armyIndex].m_monster.flags.all,
                       MONSTER_FLAGS_SUMMONED)
                       == 0) {
                armyAlive = 1;
            }
        }
        if (armyAlive == 0)
            m_combatResult = COMBAT_RESULT_DRAW;
    }

    if (combatEnded != 0) {
        DoVictory(m_combatResult);
        if (gbNoShowCombat == 0) {
            message->type = COMBAT_WIN_MESSAGE;
            message->payload.executive.command = EXECUTIVE_COMMAND_TERMINATE_LOOP;
        }
    }
    return combatEnded;
}

VA(0x0042c8ff, 0x51a)
CombatMessageCommand combatManager::GetCommand(i32 hexIndex) {
    i32 column = hexIndex % COMBAT_GRID_ROW_LENGTH;
    i32 rowPos = hexIndex / COMBAT_GRID_ROW_LENGTH;
    CombatMessageCommand command = COMBAT_MESSAGE_COMMAND_DEFAULT;
    i32 showEnemy = 0;

    if (hexIndex == INVALID_HEX) {
        command = COMBAT_MESSAGE_COMMAND_DEFAULT;
        goto smallView;
    }
    switch (hexIndex) {
        case COMBAT_GRID_RIGHT_HERO_HEX:
            if (m_heroes[1] != NULL) {
                if (m_currentSide == COMBAT_DEFENDER_SIDE)
                    command = COMBAT_MESSAGE_COMMAND_OPTIONS;
                else
                    command = COMBAT_MESSAGE_COMMAND_OPPOSING_OPTIONS;
            } else {
                command = COMBAT_MESSAGE_COMMAND_DEFAULT;
            }
            break;
        case COMBAT_GRID_LEFT_SPECIAL_HEX:
            if (m_heroes[0] != NULL) {
                if (m_currentSide == COMBAT_ATTACKER_SIDE)
                    command = COMBAT_MESSAGE_COMMAND_OPTIONS;
                else
                    command = COMBAT_MESSAGE_COMMAND_OPPOSING_OPTIONS;
            } else {
                command = COMBAT_MESSAGE_COMMAND_DEFAULT;
            }
            break;
        case COMBAT_BALLISTA_HEX:
            if (m_inCastleCombat != 0)
                command = COMBAT_MESSAGE_COMMAND_VIEW_INFO;
            else
                command = COMBAT_MESSAGE_COMMAND_DEFAULT;
            break;
        default: {
            if (hexIndex % COMBAT_GRID_ROW_LENGTH == COMBAT_GRID_ROW_LENGTH - 1) {
                command = COMBAT_MESSAGE_COMMAND_DEFAULT;
                break;
            }

            CombatSide enemySide = m_hexCells[hexIndex].m_occupantSide;
            i32 targetIndex = m_hexCells[hexIndex].m_occupantIndex;
            army* ourArmy = &m_armies[IDX(m_currentArmySide)][m_currentArmyIndex];
            ourArmy->m_targetSide = COMBAT_SIDE_NONE;
            ourArmy->m_targetIndex = -1;

            if (m_hexCells[hexIndex].m_blocked != 0
                && (gpCombatManager->m_inCastleCombat == 0
                    || (hexIndex != COMBAT_CASTLE_GATE_APPROACH_HEX
                        && hexIndex != CASTLE_GATE_HEX)
                    || (gpCombatManager->m_drawbridgeState == COMBAT_CASTLE_GATE_OPEN
                        && (gpCombatManager->m_currentSide != COMBAT_DEFENDER_SIDE
                            || gpCombatManager->m_hexCells[COMBAT_CASTLE_GATE_APPROACH_HEX]
                                       .m_occupantSide
                                   != COMBAT_SIDE_NONE
                            || gpCombatManager->m_hexCells[COMBAT_CASTLE_GATE_APPROACH_HEX]
                                       .m_deadOccupantCount
                                   != 0)))) {
                command = COMBAT_MESSAGE_COMMAND_DEFAULT;
            } else if (enemySide != COMBAT_SIDE_NONE) {
                if (m_currentArmySide != enemySide || m_currentArmyIndex != targetIndex) {
                    showEnemy = 1;
                    if (gbProcessingCombatAction == 0 && giNextAction == ACTION_NONE) {
                        m_smallViewSide[1] = enemySide;
                        m_smallViewArmyIndex[1] = targetIndex;
                        DrawSmallView(1, 1);
                    }
                }
                switch (enemySide) {
                    case COMBAT_ATTACKER_SIDE:
                    case COMBAT_DEFENDER_SIDE:
                        if (m_currentSide == enemySide
                            || (m_currentArmySide == enemySide
                                && m_currentArmyIndex == targetIndex)) {
                            return COMBAT_MESSAGE_COMMAND_VIEW_INFO;
                        } else {
                            ourArmy->m_targetSide = enemySide;
                            ourArmy->m_targetIndex = targetIndex;
                            if (ourArmy->m_monster.shots > 0
                                && ourArmy->GetAttackMask(
                                    ourArmy->m_hex,
                                    ARMY_ATTACK_TARGET_ENEMY,
                                    ARMY_HEX_INVALID
                                )
                                       == ARMY_ALL_ATTACK_DIRECTIONS) {
                                if (ShotIsThroughWall(
                                        ourArmy->m_side,
                                        ourArmy->m_hex,
                                        hexIndex
                                    )
                                    != 0)
                                    return COMBAT_MESSAGE_COMMAND_SHOOT_THROUGH_WALL;
                                else
                                    return COMBAT_MESSAGE_COMMAND_SHOOT;
                            }
                            if (ourArmy->ValidPath(hexIndex, ARMY_PATH_ANY_TARGET_HEX) == 1)
                                return COMBAT_MESSAGE_COMMAND_ATTACK;
                            else {
                                ourArmy->m_targetSide = COMBAT_SIDE_NONE;
                                ourArmy->m_targetIndex = -1;
                                command = COMBAT_MESSAGE_COMMAND_DEFAULT;
                            }
                        }
                        break;
                }
            } else {
                if (m_armies[IDX(m_currentArmySide)][m_currentArmyIndex].ValidPath(
                        hexIndex, ARMY_PATH_ANY_TARGET_HEX
                    )
                    == 1) {
                    command = CombatMessageCommand(static_cast<i8>(
                        HAS(m_armies[IDX(m_currentArmySide)][m_currentArmyIndex]
                                .m_monster.flags.all,
                            MONSTER_FLAGS_FLYING)
                            ? IDX(COMBAT_MESSAGE_COMMAND_FLY)
                            : IDX(COMBAT_MESSAGE_COMMAND_MOVE)
                    ));
                }
            }
            break;
        }
    }

smallView:
    if (showEnemy == 0 && gbProcessingCombatAction == 0) {
        m_smallViewSide[1] = COMBAT_SIDE_NONE;
        DrawSmallView(1, 1);
    }
    return command;
}

VA(0x0042ce19, 0x2a6)
i32 combatManager::RightClick(i32 hexIndex) {
    i32 col = hexIndex % COMBAT_GRID_ROW_LENGTH;
    i32 row = hexIndex / COMBAT_GRID_ROW_LENGTH;
    if (hexIndex == INVALID_HEX)
        return 0;

    switch (hexIndex) {
        case COMBAT_BALLISTA_HEX:
            if (m_inCastleCombat != 0)
                ViewBallista(1);
            return 0;
        case COMBAT_GRID_RIGHT_HERO_HEX:
            if (m_heroes[1] != NULL) {
                ViewGeneral(COMBAT_DEFENDER_SIDE, 0, 1);
                ResetMouse();
            }
            return 0;
        case COMBAT_GRID_LEFT_SPECIAL_HEX:
            if (m_heroes[0] != NULL) {
                ViewGeneral(COMBAT_ATTACKER_SIDE, 0, 1);
                ResetMouse();
            }
            return 0;
        default:
            if (hexIndex % COMBAT_GRID_ROW_LENGTH == COMBAT_GRID_ROW_LENGTH - 1)
                return 0;

            CombatSide side = m_hexCells[hexIndex].m_occupantSide;
            i32 armyIndex = m_hexCells[hexIndex].m_occupantIndex;
            if (m_hexCells[hexIndex].m_blocked != 0
                && (gpCombatManager->m_inCastleCombat == 0
                    || (hexIndex != COMBAT_CASTLE_GATE_APPROACH_HEX
                        && hexIndex != CASTLE_GATE_HEX)
                    || (gpCombatManager->m_drawbridgeState == COMBAT_CASTLE_GATE_OPEN
                        && (gpCombatManager->m_currentSide != COMBAT_DEFENDER_SIDE
                            || gpCombatManager->m_hexCells[COMBAT_CASTLE_GATE_APPROACH_HEX]
                                       .m_occupantSide
                                   != COMBAT_SIDE_NONE
                            || gpCombatManager->m_hexCells[COMBAT_CASTLE_GATE_APPROACH_HEX]
                                       .m_deadOccupantCount
                                   != 0)))) {
                return 0;
            } else {
                if (side != COMBAT_SIDE_NONE) {
                    switch (side) {
                        case COMBAT_ATTACKER_SIDE:
                        case COMBAT_DEFENDER_SIDE:
                            gpMouseManager->SetPointer(COMBAT_POINTER_DEFAULT);
                            ViewArmy(
                                &m_armies[IDX(side)][m_hexCells[m_selectedHex].m_occupantIndex], 1
                            );
                            ResetMouse();
                            return 0;
                    }
                } else {
                    return 0;
                }
            }
            break;
    }
    return 0;
}

VA(0x0042d0bf, 0x3b3)
void combatManager::DoCommand(CombatMessageCommand command) {
    i32 unusedCommandWord2;
    i32 unusedCommandWord5;
    army* currentArmy = &m_armies[IDX(m_currentArmySide)][m_currentArmyIndex];
    switch (command) {
        case COMBAT_MESSAGE_COMMAND_DEFAULT:
            break;
        case COMBAT_MESSAGE_COMMAND_MOVE:
        case COMBAT_MESSAGE_COMMAND_FLY:
        case COMBAT_MESSAGE_COMMAND_SHOOT:
        case COMBAT_MESSAGE_COMMAND_SHOOT_THROUGH_WALL:
            giNextAction = ACTION_MOVE;
            giNextActionGridIndex = m_selectedHex;
            giNextActionExtra = -1;
            break;
        case COMBAT_MESSAGE_COMMAND_ATTACK:
            giNextActionGridIndex = m_selectedHex;
            if (m_playerId[IDX(m_currentSide)] == -1 || gbHumanPlayer[m_playerId[IDX(m_currentSide)]] == 0
                || m_gridSelectionDisabled != 0) {
                giNextAction = ACTION_MOVE;
                giNextActionExtra = -1;
            } else {
                giNextAction = ACTION_ATTACK;
                giNextActionExtra = m_directionTargetHex;
            }
            break;
        case COMBAT_MESSAGE_COMMAND_OPTIONS:
            gpMouseManager->SetPointer(COMBAT_POINTER_DEFAULT);
            ViewGeneral(m_currentSide, 1, 0);
            ResetMouse();
            break;
        case COMBAT_MESSAGE_COMMAND_OPPOSING_OPTIONS:
            gpMouseManager->SetPointer(COMBAT_POINTER_DEFAULT);
            ViewGeneral(OppositeCombatSide(m_currentSide), 1, 0);
            ResetMouse();
            break;
        case COMBAT_MESSAGE_COMMAND_VIEW_INFO:
            gpMouseManager->SetPointer(COMBAT_POINTER_DEFAULT);
            if (m_selectedHex == COMBAT_BALLISTA_HEX)
                ViewBallista(0);
            else
                ViewArmy(
                    &m_armies[IDX(m_hexCells[m_selectedHex].m_occupantSide)]
                             [m_hexCells[m_selectedHex].m_occupantIndex],
                    0
                );
            ResetMouse();
            break;
        case COMBAT_MESSAGE_COMMAND_CAST_SPELL:
            if (IsNegationSphereInEffect() != 0) {
                NormalDialog(
                    DATA_COMPGEN(0x004f090c, doCommandTheSphereOfNegationArtifactIs, "The Sphere of Negation artifact is in effect for this battle, disabling all "
                    "combat spells."),
                    NORMAL_DIALOG_INFO,
                    NORMAL_DIALOG_NO_RESOURCE,
                    NORMAL_DIALOG_NO_VALUE,
                    NORMAL_DIALOG_NO_RESOURCE,
                    0,
                    NORMAL_DIALOG_NO_RESOURCE,
                    0,
                    NORMAL_DIALOG_NO_RESOURCE,
                    0
                );
                break;
            }
            ViewSpells(0);
            ResetMouse();
            break;
        case COMBAT_MESSAGE_COMMAND_RETREAT:
            NormalDialog(
                DATA_COMPGEN(0x004f0968, doCommandAreYouSureYouWantTo, "Are you sure you want to retreat?"),
                NORMAL_DIALOG_CONFIRM,
                NORMAL_DIALOG_NO_RESOURCE,
                NORMAL_DIALOG_NO_VALUE,
                NORMAL_DIALOG_NO_RESOURCE,
                0,
                NORMAL_DIALOG_NO_RESOURCE,
                0,
                NORMAL_DIALOG_NO_RESOURCE,
                0
            );
            if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_BUTTON_FIVE)
                giNextAction = ACTION_RETREAT;
            ResetMouse();
            break;
        case COMBAT_MESSAGE_COMMAND_SURRENDER:
            if (DoSurrender() == 1) {
                if (gpGame->m_players[m_playerId[IDX(m_currentSide)]].m_resources[IDX(RES_GOLD)]
                    < giSurrenderCost) {
                    NormalDialog(
                        DATA_COMPGEN(0x004f098c, doCommandYouDonTHaveEnoughGold, "You don't have enough gold!"),
                        NORMAL_DIALOG_INFO,
                        NORMAL_DIALOG_NO_RESOURCE,
                        NORMAL_DIALOG_NO_VALUE,
                        NORMAL_DIALOG_NO_RESOURCE,
                        0,
                        NORMAL_DIALOG_NO_RESOURCE,
                        0,
                        NORMAL_DIALOG_NO_RESOURCE,
                        0
                    );
                } else {
                    giNextAction = ACTION_SURRENDER;
                    giNextActionExtra = giSurrenderCost;
                }
            }
            ResetMouse();
            break;
    }
}

VA(0x0042d472, 0x57b)
MessageDispatchResult WinCombatHandler(struct tag_message& message) {
    char iconFile[WIN_LOSE_ICON_FILENAME_SIZE];
    tag_message animationMessage;
    i32 finalDelay = WIN_LOSE_INITIAL_DELAY;
    i32 frame;
    i32 iDelay;

    if (giDialogTimeout != 0 && KBTickCount() > giDialogTimeout) {
        message.type = MESSAGE_WIDGET;
        gpWindowManager->m_dialogResult = message.payload.widget.id;
        message.payload.widget.id = WIN_LOSE_CLOSE_COMMAND;
        message.payload.widget.command = BaseWidgetCommand(message.payload.widget.id);
        giDialogTimeout = 0;
        return MESSAGE_DISPATCH_FORWARD;
    }

    if (message.type == MESSAGE_WIDGET) {
        switch (message.payload.widget.command) {
            case WIDGET_COMMAND_DESELECT:
                switch (message.payload.widget.id) {
                    case WIN_LOSE_NEXT_CONTROL:
                        if (gbShowingLoseWindow != 0)
                            goto ExitDialog;
                        if (iCurTransferArtifact + 1 < iMaxTransferArtifacts) {
                            gpCombatManager->ClearWinLoseBottom(gpCombatManager->m_winLoseWindow);
                            ++iCurTransferArtifact;
                            gpCombatManager->ShowWinLoseArtifact(
                                gpCombatManager->m_winLoseWindow,
                                iTransferArtifacts[iCurTransferArtifact]
                            );
                        } else if (giSkeletonsCreated != 0 && bSkeletonsShown == 0) {
                            bSkeletonsShown = 1;
                            gpCombatManager->ClearWinLoseBottom(gpCombatManager->m_winLoseWindow);
                            gpCombatManager->ShowSkeletons(gpCombatManager->m_winLoseWindow);
                        } else {
                            if (gpCombatManager->m_eagleEyeSpell[IDX(gpCombatManager->m_combatResult)]
                                != SPELL_NONE) {
                                gpCombatManager->ClearWinLoseBottom(
                                    gpCombatManager->m_winLoseWindow
                                );
                                gpCombatManager->ShowEagleEyeSpell(
                                    gpCombatManager->m_winLoseWindow
                                );
                                gpCombatManager->m_eagleEyeSpell[IDX(gpCombatManager->m_combatResult)] =
                                    SPELL_NONE;
                            } else {
                            ExitDialog:
                                gpWindowManager->m_dialogResult = message.payload.widget.id;
                                message.payload.widget.id = WIN_LOSE_CLOSE_COMMAND;
                                message.payload.widget.command =
                                    BaseWidgetCommand(message.payload.widget.id);
                                return MESSAGE_DISPATCH_FORWARD;
                            }
                        }
                        break;
                    default:
                        break;
                }
                break;
            default:
                break;
        }
    }

    if (KBTickCount() > glTimers[0]) {
        animationMessage.type = MESSAGE_WIDGET;
        animationMessage.payload.widget.command = COMBAT_WIN_LOSE_RESOURCE_COMMAND;
        animationMessage.payload.widget.data.text = iconFile;
        ++giWinCmbtFrame;

        switch (gbWhichAnimationPlaying) {
            case WIN_LOSE_ANIMATION_CYCLE_FIRST:
                frame = giWinCmbtFrame % WIN_LOSE_LOOP_FRAME_COUNT
                        + WIN_LOSE_FIRST_ANIMATION_FRAME;
                iDelay = WIN_LOSE_LOOP_DELAY;
                break;
            case WIN_LOSE_ANIMATION_CYCLE_SECOND:
                frame = giWinCmbtFrame % WIN_LOSE_LOOP_FRAME_COUNT
                        + WIN_LOSE_FIRST_ANIMATION_FRAME;
                iDelay = WIN_LOSE_LOOP_DELAY;
                break;
            case WIN_LOSE_ANIMATION_FLEE:
                if (giWinCmbtFrame == WIN_LOSE_FLEE_SECOND_RESOURCE_FRAME) {
                    sprintf(iconFile, DATA_COMPGEN(0x004f09a8, winCombatHandlerCmbtfle2Icn, "cmbtfle2.icn"));
                    animationMessage.payload.widget.id = WIN_LOSE_RESOURCE_LOAD_ID;
                    gpCombatManager->m_winLoseWindow->BroadcastMessage(animationMessage);
                    animationMessage.payload.widget.id = WIN_LOSE_RESOURCE_DRAW_ID;
                    gpCombatManager->m_winLoseWindow->BroadcastMessage(animationMessage);
                }
                if (giWinCmbtFrame == WIN_LOSE_FLEE_THIRD_RESOURCE_FRAME) {
                    sprintf(iconFile, DATA_COMPGEN(0x004f09b8, winCombatHandlerCmbtfle3Icn, "cmbtfle3.icn"));
                    animationMessage.payload.widget.id = WIN_LOSE_RESOURCE_LOAD_ID;
                    gpCombatManager->m_winLoseWindow->BroadcastMessage(animationMessage);
                    animationMessage.payload.widget.id = WIN_LOSE_RESOURCE_DRAW_ID;
                    gpCombatManager->m_winLoseWindow->BroadcastMessage(animationMessage);
                }
                if (giWinCmbtFrame < WIN_LOSE_FLEE_SECOND_RESOURCE_FRAME) {
                    frame = giWinCmbtFrame + 1;
                } else if (giWinCmbtFrame < WIN_LOSE_FLEE_THIRD_RESOURCE_FRAME) {
                    frame = giWinCmbtFrame - (WIN_LOSE_FLEE_SECOND_RESOURCE_FRAME - 1);
                } else {
                    frame = giWinCmbtFrame - (WIN_LOSE_FLEE_THIRD_RESOURCE_FRAME - 1);
                    if (frame > WIN_LOSE_FLEE_LAST_FRAME)
                        frame = WIN_LOSE_FLEE_LAST_FRAME;
                }
                iDelay = WIN_LOSE_FLEE_DELAY;
                break;
            default:
                if (giWinCmbtFrame == WIN_LOSE_LOSS_SECOND_RESOURCE_FRAME) {
                    sprintf(iconFile, DATA_COMPGEN(0x004f09c8, winCombatHandlerCmbtlos2Icn, "cmbtlos2.icn"));
                    animationMessage.payload.widget.id = WIN_LOSE_RESOURCE_LOAD_ID;
                    gpCombatManager->m_winLoseWindow->BroadcastMessage(animationMessage);
                    animationMessage.payload.widget.id = WIN_LOSE_RESOURCE_DRAW_ID;
                    gpCombatManager->m_winLoseWindow->BroadcastMessage(animationMessage);
                }
                if (giWinCmbtFrame == WIN_LOSE_LOSS_THIRD_RESOURCE_FRAME) {
                    sprintf(iconFile, DATA_COMPGEN(0x004f09d8, winCombatHandlerCmbtlos3Icn, "cmbtlos3.icn"));
                    animationMessage.payload.widget.id = WIN_LOSE_RESOURCE_LOAD_ID;
                    gpCombatManager->m_winLoseWindow->BroadcastMessage(animationMessage);
                    animationMessage.payload.widget.id = WIN_LOSE_RESOURCE_DRAW_ID;
                    gpCombatManager->m_winLoseWindow->BroadcastMessage(animationMessage);
                }
                if (giWinCmbtFrame < WIN_LOSE_LOSS_SECOND_RESOURCE_FRAME) {
                    frame = giWinCmbtFrame + 1;
                    iDelay = WIN_LOSE_LOSS_DELAY;
                } else if (giWinCmbtFrame < WIN_LOSE_LOSS_THIRD_RESOURCE_FRAME) {
                    frame = giWinCmbtFrame - (WIN_LOSE_LOSS_SECOND_RESOURCE_FRAME - 1);
                    iDelay = WIN_LOSE_LOSS_DELAY;
                } else {
                    if (giWinCmbtFrame == WIN_LOSE_LOSS_RESET_FRAME)
                        giWinCmbtFrame = WIN_LOSE_LOSS_THIRD_RESOURCE_FRAME;
                    frame = giWinCmbtFrame - (WIN_LOSE_LOSS_THIRD_RESOURCE_FRAME - 1);
                    iDelay = WIN_LOSE_LOSS_FINAL_DELAY;
                }
                break;
        }

        message.type = MESSAGE_WIDGET;
        message.payload.widget.command = COMBAT_WIN_LOSE_ANIMATION_COMMAND;
        message.payload.widget.id = WIN_LOSE_RESOURCE_DRAW_ID;
        message.payload.widget.data.value = frame;
        gpCombatManager->m_winLoseWindow->BroadcastMessage(message);
        gpCombatManager->m_winLoseWindow->DrawWindow(1, 0, WIN_LOSE_DRAW_DEPTH);
        glTimers[0] = KBTickCount() + iDelay;
    }
    return MESSAGE_DISPATCH_CONSUME;
}

VA(0x0042d9ed, 0x110)
void combatManager::ClearWinLoseBottom(class heroWindow* window) {
    i32 widgetIndex;
    for (widgetIndex = 0; widgetIndex < WIN_LOSE_WIDGET_COUNT; widgetIndex++) {
        if (m_winLoseBottomWidgets[widgetIndex] != NULL) {
            window->RemoveWidget(m_winLoseBottomWidgets[widgetIndex]);
            delete m_winLoseBottomWidgets[widgetIndex];
        }
        if (m_winLoseBottomTextWidgets[widgetIndex] != NULL) {
            window->RemoveWidget(m_winLoseBottomTextWidgets[widgetIndex]);
            delete m_winLoseBottomTextWidgets[widgetIndex];
        }
        m_winLoseBottomWidgets[widgetIndex] = NULL;
        m_winLoseBottomTextWidgets[widgetIndex] = NULL;
    }
}

VA(0x0042dafd, 0x29a)
void combatManager::ShowWinLoseArtifact(
    class heroWindow* window, H2_ENUM_PARAM(ArtifactType, i32) artifact
) {
    DATA(0x004f09e8) static i16 artifactSourceLineBase = 0x680; // NOLINT(readability-magic-numbers)
    i16 w = CASUALTY_WINDOW_WIDTH;
    i16 winBottom = CASUALTY_WINDOW_BOTTOM;
    tag_message message;
    char* capturedArtifactName;

    sprintf(gText, DATA_COMPGEN(0x004f09ec, showWinLoseArtifactYouHaveCapturedAnEnemyArtifact, "You have captured an enemy artifact!"));
    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = COMBAT_WIN_LOSE_TEXT_COMMAND;
    message.payload.widget.id = WIN_LOSE_TEXT_ID;
    message.payload.widget.data.text = gText;
    m_winLoseWindow->BroadcastMessage(message);

    m_winLoseBottomWidgets[0] = new iconWidget(
        ARTIFACT_BACKGROUND_X,
        ARTIFACT_BACKGROUND_Y,
        ARTIFACT_BACKGROUND_SIZE,
        ARTIFACT_BACKGROUND_SIZE,
        DATA_COMPGEN(0x004f0a14, showWinLoseArtifactWinlosebIcn, "winloseb.icn"),
        0,
        ICON_DRAW_NORMAL,
        WIN_LOSE_ARTIFACT_ICON_ID,
        WIDGET_KIND_ICON_DIRECT,
        1
    );
    if (m_winLoseBottomWidgets[0] == NULL)
        MemError();
    window->AddWidget(m_winLoseBottomWidgets[0], -1);

    m_winLoseBottomWidgets[1] = new iconWidget(
        ARTIFACT_ICON_X,
        ARTIFACT_ICON_Y,
        ARTIFACT_ICON_SIZE,
        ARTIFACT_ICON_SIZE,
        DATA_COMPGEN(0x004f0a24, showWinLoseArtifactArtifactIcn, "artifact.icn"),
        IDX(artifact) + 1,
        ICON_DRAW_NORMAL,
        WIN_LOSE_ARTIFACT_IMAGE_ID,
        WIDGET_KIND_ICON_DIRECT,
        1
    );
    if (m_winLoseBottomWidgets[1] == NULL)
        MemError();
    window->AddWidget(m_winLoseBottomWidgets[1], -1);

    capturedArtifactName = static_cast<char*>(H2_ALLOC_AT(ARTIFACT_NAME_CAPACITY, DATA_COMPGEN(0x004f0a34, showWinLoseArtifactSourceFile, RETAIL_FILE), artifactSourceLineBase + 0x2b));
    sprintf(capturedArtifactName, gArtifactNames[IDX(artifact)]);
    m_winLoseBottomTextWidgets[0] = new textWidget(
        ARTIFACT_TEXT_X,
        ARTIFACT_TEXT_Y,
        CASUALTY_WINDOW_WIDTH,
        ARTIFACT_TEXT_HEIGHT,
        capturedArtifactName,
        DATA_COMPGEN(0x004f0a60, showWinLoseArtifactSmalfontFnt, "smalfont.fnt"),
        FONT_DRAW_DEFAULT,
        WIN_LOSE_ARTIFACT_TEXT_ID,
        WIDGET_KIND_TEXT,
        FONT_ALIGN_CENTER
    );
    if (m_winLoseBottomTextWidgets[0] == NULL)
        MemError();
    window->AddWidget(m_winLoseBottomTextWidgets[0], -1);

    gpCombatManager->m_winLoseWindow->DrawWindow();
    SAMPLE2 pickupSample = NULL_SAMPLE2;
    sprintf(gText, DATA_COMPGEN(0x004f0a70, showWinLoseArtifactPickup02d82M, "pickup%02d.82M"), SRandom(PICKUP_SAMPLE_FIRST, PICKUP_SAMPLE_LAST));
    pickupSample = LoadPlaySample(gText);
    WaitEndSample(pickupSample, -1);
}

VA(0x0042dd97, 0x232)
void combatManager::ShowSkeletons(class heroWindow* window) {
    DATA(0x004f0a80) static i16 skeletonSourceLineBase = 0x6c8; // NOLINT(readability-magic-numbers)
    tag_message message;
    char* skeletonCount;

    m_winLoseBottomWidgets[0] = new iconWidget(
        SKELETON_ICON_X,
        SKELETON_ICON_Y,
        SKELETON_ICON_WIDTH,
        SKELETON_ICON_HEIGHT,
        DATA_COMPGEN(0x004f0a84, showSkeletonsMons32Icn, "mons32.icn"),
        IDX(CREATURE_SKELETON),
        ICON_DRAW_NORMAL,
        WIN_LOSE_SKELETON_ICON_ID,
        WIDGET_KIND_ICON_DIRECT,
        1
    );
    if (m_winLoseBottomWidgets[0] == NULL)
        MemError();

    skeletonCount = static_cast<char*>(H2_ALLOC_AT(SKELETON_COUNT_CAPACITY, DATA_COMPGEN(0x004f0a90, showSkeletonsSourceFile, RETAIL_FILE), skeletonSourceLineBase + 0x13));
    sprintf(skeletonCount, DATA_COMPGEN(0x004f0abc, showSkeletonsD, "%d"), giSkeletonsCreated);
    m_winLoseBottomTextWidgets[0] = new textWidget(
        SKELETON_TEXT_X,
        SKELETON_TEXT_Y,
        SKELETON_TEXT_WIDTH,
        SKELETON_TEXT_HEIGHT,
        skeletonCount,
        DATA_COMPGEN(0x004f0ac0, showSkeletonsSmalfontFnt, "smalfont.fnt"),
        FONT_DRAW_DEFAULT,
        WIN_LOSE_SKELETON_TEXT_ID,
        WIDGET_KIND_TEXT,
        FONT_ALIGN_CENTER
    );
    if (m_winLoseBottomTextWidgets[0] == NULL)
        MemError();

    window->AddWidget(m_winLoseBottomWidgets[0], -1);
    window->AddWidget(m_winLoseBottomTextWidgets[0], -1);
    if (giSkeletonsCreated > 1) {
        sprintf(
            gText,
            DATA_COMPGEN(0x004f0ad0, showSkeletonsPracticingTheDarkArtsOfNecromancy, "Practicing the dark arts of necromancy, you are able to "
            "raise %d of the enemy's dead to return under your service "
            "as Skeletons."),
            giSkeletonsCreated
        );
    } else {
        sprintf(
            gText,
            DATA_COMPGEN(0x004f0b50, showSkeletonsPracticingTheDarkArtsOfNecromancy2, "Practicing the dark arts of necromancy, you are able to "
            "raise one of the enemy's dead to return under your service "
            "as a Skeleton.")
        );
    }
    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = COMBAT_WIN_LOSE_TEXT_COMMAND;
    message.payload.widget.id = WIN_LOSE_TEXT_ID;
    message.payload.widget.data.text = gText;
    m_winLoseWindow->BroadcastMessage(message);
    gpCombatManager->m_winLoseWindow->DrawWindow();

    SAMPLE2 pickupSample = NULL_SAMPLE2;
    sprintf(gText, DATA_COMPGEN(0x004f0bd4, showSkeletonsPickup02d82M, "pickup%02d.82M"), SRandom(PICKUP_SAMPLE_FIRST, PICKUP_SAMPLE_LAST));
    pickupSample = LoadPlaySample(gText);
    WaitEndSample(pickupSample, -1);
}

VA(0x0042dfc9, 0x2f6)
void combatManager::ShowEagleEyeSpell(class heroWindow* window) {
    DATA(0x004f0be4) static i16 eagleEyeSourceLineBase = 0x702; // NOLINT(readability-magic-numbers)
    SpellType displayedSpell = m_eagleEyeSpell[IDX(m_combatResult)];
    i32 x = EAGLE_PANEL_X;
    i32 y = EAGLE_PANEL_Y;
    tag_message spellMessage;
    char* spellName;

    m_winLoseBottomWidgets[0] = new iconWidget(
        x + EAGLE_BACKGROUND_X_OFFSET,
        y,
        0,
        0,
        DATA_COMPGEN(0x004f0be8, showEagleEyeSpellTownwindIcn, "townwind.icn"),
        0,
        ICON_DRAW_NORMAL,
        WIN_LOSE_EAGLE_BACKGROUND_ID,
        WIDGET_KIND_ICON_DIRECT,
        1
    );
    if (m_winLoseBottomWidgets[0] == NULL)
        MemError();

    m_winLoseBottomWidgets[1] = new iconWidget(
        x + EAGLE_ICON_X_OFFSET,
        y + EAGLE_ICON_Y_OFFSET,
        EAGLE_ICON_WIDTH,
        EAGLE_ICON_HEIGHT,
        DATA_COMPGEN(0x004f0bf8, showEagleEyeSpellSpellsIcn, "spells.icn"),
        static_cast<i16>(gsSpellInfo[IDX(displayedSpell)].iconIndex),
        ICON_DRAW_NORMAL,
        WIN_LOSE_EAGLE_SPELL_ID,
        WIDGET_KIND_ICON_CENTERED,
        1
    );
    if (m_winLoseBottomWidgets[1] == NULL)
        MemError();

    spellName = static_cast<char*>(H2_ALLOC_AT(EAGLE_SPELL_NAME_CAPACITY, DATA_COMPGEN(0x004f0c04, showEagleEyeSpellSourceFile, RETAIL_FILE), eagleEyeSourceLineBase + 0x22));
    sprintf(spellName, DATA_COMPGEN(0x004f0c30, showEagleEyeSpellS, "%s"), gSpellNames[IDX(displayedSpell)]);
    m_winLoseBottomTextWidgets[0] = new textWidget(
        x + EAGLE_TEXT_X_OFFSET,
        y + EAGLE_TEXT_Y_OFFSET,
        EAGLE_TEXT_WIDTH,
        EAGLE_TEXT_HEIGHT,
        spellName,
        DATA_COMPGEN(0x004f0c34, showEagleEyeSpellSmalfontFnt, "smalfont.fnt"),
        FONT_DRAW_DEFAULT,
        WIN_LOSE_EAGLE_TEXT_ID,
        WIDGET_KIND_TEXT,
        FONT_ALIGN_CENTER
    );
    if (m_winLoseBottomTextWidgets[0] == NULL)
        MemError();

    window->AddWidget(m_winLoseBottomWidgets[0], -1);
    window->AddWidget(m_winLoseBottomWidgets[1], -1);
    window->AddWidget(m_winLoseBottomTextWidgets[0], -1);
    sprintf(
        gText,
        DATA_COMPGEN(0x004f0c44, showEagleEyeSpellThroughEagleEyedObservationSIs, "Through eagle-eyed observation, %s is able to learn the magic "
        "spell '%s'."),
        m_heroes[IDX(m_combatResult)]->m_name,
        gSpellNames[IDX(displayedSpell)]
    );
    spellMessage.type = MESSAGE_WIDGET;
    spellMessage.payload.widget.command = COMBAT_WIN_LOSE_TEXT_COMMAND;
    spellMessage.payload.widget.id = WIN_LOSE_TEXT_ID;
    spellMessage.payload.widget.data.text = gText;
    m_winLoseWindow->BroadcastMessage(spellMessage);
    gpCombatManager->m_winLoseWindow->DrawWindow();

    SAMPLE2 playedSample = NULL_SAMPLE2;
    sprintf(gText, DATA_COMPGEN(0x004f0c90, showEagleEyeSpellPickup02d82M, "pickup%02d.82M"), SRandom(PICKUP_SAMPLE_FIRST, PICKUP_SAMPLE_LAST));
    playedSample = LoadPlaySample(gText);
    WaitEndSample(playedSample, -1);
}

VA(0x0042e2bf, 0x9cc)
void combatManager::ShowDeadArmies(class heroWindow* window) {
    DATA(0x004f0ca0) static i16 casualtySourceLineBase = 0x74b; // NOLINT(readability-magic-numbers)
    i32 casualtyQuantity_0[CASUALTY_QUANTITY_STORAGE_COUNT];
    i32 casualtyType_1[COMBAT_SIDE_COUNT][COMBAT_ARMY_SLOT_COUNT];
    i32 side_9;
    i32 armyIndex_8;
    i32 y_29;
    i16 width_9 = CASUALTY_WINDOW_WIDTH;
    i16 bottom_4 = CASUALTY_WINDOW_BOTTOM;
    i32 displayedCount_11;
    i32 spacing_9;
    i32 startX_0;
    char* text_27;
    icon* monsterIcons_2;
    i32 unusedCasualtyWord5;
    i32 unusedCasualtyWord16;
    i32 unusedCasualtyWord22;
    i32 unusedCasualtyWord49;
    i32 unusedCasualtyWord9;
    i32 unusedCasualtyWord8;
    i32 unusedCasualtyWord18;

    for (side_9 = 0; side_9 < WIN_LOSE_WIDGET_COUNT; ++side_9) {
        m_winLoseBottomWidgets[side_9] = NULL;
        m_winLoseBottomTextWidgets[side_9] = NULL;
    }
    for (side_9 = 0; side_9 < COMBAT_SIDE_COUNT; ++side_9) {
        casualtyQuantity_0[side_9] = 0;
        for (armyIndex_8 = 0; armyIndex_8 < COMBAT_ARMY_SLOT_COUNT; ++armyIndex_8) {
            if (m_armies[side_9][armyIndex_8].m_monsterType != CREATURE_NONE
                && m_armies[side_9][armyIndex_8].m_quantity
                       < m_armies[side_9][armyIndex_8].m_initialQuantity) {
                *(&casualtyType_1[0][0] + side_9 * COMBAT_ARMY_SLOT_COUNT
                  + casualtyQuantity_0[side_9]) = IDX(m_armies[side_9][armyIndex_8].m_monsterType);
                *(&casualtyQuantity_0[COMBAT_SIDE_COUNT] + side_9 * COMBAT_ARMY_SLOT_COUNT
                  + casualtyQuantity_0[side_9]) = m_armies[side_9][armyIndex_8].m_initialQuantity
                                                  - m_armies[side_9][armyIndex_8].m_quantity;
                ++casualtyQuantity_0[side_9];
            }
        }
    }

    text_27 = static_cast<char*>(H2_ALLOC_AT(CASUALTY_HEADING_CAPACITY, DATA_COMPGEN(0x004f0ca4, showDeadArmiesSourceFile, RETAIL_FILE), casualtySourceLineBase + 0x23));
    sprintf(text_27, DATA_COMPGEN(0x004f0cd0, showDeadArmiesBattlefieldCasualties, "Battlefield Casualties"));
    m_winLoseBottomTextWidgets[CASUALTY_TITLE_WIDGET] = new textWidget(
        CASUALTY_TITLE_X,
        CASUALTY_TITLE_Y,
        CASUALTY_WINDOW_WIDTH,
        CASUALTY_TEXT_HEIGHT,
        text_27,
        DATA_COMPGEN(0x004f0ce8, showDeadArmiesSmalfontFnt, "smalfont.fnt"),
        FONT_DRAW_DEFAULT,
        CASUALTY_TEXT_WIDGET_ID,
        WIDGET_KIND_TEXT,
        FONT_ALIGN_CENTER
    );
    if (m_winLoseBottomTextWidgets[CASUALTY_TITLE_WIDGET] == NULL)
        MemError();
    window->AddWidget(m_winLoseBottomTextWidgets[CASUALTY_TITLE_WIDGET], -1);

    for (side_9 = 0; side_9 < COMBAT_SIDE_COUNT; ++side_9) {
        if (side_9 == IDX(COMBAT_ATTACKER_SIDE))
            y_29 = CASUALTY_ATTACKER_Y;
        else
            y_29 = CASUALTY_DEFENDER_Y;
        text_27 = static_cast<char*>(H2_ALLOC_AT(CASUALTY_HEADING_CAPACITY, DATA_COMPGEN(0x004f0cf8, showDeadArmiesSourceFile2, RETAIL_FILE), casualtySourceLineBase + 0x38));
        sprintf(text_27, side_9 == IDX(COMBAT_ATTACKER_SIDE) ? DATA_COMPGEN(0x004f0d24, showDeadArmiesAttacker, "Attacker") : DATA_COMPGEN(0x004f0d30, showDeadArmiesDefender, "Defender"));
        m_winLoseBottomTextWidgets[CASUALTY_SIDE_WIDGET_FIRST + side_9] = new textWidget(
            CASUALTY_TITLE_X,
            y_29 + CASUALTY_SIDE_LABEL_Y_OFFSET,
            CASUALTY_WINDOW_WIDTH,
            CASUALTY_TEXT_HEIGHT,
            text_27,
            DATA_COMPGEN(0x004f0d3c, showDeadArmiesSmalfontFnt2, "smalfont.fnt"),
            FONT_DRAW_DEFAULT,
            CASUALTY_TEXT_WIDGET_ID,
            WIDGET_KIND_TEXT,
            FONT_ALIGN_CENTER
        );
        if (m_winLoseBottomTextWidgets[CASUALTY_SIDE_WIDGET_FIRST + side_9] == NULL)
            MemError();
        window->AddWidget(m_winLoseBottomTextWidgets[CASUALTY_SIDE_WIDGET_FIRST + side_9], -1);

        if (casualtyQuantity_0[side_9] <= 0) {
            text_27 = static_cast<char*>(H2_ALLOC_AT(CASUALTY_NONE_CAPACITY, DATA_COMPGEN(0x004f0d4c, showDeadArmiesSourceFile3, RETAIL_FILE), casualtySourceLineBase + 0x4b));
            sprintf(text_27, DATA_COMPGEN(0x004f0d78, showDeadArmiesNone, "None"));
            m_winLoseBottomTextWidgets[side_9 * CASUALTY_WIDGETS_PER_SIDE] = new textWidget(
                CASUALTY_TITLE_X,
                y_29 + CASUALTY_NONE_Y_OFFSET,
                CASUALTY_WINDOW_WIDTH,
                CASUALTY_TEXT_HEIGHT,
                text_27,
                DATA_COMPGEN(0x004f0d80, showDeadArmiesSmalfontFnt3, "smalfont.fnt"),
                FONT_DRAW_DEFAULT,
                side_9 * CASUALTY_WIDGET_ID_STRIDE + CASUALTY_TEXT_WIDGET_ID_FIRST,
                WIDGET_KIND_TEXT,
                FONT_ALIGN_CENTER
            );
            if (m_winLoseBottomTextWidgets[side_9 * CASUALTY_WIDGETS_PER_SIDE] == NULL)
                MemError();
            window->AddWidget(
                m_winLoseBottomTextWidgets[side_9 * CASUALTY_WIDGETS_PER_SIDE],
                -1
            );
        }

        monsterIcons_2 = gpResourceManager->GetIcon(DATA_COMPGEN(0x004f0d90, showDeadArmiesMons32Icn, "mons32.icn"));
        displayedCount_11 = casualtyQuantity_0[side_9] < CASUALTY_DISPLAY_LIMIT
                                ? casualtyQuantity_0[side_9]
                                : CASUALTY_DISPLAY_LIMIT;
        spacing_9 = CASUALTY_ICON_SPACING;
        startX_0 = (CASUALTY_WINDOW_WIDTH - displayedCount_11 * spacing_9) / CASUALTY_CENTER_DIVISOR
                   + CASUALTY_CENTER_X_OFFSET;
        for (armyIndex_8 = 0; displayedCount_11 > armyIndex_8; ++armyIndex_8) {
            m_winLoseBottomWidgets[side_9 * CASUALTY_WIDGETS_PER_SIDE + armyIndex_8] =
                new iconWidget(
                    armyIndex_8 * spacing_9 + startX_0 + CASUALTY_ICON_CELL_WIDTH / 2
                        - GetIconEntry(
                              monsterIcons_2,
                              *(&casualtyType_1[0][0] + side_9 * COMBAT_ARMY_SLOT_COUNT
                                + armyIndex_8)
                        )
                              ->x
                        + (CASUALTY_ICON_CELL_WIDTH
                           - GetIconEntry(
                                 monsterIcons_2,
                                 *(&casualtyType_1[0][0] + side_9 * COMBAT_ARMY_SLOT_COUNT
                                   + armyIndex_8)
                           )
                                 ->w)
                            / CASUALTY_CENTER_DIVISOR
                        + 1,
                    (y_29 + CASUALTY_ICON_TOP_OFFSET
                     - GetIconEntry(
                           monsterIcons_2,
                           *(&casualtyType_1[0][0] + side_9 * COMBAT_ARMY_SLOT_COUNT + armyIndex_8)
                     )
                           ->y
                     - GetIconEntry(
                           monsterIcons_2,
                           *(&casualtyType_1[0][0] + side_9 * COMBAT_ARMY_SLOT_COUNT + armyIndex_8)
                     )
                           ->h)
                        + CASUALTY_ICON_CELL_WIDTH,
                    CASUALTY_ICON_WIDTH,
                    CASUALTY_ICON_HEIGHT,
                    DATA_COMPGEN(0x004f0d9c, showDeadArmiesMons32Icn2, "mons32.icn"),
                    *(&casualtyType_1[0][0] + side_9 * COMBAT_ARMY_SLOT_COUNT + armyIndex_8),
                    ICON_DRAW_NORMAL,
                    side_9 * CASUALTY_WIDGET_ID_STRIDE + armyIndex_8 + CASUALTY_ICON_WIDGET_ID_FIRST,
                    WIDGET_KIND_ICON_DIRECT,
                    1
                );
            if (m_winLoseBottomWidgets[side_9 * CASUALTY_WIDGETS_PER_SIDE + armyIndex_8]
                == NULL)
                MemError();

            text_27 = static_cast<char*>(H2_ALLOC_AT(CASUALTY_QUANTITY_CAPACITY, DATA_COMPGEN(0x004f0da8, showDeadArmiesSourceFile4, RETAIL_FILE), casualtySourceLineBase + 0x77));
            sprintf(
                text_27,
                DATA_COMPGEN(0x004f0dd4, showDeadArmiesD, "%d"),
                *(&casualtyQuantity_0[COMBAT_SIDE_COUNT] + side_9 * COMBAT_ARMY_SLOT_COUNT
                  + armyIndex_8)
            );
            m_winLoseBottomTextWidgets[side_9 * CASUALTY_WIDGETS_PER_SIDE + armyIndex_8] =
                new textWidget(
                    armyIndex_8 * spacing_9 + startX_0 + CASUALTY_QUANTITY_X_OFFSET,
                    y_29 + CASUALTY_QUANTITY_Y_OFFSET,
                    CASUALTY_QUANTITY_WIDTH,
                    CASUALTY_QUANTITY_HEIGHT,
                    text_27,
                    DATA_COMPGEN(0x004f0dd8, showDeadArmiesSmalfontFnt4, "smalfont.fnt"),
                    FONT_DRAW_DEFAULT,
                    side_9 * CASUALTY_WIDGET_ID_STRIDE + armyIndex_8 + CASUALTY_TEXT_WIDGET_ID_FIRST,
                    WIDGET_KIND_TEXT,
                    FONT_ALIGN_CENTER
                );
            if (m_winLoseBottomTextWidgets[side_9 * CASUALTY_WIDGETS_PER_SIDE + armyIndex_8]
                == NULL)
                MemError();
            window->AddWidget(
                m_winLoseBottomWidgets[side_9 * CASUALTY_WIDGETS_PER_SIDE + armyIndex_8],
                -1
            );
            window->AddWidget(
                m_winLoseBottomTextWidgets[side_9 * CASUALTY_WIDGETS_PER_SIDE + armyIndex_8],
                -1
            );
        }
        gpResourceManager->Dispose(monsterIcons_2);
    }
}

VA(0x0042ec8b, 0xba9)
void combatManager::DoVictory(H2_ENUM_PARAM(CombatResult, i32) winningSide) {
    char experienceText[VICTORY_EXPERIENCE_TEXT_SIZE];
    i32 emptySlots;
    i32 living;
    i32 fadeCount;
    i32 timer;
    i32 i;
    i32 necroEligible;
    i32 cost;
    i32 fadeIndex26;
    army* currentArmy26;
    tag_message message;
    i32 victoryLevels = 0;
    CombatSide combatSide;
    i32 deadCreatureCount3;
    i32 lastLivingArmy0;

    if (m_heroes[IDX(COMBAT_DEFENDER_SIDE)] != NULL && m_heroes[IDX(COMBAT_DEFENDER_SIDE)]->m_isCaptain != 0)
        m_heroes[IDX(COMBAT_DEFENDER_SIDE)] = NULL;
    gbShowingLoseWindow = false;
    gbWhichAnimationPlaying = true;
    giWinCmbtFrame = 0;
    giSkeletonsCreated = 0;
    iMaxTransferArtifacts = 0;
    iCurTransferArtifact = -1;
    bSkeletonsShown = 0;
    deadCreatureCount3 = 0;
    necroEligible = 0;

    for (combatSide = COMBAT_ATTACKER_SIDE; IDX(combatSide) < COMBAT_SIDE_COUNT; ++combatSide) {
        living = 0;
        lastLivingArmy0 = -1;
        for (i = 0; i < gpCombatManager->m_armyCount[IDX(combatSide)]; ++i) {
            currentArmy26 = &m_armies[IDX(combatSide)][i];
            if (currentArmy26->m_quantity > 0) {
                lastLivingArmy0 = i;
                if (currentArmy26->m_temporaryResurrectionQuantity > 0)
                    currentArmy26->m_quantity -= currentArmy26->m_temporaryResurrectionQuantity;
                if (currentArmy26->m_quantity < 0)
                    currentArmy26->m_quantity = 0;
                living += currentArmy26->m_quantity;
            }
            if (CombatResultForSide(combatSide) == winningSide && currentArmy26->m_quantity > 0
                && HAS(currentArmy26->m_monster.flags.all, MONSTER_FLAGS_LIGHT_PALETTE) == 0
                && currentArmy26->m_monsterType != CREATURE_EARTH_ELEMENTAL
                && currentArmy26->m_monsterType != CREATURE_AIR_ELEMENTAL
                && currentArmy26->m_monsterType != CREATURE_FIRE_ELEMENTAL
                && currentArmy26->m_monsterType != CREATURE_WATER_ELEMENTAL
                && currentArmy26->m_monsterType != CREATURE_GHOST) {
                ++necroEligible;
            }
            if (winningSide == OppositeCombatResult(CombatResultForSide(combatSide))) {
                deadCreatureCount3 += currentArmy26->m_initialQuantity - currentArmy26->m_quantity;
            }
        }
        if (living == 0 && lastLivingArmy0 != -1)
            m_armies[IDX(combatSide)][lastLivingArmy0].m_quantity = 1;
    }

    if (winningSide != COMBAT_RESULT_DRAW && necroEligible < VICTORY_NECROMANCY_STACK_LIMIT
        && m_heroes[IDX(winningSide)] != NULL
        && m_heroes[IDX(winningSide)]->GetSSLevel(HERO_SKILL_NECROMANCY) != 0) {
        giSkeletonsCreated = static_cast<i32>(
            deadCreatureCount3
            * (m_heroes[IDX(winningSide)]->GetSSLevel(HERO_SKILL_NECROMANCY)
               * COMBAT_NECROMANCY_LEVEL_FACTOR)
        );
        if (giSkeletonsCreated <= 0 && deadCreatureCount3 != 0)
            giSkeletonsCreated = 1;
    }

    m_nonVisualCombat = 1;
    FreeArmies();
    CombatMessage(DATA_COMPGEN(0x004f0de8, doVictoryEmptyString, ""), 1, 1, 0);
    gpMouseManager->SetPointer(COMBAT_POINTER_DEFAULT);
    fadeCount = VICTORY_FADE_STEPS;
    if (m_terrainType == TERRAIN_WASTELAND)
        fadeCount = VICTORY_WASTELAND_FADE_STEPS;
    timer = KBTickCount();
    for (fadeIndex26 = 0; fadeCount > fadeIndex26; ++fadeIndex26) {
        PollSound();
        DelayTil(&timer);
        timer = KBTickCount() + VICTORY_FADE_DELAY;
        DimBitmapArea(
            gpWindowManager->m_screen,
            0,
            0,
            COMBAT_SCREEN_WIDTH,
            COMBAT_SCREEN_HEIGHT,
            VICTORY_DIM_STEP
        );
        PollSound();
        gpWindowManager->UpdateScreenRegion(0, 0, SCREEN_MAX_X, SCREEN_MAX_Y);
        PollSound();
    }

    switch (winningSide) {
        case COMBAT_RESULT_DRAW:
            gpSoundManager->SwitchAmbientMusic(LOSS_MUSIC);
            DoLoseWindow();
            break;
        case COMBAT_RESULT_ATTACKER:
        case COMBAT_RESULT_DEFENDER:
            if (m_heroes[IDX(winningSide)] != NULL) {
                if (m_eagleEyeSpell[IDX(winningSide)] != SPELL_NONE) {
                    m_heroes[IDX(winningSide)]->m_spells[IDX(m_eagleEyeSpell[IDX(winningSide)])] = 1;
                }
                m_experienceValue[IDX(OppositeCombatResult(winningSide))] =
                    ExperienceValueOfStack(IDX(OppositeCombatResult(winningSide)));
                if (gbRetreatWin != 0)
                    m_experienceValue[IDX(OppositeCombatResult(winningSide))] -=
                        COMBAT_HERO_EXPERIENCE_VALUE;
                if (m_combatTowns[IDX(COMBAT_DEFENDER_SIDE)] != NULL
                    && winningSide == COMBAT_RESULT_ATTACKER)
                    m_experienceValue[IDX(OppositeCombatResult(winningSide))] +=
                        COMBAT_HERO_EXPERIENCE_VALUE;
                victoryLevels = gpAdvManager->GiveExperience(
                    m_heroes[IDX(winningSide)],
                    m_experienceValue[IDX(OppositeCombatResult(winningSide))],
                    gbThisNetHumanPlayer[m_heroes[IDX(winningSide)]->m_owner] == 0
                );

                if (gbRetreatWin == 0) {
                    emptySlots = 0;
                    if (m_heroes[IDX(COMBAT_ATTACKER_SIDE)] != NULL
                        && m_heroes[IDX(COMBAT_DEFENDER_SIDE)] != NULL) {
                        for (fadeIndex26 = 0; fadeIndex26 < HERO_ARTIFACT_SLOT_COUNT; ++fadeIndex26) {
                            if (m_heroes[IDX(winningSide)]->m_artifacts[fadeIndex26]
                                == ARTIFACT_NONE) {
                                ++emptySlots;
                            }
                        }
                        for (fadeIndex26 = 0; fadeIndex26 < HERO_ARTIFACT_SLOT_COUNT; ++fadeIndex26) {
                            if (m_heroes[IDX(OppositeCombatResult(winningSide))]->m_artifacts[fadeIndex26]
                                    >= ARTIFACT_ARCANE_NECKLACE
                                && m_heroes[IDX(OppositeCombatResult(winningSide))]
                                           ->m_artifacts[fadeIndex26]
                                       != ARTIFACT_MAGIC_BOOK
                                && emptySlots > iMaxTransferArtifacts) {
                                iTransferArtifacts[iMaxTransferArtifacts] =
                                    m_heroes[IDX(OppositeCombatResult(winningSide))]
                                        ->m_artifacts[fadeIndex26];
                                iTransferArtifactsInfo[iMaxTransferArtifacts] =
                                    m_heroes[IDX(OppositeCombatResult(winningSide))]
                                        ->m_artifactExtra[fadeIndex26];
                                ++iMaxTransferArtifacts;
                            }
                        }
                    }
                }
            }

            if (!(giCurPlayer == -1 || gbThisNetHumanPlayer[giCurPlayer] == 0
                  || m_playerId[IDX(winningSide)] != giCurPlayer)
                || !(
                    giCurPlayer == -1 || m_playerId[IDX(winningSide)] == -1
                    || gbThisNetHumanPlayer[giCurPlayer] != 0
                    || gbThisNetHumanPlayer[m_playerId[IDX(winningSide)]] == 0
                )
                || !(
                    m_playerId[IDX(winningSide)] == -1
                    || gbThisNetHumanPlayer[m_playerId[IDX(winningSide)]] == 0
                )) {
                gpSoundManager->SwitchAmbientMusic(VICTORY_MUSIC);
                m_winLoseWindow = new heroWindow(WIN_LOSE_WINDOW_X, WIN_LOSE_WINDOW_Y, DATA_COMPGEN(0x004f0dec, doVictoryWincmbtBin, "wincmbt.bin"));
                if (m_winLoseWindow == NULL)
                    MemError();

                if (m_heroes[IDX(winningSide)] != NULL) {
                    if (gbCombatSurrender != 0) {
                        sprintf(gText, cBattleResults[IDX(RESULT_TEXT_ENEMY_SURRENDERED)]);
                    } else if (gbRetreatWin != 0) {
                        sprintf(gText, cBattleResults[IDX(RESULT_TEXT_ENEMY_FLED)]);
                    } else {
                        sprintf(gText, cBattleResults[IDX(RESULT_TEXT_VICTORY)]);
                    }
                    if (victoryLevels > 0 && winningSide == COMBAT_RESULT_DEFENDER
                        && giNumHumanPlayers > 1) {
                        sprintf(
                            experienceText,
                            cBattleResults[IDX(RESULT_TEXT_NETWORK_EXPERIENCE)],
                            m_heroes[IDX(winningSide)]->m_name,
                            m_experienceValue[IDX(OppositeCombatResult(winningSide))],
                            victoryLevels
                        );
                    } else {
                        sprintf(
                            experienceText,
                            cBattleResults[IDX(RESULT_TEXT_EXPERIENCE)],
                            m_heroes[IDX(winningSide)]->m_name,
                            m_experienceValue[IDX(OppositeCombatResult(winningSide))]
                        );
                    }
                    strcat(gText, experienceText);
                    m_heroes[IDX(winningSide)]->ApplyBattleWinTemps();
                } else {
                    if (gbCombatSurrender != 0) {
                        sprintf(gText, cBattleResults[IDX(RESULT_TEXT_ENEMY_SURRENDERED)]);
                    } else if (gbRetreatWin != 0) {
                        sprintf(gText, cBattleResults[IDX(RESULT_TEXT_ENEMY_FLED)]);
                    } else {
                        sprintf(gText, cBattleResults[IDX(RESULT_TEXT_VICTORY)]);
                    }
                }
                message.type = MESSAGE_WIDGET;
                message.payload.widget.command = COMBAT_WIN_LOSE_TEXT_COMMAND;
                message.payload.widget.id = WIN_LOSE_TEXT_ID;
                message.payload.widget.data.text = gText;
                m_winLoseWindow->BroadcastMessage(message);
                ShowDeadArmies(m_winLoseWindow);
                if (gbRemoteOn != 0 && gbThisNetGotAdventureControl == 0)
                    giDialogTimeout = KBTickCount() + WIN_LOSE_DIALOG_TIMEOUT;
                gpWindowManager->DoDialog(m_winLoseWindow, WinCombatHandler, 0);
                giDialogTimeout = 0;
                delete m_winLoseWindow;
                if (m_heroes[IDX(OppositeCombatResult(winningSide))] != NULL)
                    m_heroes[IDX(OppositeCombatResult(winningSide))]->ApplyBattleLossTemps();
            } else {
                if (m_heroes[IDX(winningSide)] != NULL)
                    m_heroes[IDX(winningSide)]->ApplyBattleWinTemps();
                if (m_heroes[IDX(OppositeCombatResult(winningSide))] != NULL)
                    m_heroes[IDX(OppositeCombatResult(winningSide))]->ApplyBattleLossTemps();
                gpSoundManager->SwitchAmbientMusic(LOSS_MUSIC);
                DoLoseWindow();
            }
            break;
    }
    gMapX = gpAdvManager->m_mapOriginX;
    gMapY = gpAdvManager->m_mapOriginY;
}

VA(0x0042f834, 0x3bc)
void combatManager::DoLoseWindow(void) {
    i32 unusedLoseWord_h = IDX(COMBAT_RESULT_ATTACKER);
    CombatResult losingSide_h;
    char animationFile_j[WIN_LOSE_ANIMATION_FILENAME_SIZE];
    tag_message message;

    if (m_playerId[IDX(COMBAT_ATTACKER_SIDE)] == giCurPlayer
        && gbThisNetHumanPlayer[m_playerId[IDX(COMBAT_ATTACKER_SIDE)]] != 0) {
        losingSide_h = COMBAT_RESULT_ATTACKER;
    } else if (m_playerId[IDX(COMBAT_DEFENDER_SIDE)] == giCurPlayer
               && gbThisNetHumanPlayer[m_playerId[IDX(COMBAT_DEFENDER_SIDE)]] != 0) {
        losingSide_h = COMBAT_RESULT_DEFENDER;
    } else if (m_playerId[IDX(COMBAT_ATTACKER_SIDE)] != -1
               && gbThisNetHumanPlayer[m_playerId[IDX(COMBAT_ATTACKER_SIDE)]] != 0) {
        losingSide_h = COMBAT_RESULT_ATTACKER;
    } else {
        losingSide_h = COMBAT_RESULT_DEFENDER;
    }

    gbShowingLoseWindow = true;
    if (gbCombatSurrender != 0) {
        sprintf(animationFile_j, DATA_COMPGEN(0x004f0df8, doLoseWindowCmbtsurrIcn, "cmbtsurr.icn"));
        gbWhichAnimationPlaying = WIN_LOSE_ANIMATION_CYCLE_SECOND;
    } else if (gbRetreatWin != 0) {
        sprintf(animationFile_j, DATA_COMPGEN(0x004f0e08, doLoseWindowCmbtfle1Icn, "cmbtfle1.icn"));
        gbWhichAnimationPlaying = WIN_LOSE_ANIMATION_FLEE;
    } else {
        sprintf(animationFile_j, DATA_COMPGEN(0x004f0e18, doLoseWindowCmbtlos1Icn, "cmbtlos1.icn"));
        gbWhichAnimationPlaying = WIN_LOSE_ANIMATION_LOSS;
    }

    m_winLoseWindow = new heroWindow(WIN_LOSE_WINDOW_X, WIN_LOSE_WINDOW_Y, DATA_COMPGEN(0x004f0e28, doLoseWindowWincmbtBin, "wincmbt.bin"));
    if (m_winLoseWindow == NULL)
        MemError();

    if (m_heroes[IDX(losingSide_h)] != NULL) {
        if (gbCombatSurrender != 0) {
            sprintf(
                gText,
                cBattleResults[IDX(RESULT_TEXT_HERO_SURRENDER)],
                m_heroes[IDX(losingSide_h)]->m_name
            );
        } else if (gbRetreatWin != 0) {
            sprintf(
                gText,
                cBattleResults[IDX(RESULT_TEXT_HERO_FLEE)],
                m_heroes[IDX(losingSide_h)]->m_name
            );
        } else {
            sprintf(
                gText,
                cBattleResults[IDX(RESULT_TEXT_HERO_DEFEAT)],
                m_heroes[IDX(losingSide_h)]->m_name
            );
        }
    } else {
        if (gbCombatSurrender != 0) {
            sprintf(gText, cBattleResults[IDX(RESULT_TEXT_FORCES_SURRENDER)]);
        } else if (gbRetreatWin != 0) {
            sprintf(gText, cBattleResults[IDX(RESULT_TEXT_FORCES_FLEE)]);
        } else {
            sprintf(gText, cBattleResults[IDX(RESULT_TEXT_FORCES_DEFEAT)]);
        }
    }

    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = COMBAT_WIN_LOSE_RESOURCE_COMMAND;
    message.payload.widget.id = WIN_LOSE_RESOURCE_LOAD_ID;
    message.payload.widget.data.text = animationFile_j;
    m_winLoseWindow->BroadcastMessage(message);
    message.payload.widget.id = WIN_LOSE_RESOURCE_DRAW_ID;
    message.payload.widget.data.text = animationFile_j;
    m_winLoseWindow->BroadcastMessage(message);
    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = COMBAT_WIN_LOSE_TEXT_COMMAND;
    message.payload.widget.id = WIN_LOSE_TEXT_ID;
    message.payload.widget.data.text = gText;
    m_winLoseWindow->BroadcastMessage(message);
    ShowDeadArmies(m_winLoseWindow);
    if (gbRemoteOn != 0 && gbThisNetGotAdventureControl == 0)
        giDialogTimeout = KBTickCount() + WIN_LOSE_DIALOG_TIMEOUT;
    gpWindowManager->DoDialog(m_winLoseWindow, WinCombatHandler, 0);
    giDialogTimeout = 0;
    delete m_winLoseWindow;
    m_winLoseWindow = NULL;
}

VA(0x0042fbf0, 0x43d)
i32 combatManager::DoSurrender(void) {
    i32 armyIndex_n;
    i16 dialogType;
    i16 dialogResult;
    i16 textWidth_t;
    heroWindow* window;
    tag_message message;

    giSurrenderCost = 0;
    for (armyIndex_n = 0; armyIndex_n < COMBAT_ARMY_SLOT_COUNT; ++armyIndex_n) {
        if (m_armies[IDX(m_currentSide)][armyIndex_n].IsAlive()) {
            giSurrenderCost +=
                gMonsterDatabase[IDX(m_armies[IDX(m_currentSide)][armyIndex_n].m_monsterType)].cost
                * m_armies[IDX(m_currentSide)][armyIndex_n].m_quantity;
        }
    }
    if (m_heroes[IDX(m_currentSide)]->HasArtifact(ARTIFACT_STATESMANS_QUILL) != 0)
        giSurrenderCost = static_cast<i32>(giSurrenderCost * COMBAT_SURRENDER_QUILL_FACTOR);
    else
        giSurrenderCost = static_cast<i32>(giSurrenderCost * COMBAT_SURRENDER_BASE_FACTOR);
    giSurrenderCost = static_cast<i32>(
        giSurrenderCost
        * (DATA_COMPGEN(0x004eb250, doSurrenderConstant, 1.0)
           - IDX(m_heroes[IDX(m_currentSide)]->m_secondarySkills[IDX(HERO_SKILL_DIPLOMACY)])
                 * COMBAT_SURRENDER_DIPLOMACY_FACTOR)
    );

    dialogType = SURRENDER_DIALOG_TYPE;
    dialogResult = SURRENDER_DIALOG_ACCEPT_RESULT;
    textWidth_t = SURRENDER_TEXT_WIDTH;
    window = new heroWindow(SURRENDER_WINDOW_X, SURRENDER_WINDOW_Y, DATA_COMPGEN(0x004f0e34, doSurrenderSurrendrBin, "surrendr.bin"));
    if (window == NULL)
        MemError();
    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = COMBAT_WIN_LOSE_RESOURCE_COMMAND;
    message.payload.widget.id = SURRENDER_PORTRAIT_RESOURCE_ID;
    sprintf(
        gText,
        DATA_COMPGEN(0x004f0e44, doSurrenderPort04dIcn, "port%04d.icn"),
        static_cast<i32>(m_heroes[IDX(OppositeCombatSide(m_currentSide))]->m_portrait)
    );
    message.payload.widget.data.text = gText;
    window->BroadcastMessage(message);
    if (m_heroes[IDX(OppositeCombatSide(m_currentSide))]->m_isCaptain != 0)
        message.payload.widget.command = COMBAT_SURRENDER_CAPTAIN_PORTRAIT_COMMAND;
    else
        message.payload.widget.command = COMBAT_SURRENDER_HERO_PORTRAIT_COMMAND;
    message.payload.widget.id = SURRENDER_PORTRAIT_WIDGET_ID;
    message.payload.widget.data.value = SURRENDER_PORTRAIT_DEFAULT_COLOR;
    window->BroadcastMessage(message);
    if (m_heroes[IDX(OppositeCombatSide(m_currentSide))]->m_isCaptain != 0) {
        message.payload.widget.command = COMBAT_SURRENDER_CAPTAIN_OVERLAY_COMMAND;
        if (m_playerId[IDX(OppositeCombatSide(m_currentSide))] == -1)
            message.payload.widget.data.value = COMBAT_NEUTRAL_HERO_COLOR;
        else
            message.payload.widget.data.value =
                gpGame->m_players[m_playerId[IDX(OppositeCombatSide(m_currentSide))]].m_color;
        window->BroadcastMessage(message);
    }
    message.payload.widget.data.text = gText;
    message.payload.widget.command = COMBAT_WIN_LOSE_TEXT_COMMAND;
    message.payload.widget.id = SURRENDER_TEXT_ID;
    sprintf(
        gText,
        DATA_COMPGEN(0x004f0e54, doSurrenderSStatesIWillAcceptYour, "%s states:\n\n\"I will accept your surrender and grant you and your troops safe passage "
        "for the price of %d gold.\""),
        m_heroes[IDX(OppositeCombatSide(m_currentSide))]->m_name,
        giSurrenderCost
    );
    window->BroadcastMessage(message);
    gpWindowManager->DoDialog(window, TrueFalseDialogHandler, 0);
    delete window;
    return gpWindowManager->m_dialogResult == NORMAL_DIALOG_BUTTON_TWO;
}

VA(0x0043002d, 0xdc)
void combatManager::CheckChangeSelector(void) {
    if (gbNoShowCombat != 0)
        return;
    army* currentArmy = &m_armies[IDX(m_currentArmySide)][m_currentArmyIndex];
    if (m_limitCreature == 0 || currentArmy->m_hex != m_limitCreatureHex) {
        UpdateGrid(0, 1);
        giNewMonsterCycleFrame = SELECTOR_INITIAL_FRAME;
        m_limitCreatureHex = currentArmy->m_hex;
        m_limitCreature = 1;
        DrawFrame(1, 0, 0, 0, COMMAND_FRAME_DELAY, 1, 1);
    }
    SetupSmallView();
}

VA(0x00430109, 0xea)
void combatManager::CheckCastleAttack(void) {
    if (m_inCastleCombat != 0 && m_currentSide == COMBAT_ATTACKER_SIDE) {
        while (m_catapultAttacksRemaining[IDX(m_currentSide)] > 0) {
            CatAttack(m_currentSide);
            --m_catapultAttacksRemaining[IDX(m_currentSide)];
        }
    }
    if (m_inCastleCombat != 0 && m_currentSide == COMBAT_DEFENDER_SIDE) {
        while (m_keepAttacksRemaining[IDX(m_currentSide)] > 0) {
            KeepAttack(COMBAT_TOWER_GARRISON);
            KeepAttack(COMBAT_TOWER_TOP);
            KeepAttack(COMBAT_TOWER_BOTTOM);
            --m_keepAttacksRemaining[IDX(m_currentSide)];
        }
    }
}

VA(0x004301f3, 0xdd)
void combatManager::CheckGetAIMove(void) {
    i32 retreat = AICheckRetreat();
    if (m_heroCastSpell[IDX(m_currentSide)] == 0
        && (m_playerId[IDX(m_currentSide)] == -1 || gbThisNetHumanPlayer[m_playerId[IDX(m_currentSide)]] == 0
            || gConfig.autoCombatUseSpells != 0)) {
        if (DoSpellAI(m_currentSide, retreat) != 0)
            return;
    }
    retreat = AICheckRetreat();
    if (retreat != 0) {
        giNextAction = ACTION_RETREAT;
        return;
    }
    DoCompAI(m_currentSide);
}

VA(0x004302d0, 0x18f)
void combatManager::GetControl(void) {
    m_selectedHex = INVALID_HEX;
    m_previousCommand = COMBAT_INVALID_COMMAND;
    m_previousCommand = COMBAT_INVALID_COMMAND;
    if (gpCombatManager->m_active == 1)
        gpMouseManager->SetPointer(COMBAT_POINTER_DEFAULT);
    CheckChangeSelector();
    if (gbRemoteOn == 0 || m_playerId[IDX(COMBAT_ATTACKER_SIDE)] < 0
        || m_playerId[IDX(COMBAT_DEFENDER_SIDE)] < 0
        || gbHumanPlayer[m_playerId[IDX(COMBAT_DEFENDER_SIDE)]] == 0
        || (gbHumanPlayer[m_playerId[IDX(COMBAT_ATTACKER_SIDE)]] == 0
            && (gbHumanPlayer[m_playerId[IDX(COMBAT_ATTACKER_SIDE)]] != 0
                || m_playerId[IDX(COMBAT_DEFENDER_SIDE)] == 0))) {
        gbThisNetHasControl = true;
        goto setup_view;
    }
    if (m_playerId[IDX(m_currentSide)] != -1 && gbHumanPlayer[m_playerId[IDX(m_currentSide)]] != 0
        && gbThisNetHumanPlayer[m_playerId[IDX(m_currentSide)]] == 0)
        gbThisNetHasControl = false;
    else
        gbThisNetHasControl = true;

setup_view:
    m_smallViewSide[IDX(COMBAT_DEFENDER_SIDE)] = COMBAT_SIDE_NONE;
    SetupSmallView();
    ResetMouse();
}

VA(0x0043045f, 0xd7)
void combatManager::ResetMouse(void) {
    i32 mouseY_g;
    i32 mouseX_f;
    tag_message message;

    if (gbNoShowCombat != 0)
        return;
    if (gbThisNetHasControl != 0 && m_playerId[IDX(m_currentSide)] >= 0
        && gbHumanPlayer[m_playerId[IDX(m_currentSide)]] != 0) {
        m_selectedHex = INVALID_HEX;
        ClearCombatMessages(0);
        gpMouseManager->MouseCoords(mouseX_f, mouseY_g);
        message.type = MESSAGE_MOUSE_MOVE;
        message.payload.mouse.x = message.payload.mouse.screenX = mouseX_f;
        message.payload.mouse.y = message.payload.mouse.screenY = mouseY_g;
        ProcessCombatMsg(message);
    } else {
        gpMouseManager->SetPointer(COMBAT_POINTER_DEFAULT);
    }
}

VA(0x00430536, 0x65b)
MessageDispatchResult combatManager::ProcessNextAction(struct tag_message& message) {
    i32 actionData[IDX(ACTION_DATA_COUNT)];
    i32 transmitResult;
    army* actingArmy;
    i32 shouldAdvance;
    i32 redraw;
    MessageDispatchResult dispatchResult;

    ClearCombatMessages(0);
    dispatchResult = MESSAGE_DISPATCH_CONSUME;
    redraw = 0;
    gbProcessingCombatAction = true;
    if (m_smallViewSide[IDX(COMBAT_ATTACKER_SIDE)] != COMBAT_SIDE_NONE
        || m_smallViewSide[IDX(COMBAT_DEFENDER_SIDE)] != COMBAT_SIDE_NONE) {
        m_smallViewSide[IDX(COMBAT_DEFENDER_SIDE)] = COMBAT_SIDE_NONE;
        m_smallViewSide[IDX(COMBAT_ATTACKER_SIDE)] = m_smallViewSide[IDX(COMBAT_DEFENDER_SIDE)];
        redraw = 1;
    }
    if (giNextAction != ACTION_NONE) {
        LogInt(
            DATA_COMPGEN(0x004f0ec4, processNextActionProcessAct, "Process Act"),
            IDX(giNextAction),
            giNextActionGridIndex,
            giNextActionGridIndex2,
            giNextActionExtra,
            IDX(m_currentArmySide),
            m_currentArmyIndex,
            m_armies[IDX(m_currentArmySide)][m_currentArmyIndex].m_hex
        );
    }
    gpMouseManager->SetPointer(COMBAT_POINTER_DEFAULT);
    UpdateMouseGrid(-1, 1);
    memset(m_gridState, IDX(GRID_SHADE_NONE), sizeof(m_gridState));
    if (UpdateGrid(0, 0) != 0)
        redraw = 1;
    if (redraw != 0)
        DrawFrame(1, 0, 0, 0, COMMAND_FRAME_DELAY, 1, 1);

    if (gbThisNetHasControl != 0 && gbRemoteOn != 0 && m_playerId[IDX(COMBAT_ATTACKER_SIDE)] >= 0
        && m_playerId[IDX(COMBAT_DEFENDER_SIDE)] >= 0
        && gbHumanPlayer[m_playerId[IDX(COMBAT_DEFENDER_SIDE)]] != 0
        && gbHumanPlayer[m_playerId[IDX(COMBAT_ATTACKER_SIDE)]] != 0) {
        actionData[IDX(ACTION_DATA_ACTION)] = IDX(giNextAction);
        actionData[IDX(ACTION_DATA_EXTRA)] = giNextActionExtra;
        actionData[IDX(ACTION_DATA_GRID)] = giNextActionGridIndex;
        actionData[IDX(ACTION_DATA_SECOND_GRID)] = giNextActionGridIndex2;
        LogInt(
            DATA_COMPGEN(0x004f0ed0, processNextActionAboutToT, "About to T"),
            iCombatControlNetPos[IDX(COMBAT_DEFENDER_SIDE) - IDX(m_currentSide)],
            LOG_UNUSED_VALUE,
            LOG_UNUSED_VALUE,
            LOG_UNUSED_VALUE,
            LOG_UNUSED_VALUE,
            LOG_UNUSED_VALUE,
            LOG_UNUSED_VALUE
        );
        transmitResult = TransmitRemoteData(
            reinterpret_cast<char*>(actionData),
            iCombatControlNetPos[IDX(COMBAT_DEFENDER_SIDE) - IDX(m_currentSide)],
            sizeof(actionData),
            REMOTE_COMMAND_ACTION,
            1,
            1,
            REMOTE_MESSAGE_DEFAULT
        );
        LogStr(DATA_COMPGEN(0x004f0edc, processNextActionPostT, "Post T"));
        if (transmitResult == 0)
            ShutDown(NULL);
    }

    actingArmy = &m_armies[IDX(m_currentArmySide)][m_currentArmyIndex];
    shouldAdvance = 0;
    if (CheckWin(&message) != 0)
        goto Finished;
    switch (giNextAction) {
        case ACTION_NONE:
            break;
        case ACTION_CAST_SPELL:
            ResetCyclingCreatures();
            CastSpell(
                SpellType(giNextActionExtra),
                giNextActionGridIndex,
                0,
                giNextActionGridIndex2
            );
            if (m_armies[IDX(m_currentArmySide)][m_currentArmyIndex].m_quantity <= 0)
                shouldAdvance = 1;
            ResetCycleTimers();
            break;
        case ACTION_MOVE:
            ResetCyclingCreatures();
            actingArmy->MoveAttack(giNextActionGridIndex, 0);
            actingArmy->m_monster.flags.abilityFlags |= MONSTER_ABILITY_FLAG_BAD_MORALE;
            if (CheckWin(&message) != 0) {
                dispatchResult = MESSAGE_DISPATCH_FORWARD;
                goto Finished;
            }
            CheckApplyGoodMorale(m_currentArmySide, m_currentArmyIndex);
            shouldAdvance = 1;
            ResetCycleTimers();
            break;
        case ACTION_ATTACK:
            ResetCyclingCreatures();
            if (giNextActionExtra != -1 && actingArmy->m_hex != giNextActionExtra) {
                actingArmy->MoveAttack(giNextActionExtra, 1);
            }
            actingArmy->MoveAttack(giNextActionGridIndex, 0);
            actingArmy->m_monster.flags.abilityFlags |= MONSTER_ABILITY_FLAG_BAD_MORALE;
            if (CheckWin(&message) != 0) {
                dispatchResult = MESSAGE_DISPATCH_FORWARD;
                goto Finished;
            }
            CheckApplyGoodMorale(m_currentArmySide, m_currentArmyIndex);
            shouldAdvance = 1;
            ResetCycleTimers();
            break;
        case ACTION_RETREAT:
            m_sideRetreated[IDX(m_currentSide)] = 1;
            gbRetreatWin = true;
            ResetCycleTimers();
            break;
        case ACTION_SURRENDER:
            gbCombatSurrender = true;
            gbRetreatWin = true;
            m_sideDefeated[IDX(m_currentSide)] = 1;
            gpGame->m_players[m_playerId[IDX(m_currentSide)]].m_resources[IDX(RES_GOLD)] -=
                giNextActionExtra;
            gpGame->m_players[m_playerId[IDX(COMBAT_DEFENDER_SIDE) - IDX(m_currentSide)]]
                .m_resources[IDX(RES_GOLD)] += giNextActionExtra;
            ResetCycleTimers();
            break;
        case ACTION_WAIT:
            actingArmy->m_monster.flags.abilityFlags |= MONSTER_ABILITY_FLAG_BAD_MORALE;
            shouldAdvance = 1;
            break;
        case ACTION_DEFEND:
            actingArmy->m_monster.flags.abilityFlags |= MONSTER_ABILITY_FLAG_DEFERRED_TURN;
            shouldAdvance = 1;
            break;
    }
    giNextAction = ACTION_NONE;
    if (CheckWin(&message) != 0) {
        dispatchResult = MESSAGE_DISPATCH_FORWARD;
        goto Finished;
    }
    TestRaiseDoor();
    if (shouldAdvance != 0 && GetNextArmy(1) == 0) {
        ResetRound();
        GetNextArmy(1);
    }
    CheckChangeSelector();

Finished:
    gbProcessingCombatAction = false;
    ResetMouse();
    return dispatchResult;
}

VA(0x00430b91, 0x237)
void combatManager::ResetCyclingCreatures(void) {
    army* currentArmy_p = NULL;
    i32 cyclingCount = 0;
    CombatSide side;
    i32 index;
    i32 unusedCyclingWord6;

    for (side = COMBAT_ATTACKER_SIDE; IDX(side) < COMBAT_SIDE_COUNT; ++side) {
        for (index = 0; index < gpCombatManager->m_armyCount[IDX(side)]; ++index) {
            currentArmy_p = &gpCombatManager->m_armies[IDX(side)][index];
            if (HAS(currentArmy_p->m_monster.flags.abilityFlags, MONSTER_ABILITY_FLAG_AI_EXCLUDED)
                    == 0
                && currentArmy_p->m_animationSequence >= COMBAT_CREATURE_CYCLE_SEQUENCE_FIRST
                && currentArmy_p->m_animationSequence <= COMBAT_CREATURE_CYCLE_SEQUENCE_LAST) {
                ++cyclingCount;
                ++gpCombatManager->m_limitCreatureCount[IDX(side)][index];
            }
        }
    }
    if (cyclingCount == 0)
        return;

    gpCombatManager->DrawFrame(0, 1, 1, 1, COMMAND_FRAME_DELAY, 1, 1);
    for (side = COMBAT_ATTACKER_SIDE; IDX(side) < COMBAT_SIDE_COUNT; ++side) {
        for (index = 0; index < gpCombatManager->m_armyCount[IDX(side)]; ++index) {
            currentArmy_p = &gpCombatManager->m_armies[IDX(side)][index];
            if (HAS(currentArmy_p->m_monster.flags.abilityFlags,
                    MONSTER_ABILITY_FLAG_AI_EXCLUDED)
                == 0) {
                currentArmy_p = &gpCombatManager->m_armies[IDX(side)][index];
                currentArmy_p->m_animationSequence = ARMY_ANIMATION_STAND;
                currentArmy_p->m_animationFrame = 0;
                currentArmy_p->m_lastAnimationTime = KBTickCount();
            }
        }
    }
    m_heroCycleTimer[IDX(COMBAT_ATTACKER_SIDE)] = KBTickCount();
    m_heroCycleTimer[IDX(COMBAT_DEFENDER_SIDE)] = KBTickCount();
    gpCombatManager->DrawFrame(1, 1, 0, 0, COMMAND_FRAME_DELAY, 1, 1);
}

VA(0x00430dc8, 0xf9)
void combatManager::ResetCycleTimers(void) {
    i32l currentTime_r = KBTickCount();
    CombatSide side;
    i32 index;
    army* currentArmy_p;

    m_heroCycleTimer[IDX(COMBAT_ATTACKER_SIDE)] = KBTickCount();
    m_heroCycleTimer[IDX(COMBAT_DEFENDER_SIDE)] = KBTickCount();
    for (side = COMBAT_ATTACKER_SIDE; IDX(side) < COMBAT_SIDE_COUNT; ++side) {
        for (index = 0; index < gpCombatManager->m_armyCount[IDX(side)]; ++index) {
            currentArmy_p = &gpCombatManager->m_armies[IDX(side)][index];
            currentArmy_p->m_lastAnimationTime = currentTime_r;
            if (currentArmy_p->m_frameInfo.standStillDelay > STAND_DELAY_RANDOM_THRESHOLD) {
                currentArmy_p->m_lastAnimationTime -= Random(
                    STAND_DELAY_RANDOM_MIN,
                    currentArmy_p->m_frameInfo.standStillDelay
                );
            }
        }
    }
}

VA(0x00430ec1, 0x53)
i32 InCombatArea(i32 x, i32 y) {
    return x >= 0 && x < COMBAT_SCREEN_WIDTH && y >= 0 && y < COMBAT_AREA_HEIGHT;
}

VA(0x00430f14, 0x9d9)
void combatManager::CycleCombatScreen(void) {
    army* currentArmy;
    u8 cycleArmy[COMBAT_SIDE_COUNT][COMBAT_ARMY_SLOT_COUNT];
    i32 nextHeroAnimation[COMBAT_HERO_ANIMATION_TRACK_COUNT];
    CombatSide side;
    i32 index;
    i32 animationIndex;
    float roll;
    float accumulatedChance;

    CheckUpdateCombatMessages();
    currentArmy = NULL;
    nextHeroAnimation[COMBAT_SIDE_COUNT] = 0;
    gpCombatManager->ResetLimitCreature();
    for (side = COMBAT_ATTACKER_SIDE; IDX(side) < COMBAT_SIDE_COUNT; ++side) {
        if (m_heroOverlayIcons[IDX(side)] == NULL) {
        } else {
            if (m_heroes[IDX(side)] != NULL)
                m_heroOverlayFrame[IDX(side)] = (m_heroOverlayFrame[IDX(side)] + 1) % COMBAT_HERO_OVERLAY_FRAME_COUNT;
            ++m_drawHeroOverlay[IDX(side)];
        }
    }

    memset(cycleArmy, 0, sizeof(cycleArmy));
    for (side = COMBAT_ATTACKER_SIDE; IDX(side) < COMBAT_SIDE_COUNT; ++side) {
        for (index = 0; index < gpCombatManager->m_armyCount[IDX(side)]; ++index) {
            currentArmy = gpCombatManager->m_armies[IDX(side)] + index;
            if (HAS(currentArmy->m_monster.flags.abilityFlags, MONSTER_ABILITY_FLAG_AI_EXCLUDED)
                    == 0
                && currentArmy->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_PARALYZE)] == 0
                && currentArmy->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_BLIND)] == 0
                && currentArmy->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_PETRIFIED)] == 0
                && ((currentArmy->m_animationSequence >= COMBAT_CREATURE_CYCLE_SEQUENCE_FIRST
                     && currentArmy->m_animationSequence <= COMBAT_CREATURE_CYCLE_SEQUENCE_LAST)
                    || (currentArmy->m_animationSequence == ARMY_ANIMATION_STAND
                        && currentArmy->m_frameInfo.standStillDelay
                                   + currentArmy->m_lastAnimationTime
                               < KBTickCount()))) {
                ++nextHeroAnimation[COMBAT_SIDE_COUNT];
                ++cycleArmy[IDX(side)][index];
                ++m_limitCreatureCount[IDX(side)][index];
            }
        }
    }

    for (side = COMBAT_ATTACKER_SIDE; IDX(side) < COMBAT_SIDE_COUNT; ++side) {
        nextHeroAnimation[IDX(side)] = -1;
        if (m_heroIcons[IDX(side)] == NULL) {
        } else {
            if (m_heroAnimationState[IDX(side)] == HERO_ANIMATION_DEATH_FIRST
                || m_heroAnimationState[IDX(side)] == HERO_ANIMATION_DEATH_SECOND
                || m_heroAnimationState[IDX(side)] == HERO_ANIMATION_IDLE_FIRST
                || m_heroAnimationState[IDX(side)] == HERO_ANIMATION_IDLE_SECOND
                || m_heroAnimationState[IDX(side)] == HERO_ANIMATION_IDLE_LAST) {
                m_drawHero[IDX(side)] = 1;
            } else if (m_heroAnimationState[IDX(side)] == HERO_ANIMATION_STAND
                       && m_heroDeathAnimationPlayed[IDX(side)] == 0 && m_heroDeathPending[IDX(side)] != 0) {
                if (m_playerId[IDX(m_currentSide)] == -1
                    || gbThisNetHumanPlayer[m_playerId[IDX(m_currentSide)]] == 0) {
                    m_heroAlternateDeathPending[IDX(side)] = 0;
                    m_heroDeathPending[IDX(side)] = m_heroAlternateDeathPending[IDX(side)];
                } else {
                    m_heroAlternateDeathPending[IDX(side)] = 0;
                    m_heroDeathPending[IDX(side)] = m_heroAlternateDeathPending[IDX(side)];
                    m_heroDeathAnimationPlayed[IDX(side)] = 1;
                    if (sCmbtHero[m_heroSpriteIndex[IDX(side)]]
                            .animationFrameCount[HERO_ANIMATION_DEATH_FIRST]
                        > 0) {
                        nextHeroAnimation[IDX(side)] = HERO_ANIMATION_DEATH_FIRST;
                        m_drawHero[IDX(side)] = 1;
                    }
                }
            } else if (m_heroAnimationState[IDX(side)] == HERO_ANIMATION_STAND
                       && m_heroAlternateDeathAnimationPlayed[IDX(side)] == 0
                       && m_heroAlternateDeathPending[IDX(side)] != 0) {
                if (m_playerId[IDX(m_currentSide)] == -1
                    || gbThisNetHumanPlayer[m_playerId[IDX(m_currentSide)]] == 0) {
                    m_heroAlternateDeathPending[IDX(side)] = 0;
                    m_heroDeathPending[IDX(side)] = m_heroAlternateDeathPending[IDX(side)];
                } else {
                    m_heroAlternateDeathPending[IDX(side)] = 0;
                    m_heroDeathPending[IDX(side)] = m_heroAlternateDeathPending[IDX(side)];
                    m_heroAlternateDeathAnimationPlayed[IDX(side)] = 1;
                    if (sCmbtHero[m_heroSpriteIndex[IDX(side)]]
                            .animationFrameCount[HERO_ANIMATION_DEATH_SECOND]
                        > 0) {
                        nextHeroAnimation[IDX(side)] = HERO_ANIMATION_DEATH_SECOND;
                        m_drawHero[IDX(side)] = 1;
                    }
                }
            } else if (m_heroAnimationState[IDX(side)] == HERO_ANIMATION_STAND
                       && m_heroCycleTimer[IDX(side)] + HERO_IDLE_DELAY < KBTickCount()) {
                if (sCmbtHero[m_heroSpriteIndex[IDX(side)]].idleAnimationCount > 1) {
                    nextHeroAnimation[IDX(side)] =
                        Random(
                            0,
                            sCmbtHero[m_heroSpriteIndex[IDX(side)]].idleAnimationCount
                                - 1
                        )
                        + HERO_ANIMATION_IDLE_FIRST;
                } else {
                    nextHeroAnimation[IDX(side)] = HERO_ANIMATION_IDLE_FIRST;
                }
                m_drawHero[IDX(side)] = 1;
            }
        }
    }
    if (m_heroIcons[IDX(COMBAT_ATTACKER_SIDE)] != NULL)
        m_drawHero[IDX(COMBAT_ATTACKER_SIDE)] = 1;
    if (m_heroIcons[IDX(COMBAT_DEFENDER_SIDE)] != NULL)
        m_drawHero[IDX(COMBAT_DEFENDER_SIDE)] = 1;

    if (nextHeroAnimation[COMBAT_SIDE_COUNT] == 0 && m_drawHero[IDX(COMBAT_ATTACKER_SIDE)] == 0
        && m_drawHero[IDX(COMBAT_DEFENDER_SIDE)] == 0
        && m_drawHeroOverlay[IDX(COMBAT_ATTACKER_SIDE)] == 0
        && m_drawHeroOverlay[IDX(COMBAT_DEFENDER_SIDE)] == 0)
        goto setCycleTimer;
    gpCombatManager->DrawFrame(0, 1, 1, 1, COMMAND_FRAME_DELAY, 1, 1);
    for (side = COMBAT_ATTACKER_SIDE; IDX(side) < COMBAT_SIDE_COUNT; ++side) {
        for (index = 0; index < gpCombatManager->m_armyCount[IDX(side)]; ++index) {
            currentArmy = gpCombatManager->m_armies[IDX(side)] + index;
            if (cycleArmy[IDX(side)][index] != 0) {
                if (currentArmy->m_animationSequence == ARMY_ANIMATION_STAND) {
                    roll =
                        static_cast<float>(Random(IDLE_ROLL_MIN, IDLE_ROLL_MAX))
                        / COMBAT_IDLE_ROLL_DIVISOR;
                    accumulatedChance = DATA_COMPGEN(
                        0x004eb25c, standingAnimationInitialChance, 0.0f
                    );
                    currentArmy->m_standingAnimation =
                        currentArmy->m_frameInfo.standingAnimationCount - 1;
                    for (animationIndex = 0;
                         animationIndex < currentArmy->m_frameInfo.standingAnimationCount - 1;
                         ++animationIndex) {
                        accumulatedChance +=
                            currentArmy->m_frameInfo.standingAnimationChances[animationIndex];
                        if (accumulatedChance > roll) {
                            currentArmy->m_standingAnimation = animationIndex;
                            animationIndex = STANDING_ANIMATION_SEARCH_DONE;
                        }
                    }
                    currentArmy->m_animationSequence = ArmyAnimationSequence(
                        currentArmy->m_standingAnimation
                        + IDX(COMBAT_CREATURE_CYCLE_SEQUENCE_FIRST)
                    );
                    currentArmy->m_animationFrame = 0;
                } else {
                    ++currentArmy->m_animationFrame;
                    if (currentArmy->m_frameInfo.standStillDelay == 0
                        && currentArmy->m_frameInfo.standingAnimationCount == 1
                        && Random(0, IDLE_ROLL_MAX) < IDLE_REPEAT_CHANCE) {
                        --currentArmy->m_animationFrame;
                    }
                    if (currentArmy->m_animationFrame
                        >= currentArmy->m_frameInfo.animationFrameCount
                               [currentArmy->m_standingAnimation
                                + IDX(COMBAT_CREATURE_CYCLE_SEQUENCE_FIRST)]) {
                        currentArmy->m_animationSequence = ARMY_ANIMATION_STAND;
                        currentArmy->m_animationFrame = 0;
                        currentArmy->m_lastAnimationTime = KBTickCount();
                        if (currentArmy->m_frameInfo.standStillDelay > 0) {
                            currentArmy->m_lastAnimationTime = static_cast<i32>(
                                (currentArmy->m_frameInfo.standStillDelay
                                     * COMBAT_STAND_DELAY_BASE_FACTOR
                                 - Random(0, currentArmy->m_frameInfo.standStillDelay)
                                       * COMBAT_STAND_DELAY_RANDOM_FACTOR)
                                + currentArmy->m_lastAnimationTime
                            );
                        }
                    }
                }
            }
        }
    }
    for (side = COMBAT_ATTACKER_SIDE; IDX(side) < COMBAT_SIDE_COUNT; ++side) {
        if (m_drawHero[IDX(side)] != 0) {
            if (nextHeroAnimation[IDX(side)] != -1) {
                m_heroAnimationState[IDX(side)] = nextHeroAnimation[IDX(side)];
                m_heroAnimationFrame[IDX(side)] = 0;
            } else {
                ++m_heroAnimationFrame[IDX(side)];
                if (m_heroAnimationFrame[IDX(side)]
                    >= sCmbtHero[m_heroSpriteIndex[IDX(side)]]
                           .animationFrameCount[m_heroAnimationState[IDX(side)]]) {
                    m_heroAnimationState[IDX(side)] = HERO_ANIMATION_STAND;
                    m_heroAnimationFrame[IDX(side)] = 0;
                    m_heroCycleTimer[IDX(side)] = KBTickCount();
                }
            }
        }
    }
    DrawFrame(1, 1, 0, 0, COMMAND_FRAME_DELAY, 1, 1);
setCycleTimer:
    glTimers[GLOBAL_COMBAT_CYCLE_TIMER_SLOT] = static_cast<i32>(
        KBTickCount() + gfCombatSpeedMod[gConfig.combatSpeed] * COMBAT_CYCLE_TIMER_FACTOR
    );
}

VA(0x004318ed, 0x3b)
void combatManager::SetCombatViewArmySmallLevel(i32 level) {
    gConfig.combatArmyInfoLevel = level;
    DrawFrame(1, 0, 0, 0, COMMAND_FRAME_DELAY, 1, 1);
    WritePrefs();
}

VA(0x00431928, 0xe3)
void combatManager::SetCombatGrid(i32 showGrid, i32 showMouseHex, i32 shadeLevel) {
    if (gConfig.showCombatGrid == showGrid && gConfig.showCombatMouseHex == showMouseHex
        && gConfig.combatShadeLevel == shadeLevel)
        return;

    UpdateMouseGrid(INVALID_HEX, 0);
    gConfig.showCombatGrid = showGrid;
    gConfig.showCombatMouseHex = showMouseHex;
    gConfig.combatShadeLevel = shadeLevel;
    m_backgroundDrawn = 0;
    SetupGridForArmy(&m_armies[IDX(m_currentArmySide)][m_currentArmyIndex]);
    DrawFrame(1, 0, 0, 0, COMMAND_FRAME_DELAY, 1, 1);
    ResetMouse();
    WritePrefs();
}

VA(0x00431a0b, 0x3ab)
void combatManager::AddArmy(
    H2_ENUM_PARAM(CombatSide, i32) side,
    H2_ENUM_PARAM(CreatureType, i32) monsterType,
    i32 quantity,
    i32 hex,
    H2_ENUM_PARAM(MonsterFlags, i32) flags,
    i32 animate
) {
    i32 armyIndex_r = INVALID_ARMY_INDEX;
    i32 reusedArmy_m = 0;
    i32 index_g;
    army* newArmy;
    for (index_g = 0; index_g < COMBAT_ARMY_CAPACITY; ++index_g) {
        if (m_armies[IDX(side)][index_g].m_monsterType == CREATURE_NONE) {
            armyIndex_r = index_g;
            break;
        }
        if (m_armies[IDX(side)][index_g].m_quantity == 0
            && HAS(m_armies[IDX(side)][index_g].m_monster.flags.all, MONSTER_FLAGS_AI_EXCLUDED) != 0
            && (HAS(m_armies[IDX(side)][index_g].m_monster.flags.all, MONSTER_FLAGS_MIRROR_IMAGE) != 0
                || m_armies[IDX(side)][index_g].m_monsterType == CREATURE_EARTH_ELEMENTAL
                || m_armies[IDX(side)][index_g].m_monsterType == CREATURE_AIR_ELEMENTAL
                || m_armies[IDX(side)][index_g].m_monsterType == CREATURE_FIRE_ELEMENTAL
                || m_armies[IDX(side)][index_g].m_monsterType == CREATURE_WATER_ELEMENTAL)) {
            armyIndex_r = index_g;
            reusedArmy_m = 1;
            break;
        }
    }

    if (armyIndex_r == INVALID_ARMY_INDEX
        || m_hexCells[hex].m_occupantSide != COMBAT_SIDE_NONE)
        return;

    newArmy = &m_armies[IDX(side)][armyIndex_r];
    newArmy->Init(monsterType, quantity, side, armyIndex_r, hex, INVALID_HEX);
    newArmy->LoadResources();
    newArmy->m_monster.flags.all |= flags;
    if (reusedArmy_m == 0)
        ++m_armyCount[IDX(side)];

    if (animate == 0)
        return;

    ResetLimitCreature();
    ++m_limitCreatureCount[IDX(side)][armyIndex_r];
    gpCombatManager->DrawFrame(0, 1, 0, 1, COMMAND_FRAME_DELAY, 1, 1);
    gpWindowManager->SaveFizzleSource(
        giMinExtentX,
        giMinExtentY,
        giMaxExtentX - giMinExtentX + 1,
        giMaxExtentY - giMinExtentY + 1
    );
    UpdateGrid(0, 1);
    DrawFrame(0, 0, 0, 0, COMMAND_FRAME_DELAY, 1, 1);
    gpWindowManager->FizzleForward(
        giMinExtentX,
        giMinExtentY,
        giMaxExtentX - giMinExtentX + 1,
        giMaxExtentY - giMinExtentY + 1,
        COMMAND_FRAME_DELAY,
        NULL,
        NULL
    );
}

VA(0x00431db6, 0x169)
void combatManager::SetupSmallView(void) {
    i32 unusedSmallViewWord1;
    i32 unusedSmallViewWord2;

    if (m_smallViewSide[IDX(COMBAT_DEFENDER_SIDE)] == COMBAT_SIDE_NONE
        && m_smallViewLastX[IDX(COMBAT_DEFENDER_SIDE)] != SMALL_VIEW_POSITION_NONE) {
        DrawSmallView(IDX(COMBAT_DEFENDER_SIDE), 1);
    }

    if (gbThisNetHasControl != 0 && m_playerId[IDX(m_currentSide)] != PLAYER_NONE
        && gbHumanPlayer[m_playerId[IDX(m_currentSide)]] != 0) {
        if (m_smallViewSide[IDX(COMBAT_ATTACKER_SIDE)] != m_currentSide
            || m_smallViewArmyIndex[IDX(COMBAT_ATTACKER_SIDE)] != m_currentArmyIndex) {
            if (m_smallViewSide[IDX(COMBAT_ATTACKER_SIDE)] != COMBAT_SIDE_NONE
                && m_smallViewSide[IDX(COMBAT_ATTACKER_SIDE)] != m_currentSide
                && m_smallViewLastX[IDX(COMBAT_ATTACKER_SIDE)] != SMALL_VIEW_POSITION_NONE) {
                m_smallViewSide[IDX(COMBAT_ATTACKER_SIDE)] = COMBAT_SIDE_NONE;
                DrawSmallView(IDX(COMBAT_ATTACKER_SIDE), 1);
            }
            m_smallViewSide[IDX(COMBAT_ATTACKER_SIDE)] = m_currentSide;
            m_smallViewArmyIndex[IDX(COMBAT_ATTACKER_SIDE)] = m_currentArmyIndex;
            DrawSmallView(IDX(COMBAT_ATTACKER_SIDE), 1);
        }
    } else {
        m_smallViewSide[IDX(COMBAT_ATTACKER_SIDE)] = COMBAT_SIDE_NONE;
        if (m_smallViewLastX[IDX(COMBAT_ATTACKER_SIDE)] != SMALL_VIEW_POSITION_NONE)
            DrawSmallView(IDX(COMBAT_ATTACKER_SIDE), 1);
    }
}

VA(0x00431f1f, 0x304)
void combatManager::ViewBallista(i32 quickView) {
    i32 archerCount;
    i32 attackBonus;
    char description[VIEW_DESCRIPTION_SIZE];

    m_combatTowns[IDX(COMBAT_DEFENDER_SIDE)]->CalcNumLevelArchers(&archerCount, &attackBonus);
    sprintf(gText, DATA_COMPGEN(0x004f0ee4, viewBallistaBallista, "Ballista"));
    strcpy(description, DATA_COMPGEN(0x004f0ef0, viewBallistaEmptyString, ""));
    if (m_wallStates[IDX(COMBAT_WALL_SLOT_KEEP)] != COMBAT_WALL_STATE_KEEP_STANDING) {
        sprintf(description, DATA_COMPGEN(0x004f0f00, viewBallistaTheSIsDestroyed, "\n\nThe %s is destroyed."), DATA_COMPGEN(0x004f0ef4, viewBallistaBallista2, "Ballista"));
    } else if (attackBonus > 0) {
        sprintf(
            description,
            DATA_COMPGEN(0x004f0f24, viewBallistaTheSFiresWithTheStrength, "\n\nThe %s fires with the strength of %d Archers, each "
            "with a +%d bonus to their attack skill."),
            DATA_COMPGEN(0x004f0f18, viewBallistaBallista3, "Ballista"),
            archerCount,
            attackBonus
        );
    } else {
        sprintf(
            description,
            DATA_COMPGEN(0x004f0f90, viewBallistaTheSFiresWithTheStrength2, "\n\nThe %s fires with the strength of %d Archers."),
            DATA_COMPGEN(0x004f0f84, viewBallistaBallista4, "Ballista"),
            archerCount
        );
    }
    strcat(gText, description);

    strcpy(description, DATA_COMPGEN(0x004f0fc0, viewBallistaEmptyString2, ""));
    if ((m_combatTowns[IDX(COMBAT_DEFENDER_SIDE)]->m_buildings & IDX(TOWN_BUILDING_LEFT_TURRET)) != 0) {
        if (m_wallStates[IDX(COMBAT_WALL_SLOT_TOP_TOWER)] == COMBAT_WALL_STATE_DESTROYED) {
            sprintf(description, DATA_COMPGEN(0x004f0fd0, viewBallistaTheSIsDestroyed2, "\n\nThe %s is destroyed."), DATA_COMPGEN(0x004f0fc4, viewBallistaLeftTurret, "Left Turret"));
        } else if (m_wallStates[IDX(COMBAT_WALL_SLOT_TOP_TOWER)]
                   == COMBAT_WALL_STATE_TOWER_STANDING) {
            if (attackBonus > 0) {
                sprintf(
                    description,
                    DATA_COMPGEN(0x004f0ff4, viewBallistaTheSFiresWithTheStrength3, "\n\nThe %s fires with the strength of %d Archers, "
                    "each with a +%d bonus to their attack skill."),
                    DATA_COMPGEN(0x004f0fe8, viewBallistaLeftTurret2, "Left Turret"),
                    archerCount / COMBAT_KEEP_SIDE_TOWER_SHOT_DIVISOR,
                    attackBonus
                );
            } else {
                sprintf(
                    description,
                    DATA_COMPGEN(0x004f1060, viewBallistaTheSFiresWithTheStrength4, "\n\nThe %s fires with the strength of %d Archers."),
                    DATA_COMPGEN(0x004f1054, viewBallistaLeftTurret3, "Left Turret"),
                    archerCount / COMBAT_KEEP_SIDE_TOWER_SHOT_DIVISOR
                );
            }
        }
        strcat(gText, description);
    }

    if ((m_combatTowns[IDX(COMBAT_DEFENDER_SIDE)]->m_buildings & IDX(TOWN_BUILDING_RIGHT_TURRET)) != 0) {
        strcpy(description, DATA_COMPGEN(0x004f1090, viewBallistaEmptyString3, ""));
        if (m_wallStates[IDX(COMBAT_WALL_SLOT_BOTTOM_TOWER)] == COMBAT_WALL_STATE_DESTROYED) {
            sprintf(description, DATA_COMPGEN(0x004f10a4, viewBallistaTheSIsDestroyed3, "\n\nThe %s is destroyed."), DATA_COMPGEN(0x004f1094, viewBallistaRightTurret, "Right Turret"));
        } else if (m_wallStates[IDX(COMBAT_WALL_SLOT_BOTTOM_TOWER)]
                   == COMBAT_WALL_STATE_TOWER_STANDING) {
            if (attackBonus > 0) {
                sprintf(
                    description,
                    DATA_COMPGEN(0x004f10cc, viewBallistaTheSFiresWithTheStrength5, "\n\nThe %s fires with the strength of %d Archers, "
                    "each with a +%d bonus to their attack skill."),
                    DATA_COMPGEN(0x004f10bc, viewBallistaRightTurret2, "Right Turret"),
                    archerCount / COMBAT_KEEP_SIDE_TOWER_SHOT_DIVISOR,
                    attackBonus
                );
            } else {
                sprintf(
                    description,
                    DATA_COMPGEN(0x004f113c, viewBallistaTheSFiresWithTheStrength6, "\n\nThe %s fires with the strength of %d Archers."),
                    DATA_COMPGEN(0x004f112c, viewBallistaRightTurret3, "Right Turret"),
                    archerCount / COMBAT_KEEP_SIDE_TOWER_SHOT_DIVISOR
                );
            }
        }
        strcat(gText, description);
    }

    NormalDialog(
        gText,
        quickView == 0 ? NORMAL_DIALOG_INFO : NORMAL_DIALOG_QUICK_VIEW,
        NORMAL_DIALOG_NO_RESOURCE,
        NORMAL_DIALOG_NO_VALUE,
        NORMAL_DIALOG_NO_RESOURCE,
        0,
        NORMAL_DIALOG_NO_RESOURCE,
        0,
        NORMAL_DIALOG_NO_RESOURCE,
        0
    );
}

DATA(0x005250b8) b32 gbThisNetHasControl;
DATA(0x005250bc) i32 iCurTransferArtifact;
DATA(0x005250c0) i8 iTransferArtifactsInfo[COMBAT_TRANSFER_ARTIFACT_COUNT];
DATA(0x005250d0) b32 gbWhichAnimationPlaying;
DATA(0x005250d4) i32 iMaxTransferArtifacts;
DATA(0x005250d8) i32 giNextActionExtra;
DATA(0x005250dc) i32 bSkeletonsShown;
DATA(0x005250e0) b32 gbShowingLoseWindow;
DATA(0x005250e4) i32 giWinCmbtFrame;
DATA(0x005250e8) i32 giNextActionGridIndex;
DATA(0x005250ec) i32 giSurrenderCost;
DATA(0x005250f0) i32 giSkeletonsCreated;
DATA(0x005250f8)
H2_ENUM_STORAGE(ArtifactType, i8) iTransferArtifacts[COMBAT_TRANSFER_ARTIFACT_COUNT];
DATA(0x00525108) H2_ENUM_STORAGE(CombatAction, i32) giNextAction;
DATA(0x0052510c) i32 giNextActionGridIndex2;
