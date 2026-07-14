// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\COMMAND.OBJ   from: (directly linked into exe)
// functions: 36   data: 15
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <stdlib.h>
#include <string.h>
#include <_globals_model.h>
#include <BASE/heroWindowManager.h>
#include <BASE/inputManager.h>
#include <BASE/mouseManager.h>
#include <SOURCE/advManager.h>
#include <SOURCE/combatManager.h>
#include <SOURCE/COMMAND.h>
#include <SOURCE/KB.h>
#include <SOURCE/kbwin.h>
#include <SOURCE/PATH.h>
#include <SOURCE/REMOTE.h>
#include <SOURCE/X_GLOBAL.h>
VA(0x0042a6d0, 0x36d)
int combatManager::Main(tag_message &message)
{
    int result = COMBAT_MAIN_CONTINUE;

    if (gbNoShowCombat == 0) {
        if (KBTickCount() > glTimers[0]) {
            PollSound();
            glTimers[0] = static_cast<int>(
                KBTickCount() +
                gfCombatSpeedMod[gConfig.combatSpeed] * COMBAT_SOUND_POLL_DELAY);
        }
        if (KBTickCount() > gCombatCycleTimer &&
            gbProcessingCombatAction == 0) {
            gbProcessingCombatAction = 1;
            CycleCombatScreen();
            gbProcessingCombatAction = 0;
        }
    }

    CheckCastleAttack();
    if (CheckWin(&message) != 0)
        return COMBAT_MAIN_FINISHED;

    if (gbNoShowCombat == 0) {
        CombatRemotePacket *packet = reinterpret_cast<CombatRemotePacket *>(
            GetRemoteData(COMBAT_REMOTE_PACKET_TYPE));
        if (packet != 0 && packet->category == COMBAT_REMOTE_CATEGORY_ACTION) {
            switch (packet->command) {
            case COMBAT_REMOTE_COMMAND_ACTION:
                giNextAction = packet->nextAction;
                giNextActionExtra = packet->nextActionExtra;
                giNextActionGridIndex = packet->nextActionGridIndex;
                giNextActionGridIndex2 = packet->nextActionGridIndex2;
                goto ProcessAction;
            case COMBAT_REMOTE_COMMAND_MESSAGE:
                PopNetBox(reinterpret_cast<char *>(&packet->nextAction),
                          packet->messageLength);
                break;
            }
        }

        if (gbThisNetHasControl == 0) {
            if (message.type == COMBAT_EVENT_KEY) {
                switch (message.field4) {
                case COMBAT_KEY_CLOSE_NETWORK_BOX:
                    PopNetBox(0, -1);
                    break;
                }
            }
            return COMBAT_MAIN_CONTINUE;
        }
    }

    {
        army *currentArmy =
            &m_armies[m_currentArmySide][m_currentArmyIndex];
        if (currentArmy->m_spellInfluence[ARMY_SPELL_INFLUENCE_BERSERK] != 0) {
            currentArmy->GoBerserk();
            if (CheckWin(&message) != 0)
                return COMBAT_MAIN_FINISHED;
        }
    }

    if (gbNoShowCombat == 0) {
        if (m_gridSelectionDisabled != 0) {
            while (message.type != COMBAT_EVENT_KEY &&
                   message.type != COMBAT_EVENT_MOUSE_BUTTON &&
                   message.type != COMBAT_EVENT_MOUSE_OTHER &&
                   message.type != COMBAT_EVENT_NONE) {
                message = gpInputManager->GetEvent();
            }
            if (message.type != COMBAT_EVENT_NONE)
                m_gridSelectionDisabled = 0;
        }
        CheckChangeSelector();
    }

ProcessAction:
    if (giNextAction == 0) {
        if (m_playerId[m_currentSide] == -1 ||
            gbThisNetHumanPlayer[m_playerId[m_currentSide]] == 0 ||
            m_gridSelectionDisabled != 0) {
            CheckGetAIMove();
        } else {
            result = ProcessCombatMsg(message);
        }
    }
    if (giNextAction != 0)
        result = ProcessNextAction(message);
    return result;
}

