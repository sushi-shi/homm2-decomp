// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\AI.OBJ   from: (directly linked into exe)
// functions: 19   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <string.h>
#include <SOURCE/CMBTMGR.h>
#include <SOURCE/KB.h>
#include <SOURCE/PATH.h>
#include <SOURCE/PHILAI.h>
#include <SOURCE/SPELLS.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/armyGroup.h>
#include <SOURCE/combatManager.h>
#include <SOURCE/game.h>
#include <SOURCE/hero.h>
#include <SOURCE/philAI.h>
#include <SOURCE/searchArray.h>
#include <SOURCE/town.h>

// @semantic: semantics, CFG, the 0x168 frame/local slots, and all 29/29
// effective relocation targets agree. The first code difference is at +0x8ad:
// ours emits `fcom retreatChance; fstp retreatThreshold`, while retail emits
// `fst retreatThreshold; fcomp retreatChance`. Assignment-in-condition, split
// assignment, reversed relation, scalar-SIB lvalues, and four bounded TU-state
// probes did not improve it. Anonymous floating constants differ only in
// compiler-counter names; retail also delinks __adjust_fdiv as an interior
// iLeftRightSave alias. Revisit after a material AI TU/header-state change.
VA(0x004c0790, 0x8d7)
i32 combatManager::AICheckRetreat(void)
{
    if (m_combatTowns[m_currentSide] != 0)
        return 0;
    if (m_heroes[m_currentSide] == 0)
        return 0;
    if (m_heroes[m_currentSide]->m_isCaptain != 0)
        return 0;
    if (gpGame->m_mapHeader.victoryCondition == COMBAT_AI_DEFEAT_HERO_VICTORY &&
        static_cast<u8>(m_heroes[m_currentSide]->m_id) ==
            gpGame->m_mapHeader.victoryConditionValue)
        return 0;
    if (gpGame->m_players[m_heroes[m_currentSide]->m_owner].m_townCount == 0 &&
        gpGame->m_players[m_heroes[m_currentSide]->m_owner].m_heroCount ==
            COMBAT_AI_LAST_HERO_COUNT)
        return 0;

    hero retreatHero8;
    armyGroup retreatArmy6;
    armyGroup *armyGroupPtr1;
    hero *heroPtr9;
    i32 side9;
    i32 armyIndex36;
    i32 groupCount8;
    i32 strengths1[COMBAT_AI_SIDE_COUNT];
    i32 artifactValues[COMBAT_AI_SIDE_COUNT];
    float experienceBonus17;
    i32 artifactValue15;
    float retreatChance7;
    float retreatThreshold7;

    for (side9 = 0; side9 < COMBAT_AI_SIDE_COUNT; side9++) {
        if (m_heroes[side9] != 0) {
            retreatHero8 = *m_heroes[side9];
            heroPtr9 = &retreatHero8;
            armyGroupPtr1 = &heroPtr9->m_army;
        } else {
            armyGroupPtr1 = &retreatArmy6;
            heroPtr9 = 0;
        }

        for (armyIndex36 = 0;
             armyIndex36 < COMBAT_AI_GROUP_SLOT_COUNT;
             armyIndex36++) {
            armyGroupPtr1->m_creatureTypes[armyIndex36] = -1;
            armyGroupPtr1->m_creatureCounts[armyIndex36] = 0;
        }

        groupCount8 = 0;
        for (armyIndex36 = 0;
             armyIndex36 < COMBAT_AI_ARMY_SLOT_COUNT;
             armyIndex36++) {
            if ((m_armies[0] +
                 side9 * COMBAT_ARMY_STORAGE_SLOT_COUNT +
                 armyIndex36)->IsAlive()) {
                armyGroupPtr1->m_creatureTypes[groupCount8] =
                    static_cast<i8>(
                        (m_armies[0] +
                         side9 * COMBAT_ARMY_STORAGE_SLOT_COUNT +
                         armyIndex36)->m_monsterType);
                if (((m_armies[0] +
                      side9 * COMBAT_ARMY_STORAGE_SLOT_COUNT +
                      armyIndex36)->m_monster.flags.abilityFlags &
                     MONSTER_ABILITY_FLAG_FULL_AI_QUANTITY) != 0) {
                    armyGroupPtr1->m_creatureCounts[groupCount8] =
                        static_cast<i16>(
                            (m_armies[0] +
                             side9 * COMBAT_ARMY_STORAGE_SLOT_COUNT +
                             armyIndex36)->m_quantity);
                } else {
                    armyGroupPtr1->m_creatureCounts[groupCount8] = static_cast<i16>(
                        static_cast<i32>(
                            (m_armies[0] +
                             side9 * COMBAT_ARMY_STORAGE_SLOT_COUNT +
                             armyIndex36)->m_quantity *
                            COMBAT_AI_QUANTITY_ESTIMATE));
                }
                groupCount8++;
            }
            if (groupCount8 >= COMBAT_AI_GROUP_SLOT_COUNT)
                armyIndex36 = COMBAT_AI_GROUP_SCAN_DONE;
        }

        strengths1[side9] =
            gpPhilAI->FightValueOfStack(armyGroupPtr1, heroPtr9,
                                       COMBAT_AI_FIGHT_VALUE_MODE, 0, 0, 0);
        if (m_combatTowns[side9] != 0)
            strengths1[side9] = static_cast<i32>(
                strengths1[side9] * COMBAT_AI_TOWN_STRENGTH_MODIFIER);

        artifactValues[side9] = 0;
        if (heroPtr9 != 0) {
            for (armyIndex36 = 0;
                 armyIndex36 < AI_BATTLE_ARTIFACT_SLOT_COUNT;
                 armyIndex36++) {
                if (heroPtr9->m_artifacts[armyIndex36] >= 0 &&
                    heroPtr9->m_artifacts[armyIndex36] <
                        AI_BATTLE_BASE_ARTIFACT_LIMIT) {
                    artifactValues[side9] +=
                        gArtifactBaseRV[heroPtr9->m_artifacts[armyIndex36]];
                }
            }
        }
    }

    strengths1[COMBAT_DEFENDER_SIDE - m_currentSide] =
        static_cast<i32>(strengths1[COMBAT_DEFENDER_SIDE - m_currentSide] *
                         COMBAT_AI_TOWN_STRENGTH_MODIFIER);
    artifactValue15 = artifactValues[m_currentSide];
    if (artifactValues[m_currentSide] < COMBAT_AI_MIN_ARTIFACT_VALUE &&
        m_heroes[m_currentSide]->m_experience < COMBAT_AI_MIN_HERO_EXPERIENCE)
        return 0;

    retreatChance7 = COMBAT_AI_BASE_RETREAT_CHANCE;
    if (artifactValue15 > COMBAT_AI_HIGH_ARTIFACT_VALUE)
        retreatChance7 = static_cast<float>(
            retreatChance7 + COMBAT_AI_HIGH_ARTIFACT_RETREAT_BONUS);
    else if (artifactValue15 > COMBAT_AI_MEDIUM_ARTIFACT_VALUE)
        retreatChance7 = static_cast<float>(
            retreatChance7 + COMBAT_AI_MEDIUM_ARTIFACT_RETREAT_BONUS);
    else if (artifactValue15 > 0)
        retreatChance7 = static_cast<float>(
            retreatChance7 + COMBAT_AI_LOW_ARTIFACT_RETREAT_BONUS);

    if (strengths1[m_currentSide] > COMBAT_AI_STRENGTH_40000)
        retreatChance7 -= static_cast<float>(
            strengths1[m_currentSide] / COMBAT_AI_STRENGTH_20000);
    else if (strengths1[m_currentSide] > COMBAT_AI_STRENGTH_30000)
        retreatChance7 = static_cast<float>(
            retreatChance7 - COMBAT_AI_STRENGTH_30000_RETREAT_PENALTY);
    else if (strengths1[m_currentSide] > COMBAT_AI_STRENGTH_15000)
        retreatChance7 = static_cast<float>(
            retreatChance7 - COMBAT_AI_STRENGTH_15000_RETREAT_PENALTY);
    else if (strengths1[m_currentSide] > COMBAT_AI_STRENGTH_5000)
        retreatChance7 = static_cast<float>(
            retreatChance7 - COMBAT_AI_STRENGTH_5000_RETREAT_PENALTY);
    else if (strengths1[m_currentSide] > COMBAT_AI_STRENGTH_2500)
        retreatChance7 = static_cast<float>(
            retreatChance7 - COMBAT_AI_STRENGTH_2500_RETREAT_PENALTY);

    retreatChance7 = static_cast<float>(
        retreatChance7 -
        (COMBAT_AI_MAX_DIFFICULTY - gpGame->m_difficulty) *
            COMBAT_AI_DIFFICULTY_RETREAT_STEP);
    experienceBonus17 = static_cast<float>(
        m_heroes[m_currentSide]->m_experience / COMBAT_AI_EXPERIENCE_DIVISOR);
    if (experienceBonus17 > COMBAT_AI_MAX_EXPERIENCE_BONUS_COMPARE)
        experienceBonus17 = COMBAT_AI_MAX_EXPERIENCE_BONUS;
    retreatChance7 += experienceBonus17;
    if (m_currentSide == COMBAT_ATTACKER_SIDE)
        retreatChance7 = static_cast<float>(
            retreatChance7 - COMBAT_AI_ATTACKER_RETREAT_PENALTY);
    if (retreatChance7 > COMBAT_AI_MAX_RETREAT_CHANCE_COMPARE)
        retreatChance7 = COMBAT_AI_MAX_RETREAT_CHANCE;

    if (retreatChance7 >
        (retreatThreshold7 = static_cast<float>(strengths1[m_currentSide]) /
                             (strengths1[COMBAT_ATTACKER_SIDE] +
                              strengths1[COMBAT_DEFENDER_SIDE])))
        return 1;
    return 0;
}

