// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\CMBTMGR.OBJ   from: (directly linked into exe)
// functions: 39   data: 7
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <stdio.h>
#include <string.h>
#include <BASE/Misc.h>
#include <EDITOR/mapcell.h>
#include <SOURCE/advManager.h>
#include <SOURCE/armyGroup.h>
#include <SOURCE/combatManager.h>
#include <SOURCE/CMBTMGR.h>
#include <SOURCE/KB.h>
#include <SOURCE/town.h>

VA(0x0048fd50, 0x1ba)
combatManager::combatManager(void)
{
    m_unknownF373 = -1;
    m_currentArmySide = COMBAT_DEFENDER_SIDE;
    m_currentSide = COMBAT_DEFENDER_SIDE;
    m_limitCreatureHex = 0;
    m_limitCreature = 0;
    m_showArmyQuantities = 1;
    m_unknownF2CF = 0;
    m_unknownF35B = 0;
    m_unknownF353 = -1;
    m_unknownF34F = m_unknownF353;
    m_catapultFrame = m_unknownF34F;
    m_unknownF317 = m_catapultFrame;
    m_unknownF337[COMBAT_ATTACKER_SIDE] = 0;
    m_unknownF337[COMBAT_DEFENDER_SIDE] =
        m_unknownF337[COMBAT_ATTACKER_SIDE];
    m_inCastleCombat = 0;
    m_mouseGridHex = -1;
    m_combatWindowOpen = 0;
    strcpy(m_previousCombatMessage, "");
    strcpy(m_currentCombatMessage, "");
}

VA(0x0048ff0a, 0x128)
void combatManager::CombineGroups(armyGroup *sourceGroup,
                                  armyGroup *targetGroup)
{
    if (sourceGroup == 0 || targetGroup == 0)
        return;

    int sourceIndex;
    for (sourceIndex = 0; sourceIndex < ARMY_GROUP_SLOT_COUNT; sourceIndex++) {
        if (targetGroup->IsMember(sourceGroup->m_creatureTypes[sourceIndex])) {
            targetGroup->Add(sourceGroup->m_creatureTypes[sourceIndex],
                             sourceGroup->m_creatureCounts[sourceIndex],
                             ARMY_GROUP_EMPTY_SLOT);
            sourceGroup->Dismiss(sourceIndex);
        }
    }

    for (sourceIndex = 0; sourceIndex < ARMY_GROUP_SLOT_COUNT; sourceIndex++) {
        if (sourceGroup->m_creatureTypes[sourceIndex] !=
            ARMY_GROUP_EMPTY_SLOT) {
            int targetIndex;
            for (targetIndex = 0; targetIndex < ARMY_GROUP_SLOT_COUNT;
                 targetIndex++) {
                if (targetGroup->m_creatureTypes[targetIndex] ==
                    ARMY_GROUP_EMPTY_SLOT) {
                    targetGroup->Add(
                        sourceGroup->m_creatureTypes[sourceIndex],
                        sourceGroup->m_creatureCounts[sourceIndex], targetIndex);
                    sourceGroup->Dismiss(sourceIndex);
                }
            }
        }
    }
}

