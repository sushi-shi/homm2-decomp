// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\CMBTMGR.OBJ   from: (directly linked into exe)
// functions: 39   data: 7
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <_globals_model.h>
#include <BASE/bitmap.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/inputManager.h>
#include <BASE/Misc.h>
#include <BASE/mouseManager.h>
#include <BASE/palette.h>
#include <BASE/resourceManager.h>
#include <BASE/soundManager.h>
#include <BASE/WINMGR.h>
#include <EDITOR/mapcell.h>
#include <SOURCE/advManager.h>
#include <SOURCE/armyGroup.h>
#include <SOURCE/combatManager.h>
#include <SOURCE/CMBTMGR.h>
#include <SOURCE/game.h>
#include <SOURCE/KB.h>
#include <SOURCE/kbwin.h>
#include <SOURCE/town.h>
#include <SOURCE/X_GLOBAL.h>

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
    m_sideRetreated[COMBAT_ATTACKER_SIDE] = 0;
    m_sideRetreated[COMBAT_DEFENDER_SIDE] = 0;
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
    m_currentSpeed = COMBAT_INITIAL_COMMAND;
    gbRetreatWin = 0;
    gbCombatSurrender = 0;
    m_sideDefeated[COMBAT_ATTACKER_SIDE] = 0;
    m_sideDefeated[COMBAT_DEFENDER_SIDE] = 0;
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
int combatManager::Open(int openFlags)
{
    LogStr("Op1");
    memcpy(m_savedPalette, gPalette->m_data, COMBAT_PALETTE_DATA_SIZE);
    gpMouseManager->m_forcePointerUpdate = 1;
    int savedShowMouseHex = gbShowCombatMouseHex;
    gbShowCombatMouseHex = 0;
    m_previousCombatMessageExpiration = 0;
    m_combatMessageExpiration = 0;
    m_combatMessagePending = 0;
    m_combatWindowOpen = 0;
    gpSoundManager->PlayAmbientMusic(-1, 0, -1);
    m_combatBuffer = new bitmap(0, COMBAT_BACKGROUND_COPY_WIDTH,
                                COMBAT_BACKGROUND_COPY_HEIGHT);
    m_backgroundBuffer =
        new bitmap(0, COMBAT_SCREEN_WIDTH, COMBAT_AREA_HEIGHT);
    m_mouseGridBuffer = 0;
    m_smallViewLastX[COMBAT_ATTACKER_SIDE] = -1;
    m_smallViewLastX[COMBAT_DEFENDER_SIDE] = -1;
    memset(m_gridState, 0, sizeof(m_gridState));
    LoadIcons();
    InitNonVisualVars();
    SetupAndLoadObstacles();
    memset(m_previousGridState, 0, sizeof(m_previousGridState));
    GetNextArmy(COMBAT_ATTACKER_SIDE);
    m_backgroundDrawn = 0;

    LogStr("Op2");
    SAMPLE2 preBattleSample = NULL_SAMPLE2;
    preBattleSample = LoadPlaySample("PREBATTL.82M");
    gpWindowManager->FadeScreen(1, 8, 0);
    giCycleType = m_colorCycleType;
    CycleColors(1);
    CycleColors(1);
    gCurLoadedSpellIcon = 0;
    gCurLoadedSpellEffect = -1;
    gpMouseManager->m_forcePointerUpdate = 0;
    gpMouseManager->SetPointer("cmbtmous.mse", 6, -999);
    bMouseWasVis = gpMouseManager->IsVis();
    gpMouseManager->ShowColorPointer();
    m_combatWindow = new heroWindow(0, 0, "cmbtwin.bin");
    if (m_combatWindow == 0)
        MemError();
    gpWindowManager->AddWindow(m_combatWindow, -1, 1);
    m_combatWindowOpen = 1;
    DrawFrame(1, 0, 0, 0, 0x4b, 1, 1);
    glTimers[0] = KBTickCount();
    m_combatPalette = gpResourceManager->GetPalette("kb.pal");
    KBChangeMenu(hmnuCmbt);
    CombatMessage("", 1, 1, 0);
    gbShowCombatMouseHex = savedShowMouseHex;
    if (m_combatPalette->m_data != gpBufferPalette->m_data)
        memmove(m_combatPalette->m_data, gpBufferPalette->m_data,
                COMBAT_PALETTE_DATA_SIZE);
    gpWindowManager->FadeScreen(0, 8, m_combatPalette);
    gbLimitedCombatUpdatePalette = 1;
    WaitEndSample(preBattleSample, -1);

    LogStr("Op3");
    gpSoundManager->SwitchAmbientMusic(SRandom(2, 4));
    gCombatCycleTimer = KBTickCount();
    ResetCycleTimers();
    LogStr("Op4");
    gpInputManager->Flush();
    ResetMouse();
    field_0xc = 0x200;
    field_0x10 = openFlags;
    m_active = 1;
    strcpy(name, "combatManager");
    LogStr("Op5");
    return 0;
}