// @semantic: the complete 0xb4 frame, local slots, behavior, and all 92/92
// ordered relocation targets agree. Both sides have 923 non-jump instructions;
// remaining non-jump differences are operand order at +0x25c in the five-mask
// OR and +0x5bf in a strength comparison. Retail has 27 additional five-byte
// /Ob1 continuation jumps. Right-nesting the OR was best; left association and
// OR-chaining the independent AttemptAttack guards regressed. Typed flattened
// army indexing fixed six address blocks. Four bounded TU-state probes did not
// improve the canonical score. Revisit after a material AI TU/header change.
VA(0x004c1067, 0x129c)
void combatManager::DoCompAI(i32)
{
    u32l shooterStrengths37[COMBAT_AI_SIDE_COUNT];
    i32 enemyStronger3;
    i32 attackType3;
    u32 shooterMasks6[COMBAT_AI_SIDE_COUNT];
    u32 traitorMasks13[COMBAT_AI_SIDE_COUNT];
    u32 walkerMasks15[COMBAT_AI_SIDE_COUNT];
    i32 enemySide12;
    u32l totalArmyStrength11;
    i32 adjacentDirection6;
    army *currentArmy9;
    i32 targetArmy16;
    combatManager *combat2;
    i32 fifthArmyStrength5;
    u32 mirrorMasks7[COMBAT_AI_SIDE_COUNT];
    u32l enemyShooterStrength18;
    u32 flyerMasks14[COMBAT_AI_SIDE_COUNT];
    u32 outOfItMasks15[COMBAT_AI_SIDE_COUNT];
    u32l currentShooterStrength5;
    i32 archerLevel18;
    i32 strongEnough16;
    i32 numArchers6;
    i32 goldenBowBonus;
    i32 towerStrength4;
    town *castle2;
    u8 castleBoundary27[COMBAT_AI_CASTLE_BOUNDARY_COUNT];
    i32 extraArchers29;
    hexcell *castleCell5;
    i32 castleRow29;
    i32 adjacentHex8;
    u32 castleTargetHex2;

    m_limitCreature = 0;
    currentArmy9 = m_currentArmyIndex + m_armies[m_currentArmySide];
    attackType3 = COMBAT_AI_ATTACK_NONE;
    enemySide12 = COMBAT_DEFENDER_SIDE - m_currentSide;

    mirrorMasks7[m_currentSide] = GetMirrorImageMask(m_currentSide);
    mirrorMasks7[enemySide12] = GetMirrorImageMask(enemySide12);
    shooterMasks6[m_currentSide] = GetShooterMask(m_currentSide);
    shooterMasks6[enemySide12] = GetShooterMask(enemySide12);
    flyerMasks14[m_currentSide] = GetFlyerMask(m_currentSide);
    flyerMasks14[enemySide12] = GetFlyerMask(enemySide12);
    walkerMasks15[m_currentSide] = GetWalkerMask(m_currentSide);
    walkerMasks15[enemySide12] = GetWalkerMask(enemySide12);
    outOfItMasks15[m_currentSide] = GetOutOfItMask(m_currentSide);
    outOfItMasks15[enemySide12] = GetOutOfItMask(enemySide12);
    traitorMasks13[m_currentSide] = GetTraitorMask(m_currentSide);
    traitorMasks13[enemySide12] = GetTraitorMask(enemySide12);
    shooterStrengths37[m_currentSide] = GetStrength(m_currentSide, shooterMasks6[m_currentSide]);
    shooterStrengths37[enemySide12] = GetStrength(enemySide12, shooterMasks6[enemySide12]);
    totalArmyStrength11 = GetStrength(
        m_currentSide,
        shooterMasks6[m_currentSide] |
            (flyerMasks14[m_currentSide] |
             (walkerMasks15[m_currentSide] |
              (outOfItMasks15[m_currentSide] |
               traitorMasks13[m_currentSide]))));
    fifthArmyStrength5 =
        static_cast<i32>(totalArmyStrength11 + COMBAT_AI_STRENGTH_ROUNDING) /
        COMBAT_AI_STRENGTH_FRACTION;
    strongEnough16 = 0;
    enemyStronger3 = 0;
    currentShooterStrength5 = GetStrength(m_currentSide, shooterMasks6[m_currentSide]);
    enemyShooterStrength18 = GetStrength(enemySide12, shooterMasks6[enemySide12]);

    if (m_inCastleCombat != 0) {
        if (m_heroes[COMBAT_ATTACKER_SIDE]
                    ->m_secondarySkills[HERO_SKILL_ARCHERY] ==
                HERO_SKILL_LEVEL_NONE &&
            !m_heroes[COMBAT_ATTACKER_SIDE]->HasArtifact(
                COMBAT_ARTIFACT_GOLDEN_BOW)) {
            if (m_currentSide == COMBAT_ATTACKER_SIDE)
                currentShooterStrength5 =
                    static_cast<i32>(currentShooterStrength5) /
                    COMBAT_AI_SIDE_COUNT;
            else
                enemyShooterStrength18 =
                    static_cast<i32>(enemyShooterStrength18) /
                    COMBAT_AI_SIDE_COUNT;
        }
        if (m_wallStates[COMBAT_AI_WALL_GATE] == 0) {
            castle2 = m_combatTowns[COMBAT_DEFENDER_SIDE];
            m_combatTowns[COMBAT_DEFENDER_SIDE]->CalcNumLevelArchers(
                &numArchers6, &archerLevel18);
            extraArchers29 = 0;
            if (m_wallStates[COMBAT_AI_WALL_LEFT_TOWER] ==
                COMBAT_AI_WALL_INTACT)
                extraArchers29 += numArchers6 / COMBAT_AI_SIDE_COUNT;
            if (m_wallStates[COMBAT_AI_WALL_RIGHT_TOWER] ==
                COMBAT_AI_WALL_INTACT)
                extraArchers29 += numArchers6 / COMBAT_AI_SIDE_COUNT;
            numArchers6 += extraArchers29;
            towerStrength4 = static_cast<i32>(
                (archerLevel18 * COMBAT_AI_TOWER_LEVEL_SCALE +
                 COMBAT_AI_TOWER_BASE_SCALE) *
                (numArchers6 * COMBAT_AI_TOWER_STRENGTH));
            if (m_heroes[COMBAT_ATTACKER_SIDE] != 0 &&
                (m_heroes[COMBAT_ATTACKER_SIDE]->HasArtifact(
                     COMBAT_ARTIFACT_GOLDEN_BOW) ||
                 m_heroes[COMBAT_ATTACKER_SIDE]
                         ->m_secondarySkills[HERO_SKILL_ARCHERY] !=
                     HERO_SKILL_LEVEL_NONE))
                goldenBowBonus = 0;
            else
                goldenBowBonus = 1;
            if (m_currentSide == COMBAT_DEFENDER_SIDE)
                currentShooterStrength5 += towerStrength4;
            else
                enemyShooterStrength18 += towerStrength4;
        }
    }

    if (m_heroes[m_currentSide] != 0) {
        currentShooterStrength5 = static_cast<i32>(
            static_cast<i32>(currentShooterStrength5) *
            gfSSArcheryMod[m_heroes[m_currentSide]->m_secondarySkills[HERO_SKILL_ARCHERY]]);
    }
    if (m_heroes[COMBAT_DEFENDER_SIDE - m_currentSide] != 0) {
        enemyShooterStrength18 = static_cast<i32>(
            static_cast<i32>(enemyShooterStrength18) *
            gfSSArcheryMod[m_heroes[COMBAT_DEFENDER_SIDE - m_currentSide]
                               ->m_secondarySkills[HERO_SKILL_ARCHERY]]);
    }
    if (static_cast<i32>(currentShooterStrength5) >
        static_cast<i32>(totalArmyStrength11 + COMBAT_AI_STRENGTH_ROUNDING) /
            COMBAT_AI_STRENGTH_FRACTION)
        strongEnough16 = 1;
    if (static_cast<i32>(enemyShooterStrength18) >
        static_cast<i32>(currentShooterStrength5))
        enemyStronger3 = 1;

    if ((currentArmy9->m_monster.flags.abilityFlags &
         MONSTER_ABILITY_FLAG_SHOOTER) != 0) {
        if (currentArmy9->m_monster.shots > 0)
            attackType3 = COMBAT_AI_ATTACK_SHOOT;
        else
            attackType3 = COMBAT_AI_ATTACK_WALK;
    } else if ((currentArmy9->m_monster.flags.abilityFlags &
                MONSTER_ABILITY_FLAG_FLYING) != 0) {
        attackType3 = COMBAT_AI_ATTACK_FLY;
    } else {
        attackType3 = COMBAT_AI_ATTACK_WALK;
    }

    switch (attackType3) {
    case COMBAT_AI_ATTACK_SHOOT:
        if (AttemptAdjacentAttack(currentArmy9))
            goto finish;
        if (currentArmy9->m_monsterType == ARMY_CREATURE_LICH ||
            currentArmy9->m_monsterType == ARMY_CREATURE_POWER_LICH) {
            DoLichShot(currentArmy9);
            goto finish;
        }
        targetArmy16 = GetBestArmy(enemySide12, mirrorMasks7[enemySide12]);
        if (targetArmy16 != -1) {
            giNextAction = COMBAT_AI_ACTION_MOVE;
            giNextActionGridIndex =
                (m_armies[0] + targetArmy16 +
                 enemySide12 * COMBAT_ARMY_STORAGE_SLOT_COUNT)
                    ->m_hex;
            goto finish;
        }
        targetArmy16 = GetBestArmy(enemySide12, shooterMasks6[enemySide12]);
        if (targetArmy16 != -1) {
            giNextAction = COMBAT_AI_ACTION_MOVE;
            giNextActionGridIndex =
                (m_armies[0] + targetArmy16 +
                 enemySide12 * COMBAT_ARMY_STORAGE_SLOT_COUNT)
                    ->m_hex;
            goto finish;
        }
        targetArmy16 = GetBestArmy(enemySide12, flyerMasks14[enemySide12]);
        if (targetArmy16 != -1) {
            giNextAction = COMBAT_AI_ACTION_MOVE;
            giNextActionGridIndex =
                (m_armies[0] + targetArmy16 +
                 enemySide12 * COMBAT_ARMY_STORAGE_SLOT_COUNT)
                    ->m_hex;
            goto finish;
        }
        if (walkerMasks15[enemySide12] != 0) {
            targetArmy16 = GetClosestArmy(currentArmy9, enemySide12, walkerMasks15[enemySide12]);
            if (targetArmy16 != -1) {
                giNextAction = COMBAT_AI_ACTION_MOVE;
                giNextActionGridIndex =
                    (m_armies[0] + targetArmy16 +
                     enemySide12 * COMBAT_ARMY_STORAGE_SLOT_COUNT)
                        ->m_hex;
                goto finish;
            }
        }
        targetArmy16 = GetBestArmy(enemySide12, outOfItMasks15[enemySide12]);
        if (targetArmy16 != -1) {
            giNextAction = COMBAT_AI_ACTION_MOVE;
            giNextActionGridIndex =
                (m_armies[0] + targetArmy16 +
                 enemySide12 * COMBAT_ARMY_STORAGE_SLOT_COUNT)
                    ->m_hex;
            goto finish;
        }
        targetArmy16 = GetBestArmy(enemySide12, traitorMasks13[enemySide12]);
        if (targetArmy16 != -1) {
            giNextAction = COMBAT_AI_ACTION_MOVE;
            giNextActionGridIndex =
                (m_armies[0] + targetArmy16 +
                 enemySide12 * COMBAT_ARMY_STORAGE_SLOT_COUNT)
                    ->m_hex;
            goto finish;
        }
        break;
    case COMBAT_AI_ATTACK_FLY:
        if (strongEnough16 != 0 && enemyStronger3 == 0) {
            if (AttemptAttack(currentArmy9, enemySide12, mirrorMasks7[enemySide12])) goto finish;
            if (AttemptAttack(currentArmy9, enemySide12, shooterMasks6[enemySide12])) goto finish;
            if (AttemptAttack(currentArmy9, enemySide12, flyerMasks14[enemySide12])) goto finish;
            if (AttemptAttack(currentArmy9, enemySide12, walkerMasks15[enemySide12])) goto finish;
            if (AttemptAttack(currentArmy9, enemySide12, outOfItMasks15[enemySide12])) goto finish;
            if (AttemptAttack(currentArmy9, enemySide12, traitorMasks13[enemySide12])) goto finish;
        } else {
            if (AttemptAttack(currentArmy9, enemySide12, mirrorMasks7[enemySide12])) goto finish;
            if (AttemptAttack(currentArmy9, enemySide12, shooterMasks6[enemySide12])) goto finish;
            if (AttemptAttack(currentArmy9, enemySide12, flyerMasks14[enemySide12])) goto finish;
            if (AttemptAttack(currentArmy9, enemySide12, walkerMasks15[enemySide12])) goto finish;
            if (AttemptAttack(currentArmy9, enemySide12, outOfItMasks15[enemySide12])) goto finish;
            if (AttemptAttack(currentArmy9, enemySide12, traitorMasks13[enemySide12])) goto finish;
        }
        break;
    case COMBAT_AI_ATTACK_WALK:
        if (strongEnough16 != 0 && enemyStronger3 == 0) {
            if (AttemptAttack(currentArmy9, enemySide12, mirrorMasks7[enemySide12])) goto finish;
            if (AttemptAttack(currentArmy9, enemySide12, shooterMasks6[enemySide12])) goto finish;
            if (AttemptAttack(currentArmy9, enemySide12, flyerMasks14[enemySide12])) goto finish;
            if (AttemptAttack(currentArmy9, enemySide12, walkerMasks15[enemySide12])) goto finish;
            if (AttemptAttack(currentArmy9, enemySide12, outOfItMasks15[enemySide12])) goto finish;
            if (AttemptAttack(currentArmy9, enemySide12, traitorMasks13[enemySide12])) goto finish;
            if (WalkTowardArmyFront(currentArmy9, m_currentSide,
                                    shooterMasks6[m_currentSide]))
                goto finish;
            giNextAction = COMBAT_AI_ACTION_WAIT;
            goto finish;
        }
        if (AttemptAttack(currentArmy9, enemySide12, mirrorMasks7[enemySide12])) goto finish;
        if (AttemptAttack(currentArmy9, enemySide12, shooterMasks6[enemySide12])) goto finish;
        if (AttemptAttack(currentArmy9, enemySide12, flyerMasks14[enemySide12])) goto finish;
        if (AttemptAttack(currentArmy9, enemySide12, walkerMasks15[enemySide12])) goto finish;
        if (AttemptAttack(currentArmy9, enemySide12, outOfItMasks15[enemySide12])) goto finish;
        if (AttemptAttack(currentArmy9, enemySide12, traitorMasks13[enemySide12])) goto finish;
        if (WalkTowardArmy(currentArmy9, enemySide12, shooterMasks6[enemySide12])) goto finish;
        if (WalkTowardArmy(currentArmy9, enemySide12, walkerMasks15[enemySide12])) goto finish;
        if (WalkTowardArmy(currentArmy9, enemySide12, flyerMasks14[enemySide12])) goto finish;
        if (WalkTowardArmy(currentArmy9, enemySide12, outOfItMasks15[enemySide12])) goto finish;
        if (WalkTowardArmy(currentArmy9, enemySide12, traitorMasks13[enemySide12])) goto finish;

        castleBoundary27[0] = COMBAT_AI_CASTLE_BOUNDARY_ROW_0;
        castleBoundary27[1] = COMBAT_AI_CASTLE_BOUNDARY_ROW_1;
        castleBoundary27[2] = COMBAT_AI_CASTLE_BOUNDARY_ROW_2;
        castleBoundary27[3] = COMBAT_AI_CASTLE_BOUNDARY_ROW_3;
        castleBoundary27[4] = COMBAT_AI_CASTLE_BOUNDARY_ROW_4;
        castleBoundary27[5] = COMBAT_AI_CASTLE_BOUNDARY_ROW_5;
        castleBoundary27[6] = COMBAT_AI_CASTLE_BOUNDARY_ROW_6;
        castleBoundary27[7] = COMBAT_AI_CASTLE_BOUNDARY_ROW_7;
        castleBoundary27[8] = COMBAT_AI_CASTLE_BOUNDARY_ROW_8;
        castleRow29 = currentArmy9->m_hex / ARMY_HEX_COLUMNS;
        if (m_currentSide == COMBAT_ATTACKER_SIDE && m_inCastleCombat != 0 &&
            currentArmy9->m_hex < castleBoundary27[castleRow29]) {
            castleTargetHex2 = castleBoundary27[castleRow29];
            castleCell5 = &gpCombatManager->m_hexCells[castleTargetHex2];
            if (ValidHex(castleTargetHex2) && castleCell5->m_occupantSide == -1 &&
                castleCell5->m_blocked == 0) {
                giNextAction = COMBAT_AI_ACTION_MOVE;
                giNextActionGridIndex = castleTargetHex2;
                goto finish;
            }
        }
        break;
    }
    giNextAction = COMBAT_AI_ACTION_WAIT;

finish:
    if (giNextAction == COMBAT_AI_ACTION_MOVE && giNextActionGridIndex > 0 &&
        giNextActionGridIndex < COMBAT_HEX_COUNT &&
        gpCombatManager->m_hexCells[giNextActionGridIndex].m_occupantSide == -1) {
        for (adjacentDirection6 = 0;
             adjacentDirection6 < COMBAT_AI_ADJACENT_DIRECTION_COUNT;
             adjacentDirection6++) {
            adjacentHex8 = currentArmy9->GetAdjacentCellIndex(
                giNextActionGridIndex, adjacentDirection6);
            if (adjacentHex8 > 0 && adjacentHex8 < COMBAT_HEX_COUNT &&
                gpCombatManager->m_hexCells[adjacentHex8].m_occupantSide ==
                    COMBAT_DEFENDER_SIDE - m_currentSide)
                break;
        }
        giNextActionGridIndex = adjacentHex8;
    }
}

