// VC6 SP5 measured C65/A02/S59/G75/R20 source products.
// Clean parent b737b665; source record, not a compilable replacement TU.
// Per-phase products do not claim a full cross-phase or TU-state census.
// See docs/reconstruction/C65-A02-S59-B80.md for full native/retail proof.

// g75-ai-finish src/SOURCE/AI.cpp RVA 0x1628b
// build/source-variant-batch/g75-ai-finish/results.json
// Complete 3 arms; elapsed 2.292166s; input restored.
// Disposition: original retained; all tested nonoriginal forms change native bytes. No impossibility claim for other parents/states.
// trial / bytes / text hash / normalized relocation hash / blocks candidate:retail / exact,size-only,target-shift,flow-kind / graph exact
// 0 / 4000 / 147ff2d01b0462bb / 4d24cf000b06cdfa / 190:190 / 190,0,0,0 / True
// 1 / 3812 / 118e3ce39c602436 / cb38287f82427f5b / 149:190 / 90,0,50,9 / False
// 2 / 3772 / 45017a7339ef8cd9 / e2af79127c8de8b1 / 147:190 / 45,3,33,66 / False
/* AXIS finish_owner
BEFORE
void combatManager::DoCompAI(H2_ENUM_PARAM(CombatSide, i32)) {
    u32 traitorArray[COMBAT_SIDE_COUNT];
    i32 best;
    CombatSide sideEnemy;
    u32 flyerMask[COMBAT_SIDE_COUNT];
    u32 walkers[COMBAT_SIDE_COUNT];
    b32 stronger;
    b32 shootStrong;
    u32l myShootPower;
    i32 plan;
    u32l shootStrengths[COMBAT_SIDE_COUNT];
    u32l enemyShooters;
    army* thisArmy;
    u32 oddMasks[COMBAT_SIDE_COUNT];
    CombatHexDirection dirIndex;
    u32l totalArmyStrength;
    combatManager* H2_UNUSED(combat);
    i32 H2_UNUSED(fifth);
    u32 shooters[COMBAT_SIDE_COUNT];
    i32 archers;
    u32 mirrorMask[COMBAT_SIDE_COUNT];
    i32 rowIndex;
    b32 H2_UNUSED(archeryBonus);
    i32 plusArchers;
    i32 keepStrength;
    i32 grade;
    town* H2_UNUSED(pCastle);
    u8 rowLimit[COMBAT_AI_CASTLE_BOUNDARY_COUNT];
    i32 adjCell;
    hexcell* targetCell;
    u32 targetHex;

    m_limitCreature = false;
    thisArmy = m_currentArmyIndex + m_armies[IDX(m_currentArmySide)];
    plan = COMBAT_AI_ATTACK_NONE;
    sideEnemy = OppositeCombatSide(m_currentSide);

    mirrorMask[IDX(m_currentSide)] = GetMirrorImageMask(m_currentSide);
    mirrorMask[IDX(sideEnemy)] = GetMirrorImageMask(sideEnemy);
    shooters[IDX(m_currentSide)] = GetShooterMask(m_currentSide);
    shooters[IDX(sideEnemy)] = GetShooterMask(sideEnemy);
    flyerMask[IDX(m_currentSide)] = GetFlyerMask(m_currentSide);
    flyerMask[IDX(sideEnemy)] = GetFlyerMask(sideEnemy);
    walkers[IDX(m_currentSide)] = GetWalkerMask(m_currentSide);
    walkers[IDX(sideEnemy)] = GetWalkerMask(sideEnemy);
    oddMasks[IDX(m_currentSide)] = GetOutOfItMask(m_currentSide);
    oddMasks[IDX(sideEnemy)] = GetOutOfItMask(sideEnemy);
    traitorArray[IDX(m_currentSide)] = GetTraitorMask(m_currentSide);
    traitorArray[IDX(sideEnemy)] = GetTraitorMask(sideEnemy);
    shootStrengths[IDX(m_currentSide)] = GetStrength(m_currentSide, shooters[IDX(m_currentSide)]);
    shootStrengths[IDX(sideEnemy)] = GetStrength(sideEnemy, shooters[IDX(sideEnemy)]);
    totalArmyStrength = GetStrength(
        m_currentSide,
        shooters[IDX(m_currentSide)] | flyerMask[IDX(m_currentSide)]
            | walkers[IDX(m_currentSide)] | oddMasks[IDX(m_currentSide)]
            | traitorArray[IDX(m_currentSide)]
    );
    fifth = static_cast<i32>(totalArmyStrength + COMBAT_AI_STRENGTH_ROUNDING)
                         / COMBAT_AI_STRENGTH_FRACTION;
    shootStrong = false;
    stronger = false;
    myShootPower = GetStrength(m_currentSide, shooters[IDX(m_currentSide)]);
    enemyShooters = GetStrength(sideEnemy, shooters[IDX(sideEnemy)]);

    if (m_inCastleCombat != 0) {
        if (m_heroes[IDX(COMBAT_ATTACKER_SIDE)]->m_secondarySkills[IDX(HERO_SKILL_ARCHERY)]
                == HERO_SKILL_LEVEL_NONE
            && !m_heroes[IDX(COMBAT_ATTACKER_SIDE)]->HasArtifact(ARTIFACT_GOLDEN_BOW)) {
            if (m_currentSide == COMBAT_ATTACKER_SIDE)
                myShootPower =
                    static_cast<i32>(myShootPower) / COMBAT_SIDE_COUNT;
            else
                enemyShooters =
                    static_cast<i32>(enemyShooters) / COMBAT_SIDE_COUNT;
        }
        if (m_wallStates[IDX(COMBAT_WALL_SLOT_KEEP)] == COMBAT_WALL_STATE_KEEP_STANDING) {
            pCastle = m_combatTowns[IDX(COMBAT_DEFENDER_SIDE)];
            m_combatTowns[IDX(COMBAT_DEFENDER_SIDE)]->CalcNumLevelArchers(&archers, &grade);
            plusArchers = 0;
            if (m_wallStates[IDX(COMBAT_WALL_SLOT_TOP_TOWER)]
                == COMBAT_WALL_STATE_TOWER_STANDING)
                plusArchers += archers / COMBAT_SIDE_COUNT;
            if (m_wallStates[IDX(COMBAT_WALL_SLOT_BOTTOM_TOWER)]
                == COMBAT_WALL_STATE_TOWER_STANDING)
                plusArchers += archers / COMBAT_SIDE_COUNT;
            archers += plusArchers;
            keepStrength = static_cast<i32>(
                archers * COMBAT_AI_TOWER_STRENGTH
                * (grade * COMBAT_AI_TOWER_LEVEL_SCALE + COMBAT_AI_TOWER_BASE_SCALE)
            );
            archeryBonus =
                m_heroes[IDX(COMBAT_ATTACKER_SIDE)] != NULL
                        && (m_heroes[IDX(COMBAT_ATTACKER_SIDE)]->HasArtifact(ARTIFACT_GOLDEN_BOW)
                            || m_heroes[IDX(COMBAT_ATTACKER_SIDE)]
                                       ->m_secondarySkills[IDX(HERO_SKILL_ARCHERY)]
                                   != HERO_SKILL_LEVEL_NONE)
                    ? 0
                    : 1;
            if (m_currentSide == COMBAT_DEFENDER_SIDE)
                myShootPower += keepStrength;
            else
                enemyShooters += keepStrength;
        }
    }

    if (m_heroes[IDX(m_currentSide)] != NULL) {
        myShootPower = static_cast<i32>(
            static_cast<i32>(myShootPower)
            * gfSSArcheryMod
                [IDX(m_heroes[IDX(m_currentSide)]->m_secondarySkills[IDX(HERO_SKILL_ARCHERY)])]
        );
    }
    if (m_heroes[IDX(OppositeCombatSide(m_currentSide))] != NULL) {
        enemyShooters = static_cast<i32>(
            static_cast<i32>(enemyShooters)
            * gfSSArcheryMod
                [IDX(m_heroes[IDX(OppositeCombatSide(m_currentSide))]
                         ->m_secondarySkills[IDX(HERO_SKILL_ARCHERY)])]
        );
    }
    if (static_cast<i32>(totalArmyStrength + COMBAT_AI_STRENGTH_ROUNDING)
            / COMBAT_AI_STRENGTH_FRACTION
        < static_cast<i32>(myShootPower))
        shootStrong = true;
    if (static_cast<i32>(enemyShooters) > static_cast<i32>(myShootPower))
        stronger = true;

    if (HAS(thisArmy->m_monster.flags.abilityFlags, MONSTER_ABILITY_FLAG_SHOOTER) != 0) {
        if (thisArmy->m_monster.shots > 0)
            plan = COMBAT_AI_ATTACK_SHOOT;
        else
            plan = COMBAT_AI_ATTACK_WALK;
    } else if (HAS(thisArmy->m_monster.flags.abilityFlags, MONSTER_ABILITY_FLAG_FLYING) != 0) {
        plan = COMBAT_AI_ATTACK_FLY;
    } else {
        plan = COMBAT_AI_ATTACK_WALK;
    }

    switch (plan) {
        case COMBAT_AI_ATTACK_SHOOT:
            if (AttemptAdjacentAttack(thisArmy))
                goto finish;
            if (thisArmy->m_monsterType == CREATURE_LICH
                || thisArmy->m_monsterType == CREATURE_POWER_LICH) {
                DoLichShot(thisArmy);
                goto finish;
            }
            best = GetBestArmy(sideEnemy, mirrorMask[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            best = GetBestArmy(sideEnemy, shooters[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            best = GetBestArmy(sideEnemy, flyerMask[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            if (walkers[IDX(sideEnemy)] != 0) {
                best =
                    GetClosestArmy(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]);
                if (best != -1) {
                    giNextAction = ACTION_MOVE;
                    giNextActionGridIndex =
                        (m_armies[IDX(sideEnemy)] + best)
                            ->m_hex;
                    goto finish;
                }
            }
            best = GetBestArmy(sideEnemy, oddMasks[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            best = GetBestArmy(sideEnemy, traitorArray[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            break;
        case COMBAT_AI_ATTACK_FLY:
            if (shootStrong != 0 && stronger == 0) {
                if (AttemptAttack(thisArmy, sideEnemy, mirrorMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                    goto finish;
            } else {
                if (AttemptAttack(thisArmy, sideEnemy, mirrorMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                    goto finish;
            }
            break;
        case COMBAT_AI_ATTACK_WALK:
            if (COMBAT_AI_ATTACK_NONE)
                goto finish;
            if (shootStrong != 0 && stronger == 0) {
                if (AttemptAttack(thisArmy, sideEnemy, mirrorMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                    goto finish;
                if (WalkTowardArmyFront(thisArmy, m_currentSide, shooters[IDX(m_currentSide)]))
                    goto finish;
                giNextAction = ACTION_WAIT;
                goto finish;
            }
            if (AttemptAttack(thisArmy, sideEnemy, mirrorMask[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                goto finish;

            rowLimit[0] = COMBAT_AI_CASTLE_BOUNDARY_ROW_0;
            rowLimit[1] = COMBAT_AI_CASTLE_BOUNDARY_ROW_1;
            rowLimit[2] = COMBAT_AI_CASTLE_BOUNDARY_ROW_2;
            rowLimit[3] = COMBAT_AI_CASTLE_BOUNDARY_ROW_3;
            rowLimit[4] = COMBAT_AI_CASTLE_BOUNDARY_ROW_4;
            rowLimit[5] = COMBAT_AI_CASTLE_BOUNDARY_ROW_5;
            rowLimit[6] = COMBAT_AI_CASTLE_BOUNDARY_ROW_6;
            rowLimit[7] = COMBAT_AI_CASTLE_BOUNDARY_ROW_7;
            rowLimit[8] = COMBAT_AI_CASTLE_BOUNDARY_ROW_8;
            rowIndex = thisArmy->m_hex / ARMY_HEX_COLUMNS;
            if (m_currentSide == COMBAT_ATTACKER_SIDE && m_inCastleCombat != 0
                && thisArmy->m_hex < rowLimit[rowIndex]) {
                targetHex = rowLimit[rowIndex];
                targetCell = &gpCombatManager->m_hexCells[targetHex];
                if (ValidHex(targetHex) && targetCell->m_occupantSide == COMBAT_SIDE_NONE
                    && targetCell->m_blocked == 0) {
                    giNextAction = ACTION_MOVE;
                    giNextActionGridIndex = targetHex;
                    goto finish;
                }
            }
            break;
    }
    giNextAction = ACTION_WAIT;

finish:
    if (giNextAction == ACTION_MOVE && giNextActionGridIndex > 0
        && giNextActionGridIndex < COMBAT_HEX_COUNT
        && gpCombatManager->m_hexCells[giNextActionGridIndex].m_occupantSide == COMBAT_SIDE_NONE) {
        for (dirIndex = COMBAT_DIRECTION_NORTHEAST;
             IDX(dirIndex) < COMBAT_AI_ADJACENT_DIRECTION_COUNT;
             dirIndex++) {
            adjCell = thisArmy->GetAdjacentCellIndex(
                giNextActionGridIndex, dirIndex
            );
            if (adjCell > 0 && adjCell < COMBAT_HEX_COUNT
                && gpCombatManager->m_hexCells[adjCell].m_occupantSide
                       == OppositeCombatSide(m_currentSide)) {
                giNextActionGridIndex = adjCell;
                return;
            }
        }
    }
}


AFTER switch_exits
void combatManager::DoCompAI(H2_ENUM_PARAM(CombatSide, i32)) {
    u32 traitorArray[COMBAT_SIDE_COUNT];
    i32 best;
    CombatSide sideEnemy;
    u32 flyerMask[COMBAT_SIDE_COUNT];
    u32 walkers[COMBAT_SIDE_COUNT];
    b32 stronger;
    b32 shootStrong;
    u32l myShootPower;
    i32 plan;
    u32l shootStrengths[COMBAT_SIDE_COUNT];
    u32l enemyShooters;
    army* thisArmy;
    u32 oddMasks[COMBAT_SIDE_COUNT];
    CombatHexDirection dirIndex;
    u32l totalArmyStrength;
    combatManager* H2_UNUSED(combat);
    i32 H2_UNUSED(fifth);
    u32 shooters[COMBAT_SIDE_COUNT];
    i32 archers;
    u32 mirrorMask[COMBAT_SIDE_COUNT];
    i32 rowIndex;
    b32 H2_UNUSED(archeryBonus);
    i32 plusArchers;
    i32 keepStrength;
    i32 grade;
    town* H2_UNUSED(pCastle);
    u8 rowLimit[COMBAT_AI_CASTLE_BOUNDARY_COUNT];
    i32 adjCell;
    hexcell* targetCell;
    u32 targetHex;

    m_limitCreature = false;
    thisArmy = m_currentArmyIndex + m_armies[IDX(m_currentArmySide)];
    plan = COMBAT_AI_ATTACK_NONE;
    sideEnemy = OppositeCombatSide(m_currentSide);

    mirrorMask[IDX(m_currentSide)] = GetMirrorImageMask(m_currentSide);
    mirrorMask[IDX(sideEnemy)] = GetMirrorImageMask(sideEnemy);
    shooters[IDX(m_currentSide)] = GetShooterMask(m_currentSide);
    shooters[IDX(sideEnemy)] = GetShooterMask(sideEnemy);
    flyerMask[IDX(m_currentSide)] = GetFlyerMask(m_currentSide);
    flyerMask[IDX(sideEnemy)] = GetFlyerMask(sideEnemy);
    walkers[IDX(m_currentSide)] = GetWalkerMask(m_currentSide);
    walkers[IDX(sideEnemy)] = GetWalkerMask(sideEnemy);
    oddMasks[IDX(m_currentSide)] = GetOutOfItMask(m_currentSide);
    oddMasks[IDX(sideEnemy)] = GetOutOfItMask(sideEnemy);
    traitorArray[IDX(m_currentSide)] = GetTraitorMask(m_currentSide);
    traitorArray[IDX(sideEnemy)] = GetTraitorMask(sideEnemy);
    shootStrengths[IDX(m_currentSide)] = GetStrength(m_currentSide, shooters[IDX(m_currentSide)]);
    shootStrengths[IDX(sideEnemy)] = GetStrength(sideEnemy, shooters[IDX(sideEnemy)]);
    totalArmyStrength = GetStrength(
        m_currentSide,
        shooters[IDX(m_currentSide)] | flyerMask[IDX(m_currentSide)]
            | walkers[IDX(m_currentSide)] | oddMasks[IDX(m_currentSide)]
            | traitorArray[IDX(m_currentSide)]
    );
    fifth = static_cast<i32>(totalArmyStrength + COMBAT_AI_STRENGTH_ROUNDING)
                         / COMBAT_AI_STRENGTH_FRACTION;
    shootStrong = false;
    stronger = false;
    myShootPower = GetStrength(m_currentSide, shooters[IDX(m_currentSide)]);
    enemyShooters = GetStrength(sideEnemy, shooters[IDX(sideEnemy)]);

    if (m_inCastleCombat != 0) {
        if (m_heroes[IDX(COMBAT_ATTACKER_SIDE)]->m_secondarySkills[IDX(HERO_SKILL_ARCHERY)]
                == HERO_SKILL_LEVEL_NONE
            && !m_heroes[IDX(COMBAT_ATTACKER_SIDE)]->HasArtifact(ARTIFACT_GOLDEN_BOW)) {
            if (m_currentSide == COMBAT_ATTACKER_SIDE)
                myShootPower =
                    static_cast<i32>(myShootPower) / COMBAT_SIDE_COUNT;
            else
                enemyShooters =
                    static_cast<i32>(enemyShooters) / COMBAT_SIDE_COUNT;
        }
        if (m_wallStates[IDX(COMBAT_WALL_SLOT_KEEP)] == COMBAT_WALL_STATE_KEEP_STANDING) {
            pCastle = m_combatTowns[IDX(COMBAT_DEFENDER_SIDE)];
            m_combatTowns[IDX(COMBAT_DEFENDER_SIDE)]->CalcNumLevelArchers(&archers, &grade);
            plusArchers = 0;
            if (m_wallStates[IDX(COMBAT_WALL_SLOT_TOP_TOWER)]
                == COMBAT_WALL_STATE_TOWER_STANDING)
                plusArchers += archers / COMBAT_SIDE_COUNT;
            if (m_wallStates[IDX(COMBAT_WALL_SLOT_BOTTOM_TOWER)]
                == COMBAT_WALL_STATE_TOWER_STANDING)
                plusArchers += archers / COMBAT_SIDE_COUNT;
            archers += plusArchers;
            keepStrength = static_cast<i32>(
                archers * COMBAT_AI_TOWER_STRENGTH
                * (grade * COMBAT_AI_TOWER_LEVEL_SCALE + COMBAT_AI_TOWER_BASE_SCALE)
            );
            archeryBonus =
                m_heroes[IDX(COMBAT_ATTACKER_SIDE)] != NULL
                        && (m_heroes[IDX(COMBAT_ATTACKER_SIDE)]->HasArtifact(ARTIFACT_GOLDEN_BOW)
                            || m_heroes[IDX(COMBAT_ATTACKER_SIDE)]
                                       ->m_secondarySkills[IDX(HERO_SKILL_ARCHERY)]
                                   != HERO_SKILL_LEVEL_NONE)
                    ? 0
                    : 1;
            if (m_currentSide == COMBAT_DEFENDER_SIDE)
                myShootPower += keepStrength;
            else
                enemyShooters += keepStrength;
        }
    }

    if (m_heroes[IDX(m_currentSide)] != NULL) {
        myShootPower = static_cast<i32>(
            static_cast<i32>(myShootPower)
            * gfSSArcheryMod
                [IDX(m_heroes[IDX(m_currentSide)]->m_secondarySkills[IDX(HERO_SKILL_ARCHERY)])]
        );
    }
    if (m_heroes[IDX(OppositeCombatSide(m_currentSide))] != NULL) {
        enemyShooters = static_cast<i32>(
            static_cast<i32>(enemyShooters)
            * gfSSArcheryMod
                [IDX(m_heroes[IDX(OppositeCombatSide(m_currentSide))]
                         ->m_secondarySkills[IDX(HERO_SKILL_ARCHERY)])]
        );
    }
    if (static_cast<i32>(totalArmyStrength + COMBAT_AI_STRENGTH_ROUNDING)
            / COMBAT_AI_STRENGTH_FRACTION
        < static_cast<i32>(myShootPower))
        shootStrong = true;
    if (static_cast<i32>(enemyShooters) > static_cast<i32>(myShootPower))
        stronger = true;

    if (HAS(thisArmy->m_monster.flags.abilityFlags, MONSTER_ABILITY_FLAG_SHOOTER) != 0) {
        if (thisArmy->m_monster.shots > 0)
            plan = COMBAT_AI_ATTACK_SHOOT;
        else
            plan = COMBAT_AI_ATTACK_WALK;
    } else if (HAS(thisArmy->m_monster.flags.abilityFlags, MONSTER_ABILITY_FLAG_FLYING) != 0) {
        plan = COMBAT_AI_ATTACK_FLY;
    } else {
        plan = COMBAT_AI_ATTACK_WALK;
    }

    switch (plan) {
        case COMBAT_AI_ATTACK_SHOOT:
            if (AttemptAdjacentAttack(thisArmy))
                break;
            if (thisArmy->m_monsterType == CREATURE_LICH
                || thisArmy->m_monsterType == CREATURE_POWER_LICH) {
                DoLichShot(thisArmy);
                break;
            }
            best = GetBestArmy(sideEnemy, mirrorMask[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                break;
            }
            best = GetBestArmy(sideEnemy, shooters[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                break;
            }
            best = GetBestArmy(sideEnemy, flyerMask[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                break;
            }
            if (walkers[IDX(sideEnemy)] != 0) {
                best =
                    GetClosestArmy(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]);
                if (best != -1) {
                    giNextAction = ACTION_MOVE;
                    giNextActionGridIndex =
                        (m_armies[IDX(sideEnemy)] + best)
                            ->m_hex;
                    break;
                }
            }
            best = GetBestArmy(sideEnemy, oddMasks[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                break;
            }
            best = GetBestArmy(sideEnemy, traitorArray[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                break;
            }
            giNextAction = ACTION_WAIT;
            break;
        case COMBAT_AI_ATTACK_FLY:
            if (shootStrong != 0 && stronger == 0) {
                if (AttemptAttack(thisArmy, sideEnemy, mirrorMask[IDX(sideEnemy)]))
                    break;
                if (AttemptAttack(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                    break;
                if (AttemptAttack(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                    break;
                if (AttemptAttack(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                    break;
                if (AttemptAttack(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                    break;
                if (AttemptAttack(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                    break;
            } else {
                if (AttemptAttack(thisArmy, sideEnemy, mirrorMask[IDX(sideEnemy)]))
                    break;
                if (AttemptAttack(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                    break;
                if (AttemptAttack(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                    break;
                if (AttemptAttack(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                    break;
                if (AttemptAttack(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                    break;
                if (AttemptAttack(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                    break;
            }
            giNextAction = ACTION_WAIT;
            break;
        case COMBAT_AI_ATTACK_WALK:
            if (COMBAT_AI_ATTACK_NONE)
                break;
            if (shootStrong != 0 && stronger == 0) {
                if (AttemptAttack(thisArmy, sideEnemy, mirrorMask[IDX(sideEnemy)]))
                    break;
                if (AttemptAttack(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                    break;
                if (AttemptAttack(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                    break;
                if (AttemptAttack(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                    break;
                if (AttemptAttack(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                    break;
                if (AttemptAttack(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                    break;
                if (WalkTowardArmyFront(thisArmy, m_currentSide, shooters[IDX(m_currentSide)]))
                    break;
                giNextAction = ACTION_WAIT;
                break;
            }
            if (AttemptAttack(thisArmy, sideEnemy, mirrorMask[IDX(sideEnemy)]))
                break;
            if (AttemptAttack(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                break;
            if (AttemptAttack(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                break;
            if (AttemptAttack(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                break;
            if (AttemptAttack(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                break;
            if (AttemptAttack(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                break;
            if (WalkTowardArmy(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                break;
            if (WalkTowardArmy(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                break;
            if (WalkTowardArmy(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                break;
            if (WalkTowardArmy(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                break;
            if (WalkTowardArmy(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                break;

            rowLimit[0] = COMBAT_AI_CASTLE_BOUNDARY_ROW_0;
            rowLimit[1] = COMBAT_AI_CASTLE_BOUNDARY_ROW_1;
            rowLimit[2] = COMBAT_AI_CASTLE_BOUNDARY_ROW_2;
            rowLimit[3] = COMBAT_AI_CASTLE_BOUNDARY_ROW_3;
            rowLimit[4] = COMBAT_AI_CASTLE_BOUNDARY_ROW_4;
            rowLimit[5] = COMBAT_AI_CASTLE_BOUNDARY_ROW_5;
            rowLimit[6] = COMBAT_AI_CASTLE_BOUNDARY_ROW_6;
            rowLimit[7] = COMBAT_AI_CASTLE_BOUNDARY_ROW_7;
            rowLimit[8] = COMBAT_AI_CASTLE_BOUNDARY_ROW_8;
            rowIndex = thisArmy->m_hex / ARMY_HEX_COLUMNS;
            if (m_currentSide == COMBAT_ATTACKER_SIDE && m_inCastleCombat != 0
                && thisArmy->m_hex < rowLimit[rowIndex]) {
                targetHex = rowLimit[rowIndex];
                targetCell = &gpCombatManager->m_hexCells[targetHex];
                if (ValidHex(targetHex) && targetCell->m_occupantSide == COMBAT_SIDE_NONE
                    && targetCell->m_blocked == 0) {
                    giNextAction = ACTION_MOVE;
                    giNextActionGridIndex = targetHex;
                    break;
                }
            }
            giNextAction = ACTION_WAIT;
            break;
        default:
            giNextAction = ACTION_WAIT;
            break;
    }

    if (giNextAction == ACTION_MOVE && giNextActionGridIndex > 0
        && giNextActionGridIndex < COMBAT_HEX_COUNT
        && gpCombatManager->m_hexCells[giNextActionGridIndex].m_occupantSide == COMBAT_SIDE_NONE) {
        for (dirIndex = COMBAT_DIRECTION_NORTHEAST;
             IDX(dirIndex) < COMBAT_AI_ADJACENT_DIRECTION_COUNT;
             dirIndex++) {
            adjCell = thisArmy->GetAdjacentCellIndex(
                giNextActionGridIndex, dirIndex
            );
            if (adjCell > 0 && adjCell < COMBAT_HEX_COUNT
                && gpCombatManager->m_hexCells[adjCell].m_occupantSide
                       == OppositeCombatSide(m_currentSide)) {
                giNextActionGridIndex = adjCell;
                return;
            }
        }
    }
}


AFTER single_pass_dispatch
void combatManager::DoCompAI(H2_ENUM_PARAM(CombatSide, i32)) {
    u32 traitorArray[COMBAT_SIDE_COUNT];
    i32 best;
    CombatSide sideEnemy;
    u32 flyerMask[COMBAT_SIDE_COUNT];
    u32 walkers[COMBAT_SIDE_COUNT];
    b32 stronger;
    b32 shootStrong;
    u32l myShootPower;
    i32 plan;
    u32l shootStrengths[COMBAT_SIDE_COUNT];
    u32l enemyShooters;
    army* thisArmy;
    u32 oddMasks[COMBAT_SIDE_COUNT];
    CombatHexDirection dirIndex;
    u32l totalArmyStrength;
    combatManager* H2_UNUSED(combat);
    i32 H2_UNUSED(fifth);
    u32 shooters[COMBAT_SIDE_COUNT];
    i32 archers;
    u32 mirrorMask[COMBAT_SIDE_COUNT];
    i32 rowIndex;
    b32 H2_UNUSED(archeryBonus);
    i32 plusArchers;
    i32 keepStrength;
    i32 grade;
    town* H2_UNUSED(pCastle);
    u8 rowLimit[COMBAT_AI_CASTLE_BOUNDARY_COUNT];
    i32 adjCell;
    hexcell* targetCell;
    u32 targetHex;

    m_limitCreature = false;
    thisArmy = m_currentArmyIndex + m_armies[IDX(m_currentArmySide)];
    plan = COMBAT_AI_ATTACK_NONE;
    sideEnemy = OppositeCombatSide(m_currentSide);

    mirrorMask[IDX(m_currentSide)] = GetMirrorImageMask(m_currentSide);
    mirrorMask[IDX(sideEnemy)] = GetMirrorImageMask(sideEnemy);
    shooters[IDX(m_currentSide)] = GetShooterMask(m_currentSide);
    shooters[IDX(sideEnemy)] = GetShooterMask(sideEnemy);
    flyerMask[IDX(m_currentSide)] = GetFlyerMask(m_currentSide);
    flyerMask[IDX(sideEnemy)] = GetFlyerMask(sideEnemy);
    walkers[IDX(m_currentSide)] = GetWalkerMask(m_currentSide);
    walkers[IDX(sideEnemy)] = GetWalkerMask(sideEnemy);
    oddMasks[IDX(m_currentSide)] = GetOutOfItMask(m_currentSide);
    oddMasks[IDX(sideEnemy)] = GetOutOfItMask(sideEnemy);
    traitorArray[IDX(m_currentSide)] = GetTraitorMask(m_currentSide);
    traitorArray[IDX(sideEnemy)] = GetTraitorMask(sideEnemy);
    shootStrengths[IDX(m_currentSide)] = GetStrength(m_currentSide, shooters[IDX(m_currentSide)]);
    shootStrengths[IDX(sideEnemy)] = GetStrength(sideEnemy, shooters[IDX(sideEnemy)]);
    totalArmyStrength = GetStrength(
        m_currentSide,
        shooters[IDX(m_currentSide)] | flyerMask[IDX(m_currentSide)]
            | walkers[IDX(m_currentSide)] | oddMasks[IDX(m_currentSide)]
            | traitorArray[IDX(m_currentSide)]
    );
    fifth = static_cast<i32>(totalArmyStrength + COMBAT_AI_STRENGTH_ROUNDING)
                         / COMBAT_AI_STRENGTH_FRACTION;
    shootStrong = false;
    stronger = false;
    myShootPower = GetStrength(m_currentSide, shooters[IDX(m_currentSide)]);
    enemyShooters = GetStrength(sideEnemy, shooters[IDX(sideEnemy)]);

    if (m_inCastleCombat != 0) {
        if (m_heroes[IDX(COMBAT_ATTACKER_SIDE)]->m_secondarySkills[IDX(HERO_SKILL_ARCHERY)]
                == HERO_SKILL_LEVEL_NONE
            && !m_heroes[IDX(COMBAT_ATTACKER_SIDE)]->HasArtifact(ARTIFACT_GOLDEN_BOW)) {
            if (m_currentSide == COMBAT_ATTACKER_SIDE)
                myShootPower =
                    static_cast<i32>(myShootPower) / COMBAT_SIDE_COUNT;
            else
                enemyShooters =
                    static_cast<i32>(enemyShooters) / COMBAT_SIDE_COUNT;
        }
        if (m_wallStates[IDX(COMBAT_WALL_SLOT_KEEP)] == COMBAT_WALL_STATE_KEEP_STANDING) {
            pCastle = m_combatTowns[IDX(COMBAT_DEFENDER_SIDE)];
            m_combatTowns[IDX(COMBAT_DEFENDER_SIDE)]->CalcNumLevelArchers(&archers, &grade);
            plusArchers = 0;
            if (m_wallStates[IDX(COMBAT_WALL_SLOT_TOP_TOWER)]
                == COMBAT_WALL_STATE_TOWER_STANDING)
                plusArchers += archers / COMBAT_SIDE_COUNT;
            if (m_wallStates[IDX(COMBAT_WALL_SLOT_BOTTOM_TOWER)]
                == COMBAT_WALL_STATE_TOWER_STANDING)
                plusArchers += archers / COMBAT_SIDE_COUNT;
            archers += plusArchers;
            keepStrength = static_cast<i32>(
                archers * COMBAT_AI_TOWER_STRENGTH
                * (grade * COMBAT_AI_TOWER_LEVEL_SCALE + COMBAT_AI_TOWER_BASE_SCALE)
            );
            archeryBonus =
                m_heroes[IDX(COMBAT_ATTACKER_SIDE)] != NULL
                        && (m_heroes[IDX(COMBAT_ATTACKER_SIDE)]->HasArtifact(ARTIFACT_GOLDEN_BOW)
                            || m_heroes[IDX(COMBAT_ATTACKER_SIDE)]
                                       ->m_secondarySkills[IDX(HERO_SKILL_ARCHERY)]
                                   != HERO_SKILL_LEVEL_NONE)
                    ? 0
                    : 1;
            if (m_currentSide == COMBAT_DEFENDER_SIDE)
                myShootPower += keepStrength;
            else
                enemyShooters += keepStrength;
        }
    }

    if (m_heroes[IDX(m_currentSide)] != NULL) {
        myShootPower = static_cast<i32>(
            static_cast<i32>(myShootPower)
            * gfSSArcheryMod
                [IDX(m_heroes[IDX(m_currentSide)]->m_secondarySkills[IDX(HERO_SKILL_ARCHERY)])]
        );
    }
    if (m_heroes[IDX(OppositeCombatSide(m_currentSide))] != NULL) {
        enemyShooters = static_cast<i32>(
            static_cast<i32>(enemyShooters)
            * gfSSArcheryMod
                [IDX(m_heroes[IDX(OppositeCombatSide(m_currentSide))]
                         ->m_secondarySkills[IDX(HERO_SKILL_ARCHERY)])]
        );
    }
    if (static_cast<i32>(totalArmyStrength + COMBAT_AI_STRENGTH_ROUNDING)
            / COMBAT_AI_STRENGTH_FRACTION
        < static_cast<i32>(myShootPower))
        shootStrong = true;
    if (static_cast<i32>(enemyShooters) > static_cast<i32>(myShootPower))
        stronger = true;

    if (HAS(thisArmy->m_monster.flags.abilityFlags, MONSTER_ABILITY_FLAG_SHOOTER) != 0) {
        if (thisArmy->m_monster.shots > 0)
            plan = COMBAT_AI_ATTACK_SHOOT;
        else
            plan = COMBAT_AI_ATTACK_WALK;
    } else if (HAS(thisArmy->m_monster.flags.abilityFlags, MONSTER_ABILITY_FLAG_FLYING) != 0) {
        plan = COMBAT_AI_ATTACK_FLY;
    } else {
        plan = COMBAT_AI_ATTACK_WALK;
    }

    do {
        if (plan == COMBAT_AI_ATTACK_SHOOT) {
            if (AttemptAdjacentAttack(thisArmy))
                break;
            if (thisArmy->m_monsterType == CREATURE_LICH
                || thisArmy->m_monsterType == CREATURE_POWER_LICH) {
                DoLichShot(thisArmy);
                break;
            }
            best = GetBestArmy(sideEnemy, mirrorMask[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                break;
            }
            best = GetBestArmy(sideEnemy, shooters[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                break;
            }
            best = GetBestArmy(sideEnemy, flyerMask[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                break;
            }
            if (walkers[IDX(sideEnemy)] != 0) {
                best =
                    GetClosestArmy(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]);
                if (best != -1) {
                    giNextAction = ACTION_MOVE;
                    giNextActionGridIndex =
                        (m_armies[IDX(sideEnemy)] + best)
                            ->m_hex;
                    break;
                }
            }
            best = GetBestArmy(sideEnemy, oddMasks[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                break;
            }
            best = GetBestArmy(sideEnemy, traitorArray[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                break;
            }
        } else if (plan == COMBAT_AI_ATTACK_FLY) {
            if (shootStrong != 0 && stronger == 0) {
                if (AttemptAttack(thisArmy, sideEnemy, mirrorMask[IDX(sideEnemy)]))
                    break;
                if (AttemptAttack(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                    break;
                if (AttemptAttack(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                    break;
                if (AttemptAttack(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                    break;
                if (AttemptAttack(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                    break;
                if (AttemptAttack(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                    break;
            } else {
                if (AttemptAttack(thisArmy, sideEnemy, mirrorMask[IDX(sideEnemy)]))
                    break;
                if (AttemptAttack(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                    break;
                if (AttemptAttack(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                    break;
                if (AttemptAttack(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                    break;
                if (AttemptAttack(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                    break;
                if (AttemptAttack(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                    break;
            }
        } else if (plan == COMBAT_AI_ATTACK_WALK) {
            if (COMBAT_AI_ATTACK_NONE)
                break;
            if (shootStrong != 0 && stronger == 0) {
                if (AttemptAttack(thisArmy, sideEnemy, mirrorMask[IDX(sideEnemy)]))
                    break;
                if (AttemptAttack(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                    break;
                if (AttemptAttack(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                    break;
                if (AttemptAttack(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                    break;
                if (AttemptAttack(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                    break;
                if (AttemptAttack(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                    break;
                if (WalkTowardArmyFront(thisArmy, m_currentSide, shooters[IDX(m_currentSide)]))
                    break;
                giNextAction = ACTION_WAIT;
                break;
            }
            if (AttemptAttack(thisArmy, sideEnemy, mirrorMask[IDX(sideEnemy)]))
                break;
            if (AttemptAttack(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                break;
            if (AttemptAttack(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                break;
            if (AttemptAttack(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                break;
            if (AttemptAttack(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                break;
            if (AttemptAttack(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                break;
            if (WalkTowardArmy(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                break;
            if (WalkTowardArmy(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                break;
            if (WalkTowardArmy(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                break;
            if (WalkTowardArmy(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                break;
            if (WalkTowardArmy(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                break;

            rowLimit[0] = COMBAT_AI_CASTLE_BOUNDARY_ROW_0;
            rowLimit[1] = COMBAT_AI_CASTLE_BOUNDARY_ROW_1;
            rowLimit[2] = COMBAT_AI_CASTLE_BOUNDARY_ROW_2;
            rowLimit[3] = COMBAT_AI_CASTLE_BOUNDARY_ROW_3;
            rowLimit[4] = COMBAT_AI_CASTLE_BOUNDARY_ROW_4;
            rowLimit[5] = COMBAT_AI_CASTLE_BOUNDARY_ROW_5;
            rowLimit[6] = COMBAT_AI_CASTLE_BOUNDARY_ROW_6;
            rowLimit[7] = COMBAT_AI_CASTLE_BOUNDARY_ROW_7;
            rowLimit[8] = COMBAT_AI_CASTLE_BOUNDARY_ROW_8;
            rowIndex = thisArmy->m_hex / ARMY_HEX_COLUMNS;
            if (m_currentSide == COMBAT_ATTACKER_SIDE && m_inCastleCombat != 0
                && thisArmy->m_hex < rowLimit[rowIndex]) {
                targetHex = rowLimit[rowIndex];
                targetCell = &gpCombatManager->m_hexCells[targetHex];
                if (ValidHex(targetHex) && targetCell->m_occupantSide == COMBAT_SIDE_NONE
                    && targetCell->m_blocked == 0) {
                    giNextAction = ACTION_MOVE;
                    giNextActionGridIndex = targetHex;
                    break;
                }
            }
        }
        giNextAction = ACTION_WAIT;
    } while (0);

    if (giNextAction == ACTION_MOVE && giNextActionGridIndex > 0
        && giNextActionGridIndex < COMBAT_HEX_COUNT
        && gpCombatManager->m_hexCells[giNextActionGridIndex].m_occupantSide == COMBAT_SIDE_NONE) {
        for (dirIndex = COMBAT_DIRECTION_NORTHEAST;
             IDX(dirIndex) < COMBAT_AI_ADJACENT_DIRECTION_COUNT;
             dirIndex++) {
            adjCell = thisArmy->GetAdjacentCellIndex(
                giNextActionGridIndex, dirIndex
            );
            if (adjCell > 0 && adjCell < COMBAT_HEX_COUNT
                && gpCombatManager->m_hexCells[adjCell].m_occupantSide
                       == OppositeCombatSide(m_currentSide)) {
                giNextActionGridIndex = adjCell;
                return;
            }
        }
    }
}

*/