VA(0x00490032, 0x5f2)
void combatManager::SetupCombat(int mapX, int mapY, hero *attackerHero,
                                armyGroup *attackerGroup, town *defenderTown,
                                hero *defenderHero,
                                armyGroup *defenderGroup, int combatX,
                                int combatY, int randomSeed)
{
    giSeed = randomSeed;
    SRand(combatX * COMBAT_RANDOM_X_MULTIPLIER + combatY);
    m_combatX = combatX;
    m_combatY = combatY;

    if (mapX >= 0 && mapY >= 0)
        m_battlefieldCell = gpAdvManager->GetCell(mapX, mapY);
    else
        m_battlefieldCell = 0;

    m_terrainType = giGroundToTerrain[m_battlefieldCell->tile];
    sprintf(m_battlefieldBackgroundName, GetBackgroundName());

    if (attackerHero != 0) {
        m_playerId[COMBAT_ATTACKER_SIDE] = attackerHero->m_owner;
        attackerGroup = &attackerHero->m_army;
    } else {
        m_playerId[COMBAT_ATTACKER_SIDE] = -1;
    }

    if (defenderHero != 0) {
        m_playerId[COMBAT_DEFENDER_SIDE] = defenderHero->m_owner;
        defenderGroup = &defenderHero->m_army;
    } else if (defenderTown != 0) {
        m_playerId[COMBAT_DEFENDER_SIDE] = defenderTown->m_owner;
        defenderGroup = &defenderTown->m_army;
    } else {
        m_playerId[COMBAT_DEFENDER_SIDE] = -1;
    }

    int side;
    for (side = COMBAT_ATTACKER_SIDE; side < COMBAT_SIDE_COUNT_DRAWING; side++) {
        if (m_playerId[side] >= 0)
            m_networkArmyPresent[side] = gbHumanPlayer[m_playerId[side]];
        else
            m_networkArmyPresent[side] = 0;

        if (side == COMBAT_ATTACKER_SIDE)
            m_heroes[side] = attackerHero;
        else
            m_heroes[side] = defenderHero;

        if (m_heroes[side] != 0)
            m_heroes[side]->m_isCaptain = 0;

        if (side == COMBAT_ATTACKER_SIDE)
            m_armyGroups[side] = attackerGroup;
        else
            m_armyGroups[side] = defenderGroup;

        m_unknownF323[side] = 1;
        m_unknownF31B[side] = m_unknownF323[side];
        if (m_heroes[side] != 0 &&
            m_heroes[side]->HasArtifact(COMBAT_ARTIFACT_BALLISTA)) {
            m_unknownF323[side] = 2;
            m_unknownF31B[side] = m_unknownF323[side];
        }
        if (m_heroes[side] != 0 &&
            m_heroes[side]->m_secondarySkills[HERO_SKILL_BALLISTICS] >=
                HERO_SKILL_LEVEL_ADVANCED) {
            m_unknownF31B[side]++;
            m_unknownF323[side]++;
        }
        m_unknownF32B[side] = 1;
        m_visitingHeroPresent[side] = 0;
        m_heroCastSpell[side] = 0;
    }

    m_drawbridgeBackgroundVisible = 0;
    if (defenderTown != 0) {
        if (defenderTown->m_occupyingHeroId != -1) {
            m_armyGroups[COMBAT_DEFENDER_SIDE] =
                &m_heroes[COMBAT_DEFENDER_SIDE]->m_army;
            CombineGroups(&defenderTown->m_army,
                          &m_heroes[COMBAT_DEFENDER_SIDE]->m_army);
            m_visitingHeroPresent[COMBAT_DEFENDER_SIDE] = 1;
        } else {
            m_visitingHeroPresent[COMBAT_DEFENDER_SIDE] = 0;
        }

        if (defenderTown->m_buildings & TOWN_BUILDING_CASTLE)
            m_inCastleCombat = 1;
        else
            m_inCastleCombat = 0;

        if (m_inCastleCombat != 0) {
            if (defenderTown->m_buildings & TOWN_BUILDING_MOAT)
                m_drawbridgeBackgroundVisible = 1;
            else
                m_drawbridgeBackgroundVisible = 0;
        }

        m_drawbridgeState = COMBAT_CASTLE_GATE_OPEN;
        m_combatTowns[COMBAT_DEFENDER_SIDE] = defenderTown;
        m_originalCombatTown = m_combatTowns[COMBAT_DEFENDER_SIDE];

        if (m_heroes[COMBAT_DEFENDER_SIDE] == 0 &&
            (defenderTown->m_buildings & TOWN_BUILDING_CAPTAIN_QUARTERS)) {
            m_heroes[COMBAT_DEFENDER_SIDE] = &m_captain;
            memset(&m_captain, 0, sizeof(m_captain));
            for (side = 0; side < HERO_PRIMARY_STAT_COUNT; side++)
                m_captain.m_primaryStats[side] =
                    captainStats[m_combatTowns[COMBAT_DEFENDER_SIDE]->m_type]
                                [side];
            m_captain.m_spellPoints =
                m_captain.Stats(HERO_PRIMARY_KNOWLEDGE) *
                COMBAT_CAPTAIN_SPELL_POINT_MULTIPLIER;
            m_captain.m_cursorType =
                m_combatTowns[COMBAT_DEFENDER_SIDE]->m_type;
            m_captain.m_portrait =
                m_combatTowns[COMBAT_DEFENDER_SIDE]->m_type +
                COMBAT_CAPTAIN_PORTRAIT_BASE;
            strcpy(m_captain.m_name, "Captain");
            for (side = 0; side < ARMY_GROUP_SLOT_COUNT; side++)
                m_captain.m_army.m_creatureTypes[side] =
                    ARMY_GROUP_EMPTY_SLOT;
            for (side = 0; side < HERO_ARTIFACT_SLOT_COUNT; side++)
                m_captain.m_artifacts[side] = ARMY_GROUP_EMPTY_SLOT;
            m_captain.m_artifacts[0] = COMBAT_CAPTAIN_MAGIC_BOOK;
            m_combatTowns[COMBAT_DEFENDER_SIDE]->GiveSpells(&m_captain);
            m_captain.m_isCaptain = 1;
        }
    } else {
        m_inCastleCombat = 0;
        m_combatTowns[COMBAT_DEFENDER_SIDE] = 0;
    }
    m_combatTowns[COMBAT_ATTACKER_SIDE] = 0;
}