// @semantic
// Shared enum declaration state changed the x87 compare/store order at +0x37:
// ours uses fst/fcomp where retail uses fcom/fstp. Frame, CFG, operands, and
// external relocations remain aligned; revisit during the byte-last-mile pass.
VA(0x004c2303, 0xc9)
float combatManager::GetModLichDamage(class army *target, float damage)
{
    float modifiedDamage = damage;
    float remainingHitPoints = static_cast<float>(
        target->m_monster.hitPoints * target->m_quantity -
        target->m_hitPointsLost);

    if (remainingHitPoints < modifiedDamage)
        modifiedDamage = remainingHitPoints;
    if ((target->m_monster.flags.abilityFlags &
         MONSTER_ABILITY_FLAG_SHOOTER) != 0)
        modifiedDamage = static_cast<float>(
            modifiedDamage * COMBAT_AI_LICH_PRIORITY_MULTIPLIER);
    if ((target->m_monster.flags.abilityFlags &
         MONSTER_ABILITY_FLAG_FLYING) != 0)
        modifiedDamage = static_cast<float>(
            modifiedDamage * COMBAT_AI_LICH_PRIORITY_MULTIPLIER);
    modifiedDamage = static_cast<float>(
        (target->m_monster.hitPoints + COMBAT_AI_LICH_HIT_POINT_BONUS) *
        modifiedDamage /
        COMBAT_AI_LICH_HIT_POINT_SCALE);
    return modifiedDamage;
}

