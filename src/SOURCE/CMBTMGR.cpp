// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\CMBTMGR.OBJ   from: (directly linked into exe)
// functions: 39   data: 7
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <BASE/bitmap.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/inputManager.h>
#include <BASE/Icon2b.h>
#include <BASE/Misc.h>
#include <BASE/mouseManager.h>
#include <BASE/palette.h>
#include <BASE/resourceManager.h>
#include <BASE/sample.h>
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
#include <SOURCE/NOOPT.h>
#include <SOURCE/PATH.h>
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
    m_currentCommand = 0;
    m_unknownF35B = 0;
    m_unknownF353 = -1;
    m_unknownF34F = m_unknownF353;
    m_catapultFrame[COMBAT_ATTACKER_SIDE] = m_unknownF34F;
    m_catapultFrame[COMBAT_DEFENDER_SIDE] =
        m_catapultFrame[COMBAT_ATTACKER_SIDE];
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

    i32 sourceIndex;
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
            i32 targetIndex;
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
void combatManager::SetupCombat(i32 mapX, i32 mapY, hero *attackerHero,
                                armyGroup *attackerGroup, town *defenderTown,
                                hero *defenderHero,
                                armyGroup *defenderGroup, i32 combatX,
                                i32 combatY, i32 randomSeed)
{
    giSeed = randomSeed;
    SRand(combatX * COMBAT_RANDOM_X_MULTIPLIER + combatY);
    m_combatX = combatX;
    m_combatY = combatY;

    if (mapX >= 0 && mapY >= 0)
        m_battlefieldCell = gpAdvManager->GetCell(mapX, mapY);
    else
        m_battlefieldCell = 0;

    m_terrainType = giGroundToTerrain[m_battlefieldCell->m_terrainImageIndex];
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

    i32 side;
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

        m_catapultAttacksRemaining[side] = 1;
        m_catapultAttackCount[side] = m_catapultAttacksRemaining[side];
        if (m_heroes[side] != 0 &&
            m_heroes[side]->HasArtifact(COMBAT_ARTIFACT_BALLISTA)) {
            m_catapultAttacksRemaining[side] = 2;
            m_catapultAttackCount[side] = m_catapultAttacksRemaining[side];
        }
        if (m_heroes[side] != 0 &&
            m_heroes[side]->m_secondarySkills[HERO_SKILL_BALLISTICS] >=
                HERO_SKILL_LEVEL_ADVANCED) {
            m_catapultAttackCount[side]++;
            m_catapultAttacksRemaining[side]++;
        }
        m_keepAttacksRemaining[side] = 1;
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
    i32 side;
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
    m_heroDeathAnimationPlayed[0] = m_heroDeathAnimationPlayed[1] = 0;
    m_heroAlternateDeathAnimationPlayed[0] =
        m_heroAlternateDeathAnimationPlayed[1] = 0;
    m_heroDeathPending[0] = m_heroDeathPending[1] = 0;
    m_heroAlternateDeathPending[0] = m_heroAlternateDeathPending[1] = 0;
    m_eagleEyeSpell[COMBAT_ATTACKER_SIDE] = -1;
    m_eagleEyeSpell[COMBAT_DEFENDER_SIDE] = -1;
    giNextAction = 0;
    m_unknown351D[0] = 0;
    m_unknown351D[1] = 0;
    m_selectedHex = -1;
    m_limitCreatureHex = -1;
    m_previousCommand = COMBAT_INVALID_HISTORY_INDEX;
    m_currentSide = COMBAT_DEFENDER_SIDE;
    m_currentArmySide = COMBAT_DEFENDER_SIDE;
    m_currentSpeed = COMBAT_INITIAL_COMMAND;
    gbRetreatWin = 0;
    gbCombatSurrender = 0;
    m_sideDefeated[COMBAT_ATTACKER_SIDE] = 0;
    m_sideDefeated[COMBAT_DEFENDER_SIDE] = 0;
    m_limitCreature = 1;
    m_obstacleCount = 0;
    SetupAdjacencyArray();
    GenerateMap();
    LoadArmies();
}

// @early-stop
// raw-byte exact outside relocated jump operand/table at +0x165..+0x180; local-label addends only
VA(0x0049089d, 0x203)
void combatManager::SetupAdjacencyArray(void)
{
    i32 destinationHex = 0;
    i32 sourceHex;
    for (sourceHex = 0; sourceHex < COMBAT_HEX_COUNT; sourceHex++) {
        i32 rowIndex = sourceHex / COMBAT_GRID_ROW_LENGTH;
        i32 direction;
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
                        static_cast<i8>(destinationHex);
            }
        }
    }
}

VA(0x00490aa0, 0x43f)
i32 combatManager::Open(i32 openFlags)
{
    LogStr("Op1");
    memcpy(m_savedPalette, gPalette->m_data, COMBAT_PALETTE_DATA_SIZE);
    gpMouseManager->m_forcePointerUpdate = 1;
    i32 savedShowMouseHex = gConfig.showCombatMouseHex;
    gConfig.showCombatMouseHex = 0;
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
    gpMouseManager->SetPointer("cmbtmous.mse", 6, MOUSE_AUTO_CURSOR_TYPE);
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
    gConfig.showCombatMouseHex = savedShowMouseHex;
    if (m_combatPalette->m_data != gpBufferPalette->m_data)
        memmove(m_combatPalette->m_data, gpBufferPalette->m_data,
                COMBAT_PALETTE_DATA_SIZE);
    gpWindowManager->FadeScreen(0, 8, m_combatPalette);
    gbLimitedCombatUpdatePalette = 1;
    WaitEndSample(preBattleSample, -1);

    LogStr("Op3");
    gpSoundManager->SwitchAmbientMusic(SRandom(2, 4));
    glTimers[GLOBAL_COMBAT_CYCLE_TIMER_SLOT] = KBTickCount();
    ResetCycleTimers();
    LogStr("Op4");
    gpInputManager->Flush();
    ResetMouse();
    m_messageMask = 0x200;
    m_priority = openFlags;
    m_active = 1;
    strcpy(m_name, "combatManager");
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

    i32 total;
    i32 groupSide;
    i32 index;
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

    if (m_battlefieldCell->m_triggerType == COMBAT_TRIGGER_MONSTER) {
        if (total > 4000)
            total = 4000;
        m_battlefieldCell->m_objectMetadata = total & 0xfff;
    }

    if (m_battlefieldCell->m_triggerType == COMBAT_TRIGGER_MINE &&
        gpGame->m_mines[m_battlefieldCell->m_objectMetadata].guardianType != -1)
        gpGame->m_mines[m_battlefieldCell->m_objectMetadata].guardianCount =
            static_cast<u8>(total);

    if (m_battlefieldCell->m_triggerType == COMBAT_TRIGGER_HERO) {
        hero *combatHero = gpGame->GetHero(m_battlefieldCell->m_objectMetadata);
        if (combatHero->m_locationType == COMBAT_TRIGGER_MINE &&
            gpGame->m_mines[combatHero->m_occupiedTown].guardianType != -1)
            gpGame->m_mines[combatHero->m_occupiedTown].guardianCount =
                static_cast<u8>(total);
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
void combatManager::UpdateArmyGroup(i32 side)
{
    i32 index;
    i32 pos;
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
                static_cast<i8>(m_armies[side][index].m_monsterType);
            m_armyGroups[side]
                ->m_creatureCounts[m_armies[side][index].m_armyGroupSlot] =
                static_cast<i16>(m_armies[side][index].m_quantity);
        }
    }

    if (giSkeletonsCreated && m_combatResult == side)
        m_armyGroups[side]->Add(ARMY_CREATURE_SKELETON, giSkeletonsCreated,
                                ARMY_GROUP_EMPTY_SLOT);
}

