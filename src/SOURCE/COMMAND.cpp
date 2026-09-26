#include <Ints.h>
#include <BASE/Utf8.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
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
#include <IRONFIST/creatures.h>
#include <IRONFIST/expansions.h>
#include <SOURCE/advManager.h>
#include <SOURCE/combatManager.h>
#include <SOURCE/COMMAND.h>
#include <SOURCE/GAME.h>
#include <SOURCE/game.h>
#include <SOURCE/KB.h>
#include <PLATFORM/Runtime.h>
#include <SOURCE/NOOPT.h>
#include <SOURCE/PATH.h>
#include <SOURCE/PHILAI.h>
#include <SOURCE/REMOTE.h>
#include <SOURCE/town.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/Localization.h>
#include <BASE/dialog.h>
#include <BASE/display.h>
#include <SOURCE/ARMY.h>
#include <SOURCE/combatTypes.h>

// Debug toggle: every battle ends instantly in the human side's favor.
bool gbAutoWinBattles;

#define COMBAT_NECROMANCY_LEVEL_FACTOR 0.1
#define COMBAT_SURRENDER_QUILL_FACTOR 0.1
#define COMBAT_SURRENDER_BASE_FACTOR 0.5
#define COMBAT_SURRENDER_DIPLOMACY_FACTOR                                          \
    0.2
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

    typedef enum CombatWinLoseConstant {
        WIN_LOSE_NEXT_CONTROL = DIALOG_BUTTON_0,
        WIN_LOSE_TEXT_ID = 101,
        WIN_LOSE_RESOURCE_LOAD_ID = 200,
        WIN_LOSE_RESOURCE_DRAW_ID = 201,
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
    } CombatWinLoseConstant;

    enum class CombatBattleResultText : i32 {
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
    };
using enum CombatBattleResultText;

    enum class CombatControlId : i32 {
        CONTROL_DEFER_TURN = 1,
        CONTROL_SKIP_TURN = 2,
        CONTROL_DISABLE_SELECTION = 3,
        CONTROL_SYSTEM_OPTIONS = 4,
        CONTROL_HELP_FIRST = 10,
        CONTROL_HELP_SECOND = 11,
        CONTROL_HELP_THIRD = 12,
        CONTROL_HELP_FOURTH = 13,
        CONTROL_MAIN_BUTTON = 0x40
    };
using enum CombatControlId;

    constexpr CombatControlId CombatControlIdFromCode(i32 value) {
        return static_cast<CombatControlId>(value); // H2_ENUM_CODE_BOUNDARY
    }

    typedef enum CombatRemoteConstant {
        REMOTE_PACKET_TYPE = 1,
        REMOTE_COMMAND_MESSAGE = 11,
        REMOTE_COMMAND_ACTION = 23
    } CombatRemoteConstant;

    enum class CombatActionDataIndex : i32 {
        ACTION_DATA_ACTION = 0,
        ACTION_DATA_EXTRA = 1,
        ACTION_DATA_GRID = 2,
        ACTION_DATA_SECOND_GRID = 3,
        ACTION_DATA_COUNT = 4
    };
using enum CombatActionDataIndex;

    typedef enum CombatDirectionConstant {
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
    } CombatDirectionConstant;

    typedef enum CombatCommandConstant {
        POINTER_VIEW = 5,
        POINTER_ATTACK_OFFSET = 7,
        INVALID_ARMY_INDEX = -1,
        INVALID_HEX = -1,
        PLAYER_NONE = -1,
        SMALL_VIEW_POSITION_NONE = -1,
        IGNORED_HEX = -2,
        CASTLE_GATE_HEX = 59
    } CombatCommandConstant;

    typedef enum CombatRoundConstant {
        ROUND_INITIAL_SPEED = 15
    } CombatRoundConstant;

    typedef enum CombatCommandGeometry {
        CONTROL_RIGHT_MIN_X = 590,
        CONTROL_LEFT_MAX_X = 50,
        CONTROL_SYSTEM_OPTIONS_MIN_Y = 460,
        ARMY_VIEW_LEVEL_COUNT = 3,
        COMMAND_FRAME_DELAY = 75
    } CombatCommandGeometry;

    typedef enum CombatCasualtyConstant {
        CASUALTY_DISPLAY_LIMIT = 7,
        CASUALTY_WIDGETS_PER_SIDE = 7,
        CASUALTY_WINDOW_WIDTH = 320,
        CASUALTY_WINDOW_BOTTOM = 458
    } CombatCasualtyConstant;

    typedef enum CombatArtifactPanelConstant {
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
    } CombatArtifactPanelConstant;

    typedef enum CombatSkeletonPanelConstant {
        SKELETON_ICON_X = 173,
        SKELETON_ICON_Y = 270,
        SKELETON_ICON_WIDTH = 32,
        SKELETON_ICON_HEIGHT = 30,
        SKELETON_TEXT_X = 165,
        SKELETON_TEXT_Y = 300,
        SKELETON_TEXT_WIDTH = 32,
        SKELETON_TEXT_HEIGHT = 12,
        SKELETON_COUNT_CAPACITY = 9
    } CombatSkeletonPanelConstant;

    typedef enum CombatEagleEyePanelConstant {
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
    } CombatEagleEyePanelConstant;

    typedef enum CombatCasualtyPanelConstant {
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
    } CombatCasualtyPanelConstant;

    typedef enum CombatCycleConstant {
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
    } CombatCycleConstant;

    typedef enum CombatVictoryConstant {
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
    } CombatVictoryConstant;

    typedef enum CombatSurrenderDialogConstant {
        SURRENDER_DIALOG_TYPE = 1,
        SURRENDER_DIALOG_ACCEPT_RESULT = 2,
        SURRENDER_TEXT_WIDTH = 30,
        SURRENDER_PORTRAIT_RESOURCE_ID = 1,
        SURRENDER_PORTRAIT_WIDGET_ID = 30,
        SURRENDER_PORTRAIT_DEFAULT_COLOR = 4,
        SURRENDER_TEXT_ID = 2,
        SURRENDER_WINDOW_X = 74,
        SURRENDER_WINDOW_Y = 80
    } CombatSurrenderDialogConstant;

    enum class CombatHelpTextIndex : i32 {
        HELP_SKIP_UNIT = 1,
        HELP_AUTO_COMBAT = 2,
        HELP_SYSTEM_OPTIONS = 3,
        HELP_OTHER_CONTROL = 4
    };
using enum CombatHelpTextIndex;

    enum class CombatLongHelpIndex : i32 {
        LONG_HELP_DEFER_TURN = 0,
        LONG_HELP_SKIP_TURN = 1,
        LONG_HELP_DISABLE_SELECTION = 2,
        LONG_HELP_SYSTEM_OPTIONS = 3,
        LONG_HELP_CONTROLS = 4
    };
using enum CombatLongHelpIndex;

    typedef enum CombatStorageConstant {
        SELECTOR_INITIAL_FRAME = 7,
        VIEW_DESCRIPTION_SIZE = 300
    } CombatStorageConstant;

}

MessageDispatchResult combatManager::Main(tag_message& message) {
    MessageDispatchResult result = MESSAGE_DISPATCH_CONSUME;
    army* currentArmy;

    if (gbNoShowCombat == 0) {
        if (glTimers[0] < platform::Ticks()) {
            PollSound();
            glTimers[0] = COMBAT_DEADLINE(COMBAT_SOUND_POLL_DELAY);
        }
        if (glTimers[GLOBAL_COMBAT_CYCLE_TIMER_SLOT] < platform::Ticks()
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
        CombatRemotePacket* remotePacket =
            reinterpret_cast<CombatRemotePacket*>(GetRemoteData(REMOTE_PACKET_TYPE));
        if (remotePacket != NULL && remotePacket->type == REMOTE_MESSAGE_RELIABLE) {
            switch (remotePacket->command) {
                case REMOTE_COMMAND_ACTION:
                    giNextAction = remotePacket->nextAction;
                    giNextActionExtra = remotePacket->nextActionExtra;
                    giNextActionGridIndex = remotePacket->nextActionGridIndex;
                    giNextActionGridIndex2 = remotePacket->nextActionGridIndex2;
                    goto ProcessAction;
                case REMOTE_COMMAND_MESSAGE:
                    PopNetBox(remotePacket->text, remotePacket->sender);
                    break;
            }
        }

        if (gbThisNetHasControl == 0) {
            if (message.type == MESSAGE_KEY_DOWN) {
                switch (InputManagerScanCodeFromCode(message.payload.keyboard.keyCode)) {
                    case H2EnumIndex(INPUT_SCAN_F2):
                        PopNetBox(NULL, -1);
                        break;
                }
            }
            return MESSAGE_DISPATCH_CONSUME;
        }
    }

    currentArmy = &m_armies[H2EnumIndex(m_currentArmySide)][m_currentArmyIndex];
    if (currentArmy->m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_BERSERK)] != 0) {
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
                m_gridSelectionDisabled = false;
        }
        CheckChangeSelector();
    }

