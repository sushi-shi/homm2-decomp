#include <va.h>
#include <SOURCE/hero.h>
#include <SOURCE/army.h>
#include <SOURCE/KB_TYPES.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <BASE/message.h>
#include <BASE/DebugCheck.h>
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
#include <SOURCE/GAME.h>
#include <SOURCE/KB.h>
#include <SOURCE/kbwin.h>
#include <SOURCE/NOOPT.h>
#include <SOURCE/PATH.h>
#include <SOURCE/town.h>
#include <SOURCE/X_GLOBAL.h>

#define COMBAT_CATAPULT_HORIZONTAL_STEP_DIVISOR 12.5
#define COMBAT_CATAPULT_VERTICAL_STEP_DIVISOR 78.0f
#define COMBAT_CATAPULT_DISTANCE_ARC_SCALE                                         \
    0.3
#define COMBAT_CATAPULT_TARGET_ARC_SCALE 0.35
#define COMBAT_MISSILE_DEGREES_PER_RADIAN 180.0
#define COMBAT_MISSILE_PI 3.14159
#define COMBAT_MISSILE_DIRECTION_AVERAGE_DIVISOR 2.0f
#define COMBAT_CREATURE_VANISH_DURATION 150.0f

namespace {

H2_ENUM_BEGIN(CombatSystemOptionWidget)
    SYSTEM_OPTION_SPEED_BUTTON      = 10,
    SYSTEM_OPTION_ARMY_INFO_BUTTON  = 11,
    SYSTEM_OPTION_AUTO_SPELL_BUTTON = 12,
    SYSTEM_OPTION_GRID_BUTTON       = 13,
    SYSTEM_OPTION_SHADE_BUTTON      = 14,
    SYSTEM_OPTION_MOUSE_HEX_BUTTON  = 15,
    SYSTEM_OPTION_SPEED_TEXT        = 20,
    SYSTEM_OPTION_ARMY_INFO_TEXT    = 21,
    SYSTEM_OPTION_AUTO_SPELL_TEXT   = 22,
    SYSTEM_OPTION_GRID_TEXT         = 23,
    SYSTEM_OPTION_SHADE_TEXT        = 24,
    SYSTEM_OPTION_MOUSE_HEX_TEXT    = 25,
    SYSTEM_OPTION_CLOSE_BUTTON      = 0x7800
H2_ENUM_END(CombatSystemOptionWidget)

H2_ENUM_BEGIN(CombatSystemOptionConstant)
    SYSTEM_OPTION_CYCLE_COUNT             = 3,
    SYSTEM_OPTION_HELP_DIALOG             = 4,
    SYSTEM_OPTION_SPEED_STATE_OFFSET      = 0,
    SYSTEM_OPTION_ARMY_INFO_STATE_OFFSET  = 3,
    SYSTEM_OPTION_AUTO_SPELL_STATE_OFFSET = 6,
    SYSTEM_OPTION_GRID_STATE_OFFSET       = 8,
    SYSTEM_OPTION_SHADE_STATE_OFFSET      = 10,
    SYSTEM_OPTION_MOUSE_HEX_STATE_OFFSET  = 12,
    SYSTEM_OPTION_DRAW_MASK               = 0x7fff,
    SYSTEM_OPTION_WINDOW_X                = 160,
    SYSTEM_OPTION_WINDOW_Y                = 33
H2_ENUM_END(CombatSystemOptionConstant)

H2_ENUM_CLASS_BEGIN(CombatSystemOptionHelp)
    HELP_CLOSE      = 0,
    HELP_SPEED      = 1,
    HELP_ARMY_INFO  = 2,
    HELP_AUTO_SPELL = 3,
    HELP_GRID       = 4,
    HELP_SHADE      = 5,
    HELP_MOUSE_HEX  = 6
H2_ENUM_CLASS_END(CombatSystemOptionHelp)

H2_ENUM_BEGIN(CombatSetupConstant)
    BALLISTA_CATAPULT_ATTACK_COUNT      = 2,
    NECROMANCER_SHRINE_POWER_BONUS      = 2,
    DEFENDER_HERO_OVERLAY_INITIAL_FRAME = 3
H2_ENUM_END(CombatSetupConstant)

H2_ENUM_BEGIN(CombatPresentationConstant)
    FADE_STEPS          = 8,
    AMBIENT_MUSIC_FIRST = 2,
    AMBIENT_MUSIC_LAST  = 4
H2_ENUM_END(CombatPresentationConstant)

H2_ENUM_BEGIN(CombatMapConstant)
    MONSTER_COUNT_SAVE_LIMIT  = 4000,
    MAP_RANDOM_OFFSET_MINIMUM = 8,
    MAP_RANDOM_OFFSET_MAXIMUM = 15
H2_ENUM_END(CombatMapConstant)

H2_ENUM_BEGIN(CombatNearbyConstant)
    NEARBY_RADIUS_COUNT = 3
H2_ENUM_END(CombatNearbyConstant)

H2_ENUM_BEGIN(CombatKeepStorageConstant)
    KEEP_TOWER_SCRATCH_COUNT = 4
H2_ENUM_END(CombatKeepStorageConstant)

H2_ENUM_BEGIN(CombatObstacleConstant)
    ELEVATION_OBSTACLE_WEIGHT_DIVISOR = 2
H2_ENUM_END(CombatObstacleConstant)

H2_ENUM_BEGIN(CombatMissileConstant)
    MISSILE_DIAMETER_MULTIPLIER = 2
H2_ENUM_END(CombatMissileConstant)

H2_ENUM_CLASS_BEGIN(BattlefieldBackgroundIndex)
    BACKGROUND_WATER       = 0,
    BACKGROUND_GRASS_TREES = 2,
    BACKGROUND_GRASS       = 3,
    BACKGROUND_SNOW_TREES  = 4,
    BACKGROUND_SNOW        = 5,
    BACKGROUND_SWAMP       = 6,
    BACKGROUND_LAVA        = 8,
    BACKGROUND_DESERT      = 10,
    BACKGROUND_DIRT_TREES  = 12,
    BACKGROUND_DIRT        = 13,
    BACKGROUND_WASTELAND   = 14,
    BACKGROUND_BEACH       = 16
H2_ENUM_CLASS_END(BattlefieldBackgroundIndex)

H2_ENUM_CLASS_BEGIN_T(NearbyFeature, i8)
    NEARBY_UNKNOWN  = -1,
    NEARBY_MOUNTAIN = 0,
    NEARBY_TREE     = 1
H2_ENUM_CLASS_END_T(NearbyFeature, i8)

H2_ENUM_BEGIN(CombatCastleInteriorRange)
    CASTLE_INTERIOR_ROW_0_FIRST = 0,
    CASTLE_INTERIOR_ROW_0_LAST  = 8,
    CASTLE_INTERIOR_ROW_1_FIRST = 13,
    CASTLE_INTERIOR_ROW_1_LAST  = 21,
    CASTLE_INTERIOR_ROW_2_FIRST = 26,
    CASTLE_INTERIOR_ROW_2_LAST  = 33,
    CASTLE_INTERIOR_ROW_3_FIRST = 39,
    CASTLE_INTERIOR_ROW_3_LAST  = 46,
    CASTLE_INTERIOR_ROW_4_FIRST = 52,
    CASTLE_INTERIOR_ROW_4_LAST  = 58,
    CASTLE_INTERIOR_ROW_5_FIRST = 65,
    CASTLE_INTERIOR_ROW_5_LAST  = 72,
    CASTLE_INTERIOR_ROW_6_FIRST = 78,
    CASTLE_INTERIOR_ROW_6_LAST  = 85,
    CASTLE_INTERIOR_ROW_7_FIRST = 91,
    CASTLE_INTERIOR_ROW_7_LAST  = 99,
    CASTLE_INTERIOR_ROW_8_FIRST = 104,
    CASTLE_INTERIOR_ROW_8_LAST  = 112
H2_ENUM_END(CombatCastleInteriorRange)

H2_ENUM_CLASS_BEGIN(CombatCatapultDamage)
    CATAPULT_DAMAGE_NONE   = 0,
    CATAPULT_DAMAGE_NORMAL = 1,
    CATAPULT_DAMAGE_DOUBLE = 2
H2_ENUM_CLASS_END(CombatCatapultDamage)

H2_ENUM_CLASS_BEGIN(CombatKeepTargetPriority)
    KEEP_PRIORITY_NONE     = -1,
    KEEP_PRIORITY_DISABLED = 0,
    KEEP_PRIORITY_WALKER   = 1,
    KEEP_PRIORITY_FLYER    = 2,
    KEEP_PRIORITY_SHOOTER  = 3
H2_ENUM_CLASS_END(CombatKeepTargetPriority)

H2_ENUM_BEGIN(CombatMoraleConstant)
    MORALE_ROLL_MIN              = 1,
    GOOD_MORALE_ROLL_MAX         = 24,
    BAD_MORALE_ROLL_MAX          = 12,
    BAD_MORALE_NETWORK_ROLL_MAX  = 4,
    BAD_MORALE_NETWORK_SKIP_ROLL = 1,
    MORALE_EFFECT_DURATION       = 180
H2_ENUM_END(CombatMoraleConstant)

}

VA(0x00425de0, 0x1af)
combatManager::combatManager(void) {
    m_unknownF373 = -1;
    m_currentArmySide = COMBAT_DEFENDER_SIDE;
    m_currentSide = COMBAT_DEFENDER_SIDE;
    m_limitCreatureHex = 0;
    m_limitCreature = false;
    m_showArmyQuantities = 1;
    m_currentCommand = CombatMessageCommand(0);
    m_unknownF35B = 0;
    m_unknownF353 = -1;
    m_unknownF34F = -1;
    m_catapultFrame[IDX(COMBAT_ATTACKER_SIDE)] = -1;
    m_catapultFrame[IDX(COMBAT_DEFENDER_SIDE)] = -1;
    m_unknownF337[IDX(COMBAT_ATTACKER_SIDE)] = 0;
    m_unknownF337[IDX(COMBAT_DEFENDER_SIDE)] = 0;
    m_inCastleCombat = false;
    m_mouseGridHex = -1;
    m_combatWindowOpen = false;
    strcpy(m_previousCombatMessage,
           "");
    strcpy(m_currentCombatMessage,
           "");
}

VA(0x00425f8f, 0xfa)
void combatManager::CombineGroups(armyGroup* sourceGroup, armyGroup* targetGroup) {
    if (sourceGroup == NULL || targetGroup == NULL)
        return;

    i32 sourceIndex;
    for (sourceIndex = 0; sourceIndex < ARMY_GROUP_SLOT_COUNT; sourceIndex++) {
        if (targetGroup->IsMember(sourceGroup->m_creatureTypes[sourceIndex])) {
            targetGroup->Add(
                sourceGroup->m_creatureTypes[sourceIndex],
                sourceGroup->m_creatureCounts[sourceIndex],
                ARMY_GROUP_EMPTY_SLOT
            );
            sourceGroup->Dismiss(sourceIndex);
        }
    }

    for (sourceIndex = 0; sourceIndex < ARMY_GROUP_SLOT_COUNT; sourceIndex++) {
        if (sourceGroup->m_creatureTypes[sourceIndex] != CREATURE_NONE) {
            i32 targetIndex;
            for (targetIndex = 0; targetIndex < ARMY_GROUP_SLOT_COUNT; targetIndex++) {
                if (targetGroup->m_creatureTypes[targetIndex] == CREATURE_NONE) {
                    targetGroup->Add(
                        sourceGroup->m_creatureTypes[sourceIndex],
                        sourceGroup->m_creatureCounts[sourceIndex],
                        targetIndex
                    );
                    sourceGroup->Dismiss(sourceIndex);
                }
            }
        }
    }
}

VA(0x00426089, 0x55e)
void combatManager::SetupCombat(
    i32 mapX,
    i32 mapY,
    hero* attackerHero,
    armyGroup* attackerGroup,
    town* defenderTown,
    hero* defenderHero,
    armyGroup* defenderGroup,
    i32 combatX,
    i32 combatY,
    i32 randomSeed
) {
    giSeed = randomSeed;
    SRand(combatX * COMBAT_RANDOM_X_MULTIPLIER + combatY);
    m_combatX = combatX;
    m_combatY = combatY;

    if (mapX >= 0 && mapY >= 0)
        m_battlefieldCell = gpAdvManager->GetCell(mapX, mapY);
    else
        m_battlefieldCell = NULL;

    m_terrainType = CELL_TERRAIN(m_battlefieldCell);
    sprintf(m_battlefieldBackgroundName, GetBackgroundName());

    if (attackerHero != NULL) {
        m_playerId[IDX(COMBAT_ATTACKER_SIDE)] = attackerHero->m_owner;
        attackerGroup = &attackerHero->m_army;
    } else {
        m_playerId[IDX(COMBAT_ATTACKER_SIDE)] = -1;
    }

    if (defenderHero != NULL) {
        m_playerId[IDX(COMBAT_DEFENDER_SIDE)] = defenderHero->m_owner;
        defenderGroup = &defenderHero->m_army;
    } else if (defenderTown != NULL) {
        m_playerId[IDX(COMBAT_DEFENDER_SIDE)] = defenderTown->m_owner;
        defenderGroup = &defenderTown->m_army;
    } else {
        m_playerId[IDX(COMBAT_DEFENDER_SIDE)] = -1;
    }

    i32 index;
    for (index = IDX(COMBAT_ATTACKER_SIDE); index < COMBAT_SIDE_COUNT; index++) {
        if (m_playerId[index] >= 0)
            m_networkArmyPresent[index] = gbHumanPlayer[m_playerId[index]];
        else
            m_networkArmyPresent[index] = false;

        m_heroes[index] = index == IDX(COMBAT_ATTACKER_SIDE) ? attackerHero : defenderHero;

        if (m_heroes[index] != NULL)
            m_heroes[index]->m_isCaptain = 0;

        m_armyGroups[index] = index == IDX(COMBAT_ATTACKER_SIDE) ? attackerGroup : defenderGroup;

        m_catapultAttacksRemaining[index] = 1;
        m_catapultAttackCount[index] = 1;
        if (m_heroes[index] != NULL && m_heroes[index]->HasArtifact(ARTIFACT_BALLISTA)) {
            m_catapultAttacksRemaining[index] = BALLISTA_CATAPULT_ATTACK_COUNT;
            m_catapultAttackCount[index] = BALLISTA_CATAPULT_ATTACK_COUNT;
        }
        if (m_heroes[index] != NULL
            && m_heroes[index]->m_secondarySkills[IDX(HERO_SKILL_BALLISTICS)]
                   >= HERO_SKILL_LEVEL_ADVANCED) {
            m_catapultAttackCount[index]++;
            m_catapultAttacksRemaining[index]++;
        }
        m_keepAttacksRemaining[index] = 1;
        m_visitingHeroPresent[index] = false;
        m_heroCastSpell[index] = false;
    }

    m_drawbridgeBackgroundVisible = false;
    if (defenderTown != NULL) {
        if (defenderTown->m_occupyingHeroId != -1) {
            m_armyGroups[IDX(COMBAT_DEFENDER_SIDE)] = &m_heroes[IDX(COMBAT_DEFENDER_SIDE)]->m_army;
            CombineGroups(&defenderTown->m_army, &m_heroes[IDX(COMBAT_DEFENDER_SIDE)]->m_army);
            m_visitingHeroPresent[IDX(COMBAT_DEFENDER_SIDE)] = true;
        } else {
            m_visitingHeroPresent[IDX(COMBAT_DEFENDER_SIDE)] = false;
        }

        m_inCastleCombat = HAS(defenderTown->m_buildings, IDX(TOWN_BUILDING_CASTLE)) != 0;

        if (m_inCastleCombat != 0)
            m_drawbridgeBackgroundVisible =
                HAS(defenderTown->m_buildings, IDX(TOWN_BUILDING_MOAT)) != 0;

        m_drawbridgeState = COMBAT_CASTLE_GATE_OPEN;
        m_combatTowns[IDX(COMBAT_DEFENDER_SIDE)] = defenderTown;
        m_originalCombatTown = defenderTown;

        if (m_heroes[IDX(COMBAT_DEFENDER_SIDE)] == NULL
            && HAS(defenderTown->m_buildings, IDX(TOWN_BUILDING_CAPTAIN_QUARTERS))) {
            m_heroes[IDX(COMBAT_DEFENDER_SIDE)] = &m_captain;
            memset(&m_captain, 0, sizeof(m_captain));
            for (index = 0; index < HERO_PRIMARY_STAT_COUNT; index++)
                m_captain.m_primaryStats[index] =
                    captainStats[IDX(m_combatTowns[IDX(COMBAT_DEFENDER_SIDE)]->m_type)]
                                [index];
            m_captain.m_spellPoints = HERO_NORMAL_SPELL_POINTS(m_captain);
            m_captain.m_cursorType = m_combatTowns[IDX(COMBAT_DEFENDER_SIDE)]->m_type;
            m_captain.m_portrait = static_cast<HeroPortrait>(
                static_cast<i32>(m_combatTowns[IDX(COMBAT_DEFENDER_SIDE)]->m_type)
                + static_cast<i32>(HERO_CAPTAIN_PORTRAIT_FIRST)
            );
            strcpy(
                m_captain.m_name,
                localization::Tr("hero.captain")
            );
            for (index = 0; index < ARMY_GROUP_SLOT_COUNT; index++)
                m_captain.m_army.m_creatureTypes[index] = CREATURE_NONE;
            for (index = 0; index < HERO_ARTIFACT_SLOT_COUNT; index++)
                m_captain.m_artifacts[index] = ARTIFACT_NONE;
            m_captain.m_artifacts[0] = ARTIFACT_MAGIC_BOOK;
            m_combatTowns[IDX(COMBAT_DEFENDER_SIDE)]->GiveSpells(&m_captain);
            m_captain.m_isCaptain = 1;
        }
    } else {
        m_inCastleCombat = false;
        m_combatTowns[IDX(COMBAT_DEFENDER_SIDE)] = NULL;
    }
    m_combatTowns[IDX(COMBAT_ATTACKER_SIDE)] = NULL;
}