// @match-note retained max 94.35% (combined live 86.15%): current code span is
// 0x17c versus retail 0x181; frame
// 0x4 and all 6/6 relocations agree. The first normalized residual is instruction
// 93, where the final combined occupant predicate retains opposite CFG polarity.
// Both comparison operand orders were tried before the current prefix aligned.
VA(0x0042aa3d, 0x181)
int combatManager::ValidHexToStandOn(int hexIndex)
{
    if (hexIndex == COMBAT_IGNORED_HEX)
        return 1;

    if (hexIndex == COMBAT_INVALID_HEX ||
        MAP_WIDTH - 1 == hexIndex % COMBAT_GRID_ROW_LENGTH ||
        hexIndex % COMBAT_GRID_ROW_LENGTH == 0 ||
        (m_hexCells[hexIndex].m_blocked != 0 &&
         (gpCombatManager->m_inCastleCombat == 0 ||
          (hexIndex != COMBAT_CASTLE_GATE_APPROACH_HEX &&
           hexIndex != COMBAT_CASTLE_GATE_HEX) ||
          (gpCombatManager->m_drawbridgeState == COMBAT_CASTLE_GATE_OPEN &&
           (gpCombatManager->m_currentSide != COMBAT_DEFENDER_SIDE ||
            gpCombatManager->m_hexCells[COMBAT_CASTLE_GATE_APPROACH_HEX]
                    .m_occupantSide != -1 ||
            gpCombatManager->m_hexCells[COMBAT_CASTLE_GATE_APPROACH_HEX]
                    .m_deadOccupantCount != 0)))) ||
        (m_hexCells[hexIndex].m_occupantSide != -1 &&
         (m_hexCells[hexIndex].m_occupantSide != m_currentArmySide ||
          m_hexCells[hexIndex].m_occupantIndex != m_currentArmyIndex))) {
        return 0;
    }
    return 1;
}

