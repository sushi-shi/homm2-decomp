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

H2_ENUM_CLASS_BEGIN(CombatMainResult)
    MAIN_CONTINUE = 1,
    MAIN_FINISHED = 2
H2_ENUM_CLASS_END(CombatMainResult)

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

H2_ENUM_CLASS_BEGIN(CombatWindowCommand)
    WINDOW_HOVER       = 12,
    WINDOW_CLICK       = 13,
    WINDOW_HELP        = 14,
    WINDOW_MAIN_BUTTON = 0x40,
    WINDOW_HELP_FLAG   = 0x200
H2_ENUM_CLASS_END(CombatWindowCommand)

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
    WIN_LOSE_ICON_FLAGS                 = 16,
    WIN_LOSE_SPELL_ICON_FLAGS           = 17,
    WIN_LOSE_TEXT_FLAGS                 = 512
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
    CONTROL_HELP_FOURTH       = 13
H2_ENUM_CLASS_END(CombatControlId)

H2_ENUM_BEGIN(CombatRemoteConstant)
    REMOTE_PACKET_TYPE     = 1,
    REMOTE_CATEGORY_ACTION = 2,
    REMOTE_COMMAND_MESSAGE = 11,
    REMOTE_COMMAND_ACTION  = 23
H2_ENUM_END(CombatRemoteConstant)

H2_ENUM_CLASS_BEGIN(CombatAction)
    ACTION_NONE       = 0,
    ACTION_CAST_SPELL = 1,
    ACTION_MOVE       = 2,
    ACTION_WAIT       = 3,
    ACTION_RETREAT    = 4,
    ACTION_SURRENDER  = 5,
    ACTION_ATTACK     = 6,
    ACTION_DEFEND     = 7
H2_ENUM_CLASS_END(CombatAction)

H2_ENUM_CLASS_BEGIN(CombatActionDataIndex)
    ACTION_DATA_ACTION      = 0,
    ACTION_DATA_EXTRA       = 1,
    ACTION_DATA_GRID        = 2,
    ACTION_DATA_SECOND_GRID = 3,
    ACTION_DATA_COUNT       = 4
H2_ENUM_CLASS_END(CombatActionDataIndex)

H2_ENUM_BEGIN(CombatDirectionConstant)
    DIRECTION_COUNT                          = 8,
    ADJACENT_DIRECTION_COUNT                 = 6,
    DIRECTION_SPECIAL_FIRST                  = 6,
    DIRECTION_SPECIAL_SECOND                 = 7,
    DIRECTION_SECTOR_COUNT                   = 24,
    DIRECTION_SECTOR_QUARTER                 = 6,
    DIRECTION_SECTOR_HALF                    = 12,
    DIRECTION_SECTOR_THREE_QUARTERS          = 18,
    DIRECTION_PENDING_OFFSET                 = 10,
    DIRECTION_SECTORS_PER_ADJACENT           = 4,
    DIRECTION_SPECIAL_FIRST_SECTOR_START     = 11,
    DIRECTION_SPECIAL_FIRST_SECTOR_CENTER    = 12,
    DIRECTION_SPECIAL_FIRST_SECTOR_END       = 13,
    DIRECTION_SPECIAL_SECOND_SECTOR_PREVIOUS = 23,
    DIRECTION_SPECIAL_SECOND_SECTOR_CENTER   = 0,
    DIRECTION_SPECIAL_SECOND_SECTOR_NEXT     = 1
H2_ENUM_END(CombatDirectionConstant)

H2_ENUM_BEGIN(CombatCommandConstant)
    POINTER_DEFAULT          = 6,
    POINTER_VIEW             = 5,
    POINTER_ATTACK_OFFSET    = 7,
    INVALID_ARMY_INDEX       = -1,
    INVALID_HEX              = -1,
    PLAYER_NONE              = -1,
    SMALL_VIEW_SIDE_NONE     = -1,
    SMALL_VIEW_POSITION_NONE = -1,
    IGNORED_HEX              = -2,
    CASTLE_GATE_HEX          = 59
H2_ENUM_END(CombatCommandConstant)

H2_ENUM_BEGIN(CombatRoundConstant)
    ROUND_INITIAL_SPEED = 15,
    ROUND_ABILITY_FLAGS = 0xff1f,
    RESULT_DRAW         = -1
H2_ENUM_END(CombatRoundConstant)

H2_ENUM_BEGIN(CombatCommandGeometry)
    CONTROL_RIGHT_MIN_X          = 590,
    CONTROL_LEFT_MAX_X           = 50,
    CONTROL_SYSTEM_OPTIONS_MIN_Y = 460,
    SCREEN_HEIGHT                = 480,
    SCREEN_MAX_X                 = 639,
    SCREEN_MAX_Y                 = 479,
    ARMY_VIEW_LEVEL_COUNT        = 3,
    SPELL_EFFECT_TYPE_COUNT      = 3,
    COMMAND_FRAME_DELAY          = 75,
    DEBUG_VAPORIZE_MASK          = 0x3,
    DEBUG_DOUBLE_RIPPLE_MASK     = 0xc
H2_ENUM_END(CombatCommandGeometry)

H2_ENUM_BEGIN(CombatCasualtyConstant)
    CASUALTY_QUANTITY_STORAGE_COUNT = 42,
    CASUALTY_DISPLAY_LIMIT          = 7,
    CASUALTY_WIDGETS_PER_SIDE       = 7,
    CASUALTY_WINDOW_WIDTH           = 320,
    CASUALTY_WINDOW_BOTTOM          = 458
H2_ENUM_END(CombatCasualtyConstant)

H2_ENUM_BEGIN(CombatCycleConstant)
    HERO_ANIMATION_STAND         = 0,
    HERO_ANIMATION_DEATH_FIRST   = 1,
    HERO_ANIMATION_DEATH_SECOND  = 2,
    HERO_ANIMATION_IDLE_FIRST    = 9,
    HERO_ANIMATION_IDLE_SECOND   = 10,
    HERO_ANIMATION_IDLE_LAST     = 11,
    HERO_IDLE_DELAY              = 4500,
    STAND_DELAY_RANDOM_THRESHOLD = 51,
    STAND_DELAY_RANDOM_MIN       = 50,
    IDLE_ROLL_MIN                = 1,
    IDLE_ROLL_MAX                = 100,
    IDLE_REPEAT_CHANCE           = 8
H2_ENUM_END(CombatCycleConstant)

H2_ENUM_BEGIN(CombatVictoryConstant)
    VICTORY_NECROMANCY_STACK_LIMIT = 5,
    VICTORY_FADE_STEPS             = 8,
    VICTORY_WATER_FADE_STEPS       = 5,
    VICTORY_WATER_TERRAIN          = 7,
    VICTORY_FADE_DELAY             = 60,
    VICTORY_MUSIC                  = 29,
    LOSS_MUSIC                     = 30
H2_ENUM_END(CombatVictoryConstant)

H2_ENUM_BEGIN(CombatSurrenderDialogConstant)
    SURRENDER_DIALOG_TYPE            = 1,
    SURRENDER_DIALOG_ACCEPT_RESULT   = 2,
    SURRENDER_TEXT_WIDTH             = 30,
    SURRENDER_PORTRAIT_RESOURCE_ID   = 1,
    SURRENDER_PORTRAIT_WIDGET_ID     = 30,
    SURRENDER_PORTRAIT_DEFAULT_COLOR = 4,
    SURRENDER_CAPTAIN_NEUTRAL_COLOR  = 6,
    SURRENDER_TEXT_ID                = 2
H2_ENUM_END(CombatSurrenderDialogConstant)

H2_ENUM_CLASS_BEGIN(CombatHelpTextIndex)
    HELP_SKIP_UNIT      = 1,
    HELP_AUTO_COMBAT    = 2,
    HELP_SYSTEM_OPTIONS = 3,
    HELP_OTHER_CONTROL  = 4
H2_ENUM_CLASS_END(CombatHelpTextIndex)

H2_ENUM_BEGIN(HeroArtifactConstant)
    ARTIFACT_TRANSFERABLE_FIRST = IDX(ARTIFACT_ARCANE_NECKLACE)
H2_ENUM_END(HeroArtifactConstant)

#define RETAIL_FILE "I:\\Projects\\Heroes\\Prog\\SOURCE\\COMMAND.CPP"
VA(0x0042a6d0, 0x36d)
i32 combatManager::Main(tag_message& message) {
    i32 result = IDX(MAIN_CONTINUE);

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
        return IDX(MAIN_FINISHED);

    if (gbNoShowCombat == 0) {
        CombatRemotePacket* packet =
            reinterpret_cast<CombatRemotePacket*>(GetRemoteData(REMOTE_PACKET_TYPE));
        if (packet != NULL && packet->category == REMOTE_CATEGORY_ACTION) {
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
                switch (message.payload.keyboard.keyCode) {
                    case IDX(KEY_CLOSE_NETWORK_BOX):
                        PopNetBox(NULL, -1);
                        break;
                }
            }
            return IDX(MAIN_CONTINUE);
        }
    }

    {
        army* currentArmy = &m_armies[OD_STEER(m_currentArmySide)][m_currentArmyIndex];
        if (currentArmy->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_BERSERK)] != 0) {
            currentArmy->GoBerserk();
            if (CheckWin(&message) != 0)
                return IDX(MAIN_FINISHED);
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
    if (giNextAction == 0) {
        if (m_playerId[m_currentSide] == -1 || gbThisNetHumanPlayer[m_playerId[m_currentSide]] == 0
            || m_gridSelectionDisabled != 0) {
            CheckGetAIMove();
        } else {
            result = ProcessCombatMsg(message);
        }
    }
    if (giNextAction != 0)
        result = ProcessNextAction(message);
    return result;
}

// @semantic: first normalized residual evaluates the row modulo before MAP_WIDTH - 1.
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
                                 != -1
                          || gpCombatManager->m_hexCells[COMBAT_CASTLE_GATE_APPROACH_HEX]
                                     .m_deadOccupantCount
                                 != 0))))
          || (m_hexCells[hexIndex].m_occupantSide != -1
              && (m_hexCells[hexIndex].m_occupantSide != m_currentArmySide
                  || m_hexCells[hexIndex].m_occupantIndex != m_currentArmyIndex)))) {
        return 1;
    } else {
        return 0;
    }
}

