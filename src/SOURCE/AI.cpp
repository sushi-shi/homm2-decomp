#include <va.h>
#include <string.h>
#include <SOURCE/CMBTMGR.h>
#include <SOURCE/KB.h>
#include <SOURCE/PATH.h>
#include <SOURCE/PHILAI.h>
#include <SOURCE/SPELLS.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/army.h>
#include <SOURCE/armyGroup.h>
#include <SOURCE/combatManager.h>
#include <SOURCE/game.h>
#include <SOURCE/hero.h>
#include <SOURCE/philAI.h>
#include <SOURCE/searchArray.h>
#include <SOURCE/town.h>

#define COMBAT_AI_QUANTITY_ESTIMATE 1.2
#define COMBAT_AI_TOWN_STRENGTH_MODIFIER 1.1
#define COMBAT_AI_BASE_RETREAT_CHANCE 0.16f
#define COMBAT_AI_MAX_RETREAT_CHANCE_COMPARE 0.16
#define COMBAT_AI_MAX_RETREAT_CHANCE 0.16f
#define COMBAT_AI_HIGH_ARTIFACT_RETREAT_BONUS 0.06
#define COMBAT_AI_MEDIUM_ARTIFACT_RETREAT_BONUS 0.05
#define COMBAT_AI_LOW_ARTIFACT_RETREAT_BONUS 0.04
#define COMBAT_AI_STRENGTH_30000_RETREAT_PENALTY 0.08
#define COMBAT_AI_STRENGTH_15000_RETREAT_PENALTY 0.06
#define COMBAT_AI_STRENGTH_5000_RETREAT_PENALTY 0.04
#define COMBAT_AI_STRENGTH_2500_RETREAT_PENALTY 0.02
#define COMBAT_AI_DIFFICULTY_RETREAT_STEP 0.015
#define COMBAT_AI_MAX_EXPERIENCE_BONUS_COMPARE 0.03
#define COMBAT_AI_MAX_EXPERIENCE_BONUS 0.03f
#define COMBAT_AI_ATTACKER_RETREAT_PENALTY 0.06
#define COMBAT_AI_TOWER_LEVEL_SCALE 0.1
#define COMBAT_AI_TOWER_BASE_SCALE 1.0
#define COMBAT_AI_LICH_PRIORITY_MULTIPLIER 1.3
#define COMBAT_AI_LICH_HIT_POINT_SCALE 100.0f
#define COMBAT_AI_MIN_LICH_DAMAGE_SCORE                                                      \
    (-99999.0f)

H2_ENUM_BEGIN(ArmyFrontOffset)
    SINGLE_HEX_FRONT_OFFSET    = 1,
    WIDE_CREATURE_FRONT_OFFSET = 2
H2_ENUM_END(ArmyFrontOffset)