VA(0x004265e7, 0x258)
void combatManager::InitNonVisualVars(void) {
    m_gridSelectionDisabled = false;
    m_nonVisualCombat = false;
    CombatSide side;
    for (side = COMBAT_ATTACKER_SIDE; IDX(side) < COMBAT_SIDE_COUNT; side++) {
        m_spellPower[IDX(side)] = 0;
        if (m_heroes[IDX(side)] != NULL)
            m_spellPower[IDX(side)] = m_heroes[IDX(side)]->Stats(HERO_PRIMARY_SPELL_POWER);
        if (m_combatTowns[IDX(side)] != NULL
            && m_combatTowns[IDX(side)]->m_type == FACTION_NECROMANCER
            && HAS(m_combatTowns[IDX(side)]->m_buildings, IDX(TOWN_BUILDING_SHRINE)))
            m_spellPower[IDX(side)] += NECROMANCER_SHRINE_POWER_BONUS;
    }

    m_heroOverlayFrame[IDX(COMBAT_ATTACKER_SIDE)] = 0;
    m_heroOverlayFrame[IDX(COMBAT_DEFENDER_SIDE)] = DEFENDER_HERO_OVERLAY_INITIAL_FRAME;
    m_sideRetreated[IDX(COMBAT_ATTACKER_SIDE)] = false;
    m_sideRetreated[IDX(COMBAT_DEFENDER_SIDE)] = false;
    m_combatResult = COMBAT_RESULT_PENDING;
    m_heroDeathAnimationPlayed[0] = m_heroDeathAnimationPlayed[1] = 0;
    m_heroAlternateDeathAnimationPlayed[0] = m_heroAlternateDeathAnimationPlayed[1] = 0;
    m_heroDeathPending[0] = m_heroDeathPending[1] = 0;
    m_heroAlternateDeathPending[0] = m_heroAlternateDeathPending[1] = 0;
    m_eagleEyeSpell[IDX(COMBAT_ATTACKER_SIDE)] = SPELL_NONE;
    m_eagleEyeSpell[IDX(COMBAT_DEFENDER_SIDE)] = SPELL_NONE;
    giNextAction = ACTION_NONE;
    m_summonedCreatureType[0] = CREATURE_SUMMONED_NONE;
    m_summonedCreatureType[1] = CREATURE_SUMMONED_NONE;
    m_selectedHex = -1;
    m_limitCreatureHex = -1;
    m_previousCommand = COMBAT_INVALID_COMMAND;
    m_currentSide = COMBAT_DEFENDER_SIDE;
    m_currentArmySide = COMBAT_DEFENDER_SIDE;
    m_currentSpeed = COMBAT_INITIAL_COMMAND;
    gbRetreatWin = false;
    gbCombatSurrender = false;
    m_sideDefeated[IDX(COMBAT_ATTACKER_SIDE)] = false;
    m_sideDefeated[IDX(COMBAT_DEFENDER_SIDE)] = false;
    m_limitCreature = true;
    m_obstacleCount = 0;
    SetupAdjacencyArray();
    GenerateMap();
    LoadArmies();
}

VA(0x0042683f, 0x1bf)
void combatManager::SetupAdjacencyArray(void) {
    i32 toHex = 0;
    CombatHexDirection direction;
    i32 fromHex;
    i32 row;
    for (fromHex = 0; fromHex < COMBAT_HEX_COUNT; fromHex++) {
        row = fromHex / COMBAT_GRID_ROW_LENGTH;
        for (direction = COMBAT_DIRECTION_NORTHEAST;
             IDX(direction) < COMBAT_AI_ADJACENT_DIRECTION_COUNT;
             direction++) {
            if (fromHex % COMBAT_GRID_ROW_LENGTH == 0
                || fromHex % COMBAT_GRID_ROW_LENGTH == COMBAT_GRID_ROW_LENGTH - 1) {
                m_adjacency[fromHex][IDX(direction)] = -1;
            } else {
                switch (direction) {
                    case COMBAT_DIRECTION_NORTHEAST:
                        if (row & 1)
                            toHex = fromHex - COMBAT_GRID_ROW_LENGTH;
                        else
                            toHex = fromHex - (COMBAT_GRID_ROW_LENGTH - 1);
                        break;
                    case COMBAT_DIRECTION_SOUTHEAST:
                        if (row & 1)
                            toHex = fromHex + COMBAT_GRID_ROW_LENGTH;
                        else
                            toHex = fromHex + COMBAT_GRID_ROW_LENGTH + 1;
                        break;
                    case COMBAT_DIRECTION_SOUTHWEST:
                        if (row & 1)
                            toHex = fromHex + COMBAT_GRID_ROW_LENGTH - 1;
                        else
                            toHex = fromHex + COMBAT_GRID_ROW_LENGTH;
                        break;
                    case COMBAT_DIRECTION_NORTHWEST:
                        if (row & 1)
                            toHex = fromHex - COMBAT_GRID_ROW_LENGTH - 1;
                        else
                            toHex = fromHex - COMBAT_GRID_ROW_LENGTH;
                        break;
                    case COMBAT_DIRECTION_EAST:
                        toHex = fromHex + 1;
                        break;
                    case COMBAT_DIRECTION_WEST:
                        toHex = fromHex - 1;
                        break;
                }

                if (toHex % COMBAT_GRID_ROW_LENGTH == 0
                    || toHex % COMBAT_GRID_ROW_LENGTH == COMBAT_GRID_ROW_LENGTH - 1
                    || toHex < 0 || toHex >= COMBAT_HEX_COUNT)
                    m_adjacency[fromHex][IDX(direction)] = -1;
                else
                    m_adjacency[fromHex][IDX(direction)] = static_cast<i8>(toHex);
            }
        }
    }
}

VA(0x004269fe, 0x427)
i32 combatManager::Open(i32 openFlags) {
    LogStr("Op1");
    memcpy(m_savedPalette, gPalette->m_data, COMBAT_PALETTE_DATA_SIZE);
    gpMouseManager->m_forcePointerUpdate = true;
    i32 savedMouseHex = gConfig.showCombatMouseHex;
    gConfig.showCombatMouseHex = 0;
    m_previousCombatMessageExpiration = 0;
    m_combatMessageExpiration = 0;
    m_combatMessagePending = 0;
    m_combatWindowOpen = false;
    gpSoundManager->PlayAmbientMusic(-1);
    m_combatBuffer =
        new bitmap(BITMAP_TYPE_NONE, COMBAT_BACKGROUND_COPY_WIDTH, COMBAT_BACKGROUND_COPY_HEIGHT);
    m_backgroundBuffer = new bitmap(BITMAP_TYPE_NONE, COMBAT_SCREEN_WIDTH, COMBAT_AREA_HEIGHT);
    m_mouseGridBuffer = NULL;
    m_smallViewLastX[IDX(COMBAT_ATTACKER_SIDE)] = -1;
    m_smallViewLastX[IDX(COMBAT_DEFENDER_SIDE)] = -1;
    memset(m_gridState, IDX(GRID_SHADE_NONE), sizeof(m_gridState));
    LoadIcons();
    InitNonVisualVars();
    SetupAndLoadObstacles();
    memset(m_previousGridState, IDX(GRID_SHADE_NONE), sizeof(m_previousGridState));
    GetNextArmy(0);
    m_backgroundDrawn = false;

    SAMPLE2 preBattleSample = LoadPlaySample("PREBATTL.82M");
    gpWindowManager->FadeScreen(FADE_OUT, FADE_STEPS, NULL);
    giCycleType = m_colorCycleType;
    CycleColors(1);
    CycleColors(1);
    gCurLoadedSpellIcon = NULL;
    gCurLoadedSpellEffect = COMBAT_EFFECT_INVALID;
    gpMouseManager->m_forcePointerUpdate = false;
    gpMouseManager->SetPointer("cmbtmous.mse", COMBAT_POINTER_DEFAULT, MOUSE_AUTO_CURSOR_TYPE);
    bMouseWasVis = gpMouseManager->IsVis();
    gpMouseManager->ShowColorPointer();
    m_combatWindow = new heroWindow(0, 0, "cmbtwin.bin");
    if (m_combatWindow == NULL)
        MemError();
    gpWindowManager->AddWindow(m_combatWindow, -1, 1);
    m_combatWindowOpen = true;
    DrawFrame(1, 0, 0, 0, ARMY_COMBAT_FRAME_DELAY, 1, 1);
    glTimers[0] = KBTickCount();
    m_combatPalette = gpResourceManager->GetPalette("kb.pal");
    KBChangeMenu(hmnuCmbt);
    CombatMessage("", 1, 1, 0);
    gConfig.showCombatMouseHex = savedMouseHex;
    if (gpBufferPalette->m_data != m_combatPalette->m_data)
        memmove(m_combatPalette->m_data, gpBufferPalette->m_data, COMBAT_PALETTE_DATA_SIZE);
    gpWindowManager->FadeScreen(FADE_IN, FADE_STEPS, m_combatPalette);
    gbLimitedCombatUpdatePalette = true;
    WaitEndSample(&preBattleSample);

    gpSoundManager->SwitchAmbientMusic(SRandom(AMBIENT_MUSIC_FIRST, AMBIENT_MUSIC_LAST));
    glTimers[GLOBAL_COMBAT_CYCLE_TIMER_SLOT] = KBTickCount();
    ResetCycleTimers();
    gpInputManager->Flush();
    ResetMouse();
    m_messageMask = BASE_MANAGER_ACCEPT_WIDGET;
    m_priority = openFlags;
    m_active = true;
    strcpy(m_name, "combatManager");
    return 0;
}

VA(0x00426e25, 0x388)
void combatManager::Close(void) {
    gpSoundManager->SwitchAmbientMusic(-1);
    gbLimitedCombatUpdatePalette = false;
    if (!gbClosingApp) {
        memcpy(gPalette->m_data, m_savedPalette, COMBAT_PALETTE_DATA_SIZE);
        memcpy(gpBufferPalette->m_data, m_savedPalette, COMBAT_PALETTE_DATA_SIZE);
    }
    gpWindowManager->FadeScreen(FADE_OUT, FADE_STEPS, NULL);
    giCycleType = WINDOW_COLOR_CYCLE_DEFAULT;
    CycleColors(0);
    delete m_combatBuffer;
    delete m_backgroundBuffer;
    if (m_mouseGridBuffer != NULL)
        delete m_mouseGridBuffer;

    i32 total;
    CombatSide groupSide;
    i32 i;
    for (i = IDX(COMBAT_ATTACKER_SIDE); i < COMBAT_SIDE_COUNT; i++)
        UpdateArmyGroup(static_cast<CombatSide>(i));

    total = 0;
    groupSide = static_cast<CombatSide>(m_playerId[IDX(COMBAT_DEFENDER_SIDE)] == -1);

    for (i = 0; i < ARMY_GROUP_SLOT_COUNT; i++) {
        if (m_armyGroups[IDX(groupSide)]->m_creatureTypes[i] != CREATURE_NONE)
            total += m_armyGroups[IDX(groupSide)]->m_creatureCounts[i];
    }

    if (m_battlefieldCell->m_triggerType == (MAP_ACTION_TRIGGER(MAP_OBJECT_MONSTER))) {
        if (total > MONSTER_COUNT_SAVE_LIMIT)
            total = MONSTER_COUNT_SAVE_LIMIT;
        m_battlefieldCell->m_objectMetadata = total & IDX(MAP_MONSTER_COUNT_MASK);
    }

    if (m_battlefieldCell->m_triggerType == (MAP_ACTION_TRIGGER(MAP_OBJECT_MINE))
        && gpGame->m_mines[m_battlefieldCell->m_objectMetadata].guardianType != CREATURE_NONE)
        gpGame->m_mines[m_battlefieldCell->m_objectMetadata].guardianCount = static_cast<u8>(total);

    if (m_battlefieldCell->m_triggerType
        == (MAP_ACTION_TRIGGER(MAP_OBJECT_HERO_INTERACTION))) {
        hero* combatHero = gpGame->GetHero(m_battlefieldCell->m_objectMetadata);
        if (combatHero->m_locationType == (MAP_ACTION_TRIGGER(MAP_OBJECT_MINE))
            && gpGame->m_mines[combatHero->m_occupiedTown].guardianType != CREATURE_NONE)
            gpGame->m_mines[combatHero->m_occupiedTown].guardianCount = static_cast<u8>(total);
    }

    gpWindowManager->RemoveWindow(m_combatWindow);
    FreeArmies();
    FreeIcons();
    gpResourceManager->Dispose(m_combatPalette);
    delete m_combatWindow;
    if (!bMouseWasVis)
        gpMouseManager->HideColorPointer();
    m_active = false;
    m_combatWindowOpen = false;
}

