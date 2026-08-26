#include <Ints.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

typedef enum CombatSystemOptionWidget {
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
} CombatSystemOptionWidget;

typedef enum CombatSystemOptionConstant {
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
} CombatSystemOptionConstant;

enum {
    HELP_CLOSE      = 0,
    HELP_SPEED      = 1,
    HELP_ARMY_INFO  = 2,
    HELP_AUTO_SPELL = 3,
    HELP_GRID       = 4,
    HELP_SHADE      = 5,
    HELP_MOUSE_HEX  = 6
};
typedef i32 CombatSystemOptionHelp;
typedef enum CombatSetupConstant {
    BALLISTA_CATAPULT_ATTACK_COUNT      = 2,
    NECROMANCER_SHRINE_POWER_BONUS      = 2,
    DEFENDER_HERO_OVERLAY_INITIAL_FRAME = 3
} CombatSetupConstant;

typedef enum CombatPresentationConstant {
    FADE_STEPS          = 8,
    AMBIENT_MUSIC_FIRST = 2,
    AMBIENT_MUSIC_LAST  = 4
} CombatPresentationConstant;

typedef enum CombatMapConstant {
    MONSTER_COUNT_SAVE_LIMIT  = 4000,
    MAP_RANDOM_OFFSET_MINIMUM = 8,
    MAP_RANDOM_OFFSET_MAXIMUM = 15
} CombatMapConstant;

typedef enum CombatNearbyConstant {
    NEARBY_RADIUS_COUNT = 3
} CombatNearbyConstant;

typedef enum CombatKeepStorageConstant {
    KEEP_TOWER_SCRATCH_COUNT = 4
} CombatKeepStorageConstant;

typedef enum CombatObstacleConstant {
    ELEVATION_OBSTACLE_WEIGHT_DIVISOR = 2
} CombatObstacleConstant;

typedef enum CombatMissileConstant {
    MISSILE_DIAMETER_MULTIPLIER = 2
} CombatMissileConstant;

enum {
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
};
typedef i32 BattlefieldBackgroundIndex;
enum {
    NEARBY_UNKNOWN  = -1,
    NEARBY_MOUNTAIN = 0,
    NEARBY_TREE     = 1
};
typedef i8 NearbyFeature;
typedef enum CombatCastleInteriorRange {
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
} CombatCastleInteriorRange;

enum {
    CATAPULT_DAMAGE_NONE   = 0,
    CATAPULT_DAMAGE_NORMAL = 1,
    CATAPULT_DAMAGE_DOUBLE = 2
};
typedef i32 CombatCatapultDamage;
enum {
    KEEP_PRIORITY_NONE     = -1,
    KEEP_PRIORITY_DISABLED = 0,
    KEEP_PRIORITY_WALKER   = 1,
    KEEP_PRIORITY_FLYER    = 2,
    KEEP_PRIORITY_SHOOTER  = 3
};
typedef i32 CombatKeepTargetPriority;
typedef enum CombatMoraleConstant {
    MORALE_ROLL_MIN              = 1,
    GOOD_MORALE_ROLL_MAX         = 24,
    BAD_MORALE_ROLL_MAX          = 12,
    BAD_MORALE_NETWORK_ROLL_MAX  = 4,
    BAD_MORALE_NETWORK_SKIP_ROLL = 1,
    MORALE_EFFECT_DURATION       = 180
} CombatMoraleConstant;

}

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
    m_catapultFrame[(COMBAT_ATTACKER_SIDE)] = -1;
    m_catapultFrame[(COMBAT_DEFENDER_SIDE)] = -1;
    m_unknownF337[(COMBAT_ATTACKER_SIDE)] = 0;
    m_unknownF337[(COMBAT_DEFENDER_SIDE)] = 0;
    m_inCastleCombat = false;
    m_mouseGridHex = -1;
    m_combatWindowOpen = false;
    strcpy(m_previousCombatMessage,
           "");
    strcpy(m_currentCombatMessage,
           "");
}

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

    m_terrainType = giGroundToTerrain[m_battlefieldCell->m_terrainImageIndex];
    sprintf(m_battlefieldBackgroundName, GetBackgroundName());

    if (attackerHero != NULL) {
        m_playerId[(COMBAT_ATTACKER_SIDE)] = attackerHero->m_owner;
        attackerGroup = &attackerHero->m_army;
    } else {
        m_playerId[(COMBAT_ATTACKER_SIDE)] = -1;
    }

    if (defenderHero != NULL) {
        m_playerId[(COMBAT_DEFENDER_SIDE)] = defenderHero->m_owner;
        defenderGroup = &defenderHero->m_army;
    } else if (defenderTown != NULL) {
        m_playerId[(COMBAT_DEFENDER_SIDE)] = defenderTown->m_owner;
        defenderGroup = &defenderTown->m_army;
    } else {
        m_playerId[(COMBAT_DEFENDER_SIDE)] = -1;
    }

    i32 index;
    for (index = (COMBAT_ATTACKER_SIDE); index < COMBAT_SIDE_COUNT; index++) {
        if (m_playerId[index] >= 0)
            m_networkArmyPresent[index] = gbHumanPlayer[m_playerId[index]];
        else
            m_networkArmyPresent[index] = 0;

        m_heroes[index] = index == (COMBAT_ATTACKER_SIDE) ? attackerHero : defenderHero;

        if (m_heroes[index] != NULL)
            m_heroes[index]->m_isCaptain = 0;

        m_armyGroups[index] = index == (COMBAT_ATTACKER_SIDE) ? attackerGroup : defenderGroup;

        m_catapultAttacksRemaining[index] = 1;
        m_catapultAttackCount[index] = 1;
        if (m_heroes[index] != NULL && m_heroes[index]->HasArtifact(ARTIFACT_BALLISTA)) {
            m_catapultAttacksRemaining[index] = BALLISTA_CATAPULT_ATTACK_COUNT;
            m_catapultAttackCount[index] = BALLISTA_CATAPULT_ATTACK_COUNT;
        }
        if (m_heroes[index] != NULL
            && m_heroes[index]->m_secondarySkills[(HERO_SKILL_BALLISTICS)]
                   >= HERO_SKILL_LEVEL_ADVANCED) {
            m_catapultAttackCount[index]++;
            m_catapultAttacksRemaining[index]++;
        }
        m_keepAttacksRemaining[index] = 1;
        m_visitingHeroPresent[index] = 0;
        m_heroCastSpell[index] = 0;
    }

    m_drawbridgeBackgroundVisible = false;
    if (defenderTown != NULL) {
        if (defenderTown->m_occupyingHeroId != -1) {
            m_armyGroups[(COMBAT_DEFENDER_SIDE)] = &m_heroes[(COMBAT_DEFENDER_SIDE)]->m_army;
            CombineGroups(&defenderTown->m_army, &m_heroes[(COMBAT_DEFENDER_SIDE)]->m_army);
            m_visitingHeroPresent[(COMBAT_DEFENDER_SIDE)] = 1;
        } else {
            m_visitingHeroPresent[(COMBAT_DEFENDER_SIDE)] = 0;
        }

        m_inCastleCombat = (defenderTown->m_buildings & (TOWN_BUILDING_CASTLE)) != 0;

        if (m_inCastleCombat != 0)
            m_drawbridgeBackgroundVisible = (defenderTown->m_buildings & (TOWN_BUILDING_MOAT)) != 0;

        m_drawbridgeState = COMBAT_CASTLE_GATE_OPEN;
        m_combatTowns[(COMBAT_DEFENDER_SIDE)] = defenderTown;
        m_originalCombatTown = defenderTown;

        if (m_heroes[(COMBAT_DEFENDER_SIDE)] == NULL
            && (defenderTown->m_buildings & (TOWN_BUILDING_CAPTAIN_QUARTERS))) {
            m_heroes[(COMBAT_DEFENDER_SIDE)] = &m_captain;
            memset(&m_captain, 0, sizeof(m_captain));
            for (index = 0; index < HERO_PRIMARY_STAT_COUNT; index++)
                m_captain.m_primaryStats[index] =
                    captainStats[(m_combatTowns[(COMBAT_DEFENDER_SIDE)]->m_type)]
                                [index];
            m_captain.m_spellPoints =
                m_captain.Stats(HERO_PRIMARY_KNOWLEDGE) * COMBAT_CAPTAIN_SPELL_POINT_MULTIPLIER;
            m_captain.m_cursorType = m_combatTowns[(COMBAT_DEFENDER_SIDE)]->m_type;
            m_captain.m_portrait = static_cast<HeroPortrait>(
                static_cast<i32>(m_combatTowns[(COMBAT_DEFENDER_SIDE)]->m_type)
                + static_cast<i32>(HERO_CAPTAIN_PORTRAIT_FIRST)
            );
            strcpy(
                m_captain.m_name,
                "Капитан"
            );
            for (index = 0; index < ARMY_GROUP_SLOT_COUNT; index++)
                m_captain.m_army.m_creatureTypes[index] = CREATURE_NONE;
            for (index = 0; index < HERO_ARTIFACT_SLOT_COUNT; index++)
                m_captain.m_artifacts[index] = ARTIFACT_NONE;
            m_captain.m_artifacts[0] = ARTIFACT_MAGIC_BOOK;
            m_combatTowns[(COMBAT_DEFENDER_SIDE)]->GiveSpells(&m_captain);
            m_captain.m_isCaptain = 1;
        }
    } else {
        m_inCastleCombat = false;
        m_combatTowns[(COMBAT_DEFENDER_SIDE)] = NULL;
    }
    m_combatTowns[(COMBAT_ATTACKER_SIDE)] = NULL;
}