VA(0x00415a60, 0x82b)
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
    if (gpGame->m_players[m_heroes[IDX(m_currentSide)]->m_owner].m_townCount == 0
        && gpGame->m_players[m_heroes[IDX(m_currentSide)]->m_owner].m_heroCount
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

VA(0x0041628b, 0xfa0)
void combatManager::DoCompAI(H2_ENUM_PARAM(CombatSide, i32)) {
    u32 traitorArray[COMBAT_SIDE_COUNT];
    i32 best;
    CombatSide sideEnemy;
    u32 flyerMask[COMBAT_SIDE_COUNT];
    u32 walkers[COMBAT_SIDE_COUNT];
    i32 stronger;
    i32 shootStrong;
    u32l myShootPower;
    i32 plan;
    u32l shootStrengths[COMBAT_SIDE_COUNT];
    u32l enemyShooters;
    army* thisArmy;
    u32 oddMasks[COMBAT_SIDE_COUNT];
    CombatHexDirection dirIndex;
    u32l totalArmyStrength;
    combatManager* combat;
    i32 fifth;
    u32 shooters[COMBAT_SIDE_COUNT];
    i32 archers;
    u32 mirrorMask[COMBAT_SIDE_COUNT];
    i32 rowIndex;
    i32 archeryBonus;
    i32 plusArchers;
    i32 keepStrength;
    i32 grade;
    town* pCastle;
    u8 rowLimit[COMBAT_AI_CASTLE_BOUNDARY_COUNT];
    i32 adjCell;
    hexcell* targetCell;
    u32 targetHex;

    m_limitCreature = 0;
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
    shootStrong = 0;
    stronger = 0;
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
        shootStrong = 1;
    if (static_cast<i32>(enemyShooters) > static_cast<i32>(myShootPower))
        stronger = 1;

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

VA(0x0041722b, 0xab)
float combatManager::GetModLichDamage(class army* target, float damage) {
    float modifiedDamage = damage;
    float remainingHitPoints = static_cast<float>(
        target->m_quantity * target->m_monster.hitPoints - target->m_hitPointsLost
    );

    if (modifiedDamage > remainingHitPoints)
        modifiedDamage = remainingHitPoints;
    if (HAS(target->m_monster.flags.abilityFlags, MONSTER_ABILITY_FLAG_SHOOTER) != 0)
        modifiedDamage = static_cast<float>(modifiedDamage * COMBAT_AI_LICH_PRIORITY_MULTIPLIER);
    if (HAS(target->m_monster.flags.abilityFlags, MONSTER_ABILITY_FLAG_FLYING) != 0)
        modifiedDamage = static_cast<float>(modifiedDamage * COMBAT_AI_LICH_PRIORITY_MULTIPLIER);
    modifiedDamage = ((target->m_monster.hitPoints + COMBAT_AI_LICH_HIT_POINT_BONUS) * modifiedDamage
        / COMBAT_AI_LICH_HIT_POINT_SCALE);
    return modifiedDamage;
}

VA(0x004172d6, 0x293)
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

VA(0x00417569, 0x109)
i32 combatManager::GetShooterMask(H2_ENUM_PARAM(CombatSide, i32) side) {
    i32 armyIndex = 0;
    u32 armyBit = COMBAT_AI_MASK_FIRST_BIT;
    u32 bits = 0;
    army* currentArmy;

    for (armyIndex = 0; armyIndex < m_armyCount[IDX(side)]; armyIndex++) {
        currentArmy = armyIndex + m_armies[IDX(side)];
        if (currentArmy != NULL
            && HAS(currentArmy->m_monster.flags.abilityFlags, MONSTER_ABILITY_FLAG_AI_EXCLUDED)
                   == 0
            && HAS(currentArmy->m_monster.flags.abilityFlags, MONSTER_ABILITY_FLAG_SHOOTER) != 0
            && currentArmy->m_monster.shots > 0
            && currentArmy->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_BLIND)] == 0
            && currentArmy->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_PARALYZE)] == 0
            && currentArmy->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_PETRIFIED)] == 0
            && currentArmy->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_BERSERK)] == 0
            && currentArmy->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_HYPNOTIZE)] == 0)
            bits |= armyBit;
        armyBit <<= 1;
    }
    return bits;
}

VA(0x00417672, 0xa8)
i32 combatManager::GetMirrorImageMask(H2_ENUM_PARAM(CombatSide, i32) side) {
    i32 armyIndex = 0;
    u32 armyBit = COMBAT_AI_MASK_FIRST_BIT;
    u32 bits = 0;
    army* currentArmy;

    for (armyIndex = 0; armyIndex < m_armyCount[IDX(side)]; armyIndex++) {
        currentArmy = m_armies[IDX(side)] + armyIndex;
        if (currentArmy != NULL
            && HAS(currentArmy->m_monster.flags.abilityFlags, MONSTER_ABILITY_FLAG_AI_EXCLUDED)
                   == 0
            && HAS(currentArmy->m_monster.flags.all, MONSTER_FLAGS_MIRROR_IMAGE) != 0)
            bits |= armyBit;
        armyBit <<= 1;
    }
    return bits;
}

VA(0x0041771a, 0xf7)
i32 combatManager::GetFlyerMask(H2_ENUM_PARAM(CombatSide, i32) side) {
    i32 armyIndex = 0;
    u32 armyBit = COMBAT_AI_MASK_FIRST_BIT;
    u32 bits = 0;
    army* currentArmy;

    for (armyIndex = 0; armyIndex < m_armyCount[IDX(side)]; armyIndex++) {
        currentArmy = m_armies[IDX(side)] + armyIndex;
        if (currentArmy != NULL
            && HAS(currentArmy->m_monster.flags.abilityFlags, MONSTER_ABILITY_FLAG_AI_EXCLUDED)
                   == 0
            && HAS(currentArmy->m_monster.flags.abilityFlags, MONSTER_ABILITY_FLAG_FLYING) != 0
            && currentArmy->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_BLIND)] == 0
            && currentArmy->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_PARALYZE)] == 0
            && currentArmy->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_PETRIFIED)] == 0
            && currentArmy->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_BERSERK)] == 0
            && currentArmy->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_HYPNOTIZE)] == 0)
            bits |= armyBit;
        armyBit <<= 1;
    }
    return bits;
}