VA(0x004c23cc, 0x32e)
void combatManager::DoLichShot(class army *lich)
{
    i32 bestArmy12 = COMBAT_AI_NO_ARMY;
    float bestDamage15 = COMBAT_AI_MIN_LICH_DAMAGE_SCORE;
    float lichDamage5 = static_cast<float>(
        lich->m_quantity * COMBAT_AI_LICH_DAMAGE_PER_CREATURE);
    i32 armyIndex37;
    u8 damaged19[COMBAT_AI_SIDE_COUNT * COMBAT_AI_ARMY_SLOT_COUNT];
    float damageValue10;
    float adjacentDamage6;
    i32 direction37;
    i32 adjacentHex13;
    i32 targetHex36;
    army *target17;

    for (armyIndex37 = 0;
         armyIndex37 < m_armyCount[COMBAT_DEFENDER_SIDE - m_currentSide];
         armyIndex37++) {
        memset(damaged19, 0, sizeof(damaged19));
        damageValue10 = 0;
        target17 = &m_armies[COMBAT_DEFENDER_SIDE - m_currentSide][armyIndex37];
        if (target17 == 0 ||
            (target17->m_monster.flags.abilityFlags &
             MONSTER_ABILITY_FLAG_AI_EXCLUDED) != 0 ||
            target17->m_quantity <= 0)
            continue;
        damageValue10 = GetModLichDamage(target17, lichDamage5);
        damaged19[target17->m_side * COMBAT_AI_ARMY_SLOT_COUNT +
                  target17->m_index] = 1;
        targetHex36 = target17->m_hex;
        for (direction37 = 0;
             direction37 < COMBAT_AI_ADJACENT_DIRECTION_COUNT;
             direction37++) {
            adjacentHex13 = GetAdjacentCellIndexNoArmy(targetHex36, direction37);
            if (adjacentHex13 >= 0 && adjacentHex13 < COMBAT_HEX_COUNT &&
                m_hexCells[adjacentHex13].m_occupantSide != -1 &&
                m_hexCells[adjacentHex13].m_occupantIndex != -1 &&
                damaged19[
                    m_hexCells[adjacentHex13].m_occupantSide *
                        COMBAT_AI_ARMY_SLOT_COUNT +
                    m_hexCells[adjacentHex13].m_occupantIndex] == 0) {
                adjacentDamage6 = GetModLichDamage(
                    &m_armies[m_hexCells[adjacentHex13].m_occupantSide]
                             [m_hexCells[adjacentHex13].m_occupantIndex],
                    lichDamage5);
                damaged19[
                    m_hexCells[adjacentHex13].m_occupantSide *
                        COMBAT_AI_ARMY_SLOT_COUNT +
                    m_hexCells[adjacentHex13].m_occupantIndex] = 1;
                if (m_hexCells[adjacentHex13].m_occupantSide == m_currentSide)
                    damageValue10 -= adjacentDamage6;
                else
                    damageValue10 += adjacentDamage6;
            }
        }
        if (bestArmy12 == COMBAT_AI_NO_ARMY ||
            0[&damageValue10] > bestDamage15) {
            bestDamage15 = damageValue10;
            bestArmy12 = armyIndex37;
            giNextAction = COMBAT_AI_ACTION_MOVE;
            giNextActionGridIndex = targetHex36;
        }
    }
}

