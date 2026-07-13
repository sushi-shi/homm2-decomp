// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\AI.OBJ   from: (directly linked into exe)
// functions: 19   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <SOURCE/CMBTMGR.h>
#include <SOURCE/KB.h>
#include <SOURCE/PHILAI.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/armyGroup.h>
#include <SOURCE/combatManager.h>
#include <SOURCE/game.h>
#include <SOURCE/hero.h>
#include <SOURCE/philAI.h>
#include <SOURCE/town.h>

VA(0x004c0790, 0x8d7)
int combatManager::AICheckRetreat(void)
{
    if (m_combatTowns[m_currentSide] != 0)
        return 0;
    if (m_heroes[m_currentSide] == 0)
        return 0;
    if (m_heroes[m_currentSide]->m_isCaptain != 0)
        return 0;
    if (gpGame->m_victoryConditionType == COMBAT_AI_DEFEAT_HERO_VICTORY &&
        static_cast<unsigned char>(m_heroes[m_currentSide]->m_id) ==
            gpGame->m_victoryConditionValue)
        return 0;
    if (gpGame->m_players[m_heroes[m_currentSide]->m_owner].townCount == 0 &&
        gpGame->m_players[m_heroes[m_currentSide]->m_owner].heroCount ==
            COMBAT_AI_LAST_HERO_COUNT)
        return 0;

    hero retreatHero8;
    armyGroup retreatArmy6;
    armyGroup *armyGroupPtr1;
    hero *heroPtr9;
    int side9;
    int armyIndex36;
    int groupCount8;
    int strengths1[COMBAT_AI_SIDE_COUNT];
    int artifactValues[COMBAT_AI_SIDE_COUNT];
    float experienceBonus17;
    int artifactValue15;
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
            if (m_armies[side9][armyIndex36].IsAlive()) {
                armyGroupPtr1->m_creatureTypes[groupCount8] =
                    static_cast<signed char>(m_armies[side9][armyIndex36].m_monsterType);
                if ((m_armies[side9][armyIndex36].m_monster.flags.all &
                     COMBAT_ARMY_FLAG_FULL_AI_QUANTITY) != 0) {
                    armyGroupPtr1->m_creatureCounts[groupCount8] =
                        static_cast<short>(m_armies[side9][armyIndex36].m_quantity);
                } else {
                    armyGroupPtr1->m_creatureCounts[groupCount8] = static_cast<short>(
                        static_cast<int>(m_armies[side9][armyIndex36].m_quantity *
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
            strengths1[side9] = static_cast<int>(
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
        static_cast<int>(strengths1[COMBAT_DEFENDER_SIDE - m_currentSide] *
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

VA(0x004c1067, 0x129c)
void combatManager::DoCompAI(int)
{
    unsigned long shooterStrengths37[COMBAT_AI_SIDE_COUNT];
    int enemyStronger3;
    int attackType3;
    unsigned int shooterMasks6[COMBAT_AI_SIDE_COUNT];
    unsigned int traitorMasks13[COMBAT_AI_SIDE_COUNT];
    unsigned int walkerMasks15[COMBAT_AI_SIDE_COUNT];
    int enemySide12;
    unsigned long totalArmyStrength11;
    int adjacentDirection6;
    army *currentArmy9;
    int targetArmy16;
    combatManager *combat2;
    int fifthArmyStrength5;
    unsigned int mirrorMasks7[COMBAT_AI_SIDE_COUNT];
    unsigned long enemyShooterStrength18;
    unsigned int flyerMasks14[COMBAT_AI_SIDE_COUNT];
    unsigned int outOfItMasks15[COMBAT_AI_SIDE_COUNT];
    unsigned long currentShooterStrength5;
    int archerLevel18;
    int strongEnough16;
    int numArchers6;
    int goldenBowBonus;
    int towerStrength4;
    town *castle2;
    unsigned char castleBoundary27[COMBAT_AI_CASTLE_BOUNDARY_COUNT];
    int extraArchers29;
    hexcell *castleCell5;
    int castleRow29;
    unsigned int adjacentHex8;
    unsigned int castleTargetHex2;

    m_limitCreature = 0;
    currentArmy9 = &m_armies[m_currentArmySide][m_currentArmyIndex];
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
        m_currentSide, shooterMasks6[m_currentSide] | flyerMasks14[m_currentSide] |
                           walkerMasks15[m_currentSide] | outOfItMasks15[m_currentSide] |
                           traitorMasks13[m_currentSide]);
    fifthArmyStrength5 =
        static_cast<int>(totalArmyStrength11 + COMBAT_AI_STRENGTH_ROUNDING) /
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
                    static_cast<int>(currentShooterStrength5) /
                    COMBAT_AI_SIDE_COUNT;
            else
                enemyShooterStrength18 =
                    static_cast<int>(enemyShooterStrength18) /
                    COMBAT_AI_SIDE_COUNT;
        }
        if (m_wallStates[COMBAT_AI_WALL_GATE] == 0) {
            castle2 = m_combatTowns[COMBAT_DEFENDER_SIDE];
            castle2->CalcNumLevelArchers(&numArchers6, &archerLevel18);
            extraArchers29 = 0;
            if (m_wallStates[COMBAT_AI_WALL_LEFT_TOWER] ==
                COMBAT_AI_WALL_INTACT)
                extraArchers29 += numArchers6 / COMBAT_AI_SIDE_COUNT;
            if (m_wallStates[COMBAT_AI_WALL_RIGHT_TOWER] ==
                COMBAT_AI_WALL_INTACT)
                extraArchers29 += numArchers6 / COMBAT_AI_SIDE_COUNT;
            numArchers6 += extraArchers29;
            towerStrength4 = static_cast<int>(
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
        currentShooterStrength5 = static_cast<int>(
            static_cast<int>(currentShooterStrength5) *
            gfSSArcheryMod[m_heroes[m_currentSide]->m_secondarySkills[HERO_SKILL_ARCHERY]]);
    }
    if (m_heroes[COMBAT_DEFENDER_SIDE - m_currentSide] != 0) {
        enemyShooterStrength18 = static_cast<int>(
            static_cast<int>(enemyShooterStrength18) *
            gfSSArcheryMod[m_heroes[COMBAT_DEFENDER_SIDE - m_currentSide]
                               ->m_secondarySkills[HERO_SKILL_ARCHERY]]);
    }
    if (static_cast<int>(currentShooterStrength5) >
        static_cast<int>(totalArmyStrength11 + COMBAT_AI_STRENGTH_ROUNDING) /
            COMBAT_AI_STRENGTH_FRACTION)
        strongEnough16 = 1;
    if (static_cast<int>(enemyShooterStrength18) >
        static_cast<int>(currentShooterStrength5))
        enemyStronger3 = 1;

    if ((currentArmy9->m_monster.flags.all & COMBAT_ARMY_FLAG_SHOOTER) != 0) {
        if (currentArmy9->m_monster.shots < 1)
            attackType3 = COMBAT_AI_ATTACK_WALK;
        else
            attackType3 = COMBAT_AI_ATTACK_SHOOT;
    } else if ((currentArmy9->m_monster.flags.all & COMBAT_ARMY_FLAG_FLYER) != 0) {
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
            giNextActionGridIndex = m_armies[enemySide12][targetArmy16].m_hex;
            goto finish;
        }
        targetArmy16 = GetBestArmy(enemySide12, shooterMasks6[enemySide12]);
        if (targetArmy16 != -1) {
            giNextAction = COMBAT_AI_ACTION_MOVE;
            giNextActionGridIndex = m_armies[enemySide12][targetArmy16].m_hex;
            goto finish;
        }
        targetArmy16 = GetBestArmy(enemySide12, flyerMasks14[enemySide12]);
        if (targetArmy16 != -1) {
            giNextAction = COMBAT_AI_ACTION_MOVE;
            giNextActionGridIndex = m_armies[enemySide12][targetArmy16].m_hex;
            goto finish;
        }
        if (walkerMasks15[enemySide12] != 0) {
            targetArmy16 = GetClosestArmy(currentArmy9, enemySide12, walkerMasks15[enemySide12]);
            if (targetArmy16 != -1) {
                giNextAction = COMBAT_AI_ACTION_MOVE;
                giNextActionGridIndex = m_armies[enemySide12][targetArmy16].m_hex;
                goto finish;
            }
        }
        targetArmy16 = GetBestArmy(enemySide12, outOfItMasks15[enemySide12]);
        if (targetArmy16 != -1) {
            giNextAction = COMBAT_AI_ACTION_MOVE;
            giNextActionGridIndex = m_armies[enemySide12][targetArmy16].m_hex;
            goto finish;
        }
        targetArmy16 = GetBestArmy(enemySide12, traitorMasks13[enemySide12]);
        if (targetArmy16 != -1) {
            giNextAction = COMBAT_AI_ACTION_MOVE;
            giNextActionGridIndex = m_armies[enemySide12][targetArmy16].m_hex;
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

        combat2 = gpCombatManager;
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
            castleCell5 = &combat2->m_hexCells[castleTargetHex2];
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
        adjacentDirection6 = 0;
        while (adjacentDirection6 < COMBAT_AI_ADJACENT_DIRECTION_COUNT) {
            adjacentHex8 = currentArmy9->GetAdjacentCellIndex(
                giNextActionGridIndex, adjacentDirection6);
            if (adjacentHex8 > 0 && adjacentHex8 < COMBAT_HEX_COUNT &&
                gpCombatManager->m_hexCells[adjacentHex8].m_occupantSide == enemySide12) {
                giNextActionGridIndex = adjacentHex8;
                break;
            }
            adjacentDirection6++;
        }
    }
}

VA(0x004c2303, 0xc9)
float combatManager::GetModLichDamage(class army *, float) { return 0; }

VA(0x004c23cc, 0x32e)
void combatManager::DoLichShot(class army *) {}

VA(0x004c26fa, 0x131)
int combatManager::GetShooterMask(int) { return 0; }

VA(0x004c282b, 0xc0)
int combatManager::GetMirrorImageMask(int) { return 0; }

VA(0x004c28eb, 0x11f)
int combatManager::GetFlyerMask(int) { return 0; }

VA(0x004c2a0a, 0xc0)
int combatManager::GetAllMask(int) { return 0; }

VA(0x004c2aca, 0x141)
int combatManager::GetWalkerMask(int) { return 0; }

VA(0x004c2c0b, 0xe9)
int combatManager::GetOutOfItMask(int) { return 0; }

VA(0x004c2cf4, 0xd6)
int combatManager::GetTraitorMask(int) { return 0; }

VA(0x004c2dca, 0x1f1)
int combatManager::GetBestArmy(int, int) { return 0; }

VA(0x004c2fbb, 0xc2)
int combatManager::GetWorstArmy(int, int) { return 0; }

VA(0x004c307d, 0x16f)
int combatManager::GetClosestArmy(class army *, int, int) { return 0; }

VA(0x004c31ec, 0xc1)
unsigned long int combatManager::GetStrength(int, int) { return 0; }

VA(0x004c32ad, 0x1bb)
int combatManager::AttemptAttack(class army *, int, int) { return 0; }

VA(0x004c3468, 0x182)
int combatManager::AttemptAdjacentAttack(class army *) { return 0; }

VA(0x004c35ea, 0x240)
int combatManager::WalkTowardArmyFront(class army *, int, int) { return 0; }

VA(0x004c382a, 0x244)
int combatManager::WalkTowardArmy(class army *, int, int) { return 0; }