// @match-note 86.77%: code size 0x8a1, frame 0x7c, and all 5/5 relocations
// agree. The first residual is instruction 13 in current-army address strength
// reduction, repeated for targetArmy at instruction 42; later residuals are
// condition layout in the direction/rear loops. Emitting the wide special arm
// first fixed the earlier CFG wall.
VA(0x0042abbe, 0x8a1)
void combatManager::SetCombatDirections(int targetHex)
{
    if (m_gridSelectionDisabled != 0)
        return;

    char standable[COMBAT_DIRECTION_COUNT];
    int directionHexes[COMBAT_DIRECTION_COUNT];
    int rearHexes[COMBAT_DIRECTION_COUNT];
    char pathValid[COMBAT_DIRECTION_COUNT];
    army *currentArmy = &m_armies[m_currentArmySide][m_currentArmyIndex];
    int targetSide = currentArmy->m_targetSide;
    int targetIndex = currentArmy->m_targetIndex;
    currentArmy->m_targetSide = -1;
    currentArmy->m_targetIndex = -1;
    army *targetArmy = &m_armies[targetSide][targetIndex];

    int direction;
    for (direction = 0; direction < COMBAT_DIRECTION_COUNT; direction++) {
        if (direction == COMBAT_DIRECTION_SPECIAL_FIRST ||
            direction == COMBAT_DIRECTION_SPECIAL_SECOND) {
            if ((currentArmy->m_monster.flags.all & MONSTER_FLAGS_WIDE) != 0) {
                if (currentArmy->m_facing == 1) {
                    if (direction == COMBAT_DIRECTION_SPECIAL_FIRST)
                        directionHexes[direction] = m_adjacency[targetHex][5];
                    if (direction == COMBAT_DIRECTION_SPECIAL_SECOND)
                        directionHexes[direction] = m_adjacency[targetHex][3];
                } else {
                    if (direction == COMBAT_DIRECTION_SPECIAL_FIRST)
                        directionHexes[direction] = m_adjacency[targetHex][0];
                    if (direction == COMBAT_DIRECTION_SPECIAL_SECOND)
                        directionHexes[direction] = m_adjacency[targetHex][2];
                }
            } else {
                directionHexes[direction] = COMBAT_INVALID_HEX;
            }
        } else {
            directionHexes[direction] = m_adjacency[targetHex][direction];
        }

        if ((currentArmy->m_monster.flags.all & MONSTER_FLAGS_WIDE) == 0 ||
            directionHexes[direction] == COMBAT_INVALID_HEX) {
            rearHexes[direction] = COMBAT_IGNORED_HEX;
        } else if (currentArmy->m_facing == 1) {
            if (direction == 5 || direction == 4 || direction == 3) {
                if (directionHexes[direction] % COMBAT_GRID_ROW_LENGTH ==
                    COMBAT_GRID_FIRST_COLUMN)
                    directionHexes[direction] = COMBAT_INVALID_HEX;
                else
                    directionHexes[direction]--;
            }
            if (directionHexes[direction] % COMBAT_GRID_ROW_LENGTH ==
                COMBAT_GRID_REVERSE_FIRST_COLUMN)
                rearHexes[direction] = COMBAT_INVALID_HEX;
            else
                rearHexes[direction] = directionHexes[direction] + 1;
        } else {
            if (direction == 0 || direction == 1 || direction == 2) {
                if (directionHexes[direction] % COMBAT_GRID_ROW_LENGTH ==
                    COMBAT_GRID_REVERSE_FIRST_COLUMN)
                    directionHexes[direction] = COMBAT_INVALID_HEX;
                else
                    directionHexes[direction]++;
            }
            if (directionHexes[direction] % COMBAT_GRID_ROW_LENGTH ==
                COMBAT_GRID_FIRST_COLUMN)
                rearHexes[direction] = COMBAT_INVALID_HEX;
            else
                rearHexes[direction] = directionHexes[direction] - 1;
        }

        if (ValidHexToStandOn(directionHexes[direction]) == 0 ||
            ValidHexToStandOn(rearHexes[direction]) == 0)
            standable[direction] = 0;
        else
            standable[direction] = 1;
    }

    if ((currentArmy->m_monster.flags.all & MONSTER_FLAGS_FLYING) == 0) {
        for (direction = 0; direction < COMBAT_DIRECTION_COUNT; direction++) {
            if (standable[direction] == 0) {
                pathValid[direction] = 0;
            } else if (directionHexes[direction] == currentArmy->m_hex ||
                       currentArmy->ValidPath(directionHexes[direction], 1) !=
                           0) {
                pathValid[direction] = 1;
            } else {
                pathValid[direction] = 0;
            }
        }
    } else {
        for (direction = 0; direction < COMBAT_DIRECTION_COUNT; direction++)
            pathValid[direction] = standable[direction];
    }

    m_validDirectionCount = 0;
    for (direction = 0; direction < COMBAT_DIRECTION_COUNT; direction++) {
        if (pathValid[direction] != 0)
            m_validDirectionCount++;
    }
    if (m_validDirectionCount == 0)
        pathValid[COMBAT_DIRECTION_SPECIAL_FIRST] = 1;

    memset(m_directionMap, -1, sizeof(m_directionMap));
    for (direction = 0; direction < COMBAT_DIRECTION_COUNT; direction++) {
        int outputDirection = direction;
        int mappedDirection;
        if (direction < COMBAT_ADJACENT_DIRECTION_COUNT)
            mappedDirection = (direction + 3) % COMBAT_ADJACENT_DIRECTION_COUNT;
        else if (direction == COMBAT_DIRECTION_SPECIAL_FIRST)
            mappedDirection = COMBAT_DIRECTION_SPECIAL_SECOND;
        else
            mappedDirection = COMBAT_DIRECTION_SPECIAL_FIRST;

        if (pathValid[mappedDirection] != 0) {
            if ((targetArmy->m_monster.flags.all & MONSTER_FLAGS_WIDE) != 0) {
                if (direction == 0 &&
                    m_hexCells[targetHex - 1].m_occupantSide == targetSide &&
                    m_hexCells[targetHex - 1].m_occupantIndex == targetIndex) {
                    outputDirection = COMBAT_DIRECTION_SPECIAL_FIRST;
                } else if (direction == 5 &&
                           m_hexCells[targetHex + 1].m_occupantSide == targetSide &&
                           m_hexCells[targetHex + 1].m_occupantIndex == targetIndex) {
                    outputDirection = COMBAT_DIRECTION_SPECIAL_FIRST;
                } else if (direction == 2 &&
                           m_hexCells[targetHex - 1].m_occupantSide == targetSide &&
                           m_hexCells[targetHex - 1].m_occupantIndex == targetIndex) {
                    outputDirection = COMBAT_DIRECTION_SPECIAL_SECOND;
                } else if (direction == 3 &&
                           m_hexCells[targetHex + 1].m_occupantSide == targetSide &&
                           m_hexCells[targetHex + 1].m_occupantIndex == targetIndex) {
                    outputDirection = COMBAT_DIRECTION_SPECIAL_SECOND;
                }
            }

            if (direction < COMBAT_ADJACENT_DIRECTION_COUNT) {
                memset(&m_directionMap[mappedDirection *
                                       COMBAT_DIRECTION_SECTORS_PER_ADJACENT],
                       outputDirection,
                       COMBAT_DIRECTION_SECTORS_PER_ADJACENT);
            } else if (direction == COMBAT_DIRECTION_SPECIAL_FIRST) {
                m_directionMap[COMBAT_DIRECTION_SPECIAL_FIRST_SECTOR_START] =
                    static_cast<signed char>(outputDirection);
                m_directionMap[COMBAT_DIRECTION_SPECIAL_FIRST_SECTOR_CENTER] =
                    static_cast<signed char>(outputDirection);
                m_directionMap[COMBAT_DIRECTION_SPECIAL_FIRST_SECTOR_END] =
                    static_cast<signed char>(outputDirection);
            } else {
                m_directionMap[COMBAT_DIRECTION_SPECIAL_SECOND_SECTOR_CENTER] =
                    static_cast<signed char>(outputDirection);
                m_directionMap[COMBAT_DIRECTION_SPECIAL_SECOND_SECTOR_NEXT] =
                    static_cast<signed char>(outputDirection);
                m_directionMap[COMBAT_DIRECTION_SPECIAL_SECOND_SECTOR_PREVIOUS] =
                    static_cast<signed char>(outputDirection);
            }
        }
    }

    int unresolved = COMBAT_DIRECTION_SECTOR_COUNT;
    while (unresolved > 0) {
        for (direction = 0; direction < COMBAT_DIRECTION_SECTOR_COUNT;
             direction++) {
            if (m_directionMap[direction] == -1) {
                int next = (direction + 1) % COMBAT_DIRECTION_SECTOR_COUNT;
                int previous =
                    (direction + COMBAT_DIRECTION_SECTOR_COUNT - 1) %
                    COMBAT_DIRECTION_SECTOR_COUNT;
                if (m_directionMap[next] < 0 ||
                    m_directionMap[next] >= COMBAT_DIRECTION_COUNT) {
                    if (m_directionMap[previous] >= 0 &&
                        m_directionMap[previous] < COMBAT_DIRECTION_COUNT) {
                        m_directionMap[direction] =
                            m_directionMap[previous] +
                            COMBAT_DIRECTION_PENDING_OFFSET;
                    }
                } else {
                    m_directionMap[direction] =
                        m_directionMap[next] + COMBAT_DIRECTION_PENDING_OFFSET;
                }
            }
        }
        unresolved = 0;
        for (direction = 0; direction < COMBAT_DIRECTION_SECTOR_COUNT;
             direction++) {
            if (m_directionMap[direction] < COMBAT_DIRECTION_PENDING_OFFSET) {
                if (m_directionMap[direction] == -1)
                    unresolved++;
            } else {
                m_directionMap[direction] -= COMBAT_DIRECTION_PENDING_OFFSET;
            }
        }
    }

    currentArmy->m_targetSide = targetSide;
    currentArmy->m_targetIndex = targetIndex;
}