// c65-retreat-group src/SOURCE/AI.cpp RVA 0x15a60
// build/source-variant-batch/c65-retreat-group/results.json
// Complete 8 arms; elapsed 5.106441s; input restored.
// Disposition: retain trial 7; final combined native and retail proof.
// trial / bytes / text hash / normalized relocation hash / blocks candidate:retail / exact,size-only,target-shift,flow-kind / graph exact
// 0 / 2091 / 6b2da28ff8607e3f / edc748678c3c9837 / 77:77 / 77,0,0,0 / True
// 1 / 2091 / 6b2da28ff8607e3f / edc748678c3c9837 / 77:77 / 77,0,0,0 / True
// 2 / 2091 / 6b2da28ff8607e3f / edc748678c3c9837 / 77:77 / 77,0,0,0 / True
// 3 / 2091 / 6b2da28ff8607e3f / edc748678c3c9837 / 77:77 / 77,0,0,0 / True
// 4 / 2091 / 6b2da28ff8607e3f / edc748678c3c9837 / 77:77 / 77,0,0,0 / True
// 5 / 2091 / 6b2da28ff8607e3f / edc748678c3c9837 / 77:77 / 77,0,0,0 / True
// 6 / 2091 / 6b2da28ff8607e3f / edc748678c3c9837 / 77:77 / 77,0,0,0 / True
// 7 / 2091 / 6b2da28ff8607e3f / edc748678c3c9837 / 77:77 / 77,0,0,0 / True
/* AXIS creature_type
BEFORE
static_cast<i8>(
                    (m_armies[sideNum] + armyIndex)
                        ->m_monsterType
                )

AFTER remove
(m_armies[sideNum] + armyIndex)->m_monsterType
*/
/* AXIS quantity
BEFORE
static_cast<i16>(
                        (m_armies[sideNum] + armyIndex)
                            ->m_quantity
                    )

AFTER remove
(m_armies[sideNum] + armyIndex)->m_quantity
*/
/* AXIS estimated_quantity
BEFORE
static_cast<i16>(static_cast<i32>(
                            (m_armies[sideNum] + armyIndex)
                                ->m_quantity
                            * COMBAT_AI_QUANTITY_ESTIMATE
                        ))

AFTER remove
static_cast<i32>(
                            (m_armies[sideNum] + armyIndex)
                                ->m_quantity
                            * COMBAT_AI_QUANTITY_ESTIMATE
                        )
*/

// c65-retreat-artifact src/SOURCE/AI.cpp RVA 0x15a60
// build/source-variant-batch/c65-retreat-artifact/results.json
// Complete 8 arms; elapsed 5.02056s; input restored.
// Disposition: retain trial 7; final combined native and retail proof.
// trial / bytes / text hash / normalized relocation hash / blocks candidate:retail / exact,size-only,target-shift,flow-kind / graph exact
// 0 / 2091 / 6b2da28ff8607e3f / edc748678c3c9837 / 77:77 / 77,0,0,0 / True
// 1 / 2091 / 6b2da28ff8607e3f / edc748678c3c9837 / 77:77 / 77,0,0,0 / True
// 2 / 2091 / 6b2da28ff8607e3f / edc748678c3c9837 / 77:77 / 77,0,0,0 / True
// 3 / 2091 / 6b2da28ff8607e3f / edc748678c3c9837 / 77:77 / 77,0,0,0 / True
// 4 / 2091 / 6b2da28ff8607e3f / edc748678c3c9837 / 77:77 / 77,0,0,0 / True
// 5 / 2091 / 6b2da28ff8607e3f / edc748678c3c9837 / 77:77 / 77,0,0,0 / True
// 6 / 2091 / 6b2da28ff8607e3f / edc748678c3c9837 / 77:77 / 77,0,0,0 / True
// 7 / 2091 / 6b2da28ff8607e3f / edc748678c3c9837 / 77:77 / 77,0,0,0 / True
/* AXIS high
BEFORE
static_cast<float>(chance + COMBAT_AI_HIGH_ARTIFACT_RETREAT_BONUS)

AFTER remove
chance + COMBAT_AI_HIGH_ARTIFACT_RETREAT_BONUS
*/
/* AXIS medium
BEFORE
static_cast<float>(chance + COMBAT_AI_MEDIUM_ARTIFACT_RETREAT_BONUS)

AFTER remove
chance + COMBAT_AI_MEDIUM_ARTIFACT_RETREAT_BONUS
*/
/* AXIS low
BEFORE
static_cast<float>(chance + COMBAT_AI_LOW_ARTIFACT_RETREAT_BONUS)

AFTER remove
chance + COMBAT_AI_LOW_ARTIFACT_RETREAT_BONUS
*/

// c65-retreat-force src/SOURCE/AI.cpp RVA 0x15a60
// build/source-variant-batch/c65-retreat-force/results.json
// Complete 32 arms; elapsed 18.632853s; input restored.
// Disposition: retain trial 31; final combined native and retail proof.
// trial / bytes / text hash / normalized relocation hash / blocks candidate:retail / exact,size-only,target-shift,flow-kind / graph exact
// 0 / 2091 / 6b2da28ff8607e3f / edc748678c3c9837 / 77:77 / 77,0,0,0 / True
// 1 / 2091 / 6b2da28ff8607e3f / edc748678c3c9837 / 77:77 / 77,0,0,0 / True
// 2 / 2091 / 6b2da28ff8607e3f / edc748678c3c9837 / 77:77 / 77,0,0,0 / True
// 3 / 2091 / 6b2da28ff8607e3f / edc748678c3c9837 / 77:77 / 77,0,0,0 / True
// 4 / 2091 / 6b2da28ff8607e3f / edc748678c3c9837 / 77:77 / 77,0,0,0 / True
// 5 / 2091 / 6b2da28ff8607e3f / edc748678c3c9837 / 77:77 / 77,0,0,0 / True
// 6 / 2091 / 6b2da28ff8607e3f / edc748678c3c9837 / 77:77 / 77,0,0,0 / True
// 7 / 2091 / 6b2da28ff8607e3f / edc748678c3c9837 / 77:77 / 77,0,0,0 / True
// 8 / 2091 / 6b2da28ff8607e3f / edc748678c3c9837 / 77:77 / 77,0,0,0 / True
// 9 / 2091 / 6b2da28ff8607e3f / edc748678c3c9837 / 77:77 / 77,0,0,0 / True
// 10 / 2091 / 6b2da28ff8607e3f / edc748678c3c9837 / 77:77 / 77,0,0,0 / True
// 11 / 2091 / 6b2da28ff8607e3f / edc748678c3c9837 / 77:77 / 77,0,0,0 / True
// 12 / 2091 / 6b2da28ff8607e3f / edc748678c3c9837 / 77:77 / 77,0,0,0 / True
// 13 / 2091 / 6b2da28ff8607e3f / edc748678c3c9837 / 77:77 / 77,0,0,0 / True
// 14 / 2091 / 6b2da28ff8607e3f / edc748678c3c9837 / 77:77 / 77,0,0,0 / True
// 15 / 2091 / 6b2da28ff8607e3f / edc748678c3c9837 / 77:77 / 77,0,0,0 / True
// 16 / 2091 / 6b2da28ff8607e3f / edc748678c3c9837 / 77:77 / 77,0,0,0 / True
// 17 / 2091 / 6b2da28ff8607e3f / edc748678c3c9837 / 77:77 / 77,0,0,0 / True
// 18 / 2091 / 6b2da28ff8607e3f / edc748678c3c9837 / 77:77 / 77,0,0,0 / True
// 19 / 2091 / 6b2da28ff8607e3f / edc748678c3c9837 / 77:77 / 77,0,0,0 / True
// 20 / 2091 / 6b2da28ff8607e3f / edc748678c3c9837 / 77:77 / 77,0,0,0 / True
// 21 / 2091 / 6b2da28ff8607e3f / edc748678c3c9837 / 77:77 / 77,0,0,0 / True
// 22 / 2091 / 6b2da28ff8607e3f / edc748678c3c9837 / 77:77 / 77,0,0,0 / True
// 23 / 2091 / 6b2da28ff8607e3f / edc748678c3c9837 / 77:77 / 77,0,0,0 / True
// 24 / 2091 / 6b2da28ff8607e3f / edc748678c3c9837 / 77:77 / 77,0,0,0 / True
// 25 / 2091 / 6b2da28ff8607e3f / edc748678c3c9837 / 77:77 / 77,0,0,0 / True
// 26 / 2091 / 6b2da28ff8607e3f / edc748678c3c9837 / 77:77 / 77,0,0,0 / True
// 27 / 2091 / 6b2da28ff8607e3f / edc748678c3c9837 / 77:77 / 77,0,0,0 / True
// 28 / 2091 / 6b2da28ff8607e3f / edc748678c3c9837 / 77:77 / 77,0,0,0 / True
// 29 / 2091 / 6b2da28ff8607e3f / edc748678c3c9837 / 77:77 / 77,0,0,0 / True
// 30 / 2091 / 6b2da28ff8607e3f / edc748678c3c9837 / 77:77 / 77,0,0,0 / True
// 31 / 2091 / 6b2da28ff8607e3f / edc748678c3c9837 / 77:77 / 77,0,0,0 / True
/* AXIS integer_ratio
BEFORE
static_cast<float>(force[IDX(m_currentSide)] / COMBAT_AI_STRENGTH_20000)

AFTER remove
force[IDX(m_currentSide)] / COMBAT_AI_STRENGTH_20000
*/
/* AXIS 30000
BEFORE
static_cast<float>(chance - COMBAT_AI_STRENGTH_30000_RETREAT_PENALTY)

AFTER remove
chance - COMBAT_AI_STRENGTH_30000_RETREAT_PENALTY
*/
/* AXIS 15000
BEFORE
static_cast<float>(chance - COMBAT_AI_STRENGTH_15000_RETREAT_PENALTY)

AFTER remove
chance - COMBAT_AI_STRENGTH_15000_RETREAT_PENALTY
*/
/* AXIS 5000
BEFORE
static_cast<float>(chance - COMBAT_AI_STRENGTH_5000_RETREAT_PENALTY)

AFTER remove
chance - COMBAT_AI_STRENGTH_5000_RETREAT_PENALTY
*/
/* AXIS 2500
BEFORE
static_cast<float>(chance - COMBAT_AI_STRENGTH_2500_RETREAT_PENALTY)

AFTER remove
chance - COMBAT_AI_STRENGTH_2500_RETREAT_PENALTY
*/

// c65-retreat-policy src/SOURCE/AI.cpp RVA 0x15a60
// build/source-variant-batch/c65-retreat-policy/results.json
// Complete 8 arms; elapsed 5.011966s; input restored.
// Disposition: retain trial 7; final combined native and retail proof.
// trial / bytes / text hash / normalized relocation hash / blocks candidate:retail / exact,size-only,target-shift,flow-kind / graph exact
// 0 / 2091 / 6b2da28ff8607e3f / edc748678c3c9837 / 77:77 / 77,0,0,0 / True
// 1 / 2091 / 6b2da28ff8607e3f / edc748678c3c9837 / 77:77 / 77,0,0,0 / True
// 2 / 2091 / 6b2da28ff8607e3f / edc748678c3c9837 / 77:77 / 77,0,0,0 / True
// 3 / 2091 / 6b2da28ff8607e3f / edc748678c3c9837 / 77:77 / 77,0,0,0 / True
// 4 / 2091 / 6b2da28ff8607e3f / edc748678c3c9837 / 77:77 / 77,0,0,0 / True
// 5 / 2091 / 6b2da28ff8607e3f / edc748678c3c9837 / 77:77 / 77,0,0,0 / True
// 6 / 2091 / 6b2da28ff8607e3f / edc748678c3c9837 / 77:77 / 77,0,0,0 / True
// 7 / 2091 / 6b2da28ff8607e3f / edc748678c3c9837 / 77:77 / 77,0,0,0 / True
/* AXIS difficulty
BEFORE
static_cast<float>(
        chance
        - (COMBAT_AI_MAX_DIFFICULTY - IDX(gpGame->m_difficulty))
              * COMBAT_AI_DIFFICULTY_RETREAT_STEP
    )

AFTER remove
chance
        - (COMBAT_AI_MAX_DIFFICULTY - IDX(gpGame->m_difficulty))
              * COMBAT_AI_DIFFICULTY_RETREAT_STEP
*/
/* AXIS experience
BEFORE
static_cast<float>(m_heroes[IDX(m_currentSide)]->m_experience / COMBAT_AI_EXPERIENCE_DIVISOR)

AFTER remove
m_heroes[IDX(m_currentSide)]->m_experience / COMBAT_AI_EXPERIENCE_DIVISOR
*/
/* AXIS attacker
BEFORE
static_cast<float>(chance - COMBAT_AI_ATTACKER_RETREAT_PENALTY)

AFTER remove
chance - COMBAT_AI_ATTACKER_RETREAT_PENALTY
*/

// c65-lich-mod src/SOURCE/AI.cpp RVA 0x1722b
// build/source-variant-batch/c65-lich-mod/results.json
// Complete 8 arms; elapsed 4.963265s; input restored.
// Disposition: retain trial 7; final combined native and retail proof.
// trial / bytes / text hash / normalized relocation hash / blocks candidate:retail / exact,size-only,target-shift,flow-kind / graph exact
// 0 / 171 / 1b3e891a225cef35 / 9661207459e2cfaf / 7:7 / 7,0,0,0 / True
// 1 / 171 / 1b3e891a225cef35 / 9661207459e2cfaf / 7:7 / 7,0,0,0 / True
// 2 / 171 / 1b3e891a225cef35 / 9661207459e2cfaf / 7:7 / 7,0,0,0 / True
// 3 / 171 / 1b3e891a225cef35 / 9661207459e2cfaf / 7:7 / 7,0,0,0 / True
// 4 / 171 / 1b3e891a225cef35 / 9661207459e2cfaf / 7:7 / 7,0,0,0 / True
// 5 / 171 / 1b3e891a225cef35 / 9661207459e2cfaf / 7:7 / 7,0,0,0 / True
// 6 / 171 / 1b3e891a225cef35 / 9661207459e2cfaf / 7:7 / 7,0,0,0 / True
// 7 / 171 / 1b3e891a225cef35 / 9661207459e2cfaf / 7:7 / 7,0,0,0 / True
/* AXIS hitpoints
BEFORE
static_cast<float>(
        target->m_quantity * target->m_monster.hitPoints - target->m_hitPointsLost
    )

AFTER remove
target->m_quantity * target->m_monster.hitPoints - target->m_hitPointsLost
*/
/* AXIS shooter
BEFORE
if (HAS(target->m_monster.flags.abilityFlags, MONSTER_ABILITY_FLAG_SHOOTER) != 0)
        modifiedDamage = static_cast<float>(modifiedDamage * COMBAT_AI_LICH_PRIORITY_MULTIPLIER);

AFTER remove
if (HAS(target->m_monster.flags.abilityFlags, MONSTER_ABILITY_FLAG_SHOOTER) != 0)
        modifiedDamage = modifiedDamage * COMBAT_AI_LICH_PRIORITY_MULTIPLIER;
*/
/* AXIS flyer
BEFORE
if (HAS(target->m_monster.flags.abilityFlags, MONSTER_ABILITY_FLAG_FLYING) != 0)
        modifiedDamage = static_cast<float>(modifiedDamage * COMBAT_AI_LICH_PRIORITY_MULTIPLIER);

AFTER remove
if (HAS(target->m_monster.flags.abilityFlags, MONSTER_ABILITY_FLAG_FLYING) != 0)
        modifiedDamage = modifiedDamage * COMBAT_AI_LICH_PRIORITY_MULTIPLIER;
*/

// c65-lich-shot src/SOURCE/AI.cpp RVA 0x172d6
// build/source-variant-batch/c65-lich-shot/results.json
// Complete 2 arms; elapsed 1.645878s; input restored.
// Disposition: retain trial 1; final combined native and retail proof.
// trial / bytes / text hash / normalized relocation hash / blocks candidate:retail / exact,size-only,target-shift,flow-kind / graph exact
// 0 / 659 / 94114f79cd210c5d / 3d90e810c88828fb / 24:24 / 24,0,0,0 / True
// 1 / 659 / 94114f79cd210c5d / 3d90e810c88828fb / 24:24 / 24,0,0,0 / True
/* AXIS quantity
BEFORE
static_cast<float>(lich->m_quantity * COMBAT_AI_LICH_DAMAGE_PER_CREATURE)

AFTER remove
lich->m_quantity * COMBAT_AI_LICH_DAMAGE_PER_CREATURE
*/

// c65-walk-front src/SOURCE/AI.cpp RVA 0x181fe
// build/source-variant-batch/c65-walk-front/results.json
// Complete 2 arms; elapsed 1.634733s; input restored.
// Disposition: retain trial 1; final combined native and retail proof.
// trial / bytes / text hash / normalized relocation hash / blocks candidate:retail / exact,size-only,target-shift,flow-kind / graph exact
// 0 / 519 / 2b074256b8d7faeb / 1a560bcc97feb3a5 / 21:21 / 21,0,0,0 / True
// 1 / 519 / 2b074256b8d7faeb / 1a560bcc97feb3a5 / 21:21 / 21,0,0,0 / True
/* AXIS restore_speed
BEFORE
static_cast<i8>(oldSpeed)

AFTER remove
oldSpeed
*/

// c65-walk src/SOURCE/AI.cpp RVA 0x18405
// build/source-variant-batch/c65-walk/results.json
// Complete 2 arms; elapsed 1.65658s; input restored.
// Disposition: retain trial 1; final combined native and retail proof.
// trial / bytes / text hash / normalized relocation hash / blocks candidate:retail / exact,size-only,target-shift,flow-kind / graph exact
// 0 / 516 / fd042de02fc3c0db / 876b80c8c939e51f / 28:24 / 24,0,0,0 / False
// 1 / 516 / fd042de02fc3c0db / 876b80c8c939e51f / 28:24 / 24,0,0,0 / False
/* AXIS restore_speed
BEFORE
static_cast<i8>(prevSpeed)

AFTER remove
prevSpeed
*/

// a02-lich-marker src/SOURCE/AI.cpp RVA 0x172d6
// build/source-variant-batch/a02-lich-marker/results.json
// Complete 3 arms; elapsed 2.201332s; input restored.
// Disposition: retain trial 2; final combined native and retail proof.
// trial / bytes / text hash / normalized relocation hash / blocks candidate:retail / exact,size-only,target-shift,flow-kind / graph exact
// 0 / 659 / 94114f79cd210c5d / 3d90e810c88828fb / 24:24 / 24,0,0,0 / True
// 1 / 656 / 9456cfc4604b7ec7 / 2c0eceeae56ff6ed / 24:24 / 23,1,0,0 / True
// 2 / 659 / 94114f79cd210c5d / 3d90e810c88828fb / 24:24 / 24,0,0,0 / True
/* AXIS marker_owner
BEFORE
void combatManager::DoLichShot(class army* lich) {
    float splashDamage;
    i32 armyIndex;
    i32 bestIndex = COMBAT_AI_NO_ARMY;
    float bestTotal = COMBAT_AI_MIN_LICH_DAMAGE_SCORE;
    float shotDamage = static_cast<float>(lich->m_quantity * COMBAT_AI_LICH_DAMAGE_PER_CREATURE);
    i32 sideHex;
    float score;
    CombatHexDirection iDir;
    army* targetArmy;
    u8 marked[IDX(COMBAT_SIDE_COUNT) * COMBAT_AI_ARMY_SLOT_COUNT];
    i32 targetHex;

    for (armyIndex = 0; armyIndex < m_armyCount[IDX(OppositeCombatSide(m_currentSide))];
         armyIndex++) {
        memset(marked, 0, sizeof(marked));
        score = 0;
        targetArmy = &m_armies[IDX(OppositeCombatSide(m_currentSide))][armyIndex];
        if (targetArmy == NULL
            || HAS(targetArmy->m_monster.flags.abilityFlags, MONSTER_ABILITY_FLAG_AI_EXCLUDED) != 0
            || targetArmy->m_quantity <= 0)
            continue;
        score = GetModLichDamage(targetArmy, shotDamage);
        *(marked + IDX(targetArmy->m_side) * COMBAT_AI_ARMY_SLOT_COUNT + targetArmy->m_index) =
            1;
        targetHex = targetArmy->m_hex;
        for (iDir = COMBAT_DIRECTION_NORTHEAST;
             IDX(iDir) < COMBAT_AI_ADJACENT_DIRECTION_COUNT;
             iDir++) {
            sideHex = GetAdjacentCellIndexNoArmy(targetHex, iDir);
            if (sideHex >= 0 && sideHex < COMBAT_HEX_COUNT
                && m_hexCells[sideHex].m_occupantSide != COMBAT_SIDE_NONE
                && m_hexCells[sideHex].m_occupantIndex != -1
                && *(marked + IDX(m_hexCells[sideHex].m_occupantSide) * COMBAT_AI_ARMY_SLOT_COUNT
                     + m_hexCells[sideHex].m_occupantIndex)
                       == 0) {
                splashDamage = GetModLichDamage(
                    &m_armies[IDX(m_hexCells[sideHex].m_occupantSide)]
                             [m_hexCells[sideHex].m_occupantIndex],
                    shotDamage
                );
                *(marked + IDX(m_hexCells[sideHex].m_occupantSide) * COMBAT_AI_ARMY_SLOT_COUNT
                  + m_hexCells[sideHex].m_occupantIndex) = 1;
                if (m_hexCells[sideHex].m_occupantSide == m_currentSide)
                    score -= splashDamage;
                else
                    score += splashDamage;
            }
        }
        if (bestIndex == COMBAT_AI_NO_ARMY || score > bestTotal) {
            bestTotal = score;
            bestIndex = armyIndex;
            giNextAction = ACTION_MOVE;
            giNextActionGridIndex = targetHex;
        }
    }
}


AFTER flat_index
void combatManager::DoLichShot(class army* lich) {
    float splashDamage;
    i32 armyIndex;
    i32 bestIndex = COMBAT_AI_NO_ARMY;
    float bestTotal = COMBAT_AI_MIN_LICH_DAMAGE_SCORE;
    float shotDamage = static_cast<float>(lich->m_quantity * COMBAT_AI_LICH_DAMAGE_PER_CREATURE);
    i32 sideHex;
    float score;
    CombatHexDirection iDir;
    army* targetArmy;
    u8 marked[IDX(COMBAT_SIDE_COUNT) * COMBAT_AI_ARMY_SLOT_COUNT];
    i32 targetHex;

    for (armyIndex = 0; armyIndex < m_armyCount[IDX(OppositeCombatSide(m_currentSide))];
         armyIndex++) {
        memset(marked, 0, sizeof(marked));
        score = 0;
        targetArmy = &m_armies[IDX(OppositeCombatSide(m_currentSide))][armyIndex];
        if (targetArmy == NULL
            || HAS(targetArmy->m_monster.flags.abilityFlags, MONSTER_ABILITY_FLAG_AI_EXCLUDED) != 0
            || targetArmy->m_quantity <= 0)
            continue;
        score = GetModLichDamage(targetArmy, shotDamage);
        marked[IDX(targetArmy->m_side) * COMBAT_AI_ARMY_SLOT_COUNT + targetArmy->m_index] =
            1;
        targetHex = targetArmy->m_hex;
        for (iDir = COMBAT_DIRECTION_NORTHEAST;
             IDX(iDir) < COMBAT_AI_ADJACENT_DIRECTION_COUNT;
             iDir++) {
            sideHex = GetAdjacentCellIndexNoArmy(targetHex, iDir);
            if (sideHex >= 0 && sideHex < COMBAT_HEX_COUNT
                && m_hexCells[sideHex].m_occupantSide != COMBAT_SIDE_NONE
                && m_hexCells[sideHex].m_occupantIndex != -1
                && marked[IDX(m_hexCells[sideHex].m_occupantSide) * COMBAT_AI_ARMY_SLOT_COUNT
                     + m_hexCells[sideHex].m_occupantIndex]
                       == 0) {
                splashDamage = GetModLichDamage(
                    &m_armies[IDX(m_hexCells[sideHex].m_occupantSide)]
                             [m_hexCells[sideHex].m_occupantIndex],
                    shotDamage
                );
                marked[IDX(m_hexCells[sideHex].m_occupantSide) * COMBAT_AI_ARMY_SLOT_COUNT
                  + m_hexCells[sideHex].m_occupantIndex] = 1;
                if (m_hexCells[sideHex].m_occupantSide == m_currentSide)
                    score -= splashDamage;
                else
                    score += splashDamage;
            }
        }
        if (bestIndex == COMBAT_AI_NO_ARMY || score > bestTotal) {
            bestTotal = score;
            bestIndex = armyIndex;
            giNextAction = ACTION_MOVE;
            giNextActionGridIndex = targetHex;
        }
    }
}


AFTER side_army_rows
void combatManager::DoLichShot(class army* lich) {
    float splashDamage;
    i32 armyIndex;
    i32 bestIndex = COMBAT_AI_NO_ARMY;
    float bestTotal = COMBAT_AI_MIN_LICH_DAMAGE_SCORE;
    float shotDamage = static_cast<float>(lich->m_quantity * COMBAT_AI_LICH_DAMAGE_PER_CREATURE);
    i32 sideHex;
    float score;
    CombatHexDirection iDir;
    army* targetArmy;
    u8 marked[COMBAT_SIDE_COUNT][COMBAT_AI_ARMY_SLOT_COUNT];
    i32 targetHex;

    for (armyIndex = 0; armyIndex < m_armyCount[IDX(OppositeCombatSide(m_currentSide))];
         armyIndex++) {
        memset(marked, 0, sizeof(marked));
        score = 0;
        targetArmy = &m_armies[IDX(OppositeCombatSide(m_currentSide))][armyIndex];
        if (targetArmy == NULL
            || HAS(targetArmy->m_monster.flags.abilityFlags, MONSTER_ABILITY_FLAG_AI_EXCLUDED) != 0
            || targetArmy->m_quantity <= 0)
            continue;
        score = GetModLichDamage(targetArmy, shotDamage);
        marked[IDX(targetArmy->m_side)][targetArmy->m_index] =
            1;
        targetHex = targetArmy->m_hex;
        for (iDir = COMBAT_DIRECTION_NORTHEAST;
             IDX(iDir) < COMBAT_AI_ADJACENT_DIRECTION_COUNT;
             iDir++) {
            sideHex = GetAdjacentCellIndexNoArmy(targetHex, iDir);
            if (sideHex >= 0 && sideHex < COMBAT_HEX_COUNT
                && m_hexCells[sideHex].m_occupantSide != COMBAT_SIDE_NONE
                && m_hexCells[sideHex].m_occupantIndex != -1
                && marked[IDX(m_hexCells[sideHex].m_occupantSide)][m_hexCells[sideHex].m_occupantIndex]
                       == 0) {
                splashDamage = GetModLichDamage(
                    &m_armies[IDX(m_hexCells[sideHex].m_occupantSide)]
                             [m_hexCells[sideHex].m_occupantIndex],
                    shotDamage
                );
                marked[IDX(m_hexCells[sideHex].m_occupantSide)][m_hexCells[sideHex].m_occupantIndex] = 1;
                if (m_hexCells[sideHex].m_occupantSide == m_currentSide)
                    score -= splashDamage;
                else
                    score += splashDamage;
            }
        }
        if (bestIndex == COMBAT_AI_NO_ARMY || score > bestTotal) {
            bestTotal = score;
            bestIndex = armyIndex;
            giNextAction = ACTION_MOVE;
            giNextActionGridIndex = targetHex;
        }
    }
}

*/

// s59-adjacent src/SOURCE/AI.cpp RVA 0x180ba
// build/source-variant-batch/s59-adjacent/results.json
// Complete 2 arms; elapsed 1.748262s; input restored.
// Disposition: original retained; all tested nonoriginal forms change native bytes. No impossibility claim for other parents/states.
// trial / bytes / text hash / normalized relocation hash / blocks candidate:retail / exact,size-only,target-shift,flow-kind / graph exact
// 0 / 324 / 2003985616b8e106 / f685305f4d364ea7 / 17:17 / 17,0,0,0 / True
// 1 / 324 / 498061fd8526242d / f685305f4d364ea7 / 17:17 / 17,0,0,0 / True
/* AXIS unused_hexes
BEFORE
    i32 H2_UNUSED(hexes);


AFTER remove

*/

// s59-walk src/SOURCE/AI.cpp RVA 0x18405
// build/source-variant-batch/s59-walk/results.json
// Complete 2 arms; elapsed 1.814539s; input restored.
// Disposition: original retained; all tested nonoriginal forms change native bytes. No impossibility claim for other parents/states.
// trial / bytes / text hash / normalized relocation hash / blocks candidate:retail / exact,size-only,target-shift,flow-kind / graph exact
// 0 / 516 / fd042de02fc3c0db / 876b80c8c939e51f / 28:24 / 24,0,0,0 / False
// 1 / 516 / dc6aa5f13d1f681a / 876b80c8c939e51f / 28:24 / 24,0,0,0 / False
/* AXIS unused_path
BEFORE
    i32 H2_UNUSED(path);


AFTER remove

*/

// s59-front src/SOURCE/AI.cpp RVA 0x181fe
// build/source-variant-batch/s59-front/results.json
// Complete 2 arms; elapsed 1.867578s; input restored.
// Disposition: original retained; all tested nonoriginal forms change native bytes. No impossibility claim for other parents/states.
// trial / bytes / text hash / normalized relocation hash / blocks candidate:retail / exact,size-only,target-shift,flow-kind / graph exact
// 0 / 519 / 2b074256b8d7faeb / 1a560bcc97feb3a5 / 21:21 / 21,0,0,0 / True
// 1 / 516 / 94e51532174197b3 / 90582bfc8a753b9f / 21:21 / 20,1,0,0 / True
/* AXIS unused_result_store
BEFORE
i32 combatManager::WalkTowardArmyFront(
    class army* currentArmy, H2_ENUM_PARAM(CombatSide, i32) side, i32 mask
) {
    i32 armyIndex;
    i32 frontDelta;
    i32 frontHex;
    i32 oldSpeed;
    i32 H2_UNUSED(pathFound);
    i32 left;
    i32 step;

    currentArmy->m_targetSide = COMBAT_AI_NO_ARMY;
    currentArmy->m_targetIndex = COMBAT_AI_NO_ARMY;
    armyIndex = GetClosestArmy(currentArmy, side, mask);
    if (armyIndex == COMBAT_AI_NO_ARMY)
        return 0;

    frontDelta = SINGLE_HEX_FRONT_OFFSET;
    frontHex = armyIndex[m_armies[IDX(side)]].m_hex;
    if (HAS(armyIndex[m_armies[IDX(side)]].m_monster.flags.abilityFlags,
            MONSTER_ABILITY_FLAG_WIDE)
        != 0)
        frontDelta = WIDE_CREATURE_FRONT_OFFSET;
    frontHex += currentArmy->m_facing == ARMY_FACING_RIGHT ? frontDelta : -frontDelta;
    if (frontHex % ARMY_HEX_COLUMNS == ARMY_HEX_COLUMNS - 1 || frontHex % ARMY_HEX_COLUMNS == 0)
        return WalkTowardArmy(currentArmy, side, mask);

    oldSpeed = currentArmy->m_monster.speed;
    currentArmy->m_monster.speed = COMBAT_AI_UNLIMITED_PATH_SPEED;
    pathFound = gpSearchArray->FindCombatPath(
        currentArmy->m_hex,
        frontHex,
        currentArmy,
        COMBAT_AI_PATH_TO_FRONT,
        0
    );
    currentArmy->m_monster.speed = static_cast<i8>(oldSpeed);
    if (gpSearchArray->m_pathLength > 0) {
        giNextAction = ACTION_MOVE;
        left = currentArmy->m_monster.speed;
        step = gpSearchArray->m_pathLength - 1;
        giNextActionGridIndex = currentArmy->m_hex;
        while (step >= 0 && left != 0) {
            giNextActionGridIndex = currentArmy->GetAdjacentCellIndex(
                giNextActionGridIndex,
                static_cast<CombatHexDirection>(
                    gpSearchArray->m_storage.aiPath.directions[step]
                )
            );
            step--;
            left--;
            if (giNextActionGridIndex > 0 && bIsMoatSlowed[giNextActionGridIndex] != 0)
                left = 0;
        }
        return 1;
    }
    return WalkTowardArmy(currentArmy, side, mask);
}


AFTER remove
i32 combatManager::WalkTowardArmyFront(
    class army* currentArmy, H2_ENUM_PARAM(CombatSide, i32) side, i32 mask
) {
    i32 armyIndex;
    i32 frontDelta;
    i32 frontHex;
    i32 oldSpeed;
    i32 left;
    i32 step;

    currentArmy->m_targetSide = COMBAT_AI_NO_ARMY;
    currentArmy->m_targetIndex = COMBAT_AI_NO_ARMY;
    armyIndex = GetClosestArmy(currentArmy, side, mask);
    if (armyIndex == COMBAT_AI_NO_ARMY)
        return 0;

    frontDelta = SINGLE_HEX_FRONT_OFFSET;
    frontHex = armyIndex[m_armies[IDX(side)]].m_hex;
    if (HAS(armyIndex[m_armies[IDX(side)]].m_monster.flags.abilityFlags,
            MONSTER_ABILITY_FLAG_WIDE)
        != 0)
        frontDelta = WIDE_CREATURE_FRONT_OFFSET;
    frontHex += currentArmy->m_facing == ARMY_FACING_RIGHT ? frontDelta : -frontDelta;
    if (frontHex % ARMY_HEX_COLUMNS == ARMY_HEX_COLUMNS - 1 || frontHex % ARMY_HEX_COLUMNS == 0)
        return WalkTowardArmy(currentArmy, side, mask);

    oldSpeed = currentArmy->m_monster.speed;
    currentArmy->m_monster.speed = COMBAT_AI_UNLIMITED_PATH_SPEED;
    gpSearchArray->FindCombatPath(
        currentArmy->m_hex,
        frontHex,
        currentArmy,
        COMBAT_AI_PATH_TO_FRONT,
        0
    );
    currentArmy->m_monster.speed = static_cast<i8>(oldSpeed);
    if (gpSearchArray->m_pathLength > 0) {
        giNextAction = ACTION_MOVE;
        left = currentArmy->m_monster.speed;
        step = gpSearchArray->m_pathLength - 1;
        giNextActionGridIndex = currentArmy->m_hex;
        while (step >= 0 && left != 0) {
            giNextActionGridIndex = currentArmy->GetAdjacentCellIndex(
                giNextActionGridIndex,
                static_cast<CombatHexDirection>(
                    gpSearchArray->m_storage.aiPath.directions[step]
                )
            );
            step--;
            left--;
            if (giNextActionGridIndex > 0 && bIsMoatSlowed[giNextActionGridIndex] != 0)
                left = 0;
        }
        return 1;
    }
    return WalkTowardArmy(currentArmy, side, mask);
}

*/

// s59-ai-owners src/SOURCE/AI.cpp RVA 0x1628b
// build/source-variant-batch/s59-ai-owners/results.json
// Complete 4 arms; elapsed 2.789749s; input restored.
// Disposition: retain trial 1; nine-entry size integrated through the existing header constant, not the measured alias.
// trial / bytes / text hash / normalized relocation hash / blocks candidate:retail / exact,size-only,target-shift,flow-kind / graph exact
// 0 / 4000 / 147ff2d01b0462bb / 4d24cf000b06cdfa / 190:190 / 190,0,0,0 / True
// 1 / 4000 / 147ff2d01b0462bb / 4d24cf000b06cdfa / 190:190 / 190,0,0,0 / True
// 2 / 3991 / 1ae23e83e8b061e5 / 927ae89a6b427891 / 190:190 / 190,0,0,0 / True
// 3 / 3991 / 1ae23e83e8b061e5 / 927ae89a6b427891 / 190:190 / 190,0,0,0 / True
/* AXIS unused_combat
BEFORE
    combatManager* H2_UNUSED(combat);


AFTER remove

*/
/* AXIS boundary_extent
BEFORE
u8 rowLimit[COMBAT_AI_CASTLE_BOUNDARY_COUNT];

AFTER remove
u8 rowLimit[COMBAT_GRID_ROW_COUNT];
*/