VA(0x00490edf, 0x3d6)
void combatManager::Close(void)
{
    gpSoundManager->SwitchAmbientMusic(-1);
    gbLimitedCombatUpdatePalette = 0;
    if (!gbClosingApp) {
        memcpy(gPalette->m_data, m_savedPalette, COMBAT_PALETTE_DATA_SIZE);
        memcpy(gpBufferPalette->m_data, m_savedPalette,
               COMBAT_PALETTE_DATA_SIZE);
    }
    gpWindowManager->FadeScreen(1, 8, 0);
    giCycleType = 0;
    CycleColors(0);
    delete m_combatBuffer;
    delete m_backgroundBuffer;
    if (m_mouseGridBuffer != 0)
        delete m_mouseGridBuffer;

    int total;
    int groupSide;
    int index;
    for (index = 0; index < 2; index++)
        UpdateArmyGroup(index);

    total = 0;
    if (m_playerId[COMBAT_DEFENDER_SIDE] == -1)
        groupSide = COMBAT_DEFENDER_SIDE;
    else
        groupSide = COMBAT_ATTACKER_SIDE;

    for (index = 0; index < ARMY_GROUP_SLOT_COUNT; index++) {
        if (m_armyGroups[groupSide]->m_creatureTypes[index] !=
            ARMY_GROUP_EMPTY_SLOT)
            total += m_armyGroups[groupSide]->m_creatureCounts[index];
    }

    if (m_battlefieldCell->triggerType == COMBAT_TRIGGER_MONSTER) {
        if (total > 4000)
            total = 4000;
        m_battlefieldCell->w4hi = total & 0xfff;
    }

    if (m_battlefieldCell->triggerType == COMBAT_TRIGGER_MINE &&
        gpGame->m_mines[m_battlefieldCell->w4hi].guardianType != -1)
        gpGame->m_mines[m_battlefieldCell->w4hi].guardianCount =
            static_cast<unsigned char>(total);

    if (m_battlefieldCell->triggerType == COMBAT_TRIGGER_HERO) {
        hero *combatHero = gpGame->GetHero(m_battlefieldCell->w4hi);
        if (combatHero->m_locationType == COMBAT_TRIGGER_MINE &&
            gpGame->m_mines[combatHero->m_occupiedTown].guardianType != -1)
            gpGame->m_mines[combatHero->m_occupiedTown].guardianCount =
                static_cast<unsigned char>(total);
    }

    gpWindowManager->RemoveWindow(m_combatWindow);
    FreeArmies();
    FreeIcons();
    gpResourceManager->Dispose(m_combatPalette);
    delete m_combatWindow;
    if (!bMouseWasVis)
        gpMouseManager->HideColorPointer();
    m_active = 0;
    m_combatWindowOpen = 0;
}

VA(0x004912b5, 0x38c)
void combatManager::UpdateArmyGroup(int side)
{
    int index;
    int pos;
    for (index = 0; index < ARMY_GROUP_SLOT_COUNT; index++) {
        m_armyGroups[side]->m_creatureTypes[index] = ARMY_GROUP_EMPTY_SLOT;
        m_armyGroups[side]->m_creatureCounts[index] = 0;
    }

    for (index = 0; index < m_armyCount[side]; index++) {
        if (!(m_armies[side][index].m_monster.flags.all &
              MONSTER_FLAGS_AI_EXCLUDED) &&
            m_armies[side][index].m_quantity > 0 &&
            (m_playerId[side] == -1 ||
             ((m_armies[side][index].m_monsterType !=
                   ARMY_CREATURE_EARTH_ELEMENTAL &&
               m_armies[side][index].m_monsterType !=
                   ARMY_CREATURE_AIR_ELEMENTAL &&
               m_armies[side][index].m_monsterType !=
                   ARMY_CREATURE_FIRE_ELEMENTAL &&
               m_armies[side][index].m_monsterType !=
                   ARMY_CREATURE_WATER_ELEMENTAL) ||
              !(m_armies[side][index].m_monster.flags.all &
                MONSTER_FLAGS_SUMMONED))) &&
            !(m_armies[side][index].m_monster.flags.all &
              MONSTER_FLAGS_MIRROR_IMAGE)) {
            m_armyGroups[side]
                ->m_creatureTypes[m_armies[side][index].m_armyGroupSlot] =
                static_cast<signed char>(m_armies[side][index].m_monsterType);
            m_armyGroups[side]
                ->m_creatureCounts[m_armies[side][index].m_armyGroupSlot] =
                static_cast<short>(m_armies[side][index].m_quantity);
        }
    }

    if (giSkeletonsCreated && m_combatResult == side)
        m_armyGroups[side]->Add(ARMY_CREATURE_SKELETON, giSkeletonsCreated,
                                ARMY_GROUP_EMPTY_SLOT);
}