// @match-note 98.80%: frame 0x34 and all 19/19
// relocations agree. The first residual is instruction 60, the empty quadrant
// arm's compare polarity; remaining differences are literal-pool identities and
// a one-byte span delta. Positive gating, staged coordinate subtraction, the
// retail-wide arm order, and the unused typed targetArmy local are already restored.
VA(0x0042b45f, 0x63c)
void combatManager::CheckSetMouseDirection(int mouseX, int mouseY, int targetHex)
{
    if (m_gridSelectionDisabled != 0)
        return;
    if (m_validDirectionCount <= 1 && m_mouseDirection >= 0)
        return;

    int relativeX = mouseX -
                    (targetHex % COMBAT_GRID_ROW_LENGTH - 1) *
                        COMBAT_HEX_HORIZONTAL_STEP;
    relativeX -= COMBAT_HEX_GRID_LEFT_ORIGIN;
    if (((targetHex / COMBAT_GRID_ROW_LENGTH) & 1) == 0)
        relativeX -= COMBAT_HEX_ROW_STAGGER;
    int relativeY = mouseY - COMBAT_HEX_CENTER_Y_ORIGIN -
                    targetHex / COMBAT_GRID_ROW_LENGTH *
                        COMBAT_HEX_VERTICAL_STEP;
    relativeY -= 5;
    relativeX -= COMBAT_HEX_ROW_STAGGER;
    relativeY -= 21;

    int sector = 0;
    if (relativeX < 0) {
        if (relativeY < 0)
            sector += COMBAT_DIRECTION_SECTOR_THREE_QUARTERS;
        else
            sector += COMBAT_DIRECTION_SECTOR_HALF;
    } else {
        if (relativeY >= 0)
            sector += COMBAT_DIRECTION_SECTOR_QUARTER;
    }

    relativeX = abs(relativeX);
    relativeY = abs(relativeY);
    float ratio = static_cast<float>(relativeX) / relativeY;
    if (sector == 0 || sector == COMBAT_DIRECTION_SECTOR_HALF) {
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
    int directionResult = OppositeDirection(m_directionMap[sector]);
    int direction = directionResult;
    int alternateDirection = -1;
    army *currentArmy = &m_armies[m_currentArmySide][m_currentArmyIndex];
    army *targetArmy =
        &m_armies[currentArmy->m_targetSide][currentArmy->m_targetIndex];

    int adjacentDirection;
    if (direction == COMBAT_DIRECTION_SPECIAL_FIRST ||
        direction == COMBAT_DIRECTION_SPECIAL_SECOND) {
        if ((currentArmy->m_monster.flags.all & MONSTER_FLAGS_WIDE) != 0) {
            if (currentArmy->m_facing == 1 &&
                direction == COMBAT_DIRECTION_SPECIAL_FIRST) {
                adjacentDirection = 5;
                alternateDirection = 0;
            } else if (currentArmy->m_facing == 1 &&
                       direction == COMBAT_DIRECTION_SPECIAL_SECOND) {
                adjacentDirection = 3;
                alternateDirection = 2;
            } else if (currentArmy->m_facing == 0 &&
                       direction == COMBAT_DIRECTION_SPECIAL_FIRST) {
                adjacentDirection = 0;
                alternateDirection = 5;
            } else {
                adjacentDirection = 2;
                alternateDirection = 3;
            }
        } else {
            if (m_hexCells[targetHex - 1].m_occupantSide ==
                    currentArmy->m_targetSide &&
                m_hexCells[targetHex - 1].m_occupantIndex ==
                    currentArmy->m_targetIndex) {
                targetHex--;
            }
            if (direction == COMBAT_DIRECTION_SPECIAL_FIRST)
                adjacentDirection = 0;
            else
                adjacentDirection = 2;
        }
    } else {
        adjacentDirection = direction;
        if (currentArmy->m_facing == 1 &&
            (currentArmy->m_monster.flags.all & MONSTER_FLAGS_WIDE) != 0) {
            if (direction == 5 || direction == 4 || direction == 3)
                targetHex--;
        } else if (currentArmy->m_facing == 0 &&
                   (currentArmy->m_monster.flags.all & MONSTER_FLAGS_WIDE) != 0 &&
                   (direction == 0 || direction == 1 || direction == 2)) {
            targetHex++;
        }
    }

    m_directionTargetHex = m_adjacency[targetHex][adjacentDirection];
    int rearHex = COMBAT_IGNORED_HEX;
    if (currentArmy->m_facing == 0 &&
        (currentArmy->m_monster.flags.all & MONSTER_FLAGS_WIDE) != 0) {
        rearHex = m_directionTargetHex - 1;
    }
    if (currentArmy->m_facing == 1 &&
        (currentArmy->m_monster.flags.all & MONSTER_FLAGS_WIDE) != 0) {
        rearHex = m_directionTargetHex + 1;
    }
    if (ValidHexToStandOn(m_directionTargetHex) == 0 ||
        ValidHexToStandOn(rearHex) == 0) {
        if ((currentArmy->m_monster.flags.all & MONSTER_FLAGS_WIDE) != 0 &&
            (direction == COMBAT_DIRECTION_SPECIAL_FIRST ||
             direction == COMBAT_DIRECTION_SPECIAL_SECOND)) {
            if (currentArmy->m_facing == 1)
                m_directionTargetHex++;
            else
                m_directionTargetHex--;
        } else {
            if (alternateDirection != -1)
                m_directionTargetHex =
                    m_adjacency[targetHex][alternateDirection];
        }
    }
    gpMouseManager->SetPointer(m_mouseDirection + COMBAT_POINTER_ATTACK_OFFSET);
}

VA(0x0042ba9b, 0x8b)
int combatManager::GetPointer(int command, int hexIndex)
{
    int result;
    if (command == COMBAT_MESSAGE_COMMAND_OPPOSING_OPTIONS) {
        return COMBAT_POINTER_VIEW;
    } else {
        switch (command) {
        case COMBAT_MESSAGE_COMMAND_MOVE:
        case COMBAT_MESSAGE_COMMAND_FLY:
        case COMBAT_MESSAGE_COMMAND_SHOOT:
        case COMBAT_MESSAGE_COMMAND_VIEW_INFO: {
            int x = hexIndex % COMBAT_GRID_ROW_LENGTH;
            int y = hexIndex / COMBAT_GRID_ROW_LENGTH;
            return command;
        }
        default:
            return command;
        }
    }
}

// @match-note 94.06%: current span is 0x8a9 versus retail 0x8e4 and frame is
// 0x68 versus 0x70. Manual range review finds 109 relocation entries on both
// sides with matching external-call multiplicities; the helper truncates at the
// first delinked switch label. The first normalized residual follows instruction
// 30 at the help switch table. Retail body order, all event cases, and dialog,
// pointer, grid, spell, and debug paths are reconstructed; local-slot recovery remains.
VA(0x0042bb26, 0x8e4)
int combatManager::ProcessCombatMsg(tag_message &message)
{
    int mouseX = message.field10;
    int mouseY = message.field14;
    int unusedResult = 0;

    switch (message.type) {
    case COMBAT_EVENT_WINDOW:
        if ((message.fieldC & COMBAT_WINDOW_HELP_FLAG) != 0) {
            if (message.field4 == COMBAT_WINDOW_HOVER ||
                message.field4 == COMBAT_WINDOW_HELP) {
                int helpIndex = -1;
                switch (message.field8) {
                case COMBAT_CONTROL_ATTACK:
                    helpIndex = 0;
                    break;
                case COMBAT_CONTROL_WAIT:
                    helpIndex = 1;
                    break;
                case COMBAT_CONTROL_DISABLE_SELECTION:
                    helpIndex = 2;
                    break;
                case COMBAT_CONTROL_SYSTEM_OPTIONS:
                    helpIndex = 3;
                    break;
                case COMBAT_CONTROL_HELP_FIRST:
                case COMBAT_CONTROL_HELP_SECOND:
                case COMBAT_CONTROL_HELP_THIRD:
                case COMBAT_CONTROL_HELP_FOURTH:
                    helpIndex = 4;
                    break;
                case COMBAT_WINDOW_MAIN_BUTTON:
                    RightClick(m_selectedHex);
                    break;
                }
                if (helpIndex != -1) {
                    NormalDialog(cLongCombatHelp[helpIndex],
                                 NORMAL_DIALOG_QUICK_VIEW,
                                 NORMAL_DIALOG_NO_RESOURCE,
                                 NORMAL_DIALOG_NO_VALUE,
                                 NORMAL_DIALOG_NO_RESOURCE, 0,
                                 NORMAL_DIALOG_NO_RESOURCE, 0,
                                 NORMAL_DIALOG_NO_RESOURCE, 0);
                }
            }
        } else if (message.field4 == COMBAT_WINDOW_HOVER) {
            if (message.field8 == COMBAT_WINDOW_MAIN_BUTTON)
                DoCommand(m_unknownF2CF);
        } else if (message.field4 == COMBAT_WINDOW_CLICK) {
            switch (message.field8) {
            case COMBAT_CONTROL_ATTACK:
                giNextAction = COMBAT_MESSAGE_COMMAND_ATTACK;
                break;
            case COMBAT_CONTROL_WAIT:
                giNextAction = COMBAT_AI_ACTION_WAIT;
                break;
            case COMBAT_CONTROL_DISABLE_SELECTION:
                m_gridSelectionDisabled = 1;
                break;
            case COMBAT_CONTROL_SYSTEM_OPTIONS:
                CombatSystemOptions();
                break;
            }
        }
        break;

    case COMBAT_EVENT_MOUSE_MOVE:
        if (m_gridSelectionDisabled == 0) {
            tag_message pendingMessage = gpInputManager->PeekEvent();
            if (pendingMessage.type != COMBAT_EVENT_MOUSE_MOVE) {
                int selectedHex;
                if (InCombatArea(message.field10, message.field14) != 0)
                    selectedHex = GetGridIndex(mouseX, mouseY);
                else
                    selectedHex = COMBAT_INVALID_HEX;

                UpdateMouseGrid(selectedHex, 0);
                if (InCombatArea(message.field10, message.field14) != 0) {
                    if (m_selectedHex != selectedHex ||
                        selectedHex == COMBAT_INVALID_HEX) {
                        m_selectedHex = selectedHex;
                        m_unknownF2CB = COMBAT_INVALID_COMMAND;
                        m_unknownF2CF = GetCommand(m_selectedHex);
                        m_mouseDirection = COMBAT_INVALID_HEX;
                        if (m_unknownF2CF == COMBAT_MESSAGE_COMMAND_ATTACK) {
                            SetCombatDirections(selectedHex);
                            CheckSetMouseDirection(mouseX, mouseY, selectedHex);
                        } else {
                            gpMouseManager->SetPointer(
                                GetPointer(m_unknownF2CF, selectedHex));
                        }
                    } else if (m_unknownF2CF == COMBAT_MESSAGE_COMMAND_ATTACK) {
                        CheckSetMouseDirection(mouseX, mouseY, selectedHex);
                    }
                    if (m_unknownF2CB != m_unknownF2CF) {
                        m_unknownF2CB = m_unknownF2CF;
                        CombatMessage(m_unknownF2CF);
                    }
                } else {
                    if (mouseX >= COMBAT_CONTROL_RIGHT_MIN_X) {
                        CombatMessage(cCombatHelp[COMBAT_HELP_SKIP_UNIT], 1, 0,
                                      0);
                    } else if (mouseX <= COMBAT_CONTROL_LEFT_MAX_X &&
                               mouseY < COMBAT_CONTROL_SYSTEM_OPTIONS_MIN_Y) {
                        CombatMessage(cCombatHelp[COMBAT_HELP_AUTO_COMBAT], 1,
                                      0, 0);
                    } else if (mouseX <= COMBAT_CONTROL_LEFT_MAX_X) {
                        CombatMessage(cCombatHelp[COMBAT_HELP_SYSTEM_OPTIONS], 1,
                                      0, 0);
                    } else {
                        CombatMessage(cCombatHelp[COMBAT_HELP_OTHER_CONTROL], 1,
                                      0, 0);
                    }
                    gpMouseManager->SetPointer(COMBAT_POINTER_DEFAULT);
                    m_selectedHex = COMBAT_INVALID_HEX;
                    m_unknownF2CB = COMBAT_INVALID_COMMAND;
                }
            }
        }
        return COMBAT_MAIN_CONTINUE;

    case COMBAT_EVENT_KEY:
        switch (message.field4) {
        case COMBAT_KEY_CLOSE_NETWORK_BOX:
            PopNetBox(0, -1);
            break;
        case COMBAT_KEY_REDRAW_SCREEN:
            gpWindowManager->UpdateScreenRegion(
                0, 0, COMBAT_SCREEN_WIDTH - 1, COMBAT_SCREEN_HEIGHT - 1);
            break;
        case COMBAT_KEY_CYCLE_ARMY_VIEW:
            SetCombatViewArmySmallLevel(
                (giCombatViewArmySmallLevel + 1) %
                COMBAT_ARMY_VIEW_LEVEL_COUNT);
            break;
        case COMBAT_KEY_TOGGLE_GRID:
            SetCombatGrid(1 - gbShowCombatGrid, gbShowCombatMouseHex,
                          giCombatShadeLevel);
            break;
        case COMBAT_KEY_TOGGLE_MOUSE_HEX:
            SetCombatGrid(gbShowCombatGrid, 1 - gbShowCombatMouseHex,
                          giCombatShadeLevel);
            break;
        case COMBAT_KEY_CYCLE_SHADE:
            SetCombatGrid(gbShowCombatGrid, gbShowCombatMouseHex,
                          1 - giCombatShadeLevel);
            break;
        case COMBAT_KEY_CYCLE_SPELL_EFFECT:
            giSpellEffectShowType =
                (giSpellEffectShowType + 1) % COMBAT_SPELL_EFFECT_TYPE_COUNT;
            DrawFrame(1, 0, 0, 0, COMBAT_COMMAND_FRAME_DELAY, 1, 1);
            break;
        case COMBAT_KEY_DEBUG_CREATURE_EFFECT:
            if ((message.fieldC & COMBAT_DEBUG_VAPORIZE_MASK) != 0) {
                VaporizeCreature(1, 1);
            } else if ((message.fieldC & COMBAT_DEBUG_DOUBLE_RIPPLE_MASK) != 0) {
                RippleCreature(1, 1, 1);
                RippleCreature(1, 1, 2);
            } else {
                RippleCreature(1, 1, 0);
            }
            break;
        case COMBAT_KEY_WAIT:
            giNextAction = COMBAT_AI_ACTION_WAIT;
            break;
        case COMBAT_KEY_MOUSE_COORDS: {
            int currentMouseX;
            int currentMouseY;
            gpMouseManager->MouseCoords(currentMouseX, currentMouseY);
            break;
        }
        case COMBAT_KEY_VIEW_GENERAL:
            if (m_heroes[m_currentSide] != 0) {
                gpMouseManager->SetPointer(COMBAT_POINTER_DEFAULT);
                ViewGeneral(m_currentSide, 1, 0);
                ResetMouse();
            }
            break;
        case COMBAT_KEY_VIEW_ARMY:
            gpMouseManager->SetPointer(COMBAT_POINTER_DEFAULT);
            ViewArmy(&m_armies[m_currentArmySide][m_currentArmyIndex], 0);
            ResetMouse();
            break;
        case COMBAT_KEY_CAST_SPELL:
            if (m_heroes[m_currentSide] == 0) {
                NormalDialog("You have no hero to cast a spell.",
                             NORMAL_DIALOG_INFO, NORMAL_DIALOG_NO_RESOURCE,
                             NORMAL_DIALOG_NO_VALUE, NORMAL_DIALOG_NO_RESOURCE,
                             0, NORMAL_DIALOG_NO_RESOURCE, 0,
                             NORMAL_DIALOG_NO_RESOURCE, 0);
            } else if (IsNegationSphereInEffect() != 0) {
                NormalDialog(
                    "The Sphere of Negation artifact is in effect for this battle, disabling all combat spells.",
                    NORMAL_DIALOG_INFO, NORMAL_DIALOG_NO_RESOURCE,
                    NORMAL_DIALOG_NO_VALUE, NORMAL_DIALOG_NO_RESOURCE, 0,
                    NORMAL_DIALOG_NO_RESOURCE, 0, NORMAL_DIALOG_NO_RESOURCE,
                    0);
            } else if (m_heroCastSpell[m_currentSide] != 0 &&
                       giDebugLevel == 0) {
                NormalDialog("You have already cast a spell this round.",
                             NORMAL_DIALOG_INFO, NORMAL_DIALOG_NO_RESOURCE,
                             NORMAL_DIALOG_NO_VALUE, NORMAL_DIALOG_NO_RESOURCE,
                             0, NORMAL_DIALOG_NO_RESOURCE, 0,
                             NORMAL_DIALOG_NO_RESOURCE, 0);
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

    return COMBAT_MAIN_CONTINUE;
}

VA(0x0042c40a, 0x70)
int combatManager::IsNegationSphereInEffect(void) { return 0; }

VA(0x0042c47a, 0x205)
void combatManager::ResetRound(void) {}

VA(0x0042c67f, 0x280)
int combatManager::CheckWin(struct tag_message *) { return 0; }

VA(0x0042c8ff, 0x51a)
int combatManager::GetCommand(int) { return 0; }

VA(0x0042ce19, 0x2a6)
int combatManager::RightClick(int) { return 0; }

VA(0x0042d0bf, 0x3b3)
void combatManager::DoCommand(int) {}

VA(0x0042d472, 0x57b)
int WinCombatHandler(struct tag_message &) { return 0; }

VA(0x0042d9ed, 0x110)
void combatManager::ClearWinLoseBottom(class heroWindow *) {}

VA(0x0042dafd, 0x29a)
void combatManager::ShowWinLoseArtifact(class heroWindow *, int) {}

VA(0x0042dd97, 0x232)
void combatManager::ShowSkeletons(class heroWindow *) {}

VA(0x0042dfc9, 0x2f6)
void combatManager::ShowEagleEyeSpell(class heroWindow *) {}

VA(0x0042e2bf, 0x9cc)
void combatManager::ShowDeadArmies(class heroWindow *) {}

VA(0x0042ec8b, 0xba9)
void combatManager::DoVictory(int) {}

VA(0x0042f834, 0x3bc)
void combatManager::DoLoseWindow(void) {}

VA(0x0042fbf0, 0x43d)
int combatManager::DoSurrender(void) { return 0; }

VA(0x0043002d, 0xdc)
void combatManager::CheckChangeSelector(void) {}

VA(0x00430109, 0xea)
void combatManager::CheckCastleAttack(void) {}

VA(0x004301f3, 0xdd)
void combatManager::CheckGetAIMove(void) {}

VA(0x004302d0, 0x18f)
void combatManager::GetControl(void) {}

VA(0x0043045f, 0xd7)
void combatManager::ResetMouse(void) {}

VA(0x00430536, 0x65b)
int combatManager::ProcessNextAction(struct tag_message &) { return 0; }

VA(0x00430b91, 0x237)
void combatManager::ResetCyclingCreatures(void) {}

VA(0x00430dc8, 0xf9)
void combatManager::ResetCycleTimers(void) {}

VA(0x00430ec1, 0x53)
int InCombatArea(int, int) { return 0; }

VA(0x00430f14, 0x9d9)
void combatManager::CycleCombatScreen(void) {}

VA(0x004318ed, 0x3b)
void combatManager::SetCombatViewArmySmallLevel(int) {}

VA(0x00431928, 0xe3)
void combatManager::SetCombatGrid(int, int, int) {}

VA(0x00431a0b, 0x3ab)
void combatManager::AddArmy(int, int, int, int, int, int) {}

VA(0x00431db6, 0x169)
void combatManager::SetupSmallView(void) {}

VA(0x00431f1f, 0x304)
void combatManager::ViewBallista(int) {}

// ---- globals (definitions, RVA order) ----
DATA(0x005250b8) int gbThisNetHasControl;
DATA(0x005250bc) int iCurTransferArtifact;
DATA(0x005250c0) signed char *iTransferArtifactsInfo;
DATA(0x005250d0) int gbWhichAnimationPlaying;
DATA(0x005250d4) int iMaxTransferArtifacts;
DATA(0x005250d8) int giNextActionExtra;
DATA(0x005250dc) int bSkeletonsShown;
DATA(0x005250e0) int gbShowingLoseWindow;
DATA(0x005250e4) int giWinCmbtFrame;
DATA(0x005250e8) int giNextActionGridIndex;
DATA(0x005250ec) int giSurrenderCost;
DATA(0x005250f0) int giSkeletonsCreated;
DATA(0x005250f8) signed char *iTransferArtifacts;
DATA(0x00525108) int giNextAction;
DATA(0x0052510c) int giNextActionGridIndex2;