// s59-ai-stores src/SOURCE/AI.cpp RVA 0x1628b
// build/source-variant-batch/s59-ai-stores/results.json
// Complete 16 arms; elapsed 9.522706s; input restored.
// Disposition: original retained; all tested nonoriginal forms change native bytes. No impossibility claim for other parents/states.
// trial / bytes / text hash / normalized relocation hash / blocks candidate:retail / exact,size-only,target-shift,flow-kind / graph exact
// 0 / 4000 / 147ff2d01b0462bb / 4d24cf000b06cdfa / 190:190 / 190,0,0,0 / True
// 1 / 3982 / 80f8e24a2e001079 / a08b222d0f17f807 / 190:190 / 189,1,0,0 / True
// 2 / 3976 / c0be9f8c5b35c09d / 178b8b4b03d3688d / 190:190 / 189,1,0,0 / True
// 3 / 3964 / e3817e6b281ee3e5 / ab4873ad1eb9dc71 / 190:190 / 188,2,0,0 / True
// 4 / 3974 / ef845186d8844197 / b902a5f5c3854d96 / 190:190 / 189,1,0,0 / True
// 5 / 3962 / 35e57d943d95cb8d / b80202ce9c827fb3 / 190:190 / 188,2,0,0 / True
// 6 / 3956 / f8b2ac8a4c7ea881 / a812ec9d4acf663c / 190:190 / 188,2,0,0 / True
// 7 / 3935 / 8dece941ea1fdc0f / c85b6d3d640a6a32 / 190:190 / 187,3,0,0 / True
// 8 / 3965 / b8fcb6d7698622ad / 4b0e2861fdab3bfe / 190:190 / 189,1,0,0 / True
// 9 / 3944 / 5b4cb252b7b7edf4 / 58c73bb6c2941275 / 190:190 / 188,2,0,0 / True
// 10 / 3938 / cf8be007ca5fd179 / edc899ce74a797d0 / 190:190 / 188,2,0,0 / True
// 11 / 3911 / aacd5cfa0881fd61 / 554357007f72b9e5 / 190:190 / 187,3,0,0 / True
// 12 / 3936 / 9a1e827f4654119f / e540eb18e4298a10 / 190:190 / 189,1,0,0 / True
// 13 / 3909 / 812e4f12991fb0c0 / 10a61a8b09ca2733 / 190:190 / 188,2,0,0 / True
// 14 / 3903 / bb2e2e58303c7c61 / ea2a179f08f6f0fe / 190:190 / 188,2,0,0 / True
// 15 / 3878 / a5f0a5276ed9f1f6 / 0a34d83862839eb2 / 190:190 / 187,3,0,0 / True
/* AXIS unused_result_owners
BEFORE
void combatManager::DoCompAI(H2_ENUM_PARAM(CombatSide, i32)) {
    u32 traitorArray[COMBAT_SIDE_COUNT];
    i32 best;
    CombatSide sideEnemy;
    u32 flyerMask[COMBAT_SIDE_COUNT];
    u32 walkers[COMBAT_SIDE_COUNT];
    b32 stronger;
    b32 shootStrong;
    u32l myShootPower;
    i32 plan;
    u32l shootStrengths[COMBAT_SIDE_COUNT];
    u32l enemyShooters;
    army* thisArmy;
    u32 oddMasks[COMBAT_SIDE_COUNT];
    CombatHexDirection dirIndex;
    u32l totalArmyStrength;
    combatManager* H2_UNUSED(combat);
    i32 H2_UNUSED(fifth);
    u32 shooters[COMBAT_SIDE_COUNT];
    i32 archers;
    u32 mirrorMask[COMBAT_SIDE_COUNT];
    i32 rowIndex;
    b32 H2_UNUSED(archeryBonus);
    i32 plusArchers;
    i32 keepStrength;
    i32 grade;
    town* H2_UNUSED(pCastle);
    u8 rowLimit[COMBAT_AI_CASTLE_BOUNDARY_COUNT];
    i32 adjCell;
    hexcell* targetCell;
    u32 targetHex;

    m_limitCreature = false;
    thisArmy = m_currentArmyIndex + m_armies[IDX(m_currentArmySide)];
    plan = COMBAT_AI_ATTACK_NONE;
    sideEnemy = OppositeCombatSide(m_currentSide);

    mirrorMask[IDX(m_currentSide)] = GetMirrorImageMask(m_currentSide);
    mirrorMask[IDX(sideEnemy)] = GetMirrorImageMask(sideEnemy);
    shooters[IDX(m_currentSide)] = GetShooterMask(m_currentSide);
    shooters[IDX(sideEnemy)] = GetShooterMask(sideEnemy);
    flyerMask[IDX(m_currentSide)] = GetFlyerMask(m_currentSide);
    flyerMask[IDX(sideEnemy)] = GetFlyerMask(sideEnemy);
    walkers[IDX(m_currentSide)] = GetWalkerMask(m_currentSide);
    walkers[IDX(sideEnemy)] = GetWalkerMask(sideEnemy);
    oddMasks[IDX(m_currentSide)] = GetOutOfItMask(m_currentSide);
    oddMasks[IDX(sideEnemy)] = GetOutOfItMask(sideEnemy);
    traitorArray[IDX(m_currentSide)] = GetTraitorMask(m_currentSide);
    traitorArray[IDX(sideEnemy)] = GetTraitorMask(sideEnemy);
    shootStrengths[IDX(m_currentSide)] = GetStrength(m_currentSide, shooters[IDX(m_currentSide)]);
    shootStrengths[IDX(sideEnemy)] = GetStrength(sideEnemy, shooters[IDX(sideEnemy)]);
    totalArmyStrength = GetStrength(
        m_currentSide,
        shooters[IDX(m_currentSide)] | flyerMask[IDX(m_currentSide)]
            | walkers[IDX(m_currentSide)] | oddMasks[IDX(m_currentSide)]
            | traitorArray[IDX(m_currentSide)]
    );
    fifth = static_cast<i32>(totalArmyStrength + COMBAT_AI_STRENGTH_ROUNDING)
                         / COMBAT_AI_STRENGTH_FRACTION;
    shootStrong = false;
    stronger = false;
    myShootPower = GetStrength(m_currentSide, shooters[IDX(m_currentSide)]);
    enemyShooters = GetStrength(sideEnemy, shooters[IDX(sideEnemy)]);

    if (m_inCastleCombat != 0) {
        if (m_heroes[IDX(COMBAT_ATTACKER_SIDE)]->m_secondarySkills[IDX(HERO_SKILL_ARCHERY)]
                == HERO_SKILL_LEVEL_NONE
            && !m_heroes[IDX(COMBAT_ATTACKER_SIDE)]->HasArtifact(ARTIFACT_GOLDEN_BOW)) {
            if (m_currentSide == COMBAT_ATTACKER_SIDE)
                myShootPower =
                    static_cast<i32>(myShootPower) / COMBAT_SIDE_COUNT;
            else
                enemyShooters =
                    static_cast<i32>(enemyShooters) / COMBAT_SIDE_COUNT;
        }
        if (m_wallStates[IDX(COMBAT_WALL_SLOT_KEEP)] == COMBAT_WALL_STATE_KEEP_STANDING) {
            pCastle = m_combatTowns[IDX(COMBAT_DEFENDER_SIDE)];
            m_combatTowns[IDX(COMBAT_DEFENDER_SIDE)]->CalcNumLevelArchers(&archers, &grade);
            plusArchers = 0;
            if (m_wallStates[IDX(COMBAT_WALL_SLOT_TOP_TOWER)]
                == COMBAT_WALL_STATE_TOWER_STANDING)
                plusArchers += archers / COMBAT_SIDE_COUNT;
            if (m_wallStates[IDX(COMBAT_WALL_SLOT_BOTTOM_TOWER)]
                == COMBAT_WALL_STATE_TOWER_STANDING)
                plusArchers += archers / COMBAT_SIDE_COUNT;
            archers += plusArchers;
            keepStrength = static_cast<i32>(
                archers * COMBAT_AI_TOWER_STRENGTH
                * (grade * COMBAT_AI_TOWER_LEVEL_SCALE + COMBAT_AI_TOWER_BASE_SCALE)
            );
            archeryBonus =
                m_heroes[IDX(COMBAT_ATTACKER_SIDE)] != NULL
                        && (m_heroes[IDX(COMBAT_ATTACKER_SIDE)]->HasArtifact(ARTIFACT_GOLDEN_BOW)
                            || m_heroes[IDX(COMBAT_ATTACKER_SIDE)]
                                       ->m_secondarySkills[IDX(HERO_SKILL_ARCHERY)]
                                   != HERO_SKILL_LEVEL_NONE)
                    ? 0
                    : 1;
            if (m_currentSide == COMBAT_DEFENDER_SIDE)
                myShootPower += keepStrength;
            else
                enemyShooters += keepStrength;
        }
    }

    if (m_heroes[IDX(m_currentSide)] != NULL) {
        myShootPower = static_cast<i32>(
            static_cast<i32>(myShootPower)
            * gfSSArcheryMod
                [IDX(m_heroes[IDX(m_currentSide)]->m_secondarySkills[IDX(HERO_SKILL_ARCHERY)])]
        );
    }
    if (m_heroes[IDX(OppositeCombatSide(m_currentSide))] != NULL) {
        enemyShooters = static_cast<i32>(
            static_cast<i32>(enemyShooters)
            * gfSSArcheryMod
                [IDX(m_heroes[IDX(OppositeCombatSide(m_currentSide))]
                         ->m_secondarySkills[IDX(HERO_SKILL_ARCHERY)])]
        );
    }
    if (static_cast<i32>(totalArmyStrength + COMBAT_AI_STRENGTH_ROUNDING)
            / COMBAT_AI_STRENGTH_FRACTION
        < static_cast<i32>(myShootPower))
        shootStrong = true;
    if (static_cast<i32>(enemyShooters) > static_cast<i32>(myShootPower))
        stronger = true;

    if (HAS(thisArmy->m_monster.flags.abilityFlags, MONSTER_ABILITY_FLAG_SHOOTER) != 0) {
        if (thisArmy->m_monster.shots > 0)
            plan = COMBAT_AI_ATTACK_SHOOT;
        else
            plan = COMBAT_AI_ATTACK_WALK;
    } else if (HAS(thisArmy->m_monster.flags.abilityFlags, MONSTER_ABILITY_FLAG_FLYING) != 0) {
        plan = COMBAT_AI_ATTACK_FLY;
    } else {
        plan = COMBAT_AI_ATTACK_WALK;
    }

    switch (plan) {
        case COMBAT_AI_ATTACK_SHOOT:
            if (AttemptAdjacentAttack(thisArmy))
                goto finish;
            if (thisArmy->m_monsterType == CREATURE_LICH
                || thisArmy->m_monsterType == CREATURE_POWER_LICH) {
                DoLichShot(thisArmy);
                goto finish;
            }
            best = GetBestArmy(sideEnemy, mirrorMask[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            best = GetBestArmy(sideEnemy, shooters[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            best = GetBestArmy(sideEnemy, flyerMask[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            if (walkers[IDX(sideEnemy)] != 0) {
                best =
                    GetClosestArmy(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]);
                if (best != -1) {
                    giNextAction = ACTION_MOVE;
                    giNextActionGridIndex =
                        (m_armies[IDX(sideEnemy)] + best)
                            ->m_hex;
                    goto finish;
                }
            }
            best = GetBestArmy(sideEnemy, oddMasks[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            best = GetBestArmy(sideEnemy, traitorArray[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            break;
        case COMBAT_AI_ATTACK_FLY:
            if (shootStrong != 0 && stronger == 0) {
                if (AttemptAttack(thisArmy, sideEnemy, mirrorMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                    goto finish;
            } else {
                if (AttemptAttack(thisArmy, sideEnemy, mirrorMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                    goto finish;
            }
            break;
        case COMBAT_AI_ATTACK_WALK:
            if (COMBAT_AI_ATTACK_NONE)
                goto finish;
            if (shootStrong != 0 && stronger == 0) {
                if (AttemptAttack(thisArmy, sideEnemy, mirrorMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                    goto finish;
                if (WalkTowardArmyFront(thisArmy, m_currentSide, shooters[IDX(m_currentSide)]))
                    goto finish;
                giNextAction = ACTION_WAIT;
                goto finish;
            }
            if (AttemptAttack(thisArmy, sideEnemy, mirrorMask[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                goto finish;

            rowLimit[0] = COMBAT_AI_CASTLE_BOUNDARY_ROW_0;
            rowLimit[1] = COMBAT_AI_CASTLE_BOUNDARY_ROW_1;
            rowLimit[2] = COMBAT_AI_CASTLE_BOUNDARY_ROW_2;
            rowLimit[3] = COMBAT_AI_CASTLE_BOUNDARY_ROW_3;
            rowLimit[4] = COMBAT_AI_CASTLE_BOUNDARY_ROW_4;
            rowLimit[5] = COMBAT_AI_CASTLE_BOUNDARY_ROW_5;
            rowLimit[6] = COMBAT_AI_CASTLE_BOUNDARY_ROW_6;
            rowLimit[7] = COMBAT_AI_CASTLE_BOUNDARY_ROW_7;
            rowLimit[8] = COMBAT_AI_CASTLE_BOUNDARY_ROW_8;
            rowIndex = thisArmy->m_hex / ARMY_HEX_COLUMNS;
            if (m_currentSide == COMBAT_ATTACKER_SIDE && m_inCastleCombat != 0
                && thisArmy->m_hex < rowLimit[rowIndex]) {
                targetHex = rowLimit[rowIndex];
                targetCell = &gpCombatManager->m_hexCells[targetHex];
                if (ValidHex(targetHex) && targetCell->m_occupantSide == COMBAT_SIDE_NONE
                    && targetCell->m_blocked == 0) {
                    giNextAction = ACTION_MOVE;
                    giNextActionGridIndex = targetHex;
                    goto finish;
                }
            }
            break;
    }
    giNextAction = ACTION_WAIT;

finish:
    if (giNextAction == ACTION_MOVE && giNextActionGridIndex > 0
        && giNextActionGridIndex < COMBAT_HEX_COUNT
        && gpCombatManager->m_hexCells[giNextActionGridIndex].m_occupantSide == COMBAT_SIDE_NONE) {
        for (dirIndex = COMBAT_DIRECTION_NORTHEAST;
             IDX(dirIndex) < COMBAT_AI_ADJACENT_DIRECTION_COUNT;
             dirIndex++) {
            adjCell = thisArmy->GetAdjacentCellIndex(
                giNextActionGridIndex, dirIndex
            );
            if (adjCell > 0 && adjCell < COMBAT_HEX_COUNT
                && gpCombatManager->m_hexCells[adjCell].m_occupantSide
                       == OppositeCombatSide(m_currentSide)) {
                giNextActionGridIndex = adjCell;
                return;
            }
        }
    }
}


AFTER strengths0_fifth0_castle0_archery1
void combatManager::DoCompAI(H2_ENUM_PARAM(CombatSide, i32)) {
    u32 traitorArray[COMBAT_SIDE_COUNT];
    i32 best;
    CombatSide sideEnemy;
    u32 flyerMask[COMBAT_SIDE_COUNT];
    u32 walkers[COMBAT_SIDE_COUNT];
    b32 stronger;
    b32 shootStrong;
    u32l myShootPower;
    i32 plan;
    u32l shootStrengths[COMBAT_SIDE_COUNT];
    u32l enemyShooters;
    army* thisArmy;
    u32 oddMasks[COMBAT_SIDE_COUNT];
    CombatHexDirection dirIndex;
    u32l totalArmyStrength;
    combatManager* H2_UNUSED(combat);
    i32 H2_UNUSED(fifth);
    u32 shooters[COMBAT_SIDE_COUNT];
    i32 archers;
    u32 mirrorMask[COMBAT_SIDE_COUNT];
    i32 rowIndex;
    i32 plusArchers;
    i32 keepStrength;
    i32 grade;
    town* H2_UNUSED(pCastle);
    u8 rowLimit[COMBAT_AI_CASTLE_BOUNDARY_COUNT];
    i32 adjCell;
    hexcell* targetCell;
    u32 targetHex;

    m_limitCreature = false;
    thisArmy = m_currentArmyIndex + m_armies[IDX(m_currentArmySide)];
    plan = COMBAT_AI_ATTACK_NONE;
    sideEnemy = OppositeCombatSide(m_currentSide);

    mirrorMask[IDX(m_currentSide)] = GetMirrorImageMask(m_currentSide);
    mirrorMask[IDX(sideEnemy)] = GetMirrorImageMask(sideEnemy);
    shooters[IDX(m_currentSide)] = GetShooterMask(m_currentSide);
    shooters[IDX(sideEnemy)] = GetShooterMask(sideEnemy);
    flyerMask[IDX(m_currentSide)] = GetFlyerMask(m_currentSide);
    flyerMask[IDX(sideEnemy)] = GetFlyerMask(sideEnemy);
    walkers[IDX(m_currentSide)] = GetWalkerMask(m_currentSide);
    walkers[IDX(sideEnemy)] = GetWalkerMask(sideEnemy);
    oddMasks[IDX(m_currentSide)] = GetOutOfItMask(m_currentSide);
    oddMasks[IDX(sideEnemy)] = GetOutOfItMask(sideEnemy);
    traitorArray[IDX(m_currentSide)] = GetTraitorMask(m_currentSide);
    traitorArray[IDX(sideEnemy)] = GetTraitorMask(sideEnemy);
    shootStrengths[IDX(m_currentSide)] = GetStrength(m_currentSide, shooters[IDX(m_currentSide)]);
    shootStrengths[IDX(sideEnemy)] = GetStrength(sideEnemy, shooters[IDX(sideEnemy)]);
    totalArmyStrength = GetStrength(
        m_currentSide,
        shooters[IDX(m_currentSide)] | flyerMask[IDX(m_currentSide)]
            | walkers[IDX(m_currentSide)] | oddMasks[IDX(m_currentSide)]
            | traitorArray[IDX(m_currentSide)]
    );
    fifth = static_cast<i32>(totalArmyStrength + COMBAT_AI_STRENGTH_ROUNDING)
                         / COMBAT_AI_STRENGTH_FRACTION;
    shootStrong = false;
    stronger = false;
    myShootPower = GetStrength(m_currentSide, shooters[IDX(m_currentSide)]);
    enemyShooters = GetStrength(sideEnemy, shooters[IDX(sideEnemy)]);

    if (m_inCastleCombat != 0) {
        if (m_heroes[IDX(COMBAT_ATTACKER_SIDE)]->m_secondarySkills[IDX(HERO_SKILL_ARCHERY)]
                == HERO_SKILL_LEVEL_NONE
            && !m_heroes[IDX(COMBAT_ATTACKER_SIDE)]->HasArtifact(ARTIFACT_GOLDEN_BOW)) {
            if (m_currentSide == COMBAT_ATTACKER_SIDE)
                myShootPower =
                    static_cast<i32>(myShootPower) / COMBAT_SIDE_COUNT;
            else
                enemyShooters =
                    static_cast<i32>(enemyShooters) / COMBAT_SIDE_COUNT;
        }
        if (m_wallStates[IDX(COMBAT_WALL_SLOT_KEEP)] == COMBAT_WALL_STATE_KEEP_STANDING) {
            pCastle = m_combatTowns[IDX(COMBAT_DEFENDER_SIDE)];
            m_combatTowns[IDX(COMBAT_DEFENDER_SIDE)]->CalcNumLevelArchers(&archers, &grade);
            plusArchers = 0;
            if (m_wallStates[IDX(COMBAT_WALL_SLOT_TOP_TOWER)]
                == COMBAT_WALL_STATE_TOWER_STANDING)
                plusArchers += archers / COMBAT_SIDE_COUNT;
            if (m_wallStates[IDX(COMBAT_WALL_SLOT_BOTTOM_TOWER)]
                == COMBAT_WALL_STATE_TOWER_STANDING)
                plusArchers += archers / COMBAT_SIDE_COUNT;
            archers += plusArchers;
            keepStrength = static_cast<i32>(
                archers * COMBAT_AI_TOWER_STRENGTH
                * (grade * COMBAT_AI_TOWER_LEVEL_SCALE + COMBAT_AI_TOWER_BASE_SCALE)
            );
                m_heroes[IDX(COMBAT_ATTACKER_SIDE)] != NULL
                        && (m_heroes[IDX(COMBAT_ATTACKER_SIDE)]->HasArtifact(ARTIFACT_GOLDEN_BOW)
                            || m_heroes[IDX(COMBAT_ATTACKER_SIDE)]
                                       ->m_secondarySkills[IDX(HERO_SKILL_ARCHERY)]
                                   != HERO_SKILL_LEVEL_NONE)
                    ? 0
                    : 1;
            if (m_currentSide == COMBAT_DEFENDER_SIDE)
                myShootPower += keepStrength;
            else
                enemyShooters += keepStrength;
        }
    }

    if (m_heroes[IDX(m_currentSide)] != NULL) {
        myShootPower = static_cast<i32>(
            static_cast<i32>(myShootPower)
            * gfSSArcheryMod
                [IDX(m_heroes[IDX(m_currentSide)]->m_secondarySkills[IDX(HERO_SKILL_ARCHERY)])]
        );
    }
    if (m_heroes[IDX(OppositeCombatSide(m_currentSide))] != NULL) {
        enemyShooters = static_cast<i32>(
            static_cast<i32>(enemyShooters)
            * gfSSArcheryMod
                [IDX(m_heroes[IDX(OppositeCombatSide(m_currentSide))]
                         ->m_secondarySkills[IDX(HERO_SKILL_ARCHERY)])]
        );
    }
    if (static_cast<i32>(totalArmyStrength + COMBAT_AI_STRENGTH_ROUNDING)
            / COMBAT_AI_STRENGTH_FRACTION
        < static_cast<i32>(myShootPower))
        shootStrong = true;
    if (static_cast<i32>(enemyShooters) > static_cast<i32>(myShootPower))
        stronger = true;

    if (HAS(thisArmy->m_monster.flags.abilityFlags, MONSTER_ABILITY_FLAG_SHOOTER) != 0) {
        if (thisArmy->m_monster.shots > 0)
            plan = COMBAT_AI_ATTACK_SHOOT;
        else
            plan = COMBAT_AI_ATTACK_WALK;
    } else if (HAS(thisArmy->m_monster.flags.abilityFlags, MONSTER_ABILITY_FLAG_FLYING) != 0) {
        plan = COMBAT_AI_ATTACK_FLY;
    } else {
        plan = COMBAT_AI_ATTACK_WALK;
    }

    switch (plan) {
        case COMBAT_AI_ATTACK_SHOOT:
            if (AttemptAdjacentAttack(thisArmy))
                goto finish;
            if (thisArmy->m_monsterType == CREATURE_LICH
                || thisArmy->m_monsterType == CREATURE_POWER_LICH) {
                DoLichShot(thisArmy);
                goto finish;
            }
            best = GetBestArmy(sideEnemy, mirrorMask[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            best = GetBestArmy(sideEnemy, shooters[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            best = GetBestArmy(sideEnemy, flyerMask[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            if (walkers[IDX(sideEnemy)] != 0) {
                best =
                    GetClosestArmy(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]);
                if (best != -1) {
                    giNextAction = ACTION_MOVE;
                    giNextActionGridIndex =
                        (m_armies[IDX(sideEnemy)] + best)
                            ->m_hex;
                    goto finish;
                }
            }
            best = GetBestArmy(sideEnemy, oddMasks[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            best = GetBestArmy(sideEnemy, traitorArray[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            break;
        case COMBAT_AI_ATTACK_FLY:
            if (shootStrong != 0 && stronger == 0) {
                if (AttemptAttack(thisArmy, sideEnemy, mirrorMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                    goto finish;
            } else {
                if (AttemptAttack(thisArmy, sideEnemy, mirrorMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                    goto finish;
            }
            break;
        case COMBAT_AI_ATTACK_WALK:
            if (COMBAT_AI_ATTACK_NONE)
                goto finish;
            if (shootStrong != 0 && stronger == 0) {
                if (AttemptAttack(thisArmy, sideEnemy, mirrorMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                    goto finish;
                if (WalkTowardArmyFront(thisArmy, m_currentSide, shooters[IDX(m_currentSide)]))
                    goto finish;
                giNextAction = ACTION_WAIT;
                goto finish;
            }
            if (AttemptAttack(thisArmy, sideEnemy, mirrorMask[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                goto finish;

            rowLimit[0] = COMBAT_AI_CASTLE_BOUNDARY_ROW_0;
            rowLimit[1] = COMBAT_AI_CASTLE_BOUNDARY_ROW_1;
            rowLimit[2] = COMBAT_AI_CASTLE_BOUNDARY_ROW_2;
            rowLimit[3] = COMBAT_AI_CASTLE_BOUNDARY_ROW_3;
            rowLimit[4] = COMBAT_AI_CASTLE_BOUNDARY_ROW_4;
            rowLimit[5] = COMBAT_AI_CASTLE_BOUNDARY_ROW_5;
            rowLimit[6] = COMBAT_AI_CASTLE_BOUNDARY_ROW_6;
            rowLimit[7] = COMBAT_AI_CASTLE_BOUNDARY_ROW_7;
            rowLimit[8] = COMBAT_AI_CASTLE_BOUNDARY_ROW_8;
            rowIndex = thisArmy->m_hex / ARMY_HEX_COLUMNS;
            if (m_currentSide == COMBAT_ATTACKER_SIDE && m_inCastleCombat != 0
                && thisArmy->m_hex < rowLimit[rowIndex]) {
                targetHex = rowLimit[rowIndex];
                targetCell = &gpCombatManager->m_hexCells[targetHex];
                if (ValidHex(targetHex) && targetCell->m_occupantSide == COMBAT_SIDE_NONE
                    && targetCell->m_blocked == 0) {
                    giNextAction = ACTION_MOVE;
                    giNextActionGridIndex = targetHex;
                    goto finish;
                }
            }
            break;
    }
    giNextAction = ACTION_WAIT;

finish:
    if (giNextAction == ACTION_MOVE && giNextActionGridIndex > 0
        && giNextActionGridIndex < COMBAT_HEX_COUNT
        && gpCombatManager->m_hexCells[giNextActionGridIndex].m_occupantSide == COMBAT_SIDE_NONE) {
        for (dirIndex = COMBAT_DIRECTION_NORTHEAST;
             IDX(dirIndex) < COMBAT_AI_ADJACENT_DIRECTION_COUNT;
             dirIndex++) {
            adjCell = thisArmy->GetAdjacentCellIndex(
                giNextActionGridIndex, dirIndex
            );
            if (adjCell > 0 && adjCell < COMBAT_HEX_COUNT
                && gpCombatManager->m_hexCells[adjCell].m_occupantSide
                       == OppositeCombatSide(m_currentSide)) {
                giNextActionGridIndex = adjCell;
                return;
            }
        }
    }
}


AFTER strengths0_fifth0_castle1_archery0
void combatManager::DoCompAI(H2_ENUM_PARAM(CombatSide, i32)) {
    u32 traitorArray[COMBAT_SIDE_COUNT];
    i32 best;
    CombatSide sideEnemy;
    u32 flyerMask[COMBAT_SIDE_COUNT];
    u32 walkers[COMBAT_SIDE_COUNT];
    b32 stronger;
    b32 shootStrong;
    u32l myShootPower;
    i32 plan;
    u32l shootStrengths[COMBAT_SIDE_COUNT];
    u32l enemyShooters;
    army* thisArmy;
    u32 oddMasks[COMBAT_SIDE_COUNT];
    CombatHexDirection dirIndex;
    u32l totalArmyStrength;
    combatManager* H2_UNUSED(combat);
    i32 H2_UNUSED(fifth);
    u32 shooters[COMBAT_SIDE_COUNT];
    i32 archers;
    u32 mirrorMask[COMBAT_SIDE_COUNT];
    i32 rowIndex;
    b32 H2_UNUSED(archeryBonus);
    i32 plusArchers;
    i32 keepStrength;
    i32 grade;
    u8 rowLimit[COMBAT_AI_CASTLE_BOUNDARY_COUNT];
    i32 adjCell;
    hexcell* targetCell;
    u32 targetHex;

    m_limitCreature = false;
    thisArmy = m_currentArmyIndex + m_armies[IDX(m_currentArmySide)];
    plan = COMBAT_AI_ATTACK_NONE;
    sideEnemy = OppositeCombatSide(m_currentSide);

    mirrorMask[IDX(m_currentSide)] = GetMirrorImageMask(m_currentSide);
    mirrorMask[IDX(sideEnemy)] = GetMirrorImageMask(sideEnemy);
    shooters[IDX(m_currentSide)] = GetShooterMask(m_currentSide);
    shooters[IDX(sideEnemy)] = GetShooterMask(sideEnemy);
    flyerMask[IDX(m_currentSide)] = GetFlyerMask(m_currentSide);
    flyerMask[IDX(sideEnemy)] = GetFlyerMask(sideEnemy);
    walkers[IDX(m_currentSide)] = GetWalkerMask(m_currentSide);
    walkers[IDX(sideEnemy)] = GetWalkerMask(sideEnemy);
    oddMasks[IDX(m_currentSide)] = GetOutOfItMask(m_currentSide);
    oddMasks[IDX(sideEnemy)] = GetOutOfItMask(sideEnemy);
    traitorArray[IDX(m_currentSide)] = GetTraitorMask(m_currentSide);
    traitorArray[IDX(sideEnemy)] = GetTraitorMask(sideEnemy);
    shootStrengths[IDX(m_currentSide)] = GetStrength(m_currentSide, shooters[IDX(m_currentSide)]);
    shootStrengths[IDX(sideEnemy)] = GetStrength(sideEnemy, shooters[IDX(sideEnemy)]);
    totalArmyStrength = GetStrength(
        m_currentSide,
        shooters[IDX(m_currentSide)] | flyerMask[IDX(m_currentSide)]
            | walkers[IDX(m_currentSide)] | oddMasks[IDX(m_currentSide)]
            | traitorArray[IDX(m_currentSide)]
    );
    fifth = static_cast<i32>(totalArmyStrength + COMBAT_AI_STRENGTH_ROUNDING)
                         / COMBAT_AI_STRENGTH_FRACTION;
    shootStrong = false;
    stronger = false;
    myShootPower = GetStrength(m_currentSide, shooters[IDX(m_currentSide)]);
    enemyShooters = GetStrength(sideEnemy, shooters[IDX(sideEnemy)]);

    if (m_inCastleCombat != 0) {
        if (m_heroes[IDX(COMBAT_ATTACKER_SIDE)]->m_secondarySkills[IDX(HERO_SKILL_ARCHERY)]
                == HERO_SKILL_LEVEL_NONE
            && !m_heroes[IDX(COMBAT_ATTACKER_SIDE)]->HasArtifact(ARTIFACT_GOLDEN_BOW)) {
            if (m_currentSide == COMBAT_ATTACKER_SIDE)
                myShootPower =
                    static_cast<i32>(myShootPower) / COMBAT_SIDE_COUNT;
            else
                enemyShooters =
                    static_cast<i32>(enemyShooters) / COMBAT_SIDE_COUNT;
        }
        if (m_wallStates[IDX(COMBAT_WALL_SLOT_KEEP)] == COMBAT_WALL_STATE_KEEP_STANDING) {
            m_combatTowns[IDX(COMBAT_DEFENDER_SIDE)]->CalcNumLevelArchers(&archers, &grade);
            plusArchers = 0;
            if (m_wallStates[IDX(COMBAT_WALL_SLOT_TOP_TOWER)]
                == COMBAT_WALL_STATE_TOWER_STANDING)
                plusArchers += archers / COMBAT_SIDE_COUNT;
            if (m_wallStates[IDX(COMBAT_WALL_SLOT_BOTTOM_TOWER)]
                == COMBAT_WALL_STATE_TOWER_STANDING)
                plusArchers += archers / COMBAT_SIDE_COUNT;
            archers += plusArchers;
            keepStrength = static_cast<i32>(
                archers * COMBAT_AI_TOWER_STRENGTH
                * (grade * COMBAT_AI_TOWER_LEVEL_SCALE + COMBAT_AI_TOWER_BASE_SCALE)
            );
            archeryBonus =
                m_heroes[IDX(COMBAT_ATTACKER_SIDE)] != NULL
                        && (m_heroes[IDX(COMBAT_ATTACKER_SIDE)]->HasArtifact(ARTIFACT_GOLDEN_BOW)
                            || m_heroes[IDX(COMBAT_ATTACKER_SIDE)]
                                       ->m_secondarySkills[IDX(HERO_SKILL_ARCHERY)]
                                   != HERO_SKILL_LEVEL_NONE)
                    ? 0
                    : 1;
            if (m_currentSide == COMBAT_DEFENDER_SIDE)
                myShootPower += keepStrength;
            else
                enemyShooters += keepStrength;
        }
    }

    if (m_heroes[IDX(m_currentSide)] != NULL) {
        myShootPower = static_cast<i32>(
            static_cast<i32>(myShootPower)
            * gfSSArcheryMod
                [IDX(m_heroes[IDX(m_currentSide)]->m_secondarySkills[IDX(HERO_SKILL_ARCHERY)])]
        );
    }
    if (m_heroes[IDX(OppositeCombatSide(m_currentSide))] != NULL) {
        enemyShooters = static_cast<i32>(
            static_cast<i32>(enemyShooters)
            * gfSSArcheryMod
                [IDX(m_heroes[IDX(OppositeCombatSide(m_currentSide))]
                         ->m_secondarySkills[IDX(HERO_SKILL_ARCHERY)])]
        );
    }
    if (static_cast<i32>(totalArmyStrength + COMBAT_AI_STRENGTH_ROUNDING)
            / COMBAT_AI_STRENGTH_FRACTION
        < static_cast<i32>(myShootPower))
        shootStrong = true;
    if (static_cast<i32>(enemyShooters) > static_cast<i32>(myShootPower))
        stronger = true;

    if (HAS(thisArmy->m_monster.flags.abilityFlags, MONSTER_ABILITY_FLAG_SHOOTER) != 0) {
        if (thisArmy->m_monster.shots > 0)
            plan = COMBAT_AI_ATTACK_SHOOT;
        else
            plan = COMBAT_AI_ATTACK_WALK;
    } else if (HAS(thisArmy->m_monster.flags.abilityFlags, MONSTER_ABILITY_FLAG_FLYING) != 0) {
        plan = COMBAT_AI_ATTACK_FLY;
    } else {
        plan = COMBAT_AI_ATTACK_WALK;
    }

    switch (plan) {
        case COMBAT_AI_ATTACK_SHOOT:
            if (AttemptAdjacentAttack(thisArmy))
                goto finish;
            if (thisArmy->m_monsterType == CREATURE_LICH
                || thisArmy->m_monsterType == CREATURE_POWER_LICH) {
                DoLichShot(thisArmy);
                goto finish;
            }
            best = GetBestArmy(sideEnemy, mirrorMask[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            best = GetBestArmy(sideEnemy, shooters[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            best = GetBestArmy(sideEnemy, flyerMask[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            if (walkers[IDX(sideEnemy)] != 0) {
                best =
                    GetClosestArmy(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]);
                if (best != -1) {
                    giNextAction = ACTION_MOVE;
                    giNextActionGridIndex =
                        (m_armies[IDX(sideEnemy)] + best)
                            ->m_hex;
                    goto finish;
                }
            }
            best = GetBestArmy(sideEnemy, oddMasks[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            best = GetBestArmy(sideEnemy, traitorArray[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            break;
        case COMBAT_AI_ATTACK_FLY:
            if (shootStrong != 0 && stronger == 0) {
                if (AttemptAttack(thisArmy, sideEnemy, mirrorMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                    goto finish;
            } else {
                if (AttemptAttack(thisArmy, sideEnemy, mirrorMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                    goto finish;
            }
            break;
        case COMBAT_AI_ATTACK_WALK:
            if (COMBAT_AI_ATTACK_NONE)
                goto finish;
            if (shootStrong != 0 && stronger == 0) {
                if (AttemptAttack(thisArmy, sideEnemy, mirrorMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                    goto finish;
                if (WalkTowardArmyFront(thisArmy, m_currentSide, shooters[IDX(m_currentSide)]))
                    goto finish;
                giNextAction = ACTION_WAIT;
                goto finish;
            }
            if (AttemptAttack(thisArmy, sideEnemy, mirrorMask[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                goto finish;

            rowLimit[0] = COMBAT_AI_CASTLE_BOUNDARY_ROW_0;
            rowLimit[1] = COMBAT_AI_CASTLE_BOUNDARY_ROW_1;
            rowLimit[2] = COMBAT_AI_CASTLE_BOUNDARY_ROW_2;
            rowLimit[3] = COMBAT_AI_CASTLE_BOUNDARY_ROW_3;
            rowLimit[4] = COMBAT_AI_CASTLE_BOUNDARY_ROW_4;
            rowLimit[5] = COMBAT_AI_CASTLE_BOUNDARY_ROW_5;
            rowLimit[6] = COMBAT_AI_CASTLE_BOUNDARY_ROW_6;
            rowLimit[7] = COMBAT_AI_CASTLE_BOUNDARY_ROW_7;
            rowLimit[8] = COMBAT_AI_CASTLE_BOUNDARY_ROW_8;
            rowIndex = thisArmy->m_hex / ARMY_HEX_COLUMNS;
            if (m_currentSide == COMBAT_ATTACKER_SIDE && m_inCastleCombat != 0
                && thisArmy->m_hex < rowLimit[rowIndex]) {
                targetHex = rowLimit[rowIndex];
                targetCell = &gpCombatManager->m_hexCells[targetHex];
                if (ValidHex(targetHex) && targetCell->m_occupantSide == COMBAT_SIDE_NONE
                    && targetCell->m_blocked == 0) {
                    giNextAction = ACTION_MOVE;
                    giNextActionGridIndex = targetHex;
                    goto finish;
                }
            }
            break;
    }
    giNextAction = ACTION_WAIT;

finish:
    if (giNextAction == ACTION_MOVE && giNextActionGridIndex > 0
        && giNextActionGridIndex < COMBAT_HEX_COUNT
        && gpCombatManager->m_hexCells[giNextActionGridIndex].m_occupantSide == COMBAT_SIDE_NONE) {
        for (dirIndex = COMBAT_DIRECTION_NORTHEAST;
             IDX(dirIndex) < COMBAT_AI_ADJACENT_DIRECTION_COUNT;
             dirIndex++) {
            adjCell = thisArmy->GetAdjacentCellIndex(
                giNextActionGridIndex, dirIndex
            );
            if (adjCell > 0 && adjCell < COMBAT_HEX_COUNT
                && gpCombatManager->m_hexCells[adjCell].m_occupantSide
                       == OppositeCombatSide(m_currentSide)) {
                giNextActionGridIndex = adjCell;
                return;
            }
        }
    }
}


AFTER strengths0_fifth0_castle1_archery1
void combatManager::DoCompAI(H2_ENUM_PARAM(CombatSide, i32)) {
    u32 traitorArray[COMBAT_SIDE_COUNT];
    i32 best;
    CombatSide sideEnemy;
    u32 flyerMask[COMBAT_SIDE_COUNT];
    u32 walkers[COMBAT_SIDE_COUNT];
    b32 stronger;
    b32 shootStrong;
    u32l myShootPower;
    i32 plan;
    u32l shootStrengths[COMBAT_SIDE_COUNT];
    u32l enemyShooters;
    army* thisArmy;
    u32 oddMasks[COMBAT_SIDE_COUNT];
    CombatHexDirection dirIndex;
    u32l totalArmyStrength;
    combatManager* H2_UNUSED(combat);
    i32 H2_UNUSED(fifth);
    u32 shooters[COMBAT_SIDE_COUNT];
    i32 archers;
    u32 mirrorMask[COMBAT_SIDE_COUNT];
    i32 rowIndex;
    i32 plusArchers;
    i32 keepStrength;
    i32 grade;
    u8 rowLimit[COMBAT_AI_CASTLE_BOUNDARY_COUNT];
    i32 adjCell;
    hexcell* targetCell;
    u32 targetHex;

    m_limitCreature = false;
    thisArmy = m_currentArmyIndex + m_armies[IDX(m_currentArmySide)];
    plan = COMBAT_AI_ATTACK_NONE;
    sideEnemy = OppositeCombatSide(m_currentSide);

    mirrorMask[IDX(m_currentSide)] = GetMirrorImageMask(m_currentSide);
    mirrorMask[IDX(sideEnemy)] = GetMirrorImageMask(sideEnemy);
    shooters[IDX(m_currentSide)] = GetShooterMask(m_currentSide);
    shooters[IDX(sideEnemy)] = GetShooterMask(sideEnemy);
    flyerMask[IDX(m_currentSide)] = GetFlyerMask(m_currentSide);
    flyerMask[IDX(sideEnemy)] = GetFlyerMask(sideEnemy);
    walkers[IDX(m_currentSide)] = GetWalkerMask(m_currentSide);
    walkers[IDX(sideEnemy)] = GetWalkerMask(sideEnemy);
    oddMasks[IDX(m_currentSide)] = GetOutOfItMask(m_currentSide);
    oddMasks[IDX(sideEnemy)] = GetOutOfItMask(sideEnemy);
    traitorArray[IDX(m_currentSide)] = GetTraitorMask(m_currentSide);
    traitorArray[IDX(sideEnemy)] = GetTraitorMask(sideEnemy);
    shootStrengths[IDX(m_currentSide)] = GetStrength(m_currentSide, shooters[IDX(m_currentSide)]);
    shootStrengths[IDX(sideEnemy)] = GetStrength(sideEnemy, shooters[IDX(sideEnemy)]);
    totalArmyStrength = GetStrength(
        m_currentSide,
        shooters[IDX(m_currentSide)] | flyerMask[IDX(m_currentSide)]
            | walkers[IDX(m_currentSide)] | oddMasks[IDX(m_currentSide)]
            | traitorArray[IDX(m_currentSide)]
    );
    fifth = static_cast<i32>(totalArmyStrength + COMBAT_AI_STRENGTH_ROUNDING)
                         / COMBAT_AI_STRENGTH_FRACTION;
    shootStrong = false;
    stronger = false;
    myShootPower = GetStrength(m_currentSide, shooters[IDX(m_currentSide)]);
    enemyShooters = GetStrength(sideEnemy, shooters[IDX(sideEnemy)]);

    if (m_inCastleCombat != 0) {
        if (m_heroes[IDX(COMBAT_ATTACKER_SIDE)]->m_secondarySkills[IDX(HERO_SKILL_ARCHERY)]
                == HERO_SKILL_LEVEL_NONE
            && !m_heroes[IDX(COMBAT_ATTACKER_SIDE)]->HasArtifact(ARTIFACT_GOLDEN_BOW)) {
            if (m_currentSide == COMBAT_ATTACKER_SIDE)
                myShootPower =
                    static_cast<i32>(myShootPower) / COMBAT_SIDE_COUNT;
            else
                enemyShooters =
                    static_cast<i32>(enemyShooters) / COMBAT_SIDE_COUNT;
        }
        if (m_wallStates[IDX(COMBAT_WALL_SLOT_KEEP)] == COMBAT_WALL_STATE_KEEP_STANDING) {
            m_combatTowns[IDX(COMBAT_DEFENDER_SIDE)]->CalcNumLevelArchers(&archers, &grade);
            plusArchers = 0;
            if (m_wallStates[IDX(COMBAT_WALL_SLOT_TOP_TOWER)]
                == COMBAT_WALL_STATE_TOWER_STANDING)
                plusArchers += archers / COMBAT_SIDE_COUNT;
            if (m_wallStates[IDX(COMBAT_WALL_SLOT_BOTTOM_TOWER)]
                == COMBAT_WALL_STATE_TOWER_STANDING)
                plusArchers += archers / COMBAT_SIDE_COUNT;
            archers += plusArchers;
            keepStrength = static_cast<i32>(
                archers * COMBAT_AI_TOWER_STRENGTH
                * (grade * COMBAT_AI_TOWER_LEVEL_SCALE + COMBAT_AI_TOWER_BASE_SCALE)
            );
                m_heroes[IDX(COMBAT_ATTACKER_SIDE)] != NULL
                        && (m_heroes[IDX(COMBAT_ATTACKER_SIDE)]->HasArtifact(ARTIFACT_GOLDEN_BOW)
                            || m_heroes[IDX(COMBAT_ATTACKER_SIDE)]
                                       ->m_secondarySkills[IDX(HERO_SKILL_ARCHERY)]
                                   != HERO_SKILL_LEVEL_NONE)
                    ? 0
                    : 1;
            if (m_currentSide == COMBAT_DEFENDER_SIDE)
                myShootPower += keepStrength;
            else
                enemyShooters += keepStrength;
        }
    }

    if (m_heroes[IDX(m_currentSide)] != NULL) {
        myShootPower = static_cast<i32>(
            static_cast<i32>(myShootPower)
            * gfSSArcheryMod
                [IDX(m_heroes[IDX(m_currentSide)]->m_secondarySkills[IDX(HERO_SKILL_ARCHERY)])]
        );
    }
    if (m_heroes[IDX(OppositeCombatSide(m_currentSide))] != NULL) {
        enemyShooters = static_cast<i32>(
            static_cast<i32>(enemyShooters)
            * gfSSArcheryMod
                [IDX(m_heroes[IDX(OppositeCombatSide(m_currentSide))]
                         ->m_secondarySkills[IDX(HERO_SKILL_ARCHERY)])]
        );
    }
    if (static_cast<i32>(totalArmyStrength + COMBAT_AI_STRENGTH_ROUNDING)
            / COMBAT_AI_STRENGTH_FRACTION
        < static_cast<i32>(myShootPower))
        shootStrong = true;
    if (static_cast<i32>(enemyShooters) > static_cast<i32>(myShootPower))
        stronger = true;

    if (HAS(thisArmy->m_monster.flags.abilityFlags, MONSTER_ABILITY_FLAG_SHOOTER) != 0) {
        if (thisArmy->m_monster.shots > 0)
            plan = COMBAT_AI_ATTACK_SHOOT;
        else
            plan = COMBAT_AI_ATTACK_WALK;
    } else if (HAS(thisArmy->m_monster.flags.abilityFlags, MONSTER_ABILITY_FLAG_FLYING) != 0) {
        plan = COMBAT_AI_ATTACK_FLY;
    } else {
        plan = COMBAT_AI_ATTACK_WALK;
    }

    switch (plan) {
        case COMBAT_AI_ATTACK_SHOOT:
            if (AttemptAdjacentAttack(thisArmy))
                goto finish;
            if (thisArmy->m_monsterType == CREATURE_LICH
                || thisArmy->m_monsterType == CREATURE_POWER_LICH) {
                DoLichShot(thisArmy);
                goto finish;
            }
            best = GetBestArmy(sideEnemy, mirrorMask[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            best = GetBestArmy(sideEnemy, shooters[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            best = GetBestArmy(sideEnemy, flyerMask[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            if (walkers[IDX(sideEnemy)] != 0) {
                best =
                    GetClosestArmy(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]);
                if (best != -1) {
                    giNextAction = ACTION_MOVE;
                    giNextActionGridIndex =
                        (m_armies[IDX(sideEnemy)] + best)
                            ->m_hex;
                    goto finish;
                }
            }
            best = GetBestArmy(sideEnemy, oddMasks[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            best = GetBestArmy(sideEnemy, traitorArray[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            break;
        case COMBAT_AI_ATTACK_FLY:
            if (shootStrong != 0 && stronger == 0) {
                if (AttemptAttack(thisArmy, sideEnemy, mirrorMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                    goto finish;
            } else {
                if (AttemptAttack(thisArmy, sideEnemy, mirrorMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                    goto finish;
            }
            break;
        case COMBAT_AI_ATTACK_WALK:
            if (COMBAT_AI_ATTACK_NONE)
                goto finish;
            if (shootStrong != 0 && stronger == 0) {
                if (AttemptAttack(thisArmy, sideEnemy, mirrorMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                    goto finish;
                if (WalkTowardArmyFront(thisArmy, m_currentSide, shooters[IDX(m_currentSide)]))
                    goto finish;
                giNextAction = ACTION_WAIT;
                goto finish;
            }
            if (AttemptAttack(thisArmy, sideEnemy, mirrorMask[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                goto finish;

            rowLimit[0] = COMBAT_AI_CASTLE_BOUNDARY_ROW_0;
            rowLimit[1] = COMBAT_AI_CASTLE_BOUNDARY_ROW_1;
            rowLimit[2] = COMBAT_AI_CASTLE_BOUNDARY_ROW_2;
            rowLimit[3] = COMBAT_AI_CASTLE_BOUNDARY_ROW_3;
            rowLimit[4] = COMBAT_AI_CASTLE_BOUNDARY_ROW_4;
            rowLimit[5] = COMBAT_AI_CASTLE_BOUNDARY_ROW_5;
            rowLimit[6] = COMBAT_AI_CASTLE_BOUNDARY_ROW_6;
            rowLimit[7] = COMBAT_AI_CASTLE_BOUNDARY_ROW_7;
            rowLimit[8] = COMBAT_AI_CASTLE_BOUNDARY_ROW_8;
            rowIndex = thisArmy->m_hex / ARMY_HEX_COLUMNS;
            if (m_currentSide == COMBAT_ATTACKER_SIDE && m_inCastleCombat != 0
                && thisArmy->m_hex < rowLimit[rowIndex]) {
                targetHex = rowLimit[rowIndex];
                targetCell = &gpCombatManager->m_hexCells[targetHex];
                if (ValidHex(targetHex) && targetCell->m_occupantSide == COMBAT_SIDE_NONE
                    && targetCell->m_blocked == 0) {
                    giNextAction = ACTION_MOVE;
                    giNextActionGridIndex = targetHex;
                    goto finish;
                }
            }
            break;
    }
    giNextAction = ACTION_WAIT;

finish:
    if (giNextAction == ACTION_MOVE && giNextActionGridIndex > 0
        && giNextActionGridIndex < COMBAT_HEX_COUNT
        && gpCombatManager->m_hexCells[giNextActionGridIndex].m_occupantSide == COMBAT_SIDE_NONE) {
        for (dirIndex = COMBAT_DIRECTION_NORTHEAST;
             IDX(dirIndex) < COMBAT_AI_ADJACENT_DIRECTION_COUNT;
             dirIndex++) {
            adjCell = thisArmy->GetAdjacentCellIndex(
                giNextActionGridIndex, dirIndex
            );
            if (adjCell > 0 && adjCell < COMBAT_HEX_COUNT
                && gpCombatManager->m_hexCells[adjCell].m_occupantSide
                       == OppositeCombatSide(m_currentSide)) {
                giNextActionGridIndex = adjCell;
                return;
            }
        }
    }
}


AFTER strengths0_fifth1_castle0_archery0
void combatManager::DoCompAI(H2_ENUM_PARAM(CombatSide, i32)) {
    u32 traitorArray[COMBAT_SIDE_COUNT];
    i32 best;
    CombatSide sideEnemy;
    u32 flyerMask[COMBAT_SIDE_COUNT];
    u32 walkers[COMBAT_SIDE_COUNT];
    b32 stronger;
    b32 shootStrong;
    u32l myShootPower;
    i32 plan;
    u32l shootStrengths[COMBAT_SIDE_COUNT];
    u32l enemyShooters;
    army* thisArmy;
    u32 oddMasks[COMBAT_SIDE_COUNT];
    CombatHexDirection dirIndex;
    u32l totalArmyStrength;
    combatManager* H2_UNUSED(combat);
    u32 shooters[COMBAT_SIDE_COUNT];
    i32 archers;
    u32 mirrorMask[COMBAT_SIDE_COUNT];
    i32 rowIndex;
    b32 H2_UNUSED(archeryBonus);
    i32 plusArchers;
    i32 keepStrength;
    i32 grade;
    town* H2_UNUSED(pCastle);
    u8 rowLimit[COMBAT_AI_CASTLE_BOUNDARY_COUNT];
    i32 adjCell;
    hexcell* targetCell;
    u32 targetHex;

    m_limitCreature = false;
    thisArmy = m_currentArmyIndex + m_armies[IDX(m_currentArmySide)];
    plan = COMBAT_AI_ATTACK_NONE;
    sideEnemy = OppositeCombatSide(m_currentSide);

    mirrorMask[IDX(m_currentSide)] = GetMirrorImageMask(m_currentSide);
    mirrorMask[IDX(sideEnemy)] = GetMirrorImageMask(sideEnemy);
    shooters[IDX(m_currentSide)] = GetShooterMask(m_currentSide);
    shooters[IDX(sideEnemy)] = GetShooterMask(sideEnemy);
    flyerMask[IDX(m_currentSide)] = GetFlyerMask(m_currentSide);
    flyerMask[IDX(sideEnemy)] = GetFlyerMask(sideEnemy);
    walkers[IDX(m_currentSide)] = GetWalkerMask(m_currentSide);
    walkers[IDX(sideEnemy)] = GetWalkerMask(sideEnemy);
    oddMasks[IDX(m_currentSide)] = GetOutOfItMask(m_currentSide);
    oddMasks[IDX(sideEnemy)] = GetOutOfItMask(sideEnemy);
    traitorArray[IDX(m_currentSide)] = GetTraitorMask(m_currentSide);
    traitorArray[IDX(sideEnemy)] = GetTraitorMask(sideEnemy);
    shootStrengths[IDX(m_currentSide)] = GetStrength(m_currentSide, shooters[IDX(m_currentSide)]);
    shootStrengths[IDX(sideEnemy)] = GetStrength(sideEnemy, shooters[IDX(sideEnemy)]);
    totalArmyStrength = GetStrength(
        m_currentSide,
        shooters[IDX(m_currentSide)] | flyerMask[IDX(m_currentSide)]
            | walkers[IDX(m_currentSide)] | oddMasks[IDX(m_currentSide)]
            | traitorArray[IDX(m_currentSide)]
    );
    static_cast<i32>(totalArmyStrength + COMBAT_AI_STRENGTH_ROUNDING)
                         / COMBAT_AI_STRENGTH_FRACTION;
    shootStrong = false;
    stronger = false;
    myShootPower = GetStrength(m_currentSide, shooters[IDX(m_currentSide)]);
    enemyShooters = GetStrength(sideEnemy, shooters[IDX(sideEnemy)]);

    if (m_inCastleCombat != 0) {
        if (m_heroes[IDX(COMBAT_ATTACKER_SIDE)]->m_secondarySkills[IDX(HERO_SKILL_ARCHERY)]
                == HERO_SKILL_LEVEL_NONE
            && !m_heroes[IDX(COMBAT_ATTACKER_SIDE)]->HasArtifact(ARTIFACT_GOLDEN_BOW)) {
            if (m_currentSide == COMBAT_ATTACKER_SIDE)
                myShootPower =
                    static_cast<i32>(myShootPower) / COMBAT_SIDE_COUNT;
            else
                enemyShooters =
                    static_cast<i32>(enemyShooters) / COMBAT_SIDE_COUNT;
        }
        if (m_wallStates[IDX(COMBAT_WALL_SLOT_KEEP)] == COMBAT_WALL_STATE_KEEP_STANDING) {
            pCastle = m_combatTowns[IDX(COMBAT_DEFENDER_SIDE)];
            m_combatTowns[IDX(COMBAT_DEFENDER_SIDE)]->CalcNumLevelArchers(&archers, &grade);
            plusArchers = 0;
            if (m_wallStates[IDX(COMBAT_WALL_SLOT_TOP_TOWER)]
                == COMBAT_WALL_STATE_TOWER_STANDING)
                plusArchers += archers / COMBAT_SIDE_COUNT;
            if (m_wallStates[IDX(COMBAT_WALL_SLOT_BOTTOM_TOWER)]
                == COMBAT_WALL_STATE_TOWER_STANDING)
                plusArchers += archers / COMBAT_SIDE_COUNT;
            archers += plusArchers;
            keepStrength = static_cast<i32>(
                archers * COMBAT_AI_TOWER_STRENGTH
                * (grade * COMBAT_AI_TOWER_LEVEL_SCALE + COMBAT_AI_TOWER_BASE_SCALE)
            );
            archeryBonus =
                m_heroes[IDX(COMBAT_ATTACKER_SIDE)] != NULL
                        && (m_heroes[IDX(COMBAT_ATTACKER_SIDE)]->HasArtifact(ARTIFACT_GOLDEN_BOW)
                            || m_heroes[IDX(COMBAT_ATTACKER_SIDE)]
                                       ->m_secondarySkills[IDX(HERO_SKILL_ARCHERY)]
                                   != HERO_SKILL_LEVEL_NONE)
                    ? 0
                    : 1;
            if (m_currentSide == COMBAT_DEFENDER_SIDE)
                myShootPower += keepStrength;
            else
                enemyShooters += keepStrength;
        }
    }

    if (m_heroes[IDX(m_currentSide)] != NULL) {
        myShootPower = static_cast<i32>(
            static_cast<i32>(myShootPower)
            * gfSSArcheryMod
                [IDX(m_heroes[IDX(m_currentSide)]->m_secondarySkills[IDX(HERO_SKILL_ARCHERY)])]
        );
    }
    if (m_heroes[IDX(OppositeCombatSide(m_currentSide))] != NULL) {
        enemyShooters = static_cast<i32>(
            static_cast<i32>(enemyShooters)
            * gfSSArcheryMod
                [IDX(m_heroes[IDX(OppositeCombatSide(m_currentSide))]
                         ->m_secondarySkills[IDX(HERO_SKILL_ARCHERY)])]
        );
    }
    if (static_cast<i32>(totalArmyStrength + COMBAT_AI_STRENGTH_ROUNDING)
            / COMBAT_AI_STRENGTH_FRACTION
        < static_cast<i32>(myShootPower))
        shootStrong = true;
    if (static_cast<i32>(enemyShooters) > static_cast<i32>(myShootPower))
        stronger = true;

    if (HAS(thisArmy->m_monster.flags.abilityFlags, MONSTER_ABILITY_FLAG_SHOOTER) != 0) {
        if (thisArmy->m_monster.shots > 0)
            plan = COMBAT_AI_ATTACK_SHOOT;
        else
            plan = COMBAT_AI_ATTACK_WALK;
    } else if (HAS(thisArmy->m_monster.flags.abilityFlags, MONSTER_ABILITY_FLAG_FLYING) != 0) {
        plan = COMBAT_AI_ATTACK_FLY;
    } else {
        plan = COMBAT_AI_ATTACK_WALK;
    }

    switch (plan) {
        case COMBAT_AI_ATTACK_SHOOT:
            if (AttemptAdjacentAttack(thisArmy))
                goto finish;
            if (thisArmy->m_monsterType == CREATURE_LICH
                || thisArmy->m_monsterType == CREATURE_POWER_LICH) {
                DoLichShot(thisArmy);
                goto finish;
            }
            best = GetBestArmy(sideEnemy, mirrorMask[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            best = GetBestArmy(sideEnemy, shooters[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            best = GetBestArmy(sideEnemy, flyerMask[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            if (walkers[IDX(sideEnemy)] != 0) {
                best =
                    GetClosestArmy(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]);
                if (best != -1) {
                    giNextAction = ACTION_MOVE;
                    giNextActionGridIndex =
                        (m_armies[IDX(sideEnemy)] + best)
                            ->m_hex;
                    goto finish;
                }
            }
            best = GetBestArmy(sideEnemy, oddMasks[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            best = GetBestArmy(sideEnemy, traitorArray[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            break;
        case COMBAT_AI_ATTACK_FLY:
            if (shootStrong != 0 && stronger == 0) {
                if (AttemptAttack(thisArmy, sideEnemy, mirrorMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                    goto finish;
            } else {
                if (AttemptAttack(thisArmy, sideEnemy, mirrorMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                    goto finish;
            }
            break;
        case COMBAT_AI_ATTACK_WALK:
            if (COMBAT_AI_ATTACK_NONE)
                goto finish;
            if (shootStrong != 0 && stronger == 0) {
                if (AttemptAttack(thisArmy, sideEnemy, mirrorMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                    goto finish;
                if (WalkTowardArmyFront(thisArmy, m_currentSide, shooters[IDX(m_currentSide)]))
                    goto finish;
                giNextAction = ACTION_WAIT;
                goto finish;
            }
            if (AttemptAttack(thisArmy, sideEnemy, mirrorMask[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                goto finish;

            rowLimit[0] = COMBAT_AI_CASTLE_BOUNDARY_ROW_0;
            rowLimit[1] = COMBAT_AI_CASTLE_BOUNDARY_ROW_1;
            rowLimit[2] = COMBAT_AI_CASTLE_BOUNDARY_ROW_2;
            rowLimit[3] = COMBAT_AI_CASTLE_BOUNDARY_ROW_3;
            rowLimit[4] = COMBAT_AI_CASTLE_BOUNDARY_ROW_4;
            rowLimit[5] = COMBAT_AI_CASTLE_BOUNDARY_ROW_5;
            rowLimit[6] = COMBAT_AI_CASTLE_BOUNDARY_ROW_6;
            rowLimit[7] = COMBAT_AI_CASTLE_BOUNDARY_ROW_7;
            rowLimit[8] = COMBAT_AI_CASTLE_BOUNDARY_ROW_8;
            rowIndex = thisArmy->m_hex / ARMY_HEX_COLUMNS;
            if (m_currentSide == COMBAT_ATTACKER_SIDE && m_inCastleCombat != 0
                && thisArmy->m_hex < rowLimit[rowIndex]) {
                targetHex = rowLimit[rowIndex];
                targetCell = &gpCombatManager->m_hexCells[targetHex];
                if (ValidHex(targetHex) && targetCell->m_occupantSide == COMBAT_SIDE_NONE
                    && targetCell->m_blocked == 0) {
                    giNextAction = ACTION_MOVE;
                    giNextActionGridIndex = targetHex;
                    goto finish;
                }
            }
            break;
    }
    giNextAction = ACTION_WAIT;

finish:
    if (giNextAction == ACTION_MOVE && giNextActionGridIndex > 0
        && giNextActionGridIndex < COMBAT_HEX_COUNT
        && gpCombatManager->m_hexCells[giNextActionGridIndex].m_occupantSide == COMBAT_SIDE_NONE) {
        for (dirIndex = COMBAT_DIRECTION_NORTHEAST;
             IDX(dirIndex) < COMBAT_AI_ADJACENT_DIRECTION_COUNT;
             dirIndex++) {
            adjCell = thisArmy->GetAdjacentCellIndex(
                giNextActionGridIndex, dirIndex
            );
            if (adjCell > 0 && adjCell < COMBAT_HEX_COUNT
                && gpCombatManager->m_hexCells[adjCell].m_occupantSide
                       == OppositeCombatSide(m_currentSide)) {
                giNextActionGridIndex = adjCell;
                return;
            }
        }
    }
}


AFTER strengths0_fifth1_castle0_archery1
void combatManager::DoCompAI(H2_ENUM_PARAM(CombatSide, i32)) {
    u32 traitorArray[COMBAT_SIDE_COUNT];
    i32 best;
    CombatSide sideEnemy;
    u32 flyerMask[COMBAT_SIDE_COUNT];
    u32 walkers[COMBAT_SIDE_COUNT];
    b32 stronger;
    b32 shootStrong;
    u32l myShootPower;
    i32 plan;
    u32l shootStrengths[COMBAT_SIDE_COUNT];
    u32l enemyShooters;
    army* thisArmy;
    u32 oddMasks[COMBAT_SIDE_COUNT];
    CombatHexDirection dirIndex;
    u32l totalArmyStrength;
    combatManager* H2_UNUSED(combat);
    u32 shooters[COMBAT_SIDE_COUNT];
    i32 archers;
    u32 mirrorMask[COMBAT_SIDE_COUNT];
    i32 rowIndex;
    i32 plusArchers;
    i32 keepStrength;
    i32 grade;
    town* H2_UNUSED(pCastle);
    u8 rowLimit[COMBAT_AI_CASTLE_BOUNDARY_COUNT];
    i32 adjCell;
    hexcell* targetCell;
    u32 targetHex;

    m_limitCreature = false;
    thisArmy = m_currentArmyIndex + m_armies[IDX(m_currentArmySide)];
    plan = COMBAT_AI_ATTACK_NONE;
    sideEnemy = OppositeCombatSide(m_currentSide);

    mirrorMask[IDX(m_currentSide)] = GetMirrorImageMask(m_currentSide);
    mirrorMask[IDX(sideEnemy)] = GetMirrorImageMask(sideEnemy);
    shooters[IDX(m_currentSide)] = GetShooterMask(m_currentSide);
    shooters[IDX(sideEnemy)] = GetShooterMask(sideEnemy);
    flyerMask[IDX(m_currentSide)] = GetFlyerMask(m_currentSide);
    flyerMask[IDX(sideEnemy)] = GetFlyerMask(sideEnemy);
    walkers[IDX(m_currentSide)] = GetWalkerMask(m_currentSide);
    walkers[IDX(sideEnemy)] = GetWalkerMask(sideEnemy);
    oddMasks[IDX(m_currentSide)] = GetOutOfItMask(m_currentSide);
    oddMasks[IDX(sideEnemy)] = GetOutOfItMask(sideEnemy);
    traitorArray[IDX(m_currentSide)] = GetTraitorMask(m_currentSide);
    traitorArray[IDX(sideEnemy)] = GetTraitorMask(sideEnemy);
    shootStrengths[IDX(m_currentSide)] = GetStrength(m_currentSide, shooters[IDX(m_currentSide)]);
    shootStrengths[IDX(sideEnemy)] = GetStrength(sideEnemy, shooters[IDX(sideEnemy)]);
    totalArmyStrength = GetStrength(
        m_currentSide,
        shooters[IDX(m_currentSide)] | flyerMask[IDX(m_currentSide)]
            | walkers[IDX(m_currentSide)] | oddMasks[IDX(m_currentSide)]
            | traitorArray[IDX(m_currentSide)]
    );
    static_cast<i32>(totalArmyStrength + COMBAT_AI_STRENGTH_ROUNDING)
                         / COMBAT_AI_STRENGTH_FRACTION;
    shootStrong = false;
    stronger = false;
    myShootPower = GetStrength(m_currentSide, shooters[IDX(m_currentSide)]);
    enemyShooters = GetStrength(sideEnemy, shooters[IDX(sideEnemy)]);

    if (m_inCastleCombat != 0) {
        if (m_heroes[IDX(COMBAT_ATTACKER_SIDE)]->m_secondarySkills[IDX(HERO_SKILL_ARCHERY)]
                == HERO_SKILL_LEVEL_NONE
            && !m_heroes[IDX(COMBAT_ATTACKER_SIDE)]->HasArtifact(ARTIFACT_GOLDEN_BOW)) {
            if (m_currentSide == COMBAT_ATTACKER_SIDE)
                myShootPower =
                    static_cast<i32>(myShootPower) / COMBAT_SIDE_COUNT;
            else
                enemyShooters =
                    static_cast<i32>(enemyShooters) / COMBAT_SIDE_COUNT;
        }
        if (m_wallStates[IDX(COMBAT_WALL_SLOT_KEEP)] == COMBAT_WALL_STATE_KEEP_STANDING) {
            pCastle = m_combatTowns[IDX(COMBAT_DEFENDER_SIDE)];
            m_combatTowns[IDX(COMBAT_DEFENDER_SIDE)]->CalcNumLevelArchers(&archers, &grade);
            plusArchers = 0;
            if (m_wallStates[IDX(COMBAT_WALL_SLOT_TOP_TOWER)]
                == COMBAT_WALL_STATE_TOWER_STANDING)
                plusArchers += archers / COMBAT_SIDE_COUNT;
            if (m_wallStates[IDX(COMBAT_WALL_SLOT_BOTTOM_TOWER)]
                == COMBAT_WALL_STATE_TOWER_STANDING)
                plusArchers += archers / COMBAT_SIDE_COUNT;
            archers += plusArchers;
            keepStrength = static_cast<i32>(
                archers * COMBAT_AI_TOWER_STRENGTH
                * (grade * COMBAT_AI_TOWER_LEVEL_SCALE + COMBAT_AI_TOWER_BASE_SCALE)
            );
                m_heroes[IDX(COMBAT_ATTACKER_SIDE)] != NULL
                        && (m_heroes[IDX(COMBAT_ATTACKER_SIDE)]->HasArtifact(ARTIFACT_GOLDEN_BOW)
                            || m_heroes[IDX(COMBAT_ATTACKER_SIDE)]
                                       ->m_secondarySkills[IDX(HERO_SKILL_ARCHERY)]
                                   != HERO_SKILL_LEVEL_NONE)
                    ? 0
                    : 1;
            if (m_currentSide == COMBAT_DEFENDER_SIDE)
                myShootPower += keepStrength;
            else
                enemyShooters += keepStrength;
        }
    }

    if (m_heroes[IDX(m_currentSide)] != NULL) {
        myShootPower = static_cast<i32>(
            static_cast<i32>(myShootPower)
            * gfSSArcheryMod
                [IDX(m_heroes[IDX(m_currentSide)]->m_secondarySkills[IDX(HERO_SKILL_ARCHERY)])]
        );
    }
    if (m_heroes[IDX(OppositeCombatSide(m_currentSide))] != NULL) {
        enemyShooters = static_cast<i32>(
            static_cast<i32>(enemyShooters)
            * gfSSArcheryMod
                [IDX(m_heroes[IDX(OppositeCombatSide(m_currentSide))]
                         ->m_secondarySkills[IDX(HERO_SKILL_ARCHERY)])]
        );
    }
    if (static_cast<i32>(totalArmyStrength + COMBAT_AI_STRENGTH_ROUNDING)
            / COMBAT_AI_STRENGTH_FRACTION
        < static_cast<i32>(myShootPower))
        shootStrong = true;
    if (static_cast<i32>(enemyShooters) > static_cast<i32>(myShootPower))
        stronger = true;

    if (HAS(thisArmy->m_monster.flags.abilityFlags, MONSTER_ABILITY_FLAG_SHOOTER) != 0) {
        if (thisArmy->m_monster.shots > 0)
            plan = COMBAT_AI_ATTACK_SHOOT;
        else
            plan = COMBAT_AI_ATTACK_WALK;
    } else if (HAS(thisArmy->m_monster.flags.abilityFlags, MONSTER_ABILITY_FLAG_FLYING) != 0) {
        plan = COMBAT_AI_ATTACK_FLY;
    } else {
        plan = COMBAT_AI_ATTACK_WALK;
    }

    switch (plan) {
        case COMBAT_AI_ATTACK_SHOOT:
            if (AttemptAdjacentAttack(thisArmy))
                goto finish;
            if (thisArmy->m_monsterType == CREATURE_LICH
                || thisArmy->m_monsterType == CREATURE_POWER_LICH) {
                DoLichShot(thisArmy);
                goto finish;
            }
            best = GetBestArmy(sideEnemy, mirrorMask[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            best = GetBestArmy(sideEnemy, shooters[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            best = GetBestArmy(sideEnemy, flyerMask[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            if (walkers[IDX(sideEnemy)] != 0) {
                best =
                    GetClosestArmy(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]);
                if (best != -1) {
                    giNextAction = ACTION_MOVE;
                    giNextActionGridIndex =
                        (m_armies[IDX(sideEnemy)] + best)
                            ->m_hex;
                    goto finish;
                }
            }
            best = GetBestArmy(sideEnemy, oddMasks[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            best = GetBestArmy(sideEnemy, traitorArray[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            break;
        case COMBAT_AI_ATTACK_FLY:
            if (shootStrong != 0 && stronger == 0) {
                if (AttemptAttack(thisArmy, sideEnemy, mirrorMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                    goto finish;
            } else {
                if (AttemptAttack(thisArmy, sideEnemy, mirrorMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                    goto finish;
            }
            break;
        case COMBAT_AI_ATTACK_WALK:
            if (COMBAT_AI_ATTACK_NONE)
                goto finish;
            if (shootStrong != 0 && stronger == 0) {
                if (AttemptAttack(thisArmy, sideEnemy, mirrorMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                    goto finish;
                if (WalkTowardArmyFront(thisArmy, m_currentSide, shooters[IDX(m_currentSide)]))
                    goto finish;
                giNextAction = ACTION_WAIT;
                goto finish;
            }
            if (AttemptAttack(thisArmy, sideEnemy, mirrorMask[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                goto finish;

            rowLimit[0] = COMBAT_AI_CASTLE_BOUNDARY_ROW_0;
            rowLimit[1] = COMBAT_AI_CASTLE_BOUNDARY_ROW_1;
            rowLimit[2] = COMBAT_AI_CASTLE_BOUNDARY_ROW_2;
            rowLimit[3] = COMBAT_AI_CASTLE_BOUNDARY_ROW_3;
            rowLimit[4] = COMBAT_AI_CASTLE_BOUNDARY_ROW_4;
            rowLimit[5] = COMBAT_AI_CASTLE_BOUNDARY_ROW_5;
            rowLimit[6] = COMBAT_AI_CASTLE_BOUNDARY_ROW_6;
            rowLimit[7] = COMBAT_AI_CASTLE_BOUNDARY_ROW_7;
            rowLimit[8] = COMBAT_AI_CASTLE_BOUNDARY_ROW_8;
            rowIndex = thisArmy->m_hex / ARMY_HEX_COLUMNS;
            if (m_currentSide == COMBAT_ATTACKER_SIDE && m_inCastleCombat != 0
                && thisArmy->m_hex < rowLimit[rowIndex]) {
                targetHex = rowLimit[rowIndex];
                targetCell = &gpCombatManager->m_hexCells[targetHex];
                if (ValidHex(targetHex) && targetCell->m_occupantSide == COMBAT_SIDE_NONE
                    && targetCell->m_blocked == 0) {
                    giNextAction = ACTION_MOVE;
                    giNextActionGridIndex = targetHex;
                    goto finish;
                }
            }
            break;
    }
    giNextAction = ACTION_WAIT;

finish:
    if (giNextAction == ACTION_MOVE && giNextActionGridIndex > 0
        && giNextActionGridIndex < COMBAT_HEX_COUNT
        && gpCombatManager->m_hexCells[giNextActionGridIndex].m_occupantSide == COMBAT_SIDE_NONE) {
        for (dirIndex = COMBAT_DIRECTION_NORTHEAST;
             IDX(dirIndex) < COMBAT_AI_ADJACENT_DIRECTION_COUNT;
             dirIndex++) {
            adjCell = thisArmy->GetAdjacentCellIndex(
                giNextActionGridIndex, dirIndex
            );
            if (adjCell > 0 && adjCell < COMBAT_HEX_COUNT
                && gpCombatManager->m_hexCells[adjCell].m_occupantSide
                       == OppositeCombatSide(m_currentSide)) {
                giNextActionGridIndex = adjCell;
                return;
            }
        }
    }
}


AFTER strengths0_fifth1_castle1_archery0
void combatManager::DoCompAI(H2_ENUM_PARAM(CombatSide, i32)) {
    u32 traitorArray[COMBAT_SIDE_COUNT];
    i32 best;
    CombatSide sideEnemy;
    u32 flyerMask[COMBAT_SIDE_COUNT];
    u32 walkers[COMBAT_SIDE_COUNT];
    b32 stronger;
    b32 shootStrong;
    u32l myShootPower;
    i32 plan;
    u32l shootStrengths[COMBAT_SIDE_COUNT];
    u32l enemyShooters;
    army* thisArmy;
    u32 oddMasks[COMBAT_SIDE_COUNT];
    CombatHexDirection dirIndex;
    u32l totalArmyStrength;
    combatManager* H2_UNUSED(combat);
    u32 shooters[COMBAT_SIDE_COUNT];
    i32 archers;
    u32 mirrorMask[COMBAT_SIDE_COUNT];
    i32 rowIndex;
    b32 H2_UNUSED(archeryBonus);
    i32 plusArchers;
    i32 keepStrength;
    i32 grade;
    u8 rowLimit[COMBAT_AI_CASTLE_BOUNDARY_COUNT];
    i32 adjCell;
    hexcell* targetCell;
    u32 targetHex;

    m_limitCreature = false;
    thisArmy = m_currentArmyIndex + m_armies[IDX(m_currentArmySide)];
    plan = COMBAT_AI_ATTACK_NONE;
    sideEnemy = OppositeCombatSide(m_currentSide);

    mirrorMask[IDX(m_currentSide)] = GetMirrorImageMask(m_currentSide);
    mirrorMask[IDX(sideEnemy)] = GetMirrorImageMask(sideEnemy);
    shooters[IDX(m_currentSide)] = GetShooterMask(m_currentSide);
    shooters[IDX(sideEnemy)] = GetShooterMask(sideEnemy);
    flyerMask[IDX(m_currentSide)] = GetFlyerMask(m_currentSide);
    flyerMask[IDX(sideEnemy)] = GetFlyerMask(sideEnemy);
    walkers[IDX(m_currentSide)] = GetWalkerMask(m_currentSide);
    walkers[IDX(sideEnemy)] = GetWalkerMask(sideEnemy);
    oddMasks[IDX(m_currentSide)] = GetOutOfItMask(m_currentSide);
    oddMasks[IDX(sideEnemy)] = GetOutOfItMask(sideEnemy);
    traitorArray[IDX(m_currentSide)] = GetTraitorMask(m_currentSide);
    traitorArray[IDX(sideEnemy)] = GetTraitorMask(sideEnemy);
    shootStrengths[IDX(m_currentSide)] = GetStrength(m_currentSide, shooters[IDX(m_currentSide)]);
    shootStrengths[IDX(sideEnemy)] = GetStrength(sideEnemy, shooters[IDX(sideEnemy)]);
    totalArmyStrength = GetStrength(
        m_currentSide,
        shooters[IDX(m_currentSide)] | flyerMask[IDX(m_currentSide)]
            | walkers[IDX(m_currentSide)] | oddMasks[IDX(m_currentSide)]
            | traitorArray[IDX(m_currentSide)]
    );
    static_cast<i32>(totalArmyStrength + COMBAT_AI_STRENGTH_ROUNDING)
                         / COMBAT_AI_STRENGTH_FRACTION;
    shootStrong = false;
    stronger = false;
    myShootPower = GetStrength(m_currentSide, shooters[IDX(m_currentSide)]);
    enemyShooters = GetStrength(sideEnemy, shooters[IDX(sideEnemy)]);

    if (m_inCastleCombat != 0) {
        if (m_heroes[IDX(COMBAT_ATTACKER_SIDE)]->m_secondarySkills[IDX(HERO_SKILL_ARCHERY)]
                == HERO_SKILL_LEVEL_NONE
            && !m_heroes[IDX(COMBAT_ATTACKER_SIDE)]->HasArtifact(ARTIFACT_GOLDEN_BOW)) {
            if (m_currentSide == COMBAT_ATTACKER_SIDE)
                myShootPower =
                    static_cast<i32>(myShootPower) / COMBAT_SIDE_COUNT;
            else
                enemyShooters =
                    static_cast<i32>(enemyShooters) / COMBAT_SIDE_COUNT;
        }
        if (m_wallStates[IDX(COMBAT_WALL_SLOT_KEEP)] == COMBAT_WALL_STATE_KEEP_STANDING) {
            m_combatTowns[IDX(COMBAT_DEFENDER_SIDE)]->CalcNumLevelArchers(&archers, &grade);
            plusArchers = 0;
            if (m_wallStates[IDX(COMBAT_WALL_SLOT_TOP_TOWER)]
                == COMBAT_WALL_STATE_TOWER_STANDING)
                plusArchers += archers / COMBAT_SIDE_COUNT;
            if (m_wallStates[IDX(COMBAT_WALL_SLOT_BOTTOM_TOWER)]
                == COMBAT_WALL_STATE_TOWER_STANDING)
                plusArchers += archers / COMBAT_SIDE_COUNT;
            archers += plusArchers;
            keepStrength = static_cast<i32>(
                archers * COMBAT_AI_TOWER_STRENGTH
                * (grade * COMBAT_AI_TOWER_LEVEL_SCALE + COMBAT_AI_TOWER_BASE_SCALE)
            );
            archeryBonus =
                m_heroes[IDX(COMBAT_ATTACKER_SIDE)] != NULL
                        && (m_heroes[IDX(COMBAT_ATTACKER_SIDE)]->HasArtifact(ARTIFACT_GOLDEN_BOW)
                            || m_heroes[IDX(COMBAT_ATTACKER_SIDE)]
                                       ->m_secondarySkills[IDX(HERO_SKILL_ARCHERY)]
                                   != HERO_SKILL_LEVEL_NONE)
                    ? 0
                    : 1;
            if (m_currentSide == COMBAT_DEFENDER_SIDE)
                myShootPower += keepStrength;
            else
                enemyShooters += keepStrength;
        }
    }

    if (m_heroes[IDX(m_currentSide)] != NULL) {
        myShootPower = static_cast<i32>(
            static_cast<i32>(myShootPower)
            * gfSSArcheryMod
                [IDX(m_heroes[IDX(m_currentSide)]->m_secondarySkills[IDX(HERO_SKILL_ARCHERY)])]
        );
    }
    if (m_heroes[IDX(OppositeCombatSide(m_currentSide))] != NULL) {
        enemyShooters = static_cast<i32>(
            static_cast<i32>(enemyShooters)
            * gfSSArcheryMod
                [IDX(m_heroes[IDX(OppositeCombatSide(m_currentSide))]
                         ->m_secondarySkills[IDX(HERO_SKILL_ARCHERY)])]
        );
    }
    if (static_cast<i32>(totalArmyStrength + COMBAT_AI_STRENGTH_ROUNDING)
            / COMBAT_AI_STRENGTH_FRACTION
        < static_cast<i32>(myShootPower))
        shootStrong = true;
    if (static_cast<i32>(enemyShooters) > static_cast<i32>(myShootPower))
        stronger = true;

    if (HAS(thisArmy->m_monster.flags.abilityFlags, MONSTER_ABILITY_FLAG_SHOOTER) != 0) {
        if (thisArmy->m_monster.shots > 0)
            plan = COMBAT_AI_ATTACK_SHOOT;
        else
            plan = COMBAT_AI_ATTACK_WALK;
    } else if (HAS(thisArmy->m_monster.flags.abilityFlags, MONSTER_ABILITY_FLAG_FLYING) != 0) {
        plan = COMBAT_AI_ATTACK_FLY;
    } else {
        plan = COMBAT_AI_ATTACK_WALK;
    }

    switch (plan) {
        case COMBAT_AI_ATTACK_SHOOT:
            if (AttemptAdjacentAttack(thisArmy))
                goto finish;
            if (thisArmy->m_monsterType == CREATURE_LICH
                || thisArmy->m_monsterType == CREATURE_POWER_LICH) {
                DoLichShot(thisArmy);
                goto finish;
            }
            best = GetBestArmy(sideEnemy, mirrorMask[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            best = GetBestArmy(sideEnemy, shooters[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            best = GetBestArmy(sideEnemy, flyerMask[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            if (walkers[IDX(sideEnemy)] != 0) {
                best =
                    GetClosestArmy(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]);
                if (best != -1) {
                    giNextAction = ACTION_MOVE;
                    giNextActionGridIndex =
                        (m_armies[IDX(sideEnemy)] + best)
                            ->m_hex;
                    goto finish;
                }
            }
            best = GetBestArmy(sideEnemy, oddMasks[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            best = GetBestArmy(sideEnemy, traitorArray[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            break;
        case COMBAT_AI_ATTACK_FLY:
            if (shootStrong != 0 && stronger == 0) {
                if (AttemptAttack(thisArmy, sideEnemy, mirrorMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                    goto finish;
            } else {
                if (AttemptAttack(thisArmy, sideEnemy, mirrorMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                    goto finish;
            }
            break;
        case COMBAT_AI_ATTACK_WALK:
            if (COMBAT_AI_ATTACK_NONE)
                goto finish;
            if (shootStrong != 0 && stronger == 0) {
                if (AttemptAttack(thisArmy, sideEnemy, mirrorMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                    goto finish;
                if (WalkTowardArmyFront(thisArmy, m_currentSide, shooters[IDX(m_currentSide)]))
                    goto finish;
                giNextAction = ACTION_WAIT;
                goto finish;
            }
            if (AttemptAttack(thisArmy, sideEnemy, mirrorMask[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                goto finish;

            rowLimit[0] = COMBAT_AI_CASTLE_BOUNDARY_ROW_0;
            rowLimit[1] = COMBAT_AI_CASTLE_BOUNDARY_ROW_1;
            rowLimit[2] = COMBAT_AI_CASTLE_BOUNDARY_ROW_2;
            rowLimit[3] = COMBAT_AI_CASTLE_BOUNDARY_ROW_3;
            rowLimit[4] = COMBAT_AI_CASTLE_BOUNDARY_ROW_4;
            rowLimit[5] = COMBAT_AI_CASTLE_BOUNDARY_ROW_5;
            rowLimit[6] = COMBAT_AI_CASTLE_BOUNDARY_ROW_6;
            rowLimit[7] = COMBAT_AI_CASTLE_BOUNDARY_ROW_7;
            rowLimit[8] = COMBAT_AI_CASTLE_BOUNDARY_ROW_8;
            rowIndex = thisArmy->m_hex / ARMY_HEX_COLUMNS;
            if (m_currentSide == COMBAT_ATTACKER_SIDE && m_inCastleCombat != 0
                && thisArmy->m_hex < rowLimit[rowIndex]) {
                targetHex = rowLimit[rowIndex];
                targetCell = &gpCombatManager->m_hexCells[targetHex];
                if (ValidHex(targetHex) && targetCell->m_occupantSide == COMBAT_SIDE_NONE
                    && targetCell->m_blocked == 0) {
                    giNextAction = ACTION_MOVE;
                    giNextActionGridIndex = targetHex;
                    goto finish;
                }
            }
            break;
    }
    giNextAction = ACTION_WAIT;

finish:
    if (giNextAction == ACTION_MOVE && giNextActionGridIndex > 0
        && giNextActionGridIndex < COMBAT_HEX_COUNT
        && gpCombatManager->m_hexCells[giNextActionGridIndex].m_occupantSide == COMBAT_SIDE_NONE) {
        for (dirIndex = COMBAT_DIRECTION_NORTHEAST;
             IDX(dirIndex) < COMBAT_AI_ADJACENT_DIRECTION_COUNT;
             dirIndex++) {
            adjCell = thisArmy->GetAdjacentCellIndex(
                giNextActionGridIndex, dirIndex
            );
            if (adjCell > 0 && adjCell < COMBAT_HEX_COUNT
                && gpCombatManager->m_hexCells[adjCell].m_occupantSide
                       == OppositeCombatSide(m_currentSide)) {
                giNextActionGridIndex = adjCell;
                return;
            }
        }
    }
}


AFTER strengths0_fifth1_castle1_archery1
void combatManager::DoCompAI(H2_ENUM_PARAM(CombatSide, i32)) {
    u32 traitorArray[COMBAT_SIDE_COUNT];
    i32 best;
    CombatSide sideEnemy;
    u32 flyerMask[COMBAT_SIDE_COUNT];
    u32 walkers[COMBAT_SIDE_COUNT];
    b32 stronger;
    b32 shootStrong;
    u32l myShootPower;
    i32 plan;
    u32l shootStrengths[COMBAT_SIDE_COUNT];
    u32l enemyShooters;
    army* thisArmy;
    u32 oddMasks[COMBAT_SIDE_COUNT];
    CombatHexDirection dirIndex;
    u32l totalArmyStrength;
    combatManager* H2_UNUSED(combat);
    u32 shooters[COMBAT_SIDE_COUNT];
    i32 archers;
    u32 mirrorMask[COMBAT_SIDE_COUNT];
    i32 rowIndex;
    i32 plusArchers;
    i32 keepStrength;
    i32 grade;
    u8 rowLimit[COMBAT_AI_CASTLE_BOUNDARY_COUNT];
    i32 adjCell;
    hexcell* targetCell;
    u32 targetHex;

    m_limitCreature = false;
    thisArmy = m_currentArmyIndex + m_armies[IDX(m_currentArmySide)];
    plan = COMBAT_AI_ATTACK_NONE;
    sideEnemy = OppositeCombatSide(m_currentSide);

    mirrorMask[IDX(m_currentSide)] = GetMirrorImageMask(m_currentSide);
    mirrorMask[IDX(sideEnemy)] = GetMirrorImageMask(sideEnemy);
    shooters[IDX(m_currentSide)] = GetShooterMask(m_currentSide);
    shooters[IDX(sideEnemy)] = GetShooterMask(sideEnemy);
    flyerMask[IDX(m_currentSide)] = GetFlyerMask(m_currentSide);
    flyerMask[IDX(sideEnemy)] = GetFlyerMask(sideEnemy);
    walkers[IDX(m_currentSide)] = GetWalkerMask(m_currentSide);
    walkers[IDX(sideEnemy)] = GetWalkerMask(sideEnemy);
    oddMasks[IDX(m_currentSide)] = GetOutOfItMask(m_currentSide);
    oddMasks[IDX(sideEnemy)] = GetOutOfItMask(sideEnemy);
    traitorArray[IDX(m_currentSide)] = GetTraitorMask(m_currentSide);
    traitorArray[IDX(sideEnemy)] = GetTraitorMask(sideEnemy);
    shootStrengths[IDX(m_currentSide)] = GetStrength(m_currentSide, shooters[IDX(m_currentSide)]);
    shootStrengths[IDX(sideEnemy)] = GetStrength(sideEnemy, shooters[IDX(sideEnemy)]);
    totalArmyStrength = GetStrength(
        m_currentSide,
        shooters[IDX(m_currentSide)] | flyerMask[IDX(m_currentSide)]
            | walkers[IDX(m_currentSide)] | oddMasks[IDX(m_currentSide)]
            | traitorArray[IDX(m_currentSide)]
    );
    static_cast<i32>(totalArmyStrength + COMBAT_AI_STRENGTH_ROUNDING)
                         / COMBAT_AI_STRENGTH_FRACTION;
    shootStrong = false;
    stronger = false;
    myShootPower = GetStrength(m_currentSide, shooters[IDX(m_currentSide)]);
    enemyShooters = GetStrength(sideEnemy, shooters[IDX(sideEnemy)]);

    if (m_inCastleCombat != 0) {
        if (m_heroes[IDX(COMBAT_ATTACKER_SIDE)]->m_secondarySkills[IDX(HERO_SKILL_ARCHERY)]
                == HERO_SKILL_LEVEL_NONE
            && !m_heroes[IDX(COMBAT_ATTACKER_SIDE)]->HasArtifact(ARTIFACT_GOLDEN_BOW)) {
            if (m_currentSide == COMBAT_ATTACKER_SIDE)
                myShootPower =
                    static_cast<i32>(myShootPower) / COMBAT_SIDE_COUNT;
            else
                enemyShooters =
                    static_cast<i32>(enemyShooters) / COMBAT_SIDE_COUNT;
        }
        if (m_wallStates[IDX(COMBAT_WALL_SLOT_KEEP)] == COMBAT_WALL_STATE_KEEP_STANDING) {
            m_combatTowns[IDX(COMBAT_DEFENDER_SIDE)]->CalcNumLevelArchers(&archers, &grade);
            plusArchers = 0;
            if (m_wallStates[IDX(COMBAT_WALL_SLOT_TOP_TOWER)]
                == COMBAT_WALL_STATE_TOWER_STANDING)
                plusArchers += archers / COMBAT_SIDE_COUNT;
            if (m_wallStates[IDX(COMBAT_WALL_SLOT_BOTTOM_TOWER)]
                == COMBAT_WALL_STATE_TOWER_STANDING)
                plusArchers += archers / COMBAT_SIDE_COUNT;
            archers += plusArchers;
            keepStrength = static_cast<i32>(
                archers * COMBAT_AI_TOWER_STRENGTH
                * (grade * COMBAT_AI_TOWER_LEVEL_SCALE + COMBAT_AI_TOWER_BASE_SCALE)
            );
                m_heroes[IDX(COMBAT_ATTACKER_SIDE)] != NULL
                        && (m_heroes[IDX(COMBAT_ATTACKER_SIDE)]->HasArtifact(ARTIFACT_GOLDEN_BOW)
                            || m_heroes[IDX(COMBAT_ATTACKER_SIDE)]
                                       ->m_secondarySkills[IDX(HERO_SKILL_ARCHERY)]
                                   != HERO_SKILL_LEVEL_NONE)
                    ? 0
                    : 1;
            if (m_currentSide == COMBAT_DEFENDER_SIDE)
                myShootPower += keepStrength;
            else
                enemyShooters += keepStrength;
        }
    }

    if (m_heroes[IDX(m_currentSide)] != NULL) {
        myShootPower = static_cast<i32>(
            static_cast<i32>(myShootPower)
            * gfSSArcheryMod
                [IDX(m_heroes[IDX(m_currentSide)]->m_secondarySkills[IDX(HERO_SKILL_ARCHERY)])]
        );
    }
    if (m_heroes[IDX(OppositeCombatSide(m_currentSide))] != NULL) {
        enemyShooters = static_cast<i32>(
            static_cast<i32>(enemyShooters)
            * gfSSArcheryMod
                [IDX(m_heroes[IDX(OppositeCombatSide(m_currentSide))]
                         ->m_secondarySkills[IDX(HERO_SKILL_ARCHERY)])]
        );
    }
    if (static_cast<i32>(totalArmyStrength + COMBAT_AI_STRENGTH_ROUNDING)
            / COMBAT_AI_STRENGTH_FRACTION
        < static_cast<i32>(myShootPower))
        shootStrong = true;
    if (static_cast<i32>(enemyShooters) > static_cast<i32>(myShootPower))
        stronger = true;

    if (HAS(thisArmy->m_monster.flags.abilityFlags, MONSTER_ABILITY_FLAG_SHOOTER) != 0) {
        if (thisArmy->m_monster.shots > 0)
            plan = COMBAT_AI_ATTACK_SHOOT;
        else
            plan = COMBAT_AI_ATTACK_WALK;
    } else if (HAS(thisArmy->m_monster.flags.abilityFlags, MONSTER_ABILITY_FLAG_FLYING) != 0) {
        plan = COMBAT_AI_ATTACK_FLY;
    } else {
        plan = COMBAT_AI_ATTACK_WALK;
    }

    switch (plan) {
        case COMBAT_AI_ATTACK_SHOOT:
            if (AttemptAdjacentAttack(thisArmy))
                goto finish;
            if (thisArmy->m_monsterType == CREATURE_LICH
                || thisArmy->m_monsterType == CREATURE_POWER_LICH) {
                DoLichShot(thisArmy);
                goto finish;
            }
            best = GetBestArmy(sideEnemy, mirrorMask[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            best = GetBestArmy(sideEnemy, shooters[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            best = GetBestArmy(sideEnemy, flyerMask[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            if (walkers[IDX(sideEnemy)] != 0) {
                best =
                    GetClosestArmy(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]);
                if (best != -1) {
                    giNextAction = ACTION_MOVE;
                    giNextActionGridIndex =
                        (m_armies[IDX(sideEnemy)] + best)
                            ->m_hex;
                    goto finish;
                }
            }
            best = GetBestArmy(sideEnemy, oddMasks[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            best = GetBestArmy(sideEnemy, traitorArray[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            break;
        case COMBAT_AI_ATTACK_FLY:
            if (shootStrong != 0 && stronger == 0) {
                if (AttemptAttack(thisArmy, sideEnemy, mirrorMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                    goto finish;
            } else {
                if (AttemptAttack(thisArmy, sideEnemy, mirrorMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                    goto finish;
            }
            break;
        case COMBAT_AI_ATTACK_WALK:
            if (COMBAT_AI_ATTACK_NONE)
                goto finish;
            if (shootStrong != 0 && stronger == 0) {
                if (AttemptAttack(thisArmy, sideEnemy, mirrorMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                    goto finish;
                if (WalkTowardArmyFront(thisArmy, m_currentSide, shooters[IDX(m_currentSide)]))
                    goto finish;
                giNextAction = ACTION_WAIT;
                goto finish;
            }
            if (AttemptAttack(thisArmy, sideEnemy, mirrorMask[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                goto finish;

            rowLimit[0] = COMBAT_AI_CASTLE_BOUNDARY_ROW_0;
            rowLimit[1] = COMBAT_AI_CASTLE_BOUNDARY_ROW_1;
            rowLimit[2] = COMBAT_AI_CASTLE_BOUNDARY_ROW_2;
            rowLimit[3] = COMBAT_AI_CASTLE_BOUNDARY_ROW_3;
            rowLimit[4] = COMBAT_AI_CASTLE_BOUNDARY_ROW_4;
            rowLimit[5] = COMBAT_AI_CASTLE_BOUNDARY_ROW_5;
            rowLimit[6] = COMBAT_AI_CASTLE_BOUNDARY_ROW_6;
            rowLimit[7] = COMBAT_AI_CASTLE_BOUNDARY_ROW_7;
            rowLimit[8] = COMBAT_AI_CASTLE_BOUNDARY_ROW_8;
            rowIndex = thisArmy->m_hex / ARMY_HEX_COLUMNS;
            if (m_currentSide == COMBAT_ATTACKER_SIDE && m_inCastleCombat != 0
                && thisArmy->m_hex < rowLimit[rowIndex]) {
                targetHex = rowLimit[rowIndex];
                targetCell = &gpCombatManager->m_hexCells[targetHex];
                if (ValidHex(targetHex) && targetCell->m_occupantSide == COMBAT_SIDE_NONE
                    && targetCell->m_blocked == 0) {
                    giNextAction = ACTION_MOVE;
                    giNextActionGridIndex = targetHex;
                    goto finish;
                }
            }
            break;
    }
    giNextAction = ACTION_WAIT;

finish:
    if (giNextAction == ACTION_MOVE && giNextActionGridIndex > 0
        && giNextActionGridIndex < COMBAT_HEX_COUNT
        && gpCombatManager->m_hexCells[giNextActionGridIndex].m_occupantSide == COMBAT_SIDE_NONE) {
        for (dirIndex = COMBAT_DIRECTION_NORTHEAST;
             IDX(dirIndex) < COMBAT_AI_ADJACENT_DIRECTION_COUNT;
             dirIndex++) {
            adjCell = thisArmy->GetAdjacentCellIndex(
                giNextActionGridIndex, dirIndex
            );
            if (adjCell > 0 && adjCell < COMBAT_HEX_COUNT
                && gpCombatManager->m_hexCells[adjCell].m_occupantSide
                       == OppositeCombatSide(m_currentSide)) {
                giNextActionGridIndex = adjCell;
                return;
            }
        }
    }
}


AFTER strengths1_fifth0_castle0_archery0
void combatManager::DoCompAI(H2_ENUM_PARAM(CombatSide, i32)) {
    u32 traitorArray[COMBAT_SIDE_COUNT];
    i32 best;
    CombatSide sideEnemy;
    u32 flyerMask[COMBAT_SIDE_COUNT];
    u32 walkers[COMBAT_SIDE_COUNT];
    b32 stronger;
    b32 shootStrong;
    u32l myShootPower;
    i32 plan;
    u32l enemyShooters;
    army* thisArmy;
    u32 oddMasks[COMBAT_SIDE_COUNT];
    CombatHexDirection dirIndex;
    u32l totalArmyStrength;
    combatManager* H2_UNUSED(combat);
    i32 H2_UNUSED(fifth);
    u32 shooters[COMBAT_SIDE_COUNT];
    i32 archers;
    u32 mirrorMask[COMBAT_SIDE_COUNT];
    i32 rowIndex;
    b32 H2_UNUSED(archeryBonus);
    i32 plusArchers;
    i32 keepStrength;
    i32 grade;
    town* H2_UNUSED(pCastle);
    u8 rowLimit[COMBAT_AI_CASTLE_BOUNDARY_COUNT];
    i32 adjCell;
    hexcell* targetCell;
    u32 targetHex;

    m_limitCreature = false;
    thisArmy = m_currentArmyIndex + m_armies[IDX(m_currentArmySide)];
    plan = COMBAT_AI_ATTACK_NONE;
    sideEnemy = OppositeCombatSide(m_currentSide);

    mirrorMask[IDX(m_currentSide)] = GetMirrorImageMask(m_currentSide);
    mirrorMask[IDX(sideEnemy)] = GetMirrorImageMask(sideEnemy);
    shooters[IDX(m_currentSide)] = GetShooterMask(m_currentSide);
    shooters[IDX(sideEnemy)] = GetShooterMask(sideEnemy);
    flyerMask[IDX(m_currentSide)] = GetFlyerMask(m_currentSide);
    flyerMask[IDX(sideEnemy)] = GetFlyerMask(sideEnemy);
    walkers[IDX(m_currentSide)] = GetWalkerMask(m_currentSide);
    walkers[IDX(sideEnemy)] = GetWalkerMask(sideEnemy);
    oddMasks[IDX(m_currentSide)] = GetOutOfItMask(m_currentSide);
    oddMasks[IDX(sideEnemy)] = GetOutOfItMask(sideEnemy);
    traitorArray[IDX(m_currentSide)] = GetTraitorMask(m_currentSide);
    traitorArray[IDX(sideEnemy)] = GetTraitorMask(sideEnemy);
    GetStrength(m_currentSide, shooters[IDX(m_currentSide)]);
    GetStrength(sideEnemy, shooters[IDX(sideEnemy)]);
    totalArmyStrength = GetStrength(
        m_currentSide,
        shooters[IDX(m_currentSide)] | flyerMask[IDX(m_currentSide)]
            | walkers[IDX(m_currentSide)] | oddMasks[IDX(m_currentSide)]
            | traitorArray[IDX(m_currentSide)]
    );
    fifth = static_cast<i32>(totalArmyStrength + COMBAT_AI_STRENGTH_ROUNDING)
                         / COMBAT_AI_STRENGTH_FRACTION;
    shootStrong = false;
    stronger = false;
    myShootPower = GetStrength(m_currentSide, shooters[IDX(m_currentSide)]);
    enemyShooters = GetStrength(sideEnemy, shooters[IDX(sideEnemy)]);

    if (m_inCastleCombat != 0) {
        if (m_heroes[IDX(COMBAT_ATTACKER_SIDE)]->m_secondarySkills[IDX(HERO_SKILL_ARCHERY)]
                == HERO_SKILL_LEVEL_NONE
            && !m_heroes[IDX(COMBAT_ATTACKER_SIDE)]->HasArtifact(ARTIFACT_GOLDEN_BOW)) {
            if (m_currentSide == COMBAT_ATTACKER_SIDE)
                myShootPower =
                    static_cast<i32>(myShootPower) / COMBAT_SIDE_COUNT;
            else
                enemyShooters =
                    static_cast<i32>(enemyShooters) / COMBAT_SIDE_COUNT;
        }
        if (m_wallStates[IDX(COMBAT_WALL_SLOT_KEEP)] == COMBAT_WALL_STATE_KEEP_STANDING) {
            pCastle = m_combatTowns[IDX(COMBAT_DEFENDER_SIDE)];
            m_combatTowns[IDX(COMBAT_DEFENDER_SIDE)]->CalcNumLevelArchers(&archers, &grade);
            plusArchers = 0;
            if (m_wallStates[IDX(COMBAT_WALL_SLOT_TOP_TOWER)]
                == COMBAT_WALL_STATE_TOWER_STANDING)
                plusArchers += archers / COMBAT_SIDE_COUNT;
            if (m_wallStates[IDX(COMBAT_WALL_SLOT_BOTTOM_TOWER)]
                == COMBAT_WALL_STATE_TOWER_STANDING)
                plusArchers += archers / COMBAT_SIDE_COUNT;
            archers += plusArchers;
            keepStrength = static_cast<i32>(
                archers * COMBAT_AI_TOWER_STRENGTH
                * (grade * COMBAT_AI_TOWER_LEVEL_SCALE + COMBAT_AI_TOWER_BASE_SCALE)
            );
            archeryBonus =
                m_heroes[IDX(COMBAT_ATTACKER_SIDE)] != NULL
                        && (m_heroes[IDX(COMBAT_ATTACKER_SIDE)]->HasArtifact(ARTIFACT_GOLDEN_BOW)
                            || m_heroes[IDX(COMBAT_ATTACKER_SIDE)]
                                       ->m_secondarySkills[IDX(HERO_SKILL_ARCHERY)]
                                   != HERO_SKILL_LEVEL_NONE)
                    ? 0
                    : 1;
            if (m_currentSide == COMBAT_DEFENDER_SIDE)
                myShootPower += keepStrength;
            else
                enemyShooters += keepStrength;
        }
    }

    if (m_heroes[IDX(m_currentSide)] != NULL) {
        myShootPower = static_cast<i32>(
            static_cast<i32>(myShootPower)
            * gfSSArcheryMod
                [IDX(m_heroes[IDX(m_currentSide)]->m_secondarySkills[IDX(HERO_SKILL_ARCHERY)])]
        );
    }
    if (m_heroes[IDX(OppositeCombatSide(m_currentSide))] != NULL) {
        enemyShooters = static_cast<i32>(
            static_cast<i32>(enemyShooters)
            * gfSSArcheryMod
                [IDX(m_heroes[IDX(OppositeCombatSide(m_currentSide))]
                         ->m_secondarySkills[IDX(HERO_SKILL_ARCHERY)])]
        );
    }
    if (static_cast<i32>(totalArmyStrength + COMBAT_AI_STRENGTH_ROUNDING)
            / COMBAT_AI_STRENGTH_FRACTION
        < static_cast<i32>(myShootPower))
        shootStrong = true;
    if (static_cast<i32>(enemyShooters) > static_cast<i32>(myShootPower))
        stronger = true;

    if (HAS(thisArmy->m_monster.flags.abilityFlags, MONSTER_ABILITY_FLAG_SHOOTER) != 0) {
        if (thisArmy->m_monster.shots > 0)
            plan = COMBAT_AI_ATTACK_SHOOT;
        else
            plan = COMBAT_AI_ATTACK_WALK;
    } else if (HAS(thisArmy->m_monster.flags.abilityFlags, MONSTER_ABILITY_FLAG_FLYING) != 0) {
        plan = COMBAT_AI_ATTACK_FLY;
    } else {
        plan = COMBAT_AI_ATTACK_WALK;
    }

    switch (plan) {
        case COMBAT_AI_ATTACK_SHOOT:
            if (AttemptAdjacentAttack(thisArmy))
                goto finish;
            if (thisArmy->m_monsterType == CREATURE_LICH
                || thisArmy->m_monsterType == CREATURE_POWER_LICH) {
                DoLichShot(thisArmy);
                goto finish;
            }
            best = GetBestArmy(sideEnemy, mirrorMask[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            best = GetBestArmy(sideEnemy, shooters[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            best = GetBestArmy(sideEnemy, flyerMask[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            if (walkers[IDX(sideEnemy)] != 0) {
                best =
                    GetClosestArmy(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]);
                if (best != -1) {
                    giNextAction = ACTION_MOVE;
                    giNextActionGridIndex =
                        (m_armies[IDX(sideEnemy)] + best)
                            ->m_hex;
                    goto finish;
                }
            }
            best = GetBestArmy(sideEnemy, oddMasks[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            best = GetBestArmy(sideEnemy, traitorArray[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            break;
        case COMBAT_AI_ATTACK_FLY:
            if (shootStrong != 0 && stronger == 0) {
                if (AttemptAttack(thisArmy, sideEnemy, mirrorMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                    goto finish;
            } else {
                if (AttemptAttack(thisArmy, sideEnemy, mirrorMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                    goto finish;
            }
            break;
        case COMBAT_AI_ATTACK_WALK:
            if (COMBAT_AI_ATTACK_NONE)
                goto finish;
            if (shootStrong != 0 && stronger == 0) {
                if (AttemptAttack(thisArmy, sideEnemy, mirrorMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                    goto finish;
                if (WalkTowardArmyFront(thisArmy, m_currentSide, shooters[IDX(m_currentSide)]))
                    goto finish;
                giNextAction = ACTION_WAIT;
                goto finish;
            }
            if (AttemptAttack(thisArmy, sideEnemy, mirrorMask[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                goto finish;

            rowLimit[0] = COMBAT_AI_CASTLE_BOUNDARY_ROW_0;
            rowLimit[1] = COMBAT_AI_CASTLE_BOUNDARY_ROW_1;
            rowLimit[2] = COMBAT_AI_CASTLE_BOUNDARY_ROW_2;
            rowLimit[3] = COMBAT_AI_CASTLE_BOUNDARY_ROW_3;
            rowLimit[4] = COMBAT_AI_CASTLE_BOUNDARY_ROW_4;
            rowLimit[5] = COMBAT_AI_CASTLE_BOUNDARY_ROW_5;
            rowLimit[6] = COMBAT_AI_CASTLE_BOUNDARY_ROW_6;
            rowLimit[7] = COMBAT_AI_CASTLE_BOUNDARY_ROW_7;
            rowLimit[8] = COMBAT_AI_CASTLE_BOUNDARY_ROW_8;
            rowIndex = thisArmy->m_hex / ARMY_HEX_COLUMNS;
            if (m_currentSide == COMBAT_ATTACKER_SIDE && m_inCastleCombat != 0
                && thisArmy->m_hex < rowLimit[rowIndex]) {
                targetHex = rowLimit[rowIndex];
                targetCell = &gpCombatManager->m_hexCells[targetHex];
                if (ValidHex(targetHex) && targetCell->m_occupantSide == COMBAT_SIDE_NONE
                    && targetCell->m_blocked == 0) {
                    giNextAction = ACTION_MOVE;
                    giNextActionGridIndex = targetHex;
                    goto finish;
                }
            }
            break;
    }
    giNextAction = ACTION_WAIT;

finish:
    if (giNextAction == ACTION_MOVE && giNextActionGridIndex > 0
        && giNextActionGridIndex < COMBAT_HEX_COUNT
        && gpCombatManager->m_hexCells[giNextActionGridIndex].m_occupantSide == COMBAT_SIDE_NONE) {
        for (dirIndex = COMBAT_DIRECTION_NORTHEAST;
             IDX(dirIndex) < COMBAT_AI_ADJACENT_DIRECTION_COUNT;
             dirIndex++) {
            adjCell = thisArmy->GetAdjacentCellIndex(
                giNextActionGridIndex, dirIndex
            );
            if (adjCell > 0 && adjCell < COMBAT_HEX_COUNT
                && gpCombatManager->m_hexCells[adjCell].m_occupantSide
                       == OppositeCombatSide(m_currentSide)) {
                giNextActionGridIndex = adjCell;
                return;
            }
        }
    }
}


AFTER strengths1_fifth0_castle0_archery1
void combatManager::DoCompAI(H2_ENUM_PARAM(CombatSide, i32)) {
    u32 traitorArray[COMBAT_SIDE_COUNT];
    i32 best;
    CombatSide sideEnemy;
    u32 flyerMask[COMBAT_SIDE_COUNT];
    u32 walkers[COMBAT_SIDE_COUNT];
    b32 stronger;
    b32 shootStrong;
    u32l myShootPower;
    i32 plan;
    u32l enemyShooters;
    army* thisArmy;
    u32 oddMasks[COMBAT_SIDE_COUNT];
    CombatHexDirection dirIndex;
    u32l totalArmyStrength;
    combatManager* H2_UNUSED(combat);
    i32 H2_UNUSED(fifth);
    u32 shooters[COMBAT_SIDE_COUNT];
    i32 archers;
    u32 mirrorMask[COMBAT_SIDE_COUNT];
    i32 rowIndex;
    i32 plusArchers;
    i32 keepStrength;
    i32 grade;
    town* H2_UNUSED(pCastle);
    u8 rowLimit[COMBAT_AI_CASTLE_BOUNDARY_COUNT];
    i32 adjCell;
    hexcell* targetCell;
    u32 targetHex;

    m_limitCreature = false;
    thisArmy = m_currentArmyIndex + m_armies[IDX(m_currentArmySide)];
    plan = COMBAT_AI_ATTACK_NONE;
    sideEnemy = OppositeCombatSide(m_currentSide);

    mirrorMask[IDX(m_currentSide)] = GetMirrorImageMask(m_currentSide);
    mirrorMask[IDX(sideEnemy)] = GetMirrorImageMask(sideEnemy);
    shooters[IDX(m_currentSide)] = GetShooterMask(m_currentSide);
    shooters[IDX(sideEnemy)] = GetShooterMask(sideEnemy);
    flyerMask[IDX(m_currentSide)] = GetFlyerMask(m_currentSide);
    flyerMask[IDX(sideEnemy)] = GetFlyerMask(sideEnemy);
    walkers[IDX(m_currentSide)] = GetWalkerMask(m_currentSide);
    walkers[IDX(sideEnemy)] = GetWalkerMask(sideEnemy);
    oddMasks[IDX(m_currentSide)] = GetOutOfItMask(m_currentSide);
    oddMasks[IDX(sideEnemy)] = GetOutOfItMask(sideEnemy);
    traitorArray[IDX(m_currentSide)] = GetTraitorMask(m_currentSide);
    traitorArray[IDX(sideEnemy)] = GetTraitorMask(sideEnemy);
    GetStrength(m_currentSide, shooters[IDX(m_currentSide)]);
    GetStrength(sideEnemy, shooters[IDX(sideEnemy)]);
    totalArmyStrength = GetStrength(
        m_currentSide,
        shooters[IDX(m_currentSide)] | flyerMask[IDX(m_currentSide)]
            | walkers[IDX(m_currentSide)] | oddMasks[IDX(m_currentSide)]
            | traitorArray[IDX(m_currentSide)]
    );
    fifth = static_cast<i32>(totalArmyStrength + COMBAT_AI_STRENGTH_ROUNDING)
                         / COMBAT_AI_STRENGTH_FRACTION;
    shootStrong = false;
    stronger = false;
    myShootPower = GetStrength(m_currentSide, shooters[IDX(m_currentSide)]);
    enemyShooters = GetStrength(sideEnemy, shooters[IDX(sideEnemy)]);

    if (m_inCastleCombat != 0) {
        if (m_heroes[IDX(COMBAT_ATTACKER_SIDE)]->m_secondarySkills[IDX(HERO_SKILL_ARCHERY)]
                == HERO_SKILL_LEVEL_NONE
            && !m_heroes[IDX(COMBAT_ATTACKER_SIDE)]->HasArtifact(ARTIFACT_GOLDEN_BOW)) {
            if (m_currentSide == COMBAT_ATTACKER_SIDE)
                myShootPower =
                    static_cast<i32>(myShootPower) / COMBAT_SIDE_COUNT;
            else
                enemyShooters =
                    static_cast<i32>(enemyShooters) / COMBAT_SIDE_COUNT;
        }
        if (m_wallStates[IDX(COMBAT_WALL_SLOT_KEEP)] == COMBAT_WALL_STATE_KEEP_STANDING) {
            pCastle = m_combatTowns[IDX(COMBAT_DEFENDER_SIDE)];
            m_combatTowns[IDX(COMBAT_DEFENDER_SIDE)]->CalcNumLevelArchers(&archers, &grade);
            plusArchers = 0;
            if (m_wallStates[IDX(COMBAT_WALL_SLOT_TOP_TOWER)]
                == COMBAT_WALL_STATE_TOWER_STANDING)
                plusArchers += archers / COMBAT_SIDE_COUNT;
            if (m_wallStates[IDX(COMBAT_WALL_SLOT_BOTTOM_TOWER)]
                == COMBAT_WALL_STATE_TOWER_STANDING)
                plusArchers += archers / COMBAT_SIDE_COUNT;
            archers += plusArchers;
            keepStrength = static_cast<i32>(
                archers * COMBAT_AI_TOWER_STRENGTH
                * (grade * COMBAT_AI_TOWER_LEVEL_SCALE + COMBAT_AI_TOWER_BASE_SCALE)
            );
                m_heroes[IDX(COMBAT_ATTACKER_SIDE)] != NULL
                        && (m_heroes[IDX(COMBAT_ATTACKER_SIDE)]->HasArtifact(ARTIFACT_GOLDEN_BOW)
                            || m_heroes[IDX(COMBAT_ATTACKER_SIDE)]
                                       ->m_secondarySkills[IDX(HERO_SKILL_ARCHERY)]
                                   != HERO_SKILL_LEVEL_NONE)
                    ? 0
                    : 1;
            if (m_currentSide == COMBAT_DEFENDER_SIDE)
                myShootPower += keepStrength;
            else
                enemyShooters += keepStrength;
        }
    }

    if (m_heroes[IDX(m_currentSide)] != NULL) {
        myShootPower = static_cast<i32>(
            static_cast<i32>(myShootPower)
            * gfSSArcheryMod
                [IDX(m_heroes[IDX(m_currentSide)]->m_secondarySkills[IDX(HERO_SKILL_ARCHERY)])]
        );
    }
    if (m_heroes[IDX(OppositeCombatSide(m_currentSide))] != NULL) {
        enemyShooters = static_cast<i32>(
            static_cast<i32>(enemyShooters)
            * gfSSArcheryMod
                [IDX(m_heroes[IDX(OppositeCombatSide(m_currentSide))]
                         ->m_secondarySkills[IDX(HERO_SKILL_ARCHERY)])]
        );
    }
    if (static_cast<i32>(totalArmyStrength + COMBAT_AI_STRENGTH_ROUNDING)
            / COMBAT_AI_STRENGTH_FRACTION
        < static_cast<i32>(myShootPower))
        shootStrong = true;
    if (static_cast<i32>(enemyShooters) > static_cast<i32>(myShootPower))
        stronger = true;

    if (HAS(thisArmy->m_monster.flags.abilityFlags, MONSTER_ABILITY_FLAG_SHOOTER) != 0) {
        if (thisArmy->m_monster.shots > 0)
            plan = COMBAT_AI_ATTACK_SHOOT;
        else
            plan = COMBAT_AI_ATTACK_WALK;
    } else if (HAS(thisArmy->m_monster.flags.abilityFlags, MONSTER_ABILITY_FLAG_FLYING) != 0) {
        plan = COMBAT_AI_ATTACK_FLY;
    } else {
        plan = COMBAT_AI_ATTACK_WALK;
    }

    switch (plan) {
        case COMBAT_AI_ATTACK_SHOOT:
            if (AttemptAdjacentAttack(thisArmy))
                goto finish;
            if (thisArmy->m_monsterType == CREATURE_LICH
                || thisArmy->m_monsterType == CREATURE_POWER_LICH) {
                DoLichShot(thisArmy);
                goto finish;
            }
            best = GetBestArmy(sideEnemy, mirrorMask[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            best = GetBestArmy(sideEnemy, shooters[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            best = GetBestArmy(sideEnemy, flyerMask[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            if (walkers[IDX(sideEnemy)] != 0) {
                best =
                    GetClosestArmy(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]);
                if (best != -1) {
                    giNextAction = ACTION_MOVE;
                    giNextActionGridIndex =
                        (m_armies[IDX(sideEnemy)] + best)
                            ->m_hex;
                    goto finish;
                }
            }
            best = GetBestArmy(sideEnemy, oddMasks[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            best = GetBestArmy(sideEnemy, traitorArray[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            break;
        case COMBAT_AI_ATTACK_FLY:
            if (shootStrong != 0 && stronger == 0) {
                if (AttemptAttack(thisArmy, sideEnemy, mirrorMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                    goto finish;
            } else {
                if (AttemptAttack(thisArmy, sideEnemy, mirrorMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                    goto finish;
            }
            break;
        case COMBAT_AI_ATTACK_WALK:
            if (COMBAT_AI_ATTACK_NONE)
                goto finish;
            if (shootStrong != 0 && stronger == 0) {
                if (AttemptAttack(thisArmy, sideEnemy, mirrorMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                    goto finish;
                if (WalkTowardArmyFront(thisArmy, m_currentSide, shooters[IDX(m_currentSide)]))
                    goto finish;
                giNextAction = ACTION_WAIT;
                goto finish;
            }
            if (AttemptAttack(thisArmy, sideEnemy, mirrorMask[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                goto finish;

            rowLimit[0] = COMBAT_AI_CASTLE_BOUNDARY_ROW_0;
            rowLimit[1] = COMBAT_AI_CASTLE_BOUNDARY_ROW_1;
            rowLimit[2] = COMBAT_AI_CASTLE_BOUNDARY_ROW_2;
            rowLimit[3] = COMBAT_AI_CASTLE_BOUNDARY_ROW_3;
            rowLimit[4] = COMBAT_AI_CASTLE_BOUNDARY_ROW_4;
            rowLimit[5] = COMBAT_AI_CASTLE_BOUNDARY_ROW_5;
            rowLimit[6] = COMBAT_AI_CASTLE_BOUNDARY_ROW_6;
            rowLimit[7] = COMBAT_AI_CASTLE_BOUNDARY_ROW_7;
            rowLimit[8] = COMBAT_AI_CASTLE_BOUNDARY_ROW_8;
            rowIndex = thisArmy->m_hex / ARMY_HEX_COLUMNS;
            if (m_currentSide == COMBAT_ATTACKER_SIDE && m_inCastleCombat != 0
                && thisArmy->m_hex < rowLimit[rowIndex]) {
                targetHex = rowLimit[rowIndex];
                targetCell = &gpCombatManager->m_hexCells[targetHex];
                if (ValidHex(targetHex) && targetCell->m_occupantSide == COMBAT_SIDE_NONE
                    && targetCell->m_blocked == 0) {
                    giNextAction = ACTION_MOVE;
                    giNextActionGridIndex = targetHex;
                    goto finish;
                }
            }
            break;
    }
    giNextAction = ACTION_WAIT;

finish:
    if (giNextAction == ACTION_MOVE && giNextActionGridIndex > 0
        && giNextActionGridIndex < COMBAT_HEX_COUNT
        && gpCombatManager->m_hexCells[giNextActionGridIndex].m_occupantSide == COMBAT_SIDE_NONE) {
        for (dirIndex = COMBAT_DIRECTION_NORTHEAST;
             IDX(dirIndex) < COMBAT_AI_ADJACENT_DIRECTION_COUNT;
             dirIndex++) {
            adjCell = thisArmy->GetAdjacentCellIndex(
                giNextActionGridIndex, dirIndex
            );
            if (adjCell > 0 && adjCell < COMBAT_HEX_COUNT
                && gpCombatManager->m_hexCells[adjCell].m_occupantSide
                       == OppositeCombatSide(m_currentSide)) {
                giNextActionGridIndex = adjCell;
                return;
            }
        }
    }
}


AFTER strengths1_fifth0_castle1_archery0
void combatManager::DoCompAI(H2_ENUM_PARAM(CombatSide, i32)) {
    u32 traitorArray[COMBAT_SIDE_COUNT];
    i32 best;
    CombatSide sideEnemy;
    u32 flyerMask[COMBAT_SIDE_COUNT];
    u32 walkers[COMBAT_SIDE_COUNT];
    b32 stronger;
    b32 shootStrong;
    u32l myShootPower;
    i32 plan;
    u32l enemyShooters;
    army* thisArmy;
    u32 oddMasks[COMBAT_SIDE_COUNT];
    CombatHexDirection dirIndex;
    u32l totalArmyStrength;
    combatManager* H2_UNUSED(combat);
    i32 H2_UNUSED(fifth);
    u32 shooters[COMBAT_SIDE_COUNT];
    i32 archers;
    u32 mirrorMask[COMBAT_SIDE_COUNT];
    i32 rowIndex;
    b32 H2_UNUSED(archeryBonus);
    i32 plusArchers;
    i32 keepStrength;
    i32 grade;
    u8 rowLimit[COMBAT_AI_CASTLE_BOUNDARY_COUNT];
    i32 adjCell;
    hexcell* targetCell;
    u32 targetHex;

    m_limitCreature = false;
    thisArmy = m_currentArmyIndex + m_armies[IDX(m_currentArmySide)];
    plan = COMBAT_AI_ATTACK_NONE;
    sideEnemy = OppositeCombatSide(m_currentSide);

    mirrorMask[IDX(m_currentSide)] = GetMirrorImageMask(m_currentSide);
    mirrorMask[IDX(sideEnemy)] = GetMirrorImageMask(sideEnemy);
    shooters[IDX(m_currentSide)] = GetShooterMask(m_currentSide);
    shooters[IDX(sideEnemy)] = GetShooterMask(sideEnemy);
    flyerMask[IDX(m_currentSide)] = GetFlyerMask(m_currentSide);
    flyerMask[IDX(sideEnemy)] = GetFlyerMask(sideEnemy);
    walkers[IDX(m_currentSide)] = GetWalkerMask(m_currentSide);
    walkers[IDX(sideEnemy)] = GetWalkerMask(sideEnemy);
    oddMasks[IDX(m_currentSide)] = GetOutOfItMask(m_currentSide);
    oddMasks[IDX(sideEnemy)] = GetOutOfItMask(sideEnemy);
    traitorArray[IDX(m_currentSide)] = GetTraitorMask(m_currentSide);
    traitorArray[IDX(sideEnemy)] = GetTraitorMask(sideEnemy);
    GetStrength(m_currentSide, shooters[IDX(m_currentSide)]);
    GetStrength(sideEnemy, shooters[IDX(sideEnemy)]);
    totalArmyStrength = GetStrength(
        m_currentSide,
        shooters[IDX(m_currentSide)] | flyerMask[IDX(m_currentSide)]
            | walkers[IDX(m_currentSide)] | oddMasks[IDX(m_currentSide)]
            | traitorArray[IDX(m_currentSide)]
    );
    fifth = static_cast<i32>(totalArmyStrength + COMBAT_AI_STRENGTH_ROUNDING)
                         / COMBAT_AI_STRENGTH_FRACTION;
    shootStrong = false;
    stronger = false;
    myShootPower = GetStrength(m_currentSide, shooters[IDX(m_currentSide)]);
    enemyShooters = GetStrength(sideEnemy, shooters[IDX(sideEnemy)]);

    if (m_inCastleCombat != 0) {
        if (m_heroes[IDX(COMBAT_ATTACKER_SIDE)]->m_secondarySkills[IDX(HERO_SKILL_ARCHERY)]
                == HERO_SKILL_LEVEL_NONE
            && !m_heroes[IDX(COMBAT_ATTACKER_SIDE)]->HasArtifact(ARTIFACT_GOLDEN_BOW)) {
            if (m_currentSide == COMBAT_ATTACKER_SIDE)
                myShootPower =
                    static_cast<i32>(myShootPower) / COMBAT_SIDE_COUNT;
            else
                enemyShooters =
                    static_cast<i32>(enemyShooters) / COMBAT_SIDE_COUNT;
        }
        if (m_wallStates[IDX(COMBAT_WALL_SLOT_KEEP)] == COMBAT_WALL_STATE_KEEP_STANDING) {
            m_combatTowns[IDX(COMBAT_DEFENDER_SIDE)]->CalcNumLevelArchers(&archers, &grade);
            plusArchers = 0;
            if (m_wallStates[IDX(COMBAT_WALL_SLOT_TOP_TOWER)]
                == COMBAT_WALL_STATE_TOWER_STANDING)
                plusArchers += archers / COMBAT_SIDE_COUNT;
            if (m_wallStates[IDX(COMBAT_WALL_SLOT_BOTTOM_TOWER)]
                == COMBAT_WALL_STATE_TOWER_STANDING)
                plusArchers += archers / COMBAT_SIDE_COUNT;
            archers += plusArchers;
            keepStrength = static_cast<i32>(
                archers * COMBAT_AI_TOWER_STRENGTH
                * (grade * COMBAT_AI_TOWER_LEVEL_SCALE + COMBAT_AI_TOWER_BASE_SCALE)
            );
            archeryBonus =
                m_heroes[IDX(COMBAT_ATTACKER_SIDE)] != NULL
                        && (m_heroes[IDX(COMBAT_ATTACKER_SIDE)]->HasArtifact(ARTIFACT_GOLDEN_BOW)
                            || m_heroes[IDX(COMBAT_ATTACKER_SIDE)]
                                       ->m_secondarySkills[IDX(HERO_SKILL_ARCHERY)]
                                   != HERO_SKILL_LEVEL_NONE)
                    ? 0
                    : 1;
            if (m_currentSide == COMBAT_DEFENDER_SIDE)
                myShootPower += keepStrength;
            else
                enemyShooters += keepStrength;
        }
    }

    if (m_heroes[IDX(m_currentSide)] != NULL) {
        myShootPower = static_cast<i32>(
            static_cast<i32>(myShootPower)
            * gfSSArcheryMod
                [IDX(m_heroes[IDX(m_currentSide)]->m_secondarySkills[IDX(HERO_SKILL_ARCHERY)])]
        );
    }
    if (m_heroes[IDX(OppositeCombatSide(m_currentSide))] != NULL) {
        enemyShooters = static_cast<i32>(
            static_cast<i32>(enemyShooters)
            * gfSSArcheryMod
                [IDX(m_heroes[IDX(OppositeCombatSide(m_currentSide))]
                         ->m_secondarySkills[IDX(HERO_SKILL_ARCHERY)])]
        );
    }
    if (static_cast<i32>(totalArmyStrength + COMBAT_AI_STRENGTH_ROUNDING)
            / COMBAT_AI_STRENGTH_FRACTION
        < static_cast<i32>(myShootPower))
        shootStrong = true;
    if (static_cast<i32>(enemyShooters) > static_cast<i32>(myShootPower))
        stronger = true;

    if (HAS(thisArmy->m_monster.flags.abilityFlags, MONSTER_ABILITY_FLAG_SHOOTER) != 0) {
        if (thisArmy->m_monster.shots > 0)
            plan = COMBAT_AI_ATTACK_SHOOT;
        else
            plan = COMBAT_AI_ATTACK_WALK;
    } else if (HAS(thisArmy->m_monster.flags.abilityFlags, MONSTER_ABILITY_FLAG_FLYING) != 0) {
        plan = COMBAT_AI_ATTACK_FLY;
    } else {
        plan = COMBAT_AI_ATTACK_WALK;
    }

    switch (plan) {
        case COMBAT_AI_ATTACK_SHOOT:
            if (AttemptAdjacentAttack(thisArmy))
                goto finish;
            if (thisArmy->m_monsterType == CREATURE_LICH
                || thisArmy->m_monsterType == CREATURE_POWER_LICH) {
                DoLichShot(thisArmy);
                goto finish;
            }
            best = GetBestArmy(sideEnemy, mirrorMask[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            best = GetBestArmy(sideEnemy, shooters[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            best = GetBestArmy(sideEnemy, flyerMask[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            if (walkers[IDX(sideEnemy)] != 0) {
                best =
                    GetClosestArmy(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]);
                if (best != -1) {
                    giNextAction = ACTION_MOVE;
                    giNextActionGridIndex =
                        (m_armies[IDX(sideEnemy)] + best)
                            ->m_hex;
                    goto finish;
                }
            }
            best = GetBestArmy(sideEnemy, oddMasks[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            best = GetBestArmy(sideEnemy, traitorArray[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            break;
        case COMBAT_AI_ATTACK_FLY:
            if (shootStrong != 0 && stronger == 0) {
                if (AttemptAttack(thisArmy, sideEnemy, mirrorMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                    goto finish;
            } else {
                if (AttemptAttack(thisArmy, sideEnemy, mirrorMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                    goto finish;
            }
            break;
        case COMBAT_AI_ATTACK_WALK:
            if (COMBAT_AI_ATTACK_NONE)
                goto finish;
            if (shootStrong != 0 && stronger == 0) {
                if (AttemptAttack(thisArmy, sideEnemy, mirrorMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                    goto finish;
                if (WalkTowardArmyFront(thisArmy, m_currentSide, shooters[IDX(m_currentSide)]))
                    goto finish;
                giNextAction = ACTION_WAIT;
                goto finish;
            }
            if (AttemptAttack(thisArmy, sideEnemy, mirrorMask[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                goto finish;

            rowLimit[0] = COMBAT_AI_CASTLE_BOUNDARY_ROW_0;
            rowLimit[1] = COMBAT_AI_CASTLE_BOUNDARY_ROW_1;
            rowLimit[2] = COMBAT_AI_CASTLE_BOUNDARY_ROW_2;
            rowLimit[3] = COMBAT_AI_CASTLE_BOUNDARY_ROW_3;
            rowLimit[4] = COMBAT_AI_CASTLE_BOUNDARY_ROW_4;
            rowLimit[5] = COMBAT_AI_CASTLE_BOUNDARY_ROW_5;
            rowLimit[6] = COMBAT_AI_CASTLE_BOUNDARY_ROW_6;
            rowLimit[7] = COMBAT_AI_CASTLE_BOUNDARY_ROW_7;
            rowLimit[8] = COMBAT_AI_CASTLE_BOUNDARY_ROW_8;
            rowIndex = thisArmy->m_hex / ARMY_HEX_COLUMNS;
            if (m_currentSide == COMBAT_ATTACKER_SIDE && m_inCastleCombat != 0
                && thisArmy->m_hex < rowLimit[rowIndex]) {
                targetHex = rowLimit[rowIndex];
                targetCell = &gpCombatManager->m_hexCells[targetHex];
                if (ValidHex(targetHex) && targetCell->m_occupantSide == COMBAT_SIDE_NONE
                    && targetCell->m_blocked == 0) {
                    giNextAction = ACTION_MOVE;
                    giNextActionGridIndex = targetHex;
                    goto finish;
                }
            }
            break;
    }
    giNextAction = ACTION_WAIT;

finish:
    if (giNextAction == ACTION_MOVE && giNextActionGridIndex > 0
        && giNextActionGridIndex < COMBAT_HEX_COUNT
        && gpCombatManager->m_hexCells[giNextActionGridIndex].m_occupantSide == COMBAT_SIDE_NONE) {
        for (dirIndex = COMBAT_DIRECTION_NORTHEAST;
             IDX(dirIndex) < COMBAT_AI_ADJACENT_DIRECTION_COUNT;
             dirIndex++) {
            adjCell = thisArmy->GetAdjacentCellIndex(
                giNextActionGridIndex, dirIndex
            );
            if (adjCell > 0 && adjCell < COMBAT_HEX_COUNT
                && gpCombatManager->m_hexCells[adjCell].m_occupantSide
                       == OppositeCombatSide(m_currentSide)) {
                giNextActionGridIndex = adjCell;
                return;
            }
        }
    }
}


AFTER strengths1_fifth0_castle1_archery1
void combatManager::DoCompAI(H2_ENUM_PARAM(CombatSide, i32)) {
    u32 traitorArray[COMBAT_SIDE_COUNT];
    i32 best;
    CombatSide sideEnemy;
    u32 flyerMask[COMBAT_SIDE_COUNT];
    u32 walkers[COMBAT_SIDE_COUNT];
    b32 stronger;
    b32 shootStrong;
    u32l myShootPower;
    i32 plan;
    u32l enemyShooters;
    army* thisArmy;
    u32 oddMasks[COMBAT_SIDE_COUNT];
    CombatHexDirection dirIndex;
    u32l totalArmyStrength;
    combatManager* H2_UNUSED(combat);
    i32 H2_UNUSED(fifth);
    u32 shooters[COMBAT_SIDE_COUNT];
    i32 archers;
    u32 mirrorMask[COMBAT_SIDE_COUNT];
    i32 rowIndex;
    i32 plusArchers;
    i32 keepStrength;
    i32 grade;
    u8 rowLimit[COMBAT_AI_CASTLE_BOUNDARY_COUNT];
    i32 adjCell;
    hexcell* targetCell;
    u32 targetHex;

    m_limitCreature = false;
    thisArmy = m_currentArmyIndex + m_armies[IDX(m_currentArmySide)];
    plan = COMBAT_AI_ATTACK_NONE;
    sideEnemy = OppositeCombatSide(m_currentSide);

    mirrorMask[IDX(m_currentSide)] = GetMirrorImageMask(m_currentSide);
    mirrorMask[IDX(sideEnemy)] = GetMirrorImageMask(sideEnemy);
    shooters[IDX(m_currentSide)] = GetShooterMask(m_currentSide);
    shooters[IDX(sideEnemy)] = GetShooterMask(sideEnemy);
    flyerMask[IDX(m_currentSide)] = GetFlyerMask(m_currentSide);
    flyerMask[IDX(sideEnemy)] = GetFlyerMask(sideEnemy);
    walkers[IDX(m_currentSide)] = GetWalkerMask(m_currentSide);
    walkers[IDX(sideEnemy)] = GetWalkerMask(sideEnemy);
    oddMasks[IDX(m_currentSide)] = GetOutOfItMask(m_currentSide);
    oddMasks[IDX(sideEnemy)] = GetOutOfItMask(sideEnemy);
    traitorArray[IDX(m_currentSide)] = GetTraitorMask(m_currentSide);
    traitorArray[IDX(sideEnemy)] = GetTraitorMask(sideEnemy);
    GetStrength(m_currentSide, shooters[IDX(m_currentSide)]);
    GetStrength(sideEnemy, shooters[IDX(sideEnemy)]);
    totalArmyStrength = GetStrength(
        m_currentSide,
        shooters[IDX(m_currentSide)] | flyerMask[IDX(m_currentSide)]
            | walkers[IDX(m_currentSide)] | oddMasks[IDX(m_currentSide)]
            | traitorArray[IDX(m_currentSide)]
    );
    fifth = static_cast<i32>(totalArmyStrength + COMBAT_AI_STRENGTH_ROUNDING)
                         / COMBAT_AI_STRENGTH_FRACTION;
    shootStrong = false;
    stronger = false;
    myShootPower = GetStrength(m_currentSide, shooters[IDX(m_currentSide)]);
    enemyShooters = GetStrength(sideEnemy, shooters[IDX(sideEnemy)]);

    if (m_inCastleCombat != 0) {
        if (m_heroes[IDX(COMBAT_ATTACKER_SIDE)]->m_secondarySkills[IDX(HERO_SKILL_ARCHERY)]
                == HERO_SKILL_LEVEL_NONE
            && !m_heroes[IDX(COMBAT_ATTACKER_SIDE)]->HasArtifact(ARTIFACT_GOLDEN_BOW)) {
            if (m_currentSide == COMBAT_ATTACKER_SIDE)
                myShootPower =
                    static_cast<i32>(myShootPower) / COMBAT_SIDE_COUNT;
            else
                enemyShooters =
                    static_cast<i32>(enemyShooters) / COMBAT_SIDE_COUNT;
        }
        if (m_wallStates[IDX(COMBAT_WALL_SLOT_KEEP)] == COMBAT_WALL_STATE_KEEP_STANDING) {
            m_combatTowns[IDX(COMBAT_DEFENDER_SIDE)]->CalcNumLevelArchers(&archers, &grade);
            plusArchers = 0;
            if (m_wallStates[IDX(COMBAT_WALL_SLOT_TOP_TOWER)]
                == COMBAT_WALL_STATE_TOWER_STANDING)
                plusArchers += archers / COMBAT_SIDE_COUNT;
            if (m_wallStates[IDX(COMBAT_WALL_SLOT_BOTTOM_TOWER)]
                == COMBAT_WALL_STATE_TOWER_STANDING)
                plusArchers += archers / COMBAT_SIDE_COUNT;
            archers += plusArchers;
            keepStrength = static_cast<i32>(
                archers * COMBAT_AI_TOWER_STRENGTH
                * (grade * COMBAT_AI_TOWER_LEVEL_SCALE + COMBAT_AI_TOWER_BASE_SCALE)
            );
                m_heroes[IDX(COMBAT_ATTACKER_SIDE)] != NULL
                        && (m_heroes[IDX(COMBAT_ATTACKER_SIDE)]->HasArtifact(ARTIFACT_GOLDEN_BOW)
                            || m_heroes[IDX(COMBAT_ATTACKER_SIDE)]
                                       ->m_secondarySkills[IDX(HERO_SKILL_ARCHERY)]
                                   != HERO_SKILL_LEVEL_NONE)
                    ? 0
                    : 1;
            if (m_currentSide == COMBAT_DEFENDER_SIDE)
                myShootPower += keepStrength;
            else
                enemyShooters += keepStrength;
        }
    }

    if (m_heroes[IDX(m_currentSide)] != NULL) {
        myShootPower = static_cast<i32>(
            static_cast<i32>(myShootPower)
            * gfSSArcheryMod
                [IDX(m_heroes[IDX(m_currentSide)]->m_secondarySkills[IDX(HERO_SKILL_ARCHERY)])]
        );
    }
    if (m_heroes[IDX(OppositeCombatSide(m_currentSide))] != NULL) {
        enemyShooters = static_cast<i32>(
            static_cast<i32>(enemyShooters)
            * gfSSArcheryMod
                [IDX(m_heroes[IDX(OppositeCombatSide(m_currentSide))]
                         ->m_secondarySkills[IDX(HERO_SKILL_ARCHERY)])]
        );
    }
    if (static_cast<i32>(totalArmyStrength + COMBAT_AI_STRENGTH_ROUNDING)
            / COMBAT_AI_STRENGTH_FRACTION
        < static_cast<i32>(myShootPower))
        shootStrong = true;
    if (static_cast<i32>(enemyShooters) > static_cast<i32>(myShootPower))
        stronger = true;

    if (HAS(thisArmy->m_monster.flags.abilityFlags, MONSTER_ABILITY_FLAG_SHOOTER) != 0) {
        if (thisArmy->m_monster.shots > 0)
            plan = COMBAT_AI_ATTACK_SHOOT;
        else
            plan = COMBAT_AI_ATTACK_WALK;
    } else if (HAS(thisArmy->m_monster.flags.abilityFlags, MONSTER_ABILITY_FLAG_FLYING) != 0) {
        plan = COMBAT_AI_ATTACK_FLY;
    } else {
        plan = COMBAT_AI_ATTACK_WALK;
    }

    switch (plan) {
        case COMBAT_AI_ATTACK_SHOOT:
            if (AttemptAdjacentAttack(thisArmy))
                goto finish;
            if (thisArmy->m_monsterType == CREATURE_LICH
                || thisArmy->m_monsterType == CREATURE_POWER_LICH) {
                DoLichShot(thisArmy);
                goto finish;
            }
            best = GetBestArmy(sideEnemy, mirrorMask[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            best = GetBestArmy(sideEnemy, shooters[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            best = GetBestArmy(sideEnemy, flyerMask[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            if (walkers[IDX(sideEnemy)] != 0) {
                best =
                    GetClosestArmy(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]);
                if (best != -1) {
                    giNextAction = ACTION_MOVE;
                    giNextActionGridIndex =
                        (m_armies[IDX(sideEnemy)] + best)
                            ->m_hex;
                    goto finish;
                }
            }
            best = GetBestArmy(sideEnemy, oddMasks[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            best = GetBestArmy(sideEnemy, traitorArray[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            break;
        case COMBAT_AI_ATTACK_FLY:
            if (shootStrong != 0 && stronger == 0) {
                if (AttemptAttack(thisArmy, sideEnemy, mirrorMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                    goto finish;
            } else {
                if (AttemptAttack(thisArmy, sideEnemy, mirrorMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                    goto finish;
            }
            break;
        case COMBAT_AI_ATTACK_WALK:
            if (COMBAT_AI_ATTACK_NONE)
                goto finish;
            if (shootStrong != 0 && stronger == 0) {
                if (AttemptAttack(thisArmy, sideEnemy, mirrorMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                    goto finish;
                if (WalkTowardArmyFront(thisArmy, m_currentSide, shooters[IDX(m_currentSide)]))
                    goto finish;
                giNextAction = ACTION_WAIT;
                goto finish;
            }
            if (AttemptAttack(thisArmy, sideEnemy, mirrorMask[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                goto finish;

            rowLimit[0] = COMBAT_AI_CASTLE_BOUNDARY_ROW_0;
            rowLimit[1] = COMBAT_AI_CASTLE_BOUNDARY_ROW_1;
            rowLimit[2] = COMBAT_AI_CASTLE_BOUNDARY_ROW_2;
            rowLimit[3] = COMBAT_AI_CASTLE_BOUNDARY_ROW_3;
            rowLimit[4] = COMBAT_AI_CASTLE_BOUNDARY_ROW_4;
            rowLimit[5] = COMBAT_AI_CASTLE_BOUNDARY_ROW_5;
            rowLimit[6] = COMBAT_AI_CASTLE_BOUNDARY_ROW_6;
            rowLimit[7] = COMBAT_AI_CASTLE_BOUNDARY_ROW_7;
            rowLimit[8] = COMBAT_AI_CASTLE_BOUNDARY_ROW_8;
            rowIndex = thisArmy->m_hex / ARMY_HEX_COLUMNS;
            if (m_currentSide == COMBAT_ATTACKER_SIDE && m_inCastleCombat != 0
                && thisArmy->m_hex < rowLimit[rowIndex]) {
                targetHex = rowLimit[rowIndex];
                targetCell = &gpCombatManager->m_hexCells[targetHex];
                if (ValidHex(targetHex) && targetCell->m_occupantSide == COMBAT_SIDE_NONE
                    && targetCell->m_blocked == 0) {
                    giNextAction = ACTION_MOVE;
                    giNextActionGridIndex = targetHex;
                    goto finish;
                }
            }
            break;
    }
    giNextAction = ACTION_WAIT;

finish:
    if (giNextAction == ACTION_MOVE && giNextActionGridIndex > 0
        && giNextActionGridIndex < COMBAT_HEX_COUNT
        && gpCombatManager->m_hexCells[giNextActionGridIndex].m_occupantSide == COMBAT_SIDE_NONE) {
        for (dirIndex = COMBAT_DIRECTION_NORTHEAST;
             IDX(dirIndex) < COMBAT_AI_ADJACENT_DIRECTION_COUNT;
             dirIndex++) {
            adjCell = thisArmy->GetAdjacentCellIndex(
                giNextActionGridIndex, dirIndex
            );
            if (adjCell > 0 && adjCell < COMBAT_HEX_COUNT
                && gpCombatManager->m_hexCells[adjCell].m_occupantSide
                       == OppositeCombatSide(m_currentSide)) {
                giNextActionGridIndex = adjCell;
                return;
            }
        }
    }
}


AFTER strengths1_fifth1_castle0_archery0
void combatManager::DoCompAI(H2_ENUM_PARAM(CombatSide, i32)) {
    u32 traitorArray[COMBAT_SIDE_COUNT];
    i32 best;
    CombatSide sideEnemy;
    u32 flyerMask[COMBAT_SIDE_COUNT];
    u32 walkers[COMBAT_SIDE_COUNT];
    b32 stronger;
    b32 shootStrong;
    u32l myShootPower;
    i32 plan;
    u32l enemyShooters;
    army* thisArmy;
    u32 oddMasks[COMBAT_SIDE_COUNT];
    CombatHexDirection dirIndex;
    u32l totalArmyStrength;
    combatManager* H2_UNUSED(combat);
    u32 shooters[COMBAT_SIDE_COUNT];
    i32 archers;
    u32 mirrorMask[COMBAT_SIDE_COUNT];
    i32 rowIndex;
    b32 H2_UNUSED(archeryBonus);
    i32 plusArchers;
    i32 keepStrength;
    i32 grade;
    town* H2_UNUSED(pCastle);
    u8 rowLimit[COMBAT_AI_CASTLE_BOUNDARY_COUNT];
    i32 adjCell;
    hexcell* targetCell;
    u32 targetHex;

    m_limitCreature = false;
    thisArmy = m_currentArmyIndex + m_armies[IDX(m_currentArmySide)];
    plan = COMBAT_AI_ATTACK_NONE;
    sideEnemy = OppositeCombatSide(m_currentSide);

    mirrorMask[IDX(m_currentSide)] = GetMirrorImageMask(m_currentSide);
    mirrorMask[IDX(sideEnemy)] = GetMirrorImageMask(sideEnemy);
    shooters[IDX(m_currentSide)] = GetShooterMask(m_currentSide);
    shooters[IDX(sideEnemy)] = GetShooterMask(sideEnemy);
    flyerMask[IDX(m_currentSide)] = GetFlyerMask(m_currentSide);
    flyerMask[IDX(sideEnemy)] = GetFlyerMask(sideEnemy);
    walkers[IDX(m_currentSide)] = GetWalkerMask(m_currentSide);
    walkers[IDX(sideEnemy)] = GetWalkerMask(sideEnemy);
    oddMasks[IDX(m_currentSide)] = GetOutOfItMask(m_currentSide);
    oddMasks[IDX(sideEnemy)] = GetOutOfItMask(sideEnemy);
    traitorArray[IDX(m_currentSide)] = GetTraitorMask(m_currentSide);
    traitorArray[IDX(sideEnemy)] = GetTraitorMask(sideEnemy);
    GetStrength(m_currentSide, shooters[IDX(m_currentSide)]);
    GetStrength(sideEnemy, shooters[IDX(sideEnemy)]);
    totalArmyStrength = GetStrength(
        m_currentSide,
        shooters[IDX(m_currentSide)] | flyerMask[IDX(m_currentSide)]
            | walkers[IDX(m_currentSide)] | oddMasks[IDX(m_currentSide)]
            | traitorArray[IDX(m_currentSide)]
    );
    static_cast<i32>(totalArmyStrength + COMBAT_AI_STRENGTH_ROUNDING)
                         / COMBAT_AI_STRENGTH_FRACTION;
    shootStrong = false;
    stronger = false;
    myShootPower = GetStrength(m_currentSide, shooters[IDX(m_currentSide)]);
    enemyShooters = GetStrength(sideEnemy, shooters[IDX(sideEnemy)]);

    if (m_inCastleCombat != 0) {
        if (m_heroes[IDX(COMBAT_ATTACKER_SIDE)]->m_secondarySkills[IDX(HERO_SKILL_ARCHERY)]
                == HERO_SKILL_LEVEL_NONE
            && !m_heroes[IDX(COMBAT_ATTACKER_SIDE)]->HasArtifact(ARTIFACT_GOLDEN_BOW)) {
            if (m_currentSide == COMBAT_ATTACKER_SIDE)
                myShootPower =
                    static_cast<i32>(myShootPower) / COMBAT_SIDE_COUNT;
            else
                enemyShooters =
                    static_cast<i32>(enemyShooters) / COMBAT_SIDE_COUNT;
        }
        if (m_wallStates[IDX(COMBAT_WALL_SLOT_KEEP)] == COMBAT_WALL_STATE_KEEP_STANDING) {
            pCastle = m_combatTowns[IDX(COMBAT_DEFENDER_SIDE)];
            m_combatTowns[IDX(COMBAT_DEFENDER_SIDE)]->CalcNumLevelArchers(&archers, &grade);
            plusArchers = 0;
            if (m_wallStates[IDX(COMBAT_WALL_SLOT_TOP_TOWER)]
                == COMBAT_WALL_STATE_TOWER_STANDING)
                plusArchers += archers / COMBAT_SIDE_COUNT;
            if (m_wallStates[IDX(COMBAT_WALL_SLOT_BOTTOM_TOWER)]
                == COMBAT_WALL_STATE_TOWER_STANDING)
                plusArchers += archers / COMBAT_SIDE_COUNT;
            archers += plusArchers;
            keepStrength = static_cast<i32>(
                archers * COMBAT_AI_TOWER_STRENGTH
                * (grade * COMBAT_AI_TOWER_LEVEL_SCALE + COMBAT_AI_TOWER_BASE_SCALE)
            );
            archeryBonus =
                m_heroes[IDX(COMBAT_ATTACKER_SIDE)] != NULL
                        && (m_heroes[IDX(COMBAT_ATTACKER_SIDE)]->HasArtifact(ARTIFACT_GOLDEN_BOW)
                            || m_heroes[IDX(COMBAT_ATTACKER_SIDE)]
                                       ->m_secondarySkills[IDX(HERO_SKILL_ARCHERY)]
                                   != HERO_SKILL_LEVEL_NONE)
                    ? 0
                    : 1;
            if (m_currentSide == COMBAT_DEFENDER_SIDE)
                myShootPower += keepStrength;
            else
                enemyShooters += keepStrength;
        }
    }

    if (m_heroes[IDX(m_currentSide)] != NULL) {
        myShootPower = static_cast<i32>(
            static_cast<i32>(myShootPower)
            * gfSSArcheryMod
                [IDX(m_heroes[IDX(m_currentSide)]->m_secondarySkills[IDX(HERO_SKILL_ARCHERY)])]
        );
    }
    if (m_heroes[IDX(OppositeCombatSide(m_currentSide))] != NULL) {
        enemyShooters = static_cast<i32>(
            static_cast<i32>(enemyShooters)
            * gfSSArcheryMod
                [IDX(m_heroes[IDX(OppositeCombatSide(m_currentSide))]
                         ->m_secondarySkills[IDX(HERO_SKILL_ARCHERY)])]
        );
    }
    if (static_cast<i32>(totalArmyStrength + COMBAT_AI_STRENGTH_ROUNDING)
            / COMBAT_AI_STRENGTH_FRACTION
        < static_cast<i32>(myShootPower))
        shootStrong = true;
    if (static_cast<i32>(enemyShooters) > static_cast<i32>(myShootPower))
        stronger = true;

    if (HAS(thisArmy->m_monster.flags.abilityFlags, MONSTER_ABILITY_FLAG_SHOOTER) != 0) {
        if (thisArmy->m_monster.shots > 0)
            plan = COMBAT_AI_ATTACK_SHOOT;
        else
            plan = COMBAT_AI_ATTACK_WALK;
    } else if (HAS(thisArmy->m_monster.flags.abilityFlags, MONSTER_ABILITY_FLAG_FLYING) != 0) {
        plan = COMBAT_AI_ATTACK_FLY;
    } else {
        plan = COMBAT_AI_ATTACK_WALK;
    }

    switch (plan) {
        case COMBAT_AI_ATTACK_SHOOT:
            if (AttemptAdjacentAttack(thisArmy))
                goto finish;
            if (thisArmy->m_monsterType == CREATURE_LICH
                || thisArmy->m_monsterType == CREATURE_POWER_LICH) {
                DoLichShot(thisArmy);
                goto finish;
            }
            best = GetBestArmy(sideEnemy, mirrorMask[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            best = GetBestArmy(sideEnemy, shooters[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            best = GetBestArmy(sideEnemy, flyerMask[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            if (walkers[IDX(sideEnemy)] != 0) {
                best =
                    GetClosestArmy(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]);
                if (best != -1) {
                    giNextAction = ACTION_MOVE;
                    giNextActionGridIndex =
                        (m_armies[IDX(sideEnemy)] + best)
                            ->m_hex;
                    goto finish;
                }
            }
            best = GetBestArmy(sideEnemy, oddMasks[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            best = GetBestArmy(sideEnemy, traitorArray[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            break;
        case COMBAT_AI_ATTACK_FLY:
            if (shootStrong != 0 && stronger == 0) {
                if (AttemptAttack(thisArmy, sideEnemy, mirrorMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                    goto finish;
            } else {
                if (AttemptAttack(thisArmy, sideEnemy, mirrorMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                    goto finish;
            }
            break;
        case COMBAT_AI_ATTACK_WALK:
            if (COMBAT_AI_ATTACK_NONE)
                goto finish;
            if (shootStrong != 0 && stronger == 0) {
                if (AttemptAttack(thisArmy, sideEnemy, mirrorMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                    goto finish;
                if (WalkTowardArmyFront(thisArmy, m_currentSide, shooters[IDX(m_currentSide)]))
                    goto finish;
                giNextAction = ACTION_WAIT;
                goto finish;
            }
            if (AttemptAttack(thisArmy, sideEnemy, mirrorMask[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                goto finish;

            rowLimit[0] = COMBAT_AI_CASTLE_BOUNDARY_ROW_0;
            rowLimit[1] = COMBAT_AI_CASTLE_BOUNDARY_ROW_1;
            rowLimit[2] = COMBAT_AI_CASTLE_BOUNDARY_ROW_2;
            rowLimit[3] = COMBAT_AI_CASTLE_BOUNDARY_ROW_3;
            rowLimit[4] = COMBAT_AI_CASTLE_BOUNDARY_ROW_4;
            rowLimit[5] = COMBAT_AI_CASTLE_BOUNDARY_ROW_5;
            rowLimit[6] = COMBAT_AI_CASTLE_BOUNDARY_ROW_6;
            rowLimit[7] = COMBAT_AI_CASTLE_BOUNDARY_ROW_7;
            rowLimit[8] = COMBAT_AI_CASTLE_BOUNDARY_ROW_8;
            rowIndex = thisArmy->m_hex / ARMY_HEX_COLUMNS;
            if (m_currentSide == COMBAT_ATTACKER_SIDE && m_inCastleCombat != 0
                && thisArmy->m_hex < rowLimit[rowIndex]) {
                targetHex = rowLimit[rowIndex];
                targetCell = &gpCombatManager->m_hexCells[targetHex];
                if (ValidHex(targetHex) && targetCell->m_occupantSide == COMBAT_SIDE_NONE
                    && targetCell->m_blocked == 0) {
                    giNextAction = ACTION_MOVE;
                    giNextActionGridIndex = targetHex;
                    goto finish;
                }
            }
            break;
    }
    giNextAction = ACTION_WAIT;

finish:
    if (giNextAction == ACTION_MOVE && giNextActionGridIndex > 0
        && giNextActionGridIndex < COMBAT_HEX_COUNT
        && gpCombatManager->m_hexCells[giNextActionGridIndex].m_occupantSide == COMBAT_SIDE_NONE) {
        for (dirIndex = COMBAT_DIRECTION_NORTHEAST;
             IDX(dirIndex) < COMBAT_AI_ADJACENT_DIRECTION_COUNT;
             dirIndex++) {
            adjCell = thisArmy->GetAdjacentCellIndex(
                giNextActionGridIndex, dirIndex
            );
            if (adjCell > 0 && adjCell < COMBAT_HEX_COUNT
                && gpCombatManager->m_hexCells[adjCell].m_occupantSide
                       == OppositeCombatSide(m_currentSide)) {
                giNextActionGridIndex = adjCell;
                return;
            }
        }
    }
}


AFTER strengths1_fifth1_castle0_archery1
void combatManager::DoCompAI(H2_ENUM_PARAM(CombatSide, i32)) {
    u32 traitorArray[COMBAT_SIDE_COUNT];
    i32 best;
    CombatSide sideEnemy;
    u32 flyerMask[COMBAT_SIDE_COUNT];
    u32 walkers[COMBAT_SIDE_COUNT];
    b32 stronger;
    b32 shootStrong;
    u32l myShootPower;
    i32 plan;
    u32l enemyShooters;
    army* thisArmy;
    u32 oddMasks[COMBAT_SIDE_COUNT];
    CombatHexDirection dirIndex;
    u32l totalArmyStrength;
    combatManager* H2_UNUSED(combat);
    u32 shooters[COMBAT_SIDE_COUNT];
    i32 archers;
    u32 mirrorMask[COMBAT_SIDE_COUNT];
    i32 rowIndex;
    i32 plusArchers;
    i32 keepStrength;
    i32 grade;
    town* H2_UNUSED(pCastle);
    u8 rowLimit[COMBAT_AI_CASTLE_BOUNDARY_COUNT];
    i32 adjCell;
    hexcell* targetCell;
    u32 targetHex;

    m_limitCreature = false;
    thisArmy = m_currentArmyIndex + m_armies[IDX(m_currentArmySide)];
    plan = COMBAT_AI_ATTACK_NONE;
    sideEnemy = OppositeCombatSide(m_currentSide);

    mirrorMask[IDX(m_currentSide)] = GetMirrorImageMask(m_currentSide);
    mirrorMask[IDX(sideEnemy)] = GetMirrorImageMask(sideEnemy);
    shooters[IDX(m_currentSide)] = GetShooterMask(m_currentSide);
    shooters[IDX(sideEnemy)] = GetShooterMask(sideEnemy);
    flyerMask[IDX(m_currentSide)] = GetFlyerMask(m_currentSide);
    flyerMask[IDX(sideEnemy)] = GetFlyerMask(sideEnemy);
    walkers[IDX(m_currentSide)] = GetWalkerMask(m_currentSide);
    walkers[IDX(sideEnemy)] = GetWalkerMask(sideEnemy);
    oddMasks[IDX(m_currentSide)] = GetOutOfItMask(m_currentSide);
    oddMasks[IDX(sideEnemy)] = GetOutOfItMask(sideEnemy);
    traitorArray[IDX(m_currentSide)] = GetTraitorMask(m_currentSide);
    traitorArray[IDX(sideEnemy)] = GetTraitorMask(sideEnemy);
    GetStrength(m_currentSide, shooters[IDX(m_currentSide)]);
    GetStrength(sideEnemy, shooters[IDX(sideEnemy)]);
    totalArmyStrength = GetStrength(
        m_currentSide,
        shooters[IDX(m_currentSide)] | flyerMask[IDX(m_currentSide)]
            | walkers[IDX(m_currentSide)] | oddMasks[IDX(m_currentSide)]
            | traitorArray[IDX(m_currentSide)]
    );
    static_cast<i32>(totalArmyStrength + COMBAT_AI_STRENGTH_ROUNDING)
                         / COMBAT_AI_STRENGTH_FRACTION;
    shootStrong = false;
    stronger = false;
    myShootPower = GetStrength(m_currentSide, shooters[IDX(m_currentSide)]);
    enemyShooters = GetStrength(sideEnemy, shooters[IDX(sideEnemy)]);

    if (m_inCastleCombat != 0) {
        if (m_heroes[IDX(COMBAT_ATTACKER_SIDE)]->m_secondarySkills[IDX(HERO_SKILL_ARCHERY)]
                == HERO_SKILL_LEVEL_NONE
            && !m_heroes[IDX(COMBAT_ATTACKER_SIDE)]->HasArtifact(ARTIFACT_GOLDEN_BOW)) {
            if (m_currentSide == COMBAT_ATTACKER_SIDE)
                myShootPower =
                    static_cast<i32>(myShootPower) / COMBAT_SIDE_COUNT;
            else
                enemyShooters =
                    static_cast<i32>(enemyShooters) / COMBAT_SIDE_COUNT;
        }
        if (m_wallStates[IDX(COMBAT_WALL_SLOT_KEEP)] == COMBAT_WALL_STATE_KEEP_STANDING) {
            pCastle = m_combatTowns[IDX(COMBAT_DEFENDER_SIDE)];
            m_combatTowns[IDX(COMBAT_DEFENDER_SIDE)]->CalcNumLevelArchers(&archers, &grade);
            plusArchers = 0;
            if (m_wallStates[IDX(COMBAT_WALL_SLOT_TOP_TOWER)]
                == COMBAT_WALL_STATE_TOWER_STANDING)
                plusArchers += archers / COMBAT_SIDE_COUNT;
            if (m_wallStates[IDX(COMBAT_WALL_SLOT_BOTTOM_TOWER)]
                == COMBAT_WALL_STATE_TOWER_STANDING)
                plusArchers += archers / COMBAT_SIDE_COUNT;
            archers += plusArchers;
            keepStrength = static_cast<i32>(
                archers * COMBAT_AI_TOWER_STRENGTH
                * (grade * COMBAT_AI_TOWER_LEVEL_SCALE + COMBAT_AI_TOWER_BASE_SCALE)
            );
                m_heroes[IDX(COMBAT_ATTACKER_SIDE)] != NULL
                        && (m_heroes[IDX(COMBAT_ATTACKER_SIDE)]->HasArtifact(ARTIFACT_GOLDEN_BOW)
                            || m_heroes[IDX(COMBAT_ATTACKER_SIDE)]
                                       ->m_secondarySkills[IDX(HERO_SKILL_ARCHERY)]
                                   != HERO_SKILL_LEVEL_NONE)
                    ? 0
                    : 1;
            if (m_currentSide == COMBAT_DEFENDER_SIDE)
                myShootPower += keepStrength;
            else
                enemyShooters += keepStrength;
        }
    }

    if (m_heroes[IDX(m_currentSide)] != NULL) {
        myShootPower = static_cast<i32>(
            static_cast<i32>(myShootPower)
            * gfSSArcheryMod
                [IDX(m_heroes[IDX(m_currentSide)]->m_secondarySkills[IDX(HERO_SKILL_ARCHERY)])]
        );
    }
    if (m_heroes[IDX(OppositeCombatSide(m_currentSide))] != NULL) {
        enemyShooters = static_cast<i32>(
            static_cast<i32>(enemyShooters)
            * gfSSArcheryMod
                [IDX(m_heroes[IDX(OppositeCombatSide(m_currentSide))]
                         ->m_secondarySkills[IDX(HERO_SKILL_ARCHERY)])]
        );
    }
    if (static_cast<i32>(totalArmyStrength + COMBAT_AI_STRENGTH_ROUNDING)
            / COMBAT_AI_STRENGTH_FRACTION
        < static_cast<i32>(myShootPower))
        shootStrong = true;
    if (static_cast<i32>(enemyShooters) > static_cast<i32>(myShootPower))
        stronger = true;

    if (HAS(thisArmy->m_monster.flags.abilityFlags, MONSTER_ABILITY_FLAG_SHOOTER) != 0) {
        if (thisArmy->m_monster.shots > 0)
            plan = COMBAT_AI_ATTACK_SHOOT;
        else
            plan = COMBAT_AI_ATTACK_WALK;
    } else if (HAS(thisArmy->m_monster.flags.abilityFlags, MONSTER_ABILITY_FLAG_FLYING) != 0) {
        plan = COMBAT_AI_ATTACK_FLY;
    } else {
        plan = COMBAT_AI_ATTACK_WALK;
    }

    switch (plan) {
        case COMBAT_AI_ATTACK_SHOOT:
            if (AttemptAdjacentAttack(thisArmy))
                goto finish;
            if (thisArmy->m_monsterType == CREATURE_LICH
                || thisArmy->m_monsterType == CREATURE_POWER_LICH) {
                DoLichShot(thisArmy);
                goto finish;
            }
            best = GetBestArmy(sideEnemy, mirrorMask[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            best = GetBestArmy(sideEnemy, shooters[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            best = GetBestArmy(sideEnemy, flyerMask[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            if (walkers[IDX(sideEnemy)] != 0) {
                best =
                    GetClosestArmy(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]);
                if (best != -1) {
                    giNextAction = ACTION_MOVE;
                    giNextActionGridIndex =
                        (m_armies[IDX(sideEnemy)] + best)
                            ->m_hex;
                    goto finish;
                }
            }
            best = GetBestArmy(sideEnemy, oddMasks[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            best = GetBestArmy(sideEnemy, traitorArray[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            break;
        case COMBAT_AI_ATTACK_FLY:
            if (shootStrong != 0 && stronger == 0) {
                if (AttemptAttack(thisArmy, sideEnemy, mirrorMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                    goto finish;
            } else {
                if (AttemptAttack(thisArmy, sideEnemy, mirrorMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                    goto finish;
            }
            break;
        case COMBAT_AI_ATTACK_WALK:
            if (COMBAT_AI_ATTACK_NONE)
                goto finish;
            if (shootStrong != 0 && stronger == 0) {
                if (AttemptAttack(thisArmy, sideEnemy, mirrorMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                    goto finish;
                if (WalkTowardArmyFront(thisArmy, m_currentSide, shooters[IDX(m_currentSide)]))
                    goto finish;
                giNextAction = ACTION_WAIT;
                goto finish;
            }
            if (AttemptAttack(thisArmy, sideEnemy, mirrorMask[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                goto finish;

            rowLimit[0] = COMBAT_AI_CASTLE_BOUNDARY_ROW_0;
            rowLimit[1] = COMBAT_AI_CASTLE_BOUNDARY_ROW_1;
            rowLimit[2] = COMBAT_AI_CASTLE_BOUNDARY_ROW_2;
            rowLimit[3] = COMBAT_AI_CASTLE_BOUNDARY_ROW_3;
            rowLimit[4] = COMBAT_AI_CASTLE_BOUNDARY_ROW_4;
            rowLimit[5] = COMBAT_AI_CASTLE_BOUNDARY_ROW_5;
            rowLimit[6] = COMBAT_AI_CASTLE_BOUNDARY_ROW_6;
            rowLimit[7] = COMBAT_AI_CASTLE_BOUNDARY_ROW_7;
            rowLimit[8] = COMBAT_AI_CASTLE_BOUNDARY_ROW_8;
            rowIndex = thisArmy->m_hex / ARMY_HEX_COLUMNS;
            if (m_currentSide == COMBAT_ATTACKER_SIDE && m_inCastleCombat != 0
                && thisArmy->m_hex < rowLimit[rowIndex]) {
                targetHex = rowLimit[rowIndex];
                targetCell = &gpCombatManager->m_hexCells[targetHex];
                if (ValidHex(targetHex) && targetCell->m_occupantSide == COMBAT_SIDE_NONE
                    && targetCell->m_blocked == 0) {
                    giNextAction = ACTION_MOVE;
                    giNextActionGridIndex = targetHex;
                    goto finish;
                }
            }
            break;
    }
    giNextAction = ACTION_WAIT;

finish:
    if (giNextAction == ACTION_MOVE && giNextActionGridIndex > 0
        && giNextActionGridIndex < COMBAT_HEX_COUNT
        && gpCombatManager->m_hexCells[giNextActionGridIndex].m_occupantSide == COMBAT_SIDE_NONE) {
        for (dirIndex = COMBAT_DIRECTION_NORTHEAST;
             IDX(dirIndex) < COMBAT_AI_ADJACENT_DIRECTION_COUNT;
             dirIndex++) {
            adjCell = thisArmy->GetAdjacentCellIndex(
                giNextActionGridIndex, dirIndex
            );
            if (adjCell > 0 && adjCell < COMBAT_HEX_COUNT
                && gpCombatManager->m_hexCells[adjCell].m_occupantSide
                       == OppositeCombatSide(m_currentSide)) {
                giNextActionGridIndex = adjCell;
                return;
            }
        }
    }
}


AFTER strengths1_fifth1_castle1_archery0
void combatManager::DoCompAI(H2_ENUM_PARAM(CombatSide, i32)) {
    u32 traitorArray[COMBAT_SIDE_COUNT];
    i32 best;
    CombatSide sideEnemy;
    u32 flyerMask[COMBAT_SIDE_COUNT];
    u32 walkers[COMBAT_SIDE_COUNT];
    b32 stronger;
    b32 shootStrong;
    u32l myShootPower;
    i32 plan;
    u32l enemyShooters;
    army* thisArmy;
    u32 oddMasks[COMBAT_SIDE_COUNT];
    CombatHexDirection dirIndex;
    u32l totalArmyStrength;
    combatManager* H2_UNUSED(combat);
    u32 shooters[COMBAT_SIDE_COUNT];
    i32 archers;
    u32 mirrorMask[COMBAT_SIDE_COUNT];
    i32 rowIndex;
    b32 H2_UNUSED(archeryBonus);
    i32 plusArchers;
    i32 keepStrength;
    i32 grade;
    u8 rowLimit[COMBAT_AI_CASTLE_BOUNDARY_COUNT];
    i32 adjCell;
    hexcell* targetCell;
    u32 targetHex;

    m_limitCreature = false;
    thisArmy = m_currentArmyIndex + m_armies[IDX(m_currentArmySide)];
    plan = COMBAT_AI_ATTACK_NONE;
    sideEnemy = OppositeCombatSide(m_currentSide);

    mirrorMask[IDX(m_currentSide)] = GetMirrorImageMask(m_currentSide);
    mirrorMask[IDX(sideEnemy)] = GetMirrorImageMask(sideEnemy);
    shooters[IDX(m_currentSide)] = GetShooterMask(m_currentSide);
    shooters[IDX(sideEnemy)] = GetShooterMask(sideEnemy);
    flyerMask[IDX(m_currentSide)] = GetFlyerMask(m_currentSide);
    flyerMask[IDX(sideEnemy)] = GetFlyerMask(sideEnemy);
    walkers[IDX(m_currentSide)] = GetWalkerMask(m_currentSide);
    walkers[IDX(sideEnemy)] = GetWalkerMask(sideEnemy);
    oddMasks[IDX(m_currentSide)] = GetOutOfItMask(m_currentSide);
    oddMasks[IDX(sideEnemy)] = GetOutOfItMask(sideEnemy);
    traitorArray[IDX(m_currentSide)] = GetTraitorMask(m_currentSide);
    traitorArray[IDX(sideEnemy)] = GetTraitorMask(sideEnemy);
    GetStrength(m_currentSide, shooters[IDX(m_currentSide)]);
    GetStrength(sideEnemy, shooters[IDX(sideEnemy)]);
    totalArmyStrength = GetStrength(
        m_currentSide,
        shooters[IDX(m_currentSide)] | flyerMask[IDX(m_currentSide)]
            | walkers[IDX(m_currentSide)] | oddMasks[IDX(m_currentSide)]
            | traitorArray[IDX(m_currentSide)]
    );
    static_cast<i32>(totalArmyStrength + COMBAT_AI_STRENGTH_ROUNDING)
                         / COMBAT_AI_STRENGTH_FRACTION;
    shootStrong = false;
    stronger = false;
    myShootPower = GetStrength(m_currentSide, shooters[IDX(m_currentSide)]);
    enemyShooters = GetStrength(sideEnemy, shooters[IDX(sideEnemy)]);

    if (m_inCastleCombat != 0) {
        if (m_heroes[IDX(COMBAT_ATTACKER_SIDE)]->m_secondarySkills[IDX(HERO_SKILL_ARCHERY)]
                == HERO_SKILL_LEVEL_NONE
            && !m_heroes[IDX(COMBAT_ATTACKER_SIDE)]->HasArtifact(ARTIFACT_GOLDEN_BOW)) {
            if (m_currentSide == COMBAT_ATTACKER_SIDE)
                myShootPower =
                    static_cast<i32>(myShootPower) / COMBAT_SIDE_COUNT;
            else
                enemyShooters =
                    static_cast<i32>(enemyShooters) / COMBAT_SIDE_COUNT;
        }
        if (m_wallStates[IDX(COMBAT_WALL_SLOT_KEEP)] == COMBAT_WALL_STATE_KEEP_STANDING) {
            m_combatTowns[IDX(COMBAT_DEFENDER_SIDE)]->CalcNumLevelArchers(&archers, &grade);
            plusArchers = 0;
            if (m_wallStates[IDX(COMBAT_WALL_SLOT_TOP_TOWER)]
                == COMBAT_WALL_STATE_TOWER_STANDING)
                plusArchers += archers / COMBAT_SIDE_COUNT;
            if (m_wallStates[IDX(COMBAT_WALL_SLOT_BOTTOM_TOWER)]
                == COMBAT_WALL_STATE_TOWER_STANDING)
                plusArchers += archers / COMBAT_SIDE_COUNT;
            archers += plusArchers;
            keepStrength = static_cast<i32>(
                archers * COMBAT_AI_TOWER_STRENGTH
                * (grade * COMBAT_AI_TOWER_LEVEL_SCALE + COMBAT_AI_TOWER_BASE_SCALE)
            );
            archeryBonus =
                m_heroes[IDX(COMBAT_ATTACKER_SIDE)] != NULL
                        && (m_heroes[IDX(COMBAT_ATTACKER_SIDE)]->HasArtifact(ARTIFACT_GOLDEN_BOW)
                            || m_heroes[IDX(COMBAT_ATTACKER_SIDE)]
                                       ->m_secondarySkills[IDX(HERO_SKILL_ARCHERY)]
                                   != HERO_SKILL_LEVEL_NONE)
                    ? 0
                    : 1;
            if (m_currentSide == COMBAT_DEFENDER_SIDE)
                myShootPower += keepStrength;
            else
                enemyShooters += keepStrength;
        }
    }

    if (m_heroes[IDX(m_currentSide)] != NULL) {
        myShootPower = static_cast<i32>(
            static_cast<i32>(myShootPower)
            * gfSSArcheryMod
                [IDX(m_heroes[IDX(m_currentSide)]->m_secondarySkills[IDX(HERO_SKILL_ARCHERY)])]
        );
    }
    if (m_heroes[IDX(OppositeCombatSide(m_currentSide))] != NULL) {
        enemyShooters = static_cast<i32>(
            static_cast<i32>(enemyShooters)
            * gfSSArcheryMod
                [IDX(m_heroes[IDX(OppositeCombatSide(m_currentSide))]
                         ->m_secondarySkills[IDX(HERO_SKILL_ARCHERY)])]
        );
    }
    if (static_cast<i32>(totalArmyStrength + COMBAT_AI_STRENGTH_ROUNDING)
            / COMBAT_AI_STRENGTH_FRACTION
        < static_cast<i32>(myShootPower))
        shootStrong = true;
    if (static_cast<i32>(enemyShooters) > static_cast<i32>(myShootPower))
        stronger = true;

    if (HAS(thisArmy->m_monster.flags.abilityFlags, MONSTER_ABILITY_FLAG_SHOOTER) != 0) {
        if (thisArmy->m_monster.shots > 0)
            plan = COMBAT_AI_ATTACK_SHOOT;
        else
            plan = COMBAT_AI_ATTACK_WALK;
    } else if (HAS(thisArmy->m_monster.flags.abilityFlags, MONSTER_ABILITY_FLAG_FLYING) != 0) {
        plan = COMBAT_AI_ATTACK_FLY;
    } else {
        plan = COMBAT_AI_ATTACK_WALK;
    }

    switch (plan) {
        case COMBAT_AI_ATTACK_SHOOT:
            if (AttemptAdjacentAttack(thisArmy))
                goto finish;
            if (thisArmy->m_monsterType == CREATURE_LICH
                || thisArmy->m_monsterType == CREATURE_POWER_LICH) {
                DoLichShot(thisArmy);
                goto finish;
            }
            best = GetBestArmy(sideEnemy, mirrorMask[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            best = GetBestArmy(sideEnemy, shooters[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            best = GetBestArmy(sideEnemy, flyerMask[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            if (walkers[IDX(sideEnemy)] != 0) {
                best =
                    GetClosestArmy(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]);
                if (best != -1) {
                    giNextAction = ACTION_MOVE;
                    giNextActionGridIndex =
                        (m_armies[IDX(sideEnemy)] + best)
                            ->m_hex;
                    goto finish;
                }
            }
            best = GetBestArmy(sideEnemy, oddMasks[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            best = GetBestArmy(sideEnemy, traitorArray[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            break;
        case COMBAT_AI_ATTACK_FLY:
            if (shootStrong != 0 && stronger == 0) {
                if (AttemptAttack(thisArmy, sideEnemy, mirrorMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                    goto finish;
            } else {
                if (AttemptAttack(thisArmy, sideEnemy, mirrorMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                    goto finish;
            }
            break;
        case COMBAT_AI_ATTACK_WALK:
            if (COMBAT_AI_ATTACK_NONE)
                goto finish;
            if (shootStrong != 0 && stronger == 0) {
                if (AttemptAttack(thisArmy, sideEnemy, mirrorMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                    goto finish;
                if (WalkTowardArmyFront(thisArmy, m_currentSide, shooters[IDX(m_currentSide)]))
                    goto finish;
                giNextAction = ACTION_WAIT;
                goto finish;
            }
            if (AttemptAttack(thisArmy, sideEnemy, mirrorMask[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                goto finish;

            rowLimit[0] = COMBAT_AI_CASTLE_BOUNDARY_ROW_0;
            rowLimit[1] = COMBAT_AI_CASTLE_BOUNDARY_ROW_1;
            rowLimit[2] = COMBAT_AI_CASTLE_BOUNDARY_ROW_2;
            rowLimit[3] = COMBAT_AI_CASTLE_BOUNDARY_ROW_3;
            rowLimit[4] = COMBAT_AI_CASTLE_BOUNDARY_ROW_4;
            rowLimit[5] = COMBAT_AI_CASTLE_BOUNDARY_ROW_5;
            rowLimit[6] = COMBAT_AI_CASTLE_BOUNDARY_ROW_6;
            rowLimit[7] = COMBAT_AI_CASTLE_BOUNDARY_ROW_7;
            rowLimit[8] = COMBAT_AI_CASTLE_BOUNDARY_ROW_8;
            rowIndex = thisArmy->m_hex / ARMY_HEX_COLUMNS;
            if (m_currentSide == COMBAT_ATTACKER_SIDE && m_inCastleCombat != 0
                && thisArmy->m_hex < rowLimit[rowIndex]) {
                targetHex = rowLimit[rowIndex];
                targetCell = &gpCombatManager->m_hexCells[targetHex];
                if (ValidHex(targetHex) && targetCell->m_occupantSide == COMBAT_SIDE_NONE
                    && targetCell->m_blocked == 0) {
                    giNextAction = ACTION_MOVE;
                    giNextActionGridIndex = targetHex;
                    goto finish;
                }
            }
            break;
    }
    giNextAction = ACTION_WAIT;

finish:
    if (giNextAction == ACTION_MOVE && giNextActionGridIndex > 0
        && giNextActionGridIndex < COMBAT_HEX_COUNT
        && gpCombatManager->m_hexCells[giNextActionGridIndex].m_occupantSide == COMBAT_SIDE_NONE) {
        for (dirIndex = COMBAT_DIRECTION_NORTHEAST;
             IDX(dirIndex) < COMBAT_AI_ADJACENT_DIRECTION_COUNT;
             dirIndex++) {
            adjCell = thisArmy->GetAdjacentCellIndex(
                giNextActionGridIndex, dirIndex
            );
            if (adjCell > 0 && adjCell < COMBAT_HEX_COUNT
                && gpCombatManager->m_hexCells[adjCell].m_occupantSide
                       == OppositeCombatSide(m_currentSide)) {
                giNextActionGridIndex = adjCell;
                return;
            }
        }
    }
}


AFTER strengths1_fifth1_castle1_archery1
void combatManager::DoCompAI(H2_ENUM_PARAM(CombatSide, i32)) {
    u32 traitorArray[COMBAT_SIDE_COUNT];
    i32 best;
    CombatSide sideEnemy;
    u32 flyerMask[COMBAT_SIDE_COUNT];
    u32 walkers[COMBAT_SIDE_COUNT];
    b32 stronger;
    b32 shootStrong;
    u32l myShootPower;
    i32 plan;
    u32l enemyShooters;
    army* thisArmy;
    u32 oddMasks[COMBAT_SIDE_COUNT];
    CombatHexDirection dirIndex;
    u32l totalArmyStrength;
    combatManager* H2_UNUSED(combat);
    u32 shooters[COMBAT_SIDE_COUNT];
    i32 archers;
    u32 mirrorMask[COMBAT_SIDE_COUNT];
    i32 rowIndex;
    i32 plusArchers;
    i32 keepStrength;
    i32 grade;
    u8 rowLimit[COMBAT_AI_CASTLE_BOUNDARY_COUNT];
    i32 adjCell;
    hexcell* targetCell;
    u32 targetHex;

    m_limitCreature = false;
    thisArmy = m_currentArmyIndex + m_armies[IDX(m_currentArmySide)];
    plan = COMBAT_AI_ATTACK_NONE;
    sideEnemy = OppositeCombatSide(m_currentSide);

    mirrorMask[IDX(m_currentSide)] = GetMirrorImageMask(m_currentSide);
    mirrorMask[IDX(sideEnemy)] = GetMirrorImageMask(sideEnemy);
    shooters[IDX(m_currentSide)] = GetShooterMask(m_currentSide);
    shooters[IDX(sideEnemy)] = GetShooterMask(sideEnemy);
    flyerMask[IDX(m_currentSide)] = GetFlyerMask(m_currentSide);
    flyerMask[IDX(sideEnemy)] = GetFlyerMask(sideEnemy);
    walkers[IDX(m_currentSide)] = GetWalkerMask(m_currentSide);
    walkers[IDX(sideEnemy)] = GetWalkerMask(sideEnemy);
    oddMasks[IDX(m_currentSide)] = GetOutOfItMask(m_currentSide);
    oddMasks[IDX(sideEnemy)] = GetOutOfItMask(sideEnemy);
    traitorArray[IDX(m_currentSide)] = GetTraitorMask(m_currentSide);
    traitorArray[IDX(sideEnemy)] = GetTraitorMask(sideEnemy);
    GetStrength(m_currentSide, shooters[IDX(m_currentSide)]);
    GetStrength(sideEnemy, shooters[IDX(sideEnemy)]);
    totalArmyStrength = GetStrength(
        m_currentSide,
        shooters[IDX(m_currentSide)] | flyerMask[IDX(m_currentSide)]
            | walkers[IDX(m_currentSide)] | oddMasks[IDX(m_currentSide)]
            | traitorArray[IDX(m_currentSide)]
    );
    static_cast<i32>(totalArmyStrength + COMBAT_AI_STRENGTH_ROUNDING)
                         / COMBAT_AI_STRENGTH_FRACTION;
    shootStrong = false;
    stronger = false;
    myShootPower = GetStrength(m_currentSide, shooters[IDX(m_currentSide)]);
    enemyShooters = GetStrength(sideEnemy, shooters[IDX(sideEnemy)]);

    if (m_inCastleCombat != 0) {
        if (m_heroes[IDX(COMBAT_ATTACKER_SIDE)]->m_secondarySkills[IDX(HERO_SKILL_ARCHERY)]
                == HERO_SKILL_LEVEL_NONE
            && !m_heroes[IDX(COMBAT_ATTACKER_SIDE)]->HasArtifact(ARTIFACT_GOLDEN_BOW)) {
            if (m_currentSide == COMBAT_ATTACKER_SIDE)
                myShootPower =
                    static_cast<i32>(myShootPower) / COMBAT_SIDE_COUNT;
            else
                enemyShooters =
                    static_cast<i32>(enemyShooters) / COMBAT_SIDE_COUNT;
        }
        if (m_wallStates[IDX(COMBAT_WALL_SLOT_KEEP)] == COMBAT_WALL_STATE_KEEP_STANDING) {
            m_combatTowns[IDX(COMBAT_DEFENDER_SIDE)]->CalcNumLevelArchers(&archers, &grade);
            plusArchers = 0;
            if (m_wallStates[IDX(COMBAT_WALL_SLOT_TOP_TOWER)]
                == COMBAT_WALL_STATE_TOWER_STANDING)
                plusArchers += archers / COMBAT_SIDE_COUNT;
            if (m_wallStates[IDX(COMBAT_WALL_SLOT_BOTTOM_TOWER)]
                == COMBAT_WALL_STATE_TOWER_STANDING)
                plusArchers += archers / COMBAT_SIDE_COUNT;
            archers += plusArchers;
            keepStrength = static_cast<i32>(
                archers * COMBAT_AI_TOWER_STRENGTH
                * (grade * COMBAT_AI_TOWER_LEVEL_SCALE + COMBAT_AI_TOWER_BASE_SCALE)
            );
                m_heroes[IDX(COMBAT_ATTACKER_SIDE)] != NULL
                        && (m_heroes[IDX(COMBAT_ATTACKER_SIDE)]->HasArtifact(ARTIFACT_GOLDEN_BOW)
                            || m_heroes[IDX(COMBAT_ATTACKER_SIDE)]
                                       ->m_secondarySkills[IDX(HERO_SKILL_ARCHERY)]
                                   != HERO_SKILL_LEVEL_NONE)
                    ? 0
                    : 1;
            if (m_currentSide == COMBAT_DEFENDER_SIDE)
                myShootPower += keepStrength;
            else
                enemyShooters += keepStrength;
        }
    }

    if (m_heroes[IDX(m_currentSide)] != NULL) {
        myShootPower = static_cast<i32>(
            static_cast<i32>(myShootPower)
            * gfSSArcheryMod
                [IDX(m_heroes[IDX(m_currentSide)]->m_secondarySkills[IDX(HERO_SKILL_ARCHERY)])]
        );
    }
    if (m_heroes[IDX(OppositeCombatSide(m_currentSide))] != NULL) {
        enemyShooters = static_cast<i32>(
            static_cast<i32>(enemyShooters)
            * gfSSArcheryMod
                [IDX(m_heroes[IDX(OppositeCombatSide(m_currentSide))]
                         ->m_secondarySkills[IDX(HERO_SKILL_ARCHERY)])]
        );
    }
    if (static_cast<i32>(totalArmyStrength + COMBAT_AI_STRENGTH_ROUNDING)
            / COMBAT_AI_STRENGTH_FRACTION
        < static_cast<i32>(myShootPower))
        shootStrong = true;
    if (static_cast<i32>(enemyShooters) > static_cast<i32>(myShootPower))
        stronger = true;

    if (HAS(thisArmy->m_monster.flags.abilityFlags, MONSTER_ABILITY_FLAG_SHOOTER) != 0) {
        if (thisArmy->m_monster.shots > 0)
            plan = COMBAT_AI_ATTACK_SHOOT;
        else
            plan = COMBAT_AI_ATTACK_WALK;
    } else if (HAS(thisArmy->m_monster.flags.abilityFlags, MONSTER_ABILITY_FLAG_FLYING) != 0) {
        plan = COMBAT_AI_ATTACK_FLY;
    } else {
        plan = COMBAT_AI_ATTACK_WALK;
    }

    switch (plan) {
        case COMBAT_AI_ATTACK_SHOOT:
            if (AttemptAdjacentAttack(thisArmy))
                goto finish;
            if (thisArmy->m_monsterType == CREATURE_LICH
                || thisArmy->m_monsterType == CREATURE_POWER_LICH) {
                DoLichShot(thisArmy);
                goto finish;
            }
            best = GetBestArmy(sideEnemy, mirrorMask[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            best = GetBestArmy(sideEnemy, shooters[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            best = GetBestArmy(sideEnemy, flyerMask[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            if (walkers[IDX(sideEnemy)] != 0) {
                best =
                    GetClosestArmy(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]);
                if (best != -1) {
                    giNextAction = ACTION_MOVE;
                    giNextActionGridIndex =
                        (m_armies[IDX(sideEnemy)] + best)
                            ->m_hex;
                    goto finish;
                }
            }
            best = GetBestArmy(sideEnemy, oddMasks[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            best = GetBestArmy(sideEnemy, traitorArray[IDX(sideEnemy)]);
            if (best != -1) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[IDX(sideEnemy)] + best)
                        ->m_hex;
                goto finish;
            }
            break;
        case COMBAT_AI_ATTACK_FLY:
            if (shootStrong != 0 && stronger == 0) {
                if (AttemptAttack(thisArmy, sideEnemy, mirrorMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                    goto finish;
            } else {
                if (AttemptAttack(thisArmy, sideEnemy, mirrorMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                    goto finish;
            }
            break;
        case COMBAT_AI_ATTACK_WALK:
            if (COMBAT_AI_ATTACK_NONE)
                goto finish;
            if (shootStrong != 0 && stronger == 0) {
                if (AttemptAttack(thisArmy, sideEnemy, mirrorMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                    goto finish;
                if (AttemptAttack(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                    goto finish;
                if (WalkTowardArmyFront(thisArmy, m_currentSide, shooters[IDX(m_currentSide)]))
                    goto finish;
                giNextAction = ACTION_WAIT;
                goto finish;
            }
            if (AttemptAttack(thisArmy, sideEnemy, mirrorMask[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                goto finish;
            if (AttemptAttack(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, shooters[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, walkers[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, flyerMask[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, oddMasks[IDX(sideEnemy)]))
                goto finish;
            if (WalkTowardArmy(thisArmy, sideEnemy, traitorArray[IDX(sideEnemy)]))
                goto finish;

            rowLimit[0] = COMBAT_AI_CASTLE_BOUNDARY_ROW_0;
            rowLimit[1] = COMBAT_AI_CASTLE_BOUNDARY_ROW_1;
            rowLimit[2] = COMBAT_AI_CASTLE_BOUNDARY_ROW_2;
            rowLimit[3] = COMBAT_AI_CASTLE_BOUNDARY_ROW_3;
            rowLimit[4] = COMBAT_AI_CASTLE_BOUNDARY_ROW_4;
            rowLimit[5] = COMBAT_AI_CASTLE_BOUNDARY_ROW_5;
            rowLimit[6] = COMBAT_AI_CASTLE_BOUNDARY_ROW_6;
            rowLimit[7] = COMBAT_AI_CASTLE_BOUNDARY_ROW_7;
            rowLimit[8] = COMBAT_AI_CASTLE_BOUNDARY_ROW_8;
            rowIndex = thisArmy->m_hex / ARMY_HEX_COLUMNS;
            if (m_currentSide == COMBAT_ATTACKER_SIDE && m_inCastleCombat != 0
                && thisArmy->m_hex < rowLimit[rowIndex]) {
                targetHex = rowLimit[rowIndex];
                targetCell = &gpCombatManager->m_hexCells[targetHex];
                if (ValidHex(targetHex) && targetCell->m_occupantSide == COMBAT_SIDE_NONE
                    && targetCell->m_blocked == 0) {
                    giNextAction = ACTION_MOVE;
                    giNextActionGridIndex = targetHex;
                    goto finish;
                }
            }
            break;
    }
    giNextAction = ACTION_WAIT;

finish:
    if (giNextAction == ACTION_MOVE && giNextActionGridIndex > 0
        && giNextActionGridIndex < COMBAT_HEX_COUNT
        && gpCombatManager->m_hexCells[giNextActionGridIndex].m_occupantSide == COMBAT_SIDE_NONE) {
        for (dirIndex = COMBAT_DIRECTION_NORTHEAST;
             IDX(dirIndex) < COMBAT_AI_ADJACENT_DIRECTION_COUNT;
             dirIndex++) {
            adjCell = thisArmy->GetAdjacentCellIndex(
                giNextActionGridIndex, dirIndex
            );
            if (adjCell > 0 && adjCell < COMBAT_HEX_COUNT
                && gpCombatManager->m_hexCells[adjCell].m_occupantSide
                       == OppositeCombatSide(m_currentSide)) {
                giNextActionGridIndex = adjCell;
                return;
            }
        }
    }
}

*/