VA(0x00491641, 0x365)
void combatManager::GenerateMap(void)
{
    int gridX;
    int randomOffset;
    int x;
    unsigned int y;
    int coordinateY;

    if (m_inCastleCombat == 1)
        m_catapultFrame = 0;
    else
        m_catapultFrame = -1;

    for (y = 0; static_cast<int>(y) < COMBAT_GRID_ROW_COUNT; y++) {
        for (x = 0; x < COMBAT_GRID_ROW_LENGTH; x++) {
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_y =
                static_cast<short>((y + 1) * COMBAT_HEX_VERTICAL_STEP +
                                   COMBAT_HEX_CENTER_Y_ORIGIN);
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_x =
                static_cast<short>(
                ((y & 1) ? COMBAT_HEX_ROW_STAGGER :
                           COMBAT_HEX_HORIZONTAL_STEP) +
                (x - 1) * COMBAT_HEX_HORIZONTAL_STEP +
                COMBAT_HEX_GRID_LEFT_ORIGIN);
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridLeft =
                static_cast<short>(
                ((y & 1) ? 0 : COMBAT_HEX_ROW_STAGGER) +
                (x - 1) * COMBAT_HEX_HORIZONTAL_STEP +
                COMBAT_HEX_GRID_LEFT_ORIGIN);
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridTop =
                static_cast<short>(
                y * COMBAT_HEX_VERTICAL_STEP + COMBAT_HEX_GRID_TOP_ORIGIN);
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridRight =
                m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridLeft +
                COMBAT_HEX_HORIZONTAL_STEP;
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridBodyBottom =
                m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridTop +
                COMBAT_HEX_VERTICAL_STEP;
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridBottom =
                m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridTop +
                COMBAT_MOUSE_HEX_HEIGHT;
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_occupantSide =
                -1;
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_occupantIndex = -1;
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_occupantFrame = -1;
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_obstacleIndex = -1;
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_blocked = 0;
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x]
                .m_deadOccupantCount = 0;
        }
    }
    randomOffset = SRandom(8, 15);
}

// @early-stop
// Instruction bytes and external relocations match; target offsets
// +0x1e8..+0x20c are the delinked local jump table.
VA(0x004919a6, 0x224)
char * combatManager::GetBackgroundName(void)
{
    int backgroundIndex;
    m_colorCycleType = 1;
    m_battlefieldFringe = -1;
    switch (m_terrainType) {
    case COMBAT_TERRAIN_WATER:
        backgroundIndex = 0;
        m_battlefieldFringe = 13;
        break;
    case COMBAT_TERRAIN_GRASS:
        if (MoreTreesNear()) {
            backgroundIndex = 2;
            m_battlefieldFringe = 12;
        } else {
            backgroundIndex = 3;
            m_battlefieldFringe = 11;
        }
        break;
    case COMBAT_TERRAIN_SNOW:
        m_colorCycleType = 3;
        if (MoreTreesNear()) {
            backgroundIndex = 4;
            m_battlefieldFringe = 6;
        } else {
            backgroundIndex = 5;
            m_battlefieldFringe = 7;
        }
        break;
    case COMBAT_TERRAIN_SWAMP:
        backgroundIndex = 6;
        m_battlefieldFringe = 8;
        break;
    case COMBAT_TERRAIN_LAVA:
        backgroundIndex = 8;
        m_battlefieldFringe = 5;
        break;
    case COMBAT_TERRAIN_DESERT:
        m_colorCycleType = 3;
        backgroundIndex = 10;
        m_battlefieldFringe = 4;
        break;
    case COMBAT_TERRAIN_DIRT:
        if (MoreTreesNear()) {
            backgroundIndex = 12;
            m_battlefieldFringe = 10;
        } else {
            backgroundIndex = 13;
            m_battlefieldFringe = 9;
        }
        break;
    case COMBAT_TERRAIN_WASTELAND:
        m_colorCycleType = 3;
        backgroundIndex = 14;
        m_battlefieldFringe = 3;
        break;
    case COMBAT_TERRAIN_BEACH:
        m_colorCycleType = 3;
        backgroundIndex = 16;
        m_battlefieldFringe = 2;
        break;
    default:
        backgroundIndex = 0;
        break;
    }
    return cCombatBkgNames[backgroundIndex];
}