void combatManager::InitNonVisualVars(void) {
    m_gridSelectionDisabled = false;
    m_nonVisualCombat = false;
    CombatSide side;
    for (side = COMBAT_ATTACKER_SIDE; (side) < COMBAT_SIDE_COUNT; side++) {
        m_spellPower[(side)] = 0;
        if (m_heroes[(side)] != NULL)
            m_spellPower[(side)] = m_heroes[(side)]->Stats(HERO_PRIMARY_SPELL_POWER);
        if (m_combatTowns[(side)] != NULL && m_combatTowns[(side)]->m_type == FACTION_NECROMANCER
            && (m_combatTowns[(side)]->m_buildings & (TOWN_BUILDING_SHRINE)))
            m_spellPower[(side)] += NECROMANCER_SHRINE_POWER_BONUS;
    }

    m_heroOverlayFrame[(COMBAT_ATTACKER_SIDE)] = 0;
    m_heroOverlayFrame[(COMBAT_DEFENDER_SIDE)] = DEFENDER_HERO_OVERLAY_INITIAL_FRAME;
    m_sideRetreated[(COMBAT_ATTACKER_SIDE)] = 0;
    m_sideRetreated[(COMBAT_DEFENDER_SIDE)] = 0;
    m_combatResult = COMBAT_RESULT_PENDING;
    m_heroDeathAnimationPlayed[0] = m_heroDeathAnimationPlayed[1] = 0;
    m_heroAlternateDeathAnimationPlayed[0] = m_heroAlternateDeathAnimationPlayed[1] = 0;
    m_heroDeathPending[0] = m_heroDeathPending[1] = 0;
    m_heroAlternateDeathPending[0] = m_heroAlternateDeathPending[1] = 0;
    m_eagleEyeSpell[(COMBAT_ATTACKER_SIDE)] = SPELL_NONE;
    m_eagleEyeSpell[(COMBAT_DEFENDER_SIDE)] = SPELL_NONE;
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
    m_sideDefeated[(COMBAT_ATTACKER_SIDE)] = 0;
    m_sideDefeated[(COMBAT_DEFENDER_SIDE)] = 0;
    m_limitCreature = true;
    m_obstacleCount = 0;
    SetupAdjacencyArray();
    GenerateMap();
    LoadArmies();
}

void combatManager::SetupAdjacencyArray(void) {
    i32 toHex = 0;
    CombatHexDirection direction;
    i32 fromHex;
    i32 row;
    for (fromHex = 0; fromHex < COMBAT_HEX_COUNT; fromHex++) {
        row = fromHex / COMBAT_GRID_ROW_LENGTH;
        for (direction = COMBAT_DIRECTION_NORTHEAST;
             (direction) < COMBAT_AI_ADJACENT_DIRECTION_COUNT;
             direction++) {
            if (fromHex % COMBAT_GRID_ROW_LENGTH == 0
                || fromHex % COMBAT_GRID_ROW_LENGTH == COMBAT_GRID_ROW_LENGTH - 1) {
                m_adjacency[fromHex][(direction)] = -1;
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
                    m_adjacency[fromHex][(direction)] = -1;
                else
                    m_adjacency[fromHex][(direction)] = static_cast<i8>(toHex);
            }
        }
    }
}