VA(0x00491641, 0x365)
void combatManager::GenerateMap(void)
{
    i32 gridX;
    i32 randomOffset;
    i32 x;
    u32 y;
    i32 coordinateY;

    if (m_inCastleCombat == 1)
        m_catapultFrame[COMBAT_ATTACKER_SIDE] = 0;
    else
        m_catapultFrame[COMBAT_ATTACKER_SIDE] = -1;

    for (y = 0; static_cast<i32>(y) < COMBAT_GRID_ROW_COUNT; y++) {
        for (x = 0; x < COMBAT_GRID_ROW_LENGTH; x++) {
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_y =
                static_cast<i16>((y + 1) * COMBAT_HEX_VERTICAL_STEP +
                                   COMBAT_HEX_CENTER_Y_ORIGIN);
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_x =
                static_cast<i16>(
                ((y & 1) ? COMBAT_HEX_ROW_STAGGER :
                           COMBAT_HEX_HORIZONTAL_STEP) +
                (x - 1) * COMBAT_HEX_HORIZONTAL_STEP +
                COMBAT_HEX_GRID_LEFT_ORIGIN);
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridLeft =
                static_cast<i16>(
                ((y & 1) ? 0 : COMBAT_HEX_ROW_STAGGER) +
                (x - 1) * COMBAT_HEX_HORIZONTAL_STEP +
                COMBAT_HEX_GRID_LEFT_ORIGIN);
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridTop =
                static_cast<i16>(
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
    i32 backgroundIndex;
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
i32 combatManager::MoreTreesNear(void)
{
    i32 treeCount;
    i32 x;
    i32 y;
    i32 mountainCounter;
    mapCell *combatCell;
    i32 radius;
    i32 combatOriginX;
    i8 nearbyTypeTable[3][8];
    u8 nearbyTileset;
    i32 nearbyDirection;
    i32 centerY;

    memset(nearbyTypeTable, -1, sizeof(nearbyTypeTable));
    combatOriginX = m_combatX;
    centerY = m_combatY;

    for (radius = 0; radius < 3; radius++) {
        for (nearbyDirection = 0; nearbyDirection < 8; nearbyDirection++) {
            x = normalDirTable[nearbyDirection].x * radius + combatOriginX;
            y = normalDirTable[nearbyDirection].y * radius + centerY;
            if (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT) {
                combatCell = gpAdvManager->GetCell(x, y);
                nearbyTileset = combatCell->m_objectTileset;
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

// @early-stop
// Exact 0x3e7-byte span and 58 relocations; every non-jump operand matches,
// with only the GetPlayerColor /Ob1 continuation at +0x372 versus retail +0x3a0.
VA(0x00491dda, 0x3e7)
void combatManager::LoadIcons(void)
{
    i32 index;
    i32 heroColor;
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
                    static_cast<i8>(m_playerId[index]));
            sprintf(gText, "herofl%02d.icn", heroColor);
            m_heroOverlayIcons[index] = gpResourceManager->GetIcon(gText);
        }
    }
}

VA(0x004921c1, 0x124)
void combatManager::FreeIcons(void)
{
    i32 index;
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
    i32 groupSlot;
    i32 side;
    i32 combatHex;

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
    i32 index;
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
i32 combatManager::GetGridIndex(i32 x, i32 y)
{
    i32 gridX;
    i32 gridY;
    i32 rowIndex;
    i32 gridColumn;
    i32 diagonalDistance;
    i32 yOffset;
    i32 xResidual;

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
        if ((yOffset | 0) < diagonalDistance) {
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
void combatManager::CheckApplyGoodMorale(i32 side, i32 index)
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

// @semantic: Complete behavior, frame/slots, morale CFG, and ordered external
// relocations. The first residual at +0x27 is equivalent m_armies indexing:
// retail scales index before side while candidate scales side before index.
// Direct indexing, cached army pointers, and commuted address forms were tried.
VA(0x00492afa, 0x1cd)
i32 combatManager::CheckApplyBadMorale(i32 side, i32 index)
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
i32 combatManager::GetNextArmy(i32 checkMorale)
{
    army *activeArmy;
    i32 speedLoop;
    i32 stackSide;
    i32 armyCounter;
    i32 sideLoop;
    i32 hasDeferred;
    i32 skipEntry;
    i32 armyOffset;

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
i32 combatManager::IsWinner(i32 side)
{
    i32 winner;
    i32 index;

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

// @early-stop
// Soft TU-cumulative wall: size, frame/slots, CFG, and all 150 ordered
// relocations agree. The only unmasked bytes are commutative load order at
// +0x636..+0x63d, +0xac5..+0xac8, and +0xe94..+0xe9b. Direct/AST commutation,
// projectile +=, prefix increment, SIB forms, val|0, and +=1 were exhausted.
// Float guards spell __adjust_fdiv as proven iLeftRightSave+0x10. Revisit only
// after the source/TU/header or comparison epoch changes.
VA(0x0049311f, 0x100e)
void combatManager::CatAttack(i32 side)
{
    if (!m_inCastleCombat)
        return;

    LogStr("CA1");
    if (m_smallViewSide[COMBAT_ATTACKER_SIDE] != -1 ||
        m_smallViewSide[COMBAT_DEFENDER_SIDE] != -1) {
        m_smallViewSide[COMBAT_ATTACKER_SIDE] =
            m_smallViewSide[COMBAT_DEFENDER_SIDE] = -1;
        DrawSmallView(COMBAT_ATTACKER_SIDE, 1);
        DrawSmallView(COMBAT_DEFENDER_SIDE, 1);
    }

    i32 random8 = SRandom(COMBAT_CATAPULT_TARGET_ROLL_MIN,
                          COMBAT_CATAPULT_TARGET_ROLL_MAX);
    i32 wallCount7 = 0;
    i32 towerCount1 = 0;
    i32 wallIndex17 = -1;
    i32 towerIndex27 = -1;
    i32 gateIndex2 = -1;
    i32 keepIndex6 = -1;
    i32 targetHex4 = -1;
    i32 missShot19 = 0;
    i32 damageLevel13 = COMBAT_CATAPULT_DAMAGE_NORMAL;
    i32 firstRoll7;
    i32 advancedRoll5;
    i32 index28;

    if (m_wallStates[COMBAT_WALL_SLOT_SECTION_FIRST] !=
            COMBAT_WALL_STATE_DESTROYED &&
        m_wallStates[COMBAT_WALL_SLOT_SECTION_FIRST] !=
            COMBAT_WALL_STATE_SECTION_DESTROYED)
        wallCount7++;
    if (m_wallStates[COMBAT_WALL_SLOT_SECTION_SECOND] !=
            COMBAT_WALL_STATE_DESTROYED &&
        m_wallStates[COMBAT_WALL_SLOT_SECTION_SECOND] !=
            COMBAT_WALL_STATE_SECTION_DESTROYED)
        wallCount7++;
    if (m_wallStates[COMBAT_WALL_SLOT_SECTION_THIRD] !=
            COMBAT_WALL_STATE_DESTROYED &&
        m_wallStates[COMBAT_WALL_SLOT_SECTION_THIRD] !=
            COMBAT_WALL_STATE_SECTION_DESTROYED)
        wallCount7++;
    if (m_wallStates[COMBAT_WALL_SLOT_SECTION_FOURTH] !=
            COMBAT_WALL_STATE_DESTROYED &&
        m_wallStates[COMBAT_WALL_SLOT_SECTION_FOURTH] !=
            COMBAT_WALL_STATE_SECTION_DESTROYED)
        wallCount7++;
    if (m_wallStates[COMBAT_WALL_SLOT_TOP_TOWER] ==
        COMBAT_WALL_STATE_TOWER_STANDING)
        towerCount1++;
    if (m_wallStates[COMBAT_WALL_SLOT_BOTTOM_TOWER] ==
        COMBAT_WALL_STATE_TOWER_STANDING)
        towerCount1++;

    if (wallCount7 != 0) {
        random8 = random8 % wallCount7 + 1;
        wallIndex17 = -1;
        while (random8 != 0) {
            wallIndex17++;
            if (m_wallStates[wallIndex17 + COMBAT_WALL_SLOT_SECTION_FIRST] !=
                    COMBAT_WALL_STATE_DESTROYED &&
                m_wallStates[wallIndex17 + COMBAT_WALL_SLOT_SECTION_FIRST] !=
                    COMBAT_WALL_STATE_SECTION_DESTROYED)
                random8--;
        }
    } else if (towerCount1 != 0) {
        random8 %= towerCount1;
        if (random8 == 1 ||
            m_wallStates[COMBAT_WALL_SLOT_TOP_TOWER] ==
                COMBAT_WALL_STATE_DESTROYED)
            towerIndex27 = COMBAT_WALL_SLOT_BOTTOM_TOWER;
        else
            towerIndex27 = COMBAT_WALL_SLOT_TOP_TOWER;
    } else if (m_drawbridgeState != COMBAT_CASTLE_GATE_HIDDEN) {
        gateIndex2 = 1;
    } else {
        if (m_wallStates[COMBAT_WALL_SLOT_KEEP] ==
            COMBAT_WALL_STATE_KEEP_STANDING)
            keepIndex6 = 0;
    }

    if (towerIndex27 == -1 && wallIndex17 == -1 && gateIndex2 == -1 &&
        keepIndex6 == -1)
        return;

    sprintf(gText, "catsnd%02d.82M", COMBAT_CATAPULT_IMPACT_SOUND);
    sample *loadedSample26 = gpResourceManager->GetSample(gText);
    i32 unknown15;
    SAMPLE2 catapultSound37 = NULL_SAMPLE2;
    SAMPLE2 impactSound1 = NULL_SAMPLE2;
    icon *boulder37 = gpResourceManager->GetIcon("boulder.icn");
    sprintf(gText, "catsnd%02d.82M", COMBAT_CATAPULT_LAUNCH_SOUND);
    catapultSound37 = LoadPlaySample(gText);

    i32 impactX5 = -1;
    i32 impactY0 = -1;
    if (wallIndex17 != -1) {
        impactX5 = wallPos[wallIndex17][COMBAT_COORDINATE_X];
        impactY0 = wallPos[wallIndex17][COMBAT_COORDINATE_Y];
        targetHex4 = iWallToHexCell[wallIndex17];
    }
    if (towerIndex27 != -1) {
        impactX5 = towerPos[towerIndex27][COMBAT_COORDINATE_X];
        impactY0 = towerPos[towerIndex27][COMBAT_COORDINATE_Y];
        targetHex4 = iTowerToHexCell[towerIndex27];
    }
    if (gateIndex2 != -1) {
        impactX5 = doorPos[0][COMBAT_COORDINATE_X];
        impactY0 = doorPos[0][COMBAT_COORDINATE_Y];
        targetHex4 = COMBAT_CASTLE_HEX_GATE;
    }
    if (keepIndex6 != -1) {
        impactX5 = COMBAT_CATAPULT_KEEP_IMPACT_X;
        impactY0 = COMBAT_CATAPULT_KEEP_IMPACT_Y;
        targetHex4 = COMBAT_CASTLE_HEX_KEEP;
    }

    i32 startX7;
    float bounceY0;
    i32 frame18;

    if (m_heroes[COMBAT_ATTACKER_SIDE]
            ->m_secondarySkills[HERO_SKILL_BALLISTICS] ==
        HERO_SKILL_LEVEL_NONE) {
        firstRoll7 = SRandom(COMBAT_CATAPULT_BALLISTICS_ROLL_MIN,
                             COMBAT_CATAPULT_BALLISTICS_ROLL_MAX);
        if (!gbHumanPlayer[m_heroes[COMBAT_ATTACKER_SIDE]->m_owner])
            firstRoll7 -= COMBAT_CATAPULT_AI_ROLL_PENALTY;
        if (firstRoll7 < COMBAT_CATAPULT_NO_SKILL_DOUBLE_THRESHOLD)
            damageLevel13 = COMBAT_CATAPULT_DAMAGE_DOUBLE;
        else if (firstRoll7 > COMBAT_CATAPULT_NO_SKILL_MISS_THRESHOLD) {
            missShot19 = 1;
            damageLevel13 = COMBAT_CATAPULT_DAMAGE_NONE;
        }
    } else if (m_heroes[COMBAT_ATTACKER_SIDE]
                   ->m_secondarySkills[HERO_SKILL_BALLISTICS] <=
               HERO_SKILL_LEVEL_ADVANCED) {
        advancedRoll5 = SRandom(COMBAT_CATAPULT_BALLISTICS_ROLL_MIN,
                                COMBAT_CATAPULT_BALLISTICS_ROLL_MAX);
        if (!gbHumanPlayer[m_heroes[COMBAT_ATTACKER_SIDE]->m_owner])
            advancedRoll5 -= COMBAT_CATAPULT_AI_ROLL_PENALTY;
        if (advancedRoll5 < COMBAT_CATAPULT_ADVANCED_DOUBLE_THRESHOLD)
            damageLevel13 = COMBAT_CATAPULT_DAMAGE_DOUBLE;
    } else if (m_heroes[COMBAT_ATTACKER_SIDE]
                   ->m_secondarySkills[HERO_SKILL_BALLISTICS] ==
               HERO_SKILL_LEVEL_EXPERT) {
        damageLevel13 = COMBAT_CATAPULT_DAMAGE_DOUBLE;
    }

    if (missShot19) {
        u8 validMissHexes[COMBAT_CATAPULT_MISS_HEX_COUNT] = {
            32, 46, 10, 23, 35, 48, 60, 74, 87, 101, 114
        };
        i32 startDirection = SRandom(COMBAT_CATAPULT_DIRECTION_ROLL_MIN,
                                     COMBAT_CATAPULT_DIRECTION_ROLL_MAX);
        for (frame18 = 0; frame18 < COMBAT_CATAPULT_DIRECTION_COUNT;
             frame18++) {
            i32 adjacentHex = GetAdjacentCellIndexNoArmy(
                targetHex4,
                (frame18 + startDirection) % COMBAT_CATAPULT_DIRECTION_COUNT);
            if (adjacentHex != -1 &&
                m_hexCells[adjacentHex].m_occupantSide == -1) {
                for (index28 = 0; index28 < COMBAT_CATAPULT_MISS_HEX_COUNT;
                     index28++) {
                    if (validMissHexes[index28] == adjacentHex) {
                        impactX5 = m_hexCells[adjacentHex].m_x;
                        impactY0 = m_hexCells[adjacentHex].m_y -
                                   COMBAT_CATAPULT_MISS_Y_OFFSET;
                        goto foundMissHex;
                    }
                }
            }
        }
        missShot19 = 0;
        damageLevel13 = COMBAT_CATAPULT_DAMAGE_NORMAL;
foundMissHex:
        frame18++;
    }

    giMinExtentX = 0;
    giMaxExtentX = COMBAT_CATAPULT_INITIAL_MAX_X;
    giMinExtentY = COMBAT_CATAPULT_INITIAL_MIN_Y;
    giMaxExtentY = COMBAT_CATAPULT_INITIAL_MAX_Y;
    m_catapultFrame[side] = 0;
    while (m_catapultFrame[side] < COMBAT_CATAPULT_LOAD_FRAME_COUNT) {
        DrawFrame(1, 0, 1, 0, COMBAT_CATAPULT_ANIMATION_DELAY, 1, 1);
        m_catapultFrame[side]++;
    }

    startX7 = COMBAT_CATAPULT_START_X;
    i32 startY9 = COMBAT_CATAPULT_START_Y;
    i32 spriteFrame27 = 0;
    float projectileX11 = static_cast<float>(startX7);
    float projectileY4 = static_cast<float>(startY9);
    float midpointX19 = static_cast<float>(
        (impactX5 + startX7) / COMBAT_CATAPULT_MIDPOINT_DIVISOR);
    bounceY0 = static_cast<float>(
        (impactY0 - (impactX5 - startX7) *
                         COMBAT_CATAPULT_DISTANCE_ARC_SCALE) -
        impactY0 * COMBAT_CATAPULT_TARGET_ARC_SCALE);
    float xStep0 = static_cast<float>(
        (midpointX19 - startX7) / COMBAT_CATAPULT_HORIZONTAL_STEP_DIVISOR);
    float yStep15 = (bounceY0 - startY9) /
                    COMBAT_CATAPULT_VERTICAL_STEP_DIVISOR;
    i32 previousX5 = -1;
    i32 previousY2 = -1;
    SLimitData limits1;
    for (frame18 = 0; frame18 < COMBAT_CATAPULT_PROJECTILE_FRAME_COUNT;
         frame18++) {
        if (frame18 == COMBAT_CATAPULT_DESCENT_FRAME)
            yStep15 = (bounceY0 - impactY0) /
                      COMBAT_CATAPULT_VERTICAL_STEP_DIVISOR;
        if (frame18 != 0) {
            giMinExtentX = previousX5 -
                           COMBAT_CATAPULT_PROJECTILE_EXTENT_RADIUS;
            giMaxExtentX = static_cast<i32l>(
                projectileX11 + COMBAT_CATAPULT_PROJECTILE_EXTENT_RADIUS);
            giMinExtentY = static_cast<i32l>(
                projectileY4 - COMBAT_CATAPULT_PROJECTILE_EXTENT_RADIUS);
            giMaxExtentY = static_cast<i32l>(
                projectileY4 + COMBAT_CATAPULT_PROJECTILE_EXTENT_RADIUS);
            if (previousY2 < projectileY4)
                giMinExtentY = previousY2 -
                               COMBAT_CATAPULT_PROJECTILE_EXTENT_RADIUS;
            else
                giMaxExtentY = previousY2 +
                               COMBAT_CATAPULT_PROJECTILE_EXTENT_RADIUS;
            if (frame18 <= COMBAT_CATAPULT_EARLY_EXTENT_LAST_FRAME) {
                giMinExtentX = 0;
                giMaxExtentY = COMBAT_CATAPULT_INITIAL_MAX_Y;
            }
            if (giMinExtentX < 0)
                giMinExtentX = 0;
            if (giMinExtentY < 0)
                giMinExtentY = 0;
            if (giMaxExtentX > COMBAT_MAX_EXTENT_X)
                giMaxExtentX = COMBAT_MAX_EXTENT_X;
            if (giMaxExtentY > COMBAT_MAX_EXTENT_Y)
                giMaxExtentY = COMBAT_MAX_EXTENT_Y;
            if (giMaxExtentY < COMBAT_CATAPULT_MIN_VISIBLE_EXTENT_Y)
                giMaxExtentY = COMBAT_CATAPULT_MIN_VISIBLE_EXTENT_Y;
        }
        DrawFrame(0, 0, 1, 0, COMBAT_CATAPULT_PROJECTILE_DELAY, 1, 1);
        boulder37->CombatClipDrawToBuffer(
            static_cast<i32l>(projectileX11), static_cast<i32l>(projectileY4),
            spriteFrame27, &limits1, 0, 0, 0, 0);
        gpWindowManager->UpdateScreenRegion(
            giMinExtentX, giMinExtentY,
            giMaxExtentX - giMinExtentX + 1,
            giMaxExtentY - giMinExtentY + 1);
        previousX5 = static_cast<i32l>(projectileX11);
        previousY2 = static_cast<i32l>(projectileY4);
        projectileX11 = projectileX11 + xStep0;
        projectileY4 = (COMBAT_CATAPULT_DESCENT_FRAME - frame18) * yStep15 +
                       projectileY4;
        spriteFrame27++;
        spriteFrame27 %= COMBAT_CATAPULT_BOULDER_FRAME_COUNT;
        if (frame18 < COMBAT_CATAPULT_ADVANCE_FRAME_COUNT)
            m_catapultFrame[side]++;
    }

    sprintf(gText, "catsnd%02d.82M", COMBAT_CATAPULT_IMPACT_SOUND);
    impactSound1 = LoadPlaySample(gText);
    m_unknownF34B = 0;
    m_unknownF34F = 0;

    giMinExtentX = impactX5 - COMBAT_CATAPULT_IMPACT_EXTENT_X;
    giMaxExtentX = impactX5 + COMBAT_CATAPULT_IMPACT_EXTENT_X;
    giMinExtentY = impactY0 - COMBAT_CATAPULT_IMPACT_EXTENT_TOP;
    giMaxExtentY = impactY0 + COMBAT_CATAPULT_IMPACT_EXTENT_BOTTOM;
    if (gateIndex2 != -1)
        giMinExtentX -= COMBAT_CATAPULT_IMPACT_EXTENT_X;
    if (giMinExtentX < 0)
        giMinExtentX = 0;
    if (giMaxExtentX > COMBAT_MAX_EXTENT_X)
        giMaxExtentX = COMBAT_MAX_EXTENT_X;
    if (giMinExtentY < 0)
        giMinExtentY = 0;
    if (giMaxExtentY > COMBAT_MAX_EXTENT_Y)
        giMaxExtentY = COMBAT_MAX_EXTENT_Y;

    icon *cloud29 = gpResourceManager->GetIcon(
        missShot19 ? "smalclod.icn" : "lichclod.icn");

    for (frame18 = 0; frame18 < COMBAT_CATAPULT_CLOUD_FRAME_COUNT;
         frame18++) {
        if (frame18 >= COMBAT_CATAPULT_CLOUD_VISIBLE_FRAME_COUNT &&
            (gateIndex2 != -1 || missShot19 != 0))
            continue;
        glTimers[COMBAT_CATAPULT_TIMER_SLOT] = static_cast<i32l>(
            KBTickCount() + gfCombatSpeedMod[gConfig.combatSpeed] *
                                COMBAT_CATAPULT_ANIMATION_DELAY);
        DrawFrame(0, 0, 1, 0, 0, 1, 0);
        if (gateIndex2 != -1 &&
            frame18 >= COMBAT_CATAPULT_GATE_CLOUD_FIRST_FRAME &&
            !missShot19) {
            IconToBitmap(
                cloud29, gpWindowManager->m_screen,
                impactX5 - COMBAT_CATAPULT_GATE_CLOUD_X_OFFSET,
                impactY0 + COMBAT_CATAPULT_GATE_CLOUD_Y_OFFSET,
                frame18 - COMBAT_CATAPULT_GATE_CLOUD_FIRST_FRAME, 1, 0, 0,
                COMBAT_SCREEN_WIDTH, COMBAT_AREA_HEIGHT, 0);
        }
        if (frame18 < COMBAT_CATAPULT_CLOUD_VISIBLE_FRAME_COUNT) {
            IconToBitmap(cloud29, gpWindowManager->m_screen, impactX5,
                         impactY0 + (!missShot19
                                         ? COMBAT_CATAPULT_CLOUD_Y_OFFSET
                                         : 0),
                         frame18, 1, 0, 0, COMBAT_SCREEN_WIDTH,
                         COMBAT_AREA_HEIGHT, 0);
        }
        gpWindowManager->UpdateScreenRegion(
            giMinExtentX, giMinExtentY,
            giMaxExtentX - giMinExtentX + 1,
            giMaxExtentY - giMinExtentY + 1);
        DelayTil(&glTimers[COMBAT_CATAPULT_TIMER_SLOT]);

        if (((frame18 == COMBAT_CATAPULT_WALL_IMPACT_FRAME &&
              gateIndex2 == -1) ||
             (frame18 == COMBAT_CATAPULT_GATE_IMPACT_FRAME &&
              gateIndex2 != -1)) &&
            !missShot19) {
            if (wallIndex17 != -1) {
                if (m_wallStates[wallIndex17 +
                                 COMBAT_WALL_SLOT_SECTION_FIRST] >=
                        COMBAT_WALL_STATE_SECTION_DAMAGE_FIRST &&
                    m_wallStates[wallIndex17 +
                                 COMBAT_WALL_SLOT_SECTION_FIRST] <=
                        COMBAT_WALL_STATE_SECTION_DAMAGE_LAST) {
                    m_wallStates[wallIndex17 +
                                 COMBAT_WALL_SLOT_SECTION_FIRST] +=
                        damageLevel13;
                    if (m_wallStates[wallIndex17 +
                                     COMBAT_WALL_SLOT_SECTION_FIRST] >
                        COMBAT_WALL_STATE_SECTION_DAMAGE_LAST)
                        m_wallStates[wallIndex17 +
                                     COMBAT_WALL_SLOT_SECTION_FIRST] =
                            COMBAT_WALL_STATE_SECTION_DESTROYED;
                } else {
                    if (damageLevel13 > COMBAT_CATAPULT_DAMAGE_NORMAL)
                        m_wallStates[wallIndex17 +
                                     COMBAT_WALL_SLOT_SECTION_FIRST] =
                            COMBAT_WALL_STATE_DESTROYED;
                    else
                        m_wallStates[wallIndex17 +
                                     COMBAT_WALL_SLOT_SECTION_FIRST]++;
                }
                if (m_wallStates[wallIndex17 +
                                 COMBAT_WALL_SLOT_SECTION_FIRST] ==
                        COMBAT_WALL_STATE_DESTROYED ||
                    m_wallStates[wallIndex17 +
                                 COMBAT_WALL_SLOT_SECTION_FIRST] ==
                        COMBAT_WALL_STATE_SECTION_DESTROYED)
                    m_hexCells[iWallToHexCell[wallIndex17]].m_blocked = 0;
            } else if (towerIndex27 != -1) {
                m_wallStates[towerIndex27] = COMBAT_WALL_STATE_DESTROYED;
            } else if (gateIndex2 != -1) {
                m_drawbridgeState = COMBAT_CASTLE_GATE_HIDDEN;
            } else if (keepIndex6 != -1) {
                m_wallStates[COMBAT_WALL_SLOT_KEEP] =
                    COMBAT_WALL_STATE_KEEP_DESTROYED;
            }
        }
    }

    gpResourceManager->Dispose(cloud29);
    m_catapultFrame[side] = 0;
    DrawFrame(1, 0, 0, 0, COMBAT_CATAPULT_ANIMATION_DELAY, 1, 1);
    gpResourceManager->Dispose(boulder37);
    WaitEndSample(impactSound1, -1);
    WaitEndSample(catapultSound37, -1);
    if (loadedSample26)
        gpResourceManager->Dispose(loadedSample26);
    LogStr("CA2");
}

// @semantic: The first raw divergence is +0x2e0: the SAMPLE2 compiler temporary
// uses base -0xcc/-0xc8 versus retail -0xc8/-0xc4; nested armyNameValue uses
// base -0xc4 versus retail -0xcc. The 0xd8 frame size, other slots, CFG, and all
// 44 ordered relocations agree. Three val|0 spellings recover both comparison
// orders. The recovered defense field and the singular "perishes" / plural
// "perish" selection replace incorrect source semantics without changing the
// current score or resolved relocations. Bucket-12/-14 wider-scope pointer
// placements regressed and a nested bucket rename was stagnant. Revisit only
// after compiler allocation or the source/TU/header epoch changes.
VA(0x0049412d, 0x74f)
void combatManager::KeepAttack(i32 tower)
{
    if (!m_inCastleCombat)
        return;
    if ((tower == COMBAT_TOWER_SELECTOR_GARRISON &&
         m_wallStates[COMBAT_WALL_SLOT_KEEP] !=
             COMBAT_WALL_STATE_KEEP_STANDING) ||
        (tower == COMBAT_TOWER_SELECTOR_TOP &&
         m_wallStates[COMBAT_WALL_SLOT_TOP_TOWER] !=
             COMBAT_WALL_STATE_TOWER_STANDING) ||
        (tower == COMBAT_TOWER_SELECTOR_BOTTOM &&
         m_wallStates[COMBAT_WALL_SLOT_BOTTOM_TOWER] !=
             COMBAT_WALL_STATE_TOWER_STANDING))
        return;

    LogStr("KA1");

    i32 bestPriority0 = -1;
    i32 bestValue10 = 0;
    i32 bestArmyIndex5 = -1;
    i32 armyIndex3;
    army *target0;
    i32 value26;
    for (armyIndex3 = 0; armyIndex3 < COMBAT_ARMY_CAPACITY; armyIndex3++) {
        if (m_armies[COMBAT_ATTACKER_SIDE][armyIndex3].IsAlive()) {
            target0 = &m_armies[COMBAT_ATTACKER_SIDE][armyIndex3];
            i32 priority;
            if (target0->m_spellInfluence[ARMY_SPELL_INFLUENCE_BLIND] ||
                target0->m_spellInfluence[ARMY_SPELL_INFLUENCE_PARALYZE] ||
                target0->m_spellInfluence[ARMY_SPELL_INFLUENCE_PETRIFIED] ||
                target0->m_spellInfluence[ARMY_SPELL_INFLUENCE_BERSERK] ||
                target0->m_spellInfluence[ARMY_SPELL_INFLUENCE_HYPNOTIZE]) {
                priority = COMBAT_KEEP_PRIORITY_DISABLED;
            } else if (target0->m_monster.flags.all &
                       MONSTER_FLAGS_SHOOTER) {
                priority = COMBAT_KEEP_PRIORITY_SHOOTER;
            } else if (target0->m_monster.flags.all &
                       MONSTER_FLAGS_FLYING) {
                priority = COMBAT_KEEP_PRIORITY_FLYER;
            } else {
                priority = COMBAT_KEEP_PRIORITY_WALKER;
            }

            value26 = gMonsterDatabase[target0->m_monsterType].fightValue *
                      target0->m_quantity;
            // Retail +0x25b loads bestValue10, compares value26, then uses jle.
            // Reversing both operands produces equivalent jge and differs at
            // function offsets +0x25d, +0x260, and +0x262.
            if ((bestPriority0 | 0) < priority ||
                ((bestPriority0 | 0) == priority && bestValue10 < value26)) {
                bestValue10 = value26;
                bestPriority0 = priority;
                bestArmyIndex5 = armyIndex3;
            }
        }
    }

    if (bestArmyIndex5 == -1)
        return;

    target0 = &gpCombatManager->m_armies[COMBAT_ATTACKER_SIDE][bestArmyIndex5];
    sprintf(gText, "keepshot.82M");
    SAMPLE2 keepSample19 = NULL_SAMPLE2;
    keepSample19 = LoadPlaySample(gText);

    CombatTowerOrigin
        towerOrigins4[COMBAT_KEEP_TOWN_TYPE_COUNT][COMBAT_KEEP_TOWER_COUNT] = {
        {{586, 177}, {428, 60}, {428, 314}},
        {{586, 177}, {428, 60}, {428, 314}},
        {{586, 177}, {428, 60}, {428, 314}},
        {{586, 177}, {428, 60}, {428, 314}},
        {{586, 177}, {428, 60}, {428, 314}},
        {{586, 177}, {428, 60}, {428, 314}}
    };
    i32 unknownTowerData6[4];
    i32 sourceX9 = towerOrigins4[m_combatTowns[COMBAT_DEFENDER_SIDE]->m_type]
                              [tower]
                                  .x;
    i32 sourceY6 = towerOrigins4[m_combatTowns[COMBAT_DEFENDER_SIDE]->m_type]
                              [tower]
                                  .y;
    i32 targetX9 = target0->MidX();
    i32 targetY8 = target0->MidY();
    float missileAngles0[COMBAT_KEEP_MISSILE_ANGLE_COUNT] = {
        90.0f, 68.5f, 45.0f, 20.8f, 0.0f,
        -20.8f, -45.0f, -68.5f, -90.0f
    };
    ShootMissile(sourceX9, sourceY6, targetX9, targetY8, missileAngles0,
                 m_combatIcons[COMBAT_ICON_KEEP]);

    i32 shotCount28;
    i32 attackBonus4;
    m_combatTowns[COMBAT_DEFENDER_SIDE]->CalcNumLevelArchers(&shotCount28,
                                                              &attackBonus4);
    attackBonus4 += COMBAT_KEEP_TOWER_DAMAGE_BONUS;
    if (m_heroes[COMBAT_DEFENDER_SIDE])
        attackBonus4 +=
            m_heroes[COMBAT_DEFENDER_SIDE]->Stats(HERO_PRIMARY_ATTACK);
    attackBonus4 -= target0->m_monster.defense;
    if (attackBonus4 > COMBAT_KEEP_ATTACK_STAT_LIMIT)
        attackBonus4 = COMBAT_KEEP_ATTACK_STAT_LIMIT;
    if (attackBonus4 < -COMBAT_KEEP_ATTACK_STAT_LIMIT)
        attackBonus4 = -COMBAT_KEEP_ATTACK_STAT_LIMIT;
    if (tower != COMBAT_TOWER_SELECTOR_GARRISON)
        shotCount28 /= COMBAT_KEEP_SIDE_TOWER_SHOT_DIVISOR;

    i32 damage8 = 0;
    for (armyIndex3 = 0; (shotCount28 | 0) > armyIndex3; armyIndex3++)
        damage8 += SRandom(COMBAT_KEEP_RANDOM_DAMAGE_MIN,
                           COMBAT_KEEP_RANDOM_DAMAGE_MAX);
    damage8 = static_cast<i32l>(
        damage8 * gfBattleStat[attackBonus4 +
                              COMBAT_KEEP_ATTACK_STAT_INDEX_OFFSET]);
    if (damage8 <= 0)
        damage8 = COMBAT_KEEP_MIN_DAMAGE;

    i32 killed29 = target0->Damage(damage8, -1);
    if (killed29 > 0) {
        char *armyNameValue;
        if (killed29 > 1)
            armyNameValue = gArmyNamesPlural[target0->m_monsterType];
        else
            armyNameValue = gArmyNames[target0->m_monsterType];
        sprintf(gText, "%s %d %s.\n%d %s %s.",
                tower == COMBAT_TOWER_SELECTOR_GARRISON ? "Garrison does"
                                                        : "Tower does",
                damage8,
                "damage", killed29, armyNameValue,
                killed29 > 1 ? "perish" : "perishes");
    } else {
        sprintf(gText, "%s %d %s.",
                tower == COMBAT_TOWER_SELECTOR_GARRISON ? "Garrison does"
                                                        : "Tower does",
                damage8,
                "damage");
    }
    gpCombatManager->CombatMessage(gText, 1, 1, 0);
    target0->CancelSpellType(ARMY_CANCEL_SPELLS_AFTER_DAMAGE);
    target0->PowEffect(-1, 1, -1, -1);
    WaitEndSample(keepSample19, -1);
    LogStr("KA2");
}

// @semantic: Retained 100.00% at source hash f53df3a552e3; do not replace this
// exact recovered body to improve the transient live score. The 0xc frame, all
// three stack slots, 0x17b size, loop CFG, and sole relocation still align. The
// current TU expands the commutative army index before the side offset, while
// retail used side-first order. Flattened pointer arithmetic raised the live
// score to 84.09% but changed the exact source hash; reversed/indexed subscripts,
// explicit array casts, operand swaps, scalar wrappers, and an inline side-offset
// helper did not restore exact output. A diagnostic TU-state probe reached
// 99.99167% but invented a class solely for matching and regressed a sibling.
// The current ten-trial diagnostic again produced a disposable exact member probe,
// rejected by the sibling/exact guards; the other nine were stagnant. Revisit only
// after a real TU/header or comparison-epoch change.
VA(0x0049487c, 0x17b)
i32 combatManager::ExperienceValueOfStack(i32 side)
{
    i32 experienceValue6 = 0;
    i32 index;

    for (index = 0; index < COMBAT_ARMY_CAPACITY; index++) {
        if (m_armies[side][index].m_monsterType != -1 &&
            !(m_armies[side][index].m_monster.flags.all &
              MONSTER_FLAGS_SUMMONED)) {
            experienceValue6 +=
                (m_armies[side][index].m_initialQuantity -
                 m_armies[side][index].m_quantity) *
                gMonsterDatabase[m_armies[side][index].m_monsterType]
                    .hitPoints;
        }
    }
    if (m_heroes[side])
        experienceValue6 += COMBAT_HERO_EXPERIENCE_VALUE;
    return experienceValue6;
}

VA(0x004949f7, 0x88)
void combatManager::ResetHitByCreature(void)
{
    i32 side;
    i32 index;

    for (side = 0; side < COMBAT_MANAGER_SIDE_COUNT; side++) {
        for (index = 0; index < COMBAT_ARMY_CAPACITY; index++)
            m_armies[side][index].m_hitByCreature = 0;
    }
}

VA(0x00494a7f, 0x36)
i32 ValidHex(i32 hex)
{
    return hex >= 0 && hex <= COMBAT_VALID_HEX_MAX ? 1 : 0;
}

VA(0x00494ab5, 0x16)
void combatManager::SaveCombatBorder(void)
{
    return;
}

VA(0x00494acb, 0x16)
void combatManager::DrawCombatBorder(void)
{
    return;
}

// @semantic: The 0x58 frame, every local slot, semantics, CFG, and all 22/22
// external relocation targets align. Castle checks now use retail's defender-town
// field at +0x3283, not the stale original-town pointer at +0x31e6. Candidate size
// 0x4c9 is 15 bytes below retail 0x4d8: normalized instructions 179, 194, and 242
// use direct branches where retail uses the opposite branch plus a five-byte
// continuation. Instructions 207 and 264 retain equivalent obstacle/cell-index
// address order. Split conditions, explicit exits/continues, <= 2 / >= 10 bounds,
// and direct versus typed inverted cell-offset subscripts are exhausted.
// The table is exactly 0x1c0 bytes (32 * 0xe) before gEstatesGoldLevel; retail
// SRandom is inclusive and its literal high 32 can index past both 32-entry
// arrays. Preserve that retail defect; never expand the table or change it to 31.
// Revisit only after the source/TU/header or comparison epoch changes.
VA(0x00494ae1, 0x4d8)
void combatManager::SetupAndLoadObstacles(void)
{
    u8 obstacleUsed[COMBAT_OBSTACLE_TYPE_COUNT];
    i32 overlayIndex14;
    i32 obstacleType4;
    i32 blocked6;
    i32 anchorHex9;
    i32 tryCount28;
    i32 anchorRow2;
    i32 obstacleCells18;
    i32 cellIndex1;
    u32 terrainMask9;
    i32 obstacleHex2;
    i32 elevationCells4;
    i32 obstacleGoal7;

    m_debugFormation = 0;
    if (m_inCastleCombat) {
        m_wallStates[COMBAT_WALL_SLOT_KEEP] = COMBAT_WALL_STATE_KEEP_STANDING;
        for (cellIndex1 = 0;
             cellIndex1 < COMBAT_CASTLE_STRUCTURE_COUNT;
             cellIndex1++) {
            m_wallStates[cellIndex1 + COMBAT_WALL_SLOT_SECTION_FIRST] =
                COMBAT_WALL_STATE_KEEP_STANDING;
            if (m_combatTowns[COMBAT_DEFENDER_SIDE]->m_type == TOWN_TYPE_KNIGHT &&
                (m_combatTowns[COMBAT_DEFENDER_SIDE]->m_buildings &
                 TOWN_BUILDING_FORTIFICATIONS)) {
                m_wallStates[cellIndex1 + COMBAT_WALL_SLOT_SECTION_FIRST] =
                    COMBAT_WALL_STATE_SECTION_DAMAGE_FIRST;
            }
            m_wallStates[cellIndex1] = COMBAT_WALL_STATE_KEEP_STANDING;
        }
        if (m_combatTowns[COMBAT_DEFENDER_SIDE]->m_buildings &
            TOWN_BUILDING_LEFT_TURRET)
            m_wallStates[COMBAT_WALL_SLOT_TOP_TOWER] =
                COMBAT_WALL_STATE_TOWER_STANDING;
        if (m_combatTowns[COMBAT_DEFENDER_SIDE]->m_buildings &
            TOWN_BUILDING_RIGHT_TURRET)
            m_wallStates[COMBAT_WALL_SLOT_BOTTOM_TOWER] =
                COMBAT_WALL_STATE_TOWER_STANDING;

        m_hexCells[COMBAT_CASTLE_HEX_TOP_TOWER].m_blocked = 1;
        m_hexCells[COMBAT_CASTLE_HEX_TOP_WALL].m_blocked = 1;
        m_hexCells[COMBAT_CASTLE_HEX_SECOND_TOWER].m_blocked = 1;
        m_hexCells[COMBAT_CASTLE_HEX_SECOND_WALL].m_blocked = 1;
        m_hexCells[COMBAT_CASTLE_HEX_GATE].m_blocked = 1;
        m_hexCells[COMBAT_CASTLE_HEX_THIRD_WALL].m_blocked = 1;
        m_hexCells[COMBAT_CASTLE_HEX_THIRD_TOWER].m_blocked = 1;
        m_hexCells[COMBAT_CASTLE_HEX_BOTTOM_WALL].m_blocked = 1;
        m_hexCells[COMBAT_CASTLE_HEX_BOTTOM_TOWER].m_blocked = 1;
        m_hexCells[COMBAT_CASTLE_HEX_MOAT].m_blocked = 1;
    } else {
        obstacleGoal7 = SRandom(COMBAT_RANDOM_OBSTACLE_MIN,
                                COMBAT_RANDOM_OBSTACLE_MAX);
        obstacleCells18 = 0;
        terrainMask9 = 1 << m_terrainType;
        tryCount28 = 0;
        elevationCells4 = 0;
        if (SRandom(0, COMBAT_RANDOM_PERCENT_MAX) <
            COMBAT_ELEVATION_OVERLAY_CHANCE) {
            while (tryCount28++ < COMBAT_ELEVATION_OVERLAY_TRY_LIMIT) {
                overlayIndex14 = SRandom(0, COMBAT_ELEVATION_OVERLAY_COUNT - 1);
                if (terrainMask9 &
                    sElevationOverlay[overlayIndex14].terrainMask) {
                    m_debugFormation = overlayIndex14;
                    for (cellIndex1 = 0;
                         cellIndex1 < COMBAT_ELEVATION_OVERLAY_CELL_COUNT;
                         cellIndex1++) {
                        if (sElevationOverlay[m_debugFormation]
                                .cellOffsets[cellIndex1] != -1) {
                            m_hexCells[sElevationOverlay[m_debugFormation]
                                           .cellOffsets[cellIndex1]]
                                .m_blocked = 1;
                            elevationCells4++;
                        }
                    }
                    break;
                }
            }
        }
        obstacleGoal7 -= elevationCells4 / 2;
        tryCount28 = 0;
        memset(obstacleUsed, 0, sizeof(obstacleUsed));
        while (obstacleCells18 < obstacleGoal7 &&
               tryCount28 < COMBAT_OBSTACLE_TRY_LIMIT) {
            tryCount28++;
            anchorHex9 = SRandom(0, COMBAT_OBSTACLE_CELL_ROLL_MAX);
            // Retail's inclusive high endpoint permits the out-of-range value 32.
            obstacleType4 = SRandom(0, COMBAT_OBSTACLE_INCLUSIVE_ROLL_HIGH);
            if (terrainMask9 & sCmbtObstacles[obstacleType4].terrainMask) {
                if (obstacleUsed[obstacleType4] == 0) {
                    anchorRow2 = anchorHex9 / COMBAT_GRID_ROW_LENGTH;
                    if (sCmbtObstacles[obstacleType4].minimumColumn <=
                        anchorRow2 + COMBAT_OBSTACLE_MIN_COLUMN_OFFSET) {
                        blocked6 = 0;
                        for (cellIndex1 = 0;
                             cellIndex1 < sCmbtObstacles[obstacleType4].cellCount;
                             cellIndex1++) {
                            obstacleHex2 =
                                anchorHex9 +
                                sCmbtObstacles[obstacleType4]
                                    .cellOffsets[cellIndex1];
                            if (obstacleHex2 % COMBAT_GRID_ROW_LENGTH <=
                                    COMBAT_OBSTACLE_LEFT_COLUMN_LIMIT - 1 ||
                                obstacleHex2 % COMBAT_GRID_ROW_LENGTH >=
                                    COMBAT_OBSTACLE_RIGHT_COLUMN_FIRST) {
                                blocked6 = 1;
                            }
                            if (m_hexCells[obstacleHex2].m_blocked != 0)
                                blocked6 = 1;
                        }
                        if (blocked6 == 0) {
                            tryCount28 = 0;
                            obstacleCells18 +=
                                sCmbtObstacles[obstacleType4].cellCount;
                            obstacleUsed[obstacleType4] = 1;
                            for (cellIndex1 = 0;
                                 cellIndex1 <
                                     sCmbtObstacles[obstacleType4].cellCount;
                                 cellIndex1++) {
                                m_hexCells[anchorHex9 +
                                           sCmbtObstacles[obstacleType4]
                                               .cellOffsets[cellIndex1]]
                                    .m_blocked = 1;
                            }
                            sprintf(gText, "cobj%04d.icn", obstacleType4);
                            m_obstacleIcons[m_obstacleCount] =
                                gpResourceManager->GetIcon(gText);
                            m_hexCells[anchorHex9].m_obstacleIndex =
                                static_cast<i8>(m_obstacleCount);
                            m_obstacleCount++;
                        }
                    }
                }
            }
        }
    }
}

// @early-stop
// Soft TU-cumulative wall: the exact 0x2a1 span, 0x20 frame, all seven semantic
// slots, CFG, and 20 ordered relocation sites agree. The ternary mask recovers
// retail's two sbb sequences. Only 11 unmasked bytes at +0x77..+0x83 remain: the
// commutative side/army-index address order for m_limitCreatureCount. Direct and
// typed inverted subscripts compile to the same instructions. Revisit after a
// source/TU/header or comparison-epoch change.
VA(0x00494fb9, 0x2a1)
void combatManager::MakeCreaturesVanish(void)
{
    ResetLimitCreature();
    i32 side3;
    i32 armyIndex0;
    army *removedArmy27;
    for (side3 = 0; side3 < COMBAT_MANAGER_SIDE_COUNT; side3++) {
        for (armyIndex0 = 0;
             armyIndex0 < gpCombatManager->m_armyCount[side3]; armyIndex0++) {
            if (m_removedArmies[side3][armyIndex0])
                m_limitCreatureCount[side3][armyIndex0] = 1;
        }
    }
    DrawFrame(0, 1, 0, 1, COMBAT_DOOR_ANIMATION_DELAY, 1, 1);
    i32 extentX2 = giMinExtentX;
    i32 extentY37 = giMinExtentY;
    i32 extentWidth8 = giMaxExtentX - giMinExtentX + 1;
    i32 extentHeight9 = giMaxExtentY - giMinExtentY + 1;
    for (side3 = 0; side3 < COMBAT_MANAGER_SIDE_COUNT; side3++) {
        for (armyIndex0 = 0;
             armyIndex0 < gpCombatManager->m_armyCount[side3]; armyIndex0++) {
            if (m_removedArmies[side3][armyIndex0]) {
                removedArmy27 = &m_armies[side3][armyIndex0];
                m_hexCells[removedArmy27->m_hex].m_occupantSide = -1;
                m_hexCells[removedArmy27->m_hex].m_occupantIndex = -1;
                if (removedArmy27->m_monster.flags.all & MONSTER_FLAGS_WIDE) {
                    m_hexCells[(((static_cast<u32>(
                                           removedArmy27->m_facing - 1) < 1 ? -1 : 0) &
                                  2) -
                                 1) +
                               removedArmy27->m_hex]
                        .m_occupantSide = -1;
                    m_hexCells[(((static_cast<u32>(
                                           removedArmy27->m_facing - 1) < 1 ? -1 : 0) &
                                  2) -
                                 1) +
                               removedArmy27->m_hex]
                        .m_occupantIndex = -1;
                }
            }
        }
    }
    gpWindowManager->SaveFizzleSource(extentX2, extentY37, extentWidth8,
                                      extentHeight9);
    gpCombatManager->DrawFrame(0, 0, 1, 0, COMBAT_DOOR_ANIMATION_DELAY, 1, 1);
    gpWindowManager->FizzleForward(
        extentX2, extentY37, extentWidth8, extentHeight9,
        static_cast<i32>(gfCombatSpeedMod[gConfig.combatSpeed] *
                         COMBAT_CREATURE_VANISH_DURATION),
        0, 0);
}

VA(0x0049525a, 0xbd)
void combatManager::LowerDoor(void)
{
    SAMPLE2 drawbridgeSample = NULL_SAMPLE2;
    drawbridgeSample = LoadPlaySample("drawbrg.82m");
    giMinExtentX = COMBAT_DOOR_EXTENT_MIN_X;
    giMinExtentY = COMBAT_DOOR_EXTENT_MIN_Y;
    giMaxExtentX = COMBAT_DOOR_EXTENT_MAX_X;
    giMaxExtentY = COMBAT_DOOR_EXTENT_MAX_Y;
    i32 bridgeFrame;
    for (bridgeFrame = COMBAT_DRAWBRIDGE_RAISE_FRAME_SECOND;
         bridgeFrame >= COMBAT_DRAWBRIDGE_LOWERED; bridgeFrame--) {
        m_drawbridgeState = bridgeFrame;
        DrawFrame(1, 0, 1, 0, COMBAT_DOOR_ANIMATION_DELAY, 1, 1);
    }
    WaitEndSample(drawbridgeSample, -1);
}

VA(0x00495317, 0xe6)
void combatManager::RaiseDoor(void)
{
    SAMPLE2 drawbridgeSample = NULL_SAMPLE2;
    drawbridgeSample = LoadPlaySample("drawbrg.82m");
    giMinExtentX = COMBAT_DOOR_EXTENT_MIN_X;
    giMinExtentY = COMBAT_DOOR_EXTENT_MIN_Y;
    giMaxExtentX = COMBAT_DOOR_EXTENT_MAX_X;
    giMaxExtentY = COMBAT_DOOR_EXTENT_MAX_Y;
    m_drawbridgeState = COMBAT_DRAWBRIDGE_RAISE_FRAME_FIRST;
    DrawFrame(1, 0, 1, 0, COMBAT_DOOR_ANIMATION_DELAY, 1, 1);
    m_drawbridgeState = COMBAT_DRAWBRIDGE_RAISE_FRAME_SECOND;
    DrawFrame(1, 0, 1, 0, COMBAT_DOOR_ANIMATION_DELAY, 1, 1);
    m_drawbridgeState = COMBAT_DRAWBRIDGE_RAISED;
    DrawFrame(1, 0, 1, 0, COMBAT_DOOR_ANIMATION_DELAY, 1, 1);
    WaitEndSample(drawbridgeSample, -1);
}

VA(0x004953fd, 0x84)
void combatManager::TestRaiseDoor(void)
{
    if (m_inCastleCombat &&
        m_drawbridgeState == COMBAT_DRAWBRIDGE_LOWERED &&
        m_hexCells[COMBAT_CASTLE_GATE_APPROACH_HEX].m_occupantSide == -1 &&
        m_hexCells[COMBAT_CASTLE_GATE_APPROACH_HEX].m_deadOccupantCount == 0 &&
        m_hexCells[COMBAT_CASTLE_HEX_GATE].m_occupantSide == -1 &&
        m_hexCells[COMBAT_CASTLE_HEX_GATE].m_deadOccupantCount == 0) {
        RaiseDoor();
    }
}

VA(0x00495481, 0xd8)
i32 combatManager::InCastle(i32 hex)
{
    return ((hex < COMBAT_CASTLE_INTERIOR_ROW_0_FIRST ||
         hex > COMBAT_CASTLE_INTERIOR_ROW_0_LAST) &&
        (hex < COMBAT_CASTLE_INTERIOR_ROW_1_FIRST ||
         hex > COMBAT_CASTLE_INTERIOR_ROW_1_LAST) &&
        (hex < COMBAT_CASTLE_INTERIOR_ROW_2_FIRST ||
         hex > COMBAT_CASTLE_INTERIOR_ROW_2_LAST) &&
        (hex < COMBAT_CASTLE_INTERIOR_ROW_3_FIRST ||
         hex > COMBAT_CASTLE_INTERIOR_ROW_3_LAST) &&
        (hex < COMBAT_CASTLE_INTERIOR_ROW_4_FIRST ||
         hex > COMBAT_CASTLE_INTERIOR_ROW_4_LAST) &&
        (hex < COMBAT_CASTLE_INTERIOR_ROW_5_FIRST ||
         hex > COMBAT_CASTLE_INTERIOR_ROW_5_LAST) &&
        (hex < COMBAT_CASTLE_INTERIOR_ROW_6_FIRST ||
         hex > COMBAT_CASTLE_INTERIOR_ROW_6_LAST) &&
        (hex < COMBAT_CASTLE_INTERIOR_ROW_7_FIRST ||
         hex > COMBAT_CASTLE_INTERIOR_ROW_7_LAST) &&
        (hex < COMBAT_CASTLE_INTERIOR_ROW_8_FIRST ||
         hex > COMBAT_CASTLE_INTERIOR_ROW_8_LAST)) ? 1 : 0;
}

// @early-stop
// Soft TU-cumulative wall: the 0x5c frame, all semantic/compiler-temporary slots,
// CFG, and 25 resolved relocations agree. Only +0x245/+0x248 and +0x24e/+0x251
// differ: retail loads traceColumn/traceRow before the commutative step, while this
// TU loads the step first. += and both explicit addition orders compile identically.
VA(0x00495559, 0x346)
i32 combatManager::ShotIsThroughWall(i32 side, i32 sourceHex, i32 targetHex)
{
    if (!m_inCastleCombat)
        return 0;
    if (m_heroes[side] &&
        (m_heroes[side]->HasArtifact(COMBAT_ARTIFACT_GOLDEN_BOW) ||
         m_heroes[side]->m_secondarySkills[HERO_SKILL_ARCHERY])) {
        return 0;
    }
    if (InCastle(sourceHex) || !InCastle(targetHex))
        return 0;

    i32 sourceColumn1 = sourceHex % COMBAT_GRID_ROW_LENGTH;
    i32 sourceRow9 = sourceHex / COMBAT_GRID_ROW_LENGTH;
    i32 targetColumn8 = targetHex % COMBAT_GRID_ROW_LENGTH;
    i32 targetRow26 = targetHex / COMBAT_GRID_ROW_LENGTH;
    i32 columnDistance4 = targetColumn8 - sourceColumn1;
    i32 rowDistance17 = targetRow26 - sourceRow9;
    i32 traceLength1;
    float columnStep5;
    float rowStep2;
    if (abs(columnDistance4) > abs(rowDistance17)) {
        traceLength1 = abs(columnDistance4);
        columnStep5 = columnDistance4 > 0 ? 1 : -1;
        rowStep2 = static_cast<float>(rowDistance17) /
                  static_cast<float>(abs(columnDistance4));
    } else {
        traceLength1 = abs(rowDistance17);
        rowStep2 = rowDistance17 > 0 ? 1 : -1;
        columnStep5 = static_cast<float>(columnDistance4) /
                     static_cast<float>(abs(rowDistance17));
    }
    columnStep5 /= static_cast<float>(COMBAT_WALL_TRACE_SUBDIVISIONS);
    rowStep2 /= static_cast<float>(COMBAT_WALL_TRACE_SUBDIVISIONS);
    float traceColumn6 = static_cast<float>(sourceColumn1);
    float traceRow1 = static_cast<float>(sourceRow9);
    i32 traceIndex13;
    i32 traceHex11;
    i32 structureIndex0;
    for (traceIndex13 = 0;
         traceIndex13 < traceLength1 * COMBAT_WALL_TRACE_SUBDIVISIONS;
         traceIndex13++) {
        traceColumn6 += columnStep5;
        traceRow1 += rowStep2;
        traceHex11 = static_cast<i32>(traceRow1) * COMBAT_GRID_ROW_LENGTH +
                     static_cast<i32>(traceColumn6);
        for (structureIndex0 = 0;
             structureIndex0 < COMBAT_CASTLE_STRUCTURE_COUNT;
             structureIndex0++) {
            if (iWallToHexCell[structureIndex0] == traceHex11 &&
                m_wallStates[structureIndex0 + COMBAT_WALL_SLOT_SECTION_FIRST] !=
                    COMBAT_WALL_STATE_DESTROYED &&
                m_wallStates[structureIndex0 + COMBAT_WALL_SLOT_SECTION_FIRST] !=
                    COMBAT_WALL_STATE_SECTION_DESTROYED) {
                return 1;
            }
            if (iTowerToHexCell[structureIndex0] == traceHex11 &&
                m_wallStates[structureIndex0] != COMBAT_WALL_STATE_DESTROYED) {
                return 1;
            }
            if (traceHex11 == COMBAT_CASTLE_HEX_GATE &&
                m_drawbridgeState == COMBAT_DRAWBRIDGE_RAISED) {
                return 1;
            }
        }
    }
    return 0;
}

// @semantic: One frame16 lifetime now serves direction and animation loops, giving
// retail's 0x80 frame; all 24 semantic slots, compiler temporaries, CFG, calls, and
// 52 resolved relocations agree. Residuals are the x/y square order, five integer
// sums, and three extent comparisons; the latter shift subsequent relocation sites.
// Direct commutation of every sum and both orientations of the comparisons compile
// identically. Remaining identities are QIfdiv/floating constants and gConfig.
// Revisit only after the source/TU/header or comparison epoch changes.
VA(0x0049589f, 0x52e)
void combatManager::ShootMissile(i32 sourceX, i32 sourceY, i32 targetX,
                                 i32 targetY, float *directionAngles,
                                 icon *missileIcon)
{
    i32 xDistance1 = targetX - sourceX;
    i32 yDistance19 = targetY - sourceY;
    i32 absoluteXDistance15 = targetX - sourceX;
    i8 reverseMissile7 = 0;
    if (absoluteXDistance15 < 0) {
        reverseMissile7 = 1;
        absoluteXDistance15 = -absoluteXDistance15;
    }
    i32 directionYDistance1 = targetY - sourceY;
    i32 directionFrame27;
    i32 frame16;
    if (absoluteXDistance15 == 0) {
        if (directionYDistance1 > 0)
            directionFrame27 = COMBAT_MISSILE_LAST_DIRECTION;
        else
            directionFrame27 = 0;
    } else {
        float directionSlope4 =
            static_cast<float>(-directionYDistance1) / absoluteXDistance15;
        float angle3 = static_cast<float>(
            atan(static_cast<double>(directionSlope4)) *
            COMBAT_MISSILE_DEGREES_PER_RADIAN / COMBAT_MISSILE_PI);
        for (frame16 = 1; frame16 < COMBAT_MISSILE_DIRECTION_COUNT;
             frame16++) {
            if ((directionAngles[frame16 - 1] +
                 directionAngles[frame16]) /
                    COMBAT_MISSILE_DIRECTION_AVERAGE_DIVISOR <
                angle3) {
                break;
            }
        }
        if (frame16 < COMBAT_MISSILE_DIRECTION_COUNT)
            directionFrame27 = frame16 - 1;
        else
            directionFrame27 = COMBAT_MISSILE_LAST_DIRECTION;
    }

    i32 distance4 = static_cast<i32>(sqrt(static_cast<double>(
        xDistance1 * xDistance1 + yDistance19 * yDistance19)));
    i32 missileSteps8 =
        (distance4 + COMBAT_MISSILE_SPACING_ROUND) / COMBAT_MISSILE_SPACING;
    i32 xStep29;
    i32 yStep17;
    if (missileSteps8 > 1) {
        xStep29 = xDistance1 / (missileSteps8 - 1);
        yStep17 = yDistance19 / (missileSteps8 - 1);
    } else {
        xStep29 = xDistance1;
        yStep17 = yDistance19;
    }
    i32 missileX16 = sourceX;
    i32 missileY7 = sourceY;
    i32 missileHalfWidth5 = COMBAT_MISSILE_HALF_WIDTH;
    i32 missileHalfHeight28 = COMBAT_MISSILE_HALF_HEIGHT;
    bitmap *missileBackground9 =
        new bitmap(COMBAT_MISSILE_BITMAP_TYPE, missileHalfWidth5 * 2,
                   missileHalfHeight28 * 2);
    missileBackground9->GrabBitmapCareful(
        gpWindowManager->m_screen,
        static_cast<i16>(missileX16 - missileHalfWidth5),
        static_cast<i16>(missileY7 - missileHalfHeight28));

    i32 oldX8 = missileX16;
    i32 oldY5 = missileY7;
    i32 minX8 = COMBAT_MAX_EXTENT_X;
    i32 maxX9 = 0;
    i32 minY5 = 480;
    i32 maxY6 = 0;
    for (frame16 = 0; missileSteps8 > frame16; frame16++) {
        if (oldX8 - missileHalfWidth5 < minX8)
            minX8 = oldX8 - missileHalfWidth5;
        if (minX8 < 0)
            minX8 = 0;
        if (oldX8 + missileHalfWidth5 > maxX9)
            maxX9 = oldX8 + missileHalfWidth5;
        if (maxX9 > COMBAT_MAX_EXTENT_X)
            maxX9 = COMBAT_MAX_EXTENT_X;
        if (oldY5 - missileHalfHeight28 < minY5)
            minY5 = oldY5 - missileHalfHeight28;
        if (minY5 < 0)
            minY5 = 0;
        if (oldY5 + missileHalfHeight28 > maxY6)
            maxY6 = oldY5 + missileHalfHeight28;
        if (maxY6 > COMBAT_MAX_EXTENT_Y)
            maxY6 = COMBAT_MAX_EXTENT_Y;

        if (frame16 != 0) {
            missileBackground9->DrawToBufferCareful(
                static_cast<i16>(oldX8 - missileHalfWidth5),
                static_cast<i16>(oldY5 - missileHalfHeight28));
            missileBackground9->GrabBitmapCareful(
                gpWindowManager->m_screen,
                static_cast<i16>(missileX16 - missileHalfWidth5),
                static_cast<i16>(missileY7 - missileHalfHeight28));
        } else {
            if (giMinExtentX > minX8)
                giMinExtentX = minX8;
            if (maxX9 > giMaxExtentX)
                giMaxExtentX = maxX9;
            if (minY5 < giMinExtentY)
                giMinExtentY = minY5;
            if (maxY6 > giMaxExtentY)
                giMaxExtentY = maxY6;
        }
        missileIcon->DrawToBuffer(missileX16, missileY7, directionFrame27,
                                  reverseMissile7);
        if (frame16 == 0) {
            gpWindowManager->UpdateScreenRegion(
                giMinExtentX, giMinExtentY,
                giMaxExtentX - giMinExtentX + 1,
                giMaxExtentY - giMinExtentY + 1);
        } else {
            DelayTil(glTimers);
            gpWindowManager->UpdateScreenRegion(
                minX8, minY5, maxX9 - minX8 + 1, maxY6 - minY5 + 1);
        }
        glTimers[0] = static_cast<i32>(
            KBTickCount() + gfCombatSpeedMod[gConfig.combatSpeed] *
                                COMBAT_MISSILE_TIMER_DELAY);
        oldX8 = missileX16;
        oldY5 = missileY7;
        missileX16 += xStep29;
        missileY7 += yStep17;
        minX8 = missileX16 - missileHalfWidth5;
        maxX9 = missileX16 + missileHalfWidth5;
        minY5 = missileY7 - missileHalfHeight28;
        maxY6 = missileY7 + missileHalfHeight28;
    }
    missileBackground9->DrawToBuffer(
        static_cast<i16>(oldX8 - missileHalfWidth5),
        static_cast<i16>(oldY5 - missileHalfHeight28));
    gpWindowManager->UpdateScreenRegion(
        oldX8 - missileHalfWidth5, oldY5 - missileHalfHeight28,
        missileHalfWidth5 * 2, missileHalfHeight28 * 2);
    delete missileBackground9;
}

VA(0x00495dcd, 0xf2)
void combatManager::CombatSystemOptions(void)
{
    tag_message message;
    bCPrefsChanged = 0;
    CSPanel = new heroWindow(160, 33, "cspanel.bin");
    if (!CSPanel)
        MemError();
    SetWinText(CSPanel, 1);
    UpdateCombatSystemOptions(1);
    gpWindowManager->DoDialog(CSPanel, CombatSystemOptionsHandler, 0);
    delete CSPanel;
    if (bCPrefsChanged)
        WritePrefs();
    m_backgroundDrawn = 0;
    DrawFrame(1, 0, 0, 0, COMBAT_DOOR_ANIMATION_DELAY, 1, 1);
}

VA(0x00495ebf, 0x1ea)
void UpdateCombatSystemOptions(i32 initialDraw)
{
    tag_message message;
    message.type = COMBAT_SYSTEM_OPTION_EVENT;
    message.payload.widget.command = COMBAT_SYSTEM_OPTION_BUTTON_MESSAGE;
    message.payload.widget.id = COMBAT_SYSTEM_OPTION_SPEED_BUTTON;
    message.payload.widget.data.value = gConfig.combatSpeed +
                      COMBAT_SYSTEM_OPTION_SPEED_STATE_OFFSET;
    CSPanel->BroadcastMessage(message);
    message.payload.widget.id = COMBAT_SYSTEM_OPTION_ARMY_INFO_BUTTON;
    message.payload.widget.data.value = gConfig.combatArmyInfoLevel +
                      COMBAT_SYSTEM_OPTION_ARMY_INFO_STATE_OFFSET;
    CSPanel->BroadcastMessage(message);
    message.payload.widget.id = COMBAT_SYSTEM_OPTION_AUTO_SPELL_BUTTON;
    message.payload.widget.data.value = gConfig.autoCombatUseSpells +
                      COMBAT_SYSTEM_OPTION_AUTO_SPELL_STATE_OFFSET;
    CSPanel->BroadcastMessage(message);
    message.payload.widget.id = COMBAT_SYSTEM_OPTION_GRID_BUTTON;
    message.payload.widget.data.value = gConfig.showCombatGrid +
                      COMBAT_SYSTEM_OPTION_GRID_STATE_OFFSET;
    CSPanel->BroadcastMessage(message);
    message.payload.widget.id = COMBAT_SYSTEM_OPTION_SHADE_BUTTON;
    message.payload.widget.data.value = gConfig.combatShadeLevel +
                      COMBAT_SYSTEM_OPTION_SHADE_STATE_OFFSET;
    CSPanel->BroadcastMessage(message);
    message.payload.widget.id = COMBAT_SYSTEM_OPTION_MOUSE_HEX_BUTTON;
    message.payload.widget.data.value = gConfig.showCombatMouseHex +
                      COMBAT_SYSTEM_OPTION_MOUSE_HEX_STATE_OFFSET;
    CSPanel->BroadcastMessage(message);

    message.payload.widget.command = COMBAT_SYSTEM_OPTION_TEXT_MESSAGE;
    message.payload.widget.id = COMBAT_SYSTEM_OPTION_SPEED_TEXT;
    message.payload.widget.data.text = combatSpeedText[gConfig.combatSpeed];
    CSPanel->BroadcastMessage(message);
    message.payload.widget.id = COMBAT_SYSTEM_OPTION_ARMY_INFO_TEXT;
    message.payload.widget.data.text = combatMiniInfoText[gConfig.combatArmyInfoLevel];
    CSPanel->BroadcastMessage(message);
    message.payload.widget.id = COMBAT_SYSTEM_OPTION_AUTO_SPELL_TEXT;
    message.payload.widget.data.text = onOffText[gConfig.autoCombatUseSpells];
    CSPanel->BroadcastMessage(message);
    message.payload.widget.id = COMBAT_SYSTEM_OPTION_GRID_TEXT;
    message.payload.widget.data.text = onOffText[gConfig.showCombatGrid];
    CSPanel->BroadcastMessage(message);
    message.payload.widget.id = COMBAT_SYSTEM_OPTION_SHADE_TEXT;
    message.payload.widget.data.text = onOffText[gConfig.combatShadeLevel];
    CSPanel->BroadcastMessage(message);
    message.payload.widget.id = COMBAT_SYSTEM_OPTION_MOUSE_HEX_TEXT;
    message.payload.widget.data.text = onOffText[gConfig.showCombatMouseHex];
    CSPanel->BroadcastMessage(message);
    if (!initialDraw)
        CSPanel->DrawWindow(1, 0, COMBAT_SYSTEM_OPTION_DRAW_MASK);
}

// @early-stop
// Retained/live 99.88%; relocation-masked raw bytes are identical
// across all 0x39a bytes, including jump-table spans +0x121..+0x13c and
// +0x2f0..+0x30b. Frame, slots, and CFG match; all 36/36 relocation sites align,
// with retail delinking 12 gConfig sites as local DATA aliases. Flat equality
// close handling was 95.17%; the nested switch supplies retail's hidden temp.
// Redraw/done declaration order and right-button flag forms are exhausted.
VA(0x004960a9, 0x39a)
i32 CombatSystemOptionsHandler(tag_message &message)
{
    i32 bRedraw = 0;
    i32 bDone = 0;
    char optionText[COMBAT_MESSAGE_LINE_SIZE];
    if (message.type == COMBAT_SYSTEM_OPTION_EVENT) {
        if (message.payload.widget.parameter & COMBAT_SYSTEM_OPTION_RIGHT_BUTTON) {
            if (message.payload.widget.command == COMBAT_SYSTEM_OPTION_BUTTON_EVENT ||
                message.payload.widget.command == COMBAT_SYSTEM_OPTION_HOVER_EVENT) {
                i32 helpIndex = -1;
                switch (message.payload.widget.id) {
                case COMBAT_SYSTEM_OPTION_CLOSE_BUTTON:
                    helpIndex = 0;
                    break;
                case COMBAT_SYSTEM_OPTION_SPEED_BUTTON:
                    helpIndex = 1;
                    break;
                case COMBAT_SYSTEM_OPTION_ARMY_INFO_BUTTON:
                    helpIndex = 2;
                    break;
                case COMBAT_SYSTEM_OPTION_AUTO_SPELL_BUTTON:
                    helpIndex = 3;
                    break;
                case COMBAT_SYSTEM_OPTION_GRID_BUTTON:
                    helpIndex = 4;
                    break;
                case COMBAT_SYSTEM_OPTION_SHADE_BUTTON:
                    helpIndex = 5;
                    break;
                case COMBAT_SYSTEM_OPTION_MOUSE_HEX_BUTTON:
                    helpIndex = 6;
                    break;
                }
                if (helpIndex >= 0) {
                    NormalDialog(gCSPanelHelp[helpIndex],
                                 COMBAT_SYSTEM_OPTION_HELP_DIALOG, -1, -1,
                                 -1, 0, -1, 0, -1, 0);
                }
            }
        } else {
            switch (message.payload.widget.command) {
            case COMBAT_SYSTEM_OPTION_CLOSE_EVENT:
                switch (message.payload.widget.id) {
                case COMBAT_SYSTEM_OPTION_CLOSE_BUTTON:
                    bDone = 1;
                    break;
                }
                break;
            case COMBAT_SYSTEM_OPTION_BUTTON_EVENT:
                switch (message.payload.widget.id) {
                case COMBAT_SYSTEM_OPTION_SPEED_BUTTON:
                    gConfig.combatSpeed =
                        (gConfig.combatSpeed + 1) %
                        COMBAT_SYSTEM_OPTION_CYCLE_COUNT;
                    bRedraw = 1;
                    bCPrefsChanged = 1;
                    break;
                case COMBAT_SYSTEM_OPTION_ARMY_INFO_BUTTON:
                    gConfig.combatArmyInfoLevel =
                        (gConfig.combatArmyInfoLevel + 1) %
                        COMBAT_SYSTEM_OPTION_CYCLE_COUNT;
                    bRedraw = 1;
                    bCPrefsChanged = 1;
                    break;
                case COMBAT_SYSTEM_OPTION_AUTO_SPELL_BUTTON:
                    gConfig.autoCombatUseSpells =
                        1 - gConfig.autoCombatUseSpells;
                    bRedraw = 1;
                    bCPrefsChanged = 1;
                    break;
                case COMBAT_SYSTEM_OPTION_GRID_BUTTON:
                    gConfig.showCombatGrid = 1 - gConfig.showCombatGrid;
                    bRedraw = 1;
                    bCPrefsChanged = 1;
                    break;
                case COMBAT_SYSTEM_OPTION_SHADE_BUTTON:
                    gConfig.combatShadeLevel = 1 - gConfig.combatShadeLevel;
                    bRedraw = 1;
                    bCPrefsChanged = 1;
                    break;
                case COMBAT_SYSTEM_OPTION_MOUSE_HEX_BUTTON:
                    gConfig.showCombatMouseHex =
                        1 - gConfig.showCombatMouseHex;
                    bRedraw = 1;
                    bCPrefsChanged = 1;
                    break;
                }
                break;
            }
        }
    }
    if (bRedraw)
        UpdateCombatSystemOptions(0);
    if (bDone) {
        gpWindowManager->m_dialogResult = message.payload.widget.id;
        message.payload.widget.id = COMBAT_SYSTEM_OPTION_SPEED_BUTTON;
        message.payload.widget.command = message.payload.widget.id;
        return COMBAT_SYSTEM_OPTION_HANDLER_CLOSE;
    }
    return COMBAT_SYSTEM_OPTION_HANDLER_CONTINUE;
}


// ===== vtable combatManager : public baseManager  (3 slots) =====
//  [ 0] VA(0x00490aa0, 0x43f)  int combatManager::Open(int)   <- override (implements baseManager pure virtual)
//  [ 1] VA(0x00490edf, 0x3d6)  void combatManager::Close(void)   <- override (implements baseManager pure virtual)
//  [ 2] VA(0x0042a6d0, 0x36d)  int combatManager::Main(struct tag_message &)   <- override (implements baseManager pure virtual)

// ---- vtables (compiler-emitted; census) ----
VTBL(combatManager, 0x004eb898);

// ---- globals (definitions, RVA order) ----
// @data-layout-note Retail CMBTMGR .data is 0xf8900+0x358. Its 59 local
// allocations and two referenced public integers account for all 64 HIGHLOW
// references to 61 distinct owner RVAs; the fresh candidate has the same
// target multiset, with one zero-addend reference per local allocation. Moving
// candidate wallHex from +0x8 to retail +0x31c and appending the retail four-byte
// zero tail translates the complete 0x354 candidate payload exactly: translated
// and retail SHA-256 are
// a0f7d69a73915ea5e0da8783fff680d9f5ea1995532c50896a13c31b84866fda.
// This is compiler allocation order, not missing storage. Candidate BSS is 0xc
// versus retail 0x10: candidate order is bCPrefsChanged, CSPanel,
// bMouseWasVis, while retail is bMouseWasVis, CSPanel, bCPrefsChanged followed
// by a four-byte loader-zero tail. Its 29 references match exactly at addend
// zero: 2 to bMouseWasVis, 19 to CSPanel, and 8 to bCPrefsChanged. The 0x68
// non-COMDAT .rdata pool is byte-exact (SHA-256
// 2e4e4857fd9f5831b891b16432a5068ad50eb1dba97f138793ad65a25c77728e).
// The vtable relocations are exactly Open, Close, Main; its candidate COMDAT is
// 0xc while the retail owner range is 0x10 with a zero tail. All six public
// allocations have the proven types, storage classes, and payloads. Revisit
// allocation order only with natural compiler evidence; do not add padding,
// aliases, synthetic identities, or unattached literals.
DATA(0x004f8900) i32 bInHighMoraleBonus = 0;
DATA(0x004f8904) i32 giSeed = 1;
DATA(0x004f8c1c) u8 wallHex[4] = { 9, 34, 86, 113 };
DATA(0x00528588) i32 bMouseWasVis;
DATA(0x0052858c) class heroWindow *CSPanel;
DATA(0x00528590) i32 bCPrefsChanged;