VA(0x00417811, 0xa1)
i32 combatManager::GetAllMask(H2_ENUM_PARAM(CombatSide, i32) side) {
    i32 armyIndex = 0;
    u32 armyBit = COMBAT_AI_MASK_FIRST_BIT;
    u32 bits = 0;
    army* currentArmy;

    for (armyIndex = 0; armyIndex < m_armyCount[IDX(side)]; armyIndex++) {
        currentArmy = &m_armies[IDX(side)][armyIndex];
        if (currentArmy != NULL
            && HAS(currentArmy->m_monster.flags.abilityFlags, MONSTER_ABILITY_FLAG_AI_EXCLUDED)
                   == 0
            && currentArmy->m_quantity > 0)
            bits |= armyBit;
        armyBit <<= 1;
    }
    return bits;
}

VA(0x004178b2, 0x11d)
i32 combatManager::GetWalkerMask(H2_ENUM_PARAM(CombatSide, i32) side) {
    i32 armyIndex = 0;
    u32 armyBit = COMBAT_AI_MASK_FIRST_BIT;
    u32 bits = 0;
    army* currentArmy;

    for (armyIndex = 0; armyIndex < m_armyCount[IDX(side)]; armyIndex++) {
        currentArmy = m_armies[IDX(side)] + armyIndex;
        if (currentArmy != NULL
            && HAS(currentArmy->m_monster.flags.abilityFlags, MONSTER_ABILITY_FLAG_AI_EXCLUDED)
                   == 0
            && HAS(currentArmy->m_monster.flags.abilityFlags, MONSTER_ABILITY_FLAG_FLYING) == 0
            && (HAS(currentArmy->m_monster.flags.abilityFlags, MONSTER_ABILITY_FLAG_SHOOTER) == 0
                || currentArmy->m_monster.shots <= 0)
            && currentArmy->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_BLIND)] == 0
            && currentArmy->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_PARALYZE)] == 0
            && currentArmy->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_PETRIFIED)] == 0
            && currentArmy->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_BERSERK)] == 0
            && currentArmy->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_HYPNOTIZE)] == 0)
            bits |= armyBit;
        armyBit <<= 1;
    }
    return bits;
}

VA(0x004179cf, 0xc5)
i32 combatManager::GetOutOfItMask(H2_ENUM_PARAM(CombatSide, i32) side) {
    i32 idx = 0;
    u32 bitMask = COMBAT_AI_MASK_FIRST_BIT;
    u32 result = 0;
    army* currentArmy;

    for (idx = 0; idx < m_armyCount[IDX(side)]; idx++) {
        currentArmy =
            m_armies[IDX(side)] + idx;
        if (currentArmy != NULL
            && HAS(currentArmy->m_monster.flags.abilityFlags, MONSTER_ABILITY_FLAG_AI_EXCLUDED)
                   == 0
            && (currentArmy->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_BLIND)] != 0
                || currentArmy->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_PARALYZE)] != 0
                || currentArmy->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_PETRIFIED)] != 0))
            result |= bitMask;
        bitMask <<= 1;
    }
    return result;
}

VA(0x00417a94, 0xb6)
i32 combatManager::GetTraitorMask(H2_ENUM_PARAM(CombatSide, i32) side) {
    i32 armyIndex = 0;
    u32 armyBit = COMBAT_AI_MASK_FIRST_BIT;
    u32 bits = 0;
    army* currentArmy;

    for (armyIndex = 0; armyIndex < m_armyCount[IDX(side)]; armyIndex++) {
        currentArmy = &m_armies[IDX(side)][armyIndex];
        if (currentArmy != NULL
            && HAS(currentArmy->m_monster.flags.abilityFlags, MONSTER_ABILITY_FLAG_AI_EXCLUDED)
                   == 0
            && (currentArmy->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_BERSERK)] != 0
                || currentArmy->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_HYPNOTIZE)] != 0))
            bits |= armyBit;
        armyBit <<= 1;
    }
    return bits;
}