i32 combatManager::Open(i32 openFlags) {
    LogStr("Op1");
    memcpy(m_savedPalette, gPalette->m_data, COMBAT_PALETTE_DATA_SIZE);
    gpMouseManager->m_forcePointerUpdate = 1;
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
    m_smallViewLastX[(COMBAT_ATTACKER_SIDE)] = -1;
    m_smallViewLastX[(COMBAT_DEFENDER_SIDE)] = -1;
    memset(m_gridState, (GRID_SHADE_NONE), sizeof(m_gridState));
    LoadIcons();
    InitNonVisualVars();
    SetupAndLoadObstacles();
    memset(m_previousGridState, (GRID_SHADE_NONE), sizeof(m_previousGridState));
    GetNextArmy(0);
    m_backgroundDrawn = false;

    SAMPLE2 preBattleSample = LoadPlaySample("PREBATTL.82M");
    gpWindowManager->FadeScreen(FADE_OUT, FADE_STEPS, NULL);
    giCycleType = m_colorCycleType;
    CycleColors(1);
    CycleColors(1);
    gCurLoadedSpellIcon = NULL;
    gCurLoadedSpellEffect = COMBAT_EFFECT_INVALID;
    gpMouseManager->m_forcePointerUpdate = 0;
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
    WaitEndSample(&preBattleSample, -1);

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
    for (i = (COMBAT_ATTACKER_SIDE); i < COMBAT_SIDE_COUNT; i++)
        UpdateArmyGroup(static_cast<CombatSide>(i));

    total = 0;
    groupSide = static_cast<CombatSide>(m_playerId[(COMBAT_DEFENDER_SIDE)] == -1);

    for (i = 0; i < ARMY_GROUP_SLOT_COUNT; i++) {
        if (m_armyGroups[(groupSide)]->m_creatureTypes[i] != CREATURE_NONE)
            total += m_armyGroups[(groupSide)]->m_creatureCounts[i];
    }

    if (m_battlefieldCell->m_triggerType == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_MONSTER)) {
        if (total > MONSTER_COUNT_SAVE_LIMIT)
            total = MONSTER_COUNT_SAVE_LIMIT;
        m_battlefieldCell->m_objectMetadata = total & (MAP_MONSTER_COUNT_MASK);
    }

    if (m_battlefieldCell->m_triggerType == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_MINE)
        && gpGame->m_mines[m_battlefieldCell->m_objectMetadata].guardianType != CREATURE_NONE)
        gpGame->m_mines[m_battlefieldCell->m_objectMetadata].guardianCount = static_cast<u8>(total);

    if (m_battlefieldCell->m_triggerType
        == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_HERO_INTERACTION)) {
        hero* combatHero = gpGame->GetHero(m_battlefieldCell->m_objectMetadata);
        if (combatHero->m_locationType == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_MINE)
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

void combatManager::UpdateArmyGroup(CombatSide side) {
    i32 index;
    i32 pos;
    for (index = 0; index < ARMY_GROUP_SLOT_COUNT; index++) {
        m_armyGroups[(side)]->m_creatureTypes[index] = CREATURE_NONE;
        m_armyGroups[(side)]->m_creatureCounts[index] = 0;
    }

    for (index = 0; index < m_armyCount[(side)]; index++) {
        if (!(((m_armies[(side)][index].m_monster.flags.all) & (MONSTER_FLAGS_AI_EXCLUDED)))
            && m_armies[(side)][index].m_quantity > 0
            && (m_playerId[(side)] == -1
                || ((m_armies[(side)][index].m_monsterType != CREATURE_EARTH_ELEMENTAL
                     && m_armies[(side)][index].m_monsterType != CREATURE_AIR_ELEMENTAL
                     && m_armies[(side)][index].m_monsterType != CREATURE_FIRE_ELEMENTAL
                     && m_armies[(side)][index].m_monsterType != CREATURE_WATER_ELEMENTAL)
                    || !(((m_armies[(side)][index].m_monster.flags.all) & (MONSTER_FLAGS_SUMMONED)))))
            && !(((m_armies[(side)][index].m_monster.flags.all) & (MONSTER_FLAGS_MIRROR_IMAGE)))) {
            m_armyGroups[(side)]->m_creatureTypes[m_armies[(side)][index].m_armyGroupSlot] =
                m_armies[(side)][index].m_monsterType;
            m_armyGroups[(side)]->m_creatureCounts[m_armies[(side)][index].m_armyGroupSlot] =
                static_cast<i16>(m_armies[(side)][index].m_quantity);
        }
    }

    if (giSkeletonsCreated && CombatResultForSide(side) == m_combatResult)
        m_armyGroups[(side)]->Add(CREATURE_SKELETON, giSkeletonsCreated, ARMY_GROUP_EMPTY_SLOT);
}

void combatManager::GenerateMap(void) {
    i32 gridX4;
    i32 randomOffset;
    i32 x;
    u32 y;
    i32 coordinateY;

    m_catapultFrame[(COMBAT_ATTACKER_SIDE)] = m_inCastleCombat == 1 ? 0 : -1;

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

char* combatManager::GetBackgroundName(void) {
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
    return cCombatBkgNames[(backgroundIndex)];
}

i32 combatManager::MoreTreesNear(void) {
    i32 treeCount6;
    i32 x;
    i32 y;
    i32 mountainCounter1;
    mapCell* combatCell10;
    i32 radius;
    i32 combatOriginX10;
    NearbyFeature nearbyTypeTable4[NEARBY_RADIUS_COUNT][NORMAL_DIRECTION_COUNT];
    u8 nearbyTileset;
    i32 nearbyDirection6;
    i32 centerY8;

    memset(nearbyTypeTable4, (NEARBY_UNKNOWN), sizeof(nearbyTypeTable4));
    combatOriginX10 = m_combatX;
    centerY8 = m_combatY;

    for (radius = 0; radius < NEARBY_RADIUS_COUNT; radius++) {
        for (nearbyDirection6 = 0; nearbyDirection6 < NORMAL_DIRECTION_COUNT; nearbyDirection6++) {
            x = combatOriginX10 + normalDirTable[nearbyDirection6].x * radius;
            y = centerY8 + normalDirTable[nearbyDirection6].y * radius;
            if (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT) {
                combatCell10 = gpAdvManager->GetCell(x, y);
                nearbyTileset = combatCell10->m_objectTileset;
                switch (nearbyTileset) {
                    case TILESET_MTNSNOW:
                    case TILESET_MTNSWMP:
                    case TILESET_MTNLAVA:
                    case TILESET_MTNDSRT:
                    case TILESET_MTNDIRT:
                    case TILESET_MTNMULT:
                    case TILESET_MTNCRCK:
                    case TILESET_MTNGRAS:
                        nearbyTypeTable4[radius][nearbyDirection6] = NEARBY_MOUNTAIN;
                        break;
                    case TILESET_TREJNGL:
                    case TILESET_TREEVIL:
                    case TILESET_TRESNOW:
                    case TILESET_TREFIR:
                    case TILESET_TREFALL:
                        nearbyTypeTable4[radius][nearbyDirection6] = NEARBY_TREE;
                        break;
                }
            }
        }
    }

    treeCount6 = 0;
    mountainCounter1 = 0;
    for (radius = 0; radius < NEARBY_RADIUS_COUNT; radius++) {
        for (nearbyDirection6 = 0; nearbyDirection6 < NORMAL_DIRECTION_COUNT; nearbyDirection6++) {
            if (nearbyTypeTable4[radius][nearbyDirection6] == NEARBY_MOUNTAIN)
                mountainCounter1++;
            if (nearbyTypeTable4[radius][nearbyDirection6] == NEARBY_TREE)
                treeCount6++;
        }
    }
    if (treeCount6 > mountainCounter1)
        return 1;
    return 0;
}

void combatManager::LoadIcons(void) {
    i32 index;
    for (index = 0; index < COMBAT_FIXED_ICON_COUNT; index++)
        m_combatIcons[index] = NULL;

    for (index = 0; index < COMBAT_OBSTACLE_ICON_LOAD_COUNT; index++)
        m_obstacleIcons[index] = NULL;

    m_combatIcons[(COMBAT_ICON_SPELLS)] = gpResourceManager->GetIcon("spells.icn");
    m_combatIcons[(COMBAT_ICON_STATUS)] = gpResourceManager->GetIcon("textbar.icn");
    m_combatIcons[(COMBAT_ICON_GRID)] = gpResourceManager->GetIcon("cmbtmisc.icn");
    m_combatIcons[(COMBAT_ICON_SMALL_VIEW_BACKGROUND)] =
        gpResourceManager->GetIcon("viewarsm.icn");
    m_combatIcons[(COMBAT_ICON_SMALL_VIEW_MODIFIER)] =
        gpResourceManager->GetIcon("minilkmr.icn");
    m_combatIcons[(COMBAT_ICON_SMALL_VIEW_SPELL)] = gpResourceManager->GetIcon("spellinf.icn");

    if (m_inCastleCombat) {
        if (m_combatTowns[(COMBAT_DEFENDER_SIDE)]->m_buildings & (TOWN_BUILDING_MOAT)) {
            m_combatIcons[(COMBAT_ICON_MOAT)] = gpResourceManager->GetIcon("moatpart.icn");
            m_combatIcons[(COMBAT_ICON_DRAWBRIDGE)] = gpResourceManager->GetIcon("moatwhol.icn");
        }
        m_combatIcons[(COMBAT_ICON_CATAPULT)] = gpResourceManager->GetIcon("catapult.icn");
        sprintf(
            gText,
            "castle%c.icn",
            cHeroTypeInitial[(m_combatTowns[(COMBAT_DEFENDER_SIDE)]->m_type)]
        );
        m_combatIcons[(COMBAT_ICON_TOWER)] = gpResourceManager->GetIcon(gText);
        m_combatIcons[(COMBAT_ICON_KEEP)] = gpResourceManager->GetIcon("keep.icn");
    }

    for (index = 0; index < COMBAT_SIDE_COUNT; index++) {
        m_heroIcons[index] = NULL;
        m_heroOverlayIcons[index] = NULL;
        m_heroAnimationState[index] = 0;
        m_heroAnimationFrame[index] = 0;
        m_heroSpriteIndex[index] = -1;

        if (m_heroes[index]) {
            if (m_heroes[index]->m_isCaptain) {
                sprintf(gText, "cmbtcap%c.icn", cHeroTypeInitial[(m_heroes[index]->m_cursorType)]);
                m_heroIcons[index] = gpResourceManager->GetIcon(gText);
                m_heroSpriteIndex[index] =
                    (m_heroes[index]->m_cursorType) + COMBAT_CAPTAIN_SPRITE_OFFSET;
            } else {
                sprintf(gText, "cmbthro%c.icn", cHeroTypeInitial[(m_heroes[index]->m_cursorType)]);
                m_heroIcons[index] = gpResourceManager->GetIcon(gText);
                m_heroSpriteIndex[index] = (m_heroes[index]->m_cursorType);
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

void combatManager::LoadArmies(void) {
    i32 groupSlot;
    CombatSide side;
    i32 hex;

    m_armyCount[(COMBAT_ATTACKER_SIDE)] = m_armyCount[(COMBAT_DEFENDER_SIDE)] = 0;

    for (groupSlot = 0; groupSlot < COMBAT_ARMY_CAPACITY; groupSlot++) {
        for (side = COMBAT_ATTACKER_SIDE; (side) < COMBAT_SIDE_COUNT; side++) {
            m_armies[(side)][groupSlot].m_quantity = 0;
            m_armies[(side)][groupSlot].m_monsterType = CREATURE_NONE;
        }
    }

    for (side = COMBAT_ATTACKER_SIDE; (side) < COMBAT_SIDE_COUNT; side++) {
        for (groupSlot = 0; groupSlot < COMBAT_ARMY_CAPACITY; groupSlot++)
            m_armies[(side)][groupSlot].InitClean();
    }

    for (groupSlot = 0; groupSlot < ARMY_GROUP_SLOT_COUNT; groupSlot++) {
        if (m_armyGroups[(COMBAT_ATTACKER_SIDE)]->m_creatureTypes[groupSlot]
            != CREATURE_NONE) {
            if (m_heroes[(COMBAT_ATTACKER_SIDE)]
                && (((m_heroes[(COMBAT_ATTACKER_SIDE)]->m_eventFlags) & (HERO_EVENT_GROUPED_FORMATION))))
                hex = COMBAT_GROUPED_HEX_STEP * groupSlot + COMBAT_ATTACKER_GROUPED_HEX;
            else
                hex = COMBAT_GRID_ROW_LENGTH * groupSlot * 2 + COMBAT_ATTACKER_SPREAD_HEX;

            m_armies[(COMBAT_ATTACKER_SIDE)][m_armyCount[(COMBAT_ATTACKER_SIDE)]].Init(
                m_armyGroups[(COMBAT_ATTACKER_SIDE)]->m_creatureTypes[groupSlot],
                m_armyGroups[(COMBAT_ATTACKER_SIDE)]->m_creatureCounts[groupSlot],
                COMBAT_ATTACKER_SIDE,
                m_armyCount[(COMBAT_ATTACKER_SIDE)],
                hex,
                groupSlot
            );
            m_armies[(COMBAT_ATTACKER_SIDE)][m_armyCount[(COMBAT_ATTACKER_SIDE)]].LoadResources();
            m_armyCount[(COMBAT_ATTACKER_SIDE)]++;
        }

        if (m_armyGroups[(COMBAT_DEFENDER_SIDE)]->m_creatureTypes[groupSlot]
            != CREATURE_NONE) {
            if ((m_heroes[(COMBAT_DEFENDER_SIDE)]
                 && (((m_heroes[(COMBAT_DEFENDER_SIDE)]->m_eventFlags) & (HERO_EVENT_GROUPED_FORMATION))))
                || (m_combatTowns[(COMBAT_DEFENDER_SIDE)]
                    && m_combatTowns[(COMBAT_DEFENDER_SIDE)]->m_formation))
                hex = COMBAT_GROUPED_HEX_STEP * groupSlot + COMBAT_DEFENDER_GROUPED_HEX;
            else
                hex = COMBAT_GRID_ROW_LENGTH * groupSlot * 2 + COMBAT_DEFENDER_SPREAD_HEX;

            m_armies[(COMBAT_DEFENDER_SIDE)][m_armyCount[(COMBAT_DEFENDER_SIDE)]].Init(
                m_armyGroups[(COMBAT_DEFENDER_SIDE)]->m_creatureTypes[groupSlot],
                m_armyGroups[(COMBAT_DEFENDER_SIDE)]->m_creatureCounts[groupSlot],
                COMBAT_DEFENDER_SIDE,
                m_armyCount[(COMBAT_DEFENDER_SIDE)],
                hex,
                groupSlot
            );
            m_armies[(COMBAT_DEFENDER_SIDE)][m_armyCount[(COMBAT_DEFENDER_SIDE)]].LoadResources();
            m_armyCount[(COMBAT_DEFENDER_SIDE)]++;
        }
    }
}

void combatManager::FreeArmies(void) {
    i32 index;
    gpSoundManager->StopAllSamples(1);

    for (index = 0; index < m_armyCount[(COMBAT_ATTACKER_SIDE)]; index++)
        m_armies[(COMBAT_ATTACKER_SIDE)][index].FreeResources();
    for (index = 0; index < m_armyCount[(COMBAT_DEFENDER_SIDE)]; index++)
        m_armies[(COMBAT_DEFENDER_SIDE)][index].FreeResources();

    if (gCurLoadedSpellIcon)
        gpResourceManager->Dispose(gCurLoadedSpellIcon);
    gCurLoadedSpellIcon = NULL;
    gCurLoadedSpellEffect = COMBAT_EFFECT_INVALID;
}

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

void combatManager::CheckApplyGoodMorale(CombatSide side, i32 index) {
    if (side < COMBAT_ATTACKER_SIDE || index < 0)
        return;
    if (bInHighMoraleBonus) {
        bInHighMoraleBonus = 0;
        return;
    }
    bInHighMoraleBonus = 0;

    army* activeArmy = &m_armies[(side)][index];
    if ((((activeArmy->m_monster.flags.all) & (MONSTER_FLAGS_NO_MORALE))))
        return;
    if (activeArmy->m_quantity == 0)
        return;
    if (activeArmy->m_morale <= 0
        || SRandom(MORALE_ROLL_MIN, GOOD_MORALE_ROLL_MAX) > activeArmy->m_morale)
        return;

    bInHighMoraleBonus = 1;
    SAMPLE2 moraleSample = NULL;
    if (!gbNoShowCombat) {
        sprintf(gText, "goodmrle.82M");
        moraleSample = LoadPlaySample(gText);
        if (activeArmy->m_quantity <= 1)
            sprintf(
                gText,
                "Высокая мораль %s позволила \n"
                "им еще раз атаковать врага.",
                gArmyNames[(activeArmy->m_monsterType)]
            );
        else
            sprintf(
                gText,
                "Высокая мораль %s позволила \n"
                "им еще раз атаковать врага.",
                gArmyNamesPlural[(activeArmy->m_monsterType)]
            );
        CombatMessage(gText, 1, 1, 0);
    }

    activeArmy->SpellEffect(COMBAT_EFFECT_GOOD_MORALE, MORALE_EFFECT_DURATION, 0);
    if (((activeArmy->m_monster.flags.abilityFlags) & (MONSTER_ABILITY_FLAG_BAD_MORALE)))
        ((activeArmy->m_monster.flags.abilityFlags) &= ~(MONSTER_ABILITY_FLAG_BAD_MORALE));
    activeArmy->m_monster.flags.abilityFlags |= MONSTER_ABILITY_FLAG_HIGH_MORALE;

    if (!gbNoShowCombat)
        WaitEndSample(&moraleSample, -1);
}

i32 combatManager::CheckApplyBadMorale(
    CombatSide side, i32 index
) {
    if (side < COMBAT_ATTACKER_SIDE || index < 0)
        return 0;

    army* activeArmy = &m_armies[(side)][index];
    if ((((activeArmy->m_monster.flags.all) & (MONSTER_FLAGS_NO_MORALE))))
        return 0;
    if (activeArmy->m_morale >= 0
        || SRandom(MORALE_ROLL_MIN, BAD_MORALE_ROLL_MAX) > -activeArmy->m_morale)
        return 0;
    if (!m_networkArmyPresent[(side)]
        && SRandom(MORALE_ROLL_MIN, BAD_MORALE_NETWORK_ROLL_MAX)
               == BAD_MORALE_NETWORK_SKIP_ROLL)
        return 0;

    SAMPLE2 moraleSample = NULL;
    if (!gbNoShowCombat) {
        moraleSample = LoadPlaySample("BADMRLE.82M");
        if (activeArmy->m_quantity <= 1)
            sprintf(
                gText,
                "Низкая мораль %s привела к тому, что \n"
                "они застыли в панике.",
                gArmyNames[(activeArmy->m_monsterType)]
            );
        else
            sprintf(
                gText,
                "Низкая мораль %s привела к тому, что \n"
                "они застыли в панике.",
                gArmyNamesPlural[(activeArmy->m_monsterType)]
            );
        CombatMessage(gText, 1, 1, 0);
    }

    activeArmy->SpellEffect(COMBAT_EFFECT_BAD_MORALE, MORALE_EFFECT_DURATION, 1);
    activeArmy->m_monster.flags.abilityFlags |= MONSTER_ABILITY_FLAG_BAD_MORALE;
    if (!gbNoShowCombat)
        WaitEndSample(&moraleSample, -1);
    return 1;
}

i32 combatManager::GetNextArmy(i32 checkMorale) {
    i32 skipEnt;
    i32 speedIter;
    i32 hasPending;
    i32 sideLoop;
    i32 stackCounter;
    army* curArmy;
    CombatSide stackSide;
    i32 i;

restart:
    hasPending = 0;
    stackSide = m_currentArmySide;
    m_currentSpeed = COMBAT_MAX_SPEED;
    for (speedIter = 0; speedIter < COMBAT_SPEED_LEVEL_COUNT; speedIter++) {
        for (sideLoop = 0; sideLoop < COMBAT_SIDE_COUNT; sideLoop++) {
            stackSide ^= 1;
            for (stackCounter = 0; stackCounter < m_armyCount[(stackSide)]; stackCounter++) {
                skipEnt = 0;
                curArmy = stackCounter + m_armies[(stackSide)];
                if ((((curArmy->m_monster.flags.abilityFlags) & (MONSTER_ABILITY_FLAG_AI_EXCLUDED | MONSTER_ABILITY_FLAG_BAD_MORALE)))
                    || (curArmy->m_spellInfluence[(ARMY_SPELL_INFLUENCE_PARALYZE)])
                    || curArmy->m_spellInfluence[(ARMY_SPELL_INFLUENCE_PETRIFIED)]
                    || curArmy->m_spellInfluence[(ARMY_SPELL_INFLUENCE_BLIND)]
                    || (curArmy->m_monster.speed != m_currentSpeed
                        && !(
                            curArmy->m_monster.flags.abilityFlags
                            & MONSTER_ABILITY_FLAG_HIGH_MORALE
                        )))
                    skipEnt = 1;

                if (!skipEnt && speedIter == 0
                    && !(
                        curArmy->m_monster.flags.abilityFlags & MONSTER_ABILITY_FLAG_HIGH_MORALE
                    ))
                    skipEnt = 1;

                if (((curArmy->m_monster.flags.abilityFlags) & (MONSTER_ABILITY_FLAG_DEFERRED_TURN))) {
                    skipEnt = 1;
                    hasPending = 1;
                }

                if (!skipEnt && checkMorale && CheckApplyBadMorale(stackSide, stackCounter))
                    skipEnt = 1;
                if (!skipEnt)
                    break;
            }

            if (stackCounter != m_armyCount[(stackSide)]) {
                m_currentArmySide = stackSide;
                m_currentArmyIndex = stackCounter;
                if ((stackCounter + m_armies[(stackSide)])
                        ->m_spellInfluence[(ARMY_SPELL_INFLUENCE_HYPNOTIZE)])
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

i32 combatManager::IsWinner(CombatSide side) {
    i32 result;
    i32 index;

    if (m_sideDefeated[(COMBAT_DEFENDER_SIDE) - (side)])
        return 1;
    if (m_sideRetreated[(COMBAT_DEFENDER_SIDE) - (side)])
        return 1;

    side ^= 1;
    result = 1;
    for (index = 0; index < m_armyCount[(side)]; index++) {
        if (!(m_armies[(side)][index].m_monster.flags.abilityFlags
              & MONSTER_ABILITY_FLAG_AI_EXCLUDED))
            result = 0;
    }
    return result;
}

void combatManager::CatAttack(CombatSide side) {
    if (!m_inCastleCombat)
        return;

    LogStr("CA1");
    if (m_smallViewSide[(COMBAT_ATTACKER_SIDE)] != COMBAT_SIDE_NONE
        || m_smallViewSide[(COMBAT_DEFENDER_SIDE)] != COMBAT_SIDE_NONE) {
        m_smallViewSide[(COMBAT_ATTACKER_SIDE)] = m_smallViewSide[(COMBAT_DEFENDER_SIDE)] = COMBAT_SIDE_NONE;
        DrawSmallView((COMBAT_ATTACKER_SIDE), 1);
        DrawSmallView((COMBAT_DEFENDER_SIDE), 1);
    }

    i32 random2 = SRandom(COMBAT_CATAPULT_TARGET_ROLL_MIN, COMBAT_CATAPULT_TARGET_ROLL_MAX);
    i32 wallCount14 = 0;
    i32 towerCount8 = 0;
    i32 wallIndex = -1;
    CombatCastleWallSlot towerIndex6 = COMBAT_WALL_SLOT_NONE;
    i32 gateIndex11 = -1;
    i32 keepIndex13 = -1;
    CombatCastleHex targetHex4 = COMBAT_CASTLE_HEX_NONE;
    i32 missShot19 = 0;
    CombatCatapultDamage damageLevel15 = CATAPULT_DAMAGE_NORMAL;
    i32 firstRoll7;
    i32 advancedRoll;
    i32 index26;

    if (m_wallStates[(COMBAT_WALL_SLOT_SECTION_FIRST)] != COMBAT_WALL_STATE_DESTROYED
        && m_wallStates[(COMBAT_WALL_SLOT_SECTION_FIRST)]
               != COMBAT_WALL_STATE_SECTION_DESTROYED)
        wallCount14++;
    if (m_wallStates[(COMBAT_WALL_SLOT_SECTION_SECOND)] != COMBAT_WALL_STATE_DESTROYED
        && m_wallStates[(COMBAT_WALL_SLOT_SECTION_SECOND)]
               != COMBAT_WALL_STATE_SECTION_DESTROYED)
        wallCount14++;
    if (m_wallStates[(COMBAT_WALL_SLOT_SECTION_THIRD)] != COMBAT_WALL_STATE_DESTROYED
        && m_wallStates[(COMBAT_WALL_SLOT_SECTION_THIRD)]
               != COMBAT_WALL_STATE_SECTION_DESTROYED)
        wallCount14++;
    if (m_wallStates[(COMBAT_WALL_SLOT_SECTION_FOURTH)] != COMBAT_WALL_STATE_DESTROYED
        && m_wallStates[(COMBAT_WALL_SLOT_SECTION_FOURTH)]
               != COMBAT_WALL_STATE_SECTION_DESTROYED)
        wallCount14++;
    if (m_wallStates[(COMBAT_WALL_SLOT_TOP_TOWER)] == COMBAT_WALL_STATE_TOWER_STANDING)
        towerCount8++;
    if (m_wallStates[(COMBAT_WALL_SLOT_BOTTOM_TOWER)] == COMBAT_WALL_STATE_TOWER_STANDING)
        towerCount8++;

    if (wallCount14 != 0) {
        random2 = random2 % wallCount14 + 1;
        wallIndex = -1;
        while (random2 != 0) {
            wallIndex++;
            if (m_wallStates[wallIndex + (COMBAT_WALL_SLOT_SECTION_FIRST)]
                    != COMBAT_WALL_STATE_DESTROYED
                && m_wallStates[wallIndex + (COMBAT_WALL_SLOT_SECTION_FIRST)]
                       != COMBAT_WALL_STATE_SECTION_DESTROYED)
                random2--;
        }
    } else if (towerCount8 != 0) {
        random2 %= towerCount8;
        if (random2 == 1
            || m_wallStates[(COMBAT_WALL_SLOT_TOP_TOWER)] == COMBAT_WALL_STATE_DESTROYED)
            towerIndex6 = COMBAT_WALL_SLOT_BOTTOM_TOWER;
        else
            towerIndex6 = COMBAT_WALL_SLOT_TOP_TOWER;
    } else if (m_drawbridgeState != COMBAT_CASTLE_GATE_HIDDEN) {
        gateIndex11 = 1;
    } else {
        if (m_wallStates[(COMBAT_WALL_SLOT_KEEP)] == COMBAT_WALL_STATE_KEEP_STANDING)
            keepIndex13 = 0;
    }

    if (towerIndex6 == COMBAT_WALL_SLOT_NONE && wallIndex == -1 && gateIndex11 == -1
        && keepIndex13 == -1)
        return;

    sample* loadedSample18 = NULL;
    sprintf(gText, "catsnd%02d.82M", COMBAT_CATAPULT_IMPACT_SOUND);
    loadedSample18 = gpResourceManager->GetSample(gText);
    SAMPLE2 catapultSound = NULL;
    SAMPLE2 impactSound19 = NULL;
    icon* boulder3 = gpResourceManager->GetIcon("boulder.icn");
    sprintf(gText, "catsnd%02d.82M", COMBAT_CATAPULT_LAUNCH_SOUND);
    catapultSound = LoadPlaySample(gText);

    i32 impactX10 = -1;
    i32 impactY6 = -1;
    if (wallIndex != -1) {
        impactX10 = wallPos[wallIndex][(COORDINATE_AXIS_X)];
        impactY6 = wallPos[wallIndex][(COORDINATE_AXIS_Y)];
        targetHex4 = CombatCastleHex(iWallToHexCell[wallIndex]);
    }
    if (towerIndex6 != COMBAT_WALL_SLOT_NONE) {
        impactX10 = towerPos[(towerIndex6)][(COORDINATE_AXIS_X)];
        impactY6 = towerPos[(towerIndex6)][(COORDINATE_AXIS_Y)];
        targetHex4 = CombatCastleHex(iTowerToHexCell[(towerIndex6)]);
    }
    if (gateIndex11 != -1) {
        impactX10 = doorPos[0][(COORDINATE_AXIS_X)];
        impactY6 = doorPos[0][(COORDINATE_AXIS_Y)];
        targetHex4 = COMBAT_CASTLE_HEX_GATE;
    }
    if (keepIndex13 != -1) {
        impactX10 = COMBAT_CATAPULT_KEEP_IMPACT_X;
        impactY6 = COMBAT_CATAPULT_KEEP_IMPACT_Y;
        targetHex4 = COMBAT_CASTLE_HEX_KEEP;
    }

    i32 startX8;
    float bounceY7;
    i32 frame;

    if (m_heroes[(COMBAT_ATTACKER_SIDE)]->m_secondarySkills[(HERO_SKILL_BALLISTICS)]
        == HERO_SKILL_LEVEL_NONE) {
        firstRoll7 =
            SRandom(COMBAT_CATAPULT_BALLISTICS_ROLL_MIN, COMBAT_CATAPULT_BALLISTICS_ROLL_MAX);
        if (!gbHumanPlayer[m_heroes[(COMBAT_ATTACKER_SIDE)]->m_owner])
            firstRoll7 -= COMBAT_CATAPULT_AI_ROLL_PENALTY;
        if (firstRoll7 < COMBAT_CATAPULT_NO_SKILL_DOUBLE_THRESHOLD)
            damageLevel15 = CATAPULT_DAMAGE_DOUBLE;
        else if (firstRoll7 > COMBAT_CATAPULT_NO_SKILL_MISS_THRESHOLD) {
            missShot19 = 1;
            damageLevel15 = CATAPULT_DAMAGE_NONE;
        }
    } else if (m_heroes[(COMBAT_ATTACKER_SIDE)]->m_secondarySkills[(HERO_SKILL_BALLISTICS)]
               <= HERO_SKILL_LEVEL_ADVANCED) {
        advancedRoll =
            SRandom(COMBAT_CATAPULT_BALLISTICS_ROLL_MIN, COMBAT_CATAPULT_BALLISTICS_ROLL_MAX);
        if (!gbHumanPlayer[m_heroes[(COMBAT_ATTACKER_SIDE)]->m_owner])
            advancedRoll -= COMBAT_CATAPULT_AI_ROLL_PENALTY;
        if (advancedRoll < COMBAT_CATAPULT_ADVANCED_DOUBLE_THRESHOLD)
            damageLevel15 = CATAPULT_DAMAGE_DOUBLE;
    } else if (m_heroes[(COMBAT_ATTACKER_SIDE)]->m_secondarySkills[(HERO_SKILL_BALLISTICS)]
               == HERO_SKILL_LEVEL_EXPERT) {
        damageLevel15 = CATAPULT_DAMAGE_DOUBLE;
    }

    if (missShot19) {
        u8 validMissHexes[COMBAT_CATAPULT_MISS_HEX_COUNT] =
            {32, 46, 10, 23, 35, 48, 60, 74, 87, 101, 114};
        i32 startDirection3 =
            SRandom(COMBAT_CATAPULT_DIRECTION_ROLL_MIN, COMBAT_CATAPULT_DIRECTION_ROLL_MAX);
        i32 adjacentHex;
        for (frame = 0; frame < COMBAT_CATAPULT_DIRECTION_COUNT; frame++) {
            adjacentHex = GetAdjacentCellIndexNoArmy(
                (targetHex4),
                static_cast<CombatHexDirection>(
                    (startDirection3 + frame) % COMBAT_CATAPULT_DIRECTION_COUNT
                )
            );
            if (adjacentHex != -1 && m_hexCells[adjacentHex].m_occupantSide == COMBAT_SIDE_NONE) {
                for (index26 = 0; index26 < COMBAT_CATAPULT_MISS_HEX_COUNT; index26++) {
                    if (adjacentHex == validMissHexes[index26]) {
                        impactX10 = m_hexCells[adjacentHex].m_x;
                        impactY6 = m_hexCells[adjacentHex].m_y - COMBAT_CATAPULT_MISS_Y_OFFSET;
                        goto foundMissHex;
                    }
                }
            }
        }
        missShot19 = 0;
        damageLevel15 = CATAPULT_DAMAGE_NORMAL;
    foundMissHex:
        frame++;
    }

    giMinExtentX = 0;
    giMaxExtentX = COMBAT_CATAPULT_INITIAL_MAX_X;
    giMinExtentY = COMBAT_CATAPULT_INITIAL_MIN_Y;
    giMaxExtentY = COMBAT_CATAPULT_INITIAL_MAX_Y;
    m_catapultFrame[(side)] = 0;
    while (m_catapultFrame[(side)] < COMBAT_CATAPULT_LOAD_FRAME_COUNT) {
        DrawFrame(1, 0, 1, 0, COMBAT_CATAPULT_ANIMATION_DELAY, 1, 1);
        m_catapultFrame[(side)]++;
    }

    startX8 = COMBAT_CATAPULT_START_X;
    i32 startY9 = COMBAT_CATAPULT_START_Y;
    i32 spriteFrame16 = 0;
    float projectileX11 = static_cast<float>(startX8);
    float projectileY4 = static_cast<float>(startY9);
    float midpointX6 = static_cast<float>(
        (startX8 + impactX10) / COMBAT_CATAPULT_MIDPOINT_DIVISOR
    );
    bounceY7 = static_cast<float>(
        (impactY6 - (impactX10 - startX8) * COMBAT_CATAPULT_DISTANCE_ARC_SCALE)
        - impactY6 * COMBAT_CATAPULT_TARGET_ARC_SCALE
    );
    float xStep0 =
        static_cast<float>((midpointX6 - startX8) / COMBAT_CATAPULT_HORIZONTAL_STEP_DIVISOR);
    float yStep = (bounceY7 - startY9) / COMBAT_CATAPULT_VERTICAL_STEP_DIVISOR;
    i32 previousX7 = -1;
    i32 previousY4 = -1;
    SLimitData limits9;
    for (frame = 0; frame < COMBAT_CATAPULT_PROJECTILE_FRAME_COUNT; frame++) {
        if (frame == COMBAT_CATAPULT_DESCENT_FRAME)
            yStep = (bounceY7 - impactY6) / COMBAT_CATAPULT_VERTICAL_STEP_DIVISOR;
        if (frame != 0) {
            giMinExtentX = previousX7 - COMBAT_CATAPULT_PROJECTILE_EXTENT_RADIUS;
            giMaxExtentX =
                static_cast<i32l>(
                    projectileX11
                    + (COMBAT_CATAPULT_PROJECTILE_EXTENT_RADIUS)

                );
            giMinExtentY =
                static_cast<i32l>(
                    projectileY4 - (COMBAT_CATAPULT_PROJECTILE_EXTENT_RADIUS)
                );
            giMaxExtentY =
                static_cast<i32l>(
                    projectileY4 + (COMBAT_CATAPULT_PROJECTILE_EXTENT_RADIUS)
                );
            if (previousY4 < projectileY4)
                giMinExtentY = previousY4 - COMBAT_CATAPULT_PROJECTILE_EXTENT_RADIUS;
            else
                giMaxExtentY = previousY4 + COMBAT_CATAPULT_PROJECTILE_EXTENT_RADIUS;
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
        boulder3->CombatClipDrawToBuffer(
            static_cast<i32l>(projectileX11),
            static_cast<i32l>(projectileY4),
            spriteFrame16,
            &limits9,
            ICON_DRAW_NORMAL,
            0,
            NULL,
            NULL
        );
        gpWindowManager->UpdateScreenRegion(
            giMinExtentX,
            giMinExtentY,
            giMaxExtentX - giMinExtentX + 1,
            giMaxExtentY - giMinExtentY + 1
        );
        previousX7 = static_cast<i32l>(projectileX11);
        previousY4 = static_cast<i32l>(projectileY4);
        projectileX11 += xStep0;
        projectileY4 = (COMBAT_CATAPULT_DESCENT_FRAME - frame) * yStep + projectileY4;
        spriteFrame16++;
        spriteFrame16 %= COMBAT_CATAPULT_BOULDER_FRAME_COUNT;
        if (frame < COMBAT_CATAPULT_ADVANCE_FRAME_COUNT)
            m_catapultFrame[(side)]++;
    }

    sprintf(gText, "catsnd%02d.82M", COMBAT_CATAPULT_IMPACT_SOUND);
    impactSound19 = LoadPlaySample(gText);
    m_unknownF34B = 0;
    m_unknownF34F = 0;

    giMinExtentX = impactX10 - COMBAT_CATAPULT_IMPACT_EXTENT_X;
    giMaxExtentX = impactX10 + COMBAT_CATAPULT_IMPACT_EXTENT_X;
    giMinExtentY = impactY6 - COMBAT_CATAPULT_IMPACT_EXTENT_TOP;
    giMaxExtentY = impactY6 + COMBAT_CATAPULT_IMPACT_EXTENT_BOTTOM;
    if (gateIndex11 != -1)
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
        gpResourceManager->GetIcon(const_cast<char*>(missShot19 ? "smalclod.icn" : "lichclod.icn"));

    for (frame = 0; frame < COMBAT_CATAPULT_CLOUD_FRAME_COUNT; frame++) {
        if (frame >= COMBAT_CATAPULT_CLOUD_VISIBLE_FRAME_COUNT
            && (gateIndex11 != -1 || missShot19 != 0))
            continue;
        glTimers[COMBAT_CATAPULT_TIMER_SLOT] = static_cast<i32l>(
            KBTickCount()
            + (COMBAT_CATAPULT_ANIMATION_DELAY) * gfCombatSpeedMod[gConfig.combatSpeed]
        );
        DrawFrame(0, 0, 1, 0, 0, 1, 0);
        if (gateIndex11 != -1 && frame >= COMBAT_CATAPULT_GATE_CLOUD_FIRST_FRAME && !missShot19) {
            IconToBitmap(
                cloud,
                gpWindowManager->m_screen,
                impactX10 - COMBAT_CATAPULT_GATE_CLOUD_X_OFFSET,
                impactY6 + COMBAT_CATAPULT_GATE_CLOUD_Y_OFFSET,
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
                impactX10,
                impactY6 + (!missShot19 ? COMBAT_CATAPULT_CLOUD_Y_OFFSET : 0),
                frame,
                ICON_DRAW_CLIP,
                0,
                0,
                COMBAT_SCREEN_WIDTH,
                COMBAT_AREA_HEIGHT,
                0
            );
        }
        gpWindowManager->UpdateScreenRegion(
            giMinExtentX,
            giMinExtentY,
            giMaxExtentX - giMinExtentX + 1,
            giMaxExtentY - giMinExtentY + 1
        );
        DelayTil(&glTimers[COMBAT_CATAPULT_TIMER_SLOT]);

        if (((frame == COMBAT_CATAPULT_WALL_IMPACT_FRAME && gateIndex11 == -1)
             || (frame == COMBAT_CATAPULT_GATE_IMPACT_FRAME && gateIndex11 != -1))
            && !missShot19) {
            if (wallIndex != -1) {
                if (m_wallStates[wallIndex + (COMBAT_WALL_SLOT_SECTION_FIRST)]
                        >= COMBAT_WALL_STATE_SECTION_DAMAGE_FIRST
                    && m_wallStates[wallIndex + (COMBAT_WALL_SLOT_SECTION_FIRST)]
                           <= COMBAT_WALL_STATE_SECTION_DAMAGE_LAST) {
                    m_wallStates[wallIndex + (COMBAT_WALL_SLOT_SECTION_FIRST)] +=
                        (damageLevel15);
                    if (m_wallStates[wallIndex + (COMBAT_WALL_SLOT_SECTION_FIRST)]
                        > COMBAT_WALL_STATE_SECTION_DAMAGE_LAST)
                        m_wallStates[wallIndex + (COMBAT_WALL_SLOT_SECTION_FIRST)] =
                            COMBAT_WALL_STATE_SECTION_DESTROYED;
                } else {
                    if (damageLevel15 > CATAPULT_DAMAGE_NORMAL)
                        m_wallStates[wallIndex + (COMBAT_WALL_SLOT_SECTION_FIRST)] =
                            COMBAT_WALL_STATE_DESTROYED;
                    else
                        m_wallStates[wallIndex + (COMBAT_WALL_SLOT_SECTION_FIRST)]++;
                }
                if (m_wallStates[wallIndex + (COMBAT_WALL_SLOT_SECTION_FIRST)]
                        == COMBAT_WALL_STATE_DESTROYED
                    || m_wallStates[wallIndex + (COMBAT_WALL_SLOT_SECTION_FIRST)]
                           == COMBAT_WALL_STATE_SECTION_DESTROYED)
                    m_hexCells[iWallToHexCell[wallIndex]].m_blocked = 0;
            } else if (towerIndex6 != COMBAT_WALL_SLOT_NONE) {
                m_wallStates[(towerIndex6)] = COMBAT_WALL_STATE_DESTROYED;
            } else if (gateIndex11 != -1) {
                m_drawbridgeState = COMBAT_CASTLE_GATE_HIDDEN;
            } else if (keepIndex13 != -1) {
                m_wallStates[(COMBAT_WALL_SLOT_KEEP)] = COMBAT_WALL_STATE_KEEP_DESTROYED;
            }
        }
    }

    gpResourceManager->Dispose(cloud);
    m_catapultFrame[(side)] = 0;
    DrawFrame(1, 0, 0, 0, COMBAT_CATAPULT_ANIMATION_DELAY, 1, 1);
    gpResourceManager->Dispose(boulder3);
    WaitEndSample(&impactSound19, -1);
    WaitEndSample(&catapultSound, -1);
    if (loadedSample18)
        gpResourceManager->Dispose(loadedSample18);
    LogStr("CA2");
}

void combatManager::KeepAttack(CombatTowerSelector tower) {
    if (!m_inCastleCombat)
        return;
    if ((tower == COMBAT_TOWER_GARRISON
         && m_wallStates[(COMBAT_WALL_SLOT_KEEP)] != COMBAT_WALL_STATE_KEEP_STANDING)
        || (tower == COMBAT_TOWER_TOP
            && m_wallStates[(COMBAT_WALL_SLOT_TOP_TOWER)]
                   != COMBAT_WALL_STATE_TOWER_STANDING)
        || (tower == COMBAT_TOWER_BOTTOM
            && m_wallStates[(COMBAT_WALL_SLOT_BOTTOM_TOWER)]
                   != COMBAT_WALL_STATE_TOWER_STANDING))
        return;

    LogStr("KA1");

    CombatKeepTargetPriority bestPriority = KEEP_PRIORITY_NONE;
    i32 bestValue5 = 0;
    i32 bestArmyIndex = -1;
    i32 armyIndex;
    army* target9;
    i32 value9;
    CombatKeepTargetPriority priority7;
    for (armyIndex = 0; armyIndex < COMBAT_ARMY_CAPACITY; armyIndex++) {
        if (m_armies[(COMBAT_ATTACKER_SIDE)][armyIndex].IsAlive()) {
            target9 = &m_armies[(COMBAT_ATTACKER_SIDE)][armyIndex];
            if (target9->m_spellInfluence[(ARMY_SPELL_INFLUENCE_BLIND)]
                || target9->m_spellInfluence[(ARMY_SPELL_INFLUENCE_PARALYZE)]
                || target9->m_spellInfluence[(ARMY_SPELL_INFLUENCE_PETRIFIED)]
                || target9->m_spellInfluence[(ARMY_SPELL_INFLUENCE_BERSERK)]
                || target9->m_spellInfluence[(ARMY_SPELL_INFLUENCE_HYPNOTIZE)]) {
                priority7 = KEEP_PRIORITY_DISABLED;
            } else if ((((target9->m_monster.flags.all) & (MONSTER_FLAGS_SHOOTER)))) {
                priority7 = KEEP_PRIORITY_SHOOTER;
            } else if ((((target9->m_monster.flags.all) & (MONSTER_FLAGS_FLYING)))) {
                priority7 = KEEP_PRIORITY_FLYER;
            } else {
                priority7 = KEEP_PRIORITY_WALKER;
            }

            value9 =
                target9->m_quantity * gMonsterDatabase[(target9->m_monsterType)].fightValue;
            if (priority7 > bestPriority
                || (priority7 == bestPriority && value9 > bestValue5)) {
                bestValue5 = value9;
                bestPriority = priority7;
                bestArmyIndex = armyIndex;
            }
        }
    }

    if (bestArmyIndex == -1)
        return;

    target9 = &gpCombatManager->m_armies[(COMBAT_ATTACKER_SIDE)][bestArmyIndex];
    sprintf(gText, "keepshot.82M");
    SAMPLE2 keepSample7 = LoadPlaySample(gText);

    CombatTowerOrigin towerOrigins7[COMBAT_KEEP_FACTION_COUNT][COMBAT_KEEP_TOWER_COUNT] = {
        {{586, 177}, {428, 60}, {428, 314}},
        {{586, 177}, {428, 60}, {428, 314}},
        {{586, 177}, {428, 60}, {428, 314}},
        {{586, 177}, {428, 60}, {428, 314}},
        {{586, 177}, {428, 60}, {428, 314}},
        {{586, 177}, {428, 60}, {428, 314}}
    };
    i32 unknownTowerData1[KEEP_TOWER_SCRATCH_COUNT];
    i32 sourceX8 = towerOrigins7[(m_combatTowns[(COMBAT_DEFENDER_SIDE)]->m_type)][(tower)].x;
    i32 sourceY8 = towerOrigins7[(m_combatTowns[(COMBAT_DEFENDER_SIDE)]->m_type)][(tower)].y;
    i32 targetX6 = target9->MidX();
    i32 targetY1 = target9->MidY();
    float missileAngles1[COMBAT_KEEP_MISSILE_ANGLE_COUNT] =
        {90.0f, 68.5f, 45.0f, 20.8f, 0.0f, -20.8f, -45.0f, -68.5f, -90.0f};
    ShootMissile(
        sourceX8,
        sourceY8,
        targetX6,
        targetY1,
        missileAngles1,
        m_combatIcons[(COMBAT_ICON_KEEP)]
    );

    i32 shotCount0;
    i32 attackBonus4;
    m_combatTowns[(COMBAT_DEFENDER_SIDE)]->CalcNumLevelArchers(&shotCount0, &attackBonus4);
    attackBonus4 += COMBAT_KEEP_TOWER_DAMAGE_BONUS;
    if (m_heroes[(COMBAT_DEFENDER_SIDE)])
        attackBonus4 += m_heroes[(COMBAT_DEFENDER_SIDE)]->Stats(HERO_PRIMARY_ATTACK);
    attackBonus4 -= target9->m_monster.defense;
    if (attackBonus4 > COMBAT_KEEP_ATTACK_STAT_LIMIT)
        attackBonus4 = COMBAT_KEEP_ATTACK_STAT_LIMIT;
    if (attackBonus4 < -COMBAT_KEEP_ATTACK_STAT_LIMIT)
        attackBonus4 = -COMBAT_KEEP_ATTACK_STAT_LIMIT;
    if (tower != COMBAT_TOWER_GARRISON)
        shotCount0 /= COMBAT_KEEP_SIDE_TOWER_SHOT_DIVISOR;

    i32 damage5 = 0;
    for (armyIndex = 0; armyIndex < shotCount0; armyIndex++)
        damage5 += SRandom(COMBAT_KEEP_RANDOM_DAMAGE_MIN, COMBAT_KEEP_RANDOM_DAMAGE_MAX);
    damage5 = static_cast<i32l>(
        damage5 * gfBattleStat[attackBonus4 + COMBAT_KEEP_ATTACK_STAT_INDEX_OFFSET]
    );
    if (damage5 <= 0)
        damage5 = COMBAT_KEEP_MIN_DAMAGE;

    i32 killed0 = target9->Damage(damage5, SPELL_NONE);
    if (killed0 > 0) {
        sprintf(
            gText,
            "%s %d %s.\n%d %s %s.",
            tower == COMBAT_TOWER_GARRISON ? "Гарнизон наносит"
                                                : "Башня наносит",
            damage5,
            "ед. урона",
            killed0,
            killed0 <= 1 ? gArmyNames[(target9->m_monsterType)]
                         : gArmyNamesPlural[(target9->m_monsterType)],
            killed0 <= 1 ? "умирает" : "убито"
        );
    } else {
        sprintf(
            gText,
            "%s %d %s.",
            tower == COMBAT_TOWER_GARRISON ? "Гарнизон наносит"
                                                : "Башня наносит",
            damage5,
            "ед. урона"
        );
    }
    gpCombatManager->CombatMessage(gText, 1, 1, 0);
    target9->CancelSpellType(ARMY_CANCEL_SPELLS_AFTER_DAMAGE);
    target9->PowEffect(COMBAT_EFFECT_INVALID, 1, -1, -1);
    WaitEndSample(&keepSample7, -1);
}

i32 combatManager::ExperienceValueOfStack(CombatSide side) {
    i32 experienceValue6 = 0;
    i32 index;

    for (index = 0; index < COMBAT_ARMY_CAPACITY; index++) {
        if (m_armies[(side)][index].m_monsterType != CREATURE_NONE
            && !(((m_armies[(side)][index].m_monster.flags.all) & (MONSTER_FLAGS_SUMMONED)))) {
            experienceValue6 +=
                (m_armies[(side)][index].m_initialQuantity - m_armies[(side)][index].m_quantity)
                * gMonsterDatabase[(m_armies[(side)][index].m_monsterType)].hitPoints;
        }
    }
    if (m_heroes[(side)])
        experienceValue6 += COMBAT_HERO_EXPERIENCE_VALUE;
    return experienceValue6;
}

void combatManager::ResetHitByCreature(void) {
    CombatSide side;
    i32 index;

    for (side = COMBAT_ATTACKER_SIDE; (side) < COMBAT_SIDE_COUNT; side++) {
        for (index = 0; index < COMBAT_ARMY_CAPACITY; index++)
            m_armies[(side)][index].m_hitByCreature = false;
    }
}

i32 ValidHex(i32 hex) {
    return hex >= 0 && hex < COMBAT_HEX_COUNT ? 1 : 0;
}

void combatManager::SaveCombatBorder(void) {
    return;
}

void combatManager::DrawCombatBorder(void) {
    return;
}

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
    i32 blocked;
    i32 placedCells;
    i32 obstacleHex;

    m_debugFormation = 0;
    if (m_inCastleCombat) {
        m_wallStates[(COMBAT_WALL_SLOT_KEEP)] = COMBAT_WALL_STATE_KEEP_STANDING;
        for (cellIndex = 0; cellIndex < COMBAT_CASTLE_STRUCTURE_COUNT; cellIndex++) {
            m_wallStates[cellIndex + (COMBAT_WALL_SLOT_SECTION_FIRST)] =
                COMBAT_WALL_STATE_KEEP_STANDING;
            if (m_combatTowns[(COMBAT_DEFENDER_SIDE)]->m_type == FACTION_KNIGHT
                && (m_combatTowns[(COMBAT_DEFENDER_SIDE)]->m_buildings
                    & (TOWN_BUILDING_FORTIFICATIONS))) {
                m_wallStates[cellIndex + (COMBAT_WALL_SLOT_SECTION_FIRST)] =
                    COMBAT_WALL_STATE_SECTION_DAMAGE_FIRST;
            }
            m_wallStates[cellIndex] = COMBAT_WALL_STATE_KEEP_STANDING;
        }
        if (m_combatTowns[(COMBAT_DEFENDER_SIDE)]->m_buildings & (TOWN_BUILDING_LEFT_TURRET))
            m_wallStates[(COMBAT_WALL_SLOT_TOP_TOWER)] = COMBAT_WALL_STATE_TOWER_STANDING;
        if (m_combatTowns[(COMBAT_DEFENDER_SIDE)]->m_buildings & (TOWN_BUILDING_RIGHT_TURRET))
            m_wallStates[(COMBAT_WALL_SLOT_BOTTOM_TOWER)] =
                COMBAT_WALL_STATE_TOWER_STANDING;

        m_hexCells[(COMBAT_CASTLE_HEX_TOP_TOWER)].m_blocked = 1;
        m_hexCells[(COMBAT_CASTLE_HEX_TOP_WALL)].m_blocked = 1;
        m_hexCells[(COMBAT_CASTLE_HEX_SECOND_TOWER)].m_blocked = 1;
        m_hexCells[(COMBAT_CASTLE_HEX_SECOND_WALL)].m_blocked = 1;
        m_hexCells[(COMBAT_CASTLE_HEX_GATE)].m_blocked = 1;
        m_hexCells[(COMBAT_CASTLE_HEX_THIRD_WALL)].m_blocked = 1;
        m_hexCells[(COMBAT_CASTLE_HEX_THIRD_TOWER)].m_blocked = 1;
        m_hexCells[(COMBAT_CASTLE_HEX_BOTTOM_WALL)].m_blocked = 1;
        m_hexCells[(COMBAT_CASTLE_HEX_BOTTOM_TOWER)].m_blocked = 1;
        m_hexCells[(COMBAT_CASTLE_HEX_MOAT)].m_blocked = 1;
    } else {
        obstacleGoal = SRandom(COMBAT_RANDOM_OBSTACLE_MIN, COMBAT_RANDOM_OBSTACLE_MAX);
        placedCells = 0;
        groundMask = 1 << (m_terrainType);
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
            blocked = 0;
            for (cellIndex = 0; cellIndex < sCmbtObstacles[obstacleType].cellCount; cellIndex++) {
                obstacleHex = site + sCmbtObstacles[obstacleType].cellOffsets[cellIndex];
                if (obstacleHex % COMBAT_GRID_ROW_LENGTH <= COMBAT_OBSTACLE_LEFT_COLUMN_LIMIT - 1
                    || obstacleHex % COMBAT_GRID_ROW_LENGTH >= COMBAT_OBSTACLE_RIGHT_COLUMN_FIRST) {
                    blocked = 1;
                }
                if (m_hexCells[obstacleHex].m_blocked != 0)
                    blocked = 1;
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
                m_hexCells[removedArmy->m_hex].m_occupantSide = COMBAT_SIDE_NONE;
                m_hexCells[removedArmy->m_hex].m_occupantIndex = -1;
                if ((((removedArmy->m_monster.flags.all) & (MONSTER_FLAGS_WIDE)))) {
                    m_hexCells[removedArmy->m_hex
                               + ArmyFacingRearHexOffset(removedArmy->m_facing)]
                            .m_occupantSide = COMBAT_SIDE_NONE;
                    m_hexCells[removedArmy->m_hex
                               + ArmyFacingRearHexOffset(removedArmy->m_facing)]
                            .m_occupantIndex = -1;
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
    WaitEndSample(&drawbridgeSample, -1);
}

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
    WaitEndSample(&drawbridgeSample, -1);
}

void combatManager::TestRaiseDoor(void) {
    if (m_inCastleCombat && m_drawbridgeState == COMBAT_DRAWBRIDGE_LOWERED
        && m_hexCells[COMBAT_CASTLE_GATE_APPROACH_HEX].m_occupantSide == COMBAT_SIDE_NONE
        && m_hexCells[COMBAT_CASTLE_GATE_APPROACH_HEX].m_deadOccupantCount == 0
        && m_hexCells[(COMBAT_CASTLE_HEX_GATE)].m_occupantSide == COMBAT_SIDE_NONE
        && m_hexCells[(COMBAT_CASTLE_HEX_GATE)].m_deadOccupantCount == 0) {
        RaiseDoor();
    }
}

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

i32 combatManager::ShotIsThroughWall(
    CombatSide side, i32 sourceHex, i32 targetHex
) {
    float columnStride;
    i32 colTarget;
    float rowStride;
    i32 traceLength;
    i32 tgtLine;
    i32 traceSquare;
    float traceRow;
    float traceColumn;
    i32 structIndex;
    i32 traceIx;
    i32 columnDist;
    i32 rowSpan;
    i32 sourceColumn;
    i32 srcLine;

    if (!m_inCastleCombat)
        return 0;
    if (m_heroes[(side)]
        && (m_heroes[(side)]->HasArtifact(ARTIFACT_GOLDEN_BOW)
            || m_heroes[(side)]->m_secondarySkills[(HERO_SKILL_ARCHERY)]
                   != HERO_SKILL_LEVEL_NONE)) {
        return 0;
    }
    if (InCastle(sourceHex) || !InCastle(targetHex))
        return 0;

    sourceColumn = sourceHex % COMBAT_GRID_ROW_LENGTH;
    srcLine = sourceHex / COMBAT_GRID_ROW_LENGTH;
    colTarget = targetHex % COMBAT_GRID_ROW_LENGTH;
    tgtLine = targetHex / COMBAT_GRID_ROW_LENGTH;
    columnDist = colTarget - sourceColumn;
    rowSpan = tgtLine - srcLine;
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
    traceRow = static_cast<float>(srcLine);
    for (traceIx = 0; traceIx < traceLength * COMBAT_WALL_TRACE_SUBDIVISIONS;
         traceIx++) {
        traceColumn += columnStride;
        traceRow += rowStride;
        traceSquare =
            static_cast<i32>(traceRow) * COMBAT_GRID_ROW_LENGTH + static_cast<i32>(traceColumn);
        for (structIndex = 0; structIndex < COMBAT_CASTLE_STRUCTURE_COUNT;
             structIndex++) {
            if (traceSquare == iWallToHexCell[structIndex]
                && m_wallStates[structIndex + (COMBAT_WALL_SLOT_SECTION_FIRST)]
                       != COMBAT_WALL_STATE_DESTROYED
                && m_wallStates[structIndex + (COMBAT_WALL_SLOT_SECTION_FIRST)]
                       != COMBAT_WALL_STATE_SECTION_DESTROYED) {
                return 1;
            }
            if (traceSquare == iTowerToHexCell[structIndex]
                && m_wallStates[structIndex] != COMBAT_WALL_STATE_DESTROYED) {
                return 1;
            }
            if (traceSquare == (COMBAT_CASTLE_HEX_GATE)
                && m_drawbridgeState == COMBAT_DRAWBRIDGE_RAISED) {
                return 1;
            }
        }
    }
    return 0;
}

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
    i32 yLen;
    i8 reverseMissile;
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
    i32 absXLen;
    i32 missileHalfHeight;
    i32 maxX;
    i32 slopeDy;
    i32 maxY;

    xSize = targetX - sourceX;
    yLen = targetY - sourceY;
    absXLen = targetX - sourceX;
    reverseMissile = ICON_DRAW_NORMAL;
    if (absXLen < 0) {
        reverseMissile = ICON_DRAW_FLIPPED;
        absXLen = -absXLen;
    }
    slopeDy = targetY - sourceY;
    if (absXLen == 0) {
        angleFrame = slopeDy > 0 ? COMBAT_MISSILE_LAST_DIRECTION : 0;
    } else {
        float slope = static_cast<double>(-slopeDy) / (static_cast<double>(absXLen));
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

    total = static_cast<i32>(sqrt(static_cast<double>(xSize * xSize + yLen * yLen)));
    missileSteps = (total + COMBAT_MISSILE_SPACING_ROUND) / COMBAT_MISSILE_SPACING;
    if (missileSteps > 1) {
        incX = xSize / (missileSteps - 1);
        yStep = yLen / (missileSteps - 1);
    } else {
        incX = xSize;
        yStep = yLen;
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
            gpWindowManager->UpdateScreenRegion(
                giMinExtentX,
                giMinExtentY,
                giMaxExtentX - giMinExtentX + 1,
                giMaxExtentY - giMinExtentY + 1
            );
        } else {
            DelayTil(glTimers);
            gpWindowManager->UpdateScreenRegion(minimumX, minY, maxX - minimumX + 1, maxY - minY + 1);
        }
        glTimers[0] = static_cast<i32>(
            KBTickCount()
            + (COMBAT_MISSILE_TIMER_DELAY) * gfCombatSpeedMod[gConfig.combatSpeed]
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

void combatManager::CombatSystemOptions(void) {
    tag_message message;
    bCPrefsChanged = 0;
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

void UpdateCombatSystemOptions(i32 initialDraw) {
    tag_message message;
    message.type = COMBAT_SYSTEM_OPTION_EVENT;
    message.payload.widget.command = COMBAT_SYSTEM_OPTION_BUTTON_MESSAGE;
    message.payload.widget.id = SYSTEM_OPTION_SPEED_BUTTON;
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

MessageDispatchResult CombatSystemOptionsHandler(tag_message& message) {
    i32 bRedraw = 0;
    i32 bDone = 0;
    if (message.type == COMBAT_SYSTEM_OPTION_EVENT) {
        if ((((message.payload.widget.modifiers) & (MESSAGE_MODIFIER_RIGHT_BUTTON)))) {
            if (message.payload.widget.command == COMBAT_SYSTEM_OPTION_BUTTON_EVENT
                || message.payload.widget.command == COMBAT_SYSTEM_OPTION_HOVER_EVENT) {
                i32 helpIndex = -1;
                switch (message.payload.widget.id) {
                    case SYSTEM_OPTION_CLOSE_BUTTON:
                        helpIndex = (HELP_CLOSE);
                        break;
                    case SYSTEM_OPTION_SPEED_BUTTON:
                        helpIndex = (HELP_SPEED);
                        break;
                    case SYSTEM_OPTION_ARMY_INFO_BUTTON:
                        helpIndex = (HELP_ARMY_INFO);
                        break;
                    case SYSTEM_OPTION_AUTO_SPELL_BUTTON:
                        helpIndex = (HELP_AUTO_SPELL);
                        break;
                    case SYSTEM_OPTION_GRID_BUTTON:
                        helpIndex = (HELP_GRID);
                        break;
                    case SYSTEM_OPTION_SHADE_BUTTON:
                        helpIndex = (HELP_SHADE);
                        break;
                    case SYSTEM_OPTION_MOUSE_HEX_BUTTON:
                        helpIndex = (HELP_MOUSE_HEX);
                        break;
                }
                if (helpIndex >= 0) {
                    NormalDialog(
                        gCSPanelHelp[helpIndex],
                        SYSTEM_OPTION_HELP_DIALOG,
                        -1,
                        -1,
                        -1,
                        0,
                        -1,
                        0,
                        -1,
                        0
                    );
                }
            }
        } else {
            switch (message.payload.widget.command) {
                case COMBAT_SYSTEM_OPTION_CLOSE_EVENT:
                    switch (message.payload.widget.id) {
                        case SYSTEM_OPTION_CLOSE_BUTTON:
                            bDone = 1;
                            break;
                    }
                    break;
                case COMBAT_SYSTEM_OPTION_BUTTON_EVENT:
                    switch (message.payload.widget.id) {
                        case SYSTEM_OPTION_SPEED_BUTTON:
                            gConfig.combatSpeed =
                                (gConfig.combatSpeed + 1) % SYSTEM_OPTION_CYCLE_COUNT;
                            bRedraw = 1;
                            bCPrefsChanged = 1;
                            break;
                        case SYSTEM_OPTION_ARMY_INFO_BUTTON:
                            gConfig.combatArmyInfoLevel = (gConfig.combatArmyInfoLevel + 1)
                                                          % SYSTEM_OPTION_CYCLE_COUNT;
                            bRedraw = 1;
                            bCPrefsChanged = 1;
                            break;
                        case SYSTEM_OPTION_AUTO_SPELL_BUTTON:
                            gConfig.autoCombatUseSpells = 1 - gConfig.autoCombatUseSpells;
                            bRedraw = 1;
                            bCPrefsChanged = 1;
                            break;
                        case SYSTEM_OPTION_GRID_BUTTON:
                            gConfig.showCombatGrid = 1 - gConfig.showCombatGrid;
                            bRedraw = 1;
                            bCPrefsChanged = 1;
                            break;
                        case SYSTEM_OPTION_SHADE_BUTTON:
                            gConfig.combatShadeLevel = 1 - gConfig.combatShadeLevel;
                            bRedraw = 1;
                            bCPrefsChanged = 1;
                            break;
                        case SYSTEM_OPTION_MOUSE_HEX_BUTTON:
                            gConfig.showCombatMouseHex = 1 - gConfig.showCombatMouseHex;
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
        message.payload.widget.id = SYSTEM_OPTION_SPEED_BUTTON;
        message.payload.widget.command = BaseWidgetCommand(SYSTEM_OPTION_SPEED_BUTTON);
        return MESSAGE_DISPATCH_FORWARD;
    }
    return MESSAGE_DISPATCH_CONSUME;
}


i32 bInHighMoraleBonus = 0;
i32 giSeed = 1;
u8 wallHex[COMBAT_WALL_SECTION_COUNT] = {9, 34, 86, 113};
i32 bMouseWasVis;
class heroWindow* CSPanel;
i32 bCPrefsChanged;