VA(0x004c26fa, 0x131)
i32 combatManager::GetShooterMask(i32 side)
{
    i32 armyIndex2 = 0;
    u32 bit1 = COMBAT_AI_MASK_FIRST_BIT;
    u32 mask5 = 0;
    army *currentArmy10;

    for (armyIndex2 = 0; armyIndex2 < m_armyCount[side]; armyIndex2++) {
        currentArmy10 = armyIndex2 + m_armies[side];
        if (currentArmy10 != 0 &&
            (currentArmy10->m_monster.flags.abilityFlags &
             MONSTER_ABILITY_FLAG_AI_EXCLUDED) == 0 &&
            (currentArmy10->m_monster.flags.abilityFlags &
             MONSTER_ABILITY_FLAG_SHOOTER) != 0 &&
            currentArmy10->m_monster.shots > 0 &&
            currentArmy10->m_spellInfluence[SPELL_INFLUENCE_BLIND] == 0 &&
            currentArmy10->m_spellInfluence[SPELL_INFLUENCE_PARALYZE] == 0 &&
            currentArmy10->m_spellInfluence[SPELL_INFLUENCE_MIRROR_IMAGE] == 0 &&
            currentArmy10->m_spellInfluence[SPELL_INFLUENCE_BERSERKER] == 0 &&
            currentArmy10->m_spellInfluence[SPELL_INFLUENCE_HYPNOTIZE] == 0)
            mask5 |= bit1;
        bit1 <<= 1;
    }
    return mask5;
}