// @early-stop
// Logic and frame slots are byte-exact. The +0xb2 bound test is the
// TU-cumulative /Od MAP_HEIGHT operand-load order; the target delinker names
// normalDirTable+1 as a string, and moves the 0x14-byte local jump table from
// target +0x13c to base +0x13d after the one-byte bound-test delta.
VA(0x00491bca, 0x210)
int combatManager::MoreTreesNear(void)
{
    int treeCount;
    int x;
    int y;
    int mountainCounter;
    mapCell *combatCell;
    int radius;
    int combatOriginX;
    signed char nearbyTypeTable[3][8];
    unsigned char nearbyTileset;
    int nearbyDirection;
    int centerY;

    memset(nearbyTypeTable, -1, sizeof(nearbyTypeTable));
    combatOriginX = m_combatX;
    centerY = m_combatY;

    for (radius = 0; radius < 3; radius++) {
        for (nearbyDirection = 0; nearbyDirection < 8; nearbyDirection++) {
            x = normalDirTable[nearbyDirection].x * radius + combatOriginX;
            y = normalDirTable[nearbyDirection]._1 * radius + centerY;
            if (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT) {
                combatCell = gpAdvManager->GetCell(x, y);
                nearbyTileset = combatCell->objTileset;
                switch (nearbyTileset) {
                case COMBAT_TILESET_SNOW_MOUNTAINS:
                case COMBAT_TILESET_SWAMP_MOUNTAINS:
                case COMBAT_TILESET_LAVA_MOUNTAINS:
                case COMBAT_TILESET_DESERT_MOUNTAINS:
                case COMBAT_TILESET_DIRT_MOUNTAINS:
                case COMBAT_TILESET_MIXED_MOUNTAINS:
                case COMBAT_TILESET_CRACKED_MOUNTAINS:
                case COMBAT_TILESET_GRASS_MOUNTAINS:
                    nearbyTypeTable[radius][nearbyDirection] = 0;
                    break;
                case COMBAT_TILESET_JUNGLE_TREES:
                case COMBAT_TILESET_EVIL_TREES:
                case COMBAT_TILESET_SNOW_TREES:
                case COMBAT_TILESET_SUMMER_TREES:
                case COMBAT_TILESET_AUTUMN_TREES:
                    nearbyTypeTable[radius][nearbyDirection] = 1;
                    break;
                }
            }
        }
    }

    treeCount = 0;
    mountainCounter = 0;
    for (radius = 0; radius < 3; radius++) {
        for (nearbyDirection = 0; nearbyDirection < 8; nearbyDirection++) {
            if (nearbyTypeTable[radius][nearbyDirection] == 0)
                mountainCounter++;
            if (nearbyTypeTable[radius][nearbyDirection] == 1)
                treeCount++;
        }
    }
    return mountainCounter < treeCount;
}

VA(0x00491dda, 0x3e7)
// @early-stop exact 0x3e7-byte span and 58 relocation targets. Every
// non-jump opcode and operand matches; the sole residual is the five-byte
// GetPlayerColor /Ob1 continuation at +0x372 here versus retail +0x3a0.
void combatManager::LoadIcons(void)
{
    int index;
    int heroColor;
    for (index = 0; index < COMBAT_FIXED_ICON_COUNT; index++)
        m_combatIcons[index] = 0;

    for (index = 0; index < COMBAT_OBSTACLE_ICON_LOAD_COUNT; index++)
        m_obstacleIcons[index] = 0;

    m_combatIcons[COMBAT_ICON_SPELLS] = gpResourceManager->GetIcon("spells.icn");
    m_combatIcons[COMBAT_ICON_STATUS] = gpResourceManager->GetIcon("textbar.icn");
    m_combatIcons[COMBAT_ICON_GRID] = gpResourceManager->GetIcon("cmbtmisc.icn");
    m_combatIcons[COMBAT_ICON_SMALL_VIEW_BACKGROUND] =
        gpResourceManager->GetIcon("viewarsm.icn");
    m_combatIcons[COMBAT_ICON_SMALL_VIEW_MODIFIER] =
        gpResourceManager->GetIcon("minilkmr.icn");
    m_combatIcons[COMBAT_ICON_SMALL_VIEW_SPELL] =
        gpResourceManager->GetIcon("spellinf.icn");

    if (m_inCastleCombat) {
        if (m_combatTowns[COMBAT_DEFENDER_SIDE]->m_buildings &
            TOWN_BUILDING_MOAT) {
            m_combatIcons[COMBAT_ICON_MOAT] =
                gpResourceManager->GetIcon("moatpart.icn");
            m_combatIcons[COMBAT_ICON_DRAWBRIDGE] =
                gpResourceManager->GetIcon("moatwhol.icn");
        }
        m_combatIcons[COMBAT_ICON_CATAPULT] =
            gpResourceManager->GetIcon("catapult.icn");
        sprintf(gText, "castle%c.icn",
                cHeroTypeInitial[m_combatTowns[COMBAT_DEFENDER_SIDE]->m_type]);
        m_combatIcons[COMBAT_ICON_TOWER] = gpResourceManager->GetIcon(gText);
        m_combatIcons[COMBAT_ICON_KEEP] = gpResourceManager->GetIcon("keep.icn");
    }

    for (index = 0; index < COMBAT_MANAGER_SIDE_COUNT; index++) {
        m_heroIcons[index] = 0;
        m_heroOverlayIcons[index] = 0;
        m_heroAnimationState[index] = 0;
        m_heroAnimationFrame[index] = 0;
        m_heroSpriteIndex[index] = -1;

        if (m_heroes[index]) {
            if (m_heroes[index]->m_isCaptain) {
                sprintf(gText, "cmbtcap%c.icn",
                        cHeroTypeInitial[m_heroes[index]->m_cursorType]);
                m_heroIcons[index] = gpResourceManager->GetIcon(gText);
                m_heroSpriteIndex[index] = m_heroes[index]->m_cursorType + 6;
            } else {
                sprintf(gText, "cmbthro%c.icn",
                        cHeroTypeInitial[m_heroes[index]->m_cursorType]);
                m_heroIcons[index] = gpResourceManager->GetIcon(gText);
                m_heroSpriteIndex[index] = m_heroes[index]->m_cursorType;
            }
        }

        if (m_heroIcons[index]) {
            if (m_playerId[index] == -1)
                heroColor = 6;
            else
                heroColor = gpGame->GetPlayerColor(
                    static_cast<signed char>(m_playerId[index]));
            sprintf(gText, "herofl%02d.icn", heroColor);
            m_heroOverlayIcons[index] = gpResourceManager->GetIcon(gText);
        }
    }
}

