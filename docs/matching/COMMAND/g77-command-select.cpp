// VC6 SP5 COMMAND source family g77-command-select; parent34b6e303 plus U17 packet view.
// Source record only. All alternatives below are relative to the stated BEFORE,
// not cumulative diffs. No cross-family/TU-state census is claimed.
// build/source-variant-batch/g77-command-select/results.json; RVA 0x2d56e
// Complete 3 arms; elapsed 2.758578s; input restored.
// Disposition: retain original; every tested alternative changes text.
// EH private symbol hashes may differ; native comparison canonicalizes actual section owners.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'structured_control': 'keep', 'candidate': 'baseline'} / 1014 / 5ba0c6a613982b3c / 37b27283eb4d4b55 / 65:65 / 65,0,0,0 / True
// 1 / {'structured_control': 'if_else_switch', 'candidate': 'baseline'} / 1010 / f1f847d2ce242d27 / 37b27283eb4d4b55 / 64:65 / 57,0,5,2 / False
// 2 / {'structured_control': 'guarded_switch', 'candidate': 'baseline'} / 1002 / 296b8f3103c521fa / 9ca0417e7ecfdfd6 / 63:65 / 1,3,20,39 / False

// AXIS structured_control
// BEFORE (complete exact span):
// | CombatMessageCommand combatManager::GetCommand(i32 hexIndex) {
// |     i32 H2_UNUSED(column) = hexIndex % COMBAT_GRID_ROW_LENGTH;
// |     i32 H2_UNUSED(rowPos) = hexIndex / COMBAT_GRID_ROW_LENGTH;
// |     CombatMessageCommand command = COMBAT_MESSAGE_COMMAND_DEFAULT;
// |     b32 showEnemy_12 = false;
// |     CombatSide enemySide_27;
// |     i32 targetIndex;
// |     army* ourArmy_13;
// |
// |     if (hexIndex == INVALID_HEX) {
// |         command = COMBAT_MESSAGE_COMMAND_DEFAULT;
// |         goto smallView;
// |     }
// |     switch (hexIndex) {
// |         case COMBAT_GRID_RIGHT_HERO_HEX:
// |             if (m_heroes[1] != NULL) {
// |                 if (m_currentSide == COMBAT_DEFENDER_SIDE)
// |                     command = COMBAT_MESSAGE_COMMAND_OPTIONS;
// |                 else
// |                     command = COMBAT_MESSAGE_COMMAND_OPPOSING_OPTIONS;
// |             } else {
// |                 command = COMBAT_MESSAGE_COMMAND_DEFAULT;
// |             }
// |             break;
// |         case COMBAT_GRID_LEFT_SPECIAL_HEX:
// |             if (m_heroes[0] != NULL) {
// |                 if (m_currentSide == COMBAT_ATTACKER_SIDE)
// |                     command = COMBAT_MESSAGE_COMMAND_OPTIONS;
// |                 else
// |                     command = COMBAT_MESSAGE_COMMAND_OPPOSING_OPTIONS;
// |             } else {
// |                 command = COMBAT_MESSAGE_COMMAND_DEFAULT;
// |             }
// |             break;
// |         case COMBAT_BALLISTA_HEX:
// |             if (m_inCastleCombat != 0)
// |                 command = COMBAT_MESSAGE_COMMAND_VIEW_INFO;
// |             else
// |                 command = COMBAT_MESSAGE_COMMAND_DEFAULT;
// |             break;
// |         default: {
// |             if (hexIndex % COMBAT_GRID_ROW_LENGTH == COMBAT_GRID_ROW_LENGTH - 1) {
// |                 command = COMBAT_MESSAGE_COMMAND_DEFAULT;
// |                 break;
// |             }
// |
// |             enemySide_27 = m_hexCells[hexIndex].m_occupantSide;
// |             targetIndex = m_hexCells[hexIndex].m_occupantIndex;
// |             ourArmy_13 = &m_armies[IDX(m_currentArmySide)][m_currentArmyIndex];
// |             ourArmy_13->m_targetSide = COMBAT_SIDE_NONE;
// |             ourArmy_13->m_targetIndex = -1;
// |
// |             if (m_hexCells[hexIndex].m_blocked != 0
// |                 && (gpCombatManager->m_inCastleCombat == 0
// |                     || (hexIndex != COMBAT_CASTLE_GATE_APPROACH_HEX && hexIndex != CASTLE_GATE_HEX)
// |                     || (gpCombatManager->m_drawbridgeState == COMBAT_CASTLE_GATE_OPEN
// |                         && (gpCombatManager->m_currentSide != COMBAT_DEFENDER_SIDE
// |                             || gpCombatManager->m_hexCells[COMBAT_CASTLE_GATE_APPROACH_HEX]
// |                                        .m_occupantSide
// |                                    != COMBAT_SIDE_NONE
// |                             || gpCombatManager->m_hexCells[COMBAT_CASTLE_GATE_APPROACH_HEX]
// |                                        .m_deadOccupantCount
// |                                    != 0)))) {
// |                 command = COMBAT_MESSAGE_COMMAND_DEFAULT;
// |             } else if (enemySide_27 != COMBAT_SIDE_NONE) {
// |                 if (enemySide_27 != m_currentArmySide || targetIndex != m_currentArmyIndex) {
// |                     showEnemy_12 = true;
// |                     if (gbProcessingCombatAction == 0 && giNextAction == ACTION_NONE) {
// |                         m_smallViewSide[1] = enemySide_27;
// |                         m_smallViewArmyIndex[1] = targetIndex;
// |                         DrawSmallView(1, 1);
// |                     }
// |                 }
// |                 switch (enemySide_27) {
// |                     case COMBAT_ATTACKER_SIDE:
// |                     case COMBAT_DEFENDER_SIDE:
// |                         if (enemySide_27 == m_currentSide
// |                             || (enemySide_27 == m_currentArmySide
// |                                 && targetIndex == m_currentArmyIndex)) {
// |                             return COMBAT_MESSAGE_COMMAND_VIEW_INFO;
// |                         } else {
// |                             ourArmy_13->m_targetSide = enemySide_27;
// |                             ourArmy_13->m_targetIndex = targetIndex;
// |                             if (ourArmy_13->m_monster.shots > 0
// |                                 && ourArmy_13->GetAttackMask(
// |                                        ourArmy_13->m_hex,
// |                                        ARMY_ATTACK_TARGET_ENEMY,
// |                                        ARMY_HEX_INVALID
// |                                    ) == ARMY_ALL_ATTACK_DIRECTIONS) {
// |                                 if (ShotIsThroughWall(
// |                                         ourArmy_13->m_side,
// |                                         ourArmy_13->m_hex,
// |                                         hexIndex
// |                                     )
// |                                     != 0)
// |                                     return COMBAT_MESSAGE_COMMAND_SHOOT_THROUGH_WALL;
// |                                 else
// |                                     return COMBAT_MESSAGE_COMMAND_SHOOT;
// |                             }
// |                             if (ourArmy_13->ValidPath(hexIndex, ARMY_PATH_ANY_TARGET_HEX) == 1)
// |                                 return COMBAT_MESSAGE_COMMAND_ATTACK;
// |                             else {
// |                                 ourArmy_13->m_targetSide = COMBAT_SIDE_NONE;
// |                                 ourArmy_13->m_targetIndex = -1;
// |                                 command = COMBAT_MESSAGE_COMMAND_DEFAULT;
// |                             }
// |                         }
// |                         break;
// |                 }
// |             } else {
// |                 if (m_armies[IDX(m_currentArmySide)][m_currentArmyIndex]
// |                         .ValidPath(hexIndex, ARMY_PATH_ANY_TARGET_HEX)
// |                     == 1) {
// |                     command = CombatMessageCommand(
// |                         HAS(m_armies[IDX(m_currentArmySide)][m_currentArmyIndex]
// |                                 .m_monster.flags.all,
// |                             MONSTER_FLAGS_FLYING)
// |                             ? static_cast<i8>(IDX(COMBAT_MESSAGE_COMMAND_FLY))
// |                             : static_cast<i8>(IDX(COMBAT_MESSAGE_COMMAND_MOVE))
// |                     );
// |                 }
// |             }
// |             break;
// |         }
// |     }
// |
// | smallView:
// |     if (showEnemy_12 == 0 && gbProcessingCombatAction == 0) {
// |         m_smallViewSide[1] = COMBAT_SIDE_NONE;
// |         DrawSmallView(1, 1);
// |     }
// |     return command;
// | }
// ALTERNATIVE if_else_switch (unified source delta from BEFORE):
// | --- before
// | +++ if_else_switch
// | @@ -9,122 +9,122 @@
// |
// |      if (hexIndex == INVALID_HEX) {
// |          command = COMBAT_MESSAGE_COMMAND_DEFAULT;
// | -        goto smallView;
// | -    }
// | -    switch (hexIndex) {
// | -        case COMBAT_GRID_RIGHT_HERO_HEX:
// | -            if (m_heroes[1] != NULL) {
// | -                if (m_currentSide == COMBAT_DEFENDER_SIDE)
// | -                    command = COMBAT_MESSAGE_COMMAND_OPTIONS;
// | +    } else {
// | +        switch (hexIndex) {
// | +            case COMBAT_GRID_RIGHT_HERO_HEX:
// | +                if (m_heroes[1] != NULL) {
// | +                    if (m_currentSide == COMBAT_DEFENDER_SIDE)
// | +                        command = COMBAT_MESSAGE_COMMAND_OPTIONS;
// | +                    else
// | +                        command = COMBAT_MESSAGE_COMMAND_OPPOSING_OPTIONS;
// | +                } else {
// | +                    command = COMBAT_MESSAGE_COMMAND_DEFAULT;
// | +                }
// | +                break;
// | +            case COMBAT_GRID_LEFT_SPECIAL_HEX:
// | +                if (m_heroes[0] != NULL) {
// | +                    if (m_currentSide == COMBAT_ATTACKER_SIDE)
// | +                        command = COMBAT_MESSAGE_COMMAND_OPTIONS;
// | +                    else
// | +                        command = COMBAT_MESSAGE_COMMAND_OPPOSING_OPTIONS;
// | +                } else {
// | +                    command = COMBAT_MESSAGE_COMMAND_DEFAULT;
// | +                }
// | +                break;
// | +            case COMBAT_BALLISTA_HEX:
// | +                if (m_inCastleCombat != 0)
// | +                    command = COMBAT_MESSAGE_COMMAND_VIEW_INFO;
// |                  else
// | -                    command = COMBAT_MESSAGE_COMMAND_OPPOSING_OPTIONS;
// | -            } else {
// | -                command = COMBAT_MESSAGE_COMMAND_DEFAULT;
// | -            }
// | -            break;
// | -        case COMBAT_GRID_LEFT_SPECIAL_HEX:
// | -            if (m_heroes[0] != NULL) {
// | -                if (m_currentSide == COMBAT_ATTACKER_SIDE)
// | -                    command = COMBAT_MESSAGE_COMMAND_OPTIONS;
// | -                else
// | -                    command = COMBAT_MESSAGE_COMMAND_OPPOSING_OPTIONS;
// | -            } else {
// | -                command = COMBAT_MESSAGE_COMMAND_DEFAULT;
// | -            }
// | -            break;
// | -        case COMBAT_BALLISTA_HEX:
// | -            if (m_inCastleCombat != 0)
// | -                command = COMBAT_MESSAGE_COMMAND_VIEW_INFO;
// | -            else
// | -                command = COMBAT_MESSAGE_COMMAND_DEFAULT;
// | -            break;
// | -        default: {
// | -            if (hexIndex % COMBAT_GRID_ROW_LENGTH == COMBAT_GRID_ROW_LENGTH - 1) {
// | -                command = COMBAT_MESSAGE_COMMAND_DEFAULT;
// | +                    command = COMBAT_MESSAGE_COMMAND_DEFAULT;
// | +                break;
// | +            default: {
// | +                if (hexIndex % COMBAT_GRID_ROW_LENGTH == COMBAT_GRID_ROW_LENGTH - 1) {
// | +                    command = COMBAT_MESSAGE_COMMAND_DEFAULT;
// | +                    break;
// | +                }
// | +
// | +                enemySide_27 = m_hexCells[hexIndex].m_occupantSide;
// | +                targetIndex = m_hexCells[hexIndex].m_occupantIndex;
// | +                ourArmy_13 = &m_armies[IDX(m_currentArmySide)][m_currentArmyIndex];
// | +                ourArmy_13->m_targetSide = COMBAT_SIDE_NONE;
// | +                ourArmy_13->m_targetIndex = -1;
// | +
// | +                if (m_hexCells[hexIndex].m_blocked != 0
// | +                    && (gpCombatManager->m_inCastleCombat == 0
// | +                        || (hexIndex != COMBAT_CASTLE_GATE_APPROACH_HEX && hexIndex != CASTLE_GATE_HEX)
// | +                        || (gpCombatManager->m_drawbridgeState == COMBAT_CASTLE_GATE_OPEN
// | +                            && (gpCombatManager->m_currentSide != COMBAT_DEFENDER_SIDE
// | +                                || gpCombatManager->m_hexCells[COMBAT_CASTLE_GATE_APPROACH_HEX]
// | +                                           .m_occupantSide
// | +                                       != COMBAT_SIDE_NONE
// | +                                || gpCombatManager->m_hexCells[COMBAT_CASTLE_GATE_APPROACH_HEX]
// | +                                           .m_deadOccupantCount
// | +                                       != 0)))) {
// | +                    command = COMBAT_MESSAGE_COMMAND_DEFAULT;
// | +                } else if (enemySide_27 != COMBAT_SIDE_NONE) {
// | +                    if (enemySide_27 != m_currentArmySide || targetIndex != m_currentArmyIndex) {
// | +                        showEnemy_12 = true;
// | +                        if (gbProcessingCombatAction == 0 && giNextAction == ACTION_NONE) {
// | +                            m_smallViewSide[1] = enemySide_27;
// | +                            m_smallViewArmyIndex[1] = targetIndex;
// | +                            DrawSmallView(1, 1);
// | +                        }
// | +                    }
// | +                    switch (enemySide_27) {
// | +                        case COMBAT_ATTACKER_SIDE:
// | +                        case COMBAT_DEFENDER_SIDE:
// | +                            if (enemySide_27 == m_currentSide
// | +                                || (enemySide_27 == m_currentArmySide
// | +                                    && targetIndex == m_currentArmyIndex)) {
// | +                                return COMBAT_MESSAGE_COMMAND_VIEW_INFO;
// | +                            } else {
// | +                                ourArmy_13->m_targetSide = enemySide_27;
// | +                                ourArmy_13->m_targetIndex = targetIndex;
// | +                                if (ourArmy_13->m_monster.shots > 0
// | +                                    && ourArmy_13->GetAttackMask(
// | +                                           ourArmy_13->m_hex,
// | +                                           ARMY_ATTACK_TARGET_ENEMY,
// | +                                           ARMY_HEX_INVALID
// | +                                       ) == ARMY_ALL_ATTACK_DIRECTIONS) {
// | +                                    if (ShotIsThroughWall(
// | +                                            ourArmy_13->m_side,
// | +                                            ourArmy_13->m_hex,
// | +                                            hexIndex
// | +                                        )
// | +                                        != 0)
// | +                                        return COMBAT_MESSAGE_COMMAND_SHOOT_THROUGH_WALL;
// | +                                    else
// | +                                        return COMBAT_MESSAGE_COMMAND_SHOOT;
// | +                                }
// | +                                if (ourArmy_13->ValidPath(hexIndex, ARMY_PATH_ANY_TARGET_HEX) == 1)
// | +                                    return COMBAT_MESSAGE_COMMAND_ATTACK;
// | +                                else {
// | +                                    ourArmy_13->m_targetSide = COMBAT_SIDE_NONE;
// | +                                    ourArmy_13->m_targetIndex = -1;
// | +                                    command = COMBAT_MESSAGE_COMMAND_DEFAULT;
// | +                                }
// | +                            }
// | +                            break;
// | +                    }
// | +                } else {
// | +                    if (m_armies[IDX(m_currentArmySide)][m_currentArmyIndex]
// | +                            .ValidPath(hexIndex, ARMY_PATH_ANY_TARGET_HEX)
// | +                        == 1) {
// | +                        command = CombatMessageCommand(
// | +                            HAS(m_armies[IDX(m_currentArmySide)][m_currentArmyIndex]
// | +                                    .m_monster.flags.all,
// | +                                MONSTER_FLAGS_FLYING)
// | +                                ? static_cast<i8>(IDX(COMBAT_MESSAGE_COMMAND_FLY))
// | +                                : static_cast<i8>(IDX(COMBAT_MESSAGE_COMMAND_MOVE))
// | +                        );
// | +                    }
// | +                }
// |                  break;
// |              }
// | +        }
// |
// | -            enemySide_27 = m_hexCells[hexIndex].m_occupantSide;
// | -            targetIndex = m_hexCells[hexIndex].m_occupantIndex;
// | -            ourArmy_13 = &m_armies[IDX(m_currentArmySide)][m_currentArmyIndex];
// | -            ourArmy_13->m_targetSide = COMBAT_SIDE_NONE;
// | -            ourArmy_13->m_targetIndex = -1;
// | -
// | -            if (m_hexCells[hexIndex].m_blocked != 0
// | -                && (gpCombatManager->m_inCastleCombat == 0
// | -                    || (hexIndex != COMBAT_CASTLE_GATE_APPROACH_HEX && hexIndex != CASTLE_GATE_HEX)
// | -                    || (gpCombatManager->m_drawbridgeState == COMBAT_CASTLE_GATE_OPEN
// | -                        && (gpCombatManager->m_currentSide != COMBAT_DEFENDER_SIDE
// | -                            || gpCombatManager->m_hexCells[COMBAT_CASTLE_GATE_APPROACH_HEX]
// | -                                       .m_occupantSide
// | -                                   != COMBAT_SIDE_NONE
// | -                            || gpCombatManager->m_hexCells[COMBAT_CASTLE_GATE_APPROACH_HEX]
// | -                                       .m_deadOccupantCount
// | -                                   != 0)))) {
// | -                command = COMBAT_MESSAGE_COMMAND_DEFAULT;
// | -            } else if (enemySide_27 != COMBAT_SIDE_NONE) {
// | -                if (enemySide_27 != m_currentArmySide || targetIndex != m_currentArmyIndex) {
// | -                    showEnemy_12 = true;
// | -                    if (gbProcessingCombatAction == 0 && giNextAction == ACTION_NONE) {
// | -                        m_smallViewSide[1] = enemySide_27;
// | -                        m_smallViewArmyIndex[1] = targetIndex;
// | -                        DrawSmallView(1, 1);
// | -                    }
// | -                }
// | -                switch (enemySide_27) {
// | -                    case COMBAT_ATTACKER_SIDE:
// | -                    case COMBAT_DEFENDER_SIDE:
// | -                        if (enemySide_27 == m_currentSide
// | -                            || (enemySide_27 == m_currentArmySide
// | -                                && targetIndex == m_currentArmyIndex)) {
// | -                            return COMBAT_MESSAGE_COMMAND_VIEW_INFO;
// | -                        } else {
// | -                            ourArmy_13->m_targetSide = enemySide_27;
// | -                            ourArmy_13->m_targetIndex = targetIndex;
// | -                            if (ourArmy_13->m_monster.shots > 0
// | -                                && ourArmy_13->GetAttackMask(
// | -                                       ourArmy_13->m_hex,
// | -                                       ARMY_ATTACK_TARGET_ENEMY,
// | -                                       ARMY_HEX_INVALID
// | -                                   ) == ARMY_ALL_ATTACK_DIRECTIONS) {
// | -                                if (ShotIsThroughWall(
// | -                                        ourArmy_13->m_side,
// | -                                        ourArmy_13->m_hex,
// | -                                        hexIndex
// | -                                    )
// | -                                    != 0)
// | -                                    return COMBAT_MESSAGE_COMMAND_SHOOT_THROUGH_WALL;
// | -                                else
// | -                                    return COMBAT_MESSAGE_COMMAND_SHOOT;
// | -                            }
// | -                            if (ourArmy_13->ValidPath(hexIndex, ARMY_PATH_ANY_TARGET_HEX) == 1)
// | -                                return COMBAT_MESSAGE_COMMAND_ATTACK;
// | -                            else {
// | -                                ourArmy_13->m_targetSide = COMBAT_SIDE_NONE;
// | -                                ourArmy_13->m_targetIndex = -1;
// | -                                command = COMBAT_MESSAGE_COMMAND_DEFAULT;
// | -                            }
// | -                        }
// | -                        break;
// | -                }
// | -            } else {
// | -                if (m_armies[IDX(m_currentArmySide)][m_currentArmyIndex]
// | -                        .ValidPath(hexIndex, ARMY_PATH_ANY_TARGET_HEX)
// | -                    == 1) {
// | -                    command = CombatMessageCommand(
// | -                        HAS(m_armies[IDX(m_currentArmySide)][m_currentArmyIndex]
// | -                                .m_monster.flags.all,
// | -                            MONSTER_FLAGS_FLYING)
// | -                            ? static_cast<i8>(IDX(COMBAT_MESSAGE_COMMAND_FLY))
// | -                            : static_cast<i8>(IDX(COMBAT_MESSAGE_COMMAND_MOVE))
// | -                    );
// | -                }
// | -            }
// | -            break;
// | -        }
// |      }
// |
// | -smallView:
// |      if (showEnemy_12 == 0 && gbProcessingCombatAction == 0) {
// |          m_smallViewSide[1] = COMBAT_SIDE_NONE;
// |          DrawSmallView(1, 1);
// ALTERNATIVE guarded_switch (unified source delta from BEFORE):
// | --- before
// | +++ guarded_switch
// | @@ -7,124 +7,122 @@
// |      i32 targetIndex;
// |      army* ourArmy_13;
// |
// | -    if (hexIndex == INVALID_HEX) {
// | -        command = COMBAT_MESSAGE_COMMAND_DEFAULT;
// | -        goto smallView;
// | -    }
// | -    switch (hexIndex) {
// | -        case COMBAT_GRID_RIGHT_HERO_HEX:
// | -            if (m_heroes[1] != NULL) {
// | -                if (m_currentSide == COMBAT_DEFENDER_SIDE)
// | -                    command = COMBAT_MESSAGE_COMMAND_OPTIONS;
// | +    if (hexIndex != INVALID_HEX) {
// | +        switch (hexIndex) {
// | +            case COMBAT_GRID_RIGHT_HERO_HEX:
// | +                if (m_heroes[1] != NULL) {
// | +                    if (m_currentSide == COMBAT_DEFENDER_SIDE)
// | +                        command = COMBAT_MESSAGE_COMMAND_OPTIONS;
// | +                    else
// | +                        command = COMBAT_MESSAGE_COMMAND_OPPOSING_OPTIONS;
// | +                } else {
// | +                    command = COMBAT_MESSAGE_COMMAND_DEFAULT;
// | +                }
// | +                break;
// | +            case COMBAT_GRID_LEFT_SPECIAL_HEX:
// | +                if (m_heroes[0] != NULL) {
// | +                    if (m_currentSide == COMBAT_ATTACKER_SIDE)
// | +                        command = COMBAT_MESSAGE_COMMAND_OPTIONS;
// | +                    else
// | +                        command = COMBAT_MESSAGE_COMMAND_OPPOSING_OPTIONS;
// | +                } else {
// | +                    command = COMBAT_MESSAGE_COMMAND_DEFAULT;
// | +                }
// | +                break;
// | +            case COMBAT_BALLISTA_HEX:
// | +                if (m_inCastleCombat != 0)
// | +                    command = COMBAT_MESSAGE_COMMAND_VIEW_INFO;
// |                  else
// | -                    command = COMBAT_MESSAGE_COMMAND_OPPOSING_OPTIONS;
// | -            } else {
// | -                command = COMBAT_MESSAGE_COMMAND_DEFAULT;
// | -            }
// | -            break;
// | -        case COMBAT_GRID_LEFT_SPECIAL_HEX:
// | -            if (m_heroes[0] != NULL) {
// | -                if (m_currentSide == COMBAT_ATTACKER_SIDE)
// | -                    command = COMBAT_MESSAGE_COMMAND_OPTIONS;
// | -                else
// | -                    command = COMBAT_MESSAGE_COMMAND_OPPOSING_OPTIONS;
// | -            } else {
// | -                command = COMBAT_MESSAGE_COMMAND_DEFAULT;
// | -            }
// | -            break;
// | -        case COMBAT_BALLISTA_HEX:
// | -            if (m_inCastleCombat != 0)
// | -                command = COMBAT_MESSAGE_COMMAND_VIEW_INFO;
// | -            else
// | -                command = COMBAT_MESSAGE_COMMAND_DEFAULT;
// | -            break;
// | -        default: {
// | -            if (hexIndex % COMBAT_GRID_ROW_LENGTH == COMBAT_GRID_ROW_LENGTH - 1) {
// | -                command = COMBAT_MESSAGE_COMMAND_DEFAULT;
// | +                    command = COMBAT_MESSAGE_COMMAND_DEFAULT;
// | +                break;
// | +            default: {
// | +                if (hexIndex % COMBAT_GRID_ROW_LENGTH == COMBAT_GRID_ROW_LENGTH - 1) {
// | +                    command = COMBAT_MESSAGE_COMMAND_DEFAULT;
// | +                    break;
// | +                }
// | +
// | +                enemySide_27 = m_hexCells[hexIndex].m_occupantSide;
// | +                targetIndex = m_hexCells[hexIndex].m_occupantIndex;
// | +                ourArmy_13 = &m_armies[IDX(m_currentArmySide)][m_currentArmyIndex];
// | +                ourArmy_13->m_targetSide = COMBAT_SIDE_NONE;
// | +                ourArmy_13->m_targetIndex = -1;
// | +
// | +                if (m_hexCells[hexIndex].m_blocked != 0
// | +                    && (gpCombatManager->m_inCastleCombat == 0
// | +                        || (hexIndex != COMBAT_CASTLE_GATE_APPROACH_HEX && hexIndex != CASTLE_GATE_HEX)
// | +                        || (gpCombatManager->m_drawbridgeState == COMBAT_CASTLE_GATE_OPEN
// | +                            && (gpCombatManager->m_currentSide != COMBAT_DEFENDER_SIDE
// | +                                || gpCombatManager->m_hexCells[COMBAT_CASTLE_GATE_APPROACH_HEX]
// | +                                           .m_occupantSide
// | +                                       != COMBAT_SIDE_NONE
// | +                                || gpCombatManager->m_hexCells[COMBAT_CASTLE_GATE_APPROACH_HEX]
// | +                                           .m_deadOccupantCount
// | +                                       != 0)))) {
// | +                    command = COMBAT_MESSAGE_COMMAND_DEFAULT;
// | +                } else if (enemySide_27 != COMBAT_SIDE_NONE) {
// | +                    if (enemySide_27 != m_currentArmySide || targetIndex != m_currentArmyIndex) {
// | +                        showEnemy_12 = true;
// | +                        if (gbProcessingCombatAction == 0 && giNextAction == ACTION_NONE) {
// | +                            m_smallViewSide[1] = enemySide_27;
// | +                            m_smallViewArmyIndex[1] = targetIndex;
// | +                            DrawSmallView(1, 1);
// | +                        }
// | +                    }
// | +                    switch (enemySide_27) {
// | +                        case COMBAT_ATTACKER_SIDE:
// | +                        case COMBAT_DEFENDER_SIDE:
// | +                            if (enemySide_27 == m_currentSide
// | +                                || (enemySide_27 == m_currentArmySide
// | +                                    && targetIndex == m_currentArmyIndex)) {
// | +                                return COMBAT_MESSAGE_COMMAND_VIEW_INFO;
// | +                            } else {
// | +                                ourArmy_13->m_targetSide = enemySide_27;
// | +                                ourArmy_13->m_targetIndex = targetIndex;
// | +                                if (ourArmy_13->m_monster.shots > 0
// | +                                    && ourArmy_13->GetAttackMask(
// | +                                           ourArmy_13->m_hex,
// | +                                           ARMY_ATTACK_TARGET_ENEMY,
// | +                                           ARMY_HEX_INVALID
// | +                                       ) == ARMY_ALL_ATTACK_DIRECTIONS) {
// | +                                    if (ShotIsThroughWall(
// | +                                            ourArmy_13->m_side,
// | +                                            ourArmy_13->m_hex,
// | +                                            hexIndex
// | +                                        )
// | +                                        != 0)
// | +                                        return COMBAT_MESSAGE_COMMAND_SHOOT_THROUGH_WALL;
// | +                                    else
// | +                                        return COMBAT_MESSAGE_COMMAND_SHOOT;
// | +                                }
// | +                                if (ourArmy_13->ValidPath(hexIndex, ARMY_PATH_ANY_TARGET_HEX) == 1)
// | +                                    return COMBAT_MESSAGE_COMMAND_ATTACK;
// | +                                else {
// | +                                    ourArmy_13->m_targetSide = COMBAT_SIDE_NONE;
// | +                                    ourArmy_13->m_targetIndex = -1;
// | +                                    command = COMBAT_MESSAGE_COMMAND_DEFAULT;
// | +                                }
// | +                            }
// | +                            break;
// | +                    }
// | +                } else {
// | +                    if (m_armies[IDX(m_currentArmySide)][m_currentArmyIndex]
// | +                            .ValidPath(hexIndex, ARMY_PATH_ANY_TARGET_HEX)
// | +                        == 1) {
// | +                        command = CombatMessageCommand(
// | +                            HAS(m_armies[IDX(m_currentArmySide)][m_currentArmyIndex]
// | +                                    .m_monster.flags.all,
// | +                                MONSTER_FLAGS_FLYING)
// | +                                ? static_cast<i8>(IDX(COMBAT_MESSAGE_COMMAND_FLY))
// | +                                : static_cast<i8>(IDX(COMBAT_MESSAGE_COMMAND_MOVE))
// | +                        );
// | +                    }
// | +                }
// |                  break;
// |              }
// | +        }
// |
// | -            enemySide_27 = m_hexCells[hexIndex].m_occupantSide;
// | -            targetIndex = m_hexCells[hexIndex].m_occupantIndex;
// | -            ourArmy_13 = &m_armies[IDX(m_currentArmySide)][m_currentArmyIndex];
// | -            ourArmy_13->m_targetSide = COMBAT_SIDE_NONE;
// | -            ourArmy_13->m_targetIndex = -1;
// | -
// | -            if (m_hexCells[hexIndex].m_blocked != 0
// | -                && (gpCombatManager->m_inCastleCombat == 0
// | -                    || (hexIndex != COMBAT_CASTLE_GATE_APPROACH_HEX && hexIndex != CASTLE_GATE_HEX)
// | -                    || (gpCombatManager->m_drawbridgeState == COMBAT_CASTLE_GATE_OPEN
// | -                        && (gpCombatManager->m_currentSide != COMBAT_DEFENDER_SIDE
// | -                            || gpCombatManager->m_hexCells[COMBAT_CASTLE_GATE_APPROACH_HEX]
// | -                                       .m_occupantSide
// | -                                   != COMBAT_SIDE_NONE
// | -                            || gpCombatManager->m_hexCells[COMBAT_CASTLE_GATE_APPROACH_HEX]
// | -                                       .m_deadOccupantCount
// | -                                   != 0)))) {
// | -                command = COMBAT_MESSAGE_COMMAND_DEFAULT;
// | -            } else if (enemySide_27 != COMBAT_SIDE_NONE) {
// | -                if (enemySide_27 != m_currentArmySide || targetIndex != m_currentArmyIndex) {
// | -                    showEnemy_12 = true;
// | -                    if (gbProcessingCombatAction == 0 && giNextAction == ACTION_NONE) {
// | -                        m_smallViewSide[1] = enemySide_27;
// | -                        m_smallViewArmyIndex[1] = targetIndex;
// | -                        DrawSmallView(1, 1);
// | -                    }
// | -                }
// | -                switch (enemySide_27) {
// | -                    case COMBAT_ATTACKER_SIDE:
// | -                    case COMBAT_DEFENDER_SIDE:
// | -                        if (enemySide_27 == m_currentSide
// | -                            || (enemySide_27 == m_currentArmySide
// | -                                && targetIndex == m_currentArmyIndex)) {
// | -                            return COMBAT_MESSAGE_COMMAND_VIEW_INFO;
// | -                        } else {
// | -                            ourArmy_13->m_targetSide = enemySide_27;
// | -                            ourArmy_13->m_targetIndex = targetIndex;
// | -                            if (ourArmy_13->m_monster.shots > 0
// | -                                && ourArmy_13->GetAttackMask(
// | -                                       ourArmy_13->m_hex,
// | -                                       ARMY_ATTACK_TARGET_ENEMY,
// | -                                       ARMY_HEX_INVALID
// | -                                   ) == ARMY_ALL_ATTACK_DIRECTIONS) {
// | -                                if (ShotIsThroughWall(
// | -                                        ourArmy_13->m_side,
// | -                                        ourArmy_13->m_hex,
// | -                                        hexIndex
// | -                                    )
// | -                                    != 0)
// | -                                    return COMBAT_MESSAGE_COMMAND_SHOOT_THROUGH_WALL;
// | -                                else
// | -                                    return COMBAT_MESSAGE_COMMAND_SHOOT;
// | -                            }
// | -                            if (ourArmy_13->ValidPath(hexIndex, ARMY_PATH_ANY_TARGET_HEX) == 1)
// | -                                return COMBAT_MESSAGE_COMMAND_ATTACK;
// | -                            else {
// | -                                ourArmy_13->m_targetSide = COMBAT_SIDE_NONE;
// | -                                ourArmy_13->m_targetIndex = -1;
// | -                                command = COMBAT_MESSAGE_COMMAND_DEFAULT;
// | -                            }
// | -                        }
// | -                        break;
// | -                }
// | -            } else {
// | -                if (m_armies[IDX(m_currentArmySide)][m_currentArmyIndex]
// | -                        .ValidPath(hexIndex, ARMY_PATH_ANY_TARGET_HEX)
// | -                    == 1) {
// | -                    command = CombatMessageCommand(
// | -                        HAS(m_armies[IDX(m_currentArmySide)][m_currentArmyIndex]
// | -                                .m_monster.flags.all,
// | -                            MONSTER_FLAGS_FLYING)
// | -                            ? static_cast<i8>(IDX(COMBAT_MESSAGE_COMMAND_FLY))
// | -                            : static_cast<i8>(IDX(COMBAT_MESSAGE_COMMAND_MOVE))
// | -                    );
// | -                }
// | -            }
// | -            break;
// | -        }
// |      }
// |
// | -smallView:
// |      if (showEnemy_12 == 0 && gbProcessingCombatAction == 0) {
// |          m_smallViewSide[1] = COMBAT_SIDE_NONE;
// |          DrawSmallView(1, 1);