VA(0x004c282b, 0xc0)
i32 combatManager::GetMirrorImageMask(i32 side)
{
    i32 armyIndex2 = 0;
    u32 bit1 = COMBAT_AI_MASK_FIRST_BIT;
    u32 mask5 = 0;
    army *currentArmy10;

    for (armyIndex2 = 0; armyIndex2 < m_armyCount[side]; armyIndex2++) {
        currentArmy10 = m_armies[side] + armyIndex2;
        if (currentArmy10 != 0 &&
            (currentArmy10->m_monster.flags.abilityFlags &
             MONSTER_ABILITY_FLAG_AI_EXCLUDED) == 0 &&
            (currentArmy10->m_monster.flags.all &
             MONSTER_FLAGS_MIRROR_IMAGE) != 0)
            mask5 |= bit1;
        bit1 <<= 1;
    }
    return mask5;
}

VA(0x004c28eb, 0x11f)
i32 combatManager::GetFlyerMask(i32 side)
{
    i32 armyIndex2 = 0;
    u32 bit1 = COMBAT_AI_MASK_FIRST_BIT;
    u32 mask5 = 0;
    army *currentArmy10;

    for (armyIndex2 = 0; armyIndex2 < m_armyCount[side]; armyIndex2++) {
        currentArmy10 = m_armies[side] + armyIndex2;
        if (currentArmy10 != 0 &&
            (currentArmy10->m_monster.flags.abilityFlags &
             MONSTER_ABILITY_FLAG_AI_EXCLUDED) == 0 &&
            (currentArmy10->m_monster.flags.abilityFlags &
             MONSTER_ABILITY_FLAG_FLYING) != 0 &&
            currentArmy10->m_spellInfluence[SPELL_INFLUENCE_BLIND] == 0 &&
            currentArmy10->m_spellInfluence[SPELL_INFLUENCE_PARALYZE] == 0 &&
            currentArmy10->m_spellInfluence[SPELL_INFLUENCE_MIRROR_IMAGE] == 0 &&
            currentArmy10->m_spellInfluence[SPELL_INFLUENCE_BERSERKER] == 0 &&
            currentArmy10->m_spellInfluence[SPELL_INFLUENCE_HYPNOTIZE] == 0)
            mask5 |= bit1;
        bit1 <<= 1;
    }
    return mask5;
}

VA(0x004c2a0a, 0xc0)
i32 combatManager::GetAllMask(i32 side)
{
    i32 armyIndex11 = 0;
    u32 bit1 = COMBAT_AI_MASK_FIRST_BIT;
    u32 mask5 = 0;
    army *currentArmy38;

    for (armyIndex11 = 0; armyIndex11 < m_armyCount[side]; armyIndex11++) {
        currentArmy38 = reinterpret_cast<army *>(
                            reinterpret_cast<u8 *>(m_armies) +
                            side * sizeof(m_armies[0])) +
            armyIndex11;
        if (currentArmy38 != 0 &&
            (currentArmy38->m_monster.flags.abilityFlags &
             MONSTER_ABILITY_FLAG_AI_EXCLUDED) == 0 &&
            currentArmy38->m_quantity > 0)
            mask5 |= bit1;
        bit1 <<= 1;
    }
    return mask5;
}

VA(0x004c2aca, 0x141)
i32 combatManager::GetWalkerMask(i32 side)
{
    i32 armyIndex2 = 0;
    u32 bit1 = COMBAT_AI_MASK_FIRST_BIT;
    u32 mask5 = 0;
    army *currentArmy10;

    for (armyIndex2 = 0; armyIndex2 < m_armyCount[side]; armyIndex2++) {
        currentArmy10 = m_armies[side] + armyIndex2;
        if (currentArmy10 != 0 &&
            (currentArmy10->m_monster.flags.abilityFlags &
             MONSTER_ABILITY_FLAG_AI_EXCLUDED) == 0 &&
            (currentArmy10->m_monster.flags.abilityFlags &
             MONSTER_ABILITY_FLAG_FLYING) == 0 &&
            ((currentArmy10->m_monster.flags.abilityFlags &
              MONSTER_ABILITY_FLAG_SHOOTER) == 0 ||
             currentArmy10->m_monster.shots <= 0) &&
            currentArmy10->m_spellInfluence[SPELL_INFLUENCE_BLIND] == 0 &&
            currentArmy10->m_spellInfluence[SPELL_INFLUENCE_PARALYZE] == 0 &&
            currentArmy10->m_spellInfluence[SPELL_INFLUENCE_MIRROR_IMAGE] == 0 &&
            currentArmy10->m_spellInfluence[SPELL_INFLUENCE_BERSERKER] == 0 &&
            currentArmy10->m_spellInfluence[SPELL_INFLUENCE_HYPNOTIZE] == 0)
            mask5 |= bit1;
        bit1 <<= 1;
    }
    return mask5;
}

VA(0x004c2c0b, 0xe9)
i32 combatManager::GetOutOfItMask(i32 side)
{
    i32 armyIndex2 = 0;
    u32 bit1 = COMBAT_AI_MASK_FIRST_BIT;
    u32 mask5 = 0;
    army *currentArmy10;

    for (armyIndex2 = 0; armyIndex2 < m_armyCount[side]; armyIndex2++) {
        currentArmy10 = m_armies[side] + armyIndex2;
        if (currentArmy10 != 0 &&
            (currentArmy10->m_monster.flags.abilityFlags &
             MONSTER_ABILITY_FLAG_AI_EXCLUDED) == 0 &&
            (currentArmy10->m_spellInfluence[SPELL_INFLUENCE_BLIND] != 0 ||
             currentArmy10->m_spellInfluence[SPELL_INFLUENCE_PARALYZE] != 0 ||
             currentArmy10->m_spellInfluence[SPELL_INFLUENCE_MIRROR_IMAGE] != 0))
            mask5 |= bit1;
        bit1 <<= 1;
    }
    return mask5;
}

VA(0x004c2cf4, 0xd6)
i32 combatManager::GetTraitorMask(i32 side)
{
    i32 armyIndex2 = 0;
    u32 bit1 = COMBAT_AI_MASK_FIRST_BIT;
    u32 mask5 = 0;
    army *currentArmy10;

    for (armyIndex2 = 0; armyIndex2 < m_armyCount[side]; armyIndex2++) {
        currentArmy10 = armyIndex2 + m_armies[side];
        if (currentArmy10 != 0 &&
            (currentArmy10->m_monster.flags.abilityFlags &
             MONSTER_ABILITY_FLAG_AI_EXCLUDED) == 0 &&
            (currentArmy10->m_spellInfluence[SPELL_INFLUENCE_BERSERKER] != 0 ||
             currentArmy10->m_spellInfluence[SPELL_INFLUENCE_HYPNOTIZE] != 0))
            mask5 |= bit1;
        bit1 <<= 1;
    }
    return mask5;
}