VA(0x004921c1, 0x124)
void combatManager::FreeIcons(void)
{
    int index;
    for (index = 0; index < COMBAT_FIXED_ICON_COUNT; index++) {
        if (m_combatIcons[index])
            gpResourceManager->Dispose(m_combatIcons[index]);
    }

    for (index = 0; index < COMBAT_OBSTACLE_ICON_LOAD_COUNT; index++) {
        if (m_obstacleIcons[index])
            gpResourceManager->Dispose(m_obstacleIcons[index]);
    }

    for (index = 0; index < COMBAT_MANAGER_SIDE_COUNT; index++) {
        if (m_heroIcons[index])
            gpResourceManager->Dispose(m_heroIcons[index]);
        if (m_heroOverlayIcons[index])
            gpResourceManager->Dispose(m_heroOverlayIcons[index]);
    }
}

VA(0x004922e5, 0x36d)
void combatManager::LoadArmies(void)
{
    int groupSlot;
    int side;
    int combatHex;

    m_armyCount[COMBAT_ATTACKER_SIDE] =
        m_armyCount[COMBAT_DEFENDER_SIDE] = 0;

    for (groupSlot = 0; groupSlot < COMBAT_ARMY_CAPACITY; groupSlot++) {
        for (side = 0; side < COMBAT_MANAGER_SIDE_COUNT; side++) {
            m_armies[side][groupSlot].m_quantity = 0;
            m_armies[side][groupSlot].m_monsterType = -1;
        }
    }

    for (side = 0; side < COMBAT_MANAGER_SIDE_COUNT; side++) {
        for (groupSlot = 0; groupSlot < COMBAT_ARMY_CAPACITY; groupSlot++)
            m_armies[side][groupSlot].InitClean();
    }

    for (groupSlot = 0; groupSlot < ARMY_GROUP_SLOT_COUNT; groupSlot++) {
        if (m_armyGroups[COMBAT_ATTACKER_SIDE]->m_creatureTypes[groupSlot] !=
            ARMY_GROUP_EMPTY_SLOT) {
            if (m_heroes[COMBAT_ATTACKER_SIDE] &&
                (m_heroes[COMBAT_ATTACKER_SIDE]->m_eventFlags &
                 HERO_EVENT_GROUPED_FORMATION))
                combatHex = COMBAT_GROUPED_HEX_STEP * groupSlot +
                            COMBAT_ATTACKER_GROUPED_HEX;
            else
                combatHex = COMBAT_SPREAD_HEX_STEP * groupSlot +
                            COMBAT_ATTACKER_SPREAD_HEX;

            m_armies[COMBAT_ATTACKER_SIDE]
                    [m_armyCount[COMBAT_ATTACKER_SIDE]]
                        .Init(m_armyGroups[COMBAT_ATTACKER_SIDE]
                                  ->m_creatureTypes[groupSlot],
                              m_armyGroups[COMBAT_ATTACKER_SIDE]
                                  ->m_creatureCounts[groupSlot],
                              COMBAT_ATTACKER_SIDE,
                              m_armyCount[COMBAT_ATTACKER_SIDE], combatHex,
                              groupSlot);
            m_armies[COMBAT_ATTACKER_SIDE]
                    [m_armyCount[COMBAT_ATTACKER_SIDE]]
                        .LoadResources();
            m_armyCount[COMBAT_ATTACKER_SIDE]++;
        }

        if (m_armyGroups[COMBAT_DEFENDER_SIDE]->m_creatureTypes[groupSlot] !=
            ARMY_GROUP_EMPTY_SLOT) {
            if ((m_heroes[COMBAT_DEFENDER_SIDE] &&
                 (m_heroes[COMBAT_DEFENDER_SIDE]->m_eventFlags &
                  HERO_EVENT_GROUPED_FORMATION)) ||
                (m_combatTowns[COMBAT_DEFENDER_SIDE] &&
                 m_combatTowns[COMBAT_DEFENDER_SIDE]->m_formation))
                combatHex = COMBAT_GROUPED_HEX_STEP * groupSlot +
                            COMBAT_DEFENDER_GROUPED_HEX;
            else
                combatHex = COMBAT_SPREAD_HEX_STEP * groupSlot +
                            COMBAT_DEFENDER_SPREAD_HEX;

            m_armies[COMBAT_DEFENDER_SIDE]
                    [m_armyCount[COMBAT_DEFENDER_SIDE]]
                        .Init(m_armyGroups[COMBAT_DEFENDER_SIDE]
                                  ->m_creatureTypes[groupSlot],
                              m_armyGroups[COMBAT_DEFENDER_SIDE]
                                  ->m_creatureCounts[groupSlot],
                              COMBAT_DEFENDER_SIDE,
                              m_armyCount[COMBAT_DEFENDER_SIDE], combatHex,
                              groupSlot);
            m_armies[COMBAT_DEFENDER_SIDE]
                    [m_armyCount[COMBAT_DEFENDER_SIDE]]
                        .LoadResources();
            m_armyCount[COMBAT_DEFENDER_SIDE]++;
        }
    }
}