VA(0x00417b4a, 0x184)
i32 combatManager::GetBestArmy(H2_ENUM_PARAM(CombatSide, i32) side, i32 mask) {
    i32 armyIndex2 = 0;
    u32 armyBit = COMBAT_AI_MASK_FIRST_BIT;
    u32l bestStrength8 = 0;
    i32 best = COMBAT_AI_NO_ARMY;
    u32l strength8;

    for (armyIndex2 = 0; armyIndex2 < m_armyCount[IDX(side)]; armyIndex2++) {
        if ((mask & armyBit) != 0) {
            strength8 =
                (m_armies[IDX(side)] + armyIndex2)
                    ->Strength();
            if ((m_armies[IDX(side)] + armyIndex2)
                        ->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_BLIND)]
                    != 0
                || (m_armies[IDX(side)] + armyIndex2)
                           ->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_PARALYZE)]
                       != 0
                || (m_armies[IDX(side)] + armyIndex2)
                           ->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_PETRIFIED)]
                       != 0
                || (m_armies[IDX(side)] + armyIndex2)
                           ->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_BERSERK)]
                       != 0
                || (m_armies[IDX(side)] + armyIndex2)
                           ->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_HYPNOTIZE)]
                       != 0)
                strength8 >>= 1;
            if (strength8 > bestStrength8) {
                best = armyIndex2;
                bestStrength8 = strength8;
            }
        }
        armyBit <<= 1;
    }
    return best;
}

VA(0x00417cce, 0xa0)
i32 combatManager::GetWorstArmy(H2_ENUM_PARAM(CombatSide, i32) side, i32 mask) {
    i32 armyIndex = 0;
    u32 bit = COMBAT_AI_MASK_FIRST_BIT;
    u32l weakestStrength = COMBAT_AI_WORST_STRENGTH_LIMIT;
    i32 weakestArmy = COMBAT_AI_NO_ARMY;
    u32l force;

    for (armyIndex = 0; armyIndex < m_armyCount[IDX(side)]; armyIndex++) {
        if ((mask & bit) != 0) {
            force = m_armies[IDX(side)][armyIndex].Strength();
            if (force < weakestStrength) {
                weakestArmy = armyIndex;
                weakestStrength = force;
            }
        }
        bit <<= 1;
    }
    return weakestArmy;
}

VA(0x00417d6e, 0x12e)
i32 combatManager::GetClosestArmy(
    class army* currentArmy, H2_ENUM_PARAM(CombatSide, i32) side, i32 mask
) {
    i32 armyIndex = 0;
    army* target;
    u32 bitFlag = COMBAT_AI_MASK_FIRST_BIT;
    i32 bestValue = COMBAT_AI_CLOSEST_ARMY_LIMIT;
    i32 armyFound = COMBAT_AI_NO_ARMY;
    i32 val;

    for (armyIndex = 0; armyIndex < m_armyCount[IDX(side)]; armyIndex++) {
        if ((mask & bitFlag) != 0) {
            target = &m_armies[IDX(side)][armyIndex];
            val = gpSearchArray->QuickDistance(
                m_hexCells[currentArmy->m_hex].m_x,
                m_hexCells[currentArmy->m_hex].m_y,
                m_hexCells[target->m_hex].m_x,
                m_hexCells[target->m_hex].m_y
            );
            val = val * COMBAT_AI_DISTANCE_WEIGHT
                  - target->m_quantity * target->m_monster.hitPoints;
            if (val < bestValue) {
                armyFound = armyIndex;
                bestValue = val;
            }
        }
        bitFlag <<= 1;
    }
    return armyFound;
}

VA(0x00417e9c, 0xa6)
u32l combatManager::GetStrength(H2_ENUM_PARAM(CombatSide, i32) side, i32 mask) {
    i32 idx = 0;
    u32 bitMask = COMBAT_AI_MASK_FIRST_BIT;
    u32l totalStrength = 0;
    army* currentArmy;

    for (idx = 0; idx < m_armyCount[IDX(side)]; idx++) {
        if ((mask & bitMask) != 0) {
            currentArmy = &m_armies[IDX(side)][idx];
            if (currentArmy != NULL
                && HAS(currentArmy->m_monster.flags.abilityFlags, MONSTER_ABILITY_FLAG_AI_EXCLUDED)
                       == 0)
                totalStrength += currentArmy->Strength();
        }
        bitMask <<= 1;
    }
    return totalStrength;
}

VA(0x00417f42, 0x178)
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