VA(0x00490624, 0x279)
void combatManager::InitNonVisualVars(void)
{
    m_gridSelectionDisabled = 0;
    m_nonVisualCombat = 0;
    int side;
    for (side = COMBAT_ATTACKER_SIDE; side < COMBAT_SIDE_COUNT_DRAWING; side++) {
        m_spellPower[side] = 0;
        if (m_heroes[side] != 0)
            m_spellPower[side] = m_heroes[side]->Stats(HERO_PRIMARY_SPELL_POWER);
        if (m_combatTowns[side] != 0 &&
            m_combatTowns[side]->m_type == TOWN_TYPE_NECROMANCER &&
            (m_combatTowns[side]->m_buildings & TOWN_BUILDING_SHRINE))
            m_spellPower[side] += 2;
    }

    m_heroOverlayFrame[COMBAT_ATTACKER_SIDE] = 0;
    m_heroOverlayFrame[COMBAT_DEFENDER_SIDE] = 3;
    m_unknownF377 = 0;
    m_unknownF37B = 0;
    m_combatResult = 3;
    m_deathFlags[4] = m_deathFlags[5] = 0;
    m_deathFlags[6] = m_deathFlags[7] = 0;
    m_deathFlags[0] = m_deathFlags[1] = 0;
    m_deathFlags[2] = m_deathFlags[3] = 0;
    m_eagleEyeSpell[COMBAT_ATTACKER_SIDE] = -1;
    m_eagleEyeSpell[COMBAT_DEFENDER_SIDE] = -1;
    giNextAction = 0;
    m_unknown351D[0] = 0;
    m_unknown351D[1] = 0;
    m_selectedHex = -1;
    m_limitCreatureHex = -1;
    m_unknownF2CB = COMBAT_INVALID_HISTORY_INDEX;
    m_currentSide = COMBAT_DEFENDER_SIDE;
    m_currentArmySide = COMBAT_DEFENDER_SIDE;
    m_unknownF2AB = COMBAT_INITIAL_COMMAND;
    gbRetreatWin = 0;
    gbCombatSurrender = 0;
    m_unknown351F = 0;
    m_unknown3523 = 0;
    m_limitCreature = 1;
    m_obstacleIcons[8] = 0;
    SetupAdjacencyArray();
    GenerateMap();
    LoadArmies();
}

// @early-stop
// raw-byte exact outside relocated jump operand/table at +0x165..+0x180; local-label addends only
VA(0x0049089d, 0x203)
void combatManager::SetupAdjacencyArray(void)
{
    int destinationHex = 0;
    int sourceHex;
    for (sourceHex = 0; sourceHex < COMBAT_HEX_COUNT; sourceHex++) {
        int rowIndex = sourceHex / COMBAT_GRID_ROW_LENGTH;
        int direction;
        for (direction = 0; direction < COMBAT_AI_ADJACENT_DIRECTION_COUNT;
             direction++) {
            if (sourceHex % COMBAT_GRID_ROW_LENGTH == 0 ||
                sourceHex % COMBAT_GRID_ROW_LENGTH ==
                    COMBAT_GRID_ROW_LENGTH - 1) {
                m_adjacency[sourceHex][direction] = -1;
            } else {
                switch (direction) {
                case 0:
                    if (rowIndex & 1)
                        destinationHex = sourceHex - COMBAT_GRID_ROW_LENGTH;
                    else
                        destinationHex =
                            sourceHex - (COMBAT_GRID_ROW_LENGTH - 1);
                    break;
                case 2:
                    if (rowIndex & 1)
                        destinationHex = sourceHex + COMBAT_GRID_ROW_LENGTH;
                    else
                        destinationHex =
                            sourceHex + COMBAT_GRID_ROW_LENGTH + 1;
                    break;
                case 3:
                    if (rowIndex & 1)
                        destinationHex =
                            sourceHex + COMBAT_GRID_ROW_LENGTH - 1;
                    else
                        destinationHex = sourceHex + COMBAT_GRID_ROW_LENGTH;
                    break;
                case 5:
                    if (rowIndex & 1)
                        destinationHex =
                            sourceHex - COMBAT_GRID_ROW_LENGTH - 1;
                    else
                        destinationHex = sourceHex - COMBAT_GRID_ROW_LENGTH;
                    break;
                case 1:
                    destinationHex = sourceHex + 1;
                    break;
                case 4:
                    destinationHex = sourceHex - 1;
                    break;
                }

                if (destinationHex % COMBAT_GRID_ROW_LENGTH == 0 ||
                    destinationHex % COMBAT_GRID_ROW_LENGTH ==
                        COMBAT_GRID_ROW_LENGTH - 1 ||
                    destinationHex < 0 || destinationHex >= COMBAT_HEX_COUNT)
                    m_adjacency[sourceHex][direction] = -1;
                else
                    m_adjacency[sourceHex][direction] =
                        static_cast<signed char>(destinationHex);
            }
        }
    }
}