#if H2_RETAIL_COMPILER
#define position pos
#endif
VA(0x004271ad, 0x2c1)
void combatManager::UpdateArmyGroup(H2_ENUM_PARAM(CombatSide, i32) side) {
    i32 index;
    i32 H2_UNUSED(position);
    for (index = 0; index < ARMY_GROUP_SLOT_COUNT; index++) {
        m_armyGroups[IDX(side)]->m_creatureTypes[index] = CREATURE_NONE;
        m_armyGroups[IDX(side)]->m_creatureCounts[index] = 0;
    }

    for (index = 0; index < m_armyCount[IDX(side)]; index++) {
        if (!HAS(m_armies[IDX(side)][index].m_monster.flags.all, MONSTER_FLAGS_AI_EXCLUDED)
            && m_armies[IDX(side)][index].m_quantity > 0
            && (m_playerId[IDX(side)] == -1
                || ((!IS_ELEMENTAL_CREATURE(m_armies[IDX(side)][index].m_monsterType))
                    || !HAS(
                        m_armies[IDX(side)][index].m_monster.flags.all,
                        MONSTER_FLAGS_SUMMONED
                    )))
            && !HAS(m_armies[IDX(side)][index].m_monster.flags.all, MONSTER_FLAGS_MIRROR_IMAGE)) {
            m_armyGroups[IDX(side)]->m_creatureTypes[m_armies[IDX(side)][index].m_armyGroupSlot] =
                m_armies[IDX(side)][index].m_monsterType;
            m_armyGroups[IDX(side)]->m_creatureCounts[m_armies[IDX(side)][index].m_armyGroupSlot] =
                static_cast<i16>(m_armies[IDX(side)][index].m_quantity);
        }
    }

    if (giSkeletonsCreated && CombatResultForSide(side) == m_combatResult)
        m_armyGroups[IDX(side)]->Add(CREATURE_SKELETON, giSkeletonsCreated, ARMY_GROUP_EMPTY_SLOT);
}
#if H2_RETAIL_COMPILER
#undef position
#endif

#if H2_RETAIL_COMPILER
#define gridX gridX4
#endif
VA(0x0042746e, 0x244)
void combatManager::GenerateMap(void) {
    i32 H2_UNUSED(gridX);
    i32 H2_UNUSED(randomOffset);
    i32 x;
    u32 y;
    i32 H2_UNUSED(coordinateY);

    m_catapultFrame[IDX(COMBAT_ATTACKER_SIDE)] = m_inCastleCombat == 1 ? 0 : -1;

    for (y = 0; static_cast<i32>(y) < COMBAT_GRID_ROW_COUNT; y++) {
        for (x = 0; x < COMBAT_GRID_ROW_LENGTH; x++) {
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_y =
                static_cast<i16>((y + 1) * COMBAT_HEX_VERTICAL_STEP + COMBAT_HEX_CENTER_Y_ORIGIN);
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_x = static_cast<i16>(
                (x - 1) * COMBAT_HEX_HORIZONTAL_STEP
                + ((y & 1) ? COMBAT_HEX_ROW_STAGGER : COMBAT_HEX_HORIZONTAL_STEP)
                + COMBAT_HEX_GRID_LEFT_ORIGIN
            );
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridLeft = static_cast<i16>(
                (x - 1) * COMBAT_HEX_HORIZONTAL_STEP + ((y & 1) ? 0 : COMBAT_HEX_ROW_STAGGER)
                + COMBAT_HEX_GRID_LEFT_ORIGIN
            );
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridTop =
                static_cast<i16>(y * COMBAT_HEX_VERTICAL_STEP + COMBAT_HEX_GRID_TOP_ORIGIN);
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridRight =
                m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridLeft + COMBAT_HEX_HORIZONTAL_STEP;
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridBodyBottom =
                m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridTop + COMBAT_HEX_VERTICAL_STEP;
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridBottom =
                m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridTop + COMBAT_MOUSE_HEX_HEIGHT;
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_occupantSide = COMBAT_SIDE_NONE;
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_occupantIndex = -1;
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_occupantFrame = ARMY_FACING_NONE;
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_obstacleIndex = -1;
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_blocked = 0;
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_deadOccupantCount = 0;
        }
    }
    randomOffset = SRandom(MAP_RANDOM_OFFSET_MINIMUM, MAP_RANDOM_OFFSET_MAXIMUM);
}
#if H2_RETAIL_COMPILER
#undef gridX
#endif

VA(0x004276b2, 0x1ee)
H2_CONST char* combatManager::GetBackgroundName(void) {
    BattlefieldBackgroundIndex backgroundIndex;
    m_colorCycleType = WINDOW_COLOR_CYCLE_COMBAT;
    m_battlefieldFringe = FRINGE_NONE;
    switch (m_terrainType) {
        case TERRAIN_WATER:
            backgroundIndex = BACKGROUND_WATER;
            m_battlefieldFringe = FRINGE_WATER;
            break;
        case TERRAIN_GRASS:
            if (MoreTreesNear()) {
                backgroundIndex = BACKGROUND_GRASS_TREES;
                m_battlefieldFringe = FRINGE_GRASS_TREES;
            } else {
                backgroundIndex = BACKGROUND_GRASS;
                m_battlefieldFringe = FRINGE_GRASS;
            }
            break;
        case TERRAIN_SNOW:
            m_colorCycleType = WINDOW_COLOR_CYCLE_COMBAT_ALTERNATE;
            if (MoreTreesNear()) {
                backgroundIndex = BACKGROUND_SNOW_TREES;
                m_battlefieldFringe = FRINGE_SNOW_TREES;
            } else {
                backgroundIndex = BACKGROUND_SNOW;
                m_battlefieldFringe = FRINGE_SNOW;
            }
            break;
        case TERRAIN_SWAMP:
            backgroundIndex = BACKGROUND_SWAMP;
            m_battlefieldFringe = FRINGE_SWAMP;
            break;
        case TERRAIN_LAVA:
            backgroundIndex = BACKGROUND_LAVA;
            m_battlefieldFringe = FRINGE_LAVA;
            break;
        case TERRAIN_DESERT:
            m_colorCycleType = WINDOW_COLOR_CYCLE_COMBAT_ALTERNATE;
            backgroundIndex = BACKGROUND_DESERT;
            m_battlefieldFringe = FRINGE_DESERT;
            break;
        case TERRAIN_DIRT:
            if (MoreTreesNear()) {
                backgroundIndex = BACKGROUND_DIRT_TREES;
                m_battlefieldFringe = FRINGE_DIRT_TREES;
            } else {
                backgroundIndex = BACKGROUND_DIRT;
                m_battlefieldFringe = FRINGE_DIRT;
            }
            break;
        case TERRAIN_WASTELAND:
            m_colorCycleType = WINDOW_COLOR_CYCLE_COMBAT_ALTERNATE;
            backgroundIndex = BACKGROUND_WASTELAND;
            m_battlefieldFringe = FRINGE_WASTELAND;
            break;
        case TERRAIN_BEACH:
            m_colorCycleType = WINDOW_COLOR_CYCLE_COMBAT_ALTERNATE;
            backgroundIndex = BACKGROUND_BEACH;
            m_battlefieldFringe = FRINGE_BEACH;
            break;
        default:
            backgroundIndex = BACKGROUND_WATER;
            break;
    }
    return cCombatBkgNames[IDX(backgroundIndex)];
}

#if H2_RETAIL_COMPILER
#define centerY centerY8
#define combatCell combatCell10
#define combatOriginX combatOriginX10
#define mountainCounter mountainCounter1
#define nearbyDirection nearbyDirection6
#define nearbyTypeTable nearbyTypeTable4
#define treeCount treeCount6
#endif
VA(0x004278a0, 0x1eb)
i32 combatManager::MoreTreesNear(void) {
    i32 treeCount;
    i32 x;
    i32 y;
    i32 mountainCounter;
    mapCell* combatCell;
    i32 radius;
    i32 combatOriginX;
    NearbyFeature nearbyTypeTable[NEARBY_RADIUS_COUNT][NORMAL_DIRECTION_COUNT];
    H2_ENUM_STORAGE(TilesetId, u8) nearbyTileset;
    i32 nearbyDirection;
    i32 centerY;

    memset(nearbyTypeTable, IDX(NEARBY_UNKNOWN), sizeof(nearbyTypeTable));
    combatOriginX = m_combatX;
    centerY = m_combatY;

    for (radius = 0; radius < NEARBY_RADIUS_COUNT; radius++) {
        for (nearbyDirection = 0; nearbyDirection < NORMAL_DIRECTION_COUNT; nearbyDirection++) {
            x = combatOriginX + normalDirTable[nearbyDirection].x * radius;
            y = centerY + normalDirTable[nearbyDirection].y * radius;
            if (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT) {
                combatCell = gpAdvManager->GetCell(x, y);
                nearbyTileset = combatCell->m_objectTileset;
                switch (nearbyTileset) {
                    case TILESET_MTNSNOW:
                    case TILESET_MTNSWMP:
                    case TILESET_MTNLAVA:
                    case TILESET_MTNDSRT:
                    case TILESET_MTNDIRT:
                    case TILESET_MTNMULT:
                    case TILESET_MTNCRCK:
                    case TILESET_MTNGRAS:
                        nearbyTypeTable[radius][nearbyDirection] = NEARBY_MOUNTAIN;
                        break;
                    case TILESET_TREJNGL:
                    case TILESET_TREEVIL:
                    case TILESET_TRESNOW:
                    case TILESET_TREFIR:
                    case TILESET_TREFALL:
                        nearbyTypeTable[radius][nearbyDirection] = NEARBY_TREE;
                        break;
                }
            }
        }
    }

    treeCount = 0;
    mountainCounter = 0;
    for (radius = 0; radius < NEARBY_RADIUS_COUNT; radius++) {
        for (nearbyDirection = 0; nearbyDirection < NORMAL_DIRECTION_COUNT; nearbyDirection++) {
            if (nearbyTypeTable[radius][nearbyDirection] == NEARBY_MOUNTAIN)
                mountainCounter++;
            if (nearbyTypeTable[radius][nearbyDirection] == NEARBY_TREE)
                treeCount++;
        }
    }
    if (treeCount > mountainCounter)
        return 1;
    return 0;
}
#if H2_RETAIL_COMPILER
#undef centerY
#undef combatCell
#undef combatOriginX
#undef mountainCounter
#undef nearbyDirection
#undef nearbyTypeTable
#undef treeCount
#endif

VA(0x00427a8b, 0x3ba)
void combatManager::LoadIcons(void) {
    i32 index;
    for (index = 0; index < COMBAT_FIXED_ICON_COUNT; index++)
        m_combatIcons[index] = NULL;

    for (index = 0; index < COMBAT_OBSTACLE_ICON_LOAD_COUNT; index++)
        m_obstacleIcons[index] = NULL;

    m_combatIcons[IDX(COMBAT_ICON_SPELLS)] = gpResourceManager->GetIcon("spells.icn");
    m_combatIcons[IDX(COMBAT_ICON_STATUS)] = gpResourceManager->GetIcon("textbar.icn");
    m_combatIcons[IDX(COMBAT_ICON_GRID)] = gpResourceManager->GetIcon("cmbtmisc.icn");
    m_combatIcons[IDX(COMBAT_ICON_SMALL_VIEW_BACKGROUND)] =
        gpResourceManager->GetIcon("viewarsm.icn");
    m_combatIcons[IDX(COMBAT_ICON_SMALL_VIEW_MODIFIER)] =
        gpResourceManager->GetIcon("minilkmr.icn");
    m_combatIcons[IDX(COMBAT_ICON_SMALL_VIEW_SPELL)] = gpResourceManager->GetIcon("spellinf.icn");

    if (m_inCastleCombat) {
        if (HAS(m_combatTowns[IDX(COMBAT_DEFENDER_SIDE)]->m_buildings, IDX(TOWN_BUILDING_MOAT))) {
            m_combatIcons[IDX(COMBAT_ICON_MOAT)] = gpResourceManager->GetIcon("moatpart.icn");
            m_combatIcons[IDX(COMBAT_ICON_DRAWBRIDGE)] = gpResourceManager->GetIcon("moatwhol.icn");
        }
        m_combatIcons[IDX(COMBAT_ICON_CATAPULT)] = gpResourceManager->GetIcon("catapult.icn");
        sprintf(
            gText,
            "castle%c.icn",
            cHeroTypeInitial[IDX(m_combatTowns[IDX(COMBAT_DEFENDER_SIDE)]->m_type)]
        );
        m_combatIcons[IDX(COMBAT_ICON_TOWER)] = gpResourceManager->GetIcon(gText);
        m_combatIcons[IDX(COMBAT_ICON_KEEP)] = gpResourceManager->GetIcon("keep.icn");
    }

    for (index = 0; index < COMBAT_SIDE_COUNT; index++) {
        m_heroIcons[index] = NULL;
        m_heroOverlayIcons[index] = NULL;
        m_heroAnimationState[index] = 0;
        m_heroAnimationFrame[index] = 0;
        m_heroSpriteIndex[index] = -1;

        if (m_heroes[index]) {
            if (m_heroes[index]->m_isCaptain) {
                sprintf(gText, "cmbtcap%c.icn", cHeroTypeInitial[IDX(m_heroes[index]->m_cursorType)]);
                m_heroIcons[index] = gpResourceManager->GetIcon(gText);
                m_heroSpriteIndex[index] =
                    IDX(m_heroes[index]->m_cursorType) + COMBAT_CAPTAIN_SPRITE_OFFSET;
            } else {
                sprintf(gText, "cmbthro%c.icn", cHeroTypeInitial[IDX(m_heroes[index]->m_cursorType)]);
                m_heroIcons[index] = gpResourceManager->GetIcon(gText);
                m_heroSpriteIndex[index] = IDX(m_heroes[index]->m_cursorType);
            }
        }

        if (m_heroIcons[index]) {
            DebugCheck();
            sprintf(
                gText,
                "herofl%02d.icn",
                m_playerId[index] == -1
                    ? COMBAT_NEUTRAL_HERO_COLOR
                    : gpGame->m_players[static_cast<i8>(m_playerId[index])].m_color
            );
            m_heroOverlayIcons[index] = gpResourceManager->GetIcon(gText);
        }
    }
}

VA(0x00427e45, 0xff)
void combatManager::FreeIcons(void) {
    i32 index;
    for (index = 0; index < COMBAT_FIXED_ICON_COUNT; index++) {
        if (m_combatIcons[index])
            gpResourceManager->Dispose(m_combatIcons[index]);
    }

    for (index = 0; index < COMBAT_OBSTACLE_ICON_LOAD_COUNT; index++) {
        if (m_obstacleIcons[index])
            gpResourceManager->Dispose(m_obstacleIcons[index]);
    }

    for (index = 0; index < COMBAT_SIDE_COUNT; index++) {
        if (m_heroIcons[index])
            gpResourceManager->Dispose(m_heroIcons[index]);
        if (m_heroOverlayIcons[index])
            gpResourceManager->Dispose(m_heroOverlayIcons[index]);
    }
}