// @semantic: first residual is army-side/index load order at +0x26.
VA(0x0042abbe, 0x8a1)
void combatManager::SetCombatDirections(i32 targetHex) {
    if (m_gridSelectionDisabled != 0)
        return;

    char standable_3[DIRECTION_COUNT];
    i32 directionHexes_4[DIRECTION_COUNT];
    i32 rearHexes_15[DIRECTION_COUNT];
    char pathValid_1[DIRECTION_COUNT];
    army* currentArmy_3 = &m_armies[m_currentArmySide][m_currentArmyIndex];
    i32 targetSide_36 = currentArmy_3->m_targetSide;
    i32 targetIndex_6 = currentArmy_3->m_targetIndex;
    currentArmy_3->m_targetSide = -1;
    currentArmy_3->m_targetIndex = -1;
    army* targetArmy_1 = &m_armies[targetSide_36][targetIndex_6];

    i32 direction_27;
    for (direction_27 = 0; direction_27 < DIRECTION_COUNT; direction_27++) {
        if (direction_27 == DIRECTION_SPECIAL_FIRST
            || direction_27 == DIRECTION_SPECIAL_SECOND) {
            if (HAS(currentArmy_3->m_monster.flags.all, MONSTER_FLAGS_WIDE) != 0) {
                if (currentArmy_3->m_facing == 1) {
                    if (direction_27 == DIRECTION_SPECIAL_FIRST)
                        directionHexes_4[direction_27] = m_adjacency[targetHex][5];
                    if (direction_27 == DIRECTION_SPECIAL_SECOND)
                        directionHexes_4[direction_27] = m_adjacency[targetHex][3];
                } else {
                    if (direction_27 == DIRECTION_SPECIAL_FIRST)
                        directionHexes_4[direction_27] = m_adjacency[targetHex][0];
                    if (direction_27 == DIRECTION_SPECIAL_SECOND)
                        directionHexes_4[direction_27] = m_adjacency[targetHex][2];
                }
            } else {
                directionHexes_4[direction_27] = INVALID_HEX;
            }
        } else {
            directionHexes_4[direction_27] =
                *(&m_adjacency[0][0] + targetHex * ADJACENT_DIRECTION_COUNT + direction_27);
        }

        if (HAS(currentArmy_3->m_monster.flags.all, MONSTER_FLAGS_WIDE) != 0
            && directionHexes_4[direction_27] != INVALID_HEX) {
            if (currentArmy_3->m_facing == 1) {
                if (direction_27 == 5 || direction_27 == 4 || direction_27 == 3) {
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
                if (direction_27 == 0 || direction_27 == 1 || direction_27 == 2) {
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
        for (direction_27 = 0; direction_27 < DIRECTION_COUNT; direction_27++)
            pathValid_1[direction_27] = standable_3[direction_27];
    } else {
        for (direction_27 = 0; direction_27 < DIRECTION_COUNT; direction_27++) {
            if (standable_3[direction_27] != 0) {
                if (directionHexes_4[direction_27] == currentArmy_3->m_hex
                    || currentArmy_3->ValidPath(directionHexes_4[direction_27], 1) != 0)
                    pathValid_1[direction_27] = 1;
                else
                    pathValid_1[direction_27] = 0;
            } else {
                pathValid_1[direction_27] = 0;
            }
        }
    }

    m_validDirectionCount = 0;
    for (direction_27 = 0; direction_27 < DIRECTION_COUNT; direction_27++) {
        if (pathValid_1[direction_27] != 0)
            m_validDirectionCount++;
    }
    if (m_validDirectionCount == 0)
        pathValid_1[DIRECTION_SPECIAL_FIRST] = 1;

    memset(m_directionMap, -1, sizeof(m_directionMap));
    for (direction_27 = 0; direction_27 < DIRECTION_COUNT; direction_27++) {
        i32 outputDirection_11 = direction_27;
        i32 mappedDirection_7;
        if (direction_27 < ADJACENT_DIRECTION_COUNT)
            mappedDirection_7 = (direction_27 + 3) % ADJACENT_DIRECTION_COUNT;
        else if (direction_27 == DIRECTION_SPECIAL_FIRST)
            mappedDirection_7 = DIRECTION_SPECIAL_SECOND;
        else
            mappedDirection_7 = DIRECTION_SPECIAL_FIRST;

        if (pathValid_1[mappedDirection_7] != 0) {
            if (HAS(targetArmy_1->m_monster.flags.all, MONSTER_FLAGS_WIDE) != 0) {
                if (direction_27 == 0 && m_hexCells[targetHex - 1].m_occupantSide == targetSide_36
                    && m_hexCells[targetHex - 1].m_occupantIndex == targetIndex_6) {
                    outputDirection_11 = DIRECTION_SPECIAL_FIRST;
                } else if (direction_27 == 5
                           && m_hexCells[targetHex + 1].m_occupantSide == targetSide_36
                           && m_hexCells[targetHex + 1].m_occupantIndex == targetIndex_6) {
                    outputDirection_11 = DIRECTION_SPECIAL_FIRST;
                } else if (direction_27 == 2
                           && m_hexCells[targetHex - 1].m_occupantSide == targetSide_36
                           && m_hexCells[targetHex - 1].m_occupantIndex == targetIndex_6) {
                    outputDirection_11 = DIRECTION_SPECIAL_SECOND;
                } else if (direction_27 == 3
                           && m_hexCells[targetHex + 1].m_occupantSide == targetSide_36
                           && m_hexCells[targetHex + 1].m_occupantIndex == targetIndex_6) {
                    outputDirection_11 = DIRECTION_SPECIAL_SECOND;
                }
            }

            if (direction_27 < ADJACENT_DIRECTION_COUNT) {
                memset(
                    &m_directionMap[mappedDirection_7 * DIRECTION_SECTORS_PER_ADJACENT],
                    outputDirection_11,
                    DIRECTION_SECTORS_PER_ADJACENT
                );
            } else if (direction_27 == DIRECTION_SPECIAL_FIRST) {
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
                    && m_directionMap[next_3] <= DIRECTION_SPECIAL_SECOND) {
                    m_directionMap[direction_27] =
                        m_directionMap[next_3] + DIRECTION_PENDING_OFFSET;
                } else if (m_directionMap[previous_1] >= 0
                           && m_directionMap[previous_1] <= DIRECTION_SPECIAL_SECOND) {
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

// @semantic: compiler-shape residual.
VA(0x0042b45f, 0x63c)
void combatManager::CheckSetMouseDirection(i32 mouseX, i32 mouseY, i32 targetHex) {
    if (m_gridSelectionDisabled != 0)
        return;
    if (m_validDirectionCount <= 1 && m_mouseDirection >= 0)
        return;

    i32 relativeX = mouseX - (targetHex % COMBAT_GRID_ROW_LENGTH - 1) * COMBAT_HEX_HORIZONTAL_STEP;
    relativeX -= COMBAT_HEX_GRID_LEFT_ORIGIN;
    if (((targetHex / COMBAT_GRID_ROW_LENGTH) & 1) == 0)
        relativeX -= COMBAT_HEX_ROW_STAGGER;
    i32 relativeY = mouseY - COMBAT_HEX_CENTER_Y_ORIGIN
                    - targetHex / COMBAT_GRID_ROW_LENGTH * COMBAT_HEX_VERTICAL_STEP;
    relativeY -= 5;
    relativeX -= COMBAT_HEX_ROW_STAGGER;
    relativeY -= 21;

    i32 sector = 0;
    if (relativeX < 0) {
        if (relativeY < 0)
            sector += DIRECTION_SECTOR_THREE_QUARTERS;
        else
            sector += DIRECTION_SECTOR_HALF;
    } else {
        if (relativeY < 0) {
        } else {
            sector += DIRECTION_SECTOR_QUARTER;
        }
    }

    relativeX = abs(relativeX);
    relativeY = abs(relativeY);
    float ratio = static_cast<float>(relativeX) / relativeY;
    if (sector == 0 || sector == DIRECTION_SECTOR_HALF) {
        if (ratio > COMBAT_DIRECTION_SLOPE_STEEPEST)
            sector += 5;
        else if (ratio > COMBAT_DIRECTION_SLOPE_STEEP)
            sector += 4;
        else if (ratio > COMBAT_DIRECTION_SLOPE_DIAGONAL)
            sector += 3;
        else if (ratio > COMBAT_DIRECTION_SLOPE_SHALLOW)
            sector += 2;
        else if (ratio > COMBAT_DIRECTION_SLOPE_SHALLOWEST)
            sector++;
    } else {
        if (ratio < COMBAT_DIRECTION_SLOPE_SHALLOWEST)
            sector += 5;
        else if (ratio < COMBAT_DIRECTION_SLOPE_SHALLOW)
            sector += 4;
        else if (ratio < COMBAT_DIRECTION_SLOPE_DIAGONAL)
            sector += 3;
        else if (ratio < COMBAT_DIRECTION_SLOPE_STEEP)
            sector += 2;
        else if (ratio < COMBAT_DIRECTION_SLOPE_STEEPEST)
            sector++;
    }

    if (m_directionMap[sector] == m_mouseDirection)
        return;

    m_mouseDirection = m_directionMap[sector];
    i32 directionResult = OppositeDirection(m_directionMap[sector]);
    i32 direction = directionResult;
    i32 alternateDirection = -1;
    army* currentArmy = &m_armies[m_currentArmySide][m_currentArmyIndex];
    army* targetArmy = &m_armies[currentArmy->m_targetSide][currentArmy->m_targetIndex];

    if (direction == DIRECTION_SPECIAL_FIRST
        || direction == DIRECTION_SPECIAL_SECOND) {
        if (HAS(currentArmy->m_monster.flags.all, MONSTER_FLAGS_WIDE) != 0) {
            if (currentArmy->m_facing == 1 && direction == DIRECTION_SPECIAL_FIRST) {
                direction = 5;
                alternateDirection = 0;
            } else if (currentArmy->m_facing == 1 && direction == DIRECTION_SPECIAL_SECOND) {
                direction = 3;
                alternateDirection = 2;
            } else if (currentArmy->m_facing == 0 && direction == DIRECTION_SPECIAL_FIRST) {
                direction = 0;
                alternateDirection = 5;
            } else {
                direction = 2;
                alternateDirection = 3;
            }
        } else {
            if (m_hexCells[targetHex - 1].m_occupantSide == currentArmy->m_targetSide
                && m_hexCells[targetHex - 1].m_occupantIndex == currentArmy->m_targetIndex) {
                targetHex--;
            }
            if (direction == DIRECTION_SPECIAL_FIRST)
                direction = 0;
            else
                direction = 2;
        }
    } else {
        if (currentArmy->m_facing == 1
            && HAS(currentArmy->m_monster.flags.all, MONSTER_FLAGS_WIDE) != 0) {
            if (direction == 5 || direction == 4 || direction == 3)
                targetHex--;
        } else if (currentArmy->m_facing == 0
                   && HAS(currentArmy->m_monster.flags.all, MONSTER_FLAGS_WIDE) != 0
                   && (direction == 0 || direction == 1 || direction == 2)) {
            targetHex++;
        }
    }

    m_directionTargetHex = m_adjacency[targetHex][direction];
    i32 rearHex = IGNORED_HEX;
    if (currentArmy->m_facing == 0
        && HAS(currentArmy->m_monster.flags.all, MONSTER_FLAGS_WIDE) != 0) {
        rearHex = m_directionTargetHex - 1;
    }
    if (currentArmy->m_facing == 1
        && HAS(currentArmy->m_monster.flags.all, MONSTER_FLAGS_WIDE) != 0) {
        rearHex = m_directionTargetHex + 1;
    }
    if (ValidHexToStandOn(m_directionTargetHex) == 0 || ValidHexToStandOn(rearHex) == 0) {
        if (HAS(currentArmy->m_monster.flags.all, MONSTER_FLAGS_WIDE) != 0
            && (direction == DIRECTION_SPECIAL_FIRST
                || direction == DIRECTION_SPECIAL_SECOND)) {
            if (currentArmy->m_facing == 1)
                m_directionTargetHex++;
            else
                m_directionTargetHex--;
        } else {
            if (alternateDirection != -1)
                m_directionTargetHex = m_adjacency[targetHex][alternateDirection];
        }
    }
    gpMouseManager->SetPointer(m_mouseDirection + POINTER_ATTACK_OFFSET);
}

VA(0x0042ba9b, 0x8b)
i32 combatManager::GetPointer(CombatMessageCommand command, i32 hexIndex) {
    CombatMainResult result;
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

// @semantic: ten-byte switch-continuation residual.
VA(0x0042bb26, 0x8e4)
i32 combatManager::ProcessCombatMsg(tag_message& message) {
    i32 mouseX = message.payload.mouse.screenX;
    i32 mouseY = message.payload.mouse.screenY;
    i32 unusedResult_14 = 0;
    i32 selectedHex_36;
    tag_message pendingMessage;

    switch (message.type) {
        case MESSAGE_WIDGET:
            if ((message.payload.widget.parameter & IDX(WINDOW_HELP_FLAG)) != 0) {
                if (message.payload.widget.command == IDX(WINDOW_HOVER)
                    || message.payload.widget.command == IDX(WINDOW_HELP)) {
                    i32 helpIndex = -1;
                    switch (message.payload.widget.id) {
                        case IDX(CONTROL_ATTACK):
                            helpIndex = 0;
                            break;
                        case IDX(CONTROL_WAIT):
                            helpIndex = 1;
                            break;
                        case IDX(CONTROL_DISABLE_SELECTION):
                            helpIndex = 2;
                            break;
                        case IDX(CONTROL_SYSTEM_OPTIONS):
                            helpIndex = 3;
                            break;
                        case IDX(CONTROL_HELP_FIRST):
                        case IDX(CONTROL_HELP_SECOND):
                        case IDX(CONTROL_HELP_THIRD):
                        case IDX(CONTROL_HELP_FOURTH):
                            helpIndex = 4;
                            break;
                        case IDX(WINDOW_MAIN_BUTTON):
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
                    case IDX(WINDOW_HOVER):
                        switch (message.payload.widget.id) {
                            case IDX(WINDOW_MAIN_BUTTON):
                                DoCommand(IDX(m_currentCommand));
                                break;
                        }
                        break;
                    case IDX(WINDOW_CLICK):
                        switch (message.payload.widget.id) {
                            case IDX(CONTROL_ATTACK):
                                giNextAction = IDX(COMBAT_MESSAGE_COMMAND_ATTACK);
                                break;
                            case IDX(CONTROL_WAIT):
                                giNextAction = COMBAT_AI_ACTION_WAIT;
                                break;
                            case IDX(CONTROL_DISABLE_SELECTION):
                                m_gridSelectionDisabled = 1;
                                break;
                            case IDX(CONTROL_SYSTEM_OPTIONS):
                                CombatSystemOptions();
                                break;
                        }
                        break;
                }
            }
            break;

        case MESSAGE_MOUSE_MOVE:
            if (m_gridSelectionDisabled == 0) {
                pendingMessage = gpInputManager->PeekEvent();
                if (pendingMessage.type != MESSAGE_MOUSE_MOVE) {
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
                            CombatMessage(IDX(m_currentCommand));
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
                        gpMouseManager->SetPointer(POINTER_DEFAULT);
                        m_selectedHex = INVALID_HEX;
                        m_previousCommand = COMBAT_INVALID_COMMAND;
                    }
                }
            }
            return IDX(MAIN_CONTINUE);

        case MESSAGE_KEY_DOWN:
            switch (message.payload.keyboard.keyCode) {
                case IDX(KEY_CLOSE_NETWORK_BOX):
                    PopNetBox(NULL, -1);
                    break;
                case IDX(KEY_REDRAW_SCREEN):
                    gpWindowManager->UpdateScreenRegion(
                        0,
                        0,
                        COMBAT_SCREEN_WIDTH - 1,
                        SCREEN_HEIGHT - 1
                    );
                    break;
                case IDX(KEY_CYCLE_ARMY_VIEW):
                    SetCombatViewArmySmallLevel(
                        (gConfig.combatArmyInfoLevel + 1) % ARMY_VIEW_LEVEL_COUNT
                    );
                    break;
                case IDX(KEY_TOGGLE_GRID):
                    SetCombatGrid(
                        1 - gConfig.showCombatGrid,
                        gConfig.showCombatMouseHex,
                        gConfig.combatShadeLevel
                    );
                    break;
                case IDX(KEY_TOGGLE_MOUSE_HEX):
                    SetCombatGrid(
                        gConfig.showCombatGrid,
                        1 - gConfig.showCombatMouseHex,
                        gConfig.combatShadeLevel
                    );
                    break;
                case IDX(KEY_CYCLE_SHADE):
                    SetCombatGrid(
                        gConfig.showCombatGrid,
                        gConfig.showCombatMouseHex,
                        1 - gConfig.combatShadeLevel
                    );
                    break;
                case IDX(KEY_CYCLE_SPELL_EFFECT):
                    giSpellEffectShowType =
                        (giSpellEffectShowType + 1) % SPELL_EFFECT_TYPE_COUNT;
                    DrawFrame(1, 0, 0, 0, COMMAND_FRAME_DELAY, 1, 1);
                    break;
                case IDX(KEY_DEBUG_CREATURE_EFFECT):
                    if ((message.payload.keyboard.modifiers & DEBUG_VAPORIZE_MASK) != 0) {
                        VaporizeCreature(1, 1);
                    } else if ((message.payload.keyboard.modifiers
                                & DEBUG_DOUBLE_RIPPLE_MASK)
                               != 0) {
                        RippleCreature(1, 1, 1);
                        RippleCreature(1, 1, 2);
                    } else {
                        RippleCreature(1, 1, 0);
                    }
                    break;
                case IDX(KEY_WAIT):
                    giNextAction = COMBAT_AI_ACTION_WAIT;
                    break;
                case IDX(KEY_MOUSE_COORDS): {
                    i32 currentMouseX_18;
                    i32 currentMouseY_18;
                    gpMouseManager->MouseCoords(currentMouseX_18, currentMouseY_18);
                    break;
                }
                case IDX(KEY_VIEW_GENERAL):
                    if (m_heroes[m_currentSide] != NULL) {
                        gpMouseManager->SetPointer(POINTER_DEFAULT);
                        ViewGeneral(m_currentSide, 1, 0);
                        ResetMouse();
                    }
                    break;
                case IDX(KEY_VIEW_ARMY):
                    gpMouseManager->SetPointer(POINTER_DEFAULT);
                    ViewArmy(&m_armies[m_currentArmySide][m_currentArmyIndex], 0);
                    ResetMouse();
                    break;
                case IDX(KEY_CAST_SPELL):
                    if (m_heroes[m_currentSide] == NULL) {
                        NormalDialog(
                            "You have no hero to cast a spell.",
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
                            "The Sphere of Negation artifact is in effect for this battle, "
                            "disabling all combat spells.",
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
                    } else if (m_heroCastSpell[m_currentSide] != 0 && giDebugLevel == 0) {
                        NormalDialog(
                            "You have already cast a spell this round.",
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
                        gpMouseManager->SetPointer(POINTER_DEFAULT);
                        giCurGeneral = m_currentSide;
                        ViewSpells(0);
                        ResetMouse();
                    }
                    break;
            }
            break;
    }

    return IDX(MAIN_CONTINUE);
}

VA(0x0042c40a, 0x70)
i32 combatManager::IsNegationSphereInEffect(void) {
    i32 side;
    for (side = 0; side < COMBAT_MANAGER_SIDE_COUNT; side++) {
        if (m_heroes[side] != NULL && m_heroes[side]->HasArtifact(ARTIFACT_SPHERE_NEGATION) != 0)
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

    i32 side;
    i32 armyIndex;
    for (side = 0; side < COMBAT_MANAGER_SIDE_COUNT; side++) {
        for (armyIndex = 0; armyIndex < COMBAT_ARMY_SLOT_COUNT; armyIndex++) {
            army* currentArmy = m_armies[side] + armyIndex;
            if (currentArmy->m_quantity > 0) {
                currentArmy->m_monster.flags.abilityFlags &=
                    MonsterAbilityFlags(IDX(ROUND_ABILITY_FLAGS));
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
        if (IsWinner(1 - m_currentSide) != 0)
            m_combatResult = RESULT_DRAW;
        else
            m_combatResult = m_currentSide;
    } else if (IsWinner(1 - m_currentSide) != 0) {
        combatEnded = 1;
        m_combatResult = 1 - m_currentSide;
    } else if (m_sideRetreated[0] != 0 || m_sideRetreated[1] != 0) {
        combatEnded = 1;
        gbRetreatWin = true;
        if (m_sideRetreated[0] != 0)
            m_combatResult = 1;
        else
            m_combatResult = 0;
    }

    if (combatEnded != 0 && m_combatResult != RESULT_DRAW) {
        i32 armyAlive = 0;
        i32 unusedWinWord37;
        i32 armyIndex;
        for (armyIndex = 0; armyIndex < COMBAT_ARMY_SLOT_COUNT; armyIndex++) {
            if (m_armies[m_combatResult][armyIndex].m_monsterType != CREATURE_NONE
                && m_armies[m_combatResult][armyIndex].m_quantity > 0
                && HAS(m_armies[m_combatResult][armyIndex].m_monster.flags.all,
                       MONSTER_FLAGS_SUMMONED)
                       == 0) {
                armyAlive = 1;
            }
        }
        if (armyAlive == 0)
            m_combatResult = RESULT_DRAW;
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

// @semantic: first normalized residual is instruction 21 in the source-ordered special-hex switch.
VA(0x0042c8ff, 0x51a)
CombatMessageCommand combatManager::GetCommand(i32 hexIndex) {
    i32 column = hexIndex % COMBAT_GRID_ROW_LENGTH;
    i32 row = hexIndex / COMBAT_GRID_ROW_LENGTH;
    CombatMessageCommand command = COMBAT_MESSAGE_COMMAND_DEFAULT;
    i32 showSmallView = 0;

    if (hexIndex == INVALID_HEX) {
        command = COMBAT_MESSAGE_COMMAND_DEFAULT;
    } else {
        switch (hexIndex) {
            case COMBAT_GRID_RIGHT_HERO_HEX:
                if (m_heroes[1] != NULL) {
                    if (m_currentSide == 1)
                        command = COMBAT_MESSAGE_COMMAND_OPTIONS;
                    else
                        command = COMBAT_MESSAGE_COMMAND_OPPOSING_OPTIONS;
                } else {
                    command = COMBAT_MESSAGE_COMMAND_DEFAULT;
                }
                break;
            case COMBAT_GRID_LEFT_SPECIAL_HEX:
                if (m_heroes[0] != NULL) {
                    if (m_currentSide == 0)
                        command = COMBAT_MESSAGE_COMMAND_OPTIONS;
                    else
                        command = COMBAT_MESSAGE_COMMAND_OPPOSING_OPTIONS;
                } else {
                    command = COMBAT_MESSAGE_COMMAND_DEFAULT;
                }
                break;
            case COMBAT_BALLISTA_HEX:
                if (m_inCastleCombat == 0)
                    command = COMBAT_MESSAGE_COMMAND_DEFAULT;
                else
                    command = COMBAT_MESSAGE_COMMAND_VIEW_INFO;
                break;
            default: {
                if (hexIndex % COMBAT_GRID_ROW_LENGTH == COMBAT_GRID_ROW_LENGTH - 1) {
                    command = COMBAT_MESSAGE_COMMAND_DEFAULT;
                    break;
                }

                i32 targetSide = m_hexCells[hexIndex].m_occupantSide;
                i32 targetIndex = m_hexCells[hexIndex].m_occupantIndex;
                army* currentArmy = &m_armies[m_currentArmySide][m_currentArmyIndex];
                currentArmy->m_targetSide = -1;
                currentArmy->m_targetIndex = -1;

                if (m_hexCells[hexIndex].m_blocked != 0
                    && (gpCombatManager->m_inCastleCombat == 0
                        || (hexIndex != COMBAT_CASTLE_GATE_APPROACH_HEX
                            && hexIndex != CASTLE_GATE_HEX)
                        || (gpCombatManager->m_drawbridgeState == COMBAT_CASTLE_GATE_OPEN
                            && (gpCombatManager->m_currentSide != COMBAT_DEFENDER_SIDE
                                || gpCombatManager->m_hexCells[COMBAT_CASTLE_GATE_APPROACH_HEX]
                                           .m_occupantSide
                                       != -1
                                || gpCombatManager->m_hexCells[COMBAT_CASTLE_GATE_APPROACH_HEX]
                                           .m_deadOccupantCount
                                       != 0)))) {
                    command = COMBAT_MESSAGE_COMMAND_DEFAULT;
                } else if (targetSide != -1) {
                    if (m_currentArmySide != targetSide || m_currentArmyIndex != targetIndex) {
                        showSmallView = 1;
                        if (gbProcessingCombatAction == 0 && giNextAction == 0) {
                            m_smallViewSide[1] = targetSide;
                            m_smallViewArmyIndex[1] = targetIndex;
                            DrawSmallView(1, 1);
                        }
                    }
                    switch (targetSide) {
                        case 0:
                        case 1:
                            if (m_currentSide == targetSide
                                || (m_currentArmySide == targetSide
                                    && m_currentArmyIndex == targetIndex)) {
                                return COMBAT_MESSAGE_COMMAND_VIEW_INFO;
                            }
                            currentArmy->m_targetSide = targetSide;
                            currentArmy->m_targetIndex = targetIndex;
                            if (currentArmy->m_monster.shots > 0
                                && currentArmy->GetAttackMask(
                                    currentArmy->m_hex, ARMY_ATTACK_TARGET_ENEMY, -1
                                )
                                       == ARMY_ALL_ATTACK_DIRECTIONS) {
                                if (ShotIsThroughWall(
                                        currentArmy->m_side,
                                        currentArmy->m_hex,
                                        hexIndex
                                    )
                                    != 0)
                                    return COMBAT_MESSAGE_COMMAND_SHOOT_THROUGH_WALL;
                                return COMBAT_MESSAGE_COMMAND_SHOOT;
                            }
                            if (currentArmy->ValidPath(hexIndex, 0) == 1)
                                return COMBAT_MESSAGE_COMMAND_ATTACK;
                            currentArmy->m_targetSide = -1;
                            currentArmy->m_targetIndex = -1;
                    }
                    command = COMBAT_MESSAGE_COMMAND_DEFAULT;
                } else {
                    if (m_armies[m_currentArmySide][m_currentArmyIndex].ValidPath(hexIndex, 0)
                        == 1) {
                        command = CombatMessageCommand(
                            2
                            - (HAS(m_armies[m_currentArmySide][m_currentArmyIndex]
                                       .m_monster.flags.all,
                                   MONSTER_FLAGS_FLYING)
                               == 0)
                        );
                    }
                }
                break;
            }
        }
    }

    if (showSmallView == 0 && gbProcessingCombatAction == 0) {
        m_smallViewSide[1] = -1;
        DrawSmallView(1, 1);
    }
    return command;
}

// @semantic: extra continuation jump at instruction 123.
VA(0x0042ce19, 0x2a6)
i32 combatManager::RightClick(i32 hexIndex) {
    i32 column = hexIndex % COMBAT_GRID_ROW_LENGTH;
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
                ViewGeneral(1, 0, 1);
                ResetMouse();
            }
            return 0;
        case COMBAT_GRID_LEFT_SPECIAL_HEX:
            if (m_heroes[0] != NULL) {
                ViewGeneral(0, 0, 1);
                ResetMouse();
            }
            return 0;
        default:
            if (hexIndex % COMBAT_GRID_ROW_LENGTH == COMBAT_GRID_ROW_LENGTH - 1)
                return 0;

            i32 side = m_hexCells[hexIndex].m_occupantSide;
            i32 armyIndex = m_hexCells[hexIndex].m_occupantIndex;
            if (m_hexCells[hexIndex].m_blocked != 0
                && (gpCombatManager->m_inCastleCombat == 0
                    || (hexIndex != COMBAT_CASTLE_GATE_APPROACH_HEX
                        && hexIndex != CASTLE_GATE_HEX)
                    || (gpCombatManager->m_drawbridgeState == COMBAT_CASTLE_GATE_OPEN
                        && (gpCombatManager->m_currentSide != COMBAT_DEFENDER_SIDE
                            || gpCombatManager->m_hexCells[COMBAT_CASTLE_GATE_APPROACH_HEX]
                                       .m_occupantSide
                                   != -1
                            || gpCombatManager->m_hexCells[COMBAT_CASTLE_GATE_APPROACH_HEX]
                                       .m_deadOccupantCount
                                   != 0))))
                return 0;

            if (side != -1) {
                switch (side) {
                    case 0:
                    case 1:
                        gpMouseManager->SetPointer(POINTER_DEFAULT);
                        ViewArmy(&m_armies[side][m_hexCells[m_selectedHex].m_occupantIndex], 1);
                        ResetMouse();
                        return 0;
                }
            }
    }
    return 0;
}

// @semantic: first normalized residual is the initial current-side/current-index address multiplication order.
VA(0x0042d0bf, 0x3b3)
void combatManager::DoCommand(i32 command) {
    i32 unusedCommandWord2;
    i32 unusedCommandWord5;
    army* currentArmy = &m_armies[m_currentArmySide][m_currentArmyIndex];
    switch (command) {
        case IDX(COMBAT_MESSAGE_COMMAND_DEFAULT):
            break;
        case IDX(COMBAT_MESSAGE_COMMAND_MOVE):
        case IDX(COMBAT_MESSAGE_COMMAND_FLY):
        case IDX(COMBAT_MESSAGE_COMMAND_SHOOT):
        case IDX(COMBAT_MESSAGE_COMMAND_SHOOT_THROUGH_WALL):
            giNextAction = COMBAT_AI_ACTION_MOVE;
            giNextActionGridIndex = m_selectedHex;
            giNextActionExtra = -1;
            break;
        case IDX(COMBAT_MESSAGE_COMMAND_ATTACK):
            giNextActionGridIndex = m_selectedHex;
            if (m_playerId[m_currentSide] == -1 || gbHumanPlayer[m_playerId[m_currentSide]] == 0
                || m_gridSelectionDisabled != 0) {
                giNextAction = COMBAT_AI_ACTION_MOVE;
                giNextActionExtra = -1;
            } else {
                giNextAction = COMBAT_AI_ACTION_ATTACK;
                giNextActionExtra = m_directionTargetHex;
            }
            break;
        case IDX(COMBAT_MESSAGE_COMMAND_OPTIONS):
            gpMouseManager->SetPointer(POINTER_DEFAULT);
            ViewGeneral(m_currentSide, 1, 0);
            ResetMouse();
            break;
        case IDX(COMBAT_MESSAGE_COMMAND_OPPOSING_OPTIONS):
            gpMouseManager->SetPointer(POINTER_DEFAULT);
            ViewGeneral(1 - m_currentSide, 1, 0);
            ResetMouse();
            break;
        case IDX(COMBAT_MESSAGE_COMMAND_VIEW_INFO):
            gpMouseManager->SetPointer(POINTER_DEFAULT);
            if (m_selectedHex == COMBAT_BALLISTA_HEX)
                ViewBallista(0);
            else
                ViewArmy(
                    &m_armies[m_hexCells[m_selectedHex].m_occupantSide]
                             [m_hexCells[m_selectedHex].m_occupantIndex],
                    0
                );
            ResetMouse();
            break;
        case IDX(COMBAT_MESSAGE_COMMAND_CAST_SPELL):
            if (IsNegationSphereInEffect() != 0) {
                NormalDialog(
                    "The Sphere of Negation artifact is in effect for this battle, disabling all "
                    "combat spells.",
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
                ViewSpells(0);
                ResetMouse();
            }
            break;
        case IDX(COMBAT_MESSAGE_COMMAND_RETREAT):
            NormalDialog(
                "Are you sure you want to retreat?",
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
                giNextAction = COMBAT_AI_ACTION_RETREAT;
            ResetMouse();
            break;
        case IDX(COMBAT_MESSAGE_COMMAND_SURRENDER):
            if (DoSurrender() == 1) {
                if (gpGame->m_players[m_playerId[m_currentSide]].m_resources[IDX(RES_GOLD)]
                    < giSurrenderCost) {
                    NormalDialog(
                        "You don't have enough gold!",
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
                    giNextAction = COMBAT_AI_ACTION_SURRENDER;
                    giNextActionExtra = giSurrenderCost;
                }
            }
            ResetMouse();
            break;
    }
}

// @early-stop: byte-proven compiler artifact.
VA(0x0042d472, 0x57b)
i32 WinCombatHandler(struct tag_message& message) {
    char iconFile[40];
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
        return IDX(MAIN_FINISHED);
    }

    if (message.type == MESSAGE_WIDGET) {
        switch (message.payload.widget.command) {
            case IDX(WINDOW_CLICK):
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
                            if (gpCombatManager->m_eagleEyeSpell[gpCombatManager->m_combatResult]
                                != -1) {
                                gpCombatManager->ClearWinLoseBottom(
                                    gpCombatManager->m_winLoseWindow
                                );
                                gpCombatManager->ShowEagleEyeSpell(
                                    gpCombatManager->m_winLoseWindow
                                );
                                gpCombatManager->m_eagleEyeSpell[gpCombatManager->m_combatResult] =
                                    -1;
                            } else {
                            ExitDialog:
                                gpWindowManager->m_dialogResult = message.payload.widget.id;
                                message.payload.widget.id = WIN_LOSE_CLOSE_COMMAND;
                                message.payload.widget.command =
                                    BaseWidgetCommand(message.payload.widget.id);
                                return IDX(MAIN_FINISHED);
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
                    sprintf(iconFile, "cmbtfle2.icn");
                    animationMessage.payload.widget.id = WIN_LOSE_RESOURCE_LOAD_ID;
                    gpCombatManager->m_winLoseWindow->BroadcastMessage(animationMessage);
                    animationMessage.payload.widget.id = WIN_LOSE_RESOURCE_DRAW_ID;
                    gpCombatManager->m_winLoseWindow->BroadcastMessage(animationMessage);
                }
                if (giWinCmbtFrame == WIN_LOSE_FLEE_THIRD_RESOURCE_FRAME) {
                    sprintf(iconFile, "cmbtfle3.icn");
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
                    sprintf(iconFile, "cmbtlos2.icn");
                    animationMessage.payload.widget.id = WIN_LOSE_RESOURCE_LOAD_ID;
                    gpCombatManager->m_winLoseWindow->BroadcastMessage(animationMessage);
                    animationMessage.payload.widget.id = WIN_LOSE_RESOURCE_DRAW_ID;
                    gpCombatManager->m_winLoseWindow->BroadcastMessage(animationMessage);
                }
                if (giWinCmbtFrame == WIN_LOSE_LOSS_THIRD_RESOURCE_FRAME) {
                    sprintf(iconFile, "cmbtlos3.icn");
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
    return IDX(MAIN_CONTINUE);
}

// @early-stop: byte-proven compiler artifact.
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

// @early-stop: byte-proven compiler artifact.
VA(0x0042dafd, 0x29a)
void combatManager::ShowWinLoseArtifact(class heroWindow* window, i32 artifact) {
    DATA(0x004f09e8) static i16 artifactSourceLineBase = 0x680;
    i16 w = 320;
    i16 winBottom = 458;
    tag_message message;
    char* capturedArtifactName;

    sprintf(gText, "You have captured an enemy artifact!");
    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = COMBAT_WIN_LOSE_TEXT_COMMAND;
    message.payload.widget.id = WIN_LOSE_TEXT_ID;
    message.payload.widget.data.text = gText;
    m_winLoseWindow->BroadcastMessage(message);

    m_winLoseBottomWidgets[0] = new iconWidget(
        136,
        310,
        80,
        80,
        "winloseb.icn",
        0,
        0,
        WIN_LOSE_ARTIFACT_ICON_ID,
        WIN_LOSE_ICON_FLAGS,
        1
    );
    if (m_winLoseBottomWidgets[0] == NULL)
        MemError();
    window->AddWidget(m_winLoseBottomWidgets[0], -1);

    m_winLoseBottomWidgets[1] = new iconWidget(
        144,
        318,
        64,
        64,
        "artifact.icn",
        artifact + 1,
        0,
        WIN_LOSE_ARTIFACT_IMAGE_ID,
        WIN_LOSE_ICON_FLAGS,
        1
    );
    if (m_winLoseBottomWidgets[1] == NULL)
        MemError();
    window->AddWidget(m_winLoseBottomWidgets[1], -1);

    capturedArtifactName = static_cast<char*>(H2_ALLOC(60, 1707));
    sprintf(capturedArtifactName, gArtifactNames[artifact]);
    m_winLoseBottomTextWidgets[0] = new textWidget(
        16,
        397,
        320,
        12,
        capturedArtifactName,
        "smalfont.fnt",
        1,
        WIN_LOSE_ARTIFACT_TEXT_ID,
        WIN_LOSE_TEXT_FLAGS,
        1
    );
    if (m_winLoseBottomTextWidgets[0] == NULL)
        MemError();
    window->AddWidget(m_winLoseBottomTextWidgets[0], -1);

    gpCombatManager->m_winLoseWindow->DrawWindow();
    SAMPLE2 pickupSample = NULL_SAMPLE2;
    sprintf(gText, "pickup%02d.82M", SRandom(1, 5));
    pickupSample = LoadPlaySample(gText);
    WaitEndSample(pickupSample, -1);
}

// @early-stop: byte-proven compiler artifact.
VA(0x0042dd97, 0x232)
void combatManager::ShowSkeletons(class heroWindow* window) {
    DATA(0x004f0a80) static i16 skeletonSourceLineBase = 0x6c8;
    tag_message message;
    char* skeletonCount;

    m_winLoseBottomWidgets[0] = new iconWidget(
        173,
        270,
        32,
        30,
        "mons32.icn",
        47,
        0,
        WIN_LOSE_SKELETON_ICON_ID,
        WIN_LOSE_ICON_FLAGS,
        1
    );
    if (m_winLoseBottomWidgets[0] == NULL)
        MemError();

    skeletonCount = static_cast<char*>(H2_ALLOC(9, 1755));
    sprintf(skeletonCount, "%d", giSkeletonsCreated);
    m_winLoseBottomTextWidgets[0] = new textWidget(
        165,
        300,
        32,
        12,
        skeletonCount,
        "smalfont.fnt",
        1,
        WIN_LOSE_SKELETON_TEXT_ID,
        WIN_LOSE_TEXT_FLAGS,
        1
    );
    if (m_winLoseBottomTextWidgets[0] == NULL)
        MemError();

    window->AddWidget(m_winLoseBottomWidgets[0], -1);
    window->AddWidget(m_winLoseBottomTextWidgets[0], -1);
    if (giSkeletonsCreated > 1) {
        sprintf(
            gText,
            "Practicing the dark arts of necromancy, you are able to "
            "raise %d of the enemy's dead to return under your service "
            "as Skeletons.",
            giSkeletonsCreated
        );
    } else {
        sprintf(
            gText,
            "Practicing the dark arts of necromancy, you are able to "
            "raise one of the enemy's dead to return under your service "
            "as a Skeleton."
        );
    }
    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = COMBAT_WIN_LOSE_TEXT_COMMAND;
    message.payload.widget.id = WIN_LOSE_TEXT_ID;
    message.payload.widget.data.text = gText;
    m_winLoseWindow->BroadcastMessage(message);
    gpCombatManager->m_winLoseWindow->DrawWindow();

    SAMPLE2 pickupSample = NULL_SAMPLE2;
    sprintf(gText, "pickup%02d.82M", SRandom(1, 5));
    pickupSample = LoadPlaySample(gText);
    WaitEndSample(pickupSample, -1);
}

// @early-stop: byte-proven compiler artifact.
VA(0x0042dfc9, 0x2f6)
void combatManager::ShowEagleEyeSpell(class heroWindow* window) {
    DATA(0x004f0be4) static i16 eagleEyeSourceLineBase = 0x702;
    i32 displayedSpell = m_eagleEyeSpell[m_combatResult];
    i32 x = 105;
    i32 y = 275;
    tag_message spellMessage;
    char* spellName;

    m_winLoseBottomWidgets[0] = new iconWidget(
        x + 16,
        y,
        0,
        0,
        "townwind.icn",
        0,
        0,
        WIN_LOSE_EAGLE_BACKGROUND_ID,
        WIN_LOSE_ICON_FLAGS,
        1
    );
    if (m_winLoseBottomWidgets[0] == NULL)
        MemError();

    m_winLoseBottomWidgets[1] = new iconWidget(
        x + 36,
        y + 5,
        72,
        51,
        "spells.icn",
        static_cast<i16>(gsSpellInfo[displayedSpell].iconIndex),
        0,
        WIN_LOSE_EAGLE_SPELL_ID,
        WIN_LOSE_SPELL_ICON_FLAGS,
        1
    );
    if (m_winLoseBottomWidgets[1] == NULL)
        MemError();

    spellName = static_cast<char*>(H2_ALLOC(200, 1828));
    sprintf(spellName, "%s", gSpellNames[displayedSpell]);
    m_winLoseBottomTextWidgets[0] = new textWidget(
        x + 34,
        y + 57,
        80,
        24,
        spellName,
        "smalfont.fnt",
        1,
        WIN_LOSE_EAGLE_TEXT_ID,
        WIN_LOSE_TEXT_FLAGS,
        1
    );
    if (m_winLoseBottomTextWidgets[0] == NULL)
        MemError();

    window->AddWidget(m_winLoseBottomWidgets[0], -1);
    window->AddWidget(m_winLoseBottomWidgets[1], -1);
    window->AddWidget(m_winLoseBottomTextWidgets[0], -1);
    sprintf(
        gText,
        "Through eagle-eyed observation, %s is able to learn the magic "
        "spell '%s'.",
        m_heroes[m_combatResult]->m_name,
        gSpellNames[displayedSpell]
    );
    spellMessage.type = MESSAGE_WIDGET;
    spellMessage.payload.widget.command = COMBAT_WIN_LOSE_TEXT_COMMAND;
    spellMessage.payload.widget.id = WIN_LOSE_TEXT_ID;
    spellMessage.payload.widget.data.text = gText;
    m_winLoseWindow->BroadcastMessage(spellMessage);
    gpCombatManager->m_winLoseWindow->DrawWindow();

    SAMPLE2 playedSample = NULL_SAMPLE2;
    sprintf(gText, "pickup%02d.82M", SRandom(1, 5));
    playedSample = LoadPlaySample(gText);
    WaitEndSample(playedSample, -1);
}

// @semantic: first non-relocation normalized residual is instruction 399, ours jle versus retail jge at the displayed-army loop.
VA(0x0042e2bf, 0x9cc)
void combatManager::ShowDeadArmies(class heroWindow* window) {
    DATA(0x004f0ca0) static i16 casualtySourceLineBase = 0x74b;
    i32 casualtyQuantity_0[CASUALTY_QUANTITY_STORAGE_COUNT];
    i32 casualtyType_1[COMBAT_MANAGER_SIDE_COUNT][COMBAT_ARMY_SLOT_COUNT];
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
    for (side_9 = 0; side_9 < COMBAT_MANAGER_SIDE_COUNT; ++side_9) {
        casualtyQuantity_0[side_9] = 0;
        for (armyIndex_8 = 0; armyIndex_8 < COMBAT_ARMY_SLOT_COUNT; ++armyIndex_8) {
            if (m_armies[side_9][armyIndex_8].m_monsterType != CREATURE_NONE
                && m_armies[side_9][armyIndex_8].m_quantity
                       < m_armies[side_9][armyIndex_8].m_initialQuantity) {
                *(&casualtyType_1[0][0] + side_9 * COMBAT_ARMY_SLOT_COUNT
                  + casualtyQuantity_0[side_9]) = IDX(m_armies[side_9][armyIndex_8].m_monsterType);
                *(&casualtyQuantity_0[COMBAT_MANAGER_SIDE_COUNT] + side_9 * COMBAT_ARMY_SLOT_COUNT
                  + casualtyQuantity_0[side_9]) = m_armies[side_9][armyIndex_8].m_initialQuantity
                                                  - m_armies[side_9][armyIndex_8].m_quantity;
                ++casualtyQuantity_0[side_9];
            }
        }
    }

    text_27 = static_cast<char*>(H2_ALLOC(30, 1902));
    sprintf(text_27, "Battlefield Casualties");
    m_winLoseBottomTextWidgets[17] = new textWidget(
        16,
        263,
        CASUALTY_WINDOW_WIDTH,
        20,
        text_27,
        "smalfont.fnt",
        1,
        2110,
        WIN_LOSE_TEXT_FLAGS,
        1
    );
    if (m_winLoseBottomTextWidgets[17] == NULL)
        MemError();
    window->AddWidget(m_winLoseBottomTextWidgets[17], -1);

    for (side_9 = 0; side_9 < COMBAT_MANAGER_SIDE_COUNT; ++side_9) {
        if (side_9 == COMBAT_ATTACKER_SIDE)
            y_29 = 279;
        else
            y_29 = 346;
        text_27 = static_cast<char*>(H2_ALLOC(30, 1923));
        sprintf(text_27, side_9 == COMBAT_ATTACKER_SIDE ? "Attacker" : "Defender");
        m_winLoseBottomTextWidgets[15 + side_9] = new textWidget(
            16,
            y_29 + 3,
            CASUALTY_WINDOW_WIDTH,
            20,
            text_27,
            "smalfont.fnt",
            1,
            2110,
            WIN_LOSE_TEXT_FLAGS,
            1
        );
        if (m_winLoseBottomTextWidgets[15 + side_9] == NULL)
            MemError();
        window->AddWidget(m_winLoseBottomTextWidgets[15 + side_9], -1);

        if (casualtyQuantity_0[side_9] <= 0) {
            text_27 = static_cast<char*>(H2_ALLOC(10, 1942));
            sprintf(text_27, "None");
            m_winLoseBottomTextWidgets[side_9 * CASUALTY_WIDGETS_PER_SIDE] = new textWidget(
                16,
                y_29 + 21,
                CASUALTY_WINDOW_WIDTH,
                20,
                text_27,
                "smalfont.fnt",
                1,
                side_9 * 5 + 2100,
                WIN_LOSE_TEXT_FLAGS,
                1
            );
            if (m_winLoseBottomTextWidgets[side_9 * CASUALTY_WIDGETS_PER_SIDE] == NULL)
                MemError();
            window->AddWidget(
                m_winLoseBottomTextWidgets[side_9 * CASUALTY_WIDGETS_PER_SIDE],
                -1
            );
        }

        monsterIcons_2 = gpResourceManager->GetIcon("mons32.icn");
        displayedCount_11 = casualtyQuantity_0[side_9] < CASUALTY_DISPLAY_LIMIT
                                ? casualtyQuantity_0[side_9]
                                : CASUALTY_DISPLAY_LIMIT;
        spacing_9 = 40;
        startX_0 = (CASUALTY_WINDOW_WIDTH - displayedCount_11 * spacing_9) / 2 + 3;
        for (armyIndex_8 = 0; displayedCount_11 > armyIndex_8; ++armyIndex_8) {
            m_winLoseBottomWidgets[side_9 * CASUALTY_WIDGETS_PER_SIDE + armyIndex_8] =
                new iconWidget(
                    armyIndex_8 * spacing_9 + startX_0
                        - GetIconEntry(
                              monsterIcons_2,
                              *(&casualtyType_1[0][0] + side_9 * COMBAT_ARMY_SLOT_COUNT
                                + armyIndex_8)
                        )
                              ->x
                        + static_cast<i16>(
                            (32
                             - GetIconEntry(
                                   monsterIcons_2,
                                   *(&casualtyType_1[0][0] + side_9 * COMBAT_ARMY_SLOT_COUNT
                                     + armyIndex_8)
                             )
                                   ->w)
                            / 2
                        )
                        + 17,
                    (y_29
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
                        + 51,
                    32,
                    28,
                    "mons32.icn",
                    *(&casualtyType_1[0][0] + side_9 * COMBAT_ARMY_SLOT_COUNT + armyIndex_8),
                    0,
                    side_9 * 5 + armyIndex_8 + 2000,
                    WIN_LOSE_ICON_FLAGS,
                    1
                );
            if (m_winLoseBottomWidgets[side_9 * CASUALTY_WIDGETS_PER_SIDE + armyIndex_8]
                == NULL)
                MemError();

            text_27 = static_cast<char*>(H2_ALLOC(9, 1986));
            sprintf(
                text_27,
                "%d",
                *(&casualtyQuantity_0[COMBAT_MANAGER_SIDE_COUNT] + side_9 * COMBAT_ARMY_SLOT_COUNT
                  + armyIndex_8)
            );
            m_winLoseBottomTextWidgets[side_9 * CASUALTY_WIDGETS_PER_SIDE + armyIndex_8] =
                new textWidget(
                    armyIndex_8 * spacing_9 + startX_0 + 16,
                    y_29 + 53,
                    32,
                    12,
                    text_27,
                    "smalfont.fnt",
                    1,
                    side_9 * 5 + armyIndex_8 + 2100,
                    WIN_LOSE_TEXT_FLAGS,
                    1
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

// @semantic: First residual is instruction 52 (armyIndex/side strength-reduction order), repeated at 131.
VA(0x0042ec8b, 0xba9)
void combatManager::DoVictory(i32 winningSide) {
    char experienceText[152];
    i32 experienceLevels = 0;
    i32 deadCreatureCount;
    i32 eligibleWinnerStacks;
    i32 side;
    i32 armyIndex;
    i32 lastLivingArmy;
    i32 livingCreatureCount;
    army* currentArmy;
    i32 fadeCount;
    i32 fadeTimer;
    i32 fadeIndex;
    i32 emptyArtifactSlots;
    tag_message message;

    if (m_heroes[COMBAT_DEFENDER_SIDE] != NULL && m_heroes[COMBAT_DEFENDER_SIDE]->m_isCaptain != 0)
        m_heroes[COMBAT_DEFENDER_SIDE] = NULL;
    gbShowingLoseWindow = false;
    gbWhichAnimationPlaying = true;
    giWinCmbtFrame = 0;
    giSkeletonsCreated = 0;
    iMaxTransferArtifacts = 0;
    iCurTransferArtifact = -1;
    bSkeletonsShown = 0;
    deadCreatureCount = 0;
    eligibleWinnerStacks = 0;

    for (side = 0; side < COMBAT_MANAGER_SIDE_COUNT; ++side) {
        livingCreatureCount = 0;
        lastLivingArmy = -1;
        for (armyIndex = 0; armyIndex < gpCombatManager->m_armyCount[side]; ++armyIndex) {
            currentArmy =
                side * COMBAT_ARMY_STORAGE_SLOT_COUNT + &m_armies[COMBAT_ATTACKER_SIDE][armyIndex];
            if (currentArmy->m_quantity > 0) {
                lastLivingArmy = armyIndex;
                if (currentArmy->m_temporaryResurrectionQuantity > 0)
                    currentArmy->m_quantity -= currentArmy->m_temporaryResurrectionQuantity;
                if (currentArmy->m_quantity < 0)
                    currentArmy->m_quantity = 0;
                livingCreatureCount += currentArmy->m_quantity;
            }
            if (side == winningSide && currentArmy->m_quantity > 0
                && HAS(currentArmy->m_monster.flags.all, MONSTER_FLAGS_LIGHT_PALETTE) == 0
                && currentArmy->m_monsterType != CREATURE_EARTH_ELEMENTAL
                && currentArmy->m_monsterType != CREATURE_AIR_ELEMENTAL
                && currentArmy->m_monsterType != CREATURE_FIRE_ELEMENTAL
                && currentArmy->m_monsterType != CREATURE_WATER_ELEMENTAL
                && currentArmy->m_monsterType != CREATURE_GHOST) {
                ++eligibleWinnerStacks;
            }
            if (winningSide == COMBAT_DEFENDER_SIDE - side) {
                deadCreatureCount += currentArmy->m_initialQuantity - currentArmy->m_quantity;
            }
        }
        if (livingCreatureCount == 0 && lastLivingArmy != -1)
            (side * COMBAT_ARMY_STORAGE_SLOT_COUNT
             + &m_armies[COMBAT_ATTACKER_SIDE][lastLivingArmy])
                ->m_quantity = 1;
    }

    if (winningSide != RESULT_DRAW
        && eligibleWinnerStacks < VICTORY_NECROMANCY_STACK_LIMIT
        && m_heroes[winningSide] != NULL
        && m_heroes[winningSide]->GetSSLevel(IDX(HERO_SKILL_NECROMANCY)) != 0) {
        giSkeletonsCreated = static_cast<i32>(
            deadCreatureCount
            * (m_heroes[winningSide]->GetSSLevel(IDX(HERO_SKILL_NECROMANCY))
               * COMBAT_NECROMANCY_LEVEL_FACTOR)
        );
        if (giSkeletonsCreated <= 0 && deadCreatureCount != 0)
            giSkeletonsCreated = 1;
    }

    m_nonVisualCombat = 1;
    FreeArmies();
    CombatMessage("", 1, 1, 0);
    gpMouseManager->SetPointer(POINTER_DEFAULT);
    fadeCount = VICTORY_FADE_STEPS;
    if (m_terrainType == VICTORY_WATER_TERRAIN)
        fadeCount = VICTORY_WATER_FADE_STEPS;
    fadeTimer = KBTickCount();
    for (fadeIndex = 0; fadeCount > fadeIndex; ++fadeIndex) {
        PollSound();
        DelayTil(&fadeTimer);
        fadeTimer = KBTickCount() + VICTORY_FADE_DELAY;
        DimBitmapArea(
            gpWindowManager->m_screen,
            0,
            0,
            COMBAT_SCREEN_WIDTH,
            SCREEN_HEIGHT,
            3
        );
        PollSound();
        gpWindowManager->UpdateScreenRegion(0, 0, SCREEN_MAX_X, SCREEN_MAX_Y);
        PollSound();
    }

    switch (winningSide) {
        case RESULT_DRAW:
            gpSoundManager->SwitchAmbientMusic(LOSS_MUSIC);
            DoLoseWindow();
            break;
        case COMBAT_ATTACKER_SIDE:
        case COMBAT_DEFENDER_SIDE:
            if (m_heroes[winningSide] != NULL) {
                if (m_eagleEyeSpell[winningSide] != IDX(SPELL_NONE)) {
                    m_heroes[winningSide]->m_spells[m_eagleEyeSpell[winningSide]] = 1;
                }
                m_experienceValue[COMBAT_DEFENDER_SIDE - winningSide] =
                    ExperienceValueOfStack(COMBAT_DEFENDER_SIDE - winningSide);
                if (gbRetreatWin != 0)
                    m_experienceValue[COMBAT_DEFENDER_SIDE - winningSide] -=
                        COMBAT_HERO_EXPERIENCE_VALUE;
                if (m_combatTowns[COMBAT_DEFENDER_SIDE] != NULL && winningSide == COMBAT_ATTACKER_SIDE)
                    m_experienceValue[COMBAT_DEFENDER_SIDE - winningSide] +=
                        COMBAT_HERO_EXPERIENCE_VALUE;
                experienceLevels = gpAdvManager->GiveExperience(
                    m_heroes[winningSide],
                    m_experienceValue[COMBAT_DEFENDER_SIDE - winningSide],
                    gbThisNetHumanPlayer[m_heroes[winningSide]->m_owner] == 0
                );

                if (gbRetreatWin == 0) {
                    emptyArtifactSlots = 0;
                    if (m_heroes[COMBAT_ATTACKER_SIDE] != NULL
                        && m_heroes[COMBAT_DEFENDER_SIDE] != NULL) {
                        for (fadeIndex = 0; fadeIndex < HERO_ARTIFACT_SLOT_COUNT; ++fadeIndex) {
                            if (m_heroes[winningSide]->m_artifacts[fadeIndex]
                                == IDX(ARTIFACT_NONE)) {
                                ++emptyArtifactSlots;
                            }
                        }
                        for (fadeIndex = 0; fadeIndex < HERO_ARTIFACT_SLOT_COUNT; ++fadeIndex) {
                            if (m_heroes[COMBAT_DEFENDER_SIDE - winningSide]->m_artifacts[fadeIndex]
                                    >= ARTIFACT_TRANSFERABLE_FIRST
                                && m_heroes[COMBAT_DEFENDER_SIDE - winningSide]
                                           ->m_artifacts[fadeIndex]
                                       != IDX(ARTIFACT_MAGIC_BOOK)
                                && emptyArtifactSlots > iMaxTransferArtifacts) {
                                iTransferArtifacts[iMaxTransferArtifacts] =
                                    m_heroes[COMBAT_DEFENDER_SIDE - winningSide]
                                        ->m_artifacts[fadeIndex];
                                iTransferArtifactsInfo[iMaxTransferArtifacts] =
                                    m_heroes[COMBAT_DEFENDER_SIDE - winningSide]
                                        ->m_artifactExtra[fadeIndex];
                                ++iMaxTransferArtifacts;
                            }
                        }
                    }
                }
            }

            if (!(giCurPlayer == -1 || gbThisNetHumanPlayer[giCurPlayer] == 0
                  || m_playerId[winningSide] != giCurPlayer)
                || !(
                    giCurPlayer == -1 || m_playerId[winningSide] == -1
                    || gbThisNetHumanPlayer[giCurPlayer] != 0
                    || gbThisNetHumanPlayer[m_playerId[winningSide]] == 0
                )
                || !(
                    m_playerId[winningSide] == -1
                    || gbThisNetHumanPlayer[m_playerId[winningSide]] == 0
                )) {
                gpSoundManager->SwitchAmbientMusic(VICTORY_MUSIC);
                m_winLoseWindow = new heroWindow(143, 10, "wincmbt.bin");
                if (m_winLoseWindow == NULL)
                    MemError();

                if (m_heroes[winningSide] != NULL) {
                    if (gbCombatSurrender != 0) {
                        sprintf(gText, cBattleResults[IDX(RESULT_TEXT_ENEMY_SURRENDERED)]);
                    } else if (gbRetreatWin != 0) {
                        sprintf(gText, cBattleResults[IDX(RESULT_TEXT_ENEMY_FLED)]);
                    } else {
                        sprintf(gText, cBattleResults[IDX(RESULT_TEXT_VICTORY)]);
                    }
                    if (experienceLevels > 0 && winningSide == COMBAT_DEFENDER_SIDE
                        && giNumHumanPlayers > 1) {
                        sprintf(
                            experienceText,
                            cBattleResults[IDX(RESULT_TEXT_NETWORK_EXPERIENCE)],
                            m_heroes[winningSide]->m_name,
                            m_experienceValue[COMBAT_DEFENDER_SIDE - winningSide],
                            experienceLevels
                        );
                    } else {
                        sprintf(
                            experienceText,
                            cBattleResults[IDX(RESULT_TEXT_EXPERIENCE)],
                            m_heroes[winningSide]->m_name,
                            m_experienceValue[COMBAT_DEFENDER_SIDE - winningSide]
                        );
                    }
                    strcat(gText, experienceText);
                    m_heroes[winningSide]->ApplyBattleWinTemps();
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
                    giDialogTimeout = KBTickCount() + 15000;
                gpWindowManager->DoDialog(m_winLoseWindow, WinCombatHandler, 0);
                giDialogTimeout = 0;
                delete m_winLoseWindow;
                if (m_heroes[COMBAT_DEFENDER_SIDE - winningSide] != NULL)
                    m_heroes[COMBAT_DEFENDER_SIDE - winningSide]->ApplyBattleLossTemps();
            } else {
                if (m_heroes[winningSide] != NULL)
                    m_heroes[winningSide]->ApplyBattleWinTemps();
                if (m_heroes[COMBAT_DEFENDER_SIDE - winningSide] != NULL)
                    m_heroes[COMBAT_DEFENDER_SIDE - winningSide]->ApplyBattleLossTemps();
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
    i32 unusedLoseWord_h = COMBAT_ATTACKER_SIDE;
    i32 losingSide_h;
    char animationFile_j[52];
    tag_message message;

    if (m_playerId[COMBAT_ATTACKER_SIDE] == giCurPlayer
        && gbThisNetHumanPlayer[m_playerId[COMBAT_ATTACKER_SIDE]] != 0) {
        losingSide_h = COMBAT_ATTACKER_SIDE;
    } else if (m_playerId[COMBAT_DEFENDER_SIDE] == giCurPlayer
               && gbThisNetHumanPlayer[m_playerId[COMBAT_DEFENDER_SIDE]] != 0) {
        losingSide_h = COMBAT_DEFENDER_SIDE;
    } else if (m_playerId[COMBAT_ATTACKER_SIDE] != -1
               && gbThisNetHumanPlayer[m_playerId[COMBAT_ATTACKER_SIDE]] != 0) {
        losingSide_h = COMBAT_ATTACKER_SIDE;
    } else {
        losingSide_h = COMBAT_DEFENDER_SIDE;
    }

    gbShowingLoseWindow = true;
    if (gbCombatSurrender != 0) {
        sprintf(animationFile_j, "cmbtsurr.icn");
        gbWhichAnimationPlaying = WIN_LOSE_ANIMATION_CYCLE_SECOND;
    } else if (gbRetreatWin != 0) {
        sprintf(animationFile_j, "cmbtfle1.icn");
        gbWhichAnimationPlaying = WIN_LOSE_ANIMATION_FLEE;
    } else {
        sprintf(animationFile_j, "cmbtlos1.icn");
        gbWhichAnimationPlaying = WIN_LOSE_ANIMATION_LOSS;
    }

    m_winLoseWindow = new heroWindow(143, 10, "wincmbt.bin");
    if (m_winLoseWindow == NULL)
        MemError();

    if (m_heroes[losingSide_h] != NULL) {
        if (gbCombatSurrender != 0) {
            sprintf(
                gText,
                cBattleResults[IDX(RESULT_TEXT_HERO_SURRENDER)],
                m_heroes[losingSide_h]->m_name
            );
        } else if (gbRetreatWin != 0) {
            sprintf(
                gText,
                cBattleResults[IDX(RESULT_TEXT_HERO_FLEE)],
                m_heroes[losingSide_h]->m_name
            );
        } else {
            sprintf(
                gText,
                cBattleResults[IDX(RESULT_TEXT_HERO_DEFEAT)],
                m_heroes[losingSide_h]->m_name
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
        giDialogTimeout = KBTickCount() + 15000;
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
        if (m_armies[m_currentSide][armyIndex_n].IsAlive()) {
            giSurrenderCost +=
                static_cast<i16>(
                    gMonsterDatabase[IDX(m_armies[m_currentSide][armyIndex_n].m_monsterType)].cost
                )
                * m_armies[m_currentSide][armyIndex_n].m_quantity;
        }
    }
    if (m_heroes[m_currentSide]->HasArtifact(ARTIFACT_STATESMANS_QUILL) != 0)
        giSurrenderCost = static_cast<i32>(giSurrenderCost * COMBAT_SURRENDER_QUILL_FACTOR);
    else
        giSurrenderCost = static_cast<i32>(giSurrenderCost * COMBAT_SURRENDER_BASE_FACTOR);
    giSurrenderCost = static_cast<i32>(
        giSurrenderCost
        * (1.0
           - m_heroes[m_currentSide]->m_secondarySkills[IDX(HERO_SKILL_DIPLOMACY)]
                 * COMBAT_SURRENDER_DIPLOMACY_FACTOR)
    );

    dialogType = SURRENDER_DIALOG_TYPE;
    dialogResult = SURRENDER_DIALOG_ACCEPT_RESULT;
    textWidth_t = SURRENDER_TEXT_WIDTH;
    window = new heroWindow(74, 80, "surrendr.bin");
    if (window == NULL)
        MemError();
    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = COMBAT_WIN_LOSE_RESOURCE_COMMAND;
    message.payload.widget.id = SURRENDER_PORTRAIT_RESOURCE_ID;
    sprintf(
        gText,
        "port%04d.icn",
        static_cast<u32>(m_heroes[COMBAT_DEFENDER_SIDE - m_currentSide]->m_portrait)
    );
    message.payload.widget.data.text = gText;
    window->BroadcastMessage(message);
    if (m_heroes[COMBAT_DEFENDER_SIDE - m_currentSide]->m_isCaptain != 0)
        message.payload.widget.command = COMBAT_SURRENDER_CAPTAIN_PORTRAIT_COMMAND;
    else
        message.payload.widget.command = COMBAT_SURRENDER_HERO_PORTRAIT_COMMAND;
    message.payload.widget.id = SURRENDER_PORTRAIT_WIDGET_ID;
    message.payload.widget.data.value = SURRENDER_PORTRAIT_DEFAULT_COLOR;
    window->BroadcastMessage(message);
    if (m_heroes[COMBAT_DEFENDER_SIDE - m_currentSide]->m_isCaptain != 0) {
        message.payload.widget.command = COMBAT_SURRENDER_CAPTAIN_OVERLAY_COMMAND;
        if (m_playerId[COMBAT_DEFENDER_SIDE - m_currentSide] == -1)
            message.payload.widget.data.value = SURRENDER_CAPTAIN_NEUTRAL_COLOR;
        else
            message.payload.widget.data.value =
                gpGame->m_players[m_playerId[COMBAT_DEFENDER_SIDE - m_currentSide]].m_color;
        window->BroadcastMessage(message);
    }
    message.payload.widget.data.text = gText;
    message.payload.widget.command = COMBAT_WIN_LOSE_TEXT_COMMAND;
    message.payload.widget.id = SURRENDER_TEXT_ID;
    sprintf(
        gText,
        "%s states:\n\n\"I will accept your surrender and grant you and your troops safe passage "
        "for the price of %d gold.\"",
        m_heroes[COMBAT_DEFENDER_SIDE - m_currentSide]->m_name,
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
    army* currentArmy = &m_armies[m_currentArmySide][m_currentArmyIndex];
    if (m_limitCreature == 0 || currentArmy->m_hex != m_limitCreatureHex) {
        UpdateGrid(0, 1);
        giNewMonsterCycleFrame = 7;
        m_limitCreatureHex = currentArmy->m_hex;
        m_limitCreature = 1;
        DrawFrame(1, 0, 0, 0, COMMAND_FRAME_DELAY, 1, 1);
    }
    SetupSmallView();
}

// @early-stop: byte-proven compiler artifact.
VA(0x00430109, 0xea)
void combatManager::CheckCastleAttack(void) {
    if (m_inCastleCombat != 0 && m_currentSide == 0) {
        while (m_catapultAttacksRemaining[m_currentSide] > 0) {
            CatAttack(m_currentSide);
            --m_catapultAttacksRemaining[m_currentSide];
        }
    }
    if (m_inCastleCombat != 0 && m_currentSide == 1) {
        while (m_keepAttacksRemaining[m_currentSide] > 0) {
            KeepAttack(0);
            KeepAttack(1);
            KeepAttack(2);
            --m_keepAttacksRemaining[m_currentSide];
        }
    }
}

// @early-stop: byte-proven compiler artifact.
VA(0x004301f3, 0xdd)
void combatManager::CheckGetAIMove(void) {
    i32 retreat = AICheckRetreat();
    if (m_heroCastSpell[m_currentSide] == 0
        && (m_playerId[m_currentSide] == -1 || gbThisNetHumanPlayer[m_playerId[m_currentSide]] == 0
            || gConfig.autoCombatUseSpells != 0)) {
        if (DoSpellAI(m_currentSide, retreat) != 0)
            return;
    }
    retreat = AICheckRetreat();
    if (retreat != 0)
        giNextAction = IDX(ACTION_RETREAT);
    else
        DoCompAI(m_currentSide);
}

// @early-stop: delinker jump-table artifact.
VA(0x004302d0, 0x18f)
void combatManager::GetControl(void) {
    m_selectedHex = INVALID_HEX;
    m_previousCommand = COMBAT_INVALID_COMMAND;
    m_previousCommand = COMBAT_INVALID_COMMAND;
    if (gpCombatManager->m_active)
        gpMouseManager->SetPointer(POINTER_DEFAULT);
    CheckChangeSelector();
    if (gbRemoteOn == 0 || m_playerId[COMBAT_ATTACKER_SIDE] < 0
        || m_playerId[COMBAT_DEFENDER_SIDE] < 0
        || gbHumanPlayer[m_playerId[COMBAT_DEFENDER_SIDE]] == 0
        || (gbHumanPlayer[m_playerId[COMBAT_ATTACKER_SIDE]] == 0
            && (gbHumanPlayer[m_playerId[COMBAT_ATTACKER_SIDE]] != 0
                || m_playerId[COMBAT_DEFENDER_SIDE] == 0))) {
        gbThisNetHasControl = true;
    } else {
        if (m_playerId[m_currentSide] != -1 && gbHumanPlayer[m_playerId[m_currentSide]] != 0
            && gbThisNetHumanPlayer[m_playerId[m_currentSide]] == 0)
            gbThisNetHasControl = false;
        else
            gbThisNetHasControl = true;
    }
    m_smallViewSide[COMBAT_DEFENDER_SIDE] = -1;
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
    if (gbThisNetHasControl != 0 && m_playerId[m_currentSide] >= 0
        && gbHumanPlayer[m_playerId[m_currentSide]] != 0) {
        m_selectedHex = INVALID_HEX;
        ClearCombatMessages(0);
        gpMouseManager->MouseCoords(mouseX_f, mouseY_g);
        message.type = MESSAGE_MOUSE_MOVE;
        message.payload.mouse.x = message.payload.mouse.screenX = mouseX_f;
        message.payload.mouse.y = message.payload.mouse.screenY = mouseY_g;
        ProcessCombatMsg(message);
    } else {
        gpMouseManager->SetPointer(POINTER_DEFAULT);
    }
}

// @semantic: first residual is +0x18; first normalized residual is instruction 33.
VA(0x00430536, 0x65b)
i32 combatManager::ProcessNextAction(struct tag_message& message) {
    i32 actionData[IDX(ACTION_DATA_COUNT)];
    i32 transmitResult;
    army* currentArmy;
    i32 advanceArmy;
    i32 redraw;
    CombatMainResult result;

    ClearCombatMessages(0);
    result = MAIN_CONTINUE;
    redraw = 0;
    gbProcessingCombatAction = true;
    if (m_smallViewSide[COMBAT_ATTACKER_SIDE] != -1
        || m_smallViewSide[COMBAT_DEFENDER_SIDE] != -1) {
        m_smallViewSide[COMBAT_DEFENDER_SIDE] = -1;
        m_smallViewSide[COMBAT_ATTACKER_SIDE] = m_smallViewSide[COMBAT_DEFENDER_SIDE];
        redraw = 1;
    }
    if (giNextAction != IDX(ACTION_NONE)) {
        LogInt(
            "Process Act",
            giNextAction,
            giNextActionGridIndex,
            giNextActionGridIndex2,
            giNextActionExtra,
            m_currentArmySide,
            m_currentArmyIndex,
            m_armies[m_currentArmySide][m_currentArmyIndex].m_hex
        );
    }
    gpMouseManager->SetPointer(POINTER_DEFAULT);
    UpdateMouseGrid(-1, 1);
    memset(m_gridState, 0, sizeof(m_gridState));
    if (UpdateGrid(0, 0) != 0)
        redraw = 1;
    if (redraw != 0)
        DrawFrame(1, 0, 0, 0, COMMAND_FRAME_DELAY, 1, 1);

    if (gbThisNetHasControl != 0 && gbRemoteOn != 0 && m_playerId[COMBAT_ATTACKER_SIDE] >= 0
        && m_playerId[COMBAT_DEFENDER_SIDE] >= 0
        && gbHumanPlayer[m_playerId[COMBAT_DEFENDER_SIDE]] != 0
        && gbHumanPlayer[m_playerId[COMBAT_ATTACKER_SIDE]] != 0) {
        actionData[IDX(ACTION_DATA_ACTION)] = giNextAction;
        actionData[IDX(ACTION_DATA_EXTRA)] = giNextActionExtra;
        actionData[IDX(ACTION_DATA_GRID)] = giNextActionGridIndex;
        actionData[IDX(ACTION_DATA_SECOND_GRID)] = giNextActionGridIndex2;
        LogInt(
            "About to T",
            iCombatControlNetPos[COMBAT_DEFENDER_SIDE - m_currentSide],
            -999,
            -999,
            -999,
            -999,
            -999,
            -999
        );
        transmitResult = TransmitRemoteData(
            reinterpret_cast<char*>(actionData),
            iCombatControlNetPos[COMBAT_DEFENDER_SIDE - m_currentSide],
            sizeof(actionData),
            REMOTE_COMMAND_ACTION,
            1,
            1,
            -1
        );
        LogStr("Post T");
        if (transmitResult == 0)
            ShutDown(NULL);
    }

    currentArmy = &m_armies[m_currentArmySide][m_currentArmyIndex];
    advanceArmy = 0;
    if (CheckWin(&message) == 0) {
        switch (giNextAction) {
            case IDX(ACTION_NONE):
                break;
            case IDX(ACTION_CAST_SPELL):
                ResetCyclingCreatures();
                CastSpell(
                    SpellType(giNextActionExtra),
                    giNextActionGridIndex,
                    0,
                    giNextActionGridIndex2
                );
                if (m_armies[m_currentArmySide][m_currentArmyIndex].m_quantity < 1)
                    advanceArmy = 1;
                ResetCycleTimers();
                break;
            case IDX(ACTION_MOVE):
                ResetCyclingCreatures();
                currentArmy->MoveAttack(giNextActionGridIndex, 0);
                currentArmy->m_monster.flags.abilityFlags |= MONSTER_ABILITY_FLAG_BAD_MORALE;
                if (CheckWin(&message) != 0) {
                    result = MAIN_FINISHED;
                    goto Finished;
                }
                CheckApplyGoodMorale(m_currentArmySide, m_currentArmyIndex);
                advanceArmy = 1;
                ResetCycleTimers();
                break;
            case IDX(ACTION_ATTACK):
                ResetCyclingCreatures();
                if (giNextActionExtra != -1 && currentArmy->m_hex != giNextActionExtra) {
                    currentArmy->MoveAttack(giNextActionExtra, 1);
                }
                currentArmy->MoveAttack(giNextActionGridIndex, 0);
                currentArmy->m_monster.flags.abilityFlags |= MONSTER_ABILITY_FLAG_BAD_MORALE;
                if (CheckWin(&message) != 0) {
                    result = MAIN_FINISHED;
                    goto Finished;
                }
                CheckApplyGoodMorale(m_currentArmySide, m_currentArmyIndex);
                advanceArmy = 1;
                ResetCycleTimers();
                break;
            case IDX(ACTION_RETREAT):
                m_sideRetreated[m_currentSide] = 1;
                gbRetreatWin = true;
                ResetCycleTimers();
                break;
            case IDX(ACTION_SURRENDER):
                gbCombatSurrender = true;
                gbRetreatWin = true;
                m_sideDefeated[m_currentSide] = 1;
                gpGame->m_players[m_playerId[m_currentSide]].m_resources[IDX(RES_GOLD)] -=
                    giNextActionExtra;
                gpGame->m_players[m_playerId[COMBAT_DEFENDER_SIDE - m_currentSide]]
                    .m_resources[IDX(RES_GOLD)] += giNextActionExtra;
                ResetCycleTimers();
                break;
            case IDX(ACTION_WAIT):
                currentArmy->m_monster.flags.abilityFlags |= MONSTER_ABILITY_FLAG_BAD_MORALE;
                advanceArmy = 1;
                break;
            case IDX(ACTION_DEFEND):
                currentArmy->m_monster.flags.abilityFlags |= MONSTER_ABILITY_FLAG_DEFERRED_TURN;
                advanceArmy = 1;
                break;
        }
        giNextAction = IDX(ACTION_NONE);
        if (CheckWin(&message) == 0) {
            TestRaiseDoor();
            if (advanceArmy != 0 && GetNextArmy(1) == 0) {
                ResetRound();
                GetNextArmy(1);
            }
            CheckChangeSelector();
        } else {
            result = MAIN_FINISHED;
        }
    }

Finished:
    gbProcessingCombatAction = false;
    ResetMouse();
    return IDX(result);
}

// @early-stop: delinker jump-table artifact.
VA(0x00430b91, 0x237)
void combatManager::ResetCyclingCreatures(void) {
    army* currentArmy_p = NULL;
    i32 cyclingCount = 0;
    i32 side;
    i32 index;
    i32 unusedCyclingWord6;

    for (side = 0; side < COMBAT_MANAGER_SIDE_COUNT; ++side) {
        for (index = 0; index < gpCombatManager->m_armyCount[side]; ++index) {
            currentArmy_p = gpCombatManager->m_armies[side] + index;
            if (HAS(currentArmy_p->m_monster.flags.abilityFlags, MONSTER_ABILITY_FLAG_AI_EXCLUDED)
                    == 0
                && currentArmy_p->m_animationSequence >= COMBAT_CREATURE_CYCLE_SEQUENCE_FIRST
                && currentArmy_p->m_animationSequence <= COMBAT_CREATURE_CYCLE_SEQUENCE_LAST) {
                ++cyclingCount;
                ++gpCombatManager->m_limitCreatureCount[side][index];
            }
        }
    }
    if (cyclingCount == 0) {
    } else {
        gpCombatManager->DrawFrame(0, 1, 1, 1, COMMAND_FRAME_DELAY, 1, 1);
        for (side = 0; side < COMBAT_MANAGER_SIDE_COUNT; ++side) {
            for (index = 0; index < gpCombatManager->m_armyCount[side]; ++index) {
                currentArmy_p = gpCombatManager->m_armies[side] + index;
                if (HAS(currentArmy_p->m_monster.flags.abilityFlags,
                        MONSTER_ABILITY_FLAG_AI_EXCLUDED)
                    == 0) {
                    currentArmy_p = gpCombatManager->m_armies[side] + index;
                    currentArmy_p->m_animationSequence = ARMY_ANIMATION_STAND;
                    currentArmy_p->m_animationFrame = 0;
                    currentArmy_p->m_lastAnimationTime = KBTickCount();
                }
            }
        }
        m_heroCycleTimer[COMBAT_ATTACKER_SIDE] = KBTickCount();
        m_heroCycleTimer[COMBAT_DEFENDER_SIDE] = KBTickCount();
        gpCombatManager->DrawFrame(1, 1, 0, 0, COMMAND_FRAME_DELAY, 1, 1);
    }
}

VA(0x00430dc8, 0xf9)
void combatManager::ResetCycleTimers(void) {
    i32l currentTime_r = KBTickCount();
    i32 side;
    i32 index;
    army* currentArmy_p;

    m_heroCycleTimer[COMBAT_ATTACKER_SIDE] = KBTickCount();
    m_heroCycleTimer[COMBAT_DEFENDER_SIDE] = KBTickCount();
    for (side = 0; side < COMBAT_MANAGER_SIDE_COUNT; ++side) {
        for (index = 0; index < gpCombatManager->m_armyCount[side]; ++index) {
            currentArmy_p = &gpCombatManager->m_armies[side][index];
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

// @early-stop: inline continuation artifact.
VA(0x00430ec1, 0x53)
i32 InCombatArea(i32 x, i32 y) {
    if (x >= 0 && x < COMBAT_SCREEN_WIDTH && y >= 0 && y < COMBAT_AREA_HEIGHT)
        return 1;
    return 0;
}

// @semantic: first residual is +0x16; normalized instruction 66 first reverses side/ army strength reduction.
VA(0x00430f14, 0x9d9)
void combatManager::CycleCombatScreen(void) {
    army* currentArmy;
    u8 cycleArmy[2][20];
    i32 nextHeroAnimation[3];
    i32 side;
    i32 index;
    i32 animationIndex;
    float roll;
    float accumulatedChance;

    CheckUpdateCombatMessages();
    currentArmy = NULL;
    nextHeroAnimation[COMBAT_MANAGER_SIDE_COUNT] = 0;
    gpCombatManager->ResetLimitCreature();
    for (side = 0; side < COMBAT_MANAGER_SIDE_COUNT; ++side) {
        if (m_heroOverlayIcons[side] == NULL) {
        } else {
            if (m_heroes[side] != NULL)
                m_heroOverlayFrame[side] = (m_heroOverlayFrame[side] + 1) % 5;
            ++m_drawHeroOverlay[side];
        }
    }

    memset(cycleArmy, 0, sizeof(cycleArmy));
    for (side = 0; side < COMBAT_MANAGER_SIDE_COUNT; ++side) {
        for (index = 0; index < gpCombatManager->m_armyCount[side]; ++index) {
            currentArmy = gpCombatManager->m_armies[side] + index;
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
                ++nextHeroAnimation[COMBAT_MANAGER_SIDE_COUNT];
                ++cycleArmy[side][index];
                ++m_limitCreatureCount[side][index];
            }
        }
    }

    for (side = 0; side < COMBAT_MANAGER_SIDE_COUNT; ++side) {
        nextHeroAnimation[side] = -1;
        if (m_heroIcons[side] == NULL) {
        } else {
            if (m_heroAnimationState[side] == HERO_ANIMATION_DEATH_FIRST
                || m_heroAnimationState[side] == HERO_ANIMATION_DEATH_SECOND
                || m_heroAnimationState[side] == HERO_ANIMATION_IDLE_FIRST
                || m_heroAnimationState[side] == HERO_ANIMATION_IDLE_SECOND
                || m_heroAnimationState[side] == HERO_ANIMATION_IDLE_LAST) {
                m_drawHero[side] = 1;
            } else if (m_heroAnimationState[side] == HERO_ANIMATION_STAND
                       && m_heroDeathAnimationPlayed[side] == 0 && m_heroDeathPending[side] != 0) {
                if (m_playerId[m_currentSide] == -1
                    || gbThisNetHumanPlayer[m_playerId[m_currentSide]] == 0) {
                    m_heroAlternateDeathPending[side] = 0;
                    m_heroDeathPending[side] = m_heroAlternateDeathPending[side];
                } else {
                    m_heroAlternateDeathPending[side] = 0;
                    m_heroDeathPending[side] = m_heroAlternateDeathPending[side];
                    m_heroDeathAnimationPlayed[side] = 1;
                    if (sCmbtHero[m_heroSpriteIndex[side]]
                            .animationFrameCount[HERO_ANIMATION_DEATH_FIRST]
                        > 0) {
                        nextHeroAnimation[side] = HERO_ANIMATION_DEATH_FIRST;
                        m_drawHero[side] = 1;
                    }
                }
            } else if (m_heroAnimationState[side] == HERO_ANIMATION_STAND
                       && m_heroAlternateDeathAnimationPlayed[side] == 0
                       && m_heroAlternateDeathPending[side] != 0) {
                if (m_playerId[m_currentSide] == -1
                    || gbThisNetHumanPlayer[m_playerId[m_currentSide]] == 0) {
                    m_heroAlternateDeathPending[side] = 0;
                    m_heroDeathPending[side] = m_heroAlternateDeathPending[side];
                } else {
                    m_heroAlternateDeathPending[side] = 0;
                    m_heroDeathPending[side] = m_heroAlternateDeathPending[side];
                    m_heroAlternateDeathAnimationPlayed[side] = 1;
                    if (sCmbtHero[m_heroSpriteIndex[side]]
                            .animationFrameCount[HERO_ANIMATION_DEATH_SECOND]
                        > 0) {
                        nextHeroAnimation[side] = HERO_ANIMATION_DEATH_SECOND;
                        m_drawHero[side] = 1;
                    }
                }
            } else if (m_heroAnimationState[side] == HERO_ANIMATION_STAND
                       && m_heroCycleTimer[side] + HERO_IDLE_DELAY < KBTickCount()) {
                if (static_cast<u8>(sCmbtHero[m_heroSpriteIndex[side]].idleAnimationCount) > 1) {
                    nextHeroAnimation[side] =
                        Random(
                            0,
                            static_cast<u8>(sCmbtHero[m_heroSpriteIndex[side]].idleAnimationCount)
                                - 1
                        )
                        + HERO_ANIMATION_IDLE_FIRST;
                } else {
                    nextHeroAnimation[side] = HERO_ANIMATION_IDLE_FIRST;
                }
                m_drawHero[side] = 1;
            }
        }
    }
    if (m_heroIcons[COMBAT_ATTACKER_SIDE] != NULL)
        m_drawHero[COMBAT_ATTACKER_SIDE] = 1;
    if (m_heroIcons[COMBAT_DEFENDER_SIDE] != NULL)
        m_drawHero[COMBAT_DEFENDER_SIDE] = 1;

    if (nextHeroAnimation[COMBAT_MANAGER_SIDE_COUNT] != 0 || m_drawHero[COMBAT_ATTACKER_SIDE] != 0
        || m_drawHero[COMBAT_DEFENDER_SIDE] != 0 || m_drawHeroOverlay[COMBAT_ATTACKER_SIDE] != 0
        || m_drawHeroOverlay[COMBAT_DEFENDER_SIDE] != 0) {
        gpCombatManager->DrawFrame(0, 1, 1, 1, COMMAND_FRAME_DELAY, 1, 1);
        for (side = 0; side < COMBAT_MANAGER_SIDE_COUNT; ++side) {
            for (index = 0; index < gpCombatManager->m_armyCount[side]; ++index) {
                currentArmy = gpCombatManager->m_armies[side] + index;
                if (cycleArmy[side][index] != 0) {
                    if (currentArmy->m_animationSequence == ARMY_ANIMATION_STAND) {
                        roll =
                            static_cast<float>(Random(IDLE_ROLL_MIN, IDLE_ROLL_MAX))
                            / COMBAT_IDLE_ROLL_DIVISOR;
                        accumulatedChance = 0.0f;
                        currentArmy->m_standingAnimation =
                            currentArmy->m_frameInfo.standingAnimationCount - 1;
                        for (animationIndex = 0;
                             animationIndex < currentArmy->m_frameInfo.standingAnimationCount - 1;
                             ++animationIndex) {
                            accumulatedChance +=
                                currentArmy->m_frameInfo.standingAnimationChances[animationIndex];
                            if (accumulatedChance > roll) {
                                currentArmy->m_standingAnimation = animationIndex;
                                animationIndex = 99;
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
        for (side = 0; side < COMBAT_MANAGER_SIDE_COUNT; ++side) {
            if (m_drawHero[side] != 0) {
                if (nextHeroAnimation[side] != -1) {
                    m_heroAnimationState[side] = nextHeroAnimation[side];
                    m_heroAnimationFrame[side] = 0;
                } else {
                    ++m_heroAnimationFrame[side];
                    if (m_heroAnimationFrame[side]
                        >= sCmbtHero[m_heroSpriteIndex[side]]
                               .animationFrameCount[m_heroAnimationState[side]]) {
                        m_heroAnimationState[side] = HERO_ANIMATION_STAND;
                        m_heroAnimationFrame[side] = 0;
                        m_heroCycleTimer[side] = KBTickCount();
                    }
                }
            }
        }
        DrawFrame(1, 1, 0, 0, COMMAND_FRAME_DELAY, 1, 1);
    }
    glTimers[GLOBAL_COMBAT_CYCLE_TIMER_SLOT] = static_cast<i32>(
        KBTickCount() + gfCombatSpeedMod[gConfig.combatSpeed] * COMBAT_CYCLE_TIMER_FACTOR
    );
}

// @early-stop: byte-proven compiler artifact.
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
    SetupGridForArmy(&m_armies[OD_STEER(m_currentArmySide)][m_currentArmyIndex]);
    DrawFrame(1, 0, 0, 0, COMMAND_FRAME_DELAY, 1, 1);
    ResetMouse();
    WritePrefs();
}

// @semantic: first normalized residual is side/index strength-reduction order in the initial army lookup.
VA(0x00431a0b, 0x3ab)
void combatManager::AddArmy(
    i32 side,
    i32 monsterType,
    i32 quantity,
    i32 hex,
    i32 flags,
    i32 animate
) {
    i32 armyIndex_r = INVALID_ARMY_INDEX;
    i32 reusedArmy_m = 0;
    i32 index_g;
    army* newArmy;
    for (index_g = 0; index_g < COMBAT_ARMY_CAPACITY; ++index_g) {
        if (m_armies[side][index_g].m_monsterType == CREATURE_NONE) {
            armyIndex_r = index_g;
            break;
        }
        if (m_armies[side][index_g].m_quantity == 0
            && HAS(m_armies[side][index_g].m_monster.flags.all, MONSTER_FLAGS_AI_EXCLUDED) != 0
            && (HAS(m_armies[side][index_g].m_monster.flags.all, MONSTER_FLAGS_MIRROR_IMAGE) != 0
                || m_armies[side][index_g].m_monsterType == CREATURE_EARTH_ELEMENTAL
                || m_armies[side][index_g].m_monsterType == CREATURE_AIR_ELEMENTAL
                || m_armies[side][index_g].m_monsterType == CREATURE_FIRE_ELEMENTAL
                || m_armies[side][index_g].m_monsterType == CREATURE_WATER_ELEMENTAL)) {
            armyIndex_r = index_g;
            reusedArmy_m = 1;
            break;
        }
    }

    if (armyIndex_r != INVALID_ARMY_INDEX) {
        if (m_hexCells[hex].m_occupantSide != INVALID_HEX)
            return;

        newArmy = &m_armies[side][armyIndex_r];
        newArmy
            ->Init(CreatureType(monsterType), quantity, side, armyIndex_r, hex, INVALID_HEX);
        newArmy->LoadResources();
        newArmy->m_monster.flags.all |= MonsterFlags(flags);
        if (reusedArmy_m == 0)
            ++m_armyCount[side];

        if (animate == 0)
            return;

        ResetLimitCreature();
        m_limitCreatureCount[side][armyIndex_r] += 1;
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
}

VA(0x00431db6, 0x169)
void combatManager::SetupSmallView(void) {
    i32 unusedSmallViewWord1;
    i32 unusedSmallViewWord2;

    if (m_smallViewSide[COMBAT_DEFENDER_SIDE] == SMALL_VIEW_SIDE_NONE
        && m_smallViewLastX[COMBAT_DEFENDER_SIDE] != SMALL_VIEW_POSITION_NONE) {
        DrawSmallView(COMBAT_DEFENDER_SIDE, 1);
    }

    if (gbThisNetHasControl != 0 && m_playerId[m_currentSide] != PLAYER_NONE
        && gbHumanPlayer[m_playerId[m_currentSide]] != 0) {
        if (m_smallViewSide[COMBAT_ATTACKER_SIDE] != m_currentSide
            || m_smallViewArmyIndex[COMBAT_ATTACKER_SIDE] != m_currentArmyIndex) {
            if (m_smallViewSide[COMBAT_ATTACKER_SIDE] != SMALL_VIEW_SIDE_NONE
                && m_smallViewSide[COMBAT_ATTACKER_SIDE] != m_currentSide
                && m_smallViewLastX[COMBAT_ATTACKER_SIDE] != SMALL_VIEW_POSITION_NONE) {
                m_smallViewSide[COMBAT_ATTACKER_SIDE] = SMALL_VIEW_SIDE_NONE;
                DrawSmallView(COMBAT_ATTACKER_SIDE, 1);
            }
            m_smallViewSide[COMBAT_ATTACKER_SIDE] = m_currentSide;
            m_smallViewArmyIndex[COMBAT_ATTACKER_SIDE] = m_currentArmyIndex;
            DrawSmallView(COMBAT_ATTACKER_SIDE, 1);
        }
    } else {
        m_smallViewSide[COMBAT_ATTACKER_SIDE] = SMALL_VIEW_SIDE_NONE;
        if (m_smallViewLastX[COMBAT_ATTACKER_SIDE] != SMALL_VIEW_POSITION_NONE)
            DrawSmallView(COMBAT_ATTACKER_SIDE, 1);
    }
}

// @early-stop: byte-proven compiler artifact.
VA(0x00431f1f, 0x304)
void combatManager::ViewBallista(i32 quickView) {
    i32 archerCount;
    i32 attackBonus;
    char description[300];

    m_combatTowns[COMBAT_DEFENDER_SIDE]->CalcNumLevelArchers(&archerCount, &attackBonus);
    sprintf(gText, "Ballista");
    strcpy(description, "");
    if (m_wallStates[IDX(COMBAT_WALL_SLOT_KEEP)] != IDX(COMBAT_WALL_STATE_KEEP_STANDING)) {
        sprintf(description, "\n\nThe %s is destroyed.", "Ballista");
    } else if (attackBonus > 0) {
        sprintf(
            description,
            "\n\nThe %s fires with the strength of %d Archers, each "
            "with a +%d bonus to their attack skill.",
            "Ballista",
            archerCount,
            attackBonus
        );
    } else {
        sprintf(
            description,
            "\n\nThe %s fires with the strength of %d Archers.",
            "Ballista",
            archerCount
        );
    }
    strcat(gText, description);

    strcpy(description, "");
    if ((m_combatTowns[COMBAT_DEFENDER_SIDE]->m_buildings & IDX(TOWN_BUILDING_LEFT_TURRET)) != 0) {
        if (m_wallStates[IDX(COMBAT_WALL_SLOT_TOP_TOWER)] == IDX(COMBAT_WALL_STATE_DESTROYED)) {
            sprintf(description, "\n\nThe %s is destroyed.", "Left Turret");
        } else if (m_wallStates[IDX(COMBAT_WALL_SLOT_TOP_TOWER)]
                   == IDX(COMBAT_WALL_STATE_TOWER_STANDING)) {
            if (attackBonus > 0) {
                sprintf(
                    description,
                    "\n\nThe %s fires with the strength of %d Archers, "
                    "each with a +%d bonus to their attack skill.",
                    "Left Turret",
                    archerCount / COMBAT_KEEP_SIDE_TOWER_SHOT_DIVISOR,
                    attackBonus
                );
            } else {
                sprintf(
                    description,
                    "\n\nThe %s fires with the strength of %d Archers.",
                    "Left Turret",
                    archerCount / COMBAT_KEEP_SIDE_TOWER_SHOT_DIVISOR
                );
            }
        }
        strcat(gText, description);
    }

    if ((m_combatTowns[COMBAT_DEFENDER_SIDE]->m_buildings & IDX(TOWN_BUILDING_RIGHT_TURRET)) != 0) {
        strcpy(description, "");
        if (m_wallStates[IDX(COMBAT_WALL_SLOT_BOTTOM_TOWER)] == IDX(COMBAT_WALL_STATE_DESTROYED)) {
            sprintf(description, "\n\nThe %s is destroyed.", "Right Turret");
        } else if (m_wallStates[IDX(COMBAT_WALL_SLOT_BOTTOM_TOWER)]
                   == IDX(COMBAT_WALL_STATE_TOWER_STANDING)) {
            if (attackBonus > 0) {
                sprintf(
                    description,
                    "\n\nThe %s fires with the strength of %d Archers, "
                    "each with a +%d bonus to their attack skill.",
                    "Right Turret",
                    archerCount / COMBAT_KEEP_SIDE_TOWER_SHOT_DIVISOR,
                    attackBonus
                );
            } else {
                sprintf(
                    description,
                    "\n\nThe %s fires with the strength of %d Archers.",
                    "Right Turret",
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
DATA(0x005250c0) i8 iTransferArtifactsInfo[16];
DATA(0x005250d0) b32 gbWhichAnimationPlaying;
DATA(0x005250d4) i32 iMaxTransferArtifacts;
DATA(0x005250d8) i32 giNextActionExtra;
DATA(0x005250dc) i32 bSkeletonsShown;
DATA(0x005250e0) b32 gbShowingLoseWindow;
DATA(0x005250e4) i32 giWinCmbtFrame;
DATA(0x005250e8) i32 giNextActionGridIndex;
DATA(0x005250ec) i32 giSurrenderCost;
DATA(0x005250f0) i32 giSkeletonsCreated;
DATA(0x005250f8) i8 iTransferArtifacts[16];
DATA(0x00525108) i32 giNextAction;
DATA(0x0052510c) i32 giNextActionGridIndex2;