VA(0x00490aa0, 0x43f)
int combatManager::Open(int) { return 0; }

VA(0x00490edf, 0x3d6)
void combatManager::Close(void) {}

VA(0x004912b5, 0x38c)
void combatManager::UpdateArmyGroup(int) {}

VA(0x00491641, 0x365)
void combatManager::GenerateMap(void) {}

VA(0x004919a6, 0x224)
char * combatManager::GetBackgroundName(void) { return 0; }

VA(0x00491bca, 0x210)
int combatManager::MoreTreesNear(void) { return 0; }

VA(0x00491dda, 0x3e7)
void combatManager::LoadIcons(void) {}

VA(0x004921c1, 0x124)
void combatManager::FreeIcons(void) {}

VA(0x004922e5, 0x36d)
void combatManager::LoadArmies(void) {}

VA(0x00492652, 0xdb)
void combatManager::FreeArmies(void) {}

VA(0x0049272d, 0x1e2)
int combatManager::GetGridIndex(int, int) { return 0; }

VA(0x0049290f, 0x1eb)
void combatManager::CheckApplyGoodMorale(int, int) {}

VA(0x00492afa, 0x1cd)
int combatManager::CheckApplyBadMorale(int, int) { return 0; }

VA(0x00492cc7, 0x382)
int combatManager::GetNextArmy(int) { return 0; }

VA(0x00493049, 0xd6)
int combatManager::IsWinner(int) { return 0; }

VA(0x0049311f, 0x100e)
void combatManager::CatAttack(int) {}

VA(0x0049412d, 0x74f)
void combatManager::KeepAttack(int) {}

VA(0x0049487c, 0x17b)
int combatManager::ExperienceValueOfStack(int) { return 0; }

VA(0x004949f7, 0x88)
void combatManager::ResetHitByCreature(void) {}

VA(0x00494a7f, 0x36)
int ValidHex(int) { return 0; }

VA(0x00494ab5, 0x16)
void combatManager::SaveCombatBorder(void) {}

VA(0x00494acb, 0x16)
void combatManager::DrawCombatBorder(void) {}

VA(0x00494ae1, 0x4d8)
void combatManager::SetupAndLoadObstacles(void) {}

VA(0x00494fb9, 0x2a1)
void combatManager::MakeCreaturesVanish(void) {}

VA(0x0049525a, 0xbd)
void combatManager::LowerDoor(void) {}

VA(0x00495317, 0xe6)
void combatManager::RaiseDoor(void) {}

VA(0x004953fd, 0x84)
void combatManager::TestRaiseDoor(void) {}

VA(0x00495481, 0xd8)
int combatManager::InCastle(int) { return 0; }

VA(0x00495559, 0x346)
int combatManager::ShotIsThroughWall(int, int, int) { return 0; }

VA(0x0049589f, 0x52e)
void combatManager::ShootMissile(int, int, int, int, float *, class icon *) {}

VA(0x00495dcd, 0xf2)
void combatManager::CombatSystemOptions(void) {}

VA(0x00495ebf, 0x1ea)
void UpdateCombatSystemOptions(int) {}

VA(0x004960a9, 0x39a)
int CombatSystemOptionsHandler(struct tag_message &) { return 0; }


// ===== vtable combatManager : public baseManager  (3 slots) =====
//  [ 0] VA(0x00490aa0, 0x43f)  int combatManager::Open(int)   <- override (implements baseManager pure virtual)
//  [ 1] VA(0x00490edf, 0x3d6)  void combatManager::Close(void)   <- override (implements baseManager pure virtual)
//  [ 2] VA(0x0042a6d0, 0x36d)  int combatManager::Main(struct tag_message &)   <- override (implements baseManager pure virtual)

// ---- vtables (compiler-emitted; census) ----
VTBL(combatManager, 0x004eb898);

// ---- globals (definitions, RVA order) ----
DATA(0x004f8900) int bInHighMoraleBonus;
DATA(0x004f8904) int giSeed;
DATA(0x004f8c1c) unsigned char *wallHex;
DATA(0x00528588) int bMouseWasVis;
DATA(0x0052858c) class heroWindow *CSPanel;
DATA(0x00528590) int bCPrefsChanged;
