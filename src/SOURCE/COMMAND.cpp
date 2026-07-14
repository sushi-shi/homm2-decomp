// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\COMMAND.OBJ   from: (directly linked into exe)
// functions: 36   data: 15
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <_globals_model.h>
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
                switch (message.payload.keyboard.keyCode) {
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

// @match-note retained/combined-live 94.35%: current code span is 0x17c versus
// retail 0x181; frame
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

// @match-note retained/combined-live 87.85%: code size 0x8a1, frame 0x7c, and
// all 5/5 relocations
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
    int mouseX = message.payload.mouse.screenX;
    int mouseY = message.payload.mouse.screenY;
    int unusedResult = 0;

    switch (message.type) {
    case COMBAT_EVENT_WINDOW:
        if ((message.payload.widget.parameter & COMBAT_WINDOW_HELP_FLAG) != 0) {
            if (message.payload.widget.command == COMBAT_WINDOW_HOVER ||
                message.payload.widget.command == COMBAT_WINDOW_HELP) {
                int helpIndex = -1;
                switch (message.payload.widget.id) {
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
        } else if (message.payload.widget.command == COMBAT_WINDOW_HOVER) {
            if (message.payload.widget.id == COMBAT_WINDOW_MAIN_BUTTON)
                DoCommand(m_currentCommand);
        } else if (message.payload.widget.command == COMBAT_WINDOW_CLICK) {
            switch (message.payload.widget.id) {
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
                if (InCombatArea(message.payload.mouse.screenX, message.payload.mouse.screenY) != 0)
                    selectedHex = GetGridIndex(mouseX, mouseY);
                else
                    selectedHex = COMBAT_INVALID_HEX;

                UpdateMouseGrid(selectedHex, 0);
                if (InCombatArea(message.payload.mouse.screenX, message.payload.mouse.screenY) != 0) {
                    if (m_selectedHex != selectedHex ||
                        selectedHex == COMBAT_INVALID_HEX) {
                        m_selectedHex = selectedHex;
                        m_previousCommand = COMBAT_INVALID_COMMAND;
                        m_currentCommand = GetCommand(m_selectedHex);
                        m_mouseDirection = COMBAT_INVALID_HEX;
                        if (m_currentCommand == COMBAT_MESSAGE_COMMAND_ATTACK) {
                            SetCombatDirections(selectedHex);
                            CheckSetMouseDirection(mouseX, mouseY, selectedHex);
                        } else {
                            gpMouseManager->SetPointer(
                                GetPointer(m_currentCommand, selectedHex));
                        }
                    } else if (m_currentCommand == COMBAT_MESSAGE_COMMAND_ATTACK) {
                        CheckSetMouseDirection(mouseX, mouseY, selectedHex);
                    }
                    if (m_previousCommand != m_currentCommand) {
                        m_previousCommand = m_currentCommand;
                        CombatMessage(m_currentCommand);
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
                    m_previousCommand = COMBAT_INVALID_COMMAND;
                }
            }
        }
        return COMBAT_MAIN_CONTINUE;

    case COMBAT_EVENT_KEY:
        switch (message.payload.keyboard.keyCode) {
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
            if ((message.payload.keyboard.modifiers & COMBAT_DEBUG_VAPORIZE_MASK) != 0) {
                VaporizeCreature(1, 1);
            } else if ((message.payload.keyboard.modifiers & COMBAT_DEBUG_DOUBLE_RIPPLE_MASK) != 0) {
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
int combatManager::IsNegationSphereInEffect(void)
{
    int side;
    for (side = 0; side < COMBAT_MANAGER_SIDE_COUNT; side++) {
        if (m_heroes[side] != 0 &&
            m_heroes[side]->HasArtifact(ARTIFACT_SPHERE_NEGATION) != 0)
            return 1;
    }
    return 0;
}

// @match-note 99.66%: code span is 0x205 versus retail 0x205 and all 8/8
// relocation targets agree. The 132-instruction normalized streams are
// identical; raw bytes first differ at +0x5 because retail reserves frame 0x14
// versus 0x10 here. The missing retail stack word has no recovered use, so no
// padding local was introduced.
VA(0x0042c47a, 0x205)
void combatManager::ResetRound(void)
{
    m_heroDeathAnimationPlayed[0] = m_heroDeathAnimationPlayed[1] = 0;
    m_heroAlternateDeathAnimationPlayed[0] =
        m_heroAlternateDeathAnimationPlayed[1] = 0;
    m_heroDeathPending[0] = m_heroDeathPending[1] = 0;
    m_heroAlternateDeathPending[0] = m_heroAlternateDeathPending[1] = 0;
    m_catapultAttacksRemaining[0] = m_catapultAttackCount[0];
    m_catapultAttacksRemaining[1] = m_catapultAttackCount[1];
    m_keepAttacksRemaining[0] = 1;
    m_keepAttacksRemaining[1] = 1;
    m_heroCastSpell[0] = m_heroCastSpell[1] = 0;

    memset(gpCombatManager->m_removedArmies, 0,
           sizeof(gpCombatManager->m_removedArmies));
    gpCombatManager->m_removedArmyPresent = 0;

    int side;
    int armyIndex;
    for (side = 0; side < COMBAT_MANAGER_SIDE_COUNT; side++) {
        for (armyIndex = 0; armyIndex < COMBAT_ARMY_SLOT_COUNT; armyIndex++) {
            army *currentArmy = m_armies[side] + armyIndex;
            if (currentArmy->m_quantity > 0) {
                currentArmy->m_monster.flags.abilityFlags &=
                    COMBAT_ROUND_ABILITY_FLAGS;
                if (currentArmy->m_monsterType == ARMY_CREATURE_TROLL ||
                    currentArmy->m_monsterType == ARMY_CREATURE_WAR_TROLL)
                    currentArmy->m_hitPointsLost = 0;
                currentArmy->DecrementSpellRounds();
                if (currentArmy->m_roundCounter == 0)
                    currentArmy->ProcessDeath(1);
            }
        }
    }

    if (gpCombatManager->m_removedArmyPresent != 0)
        gpCombatManager->MakeCreaturesVanish();
    m_currentSpeed = COMBAT_ROUND_INITIAL_SPEED;
}

// @match-note 99.80%: code span is 0x280 versus retail 0x280 and all 6/6
// relocation targets agree. The 163-instruction normalized streams are
// identical; raw bytes first differ at +0x5 because retail reserves frame 0x14
// versus 0x10 here. No semantic local accounts for the unused retail word.
VA(0x0042c67f, 0x280)
int combatManager::CheckWin(struct tag_message *message)
{
    int combatEnded = 0;
    if (IsWinner(m_currentSide) != 0) {
        combatEnded = 1;
        if (IsWinner(1 - m_currentSide) != 0)
            m_combatResult = COMBAT_RESULT_DRAW;
        else
            m_combatResult = m_currentSide;
    } else if (IsWinner(1 - m_currentSide) != 0) {
        combatEnded = 1;
        m_combatResult = 1 - m_currentSide;
    } else if (m_sideRetreated[0] != 0 || m_sideRetreated[1] != 0) {
        combatEnded = 1;
        gbRetreatWin = 1;
        if (m_sideRetreated[0] != 0)
            m_combatResult = 1;
        else
            m_combatResult = 0;
    }

    if (combatEnded != 0 && m_combatResult != COMBAT_RESULT_DRAW) {
        int armyAlive = 0;
        int armyIndex;
        for (armyIndex = 0; armyIndex < COMBAT_ARMY_SLOT_COUNT; armyIndex++) {
            if (m_armies[m_combatResult][armyIndex].m_monsterType != -1 &&
                m_armies[m_combatResult][armyIndex].m_quantity > 0 &&
                (m_armies[m_combatResult][armyIndex].m_monster.flags.all &
                 MONSTER_FLAGS_SUMMONED) == 0) {
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
            message->payload.widget.command = 1;
        }
    }
    return combatEnded;
}

// @match-note 94.59%: code span is 0x4fe versus retail 0x51a, frame 0x28
// agrees, and all 14/14 relocation targets agree. The first normalized residual
// is instruction 21 in the source-ordered special-hex switch; both flattened
// if/else and restored switch forms were tested. The remaining delta is branch
// layout around the special cases and the late empty-cell arm.
VA(0x0042c8ff, 0x51a)
int combatManager::GetCommand(int hexIndex)
{
    int column = hexIndex % COMBAT_GRID_ROW_LENGTH;
    int row = hexIndex / COMBAT_GRID_ROW_LENGTH;
    int command = COMBAT_MESSAGE_COMMAND_DEFAULT;
    int showSmallView = 0;

    if (hexIndex == COMBAT_INVALID_HEX) {
        command = COMBAT_MESSAGE_COMMAND_DEFAULT;
    } else {
        switch (hexIndex) {
        case COMBAT_GRID_RIGHT_HERO_HEX:
            if (m_heroes[1] != 0) {
                if (m_currentSide == 1)
                    command = COMBAT_MESSAGE_COMMAND_OPTIONS;
                else
                    command = COMBAT_MESSAGE_COMMAND_OPPOSING_OPTIONS;
            } else {
                command = COMBAT_MESSAGE_COMMAND_DEFAULT;
            }
            break;
        case COMBAT_GRID_LEFT_SPECIAL_HEX:
            if (m_heroes[0] != 0) {
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
            if (hexIndex % COMBAT_GRID_ROW_LENGTH ==
                COMBAT_GRID_ROW_LENGTH - 1) {
                command = COMBAT_MESSAGE_COMMAND_DEFAULT;
                break;
            }

            int targetSide = m_hexCells[hexIndex].m_occupantSide;
            int targetIndex = m_hexCells[hexIndex].m_occupantIndex;
            army *currentArmy =
                &m_armies[m_currentArmySide][m_currentArmyIndex];
            currentArmy->m_targetSide = -1;
            currentArmy->m_targetIndex = -1;

            if (m_hexCells[hexIndex].m_blocked != 0 &&
                (gpCombatManager->m_inCastleCombat == 0 ||
                 (hexIndex != COMBAT_CASTLE_GATE_APPROACH_HEX &&
                  hexIndex != COMBAT_CASTLE_GATE_HEX) ||
                 (gpCombatManager->m_drawbridgeState ==
                      COMBAT_CASTLE_GATE_OPEN &&
                  (gpCombatManager->m_currentSide != COMBAT_DEFENDER_SIDE ||
                   gpCombatManager
                           ->m_hexCells[COMBAT_CASTLE_GATE_APPROACH_HEX]
                           .m_occupantSide != -1 ||
                   gpCombatManager
                           ->m_hexCells[COMBAT_CASTLE_GATE_APPROACH_HEX]
                           .m_deadOccupantCount != 0)))) {
                command = COMBAT_MESSAGE_COMMAND_DEFAULT;
            } else if (targetSide != -1) {
                if (m_currentArmySide != targetSide ||
                    m_currentArmyIndex != targetIndex) {
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
                    if (m_currentSide == targetSide ||
                        (m_currentArmySide == targetSide &&
                         m_currentArmyIndex == targetIndex)) {
                        return COMBAT_MESSAGE_COMMAND_VIEW_INFO;
                    }
                    currentArmy->m_targetSide = targetSide;
                    currentArmy->m_targetIndex = targetIndex;
                    if (currentArmy->m_monster.shots > 0 &&
                        currentArmy->GetAttackMask(currentArmy->m_hex, 1, -1) ==
                            0xff) {
                        if (ShotIsThroughWall(currentArmy->m_side,
                                              currentArmy->m_hex,
                                              hexIndex) != 0)
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
                if (m_armies[m_currentArmySide][m_currentArmyIndex]
                        .ValidPath(hexIndex, 0) == 1) {
                    command = static_cast<char>(
                        2 - ((m_armies[m_currentArmySide][m_currentArmyIndex]
                                  .m_monster.flags.all &
                              MONSTER_FLAGS_FLYING) == 0));
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

// @match-note 97.19%: code span is 0x290 versus retail 0x2a6, frame 0x1c
// agrees, and all 14/14 relocation targets agree. The first normalized residual
// is instruction 123, an extra jump at the transition from the blocked-cell
// return into the inner side switch. Restoring both retail switches and their
// case-body order removed the earlier structural divergence.
VA(0x0042ce19, 0x2a6)
int combatManager::RightClick(int hexIndex)
{
    int column = hexIndex % COMBAT_GRID_ROW_LENGTH;
    int row = hexIndex / COMBAT_GRID_ROW_LENGTH;
    if (hexIndex == COMBAT_INVALID_HEX)
        return 0;

    switch (hexIndex) {
    case COMBAT_BALLISTA_HEX:
        if (m_inCastleCombat != 0)
            ViewBallista(1);
        return 0;
    case COMBAT_GRID_RIGHT_HERO_HEX:
        if (m_heroes[1] != 0) {
            ViewGeneral(1, 0, 1);
            ResetMouse();
        }
        return 0;
    case COMBAT_GRID_LEFT_SPECIAL_HEX:
        if (m_heroes[0] != 0) {
            ViewGeneral(0, 0, 1);
            ResetMouse();
        }
        return 0;
    default:
        if (hexIndex % COMBAT_GRID_ROW_LENGTH ==
            COMBAT_GRID_ROW_LENGTH - 1)
            return 0;

        int side = m_hexCells[hexIndex].m_occupantSide;
        int armyIndex = m_hexCells[hexIndex].m_occupantIndex;
        if (m_hexCells[hexIndex].m_blocked != 0 &&
            (gpCombatManager->m_inCastleCombat == 0 ||
             (hexIndex != COMBAT_CASTLE_GATE_APPROACH_HEX &&
              hexIndex != COMBAT_CASTLE_GATE_HEX) ||
             (gpCombatManager->m_drawbridgeState == COMBAT_CASTLE_GATE_OPEN &&
              (gpCombatManager->m_currentSide != COMBAT_DEFENDER_SIDE ||
               gpCombatManager->m_hexCells[COMBAT_CASTLE_GATE_APPROACH_HEX]
                       .m_occupantSide != -1 ||
               gpCombatManager->m_hexCells[COMBAT_CASTLE_GATE_APPROACH_HEX]
                       .m_deadOccupantCount != 0))))
            return 0;

        if (side != -1) {
            switch (side) {
            case 0:
            case 1:
                gpMouseManager->SetPointer(COMBAT_POINTER_DEFAULT);
                ViewArmy(&m_armies[side]
                                  [m_hexCells[m_selectedHex].m_occupantIndex],
                         1);
                ResetMouse();
                return 0;
            }
        }
    }
    return 0;
}

// @match-note 99.58%: code span is 0x3b3
// versus retail 0x3b3 with 54/54 relocation sites; all 38 external global/callee
// targets agree, while 3 string-pool and 13 jump-table targets use compiler-local
// identities. Raw bytes first differ at +0x5 because retail reserves frame 0x14
// versus 0xc here. The target jump table is +0x36b..+0x397; the disasm helper
// truncates at its delinked local label, while source-order case bodies agree.
VA(0x0042d0bf, 0x3b3)
void combatManager::DoCommand(int command)
{
    army *currentArmy = &m_armies[m_currentArmySide][m_currentArmyIndex];
    switch (command) {
    case COMBAT_MESSAGE_COMMAND_DEFAULT:
        break;
    case COMBAT_MESSAGE_COMMAND_MOVE:
    case COMBAT_MESSAGE_COMMAND_FLY:
    case COMBAT_MESSAGE_COMMAND_SHOOT:
    case COMBAT_MESSAGE_COMMAND_SHOOT_THROUGH_WALL:
        giNextAction = COMBAT_AI_ACTION_MOVE;
        giNextActionGridIndex = m_selectedHex;
        giNextActionExtra = -1;
        break;
    case COMBAT_MESSAGE_COMMAND_ATTACK:
        giNextActionGridIndex = m_selectedHex;
        if (m_playerId[m_currentSide] == -1 ||
            gbHumanPlayer[m_playerId[m_currentSide]] == 0 ||
            m_gridSelectionDisabled != 0) {
            giNextAction = COMBAT_AI_ACTION_MOVE;
            giNextActionExtra = -1;
        } else {
            giNextAction = COMBAT_AI_ACTION_ATTACK;
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
        ViewGeneral(1 - m_currentSide, 1, 0);
        ResetMouse();
        break;
    case COMBAT_MESSAGE_COMMAND_VIEW_INFO:
        gpMouseManager->SetPointer(COMBAT_POINTER_DEFAULT);
        if (m_selectedHex == COMBAT_BALLISTA_HEX)
            ViewBallista(0);
        else
            ViewArmy(&m_armies[m_hexCells[m_selectedHex].m_occupantSide]
                              [m_hexCells[m_selectedHex].m_occupantIndex],
                     0);
        ResetMouse();
        break;
    case COMBAT_MESSAGE_COMMAND_CAST_SPELL:
        if (IsNegationSphereInEffect() != 0) {
            NormalDialog(
                "The Sphere of Negation artifact is in effect for this battle, disabling all combat spells.",
                NORMAL_DIALOG_INFO, NORMAL_DIALOG_NO_RESOURCE,
                NORMAL_DIALOG_NO_VALUE, NORMAL_DIALOG_NO_RESOURCE, 0,
                NORMAL_DIALOG_NO_RESOURCE, 0, NORMAL_DIALOG_NO_RESOURCE, 0);
        } else {
            ViewSpells(0);
            ResetMouse();
        }
        break;
    case COMBAT_MESSAGE_COMMAND_RETREAT:
        NormalDialog("Are you sure you want to retreat?",
                     NORMAL_DIALOG_CONFIRM, NORMAL_DIALOG_NO_RESOURCE,
                     NORMAL_DIALOG_NO_VALUE, NORMAL_DIALOG_NO_RESOURCE, 0,
                     NORMAL_DIALOG_NO_RESOURCE, 0, NORMAL_DIALOG_NO_RESOURCE,
                     0);
        if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_BUTTON_FIVE)
            giNextAction = COMBAT_AI_ACTION_RETREAT;
        ResetMouse();
        break;
    case COMBAT_MESSAGE_COMMAND_SURRENDER:
        if (DoSurrender() == 1) {
            if (gpGame->m_players[m_playerId[m_currentSide]].resources[RES_GOLD] <
                giSurrenderCost) {
                NormalDialog("You don't have enough gold!",
                             NORMAL_DIALOG_INFO, NORMAL_DIALOG_NO_RESOURCE,
                             NORMAL_DIALOG_NO_VALUE, NORMAL_DIALOG_NO_RESOURCE,
                             0, NORMAL_DIALOG_NO_RESOURCE, 0,
                             NORMAL_DIALOG_NO_RESOURCE, 0);
            } else {
                giNextAction = COMBAT_AI_ACTION_SURRENDER;
                giNextActionExtra = giSurrenderCost;
            }
        }
        ResetMouse();
        break;
    }
}

// @early-stop
VA(0x0042d472, 0x57b)
int WinCombatHandler(struct tag_message &message)
{
    char iconFile[40];
    tag_message animationMessage;
    int finalDelay = COMBAT_WIN_LOSE_INITIAL_DELAY;
    int frame;
    int iDelay;

    if (giDialogTimeout != 0 && KBTickCount() > giDialogTimeout) {
        message.type = COMBAT_EVENT_WINDOW;
        gpWindowManager->m_dialogResult = message.payload.widget.id;
        message.payload.widget.id = COMBAT_WIN_LOSE_CLOSE_COMMAND;
        message.payload.widget.command = message.payload.widget.id;
        giDialogTimeout = 0;
        return COMBAT_MAIN_FINISHED;
    }

    if (message.type == COMBAT_EVENT_WINDOW) {
        switch (message.payload.widget.command) {
        case COMBAT_WINDOW_CLICK:
            switch (message.payload.widget.id) {
            case COMBAT_WIN_LOSE_NEXT_CONTROL:
                if (gbShowingLoseWindow != 0)
                    goto ExitDialog;
                if (iCurTransferArtifact + 1 < iMaxTransferArtifacts) {
                    gpCombatManager->ClearWinLoseBottom(
                        gpCombatManager->m_winLoseWindow);
                    ++iCurTransferArtifact;
                    gpCombatManager->ShowWinLoseArtifact(
                        gpCombatManager->m_winLoseWindow,
                        iTransferArtifacts[iCurTransferArtifact]);
                } else if (giSkeletonsCreated != 0 && bSkeletonsShown == 0) {
                    bSkeletonsShown = 1;
                    gpCombatManager->ClearWinLoseBottom(
                        gpCombatManager->m_winLoseWindow);
                    gpCombatManager->ShowSkeletons(
                        gpCombatManager->m_winLoseWindow);
                } else {
                    if (gpCombatManager
                            ->m_eagleEyeSpell[gpCombatManager->m_combatResult] !=
                        -1) {
                        gpCombatManager->ClearWinLoseBottom(
                            gpCombatManager->m_winLoseWindow);
                        gpCombatManager->ShowEagleEyeSpell(
                            gpCombatManager->m_winLoseWindow);
                        gpCombatManager
                            ->m_eagleEyeSpell[gpCombatManager->m_combatResult] =
                            -1;
                    } else {
                    ExitDialog:
                        gpWindowManager->m_dialogResult = message.payload.widget.id;
                        message.payload.widget.id = COMBAT_WIN_LOSE_CLOSE_COMMAND;
                        message.payload.widget.command = message.payload.widget.id;
                        return COMBAT_MAIN_FINISHED;
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
        animationMessage.type = COMBAT_EVENT_WINDOW;
        animationMessage.payload.widget.command = COMBAT_WIN_LOSE_RESOURCE_COMMAND;
        animationMessage.payload.widget.data.text = iconFile;
        ++giWinCmbtFrame;

        switch (gbWhichAnimationPlaying) {
        case COMBAT_WIN_LOSE_ANIMATION_CYCLE_FIRST:
            frame = giWinCmbtFrame % COMBAT_WIN_LOSE_LOOP_FRAME_COUNT +
                    COMBAT_WIN_LOSE_FIRST_ANIMATION_FRAME;
            iDelay = COMBAT_WIN_LOSE_LOOP_DELAY;
            break;
        case COMBAT_WIN_LOSE_ANIMATION_CYCLE_SECOND:
            frame = giWinCmbtFrame % COMBAT_WIN_LOSE_LOOP_FRAME_COUNT +
                    COMBAT_WIN_LOSE_FIRST_ANIMATION_FRAME;
            iDelay = COMBAT_WIN_LOSE_LOOP_DELAY;
            break;
        case COMBAT_WIN_LOSE_ANIMATION_FLEE:
            if (giWinCmbtFrame ==
                COMBAT_WIN_LOSE_FLEE_SECOND_RESOURCE_FRAME) {
                sprintf(iconFile, "cmbtfle2.icn");
                animationMessage.payload.widget.id =
                    COMBAT_WIN_LOSE_RESOURCE_LOAD_ID;
                gpCombatManager->m_winLoseWindow->BroadcastMessage(
                    animationMessage);
                animationMessage.payload.widget.id =
                    COMBAT_WIN_LOSE_RESOURCE_DRAW_ID;
                gpCombatManager->m_winLoseWindow->BroadcastMessage(
                    animationMessage);
            }
            if (giWinCmbtFrame ==
                COMBAT_WIN_LOSE_FLEE_THIRD_RESOURCE_FRAME) {
                sprintf(iconFile, "cmbtfle3.icn");
                animationMessage.payload.widget.id =
                    COMBAT_WIN_LOSE_RESOURCE_LOAD_ID;
                gpCombatManager->m_winLoseWindow->BroadcastMessage(
                    animationMessage);
                animationMessage.payload.widget.id =
                    COMBAT_WIN_LOSE_RESOURCE_DRAW_ID;
                gpCombatManager->m_winLoseWindow->BroadcastMessage(
                    animationMessage);
            }
            if (giWinCmbtFrame <
                COMBAT_WIN_LOSE_FLEE_SECOND_RESOURCE_FRAME) {
                frame = giWinCmbtFrame + 1;
            } else if (giWinCmbtFrame <
                       COMBAT_WIN_LOSE_FLEE_THIRD_RESOURCE_FRAME) {
                frame = giWinCmbtFrame -
                        (COMBAT_WIN_LOSE_FLEE_SECOND_RESOURCE_FRAME - 1);
            } else {
                frame = giWinCmbtFrame -
                        (COMBAT_WIN_LOSE_FLEE_THIRD_RESOURCE_FRAME - 1);
                if (frame > COMBAT_WIN_LOSE_FLEE_LAST_FRAME)
                    frame = COMBAT_WIN_LOSE_FLEE_LAST_FRAME;
            }
            iDelay = COMBAT_WIN_LOSE_FLEE_DELAY;
            break;
        default:
            if (giWinCmbtFrame ==
                COMBAT_WIN_LOSE_LOSS_SECOND_RESOURCE_FRAME) {
                sprintf(iconFile, "cmbtlos2.icn");
                animationMessage.payload.widget.id =
                    COMBAT_WIN_LOSE_RESOURCE_LOAD_ID;
                gpCombatManager->m_winLoseWindow->BroadcastMessage(
                    animationMessage);
                animationMessage.payload.widget.id =
                    COMBAT_WIN_LOSE_RESOURCE_DRAW_ID;
                gpCombatManager->m_winLoseWindow->BroadcastMessage(
                    animationMessage);
            }
            if (giWinCmbtFrame ==
                COMBAT_WIN_LOSE_LOSS_THIRD_RESOURCE_FRAME) {
                sprintf(iconFile, "cmbtlos3.icn");
                animationMessage.payload.widget.id =
                    COMBAT_WIN_LOSE_RESOURCE_LOAD_ID;
                gpCombatManager->m_winLoseWindow->BroadcastMessage(
                    animationMessage);
                animationMessage.payload.widget.id =
                    COMBAT_WIN_LOSE_RESOURCE_DRAW_ID;
                gpCombatManager->m_winLoseWindow->BroadcastMessage(
                    animationMessage);
            }
            if (giWinCmbtFrame <
                COMBAT_WIN_LOSE_LOSS_SECOND_RESOURCE_FRAME) {
                frame = giWinCmbtFrame + 1;
                iDelay = COMBAT_WIN_LOSE_LOSS_DELAY;
            } else if (giWinCmbtFrame <
                       COMBAT_WIN_LOSE_LOSS_THIRD_RESOURCE_FRAME) {
                frame = giWinCmbtFrame -
                        (COMBAT_WIN_LOSE_LOSS_SECOND_RESOURCE_FRAME - 1);
                iDelay = COMBAT_WIN_LOSE_LOSS_DELAY;
            } else {
                if (giWinCmbtFrame == COMBAT_WIN_LOSE_LOSS_RESET_FRAME)
                    giWinCmbtFrame =
                        COMBAT_WIN_LOSE_LOSS_THIRD_RESOURCE_FRAME;
                frame = giWinCmbtFrame -
                        (COMBAT_WIN_LOSE_LOSS_THIRD_RESOURCE_FRAME - 1);
                iDelay = COMBAT_WIN_LOSE_LOSS_FINAL_DELAY;
            }
            break;
        }

        message.type = COMBAT_EVENT_WINDOW;
        message.payload.widget.command = COMBAT_WIN_LOSE_ANIMATION_COMMAND;
        message.payload.widget.id = COMBAT_WIN_LOSE_RESOURCE_DRAW_ID;
        message.payload.widget.data.value = frame;
        gpCombatManager->m_winLoseWindow->BroadcastMessage(message);
        gpCombatManager->m_winLoseWindow->DrawWindow(
            1, 0, COMBAT_WIN_LOSE_DRAW_DEPTH);
        glTimers[0] = KBTickCount() + iDelay;
    }
    return COMBAT_MAIN_CONTINUE;
}

// @early-stop
VA(0x0042d9ed, 0x110)
void combatManager::ClearWinLoseBottom(class heroWindow *window)
{
    int widgetIndex;
    for (widgetIndex = 0; widgetIndex < COMBAT_WIN_LOSE_WIDGET_COUNT;
         widgetIndex++) {
        if (m_winLoseBottomWidgets[widgetIndex] != 0) {
            window->RemoveWidget(m_winLoseBottomWidgets[widgetIndex]);
            delete m_winLoseBottomWidgets[widgetIndex];
        }
        if (m_winLoseBottomTextWidgets[widgetIndex] != 0) {
            window->RemoveWidget(m_winLoseBottomTextWidgets[widgetIndex]);
            delete m_winLoseBottomTextWidgets[widgetIndex];
        }
        m_winLoseBottomWidgets[widgetIndex] = 0;
        m_winLoseBottomTextWidgets[widgetIndex] = 0;
    }
}

// @early-stop
VA(0x0042dafd, 0x29a)
void combatManager::ShowWinLoseArtifact(class heroWindow *window, int artifact)
{
    short w = 320;
    short winBottom = 458;
    tag_message message;
    char *capturedArtifactName;

    sprintf(gText, "You have captured an enemy artifact!");
    message.type = COMBAT_EVENT_WINDOW;
    message.payload.widget.command = COMBAT_WIN_LOSE_TEXT_COMMAND;
    message.payload.widget.id = COMBAT_WIN_LOSE_TEXT_ID;
    message.payload.widget.data.text = gText;
    m_winLoseWindow->BroadcastMessage(message);

    m_winLoseBottomWidgets[0] =
        new iconWidget(136, 310, 80, 80, "winloseb.icn", 0, 0,
                       COMBAT_WIN_LOSE_ARTIFACT_ICON_ID,
                       COMBAT_WIN_LOSE_ICON_FLAGS, 1);
    if (m_winLoseBottomWidgets[0] == 0)
        MemError();
    window->AddWidget(m_winLoseBottomWidgets[0], -1);

    m_winLoseBottomWidgets[1] =
        new iconWidget(144, 318, 64, 64, "artifact.icn",
                       artifact + 1, 0,
                       COMBAT_WIN_LOSE_ARTIFACT_IMAGE_ID,
                       COMBAT_WIN_LOSE_ICON_FLAGS, 1);
    if (m_winLoseBottomWidgets[1] == 0)
        MemError();
    window->AddWidget(m_winLoseBottomWidgets[1], -1);

    capturedArtifactName = static_cast<char *>(
        BaseAlloc(60, COMMAND_SOURCE_FILE, COMMAND_ARTIFACT_ALLOC_LINE));
    sprintf(capturedArtifactName, gArtifactNames[artifact]);
    m_winLoseBottomTextWidgets[0] =
        new textWidget(16, 397, 320, 12, capturedArtifactName,
                       "smalfont.fnt", 1,
                       COMBAT_WIN_LOSE_ARTIFACT_TEXT_ID,
                       COMBAT_WIN_LOSE_TEXT_FLAGS, 1);
    if (m_winLoseBottomTextWidgets[0] == 0)
        MemError();
    window->AddWidget(m_winLoseBottomTextWidgets[0], -1);

    gpCombatManager->m_winLoseWindow->DrawWindow();
    SAMPLE2 pickupSample = NULL_SAMPLE2;
    sprintf(gText, "pickup%02d.82M", SRandom(1, 5));
    pickupSample = LoadPlaySample(gText);
    WaitEndSample(pickupSample, -1);
}

// @early-stop
VA(0x0042dd97, 0x232)
void combatManager::ShowSkeletons(class heroWindow *window)
{
    tag_message message;
    char *skeletonCount;

    m_winLoseBottomWidgets[0] =
        new iconWidget(173, 270, 32, 30, "mons32.icn", 47, 0,
                       COMBAT_WIN_LOSE_SKELETON_ICON_ID,
                       COMBAT_WIN_LOSE_ICON_FLAGS, 1);
    if (m_winLoseBottomWidgets[0] == 0)
        MemError();

    skeletonCount = static_cast<char *>(
        BaseAlloc(9, COMMAND_SOURCE_FILE, COMMAND_SKELETON_ALLOC_LINE));
    sprintf(skeletonCount, "%d", giSkeletonsCreated);
    m_winLoseBottomTextWidgets[0] =
        new textWidget(165, 300, 32, 12, skeletonCount, "smalfont.fnt", 1,
                       COMBAT_WIN_LOSE_SKELETON_TEXT_ID,
                       COMBAT_WIN_LOSE_TEXT_FLAGS, 1);
    if (m_winLoseBottomTextWidgets[0] == 0)
        MemError();

    window->AddWidget(m_winLoseBottomWidgets[0], -1);
    window->AddWidget(m_winLoseBottomTextWidgets[0], -1);
    if (giSkeletonsCreated > 1) {
        sprintf(gText,
                "Practicing the dark arts of necromancy, you are able to "
                "raise %d of the enemy's dead to return under your service "
                "as Skeletons.",
                giSkeletonsCreated);
    } else {
        sprintf(gText,
                "Practicing the dark arts of necromancy, you are able to "
                "raise one of the enemy's dead to return under your service "
                "as a Skeleton.");
    }
    message.type = COMBAT_EVENT_WINDOW;
    message.payload.widget.command = COMBAT_WIN_LOSE_TEXT_COMMAND;
    message.payload.widget.id = COMBAT_WIN_LOSE_TEXT_ID;
    message.payload.widget.data.text = gText;
    m_winLoseWindow->BroadcastMessage(message);
    gpCombatManager->m_winLoseWindow->DrawWindow();

    SAMPLE2 pickupSample = NULL_SAMPLE2;
    sprintf(gText, "pickup%02d.82M", SRandom(1, 5));
    pickupSample = LoadPlaySample(gText);
    WaitEndSample(pickupSample, -1);
}

// @early-stop
VA(0x0042dfc9, 0x2f6)
void combatManager::ShowEagleEyeSpell(class heroWindow *window)
{
    int displayedSpell = m_eagleEyeSpell[m_combatResult];
    int x = 105;
    int y = 275;
    tag_message spellMessage;
    char *spellName;

    m_winLoseBottomWidgets[0] =
        new iconWidget(x + 16, y, 0, 0, "townwind.icn", 0, 0,
                       COMBAT_WIN_LOSE_EAGLE_BACKGROUND_ID,
                       COMBAT_WIN_LOSE_ICON_FLAGS, 1);
    if (m_winLoseBottomWidgets[0] == 0)
        MemError();

    m_winLoseBottomWidgets[1] =
        new iconWidget(x + 36, y + 5, 72, 51, "spells.icn",
                       static_cast<short>(
                           gsSpellInfo[displayedSpell].iconIndex),
                       0,
                       COMBAT_WIN_LOSE_EAGLE_SPELL_ID,
                       COMBAT_WIN_LOSE_SPELL_ICON_FLAGS, 1);
    if (m_winLoseBottomWidgets[1] == 0)
        MemError();

    spellName = static_cast<char *>(
        BaseAlloc(200, COMMAND_SOURCE_FILE, COMMAND_EAGLE_EYE_ALLOC_LINE));
    sprintf(spellName, "%s", gSpellNames[displayedSpell]);
    m_winLoseBottomTextWidgets[0] =
        new textWidget(x + 34, y + 57, 80, 24, spellName, "smalfont.fnt", 1,
                       COMBAT_WIN_LOSE_EAGLE_TEXT_ID,
                       COMBAT_WIN_LOSE_TEXT_FLAGS, 1);
    if (m_winLoseBottomTextWidgets[0] == 0)
        MemError();

    window->AddWidget(m_winLoseBottomWidgets[0], -1);
    window->AddWidget(m_winLoseBottomWidgets[1], -1);
    window->AddWidget(m_winLoseBottomTextWidgets[0], -1);
    sprintf(gText,
            "Through eagle-eyed observation, %s is able to learn the magic "
            "spell '%s'.",
            m_heroes[m_combatResult]->m_name,
            gSpellNames[displayedSpell]);
    spellMessage.type = COMBAT_EVENT_WINDOW;
    spellMessage.payload.widget.command = COMBAT_WIN_LOSE_TEXT_COMMAND;
    spellMessage.payload.widget.id = COMBAT_WIN_LOSE_TEXT_ID;
    spellMessage.payload.widget.data.text = gText;
    m_winLoseWindow->BroadcastMessage(spellMessage);
    gpCombatManager->m_winLoseWindow->DrawWindow();

    SAMPLE2 playedSample = NULL_SAMPLE2;
    sprintf(gText, "pickup%02d.82M", SRandom(1, 5));
    playedSample = LoadPlaySample(gText);
    WaitEndSample(playedSample, -1);
}

// @match-note retained/live 91.57%: the complete casualty/widget CFG compiles to
// 0xa22 bytes versus retail 0x9cc. Retail uses a 0x1a4 frame (`this` -0x1a4, quantity[42]
// -0x184, type[2][20] -0xac, army/side/y -0xdc..-0xd4, width/bottom
// -0xb4/-0xb0, spacing/text -0x0c/-0x08, window +0x08); ours is 0x188.
// First normalized residual is instruction 34, the side/army strength reduction.
// All 55 retail calls/relocations are reconstructed; offsets diverge after that
// loop, while external targets and local strings are accounted for. Direct and
// cached army access, cached versus four repeated GetIconEntry calls, and both
// index operand orders were tried. Revisit when total SOURCE fuzzy reaches 95%.
VA(0x0042e2bf, 0x9cc)
void combatManager::ShowDeadArmies(class heroWindow *window)
{
    int casualtyQuantity[COMBAT_CASUALTY_QUANTITY_STORAGE_COUNT];
    int casualtyType[COMBAT_MANAGER_SIDE_COUNT][COMBAT_ARMY_SLOT_COUNT];
    int side;
    int armyIndex;
    int y;
    short width = 320;
    short bottom = 458;
    int displayedCount;
    short spacing;
    short startX;
    char *text;
    icon *monsterIcons;

    for (armyIndex = 0; armyIndex < COMBAT_WIN_LOSE_WIDGET_COUNT;
         ++armyIndex) {
        m_winLoseBottomWidgets[armyIndex] = 0;
        m_winLoseBottomTextWidgets[armyIndex] = 0;
    }
    for (side = 0; side < COMBAT_MANAGER_SIDE_COUNT; ++side) {
        casualtyQuantity[side] = 0;
        for (armyIndex = 0; armyIndex < COMBAT_ARMY_SLOT_COUNT; ++armyIndex) {
            if (m_armies[side][armyIndex].m_monsterType != -1 &&
                m_armies[side][armyIndex].m_quantity <
                    m_armies[side][armyIndex].m_initialQuantity) {
                casualtyType[side][casualtyQuantity[side]] =
                    m_armies[side][armyIndex].m_monsterType;
                casualtyQuantity[COMBAT_MANAGER_SIDE_COUNT +
                                 side * COMBAT_ARMY_SLOT_COUNT +
                                 casualtyQuantity[side]] =
                    m_armies[side][armyIndex].m_initialQuantity -
                    m_armies[side][armyIndex].m_quantity;
                ++casualtyQuantity[side];
            }
        }
    }

    text = static_cast<char *>(BaseAlloc(
        30, COMMAND_SOURCE_FILE, COMMAND_CASUALTY_TITLE_ALLOC_LINE));
    sprintf(text, "Battlefield Casualties");
    m_winLoseBottomTextWidgets[17] =
        new textWidget(16, 263, width, 20, text, "smalfont.fnt", 1, 2110,
                       COMBAT_WIN_LOSE_TEXT_FLAGS, 1);
    if (m_winLoseBottomTextWidgets[17] == 0)
        MemError();
    window->AddWidget(m_winLoseBottomTextWidgets[17], -1);

    for (side = 0; side < COMBAT_MANAGER_SIDE_COUNT; ++side) {
        if (side == COMBAT_ATTACKER_SIDE)
            y = 279;
        else
            y = 346;
        text = static_cast<char *>(BaseAlloc(
            30, COMMAND_SOURCE_FILE, COMMAND_CASUALTY_HEADER_ALLOC_LINE));
        sprintf(text,
                side == COMBAT_ATTACKER_SIDE ? "Attacker" : "Defender");
        m_winLoseBottomTextWidgets[15 + side] =
            new textWidget(16, static_cast<short>(y) + 3, width, 20, text,
                           "smalfont.fnt", 1, 2110,
                           COMBAT_WIN_LOSE_TEXT_FLAGS, 1);
        if (m_winLoseBottomTextWidgets[15 + side] == 0)
            MemError();
        window->AddWidget(m_winLoseBottomTextWidgets[15 + side], -1);

        if (casualtyQuantity[side] < 1) {
            text = static_cast<char *>(BaseAlloc(
                10, COMMAND_SOURCE_FILE, COMMAND_CASUALTY_NONE_ALLOC_LINE));
            sprintf(text, "None");
            m_winLoseBottomTextWidgets[side * COMBAT_CASUALTY_WIDGETS_PER_SIDE] =
                new textWidget(16, static_cast<short>(y) + 21, width, 20,
                               text, "smalfont.fnt", 1, side * 5 + 2100,
                               COMBAT_WIN_LOSE_TEXT_FLAGS, 1);
            if (m_winLoseBottomTextWidgets
                    [side * COMBAT_CASUALTY_WIDGETS_PER_SIDE] == 0)
                MemError();
            window->AddWidget(
                m_winLoseBottomTextWidgets
                    [side * COMBAT_CASUALTY_WIDGETS_PER_SIDE],
                -1);
        }

        monsterIcons = gpResourceManager->GetIcon("mons32.icn");
        displayedCount = casualtyQuantity[side];
        if (displayedCount > COMBAT_CASUALTY_DISPLAY_LIMIT)
            displayedCount = COMBAT_CASUALTY_DISPLAY_LIMIT;
        spacing = 40;
        startX = static_cast<short>((320 - displayedCount * 40) / 2) + 3;
        for (armyIndex = 0; armyIndex < displayedCount; ++armyIndex) {
            m_winLoseBottomWidgets
                [side * COMBAT_CASUALTY_WIDGETS_PER_SIDE + armyIndex] =
                new iconWidget(
                    static_cast<short>(armyIndex) * spacing + startX -
                        GetIconEntry(monsterIcons,
                                     casualtyType[side][armyIndex])
                            ->x +
                        static_cast<short>(
                            (32 - GetIconEntry(
                                      monsterIcons,
                                      casualtyType[side][armyIndex])
                                      ->w) /
                            2) +
                        17,
                    (static_cast<short>(y) -
                     GetIconEntry(monsterIcons,
                                  casualtyType[side][armyIndex])
                         ->y -
                     GetIconEntry(monsterIcons,
                                  casualtyType[side][armyIndex])
                         ->h) +
                        51,
                    32, 28, "mons32.icn",
                    static_cast<short>(casualtyType[side][armyIndex]), 0,
                    static_cast<short>(side) * 5 +
                        static_cast<short>(armyIndex) + 2000,
                    COMBAT_WIN_LOSE_ICON_FLAGS, 1);
            if (m_winLoseBottomWidgets
                    [side * COMBAT_CASUALTY_WIDGETS_PER_SIDE + armyIndex] == 0)
                MemError();

            text = static_cast<char *>(BaseAlloc(
                9, COMMAND_SOURCE_FILE,
                COMMAND_CASUALTY_QUANTITY_ALLOC_LINE));
            sprintf(text, "%d",
                    casualtyQuantity[COMBAT_MANAGER_SIDE_COUNT +
                                     side * COMBAT_ARMY_SLOT_COUNT +
                                     armyIndex]);
            m_winLoseBottomTextWidgets
                [side * COMBAT_CASUALTY_WIDGETS_PER_SIDE + armyIndex] =
                new textWidget(static_cast<short>(armyIndex) * spacing +
                                   startX + 16,
                               static_cast<short>(y) + 53, 32, 12, text,
                               "smalfont.fnt", 1,
                               static_cast<short>(side) * 5 +
                                   static_cast<short>(armyIndex) + 2100,
                               COMBAT_WIN_LOSE_TEXT_FLAGS, 1);
            if (m_winLoseBottomTextWidgets
                    [side * COMBAT_CASUALTY_WIDGETS_PER_SIDE + armyIndex] == 0)
                MemError();
            window->AddWidget(
                m_winLoseBottomWidgets
                    [side * COMBAT_CASUALTY_WIDGETS_PER_SIDE + armyIndex],
                -1);
            window->AddWidget(
                m_winLoseBottomTextWidgets
                    [side * COMBAT_CASUALTY_WIDGETS_PER_SIDE + armyIndex],
                -1);
        }
        gpResourceManager->Dispose(monsterIcons);
    }
}

// @match-note retained/live 97.32%: the full winner/draw CFG compiles to 0xba8
// bytes versus retail 0xba9. Retail frame is 0x104 (`this` -0xf8, experienceText[152]
// -0xec, artifact/living/fade/army locals -0x54..-0x38, currentArmy -0x34,
// message -0x30..-0x18, experience/side/dead/last -0x14..-0x08, winningSide
// +0x08); ours is 0x100. First residual is instruction 52 (armyIndex/side
// strength-reduction order), repeated at 131. All 112 relocation sites and
// external targets were audited; later offset drift follows CFG/slot differences
// and retail-delinked constants. Multidimensional/flattened pointer forms, both
// fade predicates, and nested/combined artifact caps were tried. Revisit when
// total SOURCE fuzzy reaches 95%.
VA(0x0042ec8b, 0xba9)
void combatManager::DoVictory(int winningSide)
{
    char experienceText[152];
    int experienceLevels = 0;
    int deadCreatureCount;
    int eligibleWinnerStacks;
    int side;
    int armyIndex;
    int lastLivingArmy;
    int livingCreatureCount;
    army *currentArmy;
    int fadeCount;
    int fadeTimer;
    int fadeIndex;
    int emptyArtifactSlots;
    tag_message message;

    if (m_heroes[COMBAT_DEFENDER_SIDE] != 0 &&
        m_heroes[COMBAT_DEFENDER_SIDE]->m_isCaptain != 0)
        m_heroes[COMBAT_DEFENDER_SIDE] = 0;
    gbShowingLoseWindow = 0;
    gbWhichAnimationPlaying = 1;
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
        for (armyIndex = 0; armyIndex < gpCombatManager->m_armyCount[side];
             ++armyIndex) {
            currentArmy =
                side * COMBAT_ARMY_STORAGE_SLOT_COUNT +
                &m_armies[COMBAT_ATTACKER_SIDE][armyIndex];
            if (currentArmy->m_quantity > 0) {
                lastLivingArmy = armyIndex;
                if (currentArmy->m_temporaryResurrectionQuantity > 0)
                    currentArmy->m_quantity -=
                        currentArmy->m_temporaryResurrectionQuantity;
                if (currentArmy->m_quantity < 0)
                    currentArmy->m_quantity = 0;
                livingCreatureCount += currentArmy->m_quantity;
            }
            if (side == winningSide && currentArmy->m_quantity > 0 &&
                (currentArmy->m_monster.flags.all &
                 MONSTER_FLAGS_LIGHT_PALETTE) == 0 &&
                currentArmy->m_monsterType != ARMY_CREATURE_EARTH_ELEMENTAL &&
                currentArmy->m_monsterType != ARMY_CREATURE_AIR_ELEMENTAL &&
                currentArmy->m_monsterType != ARMY_CREATURE_FIRE_ELEMENTAL &&
                currentArmy->m_monsterType != ARMY_CREATURE_WATER_ELEMENTAL &&
                currentArmy->m_monsterType != ARMY_CREATURE_GHOST) {
                ++eligibleWinnerStacks;
            }
            if (winningSide == COMBAT_DEFENDER_SIDE - side) {
                deadCreatureCount += currentArmy->m_initialQuantity -
                                     currentArmy->m_quantity;
            }
        }
        if (livingCreatureCount == 0 && lastLivingArmy != -1)
            (side * COMBAT_ARMY_STORAGE_SLOT_COUNT +
             &m_armies[COMBAT_ATTACKER_SIDE][lastLivingArmy])
                ->m_quantity = 1;
    }

    if (winningSide != COMBAT_RESULT_DRAW &&
        eligibleWinnerStacks < COMBAT_VICTORY_NECROMANCY_STACK_LIMIT &&
        m_heroes[winningSide] != 0 &&
        m_heroes[winningSide]->GetSSLevel(HERO_SKILL_NECROMANCY) != 0) {
        giSkeletonsCreated = static_cast<int>(
            deadCreatureCount *
            (m_heroes[winningSide]->GetSSLevel(HERO_SKILL_NECROMANCY) *
             COMBAT_NECROMANCY_LEVEL_FACTOR));
        if (giSkeletonsCreated <= 0 && deadCreatureCount != 0)
            giSkeletonsCreated = 1;
    }

    m_nonVisualCombat = 1;
    FreeArmies();
    CombatMessage("", 1, 1, 0);
    gpMouseManager->SetPointer(COMBAT_POINTER_DEFAULT);
    fadeCount = COMBAT_VICTORY_FADE_STEPS;
    if (m_terrainType == COMBAT_VICTORY_WATER_TERRAIN)
        fadeCount = COMBAT_VICTORY_WATER_FADE_STEPS;
    fadeTimer = KBTickCount();
    for (fadeIndex = 0; fadeCount > fadeIndex; ++fadeIndex) {
        PollSound();
        DelayTil(&fadeTimer);
        fadeTimer = KBTickCount() + COMBAT_VICTORY_FADE_DELAY;
        DimBitmapArea(gpWindowManager->m_screen, 0, 0,
                      COMBAT_SCREEN_WIDTH, COMBAT_SCREEN_HEIGHT, 3);
        PollSound();
        gpWindowManager->UpdateScreenRegion(0, 0, COMBAT_SCREEN_MAX_X,
                                            COMBAT_SCREEN_MAX_Y);
        PollSound();
    }

    switch (winningSide) {
    case COMBAT_RESULT_DRAW:
        gpSoundManager->SwitchAmbientMusic(COMBAT_LOSS_MUSIC);
        DoLoseWindow();
        break;
    case COMBAT_ATTACKER_SIDE:
    case COMBAT_DEFENDER_SIDE:
        if (m_heroes[winningSide] != 0) {
            if (m_eagleEyeSpell[winningSide] != HERO_SPELL_NONE) {
                m_heroes[winningSide]
                    ->m_spells[m_eagleEyeSpell[winningSide]] = 1;
            }
            m_experienceValue[COMBAT_DEFENDER_SIDE - winningSide] =
                ExperienceValueOfStack(COMBAT_DEFENDER_SIDE - winningSide);
            if (gbRetreatWin != 0)
                m_experienceValue[COMBAT_DEFENDER_SIDE - winningSide] -=
                    COMBAT_HERO_EXPERIENCE_VALUE;
            if (m_combatTowns[COMBAT_DEFENDER_SIDE] != 0 &&
                winningSide == COMBAT_ATTACKER_SIDE)
                m_experienceValue[COMBAT_DEFENDER_SIDE - winningSide] +=
                    COMBAT_HERO_EXPERIENCE_VALUE;
            experienceLevels = gpAdvManager->GiveExperience(
                m_heroes[winningSide],
                m_experienceValue[COMBAT_DEFENDER_SIDE - winningSide],
                gbThisNetHumanPlayer[m_heroes[winningSide]->m_owner] == 0);

            if (gbRetreatWin == 0) {
                emptyArtifactSlots = 0;
                if (m_heroes[COMBAT_ATTACKER_SIDE] != 0 &&
                    m_heroes[COMBAT_DEFENDER_SIDE] != 0) {
                    for (fadeIndex = 0; fadeIndex < HERO_ARTIFACT_SLOT_COUNT;
                         ++fadeIndex) {
                        if (m_heroes[winningSide]->m_artifacts[fadeIndex] ==
                            HERO_ARTIFACT_NONE) {
                            ++emptyArtifactSlots;
                        }
                    }
                    for (fadeIndex = 0; fadeIndex < HERO_ARTIFACT_SLOT_COUNT;
                         ++fadeIndex) {
                        if (m_heroes[COMBAT_DEFENDER_SIDE - winningSide]
                                    ->m_artifacts[fadeIndex] >=
                                HERO_ARTIFACT_TRANSFERABLE_FIRST &&
                            m_heroes[COMBAT_DEFENDER_SIDE - winningSide]
                                    ->m_artifacts[fadeIndex] !=
                                HERO_ARTIFACT_MAGIC_BOOK &&
                            emptyArtifactSlots > iMaxTransferArtifacts) {
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

        if (!(giCurPlayer == -1 ||
              gbThisNetHumanPlayer[giCurPlayer] == 0 ||
              m_playerId[winningSide] != giCurPlayer) ||
            !(giCurPlayer == -1 || m_playerId[winningSide] == -1 ||
              gbThisNetHumanPlayer[giCurPlayer] != 0 ||
              gbThisNetHumanPlayer[m_playerId[winningSide]] == 0) ||
            !(m_playerId[winningSide] == -1 ||
              gbThisNetHumanPlayer[m_playerId[winningSide]] == 0)) {
            gpSoundManager->SwitchAmbientMusic(COMBAT_VICTORY_MUSIC);
            m_winLoseWindow = new heroWindow(143, 10, "wincmbt.bin");
            if (m_winLoseWindow == 0)
                MemError();

            if (m_heroes[winningSide] != 0) {
                if (gbCombatSurrender != 0) {
                    sprintf(gText, cBattleResults[
                                       COMBAT_RESULT_TEXT_ENEMY_SURRENDERED]);
                } else if (gbRetreatWin != 0) {
                    sprintf(gText,
                            cBattleResults[COMBAT_RESULT_TEXT_ENEMY_FLED]);
                } else {
                    sprintf(gText,
                            cBattleResults[COMBAT_RESULT_TEXT_VICTORY]);
                }
                if (experienceLevels > 0 &&
                    winningSide == COMBAT_DEFENDER_SIDE &&
                    giNumHumanPlayers > 1) {
                    sprintf(experienceText,
                            cBattleResults[
                                COMBAT_RESULT_TEXT_NETWORK_EXPERIENCE],
                            m_heroes[winningSide]->m_name,
                            m_experienceValue
                                [COMBAT_DEFENDER_SIDE - winningSide],
                            experienceLevels);
                } else {
                    sprintf(experienceText,
                            cBattleResults[COMBAT_RESULT_TEXT_EXPERIENCE],
                            m_heroes[winningSide]->m_name,
                            m_experienceValue
                                [COMBAT_DEFENDER_SIDE - winningSide]);
                }
                strcat(gText, experienceText);
                m_heroes[winningSide]->ApplyBattleWinTemps();
            } else {
                if (gbCombatSurrender != 0) {
                    sprintf(gText, cBattleResults[
                                       COMBAT_RESULT_TEXT_ENEMY_SURRENDERED]);
                } else if (gbRetreatWin != 0) {
                    sprintf(gText,
                            cBattleResults[COMBAT_RESULT_TEXT_ENEMY_FLED]);
                } else {
                    sprintf(gText,
                            cBattleResults[COMBAT_RESULT_TEXT_VICTORY]);
                }
            }
            message.type = COMBAT_EVENT_WINDOW;
            message.payload.widget.command = COMBAT_WIN_LOSE_TEXT_COMMAND;
            message.payload.widget.id = COMBAT_WIN_LOSE_TEXT_ID;
            message.payload.widget.data.text = gText;
            m_winLoseWindow->BroadcastMessage(message);
            ShowDeadArmies(m_winLoseWindow);
            if (gbRemoteOn != 0 && gbThisNetGotAdventureControl == 0)
                giDialogTimeout = KBTickCount() + 15000;
            gpWindowManager->DoDialog(m_winLoseWindow, WinCombatHandler, 0);
            giDialogTimeout = 0;
            delete m_winLoseWindow;
            if (m_heroes[COMBAT_DEFENDER_SIDE - winningSide] != 0)
                m_heroes[COMBAT_DEFENDER_SIDE - winningSide]
                    ->ApplyBattleLossTemps();
        } else {
            if (m_heroes[winningSide] != 0)
                m_heroes[winningSide]->ApplyBattleWinTemps();
            if (m_heroes[COMBAT_DEFENDER_SIDE - winningSide] != 0)
                m_heroes[COMBAT_DEFENDER_SIDE - winningSide]
                    ->ApplyBattleLossTemps();
            gpSoundManager->SwitchAmbientMusic(COMBAT_LOSS_MUSIC);
            DoLoseWindow();
        }
        break;
    }
    gMapX = gpAdvManager->m_mapOriginX;
    gMapY = gpAdvManager->m_mapOriginY;
}

// @match-note retained/live 99.74%: retail/ours both span 0x3bc and the complete
// dialog CFG agrees. Retail frame is 0x68 (`this` -0x68, losingSide -0x58,
// message -0x54..-0x3c, animationFile[52] -0x38); ours is 0x64 with the same
// named values in hash-shifted slots. Raw first residual is +0x05 (frame byte);
// normalized instruction 51 first differs only in a string relocation identity.
// All 57/57 relocation offsets align and external targets agree; ten identities
// are retail-delinked strings/cBattleResults aliases. Hero-first/no-hero-first
// arms and surrender/retreat polarity forms were tried. Revisit when total
// SOURCE fuzzy reaches 95%.
VA(0x0042f834, 0x3bc)
void combatManager::DoLoseWindow(void)
{
    int losingSide = COMBAT_ATTACKER_SIDE;
    char animationFile[52];
    tag_message message;

    if (m_playerId[COMBAT_ATTACKER_SIDE] == giCurPlayer &&
        gbThisNetHumanPlayer[m_playerId[COMBAT_ATTACKER_SIDE]] != 0) {
        losingSide = COMBAT_ATTACKER_SIDE;
    } else if (m_playerId[COMBAT_DEFENDER_SIDE] == giCurPlayer &&
               gbThisNetHumanPlayer[m_playerId[COMBAT_DEFENDER_SIDE]] != 0) {
        losingSide = COMBAT_DEFENDER_SIDE;
    } else if (m_playerId[COMBAT_ATTACKER_SIDE] != -1 &&
               gbThisNetHumanPlayer[m_playerId[COMBAT_ATTACKER_SIDE]] != 0) {
        losingSide = COMBAT_ATTACKER_SIDE;
    } else {
        losingSide = COMBAT_DEFENDER_SIDE;
    }

    gbShowingLoseWindow = 1;
    if (gbCombatSurrender != 0) {
        sprintf(animationFile, "cmbtsurr.icn");
        gbWhichAnimationPlaying = COMBAT_WIN_LOSE_ANIMATION_CYCLE_SECOND;
    } else if (gbRetreatWin != 0) {
        sprintf(animationFile, "cmbtfle1.icn");
        gbWhichAnimationPlaying = COMBAT_WIN_LOSE_ANIMATION_FLEE;
    } else {
        sprintf(animationFile, "cmbtlos1.icn");
        gbWhichAnimationPlaying = COMBAT_WIN_LOSE_ANIMATION_LOSS;
    }

    m_winLoseWindow = new heroWindow(143, 10, "wincmbt.bin");
    if (m_winLoseWindow == 0)
        MemError();

    if (m_heroes[losingSide] != 0) {
        if (gbCombatSurrender != 0) {
            sprintf(gText, cBattleResults[COMBAT_RESULT_TEXT_HERO_SURRENDER],
                    m_heroes[losingSide]->m_name);
        } else if (gbRetreatWin != 0) {
            sprintf(gText, cBattleResults[COMBAT_RESULT_TEXT_HERO_FLEE],
                    m_heroes[losingSide]->m_name);
        } else {
            sprintf(gText, cBattleResults[COMBAT_RESULT_TEXT_HERO_DEFEAT],
                    m_heroes[losingSide]->m_name);
        }
    } else {
        if (gbCombatSurrender != 0) {
            sprintf(gText,
                    cBattleResults[COMBAT_RESULT_TEXT_FORCES_SURRENDER]);
        } else if (gbRetreatWin != 0) {
            sprintf(gText, cBattleResults[COMBAT_RESULT_TEXT_FORCES_FLEE]);
        } else {
            sprintf(gText, cBattleResults[COMBAT_RESULT_TEXT_FORCES_DEFEAT]);
        }
    }

    message.type = COMBAT_EVENT_WINDOW;
    message.payload.widget.command = COMBAT_WIN_LOSE_RESOURCE_COMMAND;
    message.payload.widget.id = COMBAT_WIN_LOSE_RESOURCE_LOAD_ID;
    message.payload.widget.data.text = animationFile;
    m_winLoseWindow->BroadcastMessage(message);
    message.payload.widget.id = COMBAT_WIN_LOSE_RESOURCE_DRAW_ID;
    message.payload.widget.data.text = animationFile;
    m_winLoseWindow->BroadcastMessage(message);
    message.type = COMBAT_EVENT_WINDOW;
    message.payload.widget.command = COMBAT_WIN_LOSE_TEXT_COMMAND;
    message.payload.widget.id = COMBAT_WIN_LOSE_TEXT_ID;
    message.payload.widget.data.text = gText;
    m_winLoseWindow->BroadcastMessage(message);
    ShowDeadArmies(m_winLoseWindow);
    if (gbRemoteOn != 0 && gbThisNetGotAdventureControl == 0)
        giDialogTimeout = KBTickCount() + 15000;
    gpWindowManager->DoDialog(m_winLoseWindow, WinCombatHandler, 0);
    giDialogTimeout = 0;
    delete m_winLoseWindow;
    m_winLoseWindow = 0;
}

// @match-note retained/live 99.87%: the 0x54 frame, 0x43d span, and complete
// cost/dialog CFG agree. Retail slots are `this` -0x44, armyIndex -0x2c,
// message -0x28..-0x10, window -0x0c, textWidth -0x08; ours places armyIndex
// at -0x08. Raw first residual is +0x18 (that slot displacement); normalized
// instruction 113 first names an equivalent local 0.1 constant. All 40/40
// relocation offsets align and external targets agree; seven identities are
// retail-delinked float/string constants. Artifact branch polarity and float/
// double factor spellings were tried. Revisit when total SOURCE fuzzy reaches 95%.
VA(0x0042fbf0, 0x43d)
int combatManager::DoSurrender(void)
{
    int armyIndex;
    short dialogType;
    short dialogResult;
    short textWidth;
    heroWindow *window;
    tag_message message;

    giSurrenderCost = 0;
    for (armyIndex = 0; armyIndex < COMBAT_ARMY_SLOT_COUNT; ++armyIndex) {
        if (m_armies[m_currentSide][armyIndex].IsAlive()) {
            giSurrenderCost +=
                static_cast<short>(gMonsterDatabase[
                    m_armies[m_currentSide][armyIndex].m_monsterType].cost) *
                m_armies[m_currentSide][armyIndex].m_quantity;
        }
    }
    if (m_heroes[m_currentSide]->HasArtifact(
            HERO_ARTIFACT_STATESMANS_QUILL) != 0)
        giSurrenderCost =
            static_cast<int>(giSurrenderCost * COMBAT_SURRENDER_QUILL_FACTOR);
    else
        giSurrenderCost =
            static_cast<int>(giSurrenderCost * COMBAT_SURRENDER_BASE_FACTOR);
    giSurrenderCost = static_cast<int>(
        giSurrenderCost *
        (1.0 - m_heroes[m_currentSide]
                       ->m_secondarySkills[HERO_SKILL_DIPLOMACY] *
                   COMBAT_SURRENDER_DIPLOMACY_FACTOR));

    dialogType = COMBAT_SURRENDER_DIALOG_TYPE;
    dialogResult = COMBAT_SURRENDER_DIALOG_ACCEPT_RESULT;
    textWidth = COMBAT_SURRENDER_TEXT_WIDTH;
    window = new heroWindow(74, 80, "surrendr.bin");
    if (window == 0)
        MemError();
    message.type = COMBAT_EVENT_WINDOW;
    message.payload.widget.command = COMBAT_WIN_LOSE_RESOURCE_COMMAND;
    message.payload.widget.id = COMBAT_SURRENDER_PORTRAIT_RESOURCE_ID;
    sprintf(gText, "port%04d.icn",
            static_cast<unsigned int>(
                m_heroes[COMBAT_DEFENDER_SIDE - m_currentSide]->m_portrait));
    message.payload.widget.data.text = gText;
    window->BroadcastMessage(message);
    if (m_heroes[COMBAT_DEFENDER_SIDE - m_currentSide]->m_isCaptain != 0)
        message.payload.widget.command = COMBAT_SURRENDER_CAPTAIN_PORTRAIT_COMMAND;
    else
        message.payload.widget.command = COMBAT_SURRENDER_HERO_PORTRAIT_COMMAND;
    message.payload.widget.id = COMBAT_SURRENDER_PORTRAIT_WIDGET_ID;
    message.payload.widget.data.value = COMBAT_SURRENDER_PORTRAIT_DEFAULT_COLOR;
    window->BroadcastMessage(message);
    if (m_heroes[COMBAT_DEFENDER_SIDE - m_currentSide]->m_isCaptain != 0) {
        message.payload.widget.command = COMBAT_SURRENDER_CAPTAIN_OVERLAY_COMMAND;
        if (m_playerId[COMBAT_DEFENDER_SIDE - m_currentSide] == -1)
            message.payload.widget.data.value = COMBAT_SURRENDER_CAPTAIN_NEUTRAL_COLOR;
        else
            message.payload.widget.data.value =
                gpGame->m_players
                    [m_playerId[COMBAT_DEFENDER_SIDE - m_currentSide]]
                        .color;
        window->BroadcastMessage(message);
    }
    message.payload.widget.data.text = gText;
    message.payload.widget.command = COMBAT_WIN_LOSE_TEXT_COMMAND;
    message.payload.widget.id = COMBAT_SURRENDER_TEXT_ID;
    sprintf(gText, "%s states:\n\n\"I will accept your surrender and grant you and your troops safe passage for the price of %d gold.\"",
            m_heroes[COMBAT_DEFENDER_SIDE - m_currentSide]->m_name,
            giSurrenderCost);
    window->BroadcastMessage(message);
    gpWindowManager->DoDialog(window, TrueFalseDialogHandler, 0);
    delete window;
    return gpWindowManager->m_dialogResult == NORMAL_DIALOG_BUTTON_TWO;
}

// @early-stop
// All 0xdc bytes match after masking five aligned COFF
// relocations; relocation offsets and targets are identical.
VA(0x0043002d, 0xdc)
void combatManager::CheckChangeSelector(void)
{
    if (gbNoShowCombat != 0)
        return;
    army *currentArmy = &m_armies[m_currentArmySide][m_currentArmyIndex];
    if (m_limitCreature == 0 || currentArmy->m_hex != m_limitCreatureHex) {
        UpdateGrid(0, 1);
        giNewMonsterCycleFrame = 7;
        m_limitCreatureHex = currentArmy->m_hex;
        m_limitCreature = 1;
        DrawFrame(1, 0, 0, 0, COMBAT_COMMAND_FRAME_DELAY, 1, 1);
    }
    SetupSmallView();
}

// @early-stop
// All 0xea bytes match after masking four aligned COFF
// relocations; relocation offsets and targets are identical.
VA(0x00430109, 0xea)
void combatManager::CheckCastleAttack(void)
{
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

// @match-note retained/live 99.91%: the exact 0x08 frame (`this` -0x08,
// retreat -0x04), argument-free ABI, 0xdd span, and full AI/spell/retreat CFG
// agree. Normalized instruction 28 first differs because retail delinks
// gConfig.autoCombatUseSpells as an interior local alias; the first masked raw
// residual is +0xbd, where the retreat arm lands on the equivalent trailing
// local jump. All 7/7 relocation offsets and six external identities agree.
// Direct field access and equivalent predicate/early-return polarities were
// compared. Revisit when total SOURCE fuzzy reaches 95%.
VA(0x004301f3, 0xdd)
void combatManager::CheckGetAIMove(void)
{
    int retreat = AICheckRetreat();
    if (m_heroCastSpell[m_currentSide] == 0 &&
        (m_playerId[m_currentSide] == -1 ||
         gbThisNetHumanPlayer[m_playerId[m_currentSide]] == 0 ||
         gConfig.autoCombatUseSpells != 0)) {
        if (DoSpellAI(m_currentSide, retreat) != 0)
            return;
    }
    retreat = AICheckRetreat();
    if (retreat != 0)
        giNextAction = COMBAT_ACTION_RETREAT;
    else
        DoCompAI(m_currentSide);
}

// @match-note retained/live 96.52%: the complete network-control CFG compiles to
// 0x185 bytes versus retail 0x18f, with the exact 0x04 frame (`this` -0x04; no explicit
// arguments). Retail proves two consecutive stores to m_previousCommand at
// +0xf2cb; +0xf2cf is the distinct current command and is not written here.
// First normalized residual is instruction 53/raw +0xae, a retail continuation
// jump before the remote-player arm; another occurs at exit. All 15/15 external
// relocations are accounted for, with offsets drifting at those jumps. Nested,
// compound, De Morgan, and opposite-polarity network predicates were tried.
// Revisit when total SOURCE fuzzy reaches 95%.
VA(0x004302d0, 0x18f)
void combatManager::GetControl(void)
{
    m_selectedHex = COMBAT_INVALID_HEX;
    // Retail emits two stores to +0xf2cb here; +0xf2cf is not the second target.
    m_previousCommand = COMBAT_INVALID_COMMAND;
    m_previousCommand = COMBAT_INVALID_COMMAND;
    if (gpCombatManager->m_active == 1)
        gpMouseManager->SetPointer(COMBAT_POINTER_DEFAULT);
    CheckChangeSelector();
    if (gbRemoteOn == 0 || m_playerId[COMBAT_ATTACKER_SIDE] < 0 ||
        m_playerId[COMBAT_DEFENDER_SIDE] < 0 ||
        gbHumanPlayer[m_playerId[COMBAT_DEFENDER_SIDE]] == 0 ||
        (gbHumanPlayer[m_playerId[COMBAT_ATTACKER_SIDE]] == 0 &&
         (gbHumanPlayer[m_playerId[COMBAT_ATTACKER_SIDE]] != 0 ||
          m_playerId[COMBAT_DEFENDER_SIDE] == 0))) {
        gbThisNetHasControl = 1;
    } else if (m_playerId[m_currentSide] == -1 ||
               gbHumanPlayer[m_playerId[m_currentSide]] == 0 ||
               gbThisNetHumanPlayer[m_playerId[m_currentSide]] != 0) {
        gbThisNetHasControl = 1;
    } else {
        gbThisNetHasControl = 0;
    }
    m_smallViewSide[COMBAT_DEFENDER_SIDE] = -1;
    SetupSmallView();
    ResetMouse();
}

// @match-note retained/live 99.81%: normalized assembly, the 0x28 frame, full
// control/AI CFG, and 0xd7 span agree. Retail reuses `this` -0x28 for mouseY
// after the guard, with mouseX -0x24 and message -0x20..-0x0c; there are no
// explicit arguments. First masked raw residual is +0x7f in the MouseCoords/
// message slot displacements. All 9/9 relocation offsets and targets are exact.
// Mouse X/Y declaration order, direct message assignments, and guard/else
// polarity were compared. Revisit when total SOURCE fuzzy reaches 95%.
VA(0x0043045f, 0xd7)
void combatManager::ResetMouse(void)
{
    int mouseY;
    int mouseX;
    tag_message message;

    if (gbNoShowCombat != 0)
        return;
    if (gbThisNetHasControl != 0 && m_playerId[m_currentSide] >= 0 &&
        gbHumanPlayer[m_playerId[m_currentSide]] != 0) {
        m_selectedHex = COMBAT_INVALID_HEX;
        ClearCombatMessages(0);
        gpMouseManager->MouseCoords(mouseX, mouseY);
        message.type = COMBAT_EVENT_MOUSE_MOVE;
        message.payload.mouse.modifiers = mouseX;
        message.payload.mouse.x = mouseX;
        message.payload.mouse.screenY = mouseY;
        message.payload.mouse.y = mouseY;
        ProcessCombatMsg(message);
    } else {
        gpMouseManager->SetPointer(COMBAT_POINTER_DEFAULT);
    }
}

// @match-note retained/live 95.79%: the complete eight-action
// switch/finish CFG compiles to 0x642 bytes versus retail 0x65b and uses the
// exact 0x2c frame. Retail slots
// are `this` -0x28, actionData -0x24..-0x18, transmitResult -0x14,
// currentArmy -0x10, redraw -0x0c, result -0x08, advanceArmy -0x04, message
// +0x08; ours first places result at -0x04. Raw first residual is +0x18; first
// normalized residual is instruction 33, current-side/current-army strength
// reduction before LogInt. All 82 calls/relocations are reconstructed; offset
// order diverges around that arithmetic and the switch table, with external
// targets audited. Retail body order, pointer/direct army access, and switch arm
// order were tried. Revisit when total SOURCE fuzzy reaches 95%.
VA(0x00430536, 0x65b)
int combatManager::ProcessNextAction(struct tag_message &message)
{
    int actionData[COMBAT_ACTION_DATA_COUNT];
    int transmitResult;
    army *currentArmy;
    int advanceArmy;
    int redraw;
    int result;

    ClearCombatMessages(0);
    result = COMBAT_MAIN_CONTINUE;
    redraw = 0;
    gbProcessingCombatAction = 1;
    if (m_smallViewSide[COMBAT_ATTACKER_SIDE] != -1 ||
        m_smallViewSide[COMBAT_DEFENDER_SIDE] != -1) {
        m_smallViewSide[COMBAT_DEFENDER_SIDE] = -1;
        m_smallViewSide[COMBAT_ATTACKER_SIDE] =
            m_smallViewSide[COMBAT_DEFENDER_SIDE];
        redraw = 1;
    }
    if (giNextAction != COMBAT_ACTION_NONE) {
        LogInt("Process Act", giNextAction, giNextActionGridIndex,
               giNextActionGridIndex2, giNextActionExtra, m_currentArmySide,
               m_currentArmyIndex,
               m_armies[m_currentArmySide][m_currentArmyIndex].m_hex);
    }
    gpMouseManager->SetPointer(COMBAT_POINTER_DEFAULT);
    UpdateMouseGrid(-1, 1);
    memset(m_gridState, 0, sizeof(m_gridState));
    if (UpdateGrid(0, 0) != 0)
        redraw = 1;
    if (redraw != 0)
        DrawFrame(1, 0, 0, 0, COMBAT_COMMAND_FRAME_DELAY, 1, 1);

    if (gbThisNetHasControl != 0 && gbRemoteOn != 0 &&
        m_playerId[COMBAT_ATTACKER_SIDE] >= 0 &&
        m_playerId[COMBAT_DEFENDER_SIDE] >= 0 &&
        gbHumanPlayer[m_playerId[COMBAT_DEFENDER_SIDE]] != 0 &&
        gbHumanPlayer[m_playerId[COMBAT_ATTACKER_SIDE]] != 0) {
        actionData[COMBAT_ACTION_DATA_ACTION] = giNextAction;
        actionData[COMBAT_ACTION_DATA_EXTRA] = giNextActionExtra;
        actionData[COMBAT_ACTION_DATA_GRID] = giNextActionGridIndex;
        actionData[COMBAT_ACTION_DATA_SECOND_GRID] = giNextActionGridIndex2;
        LogInt("About to T",
               reinterpret_cast<int *>(&iCombatControlNetPos)
                   [COMBAT_DEFENDER_SIDE - m_currentSide],
               -999, -999, -999, -999, -999, -999);
        transmitResult = TransmitRemoteData(
            reinterpret_cast<char *>(actionData),
            reinterpret_cast<int *>(&iCombatControlNetPos)
                [COMBAT_DEFENDER_SIDE - m_currentSide],
            sizeof(actionData), COMBAT_REMOTE_COMMAND_ACTION, 1, 1, -1);
        LogStr("Post T");
        if (transmitResult == 0)
            ShutDown(0);
    }

    currentArmy = &m_armies[m_currentArmySide][m_currentArmyIndex];
    advanceArmy = 0;
    if (CheckWin(&message) == 0) {
        switch (giNextAction) {
        case COMBAT_ACTION_NONE:
            break;
        case COMBAT_ACTION_CAST_SPELL:
            ResetCyclingCreatures();
            CastSpell(giNextActionExtra, giNextActionGridIndex, 0,
                      giNextActionGridIndex2);
            if (m_armies[m_currentArmySide][m_currentArmyIndex].m_quantity < 1)
                advanceArmy = 1;
            ResetCycleTimers();
            break;
        case COMBAT_ACTION_MOVE:
            ResetCyclingCreatures();
            currentArmy->MoveAttack(giNextActionGridIndex, 0);
            currentArmy->m_monster.flags.all |=
                MONSTER_ABILITY_FLAG_BAD_MORALE;
            if (CheckWin(&message) != 0) {
                result = COMBAT_MAIN_FINISHED;
                goto Finished;
            }
            CheckApplyGoodMorale(m_currentArmySide, m_currentArmyIndex);
            advanceArmy = 1;
            ResetCycleTimers();
            break;
        case COMBAT_ACTION_ATTACK:
            ResetCyclingCreatures();
            if (giNextActionExtra != -1 &&
                currentArmy->m_hex != giNextActionExtra) {
                currentArmy->MoveAttack(giNextActionExtra, 1);
            }
            currentArmy->MoveAttack(giNextActionGridIndex, 0);
            currentArmy->m_monster.flags.all |=
                MONSTER_ABILITY_FLAG_BAD_MORALE;
            if (CheckWin(&message) != 0) {
                result = COMBAT_MAIN_FINISHED;
                goto Finished;
            }
            CheckApplyGoodMorale(m_currentArmySide, m_currentArmyIndex);
            advanceArmy = 1;
            ResetCycleTimers();
            break;
        case COMBAT_ACTION_RETREAT:
            m_sideRetreated[m_currentSide] = 1;
            gbRetreatWin = 1;
            ResetCycleTimers();
            break;
        case COMBAT_ACTION_SURRENDER:
            gbCombatSurrender = 1;
            gbRetreatWin = 1;
            m_sideDefeated[m_currentSide] = 1;
            gpGame->m_players[m_playerId[m_currentSide]].resources[RES_GOLD] -=
                giNextActionExtra;
            gpGame->m_players
                [m_playerId[COMBAT_DEFENDER_SIDE - m_currentSide]]
                .resources[RES_GOLD] += giNextActionExtra;
            ResetCycleTimers();
            break;
        case COMBAT_ACTION_WAIT:
            currentArmy->m_monster.flags.all |=
                MONSTER_ABILITY_FLAG_BAD_MORALE;
            advanceArmy = 1;
            break;
        case COMBAT_ACTION_DEFEND:
            currentArmy->m_monster.flags.all |=
                MONSTER_ABILITY_FLAG_DEFERRED_TURN;
            advanceArmy = 1;
            break;
        }
        giNextAction = COMBAT_ACTION_NONE;
        if (CheckWin(&message) == 0) {
            TestRaiseDoor();
            if (advanceArmy != 0 && GetNextArmy(1) == 0) {
                ResetRound();
                GetNextArmy(1);
            }
            CheckChangeSelector();
        } else {
            result = COMBAT_MAIN_FINISHED;
        }
    }

Finished:
    gbProcessingCombatAction = 0;
    ResetMouse();
    return result;
}

// @match-note retained/live 99.79%:
// the full two-pass CFG and 0x237 span are reconstructed. Retail frame is 0x18
// (`this` -0x18, armyIndex -0x14, side -0x0c, cyclingCount -0x08,
// currentArmy -0x04); current live frame is 0x14. First normalized residual is
// instruction 24/raw +0x05, side/army strength reduction after the frame byte.
// All 13/13 external relocation targets are present. Direct versus cached army
// access, byte/all flag tests, >7/<13 versus inclusive bounds, pointer
// recomputation, and empty-if polarity reached the retained identical-assembly
// maximum. Revisit when total SOURCE fuzzy reaches 95%.
VA(0x00430b91, 0x237)
void combatManager::ResetCyclingCreatures(void)
{
    army *currentArmy = 0;
    int cyclingCount = 0;
    int side;
    int index;

    for (side = 0; side < COMBAT_MANAGER_SIDE_COUNT; ++side) {
        for (index = 0; index < gpCombatManager->m_armyCount[side]; ++index) {
            currentArmy = gpCombatManager->m_armies[side] + index;
            if ((currentArmy->m_monster.flags.all &
                 MONSTER_ABILITY_FLAG_AI_EXCLUDED) == 0 &&
                currentArmy->m_animationSequence >=
                    COMBAT_CREATURE_CYCLE_SEQUENCE_FIRST &&
                currentArmy->m_animationSequence <=
                    COMBAT_CREATURE_CYCLE_SEQUENCE_LAST) {
                ++cyclingCount;
                ++gpCombatManager->m_limitCreatureCount[side][index];
            }
        }
    }
    if (cyclingCount == 0) {
    } else {
        gpCombatManager->DrawFrame(0, 1, 1, 1,
                                   COMBAT_COMMAND_FRAME_DELAY, 1, 1);
        for (side = 0; side < COMBAT_MANAGER_SIDE_COUNT; ++side) {
            for (index = 0; index < gpCombatManager->m_armyCount[side];
                 ++index) {
                currentArmy = gpCombatManager->m_armies[side] + index;
                if ((currentArmy->m_monster.flags.all &
                     MONSTER_ABILITY_FLAG_AI_EXCLUDED) == 0) {
                    currentArmy = gpCombatManager->m_armies[side] + index;
                    currentArmy->m_animationSequence =
                        ARMY_ANIMATION_STAND;
                    currentArmy->m_animationFrame = 0;
                    currentArmy->m_lastAnimationTime = KBTickCount();
                }
            }
        }
        m_heroCycleTimer[COMBAT_ATTACKER_SIDE] = KBTickCount();
        m_heroCycleTimer[COMBAT_DEFENDER_SIDE] = KBTickCount();
        gpCombatManager->DrawFrame(1, 1, 0, 0,
                                   COMBAT_COMMAND_FRAME_DELAY, 1, 1);
    }
}

// @match-note retained/live 99.83%: normalized assembly, complete nested-loop
// CFG, exact 0x14 frame, and 0xf9 span agree. Retail slots are `this` -0x14,
// currentTime -0x10 with side/index reuse at -0x08/-0x10; ours keeps
// currentTime at -0x04. First masked raw residual is +0x13 at that store.
// All 6/6 relocation offsets and targets are exact. Cached/direct army access,
// timer declaration placement, and threshold predicate spelling were compared.
// Revisit when total SOURCE fuzzy reaches 95%.
VA(0x00430dc8, 0xf9)
void combatManager::ResetCycleTimers(void)
{
    long currentTime = KBTickCount();
    int side;
    int index;

    m_heroCycleTimer[COMBAT_ATTACKER_SIDE] = KBTickCount();
    m_heroCycleTimer[COMBAT_DEFENDER_SIDE] = KBTickCount();
    for (side = 0; side < COMBAT_MANAGER_SIDE_COUNT; ++side) {
        for (index = 0; index < gpCombatManager->m_armyCount[side]; ++index) {
            army *currentArmy = &gpCombatManager->m_armies[side][index];
            currentArmy->m_lastAnimationTime = currentTime;
            if (currentArmy->m_frameInfo.standStillDelay >
                COMBAT_STAND_DELAY_RANDOM_THRESHOLD) {
                currentArmy->m_lastAnimationTime -=
                    Random(COMBAT_STAND_DELAY_RANDOM_MIN,
                           currentArmy->m_frameInfo.standStillDelay);
            }
        }
    }
}

// @match-note retained/live 99.80%: exact 0x08 frame (`x`/ECX -0x04,
// `y`/EDX -0x08), complete four-bound CFG, no relocations, and 0x53 retail span
// agree. First/only masked raw residual is +0x43: the true-arm branch targets
// retail's equivalent final `jmp $+0` continuation instead of our epilogue.
// Positive/fallthrough comparisons and an explicit else were tried; the else
// adds the jump on the wrong arm and falls to 95.80%. Revisit when total SOURCE
// fuzzy reaches 95%.
VA(0x00430ec1, 0x53)
int InCombatArea(int x, int y)
{
    if (x >= 0 && x < COMBAT_SCREEN_WIDTH && y >= 0 &&
        y < COMBAT_AREA_HEIGHT)
        return 1;
    return 0;
}

// @match-note retained/live 97.41%: the complete overlay, creature-idle, hero-
// death/idle, animation-selection, redraw, and timer CFG compiles to 0x9ca bytes
// versus retail 0x9d9 with the exact 0x64 frame. Retail slots are `this` -0x50,
// cycleArmy[2][20] -0x48,
// animationIndex -0x20, accumulatedChance -0x1c, currentArmy -0x18, side/index
// -0x14/-0x1c, nextHeroAnimation[3] -0x10; ours first puts currentArmy at -0x4c.
// Raw first residual is +0x16; normalized instruction 66 first reverses side/
// army strength reduction. All 39 calls/relocations are reconstructed; offsets
// diverge after that loop and retail uses interior SCmbtHero/constant aliases.
// Pointer/direct indexing, inclusive bounds, signed/unsigned cycle bytes,
// condition polarities, and float operand orders were tried. Revisit when total
// SOURCE fuzzy reaches 95%.
VA(0x00430f14, 0x9d9)
void combatManager::CycleCombatScreen(void)
{
    army *currentArmy;
    unsigned char cycleArmy[2][20];
    int nextHeroAnimation[3];
    int side;
    int index;
    int animationIndex;
    float roll;
    float accumulatedChance;

    CheckUpdateCombatMessages();
    currentArmy = 0;
    nextHeroAnimation[COMBAT_MANAGER_SIDE_COUNT] = 0;
    gpCombatManager->ResetLimitCreature();
    for (side = 0; side < COMBAT_MANAGER_SIDE_COUNT; ++side) {
        if (m_heroOverlayIcons[side] == 0) {
        } else {
            if (m_heroes[side] != 0)
                m_heroOverlayFrame[side] = (m_heroOverlayFrame[side] + 1) % 5;
            ++m_drawHeroOverlay[side];
        }
    }

    memset(cycleArmy, 0, sizeof(cycleArmy));
    for (side = 0; side < COMBAT_MANAGER_SIDE_COUNT; ++side) {
        for (index = 0; index < gpCombatManager->m_armyCount[side]; ++index) {
            currentArmy = gpCombatManager->m_armies[side] + index;
            if ((currentArmy->m_monster.flags.all &
                 MONSTER_ABILITY_FLAG_AI_EXCLUDED) == 0 &&
                currentArmy->m_spellInfluence[ARMY_SPELL_INFLUENCE_PARALYZE] == 0 &&
                currentArmy->m_spellInfluence[ARMY_SPELL_INFLUENCE_BLIND] == 0 &&
                currentArmy->m_spellInfluence[ARMY_SPELL_INFLUENCE_PETRIFIED] == 0 &&
                ((currentArmy->m_animationSequence >=
                      COMBAT_CREATURE_CYCLE_SEQUENCE_FIRST &&
                  currentArmy->m_animationSequence <=
                      COMBAT_CREATURE_CYCLE_SEQUENCE_LAST) ||
                 (currentArmy->m_animationSequence == ARMY_ANIMATION_STAND &&
                  currentArmy->m_frameInfo.standStillDelay +
                          currentArmy->m_lastAnimationTime <
                      KBTickCount()))) {
                ++nextHeroAnimation[COMBAT_MANAGER_SIDE_COUNT];
                ++cycleArmy[side][index];
                ++m_limitCreatureCount[side][index];
            }
        }
    }

    for (side = 0; side < COMBAT_MANAGER_SIDE_COUNT; ++side) {
        nextHeroAnimation[side] = -1;
        if (m_heroIcons[side] == 0) {
        } else {
            if (m_heroAnimationState[side] ==
                    COMBAT_HERO_ANIMATION_DEATH_FIRST ||
                m_heroAnimationState[side] ==
                    COMBAT_HERO_ANIMATION_DEATH_SECOND ||
                m_heroAnimationState[side] ==
                    COMBAT_HERO_ANIMATION_IDLE_FIRST ||
                m_heroAnimationState[side] ==
                    COMBAT_HERO_ANIMATION_IDLE_SECOND ||
                m_heroAnimationState[side] ==
                    COMBAT_HERO_ANIMATION_IDLE_LAST) {
                m_drawHero[side] = 1;
            } else if (m_heroAnimationState[side] ==
                           COMBAT_HERO_ANIMATION_STAND &&
                       m_heroDeathAnimationPlayed[side] == 0 &&
                       m_heroDeathPending[side] != 0) {
                if (m_playerId[m_currentSide] == -1 ||
                    gbThisNetHumanPlayer[m_playerId[m_currentSide]] == 0) {
                    m_heroAlternateDeathPending[side] = 0;
                    m_heroDeathPending[side] =
                        m_heroAlternateDeathPending[side];
                } else {
                    m_heroAlternateDeathPending[side] = 0;
                    m_heroDeathPending[side] =
                        m_heroAlternateDeathPending[side];
                    m_heroDeathAnimationPlayed[side] = 1;
                    if (sCmbtHero[m_heroSpriteIndex[side]]
                            .animationFrameCount
                                [COMBAT_HERO_ANIMATION_DEATH_FIRST] > 0) {
                        nextHeroAnimation[side] =
                            COMBAT_HERO_ANIMATION_DEATH_FIRST;
                        m_drawHero[side] = 1;
                    }
                }
            } else if (m_heroAnimationState[side] ==
                           COMBAT_HERO_ANIMATION_STAND &&
                       m_heroAlternateDeathAnimationPlayed[side] == 0 &&
                       m_heroAlternateDeathPending[side] != 0) {
                if (m_playerId[m_currentSide] == -1 ||
                    gbThisNetHumanPlayer[m_playerId[m_currentSide]] == 0) {
                    m_heroAlternateDeathPending[side] = 0;
                    m_heroDeathPending[side] =
                        m_heroAlternateDeathPending[side];
                } else {
                    m_heroAlternateDeathPending[side] = 0;
                    m_heroDeathPending[side] =
                        m_heroAlternateDeathPending[side];
                    m_heroAlternateDeathAnimationPlayed[side] = 1;
                    if (sCmbtHero[m_heroSpriteIndex[side]]
                            .animationFrameCount
                                [COMBAT_HERO_ANIMATION_DEATH_SECOND] > 0) {
                        nextHeroAnimation[side] =
                            COMBAT_HERO_ANIMATION_DEATH_SECOND;
                        m_drawHero[side] = 1;
                    }
                }
            } else if (m_heroAnimationState[side] ==
                           COMBAT_HERO_ANIMATION_STAND &&
                       m_heroCycleTimer[side] + COMBAT_HERO_IDLE_DELAY <
                           KBTickCount()) {
                if (static_cast<unsigned char>(
                        sCmbtHero[m_heroSpriteIndex[side]].idleAnimationCount) >
                    1) {
                    nextHeroAnimation[side] =
                        Random(0,
                               static_cast<unsigned char>(
                                   sCmbtHero[m_heroSpriteIndex[side]]
                                       .idleAnimationCount) -
                                   1) +
                        COMBAT_HERO_ANIMATION_IDLE_FIRST;
                } else {
                    nextHeroAnimation[side] =
                        COMBAT_HERO_ANIMATION_IDLE_FIRST;
                }
                m_drawHero[side] = 1;
            }
        }
    }
    if (m_heroIcons[COMBAT_ATTACKER_SIDE] != 0)
        m_drawHero[COMBAT_ATTACKER_SIDE] = 1;
    if (m_heroIcons[COMBAT_DEFENDER_SIDE] != 0)
        m_drawHero[COMBAT_DEFENDER_SIDE] = 1;

    if (nextHeroAnimation[COMBAT_MANAGER_SIDE_COUNT] != 0 ||
        m_drawHero[COMBAT_ATTACKER_SIDE] != 0 ||
        m_drawHero[COMBAT_DEFENDER_SIDE] != 0 ||
        m_drawHeroOverlay[COMBAT_ATTACKER_SIDE] != 0 ||
        m_drawHeroOverlay[COMBAT_DEFENDER_SIDE] != 0) {
        gpCombatManager->DrawFrame(0, 1, 1, 1,
                                   COMBAT_COMMAND_FRAME_DELAY, 1, 1);
        for (side = 0; side < COMBAT_MANAGER_SIDE_COUNT; ++side) {
            for (index = 0; index < gpCombatManager->m_armyCount[side];
                 ++index) {
                currentArmy = gpCombatManager->m_armies[side] + index;
                if (cycleArmy[side][index] != 0) {
                    if (currentArmy->m_animationSequence ==
                        ARMY_ANIMATION_STAND) {
                        roll = static_cast<float>(
                                   Random(COMBAT_IDLE_ROLL_MIN,
                                          COMBAT_IDLE_ROLL_MAX)) /
                               COMBAT_IDLE_ROLL_DIVISOR;
                        accumulatedChance = 0.0f;
                        currentArmy->m_standingAnimation =
                            currentArmy->m_frameInfo.standingAnimationCount - 1;
                        for (animationIndex = 0;
                             animationIndex <
                                 currentArmy->m_frameInfo
                                         .standingAnimationCount -
                                     1;
                             ++animationIndex) {
                            accumulatedChance +=
                                currentArmy->m_frameInfo
                                    .standingAnimationChances[animationIndex];
                            if (accumulatedChance > roll) {
                                currentArmy->m_standingAnimation =
                                    animationIndex;
                                animationIndex = 99;
                            }
                        }
                        currentArmy->m_animationSequence =
                            currentArmy->m_standingAnimation +
                            COMBAT_CREATURE_CYCLE_SEQUENCE_FIRST;
                        currentArmy->m_animationFrame = 0;
                    } else {
                        ++currentArmy->m_animationFrame;
                        if (currentArmy->m_frameInfo.standStillDelay == 0 &&
                            currentArmy->m_frameInfo.standingAnimationCount ==
                                1 &&
                            Random(0, COMBAT_IDLE_ROLL_MAX) <
                                COMBAT_IDLE_REPEAT_CHANCE) {
                            --currentArmy->m_animationFrame;
                        }
                        if (currentArmy->m_animationFrame >=
                            currentArmy->m_frameInfo.animationFrameCount
                                [currentArmy->m_standingAnimation +
                                 COMBAT_CREATURE_CYCLE_SEQUENCE_FIRST]) {
                            currentArmy->m_animationSequence =
                                ARMY_ANIMATION_STAND;
                            currentArmy->m_animationFrame = 0;
                            currentArmy->m_lastAnimationTime = KBTickCount();
                            if (currentArmy->m_frameInfo.standStillDelay > 0) {
                                currentArmy->m_lastAnimationTime = static_cast<int>(
                                    (currentArmy->m_frameInfo.standStillDelay *
                                         COMBAT_STAND_DELAY_BASE_FACTOR -
                                     Random(0,
                                            currentArmy->m_frameInfo
                                                .standStillDelay) *
                                         COMBAT_STAND_DELAY_RANDOM_FACTOR) +
                                    currentArmy->m_lastAnimationTime);
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
                    if (m_heroAnimationFrame[side] >=
                        sCmbtHero[m_heroSpriteIndex[side]]
                            .animationFrameCount[m_heroAnimationState[side]]) {
                        m_heroAnimationState[side] =
                            COMBAT_HERO_ANIMATION_STAND;
                        m_heroAnimationFrame[side] = 0;
                        m_heroCycleTimer[side] = KBTickCount();
                    }
                }
            }
        }
        DrawFrame(1, 1, 0, 0, COMBAT_COMMAND_FRAME_DELAY, 1, 1);
    }
    gCombatCycleTimer = static_cast<int>(
        KBTickCount() +
        gfCombatSpeedMod[gConfig.combatSpeed] * COMBAT_CYCLE_TIMER_FACTOR);
}

// @early-stop
// All 0x3b bytes and all 3 relocation targets match retail.
VA(0x004318ed, 0x3b)
void combatManager::SetCombatViewArmySmallLevel(int level)
{
    giCombatViewArmySmallLevel = level;
    DrawFrame(1, 0, 0, 0, COMBAT_COMMAND_FRAME_DELAY, 1, 1);
    WritePrefs();
}

// @early-stop
// At the retained source hash, all 0xe3 bytes and all 11 relocation targets
// match retail. Named header constants later moved MSVC's TU-cumulative state;
// the byte-proven 100% maximum is retained per campaign policy.
VA(0x00431928, 0xe3)
void combatManager::SetCombatGrid(int showGrid, int showMouseHex,
                                  int shadeLevel)
{
    if (gbShowCombatGrid == showGrid &&
        gbShowCombatMouseHex == showMouseHex &&
        giCombatShadeLevel == shadeLevel)
        return;

    UpdateMouseGrid(COMBAT_INVALID_HEX, 0);
    gbShowCombatGrid = showGrid;
    gbShowCombatMouseHex = showMouseHex;
    giCombatShadeLevel = shadeLevel;
    m_backgroundDrawn = 0;
    SetupGridForArmy(&m_armies[m_currentArmySide][m_currentArmyIndex]);
    DrawFrame(1, 0, 0, 0, COMBAT_COMMAND_FRAME_DELAY, 1, 1);
    ResetMouse();
    WritePrefs();
}

// @match-note retained 99.89%, live 79.47% after named header constants moved
// MSVC's TU-cumulative state. At the retained source hash, all 314 normalized
// instructions, the full slot-search/reuse/init/fizzle CFG, exact 0x3ab span,
// 0x14 frame, and 23/23 relocation targets agree. Retail slots are `this`
// -0x14, armyIndex -0x04,
// reusedArmy -0x10, index -0x08, and newArmy -0x0c; ours assigns the four
// locals to -0x08/-0x04/-0x0c/-0x10. The first non-relocation raw residual is
// +0x0e at armyIndex initialization. Compound versus nested destination checks,
// animate if/else versus early return, and counter ++ versus += were compared.
// Revisit when total SOURCE fuzzy reaches 95%.
VA(0x00431a0b, 0x3ab)
void combatManager::AddArmy(int side, int monsterType, int quantity, int hex,
                            int flags, int animate)
{
    int armyIndex = COMBAT_INVALID_ARMY_INDEX;
    int reusedArmy = 0;
    int index;
    for (index = 0; index < COMBAT_ARMY_CAPACITY; ++index) {
        if (m_armies[side][index].m_monsterType == ARMY_GROUP_EMPTY_SLOT) {
            armyIndex = index;
            break;
        }
        if (m_armies[side][index].m_quantity == 0 &&
            (m_armies[side][index].m_monster.flags.all &
             MONSTER_FLAGS_AI_EXCLUDED) != 0 &&
            ((m_armies[side][index].m_monster.flags.all &
              MONSTER_FLAGS_MIRROR_IMAGE) != 0 ||
             m_armies[side][index].m_monsterType ==
                 ARMY_CREATURE_EARTH_ELEMENTAL ||
             m_armies[side][index].m_monsterType ==
                 ARMY_CREATURE_AIR_ELEMENTAL ||
             m_armies[side][index].m_monsterType ==
                 ARMY_CREATURE_FIRE_ELEMENTAL ||
             m_armies[side][index].m_monsterType ==
                 ARMY_CREATURE_WATER_ELEMENTAL)) {
            armyIndex = index;
            reusedArmy = 1;
            break;
        }
    }

    if (armyIndex != COMBAT_INVALID_ARMY_INDEX) {
        if (m_hexCells[hex].m_occupantSide != COMBAT_INVALID_HEX)
            return;

        army *newArmy = &m_armies[side][armyIndex];
        newArmy->Init(monsterType, quantity, side, armyIndex, hex,
                      COMBAT_INVALID_HEX);
        newArmy->LoadResources();
        newArmy->m_monster.flags.all |= flags;
        if (reusedArmy == 0)
            ++m_armyCount[side];

        if (animate == 0)
            return;

        ResetLimitCreature();
        m_limitCreatureCount[side][armyIndex] += 1;
        gpCombatManager->DrawFrame(0, 1, 0, 1,
                                   COMBAT_COMMAND_FRAME_DELAY, 1, 1);
        gpWindowManager->SaveFizzleSource(
            giMinExtentX, giMinExtentY,
            giMaxExtentX - giMinExtentX + 1,
            giMaxExtentY - giMinExtentY + 1);
        UpdateGrid(0, 1);
        DrawFrame(0, 0, 0, 0, COMBAT_COMMAND_FRAME_DELAY, 1, 1);
        gpWindowManager->FizzleForward(
            giMinExtentX, giMinExtentY,
            giMaxExtentX - giMinExtentX + 1,
            giMaxExtentY - giMinExtentY + 1,
            COMBAT_COMMAND_FRAME_DELAY, 0, 0);
    }
}

// @match-note retained/live 99.68%: all 91 normalized instructions, the full
// controlled-player/cleanup CFG, exact 0x169 span, and all 6/6 relocation
// targets agree. Retail reserves a 0x0c frame and stores only `this` at -0x0c;
// ours reserves 0x04 and stores it at -0x04. The first non-relocation raw
// residual is the frame immediate at +0x05. Cleanup-first and controlled-first
// condition forms were compared. Revisit when total SOURCE fuzzy reaches 95%.
VA(0x00431db6, 0x169)
void combatManager::SetupSmallView(void)
{
    if (m_smallViewSide[COMBAT_DEFENDER_SIDE] == COMBAT_SMALL_VIEW_SIDE_NONE &&
        m_smallViewLastX[COMBAT_DEFENDER_SIDE] != COMBAT_SMALL_VIEW_POSITION_NONE) {
        DrawSmallView(COMBAT_DEFENDER_SIDE, 1);
    }

    if (gbThisNetHasControl != 0 &&
        m_playerId[m_currentSide] != COMBAT_PLAYER_NONE &&
        gbHumanPlayer[m_playerId[m_currentSide]] != 0) {
        if (m_smallViewSide[COMBAT_ATTACKER_SIDE] != m_currentSide ||
            m_smallViewArmyIndex[COMBAT_ATTACKER_SIDE] !=
                m_currentArmyIndex) {
            if (m_smallViewSide[COMBAT_ATTACKER_SIDE] !=
                    COMBAT_SMALL_VIEW_SIDE_NONE &&
                m_smallViewSide[COMBAT_ATTACKER_SIDE] != m_currentSide &&
                m_smallViewLastX[COMBAT_ATTACKER_SIDE] !=
                    COMBAT_SMALL_VIEW_POSITION_NONE) {
                m_smallViewSide[COMBAT_ATTACKER_SIDE] =
                    COMBAT_SMALL_VIEW_SIDE_NONE;
                DrawSmallView(COMBAT_ATTACKER_SIDE, 1);
            }
            m_smallViewSide[COMBAT_ATTACKER_SIDE] = m_currentSide;
            m_smallViewArmyIndex[COMBAT_ATTACKER_SIDE] = m_currentArmyIndex;
            DrawSmallView(COMBAT_ATTACKER_SIDE, 1);
        }
    } else {
        m_smallViewSide[COMBAT_ATTACKER_SIDE] = COMBAT_SMALL_VIEW_SIDE_NONE;
        if (m_smallViewLastX[COMBAT_ATTACKER_SIDE] !=
            COMBAT_SMALL_VIEW_POSITION_NONE)
            DrawSmallView(COMBAT_ATTACKER_SIDE, 1);
    }
}

// @early-stop
// All 0x304 bytes and all 45 relocation targets match retail.
VA(0x00431f1f, 0x304)
void combatManager::ViewBallista(int quickView)
{
    int archerCount;
    int attackBonus;
    char description[300];

    m_combatTowns[COMBAT_DEFENDER_SIDE]->CalcNumLevelArchers(&archerCount,
                                                              &attackBonus);
    sprintf(gText, "Ballista");
    strcpy(description, "");
    if (m_wallStates[COMBAT_WALL_SLOT_KEEP] !=
        COMBAT_WALL_STATE_KEEP_STANDING) {
        sprintf(description, "\n\nThe %s is destroyed.", "Ballista");
    } else if (attackBonus > 0) {
        sprintf(description,
                "\n\nThe %s fires with the strength of %d Archers, each "
                "with a +%d bonus to their attack skill.",
                "Ballista", archerCount, attackBonus);
    } else {
        sprintf(description,
                "\n\nThe %s fires with the strength of %d Archers.",
                "Ballista", archerCount);
    }
    strcat(gText, description);

    strcpy(description, "");
    if ((m_combatTowns[COMBAT_DEFENDER_SIDE]->m_buildings &
         TOWN_BUILDING_LEFT_TURRET) != 0) {
        if (m_wallStates[COMBAT_WALL_SLOT_TOP_TOWER] ==
            COMBAT_WALL_STATE_DESTROYED) {
            sprintf(description, "\n\nThe %s is destroyed.", "Left Turret");
        } else if (m_wallStates[COMBAT_WALL_SLOT_TOP_TOWER] ==
                   COMBAT_WALL_STATE_TOWER_STANDING) {
            if (attackBonus > 0) {
                sprintf(description,
                        "\n\nThe %s fires with the strength of %d Archers, "
                        "each with a +%d bonus to their attack skill.",
                        "Left Turret",
                        archerCount / COMBAT_KEEP_SIDE_TOWER_SHOT_DIVISOR,
                        attackBonus);
            } else {
                sprintf(description,
                        "\n\nThe %s fires with the strength of %d Archers.",
                        "Left Turret",
                        archerCount / COMBAT_KEEP_SIDE_TOWER_SHOT_DIVISOR);
            }
        }
        strcat(gText, description);
    }

    if ((m_combatTowns[COMBAT_DEFENDER_SIDE]->m_buildings &
         TOWN_BUILDING_RIGHT_TURRET) != 0) {
        strcpy(description, "");
        if (m_wallStates[COMBAT_WALL_SLOT_BOTTOM_TOWER] ==
            COMBAT_WALL_STATE_DESTROYED) {
            sprintf(description, "\n\nThe %s is destroyed.", "Right Turret");
        } else if (m_wallStates[COMBAT_WALL_SLOT_BOTTOM_TOWER] ==
                   COMBAT_WALL_STATE_TOWER_STANDING) {
            if (attackBonus > 0) {
                sprintf(description,
                        "\n\nThe %s fires with the strength of %d Archers, "
                        "each with a +%d bonus to their attack skill.",
                        "Right Turret",
                        archerCount / COMBAT_KEEP_SIDE_TOWER_SHOT_DIVISOR,
                        attackBonus);
            } else {
                sprintf(description,
                        "\n\nThe %s fires with the strength of %d Archers.",
                        "Right Turret",
                        archerCount / COMBAT_KEEP_SIDE_TOWER_SHOT_DIVISOR);
            }
        }
        strcat(gText, description);
    }

    NormalDialog(gText,
                 quickView == 0 ? NORMAL_DIALOG_INFO
                                : NORMAL_DIALOG_QUICK_VIEW,
                 NORMAL_DIALOG_NO_RESOURCE, NORMAL_DIALOG_NO_VALUE,
                 NORMAL_DIALOG_NO_RESOURCE, 0, NORMAL_DIALOG_NO_RESOURCE, 0,
                 NORMAL_DIALOG_NO_RESOURCE, 0);
}

// ---- globals (definitions, RVA order) ----
DATA(0x004f09e8) short const_000f09e8 = 0x680;
DATA(0x004f0a80) short const_000f0a80 = 0x6c8;
DATA(0x004f0be4) short const_000f0be4 = 0x702;
DATA(0x004f0ca0) short const_000f0ca0 = 0x74b;
DATA(0x005250b8) int gbThisNetHasControl;
DATA(0x005250bc) int iCurTransferArtifact;
DATA(0x005250c0) signed char iTransferArtifactsInfo[16];
DATA(0x005250d0) int gbWhichAnimationPlaying;
DATA(0x005250d4) int iMaxTransferArtifacts;
DATA(0x005250d8) int giNextActionExtra;
DATA(0x005250dc) int bSkeletonsShown;
DATA(0x005250e0) int gbShowingLoseWindow;
DATA(0x005250e4) int giWinCmbtFrame;
DATA(0x005250e8) int giNextActionGridIndex;
DATA(0x005250ec) int giSurrenderCost;
DATA(0x005250f0) int giSkeletonsCreated;
DATA(0x005250f8) signed char iTransferArtifacts[16];
DATA(0x00525108) int giNextAction;
DATA(0x0052510c) int giNextActionGridIndex2;