VA(0x00427f44, 0x2fd)
void combatManager::LoadArmies(void) {
    i32 groupSlot;
    CombatSide side;
    i32 hex;

    m_armyCount[IDX(COMBAT_ATTACKER_SIDE)] = m_armyCount[IDX(COMBAT_DEFENDER_SIDE)] = 0;

    for (groupSlot = 0; groupSlot < COMBAT_ARMY_CAPACITY; groupSlot++) {
        for (side = COMBAT_ATTACKER_SIDE; IDX(side) < COMBAT_SIDE_COUNT; side++) {
            m_armies[IDX(side)][groupSlot].m_quantity = 0;
            m_armies[IDX(side)][groupSlot].m_monsterType = CREATURE_NONE;
        }
    }

    for (side = COMBAT_ATTACKER_SIDE; IDX(side) < COMBAT_SIDE_COUNT; side++) {
        for (groupSlot = 0; groupSlot < COMBAT_ARMY_CAPACITY; groupSlot++)
            m_armies[IDX(side)][groupSlot].InitClean();
    }

    for (groupSlot = 0; groupSlot < ARMY_GROUP_SLOT_COUNT; groupSlot++) {
        if (m_armyGroups[IDX(COMBAT_ATTACKER_SIDE)]->m_creatureTypes[groupSlot]
            != CREATURE_NONE) {
            if (m_heroes[IDX(COMBAT_ATTACKER_SIDE)]
                && HAS(m_heroes[IDX(COMBAT_ATTACKER_SIDE)]->m_eventFlags, HERO_EVENT_GROUPED_FORMATION))
                hex = COMBAT_GROUPED_HEX_STEP * groupSlot + COMBAT_ATTACKER_GROUPED_HEX;
            else
                hex = COMBAT_GRID_ROW_LENGTH * groupSlot * 2 + COMBAT_ATTACKER_SPREAD_HEX;

            m_armies[IDX(COMBAT_ATTACKER_SIDE)][m_armyCount[IDX(COMBAT_ATTACKER_SIDE)]].Init(
                m_armyGroups[IDX(COMBAT_ATTACKER_SIDE)]->m_creatureTypes[groupSlot],
                m_armyGroups[IDX(COMBAT_ATTACKER_SIDE)]->m_creatureCounts[groupSlot],
                COMBAT_ATTACKER_SIDE,
                m_armyCount[IDX(COMBAT_ATTACKER_SIDE)],
                hex,
                groupSlot
            );
            m_armies[IDX(COMBAT_ATTACKER_SIDE)][m_armyCount[IDX(COMBAT_ATTACKER_SIDE)]].LoadResources();
            m_armyCount[IDX(COMBAT_ATTACKER_SIDE)]++;
        }

        if (m_armyGroups[IDX(COMBAT_DEFENDER_SIDE)]->m_creatureTypes[groupSlot]
            != CREATURE_NONE) {
            if ((m_heroes[IDX(COMBAT_DEFENDER_SIDE)]
                 && HAS(m_heroes[IDX(COMBAT_DEFENDER_SIDE)]->m_eventFlags, HERO_EVENT_GROUPED_FORMATION))
                || (m_combatTowns[IDX(COMBAT_DEFENDER_SIDE)]
                    && m_combatTowns[IDX(COMBAT_DEFENDER_SIDE)]->m_formation))
                hex = COMBAT_GROUPED_HEX_STEP * groupSlot + COMBAT_DEFENDER_GROUPED_HEX;
            else
                hex = COMBAT_GRID_ROW_LENGTH * groupSlot * 2 + COMBAT_DEFENDER_SPREAD_HEX;

            m_armies[IDX(COMBAT_DEFENDER_SIDE)][m_armyCount[IDX(COMBAT_DEFENDER_SIDE)]].Init(
                m_armyGroups[IDX(COMBAT_DEFENDER_SIDE)]->m_creatureTypes[groupSlot],
                m_armyGroups[IDX(COMBAT_DEFENDER_SIDE)]->m_creatureCounts[groupSlot],
                COMBAT_DEFENDER_SIDE,
                m_armyCount[IDX(COMBAT_DEFENDER_SIDE)],
                hex,
                groupSlot
            );
            m_armies[IDX(COMBAT_DEFENDER_SIDE)][m_armyCount[IDX(COMBAT_DEFENDER_SIDE)]].LoadResources();
            m_armyCount[IDX(COMBAT_DEFENDER_SIDE)]++;
        }
    }
}

VA(0x00428241, 0xbd)
void combatManager::FreeArmies(void) {
    i32 index;
    gpSoundManager->StopAllSamples(1);

    for (index = 0; index < m_armyCount[IDX(COMBAT_ATTACKER_SIDE)]; index++)
        m_armies[IDX(COMBAT_ATTACKER_SIDE)][index].FreeResources();
    for (index = 0; index < m_armyCount[IDX(COMBAT_DEFENDER_SIDE)]; index++)
        m_armies[IDX(COMBAT_DEFENDER_SIDE)][index].FreeResources();

    if (gCurLoadedSpellIcon)
        gpResourceManager->Dispose(gCurLoadedSpellIcon);
    gCurLoadedSpellIcon = NULL;
    gCurLoadedSpellEffect = COMBAT_EFFECT_INVALID;
}

VA(0x004282fe, 0x191)
i32 combatManager::GetGridIndex(i32 x, i32 y) {
    i32 xResidual;
    i32 yOffset;
    i32 mapX;
    i32 lineIndex;
    i32 cellsColumn;
    i32 diagonalDist;
    i32 mapY;

    mapX = x;
    mapY = y;
    mapY -= COMBAT_HEX_GRID_TOP_ORIGIN;
    lineIndex = mapY / COMBAT_HEX_VERTICAL_STEP;
    mapX -= COMBAT_GRID_INDEX_X_ORIGIN;
    if (!(lineIndex & 1))
        mapX -= COMBAT_HEX_ROW_STAGGER;
    cellsColumn = mapX / COMBAT_HEX_HORIZONTAL_STEP;

    if (cellsColumn < 0)
        goto specialRegion;

    yOffset = mapY % COMBAT_HEX_VERTICAL_STEP;
    if (yOffset < COMBAT_GRID_DIAGONAL_HEIGHT) {
        xResidual = mapX % COMBAT_HEX_HORIZONTAL_STEP;
        diagonalDist =
            abs(xResidual - COMBAT_HEX_ROW_STAGGER) / COMBAT_GRID_DIAGONAL_SLOPE_DIVISOR;
        if (yOffset < diagonalDist) {
            lineIndex--;
            if (xResidual < COMBAT_HEX_ROW_STAGGER) {
                if (!(lineIndex & 1))
                    cellsColumn--;
            } else if (lineIndex & 1) {
                cellsColumn++;
            }
        }
    }

    if (cellsColumn <= COMBAT_GRID_REVERSE_COLUMN_END || cellsColumn >= COMBAT_GRID_COLUMN_END
        || lineIndex >= COMBAT_GRID_ROW_COUNT || lineIndex < 0) {
    specialRegion:
        if (x >= 0 && x <= COMBAT_GRID_LEFT_SPECIAL_X_MAX && y >= COMBAT_GRID_LEFT_SPECIAL_Y_MIN
            && y <= COMBAT_GRID_LEFT_SPECIAL_Y_MAX)
            return COMBAT_GRID_LEFT_SPECIAL_HEX;
        if (x >= COMBAT_GRID_RIGHT_SPECIAL_X_MIN && x <= COMBAT_MAX_EXTENT_X
            && y >= COMBAT_GRID_RIGHT_UPPER_Y_MIN && y <= COMBAT_GRID_RIGHT_UPPER_Y_MAX)
            return COMBAT_GRID_RIGHT_UPPER_HEX;
        if (x >= COMBAT_GRID_RIGHT_SPECIAL_X_MIN && x <= COMBAT_MAX_EXTENT_X
            && y >= COMBAT_GRID_RIGHT_LOWER_Y_MIN && y <= COMBAT_GRID_RIGHT_LOWER_Y_MAX
            && m_inCastleCombat)
            return COMBAT_BALLISTA_HEX;
        return -1;
    }
    return lineIndex * COMBAT_GRID_ROW_LENGTH + cellsColumn;
}

VA(0x0042848f, 0x1ba)
void combatManager::CheckApplyGoodMorale(H2_ENUM_PARAM(CombatSide, i32) side, i32 index) {
    if (side < COMBAT_ATTACKER_SIDE || index < 0)
        return;
    if (bInHighMoraleBonus) {
        bInHighMoraleBonus = false;
        return;
    }
    bInHighMoraleBonus = false;

    army* activeArmy = &m_armies[IDX(side)][index];
    if (HAS(activeArmy->m_monster.flags.all, MONSTER_FLAGS_NO_MORALE))
        return;
    if (activeArmy->m_quantity == 0)
        return;
    if (activeArmy->m_morale <= 0
        || SRandom(MORALE_ROLL_MIN, GOOD_MORALE_ROLL_MAX) > activeArmy->m_morale)
        return;

    bInHighMoraleBonus = true;
    SAMPLE2 moraleSample = NULL;
    if (!gbNoShowCombat) {
        sprintf(gText, "goodmrle.82M");
        moraleSample = LoadPlaySample(gText);
        if (activeArmy->m_quantity <= 1)
            sprintf(
                gText,
                localization::Tr("combat.morale.high.attack_again"),
                gArmyNames[IDX(activeArmy->m_monsterType)]
            );
        else
            sprintf(
                gText,
                localization::Tr("combat.morale.high.attack_again"),
                gArmyNamesPlural[IDX(activeArmy->m_monsterType)]
            );
        CombatMessage(gText, 1, 1, 0);
    }

    activeArmy->SpellEffect(COMBAT_EFFECT_GOOD_MORALE, MORALE_EFFECT_DURATION, 0);
    if HAS (activeArmy->m_monster.flags.abilityFlags, MONSTER_ABILITY_FLAG_BAD_MORALE)
        H2_ENUM_CLEAR_FLAG(
            activeArmy->m_monster.flags.abilityFlags, MONSTER_ABILITY_FLAG_BAD_MORALE
        );
    activeArmy->m_monster.flags.abilityFlags |= MONSTER_ABILITY_FLAG_HIGH_MORALE;

    if (!gbNoShowCombat)
        WaitEndSample(&moraleSample);
}

VA(0x00428649, 0x176)
i32 combatManager::CheckApplyBadMorale(
    H2_ENUM_PARAM(CombatSide, i32) side, i32 index
) {
    if (side < COMBAT_ATTACKER_SIDE || index < 0)
        return 0;

    army* activeArmy = &m_armies[IDX(side)][index];
    if (HAS(activeArmy->m_monster.flags.all, MONSTER_FLAGS_NO_MORALE))
        return 0;
    if (activeArmy->m_morale >= 0
        || SRandom(MORALE_ROLL_MIN, BAD_MORALE_ROLL_MAX) > -activeArmy->m_morale)
        return 0;
    if (!m_networkArmyPresent[IDX(side)]
        && SRandom(MORALE_ROLL_MIN, BAD_MORALE_NETWORK_ROLL_MAX)
               == BAD_MORALE_NETWORK_SKIP_ROLL)
        return 0;

    SAMPLE2 moraleSample = NULL;
    if (!gbNoShowCombat) {
        moraleSample = LoadPlaySample("BADMRLE.82M");
        if (activeArmy->m_quantity <= 1)
            sprintf(
                gText,
                localization::Tr("combat.morale.low.freeze"),
                gArmyNames[IDX(activeArmy->m_monsterType)]
            );
        else
            sprintf(
                gText,
                localization::Tr("combat.morale.low.freeze"),
                gArmyNamesPlural[IDX(activeArmy->m_monsterType)]
            );
        CombatMessage(gText, 1, 1, 0);
    }

    activeArmy->SpellEffect(COMBAT_EFFECT_BAD_MORALE, MORALE_EFFECT_DURATION, 1);
    activeArmy->m_monster.flags.abilityFlags |= MONSTER_ABILITY_FLAG_BAD_MORALE;
    if (!gbNoShowCombat)
        WaitEndSample(&moraleSample);
    return 1;
}

VA(0x004287bf, 0x345)
i32 combatManager::GetNextArmy(i32 checkMorale) {
    b32 skipEnt;
    i32 speedIter;
    b32 hasPending;
    i32 sideLoop;
    i32 stackCounter;
    army* curArmy;
    CombatSide stackSide;
    i32 H2_UNUSED(i);

restart:
    hasPending = false;
    stackSide = m_currentArmySide;
    m_currentSpeed = COMBAT_MAX_SPEED;
    for (speedIter = 0; speedIter < COMBAT_SPEED_LEVEL_COUNT; speedIter++) {
        for (sideLoop = 0; sideLoop < COMBAT_SIDE_COUNT; sideLoop++) {
            stackSide ^= 1;
            for (stackCounter = 0; stackCounter < m_armyCount[IDX(stackSide)]; stackCounter++) {
                skipEnt = false;
                curArmy = stackCounter + m_armies[IDX(stackSide)];
                if (HAS(curArmy->m_monster.flags.abilityFlags,
                        MONSTER_ABILITY_FLAG_AI_EXCLUDED | MONSTER_ABILITY_FLAG_BAD_MORALE)
                    || IDX(curArmy->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_PARALYZE)])
                    || curArmy->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_PETRIFIED)]
                    || curArmy->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_BLIND)]
                    || (curArmy->m_monster.speed != m_currentSpeed
                        && !(
                            curArmy->m_monster.flags.abilityFlags
                            & MONSTER_ABILITY_FLAG_HIGH_MORALE
                        )))
                    skipEnt = true;

                if (!skipEnt && speedIter == 0
                    && !(
                        curArmy->m_monster.flags.abilityFlags & MONSTER_ABILITY_FLAG_HIGH_MORALE
                    ))
                    skipEnt = true;

                if HAS (curArmy->m_monster.flags.abilityFlags,
                        MONSTER_ABILITY_FLAG_DEFERRED_TURN) {
                    skipEnt = true;
                    hasPending = true;
                }

                if (!skipEnt && checkMorale && CheckApplyBadMorale(stackSide, stackCounter))
                    skipEnt = true;
                if (!skipEnt)
                    break;
            }

            if (stackCounter != m_armyCount[IDX(stackSide)]) {
                m_currentArmySide = stackSide;
                m_currentArmyIndex = stackCounter;
                if ((stackCounter + m_armies[IDX(stackSide)])
                        ->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_HYPNOTIZE)])
                    m_currentSide = OppositeCombatSide(stackSide);
                else
                    m_currentSide = stackSide;
                GetControl();
                return 1;
            }
        }

        if (speedIter) {
            m_currentSpeed--;
            if (m_currentSpeed == 0)
                m_currentSpeed = COMBAT_SPEED_LEVEL_COUNT;
        }
    }

    if (hasPending) {
        checkMorale = 0;
        for (sideLoop = 0; sideLoop < COMBAT_SIDE_COUNT; sideLoop++) {
            for (stackCounter = 0; stackCounter < m_armyCount[sideLoop]; stackCounter++) {
                (m_armies[sideLoop] + stackCounter)->m_monster.flags.abilityFlags &=
                    ~MONSTER_ABILITY_FLAG_DEFERRED_TURN;
            }
        }
        goto restart;
    }

    CheckCastleAttack();
    m_currentSide = OppositeCombatSide(m_currentSide);
    CheckCastleAttack();
    m_currentSide = OppositeCombatSide(m_currentSide);
    return 0;
}

VA(0x00428b04, 0xb4)
i32 combatManager::IsWinner(H2_ENUM_PARAM(CombatSide, i32) side) {
    b32 result;
    i32 index;

    if (m_sideDefeated[IDX(COMBAT_DEFENDER_SIDE) - IDX(side)])
        return 1;
    if (m_sideRetreated[IDX(COMBAT_DEFENDER_SIDE) - IDX(side)])
        return 1;

    side ^= 1;
    result = true;
    for (index = 0; index < m_armyCount[IDX(side)]; index++) {
        if (!(m_armies[IDX(side)][index].m_monster.flags.abilityFlags
              & MONSTER_ABILITY_FLAG_AI_EXCLUDED))
            result = false;
    }
    return result;
}