VA(0x00492652, 0xdb)
void combatManager::FreeArmies(void)
{
    int index;
    gpSoundManager->StopAllSamples(1);

    for (index = 0; index < m_armyCount[COMBAT_ATTACKER_SIDE]; index++)
        m_armies[COMBAT_ATTACKER_SIDE][index].FreeResources();
    for (index = 0; index < m_armyCount[COMBAT_DEFENDER_SIDE]; index++)
        m_armies[COMBAT_DEFENDER_SIDE][index].FreeResources();

    if (gCurLoadedSpellIcon)
        gpResourceManager->Dispose(gCurLoadedSpellIcon);
    gCurLoadedSpellIcon = 0;
    gCurLoadedSpellEffect = -1;
}

VA(0x0049272d, 0x1e2)
int combatManager::GetGridIndex(int x, int y)
{
    int gridX;
    int gridY;
    int rowIndex;
    int gridColumn;
    int diagonalDistance;
    int yOffset;
    int xResidual;

    gridX = x;
    gridY = y;
    gridY -= COMBAT_HEX_GRID_TOP_ORIGIN;
    rowIndex = gridY / COMBAT_HEX_VERTICAL_STEP;
    gridX -= COMBAT_GRID_INDEX_X_ORIGIN;
    if (!(rowIndex & 1))
        gridX -= COMBAT_HEX_ROW_STAGGER;
    gridColumn = gridX / COMBAT_HEX_HORIZONTAL_STEP;

    if (gridColumn < 0)
        goto specialRegion;

    yOffset = gridY % COMBAT_HEX_VERTICAL_STEP;
    if (yOffset < COMBAT_GRID_DIAGONAL_HEIGHT) {
        xResidual = gridX % COMBAT_HEX_HORIZONTAL_STEP;
        diagonalDistance =
            abs(xResidual - COMBAT_HEX_ROW_STAGGER) /
            COMBAT_GRID_DIAGONAL_SLOPE_DIVISOR;
        if (yOffset < diagonalDistance) {
            rowIndex--;
            if (xResidual < COMBAT_HEX_ROW_STAGGER) {
                if (!(rowIndex & 1))
                    gridColumn--;
            } else if (rowIndex & 1) {
                gridColumn++;
            }
        }
    }

    if (gridColumn <= COMBAT_GRID_REVERSE_COLUMN_END ||
        gridColumn >= COMBAT_GRID_COLUMN_END ||
        rowIndex >= COMBAT_GRID_ROW_COUNT ||
        rowIndex < 0) {
specialRegion:
        if (x >= 0 && x <= COMBAT_GRID_LEFT_SPECIAL_X_MAX &&
            y >= COMBAT_GRID_LEFT_SPECIAL_Y_MIN &&
            y <= COMBAT_GRID_LEFT_SPECIAL_Y_MAX)
            return COMBAT_GRID_LEFT_SPECIAL_HEX;
        if (x >= COMBAT_GRID_RIGHT_SPECIAL_X_MIN &&
            x <= COMBAT_MAX_EXTENT_X &&
            y >= COMBAT_GRID_RIGHT_UPPER_Y_MIN &&
            y <= COMBAT_GRID_RIGHT_UPPER_Y_MAX)
            return COMBAT_GRID_RIGHT_UPPER_HEX;
        if (x >= COMBAT_GRID_RIGHT_SPECIAL_X_MIN &&
            x <= COMBAT_MAX_EXTENT_X &&
            y >= COMBAT_GRID_RIGHT_LOWER_Y_MIN &&
            y <= COMBAT_GRID_RIGHT_LOWER_Y_MAX &&
            m_inCastleCombat)
            return COMBAT_BALLISTA_HEX;
        return -1;
    }
    return rowIndex * COMBAT_GRID_ROW_LENGTH + gridColumn;
}