// a02-attempt src/SOURCE/AI.cpp RVA 0x17f42
// build/source-variant-batch/a02-attempt/results.json
// Complete 2 arms; elapsed 1.68744s; input restored.
// Disposition: retain trial 1; final combined native and retail proof.
// trial / bytes / text hash / normalized relocation hash / blocks candidate:retail / exact,size-only,target-shift,flow-kind / graph exact
// 0 / 376 / 2e6b0dc270ff6959 / 1c4378e0e7891da5 / 18:18 / 18,0,0,0 / True
// 1 / 376 / 2e6b0dc270ff6959 / 1c4378e0e7891da5 / 18:18 / 18,0,0,0 / True
/* AXIS actual_array_base
BEFORE
i32 combatManager::AttemptAttack(
    class army* currentArmy, H2_ENUM_PARAM(CombatSide, i32) side, i32 mask
) {
    i32 targetArmy;
    i32 targetHex;

    while (mask != 0) {
        if (currentArmy->m_monsterType == CREATURE_GHOST)
            targetArmy = GetWorstArmy(side, mask);
        else
            targetArmy = GetBestArmy(side, mask);
        if (targetArmy == COMBAT_AI_NO_ARMY)
            return 0;

        currentArmy->m_targetSide = side;
        currentArmy->m_targetIndex = targetArmy;
        targetHex = targetArmy[m_armies[IDX(side)]].m_hex;
        currentArmy->m_moveTargetHex = targetHex;
        if (currentArmy->ValidPath(targetHex, ARMY_PATH_ANY_TARGET_HEX)) {
            giNextAction = ACTION_MOVE;
            giNextActionGridIndex = targetHex;
            return 1;
        }
        if (HAS(targetArmy[m_armies[IDX(side)]].m_monster.flags.abilityFlags,
                MONSTER_ABILITY_FLAG_WIDE)
            != 0) {
            if (targetArmy[m_armies[IDX(side)]].m_facing == ARMY_FACING_LEFT)
                targetHex--;
            else
                targetHex++;
            currentArmy->m_moveTargetHex = targetHex;
            if (currentArmy->ValidPath(targetHex, ARMY_PATH_ANY_TARGET_HEX)) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex = targetHex;
                return 1;
            }
        }
        mask &= ~(1 << targetArmy);
    }
    return 0;
}


AFTER array_then_index
i32 combatManager::AttemptAttack(
    class army* currentArmy, H2_ENUM_PARAM(CombatSide, i32) side, i32 mask
) {
    i32 targetArmy;
    i32 targetHex;

    while (mask != 0) {
        if (currentArmy->m_monsterType == CREATURE_GHOST)
            targetArmy = GetWorstArmy(side, mask);
        else
            targetArmy = GetBestArmy(side, mask);
        if (targetArmy == COMBAT_AI_NO_ARMY)
            return 0;

        currentArmy->m_targetSide = side;
        currentArmy->m_targetIndex = targetArmy;
        targetHex = m_armies[IDX(side)][targetArmy].m_hex;
        currentArmy->m_moveTargetHex = targetHex;
        if (currentArmy->ValidPath(targetHex, ARMY_PATH_ANY_TARGET_HEX)) {
            giNextAction = ACTION_MOVE;
            giNextActionGridIndex = targetHex;
            return 1;
        }
        if (HAS(m_armies[IDX(side)][targetArmy].m_monster.flags.abilityFlags,
                MONSTER_ABILITY_FLAG_WIDE)
            != 0) {
            if (m_armies[IDX(side)][targetArmy].m_facing == ARMY_FACING_LEFT)
                targetHex--;
            else
                targetHex++;
            currentArmy->m_moveTargetHex = targetHex;
            if (currentArmy->ValidPath(targetHex, ARMY_PATH_ANY_TARGET_HEX)) {
                giNextAction = ACTION_MOVE;
                giNextActionGridIndex = targetHex;
                return 1;
            }
        }
        mask &= ~(1 << targetArmy);
    }
    return 0;
}

*/