VA(0x004c2dca, 0x1f1)
i32 combatManager::GetBestArmy(i32 side, i32 mask)
{
    i32 armyIndex2 = 0;
    u32 bit1 = COMBAT_AI_MASK_FIRST_BIT;
    u32l bestStrength8 = 0;
    i32 bestArmy1 = COMBAT_AI_NO_ARMY;
    u32l strength8;

    for (armyIndex2 = 0; armyIndex2 < m_armyCount[side]; armyIndex2++) {
        if ((mask & bit1) != 0) {
            strength8 = (m_armies[side] + armyIndex2)->Strength();
            if ((m_armies[side] + armyIndex2)
                        ->m_spellInfluence[SPELL_INFLUENCE_BLIND] != 0 ||
                (m_armies[side] + armyIndex2)
                        ->m_spellInfluence[SPELL_INFLUENCE_PARALYZE] != 0 ||
                (m_armies[side] + armyIndex2)
                        ->m_spellInfluence[SPELL_INFLUENCE_MIRROR_IMAGE] != 0 ||
                (m_armies[side] + armyIndex2)
                        ->m_spellInfluence[SPELL_INFLUENCE_BERSERKER] != 0 ||
                (m_armies[side] + armyIndex2)
                        ->m_spellInfluence[SPELL_INFLUENCE_HYPNOTIZE] != 0)
                strength8 >>= 1;
            if (bestStrength8 < strength8) {
                bestArmy1 = armyIndex2;
                bestStrength8 = strength8;
            }
        }
        bit1 <<= 1;
    }
    return bestArmy1;
}

VA(0x004c2fbb, 0xc2)
i32 combatManager::GetWorstArmy(i32 side, i32 mask)
{
    i32 armyIndex2 = 0;
    u32 bit1 = COMBAT_AI_MASK_FIRST_BIT;
    u32l worstStrength2 = COMBAT_AI_WORST_STRENGTH_LIMIT;
    i32 worstArmy6 = COMBAT_AI_NO_ARMY;
    u32l strength8;

    for (armyIndex2 = 0; armyIndex2 < m_armyCount[side]; armyIndex2++) {
        if ((mask & bit1) != 0) {
            strength8 = m_armies[side][armyIndex2].Strength();
            if (strength8 < worstStrength2) {
                worstArmy6 = armyIndex2;
                worstStrength2 = strength8;
            }
        }
        bit1 <<= 1;
    }
    return worstArmy6;
}

VA(0x004c307d, 0x16f)
i32 combatManager::GetClosestArmy(class army *currentArmy, i32 side, i32 mask)
{
    i32 armyIndex2 = 0;
    u32 bit1 = COMBAT_AI_MASK_FIRST_BIT;
    i32 closestValue29 = COMBAT_AI_CLOSEST_ARMY_LIMIT;
    i32 closestArmy7 = COMBAT_AI_NO_ARMY;
    i32 value19;
    army *target26;

    for (armyIndex2 = 0; armyIndex2 < m_armyCount[side]; armyIndex2++) {
        if ((0[&bit1] & mask) != 0) {
            target26 = &0[&armyIndex2][m_armies[side]];
            value19 = gpSearchArray->QuickDistance(
                m_hexCells[currentArmy->m_hex].m_x,
                m_hexCells[currentArmy->m_hex].m_y,
                m_hexCells[target26->m_hex].m_x,
                m_hexCells[target26->m_hex].m_y);
            value19 = value19 * COMBAT_AI_DISTANCE_WEIGHT -
                target26->m_monster.hitPoints * target26->m_quantity;
            if (0[&closestValue29] > value19) {
                closestArmy7 = armyIndex2;
                closestValue29 = value19;
            }
        }
        bit1 <<= 1;
    }
    return closestArmy7;
}

VA(0x004c31ec, 0xc1)
u32l combatManager::GetStrength(i32 side, i32 mask)
{
    i32 armyIndex4 = 0;
    u32 bit36 = COMBAT_AI_MASK_FIRST_BIT;
    u32l strength7 = 0;
    army *currentArmy8;

    for (armyIndex4 = 0; armyIndex4 < m_armyCount[side]; armyIndex4++) {
        if ((bit36 & mask) != 0) {
            currentArmy8 = reinterpret_cast<army *>(
                               reinterpret_cast<u8 *>(m_armies) +
                               side * sizeof(m_armies[0])) +
                armyIndex4;
            if (currentArmy8 != 0 &&
                (currentArmy8->m_monster.flags.abilityFlags &
                 MONSTER_ABILITY_FLAG_AI_EXCLUDED) == 0)
                strength7 += currentArmy8->Strength();
        }
        bit36 <<= 1;
    }
    return strength7;
}

VA(0x004c32ad, 0x1bb)
i32 combatManager::AttemptAttack(class army *currentArmy, i32 side, i32 mask)
{
    i32 targetArmy;
    i32 targetHex;

    while (mask != 0) {
        if (currentArmy->m_monsterType == ARMY_CREATURE_GHOST)
            targetArmy = GetWorstArmy(side, mask);
        else
            targetArmy = GetBestArmy(side, mask);
        if (targetArmy == COMBAT_AI_NO_ARMY)
            return 0;

        currentArmy->m_targetSide = side;
        currentArmy->m_targetIndex = targetArmy;
        targetHex = 0[&targetArmy][m_armies[side]].m_hex;
        currentArmy->m_moveTargetHex = targetHex;
        if (currentArmy->ValidPath(targetHex, 0)) {
            giNextAction = COMBAT_AI_ACTION_MOVE;
            giNextActionGridIndex = targetHex;
            return 1;
        }
        if ((0[&targetArmy][m_armies[side]].m_monster.flags.abilityFlags &
             MONSTER_ABILITY_FLAG_WIDE) != 0) {
            if (0[&targetArmy][m_armies[side]].m_facing == 0)
                targetHex--;
            else
                targetHex++;
            currentArmy->m_moveTargetHex = targetHex;
            if (currentArmy->ValidPath(targetHex, 0)) {
                giNextAction = COMBAT_AI_ACTION_MOVE;
                giNextActionGridIndex = targetHex;
                return 1;
            }
        }
        mask &= ~(1 << targetArmy);
    }
    return 0;
}