VA(0x0049290f, 0x1eb)
void combatManager::CheckApplyGoodMorale(int side, int index)
{
    if (side < 0 || index < 0)
        return;
    if (bInHighMoraleBonus) {
        bInHighMoraleBonus = 0;
        return;
    }
    bInHighMoraleBonus = 0;

    army *activeArmy = &m_armies[side][index];
    if (activeArmy->m_monster.flags.all & MONSTER_FLAGS_NO_MORALE)
        return;
    if (activeArmy->m_quantity == 0)
        return;
    if (activeArmy->m_morale <= 0 ||
        SRandom(COMBAT_MORALE_ROLL_MIN, COMBAT_GOOD_MORALE_ROLL_MAX) >
            activeArmy->m_morale)
        return;

    bInHighMoraleBonus = 1;
    SAMPLE2 moraleSample;
    if (!gbNoShowCombat) {
        sprintf(gText, "goodmrle.82M");
        moraleSample = LoadPlaySample(gText);
        if (activeArmy->m_quantity <= 1)
            sprintf(gText, "High morale enables the \n%s to attack again.",
                    gArmyNames[activeArmy->m_monsterType]);
        else
            sprintf(gText, "High morale enables the \n%s to attack again.",
                    gArmyNamesPlural[activeArmy->m_monsterType]);
        CombatMessage(gText, 1, 1, 0);
    }

    activeArmy->SpellEffect(COMBAT_GOOD_MORALE_EFFECT,
                            COMBAT_MORALE_EFFECT_DURATION, 0);
    if (activeArmy->m_monster.flags.abilityFlags &
        MONSTER_ABILITY_FLAG_BAD_MORALE)
        activeArmy->m_monster.flags.abilityFlags -=
            MONSTER_ABILITY_FLAG_BAD_MORALE;
    activeArmy->m_monster.flags.abilityFlags |=
        MONSTER_ABILITY_FLAG_HIGH_MORALE;

    if (!gbNoShowCombat)
        WaitEndSample(moraleSample, -1);
}

VA(0x00492afa, 0x1cd)
int combatManager::CheckApplyBadMorale(int side, int index)
{
    if (side < 0 || index < 0)
        return 0;

    army *activeArmy = &m_armies[side][index];
    if (activeArmy->m_monster.flags.all & MONSTER_FLAGS_NO_MORALE)
        return 0;
    if (activeArmy->m_morale >= 0 ||
        SRandom(COMBAT_MORALE_ROLL_MIN, COMBAT_BAD_MORALE_ROLL_MAX) >
            -activeArmy->m_morale)
        return 0;
    if (!m_networkArmyPresent[side] &&
        SRandom(COMBAT_MORALE_ROLL_MIN,
                COMBAT_BAD_MORALE_NETWORK_ROLL_MAX) ==
            COMBAT_BAD_MORALE_NETWORK_SKIP_ROLL)
        return 0;

    SAMPLE2 moraleSample;
    if (!gbNoShowCombat) {
        moraleSample = NULL_SAMPLE2;
        moraleSample = LoadPlaySample("BADMRLE.82M");
        if (activeArmy->m_quantity <= 1)
            sprintf(gText, "Low morale causes the \n%s to freeze in panic.",
                    gArmyNames[activeArmy->m_monsterType]);
        else
            sprintf(gText, "Low morale causes the \n%s to freeze in panic.",
                    gArmyNamesPlural[activeArmy->m_monsterType]);
        CombatMessage(gText, 1, 1, 0);
    }

    activeArmy->SpellEffect(COMBAT_BAD_MORALE_EFFECT,
                            COMBAT_MORALE_EFFECT_DURATION, 1);
    activeArmy->m_monster.flags.abilityFlags |=
        MONSTER_ABILITY_FLAG_BAD_MORALE;
    if (!gbNoShowCombat)
        WaitEndSample(moraleSample, -1);
    return 1;
}