// a02-front src/SOURCE/AI.cpp RVA 0x181fe
// build/source-variant-batch/a02-front/results.json
// Complete 2 arms; elapsed 1.700671s; input restored.
// Disposition: retain trial 1; final combined native and retail proof.
// trial / bytes / text hash / normalized relocation hash / blocks candidate:retail / exact,size-only,target-shift,flow-kind / graph exact
// 0 / 519 / 2b074256b8d7faeb / 1a560bcc97feb3a5 / 21:21 / 21,0,0,0 / True
// 1 / 519 / 2b074256b8d7faeb / 1a560bcc97feb3a5 / 21:21 / 21,0,0,0 / True
/* AXIS actual_array_base
BEFORE
i32 combatManager::WalkTowardArmyFront(
    class army* currentArmy, H2_ENUM_PARAM(CombatSide, i32) side, i32 mask
) {
    i32 armyIndex;
    i32 frontDelta;
    i32 frontHex;
    i32 oldSpeed;
    i32 H2_UNUSED(pathFound);
    i32 left;
    i32 step;

    currentArmy->m_targetSide = COMBAT_AI_NO_ARMY;
    currentArmy->m_targetIndex = COMBAT_AI_NO_ARMY;
    armyIndex = GetClosestArmy(currentArmy, side, mask);
    if (armyIndex == COMBAT_AI_NO_ARMY)
        return 0;

    frontDelta = SINGLE_HEX_FRONT_OFFSET;
    frontHex = armyIndex[m_armies[IDX(side)]].m_hex;
    if (HAS(armyIndex[m_armies[IDX(side)]].m_monster.flags.abilityFlags,
            MONSTER_ABILITY_FLAG_WIDE)
        != 0)
        frontDelta = WIDE_CREATURE_FRONT_OFFSET;
    frontHex += currentArmy->m_facing == ARMY_FACING_RIGHT ? frontDelta : -frontDelta;
    if (frontHex % ARMY_HEX_COLUMNS == ARMY_HEX_COLUMNS - 1 || frontHex % ARMY_HEX_COLUMNS == 0)
        return WalkTowardArmy(currentArmy, side, mask);

    oldSpeed = currentArmy->m_monster.speed;
    currentArmy->m_monster.speed = COMBAT_AI_UNLIMITED_PATH_SPEED;
    pathFound = gpSearchArray->FindCombatPath(
        currentArmy->m_hex,
        frontHex,
        currentArmy,
        COMBAT_AI_PATH_TO_FRONT,
        0
    );
    currentArmy->m_monster.speed = static_cast<i8>(oldSpeed);
    if (gpSearchArray->m_pathLength > 0) {
        giNextAction = ACTION_MOVE;
        left = currentArmy->m_monster.speed;
        step = gpSearchArray->m_pathLength - 1;
        giNextActionGridIndex = currentArmy->m_hex;
        while (step >= 0 && left != 0) {
            giNextActionGridIndex = currentArmy->GetAdjacentCellIndex(
                giNextActionGridIndex,
                static_cast<CombatHexDirection>(
                    gpSearchArray->m_storage.aiPath.directions[step]
                )
            );
            step--;
            left--;
            if (giNextActionGridIndex > 0 && bIsMoatSlowed[giNextActionGridIndex] != 0)
                left = 0;
        }
        return 1;
    }
    return WalkTowardArmy(currentArmy, side, mask);
}


AFTER array_then_index
i32 combatManager::WalkTowardArmyFront(
    class army* currentArmy, H2_ENUM_PARAM(CombatSide, i32) side, i32 mask
) {
    i32 armyIndex;
    i32 frontDelta;
    i32 frontHex;
    i32 oldSpeed;
    i32 H2_UNUSED(pathFound);
    i32 left;
    i32 step;

    currentArmy->m_targetSide = COMBAT_AI_NO_ARMY;
    currentArmy->m_targetIndex = COMBAT_AI_NO_ARMY;
    armyIndex = GetClosestArmy(currentArmy, side, mask);
    if (armyIndex == COMBAT_AI_NO_ARMY)
        return 0;

    frontDelta = SINGLE_HEX_FRONT_OFFSET;
    frontHex = m_armies[IDX(side)][armyIndex].m_hex;
    if (HAS(m_armies[IDX(side)][armyIndex].m_monster.flags.abilityFlags,
            MONSTER_ABILITY_FLAG_WIDE)
        != 0)
        frontDelta = WIDE_CREATURE_FRONT_OFFSET;
    frontHex += currentArmy->m_facing == ARMY_FACING_RIGHT ? frontDelta : -frontDelta;
    if (frontHex % ARMY_HEX_COLUMNS == ARMY_HEX_COLUMNS - 1 || frontHex % ARMY_HEX_COLUMNS == 0)
        return WalkTowardArmy(currentArmy, side, mask);

    oldSpeed = currentArmy->m_monster.speed;
    currentArmy->m_monster.speed = COMBAT_AI_UNLIMITED_PATH_SPEED;
    pathFound = gpSearchArray->FindCombatPath(
        currentArmy->m_hex,
        frontHex,
        currentArmy,
        COMBAT_AI_PATH_TO_FRONT,
        0
    );
    currentArmy->m_monster.speed = static_cast<i8>(oldSpeed);
    if (gpSearchArray->m_pathLength > 0) {
        giNextAction = ACTION_MOVE;
        left = currentArmy->m_monster.speed;
        step = gpSearchArray->m_pathLength - 1;
        giNextActionGridIndex = currentArmy->m_hex;
        while (step >= 0 && left != 0) {
            giNextActionGridIndex = currentArmy->GetAdjacentCellIndex(
                giNextActionGridIndex,
                static_cast<CombatHexDirection>(
                    gpSearchArray->m_storage.aiPath.directions[step]
                )
            );
            step--;
            left--;
            if (giNextActionGridIndex > 0 && bIsMoatSlowed[giNextActionGridIndex] != 0)
                left = 0;
        }
        return 1;
    }
    return WalkTowardArmy(currentArmy, side, mask);
}

*/

