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

#define COMBAT_NECROMANCY_LEVEL_FACTOR 0.1
#define COMBAT_SURRENDER_QUILL_FACTOR 0.1
#define COMBAT_SURRENDER_BASE_FACTOR 0.5
#define COMBAT_SURRENDER_DIPLOMACY_FACTOR 0.2
#define COMBAT_IDLE_ROLL_DIVISOR 100.0f
#define COMBAT_STAND_DELAY_BASE_FACTOR 0.25
#define COMBAT_STAND_DELAY_RANDOM_FACTOR 0.5
#define COMBAT_CYCLE_TIMER_FACTOR 150.0f
#define COMBAT_SOUND_POLL_DELAY 75.0f
#define COMBAT_DIRECTION_SLOPE_STEEPEST 3.73
#define COMBAT_DIRECTION_SLOPE_STEEP 1.73
#define COMBAT_DIRECTION_SLOPE_DIAGONAL 1.0f
#define COMBAT_DIRECTION_SLOPE_SHALLOW 0.58
#define COMBAT_DIRECTION_SLOPE_SHALLOWEST 0.27

namespace {

    H2_ENUM_CLASS_BEGIN(CombatKeyCommand)
        KEY_MOUSE_COORDS = 2,
        KEY_VIEW_ARMY = 0x14,
        KEY_VIEW_GENERAL = 0x23,
        KEY_CAST_SPELL = 0x2e,
        KEY_WAIT = 0x39,
        KEY_CLOSE_NETWORK_BOX = 0x3c,
        KEY_REDRAW_SCREEN = 0x3d,
        KEY_CYCLE_ARMY_VIEW = 0x3f,
        KEY_TOGGLE_GRID = 0x40,
        KEY_TOGGLE_MOUSE_HEX = 0x41,
        KEY_CYCLE_SHADE = 0x42,
        KEY_CYCLE_SPELL_EFFECT = 0x43,
        KEY_DEBUG_CREATURE_EFFECT = 0x57
    H2_ENUM_CLASS_END(CombatKeyCommand)

    H2_ENUM_BEGIN(CombatWinLoseConstant)
        WIN_LOSE_WIDGET_COUNT = 25,
        WIN_LOSE_NEXT_CONTROL = 0x7800,
        WIN_LOSE_CLOSE_COMMAND = 10,
        WIN_LOSE_TEXT_ID = 101,
        WIN_LOSE_RESOURCE_LOAD_ID = 200,
        WIN_LOSE_RESOURCE_DRAW_ID = 201,
        WIN_LOSE_DRAW_DEPTH = 0x7fff,
        WIN_LOSE_ANIMATION_CYCLE_FIRST = 1,
        WIN_LOSE_ANIMATION_LOSS = 2,
        WIN_LOSE_ANIMATION_FLEE = 3,
        WIN_LOSE_ANIMATION_CYCLE_SECOND = 4,
        WIN_LOSE_FIRST_ANIMATION_FRAME = 1,
        WIN_LOSE_LOOP_FRAME_COUNT = 20,
        WIN_LOSE_FLEE_SECOND_RESOURCE_FRAME = 43,
        WIN_LOSE_FLEE_THIRD_RESOURCE_FRAME = 69,
        WIN_LOSE_FLEE_LAST_FRAME = 25,
        WIN_LOSE_LOSS_SECOND_RESOURCE_FRAME = 30,
        WIN_LOSE_LOSS_THIRD_RESOURCE_FRAME = 59,
        WIN_LOSE_LOSS_RESET_FRAME = 80,
        WIN_LOSE_INITIAL_DELAY = 90,
        WIN_LOSE_LOOP_DELAY = 99,
        WIN_LOSE_FLEE_DELAY = 40,
        WIN_LOSE_LOSS_DELAY = 50,
        WIN_LOSE_LOSS_FINAL_DELAY = 100,
        WIN_LOSE_ARTIFACT_ICON_ID = 2001,
        WIN_LOSE_ARTIFACT_IMAGE_ID = 2002,
        WIN_LOSE_ARTIFACT_TEXT_ID = 2101,
        WIN_LOSE_SKELETON_ICON_ID = 900,
        WIN_LOSE_SKELETON_TEXT_ID = 901,
        WIN_LOSE_EAGLE_BACKGROUND_ID = 902,
        WIN_LOSE_EAGLE_SPELL_ID = 903,
        WIN_LOSE_EAGLE_TEXT_ID = 904,
        WIN_LOSE_ICON_FILENAME_SIZE = 40,
        PICKUP_SAMPLE_FIRST = 1,
        PICKUP_SAMPLE_LAST = 5
    H2_ENUM_END(CombatWinLoseConstant)

    H2_ENUM_CLASS_BEGIN(CombatBattleResultText)
        RESULT_TEXT_ENEMY_SURRENDERED = 0,
        RESULT_TEXT_ENEMY_FLED = 1,
        RESULT_TEXT_VICTORY = 2,
        RESULT_TEXT_EXPERIENCE = 3,
        RESULT_TEXT_HERO_SURRENDER = 4,
        RESULT_TEXT_HERO_FLEE = 5,
        RESULT_TEXT_HERO_DEFEAT = 6,
        RESULT_TEXT_FORCES_SURRENDER = 7,
        RESULT_TEXT_FORCES_FLEE = 8,
        RESULT_TEXT_FORCES_DEFEAT = 9,
        RESULT_TEXT_NETWORK_EXPERIENCE = 10
    H2_ENUM_CLASS_END(CombatBattleResultText)

    H2_ENUM_CLASS_BEGIN(CombatControlId)
        CONTROL_ATTACK = 1,
        CONTROL_WAIT = 2,
        CONTROL_DISABLE_SELECTION = 3,
        CONTROL_SYSTEM_OPTIONS = 4,
        CONTROL_HELP_FIRST = 10,
        CONTROL_HELP_SECOND = 11,
        CONTROL_HELP_THIRD = 12,
        CONTROL_HELP_FOURTH = 13,
        CONTROL_MAIN_BUTTON = 0x40
    H2_ENUM_CLASS_END(CombatControlId)

    H2_ENUM_BEGIN(CombatRemoteConstant)
        REMOTE_PACKET_TYPE = 1,
        REMOTE_COMMAND_MESSAGE = 11,
        REMOTE_COMMAND_ACTION = 23
    H2_ENUM_END(CombatRemoteConstant)

    H2_ENUM_CLASS_BEGIN(CombatActionDataIndex)
        ACTION_DATA_ACTION = 0,
        ACTION_DATA_EXTRA = 1,
        ACTION_DATA_GRID = 2,
        ACTION_DATA_SECOND_GRID = 3,
        ACTION_DATA_COUNT = 4
    H2_ENUM_CLASS_END(CombatActionDataIndex)

    H2_ENUM_BEGIN(CombatDirectionConstant)
        DIRECTION_SECTOR_COUNT = 24,
        DIRECTION_SECTOR_ORIGIN = 0,
        DIRECTION_SECTOR_QUARTER = 6,
        DIRECTION_SECTOR_HALF = 12,
        DIRECTION_SECTOR_THREE_QUARTERS = 18,
        DIRECTION_PENDING_OFFSET = 10,
        DIRECTION_SECTORS_PER_ADJACENT = 4,
        DIRECTION_MOUSE_TOP_OFFSET = 5,
        DIRECTION_MOUSE_CENTER_OFFSET = 21,
        DIRECTION_SECTOR_OFFSET_SHALLOW = 2,
        DIRECTION_SECTOR_OFFSET_DIAGONAL = 3,
        DIRECTION_SECTOR_OFFSET_STEEP = 4,
        DIRECTION_SECTOR_OFFSET_STEEPEST = 5,
        DIRECTION_SPECIAL_FIRST_SECTOR_START = 11,
        DIRECTION_SPECIAL_FIRST_SECTOR_CENTER = 12,
        DIRECTION_SPECIAL_FIRST_SECTOR_END = 13,
        DIRECTION_SPECIAL_SECOND_SECTOR_PREVIOUS = 23,
        DIRECTION_SPECIAL_SECOND_SECTOR_CENTER = 0,
        DIRECTION_SPECIAL_SECOND_SECTOR_NEXT = 1
    H2_ENUM_END(CombatDirectionConstant)

    H2_ENUM_BEGIN(CombatCommandConstant)
        POINTER_VIEW = 5,
        POINTER_ATTACK_OFFSET = 7,
        INVALID_ARMY_INDEX = -1,
        INVALID_HEX = -1,
        PLAYER_NONE = -1,
        SMALL_VIEW_POSITION_NONE = -1,
        IGNORED_HEX = -2,
        CASTLE_GATE_HEX = 59
    H2_ENUM_END(CombatCommandConstant)

    H2_ENUM_BEGIN(CombatRoundConstant)
        ROUND_INITIAL_SPEED = 15
    H2_ENUM_END(CombatRoundConstant)

    H2_ENUM_BEGIN(CombatCommandGeometry)
        CONTROL_RIGHT_MIN_X = 590,
        CONTROL_LEFT_MAX_X = 50,
        CONTROL_SYSTEM_OPTIONS_MIN_Y = 460,
        SCREEN_MAX_X = 639,
        SCREEN_MAX_Y = 479,
        ARMY_VIEW_LEVEL_COUNT = 3,
        COMMAND_FRAME_DELAY = 75
    H2_ENUM_END(CombatCommandGeometry)

    H2_ENUM_BEGIN(CombatCasualtyConstant)
        CASUALTY_DISPLAY_LIMIT = 7,
        CASUALTY_WIDGETS_PER_SIDE = 7,
        CASUALTY_WINDOW_WIDTH = 320,
        CASUALTY_WINDOW_BOTTOM = 458
    H2_ENUM_END(CombatCasualtyConstant)

    H2_ENUM_BEGIN(CombatArtifactPanelConstant)
        ARTIFACT_BACKGROUND_X = 136,
        ARTIFACT_BACKGROUND_Y = 310,
        ARTIFACT_BACKGROUND_SIZE = 80,
        ARTIFACT_ICON_X = 144,
        ARTIFACT_ICON_Y = 318,
        ARTIFACT_ICON_SIZE = 64,
        ARTIFACT_TEXT_X = 16,
        ARTIFACT_TEXT_Y = 397,
        ARTIFACT_TEXT_HEIGHT = 12,
        ARTIFACT_NAME_CAPACITY = 60
    H2_ENUM_END(CombatArtifactPanelConstant)

    H2_ENUM_BEGIN(CombatSkeletonPanelConstant)
        SKELETON_ICON_X = 173,
        SKELETON_ICON_Y = 270,
        SKELETON_ICON_WIDTH = 32,
        SKELETON_ICON_HEIGHT = 30,
        SKELETON_TEXT_X = 165,
        SKELETON_TEXT_Y = 300,
        SKELETON_TEXT_WIDTH = 32,
        SKELETON_TEXT_HEIGHT = 12,
        SKELETON_COUNT_CAPACITY = 9
    H2_ENUM_END(CombatSkeletonPanelConstant)

    H2_ENUM_BEGIN(CombatEagleEyePanelConstant)
        EAGLE_PANEL_X = 105,
        EAGLE_PANEL_Y = 275,
        EAGLE_BACKGROUND_X_OFFSET = 16,
        EAGLE_ICON_X_OFFSET = 36,
        EAGLE_ICON_Y_OFFSET = 5,
        EAGLE_ICON_WIDTH = 72,
        EAGLE_ICON_HEIGHT = 51,
        EAGLE_TEXT_X_OFFSET = 34,
        EAGLE_TEXT_Y_OFFSET = 57,
        EAGLE_TEXT_WIDTH = 80,
        EAGLE_TEXT_HEIGHT = 24,
        EAGLE_SPELL_NAME_CAPACITY = 200
    H2_ENUM_END(CombatEagleEyePanelConstant)

    H2_ENUM_BEGIN(CombatCasualtyPanelConstant)
        CASUALTY_TITLE_WIDGET = 17,
        CASUALTY_TITLE_X = 16,
        CASUALTY_TITLE_Y = 263,
        CASUALTY_TEXT_HEIGHT = 20,
        CASUALTY_TEXT_WIDGET_ID = 2110,
        CASUALTY_SIDE_WIDGET_FIRST = 15,
        CASUALTY_ATTACKER_Y = 279,
        CASUALTY_DEFENDER_Y = 346,
        CASUALTY_SIDE_LABEL_Y_OFFSET = 3,
        CASUALTY_NONE_Y_OFFSET = 21,
        CASUALTY_WIDGET_ID_STRIDE = 5,
        CASUALTY_TEXT_WIDGET_ID_FIRST = 2100,
        CASUALTY_ICON_WIDGET_ID_FIRST = 2000,
        CASUALTY_ICON_SPACING = 40,
        CASUALTY_CENTER_DIVISOR = 2,
        CASUALTY_CENTER_X_OFFSET = 3,
        CASUALTY_ICON_CELL_WIDTH = 32,
        CASUALTY_ICON_TOP_OFFSET = 19,
        CASUALTY_ICON_WIDTH = 32,
        CASUALTY_ICON_HEIGHT = 28,
        CASUALTY_QUANTITY_X_OFFSET = 16,
        CASUALTY_QUANTITY_Y_OFFSET = 53,
        CASUALTY_QUANTITY_WIDTH = 32,
        CASUALTY_QUANTITY_HEIGHT = 12,
        CASUALTY_HEADING_CAPACITY = 30,
        CASUALTY_NONE_CAPACITY = 10,
        CASUALTY_QUANTITY_CAPACITY = 9
    H2_ENUM_END(CombatCasualtyPanelConstant)

    H2_ENUM_BEGIN(CombatCycleConstant)
        HERO_ANIMATION_STAND = 0,
        HERO_ANIMATION_DEATH_FIRST = 1,
        HERO_ANIMATION_DEATH_SECOND = 2,
        HERO_ANIMATION_IDLE_FIRST = 9,
        HERO_ANIMATION_IDLE_SECOND = 10,
        HERO_ANIMATION_IDLE_LAST = 11,
        HERO_IDLE_DELAY = 4500,
        STAND_DELAY_RANDOM_THRESHOLD = 51,
        STAND_DELAY_RANDOM_MIN = 50,
        IDLE_ROLL_MIN = 1,
        IDLE_ROLL_MAX = 100,
        IDLE_REPEAT_CHANCE = 8,
        STANDING_ANIMATION_SEARCH_DONE = 99
    H2_ENUM_END(CombatCycleConstant)

    H2_ENUM_BEGIN(CombatVictoryConstant)
        VICTORY_NECROMANCY_STACK_LIMIT = 5,
        VICTORY_FADE_STEPS = 8,
        VICTORY_WASTELAND_FADE_STEPS = 5,
        VICTORY_FADE_DELAY = 60,
        VICTORY_MUSIC = 29,
        LOSS_MUSIC = 30,
        VICTORY_EXPERIENCE_TEXT_SIZE = 152,
        WIN_LOSE_WINDOW_X = 143,
        WIN_LOSE_WINDOW_Y = 10,
        WIN_LOSE_DIALOG_TIMEOUT = 15000,
        WIN_LOSE_ANIMATION_FILENAME_SIZE = 52,
        VICTORY_DIM_STEP = 3
    H2_ENUM_END(CombatVictoryConstant)