ProcessAction:
    if (giNextAction == ACTION_NONE) {
        if (m_playerId[H2EnumIndex(m_currentSide)] == -1
            || gbThisNetHumanPlayer[m_playerId[H2EnumIndex(m_currentSide)]] == 0
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

i32 combatManager::ValidHexToStandOn(i32 hexIndex) {
    if (hexIndex == IGNORED_HEX)
        return 1;

    if (!(hexIndex == INVALID_HEX || hexIndex % COMBAT_GRID_ROW_LENGTH == MAP_WIDTH - 1
          || hexIndex % COMBAT_GRID_ROW_LENGTH == 0
          || (m_hexCells[hexIndex].m_blocked != 0 && !CAN_PASS_CASTLE_GATE(hexIndex))
          || (m_hexCells[hexIndex].m_occupantSide != COMBAT_SIDE_NONE
              && (m_hexCells[hexIndex].m_occupantSide != m_currentArmySide
                  || m_hexCells[hexIndex].m_occupantIndex != m_currentArmyIndex)))) {
        return 1;
    } else {
        return 0;
    }
}

void combatManager::SetCombatDirections(i32 targetHex) {
    if (m_gridSelectionDisabled != 0)
        return;

    bchar standable[COMBAT_DIRECTION_COUNT];
    i32 directionHexes[COMBAT_DIRECTION_COUNT];
    i32 rearHexes[COMBAT_DIRECTION_COUNT];
    bchar pathValid[COMBAT_DIRECTION_COUNT];
    i32 outputDirection;
    i32 mappedDirection;
    i32 previous;
    i32 next;
    army* currentArmy = &m_armies[H2EnumIndex(m_currentArmySide)][m_currentArmyIndex];
    CombatSide targetSide = currentArmy->m_targetSide;
    i32 targetIndex = currentArmy->m_targetIndex;
    CLEAR_ARMY_TARGET(*currentArmy);
    army* targetArmy = &m_armies[H2EnumIndex(targetSide)][targetIndex];

    i32 direction;
    for (direction = 0; direction < COMBAT_DIRECTION_COUNT; direction++) {
        if (direction == H2EnumIndex(COMBAT_DIRECTION_WIDE_WEST)
            || direction == H2EnumIndex(COMBAT_DIRECTION_WIDE_EAST)) {
            if ((H2EnumIndex((currentArmy->m_monster.attributes) & (MONSTER_FLAGS_WIDE))) != 0) {
                if (currentArmy->m_facing == ARMY_FACING_RIGHT) {
                    if (direction == H2EnumIndex(COMBAT_DIRECTION_WIDE_WEST))
                        directionHexes[direction] =
                            m_adjacency[targetHex][H2EnumIndex(COMBAT_DIRECTION_NORTHWEST)];
                    if (direction == H2EnumIndex(COMBAT_DIRECTION_WIDE_EAST))
                        directionHexes[direction] =
                            m_adjacency[targetHex][H2EnumIndex(COMBAT_DIRECTION_SOUTHWEST)];
                } else {
                    if (direction == H2EnumIndex(COMBAT_DIRECTION_WIDE_WEST))
                        directionHexes[direction] =
                            m_adjacency[targetHex][H2EnumIndex(COMBAT_DIRECTION_NORTHEAST)];
                    if (direction == H2EnumIndex(COMBAT_DIRECTION_WIDE_EAST))
                        directionHexes[direction] =
                            m_adjacency[targetHex][H2EnumIndex(COMBAT_DIRECTION_SOUTHEAST)];
                }
            } else {
                directionHexes[direction] = INVALID_HEX;
            }
        } else {
            directionHexes[direction] =
                m_adjacency[targetHex][direction];
        }

        if ((H2EnumIndex((currentArmy->m_monster.attributes) & (MONSTER_FLAGS_WIDE))) != 0
            && directionHexes[direction] != INVALID_HEX) {
            if (currentArmy->m_facing == ARMY_FACING_RIGHT) {
                if (direction == H2EnumIndex(COMBAT_DIRECTION_NORTHWEST)
                    || direction == H2EnumIndex(COMBAT_DIRECTION_WEST)
                    || direction == H2EnumIndex(COMBAT_DIRECTION_SOUTHWEST)) {
                    if (directionHexes[direction] % COMBAT_GRID_ROW_LENGTH
                        == COMBAT_GRID_FIRST_COLUMN)
                        directionHexes[direction] = INVALID_HEX;
                    else
                        directionHexes[direction]--;
                }
                if (directionHexes[direction] % COMBAT_GRID_ROW_LENGTH
                    == COMBAT_GRID_REVERSE_FIRST_COLUMN)
                    rearHexes[direction] = INVALID_HEX;
                else
                    rearHexes[direction] = directionHexes[direction] + 1;
            } else {
                if (direction == H2EnumIndex(COMBAT_DIRECTION_NORTHEAST)
                    || direction == H2EnumIndex(COMBAT_DIRECTION_EAST)
                    || direction == H2EnumIndex(COMBAT_DIRECTION_SOUTHEAST)) {
                    if (directionHexes[direction] % COMBAT_GRID_ROW_LENGTH
                        == COMBAT_GRID_REVERSE_FIRST_COLUMN)
                        directionHexes[direction] = INVALID_HEX;
                    else
                        directionHexes[direction]++;
                }
                if (directionHexes[direction] % COMBAT_GRID_ROW_LENGTH
                    == COMBAT_GRID_FIRST_COLUMN)
                    rearHexes[direction] = INVALID_HEX;
                else
                    rearHexes[direction] = directionHexes[direction] - 1;
            }
        } else {
            rearHexes[direction] = IGNORED_HEX;
        }

        if (ValidHexToStandOn(directionHexes[direction]) != 0
            && ValidHexToStandOn(rearHexes[direction]) != 0)
            standable[direction] = true;
        else
            standable[direction] = false;
    }

    if ((H2EnumIndex((currentArmy->m_monster.attributes) & (MONSTER_FLAGS_FLYING))) != 0) {
        for (direction = 0; direction < COMBAT_DIRECTION_COUNT; direction++)
            pathValid[direction] = standable[direction];
    } else {
        for (direction = 0; direction < COMBAT_DIRECTION_COUNT; direction++) {
            if (standable[direction] != 0) {
                // A charger can also come in on a clear straight line.
                if (CreatureHasAttribute(H2EnumIndex(currentArmy->m_monsterType), CHARGER)
                    && currentArmy->GetStraightLineDistanceToHex(directionHexes[direction])
                           <= currentArmy->m_monster.speed
                    && currentArmy->TargetOnStraightLine(directionHexes[direction])
                    && currentArmy->TargetOnStraightLine(targetHex)
                    && currentArmy->ValidFlight(
                        directionHexes[direction], ARMY_PATH_ANY_TARGET_HEX
                    ))
                    pathValid[direction] = true;
                else if (currentArmy->m_hex == directionHexes[direction]
                    || currentArmy->ValidPath(
                           directionHexes[direction],
                           ARMY_PATH_EXACT_TARGET_HEX
                       ) != 0)
                    pathValid[direction] = true;
                else
                    pathValid[direction] = false;
            } else {
                pathValid[direction] = false;
            }
        }
    }

    m_validDirectionCount = 0;
    for (direction = 0; direction < COMBAT_DIRECTION_COUNT; direction++) {
        if (pathValid[direction] != 0)
            m_validDirectionCount++;
    }
    if (m_validDirectionCount == 0)
        pathValid[H2EnumIndex(COMBAT_DIRECTION_WIDE_WEST)] = true;

    memset(m_directionMap, -1, sizeof(m_directionMap));
    for (direction = 0; direction < COMBAT_DIRECTION_COUNT; direction++) {
        outputDirection = direction;
        if (direction < COMBAT_DIRECTION_ADJACENT_COUNT)
            mappedDirection =
                (direction + COMBAT_DIRECTION_OPPOSITE_OFFSET) % COMBAT_DIRECTION_ADJACENT_COUNT;
        else
            mappedDirection = direction == H2EnumIndex(COMBAT_DIRECTION_WIDE_WEST)
                ? H2EnumIndex(COMBAT_DIRECTION_WIDE_EAST)
                : H2EnumIndex(COMBAT_DIRECTION_WIDE_WEST);

        if (pathValid[mappedDirection] != 0) {
            if ((H2EnumIndex((targetArmy->m_monster.attributes) & (MONSTER_FLAGS_WIDE))) != 0) {
                if (direction == H2EnumIndex(COMBAT_DIRECTION_NORTHEAST)
                    && HEX_HAS_OCCUPANT(m_hexCells[targetHex - 1], targetSide, targetIndex)) {
                    outputDirection = H2EnumIndex(COMBAT_DIRECTION_WIDE_WEST);
                } else if (direction == H2EnumIndex(COMBAT_DIRECTION_NORTHWEST)
                           && HEX_HAS_OCCUPANT(
                               m_hexCells[targetHex + 1],
                               targetSide,
                               targetIndex
                           )) {
                    outputDirection = H2EnumIndex(COMBAT_DIRECTION_WIDE_WEST);
                } else if (direction == H2EnumIndex(COMBAT_DIRECTION_SOUTHEAST)
                           && HEX_HAS_OCCUPANT(
                               m_hexCells[targetHex - 1],
                               targetSide,
                               targetIndex
                           )) {
                    outputDirection = H2EnumIndex(COMBAT_DIRECTION_WIDE_EAST);
                } else if (direction == H2EnumIndex(COMBAT_DIRECTION_SOUTHWEST)
                           && HEX_HAS_OCCUPANT(
                               m_hexCells[targetHex + 1],
                               targetSide,
                               targetIndex
                           )) {
                    outputDirection = H2EnumIndex(COMBAT_DIRECTION_WIDE_EAST);
                }
            }

            if (direction < COMBAT_DIRECTION_ADJACENT_COUNT) {
                memset(
                    &m_directionMap[mappedDirection * DIRECTION_SECTORS_PER_ADJACENT],
                    outputDirection,
                    DIRECTION_SECTORS_PER_ADJACENT
                );
            } else if (direction == H2EnumIndex(COMBAT_DIRECTION_WIDE_WEST)) {
                m_directionMap[DIRECTION_SPECIAL_FIRST_SECTOR_START] =
                    outputDirection;
                m_directionMap[DIRECTION_SPECIAL_FIRST_SECTOR_CENTER] =
                    outputDirection;
                m_directionMap[DIRECTION_SPECIAL_FIRST_SECTOR_END] =
                    outputDirection;
            } else {
                m_directionMap[DIRECTION_SPECIAL_SECOND_SECTOR_CENTER] =
                    outputDirection;
                m_directionMap[DIRECTION_SPECIAL_SECOND_SECTOR_NEXT] =
                    outputDirection;
                m_directionMap[DIRECTION_SPECIAL_SECOND_SECTOR_PREVIOUS] =
                    outputDirection;
            }
        }
    }

    i32 unresolved = DIRECTION_SECTOR_COUNT;
    while (unresolved > 0) {
        for (direction = 0; direction < DIRECTION_SECTOR_COUNT; direction++) {
            if (m_directionMap[direction] == -1) {
                next = (direction + 1) % DIRECTION_SECTOR_COUNT;
                previous =
                    (direction + DIRECTION_SECTOR_COUNT - 1) % DIRECTION_SECTOR_COUNT;
                if (m_directionMap[next] >= 0
                    && m_directionMap[next] <= H2EnumIndex(COMBAT_DIRECTION_WIDE_EAST)) {
                    m_directionMap[direction] =
                        m_directionMap[next] + DIRECTION_PENDING_OFFSET;
                } else if (m_directionMap[previous] >= 0
                           && m_directionMap[previous] <= H2EnumIndex(COMBAT_DIRECTION_WIDE_EAST)) {
                    m_directionMap[direction] =
                        m_directionMap[previous] + DIRECTION_PENDING_OFFSET;
                }
            }
        }
        unresolved = 0;
        for (direction = 0; direction < DIRECTION_SECTOR_COUNT; direction++) {
            if (m_directionMap[direction] >= DIRECTION_PENDING_OFFSET) {
                m_directionMap[direction] -= DIRECTION_PENDING_OFFSET;
            } else if (m_directionMap[direction] == INVALID_HEX) {
                unresolved++;
            }
        }
    }

    currentArmy->m_targetSide = targetSide;
    currentArmy->m_targetIndex = targetIndex;
}

void combatManager::CheckSetMouseDirection(i32 mouseX, i32 mouseY, i32 targetHex) {
    if (m_gridSelectionDisabled != 0)
        return;
    if (m_validDirectionCount <= 1 && m_mouseDirection >= 0)
        return;

    i32 relativeX =
        mouseX - (targetHex % COMBAT_GRID_ROW_LENGTH - 1) * COMBAT_HEX_HORIZONTAL_STEP;
    relativeX -= COMBAT_HEX_GRID_LEFT_ORIGIN;
    if (((targetHex / COMBAT_GRID_ROW_LENGTH) & 1) == 0)
        relativeX -= COMBAT_HEX_ROW_STAGGER;
    i32 relativeY = mouseY - COMBAT_HEX_CENTER_Y_ORIGIN
                      - targetHex / COMBAT_GRID_ROW_LENGTH * COMBAT_HEX_VERTICAL_STEP;
    relativeY -= DIRECTION_MOUSE_TOP_OFFSET;
    relativeX -= COMBAT_HEX_ROW_STAGGER;
    relativeY -= DIRECTION_MOUSE_CENTER_OFFSET;

    i32 sector = 0;
    if (relativeX < 0) {
        if (relativeY < 0)
            sector += DIRECTION_SECTOR_THREE_QUARTERS;
        else
            sector += DIRECTION_SECTOR_HALF;
    } else {
        if (relativeY < 0)
            sector += DIRECTION_SECTOR_ORIGIN;
        else
            sector += DIRECTION_SECTOR_QUARTER;
    }

    relativeX = abs(relativeX);
    relativeY = abs(relativeY);

    float ratio = static_cast<float>(relativeX) / (static_cast<float>(relativeY));
    if (sector == 0 || sector == DIRECTION_SECTOR_HALF) {
        if (ratio > COMBAT_DIRECTION_SLOPE_STEEPEST)
            sector += DIRECTION_SECTOR_OFFSET_STEEPEST;
        else if (ratio > COMBAT_DIRECTION_SLOPE_STEEP)
            sector += DIRECTION_SECTOR_OFFSET_STEEP;
        else if (ratio > COMBAT_DIRECTION_SLOPE_DIAGONAL)
            sector += DIRECTION_SECTOR_OFFSET_DIAGONAL;
        else if (ratio > COMBAT_DIRECTION_SLOPE_SHALLOW)
            sector += DIRECTION_SECTOR_OFFSET_SHALLOW;
        else if (ratio > COMBAT_DIRECTION_SLOPE_SHALLOWEST)
            sector++;
    } else {
        if (ratio < COMBAT_DIRECTION_SLOPE_SHALLOWEST)
            sector += DIRECTION_SECTOR_OFFSET_STEEPEST;
        else if (ratio < COMBAT_DIRECTION_SLOPE_SHALLOW)
            sector += DIRECTION_SECTOR_OFFSET_STEEP;
        else if (ratio < COMBAT_DIRECTION_SLOPE_DIAGONAL)
            sector += DIRECTION_SECTOR_OFFSET_DIAGONAL;
        else if (ratio < COMBAT_DIRECTION_SLOPE_STEEP)
            sector += DIRECTION_SECTOR_OFFSET_SHALLOW;
        else if (ratio < COMBAT_DIRECTION_SLOPE_STEEPEST)
            sector++;
    }

    if (m_directionMap[sector] == m_mouseDirection)
        return;

    m_mouseDirection = m_directionMap[sector];
    CombatHexDirection direction =
        OppositeDirection(CombatHexDirectionFromCode(m_directionMap[sector]));
    CombatHexDirection directionCopy = direction;
    CombatHexDirection alternateDirection = COMBAT_DIRECTION_INVALID;
    army* currentArmy = &m_armies[H2EnumIndex(m_currentArmySide)][m_currentArmyIndex];

    if (direction == COMBAT_DIRECTION_WIDE_WEST || direction == COMBAT_DIRECTION_WIDE_EAST) {
        if ((H2EnumIndex((currentArmy->m_monster.attributes) & (MONSTER_FLAGS_WIDE))) != 0) {
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
            if (HEX_HAS_OCCUPANT(
                    m_hexCells[targetHex - 1],
                    currentArmy->m_targetSide,
                    currentArmy->m_targetIndex
                )) {
                targetHex--;
            }
            if (direction == COMBAT_DIRECTION_WIDE_WEST)
                direction = COMBAT_DIRECTION_NORTHEAST;
            else
                direction = COMBAT_DIRECTION_SOUTHEAST;
        }
    } else {
        if (currentArmy->m_facing == ARMY_FACING_RIGHT
            && (H2EnumIndex((currentArmy->m_monster.attributes) & (MONSTER_FLAGS_WIDE))) != 0) {
            if (direction == COMBAT_DIRECTION_NORTHWEST || direction == COMBAT_DIRECTION_WEST
                || direction == COMBAT_DIRECTION_SOUTHWEST)
                targetHex--;
        } else if (currentArmy->m_facing == ARMY_FACING_LEFT
                   && (H2EnumIndex((currentArmy->m_monster.attributes) & (MONSTER_FLAGS_WIDE))) != 0
                   && (direction == COMBAT_DIRECTION_NORTHEAST
                       || direction == COMBAT_DIRECTION_EAST
                       || direction == COMBAT_DIRECTION_SOUTHEAST)) {
            targetHex++;
        }
    }

    m_directionTargetHex = m_adjacency[targetHex][H2EnumIndex(direction)];
    i32 rearHex = IGNORED_HEX;
    if (currentArmy->m_facing == ARMY_FACING_LEFT
        && (H2EnumIndex((currentArmy->m_monster.attributes) & (MONSTER_FLAGS_WIDE))) != 0) {
        rearHex = m_directionTargetHex - 1;
    }
    if (currentArmy->m_facing == ARMY_FACING_RIGHT
        && (H2EnumIndex((currentArmy->m_monster.attributes) & (MONSTER_FLAGS_WIDE))) != 0) {
        rearHex = m_directionTargetHex + 1;
    }
    if (ValidHexToStandOn(m_directionTargetHex) == 0 || ValidHexToStandOn(rearHex) == 0) {
        if ((H2EnumIndex((currentArmy->m_monster.attributes) & (MONSTER_FLAGS_WIDE))) != 0
            && (directionCopy == COMBAT_DIRECTION_WIDE_WEST
                || directionCopy == COMBAT_DIRECTION_WIDE_EAST)) {
            if (currentArmy->m_facing == ARMY_FACING_RIGHT)
                m_directionTargetHex++;
            else
                m_directionTargetHex--;
        } else {
            if (alternateDirection != COMBAT_DIRECTION_INVALID)
                m_directionTargetHex = m_adjacency[targetHex][H2EnumIndex(alternateDirection)];
        }
    }
    gpMouseManager->SetPointer(m_mouseDirection + POINTER_ATTACK_OFFSET);
}

i32 combatManager::GetPointer(CombatMessageCommand command, i32) {

    if (command == COMBAT_MESSAGE_COMMAND_OPPOSING_OPTIONS) {
        return POINTER_VIEW;
    } else {
        switch (command) {
            case COMBAT_MESSAGE_COMMAND_MOVE:
            case COMBAT_MESSAGE_COMMAND_FLY:
            case COMBAT_MESSAGE_COMMAND_SHOOT:
            case COMBAT_MESSAGE_COMMAND_VIEW_INFO: {

                return H2EnumIndex(command);
            }
            default:
                return H2EnumIndex(command);
        }
    }
}

MessageDispatchResult combatManager::ProcessCombatMsg(tag_message& message) {
    i32 mouseX = message.payload.mouse.screenX;
    i32 mouseY = message.payload.mouse.screenY;

    i32 selectedHex;
    tag_message pendingMessage;

    switch (message.type) {
        case MESSAGE_WIDGET:
            if ((H2EnumIndex((message.payload.widget.modifiers) & (MESSAGE_MODIFIER_RIGHT_BUTTON)))) {
                if (IS_WIDGET_SELECTION_NOTIFICATION(message.payload.widget.command)) {
                    i32 helpIndex = -1;
                    switch (CombatControlIdFromCode(message.payload.widget.id)) {
                        case CONTROL_MAIN_BUTTON:
                            RightClick(m_selectedHex);
                            break;
                        case CONTROL_DEFER_TURN:
                            helpIndex = H2EnumIndex(LONG_HELP_DEFER_TURN);
                            break;
                        case CONTROL_SKIP_TURN:
                            helpIndex = H2EnumIndex(LONG_HELP_SKIP_TURN);
                            break;
                        case CONTROL_DISABLE_SELECTION:
                            helpIndex = H2EnumIndex(LONG_HELP_DISABLE_SELECTION);
                            break;
                        case CONTROL_SYSTEM_OPTIONS:
                            helpIndex = H2EnumIndex(LONG_HELP_SYSTEM_OPTIONS);
                            break;
                        case CONTROL_HELP_FIRST:
                        case CONTROL_HELP_SECOND:
                        case CONTROL_HELP_THIRD:
                        case CONTROL_HELP_FOURTH:
                            helpIndex = H2EnumIndex(LONG_HELP_CONTROLS);
                            break;
                    }
                    if (helpIndex != -1) {
                        NormalDialog(cLongCombatHelp[helpIndex], NORMAL_DIALOG_QUICK_VIEW, NORMAL_DIALOG_NO_RESOURCE, NORMAL_DIALOG_NO_VALUE);
                    }
                }
                break;
            }
            switch (message.payload.widget.command) {
                case WIDGET_NOTIFY_SELECT:
                    switch (CombatControlIdFromCode(message.payload.widget.id)) {
                        case CONTROL_MAIN_BUTTON:
                            DoCommand(m_currentCommand);
                            break;
                    }
                    break;
                case WIDGET_NOTIFY_DESELECT:
                    switch (CombatControlIdFromCode(message.payload.widget.id)) {
                        case CONTROL_DISABLE_SELECTION:
                            m_gridSelectionDisabled = true;
                            break;
                        case CONTROL_SKIP_TURN:
                            giNextAction = ACTION_SKIP_TURN;
                            break;
                        case CONTROL_DEFER_TURN:
                            giNextAction = ACTION_DEFER_TURN;
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
            pendingMessage = gpInputManager->PeekEvent();
            if (pendingMessage.type == MESSAGE_MOUSE_MOVE)
                break;
            if (InCombatArea(message.payload.mouse.screenX, message.payload.mouse.screenY) != 0)
                selectedHex = GetGridIndex(mouseX, mouseY);
            else
                selectedHex = INVALID_HEX;

            UpdateMouseGrid(selectedHex, 0);
            if (InCombatArea(message.payload.mouse.screenX, message.payload.mouse.screenY) != 0) {
                if (selectedHex != m_selectedHex || selectedHex == INVALID_HEX) {
                    m_selectedHex = selectedHex;
                    m_previousCommand = COMBAT_INVALID_COMMAND;
                    m_currentCommand = GetCommand(m_selectedHex);
                    m_mouseDirection = INVALID_HEX;
                    if (m_currentCommand == COMBAT_MESSAGE_COMMAND_ATTACK) {
                        SetCombatDirections(selectedHex);
                        CheckSetMouseDirection(mouseX, mouseY, selectedHex);
                    } else {
                        gpMouseManager->SetPointer(GetPointer(m_currentCommand, selectedHex));
                    }
                } else if (m_currentCommand == COMBAT_MESSAGE_COMMAND_ATTACK) {
                    CheckSetMouseDirection(mouseX, mouseY, selectedHex);
                }
                if (m_currentCommand != m_previousCommand) {
                    m_previousCommand = m_currentCommand;
                    CombatMessage(m_currentCommand);
                }
            } else {
                if (mouseX >= CONTROL_RIGHT_MIN_X) {
                    CombatMessage(cCombatHelp[H2EnumIndex(HELP_SKIP_UNIT)], 1, 0, 0);
                } else if (mouseX <= CONTROL_LEFT_MAX_X && mouseY < CONTROL_SYSTEM_OPTIONS_MIN_Y) {
                    CombatMessage(cCombatHelp[H2EnumIndex(HELP_AUTO_COMBAT)], 1, 0, 0);
                } else if (mouseX <= CONTROL_LEFT_MAX_X) {
                    CombatMessage(cCombatHelp[H2EnumIndex(HELP_SYSTEM_OPTIONS)], 1, 0, 0);
                } else {
                    CombatMessage(cCombatHelp[H2EnumIndex(HELP_OTHER_CONTROL)], 1, 0, 0);
                }
                gpMouseManager->SetPointer(COMBAT_POINTER_DEFAULT);
                m_selectedHex = INVALID_HEX;
                m_previousCommand = COMBAT_INVALID_COMMAND;
            }
            return MESSAGE_DISPATCH_CONSUME;

        case MESSAGE_KEY_DOWN:
            switch (InputManagerScanCodeFromCode(message.payload.keyboard.keyCode)) {
                case H2EnumIndex(INPUT_SCAN_F2):
                    PopNetBox(NULL, -1);
                    break;
                case H2EnumIndex(INPUT_SCAN_F3):
                    gpWindowManager->UpdateScreenRegion(
                        0,
                        0,
                        LOGICAL_SCREEN_WIDTH - 1,
                        LOGICAL_SCREEN_HEIGHT - 1
                    );
                    break;
                case H2EnumIndex(INPUT_SCAN_F5):
                    SetCombatViewArmySmallLevel(
                        (gConfig.combatArmyInfoLevel + 1) % ARMY_VIEW_LEVEL_COUNT
                    );
                    break;
                case H2EnumIndex(INPUT_SCAN_F6):
                    SetCombatGrid(
                        1 - gConfig.showCombatGrid,
                        gConfig.showCombatMouseHex,
                        gConfig.combatShadeLevel
                    );
                    break;
                case H2EnumIndex(INPUT_SCAN_F7):
                    SetCombatGrid(
                        gConfig.showCombatGrid,
                        1 - gConfig.showCombatMouseHex,
                        gConfig.combatShadeLevel
                    );
                    break;
                case H2EnumIndex(INPUT_SCAN_F8):
                    SetCombatGrid(
                        gConfig.showCombatGrid,
                        gConfig.showCombatMouseHex,
                        1 - gConfig.combatShadeLevel
                    );
                    break;
                case H2EnumIndex(INPUT_SCAN_F9):
                    giSpellEffectShowType = NextSpellEffectDisplayType(giSpellEffectShowType);
                    DrawFrame(1, 0, 0, 0, COMMAND_FRAME_DELAY, 1, 1);
                    break;
                case H2EnumIndex(INPUT_SCAN_F11):
                    if ((H2EnumIndex((message.payload.keyboard.modifiers) & (MESSAGE_MODIFIER_SHIFT_KEYS)))) {
                        VaporizeCreature(COMBAT_DEFENDER_SIDE, 1);
                    } else if ((H2EnumIndex((message.payload.keyboard.modifiers) & (MESSAGE_MODIFIER_CONTROL_KEYS)))) {
                        RippleCreature(COMBAT_DEFENDER_SIDE, 1, COMBAT_RIPPLE_DEATH_RIPPLE);
                        RippleCreature(COMBAT_DEFENDER_SIDE, 1, COMBAT_RIPPLE_DEATH_WAVE);
                    } else {
                        RippleCreature(COMBAT_DEFENDER_SIDE, 1, COMBAT_RIPPLE_WAVE);
                    }
                    break;
                case H2EnumIndex(INPUT_SCAN_SPACE):
                    giNextAction = ACTION_SKIP_TURN;
                    break;
                case H2EnumIndex(INPUT_SCAN_1): {
                    i32 currentMouseX;
                    i32 currentMouseY;
                    gpMouseManager->MouseCoords(currentMouseX, currentMouseY);
                    break;
                }
                case H2EnumIndex(INPUT_SCAN_H):
                    if (m_heroes[H2EnumIndex(m_currentSide)] != NULL) {
                        gpMouseManager->SetPointer(COMBAT_POINTER_DEFAULT);
                        ViewGeneral(m_currentSide, 1, 0);
                        ResetMouse();
                    }
                    break;
                case H2EnumIndex(INPUT_SCAN_T):
                    gpMouseManager->SetPointer(COMBAT_POINTER_DEFAULT);
                    ViewArmy(&m_armies[H2EnumIndex(m_currentArmySide)][m_currentArmyIndex], 0);
                    ResetMouse();
                    break;
                case H2EnumIndex(INPUT_SCAN_C):
                    if (m_heroes[H2EnumIndex(m_currentSide)] == NULL) {
                        NormalDialog(localization::Tr("combat.spell.no_hero"), NORMAL_DIALOG_INFO, NORMAL_DIALOG_NO_RESOURCE, NORMAL_DIALOG_NO_VALUE);
                    } else if (IsNegationSphereInEffect() != 0) {
                        NormalDialog(localization::Tr("combat.spell.negation_sphere"), NORMAL_DIALOG_INFO, NORMAL_DIALOG_NO_RESOURCE, NORMAL_DIALOG_NO_VALUE);
                    } else if (m_heroCastSpell[H2EnumIndex(m_currentSide)] != 0 && giDebugLevel == 0) {
                        NormalDialog(localization::Tr("combat.spell.already_cast"), NORMAL_DIALOG_INFO, NORMAL_DIALOG_NO_RESOURCE, NORMAL_DIALOG_NO_VALUE);
                    } else {
                        gpMouseManager->SetPointer(COMBAT_POINTER_DEFAULT);
                        giCurGeneral = m_currentSide;
                        ViewSpells(0);
                        ResetMouse();
                    }
                    break;
                default:
                    break;
            }
            break;
    }

    return MESSAGE_DISPATCH_CONSUME;
}

i32 combatManager::IsNegationSphereInEffect(void) {
    CombatSide side;
    for (side = COMBAT_ATTACKER_SIDE; H2EnumIndex(side) < COMBAT_SIDE_COUNT; side++) {
        if (m_heroes[H2EnumIndex(side)] != NULL
            && m_heroes[H2EnumIndex(side)]->HasArtifact(ARTIFACT_SPHERE_NEGATION) != 0)
            return 1;
    }
    return 0;
}

void combatManager::ResetRound(void) {

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
    for (side = COMBAT_ATTACKER_SIDE; H2EnumIndex(side) < COMBAT_SIDE_COUNT; side++) {
        for (armyIndex = 0; armyIndex < COMBAT_ARMY_SLOT_COUNT; armyIndex++) {
            army* currentArmy = m_armies[H2EnumIndex(side)] + armyIndex;
            if (currentArmy->m_quantity > 0) {
                currentArmy->m_monster.attributes &= MONSTER_FLAGS_ROUND_PERSISTENT_MASK;
                if (IS_TROLL_CREATURE(currentArmy->m_monsterType))
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

    // Astral dodge re-arms every round; fire walls burn down over rounds.
    for (side = COMBAT_ATTACKER_SIDE; H2EnumIndex(side) < COMBAT_SIDE_COUNT; side++) {
        for (armyIndex = 0; armyIndex < COMBAT_ARMY_SLOT_COUNT; armyIndex++) {
            army* currentArmy = m_armies[H2EnumIndex(side)] + armyIndex;
            if (currentArmy->m_monsterType != CREATURE_NONE
                && CreatureHasAttribute(H2EnumIndex(currentArmy->m_monsterType), ASTRAL_DODGE))
                gIronfistExtra.combat.stack.abilityCounter[currentArmy][ASTRAL_DODGE] = 1;
        }
    }
    auto wall = gIronfistExtra.combat.spell.fireBombWalls.begin();
    while (wall != gIronfistExtra.combat.spell.fireBombWalls.end()) {
        wall->turnsLeft--;
        if (wall->turnsLeft < 0)
            wall = gIronfistExtra.combat.spell.fireBombWalls.erase(wall);
        else
            ++wall;
    }
}

// Which of the six hex sectors the cursor points at, for the plasma-cone
// stream. A stripped-down CheckSetMouseDirection.
CursorDirection combatManager::GetCursorDirection(i32 screenX, i32 screenY, i32 hex) {
    i32 offsetX = screenX - 44 * (hex % COMBAT_GRID_ROW_LENGTH - 1) - 67;
    if (!(hex / COMBAT_GRID_ROW_LENGTH & 1)) {
        offsetX = screenX - 44 * (hex % COMBAT_GRID_ROW_LENGTH - 1) - 89;
    }
    i32 offsetY = screenY - 63 - 42 * (hex / COMBAT_GRID_ROW_LENGTH) - 26;

    // The hex divides into 24 equal triangles; find the cursor's one.
    i32 hexPart = 0;
    i32 offsetXFromCenter = offsetX - 22;
    if (offsetXFromCenter >= 0) {
        if (offsetY >= 0) {
            hexPart = 6;
        }
    } else if (offsetY >= 0) {
        hexPart = 12;
    } else {
        hexPart = 18;
    }

    float slope = static_cast<float>(abs(offsetXFromCenter)) / abs(offsetY);
    if (hexPart && hexPart != 12) {
        if (slope >= 0.27f) {
            if (slope >= 0.58f) {
                if (slope >= 1.0f) {
                    if (slope >= 1.73f) {
                        if (slope < 3.73f) {
                            ++hexPart;
                        }
                    } else {
                        hexPart += 2;
                    }
                } else {
                    hexPart += 3;
                }
            } else {
                hexPart += 4;
            }
        } else {
            hexPart += 5;
        }
    } else if (slope <= 3.73f) {
        if (slope <= 1.73f) {
            if (slope <= 1.0f) {
                if (slope <= 0.58f) {
                    if (slope > 0.27f) {
                        ++hexPart;
                    }
                } else {
                    hexPart += 2;
                }
            } else {
                hexPart += 3;
            }
        } else {
            hexPart += 4;
        }
    } else {
        hexPart += 5;
    }

    if (hexPart < 4) {
        return CURSOR_DIRECTION_LEFT_DOWN;
    } else if (hexPart < 8) {
        return CURSOR_DIRECTION_LEFT;
    } else if (hexPart < 12) {
        return CURSOR_DIRECTION_LEFT_UP;
    } else if (hexPart < 16) {
        return CURSOR_DIRECTION_RIGHT_UP;
    } else if (hexPart < 20) {
        return CURSOR_DIRECTION_RIGHT;
    }
    return CURSOR_DIRECTION_RIGHT_DOWN;
}

void combatManager::CheckBurnCreature(army* stack) {
    for (auto& wall : gIronfistExtra.combat.spell.fireBombWalls) {
        if (wall.hexIdx == stack->m_hex) {
            stack->SetSpellInfluence(ARMY_SPELL_INFLUENCE_BURN, COMBAT_BURN_ROUNDS);
            BurnCreature(stack);
        }
    }
}

void combatManager::BurnCreature(army* stack) {
    // The walking frames offset differently; force the wince pose before
    // the effect animation.
    stack->m_animationSequence = ARMY_ANIMATION_WINCE;
    stack->m_animationFrame = 0;
    stack->SpellEffect(gsSpellInfo[H2EnumIndex(SPELL_FIRE_BOMB)].combatEffect, 0, 0);

    i32 burnDamage = COMBAT_BURN_BASE_DAMAGE + SRandom(0, COMBAT_BURN_RANDOM_DAMAGE);
    i32 creaturesKilled = stack->Damage(burnDamage, SPELL_FIRE_BOMB);

    // PowEffect clobbers the render extents; keep them intact.
    i32 minExtentX = giMinExtentX;
    i32 minExtentY = giMinExtentY;
    i32 maxExtentX = giMaxExtentX;
    i32 maxExtentY = giMaxExtentY;
    stack->PowEffect(COMBAT_EFFECT_INVALID, 1, -1, -1);
    giMinExtentX = minExtentX;
    giMinExtentY = minExtentY;
    giMaxExtentX = maxExtentX;
    giMaxExtentY = maxExtentY;

    utf8::Format(gText, GLOBAL_TEXT_BUFFER_SIZE, localization::Tr("combat.burning.damage"), burnDamage);
    if (creaturesKilled > 0) {
        const char* targetCreature = creaturesKilled > 1
            ? GetCreaturePluralName(H2EnumIndex(stack->m_monsterType))
            : GetCreatureName(H2EnumIndex(stack->m_monsterType));
        const std::size_t messageLength = strlen(gText);
        utf8::Format(
            gText + messageLength, GLOBAL_TEXT_BUFFER_SIZE - messageLength,
            localization::TrPlural("combat.burning.killed", creaturesKilled),
            creaturesKilled,
            targetCreature
        );
    }
    CombatMessage(gText, 1, 1, 0);
}

i32 combatManager::CheckWin(struct tag_message* message) {
    b32 combatEnded = false;
    // The auto-win debug toggle hands the battle to the human side.
    if (gbAutoWinBattles) {
        if (m_playerId[0] != -1 && gbHumanPlayer[m_playerId[0]])
            m_combatResult = COMBAT_RESULT_ATTACKER;
        else if (m_playerId[1] != -1 && gbHumanPlayer[m_playerId[1]])
            m_combatResult = COMBAT_RESULT_DEFENDER;
        DoVictory(m_combatResult);
        if (gbNoShowCombat == 0) {
            message->type = MESSAGE_EXECUTIVE;
            message->payload.executive.command = EXECUTIVE_COMMAND_TERMINATE_LOOP;
        }
        return 1;
    }
    if (IsWinner(m_currentSide) != 0) {
        combatEnded = true;
        if (IsWinner(OppositeCombatSide(m_currentSide)) != 0)
            m_combatResult = COMBAT_RESULT_DRAW;
        else
            m_combatResult = CombatResultForSide(m_currentSide);
    } else if (IsWinner(OppositeCombatSide(m_currentSide)) != 0) {
        combatEnded = true;
        m_combatResult = CombatResultForSide(OppositeCombatSide(m_currentSide));
    } else if (m_sideRetreated[0] != 0 || m_sideRetreated[1] != 0) {
        combatEnded = true;
        gbRetreatWin = true;
        m_combatResult = m_sideRetreated[0] != 0 ? COMBAT_RESULT_DEFENDER : COMBAT_RESULT_ATTACKER;
    }

    if (combatEnded != 0 && m_combatResult != COMBAT_RESULT_DRAW) {
        b32 armyAlive = false;

        i32 armyIndex;
        for (armyIndex = 0; armyIndex < COMBAT_ARMY_SLOT_COUNT; armyIndex++) {
            if (m_armies[H2EnumIndex(m_combatResult)][armyIndex].m_monsterType != CREATURE_NONE
                && m_armies[H2EnumIndex(m_combatResult)][armyIndex].m_quantity > 0
                && (H2EnumIndex((m_armies[H2EnumIndex(m_combatResult)][armyIndex].m_monster.attributes) & (MONSTER_FLAGS_SUMMONED)))
                       == 0) {
                armyAlive = true;
            }
        }
        if (armyAlive == 0)
            m_combatResult = COMBAT_RESULT_DRAW;
    }

    if (combatEnded != 0) {
        DoVictory(m_combatResult);
        if (gbNoShowCombat == 0) {
            message->type = MESSAGE_EXECUTIVE;
            message->payload.executive.command = EXECUTIVE_COMMAND_TERMINATE_LOOP;
        }
    }
    return combatEnded;
}

CombatMessageCommand combatManager::GetCommand(i32 hexIndex) {

    CombatMessageCommand command = COMBAT_MESSAGE_COMMAND_DEFAULT;
    b32 showEnemy = false;
    CombatSide enemySide;
    i32 targetIndex;
    army* ourArmy;

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

            enemySide = m_hexCells[hexIndex].m_occupantSide;
            targetIndex = m_hexCells[hexIndex].m_occupantIndex;
            ourArmy = &m_armies[H2EnumIndex(m_currentArmySide)][m_currentArmyIndex];
            CLEAR_ARMY_TARGET(*ourArmy);

            if (m_hexCells[hexIndex].m_blocked != 0 && !CAN_PASS_CASTLE_GATE(hexIndex)) {
                command = COMBAT_MESSAGE_COMMAND_DEFAULT;
            } else if (enemySide != COMBAT_SIDE_NONE) {
                if (enemySide != m_currentArmySide || targetIndex != m_currentArmyIndex) {
                    showEnemy = true;
                    if (gbProcessingCombatAction == 0 && giNextAction == ACTION_NONE) {
                        m_smallViewSide[1] = enemySide;
                        m_smallViewArmyIndex[1] = targetIndex;
                        DrawSmallView(1, 1);
                    }
                }
                switch (enemySide) {
                    case COMBAT_ATTACKER_SIDE:
                    case COMBAT_DEFENDER_SIDE:
                        if (enemySide == m_currentSide
                            || (enemySide == m_currentArmySide
                                && targetIndex == m_currentArmyIndex)) {
                            return COMBAT_MESSAGE_COMMAND_VIEW_INFO;
                        } else {
                            ourArmy->m_targetSide = enemySide;
                            ourArmy->m_targetIndex = targetIndex;
                            if (ourArmy->m_monster.shots > 0
                                && ourArmy->GetAttackMask(
                                       ourArmy->m_hex,
                                       ARMY_ATTACK_TARGET_ENEMY,
                                       ARMY_HEX_INVALID
                                   ) == COMBAT_ALL_DIRECTIONS_BLOCKED) {
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
                            // A charger may attack down a clear straight line
                            // even when the walking path is blocked.
                            if (ourArmy->ValidPath(hexIndex, ARMY_PATH_ANY_TARGET_HEX) == 1
                                || (CreatureHasAttribute(
                                        H2EnumIndex(ourArmy->m_monsterType), CHARGER
                                    )
                                    && ourArmy->TargetOnStraightLine(hexIndex)
                                    && ourArmy->ValidFlight(hexIndex, ARMY_PATH_ANY_TARGET_HEX)
                                    && !ourArmy->FlightThroughObstacles(hexIndex)
                                    && ourArmy->GetStraightLineDistanceToHex(hexIndex)
                                           <= ourArmy->m_monster.speed))
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
                if (m_armies[H2EnumIndex(m_currentArmySide)][m_currentArmyIndex]
                        .ValidPath(hexIndex, ARMY_PATH_ANY_TARGET_HEX)
                    == 1) {
                    command = CombatMessageCommand(
                        (H2EnumIndex((m_armies[H2EnumIndex(m_currentArmySide)][m_currentArmyIndex]
                                .m_monster.attributes) & (MONSTER_FLAGS_FLYING)))
                            ? static_cast<i8>(H2EnumIndex(COMBAT_MESSAGE_COMMAND_FLY))
                            : static_cast<i8>(H2EnumIndex(COMBAT_MESSAGE_COMMAND_MOVE))
                    );
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

i32 combatManager::RightClick(i32 hexIndex) {

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
                                &m_armies[H2EnumIndex(side)][m_hexCells[m_selectedHex].m_occupantIndex],
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

void combatManager::DoCommand(CombatMessageCommand command) {

    switch (command) {
        case COMBAT_MESSAGE_COMMAND_DEFAULT:
            break;
        case COMBAT_MESSAGE_COMMAND_MOVE:
        case COMBAT_MESSAGE_COMMAND_FLY:
        case COMBAT_MESSAGE_COMMAND_SHOOT:
        case COMBAT_MESSAGE_COMMAND_SHOOT_THROUGH_WALL:
            SET_NEXT_COMBAT_MOVE(m_selectedHex);
            giNextActionExtra = -1;
            break;
        case COMBAT_MESSAGE_COMMAND_ATTACK:
            giNextActionGridIndex = m_selectedHex;
            if (m_playerId[H2EnumIndex(m_currentSide)] == -1
                || gbHumanPlayer[m_playerId[H2EnumIndex(m_currentSide)]] == 0
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
                    &m_armies[H2EnumIndex(m_hexCells[m_selectedHex].m_occupantSide)]
                             [m_hexCells[m_selectedHex].m_occupantIndex],
                    0
                );
            ResetMouse();
            break;
        case COMBAT_MESSAGE_COMMAND_CAST_SPELL:
            if (IsNegationSphereInEffect() != 0) {
                NormalDialog(localization::Tr("combat.spell.negation_sphere"), NORMAL_DIALOG_INFO, NORMAL_DIALOG_NO_RESOURCE, NORMAL_DIALOG_NO_VALUE);
                break;
            }
            ViewSpells(0);
            ResetMouse();
            break;
        case COMBAT_MESSAGE_COMMAND_RETREAT:
            NormalDialog(localization::Tr("combat.confirm.retreat"), NORMAL_DIALOG_CONFIRM, NORMAL_DIALOG_NO_RESOURCE, NORMAL_DIALOG_NO_VALUE);
            if (gpWindowManager->m_dialogResult == DIALOG_BUTTON_5)
                giNextAction = ACTION_RETREAT;
            ResetMouse();
            break;
        case COMBAT_MESSAGE_COMMAND_SURRENDER:
            if (DoSurrender() == 1) {
                if (gpGame->m_players[m_playerId[H2EnumIndex(m_currentSide)]].m_resources[H2EnumIndex(RES_GOLD)]
                    < giSurrenderCost) {
                    NormalDialog(localization::Tr("resource.gold.insufficient"), NORMAL_DIALOG_INFO, NORMAL_DIALOG_NO_RESOURCE, NORMAL_DIALOG_NO_VALUE);
                } else {
                    giNextAction = ACTION_SURRENDER;
                    giNextActionExtra = giSurrenderCost;
                }
            }
            ResetMouse();
            break;
    }
}

MessageDispatchResult WinCombatHandler(struct tag_message& message) {
    char iconFile[WIN_LOSE_ICON_FILENAME_SIZE];
    tag_message animationMessage;

    i32 frame;
    i32 iDelay;

    if (giDialogTimeout != 0 && platform::Ticks() > giDialogTimeout) {
        message.type = MESSAGE_WIDGET;
        FINISH_DIALOG_MESSAGE(message);
        giDialogTimeout = 0;
        return MESSAGE_DISPATCH_FORWARD;
    }

    if (message.type == MESSAGE_WIDGET) {
        switch (message.payload.widget.command) {
            case WIDGET_NOTIFY_DESELECT:
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
                            bSkeletonsShown = true;
                            gpCombatManager->ClearWinLoseBottom(gpCombatManager->m_winLoseWindow);
                            gpCombatManager->ShowSkeletons(gpCombatManager->m_winLoseWindow);
                        } else {
                            if (gpCombatManager
                                    ->m_eagleEyeSpell[H2EnumIndex(gpCombatManager->m_combatResult)]
                                != SPELL_NONE) {
                                gpCombatManager->ClearWinLoseBottom(
                                    gpCombatManager->m_winLoseWindow
                                );
                                gpCombatManager->ShowEagleEyeSpell(
                                    gpCombatManager->m_winLoseWindow
                                );
                                gpCombatManager
                                    ->m_eagleEyeSpell[H2EnumIndex(gpCombatManager->m_combatResult)] =
                                    SPELL_NONE;
                            } else {
                            ExitDialog:
                                FINISH_DIALOG_MESSAGE(message);
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

    if (glTimers[0] < platform::Ticks()) {
        animationMessage.type = MESSAGE_WIDGET;
        animationMessage.payload.widget.command = WIDGET_COMMAND_SET_ICON;
        animationMessage.payload.widget.data.text = iconFile;
        ++giWinCmbtFrame;

        switch (gbWhichAnimationPlaying) {
            case WIN_LOSE_ANIMATION_CYCLE_FIRST:
                frame = giWinCmbtFrame % WIN_LOSE_LOOP_FRAME_COUNT + WIN_LOSE_FIRST_ANIMATION_FRAME;
                iDelay = WIN_LOSE_LOOP_DELAY;
                break;
            case WIN_LOSE_ANIMATION_CYCLE_SECOND:
                frame = giWinCmbtFrame % WIN_LOSE_LOOP_FRAME_COUNT + WIN_LOSE_FIRST_ANIMATION_FRAME;
                iDelay = WIN_LOSE_LOOP_DELAY;
                break;
            case WIN_LOSE_ANIMATION_FLEE:
                if (giWinCmbtFrame == WIN_LOSE_FLEE_SECOND_RESOURCE_FRAME) {
                    utf8::Format(iconFile, "cmbtfle2.icn");
                    animationMessage.payload.widget.id = WIN_LOSE_RESOURCE_LOAD_ID;
                    gpCombatManager->m_winLoseWindow->BroadcastMessage(animationMessage);
                    animationMessage.payload.widget.id = WIN_LOSE_RESOURCE_DRAW_ID;
                    gpCombatManager->m_winLoseWindow->BroadcastMessage(animationMessage);
                }
                if (giWinCmbtFrame == WIN_LOSE_FLEE_THIRD_RESOURCE_FRAME) {
                    utf8::Format(iconFile, "cmbtfle3.icn");
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
                    utf8::Format(iconFile, "cmbtlos2.icn");
                    animationMessage.payload.widget.id = WIN_LOSE_RESOURCE_LOAD_ID;
                    gpCombatManager->m_winLoseWindow->BroadcastMessage(animationMessage);
                    animationMessage.payload.widget.id = WIN_LOSE_RESOURCE_DRAW_ID;
                    gpCombatManager->m_winLoseWindow->BroadcastMessage(animationMessage);
                }
                if (giWinCmbtFrame == WIN_LOSE_LOSS_THIRD_RESOURCE_FRAME) {
                    utf8::Format(iconFile, "cmbtlos3.icn");
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

        SET_WIDGET_MESSAGE(message, WIDGET_COMMAND_SET_FRAME, WIN_LOSE_RESOURCE_DRAW_ID);
        message.payload.widget.data.value = frame;
        gpCombatManager->m_winLoseWindow->BroadcastMessage(message);
        gpCombatManager->m_winLoseWindow->DrawWindow(WINDOW_DRAW_UPDATE_SCREEN, 0, WINDOW_DRAW_ID_LIMIT);
        glTimers[0] = platform::Ticks() + iDelay;
    }
    return MESSAGE_DISPATCH_CONSUME;
}

void combatManager::ClearWinLoseBottom(class heroWindow* window) {
    i32 widgetIndex;
    for (widgetIndex = 0; widgetIndex < COMBAT_WIN_LOSE_WIDGET_COUNT; widgetIndex++) {
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

void combatManager::ShowWinLoseArtifact(
    class heroWindow* window,
    ArtifactType artifact
) {

    tag_message message;
    char* artifactName;

    utf8::Copy(gText, GLOBAL_TEXT_BUFFER_SIZE, localization::Tr("combat.reward.enemy_artifact"));
    SET_WIDGET_MESSAGE(message, WIDGET_COMMAND_SET_TEXT, WIN_LOSE_TEXT_ID);
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
        H2EnumIndex(artifact) + 1,
        ICON_DRAW_NORMAL,
        WIN_LOSE_ARTIFACT_IMAGE_ID,
        WIDGET_KIND_ICON_DIRECT,
        1
    );
    if (m_winLoseBottomWidgets[1] == NULL)
        MemError();
    window->AddWidget(m_winLoseBottomWidgets[1], -1);

    artifactName = static_cast<char*>(H2_ALLOC(ARTIFACT_NAME_CAPACITY));
    utf8::Copy(artifactName, ARTIFACT_NAME_CAPACITY, gArtifactNames[H2EnumIndex(artifact)]);
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
    utf8::Format(gText, GLOBAL_TEXT_BUFFER_SIZE, "pickup%02d.82M", SRandom(PICKUP_SAMPLE_FIRST, PICKUP_SAMPLE_LAST));
    playSample = LoadPlaySample(gText);
    WaitEndSample(&playSample);
}

void combatManager::ShowSkeletons(class heroWindow* window) {
    tag_message message;
    char* skeletonText;

    m_winLoseBottomWidgets[0] = new iconWidget(
        SKELETON_ICON_X,
        SKELETON_ICON_Y,
        SKELETON_ICON_WIDTH,
        SKELETON_ICON_HEIGHT,
        "mons32.icn",
        H2EnumIndex(CREATURE_SKELETON),
        ICON_DRAW_NORMAL,
        WIN_LOSE_SKELETON_ICON_ID,
        WIDGET_KIND_ICON_DIRECT,
        1
    );
    if (m_winLoseBottomWidgets[0] == NULL)
        MemError();

    skeletonText = static_cast<char*>(H2_ALLOC(SKELETON_COUNT_CAPACITY));
    utf8::Format(skeletonText, SKELETON_COUNT_CAPACITY, "%d", giSkeletonsCreated);
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
    utf8::Format(
        gText, GLOBAL_TEXT_BUFFER_SIZE,
        localization::TrPlural("combat.necromancy.raised", giSkeletonsCreated),
        giSkeletonsCreated
    );
    SET_WIDGET_MESSAGE(message, WIDGET_COMMAND_SET_TEXT, WIN_LOSE_TEXT_ID);
    message.payload.widget.data.text = gText;
    m_winLoseWindow->BroadcastMessage(message);
    gpCombatManager->m_winLoseWindow->DrawWindow();

    SAMPLE2 playSample;
    utf8::Format(gText, GLOBAL_TEXT_BUFFER_SIZE, "pickup%02d.82M", SRandom(PICKUP_SAMPLE_FIRST, PICKUP_SAMPLE_LAST));
    playSample = LoadPlaySample(gText);
    WaitEndSample(&playSample);
}

void combatManager::ShowEagleEyeSpell(class heroWindow* window) {
    SpellType newSpell = m_eagleEyeSpell[H2EnumIndex(m_combatResult)];
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
        gsSpellInfo[H2EnumIndex(newSpell)].iconIndex,
        ICON_DRAW_NORMAL,
        WIN_LOSE_EAGLE_SPELL_ID,
        WIDGET_KIND_ICON_CENTERED,
        1
    );
    if (m_winLoseBottomWidgets[1] == NULL)
        MemError();

    nameText = static_cast<char*>(H2_ALLOC(EAGLE_SPELL_NAME_CAPACITY));
    utf8::Format(
        nameText, EAGLE_SPELL_NAME_CAPACITY, "%s", gSpellNames[H2EnumIndex(newSpell)]
    );
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
    utf8::Format(
        gText, GLOBAL_TEXT_BUFFER_SIZE,
        localization::Tr("combat.eagle_eye.learned_spell")
,
        m_heroes[H2EnumIndex(m_combatResult)]->m_name,
        gSpellNames[H2EnumIndex(newSpell)]
    );
    SET_WIDGET_MESSAGE(message, WIDGET_COMMAND_SET_TEXT, WIN_LOSE_TEXT_ID);
    message.payload.widget.data.text = gText;
    m_winLoseWindow->BroadcastMessage(message);
    gpCombatManager->m_winLoseWindow->DrawWindow();

    SAMPLE2 playSample;
    utf8::Format(gText, GLOBAL_TEXT_BUFFER_SIZE, "pickup%02d.82M", SRandom(PICKUP_SAMPLE_FIRST, PICKUP_SAMPLE_LAST));
    playSample = LoadPlaySample(gText);
    WaitEndSample(&playSample);
}

void combatManager::ShowDeadArmies(class heroWindow* window) {

    i32 casualtyType[COMBAT_SIDE_COUNT][COMBAT_ARMY_SLOT_COUNT];
    i32 spacing;
    i32 casualtyY;

    i32 casualtyCount[COMBAT_SIDE_COUNT][COMBAT_ARMY_SLOT_COUNT];
    i32 side;

    i32 casualtyQuantity[COMBAT_SIDE_COUNT];
    i32 armyIndex;
    i32 displayedCount;
    i32 startX;
    char* text;
    icon* monsterIcons;

    for (side = 0; side < COMBAT_WIN_LOSE_WIDGET_COUNT; ++side) {
        m_winLoseBottomWidgets[side] = NULL;
        m_winLoseBottomTextWidgets[side] = NULL;
    }
    for (side = 0; side < COMBAT_SIDE_COUNT; ++side) {
        casualtyQuantity[side] = 0;
        for (armyIndex = 0; armyIndex < COMBAT_ARMY_SLOT_COUNT; ++armyIndex) {
            if (m_armies[side][armyIndex].m_monsterType != CREATURE_NONE
                && m_armies[side][armyIndex].m_initialQuantity
                       > m_armies[side][armyIndex].m_quantity) {
                casualtyType[side][casualtyQuantity[side]] =
                    H2EnumIndex(m_armies[side][armyIndex].m_monsterType);
                casualtyCount[side][casualtyQuantity[side]] =
                    m_armies[side][armyIndex].m_initialQuantity
                    - m_armies[side][armyIndex].m_quantity;
                ++casualtyQuantity[side];
            }
        }
    }

    text = static_cast<char*>(H2_ALLOC(CASUALTY_HEADING_CAPACITY));
    utf8::Copy(text, CASUALTY_HEADING_CAPACITY, localization::Tr("combat.casualties.title"));
    m_winLoseBottomTextWidgets[CASUALTY_TITLE_WIDGET] = new textWidget(
        CASUALTY_TITLE_X,
        CASUALTY_TITLE_Y,
        CASUALTY_WINDOW_WIDTH,
        CASUALTY_TEXT_HEIGHT,
        text,
        "smalfont.fnt",
        FONT_DRAW_DEFAULT,
        CASUALTY_TEXT_WIDGET_ID,
        WIDGET_KIND_TEXT,
        FONT_ALIGN_CENTER
    );
    if (m_winLoseBottomTextWidgets[CASUALTY_TITLE_WIDGET] == NULL)
        MemError();
    window->AddWidget(m_winLoseBottomTextWidgets[CASUALTY_TITLE_WIDGET], -1);

    for (side = 0; side < COMBAT_SIDE_COUNT; ++side) {
        casualtyY = side ? CASUALTY_DEFENDER_Y : CASUALTY_ATTACKER_Y;
        text = static_cast<char*>(H2_ALLOC(CASUALTY_HEADING_CAPACITY));
        utf8::Copy(
            text, CASUALTY_HEADING_CAPACITY,
            side == H2EnumIndex(COMBAT_ATTACKER_SIDE)
                ? localization::Tr("combat.casualties.attacker")
                : localization::Tr("combat.casualties.defender")
        );
        m_winLoseBottomTextWidgets[CASUALTY_SIDE_WIDGET_FIRST + side] = new textWidget(
            CASUALTY_TITLE_X,
            casualtyY + CASUALTY_SIDE_LABEL_Y_OFFSET,
            CASUALTY_WINDOW_WIDTH,
            CASUALTY_TEXT_HEIGHT,
            text,
            "smalfont.fnt",
            FONT_DRAW_DEFAULT,
            CASUALTY_TEXT_WIDGET_ID,
            WIDGET_KIND_TEXT,
            FONT_ALIGN_CENTER
        );
        if (m_winLoseBottomTextWidgets[CASUALTY_SIDE_WIDGET_FIRST + side] == NULL)
            MemError();
        window->AddWidget(m_winLoseBottomTextWidgets[CASUALTY_SIDE_WIDGET_FIRST + side], -1);

        if (casualtyQuantity[side] <= 0) {
            text = static_cast<char*>(H2_ALLOC(CASUALTY_NONE_CAPACITY));
            utf8::Format(
                text, CASUALTY_NONE_CAPACITY, "%s", localization::Tr("common.none")
            );
            m_winLoseBottomTextWidgets[side * CASUALTY_WIDGETS_PER_SIDE] = new textWidget(
                CASUALTY_TITLE_X,
                casualtyY + CASUALTY_NONE_Y_OFFSET,
                CASUALTY_WINDOW_WIDTH,
                CASUALTY_TEXT_HEIGHT,
                text,
                "smalfont.fnt",
                FONT_DRAW_DEFAULT,
                side * CASUALTY_WIDGET_ID_STRIDE + CASUALTY_TEXT_WIDGET_ID_FIRST,
                WIDGET_KIND_TEXT,
                FONT_ALIGN_CENTER
            );
            if (m_winLoseBottomTextWidgets[side * CASUALTY_WIDGETS_PER_SIDE] == NULL)
                MemError();
            window->AddWidget(m_winLoseBottomTextWidgets[side * CASUALTY_WIDGETS_PER_SIDE], -1);
        }

        monsterIcons = gpResourceManager->GetIcon("mons32.icn");
        displayedCount = casualtyQuantity[side] < CASUALTY_DISPLAY_LIMIT
                                ? casualtyQuantity[side]
                                : CASUALTY_DISPLAY_LIMIT;
        spacing = CASUALTY_ICON_SPACING;
        startX = (CASUALTY_WINDOW_WIDTH - spacing * displayedCount) / CASUALTY_CENTER_DIVISOR
                   + CASUALTY_CENTER_X_OFFSET;
        for (armyIndex = 0; armyIndex < displayedCount; ++armyIndex) {
            m_winLoseBottomWidgets[side * CASUALTY_WIDGETS_PER_SIDE + armyIndex] =
                new iconWidget(
                    startX + spacing * armyIndex + CASUALTY_ICON_CELL_WIDTH / 2
                        - GetIconEntry(monsterIcons, casualtyType[side][armyIndex])->x
                        + (CASUALTY_ICON_CELL_WIDTH
                           - GetIconEntry(monsterIcons, casualtyType[side][armyIndex])->w)
                              / CASUALTY_CENTER_DIVISOR
                        + 1,
                    casualtyY + CASUALTY_ICON_TOP_OFFSET
                        - GetIconEntry(monsterIcons, casualtyType[side][armyIndex])->y
                        + (CASUALTY_ICON_CELL_WIDTH
                           - GetIconEntry(monsterIcons, casualtyType[side][armyIndex])->h),
                    CASUALTY_ICON_WIDTH,
                    CASUALTY_ICON_HEIGHT,
                    "mons32.icn",
                    casualtyType[side][armyIndex],
                    ICON_DRAW_NORMAL,
                    side * CASUALTY_WIDGET_ID_STRIDE + armyIndex
                        + CASUALTY_ICON_WIDGET_ID_FIRST,
                    WIDGET_KIND_ICON_DIRECT,
                    1
                );
            if (m_winLoseBottomWidgets[side * CASUALTY_WIDGETS_PER_SIDE + armyIndex] == NULL)
                MemError();

            text = static_cast<char*>(H2_ALLOC(CASUALTY_QUANTITY_CAPACITY));
            utf8::Format(
                text, CASUALTY_QUANTITY_CAPACITY, "%d", casualtyCount[side][armyIndex]
            );
            m_winLoseBottomTextWidgets[side * CASUALTY_WIDGETS_PER_SIDE + armyIndex] =
                new textWidget(
                    startX + spacing * armyIndex + CASUALTY_QUANTITY_X_OFFSET,
                    casualtyY + CASUALTY_QUANTITY_Y_OFFSET,
                    CASUALTY_QUANTITY_WIDTH,
                    CASUALTY_QUANTITY_HEIGHT,
                    text,
                    "smalfont.fnt",
                    FONT_DRAW_DEFAULT,
                    side * CASUALTY_WIDGET_ID_STRIDE + armyIndex
                        + CASUALTY_TEXT_WIDGET_ID_FIRST,
                    WIDGET_KIND_TEXT,
                    FONT_ALIGN_CENTER
                );
            if (m_winLoseBottomTextWidgets[side * CASUALTY_WIDGETS_PER_SIDE + armyIndex]
                == NULL)
                MemError();
            window->AddWidget(
                m_winLoseBottomWidgets[side * CASUALTY_WIDGETS_PER_SIDE + armyIndex],
                -1
            );
            window->AddWidget(
                m_winLoseBottomTextWidgets[side * CASUALTY_WIDGETS_PER_SIDE + armyIndex],
                -1
            );
        }
        gpResourceManager->Dispose(monsterIcons);
    }
}

void combatManager::DoVictory(CombatResult winningSide) {
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

    i32 necroEligible;
    i32 emptySlots;
    char experienceText[VICTORY_EXPERIENCE_TEXT_SIZE];

    if (m_heroes[H2EnumIndex(COMBAT_DEFENDER_SIDE)] != NULL
        && m_heroes[H2EnumIndex(COMBAT_DEFENDER_SIDE)]->m_isCaptain != 0)
        m_heroes[H2EnumIndex(COMBAT_DEFENDER_SIDE)] = NULL;
    gbShowingLoseWindow = false;
    gbWhichAnimationPlaying = WIN_LOSE_ANIMATION_CYCLE_FIRST;
    giWinCmbtFrame = 0;
    giSkeletonsCreated = 0;
    iMaxTransferArtifacts = 0;
    iCurTransferArtifact = -1;
    bSkeletonsShown = false;
    bodies = 0;
    necroEligible = 0;

    for (combatSide = COMBAT_ATTACKER_SIDE; H2EnumIndex(combatSide) < COMBAT_SIDE_COUNT; ++combatSide) {
        livingCount = 0;
        lastLivingArmy = -1;
        for (troopIndex = 0; troopIndex < gpCombatManager->m_armyCount[H2EnumIndex(combatSide)];
             ++troopIndex) {
            pTroop = &m_armies[H2EnumIndex(combatSide)][troopIndex];
            if (pTroop->m_quantity > 0) {
                lastLivingArmy = troopIndex;
                if (pTroop->m_temporaryResurrectionQuantity > 0)
                    pTroop->m_quantity -= pTroop->m_temporaryResurrectionQuantity;
                if (pTroop->m_quantity < 0)
                    pTroop->m_quantity = 0;
                livingCount += pTroop->m_quantity;
            }
            if (CombatResultForSide(combatSide) == winningSide && pTroop->m_quantity > 0
                && (H2EnumIndex((pTroop->m_monster.attributes) & (MONSTER_FLAGS_LIGHT_PALETTE))) == 0
                && !IS_ELEMENTAL_CREATURE(pTroop->m_monsterType)
                && pTroop->m_monsterType != CREATURE_SKELETON) {
                ++necroEligible;
            }
            if (OppositeCombatResult(CombatResultForSide(combatSide)) == winningSide) {
                bodies += pTroop->m_initialQuantity - pTroop->m_quantity;
            }
        }
        if (livingCount == 0 && lastLivingArmy != -1)
            m_armies[H2EnumIndex(combatSide)][lastLivingArmy].m_quantity = 1;
    }

    if (winningSide != COMBAT_RESULT_DRAW && necroEligible < VICTORY_NECROMANCY_STACK_LIMIT
        && m_heroes[H2EnumIndex(winningSide)] != NULL
        && m_heroes[H2EnumIndex(winningSide)]->GetSSLevel(HERO_SKILL_NECROMANCY) != 0) {
        giSkeletonsCreated = static_cast<i32>(
            bodies
            * (m_heroes[H2EnumIndex(winningSide)]->GetSSLevel(HERO_SKILL_NECROMANCY)
               * COMBAT_NECROMANCY_LEVEL_FACTOR)
        );
        if (giSkeletonsCreated <= 0 && bodies != 0)
            giSkeletonsCreated = 1;
    }

    m_nonVisualCombat = true;
    FreeArmies();
    CombatMessage("", 1, 1, 0);
    gpMouseManager->SetPointer(COMBAT_POINTER_DEFAULT);
    numFades = VICTORY_FADE_STEPS;
    if (m_terrainType == TERRAIN_WASTELAND)
        numFades = VICTORY_WASTELAND_FADE_STEPS;
    waitTimer = platform::Ticks();
    for (loop = 0; loop < numFades; ++loop) {
        PollSound();
        DelayTil(&waitTimer);
        waitTimer = platform::Ticks() + VICTORY_FADE_DELAY;
        DimBitmapArea(
            gpWindowManager->m_screen,
            0,
            0,
            LOGICAL_SCREEN_WIDTH,
            LOGICAL_SCREEN_HEIGHT,
            VICTORY_DIM_STEP
        );
        PollSound();
        gpWindowManager->UpdateScreenRegion(0, 0, LOGICAL_SCREEN_MAX_X, LOGICAL_SCREEN_MAX_Y);
        PollSound();
    }

    switch (winningSide) {
        case COMBAT_RESULT_DRAW:
            gpSoundManager->SwitchAmbientMusic(LOSS_MUSIC);
            DoLoseWindow();
            break;
        case COMBAT_RESULT_ATTACKER:
        case COMBAT_RESULT_DEFENDER:
            if (m_heroes[H2EnumIndex(winningSide)] != NULL) {
                if (m_eagleEyeSpell[H2EnumIndex(winningSide)] != SPELL_NONE) {
                    m_heroes[H2EnumIndex(winningSide)]->m_spells[H2EnumIndex(m_eagleEyeSpell[H2EnumIndex(winningSide)])] =
                        1;
                }
                m_experienceValue[H2EnumIndex(OppositeCombatResult(winningSide))] =
                    ExperienceValueOfStack(CombatSideForResult(OppositeCombatResult(winningSide)));
                if (gbRetreatWin != 0)
                    m_experienceValue[H2EnumIndex(OppositeCombatResult(winningSide))] -=
                        COMBAT_HERO_EXPERIENCE_VALUE;
                if (m_combatTowns[H2EnumIndex(COMBAT_DEFENDER_SIDE)] != NULL
                    && winningSide == COMBAT_RESULT_ATTACKER)
                    m_experienceValue[H2EnumIndex(OppositeCombatResult(winningSide))] +=
                        COMBAT_HERO_EXPERIENCE_VALUE;
                levelsGained = gpAdvManager->GiveExperience(
                    m_heroes[H2EnumIndex(winningSide)],
                    m_experienceValue[H2EnumIndex(OppositeCombatResult(winningSide))],
                    gbThisNetHumanPlayer[m_heroes[H2EnumIndex(winningSide)]->m_owner] == 0
                );

                if (gbRetreatWin == 0) {
                    emptySlots = 0;
                    if (m_heroes[H2EnumIndex(COMBAT_ATTACKER_SIDE)] != NULL
                        && m_heroes[H2EnumIndex(COMBAT_DEFENDER_SIDE)] != NULL) {
                        for (loop = 0; loop < HERO_ARTIFACT_SLOT_COUNT; ++loop) {
                            if (m_heroes[H2EnumIndex(winningSide)]->m_artifacts[loop]
                                == ARTIFACT_NONE) {
                                ++emptySlots;
                            }
                        }
                        for (loop = 0; loop < HERO_ARTIFACT_SLOT_COUNT; ++loop) {
                            if (m_heroes[H2EnumIndex(OppositeCombatResult(winningSide))]
                                        ->m_artifacts[loop]
                                    >= ARTIFACT_ARCANE_NECKLACE
                                && m_heroes[H2EnumIndex(OppositeCombatResult(winningSide))]
                                           ->m_artifacts[loop]
                                       != ARTIFACT_MAGIC_BOOK
                                && iMaxTransferArtifacts < emptySlots) {
                                iTransferArtifacts[iMaxTransferArtifacts] =
                                    m_heroes[H2EnumIndex(OppositeCombatResult(winningSide))]
                                        ->m_artifacts[loop];
                                iTransferArtifactsInfo[iMaxTransferArtifacts] =
                                    m_heroes[H2EnumIndex(OppositeCombatResult(winningSide))]
                                        ->m_artifactExtra[loop];
                                ++iMaxTransferArtifacts;
                            }
                        }
                    }
                }
            }

            if (!(giCurPlayer == -1 || gbThisNetHumanPlayer[giCurPlayer] == 0
                  || m_playerId[H2EnumIndex(winningSide)] != giCurPlayer)
                || !(
                    giCurPlayer == -1 || m_playerId[H2EnumIndex(winningSide)] == -1
                    || gbThisNetHumanPlayer[giCurPlayer] != 0
                    || gbThisNetHumanPlayer[m_playerId[H2EnumIndex(winningSide)]] == 0
                )
                || !(
                    m_playerId[H2EnumIndex(winningSide)] == -1
                    || gbThisNetHumanPlayer[m_playerId[H2EnumIndex(winningSide)]] == 0
                )) {
                gpSoundManager->SwitchAmbientMusic(VICTORY_MUSIC);
                m_winLoseWindow =
                    new heroWindow(WIN_LOSE_WINDOW_X, WIN_LOSE_WINDOW_Y, "wincmbt.bin");
                if (m_winLoseWindow == NULL)
                    MemError();

                if (m_heroes[H2EnumIndex(winningSide)] != NULL) {
                    if (gbCombatSurrender != 0) {
                        utf8::Copy(gText, GLOBAL_TEXT_BUFFER_SIZE, cBattleResults[H2EnumIndex(RESULT_TEXT_ENEMY_SURRENDERED)]);
                    } else if (gbRetreatWin != 0) {
                        utf8::Copy(gText, GLOBAL_TEXT_BUFFER_SIZE, cBattleResults[H2EnumIndex(RESULT_TEXT_ENEMY_FLED)]);
                    } else {
                        utf8::Copy(gText, GLOBAL_TEXT_BUFFER_SIZE, cBattleResults[H2EnumIndex(RESULT_TEXT_VICTORY)]);
                    }
                    if (levelsGained > 0 && winningSide == COMBAT_RESULT_DEFENDER
                        && giNumHumanPlayers > 1) {
                        utf8::Format(
                            experienceText,
                            cBattleResults[H2EnumIndex(RESULT_TEXT_NETWORK_EXPERIENCE)],
                            m_heroes[H2EnumIndex(winningSide)]->m_name,
                            m_experienceValue[H2EnumIndex(OppositeCombatResult(winningSide))],
                            levelsGained
                        );
                    } else {
                        utf8::Format(
                            experienceText,
                            cBattleResults[H2EnumIndex(RESULT_TEXT_EXPERIENCE)],
                            m_heroes[H2EnumIndex(winningSide)]->m_name,
                            m_experienceValue[H2EnumIndex(OppositeCombatResult(winningSide))]
                        );
                    }
                    strcat(gText, experienceText);
                    m_heroes[H2EnumIndex(winningSide)]->ApplyBattleWinTemps();
                } else {
                    if (gbCombatSurrender != 0) {
                        utf8::Copy(gText, GLOBAL_TEXT_BUFFER_SIZE, cBattleResults[H2EnumIndex(RESULT_TEXT_ENEMY_SURRENDERED)]);
                    } else if (gbRetreatWin != 0) {
                        utf8::Copy(gText, GLOBAL_TEXT_BUFFER_SIZE, cBattleResults[H2EnumIndex(RESULT_TEXT_ENEMY_FLED)]);
                    } else {
                        utf8::Copy(gText, GLOBAL_TEXT_BUFFER_SIZE, cBattleResults[H2EnumIndex(RESULT_TEXT_VICTORY)]);
                    }
                }
                SET_WIDGET_MESSAGE(message, WIDGET_COMMAND_SET_TEXT, WIN_LOSE_TEXT_ID);
                message.payload.widget.data.text = gText;
                m_winLoseWindow->BroadcastMessage(message);
                ShowDeadArmies(m_winLoseWindow);
                if (gbRemoteOn != 0 && gbThisNetGotAdventureControl == 0)
                    giDialogTimeout = platform::Ticks() + WIN_LOSE_DIALOG_TIMEOUT;
                gpWindowManager->DoDialog(m_winLoseWindow, WinCombatHandler, 0);
                giDialogTimeout = 0;
                delete m_winLoseWindow;
                if (m_heroes[H2EnumIndex(OppositeCombatResult(winningSide))] != NULL)
                    m_heroes[H2EnumIndex(OppositeCombatResult(winningSide))]->ApplyBattleLossTemps();
            } else {
                if (m_heroes[H2EnumIndex(winningSide)] != NULL)
                    m_heroes[H2EnumIndex(winningSide)]->ApplyBattleWinTemps();
                if (m_heroes[H2EnumIndex(OppositeCombatResult(winningSide))] != NULL)
                    m_heroes[H2EnumIndex(OppositeCombatResult(winningSide))]->ApplyBattleLossTemps();
                gpSoundManager->SwitchAmbientMusic(LOSS_MUSIC);
                DoLoseWindow();
            }
            break;
    }
    gMapX = gpAdvManager->m_mapOriginX;
    gMapY = gpAdvManager->m_mapOriginY;
}

void combatManager::DoLoseWindow(void) {
    CombatResult losingSide;

    tag_message message;
    char animationFile[WIN_LOSE_ANIMATION_FILENAME_SIZE];

    if (giCurPlayer == m_playerId[H2EnumIndex(COMBAT_ATTACKER_SIDE)]
        && gbThisNetHumanPlayer[m_playerId[H2EnumIndex(COMBAT_ATTACKER_SIDE)]] != 0) {
        losingSide = COMBAT_RESULT_ATTACKER;
    } else if (giCurPlayer == m_playerId[H2EnumIndex(COMBAT_DEFENDER_SIDE)]
               && gbThisNetHumanPlayer[m_playerId[H2EnumIndex(COMBAT_DEFENDER_SIDE)]] != 0) {
        losingSide = COMBAT_RESULT_DEFENDER;
    } else if (m_playerId[H2EnumIndex(COMBAT_ATTACKER_SIDE)] != -1
               && gbThisNetHumanPlayer[m_playerId[H2EnumIndex(COMBAT_ATTACKER_SIDE)]] != 0) {
        losingSide = COMBAT_RESULT_ATTACKER;
    } else {
        losingSide = COMBAT_RESULT_DEFENDER;
    }

    gbShowingLoseWindow = true;
    if (gbCombatSurrender != 0) {
        utf8::Format(animationFile, "cmbtsurr.icn");
        gbWhichAnimationPlaying = WIN_LOSE_ANIMATION_CYCLE_SECOND;
    } else if (gbRetreatWin != 0) {
        utf8::Format(animationFile, "cmbtfle1.icn");
        gbWhichAnimationPlaying = WIN_LOSE_ANIMATION_FLEE;
    } else {
        utf8::Format(animationFile, "cmbtlos1.icn");
        gbWhichAnimationPlaying = WIN_LOSE_ANIMATION_LOSS;
    }

    m_winLoseWindow = new heroWindow(WIN_LOSE_WINDOW_X, WIN_LOSE_WINDOW_Y, "wincmbt.bin");
    if (m_winLoseWindow == NULL)
        MemError();

    if (m_heroes[H2EnumIndex(losingSide)] != NULL) {
        if (gbCombatSurrender != 0) {
            utf8::Format(
                gText, GLOBAL_TEXT_BUFFER_SIZE,
                cBattleResults[H2EnumIndex(RESULT_TEXT_HERO_SURRENDER)],
                m_heroes[H2EnumIndex(losingSide)]->m_name
            );
        } else if (gbRetreatWin != 0) {
            utf8::Format(
                gText, GLOBAL_TEXT_BUFFER_SIZE,
                cBattleResults[H2EnumIndex(RESULT_TEXT_HERO_FLEE)],
                m_heroes[H2EnumIndex(losingSide)]->m_name
            );
        } else {
            utf8::Format(
                gText, GLOBAL_TEXT_BUFFER_SIZE,
                cBattleResults[H2EnumIndex(RESULT_TEXT_HERO_DEFEAT)],
                m_heroes[H2EnumIndex(losingSide)]->m_name
            );
        }
    } else {
        if (gbCombatSurrender != 0) {
            utf8::Copy(gText, GLOBAL_TEXT_BUFFER_SIZE, cBattleResults[H2EnumIndex(RESULT_TEXT_FORCES_SURRENDER)]);
        } else if (gbRetreatWin != 0) {
            utf8::Copy(gText, GLOBAL_TEXT_BUFFER_SIZE, cBattleResults[H2EnumIndex(RESULT_TEXT_FORCES_FLEE)]);
        } else {
            utf8::Copy(gText, GLOBAL_TEXT_BUFFER_SIZE, cBattleResults[H2EnumIndex(RESULT_TEXT_FORCES_DEFEAT)]);
        }
    }

    SET_WIDGET_MESSAGE(message, WIDGET_COMMAND_SET_ICON, WIN_LOSE_RESOURCE_LOAD_ID);
    message.payload.widget.data.text = animationFile;
    m_winLoseWindow->BroadcastMessage(message);
    message.payload.widget.id = WIN_LOSE_RESOURCE_DRAW_ID;
    message.payload.widget.data.text = animationFile;
    m_winLoseWindow->BroadcastMessage(message);
    SET_WIDGET_MESSAGE(message, WIDGET_COMMAND_SET_TEXT, WIN_LOSE_TEXT_ID);
    message.payload.widget.data.text = gText;
    m_winLoseWindow->BroadcastMessage(message);
    ShowDeadArmies(m_winLoseWindow);
    if (gbRemoteOn != 0 && gbThisNetGotAdventureControl == 0)
        giDialogTimeout = platform::Ticks() + WIN_LOSE_DIALOG_TIMEOUT;
    gpWindowManager->DoDialog(m_winLoseWindow, WinCombatHandler, 0);
    giDialogTimeout = 0;
    delete m_winLoseWindow;
    m_winLoseWindow = NULL;
}

i32 combatManager::DoSurrender(void) {
    i32 armyNum;

    heroWindow* window;
    tag_message message;

    giSurrenderCost = 0;
    for (armyNum = 0; armyNum < COMBAT_ARMY_SLOT_COUNT; ++armyNum) {
        if (m_armies[H2EnumIndex(m_currentSide)][armyNum].IsAlive()) {
            giSurrenderCost += m_armies[H2EnumIndex(m_currentSide)][armyNum].m_quantity
                * gMonsterDatabase[H2EnumIndex(m_armies[H2EnumIndex(m_currentSide)][armyNum].m_monsterType)]
                      .cost;
        }
    }
    if (m_heroes[H2EnumIndex(m_currentSide)]->HasArtifact(ARTIFACT_STATESMANS_QUILL) != 0)
        giSurrenderCost = static_cast<i32>(giSurrenderCost * COMBAT_SURRENDER_QUILL_FACTOR);
    else
        giSurrenderCost = static_cast<i32>(giSurrenderCost * COMBAT_SURRENDER_BASE_FACTOR);
    giSurrenderCost = static_cast<i32>(
        giSurrenderCost
        * (1.0
           - H2EnumIndex(m_heroes[H2EnumIndex(m_currentSide)]->m_secondarySkills[H2EnumIndex(HERO_SKILL_DIPLOMACY)])
                 * COMBAT_SURRENDER_DIPLOMACY_FACTOR)
    );

    window = new heroWindow(SURRENDER_WINDOW_X, SURRENDER_WINDOW_Y, "surrendr.bin");
    if (window == NULL)
        MemError();
    SET_WIDGET_MESSAGE(message, WIDGET_COMMAND_SET_ICON, SURRENDER_PORTRAIT_RESOURCE_ID);
    utf8::Format(
        gText, GLOBAL_TEXT_BUFFER_SIZE,
        "port%04d.icn",
        m_heroes[H2EnumIndex(OppositeCombatSide(m_currentSide))]->m_portrait
    );
    message.payload.widget.data.text = gText;
    window->BroadcastMessage(message);
    message.payload.widget.command =
        m_heroes[H2EnumIndex(OppositeCombatSide(m_currentSide))]->m_isCaptain
        ? WIDGET_COMMAND_SET_FLAGS
        : WIDGET_COMMAND_CLEAR_FLAGS;
    message.payload.widget.id = SURRENDER_PORTRAIT_WIDGET_ID;
    message.payload.widget.data.value = SURRENDER_PORTRAIT_DEFAULT_COLOR;
    window->BroadcastMessage(message);
    if (m_heroes[H2EnumIndex(OppositeCombatSide(m_currentSide))]->m_isCaptain != 0) {
        message.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
        message.payload.widget.data.value =
            m_playerId[H2EnumIndex(OppositeCombatSide(m_currentSide))] == -1
            ? H2EnumIndex(COMBAT_NEUTRAL_HERO_COLOR)
            : gpGame->m_players[m_playerId[H2EnumIndex(OppositeCombatSide(m_currentSide))]].m_color;
        window->BroadcastMessage(message);
    }
    message.payload.widget.data.text = gText;
    message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    message.payload.widget.id = SURRENDER_TEXT_ID;
    utf8::Format(
        gText, GLOBAL_TEXT_BUFFER_SIZE,
        localization::Tr("combat.surrender.offer")
             ,
        m_heroes[H2EnumIndex(OppositeCombatSide(m_currentSide))]->m_name,
        giSurrenderCost
    );
    window->BroadcastMessage(message);
    gpWindowManager->DoDialog(window, TrueFalseDialogHandler, 0);
    delete window;
    return gpWindowManager->m_dialogResult == DIALOG_BUTTON_2;
}

void combatManager::CheckChangeSelector(void) {
    if (gbNoShowCombat != 0)
        return;
    army* currentArmy = &m_armies[H2EnumIndex(m_currentArmySide)][m_currentArmyIndex];
    if (m_limitCreature == 0 || m_limitCreatureHex != currentArmy->m_hex) {
        UpdateGrid(0, 1);
        giNewMonsterCycleFrame = SELECTOR_INITIAL_FRAME;
        m_limitCreatureHex = currentArmy->m_hex;
        m_limitCreature = true;
        DrawFrame(1, 0, 0, 0, COMMAND_FRAME_DELAY, 1, 1);
    }
    SetupSmallView();
}

void combatManager::CheckCastleAttack(void) {
    if (m_inCastleCombat != 0 && m_currentSide == COMBAT_ATTACKER_SIDE) {
        while (m_catapultAttacksRemaining[H2EnumIndex(m_currentSide)] > 0) {
            CatAttack(m_currentSide);
            --m_catapultAttacksRemaining[H2EnumIndex(m_currentSide)];
        }
    }
    if (m_inCastleCombat != 0 && m_currentSide == COMBAT_DEFENDER_SIDE) {
        while (m_keepAttacksRemaining[H2EnumIndex(m_currentSide)] > 0) {
            KeepAttack(COMBAT_TOWER_GARRISON);
            KeepAttack(COMBAT_TOWER_TOP);
            KeepAttack(COMBAT_TOWER_BOTTOM);
            --m_keepAttacksRemaining[H2EnumIndex(m_currentSide)];
        }
    }
}

void combatManager::CheckGetAIMove(void) {
    i32 retreat = AICheckRetreat();
    if (m_heroCastSpell[H2EnumIndex(m_currentSide)] == 0
        && (m_playerId[H2EnumIndex(m_currentSide)] == -1
            || gbThisNetHumanPlayer[m_playerId[H2EnumIndex(m_currentSide)]] == 0
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

void combatManager::GetControl(void) {
    m_selectedHex = INVALID_HEX;
    m_previousCommand = COMBAT_INVALID_COMMAND;
    m_previousCommand = COMBAT_INVALID_COMMAND;
    if (gpCombatManager->m_active == 1)
        gpMouseManager->SetPointer(COMBAT_POINTER_DEFAULT);
    CheckChangeSelector();
    if (gbRemoteOn == 0 || m_playerId[H2EnumIndex(COMBAT_ATTACKER_SIDE)] < 0
        || m_playerId[H2EnumIndex(COMBAT_DEFENDER_SIDE)] < 0
        || gbHumanPlayer[m_playerId[H2EnumIndex(COMBAT_DEFENDER_SIDE)]] == 0
        || (gbHumanPlayer[m_playerId[H2EnumIndex(COMBAT_ATTACKER_SIDE)]] == 0
            && (gbHumanPlayer[m_playerId[H2EnumIndex(COMBAT_ATTACKER_SIDE)]] != 0
                || m_playerId[H2EnumIndex(COMBAT_DEFENDER_SIDE)] == 0))) {
        gbThisNetHasControl = true;
        goto setup_view;
    }
    if (m_playerId[H2EnumIndex(m_currentSide)] != -1 && gbHumanPlayer[m_playerId[H2EnumIndex(m_currentSide)]] != 0
        && gbThisNetHumanPlayer[m_playerId[H2EnumIndex(m_currentSide)]] == 0)
        gbThisNetHasControl = false;
    else
        gbThisNetHasControl = true;

setup_view:
    m_smallViewSide[H2EnumIndex(COMBAT_DEFENDER_SIDE)] = COMBAT_SIDE_NONE;
    SetupSmallView();
    ResetMouse();
}

void combatManager::ResetMouse(void) {
    i32 mouseY;
    i32 mouseX;
    tag_message message;

    if (gbNoShowCombat != 0)
        return;
    if (gbThisNetHasControl != 0 && m_playerId[H2EnumIndex(m_currentSide)] >= 0
        && gbHumanPlayer[m_playerId[H2EnumIndex(m_currentSide)]] != 0) {
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

MessageDispatchResult combatManager::ProcessNextAction(struct tag_message& message) {
    i32 actionData[H2EnumIndex(ACTION_DATA_COUNT)];
    i32 transmitResult;
    army* actingArmy;
    b32 shouldAdvance;
    b32 redraw;
    MessageDispatchResult dispatchResult;

    // A stack that waits on burning ground keeps burning.
    if (giNextAction == ACTION_SKIP_TURN) {
        CheckBurnCreature(&m_armies[H2EnumIndex(m_currentArmySide)][m_currentArmyIndex]);
    }

    ClearCombatMessages(0);
    dispatchResult = MESSAGE_DISPATCH_CONSUME;
    redraw = false;
    gbProcessingCombatAction = true;
    if (m_smallViewSide[H2EnumIndex(COMBAT_ATTACKER_SIDE)] != COMBAT_SIDE_NONE
        || m_smallViewSide[H2EnumIndex(COMBAT_DEFENDER_SIDE)] != COMBAT_SIDE_NONE) {
        m_smallViewSide[H2EnumIndex(COMBAT_DEFENDER_SIDE)] = COMBAT_SIDE_NONE;
        m_smallViewSide[H2EnumIndex(COMBAT_ATTACKER_SIDE)] = COMBAT_SIDE_NONE;
        redraw = true;
    }
    if (giNextAction != ACTION_NONE) {
        LogInt(
            "Process Act",
            H2EnumIndex(giNextAction),
            giNextActionGridIndex,
            giNextActionGridIndex2,
            giNextActionExtra,
            H2EnumIndex(m_currentArmySide),
            m_currentArmyIndex,
            m_armies[H2EnumIndex(m_currentArmySide)][m_currentArmyIndex].m_hex
        );
    }
    gpMouseManager->SetPointer(COMBAT_POINTER_DEFAULT);
    UpdateMouseGrid(-1, 1);
    memset(m_gridState, H2EnumIndex(GRID_SHADE_NONE), sizeof(m_gridState));
    if (UpdateGrid(0, 0) != 0)
        redraw = true;
    if (redraw != 0)
        DrawFrame(1, 0, 0, 0, COMMAND_FRAME_DELAY, 1, 1);

    if (gbThisNetHasControl != 0 && gbRemoteOn != 0 && m_playerId[H2EnumIndex(COMBAT_ATTACKER_SIDE)] >= 0
        && m_playerId[H2EnumIndex(COMBAT_DEFENDER_SIDE)] >= 0
        && gbHumanPlayer[m_playerId[H2EnumIndex(COMBAT_DEFENDER_SIDE)]] != 0
        && gbHumanPlayer[m_playerId[H2EnumIndex(COMBAT_ATTACKER_SIDE)]] != 0) {
        actionData[H2EnumIndex(ACTION_DATA_ACTION)] = H2EnumIndex(giNextAction);
        actionData[H2EnumIndex(ACTION_DATA_EXTRA)] = giNextActionExtra;
        actionData[H2EnumIndex(ACTION_DATA_GRID)] = giNextActionGridIndex;
        actionData[H2EnumIndex(ACTION_DATA_SECOND_GRID)] = giNextActionGridIndex2;
        LogInt("About to T", iCombatControlNetPos[H2EnumIndex(COMBAT_DEFENDER_SIDE) - H2EnumIndex(m_currentSide)]);
        transmitResult = TransmitRemoteData(
            reinterpret_cast<char*>(actionData),
            iCombatControlNetPos[H2EnumIndex(COMBAT_DEFENDER_SIDE) - H2EnumIndex(m_currentSide)],
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

    actingArmy = &m_armies[H2EnumIndex(m_currentArmySide)][m_currentArmyIndex];
    shouldAdvance = false;
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
            if (m_armies[H2EnumIndex(m_currentArmySide)][m_currentArmyIndex].m_quantity <= 0)
                shouldAdvance = true;
            ResetCycleTimers();
            break;
        case ACTION_MOVE:
            ResetCyclingCreatures();
            actingArmy->MoveAttack(giNextActionGridIndex, 0);
            actingArmy->m_monster.attributes |= MONSTER_FLAGS_TURN_SPENT;
            if (CheckWin(&message) != 0) {
                dispatchResult = MESSAGE_DISPATCH_FORWARD;
                goto Finished;
            }
            CheckApplyGoodMorale(m_currentArmySide, m_currentArmyIndex);
            shouldAdvance = true;
            ResetCycleTimers();
            break;
        case ACTION_ATTACK:
            ResetCyclingCreatures();
            if (giNextActionExtra != -1 && actingArmy->m_hex != giNextActionExtra) {
                actingArmy->MoveAttack(giNextActionExtra, 1);
            }
            actingArmy->MoveAttack(giNextActionGridIndex, 0);
            actingArmy->m_monster.attributes |= MONSTER_FLAGS_TURN_SPENT;
            if (CheckWin(&message) != 0) {
                dispatchResult = MESSAGE_DISPATCH_FORWARD;
                goto Finished;
            }
            CheckApplyGoodMorale(m_currentArmySide, m_currentArmyIndex);
            shouldAdvance = true;
            ResetCycleTimers();
            break;
        case ACTION_RETREAT:
            m_sideRetreated[H2EnumIndex(m_currentSide)] = 1;
            gbRetreatWin = true;
            ResetCycleTimers();
            break;
        case ACTION_SURRENDER:
            gbCombatSurrender = true;
            gbRetreatWin = true;
            m_sideDefeated[H2EnumIndex(m_currentSide)] = 1;
            gpGame->m_players[m_playerId[H2EnumIndex(m_currentSide)]].m_resources[H2EnumIndex(RES_GOLD)] -=
                giNextActionExtra;
            gpGame->m_players[m_playerId[H2EnumIndex(COMBAT_DEFENDER_SIDE) - H2EnumIndex(m_currentSide)]]
                .m_resources[H2EnumIndex(RES_GOLD)] += giNextActionExtra;
            ResetCycleTimers();
            break;
        case ACTION_SKIP_TURN:
            actingArmy->m_monster.attributes |= MONSTER_FLAGS_TURN_SPENT;
            shouldAdvance = true;
            break;
        case ACTION_DEFER_TURN:
            actingArmy->m_monster.attributes |= MONSTER_FLAGS_DEFERRED_TURN;
            shouldAdvance = true;
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

void combatManager::ResetCyclingCreatures(void) {
    army* currentTroop = NULL;
    i32 rotateCount = 0;
    i32 index;
    CombatSide sideIndex;

    for (sideIndex = COMBAT_ATTACKER_SIDE; H2EnumIndex(sideIndex) < COMBAT_SIDE_COUNT; ++sideIndex) {
        for (index = 0; index < gpCombatManager->m_armyCount[H2EnumIndex(sideIndex)]; ++index) {
            currentTroop = &gpCombatManager->m_armies[H2EnumIndex(sideIndex)][index];
            if ((H2EnumIndex((currentTroop->m_monster.attributes) & (MONSTER_FLAGS_DEAD))) == 0
                && currentTroop->m_animationSequence >= ARMY_ANIMATION_STANDING_FIRST
                && currentTroop->m_animationSequence <= ARMY_ANIMATION_STANDING_LAST) {
                ++rotateCount;
                ++gpCombatManager->m_limitCreatureCount[H2EnumIndex(sideIndex)][index];
            }
        }
    }
    if (rotateCount == 0)
        return;

    gpCombatManager->DrawFrame(0, 1, 1, 1, COMMAND_FRAME_DELAY, 1, 1);
    for (sideIndex = COMBAT_ATTACKER_SIDE; H2EnumIndex(sideIndex) < COMBAT_SIDE_COUNT; ++sideIndex) {
        for (index = 0; index < gpCombatManager->m_armyCount[H2EnumIndex(sideIndex)]; ++index) {
            currentTroop = &gpCombatManager->m_armies[H2EnumIndex(sideIndex)][index];
            if ((H2EnumIndex((currentTroop->m_monster.attributes) & (MONSTER_FLAGS_DEAD)))
                == 0) {
                currentTroop = &gpCombatManager->m_armies[H2EnumIndex(sideIndex)][index];
                currentTroop->m_animationSequence = ARMY_ANIMATION_STAND;
                currentTroop->m_animationFrame = 0;
                currentTroop->m_lastAnimationTime = platform::Ticks();
            }
        }
    }
    m_heroCycleTimer[H2EnumIndex(COMBAT_ATTACKER_SIDE)] = platform::Ticks();
    m_heroCycleTimer[H2EnumIndex(COMBAT_DEFENDER_SIDE)] = platform::Ticks();
    gpCombatManager->DrawFrame(1, 1, 0, 0, COMMAND_FRAME_DELAY, 1, 1);
}

void combatManager::ResetCycleTimers(void) {
    i32l now = platform::Ticks();
    CombatSide which;
    i32 i;
    army* armyPointer;

    m_heroCycleTimer[H2EnumIndex(COMBAT_ATTACKER_SIDE)] = platform::Ticks();
    m_heroCycleTimer[H2EnumIndex(COMBAT_DEFENDER_SIDE)] = platform::Ticks();
    for (which = COMBAT_ATTACKER_SIDE; H2EnumIndex(which) < COMBAT_SIDE_COUNT; ++which) {
        for (i = 0; i < gpCombatManager->m_armyCount[H2EnumIndex(which)]; ++i) {
            armyPointer = &gpCombatManager->m_armies[H2EnumIndex(which)][i];
            armyPointer->m_lastAnimationTime = now;
            if (armyPointer->m_frameInfo.standStillDelay > STAND_DELAY_RANDOM_THRESHOLD) {
                armyPointer->m_lastAnimationTime -=
                    Random(STAND_DELAY_RANDOM_MIN, armyPointer->m_frameInfo.standStillDelay);
            }
        }
    }
}

i32 InCombatArea(i32 x, i32 y) {
    return x >= 0 && x < LOGICAL_SCREEN_WIDTH && y >= 0 && y < COMBAT_AREA_HEIGHT;
}

void combatManager::CycleCombatScreen(void) {
    i32 nextHeroAnimation[COMBAT_SIDE_COUNT];
    army* currentArmy;
    i32 cyclingCount;
    i32 index;
    i32 animationIndex;
    u8 cycleArmy[COMBAT_SIDE_COUNT][COMBAT_ARMY_SLOT_COUNT];
    CombatSide side;
    float accumulatedChance;
    float roll;

    CheckUpdateCombatMessages();
    currentArmy = NULL;
    cyclingCount = 0;
    gpCombatManager->ResetLimitCreature();
    for (index = H2EnumIndex(COMBAT_ATTACKER_SIDE); index < COMBAT_SIDE_COUNT; ++index) {
        if (m_heroOverlayIcons[index] == NULL)
            continue;

        if (m_heroes[index] != NULL)
            m_heroOverlayFrame[index] =
                (m_heroOverlayFrame[index] + 1) % COMBAT_HERO_OVERLAY_FRAME_COUNT;
        ++m_drawHeroOverlay[index];
    }

    memset(cycleArmy, 0, sizeof(cycleArmy));
    for (side = COMBAT_ATTACKER_SIDE; H2EnumIndex(side) < COMBAT_SIDE_COUNT; ++side) {
        for (index = 0; index < gpCombatManager->m_armyCount[H2EnumIndex(side)]; ++index) {
            currentArmy = gpCombatManager->m_armies[H2EnumIndex(side)] + index;
            if ((H2EnumIndex((currentArmy->m_monster.attributes) & (MONSTER_FLAGS_DEAD))) == 0
                && currentArmy->m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_PARALYZE)] == 0
                && currentArmy->m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_BLIND)] == 0
                && currentArmy->m_spellInfluence[H2EnumIndex(ARMY_SPELL_INFLUENCE_PETRIFIED)] == 0
                && ((currentArmy->m_animationSequence >= ARMY_ANIMATION_STANDING_FIRST
                     && currentArmy->m_animationSequence <= ARMY_ANIMATION_STANDING_LAST)
                    || (currentArmy->m_animationSequence == ARMY_ANIMATION_STAND
                        && currentArmy->m_lastAnimationTime
                                   + currentArmy->m_frameInfo.standStillDelay
                               < platform::Ticks()))) {
                ++cyclingCount;
                ++cycleArmy[H2EnumIndex(side)][index];
                ++m_limitCreatureCount[H2EnumIndex(side)][index];
            }
        }
    }

    for (index = H2EnumIndex(COMBAT_ATTACKER_SIDE); index < COMBAT_SIDE_COUNT; ++index) {
        nextHeroAnimation[index] = -1;
        if (m_heroIcons[index] == NULL)
            continue;

        if (m_heroAnimationState[index] == HERO_ANIMATION_DEATH_FIRST
            || m_heroAnimationState[index] == HERO_ANIMATION_DEATH_SECOND
            || m_heroAnimationState[index] == HERO_ANIMATION_IDLE_FIRST
            || m_heroAnimationState[index] == HERO_ANIMATION_IDLE_SECOND
            || m_heroAnimationState[index] == HERO_ANIMATION_IDLE_LAST) {
            m_drawHero[index] = 1;
        } else if (m_heroAnimationState[index] == HERO_ANIMATION_STAND
                   && m_heroDeathAnimationPlayed[index] == 0
                   && m_heroDeathPending[index] != 0) {
            if (m_playerId[H2EnumIndex(m_currentSide)] == -1
                || gbThisNetHumanPlayer[m_playerId[H2EnumIndex(m_currentSide)]] == 0) {
                m_heroAlternateDeathPending[index] = 0;
                m_heroDeathPending[index] = 0;
            } else {
                m_heroAlternateDeathPending[index] = 0;
                m_heroDeathPending[index] = 0;
                m_heroDeathAnimationPlayed[index] = 1;
                if (sCmbtHero[m_heroSpriteIndex[index]]
                        .animationFrameCount[HERO_ANIMATION_DEATH_FIRST]
                    > 0) {
                    nextHeroAnimation[index] = HERO_ANIMATION_DEATH_FIRST;
                    m_drawHero[index] = 1;
                }
            }
        } else if (m_heroAnimationState[index] == HERO_ANIMATION_STAND
                   && m_heroAlternateDeathAnimationPlayed[index] == 0
                   && m_heroAlternateDeathPending[index] != 0) {
            if (m_playerId[H2EnumIndex(m_currentSide)] == -1
                || gbThisNetHumanPlayer[m_playerId[H2EnumIndex(m_currentSide)]] == 0) {
                m_heroAlternateDeathPending[index] = 0;
                m_heroDeathPending[index] = 0;
            } else {
                m_heroAlternateDeathPending[index] = 0;
                m_heroDeathPending[index] = 0;
                m_heroAlternateDeathAnimationPlayed[index] = 1;
                if (sCmbtHero[m_heroSpriteIndex[index]]
                        .animationFrameCount[HERO_ANIMATION_DEATH_SECOND]
                    > 0) {
                    nextHeroAnimation[index] = HERO_ANIMATION_DEATH_SECOND;
                    m_drawHero[index] = 1;
                }
            }
        } else if (m_heroAnimationState[index] == HERO_ANIMATION_STAND
                   && m_heroCycleTimer[index] + HERO_IDLE_DELAY < platform::Ticks()) {
            if (sCmbtHero[m_heroSpriteIndex[index]].idleAnimationCount > 1) {
                nextHeroAnimation[index] =
                    Random(0, sCmbtHero[m_heroSpriteIndex[index]].idleAnimationCount - 1)
                    + HERO_ANIMATION_IDLE_FIRST;
            } else {
                nextHeroAnimation[index] = HERO_ANIMATION_IDLE_FIRST;
            }
            m_drawHero[index] = 1;
        }    }
    if (m_heroIcons[H2EnumIndex(COMBAT_ATTACKER_SIDE)] != NULL)
        m_drawHero[H2EnumIndex(COMBAT_ATTACKER_SIDE)] = 1;
    if (m_heroIcons[H2EnumIndex(COMBAT_DEFENDER_SIDE)] != NULL)
        m_drawHero[H2EnumIndex(COMBAT_DEFENDER_SIDE)] = 1;

    if (cyclingCount == 0 && m_drawHero[H2EnumIndex(COMBAT_ATTACKER_SIDE)] == 0
        && m_drawHero[H2EnumIndex(COMBAT_DEFENDER_SIDE)] == 0
        && m_drawHeroOverlay[H2EnumIndex(COMBAT_ATTACKER_SIDE)] == 0
        && m_drawHeroOverlay[H2EnumIndex(COMBAT_DEFENDER_SIDE)] == 0)
        goto setCycleTimer;
    gpCombatManager->DrawFrame(0, 1, 1, 1, COMMAND_FRAME_DELAY, 1, 1);
    for (side = COMBAT_ATTACKER_SIDE; H2EnumIndex(side) < COMBAT_SIDE_COUNT; ++side) {
        for (index = 0; index < gpCombatManager->m_armyCount[H2EnumIndex(side)]; ++index) {
            currentArmy = gpCombatManager->m_armies[H2EnumIndex(side)] + index;
            if (cycleArmy[H2EnumIndex(side)][index] != 0) {
                if (currentArmy->m_animationSequence == ARMY_ANIMATION_STAND) {
                    roll = Random(IDLE_ROLL_MIN, IDLE_ROLL_MAX)
                           / COMBAT_IDLE_ROLL_DIVISOR;
                    accumulatedChance = 0.0f;
                    currentArmy->m_standingAnimation =
                        currentArmy->m_frameInfo.standingAnimationCount - 1;
                    for (animationIndex = 0;
                         animationIndex < currentArmy->m_frameInfo.standingAnimationCount - 1;
                         ++animationIndex) {
                        accumulatedChance +=
                            currentArmy->m_frameInfo.standingAnimationChances[animationIndex];
                        if (roll < accumulatedChance) {
                            currentArmy->m_standingAnimation = animationIndex;
                            animationIndex = STANDING_ANIMATION_SEARCH_DONE;
                        }
                    }
                    currentArmy->m_animationSequence = ArmyAnimationSequence(
                        currentArmy->m_standingAnimation
                        + H2EnumIndex(ARMY_ANIMATION_STANDING_FIRST)
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
                                + H2EnumIndex(ARMY_ANIMATION_STANDING_FIRST)]) {
                        currentArmy->m_animationSequence = ARMY_ANIMATION_STAND;
                        currentArmy->m_animationFrame = 0;
                        currentArmy->m_lastAnimationTime = platform::Ticks();
                        if (currentArmy->m_frameInfo.standStillDelay > 0) {
                            currentArmy->m_lastAnimationTime = static_cast<i32>(
                                currentArmy->m_lastAnimationTime
                                + (Random(0, currentArmy->m_frameInfo.standStillDelay)
                                       * COMBAT_STAND_DELAY_RANDOM_FACTOR
                                   - currentArmy->m_frameInfo.standStillDelay
                                         * COMBAT_STAND_DELAY_BASE_FACTOR)
                            );
                        }
                    }
                }
            }
        }
    }
    for (index = H2EnumIndex(COMBAT_ATTACKER_SIDE); index < COMBAT_SIDE_COUNT; ++index) {
        if (m_drawHero[index] != 0) {
            if (nextHeroAnimation[index] != -1) {
                m_heroAnimationState[index] = nextHeroAnimation[index];
                m_heroAnimationFrame[index] = 0;
            } else {
                ++m_heroAnimationFrame[index];
                if (m_heroAnimationFrame[index]
                    >= sCmbtHero[m_heroSpriteIndex[index]]
                           .animationFrameCount[m_heroAnimationState[index]]) {
                    m_heroAnimationState[index] = HERO_ANIMATION_STAND;
                    m_heroAnimationFrame[index] = 0;
                    m_heroCycleTimer[index] = platform::Ticks();
                }
            }
        }
    }
    DrawFrame(1, 1, 0, 0, COMMAND_FRAME_DELAY, 1, 1);
setCycleTimer:
    glTimers[GLOBAL_COMBAT_CYCLE_TIMER_SLOT] = COMBAT_DEADLINE(COMBAT_CYCLE_TIMER_FACTOR);

    // The fire walls flicker along with the screen cycle.
    if (!gIronfistExtra.combat.spell.fireBombWalls.empty()) {
        icon* wallIcon =
            gpResourceManager->GetIcon(gCombatFxNames[H2EnumIndex(COMBAT_EFFECT_FIRE_BOMB)]);
        for (auto& wall : gIronfistExtra.combat.spell.fireBombWalls) {
            wall.currentFrame++;
            if (wall.currentFrame >= wallIcon->m_frameCount) {
                wall.currentFrame = 0;
            }
        }
    }
}

void combatManager::SetCombatViewArmySmallLevel(i32 level) {
    gConfig.combatArmyInfoLevel = level;
    DrawFrame(1, 0, 0, 0, COMMAND_FRAME_DELAY, 1, 1);
    WritePrefs();
}

void combatManager::SetCombatGrid(i32 showGrid, i32 showMouseHex, i32 shadeLevel) {
    if (gConfig.showCombatGrid == showGrid && gConfig.showCombatMouseHex == showMouseHex
        && gConfig.combatShadeLevel == shadeLevel)
        return;

    UpdateMouseGrid(INVALID_HEX, 0);
    gConfig.showCombatGrid = showGrid;
    gConfig.showCombatMouseHex = showMouseHex;
    gConfig.combatShadeLevel = shadeLevel;
    m_backgroundDrawn = false;
    SetupGridForArmy(&m_armies[H2EnumIndex(m_currentArmySide)][m_currentArmyIndex]);
    DrawFrame(1, 0, 0, 0, COMMAND_FRAME_DELAY, 1, 1);
    ResetMouse();
    WritePrefs();
}

void combatManager::AddArmy(
    CombatSide side,
    CreatureType monsterType,
    i32 quantity,
    i32 hex,
    MonsterFlags flags,
    i32 animate
) {
    i32 armyIndex = INVALID_ARMY_INDEX;
    b32 reusedArmy = false;
    i32 index;
    army* newStack;
    for (index = 0; index < COMBAT_ARMY_SLOT_COUNT; ++index) {
        if (m_armies[H2EnumIndex(side)][index].m_monsterType == CREATURE_NONE) {
            armyIndex = index;
            break;
        }
        if (m_armies[H2EnumIndex(side)][index].m_quantity == 0
            && (H2EnumIndex((m_armies[H2EnumIndex(side)][index].m_monster.attributes) & (MONSTER_FLAGS_DEAD))) != 0
            && ((H2EnumIndex((m_armies[H2EnumIndex(side)][index].m_monster.attributes) & (MONSTER_FLAGS_MIRROR_IMAGE))) != 0
                || IS_ELEMENTAL_CREATURE(m_armies[H2EnumIndex(side)][index].m_monsterType))) {
            armyIndex = index;
            reusedArmy = true;
            break;
        }
    }

    if (armyIndex == INVALID_ARMY_INDEX || m_hexCells[hex].m_occupantSide != COMBAT_SIDE_NONE)
        return;

    newStack = &m_armies[H2EnumIndex(side)][armyIndex];
    newStack->Init(monsterType, quantity, side, armyIndex, hex, INVALID_HEX);
    newStack->LoadResources();
    newStack->m_monster.attributes |= flags;
    if (reusedArmy == 0)
        ++m_armyCount[H2EnumIndex(side)];

    if (animate == 0)
        return;

    ResetLimitCreature();
    ++m_limitCreatureCount[H2EnumIndex(side)][armyIndex];
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

void combatManager::SetupSmallView(void) {

    if (m_smallViewSide[H2EnumIndex(COMBAT_DEFENDER_SIDE)] == COMBAT_SIDE_NONE
        && m_smallViewLastX[H2EnumIndex(COMBAT_DEFENDER_SIDE)] != SMALL_VIEW_POSITION_NONE) {
        DrawSmallView(H2EnumIndex(COMBAT_DEFENDER_SIDE), 1);
    }

    if (gbThisNetHasControl != 0 && m_playerId[H2EnumIndex(m_currentSide)] != PLAYER_NONE
        && gbHumanPlayer[m_playerId[H2EnumIndex(m_currentSide)]] != 0) {
        if (m_smallViewSide[H2EnumIndex(COMBAT_ATTACKER_SIDE)] != m_currentSide
            || m_smallViewArmyIndex[H2EnumIndex(COMBAT_ATTACKER_SIDE)] != m_currentArmyIndex) {
            if (m_smallViewSide[H2EnumIndex(COMBAT_ATTACKER_SIDE)] != COMBAT_SIDE_NONE
                && m_smallViewSide[H2EnumIndex(COMBAT_ATTACKER_SIDE)] != m_currentSide
                && m_smallViewLastX[H2EnumIndex(COMBAT_ATTACKER_SIDE)] != SMALL_VIEW_POSITION_NONE) {
                m_smallViewSide[H2EnumIndex(COMBAT_ATTACKER_SIDE)] = COMBAT_SIDE_NONE;
                DrawSmallView(H2EnumIndex(COMBAT_ATTACKER_SIDE), 1);
            }
            m_smallViewSide[H2EnumIndex(COMBAT_ATTACKER_SIDE)] = m_currentSide;
            m_smallViewArmyIndex[H2EnumIndex(COMBAT_ATTACKER_SIDE)] = m_currentArmyIndex;
            DrawSmallView(H2EnumIndex(COMBAT_ATTACKER_SIDE), 1);
        }
    } else {
        m_smallViewSide[H2EnumIndex(COMBAT_ATTACKER_SIDE)] = COMBAT_SIDE_NONE;
        if (m_smallViewLastX[H2EnumIndex(COMBAT_ATTACKER_SIDE)] != SMALL_VIEW_POSITION_NONE)
            DrawSmallView(H2EnumIndex(COMBAT_ATTACKER_SIDE), 1);
    }
}

void combatManager::ViewBallista(i32 quickView) {
    i32 shooterCount;
    i32 attackSkill;
    char description[VIEW_DESCRIPTION_SIZE];

    m_combatTowns[H2EnumIndex(COMBAT_DEFENDER_SIDE)]->CalcNumLevelArchers(&shooterCount, &attackSkill);
    utf8::Format(gText, GLOBAL_TEXT_BUFFER_SIZE, "%s", localization::Tr("combat.siege.ballista.title"));
    strcpy(description, "");
    if (m_wallStates[H2EnumIndex(COMBAT_WALL_SLOT_KEEP)] != COMBAT_WALL_STATE_KEEP_STANDING) {
        utf8::Format(description, "%s", localization::Tr("combat.siege.ballista.destroyed"));
    } else if (attackSkill > 0) {
        utf8::Format(
            description,
            localization::TrPlural("combat.siege.ballista.fires_with_bonus", shooterCount),
            shooterCount,
            attackSkill
        );
    } else {
        utf8::Format(
            description,
            localization::TrPlural("combat.siege.ballista.fires", shooterCount),
            shooterCount
        );
    }
    strcat(gText, description);

    strcpy(description, "");
    if ((m_combatTowns[H2EnumIndex(COMBAT_DEFENDER_SIDE)]->m_buildings & H2EnumIndex(TOWN_BUILDING_LEFT_TURRET))
        != 0) {
        if (m_wallStates[H2EnumIndex(COMBAT_WALL_SLOT_TOP_TOWER)] == COMBAT_WALL_STATE_DESTROYED) {
            utf8::Format(description, "%s", localization::Tr("combat.siege.left_turret.destroyed"));
        } else if (m_wallStates[H2EnumIndex(COMBAT_WALL_SLOT_TOP_TOWER)]
                   == COMBAT_WALL_STATE_TOWER_STANDING) {
            if (attackSkill > 0) {
                utf8::Format(
                    description,
                    localization::TrPlural(
                        "combat.siege.left_turret.fires_with_bonus",
                        shooterCount / COMBAT_KEEP_SIDE_TOWER_SHOT_DIVISOR
                    ),
                    shooterCount / COMBAT_KEEP_SIDE_TOWER_SHOT_DIVISOR,
                    attackSkill
                );
            } else {
                utf8::Format(
                    description,
                    localization::TrPlural(
                        "combat.siege.left_turret.fires",
                        shooterCount / COMBAT_KEEP_SIDE_TOWER_SHOT_DIVISOR
                    ),
                    shooterCount / COMBAT_KEEP_SIDE_TOWER_SHOT_DIVISOR
                );
            }
        }
        strcat(gText, description);
    }

    if ((m_combatTowns[H2EnumIndex(COMBAT_DEFENDER_SIDE)]->m_buildings & H2EnumIndex(TOWN_BUILDING_RIGHT_TURRET))
        != 0) {
        strcpy(description, "");
        if (m_wallStates[H2EnumIndex(COMBAT_WALL_SLOT_BOTTOM_TOWER)] == COMBAT_WALL_STATE_DESTROYED) {
            utf8::Format(description, "%s", localization::Tr("combat.siege.right_turret.destroyed"));
        } else if (m_wallStates[H2EnumIndex(COMBAT_WALL_SLOT_BOTTOM_TOWER)]
                   == COMBAT_WALL_STATE_TOWER_STANDING) {
            if (attackSkill > 0) {
                utf8::Format(
                    description,
                    localization::TrPlural(
                        "combat.siege.right_turret.fires_with_bonus",
                        shooterCount / COMBAT_KEEP_SIDE_TOWER_SHOT_DIVISOR
                    ),
                    shooterCount / COMBAT_KEEP_SIDE_TOWER_SHOT_DIVISOR,
                    attackSkill
                );
            } else {
                utf8::Format(
                    description,
                    localization::TrPlural(
                        "combat.siege.right_turret.fires",
                        shooterCount / COMBAT_KEEP_SIDE_TOWER_SHOT_DIVISOR
                    ),
                    shooterCount / COMBAT_KEEP_SIDE_TOWER_SHOT_DIVISOR
                );
            }
        }
        strcat(gText, description);
    }

    NormalDialog(gText, quickView == 0 ? NORMAL_DIALOG_INFO : NORMAL_DIALOG_QUICK_VIEW, NORMAL_DIALOG_NO_RESOURCE, NORMAL_DIALOG_NO_VALUE);
}

b32 gbThisNetHasControl;
i32 iCurTransferArtifact;
i8 iTransferArtifactsInfo[COMBAT_TRANSFER_ARTIFACT_COUNT];
H2EnumStorage<CombatWinLoseAnimation, i32> gbWhichAnimationPlaying;
i32 iMaxTransferArtifacts;
i32 giNextActionExtra;
b32 bSkeletonsShown;
b32 gbShowingLoseWindow;
i32 giWinCmbtFrame;
i32 giNextActionGridIndex;
i32 giSurrenderCost;
i32 giSkeletonsCreated;
H2EnumStorage<ArtifactType, i8> iTransferArtifacts[COMBAT_TRANSFER_ARTIFACT_COUNT];
H2EnumStorage<CombatAction, i32> giNextAction;
i32 giNextActionGridIndex2;