VA(0x00492cc7, 0x382)
int combatManager::GetNextArmy(int checkMorale)
{
    army *activeArmy;
    int speedLoop;
    int stackSide;
    int armyCounter;
    int sideLoop;
    int hasDeferred;
    int skipEntry;
    int armyOffset;

restart:
    hasDeferred = 0;
    stackSide = m_currentArmySide;
    m_currentSpeed = COMBAT_MAX_SPEED;
    for (speedLoop = 0; speedLoop < COMBAT_SPEED_LEVEL_COUNT; speedLoop++) {
        for (sideLoop = 0; sideLoop < COMBAT_MANAGER_SIDE_COUNT; sideLoop++) {
            stackSide ^= 1;
            for (armyCounter = 0; armyCounter < m_armyCount[stackSide];
                 armyCounter++) {
                skipEntry = 0;
                activeArmy = armyCounter + m_armies[stackSide];
                if ((activeArmy->m_monster.flags.abilityFlags &
                     (MONSTER_ABILITY_FLAG_AI_EXCLUDED |
                      MONSTER_ABILITY_FLAG_BAD_MORALE)) ||
                    activeArmy->m_spellInfluence
                        [ARMY_SPELL_INFLUENCE_PARALYZE] ||
                    activeArmy->m_spellInfluence
                        [ARMY_SPELL_INFLUENCE_PETRIFIED] ||
                    activeArmy->m_spellInfluence[ARMY_SPELL_INFLUENCE_BLIND] ||
                    (activeArmy->m_monster.speed != m_currentSpeed &&
                     !(activeArmy->m_monster.flags.abilityFlags &
                       MONSTER_ABILITY_FLAG_HIGH_MORALE)))
                    skipEntry = 1;

                if (!skipEntry && speedLoop == 0 &&
                    !(activeArmy->m_monster.flags.abilityFlags &
                      MONSTER_ABILITY_FLAG_HIGH_MORALE))
                    skipEntry = 1;

                if (activeArmy->m_monster.flags.abilityFlags &
                    MONSTER_ABILITY_FLAG_DEFERRED_TURN) {
                    skipEntry = 1;
                    hasDeferred = 1;
                }

                if (!skipEntry && checkMorale &&
                    CheckApplyBadMorale(stackSide, armyCounter))
                    skipEntry = 1;
                if (!skipEntry)
                    break;
            }

            if (armyCounter != m_armyCount[stackSide]) {
                m_currentArmySide = stackSide;
                m_currentArmyIndex = armyCounter;
                if ((armyCounter + m_armies[stackSide])
                        ->m_spellInfluence[ARMY_SPELL_INFLUENCE_HYPNOTIZE])
                    m_currentSide = 1 - stackSide;
                else
                    m_currentSide = stackSide;
                GetControl();
                return 1;
            }
        }

        if (speedLoop) {
            m_currentSpeed--;
            if (m_currentSpeed == 0)
                m_currentSpeed = COMBAT_SPEED_LEVEL_COUNT;
        }
    }

    if (hasDeferred) {
        checkMorale = 0;
        for (sideLoop = 0; sideLoop < COMBAT_MANAGER_SIDE_COUNT; sideLoop++) {
            for (armyCounter = 0; armyCounter < m_armyCount[sideLoop];
                 armyCounter++) {
                (m_armies[sideLoop] + armyCounter)
                    ->m_monster.flags.abilityFlags &=
                    ~MONSTER_ABILITY_FLAG_DEFERRED_TURN;
            }
        }
        goto restart;
    }

    CheckCastleAttack();
    m_currentSide = 1 - m_currentSide;
    CheckCastleAttack();
    m_currentSide = 1 - m_currentSide;
    return 0;
}

VA(0x00493049, 0xd6)
int combatManager::IsWinner(int side)
{
    int winner;
    int index;

    if (m_sideDefeated[1 - side])
        return 1;
    if (m_sideRetreated[1 - side])
        return 1;

    side ^= 1;
    winner = 1;
    for (index = 0; index < m_armyCount[side]; index++) {
        if (!(m_armies[side][index].m_monster.flags.abilityFlags &
              MONSTER_ABILITY_FLAG_AI_EXCLUDED))
            winner = 0;
    }
    return winner;
}

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