    H2_ENUM_BEGIN(CombatSurrenderDialogConstant)
        SURRENDER_DIALOG_TYPE = 1,
        SURRENDER_DIALOG_ACCEPT_RESULT = 2,
        SURRENDER_TEXT_WIDTH = 30,
        SURRENDER_PORTRAIT_RESOURCE_ID = 1,
        SURRENDER_PORTRAIT_WIDGET_ID = 30,
        SURRENDER_PORTRAIT_DEFAULT_COLOR = 4,
        SURRENDER_TEXT_ID = 2,
        SURRENDER_WINDOW_X = 74,
        SURRENDER_WINDOW_Y = 80
    H2_ENUM_END(CombatSurrenderDialogConstant)

    H2_ENUM_CLASS_BEGIN(CombatHelpTextIndex)
        HELP_SKIP_UNIT = 1,
        HELP_AUTO_COMBAT = 2,
        HELP_SYSTEM_OPTIONS = 3,
        HELP_OTHER_CONTROL = 4
    H2_ENUM_CLASS_END(CombatHelpTextIndex)

    H2_ENUM_CLASS_BEGIN(CombatLongHelpIndex)
        LONG_HELP_ATTACK = 0,
        LONG_HELP_WAIT = 1,
        LONG_HELP_DISABLE_SELECTION = 2,
        LONG_HELP_SYSTEM_OPTIONS = 3,
        LONG_HELP_CONTROLS = 4
    H2_ENUM_CLASS_END(CombatLongHelpIndex)

    H2_ENUM_BEGIN(CombatStorageConstant)
        SELECTOR_INITIAL_FRAME = 7,
        VIEW_DESCRIPTION_SIZE = 300
    H2_ENUM_END(CombatStorageConstant)

} // namespace