#if H2_RETAIL_COMPILER
#define boulder boulder3
#define bounceY bounceY7
#define damageLevel damageLevel15
#define firstRoll firstRoll7
#define gateIndex gateIndex11
#define impactSound impactSound19
#define impactX impactX10
#define impactY impactY6
#define index index26
#define keepIndex keepIndex13
#define limits limits9
#define loadedSample loadedSample18
#define midpointX midpointX6
#define missShot missShot19
#define previousX previousX7
#define previousY previousY4
#define projectileX projectileX11
#define projectileY projectileY4
#define random random2
#define spriteFrame spriteFrame16
#define startDirection startDirection3
#define startX startX8
#define startY startY9
#define targetHex targetHex4
#define towerCount towerCount8
#define towerIndex towerIndex6
#define wallCount wallCount14
#define xStep xStep0
#endif
VA(0x00428bb8, 0xdd8)
void combatManager::CatAttack(H2_ENUM_PARAM(CombatSide, i32) side) {
    if (!m_inCastleCombat)
        return;

    LogStr("CA1");
    if (m_smallViewSide[IDX(COMBAT_ATTACKER_SIDE)] != COMBAT_SIDE_NONE
        || m_smallViewSide[IDX(COMBAT_DEFENDER_SIDE)] != COMBAT_SIDE_NONE) {
        m_smallViewSide[IDX(COMBAT_ATTACKER_SIDE)] = m_smallViewSide[IDX(COMBAT_DEFENDER_SIDE)] = COMBAT_SIDE_NONE;
        DrawSmallView(IDX(COMBAT_ATTACKER_SIDE), 1);
        DrawSmallView(IDX(COMBAT_DEFENDER_SIDE), 1);
    }

    i32 random = SRandom(COMBAT_CATAPULT_TARGET_ROLL_MIN, COMBAT_CATAPULT_TARGET_ROLL_MAX);
    i32 wallCount = 0;
    i32 towerCount = 0;
    i32 wallIndex = -1;
    CombatCastleWallSlot towerIndex = COMBAT_WALL_SLOT_NONE;
    i32 gateIndex = -1;
    i32 keepIndex = -1;
    CombatCastleHex targetHex = COMBAT_CASTLE_HEX_NONE;
    b32 missShot = false;
    CombatCatapultDamage damageLevel = CATAPULT_DAMAGE_NORMAL;
    i32 firstRoll;
    i32 advancedRoll;
    i32 index;

    if (m_wallStates[IDX(COMBAT_WALL_SLOT_SECTION_FIRST)] != COMBAT_WALL_STATE_DESTROYED
        && m_wallStates[IDX(COMBAT_WALL_SLOT_SECTION_FIRST)]
               != COMBAT_WALL_STATE_SECTION_DESTROYED)
        wallCount++;
    if (m_wallStates[IDX(COMBAT_WALL_SLOT_SECTION_SECOND)] != COMBAT_WALL_STATE_DESTROYED
        && m_wallStates[IDX(COMBAT_WALL_SLOT_SECTION_SECOND)]
               != COMBAT_WALL_STATE_SECTION_DESTROYED)
        wallCount++;
    if (m_wallStates[IDX(COMBAT_WALL_SLOT_SECTION_THIRD)] != COMBAT_WALL_STATE_DESTROYED
        && m_wallStates[IDX(COMBAT_WALL_SLOT_SECTION_THIRD)]
               != COMBAT_WALL_STATE_SECTION_DESTROYED)
        wallCount++;
    if (m_wallStates[IDX(COMBAT_WALL_SLOT_SECTION_FOURTH)] != COMBAT_WALL_STATE_DESTROYED
        && m_wallStates[IDX(COMBAT_WALL_SLOT_SECTION_FOURTH)]
               != COMBAT_WALL_STATE_SECTION_DESTROYED)
        wallCount++;
    if (m_wallStates[IDX(COMBAT_WALL_SLOT_TOP_TOWER)] == COMBAT_WALL_STATE_TOWER_STANDING)
        towerCount++;
    if (m_wallStates[IDX(COMBAT_WALL_SLOT_BOTTOM_TOWER)] == COMBAT_WALL_STATE_TOWER_STANDING)
        towerCount++;

    if (wallCount != 0) {
        random = random % wallCount + 1;
        wallIndex = -1;
        while (random != 0) {
            wallIndex++;
            if (m_wallStates[wallIndex + IDX(COMBAT_WALL_SLOT_SECTION_FIRST)]
                    != COMBAT_WALL_STATE_DESTROYED
                && m_wallStates[wallIndex + IDX(COMBAT_WALL_SLOT_SECTION_FIRST)]
                       != COMBAT_WALL_STATE_SECTION_DESTROYED)
                random--;
        }
    } else if (towerCount != 0) {
        random %= towerCount;
        if (random == 1
            || m_wallStates[IDX(COMBAT_WALL_SLOT_TOP_TOWER)] == COMBAT_WALL_STATE_DESTROYED)
            towerIndex = COMBAT_WALL_SLOT_BOTTOM_TOWER;
        else
            towerIndex = COMBAT_WALL_SLOT_TOP_TOWER;
    } else if (m_drawbridgeState != COMBAT_CASTLE_GATE_HIDDEN) {
        gateIndex = 1;
    } else {
        if (m_wallStates[IDX(COMBAT_WALL_SLOT_KEEP)] == COMBAT_WALL_STATE_KEEP_STANDING)
            keepIndex = 0;
    }

    if (towerIndex == COMBAT_WALL_SLOT_NONE && wallIndex == -1 && gateIndex == -1
        && keepIndex == -1)
        return;

    sample* loadedSample = NULL;
    sprintf(gText, "catsnd%02d.82M", COMBAT_CATAPULT_IMPACT_SOUND);
    loadedSample = gpResourceManager->GetSample(gText);
    SAMPLE2 catapultSound = NULL;
    SAMPLE2 impactSound = NULL;
    icon* boulder = gpResourceManager->GetIcon("boulder.icn");
    sprintf(gText, "catsnd%02d.82M", COMBAT_CATAPULT_LAUNCH_SOUND);
    catapultSound = LoadPlaySample(gText);

    i32 impactX = -1;
    i32 impactY = -1;
    if (wallIndex != -1) {
        impactX = wallPos[wallIndex][IDX(COORDINATE_AXIS_X)];
        impactY = wallPos[wallIndex][IDX(COORDINATE_AXIS_Y)];
        targetHex = CombatCastleHex(iWallToHexCell[wallIndex]);
    }
    if (towerIndex != COMBAT_WALL_SLOT_NONE) {
        impactX = towerPos[IDX(towerIndex)][IDX(COORDINATE_AXIS_X)];
        impactY = towerPos[IDX(towerIndex)][IDX(COORDINATE_AXIS_Y)];
        targetHex = CombatCastleHex(iTowerToHexCell[IDX(towerIndex)]);
    }
    if (gateIndex != -1) {
        impactX = doorPos[0][IDX(COORDINATE_AXIS_X)];
        impactY = doorPos[0][IDX(COORDINATE_AXIS_Y)];
        targetHex = COMBAT_CASTLE_HEX_GATE;
    }
    if (keepIndex != -1) {
        impactX = COMBAT_CATAPULT_KEEP_IMPACT_X;
        impactY = COMBAT_CATAPULT_KEEP_IMPACT_Y;
        targetHex = COMBAT_CASTLE_HEX_KEEP;
    }

    i32 startX;
    float bounceY;
    i32 frame;

    if (m_heroes[IDX(COMBAT_ATTACKER_SIDE)]->m_secondarySkills[IDX(HERO_SKILL_BALLISTICS)]
        == HERO_SKILL_LEVEL_NONE) {
        firstRoll =
            SRandom(COMBAT_CATAPULT_BALLISTICS_ROLL_MIN, COMBAT_CATAPULT_BALLISTICS_ROLL_MAX);
        if (!gbHumanPlayer[IDX(m_heroes[IDX(COMBAT_ATTACKER_SIDE)]->m_owner)])
            firstRoll -= COMBAT_CATAPULT_AI_ROLL_PENALTY;
        if (firstRoll < COMBAT_CATAPULT_NO_SKILL_DOUBLE_THRESHOLD)
            damageLevel = CATAPULT_DAMAGE_DOUBLE;
        else if (firstRoll > COMBAT_CATAPULT_NO_SKILL_MISS_THRESHOLD) {
            missShot = true;
            damageLevel = CATAPULT_DAMAGE_NONE;
        }
    } else if (m_heroes[IDX(COMBAT_ATTACKER_SIDE)]->m_secondarySkills[IDX(HERO_SKILL_BALLISTICS)]
               <= HERO_SKILL_LEVEL_ADVANCED) {
        advancedRoll =
            SRandom(COMBAT_CATAPULT_BALLISTICS_ROLL_MIN, COMBAT_CATAPULT_BALLISTICS_ROLL_MAX);
        if (!gbHumanPlayer[IDX(m_heroes[IDX(COMBAT_ATTACKER_SIDE)]->m_owner)])
            advancedRoll -= COMBAT_CATAPULT_AI_ROLL_PENALTY;
        if (advancedRoll < COMBAT_CATAPULT_ADVANCED_DOUBLE_THRESHOLD)
            damageLevel = CATAPULT_DAMAGE_DOUBLE;
    } else if (m_heroes[IDX(COMBAT_ATTACKER_SIDE)]->m_secondarySkills[IDX(HERO_SKILL_BALLISTICS)]
               == HERO_SKILL_LEVEL_EXPERT) {
        damageLevel = CATAPULT_DAMAGE_DOUBLE;
    }

    if (missShot) {
        u8 validMissHexes[COMBAT_CATAPULT_MISS_HEX_COUNT] =
            {32, 46, 10, 23, 35, 48, 60, 74, 87, 101, 114};
        i32 startDirection =
            SRandom(COMBAT_CATAPULT_DIRECTION_ROLL_MIN, COMBAT_CATAPULT_DIRECTION_ROLL_MAX);
        i32 adjacentHex;
        for (frame = 0; frame < COMBAT_CATAPULT_DIRECTION_COUNT; frame++) {
            adjacentHex = GetAdjacentCellIndexNoArmy(
                IDX(targetHex),
                static_cast<CombatHexDirection>(
                    (startDirection + frame) % COMBAT_CATAPULT_DIRECTION_COUNT
                )
            );
            if (adjacentHex != -1 && m_hexCells[adjacentHex].m_occupantSide == COMBAT_SIDE_NONE) {
                for (index = 0; index < COMBAT_CATAPULT_MISS_HEX_COUNT; index++) {
                    if (adjacentHex == validMissHexes[index]) {
                        impactX = m_hexCells[adjacentHex].m_x;
                        impactY = m_hexCells[adjacentHex].m_y - COMBAT_CATAPULT_MISS_Y_OFFSET;
                        goto foundMissHex;
                    }
                }
            }
        }
        missShot = false;
        damageLevel = CATAPULT_DAMAGE_NORMAL;
    foundMissHex:
        frame++;
    }

    giMinExtentX = 0;
    giMaxExtentX = COMBAT_CATAPULT_INITIAL_MAX_X;
    giMinExtentY = COMBAT_CATAPULT_INITIAL_MIN_Y;
    giMaxExtentY = COMBAT_CATAPULT_INITIAL_MAX_Y;
    m_catapultFrame[IDX(side)] = 0;
    while (m_catapultFrame[IDX(side)] < COMBAT_CATAPULT_LOAD_FRAME_COUNT) {
        DrawFrame(1, 0, 1, 0, COMBAT_CATAPULT_ANIMATION_DELAY, 1, 1);
        m_catapultFrame[IDX(side)]++;
    }

    startX = COMBAT_CATAPULT_START_X;
    i32 startY = COMBAT_CATAPULT_START_Y;
    i32 spriteFrame = 0;
    float projectileX = static_cast<float>(startX);
    float projectileY = static_cast<float>(startY);
    float midpointX = static_cast<float>(
        (startX + impactX) / COMBAT_CATAPULT_MIDPOINT_DIVISOR
    );
    bounceY = static_cast<float>(
        (impactY - (impactX - startX) * COMBAT_CATAPULT_DISTANCE_ARC_SCALE)
        - impactY * COMBAT_CATAPULT_TARGET_ARC_SCALE
    );
    float xStep =
        static_cast<float>((midpointX - startX) / COMBAT_CATAPULT_HORIZONTAL_STEP_DIVISOR);
    float yStep = (bounceY - startY) / COMBAT_CATAPULT_VERTICAL_STEP_DIVISOR;
    i32 previousX = -1;
    i32 previousY = -1;
    SLimitData limits;
    for (frame = 0; frame < COMBAT_CATAPULT_PROJECTILE_FRAME_COUNT; frame++) {
        if (frame == COMBAT_CATAPULT_DESCENT_FRAME)
            yStep = (bounceY - impactY) / COMBAT_CATAPULT_VERTICAL_STEP_DIVISOR;
        if (frame != 0) {
            giMinExtentX = previousX - COMBAT_CATAPULT_PROJECTILE_EXTENT_RADIUS;
            giMaxExtentX =
                static_cast<i32l>(
                    projectileX
                    + IDX(COMBAT_CATAPULT_PROJECTILE_EXTENT_RADIUS)

                );
            giMinExtentY =
                static_cast<i32l>(
                    projectileY - IDX(COMBAT_CATAPULT_PROJECTILE_EXTENT_RADIUS)
                );
            giMaxExtentY =
                static_cast<i32l>(
                    projectileY + IDX(COMBAT_CATAPULT_PROJECTILE_EXTENT_RADIUS)
                );
            if (previousY < projectileY)
                giMinExtentY = previousY - COMBAT_CATAPULT_PROJECTILE_EXTENT_RADIUS;
            else
                giMaxExtentY = previousY + COMBAT_CATAPULT_PROJECTILE_EXTENT_RADIUS;
            if (frame <= COMBAT_CATAPULT_EARLY_EXTENT_LAST_FRAME) {
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
        boulder->CombatClipDrawToBuffer(
            static_cast<i32l>(projectileX),
            static_cast<i32l>(projectileY),
            spriteFrame,
            &limits,
            ICON_DRAW_NORMAL
        );
        UPDATE_INCLUSIVE_REGION(giMinExtentX, giMinExtentY, giMaxExtentX, giMaxExtentY);
        previousX = static_cast<i32l>(projectileX);
        previousY = static_cast<i32l>(projectileY);
        projectileX += xStep;
        projectileY = (COMBAT_CATAPULT_DESCENT_FRAME - frame) * yStep + projectileY;
        spriteFrame++;
        spriteFrame %= COMBAT_CATAPULT_BOULDER_FRAME_COUNT;
        if (frame < COMBAT_CATAPULT_ADVANCE_FRAME_COUNT)
            m_catapultFrame[IDX(side)]++;
    }

    sprintf(gText, "catsnd%02d.82M", COMBAT_CATAPULT_IMPACT_SOUND);
    impactSound = LoadPlaySample(gText);
    m_unknownF34B = 0;
    m_unknownF34F = 0;

    giMinExtentX = impactX - COMBAT_CATAPULT_IMPACT_EXTENT_X;
    giMaxExtentX = impactX + COMBAT_CATAPULT_IMPACT_EXTENT_X;
    giMinExtentY = impactY - COMBAT_CATAPULT_IMPACT_EXTENT_TOP;
    giMaxExtentY = impactY + COMBAT_CATAPULT_IMPACT_EXTENT_BOTTOM;
    if (gateIndex != -1)
        giMinExtentX -= COMBAT_CATAPULT_IMPACT_EXTENT_X;
    if (giMinExtentX < 0)
        giMinExtentX = 0;
    if (giMaxExtentX > COMBAT_MAX_EXTENT_X)
        giMaxExtentX = COMBAT_MAX_EXTENT_X;
    if (giMinExtentY < 0)
        giMinExtentY = 0;
    if (giMaxExtentY > COMBAT_MAX_EXTENT_Y)
        giMaxExtentY = COMBAT_MAX_EXTENT_Y;

    icon* cloud =
        gpResourceManager->GetIcon(const_cast<char*>(missShot ? "smalclod.icn" : "lichclod.icn"));

    for (frame = 0; frame < COMBAT_CATAPULT_CLOUD_FRAME_COUNT; frame++) {
        if (frame >= COMBAT_CATAPULT_CLOUD_VISIBLE_FRAME_COUNT
            && (gateIndex != -1 || missShot != 0))
            continue;
        glTimers[COMBAT_CATAPULT_TIMER_SLOT] = static_cast<i32l>(
            KBTickCount()
            + IDX(COMBAT_CATAPULT_ANIMATION_DELAY) * gfCombatSpeedMod[gConfig.combatSpeed]
        );
        DrawFrame(0, 0, 1, 0, 0, 1, 0);
        if (gateIndex != -1 && frame >= COMBAT_CATAPULT_GATE_CLOUD_FIRST_FRAME && !missShot) {
            IconToBitmap(
                cloud,
                gpWindowManager->m_screen,
                impactX - COMBAT_CATAPULT_GATE_CLOUD_X_OFFSET,
                impactY + COMBAT_CATAPULT_GATE_CLOUD_Y_OFFSET,
                frame - COMBAT_CATAPULT_GATE_CLOUD_FIRST_FRAME,
                ICON_DRAW_CLIP,
                0,
                0,
                COMBAT_SCREEN_WIDTH,
                COMBAT_AREA_HEIGHT,
                0
            );
        }
        if (frame < COMBAT_CATAPULT_CLOUD_VISIBLE_FRAME_COUNT) {
            IconToBitmap(
                cloud,
                gpWindowManager->m_screen,
                impactX,
                impactY + (!missShot ? COMBAT_CATAPULT_CLOUD_Y_OFFSET : 0),
                frame,
                ICON_DRAW_CLIP,
                0,
                0,
                COMBAT_SCREEN_WIDTH,
                COMBAT_AREA_HEIGHT,
                0
            );
        }
        UPDATE_INCLUSIVE_REGION(giMinExtentX, giMinExtentY, giMaxExtentX, giMaxExtentY);
        DelayTil(&glTimers[COMBAT_CATAPULT_TIMER_SLOT]);

        if (((frame == COMBAT_CATAPULT_WALL_IMPACT_FRAME && gateIndex == -1)
             || (frame == COMBAT_CATAPULT_GATE_IMPACT_FRAME && gateIndex != -1))
            && !missShot) {
            if (wallIndex != -1) {
                if (m_wallStates[wallIndex + IDX(COMBAT_WALL_SLOT_SECTION_FIRST)]
                        >= COMBAT_WALL_STATE_SECTION_DAMAGE_FIRST
                    && m_wallStates[wallIndex + IDX(COMBAT_WALL_SLOT_SECTION_FIRST)]
                           <= COMBAT_WALL_STATE_SECTION_DAMAGE_LAST) {
                    m_wallStates[wallIndex + IDX(COMBAT_WALL_SLOT_SECTION_FIRST)] +=
                        IDX(damageLevel);
                    if (m_wallStates[wallIndex + IDX(COMBAT_WALL_SLOT_SECTION_FIRST)]
                        > COMBAT_WALL_STATE_SECTION_DAMAGE_LAST)
                        m_wallStates[wallIndex + IDX(COMBAT_WALL_SLOT_SECTION_FIRST)] =
                            COMBAT_WALL_STATE_SECTION_DESTROYED;
                } else {
                    if (damageLevel > CATAPULT_DAMAGE_NORMAL)
                        m_wallStates[wallIndex + IDX(COMBAT_WALL_SLOT_SECTION_FIRST)] =
                            COMBAT_WALL_STATE_DESTROYED;
                    else
                        m_wallStates[wallIndex + IDX(COMBAT_WALL_SLOT_SECTION_FIRST)]++;
                }
                if (m_wallStates[wallIndex + IDX(COMBAT_WALL_SLOT_SECTION_FIRST)]
                        == COMBAT_WALL_STATE_DESTROYED
                    || m_wallStates[wallIndex + IDX(COMBAT_WALL_SLOT_SECTION_FIRST)]
                           == COMBAT_WALL_STATE_SECTION_DESTROYED)
                    m_hexCells[iWallToHexCell[wallIndex]].m_blocked = 0;
            } else if (towerIndex != COMBAT_WALL_SLOT_NONE) {
                m_wallStates[IDX(towerIndex)] = COMBAT_WALL_STATE_DESTROYED;
            } else if (gateIndex != -1) {
                m_drawbridgeState = COMBAT_CASTLE_GATE_HIDDEN;
            } else if (keepIndex != -1) {
                m_wallStates[IDX(COMBAT_WALL_SLOT_KEEP)] = COMBAT_WALL_STATE_KEEP_DESTROYED;
            }
        }
    }

    gpResourceManager->Dispose(cloud);
    m_catapultFrame[IDX(side)] = 0;
    DrawFrame(1, 0, 0, 0, COMBAT_CATAPULT_ANIMATION_DELAY, 1, 1);
    gpResourceManager->Dispose(boulder);
    WaitEndSample(&impactSound);
    WaitEndSample(&catapultSound);
    if (loadedSample)
        gpResourceManager->Dispose(loadedSample);
    LogStr("CA2");
}
#if H2_RETAIL_COMPILER
#undef boulder
#undef bounceY
#undef damageLevel
#undef firstRoll
#undef gateIndex
#undef impactSound
#undef impactX
#undef impactY
#undef index
#undef keepIndex
#undef limits
#undef loadedSample
#undef midpointX
#undef missShot
#undef previousX
#undef previousY
#undef projectileX
#undef projectileY
#undef random
#undef spriteFrame
#undef startDirection
#undef startX
#undef startY
#undef targetHex
#undef towerCount
#undef towerIndex
#undef wallCount
#undef xStep
#endif

#if H2_RETAIL_COMPILER
#define attackBonus attackBonus4
#define bestValue bestValue5
#define damage damage5
#define keepSample keepSample7
#define killed killed0
#define missileAngles missileAngles1
#define priority priority7
#define shotCount shotCount0
#define sourceX sourceX8
#define sourceY sourceY8
#define target target9
#define targetX targetX6
#define targetY targetY1
#define towerOrigins towerOrigins7
#define unknownTowerData unknownTowerData1
#define value value9
#endif
VA(0x00429990, 0x662)
void combatManager::KeepAttack(H2_ENUM_PARAM(CombatTowerSelector, i32) tower) {
    if (!m_inCastleCombat)
        return;
    if ((tower == COMBAT_TOWER_GARRISON
         && m_wallStates[IDX(COMBAT_WALL_SLOT_KEEP)] != COMBAT_WALL_STATE_KEEP_STANDING)
        || (tower == COMBAT_TOWER_TOP
            && m_wallStates[IDX(COMBAT_WALL_SLOT_TOP_TOWER)]
                   != COMBAT_WALL_STATE_TOWER_STANDING)
        || (tower == COMBAT_TOWER_BOTTOM
            && m_wallStates[IDX(COMBAT_WALL_SLOT_BOTTOM_TOWER)]
                   != COMBAT_WALL_STATE_TOWER_STANDING))
        return;

    LogStr("KA1");

    CombatKeepTargetPriority bestPriority = KEEP_PRIORITY_NONE;
    i32 bestValue = 0;
    i32 bestArmyIndex = -1;
    i32 armyIndex;
    army* target;
    i32 value;
    CombatKeepTargetPriority priority;
    for (armyIndex = 0; armyIndex < COMBAT_ARMY_CAPACITY; armyIndex++) {
        if (m_armies[IDX(COMBAT_ATTACKER_SIDE)][armyIndex].IsAlive()) {
            target = &m_armies[IDX(COMBAT_ATTACKER_SIDE)][armyIndex];
            if (ARMY_HAS_INCAPACITATING_SPELL(*target)
                || ARMY_HAS_BERSERK_OR_HYPNOTIZE(*target)) {
                priority = KEEP_PRIORITY_DISABLED;
            } else if (HAS(target->m_monster.flags.all, MONSTER_FLAGS_SHOOTER)) {
                priority = KEEP_PRIORITY_SHOOTER;
            } else if (HAS(target->m_monster.flags.all, MONSTER_FLAGS_FLYING)) {
                priority = KEEP_PRIORITY_FLYER;
            } else {
                priority = KEEP_PRIORITY_WALKER;
            }

            value =
                target->m_quantity * gMonsterDatabase[IDX(target->m_monsterType)].fightValue;
            if (priority > bestPriority
                || (priority == bestPriority && value > bestValue)) {
                bestValue = value;
                bestPriority = priority;
                bestArmyIndex = armyIndex;
            }
        }
    }

    if (bestArmyIndex == -1)
        return;

    target = &gpCombatManager->m_armies[IDX(COMBAT_ATTACKER_SIDE)][bestArmyIndex];
    sprintf(gText, "keepshot.82M");
    SAMPLE2 keepSample = LoadPlaySample(gText);

    CombatTowerOrigin towerOrigins[COMBAT_KEEP_FACTION_COUNT][COMBAT_KEEP_TOWER_COUNT] = {
        {{586, 177}, {428, 60}, {428, 314}},
        {{586, 177}, {428, 60}, {428, 314}},
        {{586, 177}, {428, 60}, {428, 314}},
        {{586, 177}, {428, 60}, {428, 314}},
        {{586, 177}, {428, 60}, {428, 314}},
        {{586, 177}, {428, 60}, {428, 314}}
    };
    i32 H2_UNUSED(unknownTowerData)[KEEP_TOWER_SCRATCH_COUNT];
    i32 sourceX = towerOrigins[IDX(m_combatTowns[IDX(COMBAT_DEFENDER_SIDE)]->m_type)][IDX(tower)].x;
    i32 sourceY = towerOrigins[IDX(m_combatTowns[IDX(COMBAT_DEFENDER_SIDE)]->m_type)][IDX(tower)].y;
    i32 targetX = target->MidX();
    i32 targetY = target->MidY();
    float missileAngles[COMBAT_KEEP_MISSILE_ANGLE_COUNT] =
        {90.0f, 68.5f, 45.0f, 20.8f, 0.0f, -20.8f, -45.0f, -68.5f, -90.0f};
    ShootMissile(
        sourceX,
        sourceY,
        targetX,
        targetY,
        missileAngles,
        m_combatIcons[IDX(COMBAT_ICON_KEEP)]
    );

    i32 shotCount;
    i32 attackBonus;
    m_combatTowns[IDX(COMBAT_DEFENDER_SIDE)]->CalcNumLevelArchers(&shotCount, &attackBonus);
    attackBonus += COMBAT_KEEP_TOWER_DAMAGE_BONUS;
    if (m_heroes[IDX(COMBAT_DEFENDER_SIDE)])
        attackBonus += m_heroes[IDX(COMBAT_DEFENDER_SIDE)]->Stats(HERO_PRIMARY_ATTACK);
    attackBonus -= target->m_monster.defense;
    if (attackBonus > COMBAT_KEEP_ATTACK_STAT_LIMIT)
        attackBonus = COMBAT_KEEP_ATTACK_STAT_LIMIT;
    if (attackBonus < -COMBAT_KEEP_ATTACK_STAT_LIMIT)
        attackBonus = -COMBAT_KEEP_ATTACK_STAT_LIMIT;
    if (tower != COMBAT_TOWER_GARRISON)
        shotCount /= COMBAT_KEEP_SIDE_TOWER_SHOT_DIVISOR;

    i32 damage = 0;
    for (armyIndex = 0; armyIndex < shotCount; armyIndex++)
        damage += SRandom(COMBAT_KEEP_RANDOM_DAMAGE_MIN, COMBAT_KEEP_RANDOM_DAMAGE_MAX);
    damage = static_cast<i32l>(
        damage * gfBattleStat[attackBonus + COMBAT_KEEP_ATTACK_STAT_INDEX_OFFSET]
    );
    if (damage <= 0)
        damage = COMBAT_KEEP_MIN_DAMAGE;

    i32 killed = target->Damage(damage, SPELL_NONE);
    if (killed > 0) {
        sprintf(
            gText,
            "%s %d %s.\n%d %s %s.",
            tower == COMBAT_TOWER_GARRISON
                ? localization::Tr("combat.tower.garrison.damage.prefix")
                : localization::Tr("combat.tower.keep.damage.prefix"),
            damage,
            localization::Tr("combat.fragment.damage_points"),
            killed,
            CREATURE_DISPLAY_NAME(target->m_monsterType, killed),
            killed <= 1 ? localization::Tr("combat.fragment.dies") : localization::Tr("combat.fragment.killed")
        );
    } else {
        sprintf(
            gText,
            "%s %d %s.",
            tower == COMBAT_TOWER_GARRISON ? localization::Tr("combat.tower.garrison.damage.prefix")
                                                : localization::Tr("combat.tower.keep.damage.prefix"),
            damage,
            localization::Tr("combat.fragment.damage_points")
        );
    }
    gpCombatManager->CombatMessage(gText, 1, 1, 0);
    target->CancelSpellType(ARMY_CANCEL_SPELLS_AFTER_DAMAGE);
    target->PowEffect(COMBAT_EFFECT_INVALID, 1, -1, -1);
    WaitEndSample(&keepSample);
}
#if H2_RETAIL_COMPILER
#undef attackBonus
#undef bestValue
#undef damage
#undef keepSample
#undef killed
#undef missileAngles
#undef priority
#undef shotCount
#undef sourceX
#undef sourceY
#undef target
#undef targetX
#undef targetY
#undef towerOrigins
#undef unknownTowerData
#undef value
#endif

#if H2_RETAIL_COMPILER
#define experienceValue experienceValue6
#endif
VA(0x00429ff2, 0x128)
i32 combatManager::ExperienceValueOfStack(H2_ENUM_PARAM(CombatSide, i32) side) {
    i32 experienceValue = 0;
    i32 index;

    for (index = 0; index < COMBAT_ARMY_CAPACITY; index++) {
        if (m_armies[IDX(side)][index].m_monsterType != CREATURE_NONE
            && !HAS(m_armies[IDX(side)][index].m_monster.flags.all, MONSTER_FLAGS_SUMMONED)) {
            experienceValue +=
                (m_armies[IDX(side)][index].m_initialQuantity - m_armies[IDX(side)][index].m_quantity)
                * gMonsterDatabase[IDX(m_armies[IDX(side)][index].m_monsterType)].hitPoints;
        }
    }
    if (m_heroes[IDX(side)])
        experienceValue += COMBAT_HERO_EXPERIENCE_VALUE;
    return experienceValue;
}
#if H2_RETAIL_COMPILER
#undef experienceValue
#endif

VA(0x0042a11a, 0x68)
void combatManager::ResetHitByCreature(void) {
    CombatSide side;
    i32 index;

    for (side = COMBAT_ATTACKER_SIDE; IDX(side) < COMBAT_SIDE_COUNT; side++) {
        for (index = 0; index < COMBAT_ARMY_CAPACITY; index++)
            m_armies[IDX(side)][index].m_hitByCreature = false;
    }
}

VA(0x0042a182, 0x2c)
i32 ValidHex(i32 hex) {
    return hex >= 0 && hex < COMBAT_HEX_COUNT ? 1 : 0;
}

VA(0x0042a1ae, 0xb)
void combatManager::SaveCombatBorder(void) {
    return;
}

VA(0x0042a1b9, 0xb)
void combatManager::DrawCombatBorder(void) {
    return;
}

VA(0x0042a1c4, 0x46a)
void combatManager::SetupAndLoadObstacles(void) {
    i32 obstacleType;
    i32 misses;
    u32 groundMask;
    i32 obstacleGoal;
    i32 startRow;
    i32 site;
    i32 elevation;
    i32 elevCells;
    i32 cellIndex;
    u8 typeUsed[COMBAT_OBSTACLE_TYPE_COUNT];
    b32 blocked;
    i32 placedCells;
    i32 obstacleHex;

    m_debugFormation = 0;
    if (m_inCastleCombat) {
        m_wallStates[IDX(COMBAT_WALL_SLOT_KEEP)] = COMBAT_WALL_STATE_KEEP_STANDING;
        for (cellIndex = 0; cellIndex < COMBAT_CASTLE_STRUCTURE_COUNT; cellIndex++) {
            m_wallStates[cellIndex + IDX(COMBAT_WALL_SLOT_SECTION_FIRST)] =
                COMBAT_WALL_STATE_KEEP_STANDING;
            if (m_combatTowns[IDX(COMBAT_DEFENDER_SIDE)]->m_type == FACTION_KNIGHT
                && HAS(
                    m_combatTowns[IDX(COMBAT_DEFENDER_SIDE)]->m_buildings,
                    IDX(TOWN_BUILDING_FORTIFICATIONS)
                )) {
                m_wallStates[cellIndex + IDX(COMBAT_WALL_SLOT_SECTION_FIRST)] =
                    COMBAT_WALL_STATE_SECTION_DAMAGE_FIRST;
            }
            m_wallStates[cellIndex] = COMBAT_WALL_STATE_KEEP_STANDING;
        }
        if (HAS(m_combatTowns[IDX(COMBAT_DEFENDER_SIDE)]->m_buildings,
                IDX(TOWN_BUILDING_LEFT_TURRET)))
            m_wallStates[IDX(COMBAT_WALL_SLOT_TOP_TOWER)] = COMBAT_WALL_STATE_TOWER_STANDING;
        if (HAS(m_combatTowns[IDX(COMBAT_DEFENDER_SIDE)]->m_buildings,
                IDX(TOWN_BUILDING_RIGHT_TURRET)))
            m_wallStates[IDX(COMBAT_WALL_SLOT_BOTTOM_TOWER)] =
                COMBAT_WALL_STATE_TOWER_STANDING;

        m_hexCells[IDX(COMBAT_CASTLE_HEX_TOP_TOWER)].m_blocked = 1;
        m_hexCells[IDX(COMBAT_CASTLE_HEX_TOP_WALL)].m_blocked = 1;
        m_hexCells[IDX(COMBAT_CASTLE_HEX_SECOND_TOWER)].m_blocked = 1;
        m_hexCells[IDX(COMBAT_CASTLE_HEX_SECOND_WALL)].m_blocked = 1;
        m_hexCells[IDX(COMBAT_CASTLE_HEX_GATE)].m_blocked = 1;
        m_hexCells[IDX(COMBAT_CASTLE_HEX_THIRD_WALL)].m_blocked = 1;
        m_hexCells[IDX(COMBAT_CASTLE_HEX_THIRD_TOWER)].m_blocked = 1;
        m_hexCells[IDX(COMBAT_CASTLE_HEX_BOTTOM_WALL)].m_blocked = 1;
        m_hexCells[IDX(COMBAT_CASTLE_HEX_BOTTOM_TOWER)].m_blocked = 1;
        m_hexCells[IDX(COMBAT_CASTLE_HEX_MOAT)].m_blocked = 1;
    } else {
        obstacleGoal = SRandom(COMBAT_RANDOM_OBSTACLE_MIN, COMBAT_RANDOM_OBSTACLE_MAX);
        placedCells = 0;
        groundMask = 1 << IDX(m_terrainType);
        misses = 0;
        elevCells = 0;
        if (SRandom(0, COMBAT_RANDOM_PERCENT_MAX) < COMBAT_ELEVATION_OVERLAY_CHANCE) {
            while (misses++ < COMBAT_ELEVATION_OVERLAY_TRY_LIMIT) {
                elevation = SRandom(0, COMBAT_ELEVATION_OVERLAY_COUNT - 1);
                if (sElevationOverlay[elevation].terrainMask & groundMask) {
                    m_debugFormation = elevation;
                    for (cellIndex = 0; cellIndex < COMBAT_ELEVATION_OVERLAY_CELL_COUNT;
                         cellIndex++) {
                        if (sElevationOverlay[m_debugFormation].cellOffsets[cellIndex] != -1) {
                            m_hexCells[sElevationOverlay[m_debugFormation].cellOffsets[cellIndex]]
                                .m_blocked = 1;
                            elevCells++;
                        }
                    }
                    break;
                }
            }
        }
        obstacleGoal -= elevCells / ELEVATION_OBSTACLE_WEIGHT_DIVISOR;
        misses = 0;
        memset(typeUsed, 0, sizeof(typeUsed));
        while (placedCells < obstacleGoal && misses < COMBAT_OBSTACLE_TRY_LIMIT) {
            misses++;
            site = SRandom(0, COMBAT_OBSTACLE_CELL_ROLL_MAX);
            obstacleType = SRandom(0, COMBAT_OBSTACLE_INCLUSIVE_ROLL_HIGH);
            if (!(sCmbtObstacles[obstacleType].terrainMask & groundMask)
                || typeUsed[obstacleType] != 0)
                continue;
            startRow = site / COMBAT_GRID_ROW_LENGTH;
            if (sCmbtObstacles[obstacleType].minimumColumn
                > startRow + COMBAT_OBSTACLE_MIN_COLUMN_OFFSET)
                continue;
            blocked = false;
            for (cellIndex = 0; cellIndex < sCmbtObstacles[obstacleType].cellCount; cellIndex++) {
                obstacleHex = site + sCmbtObstacles[obstacleType].cellOffsets[cellIndex];
                if (obstacleHex % COMBAT_GRID_ROW_LENGTH <= COMBAT_OBSTACLE_LEFT_COLUMN_LIMIT - 1
                    || obstacleHex % COMBAT_GRID_ROW_LENGTH >= COMBAT_OBSTACLE_RIGHT_COLUMN_FIRST) {
                    blocked = true;
                }
                if (m_hexCells[obstacleHex].m_blocked != 0)
                    blocked = true;
            }
            if (blocked != 0)
                continue;
            misses = 0;
            placedCells += sCmbtObstacles[obstacleType].cellCount;
            typeUsed[obstacleType] = 1;
            for (cellIndex = 0; cellIndex < sCmbtObstacles[obstacleType].cellCount; cellIndex++) {
                m_hexCells[site + sCmbtObstacles[obstacleType].cellOffsets[cellIndex]].m_blocked =
                    1;
            }
            sprintf(gText, "cobj%04d.icn", obstacleType);
            m_obstacleIcons[m_obstacleCount] = gpResourceManager->GetIcon(gText);
            m_hexCells[site].m_obstacleIndex = static_cast<i8>(m_obstacleCount);
            m_obstacleCount++;
        }
    }
}

VA(0x0042a62e, 0x263)
void combatManager::MakeCreaturesVanish(void) {
    ResetLimitCreature();
    i32 iSide;
    army* removedArmy;
    i32 armyIndex;
    for (iSide = 0; iSide < COMBAT_SIDE_COUNT; iSide++) {
        for (armyIndex = 0; armyIndex < gpCombatManager->m_armyCount[iSide]; armyIndex++) {
            if (m_removedArmies[iSide][armyIndex])
                m_limitCreatureCount[iSide][armyIndex] = 1;
        }
    }
    DrawFrame(0, 1, 0, 1, COMBAT_DOOR_ANIMATION_DELAY, 1, 1);
    i32 x = giMinExtentX;
    i32 y = giMinExtentY;
    i32 width = giMaxExtentX - giMinExtentX + 1;
    i32 height = giMaxExtentY - giMinExtentY + 1;
    for (iSide = 0; iSide < COMBAT_SIDE_COUNT; iSide++) {
        for (armyIndex = 0; armyIndex < gpCombatManager->m_armyCount[iSide]; armyIndex++) {
            if (m_removedArmies[iSide][armyIndex]) {
                removedArmy = &m_armies[iSide][armyIndex];
                CLEAR_HEX_OCCUPANT(m_hexCells[removedArmy->m_hex]);
                if (HAS(removedArmy->m_monster.flags.all, MONSTER_FLAGS_WIDE)) {
                    CLEAR_HEX_OCCUPANT(
                        m_hexCells
                            [removedArmy->m_hex + ArmyFacingRearHexOffset(removedArmy->m_facing)]
                    );
                }
            }
        }
    }
    gpWindowManager->SaveFizzleSource(x, y, width, height);
    gpCombatManager->DrawFrame(0, 0, 1, 0, COMBAT_DOOR_ANIMATION_DELAY, 1, 1);
    gpWindowManager->FizzleForward(
        x,
        y,
        width,
        height,
        static_cast<i32>(COMBAT_CREATURE_VANISH_DURATION * gfCombatSpeedMod[gConfig.combatSpeed]),
        NULL,
        NULL
    );
}

VA(0x0042a891, 0x89)
void combatManager::LowerDoor(void) {
    SAMPLE2 drawbridgeSample = LoadPlaySample("drawbrg.82m");
    giMinExtentX = COMBAT_DOOR_EXTENT_MIN_X;
    giMinExtentY = COMBAT_DOOR_EXTENT_MIN_Y;
    giMaxExtentX = COMBAT_DOOR_EXTENT_MAX_X;
    giMaxExtentY = COMBAT_DOOR_EXTENT_MAX_Y;
    CombatDrawbridgeState bridgeFrame;
    for (bridgeFrame = COMBAT_DRAWBRIDGE_RAISE_FRAME_SECOND;
         bridgeFrame >= COMBAT_DRAWBRIDGE_LOWERED;
         bridgeFrame--) {
        m_drawbridgeState = bridgeFrame;
        DrawFrame(1, 0, 1, 0, COMBAT_DOOR_ANIMATION_DELAY, 1, 1);
    }
    WaitEndSample(&drawbridgeSample);
}

VA(0x0042a91a, 0xb6)
void combatManager::RaiseDoor(void) {
    SAMPLE2 drawbridgeSample = LoadPlaySample("drawbrg.82m");
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
    WaitEndSample(&drawbridgeSample);
}

VA(0x0042a9d0, 0x61)
void combatManager::TestRaiseDoor(void) {
    if (m_inCastleCombat && m_drawbridgeState == COMBAT_DRAWBRIDGE_LOWERED
        && m_hexCells[COMBAT_CASTLE_GATE_APPROACH_HEX].m_occupantSide == COMBAT_SIDE_NONE
        && m_hexCells[COMBAT_CASTLE_GATE_APPROACH_HEX].m_deadOccupantCount == 0
        && m_hexCells[IDX(COMBAT_CASTLE_HEX_GATE)].m_occupantSide == COMBAT_SIDE_NONE
        && m_hexCells[IDX(COMBAT_CASTLE_HEX_GATE)].m_deadOccupantCount == 0) {
        RaiseDoor();
    }
}

VA(0x0042aa31, 0x8e)
i32 combatManager::InCastle(i32 hex) {
    return ((hex < CASTLE_INTERIOR_ROW_0_FIRST || hex > CASTLE_INTERIOR_ROW_0_LAST)
            && (hex < CASTLE_INTERIOR_ROW_1_FIRST || hex > CASTLE_INTERIOR_ROW_1_LAST)
            && (hex < CASTLE_INTERIOR_ROW_2_FIRST || hex > CASTLE_INTERIOR_ROW_2_LAST)
            && (hex < CASTLE_INTERIOR_ROW_3_FIRST || hex > CASTLE_INTERIOR_ROW_3_LAST)
            && (hex < CASTLE_INTERIOR_ROW_4_FIRST || hex > CASTLE_INTERIOR_ROW_4_LAST)
            && (hex < CASTLE_INTERIOR_ROW_5_FIRST || hex > CASTLE_INTERIOR_ROW_5_LAST)
            && (hex < CASTLE_INTERIOR_ROW_6_FIRST || hex > CASTLE_INTERIOR_ROW_6_LAST)
            && (hex < CASTLE_INTERIOR_ROW_7_FIRST || hex > CASTLE_INTERIOR_ROW_7_LAST)
            && (hex < CASTLE_INTERIOR_ROW_8_FIRST
                || hex > CASTLE_INTERIOR_ROW_8_LAST))
               ? 1
               : 0;
}

#if H2_RETAIL_COMPILER
#define sourceLine srcLine
#define targetLine tgtLine
#endif
VA(0x0042aabf, 0x29b)
i32 combatManager::ShotIsThroughWall(
    H2_ENUM_PARAM(CombatSide, i32) side, i32 sourceHex, i32 targetHex
) {
    float columnStride;
    i32 colTarget;
    float rowStride;
    i32 traceLength;
    i32 targetLine;
    i32 traceSquare;
    float traceRow;
    float traceColumn;
    i32 structIndex;
    i32 traceIx;
    i32 columnDist;
    i32 rowSpan;
    i32 sourceColumn;
    i32 sourceLine;

    if (!m_inCastleCombat)
        return 0;
    if (m_heroes[IDX(side)]
        && (m_heroes[IDX(side)]->HasArtifact(ARTIFACT_GOLDEN_BOW)
            || m_heroes[IDX(side)]->m_secondarySkills[IDX(HERO_SKILL_ARCHERY)]
                   != HERO_SKILL_LEVEL_NONE)) {
        return 0;
    }
    if (InCastle(sourceHex) || !InCastle(targetHex))
        return 0;

    sourceColumn = sourceHex % COMBAT_GRID_ROW_LENGTH;
    sourceLine = sourceHex / COMBAT_GRID_ROW_LENGTH;
    colTarget = targetHex % COMBAT_GRID_ROW_LENGTH;
    targetLine = targetHex / COMBAT_GRID_ROW_LENGTH;
    columnDist = colTarget - sourceColumn;
    rowSpan = targetLine - sourceLine;
    if (abs(columnDist) > abs(rowSpan)) {
        traceLength = abs(columnDist);
        columnStride = columnDist > 0 ? 1 : -1;
        rowStride = static_cast<float>(rowSpan) / static_cast<float>(abs(columnDist));
    } else {
        traceLength = abs(rowSpan);
        rowStride = rowSpan > 0 ? 1 : -1;
        columnStride = static_cast<float>(columnDist) / static_cast<float>(abs(rowSpan));
    }
    columnStride /= static_cast<float>(COMBAT_WALL_TRACE_SUBDIVISIONS)
    ;
    rowStride /= static_cast<float>(COMBAT_WALL_TRACE_SUBDIVISIONS);
    traceColumn = static_cast<float>(sourceColumn);
    traceRow = static_cast<float>(sourceLine);
    for (traceIx = 0; traceIx < traceLength * COMBAT_WALL_TRACE_SUBDIVISIONS;
         traceIx++) {
        traceColumn += columnStride;
        traceRow += rowStride;
        traceSquare =
            static_cast<i32>(traceRow) * COMBAT_GRID_ROW_LENGTH + static_cast<i32>(traceColumn);
        for (structIndex = 0; structIndex < COMBAT_CASTLE_STRUCTURE_COUNT;
             structIndex++) {
            if (traceSquare == iWallToHexCell[structIndex]
                && m_wallStates[structIndex + IDX(COMBAT_WALL_SLOT_SECTION_FIRST)]
                       != COMBAT_WALL_STATE_DESTROYED
                && m_wallStates[structIndex + IDX(COMBAT_WALL_SLOT_SECTION_FIRST)]
                       != COMBAT_WALL_STATE_SECTION_DESTROYED) {
                return 1;
            }
            if (traceSquare == iTowerToHexCell[structIndex]
                && m_wallStates[structIndex] != COMBAT_WALL_STATE_DESTROYED) {
                return 1;
            }
            if (traceSquare == IDX(COMBAT_CASTLE_HEX_GATE)
                && m_drawbridgeState == COMBAT_DRAWBRIDGE_RAISED) {
                return 1;
            }
        }
    }
    return 0;
}
#if H2_RETAIL_COMPILER
#undef sourceLine
#undef targetLine
#endif

#if H2_RETAIL_COMPILER
#define absXLength absXLen
#define yLength yLen
#endif
VA(0x0042ad5a, 0x4e0)
void combatManager::ShootMissile(
    i32 sourceX,
    i32 sourceY,
    i32 targetX,
    i32 targetY,
    float* directionAngles,
    icon* missileIcon
) {
    i32 angleFrame;
    i32 oldX;
    i32 yLength;
    H2_ENUM_STORAGE(IconDrawOrientation, i8) reverseMissile;
    i32 missileSteps;
    i32 oldY;
    i32 incX;
    i32 posX;
    bitmap* missileBackground;
    i32 minimumX;
    i32 missileHalfWidth;
    i32 total;
    i32 yStep;
    i32 posY;
    i32 xSize;
    i32 frame;
    i32 minY;
    i32 absXLength;
    i32 missileHalfHeight;
    i32 maxX;
    i32 slopeDy;
    i32 maxY;

    xSize = targetX - sourceX;
    yLength = targetY - sourceY;
    absXLength = targetX - sourceX;
    reverseMissile = ICON_DRAW_NORMAL;
    if (absXLength < 0) {
        reverseMissile = ICON_DRAW_FLIPPED;
        absXLength = -absXLength;
    }
    slopeDy = targetY - sourceY;
    if (absXLength == 0) {
        angleFrame = slopeDy > 0 ? COMBAT_MISSILE_LAST_DIRECTION : 0;
    } else {
        float slope = static_cast<double>(-slopeDy) / (static_cast<double>(absXLength));
        float degrees = static_cast<float>(
            atan(static_cast<double>(slope)) * COMBAT_MISSILE_DEGREES_PER_RADIAN
            / COMBAT_MISSILE_PI
        );
        for (frame = 1; frame < COMBAT_MISSILE_DIRECTION_COUNT; frame++) {
            if (((directionAngles[frame]) + directionAngles[frame - 1])
                    / COMBAT_MISSILE_DIRECTION_AVERAGE_DIVISOR
                < degrees) {
                break;
            }
        }
        if (frame < COMBAT_MISSILE_DIRECTION_COUNT)
            angleFrame = frame - 1;
        else
            angleFrame = COMBAT_MISSILE_LAST_DIRECTION;
    }

    total = INTEGER_VECTOR_LENGTH(xSize, yLength);
    missileSteps = (total + COMBAT_MISSILE_SPACING_ROUND) / COMBAT_MISSILE_SPACING;
    if (missileSteps > 1) {
        incX = xSize / (missileSteps - 1);
        yStep = yLength / (missileSteps - 1);
    } else {
        incX = xSize;
        yStep = yLength;
    }
    posX = sourceX;
    posY = sourceY;
    missileHalfWidth = COMBAT_MISSILE_HALF_WIDTH;
    missileHalfHeight = COMBAT_MISSILE_HALF_HEIGHT;
    missileBackground = new bitmap(
        COMBAT_MISSILE_BITMAP_TYPE,
        missileHalfWidth * MISSILE_DIAMETER_MULTIPLIER,
        missileHalfHeight * MISSILE_DIAMETER_MULTIPLIER
    );
    missileBackground->GrabBitmapCareful(
        gpWindowManager->m_screen,
        static_cast<i16>(posX - missileHalfWidth),
        static_cast<i16>(posY - missileHalfHeight)
    );

    oldX = posX;
    oldY = posY;
    minimumX = COMBAT_MAX_EXTENT_X;
    maxX = 0;
    minY = COMBAT_SCREEN_HEIGHT;
    maxY = 0;
    for (frame = 0; frame < missileSteps; frame++) {
        if (oldX - missileHalfWidth < minimumX)
            minimumX = oldX - missileHalfWidth;
        if (minimumX < 0)
            minimumX = 0;
        if (oldX + missileHalfWidth > maxX)
            maxX = oldX + missileHalfWidth;
        if (maxX > COMBAT_MAX_EXTENT_X)
            maxX = COMBAT_MAX_EXTENT_X;
        if (oldY - missileHalfHeight < minY)
            minY = oldY - missileHalfHeight;
        if (minY < 0)
            minY = 0;
        if (oldY + missileHalfHeight > maxY)
            maxY = oldY + missileHalfHeight;
        if (maxY > COMBAT_MAX_EXTENT_Y)
            maxY = COMBAT_MAX_EXTENT_Y;

        if (frame != 0) {
            missileBackground->DrawToBufferCareful(
                static_cast<i16>(oldX - missileHalfWidth),
                static_cast<i16>(oldY - missileHalfHeight)
            );
            missileBackground->GrabBitmapCareful(
                gpWindowManager->m_screen,
                static_cast<i16>(posX - missileHalfWidth),
                static_cast<i16>(posY - missileHalfHeight)
            );
        } else {
            if (minimumX < giMinExtentX)
                giMinExtentX = minimumX;
            if (maxX > giMaxExtentX)
                giMaxExtentX = maxX;
            if (minY < giMinExtentY)
                giMinExtentY = minY;
            if (maxY > giMaxExtentY)
                giMaxExtentY = maxY;
        }
        missileIcon->DrawToBuffer(posX, posY, angleFrame, reverseMissile);
        if (frame == 0) {
            UPDATE_INCLUSIVE_REGION(giMinExtentX, giMinExtentY, giMaxExtentX, giMaxExtentY);
        } else {
            DelayTil(glTimers);
            UPDATE_INCLUSIVE_REGION(minimumX, minY, maxX, maxY);
        }
        glTimers[0] = static_cast<i32>(
            KBTickCount()
            + IDX(COMBAT_MISSILE_TIMER_DELAY) * gfCombatSpeedMod[gConfig.combatSpeed]
        );
        oldX = posX;
        oldY = posY;
        posX += incX;
        posY += yStep;
        minimumX = posX - missileHalfWidth;
        maxX = posX + missileHalfWidth;
        minY = posY - missileHalfHeight;
        maxY = posY + missileHalfHeight;
    }
    missileBackground->DrawToBuffer(
        static_cast<i16>(oldX - missileHalfWidth),
        static_cast<i16>(oldY - missileHalfHeight)
    );
    gpWindowManager->UpdateScreenRegion(
        oldX - missileHalfWidth,
        oldY - missileHalfHeight,
        missileHalfWidth * MISSILE_DIAMETER_MULTIPLIER,
        missileHalfHeight * MISSILE_DIAMETER_MULTIPLIER
    );
    delete missileBackground;
}
#if H2_RETAIL_COMPILER
#undef absXLength
#undef yLength
#endif

VA(0x0042b23a, 0x10c)
void combatManager::CombatSystemOptions(void) {
    tag_message H2_UNUSED(message);
    bCPrefsChanged = false;
    CSPanel = new heroWindow(SYSTEM_OPTION_WINDOW_X, SYSTEM_OPTION_WINDOW_Y, "cspanel.bin");
    if (!CSPanel)
        MemError();
    SetWinText(CSPanel, 1);
    UpdateCombatSystemOptions(1);
    gpWindowManager->DoDialog(CSPanel, CombatSystemOptionsHandler, 0);
    delete CSPanel;
    if (bCPrefsChanged)
        WritePrefs();
    m_backgroundDrawn = false;
    DrawFrame(1, 0, 0, 0, COMBAT_DOOR_ANIMATION_DELAY, 1, 1);
}

VA(0x0042b346, 0x1e1)
void UpdateCombatSystemOptions(i32 initialDraw) {
    tag_message message;
    SET_WIDGET_MESSAGE(message, COMBAT_SYSTEM_OPTION_BUTTON_MESSAGE, SYSTEM_OPTION_SPEED_BUTTON);
    message.payload.widget.data.value =
        gConfig.combatSpeed + SYSTEM_OPTION_SPEED_STATE_OFFSET;
    CSPanel->BroadcastMessage(message);
    message.payload.widget.id = SYSTEM_OPTION_ARMY_INFO_BUTTON;
    message.payload.widget.data.value =
        gConfig.combatArmyInfoLevel + SYSTEM_OPTION_ARMY_INFO_STATE_OFFSET;
    CSPanel->BroadcastMessage(message);
    message.payload.widget.id = SYSTEM_OPTION_AUTO_SPELL_BUTTON;
    message.payload.widget.data.value =
        gConfig.autoCombatUseSpells + SYSTEM_OPTION_AUTO_SPELL_STATE_OFFSET;
    CSPanel->BroadcastMessage(message);
    message.payload.widget.id = SYSTEM_OPTION_GRID_BUTTON;
    message.payload.widget.data.value =
        gConfig.showCombatGrid + SYSTEM_OPTION_GRID_STATE_OFFSET;
    CSPanel->BroadcastMessage(message);
    message.payload.widget.id = SYSTEM_OPTION_SHADE_BUTTON;
    message.payload.widget.data.value =
        gConfig.combatShadeLevel + SYSTEM_OPTION_SHADE_STATE_OFFSET;
    CSPanel->BroadcastMessage(message);
    message.payload.widget.id = SYSTEM_OPTION_MOUSE_HEX_BUTTON;
    message.payload.widget.data.value =
        gConfig.showCombatMouseHex + SYSTEM_OPTION_MOUSE_HEX_STATE_OFFSET;
    CSPanel->BroadcastMessage(message);

    message.payload.widget.command = COMBAT_SYSTEM_OPTION_TEXT_MESSAGE;
    message.payload.widget.id = SYSTEM_OPTION_SPEED_TEXT;
    message.payload.widget.data.text = combatSpeedText[gConfig.combatSpeed];
    CSPanel->BroadcastMessage(message);
    message.payload.widget.id = SYSTEM_OPTION_ARMY_INFO_TEXT;
    message.payload.widget.data.text = combatMiniInfoText[gConfig.combatArmyInfoLevel];
    CSPanel->BroadcastMessage(message);
    message.payload.widget.id = SYSTEM_OPTION_AUTO_SPELL_TEXT;
    message.payload.widget.data.text = onOffText[gConfig.autoCombatUseSpells];
    CSPanel->BroadcastMessage(message);
    message.payload.widget.id = SYSTEM_OPTION_GRID_TEXT;
    message.payload.widget.data.text = onOffText[gConfig.showCombatGrid];
    CSPanel->BroadcastMessage(message);
    message.payload.widget.id = SYSTEM_OPTION_SHADE_TEXT;
    message.payload.widget.data.text = onOffText[gConfig.combatShadeLevel];
    CSPanel->BroadcastMessage(message);
    message.payload.widget.id = SYSTEM_OPTION_MOUSE_HEX_TEXT;
    message.payload.widget.data.text = onOffText[gConfig.showCombatMouseHex];
    CSPanel->BroadcastMessage(message);
    if (!initialDraw)
        CSPanel->DrawWindow(1, 0, SYSTEM_OPTION_DRAW_MASK);
}

VA(0x0042b527, 0x2b3)
MessageDispatchResult CombatSystemOptionsHandler(tag_message& message) {
    b32 bRedraw = false;
    b32 bDone = false;
    if (message.type == COMBAT_SYSTEM_OPTION_EVENT) {
        if (HAS(
                message.payload.widget.modifiers,
                MESSAGE_MODIFIER_RIGHT_BUTTON
            )) {
            if (message.payload.widget.command == COMBAT_SYSTEM_OPTION_BUTTON_EVENT
                || message.payload.widget.command == COMBAT_SYSTEM_OPTION_HOVER_EVENT) {
                i32 helpIndex = -1;
                switch (message.payload.widget.id) {
                    case SYSTEM_OPTION_CLOSE_BUTTON:
                        helpIndex = IDX(HELP_CLOSE);
                        break;
                    case SYSTEM_OPTION_SPEED_BUTTON:
                        helpIndex = IDX(HELP_SPEED);
                        break;
                    case SYSTEM_OPTION_ARMY_INFO_BUTTON:
                        helpIndex = IDX(HELP_ARMY_INFO);
                        break;
                    case SYSTEM_OPTION_AUTO_SPELL_BUTTON:
                        helpIndex = IDX(HELP_AUTO_SPELL);
                        break;
                    case SYSTEM_OPTION_GRID_BUTTON:
                        helpIndex = IDX(HELP_GRID);
                        break;
                    case SYSTEM_OPTION_SHADE_BUTTON:
                        helpIndex = IDX(HELP_SHADE);
                        break;
                    case SYSTEM_OPTION_MOUSE_HEX_BUTTON:
                        helpIndex = IDX(HELP_MOUSE_HEX);
                        break;
                }
                if (helpIndex >= 0) {
                    NormalDialog(gCSPanelHelp[helpIndex], SYSTEM_OPTION_HELP_DIALOG);
                }
            }
        } else {
            switch (message.payload.widget.command) {
                case COMBAT_SYSTEM_OPTION_CLOSE_EVENT:
                    switch (message.payload.widget.id) {
                        case SYSTEM_OPTION_CLOSE_BUTTON:
                            bDone = true;
                            break;
                    }
                    break;
                case COMBAT_SYSTEM_OPTION_BUTTON_EVENT:
                    switch (message.payload.widget.id) {
                        case SYSTEM_OPTION_SPEED_BUTTON:
                            gConfig.combatSpeed =
                                (gConfig.combatSpeed + 1) % SYSTEM_OPTION_CYCLE_COUNT;
                            bRedraw = true;
                            bCPrefsChanged = true;
                            break;
                        case SYSTEM_OPTION_ARMY_INFO_BUTTON:
                            gConfig.combatArmyInfoLevel = (gConfig.combatArmyInfoLevel + 1)
                                                          % SYSTEM_OPTION_CYCLE_COUNT;
                            bRedraw = true;
                            bCPrefsChanged = true;
                            break;
                        case SYSTEM_OPTION_AUTO_SPELL_BUTTON:
                            gConfig.autoCombatUseSpells = 1 - gConfig.autoCombatUseSpells;
                            bRedraw = true;
                            bCPrefsChanged = true;
                            break;
                        case SYSTEM_OPTION_GRID_BUTTON:
                            gConfig.showCombatGrid = 1 - gConfig.showCombatGrid;
                            bRedraw = true;
                            bCPrefsChanged = true;
                            break;
                        case SYSTEM_OPTION_SHADE_BUTTON:
                            gConfig.combatShadeLevel = 1 - gConfig.combatShadeLevel;
                            bRedraw = true;
                            bCPrefsChanged = true;
                            break;
                        case SYSTEM_OPTION_MOUSE_HEX_BUTTON:
                            gConfig.showCombatMouseHex = 1 - gConfig.showCombatMouseHex;
                            bRedraw = true;
                            bCPrefsChanged = true;
                            break;
                    }
                    break;
            }
        }
    }
    if (bRedraw)
        UpdateCombatSystemOptions(0);
    if (bDone) {
        FINISH_DIALOG_MESSAGE(message);
        return MESSAGE_DISPATCH_FORWARD;
    }
    return MESSAGE_DISPATCH_CONSUME;
}



DATA(0x005240e8) b32 bInHighMoraleBonus = false;
DATA(0x004f0c54) i32 giSeed = 1;
DATA(0x004f0c58) u8 wallHex[COMBAT_WALL_SECTION_COUNT] = {9, 34, 86, 113};
DATA(0x005240e4) i32 bMouseWasVis;
DATA(0x005240e0) class heroWindow* CSPanel;
DATA(0x005240dc) b32 bCPrefsChanged;

// Compiler-emitted vtables; the markers are census claims, not definitions.
VTBL(combatManager, 0x004ea540)