VA(0x004180ba, 0x144)
i32 combatManager::AttemptAdjacentAttack(class army* currentArmy) {
    u32 availableMask4 =
        ~currentArmy->GetAttackMask(
            currentArmy->m_hex, ARMY_ATTACK_TARGET_ENEMY, ARMY_HEX_INVALID
        );
    u32 oneBit;
    u32 enemyMask;
    CombatHexDirection direction;
    i32 hexes;
    i32 enemyArmy;
    i32 destHex;

    if (availableMask4 == 0)
        return 0;

    oneBit = COMBAT_AI_MASK_FIRST_BIT;
    enemyMask = 0;
    for (direction = COMBAT_DIRECTION_NORTHEAST;
         IDX(direction) < COMBAT_AI_ATTACK_DIRECTION_COUNT;
         direction++) {
        if ((availableMask4 & oneBit) != 0
            && currentArmy->ValidAttack(
                currentArmy->m_hex,
                direction,
                ARMY_ATTACK_TARGET_ENEMY,
                ARMY_HEX_INVALID,
                &destHex
            )
            && destHex >= 0)
            enemyMask |= 1 << m_hexCells[destHex].m_occupantIndex;
        oneBit <<= 1;
    }
    if (currentArmy->m_monsterType == CREATURE_GHOST)
        enemyArmy = GetWorstArmy(OppositeCombatSide(m_currentSide), enemyMask);
    else
        enemyArmy = GetBestArmy(OppositeCombatSide(m_currentSide), enemyMask);
    if (enemyArmy != COMBAT_AI_NO_ARMY) {
        giNextAction = ACTION_MOVE;
        giNextActionGridIndex = m_armies[IDX(OppositeCombatSide(m_currentSide))][enemyArmy].m_hex;
        return 1;
    } else {
        return 0;
    }
}

VA(0x004181fe, 0x207)
i32 combatManager::WalkTowardArmyFront(
    class army* currentArmy, H2_ENUM_PARAM(CombatSide, i32) side, i32 mask
) {
    i32 armyIndex;
    i32 frontDelta;
    i32 frontHex;
    i32 oldSpeed;
    i32 pathFound;
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

VA(0x00418405, 0x204)
i32 combatManager::WalkTowardArmy(
    class army* currentArmy, H2_ENUM_PARAM(CombatSide, i32) side, i32 mask
) {
    i32 targetStack;
    i32 prevSpeed;
    i32 routeGot;
    i32 atkMask;
    i32 movement;
    i32 pathNdx;
    army* targetPtr;
    i32 targetSquare;
    i32 path;

    targetStack = GetClosestArmy(currentArmy, side, mask);

    if (targetStack == COMBAT_AI_NO_ARMY)
        return 0;

    targetPtr = &m_armies[IDX(side)][targetStack];
    targetSquare = targetPtr->m_hex;
    currentArmy->m_targetSide = side;
    currentArmy->m_targetIndex = targetStack;
    atkMask =
        currentArmy->GetAttackMask(
            currentArmy->m_hex, ARMY_ATTACK_TARGET_ASSIGNED, ARMY_HEX_INVALID
        );
    if (atkMask != COMBAT_AI_ALL_ATTACK_DIRECTIONS) {
        giNextAction = ACTION_WAIT;
        return 1;
    }

    prevSpeed = currentArmy->m_monster.speed;
    currentArmy->m_monster.speed = COMBAT_AI_UNLIMITED_PATH_SPEED;
    routeGot = gpSearchArray->FindCombatPath(
        currentArmy->m_hex,
        targetSquare,
        currentArmy,
        COMBAT_AI_PATH_TO_TARGET,
        0
    );
    if (routeGot == 0
        && HAS(targetPtr->m_monster.flags.abilityFlags, MONSTER_ABILITY_FLAG_WIDE) != 0) {
        switch (targetPtr->m_facing) {
            case ARMY_FACING_LEFT:
                targetSquare--;
                break;
            case ARMY_FACING_RIGHT:
                targetSquare++;
                break;
        }
        if (targetSquare != COMBAT_AI_NO_ARMY)
            routeGot = gpSearchArray->FindCombatPath(
                currentArmy->m_hex,
                targetSquare,
                currentArmy,
                COMBAT_AI_PATH_TO_TARGET,
                0
            );
    }
    currentArmy->m_monster.speed = static_cast<i8>(prevSpeed);
    if (gpSearchArray->m_pathLength > 1) {
        giNextAction = ACTION_MOVE;
        movement = currentArmy->m_monster.speed;
        pathNdx = gpSearchArray->m_pathLength - 1;
        giNextActionGridIndex = currentArmy->m_hex;
        while (pathNdx >= 1 && movement != 0) {
            giNextActionGridIndex = currentArmy->GetAdjacentCellIndex(
                giNextActionGridIndex,
                static_cast<CombatHexDirection>(
                    gpSearchArray->m_storage.aiPath.directions[pathNdx]
                )
            );
            pathNdx--;
            movement--;
            if (giNextActionGridIndex > 0 && bIsMoatSlowed[giNextActionGridIndex] != 0)
                movement = 0;
        }
        return 1;
    }
    return 0;
}