VA(0x0042b850, 0x2be)
MessageDispatchResult combatManager::Main(tag_message& message) {
    MessageDispatchResult result = MESSAGE_DISPATCH_CONSUME;
    army* currentArmy;

    if (gbNoShowCombat == 0) {
        if (glTimers[0] < KBTickCount()) {
            PollSound();
            glTimers[0] = static_cast<i32>(
                KBTickCount() + COMBAT_SOUND_POLL_DELAY * gfCombatSpeedMod[gConfig.combatSpeed]
            );
        }
        if (glTimers[GLOBAL_COMBAT_CYCLE_TIMER_SLOT] < KBTickCount()
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

    currentArmy = &m_armies[IDX(m_currentArmySide)][m_currentArmyIndex];
    if (currentArmy->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_BERSERK)] != 0) {
        currentArmy->GoBerserk();
        if (CheckWin(&message) != 0)
            return MESSAGE_DISPATCH_FORWARD;
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
        if (m_playerId[IDX(m_currentSide)] == -1
            || gbThisNetHumanPlayer[m_playerId[IDX(m_currentSide)]] == 0
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

VA(0x0042bb0e, 0x123)
i32 combatManager::ValidHexToStandOn(i32 hexIndex) {
    if (hexIndex == IGNORED_HEX)
        return 1;

    if (!(hexIndex == INVALID_HEX || MAP_WIDTH - 1 == hexIndex % COMBAT_GRID_ROW_LENGTH
          || hexIndex % COMBAT_GRID_ROW_LENGTH == 0
          || (m_hexCells[hexIndex].m_blocked != 0
              && (gpCombatManager->m_inCastleCombat == 0
                  || (hexIndex != COMBAT_CASTLE_GATE_APPROACH_HEX && hexIndex != CASTLE_GATE_HEX)
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

VA(0x0042bc31, 0x758)
void combatManager::SetCombatDirections(i32 targetHex) {
    if (m_gridSelectionDisabled != 0)
        return;

    char standable_0[COMBAT_DIRECTION_COUNT];
    i32 directionHexes[COMBAT_DIRECTION_COUNT];
    i32 rearHexes_2[COMBAT_DIRECTION_COUNT];
    char pathValid_28[COMBAT_DIRECTION_COUNT];
    i32 outputDirection_7;
    i32 mappedDirection_5;
    i32 previous_1;
    i32 next_28;
    army* currentArmy_1 = &m_armies[IDX(m_currentArmySide)][m_currentArmyIndex];
    CombatSide targetSide_28 = currentArmy_1->m_targetSide;
    i32 targetIndex_9 = currentArmy_1->m_targetIndex;
    currentArmy_1->m_targetSide = COMBAT_SIDE_NONE;
    currentArmy_1->m_targetIndex = -1;
    army* targetArmy_13 = &m_armies[IDX(targetSide_28)][targetIndex_9];

    i32 direction_28;
    for (direction_28 = 0; direction_28 < COMBAT_DIRECTION_COUNT; direction_28++) {
        if (direction_28 == IDX(COMBAT_DIRECTION_WIDE_WEST)
            || direction_28 == IDX(COMBAT_DIRECTION_WIDE_EAST)) {
            if (HAS(currentArmy_1->m_monster.flags.all, MONSTER_FLAGS_WIDE) != 0) {
                if (currentArmy_1->m_facing == ARMY_FACING_RIGHT) {
                    if (direction_28 == IDX(COMBAT_DIRECTION_WIDE_WEST))
                        directionHexes[direction_28] =
                            m_adjacency[targetHex][IDX(COMBAT_DIRECTION_NORTHWEST)];
                    if (direction_28 == IDX(COMBAT_DIRECTION_WIDE_EAST))
                        directionHexes[direction_28] =
                            m_adjacency[targetHex][IDX(COMBAT_DIRECTION_SOUTHWEST)];
                } else {
                    if (direction_28 == IDX(COMBAT_DIRECTION_WIDE_WEST))
                        directionHexes[direction_28] =
                            m_adjacency[targetHex][IDX(COMBAT_DIRECTION_NORTHEAST)];
                    if (direction_28 == IDX(COMBAT_DIRECTION_WIDE_EAST))
                        directionHexes[direction_28] =
                            m_adjacency[targetHex][IDX(COMBAT_DIRECTION_SOUTHEAST)];
                }
            } else {
                directionHexes[direction_28] = INVALID_HEX;
            }
        } else {
            directionHexes[direction_28] =
                *(&m_adjacency[0][0] + targetHex * COMBAT_DIRECTION_ADJACENT_COUNT + direction_28);
        }

        if (HAS(currentArmy_1->m_monster.flags.all, MONSTER_FLAGS_WIDE) != 0
            && directionHexes[direction_28] != INVALID_HEX) {
            if (currentArmy_1->m_facing == ARMY_FACING_RIGHT) {
                if (direction_28 == IDX(COMBAT_DIRECTION_NORTHWEST)
                    || direction_28 == IDX(COMBAT_DIRECTION_WEST)
                    || direction_28 == IDX(COMBAT_DIRECTION_SOUTHWEST)) {
                    if (directionHexes[direction_28] % COMBAT_GRID_ROW_LENGTH
                        == COMBAT_GRID_FIRST_COLUMN)
                        directionHexes[direction_28] = INVALID_HEX;
                    else
                        directionHexes[direction_28]--;
                }
                if (directionHexes[direction_28] % COMBAT_GRID_ROW_LENGTH
                    == COMBAT_GRID_REVERSE_FIRST_COLUMN)
                    rearHexes_2[direction_28] = INVALID_HEX;
                else
                    rearHexes_2[direction_28] = directionHexes[direction_28] + 1;
            } else {
                if (direction_28 == IDX(COMBAT_DIRECTION_NORTHEAST)
                    || direction_28 == IDX(COMBAT_DIRECTION_EAST)
                    || direction_28 == IDX(COMBAT_DIRECTION_SOUTHEAST)) {
                    if (directionHexes[direction_28] % COMBAT_GRID_ROW_LENGTH
                        == COMBAT_GRID_REVERSE_FIRST_COLUMN)
                        directionHexes[direction_28] = INVALID_HEX;
                    else
                        directionHexes[direction_28]++;
                }
                if (directionHexes[direction_28] % COMBAT_GRID_ROW_LENGTH
                    == COMBAT_GRID_FIRST_COLUMN)
                    rearHexes_2[direction_28] = INVALID_HEX;
                else
                    rearHexes_2[direction_28] = directionHexes[direction_28] - 1;
            }
        } else {
            rearHexes_2[direction_28] = IGNORED_HEX;
        }

        if (ValidHexToStandOn(directionHexes[direction_28]) != 0
            && ValidHexToStandOn(rearHexes_2[direction_28]) != 0)
            standable_0[direction_28] = 1;
        else
            standable_0[direction_28] = 0;
    }

    if (HAS(currentArmy_1->m_monster.flags.all, MONSTER_FLAGS_FLYING) != 0) {
        for (direction_28 = 0; direction_28 < COMBAT_DIRECTION_COUNT; direction_28++)
            pathValid_28[direction_28] = standable_0[direction_28];
    } else {
        for (direction_28 = 0; direction_28 < COMBAT_DIRECTION_COUNT; direction_28++) {
            if (standable_0[direction_28] != 0) {
                if (currentArmy_1->m_hex == directionHexes[direction_28]
                    || currentArmy_1->ValidPath(
                           directionHexes[direction_28],
                           ARMY_PATH_EXACT_TARGET_HEX
                       ) != 0)
                    pathValid_28[direction_28] = 1;
                else
                    pathValid_28[direction_28] = 0;
            } else {
                pathValid_28[direction_28] = 0;
            }
        }
    }

    m_validDirectionCount = 0;
    for (direction_28 = 0; direction_28 < COMBAT_DIRECTION_COUNT; direction_28++) {
        if (pathValid_28[direction_28] != 0)
            m_validDirectionCount++;
    }
    if (m_validDirectionCount == 0)
        pathValid_28[IDX(COMBAT_DIRECTION_WIDE_WEST)] = 1;

    memset(m_directionMap, -1, sizeof(m_directionMap));
    for (direction_28 = 0; direction_28 < COMBAT_DIRECTION_COUNT; direction_28++) {
        outputDirection_7 = direction_28;
        if (direction_28 < COMBAT_DIRECTION_ADJACENT_COUNT)
            mappedDirection_5 =
                (direction_28 + COMBAT_DIRECTION_OPPOSITE_OFFSET) % COMBAT_DIRECTION_ADJACENT_COUNT;
        else
            mappedDirection_5 = direction_28 == IDX(COMBAT_DIRECTION_WIDE_WEST)
                ? IDX(COMBAT_DIRECTION_WIDE_EAST)
                : IDX(COMBAT_DIRECTION_WIDE_WEST);

        if (pathValid_28[mappedDirection_5] != 0) {
            if (HAS(targetArmy_13->m_monster.flags.all, MONSTER_FLAGS_WIDE) != 0) {
                if (direction_28 == IDX(COMBAT_DIRECTION_NORTHEAST)
                    && m_hexCells[targetHex - 1].m_occupantSide == targetSide_28
                    && m_hexCells[targetHex - 1].m_occupantIndex == targetIndex_9) {
                    outputDirection_7 = IDX(COMBAT_DIRECTION_WIDE_WEST);
                } else if (direction_28 == IDX(COMBAT_DIRECTION_NORTHWEST)
                           && m_hexCells[targetHex + 1].m_occupantSide == targetSide_28
                           && m_hexCells[targetHex + 1].m_occupantIndex == targetIndex_9) {
                    outputDirection_7 = IDX(COMBAT_DIRECTION_WIDE_WEST);
                } else if (direction_28 == IDX(COMBAT_DIRECTION_SOUTHEAST)
                           && m_hexCells[targetHex - 1].m_occupantSide == targetSide_28
                           && m_hexCells[targetHex - 1].m_occupantIndex == targetIndex_9) {
                    outputDirection_7 = IDX(COMBAT_DIRECTION_WIDE_EAST);
                } else if (direction_28 == IDX(COMBAT_DIRECTION_SOUTHWEST)
                           && m_hexCells[targetHex + 1].m_occupantSide == targetSide_28
                           && m_hexCells[targetHex + 1].m_occupantIndex == targetIndex_9) {
                    outputDirection_7 = IDX(COMBAT_DIRECTION_WIDE_EAST);
                }
            }

            if (direction_28 < COMBAT_DIRECTION_ADJACENT_COUNT) {
                memset(
                    &m_directionMap[mappedDirection_5 * DIRECTION_SECTORS_PER_ADJACENT],
                    outputDirection_7,
                    DIRECTION_SECTORS_PER_ADJACENT
                );
            } else if (direction_28 == IDX(COMBAT_DIRECTION_WIDE_WEST)) {
                m_directionMap[DIRECTION_SPECIAL_FIRST_SECTOR_START] =
                    static_cast<i8>(outputDirection_7);
                m_directionMap[DIRECTION_SPECIAL_FIRST_SECTOR_CENTER] =
                    static_cast<i8>(outputDirection_7);
                m_directionMap[DIRECTION_SPECIAL_FIRST_SECTOR_END] =
                    static_cast<i8>(outputDirection_7);
            } else {
                m_directionMap[DIRECTION_SPECIAL_SECOND_SECTOR_CENTER] =
                    static_cast<i8>(outputDirection_7);
                m_directionMap[DIRECTION_SPECIAL_SECOND_SECTOR_NEXT] =
                    static_cast<i8>(outputDirection_7);
                m_directionMap[DIRECTION_SPECIAL_SECOND_SECTOR_PREVIOUS] =
                    static_cast<i8>(outputDirection_7);
            }
        }
    }

    i32 unresolved_1 = DIRECTION_SECTOR_COUNT;
    while (unresolved_1 > 0) {
        for (direction_28 = 0; direction_28 < DIRECTION_SECTOR_COUNT; direction_28++) {
            if (m_directionMap[direction_28] == -1) {
                next_28 = (direction_28 + 1) % DIRECTION_SECTOR_COUNT;
                previous_1 =
                    (direction_28 + DIRECTION_SECTOR_COUNT - 1) % DIRECTION_SECTOR_COUNT;
                if (m_directionMap[next_28] >= 0
                    && m_directionMap[next_28] <= IDX(COMBAT_DIRECTION_WIDE_EAST)) {
                    m_directionMap[direction_28] =
                        m_directionMap[next_28] + DIRECTION_PENDING_OFFSET;
                } else if (m_directionMap[previous_1] >= 0
                           && m_directionMap[previous_1] <= IDX(COMBAT_DIRECTION_WIDE_EAST)) {
                    m_directionMap[direction_28] =
                        m_directionMap[previous_1] + DIRECTION_PENDING_OFFSET;
                }
            }
        }
        unresolved_1 = 0;
        for (direction_28 = 0; direction_28 < DIRECTION_SECTOR_COUNT; direction_28++) {
            if (m_directionMap[direction_28] >= DIRECTION_PENDING_OFFSET) {
                m_directionMap[direction_28] -= DIRECTION_PENDING_OFFSET;
            } else if (m_directionMap[direction_28] == INVALID_HEX) {
                unresolved_1++;
            }
        }
    }

    currentArmy_1->m_targetSide = targetSide_28;
    currentArmy_1->m_targetIndex = targetIndex_9;
}

VA(0x0042c389, 0x575)
void combatManager::CheckSetMouseDirection(i32 mouseX, i32 mouseY, i32 targetHex) {
    if (m_gridSelectionDisabled != 0)
        return;
    if (m_validDirectionCount <= 1 && m_mouseDirection >= 0)
        return;

    i32 relativeX_7 =
        mouseX - (targetHex % COMBAT_GRID_ROW_LENGTH - 1) * COMBAT_HEX_HORIZONTAL_STEP;
    relativeX_7 -= COMBAT_HEX_GRID_LEFT_ORIGIN;
    if (((targetHex / COMBAT_GRID_ROW_LENGTH) & 1) == 0)
        relativeX_7 -= COMBAT_HEX_ROW_STAGGER;
    i32 relativeY_9 = mouseY - COMBAT_HEX_CENTER_Y_ORIGIN
                      - targetHex / COMBAT_GRID_ROW_LENGTH * COMBAT_HEX_VERTICAL_STEP;
    relativeY_9 -= DIRECTION_MOUSE_TOP_OFFSET;
    relativeX_7 -= COMBAT_HEX_ROW_STAGGER;
    relativeY_9 -= DIRECTION_MOUSE_CENTER_OFFSET;

    i32 sector_6 = 0;
    if (relativeX_7 < 0) {
        if (relativeY_9 < 0)
            sector_6 += DIRECTION_SECTOR_THREE_QUARTERS;
        else
            sector_6 += DIRECTION_SECTOR_HALF;
    } else {
        if (relativeY_9 < 0)
            sector_6 += DIRECTION_SECTOR_ORIGIN;
        else
            sector_6 += DIRECTION_SECTOR_QUARTER;
    }

    relativeX_7 = abs(relativeX_7);
    relativeY_9 = abs(relativeY_9);
    /* The parenthesised divisor cast is load-bearing: without it VC6 folds the
       divisor into a single `fidiv`, while retail keeps both operands on the x87
       stack (`fild`, `fild`, `fdivp`). */
    float ratio_3 = static_cast<float>(relativeX_7) / (static_cast<float>(relativeY_9));
    if (sector_6 == 0 || sector_6 == DIRECTION_SECTOR_HALF) {
        if (ratio_3 > COMBAT_DIRECTION_SLOPE_STEEPEST)
            sector_6 += DIRECTION_SECTOR_OFFSET_STEEPEST;
        else if (ratio_3 > COMBAT_DIRECTION_SLOPE_STEEP)
            sector_6 += DIRECTION_SECTOR_OFFSET_STEEP;
        else if (ratio_3 > COMBAT_DIRECTION_SLOPE_DIAGONAL)
            sector_6 += DIRECTION_SECTOR_OFFSET_DIAGONAL;
        else if (ratio_3 > COMBAT_DIRECTION_SLOPE_SHALLOW)
            sector_6 += DIRECTION_SECTOR_OFFSET_SHALLOW;
        else if (ratio_3 > COMBAT_DIRECTION_SLOPE_SHALLOWEST)
            sector_6++;
    } else {
        if (ratio_3 < COMBAT_DIRECTION_SLOPE_SHALLOWEST)
            sector_6 += DIRECTION_SECTOR_OFFSET_STEEPEST;
        else if (ratio_3 < COMBAT_DIRECTION_SLOPE_SHALLOW)
            sector_6 += DIRECTION_SECTOR_OFFSET_STEEP;
        else if (ratio_3 < COMBAT_DIRECTION_SLOPE_DIAGONAL)
            sector_6 += DIRECTION_SECTOR_OFFSET_DIAGONAL;
        else if (ratio_3 < COMBAT_DIRECTION_SLOPE_STEEP)
            sector_6 += DIRECTION_SECTOR_OFFSET_SHALLOW;
        else if (ratio_3 < COMBAT_DIRECTION_SLOPE_STEEPEST)
            sector_6++;
    }

    if (m_directionMap[sector_6] == m_mouseDirection)
        return;

    m_mouseDirection = m_directionMap[sector_6];
    CombatHexDirection direction_5 =
        OppositeDirection(static_cast<CombatHexDirection>(m_directionMap[sector_6]));
    CombatHexDirection directionCopy_1 = direction_5;
    CombatHexDirection alternateDirection = COMBAT_DIRECTION_INVALID;
    army* currentArmy = &m_armies[IDX(m_currentArmySide)][m_currentArmyIndex];
    army* targetArmy_13 = &m_armies[IDX(currentArmy->m_targetSide)][currentArmy->m_targetIndex];

    if (direction_5 == COMBAT_DIRECTION_WIDE_WEST || direction_5 == COMBAT_DIRECTION_WIDE_EAST) {
        if (HAS(currentArmy->m_monster.flags.all, MONSTER_FLAGS_WIDE) != 0) {
            if (currentArmy->m_facing == ARMY_FACING_RIGHT
                && direction_5 == COMBAT_DIRECTION_WIDE_WEST) {
                direction_5 = COMBAT_DIRECTION_NORTHWEST;
                alternateDirection = COMBAT_DIRECTION_NORTHEAST;
            } else if (currentArmy->m_facing == ARMY_FACING_RIGHT
                       && direction_5 == COMBAT_DIRECTION_WIDE_EAST) {
                direction_5 = COMBAT_DIRECTION_SOUTHWEST;
                alternateDirection = COMBAT_DIRECTION_SOUTHEAST;
            } else if (currentArmy->m_facing == ARMY_FACING_LEFT
                       && direction_5 == COMBAT_DIRECTION_WIDE_WEST) {
                direction_5 = COMBAT_DIRECTION_NORTHEAST;
                alternateDirection = COMBAT_DIRECTION_NORTHWEST;
            } else {
                direction_5 = COMBAT_DIRECTION_SOUTHEAST;
                alternateDirection = COMBAT_DIRECTION_SOUTHWEST;
            }
        } else {
            if (m_hexCells[targetHex - 1].m_occupantSide == currentArmy->m_targetSide
                && m_hexCells[targetHex - 1].m_occupantIndex == currentArmy->m_targetIndex) {
                targetHex--;
            }
            if (direction_5 == COMBAT_DIRECTION_WIDE_WEST)
                direction_5 = COMBAT_DIRECTION_NORTHEAST;
            else
                direction_5 = COMBAT_DIRECTION_SOUTHEAST;
        }
    } else {
        if (currentArmy->m_facing == ARMY_FACING_RIGHT
            && HAS(currentArmy->m_monster.flags.all, MONSTER_FLAGS_WIDE) != 0) {
            if (direction_5 == COMBAT_DIRECTION_NORTHWEST || direction_5 == COMBAT_DIRECTION_WEST
                || direction_5 == COMBAT_DIRECTION_SOUTHWEST)
                targetHex--;
        } else if (currentArmy->m_facing == ARMY_FACING_LEFT
                   && HAS(currentArmy->m_monster.flags.all, MONSTER_FLAGS_WIDE) != 0
                   && (direction_5 == COMBAT_DIRECTION_NORTHEAST
                       || direction_5 == COMBAT_DIRECTION_EAST
                       || direction_5 == COMBAT_DIRECTION_SOUTHEAST)) {
            targetHex++;
        }
    }

    m_directionTargetHex = m_adjacency[targetHex][IDX(direction_5)];
    i32 rearHex_5 = IGNORED_HEX;
    if (currentArmy->m_facing == ARMY_FACING_LEFT
        && HAS(currentArmy->m_monster.flags.all, MONSTER_FLAGS_WIDE) != 0) {
        rearHex_5 = m_directionTargetHex - 1;
    }
    if (currentArmy->m_facing == ARMY_FACING_RIGHT
        && HAS(currentArmy->m_monster.flags.all, MONSTER_FLAGS_WIDE) != 0) {
        rearHex_5 = m_directionTargetHex + 1;
    }
    if (ValidHexToStandOn(m_directionTargetHex) == 0 || ValidHexToStandOn(rearHex_5) == 0) {
        if (HAS(currentArmy->m_monster.flags.all, MONSTER_FLAGS_WIDE) != 0
            && (directionCopy_1 == COMBAT_DIRECTION_WIDE_WEST
                || directionCopy_1 == COMBAT_DIRECTION_WIDE_EAST)) {
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

VA(0x0042c8fe, 0x5a)
i32 combatManager::GetPointer(CombatMessageCommand command, i32 hexIndex) {
    i32 x;
    i32 y;
    i32 unusedResult;
    if (command == COMBAT_MESSAGE_COMMAND_OPPOSING_OPTIONS) {
        return POINTER_VIEW;
    } else {
        switch (command) {
            case COMBAT_MESSAGE_COMMAND_MOVE:
            case COMBAT_MESSAGE_COMMAND_FLY:
            case COMBAT_MESSAGE_COMMAND_SHOOT:
            case COMBAT_MESSAGE_COMMAND_VIEW_INFO: {
                x = hexIndex % COMBAT_GRID_ROW_LENGTH;
                y = hexIndex / COMBAT_GRID_ROW_LENGTH;
                return IDX(command);
            }
            default:
                return IDX(command);
        }
    }
}

VA(0x0042c958, 0x7f5)
MessageDispatchResult combatManager::ProcessCombatMsg(tag_message& message) {
    i32 mouseX = message.payload.mouse.screenX;
    i32 mouseY = message.payload.mouse.screenY;
    i32 unusedResult = 0;
    i32 selectedHex_0;
    tag_message pendingMessage_2;

    switch (message.type) {
        case MESSAGE_WIDGET:
            if (HAS(message.payload.widget.modifiers, MESSAGE_MODIFIER_RIGHT_BUTTON)) {
                if (message.payload.widget.command == WIDGET_COMMAND_SELECT
                    || message.payload.widget.command == WIDGET_COMMAND_ALTERNATE_SELECT) {
                    i32 helpIndex = -1;
                    switch (static_cast<CombatControlId>(message.payload.widget.id)) {
                        case CONTROL_MAIN_BUTTON:
                            RightClick(m_selectedHex);
                            break;
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
                break;
            }
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
                        case CONTROL_DISABLE_SELECTION:
                            m_gridSelectionDisabled = 1;
                            break;
                        case CONTROL_WAIT:
                            giNextAction = ACTION_WAIT;
                            break;
                        case CONTROL_ATTACK:
                            giNextAction = ACTION_DEFEND;
                            break;
                        case CONTROL_SYSTEM_OPTIONS:
                            CombatSystemOptions();
                            break;
                    }
                    break;
            }
            break;

        case MESSAGE_MOUSE_MOVE:
            if (m_gridSelectionDisabled != 0)
                break;
            pendingMessage_2 = gpInputManager->PeekEvent();
            if (pendingMessage_2.type == MESSAGE_MOUSE_MOVE)
                break;
            if (InCombatArea(message.payload.mouse.screenX, message.payload.mouse.screenY) != 0)
                selectedHex_0 = GetGridIndex(mouseX, mouseY);
            else
                selectedHex_0 = INVALID_HEX;

            UpdateMouseGrid(selectedHex_0, 0);
            if (InCombatArea(message.payload.mouse.screenX, message.payload.mouse.screenY) != 0) {
                if (selectedHex_0 != m_selectedHex || selectedHex_0 == INVALID_HEX) {
                    m_selectedHex = selectedHex_0;
                    m_previousCommand = COMBAT_INVALID_COMMAND;
                    m_currentCommand = GetCommand(m_selectedHex);
                    m_mouseDirection = INVALID_HEX;
                    if (m_currentCommand == COMBAT_MESSAGE_COMMAND_ATTACK) {
                        SetCombatDirections(selectedHex_0);
                        CheckSetMouseDirection(mouseX, mouseY, selectedHex_0);
                    } else {
                        gpMouseManager->SetPointer(GetPointer(m_currentCommand, selectedHex_0));
                    }
                } else if (m_currentCommand == COMBAT_MESSAGE_COMMAND_ATTACK) {
                    CheckSetMouseDirection(mouseX, mouseY, selectedHex_0);
                }
                if (m_currentCommand != m_previousCommand) {
                    m_previousCommand = m_currentCommand;
                    CombatMessage(m_currentCommand);
                }
            } else {
                if (mouseX >= CONTROL_RIGHT_MIN_X) {
                    CombatMessage(cCombatHelp[IDX(HELP_SKIP_UNIT)], 1, 0, 0);
                } else if (mouseX <= CONTROL_LEFT_MAX_X && mouseY < CONTROL_SYSTEM_OPTIONS_MIN_Y) {
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
                    } else if (HAS(message.payload.keyboard.modifiers,
                                   MESSAGE_MODIFIER_CONTROL_KEYS)) {
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
                            "\xc8\xb5P",
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
                            "\xd1\xf4\xe5\xf0\xe0 \xe0\xed\xf2\xe8\xec\xe0\xe3\xe8\xe8 \xe2 "
                            "\xe4\xe5\xe9\xf1\xf2\xe2\xe8\xe8. \xcd\xe5 \xec\xee\xe6\xe5\xf2 "
                            "\xe1\xfb\xf2\xfc \xed\xe0\xef\xf0\xe0\xe2\xeb\xe5\xed\xee \xed\xe8 "
                            "\xee\xe4\xed\xee \xe1\xee\xe5\xe2\xee\xe5 "
                            "\xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xe5." /* "Сфера антимагии в действии. Не может быть направлено ни одно боевое заклинание." */
                            ,
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
                            "\xc2\xfb \xf3\xe6\xe5 \xed\xe0\xef\xf0\xe0\xe2\xeb\xff\xeb\xe8 \xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xff \xe2 \xfd\xf2\xee\xec \xf0\xe0\xf3\xed\xe4\xe5.",
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

VA(0x0042d14d, 0x58)
i32 combatManager::IsNegationSphereInEffect(void) {
    CombatSide side;
    for (side = COMBAT_ATTACKER_SIDE; IDX(side) < COMBAT_SIDE_COUNT; side++) {
        if (m_heroes[IDX(side)] != NULL
            && m_heroes[IDX(side)]->HasArtifact(ARTIFACT_SPHERE_NEGATION) != 0)
            return 1;
    }
    return 0;
}

VA(0x0042d1a5, 0x1c4)
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
                currentArmy->m_monster.flags.abilityFlags &= MONSTER_FLAGS_ROUND_PERSISTENT_MASK;
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

VA(0x0042d369, 0x205)
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
        m_combatResult = m_sideRetreated[0] != 0 ? COMBAT_RESULT_DEFENDER : COMBAT_RESULT_ATTACKER;
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

VA(0x0042d56e, 0x3f6)
CombatMessageCommand combatManager::GetCommand(i32 hexIndex) {
    i32 column = hexIndex % COMBAT_GRID_ROW_LENGTH;
    i32 rowPos = hexIndex / COMBAT_GRID_ROW_LENGTH;
    CombatMessageCommand command = COMBAT_MESSAGE_COMMAND_DEFAULT;
    i32 showEnemy_12 = 0;
    CombatSide enemySide_27;
    i32 targetIndex;
    army* ourArmy_13;

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

            enemySide_27 = m_hexCells[hexIndex].m_occupantSide;
            targetIndex = m_hexCells[hexIndex].m_occupantIndex;
            ourArmy_13 = &m_armies[IDX(m_currentArmySide)][m_currentArmyIndex];
            ourArmy_13->m_targetSide = COMBAT_SIDE_NONE;
            ourArmy_13->m_targetIndex = -1;

            if (m_hexCells[hexIndex].m_blocked != 0
                && (gpCombatManager->m_inCastleCombat == 0
                    || (hexIndex != COMBAT_CASTLE_GATE_APPROACH_HEX && hexIndex != CASTLE_GATE_HEX)
                    || (gpCombatManager->m_drawbridgeState == COMBAT_CASTLE_GATE_OPEN
                        && (gpCombatManager->m_currentSide != COMBAT_DEFENDER_SIDE
                            || gpCombatManager->m_hexCells[COMBAT_CASTLE_GATE_APPROACH_HEX]
                                       .m_occupantSide
                                   != COMBAT_SIDE_NONE
                            || gpCombatManager->m_hexCells[COMBAT_CASTLE_GATE_APPROACH_HEX]
                                       .m_deadOccupantCount
                                   != 0)))) {
                command = COMBAT_MESSAGE_COMMAND_DEFAULT;
            } else if (enemySide_27 != COMBAT_SIDE_NONE) {
                if (enemySide_27 != m_currentArmySide || targetIndex != m_currentArmyIndex) {
                    showEnemy_12 = 1;
                    if (gbProcessingCombatAction == 0 && giNextAction == ACTION_NONE) {
                        m_smallViewSide[1] = enemySide_27;
                        m_smallViewArmyIndex[1] = targetIndex;
                        DrawSmallView(1, 1);
                    }
                }
                switch (enemySide_27) {
                    case COMBAT_ATTACKER_SIDE:
                    case COMBAT_DEFENDER_SIDE:
                        if (enemySide_27 == m_currentSide
                            || (enemySide_27 == m_currentArmySide
                                && targetIndex == m_currentArmyIndex)) {
                            return COMBAT_MESSAGE_COMMAND_VIEW_INFO;
                        } else {
                            ourArmy_13->m_targetSide = enemySide_27;
                            ourArmy_13->m_targetIndex = targetIndex;
                            if (ourArmy_13->m_monster.shots > 0
                                && ourArmy_13->GetAttackMask(
                                       ourArmy_13->m_hex,
                                       ARMY_ATTACK_TARGET_ENEMY,
                                       ARMY_HEX_INVALID
                                   ) == ARMY_ALL_ATTACK_DIRECTIONS) {
                                if (ShotIsThroughWall(
                                        ourArmy_13->m_side,
                                        ourArmy_13->m_hex,
                                        hexIndex
                                    )
                                    != 0)
                                    return COMBAT_MESSAGE_COMMAND_SHOOT_THROUGH_WALL;
                                else
                                    return COMBAT_MESSAGE_COMMAND_SHOOT;
                            }
                            if (ourArmy_13->ValidPath(hexIndex, ARMY_PATH_ANY_TARGET_HEX) == 1)
                                return COMBAT_MESSAGE_COMMAND_ATTACK;
                            else {
                                ourArmy_13->m_targetSide = COMBAT_SIDE_NONE;
                                ourArmy_13->m_targetIndex = -1;
                                command = COMBAT_MESSAGE_COMMAND_DEFAULT;
                            }
                        }
                        break;
                }
            } else {
                if (m_armies[IDX(m_currentArmySide)][m_currentArmyIndex]
                        .ValidPath(hexIndex, ARMY_PATH_ANY_TARGET_HEX)
                    == 1) {
                    command = CombatMessageCommand(
                        HAS(m_armies[IDX(m_currentArmySide)][m_currentArmyIndex]
                                .m_monster.flags.all,
                            MONSTER_FLAGS_FLYING)
                            ? static_cast<i8>(IDX(COMBAT_MESSAGE_COMMAND_FLY))
                            : static_cast<i8>(IDX(COMBAT_MESSAGE_COMMAND_MOVE))
                    );
                }
            }
            break;
        }
    }

smallView:
    if (showEnemy_12 == 0 && gbProcessingCombatAction == 0) {
        m_smallViewSide[1] = COMBAT_SIDE_NONE;
        DrawSmallView(1, 1);
    }
    return command;
}

VA(0x0042d964, 0x1eb)
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
            i32 armyIdx = m_hexCells[hexIndex].m_occupantIndex;
            if (m_hexCells[hexIndex].m_blocked != 0
                && (gpCombatManager->m_inCastleCombat == 0
                    || (hexIndex != COMBAT_CASTLE_GATE_APPROACH_HEX && hexIndex != CASTLE_GATE_HEX)
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
                                &m_armies[IDX(side)][m_hexCells[m_selectedHex].m_occupantIndex],
                                1
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

VA(0x0042db4f, 0x32e)
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
            if (m_playerId[IDX(m_currentSide)] == -1
                || gbHumanPlayer[m_playerId[IDX(m_currentSide)]] == 0
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
                    "\xd1\xf4\xe5\xf0\xe0 \xe0\xed\xf2\xe8\xec\xe0\xe3\xe8\xe8 \xe2 "
                    "\xe4\xe5\xe9\xf1\xf2\xe2\xe8\xe8. \xcd\xe5 \xec\xee\xe6\xe5\xf2 "
                    "\xe1\xfb\xf2\xfc \xed\xe0\xef\xf0\xe0\xe2\xeb\xe5\xed\xee \xed\xe8 "
                    "\xee\xe4\xed\xee \xe1\xee\xe5\xe2\xee\xe5 "
                    "\xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xe5." /* "Сфера антимагии в действии. Не может быть направлено ни одно боевое заклинание." */
                    ,
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
                "\xc2\xfb \xe4\xe5\xe9\xf1\xf2\xe2\xe8\xf2\xe5\xeb\xfc\xed\xee "
                "\xf5\xee\xf2\xe8\xf2\xe5 \xee\xf2\xf1\xf2\xf3\xef\xe8\xf2\xfc?" /* "Вы действительно хотите отступить?" */
                ,
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
                        "\xd3 \xe2\xe0\xf1 \xed\xe5\xe4\xee\xf1\xf2\xe0\xf2\xee\xf7\xed\xee "
                        "\xe7\xee\xeb\xee\xf2\xe0!" /* "У вас недостаточно золота!" */,
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

VA(0x0042de7d, 0x4f3)
MessageDispatchResult WinCombatHandler(struct tag_message& message) {
    char iconFile_3[WIN_LOSE_ICON_FILENAME_SIZE];
    tag_message animationMessage;
    i32 finalDelay = WIN_LOSE_INITIAL_DELAY;
    i32 frame;
    i32 iDelay_3;

    if (giDialogTimeout != 0 && KBTickCount() > giDialogTimeout) {
        message.type = MESSAGE_WIDGET;
        gpWindowManager->m_dialogResult = message.payload.widget.id;
        message.payload.widget.id = WIN_LOSE_CLOSE_COMMAND;
        message.payload.widget.command = BaseWidgetCommand(WIN_LOSE_CLOSE_COMMAND);
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
                            if (gpCombatManager
                                    ->m_eagleEyeSpell[IDX(gpCombatManager->m_combatResult)]
                                != SPELL_NONE) {
                                gpCombatManager->ClearWinLoseBottom(
                                    gpCombatManager->m_winLoseWindow
                                );
                                gpCombatManager->ShowEagleEyeSpell(
                                    gpCombatManager->m_winLoseWindow
                                );
                                gpCombatManager
                                    ->m_eagleEyeSpell[IDX(gpCombatManager->m_combatResult)] =
                                    SPELL_NONE;
                            } else {
                            ExitDialog:
                                gpWindowManager->m_dialogResult = message.payload.widget.id;
                                message.payload.widget.id = WIN_LOSE_CLOSE_COMMAND;
                                message.payload.widget.command =
                                    BaseWidgetCommand(WIN_LOSE_CLOSE_COMMAND);
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

    if (glTimers[0] < KBTickCount()) {
        animationMessage.type = MESSAGE_WIDGET;
        animationMessage.payload.widget.command = COMBAT_WIN_LOSE_RESOURCE_COMMAND;
        animationMessage.payload.widget.data.text = iconFile_3;
        ++giWinCmbtFrame;

        switch (gbWhichAnimationPlaying) {
            case WIN_LOSE_ANIMATION_CYCLE_FIRST:
                frame = giWinCmbtFrame % WIN_LOSE_LOOP_FRAME_COUNT + WIN_LOSE_FIRST_ANIMATION_FRAME;
                iDelay_3 = WIN_LOSE_LOOP_DELAY;
                break;
            case WIN_LOSE_ANIMATION_CYCLE_SECOND:
                frame = giWinCmbtFrame % WIN_LOSE_LOOP_FRAME_COUNT + WIN_LOSE_FIRST_ANIMATION_FRAME;
                iDelay_3 = WIN_LOSE_LOOP_DELAY;
                break;
            case WIN_LOSE_ANIMATION_FLEE:
                if (giWinCmbtFrame == WIN_LOSE_FLEE_SECOND_RESOURCE_FRAME) {
                    sprintf(iconFile_3, "cmbtfle2.icn");
                    animationMessage.payload.widget.id = WIN_LOSE_RESOURCE_LOAD_ID;
                    gpCombatManager->m_winLoseWindow->BroadcastMessage(animationMessage);
                    animationMessage.payload.widget.id = WIN_LOSE_RESOURCE_DRAW_ID;
                    gpCombatManager->m_winLoseWindow->BroadcastMessage(animationMessage);
                }
                if (giWinCmbtFrame == WIN_LOSE_FLEE_THIRD_RESOURCE_FRAME) {
                    sprintf(iconFile_3, "cmbtfle3.icn");
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
                iDelay_3 = WIN_LOSE_FLEE_DELAY;
                break;
            default:
                if (giWinCmbtFrame == WIN_LOSE_LOSS_SECOND_RESOURCE_FRAME) {
                    sprintf(iconFile_3, "cmbtlos2.icn");
                    animationMessage.payload.widget.id = WIN_LOSE_RESOURCE_LOAD_ID;
                    gpCombatManager->m_winLoseWindow->BroadcastMessage(animationMessage);
                    animationMessage.payload.widget.id = WIN_LOSE_RESOURCE_DRAW_ID;
                    gpCombatManager->m_winLoseWindow->BroadcastMessage(animationMessage);
                }
                if (giWinCmbtFrame == WIN_LOSE_LOSS_THIRD_RESOURCE_FRAME) {
                    sprintf(iconFile_3, "cmbtlos3.icn");
                    animationMessage.payload.widget.id = WIN_LOSE_RESOURCE_LOAD_ID;
                    gpCombatManager->m_winLoseWindow->BroadcastMessage(animationMessage);
                    animationMessage.payload.widget.id = WIN_LOSE_RESOURCE_DRAW_ID;
                    gpCombatManager->m_winLoseWindow->BroadcastMessage(animationMessage);
                }
                if (giWinCmbtFrame < WIN_LOSE_LOSS_SECOND_RESOURCE_FRAME) {
                    frame = giWinCmbtFrame + 1;
                    iDelay_3 = WIN_LOSE_LOSS_DELAY;
                } else if (giWinCmbtFrame < WIN_LOSE_LOSS_THIRD_RESOURCE_FRAME) {
                    frame = giWinCmbtFrame - (WIN_LOSE_LOSS_SECOND_RESOURCE_FRAME - 1);
                    iDelay_3 = WIN_LOSE_LOSS_DELAY;
                } else {
                    if (giWinCmbtFrame == WIN_LOSE_LOSS_RESET_FRAME)
                        giWinCmbtFrame = WIN_LOSE_LOSS_THIRD_RESOURCE_FRAME;
                    frame = giWinCmbtFrame - (WIN_LOSE_LOSS_THIRD_RESOURCE_FRAME - 1);
                    iDelay_3 = WIN_LOSE_LOSS_FINAL_DELAY;
                }
                break;
        }

        message.type = MESSAGE_WIDGET;
        message.payload.widget.command = COMBAT_WIN_LOSE_ANIMATION_COMMAND;
        message.payload.widget.id = WIN_LOSE_RESOURCE_DRAW_ID;
        message.payload.widget.data.value = frame;
        gpCombatManager->m_winLoseWindow->BroadcastMessage(message);
        gpCombatManager->m_winLoseWindow->DrawWindow(1, 0, WIN_LOSE_DRAW_DEPTH);
        glTimers[0] = KBTickCount() + iDelay_3;
    }
    return MESSAGE_DISPATCH_CONSUME;
}

VA(0x0042e370, 0x108)
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

VA(0x0042e478, 0x29d)
void combatManager::ShowWinLoseArtifact(
    class heroWindow* window,
    H2_ENUM_PARAM(ArtifactType, i32) artifact
) {
    i16 width = CASUALTY_WINDOW_WIDTH;
    i16 bottom_4 = CASUALTY_WINDOW_BOTTOM;
    tag_message message;
    char* artifactName;

    sprintf(gText, "\xc2\xfb \xe7\xe0\xf5\xe2\xe0\xf2\xe8\xeb\xe8 \xe2\xf0\xe0\xe6\xe5\xf1\xea\xe8\xe9 \xe0\xf0\xf2\xe5\xf4\xe0\xea\xf2!");
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
        "winloseb.icn",
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
        "artifact.icn",
        IDX(artifact) + 1,
        ICON_DRAW_NORMAL,
        WIN_LOSE_ARTIFACT_IMAGE_ID,
        WIDGET_KIND_ICON_DIRECT,
        1
    );
    if (m_winLoseBottomWidgets[1] == NULL)
        MemError();
    window->AddWidget(m_winLoseBottomWidgets[1], -1);

    artifactName = static_cast<char*>(H2_ALLOC(ARTIFACT_NAME_CAPACITY));
    sprintf(artifactName, gArtifactNames[IDX(artifact)]);
    m_winLoseBottomTextWidgets[0] = new textWidget(
        ARTIFACT_TEXT_X,
        ARTIFACT_TEXT_Y,
        CASUALTY_WINDOW_WIDTH,
        ARTIFACT_TEXT_HEIGHT,
        artifactName,
        "smalfont.fnt",
        FONT_DRAW_DEFAULT,
        WIN_LOSE_ARTIFACT_TEXT_ID,
        WIDGET_KIND_TEXT,
        FONT_ALIGN_CENTER
    );
    if (m_winLoseBottomTextWidgets[0] == NULL)
        MemError();
    window->AddWidget(m_winLoseBottomTextWidgets[0], -1);

    gpCombatManager->m_winLoseWindow->DrawWindow();
    SAMPLE2 playSample;
    sprintf(gText, "pickup%02d.82M", SRandom(PICKUP_SAMPLE_FIRST, PICKUP_SAMPLE_LAST));
    playSample = LoadPlaySample(gText);
    WaitEndSample(&playSample, -1);
}

VA(0x0042e715, 0x225)
void combatManager::ShowSkeletons(class heroWindow* window) {
    tag_message message;
    char* skeletonText;

    m_winLoseBottomWidgets[0] = new iconWidget(
        SKELETON_ICON_X,
        SKELETON_ICON_Y,
        SKELETON_ICON_WIDTH,
        SKELETON_ICON_HEIGHT,
        "mons32.icn",
        IDX(CREATURE_SKELETON),
        ICON_DRAW_NORMAL,
        WIN_LOSE_SKELETON_ICON_ID,
        WIDGET_KIND_ICON_DIRECT,
        1
    );
    if (m_winLoseBottomWidgets[0] == NULL)
        MemError();

    skeletonText = static_cast<char*>(H2_ALLOC(SKELETON_COUNT_CAPACITY));
    sprintf(skeletonText, "%d", giSkeletonsCreated);
    m_winLoseBottomTextWidgets[0] = new textWidget(
        SKELETON_TEXT_X,
        SKELETON_TEXT_Y,
        SKELETON_TEXT_WIDTH,
        SKELETON_TEXT_HEIGHT,
        skeletonText,
        "smalfont.fnt",
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
            "\xc8\xf1\xef\xee\xeb\xfc\xe7\xee\xe2\xe0\xed\xe8\xe5 \xf7\xe5\xf0\xed\xee\xe9 "
            "\xec\xe0\xe3\xe8\xe8 \xed\xe5\xea\xf0\xee\xec\xe0\xed\xf2\xe8\xe8 "
            "\xef\xee\xe7\xe2\xee\xeb\xe8\xeb\xee \xe2\xe0\xec \xe2\xe5\xf0\xed\xf3\xf2\xfc "
            "\xed\xe0 \xf1\xeb\xf3\xe6\xe1\xf3 %d "
            "\xef\xee\xe2\xe5\xf0\xe6\xe5\xed\xed\xfb\xf5 \xe2\xf0\xe0\xe3\xee\xe2 \xe2 "
            "\xe2\xe8\xe4\xe5 \xf1\xea\xe5\xeb\xe5\xf2\xee\xe2." /* "Использование черной магии
                                                                    некромантии позволило вам
                                                                    вернуть на службу %d
                                                                    поверженных врагов в виде
                                                                    скелетов." */,
            giSkeletonsCreated
        );
    } else {
        sprintf(
            gText,
            "\xc8\xf1\xef\xee\xeb\xfc\xe7\xee\xe2\xe0\xed\xe8\xe5 \xf7\xe5\xf0\xed\xee\xe9 "
            "\xec\xe0\xe3\xe8\xe8 \xed\xe5\xea\xf0\xee\xec\xe0\xed\xf2\xe8\xe8 "
            "\xef\xee\xe7\xe2\xee\xeb\xe8\xeb\xee \xe2\xe0\xec \xe2\xe5\xf0\xed\xf3\xf2\xfc "
            "\xed\xe0 \xf1\xeb\xf3\xe6\xe1\xf3 "
            "\xef\xee\xe2\xe5\xf0\xe6\xe5\xed\xed\xee\xe3\xee \xe2\xf0\xe0\xe3\xe0 \xe2 "
            "\xe2\xe8\xe4\xe5 \xf1\xea\xe5\xeb\xe5\xf2\xe0." /* "Использование черной магии
                                                                некромантии позволило вам вернуть
                                                                на службу поверженного врага в
                                                                виде скелета." */
        );
    }
    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = COMBAT_WIN_LOSE_TEXT_COMMAND;
    message.payload.widget.id = WIN_LOSE_TEXT_ID;
    message.payload.widget.data.text = gText;
    m_winLoseWindow->BroadcastMessage(message);
    gpCombatManager->m_winLoseWindow->DrawWindow();

    SAMPLE2 playSample;
    sprintf(gText, "pickup%02d.82M", SRandom(PICKUP_SAMPLE_FIRST, PICKUP_SAMPLE_LAST));
    playSample = LoadPlaySample(gText);
    WaitEndSample(&playSample, -1);
}

VA(0x0042e93a, 0x2f2)
void combatManager::ShowEagleEyeSpell(class heroWindow* window) {
    SpellType newSpell = m_eagleEyeSpell[IDX(m_combatResult)];
    i32 x = EAGLE_PANEL_X;
    i32 y = EAGLE_PANEL_Y;
    tag_message message;
    char* nameText;

    m_winLoseBottomWidgets[0] = new iconWidget(
        x + EAGLE_BACKGROUND_X_OFFSET,
        y,
        0,
        0,
        "townwind.icn",
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
        "spells.icn",
        static_cast<i16>(gsSpellInfo[IDX(newSpell)].iconIndex),
        ICON_DRAW_NORMAL,
        WIN_LOSE_EAGLE_SPELL_ID,
        WIDGET_KIND_ICON_CENTERED,
        1
    );
    if (m_winLoseBottomWidgets[1] == NULL)
        MemError();

    nameText = static_cast<char*>(H2_ALLOC(EAGLE_SPELL_NAME_CAPACITY));
    sprintf(nameText, "%s", gSpellNames[IDX(newSpell)]);
    m_winLoseBottomTextWidgets[0] = new textWidget(
        x + EAGLE_TEXT_X_OFFSET,
        y + EAGLE_TEXT_Y_OFFSET,
        EAGLE_TEXT_WIDTH,
        EAGLE_TEXT_HEIGHT,
        nameText,
        "smalfont.fnt",
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
        "\xd1 \xef\xee\xec\xee\xf9\xfc\xfe \xe8\xf1\xea\xf3\xf1\xf1\xf2\xe2\xe0 "
        "\xee\xf0\xeb\xe8\xed\xee\xe3\xee \xe2\xe7\xee\xf0\xe0, %s "
        "\xf1\xec\xee\xe3(\xeb\xe0) \xef\xee\xe4\xe3\xeb\xff\xe4\xe5\xf2\xfc \xe8 "
        "\xe2\xfb\xf3\xf7\xe8\xf2\xfc \xe7\xe0\xea\xeb\xe8\xed\xe0\xed\xe8\xe5 "
        "'%s'." /* "С помощью искусства орлиного взора, %s
                   смог(ла) подглядеть и выучить заклинание '%s'." */,
        m_heroes[IDX(m_combatResult)]->m_name,
        gSpellNames[IDX(newSpell)]
    );
    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = COMBAT_WIN_LOSE_TEXT_COMMAND;
    message.payload.widget.id = WIN_LOSE_TEXT_ID;
    message.payload.widget.data.text = gText;
    m_winLoseWindow->BroadcastMessage(message);
    gpCombatManager->m_winLoseWindow->DrawWindow();

    SAMPLE2 playSample;
    sprintf(gText, "pickup%02d.82M", SRandom(PICKUP_SAMPLE_FIRST, PICKUP_SAMPLE_LAST));
    playSample = LoadPlaySample(gText);
    WaitEndSample(&playSample, -1);
}

VA(0x0042ec2c, 0x9d5)
void combatManager::ShowDeadArmies(class heroWindow* window) {
    i16 width = CASUALTY_WINDOW_WIDTH;
    i32 casualtyType_1[COMBAT_SIDE_COUNT][COMBAT_ARMY_SLOT_COUNT];
    i32 spacing_6;
    i32 y_7;
    i16 bottom_4 = CASUALTY_WINDOW_BOTTOM;
    i32 casualtyCount[COMBAT_SIDE_COUNT][COMBAT_ARMY_SLOT_COUNT];
    i32 side_4;
    i32 unusedCasualtyWord116;
    i32 unusedCasualtyWord92;
    i32 unusedCasualtyWord86;
    i32 unusedCasualtyWord52;
    i32 unusedCasualtyWord46;
    i32 unusedCasualtyWord13;
    i32 unusedCasualtyWord0;
    i32 casualtyQuantity_13[COMBAT_SIDE_COUNT];
    i32 armyIndex;
    i32 displayedCount;
    i32 startX_0;
    char* text_1;
    icon* monsterIcons;

    for (side_4 = 0; side_4 < WIN_LOSE_WIDGET_COUNT; ++side_4) {
        m_winLoseBottomWidgets[side_4] = NULL;
        m_winLoseBottomTextWidgets[side_4] = NULL;
    }
    for (side_4 = 0; side_4 < COMBAT_SIDE_COUNT; ++side_4) {
        casualtyQuantity_13[side_4] = 0;
        for (armyIndex = 0; armyIndex < COMBAT_ARMY_SLOT_COUNT; ++armyIndex) {
            if (m_armies[side_4][armyIndex].m_monsterType != CREATURE_NONE
                && m_armies[side_4][armyIndex].m_initialQuantity
                       > m_armies[side_4][armyIndex].m_quantity) {
                casualtyType_1[side_4][casualtyQuantity_13[side_4]] =
                    IDX(m_armies[side_4][armyIndex].m_monsterType);
                casualtyCount[side_4][casualtyQuantity_13[side_4]] =
                    m_armies[side_4][armyIndex].m_initialQuantity
                    - m_armies[side_4][armyIndex].m_quantity;
                ++casualtyQuantity_13[side_4];
            }
        }
    }

    text_1 = static_cast<char*>(H2_ALLOC(CASUALTY_HEADING_CAPACITY));
    sprintf(text_1, "\xcf\xee\xf2\xe5\xf0\xe8");
    m_winLoseBottomTextWidgets[CASUALTY_TITLE_WIDGET] = new textWidget(
        CASUALTY_TITLE_X,
        CASUALTY_TITLE_Y,
        CASUALTY_WINDOW_WIDTH,
        CASUALTY_TEXT_HEIGHT,
        text_1,
        "smalfont.fnt",
        FONT_DRAW_DEFAULT,
        CASUALTY_TEXT_WIDGET_ID,
        WIDGET_KIND_TEXT,
        FONT_ALIGN_CENTER
    );
    if (m_winLoseBottomTextWidgets[CASUALTY_TITLE_WIDGET] == NULL)
        MemError();
    window->AddWidget(m_winLoseBottomTextWidgets[CASUALTY_TITLE_WIDGET], -1);

    for (side_4 = 0; side_4 < COMBAT_SIDE_COUNT; ++side_4) {
        y_7 = side_4 ? CASUALTY_DEFENDER_Y : CASUALTY_ATTACKER_Y;
        text_1 = static_cast<char*>(H2_ALLOC(CASUALTY_HEADING_CAPACITY));
        sprintf(
            text_1,
            side_4 == IDX(COMBAT_ATTACKER_SIDE)
                ? "\xcd\xe0\xef\xe0\xe4\xe0\xe2\xf8\xe8\xe5:" /* "Нападавшие:" */
                : "\xce\xe1\xee\xf0\xee\xed\xff\xe2\xf8\xe8\xe5\xf1\xff:" /* "Оборонявшиеся:" */
        );
        m_winLoseBottomTextWidgets[CASUALTY_SIDE_WIDGET_FIRST + side_4] = new textWidget(
            CASUALTY_TITLE_X,
            y_7 + CASUALTY_SIDE_LABEL_Y_OFFSET,
            CASUALTY_WINDOW_WIDTH,
            CASUALTY_TEXT_HEIGHT,
            text_1,
            "smalfont.fnt",
            FONT_DRAW_DEFAULT,
            CASUALTY_TEXT_WIDGET_ID,
            WIDGET_KIND_TEXT,
            FONT_ALIGN_CENTER
        );
        if (m_winLoseBottomTextWidgets[CASUALTY_SIDE_WIDGET_FIRST + side_4] == NULL)
            MemError();
        window->AddWidget(m_winLoseBottomTextWidgets[CASUALTY_SIDE_WIDGET_FIRST + side_4], -1);

        if (casualtyQuantity_13[side_4] <= 0) {
            text_1 = static_cast<char*>(H2_ALLOC(CASUALTY_NONE_CAPACITY));
            sprintf(text_1, "\xcd\xe5\xf2" /* "Нет" */);
            m_winLoseBottomTextWidgets[side_4 * CASUALTY_WIDGETS_PER_SIDE] = new textWidget(
                CASUALTY_TITLE_X,
                y_7 + CASUALTY_NONE_Y_OFFSET,
                CASUALTY_WINDOW_WIDTH,
                CASUALTY_TEXT_HEIGHT,
                text_1,
                "smalfont.fnt",
                FONT_DRAW_DEFAULT,
                side_4 * CASUALTY_WIDGET_ID_STRIDE + CASUALTY_TEXT_WIDGET_ID_FIRST,
                WIDGET_KIND_TEXT,
                FONT_ALIGN_CENTER
            );
            if (m_winLoseBottomTextWidgets[side_4 * CASUALTY_WIDGETS_PER_SIDE] == NULL)
                MemError();
            window->AddWidget(m_winLoseBottomTextWidgets[side_4 * CASUALTY_WIDGETS_PER_SIDE], -1);
        }

        monsterIcons = gpResourceManager->GetIcon("mons32.icn");
        displayedCount = casualtyQuantity_13[side_4] < CASUALTY_DISPLAY_LIMIT
                                ? casualtyQuantity_13[side_4]
                                : CASUALTY_DISPLAY_LIMIT;
        spacing_6 = CASUALTY_ICON_SPACING;
        startX_0 = (CASUALTY_WINDOW_WIDTH - spacing_6 * displayedCount) / CASUALTY_CENTER_DIVISOR
                   + CASUALTY_CENTER_X_OFFSET;
        for (armyIndex = 0; armyIndex < displayedCount; ++armyIndex) {
            m_winLoseBottomWidgets[side_4 * CASUALTY_WIDGETS_PER_SIDE + armyIndex] =
                new iconWidget(
                    startX_0 + spacing_6 * armyIndex + CASUALTY_ICON_CELL_WIDTH / 2
                        - GetIconEntry(monsterIcons, casualtyType_1[side_4][armyIndex])->x
                        + (CASUALTY_ICON_CELL_WIDTH
                           - GetIconEntry(monsterIcons, casualtyType_1[side_4][armyIndex])->w)
                              / CASUALTY_CENTER_DIVISOR
                        + 1,
                    y_7 + CASUALTY_ICON_TOP_OFFSET
                        - GetIconEntry(monsterIcons, casualtyType_1[side_4][armyIndex])->y
                        + (CASUALTY_ICON_CELL_WIDTH
                           - GetIconEntry(monsterIcons, casualtyType_1[side_4][armyIndex])->h),
                    CASUALTY_ICON_WIDTH,
                    CASUALTY_ICON_HEIGHT,
                    "mons32.icn",
                    casualtyType_1[side_4][armyIndex],
                    ICON_DRAW_NORMAL,
                    side_4 * CASUALTY_WIDGET_ID_STRIDE + armyIndex
                        + CASUALTY_ICON_WIDGET_ID_FIRST,
                    WIDGET_KIND_ICON_DIRECT,
                    1
                );
            if (m_winLoseBottomWidgets[side_4 * CASUALTY_WIDGETS_PER_SIDE + armyIndex] == NULL)
                MemError();

            text_1 = static_cast<char*>(H2_ALLOC(CASUALTY_QUANTITY_CAPACITY));
            sprintf(text_1, "%d", casualtyCount[side_4][armyIndex]);
            m_winLoseBottomTextWidgets[side_4 * CASUALTY_WIDGETS_PER_SIDE + armyIndex] =
                new textWidget(
                    startX_0 + spacing_6 * armyIndex + CASUALTY_QUANTITY_X_OFFSET,
                    y_7 + CASUALTY_QUANTITY_Y_OFFSET,
                    CASUALTY_QUANTITY_WIDTH,
                    CASUALTY_QUANTITY_HEIGHT,
                    text_1,
                    "smalfont.fnt",
                    FONT_DRAW_DEFAULT,
                    side_4 * CASUALTY_WIDGET_ID_STRIDE + armyIndex
                        + CASUALTY_TEXT_WIDGET_ID_FIRST,
                    WIDGET_KIND_TEXT,
                    FONT_ALIGN_CENTER
                );
            if (m_winLoseBottomTextWidgets[side_4 * CASUALTY_WIDGETS_PER_SIDE + armyIndex]
                == NULL)
                MemError();
            window->AddWidget(
                m_winLoseBottomWidgets[side_4 * CASUALTY_WIDGETS_PER_SIDE + armyIndex],
                -1
            );
            window->AddWidget(
                m_winLoseBottomTextWidgets[side_4 * CASUALTY_WIDGETS_PER_SIDE + armyIndex],
                -1
            );
        }
        gpResourceManager->Dispose(monsterIcons);
    }
}

VA(0x0042f601, 0xb17)
void combatManager::DoVictory(H2_ENUM_PARAM(CombatResult, i32) winningSide) {
    army* pTroop;
    i32 numFades;
    i32 bodies;
    i32 levelsGained = 0;
    i32 livingCount;
    i32 lastLivingArmy;
    i32 troopIndex;
    i32 loop;
    i32 waitTimer;
    tag_message message;
    CombatSide combatSide;
    i32 cost;
    i32 necroEligible;
    i32 emptySlots;
    char experienceText[VICTORY_EXPERIENCE_TEXT_SIZE];

    if (m_heroes[IDX(COMBAT_DEFENDER_SIDE)] != NULL
        && m_heroes[IDX(COMBAT_DEFENDER_SIDE)]->m_isCaptain != 0)
        m_heroes[IDX(COMBAT_DEFENDER_SIDE)] = NULL;
    gbShowingLoseWindow = false;
    gbWhichAnimationPlaying = true;
    giWinCmbtFrame = 0;
    giSkeletonsCreated = 0;
    iMaxTransferArtifacts = 0;
    iCurTransferArtifact = -1;
    bSkeletonsShown = 0;
    bodies = 0;
    necroEligible = 0;

    for (combatSide = COMBAT_ATTACKER_SIDE; IDX(combatSide) < COMBAT_SIDE_COUNT; ++combatSide) {
        livingCount = 0;
        lastLivingArmy = -1;
        for (troopIndex = 0; troopIndex < gpCombatManager->m_armyCount[IDX(combatSide)];
             ++troopIndex) {
            pTroop = &m_armies[IDX(combatSide)][troopIndex];
            if (pTroop->m_quantity > 0) {
                lastLivingArmy = troopIndex;
                if (pTroop->m_temporaryResurrectionQuantity > 0)
                    pTroop->m_quantity -= pTroop->m_temporaryResurrectionQuantity;
                if (pTroop->m_quantity < 0)
                    pTroop->m_quantity = 0;
                livingCount += pTroop->m_quantity;
            }
            if (CombatResultForSide(combatSide) == winningSide && pTroop->m_quantity > 0
                && HAS(pTroop->m_monster.flags.all, MONSTER_FLAGS_LIGHT_PALETTE) == 0
                && pTroop->m_monsterType != CREATURE_EARTH_ELEMENTAL
                && pTroop->m_monsterType != CREATURE_AIR_ELEMENTAL
                && pTroop->m_monsterType != CREATURE_FIRE_ELEMENTAL
                && pTroop->m_monsterType != CREATURE_WATER_ELEMENTAL
                && pTroop->m_monsterType != CREATURE_SKELETON) {
                ++necroEligible;
            }
            if (OppositeCombatResult(CombatResultForSide(combatSide)) == winningSide) {
                bodies += pTroop->m_initialQuantity - pTroop->m_quantity;
            }
        }
        if (livingCount == 0 && lastLivingArmy != -1)
            m_armies[IDX(combatSide)][lastLivingArmy].m_quantity = 1;
    }

    if (winningSide != COMBAT_RESULT_DRAW && necroEligible < VICTORY_NECROMANCY_STACK_LIMIT
        && m_heroes[IDX(winningSide)] != NULL
        && m_heroes[IDX(winningSide)]->GetSSLevel(HERO_SKILL_NECROMANCY) != 0) {
        giSkeletonsCreated = static_cast<i32>(
            bodies
            * (m_heroes[IDX(winningSide)]->GetSSLevel(HERO_SKILL_NECROMANCY)
               * COMBAT_NECROMANCY_LEVEL_FACTOR)
        );
        if (giSkeletonsCreated <= 0 && bodies != 0)
            giSkeletonsCreated = 1;
    }

    m_nonVisualCombat = 1;
    FreeArmies();
    CombatMessage("", 1, 1, 0);
    gpMouseManager->SetPointer(COMBAT_POINTER_DEFAULT);
    numFades = VICTORY_FADE_STEPS;
    if (m_terrainType == TERRAIN_WASTELAND)
        numFades = VICTORY_WASTELAND_FADE_STEPS;
    waitTimer = KBTickCount();
    for (loop = 0; loop < numFades; ++loop) {
        PollSound();
        DelayTil(&waitTimer);
        waitTimer = KBTickCount() + VICTORY_FADE_DELAY;
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
                    m_heroes[IDX(winningSide)]->m_spells[IDX(m_eagleEyeSpell[IDX(winningSide)])] =
                        1;
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
                levelsGained = gpAdvManager->GiveExperience(
                    m_heroes[IDX(winningSide)],
                    m_experienceValue[IDX(OppositeCombatResult(winningSide))],
                    gbThisNetHumanPlayer[m_heroes[IDX(winningSide)]->m_owner] == 0
                );

                if (gbRetreatWin == 0) {
                    emptySlots = 0;
                    if (m_heroes[IDX(COMBAT_ATTACKER_SIDE)] != NULL
                        && m_heroes[IDX(COMBAT_DEFENDER_SIDE)] != NULL) {
                        for (loop = 0; loop < HERO_ARTIFACT_SLOT_COUNT; ++loop) {
                            if (m_heroes[IDX(winningSide)]->m_artifacts[loop]
                                == ARTIFACT_NONE) {
                                ++emptySlots;
                            }
                        }
                        for (loop = 0; loop < HERO_ARTIFACT_SLOT_COUNT; ++loop) {
                            if (m_heroes[IDX(OppositeCombatResult(winningSide))]
                                        ->m_artifacts[loop]
                                    >= ARTIFACT_ARCANE_NECKLACE
                                && m_heroes[IDX(OppositeCombatResult(winningSide))]
                                           ->m_artifacts[loop]
                                       != ARTIFACT_MAGIC_BOOK
                                && iMaxTransferArtifacts < emptySlots) {
                                iTransferArtifacts[iMaxTransferArtifacts] =
                                    m_heroes[IDX(OppositeCombatResult(winningSide))]
                                        ->m_artifacts[loop];
                                iTransferArtifactsInfo[iMaxTransferArtifacts] =
                                    m_heroes[IDX(OppositeCombatResult(winningSide))]
                                        ->m_artifactExtra[loop];
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
                m_winLoseWindow =
                    new heroWindow(WIN_LOSE_WINDOW_X, WIN_LOSE_WINDOW_Y, "wincmbt.bin");
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
                    if (levelsGained > 0 && winningSide == COMBAT_RESULT_DEFENDER
                        && giNumHumanPlayers > 1) {
                        sprintf(
                            experienceText,
                            cBattleResults[IDX(RESULT_TEXT_NETWORK_EXPERIENCE)],
                            m_heroes[IDX(winningSide)]->m_name,
                            m_experienceValue[IDX(OppositeCombatResult(winningSide))],
                            levelsGained
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

VA(0x00430118, 0x383)
void combatManager::DoLoseWindow(void) {
    CombatResult losingSide;
    i32 unusedLoseWord = IDX(COMBAT_RESULT_ATTACKER);
    tag_message message;
    char animationFile[WIN_LOSE_ANIMATION_FILENAME_SIZE];

    if (giCurPlayer == m_playerId[IDX(COMBAT_ATTACKER_SIDE)]
        && gbThisNetHumanPlayer[m_playerId[IDX(COMBAT_ATTACKER_SIDE)]] != 0) {
        losingSide = COMBAT_RESULT_ATTACKER;
    } else if (giCurPlayer == m_playerId[IDX(COMBAT_DEFENDER_SIDE)]
               && gbThisNetHumanPlayer[m_playerId[IDX(COMBAT_DEFENDER_SIDE)]] != 0) {
        losingSide = COMBAT_RESULT_DEFENDER;
    } else if (m_playerId[IDX(COMBAT_ATTACKER_SIDE)] != -1
               && gbThisNetHumanPlayer[m_playerId[IDX(COMBAT_ATTACKER_SIDE)]] != 0) {
        losingSide = COMBAT_RESULT_ATTACKER;
    } else {
        losingSide = COMBAT_RESULT_DEFENDER;
    }

    gbShowingLoseWindow = true;
    if (gbCombatSurrender != 0) {
        sprintf(animationFile, "cmbtsurr.icn");
        gbWhichAnimationPlaying = WIN_LOSE_ANIMATION_CYCLE_SECOND;
    } else if (gbRetreatWin != 0) {
        sprintf(animationFile, "cmbtfle1.icn");
        gbWhichAnimationPlaying = WIN_LOSE_ANIMATION_FLEE;
    } else {
        sprintf(animationFile, "cmbtlos1.icn");
        gbWhichAnimationPlaying = WIN_LOSE_ANIMATION_LOSS;
    }

    m_winLoseWindow = new heroWindow(WIN_LOSE_WINDOW_X, WIN_LOSE_WINDOW_Y, "wincmbt.bin");
    if (m_winLoseWindow == NULL)
        MemError();

    if (m_heroes[IDX(losingSide)] != NULL) {
        if (gbCombatSurrender != 0) {
            sprintf(
                gText,
                cBattleResults[IDX(RESULT_TEXT_HERO_SURRENDER)],
                m_heroes[IDX(losingSide)]->m_name
            );
        } else if (gbRetreatWin != 0) {
            sprintf(
                gText,
                cBattleResults[IDX(RESULT_TEXT_HERO_FLEE)],
                m_heroes[IDX(losingSide)]->m_name
            );
        } else {
            sprintf(
                gText,
                cBattleResults[IDX(RESULT_TEXT_HERO_DEFEAT)],
                m_heroes[IDX(losingSide)]->m_name
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
    message.payload.widget.data.text = animationFile;
    m_winLoseWindow->BroadcastMessage(message);
    message.payload.widget.id = WIN_LOSE_RESOURCE_DRAW_ID;
    message.payload.widget.data.text = animationFile;
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

VA(0x0043049b, 0x3bd)
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
        * (1.0
           - IDX(m_heroes[IDX(m_currentSide)]->m_secondarySkills[IDX(HERO_SKILL_DIPLOMACY)])
                 * COMBAT_SURRENDER_DIPLOMACY_FACTOR)
    );

    dialogType = SURRENDER_DIALOG_TYPE;
    dialogResult = SURRENDER_DIALOG_ACCEPT_RESULT;
    textWidth_t = SURRENDER_TEXT_WIDTH;
    window = new heroWindow(SURRENDER_WINDOW_X, SURRENDER_WINDOW_Y, "surrendr.bin");
    if (window == NULL)
        MemError();
    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = COMBAT_WIN_LOSE_RESOURCE_COMMAND;
    message.payload.widget.id = SURRENDER_PORTRAIT_RESOURCE_ID;
    sprintf(
        gText,
        "port%04d.icn",
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
        "%s states:\n\n\"I will accept your surrender and grant you and your troops safe passage "
        "for the price of %d gold.\"",
        m_heroes[IDX(OppositeCombatSide(m_currentSide))]->m_name,
        giSurrenderCost
    );
    window->BroadcastMessage(message);
    gpWindowManager->DoDialog(window, TrueFalseDialogHandler, 0);
    delete window;
    return gpWindowManager->m_dialogResult == NORMAL_DIALOG_BUTTON_TWO;
}

VA(0x00430858, 0xb5)
void combatManager::CheckChangeSelector(void) {
    if (gbNoShowCombat != 0)
        return;
    army* currentArmy = &m_armies[IDX(m_currentArmySide)][m_currentArmyIndex];
    if (m_limitCreature == 0 || m_limitCreatureHex != currentArmy->m_hex) {
        UpdateGrid(0, 1);
        giNewMonsterCycleFrame = SELECTOR_INITIAL_FRAME;
        m_limitCreatureHex = currentArmy->m_hex;
        m_limitCreature = 1;
        DrawFrame(1, 0, 0, 0, COMMAND_FRAME_DELAY, 1, 1);
    }
    SetupSmallView();
}

VA(0x0043090d, 0xed)
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

VA(0x004309fa, 0xb6)
void combatManager::CheckGetAIMove(void) {
    i32 retreat = AICheckRetreat();
    if (m_heroCastSpell[IDX(m_currentSide)] == 0
        && (m_playerId[IDX(m_currentSide)] == -1
            || gbThisNetHumanPlayer[m_playerId[IDX(m_currentSide)]] == 0
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

VA(0x00430ab0, 0x14f)
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

VA(0x00430bff, 0xbf)
void combatManager::ResetMouse(void) {
    i32 mouseY;
    i32 mouseX;
    tag_message message;

    if (gbNoShowCombat != 0)
        return;
    if (gbThisNetHasControl != 0 && m_playerId[IDX(m_currentSide)] >= 0
        && gbHumanPlayer[m_playerId[IDX(m_currentSide)]] != 0) {
        m_selectedHex = INVALID_HEX;
        ClearCombatMessages(0);
        gpMouseManager->MouseCoords(mouseX, mouseY);
        message.type = MESSAGE_MOUSE_MOVE;
        message.payload.mouse.x = message.payload.mouse.screenX = mouseX;
        message.payload.mouse.y = message.payload.mouse.screenY = mouseY;
        ProcessCombatMsg(message);
    } else {
        gpMouseManager->SetPointer(COMBAT_POINTER_DEFAULT);
    }
}

VA(0x00430cbe, 0x60b)
MessageDispatchResult combatManager::ProcessNextAction(struct tag_message& message) {
    i32 actionData[IDX(ACTION_DATA_COUNT)];
    i32 transmitResult;
    army* actingArmy_29;
    i32 shouldAdvance;
    i32 redraw_10;
    MessageDispatchResult dispatchResult_1;

    ClearCombatMessages(0);
    dispatchResult_1 = MESSAGE_DISPATCH_CONSUME;
    redraw_10 = 0;
    gbProcessingCombatAction = true;
    if (m_smallViewSide[IDX(COMBAT_ATTACKER_SIDE)] != COMBAT_SIDE_NONE
        || m_smallViewSide[IDX(COMBAT_DEFENDER_SIDE)] != COMBAT_SIDE_NONE) {
        m_smallViewSide[IDX(COMBAT_DEFENDER_SIDE)] = COMBAT_SIDE_NONE;
        m_smallViewSide[IDX(COMBAT_ATTACKER_SIDE)] = COMBAT_SIDE_NONE;
        redraw_10 = 1;
    }
    if (giNextAction != ACTION_NONE) {
        LogInt(
            "Process Act",
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
        redraw_10 = 1;
    if (redraw_10 != 0)
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
            "About to T",
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
        LogStr("Post T");
        if (transmitResult == 0)
            ShutDown(NULL);
    }

    actingArmy_29 = &m_armies[IDX(m_currentArmySide)][m_currentArmyIndex];
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
            actingArmy_29->MoveAttack(giNextActionGridIndex, 0);
            actingArmy_29->m_monster.flags.abilityFlags |= MONSTER_ABILITY_FLAG_BAD_MORALE;
            if (CheckWin(&message) != 0) {
                dispatchResult_1 = MESSAGE_DISPATCH_FORWARD;
                goto Finished;
            }
            CheckApplyGoodMorale(m_currentArmySide, m_currentArmyIndex);
            shouldAdvance = 1;
            ResetCycleTimers();
            break;
        case ACTION_ATTACK:
            ResetCyclingCreatures();
            if (giNextActionExtra != -1 && actingArmy_29->m_hex != giNextActionExtra) {
                actingArmy_29->MoveAttack(giNextActionExtra, 1);
            }
            actingArmy_29->MoveAttack(giNextActionGridIndex, 0);
            actingArmy_29->m_monster.flags.abilityFlags |= MONSTER_ABILITY_FLAG_BAD_MORALE;
            if (CheckWin(&message) != 0) {
                dispatchResult_1 = MESSAGE_DISPATCH_FORWARD;
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
            actingArmy_29->m_monster.flags.abilityFlags |= MONSTER_ABILITY_FLAG_BAD_MORALE;
            shouldAdvance = 1;
            break;
        case ACTION_DEFEND:
            actingArmy_29->m_monster.flags.abilityFlags |= MONSTER_ABILITY_FLAG_DEFERRED_TURN;
            shouldAdvance = 1;
            break;
    }
    giNextAction = ACTION_NONE;
    if (CheckWin(&message) != 0) {
        dispatchResult_1 = MESSAGE_DISPATCH_FORWARD;
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
    return dispatchResult_1;
}

VA(0x004312c9, 0x211)
void combatManager::ResetCyclingCreatures(void) {
    army* currentTroop = NULL;
    i32 rotateCount = 0;
    i32 index;
    CombatSide sideIndex;
    i32 unusedRotateWord;

    for (sideIndex = COMBAT_ATTACKER_SIDE; IDX(sideIndex) < COMBAT_SIDE_COUNT; ++sideIndex) {
        for (index = 0; index < gpCombatManager->m_armyCount[IDX(sideIndex)]; ++index) {
            currentTroop = &gpCombatManager->m_armies[IDX(sideIndex)][index];
            if (HAS(currentTroop->m_monster.flags.abilityFlags, MONSTER_ABILITY_FLAG_AI_EXCLUDED)
                    == 0
                && currentTroop->m_animationSequence >= COMBAT_CREATURE_CYCLE_SEQUENCE_FIRST
                && currentTroop->m_animationSequence <= COMBAT_CREATURE_CYCLE_SEQUENCE_LAST) {
                ++rotateCount;
                ++gpCombatManager->m_limitCreatureCount[IDX(sideIndex)][index];
            }
        }
    }
    if (rotateCount == 0)
        return;

    gpCombatManager->DrawFrame(0, 1, 1, 1, COMMAND_FRAME_DELAY, 1, 1);
    for (sideIndex = COMBAT_ATTACKER_SIDE; IDX(sideIndex) < COMBAT_SIDE_COUNT; ++sideIndex) {
        for (index = 0; index < gpCombatManager->m_armyCount[IDX(sideIndex)]; ++index) {
            currentTroop = &gpCombatManager->m_armies[IDX(sideIndex)][index];
            if (HAS(currentTroop->m_monster.flags.abilityFlags, MONSTER_ABILITY_FLAG_AI_EXCLUDED)
                == 0) {
                currentTroop = &gpCombatManager->m_armies[IDX(sideIndex)][index];
                currentTroop->m_animationSequence = ARMY_ANIMATION_STAND;
                currentTroop->m_animationFrame = 0;
                currentTroop->m_lastAnimationTime = KBTickCount();
            }
        }
    }
    m_heroCycleTimer[IDX(COMBAT_ATTACKER_SIDE)] = KBTickCount();
    m_heroCycleTimer[IDX(COMBAT_DEFENDER_SIDE)] = KBTickCount();
    gpCombatManager->DrawFrame(1, 1, 0, 0, COMMAND_FRAME_DELAY, 1, 1);
}

VA(0x004314da, 0xdf)
void combatManager::ResetCycleTimers(void) {
    i32l now = KBTickCount();
    CombatSide which;
    i32 i;
    army* ap;

    m_heroCycleTimer[IDX(COMBAT_ATTACKER_SIDE)] = KBTickCount();
    m_heroCycleTimer[IDX(COMBAT_DEFENDER_SIDE)] = KBTickCount();
    for (which = COMBAT_ATTACKER_SIDE; IDX(which) < COMBAT_SIDE_COUNT; ++which) {
        for (i = 0; i < gpCombatManager->m_armyCount[IDX(which)]; ++i) {
            ap = &gpCombatManager->m_armies[IDX(which)][i];
            ap->m_lastAnimationTime = now;
            if (ap->m_frameInfo.standStillDelay > STAND_DELAY_RANDOM_THRESHOLD) {
                ap->m_lastAnimationTime -=
                    Random(STAND_DELAY_RANDOM_MIN, ap->m_frameInfo.standStillDelay);
            }
        }
    }
}

VA(0x004315b9, 0x41)
i32 InCombatArea(i32 x, i32 y) {
    return x >= 0 && x < COMBAT_SCREEN_WIDTH && y >= 0 && y < COMBAT_AREA_HEIGHT;
}

VA(0x004315fa, 0x930)
void combatManager::CycleCombatScreen(void) {
    i32 nextHeroAnimation_0[COMBAT_SIDE_COUNT];
    army* currentArmy_2;
    i32 cyclingCount_1;
    i32 index_0;
    i32 animationIndex_27;
    u8 cycleArmy[COMBAT_SIDE_COUNT][COMBAT_ARMY_SLOT_COUNT];
    CombatSide side_7;
    float accumulatedChance;
    float roll_3;

    CheckUpdateCombatMessages();
    currentArmy_2 = NULL;
    cyclingCount_1 = 0;
    gpCombatManager->ResetLimitCreature();
    for (index_0 = COMBAT_ATTACKER_SIDE; index_0 < COMBAT_SIDE_COUNT; ++index_0) {
        if (m_heroOverlayIcons[index_0] == NULL)
            continue;

        if (m_heroes[index_0] != NULL)
            m_heroOverlayFrame[index_0] =
                (m_heroOverlayFrame[index_0] + 1) % COMBAT_HERO_OVERLAY_FRAME_COUNT;
        ++m_drawHeroOverlay[index_0];
    }

    memset(cycleArmy, 0, sizeof(cycleArmy));
    for (side_7 = COMBAT_ATTACKER_SIDE; IDX(side_7) < COMBAT_SIDE_COUNT; ++side_7) {
        for (index_0 = 0; index_0 < gpCombatManager->m_armyCount[IDX(side_7)]; ++index_0) {
            currentArmy_2 = gpCombatManager->m_armies[IDX(side_7)] + index_0;
            if (HAS(currentArmy_2->m_monster.flags.abilityFlags, MONSTER_ABILITY_FLAG_AI_EXCLUDED)
                    == 0
                && currentArmy_2->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_PARALYZE)] == 0
                && currentArmy_2->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_BLIND)] == 0
                && currentArmy_2->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_PETRIFIED)] == 0
                && ((currentArmy_2->m_animationSequence >= COMBAT_CREATURE_CYCLE_SEQUENCE_FIRST
                     && currentArmy_2->m_animationSequence <= COMBAT_CREATURE_CYCLE_SEQUENCE_LAST)
                    || (currentArmy_2->m_animationSequence == ARMY_ANIMATION_STAND
                        && currentArmy_2->m_lastAnimationTime
                                   + currentArmy_2->m_frameInfo.standStillDelay
                               < KBTickCount()))) {
                ++cyclingCount_1;
                ++cycleArmy[IDX(side_7)][index_0];
                ++m_limitCreatureCount[IDX(side_7)][index_0];
            }
        }
    }

    for (index_0 = COMBAT_ATTACKER_SIDE; index_0 < COMBAT_SIDE_COUNT; ++index_0) {
        nextHeroAnimation_0[index_0] = -1;
        if (m_heroIcons[index_0] == NULL)
            continue;

        if (m_heroAnimationState[index_0] == HERO_ANIMATION_DEATH_FIRST
            || m_heroAnimationState[index_0] == HERO_ANIMATION_DEATH_SECOND
            || m_heroAnimationState[index_0] == HERO_ANIMATION_IDLE_FIRST
            || m_heroAnimationState[index_0] == HERO_ANIMATION_IDLE_SECOND
            || m_heroAnimationState[index_0] == HERO_ANIMATION_IDLE_LAST) {
            m_drawHero[index_0] = 1;
        } else if (m_heroAnimationState[index_0] == HERO_ANIMATION_STAND
                   && m_heroDeathAnimationPlayed[index_0] == 0
                   && m_heroDeathPending[index_0] != 0) {
            if (m_playerId[IDX(m_currentSide)] == -1
                || gbThisNetHumanPlayer[m_playerId[IDX(m_currentSide)]] == 0) {
                m_heroAlternateDeathPending[index_0] = 0;
                m_heroDeathPending[index_0] = 0;
            } else {
                m_heroAlternateDeathPending[index_0] = 0;
                m_heroDeathPending[index_0] = 0;
                m_heroDeathAnimationPlayed[index_0] = 1;
                if (sCmbtHero[m_heroSpriteIndex[index_0]]
                        .animationFrameCount[HERO_ANIMATION_DEATH_FIRST]
                    > 0) {
                    nextHeroAnimation_0[index_0] = HERO_ANIMATION_DEATH_FIRST;
                    m_drawHero[index_0] = 1;
                }
            }
        } else if (m_heroAnimationState[index_0] == HERO_ANIMATION_STAND
                   && m_heroAlternateDeathAnimationPlayed[index_0] == 0
                   && m_heroAlternateDeathPending[index_0] != 0) {
            if (m_playerId[IDX(m_currentSide)] == -1
                || gbThisNetHumanPlayer[m_playerId[IDX(m_currentSide)]] == 0) {
                m_heroAlternateDeathPending[index_0] = 0;
                m_heroDeathPending[index_0] = 0;
            } else {
                m_heroAlternateDeathPending[index_0] = 0;
                m_heroDeathPending[index_0] = 0;
                m_heroAlternateDeathAnimationPlayed[index_0] = 1;
                if (sCmbtHero[m_heroSpriteIndex[index_0]]
                        .animationFrameCount[HERO_ANIMATION_DEATH_SECOND]
                    > 0) {
                    nextHeroAnimation_0[index_0] = HERO_ANIMATION_DEATH_SECOND;
                    m_drawHero[index_0] = 1;
                }
            }
        } else if (m_heroAnimationState[index_0] == HERO_ANIMATION_STAND
                   && m_heroCycleTimer[index_0] + HERO_IDLE_DELAY < KBTickCount()) {
            if (sCmbtHero[m_heroSpriteIndex[index_0]].idleAnimationCount > 1) {
                nextHeroAnimation_0[index_0] =
                    Random(0, sCmbtHero[m_heroSpriteIndex[index_0]].idleAnimationCount - 1)
                    + HERO_ANIMATION_IDLE_FIRST;
            } else {
                nextHeroAnimation_0[index_0] = HERO_ANIMATION_IDLE_FIRST;
            }
            m_drawHero[index_0] = 1;
        }    }
    if (m_heroIcons[IDX(COMBAT_ATTACKER_SIDE)] != NULL)
        m_drawHero[IDX(COMBAT_ATTACKER_SIDE)] = 1;
    if (m_heroIcons[IDX(COMBAT_DEFENDER_SIDE)] != NULL)
        m_drawHero[IDX(COMBAT_DEFENDER_SIDE)] = 1;

    if (cyclingCount_1 == 0 && m_drawHero[IDX(COMBAT_ATTACKER_SIDE)] == 0
        && m_drawHero[IDX(COMBAT_DEFENDER_SIDE)] == 0
        && m_drawHeroOverlay[IDX(COMBAT_ATTACKER_SIDE)] == 0
        && m_drawHeroOverlay[IDX(COMBAT_DEFENDER_SIDE)] == 0)
        goto setCycleTimer;
    gpCombatManager->DrawFrame(0, 1, 1, 1, COMMAND_FRAME_DELAY, 1, 1);
    for (side_7 = COMBAT_ATTACKER_SIDE; IDX(side_7) < COMBAT_SIDE_COUNT; ++side_7) {
        for (index_0 = 0; index_0 < gpCombatManager->m_armyCount[IDX(side_7)]; ++index_0) {
            currentArmy_2 = gpCombatManager->m_armies[IDX(side_7)] + index_0;
            if (cycleArmy[IDX(side_7)][index_0] != 0) {
                if (currentArmy_2->m_animationSequence == ARMY_ANIMATION_STAND) {
                    roll_3 = static_cast<float>(Random(IDLE_ROLL_MIN, IDLE_ROLL_MAX))
                           / COMBAT_IDLE_ROLL_DIVISOR;
                    accumulatedChance = 0.0f;
                    currentArmy_2->m_standingAnimation =
                        currentArmy_2->m_frameInfo.standingAnimationCount - 1;
                    for (animationIndex_27 = 0;
                         animationIndex_27 < currentArmy_2->m_frameInfo.standingAnimationCount - 1;
                         ++animationIndex_27) {
                        accumulatedChance +=
                            currentArmy_2->m_frameInfo.standingAnimationChances[animationIndex_27];
                        if (roll_3 < accumulatedChance) {
                            currentArmy_2->m_standingAnimation = animationIndex_27;
                            animationIndex_27 = STANDING_ANIMATION_SEARCH_DONE;
                        }
                    }
                    currentArmy_2->m_animationSequence = ArmyAnimationSequence(
                        currentArmy_2->m_standingAnimation
                        + IDX(COMBAT_CREATURE_CYCLE_SEQUENCE_FIRST)
                    );
                    currentArmy_2->m_animationFrame = 0;
                } else {
                    ++currentArmy_2->m_animationFrame;
                    if (currentArmy_2->m_frameInfo.standStillDelay == 0
                        && currentArmy_2->m_frameInfo.standingAnimationCount == 1
                        && Random(0, IDLE_ROLL_MAX) < IDLE_REPEAT_CHANCE) {
                        --currentArmy_2->m_animationFrame;
                    }
                    if (currentArmy_2->m_animationFrame
                        >= currentArmy_2->m_frameInfo.animationFrameCount
                               [currentArmy_2->m_standingAnimation
                                + IDX(COMBAT_CREATURE_CYCLE_SEQUENCE_FIRST)]) {
                        currentArmy_2->m_animationSequence = ARMY_ANIMATION_STAND;
                        currentArmy_2->m_animationFrame = 0;
                        currentArmy_2->m_lastAnimationTime = KBTickCount();
                        if (currentArmy_2->m_frameInfo.standStillDelay > 0) {
                            currentArmy_2->m_lastAnimationTime = static_cast<i32>(
                                currentArmy_2->m_lastAnimationTime
                                + (Random(0, currentArmy_2->m_frameInfo.standStillDelay)
                                       * COMBAT_STAND_DELAY_RANDOM_FACTOR
                                   - currentArmy_2->m_frameInfo.standStillDelay
                                         * COMBAT_STAND_DELAY_BASE_FACTOR)
                            );
                        }
                    }
                }
            }
        }
    }
    for (index_0 = COMBAT_ATTACKER_SIDE; index_0 < COMBAT_SIDE_COUNT; ++index_0) {
        if (m_drawHero[index_0] != 0) {
            if (nextHeroAnimation_0[index_0] != -1) {
                m_heroAnimationState[index_0] = nextHeroAnimation_0[index_0];
                m_heroAnimationFrame[index_0] = 0;
            } else {
                ++m_heroAnimationFrame[index_0];
                if (m_heroAnimationFrame[index_0]
                    >= sCmbtHero[m_heroSpriteIndex[index_0]]
                           .animationFrameCount[m_heroAnimationState[index_0]]) {
                    m_heroAnimationState[index_0] = HERO_ANIMATION_STAND;
                    m_heroAnimationFrame[index_0] = 0;
                    m_heroCycleTimer[index_0] = KBTickCount();
                }
            }
        }
    }
    DrawFrame(1, 1, 0, 0, COMMAND_FRAME_DELAY, 1, 1);
setCycleTimer:
    glTimers[GLOBAL_COMBAT_CYCLE_TIMER_SLOT] = static_cast<i32>(
        KBTickCount() + COMBAT_CYCLE_TIMER_FACTOR * gfCombatSpeedMod[gConfig.combatSpeed]
    );
}

VA(0x00431f2a, 0x30)
void combatManager::SetCombatViewArmySmallLevel(i32 level) {
    gConfig.combatArmyInfoLevel = level;
    DrawFrame(1, 0, 0, 0, COMMAND_FRAME_DELAY, 1, 1);
    WritePrefs();
}

VA(0x00431f5a, 0xbb)
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

VA(0x00432015, 0x323)
void combatManager::AddArmy(
    H2_ENUM_PARAM(CombatSide, i32) side,
    H2_ENUM_PARAM(CreatureType, i32) monsterType,
    i32 quantity,
    i32 hex,
    H2_ENUM_PARAM(MonsterFlags, i32) flags,
    i32 animate
) {
    i32 armyIdx = INVALID_ARMY_INDEX;
    i32 reusedArmy = 0;
    i32 index;
    army* newStack;
    for (index = 0; index < COMBAT_ARMY_CAPACITY; ++index) {
        if (m_armies[IDX(side)][index].m_monsterType == CREATURE_NONE) {
            armyIdx = index;
            break;
        }
        if (m_armies[IDX(side)][index].m_quantity == 0
            && HAS(m_armies[IDX(side)][index].m_monster.flags.all, MONSTER_FLAGS_AI_EXCLUDED) != 0
            && (HAS(m_armies[IDX(side)][index].m_monster.flags.all, MONSTER_FLAGS_MIRROR_IMAGE)
                    != 0
                || m_armies[IDX(side)][index].m_monsterType == CREATURE_EARTH_ELEMENTAL
                || m_armies[IDX(side)][index].m_monsterType == CREATURE_AIR_ELEMENTAL
                || m_armies[IDX(side)][index].m_monsterType == CREATURE_FIRE_ELEMENTAL
                || m_armies[IDX(side)][index].m_monsterType == CREATURE_WATER_ELEMENTAL)) {
            armyIdx = index;
            reusedArmy = 1;
            break;
        }
    }

    if (armyIdx == INVALID_ARMY_INDEX || m_hexCells[hex].m_occupantSide != COMBAT_SIDE_NONE)
        return;

    newStack = &m_armies[IDX(side)][armyIdx];
    newStack->Init(monsterType, quantity, side, armyIdx, hex, INVALID_HEX);
    newStack->LoadResources();
    newStack->m_monster.flags.all |= flags;
    if (reusedArmy == 0)
        ++m_armyCount[IDX(side)];

    if (animate == 0)
        return;

    ResetLimitCreature();
    ++m_limitCreatureCount[IDX(side)][armyIdx];
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

VA(0x00432338, 0x13d)
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

VA(0x00432475, 0x2ae)
void combatManager::ViewBallista(i32 quickView) {
    i32 shooterCount;
    i32 attackSkill;
    char description[VIEW_DESCRIPTION_SIZE];

    m_combatTowns[IDX(COMBAT_DEFENDER_SIDE)]->CalcNumLevelArchers(&shooterCount, &attackSkill);
    sprintf(gText, "\xc1\xe0\xeb\xeb\xe8\xf1\xf2\xe0" /* "Баллиста" */);
    strcpy(description, "");
    if (m_wallStates[IDX(COMBAT_WALL_SLOT_KEEP)] != COMBAT_WALL_STATE_KEEP_STANDING) {
        sprintf(
            description,
            "\n\n%s \xf3\xed\xe8\xf7\xf2\xee\xe6\xe5\xed\xe0." /* "\n\n%s уничтожена." */,
            "\xc1\xe0\xeb\xeb\xe8\xf1\xf2\xe0" /* "Баллиста" */
        );
    } else if (attackSkill > 0) {
        sprintf(
            description,
            "\n\n%s \xf1\xf2\xf0\xe5\xeb\xff\xe5\xf2 \xf1 \xf1\xe8\xeb\xee\xe9 %d "
            "\xf1\xf2\xf0\xe5\xeb\xea\xee\xe2, \xea\xe0\xe6\xe4\xe0\xff +%d "
            "\xe1\xee\xed\xf3\xf1\xee\xec \xea \xe8\xf5 \xf3\xf0\xee\xe2\xed\xfe "
            "\xe0\xf2\xe0\xea\xe8." /* "\n\n%s стреляет с силой %d стрелков,
                                       каждая +%d бонусом к их уровню атаки." */,
            "\xc1\xe0\xeb\xeb\xe8\xf1\xf2\xe0" /* "Баллиста" */,
            shooterCount,
            attackSkill
        );
    } else {
        sprintf(
            description,
            "\n\n%s \xf1\xf2\xf0\xe5\xeb\xff\xe5\xf2 \xf1 \xf1\xe8\xeb\xee\xe9 %d "
            "\xf1\xf2\xf0\xe5\xeb\xea\xee\xe2." /* "\n\n%s стреляет с силой %d стрелков." */,
            "\xc1\xe0\xeb\xeb\xe8\xf1\xf2\xe0" /* "Баллиста" */,
            shooterCount
        );
    }
    strcat(gText, description);

    strcpy(description, "");
    if ((m_combatTowns[IDX(COMBAT_DEFENDER_SIDE)]->m_buildings & IDX(TOWN_BUILDING_LEFT_TURRET))
        != 0) {
        if (m_wallStates[IDX(COMBAT_WALL_SLOT_TOP_TOWER)] == COMBAT_WALL_STATE_DESTROYED) {
            sprintf(
                description,
                "\n\n%s \xf3\xed\xe8\xf7\xf2\xee\xe6\xe5\xed\xe0." /* "\n\n%s уничтожена." */,
                "\xcb\xe5\xe2\xe0\xff \xe1\xe0\xf8\xed\xff" /* "Левая башня" */
            );
        } else if (m_wallStates[IDX(COMBAT_WALL_SLOT_TOP_TOWER)]
                   == COMBAT_WALL_STATE_TOWER_STANDING) {
            if (attackSkill > 0) {
                sprintf(
                    description,
                    "\n\n%s \xf1\xf2\xf0\xe5\xeb\xff\xe5\xf2 \xf1 \xf1\xe8\xeb\xee\xe9 %d "
                    "\xf1\xf2\xf0\xe5\xeb\xea\xee\xe2, \xea\xe0\xe6\xe4\xe0\xff +%d "
                    "\xe1\xee\xed\xf3\xf1\xee\xec \xea \xe8\xf5 \xf3\xf0\xee\xe2\xed\xfe "
                    "\xe0\xf2\xe0\xea\xe8." /* "\n\n%s стреляет с силой %d стрелков,
                                               каждая +%d бонусом к их уровню атаки." */,
                    "\xcb\xe5\xe2\xe0\xff \xe1\xe0\xf8\xed\xff" /* "Левая башня" */,
                    shooterCount / COMBAT_KEEP_SIDE_TOWER_SHOT_DIVISOR,
                    attackSkill
                );
            } else {
                sprintf(
                    description,
                    "\n\n%s \xf1\xf2\xf0\xe5\xeb\xff\xe5\xf2 \xf1 \xf1\xe8\xeb\xee\xe9 %d "
                    "\xf1\xf2\xf0\xe5\xeb\xea\xee\xe2." /* "\n\n%s стреляет с силой %d
                                                            стрелков." */,
                    "\xcb\xe5\xe2\xe0\xff \xe1\xe0\xf8\xed\xff" /* "Левая башня" */,
                    shooterCount / COMBAT_KEEP_SIDE_TOWER_SHOT_DIVISOR
                );
            }
        }
        strcat(gText, description);
    }

    if ((m_combatTowns[IDX(COMBAT_DEFENDER_SIDE)]->m_buildings & IDX(TOWN_BUILDING_RIGHT_TURRET))
        != 0) {
        strcpy(description, "");
        if (m_wallStates[IDX(COMBAT_WALL_SLOT_BOTTOM_TOWER)] == COMBAT_WALL_STATE_DESTROYED) {
            sprintf(
                description,
                "\n\n%s \xf3\xed\xe8\xf7\xf2\xee\xe6\xe5\xed\xe0." /* "\n\n%s уничтожена." */,
                "\xcf\xf0\xe0\xe2\xe0\xff \xe1\xe0\xf8\xed\xff" /* "Правая башня" */
            );
        } else if (m_wallStates[IDX(COMBAT_WALL_SLOT_BOTTOM_TOWER)]
                   == COMBAT_WALL_STATE_TOWER_STANDING) {
            if (attackSkill > 0) {
                sprintf(
                    description,
                    "\n\n%s \xf1\xf2\xf0\xe5\xeb\xff\xe5\xf2 \xf1 \xf1\xe8\xeb\xee\xe9 %d "
                    "\xf1\xf2\xf0\xe5\xeb\xea\xee\xe2, \xea\xe0\xe6\xe4\xe0\xff +%d "
                    "\xe1\xee\xed\xf3\xf1\xee\xec \xea \xe8\xf5 \xf3\xf0\xee\xe2\xed\xfe "
                    "\xe0\xf2\xe0\xea\xe8." /* "\n\n%s стреляет с силой %d стрелков,
                                               каждая +%d бонусом к их уровню атаки." */,
                    "\xcf\xf0\xe0\xe2\xe0\xff \xe1\xe0\xf8\xed\xff" /* "Правая башня" */,
                    shooterCount / COMBAT_KEEP_SIDE_TOWER_SHOT_DIVISOR,
                    attackSkill
                );
            } else {
                sprintf(
                    description,
                    "\n\n%s \xf1\xf2\xf0\xe5\xeb\xff\xe5\xf2 \xf1 \xf1\xe8\xeb\xee\xe9 %d "
                    "\xf1\xf2\xf0\xe5\xeb\xea\xee\xe2." /* "\n\n%s стреляет с силой %d
                                                            стрелков." */,
                    "\xcf\xf0\xe0\xe2\xe0\xff \xe1\xe0\xf8\xed\xff" /* "Правая башня" */,
                    shooterCount / COMBAT_KEEP_SIDE_TOWER_SHOT_DIVISOR
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

b32 gbThisNetHasControl;
i32 iCurTransferArtifact;
i8 iTransferArtifactsInfo[COMBAT_TRANSFER_ARTIFACT_COUNT];
b32 gbWhichAnimationPlaying;
i32 iMaxTransferArtifacts;
i32 giNextActionExtra;
i32 bSkeletonsShown;
b32 gbShowingLoseWindow;
i32 giWinCmbtFrame;
i32 giNextActionGridIndex;
i32 giSurrenderCost;
i32 giSkeletonsCreated;
H2_ENUM_STORAGE(ArtifactType, i8) iTransferArtifacts[COMBAT_TRANSFER_ARTIFACT_COUNT];
H2_ENUM_STORAGE(CombatAction, i32) giNextAction;
i32 giNextActionGridIndex2;