// @early-stop
// All 99 non-jump opcodes/operands, the 0x20 frame/local slots, CFG, and all
// 6/6 ordered external relocations agree. Retail is exactly five bytes larger:
// its sole residual is one /Ob1 continuation `jmp` at +0x16f before false return;
// four bounded TU-state probes produced no exact closure.
VA(0x004c3468, 0x182)
i32 combatManager::AttemptAdjacentAttack(class army *currentArmy)
{
    u32 availableMask4 = ~currentArmy->GetAttackMask(
        currentArmy->m_hex, 1, -1);
    u32 bit0;
    u32 targetMask29;
    i32 direction36;
    i32 attackHexes5[2];
    i32 targetArmy15;

    if (availableMask4 == 0)
        return 0;

    bit0 = COMBAT_AI_MASK_FIRST_BIT;
    targetMask29 = 0;
    for (direction36 = 0;
         direction36 < COMBAT_AI_ATTACK_DIRECTION_COUNT;
         direction36++) {
        if ((availableMask4 & bit0) != 0 &&
            currentArmy->ValidAttack(currentArmy->m_hex, direction36,
                                     1, -1, attackHexes5) &&
            attackHexes5[0] >= 0)
            targetMask29 |=
                1 << m_hexCells[attackHexes5[0]].m_occupantIndex;
        bit0 <<= 1;
    }
    if (currentArmy->m_monsterType == ARMY_CREATURE_GHOST)
        targetArmy15 = GetWorstArmy(
            COMBAT_DEFENDER_SIDE - m_currentSide, targetMask29);
    else
        targetArmy15 = GetBestArmy(
            COMBAT_DEFENDER_SIDE - m_currentSide, targetMask29);
    if (targetArmy15 != COMBAT_AI_NO_ARMY) {
        giNextAction = COMBAT_AI_ACTION_MOVE;
        giNextActionGridIndex =
            m_armies[COMBAT_DEFENDER_SIDE - m_currentSide][targetArmy15].m_hex;
        return 1;
    }
    return 0;
}

VA(0x004c35ea, 0x240)
i32 combatManager::WalkTowardArmyFront(class army *currentArmy,
                                      i32 side, i32 mask)
{
    i32 targetArmy6;
    i32 frontOffset13;
    i32 targetHex7;
    i32 savedSpeed11;
    i32 pathFound6;
    i32 movement3;
    i32 pathIndex12;

    currentArmy->m_targetSide = COMBAT_AI_NO_ARMY;
    currentArmy->m_targetIndex = COMBAT_AI_NO_ARMY;
    targetArmy6 = GetClosestArmy(currentArmy, side, mask);
    if (targetArmy6 == COMBAT_AI_NO_ARMY)
        return 0;

    frontOffset13 = 1;
    targetHex7 = 0[&targetArmy6][m_armies[side]].m_hex;
    if ((0[&targetArmy6][m_armies[side]].m_monster.flags.abilityFlags &
         MONSTER_ABILITY_FLAG_WIDE) != 0)
        frontOffset13 = 2;
    if (currentArmy->m_facing == 1)
        targetHex7 += frontOffset13;
    else
        targetHex7 += -frontOffset13;
    if (targetHex7 % ARMY_HEX_COLUMNS == ARMY_HEX_COLUMNS - 1 ||
        targetHex7 % ARMY_HEX_COLUMNS == 0)
        return WalkTowardArmy(currentArmy, side, mask);

    savedSpeed11 = currentArmy->m_monster.speed;
    currentArmy->m_monster.speed = COMBAT_AI_UNLIMITED_PATH_SPEED;
    pathFound6 = gpSearchArray->FindCombatPath(
        currentArmy->m_hex, targetHex7, currentArmy,
        COMBAT_AI_PATH_TO_FRONT, 0);
    currentArmy->m_monster.speed = static_cast<i8>(savedSpeed11);
    if (gpSearchArray->m_pathLength > 0) {
        giNextAction = COMBAT_AI_ACTION_MOVE;
        movement3 = currentArmy->m_monster.speed;
        pathIndex12 = gpSearchArray->m_pathLength - 1;
        giNextActionGridIndex = currentArmy->m_hex;
        while (pathIndex12 >= 0 && movement3 != 0) {
            giNextActionGridIndex = currentArmy->GetAdjacentCellIndex(
                giNextActionGridIndex,
                static_cast<u8>(
                    gpSearchArray->m_storage.aiPath.directions[pathIndex12]));
            pathIndex12--;
            movement3--;
            if (giNextActionGridIndex > 0 &&
                bIsMoatSlowed[giNextActionGridIndex] != 0)
                movement3 = 0;
        }
        return 1;
    }
    return WalkTowardArmy(currentArmy, side, mask);
}

// @semantic: Complete 0x2c frame, slots, path/CFG behavior, and ordered external
// relocations. The first residual at +0x34 is equivalent two-dimensional army
// indexing: retail scales targetArmy before side, candidate scales side first.
// Direct indexing, cached target pointer, and commuted address forms were tried.
VA(0x004c382a, 0x244)
i32 combatManager::WalkTowardArmy(class army *currentArmy, i32 side, i32 mask)
{
    i32 targetArmy6;
    army *targetPtr9;
    i32 targetHex7;
    i32 attackMask36;
    i32 savedSpeed12;
    i32 pathFound5;
    i32 unusedPath8;
    i32 movement27;
    i32 pathIndex14;

    targetArmy6 = GetClosestArmy(currentArmy, side, mask);

    if (targetArmy6 == COMBAT_AI_NO_ARMY)
        return 0;

    targetPtr9 = &m_armies[side][targetArmy6];
    targetHex7 = targetPtr9->m_hex;
    currentArmy->m_targetSide = side;
    currentArmy->m_targetIndex = targetArmy6;
    attackMask36 = currentArmy->GetAttackMask(currentArmy->m_hex, 0, -1);
    if (attackMask36 != COMBAT_AI_ALL_ATTACK_DIRECTIONS) {
        giNextAction = COMBAT_AI_ACTION_WAIT;
        return 1;
    }

    savedSpeed12 = currentArmy->m_monster.speed;
    currentArmy->m_monster.speed = COMBAT_AI_UNLIMITED_PATH_SPEED;
    pathFound5 = gpSearchArray->FindCombatPath(
        currentArmy->m_hex, targetHex7, currentArmy,
        COMBAT_AI_PATH_TO_TARGET, 0);
    if (pathFound5 == 0 &&
        (targetPtr9->m_monster.flags.abilityFlags &
         MONSTER_ABILITY_FLAG_WIDE) != 0) {
        switch (targetPtr9->m_facing) {
        case 0:
            targetHex7--;
            break;
        case 1:
            targetHex7++;
            break;
        }
        if (targetHex7 != COMBAT_AI_NO_ARMY)
            pathFound5 = gpSearchArray->FindCombatPath(
                currentArmy->m_hex, targetHex7, currentArmy,
                COMBAT_AI_PATH_TO_TARGET, 0);
    }
    currentArmy->m_monster.speed = static_cast<i8>(savedSpeed12);
    if (gpSearchArray->m_pathLength > 1) {
        giNextAction = COMBAT_AI_ACTION_MOVE;
        movement27 = currentArmy->m_monster.speed;
        pathIndex14 = gpSearchArray->m_pathLength - 1;
        giNextActionGridIndex = currentArmy->m_hex;
        while (pathIndex14 >= 1 && movement27 != 0) {
            giNextActionGridIndex = currentArmy->GetAdjacentCellIndex(
                giNextActionGridIndex,
                static_cast<u8>(
                    gpSearchArray->m_storage.aiPath.directions[pathIndex14]));
            pathIndex14--;
            movement27--;
            if (giNextActionGridIndex > 0 &&
                bIsMoatSlowed[giNextActionGridIndex] != 0)
                movement27 = 0;
        }
        return 1;
    }
    return 0;
}