// g75-ai-dead src/SOURCE/AI.cpp RVA 0x1628b
// build/source-variant-batch/g75-ai-dead/results.json
// Complete 2 arms; elapsed 1.831934s; input restored.
// Disposition: original retained; all tested nonoriginal forms change native bytes. No impossibility claim for other parents/states.
// trial / bytes / text hash / normalized relocation hash / blocks candidate:retail / exact,size-only,target-shift,flow-kind / graph exact
// 0 / 4000 / 147ff2d01b0462bb / 4d24cf000b06cdfa / 190:190 / 190,0,0,0 / True
// 1 / 3984 / 26c3a013de6a0d11 / 9fc250006ae09353 / 187:190 / 78,1,91,17 / False
/* AXIS constant_false_branch
BEFORE
            if (COMBAT_AI_ATTACK_NONE)
                goto finish;


AFTER remove

*/

// r20-retreat src/SOURCE/AI.cpp RVA 0x15a60
// build/source-variant-batch/r20-retreat/results.json
// Complete 3 arms; elapsed 2.252235s; input restored.
// Disposition: original retained; all tested nonoriginal forms change native bytes. No impossibility claim for other parents/states.
// trial / bytes / text hash / normalized relocation hash / blocks candidate:retail / exact,size-only,target-shift,flow-kind / graph exact
// 0 / 2091 / 6b2da28ff8607e3f / edc748678c3c9837 / 77:77 / 77,0,0,0 / True
// 1 / 2091 / 69e0bfaef06cfb52 / edc748678c3c9837 / 77:77 / 77,0,0,0 / True
// 2 / 2091 / 69e0bfaef06cfb52 / edc748678c3c9837 / 77:77 / 77,0,0,0 / True
/* AXIS artifact_counter_owner
BEFORE
i32 combatManager::AICheckRetreat(void) {
    if (m_combatTowns[IDX(m_currentSide)] != NULL)
        return 0;
    if (m_heroes[IDX(m_currentSide)] == NULL)
        return 0;
    if (m_heroes[IDX(m_currentSide)]->m_isCaptain != 0)
        return 0;
    if (gpGame->m_mapHeader.victoryCondition == MAP_VICTORY_DEFEAT_HERO
        && gpGame->m_mapHeader.victoryConditionValue == m_heroes[IDX(m_currentSide)]->m_id)
        return 0;
    if (gpGame->m_players[IDX(m_heroes[IDX(m_currentSide)]->m_owner)].m_townCount == 0
        && gpGame->m_players[IDX(m_heroes[IDX(m_currentSide)]->m_owner)].m_heroCount
               == COMBAT_AI_LAST_HERO_COUNT)
        return 0;

    float chance;
    hero heroRec;
    i32 force[COMBAT_SIDE_COUNT];
    hero* sideHero;
    armyGroup* armies;
    i32 artifactTotals[COMBAT_SIDE_COUNT];
    armyGroup retreatArmy;
    i32 armyIndex;
    float bonus;
    float retreatRatio;
    i32 artifactValue;
    i32 sideNum;
    i32 groupIndex;

    for (sideNum = 0; sideNum < COMBAT_SIDE_COUNT; sideNum++) {
        if (m_heroes[sideNum] != NULL) {
            heroRec = *m_heroes[sideNum];
            sideHero = &heroRec;
            armies = &sideHero->m_army;
        } else {
            armies = &retreatArmy;
            sideHero = NULL;
        }

        for (armyIndex = 0; armyIndex < COMBAT_AI_GROUP_SLOT_COUNT; armyIndex++) {
            armies->m_creatureTypes[armyIndex] = -1;
            armies->m_creatureCounts[armyIndex] = 0;
        }

        groupIndex = 0;
        for (armyIndex = 0; armyIndex < COMBAT_AI_ARMY_SLOT_COUNT; armyIndex++) {
            if ((m_armies[sideNum] + armyIndex)->IsAlive()) {
                armies->m_creatureTypes[groupIndex] = static_cast<i8>(
                    (m_armies[sideNum] + armyIndex)
                        ->m_monsterType
                );
                if (HAS((m_armies[sideNum] + armyIndex)
                            ->m_monster.flags.abilityFlags,
                        MONSTER_ABILITY_FLAG_FULL_AI_QUANTITY)
                    != 0) {
                    armies->m_creatureCounts[groupIndex] = static_cast<i16>(
                        (m_armies[sideNum] + armyIndex)
                            ->m_quantity
                    );
                } else {
                    armies->m_creatureCounts[groupIndex] =
                        static_cast<i16>(static_cast<i32>(
                            (m_armies[sideNum] + armyIndex)
                                ->m_quantity
                            * COMBAT_AI_QUANTITY_ESTIMATE
                        ));
                }
                groupIndex++;
            }
            if (groupIndex >= COMBAT_AI_GROUP_SLOT_COUNT)
                armyIndex = COMBAT_AI_GROUP_SCAN_DONE;
        }

        force[sideNum] =
            gpPhilAI
                ->FightValueOfStack(armies, sideHero, COMBAT_AI_FIGHT_VALUE_MODE, 0, 0, 0);
        if (m_combatTowns[sideNum] != NULL)
            force[sideNum] =
                static_cast<i32>(force[sideNum] * COMBAT_AI_TOWN_STRENGTH_MODIFIER);

        artifactTotals[sideNum] = 0;
        if (sideHero != NULL) {
            for (armyIndex = 0; armyIndex < AI_BATTLE_ARTIFACT_SLOT_COUNT; armyIndex++) {
                if (sideHero->m_artifacts[armyIndex] >= ARTIFACT_VALID_BEGIN
                    && IDX(sideHero->m_artifacts[armyIndex])
                           < AI_BATTLE_BASE_ARTIFACT_LIMIT) {
                    artifactTotals[sideNum] +=
                        gArtifactBaseRV[IDX(sideHero->m_artifacts[armyIndex])];
                }
            }
        }
    }

    force[IDX(OppositeCombatSide(m_currentSide))] *= COMBAT_AI_TOWN_STRENGTH_MODIFIER;
    artifactValue = artifactTotals[IDX(m_currentSide)];
    if (artifactTotals[IDX(m_currentSide)] < COMBAT_AI_MIN_ARTIFACT_VALUE
        && m_heroes[IDX(m_currentSide)]->m_experience < COMBAT_AI_MIN_HERO_EXPERIENCE)
        return 0;

    chance = COMBAT_AI_BASE_RETREAT_CHANCE;
    if (artifactValue > COMBAT_AI_HIGH_ARTIFACT_VALUE)
        chance = static_cast<float>(chance + COMBAT_AI_HIGH_ARTIFACT_RETREAT_BONUS);
    else if (artifactValue > COMBAT_AI_MEDIUM_ARTIFACT_VALUE)
        chance =
            static_cast<float>(chance + COMBAT_AI_MEDIUM_ARTIFACT_RETREAT_BONUS);
    else if (artifactValue > 0)
        chance = static_cast<float>(chance + COMBAT_AI_LOW_ARTIFACT_RETREAT_BONUS);

    if (force[IDX(m_currentSide)] > COMBAT_AI_STRENGTH_40000)
        chance -= static_cast<float>(force[IDX(m_currentSide)] / COMBAT_AI_STRENGTH_20000);
    else if (force[IDX(m_currentSide)] > COMBAT_AI_STRENGTH_30000)
        chance =
            static_cast<float>(chance - COMBAT_AI_STRENGTH_30000_RETREAT_PENALTY);
    else if (force[IDX(m_currentSide)] > COMBAT_AI_STRENGTH_15000)
        chance =
            static_cast<float>(chance - COMBAT_AI_STRENGTH_15000_RETREAT_PENALTY);
    else if (force[IDX(m_currentSide)] > COMBAT_AI_STRENGTH_5000)
        chance =
            static_cast<float>(chance - COMBAT_AI_STRENGTH_5000_RETREAT_PENALTY);
    else if (force[IDX(m_currentSide)] > COMBAT_AI_STRENGTH_2500)
        chance =
            static_cast<float>(chance - COMBAT_AI_STRENGTH_2500_RETREAT_PENALTY);

    chance = static_cast<float>(
        chance
        - (COMBAT_AI_MAX_DIFFICULTY - IDX(gpGame->m_difficulty))
              * COMBAT_AI_DIFFICULTY_RETREAT_STEP
    );
    bonus =
        static_cast<float>(m_heroes[IDX(m_currentSide)]->m_experience / COMBAT_AI_EXPERIENCE_DIVISOR);
    if (bonus > COMBAT_AI_MAX_EXPERIENCE_BONUS_COMPARE)
        bonus = COMBAT_AI_MAX_EXPERIENCE_BONUS;
    chance += bonus;
    if (m_currentSide == COMBAT_ATTACKER_SIDE)
        chance = static_cast<float>(chance - COMBAT_AI_ATTACKER_RETREAT_PENALTY);
    if (chance > COMBAT_AI_MAX_RETREAT_CHANCE_COMPARE)
        chance = COMBAT_AI_MAX_RETREAT_CHANCE;

    retreatRatio = static_cast<float>(force[IDX(m_currentSide)])
                   / static_cast<double>(
                       force[IDX(COMBAT_ATTACKER_SIDE)] + force[IDX(COMBAT_DEFENDER_SIDE)]
                   );
    if (retreatRatio < chance)
        return 1;
    return 0;
}


AFTER local_counter
i32 combatManager::AICheckRetreat(void) {
    if (m_combatTowns[IDX(m_currentSide)] != NULL)
        return 0;
    if (m_heroes[IDX(m_currentSide)] == NULL)
        return 0;
    if (m_heroes[IDX(m_currentSide)]->m_isCaptain != 0)
        return 0;
    if (gpGame->m_mapHeader.victoryCondition == MAP_VICTORY_DEFEAT_HERO
        && gpGame->m_mapHeader.victoryConditionValue == m_heroes[IDX(m_currentSide)]->m_id)
        return 0;
    if (gpGame->m_players[IDX(m_heroes[IDX(m_currentSide)]->m_owner)].m_townCount == 0
        && gpGame->m_players[IDX(m_heroes[IDX(m_currentSide)]->m_owner)].m_heroCount
               == COMBAT_AI_LAST_HERO_COUNT)
        return 0;

    float chance;
    hero heroRec;
    i32 force[COMBAT_SIDE_COUNT];
    hero* sideHero;
    armyGroup* armies;
    i32 artifactTotals[COMBAT_SIDE_COUNT];
    armyGroup retreatArmy;
    i32 armyIndex;
    float bonus;
    float retreatRatio;
    i32 artifactValue;
    i32 artifactIndex;
    i32 sideNum;
    i32 groupIndex;

    for (sideNum = 0; sideNum < COMBAT_SIDE_COUNT; sideNum++) {
        if (m_heroes[sideNum] != NULL) {
            heroRec = *m_heroes[sideNum];
            sideHero = &heroRec;
            armies = &sideHero->m_army;
        } else {
            armies = &retreatArmy;
            sideHero = NULL;
        }

        for (armyIndex = 0; armyIndex < COMBAT_AI_GROUP_SLOT_COUNT; armyIndex++) {
            armies->m_creatureTypes[armyIndex] = -1;
            armies->m_creatureCounts[armyIndex] = 0;
        }

        groupIndex = 0;
        for (armyIndex = 0; armyIndex < COMBAT_AI_ARMY_SLOT_COUNT; armyIndex++) {
            if ((m_armies[sideNum] + armyIndex)->IsAlive()) {
                armies->m_creatureTypes[groupIndex] = static_cast<i8>(
                    (m_armies[sideNum] + armyIndex)
                        ->m_monsterType
                );
                if (HAS((m_armies[sideNum] + armyIndex)
                            ->m_monster.flags.abilityFlags,
                        MONSTER_ABILITY_FLAG_FULL_AI_QUANTITY)
                    != 0) {
                    armies->m_creatureCounts[groupIndex] = static_cast<i16>(
                        (m_armies[sideNum] + armyIndex)
                            ->m_quantity
                    );
                } else {
                    armies->m_creatureCounts[groupIndex] =
                        static_cast<i16>(static_cast<i32>(
                            (m_armies[sideNum] + armyIndex)
                                ->m_quantity
                            * COMBAT_AI_QUANTITY_ESTIMATE
                        ));
                }
                groupIndex++;
            }
            if (groupIndex >= COMBAT_AI_GROUP_SLOT_COUNT)
                armyIndex = COMBAT_AI_GROUP_SCAN_DONE;
        }

        force[sideNum] =
            gpPhilAI
                ->FightValueOfStack(armies, sideHero, COMBAT_AI_FIGHT_VALUE_MODE, 0, 0, 0);
        if (m_combatTowns[sideNum] != NULL)
            force[sideNum] =
                static_cast<i32>(force[sideNum] * COMBAT_AI_TOWN_STRENGTH_MODIFIER);

        artifactTotals[sideNum] = 0;
        if (sideHero != NULL) {
            for (artifactIndex = 0; artifactIndex < AI_BATTLE_ARTIFACT_SLOT_COUNT; artifactIndex++) {
                if (sideHero->m_artifacts[artifactIndex] >= ARTIFACT_VALID_BEGIN
                    && IDX(sideHero->m_artifacts[artifactIndex])
                           < AI_BATTLE_BASE_ARTIFACT_LIMIT) {
                    artifactTotals[sideNum] +=
                        gArtifactBaseRV[IDX(sideHero->m_artifacts[artifactIndex])];
                }
            }
        }
    }

    force[IDX(OppositeCombatSide(m_currentSide))] *= COMBAT_AI_TOWN_STRENGTH_MODIFIER;
    artifactValue = artifactTotals[IDX(m_currentSide)];
    if (artifactTotals[IDX(m_currentSide)] < COMBAT_AI_MIN_ARTIFACT_VALUE
        && m_heroes[IDX(m_currentSide)]->m_experience < COMBAT_AI_MIN_HERO_EXPERIENCE)
        return 0;

    chance = COMBAT_AI_BASE_RETREAT_CHANCE;
    if (artifactValue > COMBAT_AI_HIGH_ARTIFACT_VALUE)
        chance = static_cast<float>(chance + COMBAT_AI_HIGH_ARTIFACT_RETREAT_BONUS);
    else if (artifactValue > COMBAT_AI_MEDIUM_ARTIFACT_VALUE)
        chance =
            static_cast<float>(chance + COMBAT_AI_MEDIUM_ARTIFACT_RETREAT_BONUS);
    else if (artifactValue > 0)
        chance = static_cast<float>(chance + COMBAT_AI_LOW_ARTIFACT_RETREAT_BONUS);

    if (force[IDX(m_currentSide)] > COMBAT_AI_STRENGTH_40000)
        chance -= static_cast<float>(force[IDX(m_currentSide)] / COMBAT_AI_STRENGTH_20000);
    else if (force[IDX(m_currentSide)] > COMBAT_AI_STRENGTH_30000)
        chance =
            static_cast<float>(chance - COMBAT_AI_STRENGTH_30000_RETREAT_PENALTY);
    else if (force[IDX(m_currentSide)] > COMBAT_AI_STRENGTH_15000)
        chance =
            static_cast<float>(chance - COMBAT_AI_STRENGTH_15000_RETREAT_PENALTY);
    else if (force[IDX(m_currentSide)] > COMBAT_AI_STRENGTH_5000)
        chance =
            static_cast<float>(chance - COMBAT_AI_STRENGTH_5000_RETREAT_PENALTY);
    else if (force[IDX(m_currentSide)] > COMBAT_AI_STRENGTH_2500)
        chance =
            static_cast<float>(chance - COMBAT_AI_STRENGTH_2500_RETREAT_PENALTY);

    chance = static_cast<float>(
        chance
        - (COMBAT_AI_MAX_DIFFICULTY - IDX(gpGame->m_difficulty))
              * COMBAT_AI_DIFFICULTY_RETREAT_STEP
    );
    bonus =
        static_cast<float>(m_heroes[IDX(m_currentSide)]->m_experience / COMBAT_AI_EXPERIENCE_DIVISOR);
    if (bonus > COMBAT_AI_MAX_EXPERIENCE_BONUS_COMPARE)
        bonus = COMBAT_AI_MAX_EXPERIENCE_BONUS;
    chance += bonus;
    if (m_currentSide == COMBAT_ATTACKER_SIDE)
        chance = static_cast<float>(chance - COMBAT_AI_ATTACKER_RETREAT_PENALTY);
    if (chance > COMBAT_AI_MAX_RETREAT_CHANCE_COMPARE)
        chance = COMBAT_AI_MAX_RETREAT_CHANCE;

    retreatRatio = static_cast<float>(force[IDX(m_currentSide)])
                   / static_cast<double>(
                       force[IDX(COMBAT_ATTACKER_SIDE)] + force[IDX(COMBAT_DEFENDER_SIDE)]
                   );
    if (retreatRatio < chance)
        return 1;
    return 0;
}


AFTER scoped_counter
i32 combatManager::AICheckRetreat(void) {
    if (m_combatTowns[IDX(m_currentSide)] != NULL)
        return 0;
    if (m_heroes[IDX(m_currentSide)] == NULL)
        return 0;
    if (m_heroes[IDX(m_currentSide)]->m_isCaptain != 0)
        return 0;
    if (gpGame->m_mapHeader.victoryCondition == MAP_VICTORY_DEFEAT_HERO
        && gpGame->m_mapHeader.victoryConditionValue == m_heroes[IDX(m_currentSide)]->m_id)
        return 0;
    if (gpGame->m_players[IDX(m_heroes[IDX(m_currentSide)]->m_owner)].m_townCount == 0
        && gpGame->m_players[IDX(m_heroes[IDX(m_currentSide)]->m_owner)].m_heroCount
               == COMBAT_AI_LAST_HERO_COUNT)
        return 0;

    float chance;
    hero heroRec;
    i32 force[COMBAT_SIDE_COUNT];
    hero* sideHero;
    armyGroup* armies;
    i32 artifactTotals[COMBAT_SIDE_COUNT];
    armyGroup retreatArmy;
    i32 armyIndex;
    float bonus;
    float retreatRatio;
    i32 artifactValue;
    i32 sideNum;
    i32 groupIndex;

    for (sideNum = 0; sideNum < COMBAT_SIDE_COUNT; sideNum++) {
        if (m_heroes[sideNum] != NULL) {
            heroRec = *m_heroes[sideNum];
            sideHero = &heroRec;
            armies = &sideHero->m_army;
        } else {
            armies = &retreatArmy;
            sideHero = NULL;
        }

        for (armyIndex = 0; armyIndex < COMBAT_AI_GROUP_SLOT_COUNT; armyIndex++) {
            armies->m_creatureTypes[armyIndex] = -1;
            armies->m_creatureCounts[armyIndex] = 0;
        }

        groupIndex = 0;
        for (armyIndex = 0; armyIndex < COMBAT_AI_ARMY_SLOT_COUNT; armyIndex++) {
            if ((m_armies[sideNum] + armyIndex)->IsAlive()) {
                armies->m_creatureTypes[groupIndex] = static_cast<i8>(
                    (m_armies[sideNum] + armyIndex)
                        ->m_monsterType
                );
                if (HAS((m_armies[sideNum] + armyIndex)
                            ->m_monster.flags.abilityFlags,
                        MONSTER_ABILITY_FLAG_FULL_AI_QUANTITY)
                    != 0) {
                    armies->m_creatureCounts[groupIndex] = static_cast<i16>(
                        (m_armies[sideNum] + armyIndex)
                            ->m_quantity
                    );
                } else {
                    armies->m_creatureCounts[groupIndex] =
                        static_cast<i16>(static_cast<i32>(
                            (m_armies[sideNum] + armyIndex)
                                ->m_quantity
                            * COMBAT_AI_QUANTITY_ESTIMATE
                        ));
                }
                groupIndex++;
            }
            if (groupIndex >= COMBAT_AI_GROUP_SLOT_COUNT)
                armyIndex = COMBAT_AI_GROUP_SCAN_DONE;
        }

        force[sideNum] =
            gpPhilAI
                ->FightValueOfStack(armies, sideHero, COMBAT_AI_FIGHT_VALUE_MODE, 0, 0, 0);
        if (m_combatTowns[sideNum] != NULL)
            force[sideNum] =
                static_cast<i32>(force[sideNum] * COMBAT_AI_TOWN_STRENGTH_MODIFIER);

        artifactTotals[sideNum] = 0;
        if (sideHero != NULL) {
            i32 artifactIndex;
            for (artifactIndex = 0; artifactIndex < AI_BATTLE_ARTIFACT_SLOT_COUNT; artifactIndex++) {
                if (sideHero->m_artifacts[artifactIndex] >= ARTIFACT_VALID_BEGIN
                    && IDX(sideHero->m_artifacts[artifactIndex])
                           < AI_BATTLE_BASE_ARTIFACT_LIMIT) {
                    artifactTotals[sideNum] +=
                        gArtifactBaseRV[IDX(sideHero->m_artifacts[artifactIndex])];
                }
            }
        }
    }

    force[IDX(OppositeCombatSide(m_currentSide))] *= COMBAT_AI_TOWN_STRENGTH_MODIFIER;
    artifactValue = artifactTotals[IDX(m_currentSide)];
    if (artifactTotals[IDX(m_currentSide)] < COMBAT_AI_MIN_ARTIFACT_VALUE
        && m_heroes[IDX(m_currentSide)]->m_experience < COMBAT_AI_MIN_HERO_EXPERIENCE)
        return 0;

    chance = COMBAT_AI_BASE_RETREAT_CHANCE;
    if (artifactValue > COMBAT_AI_HIGH_ARTIFACT_VALUE)
        chance = static_cast<float>(chance + COMBAT_AI_HIGH_ARTIFACT_RETREAT_BONUS);
    else if (artifactValue > COMBAT_AI_MEDIUM_ARTIFACT_VALUE)
        chance =
            static_cast<float>(chance + COMBAT_AI_MEDIUM_ARTIFACT_RETREAT_BONUS);
    else if (artifactValue > 0)
        chance = static_cast<float>(chance + COMBAT_AI_LOW_ARTIFACT_RETREAT_BONUS);

    if (force[IDX(m_currentSide)] > COMBAT_AI_STRENGTH_40000)
        chance -= static_cast<float>(force[IDX(m_currentSide)] / COMBAT_AI_STRENGTH_20000);
    else if (force[IDX(m_currentSide)] > COMBAT_AI_STRENGTH_30000)
        chance =
            static_cast<float>(chance - COMBAT_AI_STRENGTH_30000_RETREAT_PENALTY);
    else if (force[IDX(m_currentSide)] > COMBAT_AI_STRENGTH_15000)
        chance =
            static_cast<float>(chance - COMBAT_AI_STRENGTH_15000_RETREAT_PENALTY);
    else if (force[IDX(m_currentSide)] > COMBAT_AI_STRENGTH_5000)
        chance =
            static_cast<float>(chance - COMBAT_AI_STRENGTH_5000_RETREAT_PENALTY);
    else if (force[IDX(m_currentSide)] > COMBAT_AI_STRENGTH_2500)
        chance =
            static_cast<float>(chance - COMBAT_AI_STRENGTH_2500_RETREAT_PENALTY);

    chance = static_cast<float>(
        chance
        - (COMBAT_AI_MAX_DIFFICULTY - IDX(gpGame->m_difficulty))
              * COMBAT_AI_DIFFICULTY_RETREAT_STEP
    );
    bonus =
        static_cast<float>(m_heroes[IDX(m_currentSide)]->m_experience / COMBAT_AI_EXPERIENCE_DIVISOR);
    if (bonus > COMBAT_AI_MAX_EXPERIENCE_BONUS_COMPARE)
        bonus = COMBAT_AI_MAX_EXPERIENCE_BONUS;
    chance += bonus;
    if (m_currentSide == COMBAT_ATTACKER_SIDE)
        chance = static_cast<float>(chance - COMBAT_AI_ATTACKER_RETREAT_PENALTY);
    if (chance > COMBAT_AI_MAX_RETREAT_CHANCE_COMPARE)
        chance = COMBAT_AI_MAX_RETREAT_CHANCE;

    retreatRatio = static_cast<float>(force[IDX(m_currentSide)])
                   / static_cast<double>(
                       force[IDX(COMBAT_ATTACKER_SIDE)] + force[IDX(COMBAT_DEFENDER_SIDE)]
                   );
    if (retreatRatio < chance)
        return 1;
    return 0;
}

*/
