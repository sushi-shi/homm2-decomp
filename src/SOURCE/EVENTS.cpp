#include <va.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/bmap2.h>
#include <BASE/executive.h>
#include <BASE/widget.h>
#include <BASE/inputManager.h>
#include <BASE/Misc.h>
#include <BASE/mouseManager.h>
#include <BASE/soundManager.h>
#include <EDITOR/fullMap.h>
#include <SOURCE/advManager.h>
#include <SOURCE/ARMY.h>
#include <SOURCE/armyGroup.h>
#include <SOURCE/Campaign.h>
#include <SOURCE/CURSOR.h>
#include <SOURCE/EVENTS.h>
#include <SOURCE/ExpCampaign.h>
#include <SOURCE/GAME.h>
#include <SOURCE/game.h>
#include <SOURCE/hero.h>
#include <SOURCE/KB.h>
#include <SOURCE/PHILAI.h>
#include <SOURCE/philAI.h>
#include <SOURCE/REMOTE.h>
#include <SOURCE/combatManager.h>
#include <SOURCE/kbwin.h>
#include <SOURCE/playerData.h>
#include <SOURCE/recruitUnit.h>
#include <SOURCE/swapManager.h>
#include <SOURCE/tradpost.h>
#include <SOURCE/town.h>
#include <SOURCE/EVENTS_TYPES.h>
#include <SOURCE/townManager.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/x_arena.h>

#define GENERIC_SITE_SIREN_ARMY_REMAINDER 0.7
#define MONSTER_NECROMANCY_FRACTION 0.1
#define MONSTER_AI_JOIN_COST_FRACTION 0.75

namespace {

    H2_ENUM_BEGIN(CombatRemoteCommand)
        REMOTE_COMMAND = 0x15,
        REMOTE_CONFIRM_COMMAND = 0x16
    H2_ENUM_END(CombatRemoteCommand)

    H2_ENUM_BEGIN(CombatRemoteFragment)
        REMOTE_FIRST_HERO_FIRST = 1,
        REMOTE_FIRST_HERO_SECOND = 2,
        REMOTE_SECOND_HERO_FIRST = 3,
        REMOTE_SECOND_HERO_SECOND = 4
    H2_ENUM_END(CombatRemoteFragment)

    H2_ENUM_BEGIN(UndeadEventConstant)
        SKELETON_EVENT_SMALL_COUNT = 25,
        SKELETON_EVENT_MEDIUM_COUNT = 50,
        SKELETON_EVENT_LARGE_COUNT = 100,
        SKELETON_EVENT_HUGE_COUNT = 200,
        SKELETON_EVENT_SMALL_GOLD = 500,
        SKELETON_EVENT_MEDIUM_GOLD = 1000,
        SKELETON_EVENT_LARGE_GOLD = 2000,
        SKELETON_EVENT_HUGE_GOLD = 5000,
        ZOMBIE_EVENT_SMALL_COUNT = 6,
        EVENT_SMALL_SUPPORT_COUNT = 4,
        ZOMBIE_EVENT_MEDIUM_COUNT = 15,
        EVENT_MEDIUM_SUPPORT_COUNT = 10,
        ZOMBIE_EVENT_LARGE_COUNT = 30,
        EVENT_LARGE_SUPPORT_COUNT = 20,
        ZOMBIE_EVENT_HUGE_COUNT = 60,
        EVENT_HUGE_SUPPORT_COUNT = 40,
        EVENT_SUPPORT_STACKS = 2,
        ZOMBIE_EVENT_SMALL_GOLD = 500,
        ZOMBIE_EVENT_MEDIUM_GOLD = 1000,
        ZOMBIE_EVENT_LARGE_GOLD = 3000,
        ZOMBIE_EVENT_HUGE_GOLD = 1000,
        GHOST_EVENT_SMALL_COUNT = 10,
        GHOST_EVENT_MEDIUM_COUNT = 15,
        GHOST_EVENT_LARGE_COUNT = 25,
        GHOST_EVENT_HUGE_COUNT = 50,
        GHOST_EVENT_SMALL_GOLD = 1000,
        GHOST_EVENT_MEDIUM_GOLD = 2000,
        GHOST_EVENT_LARGE_GOLD = 5000,
        GHOST_EVENT_HUGE_GOLD = 2000
    H2_ENUM_END(UndeadEventConstant)

    H2_ENUM_BEGIN(AIEventConstant)
        EVENT_NO_OWNER = -1,
        EVENT_ROGUE_COUNT = 50,
        EVENT_GUARD_COUNT_MAX = 255,
        EVENT_MINE_SPELL_LOOP_END = 999,
        EVENT_MINE_SPELL_POWER_MAX = 51,
        EVENT_MINE_SPELL_COUNT_SCALE = 5,
        EVENT_DAEMON_GOLD = 2500,
        EVENT_SEA_CHEST_GOLD = 1500,
        EVENT_SEA_CHEST_ARTIFACT_GOLD = 1000,
        EVENT_TREE_GOLD = 2000,
        EVENT_TREE_GEMS = 10,
        EVENT_ARTIFACT_GOLD = 2000,
        EVENT_ARTIFACT_RESOURCE_3_GOLD = 2500,
        EVENT_ARTIFACT_RESOURCE_5_GOLD = 3000,
        EVENT_ARTIFACT_RESOURCE_3 = 3,
        EVENT_ARTIFACT_RESOURCE_5 = 5,
        EVENT_TELEPORT_STONE_DISTANCE = 1,
        EVENT_TELEPORT_WHIRLPOOL_DISTANCE = 3,
        EVENT_ABANDONED_MINE_ARMY_DIVISOR = 5,
        EVENT_OBSERVATION_RADIUS = 20,
        EVENT_WINDMILL_EMPTY = 99,
        EVENT_RANDOM_EVENT_SUCCESS = 40,
        EVENT_RANDOM_PERCENT_MAX = 100,
        EVENT_BOAT_RESTORE_MODE = 3,
        EVENT_CURSED_ARTIFACT_COST = 750,
        EVENT_CURSED_ARTIFACT_GOLD_THRESHOLD = 1500,
        EVENT_SIRENS_MIN_ARMY_QUANTITY = 1,
        EVENT_CREATURE_UPGRADE_MOBILITY = 400,
        EVENT_HERO_LIMIT = 8,
        EVENT_JAILED_HERO = 0x41
    H2_ENUM_END(AIEventConstant)

    H2_ENUM_BEGIN(AIEventResultIndex)
        AI_EVENT_RESULT_ERASE_OBJECT = 0,
        AI_EVENT_RESULT_UNUSED = 1,
        AI_EVENT_RESULT_PURCHASE_COUNT = 2,
        AI_EVENT_RESULT_COUNT = 3
    H2_ENUM_END(AIEventResultIndex)

    H2_ENUM_BEGIN(MonsterCombatValueIndex)
        MONSTER_COMBAT_REMAINING_COUNT = 0,
        MONSTER_COMBAT_RESULT = 1,
        MONSTER_COMBAT_VALUE_COUNT = 2
    H2_ENUM_END(MonsterCombatValueIndex)

    H2_ENUM_BEGIN(CombatFlowConstant)
        COMBAT_AUTO_RESOLVE_DEBUG_LEVEL = 4,
        COMBAT_RANDOM_SEED_MAX = 1000,
        COMBAT_NETWORK_POLL_X = 30,
        COMBAT_NETWORK_POLL_Y = 30,
        COMBAT_NETWORK_POLL_WIDTH = 4,
        COMBAT_NETWORK_POLL_HEIGHT = 4,
        COMBAT_LOW_MEMORY_LIMIT = 900,
        COMBAT_HIGH_MEMORY_LIMIT = 2900
    H2_ENUM_END(CombatFlowConstant)

    H2_ENUM_BEGIN(RiddleComparisonConstant)
        RIDDLE_PREFIX_LENGTH = 4,
        RIDDLE_EXPECTED_BUFFER_SIZE = RIDDLE_PREFIX_LENGTH + 1,
        RIDDLE_ANSWER_BUFFER_SIZE = 8
    H2_ENUM_END(RiddleComparisonConstant)

    H2_ENUM_BEGIN(BarrierEventConstant)
        COLOR_MASK = 7,
        PASSWORD_SHIFT = 3,
        INPUT_LENGTH = 14,
        INPUT_BUFFER_SIZE = 16
    H2_ENUM_END(BarrierEventConstant)

    H2_ENUM_BEGIN(GenericSiteConstant)
        SITE_ALCHEMIST_COST = 750,
        SITE_VISIBILITY_RADIUS = 10,
        SITE_HUT_COLOR = 3,
        SITE_STABLE_MOBILITY = 400,
        SITE_STABLE_REWARD_VALUE = 8,
        SITE_MERMAID_WINDOW_ICON = 10,
        SITE_STRING_LIMIT = 10
    H2_ENUM_END(GenericSiteConstant)

    H2_ENUM_CLASS_BEGIN_SPLIT(StableVisitResult, u8)
        STABLE_VISIT_NONE = 0,
        STABLE_VISIT_MOBILITY = 1,
        STABLE_VISIT_UPGRADE = 2,
        STABLE_VISIT_MOBILITY_UPGRADE = STABLE_VISIT_MOBILITY | STABLE_VISIT_UPGRADE
    H2_ENUM_CLASS_END_SPLIT(StableVisitResult, u8)
    H2_ENUM_FLAGS(StableVisitResult)

    H2_ENUM_CLASS_BEGIN(HouseRecruitmentSite)
        RECRUIT_ARCHER = 0,
        RECRUIT_GOBLIN = 1,
        RECRUIT_PEASANT = 2,
        RECRUIT_DWARF = 3,
        RECRUIT_LOG_CABIN = 4,
        RECRUIT_TREE_HOUSE = 5,
        RECRUIT_HALFLING = 6,
        RECRUIT_WATCH_TOWER = 7,
        RECRUIT_CAVE = 8,
        RECRUIT_EXCAVATION = 9,
        RECRUIT_SITE_COUNT = 10
    H2_ENUM_CLASS_END(HouseRecruitmentSite)

    H2_ENUM_BEGIN(HouseEventConstant)
        EVENT_HOUSE_DIALOG_STRIDE = 3,
        EVENT_RECRUIT_DIALOG_BASE = 23,
        EVENT_ARMY_FULL_DIALOG_BASE = 24,
        EVENT_EMPTY_DIALOG_BASE = 25
    H2_ENUM_END(HouseEventConstant)

    H2_ENUM_BEGIN(EventSoundConstant)
        SOUND_NONE = -1,
        SOUND_TRACK_1 = 1,
        SOUND_TREASURE = 19,
        SOUND_EXPERIENCE = 22,
        SOUND_TRACK_31 = 31,
        SOUND_TRACK_32 = 32,
        SOUND_TRACK_33 = 33,
        SOUND_TRACK_34 = 34,
        SOUND_TRACK_35 = 35,
        SOUND_TRACK_36 = 36,
        SOUND_TRACK_37 = 37,
        SOUND_TRACK_38 = 38,
        SOUND_TRACK_39 = 39,
        SOUND_TRACK_40 = 40,
        SOUND_MORALE = 100,
        SOUND_LUCK = 101,
        SOUND_PICKUP = 102,
        SOUND_MINE = 103,
        SOUND_FILENAME_LENGTH = 32,
        SOUND_SAMPLE_NAME_EMPTY_LIMIT = 1
    H2_ENUM_END(EventSoundConstant)

    H2_ENUM_CLASS_BEGIN(EventSoundVariant)
        SOUND_VARIANT_0 = 0,
        SOUND_VARIANT_1 = 1,
        SOUND_VARIANT_2 = 2,
        SOUND_VARIANT_3 = 3,
        SOUND_VARIANT_4 = 4,
        SOUND_VARIANT_5 = 5,
        SOUND_VARIANT_6 = 6
    H2_ENUM_CLASS_END(EventSoundVariant)

    H2_ENUM_BEGIN(EraseObjectConstant)
        NEIGHBOR_COUNT = 4,
        CELL_COUNT = 5,
        ERASE_COORDINATE_COUNT = 2,
        NO_FRAME = -1,
        EMPTY_INDEX = 0xFF,
        MAP_CHANGE_VALUE = -999,
        ENVIRONMENT_BORDER = 7
    H2_ENUM_END(EraseObjectConstant)

    H2_ENUM_BEGIN(CombatMonsterEventConstant)
        MONSTER_ARMY_SLOTS = 5,
        MONSTER_RANDOM_MAX = 100,
        MONSTER_REDUCED_STACK_CHANCE = 25,
        MONSTER_REDUCED_STACK_COUNT = 3,
        MONSTER_FOUR_STACK_THRESHOLD = 50,
        MONSTER_FOUR_STACK_COUNT = 4,
        MONSTER_UPGRADE_CHANCE = 50
    H2_ENUM_END(CombatMonsterEventConstant)

    H2_ENUM_BEGIN(FizzleCenterConstant)
        ARTIFACT_PICKUP_SOUND_FIRST = 1,
        ARTIFACT_PICKUP_SOUND_LAST = 7
    H2_ENUM_END(FizzleCenterConstant)

    H2_ENUM_BEGIN(FlotsamConstant)
        FLOTSAM_WOOD_AMOUNT = 5,
        FLOTSAM_GOLD_AMOUNT = 200,
        FLOTSAM_LARGE_WOOD_AMOUNT = 10,
        FLOTSAM_LARGE_GOLD_AMOUNT = 500
    H2_ENUM_END(FlotsamConstant)

    H2_ENUM_BEGIN(DoEventConstant)
        ARTESIAN_SPRING_MANA_MULTIPLIER = 2,
        XANADU_DIPLOMACY_MULTIPLIER = 2,
        TEMPLE_MORALE_BONUS = 2,
        PYRAMID_LUCK_PENALTY = 2,
        SIGN_MINIMUM_TEXT_LENGTH = 2,
        SIGN_RANDOM_TEXT_MASK = 3,
        CHEST_EXPERIENCE_LEVEL_OFFSET = 1,
        PRIMARY_BONUS_ATTACK = NORMAL_DIALOG_PRIMARY_BONUS_OFFSET + IDX(HERO_PRIMARY_ATTACK),
        PRIMARY_BONUS_DEFENSE = NORMAL_DIALOG_PRIMARY_BONUS_OFFSET + IDX(HERO_PRIMARY_DEFENSE),
        PRIMARY_BONUS_SPELL_POWER =
            NORMAL_DIALOG_PRIMARY_BONUS_OFFSET + IDX(HERO_PRIMARY_SPELL_POWER),
        PRIMARY_BONUS_KNOWLEDGE = NORMAL_DIALOG_PRIMARY_BONUS_OFFSET + IDX(HERO_PRIMARY_KNOWLEDGE)
    H2_ENUM_END(DoEventConstant)

    H2_ENUM_BEGIN(GuardedDwellingConstant)
        TROLL_BRIDGE_TROLL_COUNT = 12,
        TROLL_BRIDGE_WAR_TROLL_COUNT = 8,
        TROLL_BRIDGE_WAR_TROLL_STACKS = 2,
        CITY_DEAD_VAMPIRE_LORD_COUNT = 10,
        CITY_DEAD_ZOMBIE_COUNT = 40,
        CITY_DEAD_ZOMBIE_STACKS = 2,
        CITY_DEAD_POWER_LICH_COUNT = 5,
        CITY_DEAD_POWER_LICH_STACKS = 1,
        DRAGON_CITY_BASE_FACTOR = 1,
        DRAGON_CITY_CAMPAIGN_FACTOR = 2,
        DRAGON_CITY_GREEN_COUNT_FACTOR = 3,
        DRAGON_CITY_DRAGON_STACKS = 1,
        DRAGON_CITY_ARCHIBALD_SCENARIO = 6,
        PYRAMID_ROYAL_MUMMY_COUNT = 30,
        PYRAMID_VAMPIRE_LORD_COUNT = 20,
        PYRAMID_VAMPIRE_LORD_STACKS = 2
    H2_ENUM_END(GuardedDwellingConstant)

    H2_ENUM_BEGIN(AbandonedMineConversionConstant)
        MINE_TOP_A_FIRST_FRAME = 0,
        MINE_TOP_A_LAST_FRAME = 4,
        MINE_BOTTOM_A_FRAME = 104,
        MINE_SECOND_A_FIRST_FRAME = 5,
        MINE_SECOND_A_LAST_FRAME = 9,
        MINE_SECOND_BOTTOM_A_FRAME = 109,
        MINE_TOP_B_FIRST_FRAME = 0,
        MINE_TOP_B_LAST_FRAME = 3,
        MINE_BOTTOM_B_FRAME = 75,
        MINE_SECOND_B_FIRST_FRAME = 4,
        MINE_SECOND_B_LAST_FRAME = 7,
        MINE_SECOND_BOTTOM_B_FRAME = 80,
        MINE_CENTER_GHOST_FRAME = 5,
        MINE_CENTER_GOLD_FRAME = 4
    H2_ENUM_END(AbandonedMineConversionConstant)

}

#define TRADING_POST_EFFICIENCY 0.2f

#define EVENTS_REMOTE_MESSAGE(buffer) (reinterpret_cast<RemoteMessage*>(buffer))
#define EVENTS_REMOTE_COMBAT(buffer)                                                               \
    (reinterpret_cast<combatRemoteData*>(EVENTS_REMOTE_MESSAGE(buffer)->payload))
#define EVENTS_REMOTE_HERO(buffer)                                                                 \
    (reinterpret_cast<combatRemoteHeroFragment*>(EVENTS_REMOTE_MESSAGE(buffer)->payload))
#define EVENTS_HERO_BUFFER(buffer) (reinterpret_cast<combatRemoteHeroFragment*>(buffer))

static char s_twoStringFormat[] = "%s %s";

VA(0x0043b640, 0x5fc9)
void advManager::DoEvent(mapCell* cell, i32 x, i32 y) {
    CreatureType secondUpgrade1;
    CreatureType firstUpgrade_e;
    SAMPLE2 eventSample_f;
    i32 eventValue1;
    signEventExtra* signExtra_k;
    SAMPLE2 playedSample3;
    tag_message unusedEventMessage;
    MapObjectType eventType_g;
    hero* eventHero2;
    char sphinxAnswer_a[SPHINX_ANSWER_BUFFER_SIZE];
    i32 eraseObject;
    i32 fizzleType3;
    CreatureType thirdUpgrade;
    CreatureType monsterType6;
    i32 guardedCount;
    CombatResult heroCombatResult3;
    i32 teleportY;
    ArtifactType artifact8;
    mapEventExtra* eventExtra1;
    ResourceType resourceType;
    i32 teleportX3;
    i32 teleportCount;
    CreatureType guardedMonster5;
    boatRecord* boat_j;
    heroWindow* oracleWindow;
    tag_message oracleMessage1;
    hero* otherHero6;
    i32 resourceAmount6;
    i32 artifactResourceType;
    i32 secondaryAmount;
    i32 primaryAmount1;
    i32 secondaryReward_f;
    i32 primaryReward3;
    i32 correctAnswer1;
    char eventText[EVENT_TEXT_BUFFER_SIZE];
    mapCell* skeletonCell1;
    mapCell* zombieCell6;
    town* occupiedTown4;
    i32 levelExperience1;
    i32 heroLevel2;
    i32 dragonFactor_d;
    CombatResult mineCombatResult7;
    i32 adjacentMonster8;
    i32 wellSpellPoints5;
    i32 springSpellPoints;

    eventHero2 = &gpGame->m_heroRecs[gpCurPlayer->m_currentHero];
    eventType_g = cell->m_triggerType & MAP_TRIGGER_TYPE_MASK;
    eraseObject = 0;
    fizzleType3 = 0;
    playedSample3 = NULL;
    eventSample_f = NULL;
    gpMouseManager->ShowColorPointer();
    gpMouseManager->SetPointer(0);

    switch (eventType_g) {
        case MAP_OBJECT_TRADING_POST:
            EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
            DoTradingPost(0, TRADING_POST_EFFICIENCY);
            break;

        case MAP_OBJECT_MAGIC_GARDEN:
            if (cell->m_objectMetadata == MAP_EVENT_DATA_EMPTY) {
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    "{Magic Garden}\n\nYou've found a magic garden, the kind of place that "
                    "leprechauns and faeries like to cavort in, but there is no one here today.  "
                    "Perhaps you should try again next week.",
                    -1,
                    0,
                    -1,
                    0,
                    -1
                );
            } else {
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    "{Magic Garden}\n\nYou catch a leprechaun foolishly sleeping amidst a cluster "
                    "of magic mushrooms.  In exchange for his freedom, he guides you to a small "
                    "pot filled with precious things.",
                    cell->m_objectMetadata - MAP_EVENT_RESOURCE_OFFSET,
                    static_cast<ResourceType>(
                        cell->m_objectMetadata - MAP_EVENT_RESOURCE_OFFSET
                    ) == RES_GOLD
                        ? MAP_EVENT_GOLD_AMOUNT
                        : MAP_EVENT_RESOURCE_AMOUNT,
                    -1,
                    0,
                    -1
                );
                GiveResource(
                    eventHero2,
                    static_cast<ResourceType>(
                        cell->m_objectMetadata - MAP_EVENT_RESOURCE_OFFSET
                    ),
                    static_cast<ResourceType>(
                        cell->m_objectMetadata - MAP_EVENT_RESOURCE_OFFSET
                    ) == RES_GOLD
                        ? MAP_EVENT_GOLD_AMOUNT
                        : MAP_EVENT_RESOURCE_AMOUNT
                );
                cell->m_objectMetadata = MAP_EVENT_DATA_EMPTY;
            }
            break;

        case MAP_OBJECT_SPHINX:
            EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
            eventExtra1 = reinterpret_cast<mapEventExtra*>(ppMapExtra[cell->m_objectMetadata]);
            if (!eventExtra1->active) {
                NormalDialog(
                    "{Sphinx}\n\nYou come across a giant Sphinx.  The Sphinx remains strangely "
                    "quiet.",
                    NORMAL_DIALOG_INFO,
                    -1,
                    -1,
                    -1,
                    0,
                    -1,
                    0,
                    -1,
                    0
                );
            } else {
                sprintf(
                    gText,
                    "\"I have a riddle for you,\" the Sphinx says.  \"Answer correctly, and you "
                    "shall be rewarded.  Answer incorrectly, and you shall be eaten.  Do you "
                    "accept the challenge?\""
                );
                NormalDialog(gText, NORMAL_DIALOG_CONFIRM, -1, -1, -1, 0, -1, 0, -1, 0);
                if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
                    sprintf(
                        gText,
                        "The Sphinx asks you the following riddle:\n\n'%s'\n\nYour answer?",
                        eventExtra1->riddle
                    );
                    GetDataEntry(gText, sphinxAnswer_a, SPHINX_INPUT_LENGTH, NULL, 0, 1);
                    correctAnswer1 = 0;
                    for (eventValue1 = 0; eventValue1 < eventExtra1->answerCount; eventValue1++) {
                        if (RiddleStringsEqual(sphinxAnswer_a, eventExtra1->answers[eventValue1]))
                            correctAnswer1 = 1;
                    }

                    if (correctAnswer1) {
                        primaryReward3 = MAP_EVENT_REWARD_NONE;
                        primaryAmount1 = 0;
                        secondaryReward_f = MAP_EVENT_REWARD_NONE;
                        secondaryAmount = 0;
                        for (eventValue1 = 0; eventValue1 < SPHINX_RESOURCE_COUNT;
                             eventValue1++) {
                            gpGame->m_players[giCurPlayer].m_resources[eventValue1] +=
                                eventExtra1->resources[eventValue1];
                            if (gpGame->m_players[giCurPlayer].m_resources[eventValue1] < 0)
                                gpGame->m_players[giCurPlayer].m_resources[eventValue1] = 0;
                            if (eventExtra1->resources[eventValue1] != 0) {
                                if (primaryReward3 != MAP_EVENT_REWARD_NONE) {
                                    secondaryReward_f = primaryReward3;
                                    secondaryAmount = primaryAmount1;
                                }
                                primaryReward3 = eventValue1;
                                primaryAmount1 = eventExtra1->resources[eventValue1];
                            }
                        }

                        if (eventExtra1->artifact != MAP_EVENT_REWARD_NONE
                            && eventHero2->NumArtifacts() < EVENT_ARTIFACT_CAPACITY) {
                            GiveArtifact(eventHero2, ArtifactType(eventExtra1->artifact), 1, -1);
                            if (primaryReward3 != MAP_EVENT_REWARD_NONE) {
                                secondaryReward_f = primaryReward3;
                                secondaryAmount = primaryAmount1;
                            }
                            primaryReward3 = MAP_EVENT_REWARD_ARTIFACT;
                            primaryAmount1 = eventExtra1->artifact;
                        }

                        NormalDialog(
                            "Looking somewhat disappointed, the Sphinx sighs.  You've answered my "
                            "riddle so here's your reward.  Now begone.",
                            NORMAL_DIALOG_INFO,
                            -1,
                            -1,
                            primaryReward3,
                            primaryAmount1,
                            secondaryReward_f,
                            secondaryAmount,
                            -1,
                            0
                        );
                        eventExtra1->active = 0;
                    } else {
                        NormalDialog(
                            "\"You guessed incorrectly,\" the Sphinx says, smiling.  The Sphinx "
                            "swipes at you with a paw, knocking you to the ground.  Another blow "
                            "makes the world go black, and you know no more.",
                            NORMAL_DIALOG_INFO,
                            -1,
                            -1,
                            -1,
                            0,
                            -1,
                            0,
                            -1,
                            0
                        );
                        HeroLoses(eventHero2);
                    }
                }
            }
            break;

        case MAP_OBJECT_OBSERVATION_TOWER:
            EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
            NormalDialog(
                "{Observation Tower}\n\nFrom the observation tower, you are able to see distant "
                "lands.",
                NORMAL_DIALOG_INFO,
                -1,
                -1,
                -1,
                0,
                -1,
                0,
                -1,
                0
            );
            gpGame->SetVisibility(x, y, giCurPlayer, OBSERVATION_TOWER_RADIUS);
            CompleteDraw(0);
            UpdateScreen(0, 0);
            break;

        case MAP_OBJECT_HILL_FORT:
            thirdUpgrade = secondUpgrade1 = firstUpgrade_e = CREATURE_NONE;
            if (eventHero2->CreatureTypeCount(CREATURE_OGRE))
                firstUpgrade_e = CREATURE_OGRE;
            if (eventHero2->CreatureTypeCount(CREATURE_ORC)) {
                if (firstUpgrade_e == CREATURE_NONE)
                    firstUpgrade_e = CREATURE_ORC;
                else
                    secondUpgrade1 = CREATURE_ORC;
            }
            if (eventHero2->CreatureTypeCount(CREATURE_DWARF)) {
                if (firstUpgrade_e == CREATURE_NONE)
                    firstUpgrade_e = CREATURE_DWARF;
                else if (secondUpgrade1 == CREATURE_NONE)
                    secondUpgrade1 = CREATURE_DWARF;
                else
                    thirdUpgrade = CREATURE_DWARF;
            }

            if (firstUpgrade_e != CREATURE_NONE) {
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                eventHero2->UpgradeCreatures(CREATURE_OGRE, CREATURE_OGRE_LORD);
                eventHero2->UpgradeCreatures(CREATURE_ORC, CREATURE_ORC_CHIEF);
                eventHero2->UpgradeCreatures(CREATURE_DWARF, CREATURE_BATTLE_DWARF);
                if (thirdUpgrade != CREATURE_NONE) {
                    sprintf(
                        gText,
                        "{Hill Fort}\n\n All of the %s, %s and %s you have in your army have been "
                        "trained by the battle masters of the fort.  Your army now contains %s, "
                        "%s, and %s.",
                        gArmyNamesPlural[IDX(firstUpgrade_e)],
                        gArmyNamesPlural[IDX(secondUpgrade1)],
                        gArmyNamesPlural[IDX(thirdUpgrade)],
                        gArmyNamesPlural[IDX(firstUpgrade_e) + 1],
                        gArmyNamesPlural[IDX(secondUpgrade1) + 1],
                        gArmyNamesPlural[IDX(thirdUpgrade) + 1]
                    );
                } else if (secondUpgrade1 != CREATURE_NONE) {
                    sprintf(
                        gText,
                        "{Hill Fort}\n\nAll of the %s and %s you have in your army have been "
                        "trained by the battle masters of the fort.  Your army now contains %s "
                        "and %s.",
                        gArmyNamesPlural[IDX(firstUpgrade_e)],
                        gArmyNamesPlural[IDX(secondUpgrade1)],
                        gArmyNamesPlural[IDX(firstUpgrade_e) + 1],
                        gArmyNamesPlural[IDX(secondUpgrade1) + 1]
                    );
                } else {
                    sprintf(
                        gText,
                        "{Hill Fort}\n\nAll of the %s you have in your army have been trained "
                        "by the battle masters of the fort.  Your army now contains %s.",
                        gArmyNamesPlural[IDX(firstUpgrade_e)],
                        gArmyNamesPlural[IDX(firstUpgrade_e) + 1]
                    );
                }
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    gText,
                    NORMAL_DIALOG_MONSTER,
                    IDX(firstUpgrade_e) + 1,
                    secondUpgrade1 == CREATURE_NONE ? -1 : NORMAL_DIALOG_MONSTER,
                    IDX(secondUpgrade1) + 1,
                    -1
                );
            } else {
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    "{Hill Fort}\n\nAn unusual alliance of Orcs, Ogres, and Dwarves offer to train "
                    "(upgrade) any such troops brought to them.  Unfortunately, you have none with "
                    "you.",
                    -1,
                    0,
                    -1,
                    0,
                    -1
                );
            }
            break;

        case MAP_OBJECT_FREEMANS_FOUNDRY:
            thirdUpgrade = CREATURE_NONE;
            secondUpgrade1 = CREATURE_NONE;
            firstUpgrade_e = CREATURE_NONE;
            if (eventHero2->CreatureTypeCount(CREATURE_PIKEMAN))
                firstUpgrade_e = CREATURE_PIKEMAN;
            if (eventHero2->CreatureTypeCount(CREATURE_SWORDSMAN)) {
                if (firstUpgrade_e == CREATURE_NONE)
                    firstUpgrade_e = CREATURE_SWORDSMAN;
                else
                    secondUpgrade1 = CREATURE_SWORDSMAN;
            }
            if (eventHero2->CreatureTypeCount(CREATURE_IRON_GOLEM)) {
                if (firstUpgrade_e == CREATURE_NONE)
                    firstUpgrade_e = CREATURE_IRON_GOLEM;
                else if (secondUpgrade1 == CREATURE_NONE)
                    secondUpgrade1 = CREATURE_IRON_GOLEM;
                else
                    thirdUpgrade = CREATURE_IRON_GOLEM;
            }

            if (firstUpgrade_e != CREATURE_NONE) {
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                eventHero2->UpgradeCreatures(CREATURE_PIKEMAN, CREATURE_VETERAN_PIKEMAN);
                eventHero2->UpgradeCreatures(
                    CREATURE_SWORDSMAN,
                    CREATURE_MASTER_SWORDSMAN
                );
                eventHero2->UpgradeCreatures(CREATURE_IRON_GOLEM, CREATURE_STEEL_GOLEM);
                if (thirdUpgrade != CREATURE_NONE) {
                    sprintf(
                        gText,
                        "{Freeman's Foundry}\n\nAll of your  %s, %s and %s have been upgraded into "
                        "%s, %s, and %s.",
                        gArmyNamesPlural[IDX(firstUpgrade_e)],
                        gArmyNamesPlural[IDX(secondUpgrade1)],
                        gArmyNamesPlural[IDX(thirdUpgrade)],
                        gArmyNamesPlural[IDX(firstUpgrade_e) + 1],
                        gArmyNamesPlural[IDX(secondUpgrade1) + 1],
                        gArmyNamesPlural[IDX(thirdUpgrade) + 1]
                    );
                } else if (secondUpgrade1 != CREATURE_NONE) {
                    sprintf(
                        gText,
                        "{Freeman's Foundry}\n\nAll of your %s and %s have been upgraded into "
                        "%s and %s.",
                        gArmyNamesPlural[IDX(firstUpgrade_e)],
                        gArmyNamesPlural[IDX(secondUpgrade1)],
                        gArmyNamesPlural[IDX(firstUpgrade_e) + 1],
                        gArmyNamesPlural[IDX(secondUpgrade1) + 1]
                    );
                } else {
                    sprintf(
                        gText,
                        "{Freeman's Foundry}\n\nAll of your %s have been upgraded into %s.",
                        gArmyNamesPlural[IDX(firstUpgrade_e)],
                        gArmyNamesPlural[IDX(firstUpgrade_e) + 1]
                    );
                }
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    gText,
                    NORMAL_DIALOG_MONSTER,
                    IDX(firstUpgrade_e) + 1,
                    secondUpgrade1 == CREATURE_NONE ? -1 : NORMAL_DIALOG_MONSTER,
                    IDX(secondUpgrade1) + 1,
                    -1
                );
            } else {
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    "{Freeman's Foundry}\n\nA blacksmith working at the foundry offers to convert "
                    "all Pikemen and Swordsmen's weapons brought to him from iron to steel. He "
                    "also says that he knows a process that will convert Iron Golems into Steel "
                    "Golems.  Unfortunately, you have none of these troops in your army, so he "
                    "can't help you.",
                    -1,
                    0,
                    -1,
                    0,
                    -1
                );
            }
            break;

        case MAP_OBJECT_SEA_CHEST:
            if (!(cell->m_objectMetadata & CHEST_ARTIFACT_FLAG)
                || eventHero2->NumArtifacts() >= EVENT_ARTIFACT_CAPACITY) {
                if (cell->m_objectMetadata == SEA_CHEST_OUTCOME_EMPTY) {
                    NormalDialog(
                        "{Chest}\n\nAfter spending hours trying to fish the chest out of the sea, "
                        "you open it, only to find it empty.",
                        NORMAL_DIALOG_INFO,
                        -1,
                        -1,
                        -1,
                        0,
                        -1,
                        0,
                        -1,
                        0
                    );
                } else {
                    NormalDialog(
                        "{Chest}\n\nAfter spending hours trying to fish the chest out of the sea, "
                        "you open it and find 1500 gold pieces.",
                        NORMAL_DIALOG_INFO,
                        -1,
                        -1,
                        IDX(RES_GOLD),
                        SEA_CHEST_GOLD,
                        -1,
                        0,
                        -1,
                        0
                    );
                    GiveResource(eventHero2, RES_GOLD, SEA_CHEST_GOLD);
                }
            } else {
                eventValue1 = cell->m_objectMetadata & CHEST_ARTIFACT_MASK;
                sprintf(
                    gText,
                    "{Chest}\n\nAfter spending hours trying to fish the chest out of the sea, you "
                    "open it and find 1000 gold and the %s",
                    gArtifactNames[eventValue1]
                );
                NormalDialog(
                    gText,
                    NORMAL_DIALOG_INFO,
                    -1,
                    -1,
                    MAP_EVENT_REWARD_ARTIFACT,
                    eventValue1,
                    IDX(RES_GOLD),
                    SEA_CHEST_ARTIFACT_GOLD,
                    -1,
                    0
                );
                GiveArtifact(eventHero2, ArtifactType(eventValue1), 1, -1);
                GiveResource(eventHero2, RES_GOLD, SEA_CHEST_ARTIFACT_GOLD);
            }
            fizzleType3 = 1;
            eraseObject = 1;
            break;

        case MAP_OBJECT_FLOTSAM: {
            switch (static_cast<FlotsamReward>(cell->m_objectMetadata)) {
                case FLOTSAM_EMPTY:
                    NormalDialog(
                        "{Flotsam}\n\nYou search through the flotsam, but find nothing.",
                        NORMAL_DIALOG_INFO,
                        -1,
                        -1,
                        -1,
                        0,
                        -1,
                        0,
                        -1,
                        0
                    );
                    break;
                case FLOTSAM_WOOD:
                    NormalDialog(
                        "{Flotsam}\n\nYou search through the flotsam, and find some wood.",
                        NORMAL_DIALOG_INFO,
                        -1,
                        -1,
                        IDX(RES_WOOD),
                        FLOTSAM_WOOD_AMOUNT,
                        -1,
                        0,
                        -1,
                        0
                    );
                    GiveResource(eventHero2, RES_WOOD, FLOTSAM_WOOD_AMOUNT);
                    break;
                case FLOTSAM_WOOD_AND_GOLD:
                    NormalDialog(
                        "{Flotsam}\n\nYou search through the flotsam, and find some wood and some "
                        "gold.",
                        NORMAL_DIALOG_INFO,
                        -1,
                        -1,
                        IDX(RES_WOOD),
                        FLOTSAM_WOOD_AMOUNT,
                        IDX(RES_GOLD),
                        FLOTSAM_GOLD_AMOUNT,
                        -1,
                        0
                    );
                    GiveResource(eventHero2, RES_WOOD, FLOTSAM_WOOD_AMOUNT);
                    GiveResource(eventHero2, RES_GOLD, FLOTSAM_GOLD_AMOUNT);
                    break;
                case FLOTSAM_LARGE_TREASURE:
                    NormalDialog(
                        "{Flotsam}\n\nYou search through the flotsam and find some wood and some "
                        "gold.",
                        NORMAL_DIALOG_INFO,
                        -1,
                        -1,
                        IDX(RES_WOOD),
                        FLOTSAM_LARGE_WOOD_AMOUNT,
                        IDX(RES_GOLD),
                        FLOTSAM_LARGE_GOLD_AMOUNT,
                        -1,
                        0
                    );
                    GiveResource(eventHero2, RES_WOOD, FLOTSAM_LARGE_WOOD_AMOUNT);
                    GiveResource(eventHero2, RES_GOLD, FLOTSAM_LARGE_GOLD_AMOUNT);
                    break;
            }
            fizzleType3 = 1;
            eraseObject = 1;
            break;
        }

        case MAP_OBJECT_SHIPWRECK_SURVIVOR:
            if (eventHero2->NumArtifacts() < EVENT_ARTIFACT_CAPACITY) {
                sprintf(
                    gText,
                    "{Shipwreck Survivor}\n\nYou've pulled a shipwreck survivor from certain death "
                    "in an unforgiving ocean.  Grateful, he rewards you for your act of kindness "
                    "by giving you the %s.",
                    gArtifactNames[cell->m_objectMetadata]
                );
                NormalDialog(
                    gText,
                    NORMAL_DIALOG_INFO,
                    -1,
                    -1,
                    MAP_EVENT_REWARD_ARTIFACT,
                    cell->m_objectMetadata,
                    -1,
                    0,
                    -1,
                    0
                );
                GiveArtifact(eventHero2, ArtifactType(cell->m_objectMetadata), 1, -1);
            } else {
                NormalDialog(
                    "{Shipwreck Survivor}\n\nYou've pulled a shipwreck survivor from certain death "
                    "in an unforgiving ocean.  Grateful, he says, \"I would give you an artifact "
                    "as a reward, but you're all full.\"",
                    NORMAL_DIALOG_INFO,
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
            fizzleType3 = 1;
            eraseObject = 1;
            break;

        case MAP_OBJECT_MAGELLAN_MAPS:
            if (gpCurPlayer->m_resources[IDX(RES_GOLD)] < MAGELLAN_MAP_COST) {
                NormalDialog(
                    "{Magellan's Maps}\n\nThe captain sighs.  \"You don't have enough money, eh?  "
                    "You can't expect me to give my maps away for free!\"",
                    NORMAL_DIALOG_INFO,
                    -1,
                    -1,
                    -1,
                    0,
                    -1,
                    0,
                    -1,
                    0
                );
            } else {
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                NormalDialog(
                    "{Magellan's Maps}\n\nA retired captain living on this refurbished fishing "
                    "platform offers to sell you maps of the sea he made in his younger days for "
                    "1,000 gold.  Do you wish to buy the maps?",
                    NORMAL_DIALOG_CONFIRM,
                    -1,
                    -1,
                    -1,
                    0,
                    -1,
                    0,
                    -1,
                    0
                );
                if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
                    gpCurPlayer->m_resources[IDX(RES_GOLD)] -= MAGELLAN_MAP_COST;
                    gpGame->MakeAllWaterVisible(giCurPlayer);
                    CompleteDraw(0);
                    UpdateScreen(0, 0);
                }
            }
            break;

        case MAP_OBJECT_WITCH_HUT:
            if (eventHero2->m_secondarySkills[cell->m_objectMetadata] != HERO_SKILL_LEVEL_NONE) {
                sprintf(
                    gText,
                    "{Witch's Hut}\n\nYou approach the hut and observe a witch inside studying an "
                    "ancient tome on %s.  As you approach, she turns and speaks.  \"You already "
                    "know that which I would teach you. I can help you no further.\"",
                    gSecondarySkills[cell->m_objectMetadata]
                );
                NormalDialog(gText, NORMAL_DIALOG_INFO, -1, -1, -1, 0, -1, 0, -1, 0);
                break;
            }
            if (eventHero2->m_secondarySkillCount >= HERO_SECONDARY_SKILL_LIMIT) {
                sprintf(
                    gText,
                    "{Witch's Hut}\n\nYou approach the hut and observe a witch inside studying "
                    "an ancient tome on %s.  As you approach, she turns and focuses her one "
                    "glass eye on you.  \"You already know everything you deserve to learn!\" "
                    "the witch screeches. \"NOW GET OUT OF MY HOUSE!\"",
                    gSecondarySkills[cell->m_objectMetadata]
                );
                NormalDialog(gText, NORMAL_DIALOG_INFO, -1, -1, -1, 0, -1, 0, -1, 0);
                break;
            }
            EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
            sprintf(
                gText,
                "{Witch's Hut}\n\nAn ancient and immortal witch living in a hut with "
                "bird's legs for stilts teaches you %s for her own inscrutable purposes.",
                gSecondarySkills[cell->m_objectMetadata]
            );
            EventWindow(
                -1,
                NORMAL_DIALOG_INFO,
                gText,
                NORMAL_DIALOG_SECONDARY_SKILL,
                cell->m_objectMetadata * SECONDARY_SKILL_VALUE_LEVEL_COUNT,
                -1,
                0,
                -1
            );
            eventHero2->GiveSS(
                static_cast<HeroSecondarySkill>(cell->m_objectMetadata),
                HERO_SKILL_LEVEL_BASIC
            );
            break;

        case MAP_OBJECT_ARTESIAN_SPRING:
            if (!cell->m_objectMetadata) {
                NormalDialog(
                    "{Artesian Spring}\n\nThe spring only refills once a week, and someone's "
                    "already been here this week.",
                    NORMAL_DIALOG_INFO,
                    -1,
                    -1,
                    -1,
                    0,
                    -1,
                    0,
                    -1,
                    0
                );
            } else {
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                cell->m_objectMetadata = 0;
                springSpellPoints =
                    eventHero2->Stats(HERO_PRIMARY_KNOWLEDGE) * HERO_SPELL_POINTS_PER_KNOWLEDGE;
                if (eventHero2->m_spellPoints
                    < springSpellPoints * ARTESIAN_SPRING_MANA_MULTIPLIER) {
                    NormalDialog(
                        "{Artesian Spring}\n\nA drink from the spring fills your blood with magic! "
                        " You have twice your normal spell points in reserve.",
                        NORMAL_DIALOG_INFO,
                        -1,
                        -1,
                        -1,
                        0,
                        -1,
                        0,
                        -1,
                        0
                    );
                    eventHero2->m_spellPoints = springSpellPoints * ARTESIAN_SPRING_MANA_MULTIPLIER;
                } else {
                    NormalDialog(
                        "{Artesian Spring}\n\nA drink at the spring is supposed to give you twice "
                        "your normal spell points, but you are already at that level.",
                        NORMAL_DIALOG_INFO,
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
            break;

        case MAP_OBJECT_MAGIC_WELL:
            if (!HAS(eventHero2->m_eventFlags, HERO_EVENT_MAGIC_WELL)) {
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                cell->m_objectMetadata = 0;
                wellSpellPoints5 =
                    eventHero2->Stats(HERO_PRIMARY_KNOWLEDGE) * HERO_SPELL_POINTS_PER_KNOWLEDGE;
                if (eventHero2->m_spellPoints < wellSpellPoints5) {
                    NormalDialog(
                        "{Magic Well}\n\nA drink from the well has restored your spell points to "
                        "maximum.",
                        NORMAL_DIALOG_INFO,
                        -1,
                        -1,
                        -1,
                        0,
                        -1,
                        0,
                        -1,
                        0
                    );
                    eventHero2->m_eventFlags = HeroEventFlag(
                        static_cast<i32>(eventHero2->m_eventFlags) | IDX(HERO_EVENT_MAGIC_WELL)
                    );
                    eventHero2->m_spellPoints = wellSpellPoints5;
                } else {
                    NormalDialog(
                        "{Magic Well}\n\nA drink at the well is supposed to restore your spell "
                        "points, but you are already at maximum.",
                        NORMAL_DIALOG_INFO,
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
            } else {
                NormalDialog(
                    "{Magic Well}\n\nA second drink at the well in one day will not help you.",
                    NORMAL_DIALOG_INFO,
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
            break;

        case MAP_OBJECT_COAST:
            if (HAS(eventHero2->m_eventFlags, HERO_EVENT_EMBARKED)) {
                eventHero2->m_eventFlags = HeroEventFlag(
                    static_cast<i32>(eventHero2->m_eventFlags) & ~IDX(HERO_EVENT_EMBARKED)
                );
                eventHero2->m_remainingMobility = 0;
                eventHero2->m_direction = m_cursorDirection;
                m_cursorType = eventHero2->m_cursorType;
                m_cursorFrame = GetCursorBaseFrame(m_cursorDirection);
                m_cursorActive = 1;
                playedSample3 = LoadPlaySample("killfade.82m");
                gpWindowManager->SaveFizzleSource(
                    COAST_FIZZLE_X,
                    COAST_FIZZLE_Y,
                    COAST_FIZZLE_WIDTH,
                    COAST_FIZZLE_HEIGHT
                );
                CompleteDraw(m_mapOriginX, m_mapOriginY, 0, 1);
                gpWindowManager->FizzleForward(
                    COAST_FIZZLE_X,
                    COAST_FIZZLE_Y,
                    COAST_FIZZLE_WIDTH,
                    COAST_FIZZLE_HEIGHT,
                    -1,
                    NULL,
                    NULL
                );
                WaitEndSample(&playedSample3, -1);
                CheckAdjacentMon(&adjacentMonster8);
            }
            break;

        case MAP_OBJECT_BOAT:
            boat_j = &gpGame->m_boats[cell->m_objectMetadata];
            gpGame->RestoreCell(
                -1,
                -1,
                boat_j->savedTriggerType,
                boat_j->savedEventData,
                cell,
                BOAT_RESTORE_MODE
            );
            eventHero2->m_eventFlags = HeroEventFlag(
                static_cast<i32>(eventHero2->m_eventFlags) | IDX(HERO_EVENT_EMBARKED)
            );
            eventHero2->m_remainingMobility = 0;
            boat_j->heroId = eventHero2->m_id;
            boat_j->owner = eventHero2->m_owner;
            m_cursorType = HERO_TYPE_BOAT;
            m_cursorDirection = boat_j->direction;
            m_cursorFrame = GetCursorBaseFrame(m_cursorDirection);
            m_cursorActive = 1;
            CompleteDraw(m_mapOriginX, m_mapOriginY, 0, 1);
            UpdateScreen(0, 0);
            break;

        case MAP_OBJECT_MINE:
            if (gpGame->m_mineOwners[cell->m_objectMetadata] != giCurPlayer) {
                if (gpGame->m_mines[cell->m_objectMetadata].guardianType != CREATURE_NONE) {
                    mineCombatResult7 = CombatMonsterEvent(
                        eventHero2,
                        gpGame->m_mines[cell->m_objectMetadata].guardianType,
                        gpGame->m_mines[cell->m_objectMetadata].guardianCount,
                        cell,
                        x,
                        y,
                        0,
                        x,
                        y,
                        CREATURE_NONE,
                        0,
                        0,
                        CREATURE_NONE,
                        0,
                        0
                    );
                    if (mineCombatResult7 != COMBAT_RESULT_ATTACKER)
                        break;
                    gpGame->m_mines[cell->m_objectMetadata].guardianType = CREATURE_NONE;
                    eventHero2->CheckLevel();
                }
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                if (gpGame->m_mines[cell->m_objectMetadata].resourceType == MINE_TYPE_GOLD)
                    resourceAmount6 = MINE_GOLD_INCOME;
                else if (gpGame->m_mines[cell->m_objectMetadata].resourceType == MINE_TYPE_ORE)
                    resourceAmount6 = MINE_ORE_INCOME;
                else
                    resourceAmount6 = 1;
                EventWindow(
                    IDX(gpGame->m_mines[cell->m_objectMetadata].resourceType)
                        + MINE_RESOURCE_ICON_OFFSET,
                    NORMAL_DIALOG_INFO,
                    "",
                    IDX(gpGame->m_mines[cell->m_objectMetadata].resourceType),
                    -resourceAmount6,
                    -1,
                    0,
                    -1
                );
            claimMine:
                gpGame->ClaimMine(cell->m_objectMetadata, giCurPlayer);
            }
            break;

        case MAP_OBJECT_ALCHEMIST_LAB:
            if (gpGame->m_mineOwners[cell->m_objectMetadata] != giCurPlayer) {
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                EventWindow(
                    EVENT_TEXT_ALCHEMIST_CAPTURED,
                    NORMAL_DIALOG_INFO,
                    "",
                    IDX(RES_MERCURY),
                    NORMAL_DIALOG_NO_VALUE,
                    -1,
                    0,
                    -1
                );
                goto claimMine;
            }
            break;

        case MAP_OBJECT_SAWMILL:
            if (gpGame->m_mineOwners[cell->m_objectMetadata] != giCurPlayer) {
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                EventWindow(
                    EVENT_TEXT_SAWMILL_CAPTURED,
                    NORMAL_DIALOG_INFO,
                    "",
                    IDX(RES_WOOD),
                    -MINE_ORE_INCOME,
                    -1,
                    0,
                    -1
                );
                goto claimMine;
            }
            break;

        case MAP_OBJECT_LIGHTHOUSE:
            if (gpGame->m_mineOwners[cell->m_objectMetadata] != giCurPlayer) {
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                gpGame->ClaimMine(cell->m_objectMetadata, giCurPlayer);
                EventWindow(
                    EVENT_TEXT_LIGHTHOUSE_CAPTURED,
                    NORMAL_DIALOG_INFO,
                    "",
                    -1,
                    0,
                    -1,
                    0,
                    -1
                );
            }
            break;

        case MAP_OBJECT_TREASURE_CHEST:
            if (cell->m_objectMetadata & CHEST_ARTIFACT_FLAG) {
                if (eventHero2->NumArtifacts() >= EVENT_ARTIFACT_CAPACITY) {
                    cell->m_objectMetadata = CHEST_REWARD_SMALL;
                    goto chestGold;
                }
                eventValue1 = cell->m_objectMetadata & CHEST_ARTIFACT_MASK;
                sprintf(
                    gText,
                    "{Chest}\n\nAfter scouring the area, you fall upon a hidden chest, containing "
                    "the ancient artifact '%s'",
                    gArtifactNames[eventValue1]
                );
                NormalDialog(
                    gText,
                    NORMAL_DIALOG_INFO,
                    -1,
                    -1,
                    MAP_EVENT_REWARD_ARTIFACT,
                    eventValue1,
                    -1,
                    0,
                    -1,
                    0
                );
                GiveArtifact(eventHero2, ArtifactType(eventValue1), 1, -1);
            } else {
            chestGold:
                EventWindow(
                    -1,
                    NORMAL_DIALOG_CONFIRM,
                    "{Chest}\n\nAfter scouring the area, you fall upon a hidden treasure cache.  "
                    "You may take the gold or distribute the gold to the peasants for experience.  "
                    "Do you wish to keep the gold?",
                    IDX(RES_GOLD),
                    cell->m_objectMetadata * CHEST_GOLD_MULTIPLIER,
                    NORMAL_DIALOG_EXPERIENCE,
                    (cell->m_objectMetadata - CHEST_EXPERIENCE_LEVEL_OFFSET)
                        * CHEST_EXPERIENCE_MULTIPLIER,
                    NORMAL_DIALOG_SHOW_OR_TEXT
                );
                if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES)
                    GiveResource(
                        eventHero2,
                        RES_GOLD,
                        cell->m_objectMetadata * CHEST_GOLD_MULTIPLIER
                    );
                else
                    GiveExperience(
                        eventHero2,
                        (cell->m_objectMetadata - CHEST_EXPERIENCE_LEVEL_OFFSET)
                            * CHEST_EXPERIENCE_MULTIPLIER,
                        0
                    );
            }
            eraseObject = 1;
            fizzleType3 = 1;
            eventHero2->CheckLevel();
            break;

        case MAP_OBJECT_BUOY:
            if (!HAS(eventHero2->m_eventFlags, HERO_EVENT_BUOY)) {
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                eventHero2->m_eventFlags = HeroEventFlag(
                    static_cast<i32>(eventHero2->m_eventFlags) | IDX(HERO_EVENT_BUOY)
                );
                eventHero2->m_morale++;
                EventWindow(
                    EVENT_TEXT_BUOY_REWARD,
                    NORMAL_DIALOG_INFO,
                    "",
                    NORMAL_DIALOG_MORALE_BONUS,
                    0,
                    -1,
                    0,
                    -1
                );
            } else {
                EventWindow(EVENT_TEXT_BUOY_VISITED, NORMAL_DIALOG_INFO, "", -1, 0, -1, 0, -1);
            }
            break;

        case MAP_OBJECT_FAERIE_RING:
            if (!HAS(eventHero2->m_eventFlags, HERO_EVENT_FAERIE_RING)) {
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                eventHero2->m_eventFlags = HeroEventFlag(
                    static_cast<i32>(eventHero2->m_eventFlags) | IDX(HERO_EVENT_FAERIE_RING)
                );
                eventHero2->m_luck++;
                EventWindow(
                    EVENT_TEXT_FAERIE_RING_REWARD,
                    NORMAL_DIALOG_INFO,
                    "",
                    NORMAL_DIALOG_LUCK_BONUS,
                    0,
                    -1,
                    0,
                    -1
                );
            } else {
                EventWindow(
                    EVENT_TEXT_FAERIE_RING_VISITED,
                    NORMAL_DIALOG_INFO,
                    "",
                    -1,
                    0,
                    -1,
                    0,
                    -1
                );
            }
            break;

        case MAP_OBJECT_IDOL:
            if (!HAS(eventHero2->m_eventFlags, HERO_EVENT_IDOL)) {
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                eventHero2->m_eventFlags = HeroEventFlag(
                    static_cast<i32>(eventHero2->m_eventFlags) | IDX(HERO_EVENT_IDOL)
                );
                eventHero2->m_luck++;
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    "{Idol}\n\nYou've found an ancient and weathered stone idol.  Kissing it is "
                    "supposed to be lucky, so you do.  The stone is very cold to the touch.",
                    NORMAL_DIALOG_LUCK_BONUS,
                    0,
                    -1,
                    0,
                    -1
                );
            } else {
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    "{Idol}\n\nYou've found an ancient and weathered stone idol.  It is supposed "
                    "to grant luck to visitors, but since the stars are already smiling upon you, "
                    "it does nothing.",
                    -1,
                    0,
                    -1,
                    0,
                    -1
                );
            }
            break;

        case MAP_OBJECT_FOUNTAIN:
            if (!HAS(eventHero2->m_eventFlags, HERO_EVENT_FOUNTAIN)) {
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                eventHero2->m_eventFlags = HeroEventFlag(
                    static_cast<i32>(eventHero2->m_eventFlags) | IDX(HERO_EVENT_FOUNTAIN)
                );
                eventHero2->m_luck++;
                EventWindow(
                    EVENT_TEXT_FOUNTAIN_REWARD,
                    NORMAL_DIALOG_INFO,
                    "",
                    NORMAL_DIALOG_LUCK_BONUS,
                    0,
                    -1,
                    0,
                    -1
                );
            } else {
                EventWindow(EVENT_TEXT_FOUNTAIN_VISITED, NORMAL_DIALOG_INFO, "", -1, 0, -1, 0, -1);
            }
            break;

        case MAP_OBJECT_WATERING_HOLE:
            if (!HAS(eventHero2->m_eventFlags, HERO_EVENT_WATERING_HOLE)) {
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                eventHero2->m_eventFlags = HeroEventFlag(
                    static_cast<i32>(eventHero2->m_eventFlags) | IDX(HERO_EVENT_WATERING_HOLE)
                );
                eventHero2->m_morale++;
                eventHero2->m_mobility += WATERING_HOLE_MOBILITY_BONUS;
                eventHero2->m_remainingMobility += WATERING_HOLE_MOBILITY_BONUS;
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    "{Watering Hole}\n\nA drink at the watering hole fills your troops with "
                    "strength and lifts their spirits.  You can travel a bit further today.",
                    NORMAL_DIALOG_MORALE_BONUS,
                    0,
                    -1,
                    0,
                    -1
                );
            } else {
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    "{Watering Hole}\n\nThe drink at the watering hole is refreshing, but offers "
                    "no further benefit.  The watering hole might help again if you fought a "
                    "battle first.",
                    -1,
                    0,
                    -1,
                    0,
                    -1
                );
            }
            break;

        case MAP_OBJECT_OASIS:
            if (!HAS(eventHero2->m_eventFlags, HERO_EVENT_OASIS)) {
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                eventHero2->m_eventFlags = HeroEventFlag(
                    static_cast<i32>(eventHero2->m_eventFlags) | IDX(HERO_EVENT_OASIS)
                );
                eventHero2->m_morale++;
                eventHero2->m_mobility += OASIS_MOBILITY_BONUS;
                eventHero2->m_remainingMobility += OASIS_MOBILITY_BONUS;
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    "{Oasis}\n\nA drink at the oasis fills your troops with strength and lifts "
                    "their spirits.  You can travel a bit further today.",
                    NORMAL_DIALOG_MORALE_BONUS,
                    0,
                    -1,
                    0,
                    -1
                );
            } else {
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    "{Oasis}\n\nThe drink at the oasis is refreshing, but offers no further "
                    "benefit.  The oasis might help again if you fought a battle first.",
                    -1,
                    0,
                    -1,
                    0,
                    -1
                );
            }
            break;

        case MAP_OBJECT_TEMPLE:
            if (!HAS(eventHero2->m_eventFlags, HERO_EVENT_TEMPLE)) {
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                eventHero2->m_eventFlags = HeroEventFlag(
                    static_cast<i32>(eventHero2->m_eventFlags) | IDX(HERO_EVENT_TEMPLE)
                );
                eventHero2->m_morale += TEMPLE_MORALE_BONUS;
                NormalDialog(
                    "{Temple}\n\nA visit and a prayer at the temple raises the morale of your "
                    "troops.",
                    NORMAL_DIALOG_INFO,
                    -1,
                    -1,
                    NORMAL_DIALOG_MORALE_BONUS,
                    0,
                    NORMAL_DIALOG_MORALE_BONUS,
                    0,
                    -1,
                    0
                );
            } else {
                NormalDialog(
                    "{Temple}\n\nIt doesn't help to pray twice before a battle.  Come back after "
                    "you've fought.",
                    NORMAL_DIALOG_INFO,
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
            break;

        case MAP_OBJECT_LEAN_TO:
            if (!cell->m_objectMetadata) {
                NormalDialog(
                    "{Lean To}\n\nThe lean-to is long abandoned.  There is nothing of value here.",
                    NORMAL_DIALOG_INFO,
                    -1,
                    -1,
                    -1,
                    0,
                    -1,
                    0,
                    -1,
                    0
                );
            } else {
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                resourceType = static_cast<ResourceType>(
                    (cell->m_objectMetadata & CAMPFIRE_RESOURCE_MASK) - 1
                );
                resourceAmount6 =
                    (cell->m_objectMetadata & DAEMON_SERVANT_MASK) >> DAEMON_SERVANT_SHIFT;
                NormalDialog(
                    "{Lean To}\n\nYou've found an abandoned lean-to.  Poking about, you discover "
                    "some resources hidden nearby.",
                    NORMAL_DIALOG_INFO,
                    -1,
                    -1,
                    IDX(resourceType),
                    resourceAmount6,
                    -1,
                    0,
                    -1,
                    0
                );
                GiveResource(eventHero2, resourceType, resourceAmount6);
                cell->m_objectMetadata = 0;
            }
            break;

        case MAP_OBJECT_WAGON:
            if (cell->m_objectMetadata) {
                if (!(cell->m_objectMetadata & WAGON_ARTIFACT_FLAG)) {
                    resourceType = static_cast<ResourceType>(
                        (cell->m_objectMetadata & CAMPFIRE_RESOURCE_MASK) - 1
                    );
                    resourceAmount6 =
                        (cell->m_objectMetadata & DAEMON_SERVANT_MASK) >> DAEMON_SERVANT_SHIFT;
                    EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                    EventWindow(
                        -1,
                        NORMAL_DIALOG_INFO,
                        "{Wagon}\n\nYou come across an old wagon left by a trader who didn't quite "
                        "make it to safe terrain.  Inside, you find some of the wagon's cargo "
                        "still intact.",
                        IDX(resourceType),
                        resourceAmount6,
                        -1,
                        0,
                        -1
                    );
                    GiveResource(eventHero2, resourceType, resourceAmount6);
                    cell->m_objectMetadata = 0;
                    break;
                }
                if (eventHero2->NumArtifacts() != EVENT_ARTIFACT_CAPACITY) {
                    EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                    eventValue1 = cell->m_objectMetadata & WAGON_ARTIFACT_MASK;
                    sprintf(
                        gText,
                        "{Wagon}\n\nYou come across an old wagon left by a trader who didn't quite "
                        "make it to safe terrain.  Searching inside, you find the '%s'.",
                        gArtifactNames[eventValue1]
                    );
                    EventWindow(
                        -1,
                        NORMAL_DIALOG_INFO,
                        gText,
                        MAP_EVENT_REWARD_ARTIFACT,
                        eventValue1,
                        -1,
                        0,
                        -1
                    );
                    GiveArtifact(eventHero2, ArtifactType(eventValue1), 1, -1);
                    cell->m_objectMetadata = 0;
                    break;
                }
            }
            EventWindow(
                -1,
                NORMAL_DIALOG_INFO,
                "{Wagon}\n\nYou come across an old wagon left by a trader who didn't quite make it "
                "to safe terrain.  Unfortunately, others have found it first, and the wagon is "
                "empty.",
                -1,
                0,
                -1,
                0,
                -1
            );
            cell->m_objectMetadata = 0;
            break;

        case MAP_OBJECT_SKELETON: {
            switch (cell->m_objectMetadata) {
                case SKELETON_EMPTY:
                    EventWindow(
                        EVENT_TEXT_SKELETON_EMPTY,
                        NORMAL_DIALOG_INFO,
                        "",
                        -1,
                        0,
                        -1,
                        0,
                        -1
                    );
                    break;
                default:
                    EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                    if (eventHero2->NumArtifacts() == EVENT_ARTIFACT_CAPACITY) {
                        sprintf(gText, "%s.", "Treasure");
                        EventWindow(
                            -1,
                            NORMAL_DIALOG_INFO,
                            gText,
                            IDX(RES_GOLD),
                            SKELETON_GOLD,
                            -1,
                            0,
                            -1
                        );
                        GiveResource(eventHero2, RES_GOLD, SKELETON_GOLD);
                    } else {
                        eventValue1 = cell->m_objectMetadata - SKELETON_ARTIFACT_OFFSET;
                        sprintf(
                            gText,
                            s_twoStringFormat,
                            gEventText[EVENT_TEXT_SKELETON_REWARD],
                            gArtifactNames[eventValue1]
                        );
                        GiveArtifact(eventHero2, ArtifactType(eventValue1), 1, -1);
                        EventWindow(
                            -1,
                            NORMAL_DIALOG_INFO,
                            gText,
                            MAP_EVENT_REWARD_ARTIFACT,
                            eventValue1,
                            -1,
                            0,
                            -1
                        );
                    }
                    cell->m_objectMetadata = SKELETON_EMPTY;
                    break;
            }
            break;
        }

        case MAP_OBJECT_CAMPFIRE:
            EventWindow(
                EVENT_TEXT_CAMPFIRE,
                NORMAL_DIALOG_INFO,
                "",
                IDX(RES_GOLD),
                (cell->m_objectMetadata >> CAMPFIRE_AMOUNT_SHIFT) * CAMPFIRE_GOLD_MULTIPLIER,
                cell->m_objectMetadata & CAMPFIRE_RESOURCE_MASK,
                cell->m_objectMetadata >> CAMPFIRE_AMOUNT_SHIFT,
                -1
            );
            GiveResource(
                eventHero2,
                RES_GOLD,
                (cell->m_objectMetadata >> CAMPFIRE_AMOUNT_SHIFT) * CAMPFIRE_GOLD_MULTIPLIER
            );
            GiveResource(
                eventHero2,
                ResourceType(cell->m_objectMetadata & IDX(CAMPFIRE_RESOURCE_MASK)),
                cell->m_objectMetadata >> CAMPFIRE_AMOUNT_SHIFT
            );
            eraseObject = 1;
            fizzleType3 = 1;
            SetEnvironmentOrigin(
                m_mapOriginX + ENVIRONMENT_BORDER,
                m_mapOriginY + ENVIRONMENT_BORDER,
                1
            );
            break;

        case MAP_OBJECT_XANADU:
            if (eventHero2->m_xanaduVisits & (1 << cell->m_objectMetadata)) {
                NormalDialog(
                    "{Xanadu}\n\nRecognizing you, the butler refuses to admit you.  \"The "
                    "master,\" he says, \"will not see the same student twice.\"",
                    NORMAL_DIALOG_INFO,
                    -1,
                    -1,
                    -1,
                    0,
                    -1,
                    0,
                    -1,
                    0
                );
                break;
            }
            if (eventHero2->m_level
                    + IDX(eventHero2->m_secondarySkills[IDX(HERO_SKILL_DIPLOMACY)])
                          * XANADU_DIPLOMACY_MULTIPLIER
                < XANADU_ADMISSION_LEVEL) {
                NormalDialog(
                    "{Xanadu}\n\nThe butler opens the door and looks you up and down.  \"You "
                    "are neither famous nor diplomatic enough to be admitted to see my "
                    "master,\" he sniffs.  \"Come back when you think yourself worthy.\"",
                    NORMAL_DIALOG_INFO,
                    -1,
                    -1,
                    -1,
                    0,
                    -1,
                    0,
                    -1,
                    0
                );
            } else {
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                NormalDialog(
                    "{Xanadu}\n\nThe butler admits you to see the master of the house.  He "
                    "trains you in the four skills a hero should know.",
                    NORMAL_DIALOG_INFO,
                    -1,
                    -1,
                    -1,
                    0,
                    -1,
                    0,
                    -1,
                    0
                );
                eventHero2->m_primaryStats[IDX(HERO_PRIMARY_ATTACK)]++;
                eventHero2->m_primaryStats[IDX(HERO_PRIMARY_DEFENSE)]++;
                eventHero2->m_primaryStats[IDX(HERO_PRIMARY_KNOWLEDGE)]++;
                eventHero2->m_primaryStats[IDX(HERO_PRIMARY_SPELL_POWER)]++;
                eventHero2->m_xanaduVisits |= 1 << cell->m_objectMetadata;
            }
        

            break;

        case MAP_OBJECT_FORT:
            if (eventHero2->m_fortVisits & (1 << cell->m_objectMetadata)) {
                NormalDialog(
                    "{Fort}\n\n \"I'm sorry sir,\" The leader of the soldiers says, \"but you "
                    "already know everything we have to teach.\"",
                    NORMAL_DIALOG_INFO,
                    -1,
                    -1,
                    -1,
                    0,
                    -1,
                    0,
                    -1,
                    0
                );
                break;
            }
            EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
            NormalDialog(
                "{Fort}\n\n The soldiers living in the fort teach you a few new defensive "
                "tricks.",
                NORMAL_DIALOG_INFO,
                -1,
                -1,
                NORMAL_DIALOG_PRIMARY_SKILL,
                PRIMARY_BONUS_DEFENSE,
                -1,
                0,
                -1,
                0
            );
            eventHero2->m_primaryStats[IDX(HERO_PRIMARY_DEFENSE)]++;
            eventHero2->m_fortVisits |= 1 << cell->m_objectMetadata;
        

            break;

        case MAP_OBJECT_STANDING_STONES:
            if (eventHero2->m_standingStoneVisits & (1 << cell->m_objectMetadata)) {
                NormalDialog(
                    "{Standing Stones}\n\nYou've found a group of Druids worshipping at one of "
                    "their strange stone edifices.  Silently, the Druids turn you away, indicating "
                    "they have nothing new to teach you.",
                    NORMAL_DIALOG_INFO,
                    -1,
                    -1,
                    -1,
                    0,
                    -1,
                    0,
                    -1,
                    0
                );
                break;
            }
            EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
            NormalDialog(
                "{Standing Stones}\n\nYou've found a group of Druids worshipping at one of "
                "their strange stone edifices.  Silently, they teach you new ways to cast "
                "spells.",
                NORMAL_DIALOG_INFO,
                -1,
                -1,
                NORMAL_DIALOG_PRIMARY_SKILL,
                PRIMARY_BONUS_SPELL_POWER,
                -1,
                0,
                -1,
                0
            );
            eventHero2->m_primaryStats[IDX(HERO_PRIMARY_SPELL_POWER)]++;
            eventHero2->m_standingStoneVisits |= 1 << cell->m_objectMetadata;
        

            break;

        case MAP_OBJECT_WITCH_DOCTOR_HUT:
            if (eventHero2->m_witchDoctorVisits & (1 << cell->m_objectMetadata)) {
                NormalDialog(
                    "{Witch Doctor's Hut}\n\n\"Go 'way!\", the witch doctor barks, \"you know all "
                    "I know.\"",
                    NORMAL_DIALOG_INFO,
                    -1,
                    -1,
                    -1,
                    0,
                    -1,
                    0,
                    -1,
                    0
                );
                break;
            }
            EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
            NormalDialog(
                "{Witch Doctor's Hut}\n\nAn Orcish witch doctor living in the hut deepens your "
                "knowledge of magic by showing you how to cast stones, read portents, and "
                "decipher the intricacies of chicken entrails.",
                NORMAL_DIALOG_INFO,
                -1,
                -1,
                NORMAL_DIALOG_PRIMARY_SKILL,
                PRIMARY_BONUS_KNOWLEDGE,
                -1,
                0,
                -1,
                0
            );
            eventHero2->m_primaryStats[IDX(HERO_PRIMARY_KNOWLEDGE)]++;
            eventHero2->m_witchDoctorVisits |= 1 << cell->m_objectMetadata;
        

            break;

        case MAP_OBJECT_MERCENARY_CAMP:
            if (eventHero2->m_mercenaryCampVisits & (1 << cell->m_objectMetadata)) {
                NormalDialog(
                    "{Mercenary Camp}\n\nYou've come upon a mercenary camp practicing their "
                    "tactics.  \"You're too advanced for us,\" the mercenary captain says.  \"We "
                    "can teach nothing more.\"",
                    NORMAL_DIALOG_INFO,
                    -1,
                    -1,
                    -1,
                    0,
                    -1,
                    0,
                    -1,
                    0
                );
                break;
            }
            EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
            NormalDialog(
                "{Mercenary Camp}\n\nYou've come upon a mercenary camp practicing their "
                "tactics.  The mercenaries welcome you and your troops and invite you to train "
                "with them.",
                NORMAL_DIALOG_INFO,
                -1,
                -1,
                NORMAL_DIALOG_PRIMARY_SKILL,
                PRIMARY_BONUS_ATTACK,
                -1,
                0,
                -1,
                0
            );
            eventHero2->m_primaryStats[IDX(HERO_PRIMARY_ATTACK)]++;
            eventHero2->m_mercenaryCampVisits |= 1 << cell->m_objectMetadata;
        

            break;

        case MAP_OBJECT_GAZEBO:
            if (eventHero2->m_gazeboVisits & (1 << cell->m_objectMetadata)) {
                EventWindow(EVENT_TEXT_GAZEBO_VISITED, NORMAL_DIALOG_INFO, "", -1, 0, -1, 0, -1);
                break;
            }
            EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
            EventWindow(
                EVENT_TEXT_GAZEBO_REWARD,
                NORMAL_DIALOG_INFO,
                "",
                NORMAL_DIALOG_EXPERIENCE,
                GAZEBO_EXPERIENCE,
                -1,
                0,
                -1
            );
            GiveExperience(eventHero2, GAZEBO_EXPERIENCE, 0);
            eventHero2->m_gazeboVisits |= 1 << cell->m_objectMetadata;
            eventHero2->CheckLevel();
        

            break;

        case MAP_OBJECT_WATER_WHEEL:
            if (!cell->m_objectMetadata) {
                EventWindow(EVENT_TEXT_WATER_WHEEL_EMPTY, NORMAL_DIALOG_INFO, "", -1, 0, -1, 0, -1);
            } else {
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                EventWindow(
                    EVENT_TEXT_WATER_WHEEL_REWARD,
                    NORMAL_DIALOG_INFO,
                    "",
                    IDX(RES_GOLD),
                    cell->m_objectMetadata * MAP_EVENT_GOLD_AMOUNT,
                    -1,
                    0,
                    -1
                );
                GiveResource(eventHero2, RES_GOLD, cell->m_objectMetadata * MAP_EVENT_GOLD_AMOUNT);
                cell->m_objectMetadata = 0;
            }
            break;

        case MAP_OBJECT_RESOURCE:
            resourceType = static_cast<ResourceType>(cell->m_objectIndex / 2);
            GiveResource(
                eventHero2,
                resourceType,
                resourceType == RES_GOLD ? cell->m_objectMetadata * CAMPFIRE_GOLD_MULTIPLIER
                                         : cell->m_objectMetadata
            );
            strcpy(sphinxAnswer_a, gResourceNames[IDX(resourceType)]);
            sphinxAnswer_a[0] += ' ';
            sprintf(gText, gEventText[EVENT_TEXT_RESOURCE_PICKUP], sphinxAnswer_a);
            BVResMsg(
                gText,
                resourceType,
                resourceType == RES_GOLD ? cell->m_objectMetadata * CAMPFIRE_GOLD_MULTIPLIER
                                         : cell->m_objectMetadata
            );
            eraseObject = 1;
            fizzleType3 = 1;
            break;

        case MAP_OBJECT_WINDMILL:
            if (cell->m_objectMetadata < SPHINX_RESOURCE_COUNT) {
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                EventWindow(
                    EVENT_TEXT_WINDMILL_REWARD,
                    NORMAL_DIALOG_INFO,
                    "",
                    cell->m_objectMetadata,
                    WINDMILL_RESOURCE_AMOUNT,
                    -1,
                    0,
                    -1
                );
                GiveResource(
                    eventHero2,
                    ResourceType(cell->m_objectMetadata),
                    WINDMILL_RESOURCE_AMOUNT
                );
                cell->m_objectMetadata = WINDMILL_EMPTY;
            } else {
                EventWindow(EVENT_TEXT_WINDMILL_EMPTY, NORMAL_DIALOG_INFO, "", -1, 0, -1, 0, -1);
            }
            break;

        case MAP_OBJECT_ANCIENT_LAMP:
            EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
            EventWindow(EVENT_TEXT_GENIE_LAMP, NORMAL_DIALOG_CONFIRM, "", -1, 0, -1, 0, -1);
            if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
                RecruitEvent(eventHero2, CREATURE_GENIE, cell);
                if (!cell->m_objectMetadata) {
                    eraseObject = 1;
                    fizzleType3 = 1;
                }
            }
            break;

        case MAP_OBJECT_TREE_CITY:
            if (!cell->m_objectMetadata) {
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    "{Tree City}\n\nYou've found a Sprite Tree City.  Unfortunately, none of the "
                    "Sprites living there wish to join an army.  Maybe next week.",
                    -1,
                    0,
                    -1,
                    0,
                    -1
                );
            } else {
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                EventWindow(
                    -1,
                    NORMAL_DIALOG_CONFIRM,
                    "{Tree City}\n\nSome of the Sprites living in the tree city are willing to "
                    "join your army for a price.  Do you want to recruit Sprites?",
                    -1,
                    0,
                    -1,
                    0,
                    -1
                );
                if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES)
                    RecruitEvent(eventHero2, CREATURE_SPRITE, cell);
            }
            break;

        case MAP_OBJECT_RUINS:
            if (!cell->m_objectMetadata) {
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    "{Ruins}\n\nYou search the ruins, but the Medusas that used to live here are "
                    "gone.  Perhaps there will be more next week.",
                    -1,
                    0,
                    -1,
                    0,
                    -1
                );
            } else {
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                EventWindow(
                    -1,
                    NORMAL_DIALOG_CONFIRM,
                    "{Ruins}\n\nYou've found some Medusas living in the ruins.  They are willing "
                    "to join your army for a price.  Do you want to recruit Medusas?",
                    -1,
                    0,
                    -1,
                    0,
                    -1
                );
                if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES)
                    RecruitEvent(eventHero2, CREATURE_MEDUSA, cell);
            }
            break;

        case MAP_OBJECT_TROLL_BRIDGE:
            if (!cell->m_objectMetadata) {
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    "{Troll Bridge}\n\nYou've found one of those bridges that Trolls are so fond "
                    "of living under, but there are none here.  Perhaps there will be some next "
                    "week.",
                    -1,
                    0,
                    -1,
                    0,
                    -1
                );
            } else if (cell->m_objectMetadata & DWELLING_GUARDED_FLAG) {
                EventWindow(
                    -1,
                    NORMAL_DIALOG_CONFIRM,
                    "{Troll Bridge}\n\nTrolls living under the bridge challenge you.  Will you "
                    "fight them?",
                    -1,
                    0,
                    -1,
                    0,
                    -1
                );
                if (gpWindowManager->m_dialogResult != MONSTER_DIALOG_YES)
                    break;
                if (CombatMonsterEvent(
                        eventHero2,
                        CREATURE_TROLL,
                        TROLL_BRIDGE_TROLL_COUNT,
                        cell,
                        x,
                        y,
                        0,
                        x,
                        y,
                        CREATURE_WAR_TROLL,
                        TROLL_BRIDGE_WAR_TROLL_COUNT,
                        TROLL_BRIDGE_WAR_TROLL_STACKS,
                        CREATURE_NONE,
                        0,
                        0
                    )
                    != COMBAT_RESULT_ATTACKER)
                    break;
                eventHero2->CheckLevel();
                cell->m_objectMetadata -= DWELLING_GUARDED_FLAG;
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                EventWindow(
                    -1,
                    NORMAL_DIALOG_CONFIRM,
                    "{Troll Bridge}\n\nA few Trolls remain, cowering under the bridge.  They "
                    "approach you and offer to join your forces as mercenaries.  Do you want to "
                    "buy any Trolls?",
                    -1,
                    0,
                    -1,
                    0,
                    -1
                );
                if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES)
                    goto recruitTroll;
            } else {
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                EventWindow(
                    -1,
                    NORMAL_DIALOG_CONFIRM,
                    "{Troll Bridge}\n\nSome Trolls living under a bridge are willing to join your "
                    "army, but for a price.  Do you want to recruit Trolls?",
                    -1,
                    0,
                    -1,
                    0,
                    -1
                );
                if (gpWindowManager->m_dialogResult != MONSTER_DIALOG_YES)
                    break;
            recruitTroll:
                RecruitEvent(eventHero2, CREATURE_TROLL, cell);
            }
            break;

        case MAP_OBJECT_CITY_OF_DEAD:
            if (!cell->m_objectMetadata) {
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    "{City of the Dead}\n\nThe City of the Dead is empty of life, and empty of "
                    "unlife as well.  Perhaps some undead will move in next week.",
                    -1,
                    0,
                    -1,
                    0,
                    -1
                );
            } else if (cell->m_objectMetadata & DWELLING_GUARDED_FLAG) {
                EventWindow(
                    -1,
                    NORMAL_DIALOG_CONFIRM,
                    "{City of the Dead}\n\nYou've found the ruins of an ancient city, now "
                    "inhabited solely by the undead.  Will you search?",
                    -1,
                    0,
                    -1,
                    0,
                    -1
                );
                if (gpWindowManager->m_dialogResult != MONSTER_DIALOG_YES)
                    break;
                if (CombatMonsterEvent(
                        eventHero2,
                        CREATURE_VAMPIRE_LORD,
                        CITY_DEAD_VAMPIRE_LORD_COUNT,
                        cell,
                        x,
                        y,
                        0,
                        x,
                        y,
                        CREATURE_ZOMBIE,
                        CITY_DEAD_ZOMBIE_COUNT,
                        CITY_DEAD_ZOMBIE_STACKS,
                        CREATURE_POWER_LICH,
                        CITY_DEAD_POWER_LICH_COUNT,
                        CITY_DEAD_POWER_LICH_STACKS
                    )
                    != COMBAT_RESULT_ATTACKER)
                    break;
                eventHero2->CheckLevel();
                cell->m_objectMetadata -= DWELLING_GUARDED_FLAG;
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                EventWindow(
                    -1,
                    NORMAL_DIALOG_CONFIRM,
                    "{City of the Dead}\n\nSome of the surviving Liches are impressed by your "
                    "victory over their fellows, and offer to join you for a price.  Do you want "
                    "to recruit Liches?",
                    -1,
                    0,
                    -1,
                    0,
                    -1
                );
                if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES)
                    goto recruitLich;
            } else {
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                EventWindow(
                    -1,
                    NORMAL_DIALOG_CONFIRM,
                    "{City of the Dead}\n\nSome Liches living here are willing to join your army "
                    "for a price.  Do you want to recruit Liches?",
                    -1,
                    0,
                    -1,
                    0,
                    -1
                );
                if (gpWindowManager->m_dialogResult != MONSTER_DIALOG_YES)
                    break;
            recruitLich:
                RecruitEvent(eventHero2, CREATURE_POWER_LICH, cell);
            }
            break;

        case MAP_OBJECT_DRAGON_CITY:
            if (!cell->m_objectMetadata) {
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    "{Dragon City}\n\nThe Dragon city has no Dragons willing to join you this "
                    "week.  Perhaps a Dragon will become available next week.",
                    -1,
                    0,
                    -1,
                    0,
                    -1
                );
            } else if (cell->m_objectMetadata & DWELLING_GUARDED_FLAG) {
                EventWindow(
                    -1,
                    NORMAL_DIALOG_CONFIRM,
                    "{Dragon City}\n\nYou stand before the Dragon City, a place off-limits to mere "
                    "humans.  Do you wish to violate this rule and challenge the Dragons to a "
                    "fight?",
                    -1,
                    0,
                    -1,
                    0,
                    -1
                );
                if (gpWindowManager->m_dialogResult != MONSTER_DIALOG_YES)
                    break;
                dragonFactor_d = DRAGON_CITY_BASE_FACTOR;
                if (gbInCampaign && gpGame->m_campaignType == CAMPAIGN_ARCHIBALD
                    && gpGame->m_campaignScenario == DRAGON_CITY_ARCHIBALD_SCENARIO)
                    dragonFactor_d = DRAGON_CITY_CAMPAIGN_FACTOR;
                if (CombatMonsterEvent(
                        eventHero2,
                        CREATURE_GREEN_DRAGON,
                        dragonFactor_d * DRAGON_CITY_GREEN_COUNT_FACTOR,
                        cell,
                        x,
                        y,
                        0,
                        x,
                        y,
                        CREATURE_RED_DRAGON,
                        dragonFactor_d,
                        DRAGON_CITY_DRAGON_STACKS,
                        CREATURE_BLACK_DRAGON,
                        dragonFactor_d,
                        DRAGON_CITY_DRAGON_STACKS
                    )
                    != COMBAT_RESULT_ATTACKER)
                    break;
                CheckEndGame(END_GAME_FORCE_NONE, true);
                if (gbGameOver)
                    break;
                eventHero2->CheckLevel();
                cell->m_objectMetadata -= DWELLING_GUARDED_FLAG;
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                EventWindow(
                    -1,
                    NORMAL_DIALOG_CONFIRM,
                    "{Dragon City}\n\nHaving defeated the Dragon champions, the city's leaders "
                    "agree to supply some Dragons to your army for a price.  Do you wish to "
                    "recruit Dragons?",
                    -1,
                    0,
                    -1,
                    0,
                    -1
                );
                if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES)
                    goto recruitDragon;
            } else {
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                EventWindow(
                    -1,
                    NORMAL_DIALOG_CONFIRM,
                    "{Dragon City}\n\nThe Dragon city is willing to offer some Dragons for your "
                    "army for a price.  Do you wish to recruit Dragons?",
                    -1,
                    0,
                    -1,
                    0,
                    -1
                );
                if (gpWindowManager->m_dialogResult != MONSTER_DIALOG_YES)
                    break;
            recruitDragon:
                RecruitEvent(eventHero2, CREATURE_RED_DRAGON, cell);
            }
            break;

        case MAP_OBJECT_WAGON_CAMP:
            if (!cell->m_objectMetadata) {
                EventWindow(EVENT_TEXT_WAGON_EMPTY, NORMAL_DIALOG_INFO, "", -1, 0, -1, 0, -1);
            } else {
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                EventWindow(EVENT_TEXT_WAGON_RECRUIT, NORMAL_DIALOG_CONFIRM, "", -1, 0, -1, 0, -1);
                if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES)
                    RecruitEvent(eventHero2, CREATURE_ROGUE, cell);
            }
            break;

        case MAP_OBJECT_DESERT_TENT:
            if (!cell->m_objectMetadata) {
                EventWindow(EVENT_TEXT_DESERT_TENT_EMPTY, NORMAL_DIALOG_INFO, "", -1, 0, -1, 0, -1);
            } else {
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                EventWindow(
                    EVENT_TEXT_DESERT_TENT_RECRUIT,
                    NORMAL_DIALOG_CONFIRM,
                    "",
                    -1,
                    0,
                    -1,
                    0,
                    -1
                );
                if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES)
                    RecruitEvent(eventHero2, CREATURE_NOMAD, cell);
            }
            break;

        case MAP_OBJECT_ARCHER_HOUSE:
        case MAP_OBJECT_GOBLIN_HUT:
        case MAP_OBJECT_DWARF_COTTAGE:
        case MAP_OBJECT_PEASANT_HUT:
        case MAP_OBJECT_LOG_CABIN:
        case MAP_OBJECT_WATCH_TOWER:
        case MAP_OBJECT_TREE_HOUSE:
        case MAP_OBJECT_SIRENS:
        case MAP_OBJECT_HALFLING_HOLE:
        case MAP_OBJECT_EXCAVATION:
        case MAP_OBJECT_CAVE:
            if (cell->m_objectMetadata)
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
            HouseEvent(eventHero2, cell);
            break;

        case MAP_OBJECT_MONSTER:
            PlayerMonsterInteract(cell, cell, eventHero2, &eraseObject, x, y, 0, x, y);
            break;

        case MAP_OBJECT_OBELISK:
            if (!(gpGame->m_obeliskVisitors[cell->m_objectMetadata] & (1 << eventHero2->m_owner))) {
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                gpGame->m_obeliskVisitors[cell->m_objectMetadata] |= 1 << eventHero2->m_owner;
                EventWindow(EVENT_TEXT_OBELISK_REWARD, NORMAL_DIALOG_INFO, "", -1, 0, -1, 0, -1);
                ViewPuzzle();
            } else {
                EventWindow(EVENT_TEXT_OBELISK_VISITED, NORMAL_DIALOG_INFO, "", -1, 0, -1, 0, -1);
            }
            break;

        case MAP_OBJECT_TREE_OF_KNOWLEDGE: {
            if (!(eventHero2->m_treeKnowledgeVisits & (1 << cell->m_objectMetadata))) {
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                heroLevel2 = eventHero2->GetLevel(eventHero2->m_experience);
                levelExperience1 = eventHero2->GetExperience(heroLevel2 + 1)
                                   - eventHero2->GetExperience(heroLevel2);
                switch (cell->m_objectMetadata >> TREE_KNOWLEDGE_MODE_SHIFT) {
                    case TREE_KNOWLEDGE_FREE:
                        NormalDialog(
                            "{Tree of Knowledge}\n\nUpon your approach, the tree opens its eyes in "
                            "delight.  \"Ahh, an adventurer!  Allow me to teach you a little of "
                            "what I have learned over the ages.\"",
                            NORMAL_DIALOG_INFO,
                            -1,
                            -1,
                            NORMAL_DIALOG_EXPERIENCE,
                            -1,
                            -1,
                            0,
                            -1,
                            0
                        );
                        GiveExperience(eventHero2, levelExperience1, 0);
                        eventHero2->m_treeKnowledgeVisits |= 1 << cell->m_objectMetadata;
                        break;

                    case TREE_KNOWLEDGE_GOLD:
                        if (gpCurPlayer->m_resources[IDX(RES_GOLD)] < TREE_KNOWLEDGE_GOLD_COST) {
                            NormalDialog(
                                "{Tree of Knowledge}\n\nTears brim in the eyes of the tree.  \"I "
                                "need 2000 gold.\" it whispers. (sniff)  \"Well, come back when "
                                "you can pay me.\"",
                                NORMAL_DIALOG_INFO,
                                -1,
                                -1,
                                -1,
                                0,
                                -1,
                                0,
                                -1,
                                0
                            );
                        } else {
                            NormalDialog(
                                "{Tree of Knowledge}\n\nUpon your approach, the tree opens its "
                                "eyes in delight.  \"Ahh, an adventurer! I will be happy to teach "
                                "you a little of what I have learned over the ages for a mere 2000 "
                                "gold.\"  (Just bury it around my roots.)",
                                NORMAL_DIALOG_CONFIRM,
                                -1,
                                -1,
                                NORMAL_DIALOG_EXPERIENCE,
                                -1,
                                -1,
                                0,
                                -1,
                                0
                            );
                            if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
                                gpCurPlayer->m_resources[IDX(RES_GOLD)] -= TREE_KNOWLEDGE_GOLD_COST;
                                GiveExperience(eventHero2, levelExperience1, 0);
                                eventHero2->m_treeKnowledgeVisits |= 1 << cell->m_objectMetadata;
                            }
                        }
                        break;

                    case TREE_KNOWLEDGE_GEMS:
                        if (gpCurPlayer->m_resources[IDX(RES_GEMS)] < TREE_KNOWLEDGE_GEM_COST) {
                            NormalDialog(
                                "{Tree of Knowledge}\n\nTears brim in the eyes of the tree.  \"I "
                                "need 10 gems.\" it whispers. (sniff)  \"Well, come back when you "
                                "can pay me.\"",
                                NORMAL_DIALOG_INFO,
                                -1,
                                -1,
                                -1,
                                0,
                                -1,
                                0,
                                -1,
                                0
                            );
                        } else {
                            NormalDialog(
                                "{Tree of Knowledge}\n\nAhh, an adventurer! I will be happy to "
                                "teach you a little of what I have learned over the ages for a "
                                "mere 10 gems.\"  (Just bury it around my roots.)",
                                NORMAL_DIALOG_CONFIRM,
                                -1,
                                -1,
                                NORMAL_DIALOG_EXPERIENCE,
                                -1,
                                -1,
                                0,
                                -1,
                                0
                            );
                            if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
                                gpCurPlayer->m_resources[IDX(RES_GEMS)] -= TREE_KNOWLEDGE_GEM_COST;
                                GiveExperience(eventHero2, levelExperience1, 0);
                                eventHero2->m_treeKnowledgeVisits |= 1 << cell->m_objectMetadata;
                            }
                        }
                        break;
                }
            } else {
                NormalDialog(
                    "{Tree of Knowledge}\n\nUpon your approach, the tree opens its eyes in "
                    "delight.  \"It is good to see you, my student.  I hope my teachings have "
                    "helped you.\"",
                    NORMAL_DIALOG_INFO,
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
            eventHero2->CheckLevel();
            break;
        }

        case MAP_OBJECT_ORACLE:
            EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
            EventWindow(EVENT_TEXT_ORACLE, NORMAL_DIALOG_INFO, "", -1, 0, -1, 0, -1);
            oracleWindow = new heroWindow(0, 0, "thiefwin.bin");
            if (oracleWindow == NULL)
                MemError();
            SetWinText(oracleWindow, ORACLE_WINDOW_TEXT_ID);
            gpTownManager->SetupThievesGuild(oracleWindow, ORACLE_THIEVES_GUILD_RANK);
            strcpy(gText, "Shrine - Player Rankings");
            oracleMessage1.type = MESSAGE_WIDGET;
            oracleMessage1.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
            oracleMessage1.payload.widget.id = 0;
            oracleMessage1.payload.widget.data.text = gText;
            oracleWindow->BroadcastMessage(oracleMessage1);
            gpWindowManager->DoDialog(oracleWindow, TrueFalseDialogHandler, 0);
            delete oracleWindow;
            RedrawAdvScreen(1, 0);
            break;

        case MAP_OBJECT_SHRINE_FIRST_CIRCLE:
            eventValue1 = cell->m_objectMetadata - 1;
            sprintf(
                gText,
                "%s'%s'.  ",
                "{Shrine of the 1st Circle}\n\nYou come across a small shrine attended by a group "
                "of novice acolytes.  In exchange for your protection, they agree to teach you a "
                "simple spell - ",
                gSpellNames[cell->m_objectMetadata - 1]
            );
            goto shrineSpell;

        case MAP_OBJECT_SHRINE_SECOND_CIRCLE:
            eventValue1 = cell->m_objectMetadata - 1;
            sprintf(
                gText,
                "%s'%s'.  ",
                "{Shrine of the 2nd Circle}\n\nYou come across an ornate shrine attended by a "
                "group of rotund friars.  In exchange for your protection, they agree to teach you "
                "a spell - ",
                gSpellNames[cell->m_objectMetadata - 1]
            );
            goto shrineSpell;

        case MAP_OBJECT_SHRINE_THIRD_CIRCLE:
            eventValue1 = cell->m_objectMetadata - 1;
            sprintf(
                gText,
                "%s'%s'.  ",
                "{Shrine of the 3rd Circle}\n\nYou come across a lavish shrine attended by a group "
                "of high priests.  In exchange for your protection, they agree to teach you a "
                "sophisticated spell - ",
                gSpellNames[cell->m_objectMetadata - 1]
            );
        shrineSpell:
            if (eventHero2->HasArtifact(ARTIFACT_MAGIC_BOOK)) {
                if (static_cast<i32>(eventHero2->m_secondarySkills[IDX(HERO_SKILL_WISDOM)])
                        + SHRINE_WISDOM_BONUS
                    >= IDX(gsSpellInfo[eventValue1].level)) {
                    EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                    eventHero2->AddSpell(
                        static_cast<SpellType>(eventValue1),
                        eventHero2->Stats(HERO_PRIMARY_KNOWLEDGE)
                    );
                    EventWindow(
                        -1,
                        NORMAL_DIALOG_INFO,
                        gText,
                        NORMAL_DIALOG_SPELL,
                        eventValue1,
                        -1,
                        0,
                        -1
                    );
                } else {
                    strcat(
                        gText,
                        "Unfortunately, you do not have the wisdom to understand the spell, and "
                        "you are unable to learn it.  "
                    );
                    EventWindow(-1, NORMAL_DIALOG_INFO, gText, -1, 0, -1, 0, -1);
                }
            } else {
                strcat(gText, "Unfortunately, you have no Magic Book to record the spell with.");
                EventWindow(-1, NORMAL_DIALOG_INFO, gText, -1, 0, -1, 0, -1);
            }
            break;

        case MAP_OBJECT_CASTLE:
            TownEvent(cell, x, y);
            break;

        case MAP_OBJECT_WHIRLPOOL:
            EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
            DoWhirlpool(eventHero2);

        findTeleportDestination:
            teleportCount = 0;
            for (teleportY = 0; teleportY < MAP_HEIGHT; teleportY++) {
                for (teleportX3 = 0; teleportX3 < MAP_WIDTH; teleportX3++) {
                    if ((gpGame->m_worldMap.GetCell(teleportX3, teleportY))->m_triggerType
                            == static_cast<u8>(eventType_g | MAP_TRIGGER_ACTION_FLAG)
                        && (gpGame->m_worldMap.GetCell(teleportX3, teleportY))->m_objectIndex
                               == cell->m_objectIndex
                        && abs(teleportX3 - x) + abs(teleportY - y)
                               > (eventType_g == MAP_OBJECT_STONE_LITHS ? STONE_LITHS_MIN_DISTANCE
                                                                        : WHIRLPOOL_MIN_DISTANCE)) {
                        teleportCount++;
                    }
                }
            }
            if (teleportCount >= 1) {
                if (teleportCount > 1)
                    teleportCount = Random(1, teleportCount);
                for (teleportY = 0; teleportY < MAP_HEIGHT; teleportY++) {
                    for (teleportX3 = 0; teleportX3 < MAP_WIDTH; teleportX3++) {
                        if ((gpGame->m_worldMap.GetCell(teleportX3, teleportY))->m_triggerType
                                == static_cast<u8>(eventType_g | MAP_TRIGGER_ACTION_FLAG)
                            && (gpGame->m_worldMap.GetCell(teleportX3, teleportY))->m_objectIndex
                                   == cell->m_objectIndex
                            && (teleportX3 != x || teleportY != y)
                            && abs(teleportX3 - x) + abs(teleportY - y)
                                   > (eventType_g == MAP_OBJECT_STONE_LITHS
                                          ? STONE_LITHS_MIN_DISTANCE
                                          : WHIRLPOOL_MIN_DISTANCE)
                            && --teleportCount < 1) {
                            goto teleportHero;
                        }
                    }
                }
            teleportHero:
                StopCursor(1);
                gpAdvManager->TeleportTo(eventHero2, teleportX3, teleportY, 1, 0);
            }
            break;

        case MAP_OBJECT_STONE_LITHS:
            EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
            goto findTeleportDestination;

        case MAP_OBJECT_ARTIFACT: {
            artifactResourceType = (cell->m_objectMetadata & ARTIFACT_EVENT_RESOURCE_MASK)
                                   >> ARTIFACT_EVENT_RESOURCE_SHIFT;
            artifact8 = static_cast<ArtifactType>(cell->m_objectIndex / 2);
            guardedMonster5 =
                static_cast<CreatureType>(cell->m_objectMetadata & ARTIFACT_EVENT_MONSTER_MASK);
            if (eventHero2->NumArtifacts() == EVENT_ARTIFACT_CAPACITY) {
                NormalDialog(
                    "You cannot pick up this artifact, you already have a full load!",
                    NORMAL_DIALOG_INFO,
                    -1,
                    -1,
                    -1,
                    0,
                    -1,
                    0,
                    -1,
                    0
                );
                break;
            }

            if (xIsPlayingExpansionCampaign && xCampaign.IsSpecialGoldenBow(x, y)) {
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                strcpy(
                    gText,
                    "The fabled golden bow of the elves lies here in the dust.  You take it and "
                    "journey back to the elven towns.  They shower you with their graciousness and "
                    "the king promises that his people will aid you whenever you seek help."
                );
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    gText,
                    MAP_EVENT_REWARD_ARTIFACT,
                    IDX(artifact8),
                    -1,
                    0,
                    -1
                );
                GiveArtifact(eventHero2, artifact8, 1, static_cast<i8>(cell->m_objectMetadata));
                eraseObject = 1;
                fizzleType3 = 1;
                break;
            }

            if (artifact8 == ARTIFACT_SPELL_SCROLL) {
                EventSound(eventType_g, 1, &eventSample_f);
                xTheSpell = cell->m_objectMetadata;
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    gArtifactEvent[IDX(artifact8)],
                    MAP_EVENT_REWARD_ARTIFACT,
                    IDX(artifact8),
                    -1,
                    0,
                    -1
                );
                GiveArtifact(eventHero2, artifact8, 1, static_cast<i8>(cell->m_objectMetadata));
                eraseObject = 1;
                fizzleType3 = 1;
                break;
            }

            if (!(cell->m_objectMetadata & ARTIFACT_EVENT_GUARDED_FLAG)) {
                switch (cell->m_objectMetadata & ARTIFACT_EVENT_MODE_MASK) {
                    case ARTIFACT_EVENT_MODE_PICKUP:
                    artifactPickup:
                        EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                        EventWindow(
                            -1,
                            NORMAL_DIALOG_INFO,
                            gArtifactEvent[IDX(artifact8)],
                            MAP_EVENT_REWARD_ARTIFACT,
                            IDX(artifact8),
                            -1,
                            0,
                            -1
                        );
                        goto giveArtifact;

                    case ARTIFACT_EVENT_MODE_GOLD:
                        EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                        sprintf(
                            gText,
                            "{Artifact}\n\nA leprechaun offers you the %s for the small price of "
                            "2000 gold.  Do you wish to buy this artifact?",
                            gArtifactNames[IDX(artifact8)]
                        );
                        EventWindow(
                            -1,
                            NORMAL_DIALOG_CONFIRM,
                            gText,
                            MAP_EVENT_REWARD_ARTIFACT,
                            IDX(artifact8),
                            -1,
                            0,
                            -1
                        );
                        if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
                            if (gpGame->m_players[eventHero2->m_owner].m_resources[IDX(RES_GOLD)]
                                >= ARTIFACT_EVENT_GOLD_COST) {
                                gpGame->m_players[eventHero2->m_owner].m_resources[IDX(RES_GOLD)] -=
                                    ARTIFACT_EVENT_GOLD_COST;
                                goto giveArtifact;
                            }
                            NormalDialog(
                                "You try to pay the leprechaun, but realize that you can't afford "
                                "it.  The leprechaun stamps his foot and ignores you.",
                                NORMAL_DIALOG_INFO,
                                -1,
                                -1,
                                -1,
                                0,
                                -1,
                                0,
                                -1,
                                0
                            );
                        } else {
                            NormalDialog(
                                "Insulted by your refusal of his generous offer, the leprechaun "
                                "stamps his foot and ignores you.",
                                NORMAL_DIALOG_INFO,
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
                        break;

                    case ARTIFACT_EVENT_MODE_WISDOM:
                        if (eventHero2->m_secondarySkills[IDX(HERO_SKILL_WISDOM)]
                            != HERO_SKILL_LEVEL_NONE)
                            goto artifactPickup;
                        sprintf(
                            gText,
                            "{Artifact}\n\nYou've found the humble dwelling of a withered hermit.  "
                            "The hermit tells you that he is willing to give the %s to the first "
                            "wise person he meets.",
                            gArtifactNames[IDX(artifact8)]
                        );
                        NormalDialog(gText, NORMAL_DIALOG_INFO, -1, -1, -1, 0, -1, 0, -1, 0);
                        break;

                    case ARTIFACT_EVENT_MODE_LEADERSHIP:
                        if (eventHero2->m_secondarySkills[IDX(HERO_SKILL_LEADERSHIP)]
                            != HERO_SKILL_LEVEL_NONE)
                            goto artifactPickup;
                        sprintf(
                            gText,
                            "{Artifact}\n\nYou've come across the spartan quarters of a retired "
                            "soldier.  The soldier tells you that he is willing to pass on the %s "
                            "to the first true leader he meets.",
                            gArtifactNames[IDX(artifact8)]
                        );
                        NormalDialog(gText, NORMAL_DIALOG_INFO, -1, -1, -1, 0, -1, 0, -1, 0);
                        break;

                    case ARTIFACT_EVENT_MODE_RESOURCE_3:
                        EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                        sprintf(sphinxAnswer_a, gResourceNames[artifactResourceType]);
                        sphinxAnswer_a[0] += ' ';
                        sprintf(
                            gText,
                            "{Artifact}\n\nA leprechaun offers you the %s for the small price of "
                            "2500 gold and 3 %s.  Do you wish to buy this artifact?",
                            gArtifactNames[IDX(artifact8)],
                            sphinxAnswer_a
                        );
                        NormalDialog(
                            gText,
                            NORMAL_DIALOG_CONFIRM,
                            -1,
                            -1,
                            MAP_EVENT_REWARD_ARTIFACT,
                            IDX(artifact8),
                            -1,
                            0,
                            -1,
                            0
                        );
                        if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
                            if (gpGame->m_players[eventHero2->m_owner].m_resources[IDX(RES_GOLD)]
                                    >= ARTIFACT_EVENT_RESOURCE_3_GOLD_COST
                                && gpGame->m_players[eventHero2->m_owner]
                                           .m_resources[artifactResourceType]
                                       >= ARTIFACT_EVENT_RESOURCE_3_AMOUNT) {
                                gpGame->m_players[eventHero2->m_owner].m_resources[IDX(RES_GOLD)] -=
                                    ARTIFACT_EVENT_RESOURCE_3_GOLD_COST;
                                gpGame->m_players[eventHero2->m_owner]
                                    .m_resources[artifactResourceType] -=
                                    ARTIFACT_EVENT_RESOURCE_3_AMOUNT;
                                goto giveArtifact;
                            }
                            NormalDialog(
                                "You try to pay the leprechaun, but realize that you can't afford "
                                "it.  The leprechaun stamps his foot and ignores you.",
                                NORMAL_DIALOG_INFO,
                                -1,
                                -1,
                                -1,
                                0,
                                -1,
                                0,
                                -1,
                                0
                            );
                        } else {
                            NormalDialog(
                                "Insulted by your refusal of his generous offer, the leprechaun "
                                "stamps his foot and ignores you.",
                                NORMAL_DIALOG_INFO,
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
                        break;

                    case ARTIFACT_EVENT_MODE_RESOURCE_5:
                        EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                        sprintf(sphinxAnswer_a, gResourceNames[artifactResourceType]);
                        sphinxAnswer_a[0] += ' ';
                        sprintf(
                            gText,
                            "{Artifact}\n\nA leprechaun offers you the %s for the small price of "
                            "3000 gold and 5 %s.  Do you wish to buy this artifact?",
                            gArtifactNames[IDX(artifact8)],
                            sphinxAnswer_a
                        );
                        NormalDialog(
                            gText,
                            NORMAL_DIALOG_CONFIRM,
                            -1,
                            -1,
                            MAP_EVENT_REWARD_ARTIFACT,
                            IDX(artifact8),
                            -1,
                            0,
                            -1,
                            0
                        );
                        if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
                            if (gpGame->m_players[eventHero2->m_owner].m_resources[IDX(RES_GOLD)]
                                    >= ARTIFACT_EVENT_RESOURCE_5_GOLD_COST
                                && gpGame->m_players[eventHero2->m_owner]
                                           .m_resources[artifactResourceType]
                                       >= ARTIFACT_EVENT_RESOURCE_5_AMOUNT) {
                                gpGame->m_players[eventHero2->m_owner].m_resources[IDX(RES_GOLD)] -=
                                    ARTIFACT_EVENT_RESOURCE_5_GOLD_COST;
                                gpGame->m_players[eventHero2->m_owner]
                                    .m_resources[artifactResourceType] -=
                                    ARTIFACT_EVENT_RESOURCE_5_AMOUNT;
                                goto giveArtifact;
                            }
                            NormalDialog(
                                "You try to pay the leprechaun, but realize that you can't afford "
                                "it.  The leprechaun stamps his foot and ignores you.",
                                NORMAL_DIALOG_INFO,
                                -1,
                                -1,
                                -1,
                                0,
                                -1,
                                0,
                                -1,
                                0
                            );
                        } else {
                            NormalDialog(
                                "Insulted by your refusal of his generous offer, the leprechaun "
                                "stamps his foot and ignores you.",
                                NORMAL_DIALOG_INFO,
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
                        break;
                }
            } else {
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                if (guardedMonster5 == CREATURE_ROGUE) {
                    NormalDialog(
                        "{Artifact}\n\nYou come upon an ancient artifact.  As you reach for it, a "
                        "pack of Rogues leap out of the brush to guard their stolen loot.",
                        NORMAL_DIALOG_INFO,
                        -1,
                        -1,
                        -1,
                        0,
                        -1,
                        0,
                        -1,
                        0
                    );
                    guardedCount = ARTIFACT_EVENT_GUARD_ROGUE_COUNT;
                } else {
                    guardedCount = 1;
                    sprintf(
                        gText,
                        "{Artifact}\n\nThrough a clearing you observe an ancient artifact.  "
                        "Unfortunately, it's guarded by a nearby %s.  Do you want to fight the %s "
                        "for the artifact?",
                        gArmyNamesPlural[IDX(guardedMonster5)],
                        gArmyNamesPlural[IDX(guardedMonster5)]
                    );
                    NormalDialog(gText, NORMAL_DIALOG_CONFIRM, -1, -1, -1, 0, -1, 0, -1, 0);
                    if (gpWindowManager->m_dialogResult != MONSTER_DIALOG_YES) {
                        NormalDialog(
                            "Discretion is the better part of valor, and you decide to avoid this "
                            "fight for today.",
                            NORMAL_DIALOG_INFO,
                            -1,
                            -1,
                            -1,
                            0,
                            -1,
                            0,
                            -1,
                            0
                        );
                        break;
                    }
                }
                if (CombatMonsterEvent(
                        eventHero2,
                        guardedMonster5,
                        guardedCount,
                        cell,
                        x,
                        y,
                        0,
                        x,
                        y,
                        CREATURE_NONE,
                        0,
                        0,
                        CREATURE_NONE,
                        0,
                        0
                    )
                    != COMBAT_RESULT_ATTACKER)
                    break;
                eventHero2->CheckLevel();
                sprintf(
                    gText,
                    "Victorious, you take your prize, the %s",
                    gArtifactNames[IDX(artifact8)]
                );
                NormalDialog(
                    gText,
                    NORMAL_DIALOG_INFO,
                    -1,
                    -1,
                    MAP_EVENT_REWARD_ARTIFACT,
                    cell->m_objectIndex / 2,
                    -1,
                    0,
                    -1,
                    0
                );
            giveArtifact:
                GiveArtifact(eventHero2, artifact8, 1, -1);
                eraseObject = 1;
                fizzleType3 = 1;
            }
            break;
        }

        case MAP_OBJECT_HERO_INTERACTION:
            DemobilizeCurrHero();
            otherHero6 = gpGame->GetHero(cell->m_objectMetadata);
            if (otherHero6->m_owner == giCurPlayer) {
                HeroSwap(eventHero2, otherHero6);
            } else {
                occupiedTown4 = NULL;
                if (otherHero6->m_locationType
                    == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_CASTLE)) {
                    occupiedTown4 = gpGame->GetTown(otherHero6->m_occupiedTown);
                    occupiedTown4->m_occupyingHeroId = otherHero6->m_id;
                }
                heroCombatResult3 = DoCombat(
                    x,
                    y,
                    eventHero2,
                    &eventHero2->m_army,
                    occupiedTown4,
                    otherHero6,
                    &otherHero6->m_army,
                    x,
                    y,
                    -1,
                    1
                );
                if (heroCombatResult3 == COMBAT_RESULT_ATTACKER && occupiedTown4 != NULL)
                    gpGame->ClaimTown(occupiedTown4->m_id, giCurPlayer, 0);
            }
            break;

        case MAP_OBJECT_BOTTLE:
            if (cell->m_objectMetadata) {
                signExtra_k = reinterpret_cast<signEventExtra*>(ppMapExtra[cell->m_objectMetadata]);
                if (strlen(signExtra_k->text) < SIGN_MINIMUM_TEXT_LENGTH)
                    EventWindow(
                        -1,
                        NORMAL_DIALOG_INFO,
                        cRandomSignText[(x < 0 ? -x : x) & SIGN_RANDOM_TEXT_MASK],
                        -1,
                        0,
                        -1,
                        0,
                        -1
                    );
                else
                    EventWindow(-1, NORMAL_DIALOG_INFO, signExtra_k->text, -1, 0, -1, 0, -1);
            }
            fizzleType3 = 1;
            eraseObject = 1;
            break;

        case MAP_OBJECT_SIGN:
            if (cell->m_objectMetadata) {
                signExtra_k = reinterpret_cast<signEventExtra*>(ppMapExtra[cell->m_objectMetadata]);
                if (strlen(signExtra_k->text) < SIGN_MINIMUM_TEXT_LENGTH)
                    EventWindow(
                        -1,
                        NORMAL_DIALOG_INFO,
                        cRandomSignText[(x < 0 ? -x : x) & SIGN_RANDOM_TEXT_MASK],
                        -1,
                        0,
                        -1,
                        0,
                        -1
                    );
                else
                    EventWindow(-1, NORMAL_DIALOG_INFO, signExtra_k->text, -1, 0, -1, 0, -1);
            }
            break;

        case MAP_OBJECT_DAEMON_CAVE: {
            EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
            EventWindow(
                -1,
                NORMAL_DIALOG_CONFIRM,
                "{Daemon Cave}\n\nThe entrance to the cave is dark, and a foul, sulfurous smell "
                "issues from the cave mouth.  Will you enter?",
                -1,
                0,
                -1,
                0,
                -1
            );
            if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_NO)
                break;
            if (cell->m_objectMetadata == DAEMON_CAVE_EMPTY) {
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    "Except for evidence of a terrible battle, the cave is empty.",
                    -1,
                    0,
                    -1,
                    0,
                    -1
                );
                break;
            }

            monsterType6 = static_cast<CreatureType>(
                ((cell->m_objectMetadata & DAEMON_SERVANT_MASK) >> DAEMON_SERVANT_SHIFT)
                + DAEMON_SERVANT_BASE
            );
            sprintf(
                gText,
                "You find a powerful and grotesque Demon in the cave.  \"Today,\" it rasps, \"you "
                "will fight and surely die.  But I will give you a choice of deaths.  You may "
                "fight me, or you may fight my servants.  Do you prefer to fight my servants?\""
            );
            EventWindow(-1, NORMAL_DIALOG_CONFIRM, gText, -1, 0, -1, 0, -1);
            if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
                if (CombatMonsterEvent(
                        eventHero2,
                        monsterType6,
                        DAEMON_SERVANT_COUNT,
                        cell,
                        x,
                        y,
                        0,
                        x,
                        y,
                        CREATURE_NONE,
                        0,
                        0,
                        CREATURE_NONE,
                        0,
                        0
                    )
                    == COMBAT_RESULT_ATTACKER) {
                    eventHero2->CheckLevel();
                    NormalDialog(
                        "Upon defeating the daemon's servants, you find a hidden cache with 2500 "
                        "gold.",
                        NORMAL_DIALOG_INFO,
                        -1,
                        -1,
                        IDX(RES_GOLD),
                        DAEMON_GOLD,
                        -1,
                        0,
                        -1,
                        0
                    );
                    GiveResource(eventHero2, RES_GOLD, DAEMON_GOLD);
                    cell->m_objectMetadata = DAEMON_CAVE_EMPTY;
                }
                break;
            }

            switch (cell->m_objectMetadata & DAEMON_REWARD_MASK) {
                case DAEMON_REWARD_EXPERIENCE:
                    GiveExperience(eventHero2, DAEMON_EXPERIENCE, 0);
                    EventWindow(
                        -1,
                        NORMAL_DIALOG_INFO,
                        "The Demon screams its challenge and attacks!  After a short, desperate "
                        "battle, you slay the monster and receive 1,000 experience points.",
                        NORMAL_DIALOG_EXPERIENCE,
                        DAEMON_EXPERIENCE,
                        -1,
                        0,
                        -1
                    );
                    cell->m_objectMetadata = DAEMON_CAVE_EMPTY;
                    eventHero2->CheckLevel();
                    break;

                case DAEMON_REWARD_ARTIFACT:
                    if (eventHero2->NumArtifacts() == EVENT_ARTIFACT_CAPACITY
                        || gpGame->GetRandomArtifactId(ARTIFACT_LEVEL_RANDOM, true) == -1)
                        goto daemonExperienceGold;
                    GiveExperience(eventHero2, DAEMON_EXPERIENCE, 0);
                    eventValue1 = IDX(GiveRandomArtifact(eventHero2));
                    sprintf(
                        gText,
                        "The Demon screams its challenge and attacks!  After a short, desperate "
                        "battle, you slay the monster and find the %s in the back of the cave.",
                        gArtifactNames[eventValue1]
                    );
                    EventWindow(
                        -1,
                        NORMAL_DIALOG_INFO,
                        gText,
                        MAP_EVENT_REWARD_ARTIFACT,
                        eventValue1,
                        NORMAL_DIALOG_EXPERIENCE,
                        DAEMON_EXPERIENCE,
                        -1
                    );
                    cell->m_objectMetadata = DAEMON_CAVE_EMPTY;
                    eventHero2->CheckLevel();
                    break;

                case DAEMON_REWARD_EXPERIENCE_GOLD:
                daemonExperienceGold:
                    EventWindow(
                        -1,
                        NORMAL_DIALOG_INFO,
                        "The Demon screams its challenge and attacks!  After a short, desperate "
                        "battle, you slay the monster and receive 1,000 experience points and "
                        "2,500 gold.",
                        IDX(RES_GOLD),
                        DAEMON_GOLD,
                        NORMAL_DIALOG_EXPERIENCE,
                        DAEMON_EXPERIENCE,
                        -1
                    );
                    GiveExperience(eventHero2, DAEMON_EXPERIENCE, 0);
                    GiveResource(eventHero2, RES_GOLD, DAEMON_GOLD);
                    cell->m_objectMetadata = DAEMON_CAVE_EMPTY;
                    eventHero2->CheckLevel();
                    break;

                case DAEMON_REWARD_RANSOM:
                    EventWindow(
                        -1,
                        NORMAL_DIALOG_CONFIRM,
                        "The Demon leaps upon you and has its claws at your throat before you can "
                        "even draw your sword.  \"Your life is mine,\" it says.  \"I will sell it "
                        "back to you for 2,500 gold.\"",
                        -1,
                        0,
                        -1,
                        0,
                        -1
                    );
                    if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
                        if (gpGame->m_players[eventHero2->m_owner].m_resources[IDX(RES_GOLD)]
                            < DAEMON_GOLD) {
                            EventWindow(
                                -1,
                                NORMAL_DIALOG_INFO,
                                "Seeing that you do not have 2500 gold, the demon slashes you with "
                                "its claws, and the last thing you see is a red haze.",
                                -1,
                                0,
                                -1,
                                0,
                                -1
                            );
                            HeroLoses(eventHero2);
                        } else {
                            gpGame->m_players[eventHero2->m_owner].m_resources[IDX(RES_GOLD)] -=
                                DAEMON_GOLD;
                        }
                    } else {
                        HeroLoses(eventHero2);
                    }
                    break;
            }
            cell->m_objectMetadata = DAEMON_CAVE_EMPTY;
            break;
        }

        case MAP_OBJECT_SHIPWRECK: {
            EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
            EventWindow(
                -1,
                NORMAL_DIALOG_CONFIRM,
                "{Shipwreck}\n\nThe rotting hulk of a great pirate ship creaks eerily as it is "
                "pushed against the rocks.  Do you wish to search the shipwreck?",
                -1,
                0,
                -1,
                0,
                -1
            );
            if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
                switch (cell->m_objectMetadata) {
                    case SKELETON_EMPTY:
                        EventWindow(
                            -1,
                            NORMAL_DIALOG_INFO,
                            "Upon defeating the Ghosts you spend several hours sifting through the "
                            "debris and find nothing.  Such a despicable act reduces your army's "
                            "morale.",
                            NORMAL_DIALOG_MORALE_PENALTY,
                            0,
                            -1,
                            0,
                            -1
                        );
                        if (!HAS(eventHero2->m_eventFlags, HERO_EVENT_SHIPWRECK)) {
                            eventHero2->m_eventFlags =
                                eventHero2->m_eventFlags | HERO_EVENT_SHIPWRECK;
                            eventHero2->m_morale--;
                        }
                        break;
                    default:
                        if (GhostEvent(
                                eventHero2,
                                cell,
                                "Upon defeating the Ghosts you sift through the debris and find "
                                "something!",
                                x,
                                y
                            ))
                            cell->m_objectMetadata = SKELETON_EMPTY;
                        break;
                }
            }
            break;
        }

        case MAP_OBJECT_GRAVEYARD: {
            EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
            EventWindow(EVENT_TEXT_GRAVEYARD_PROMPT, NORMAL_DIALOG_CONFIRM, "", -1, 0, -1, 0, -1);
            if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
                switch (cell->m_objectMetadata) {
                    case SKELETON_EMPTY:
                        EventWindow(
                            EVENT_TEXT_GRAVEYARD_EMPTY,
                            NORMAL_DIALOG_INFO,
                            "",
                            NORMAL_DIALOG_MORALE_PENALTY,
                            0,
                            -1,
                            0,
                            -1
                        );
                        if (!HAS(eventHero2->m_eventFlags, HERO_EVENT_GRAVEYARD)) {
                            eventHero2->m_eventFlags =
                                eventHero2->m_eventFlags | HERO_EVENT_GRAVEYARD;
                            eventHero2->m_morale--;
                        }
                        break;
                    default:
                        zombieCell6 = GetCell(
                            x - normalDirTable[IDX(eventHero2->m_direction)].x,
                            y - normalDirTable[IDX(eventHero2->m_direction)].y
                        );
                        if (ZombieEvent(
                                eventHero2,
                                zombieCell6,
                                gEventText[EVENT_TEXT_GRAVEYARD_REWARD],
                                x,
                                y
                            ))
                            cell->m_objectMetadata = SKELETON_EMPTY;
                        break;
                }
            }
            break;
        }

        case MAP_OBJECT_DERELICT_SHIP: {
            EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
            EventWindow(
                -1,
                NORMAL_DIALOG_CONFIRM,
                "{Derelict Ship}\n\nThe rotting hulk of a great pirate ship creaks eerily as it is "
                "pushed against the rocks.  Do you wish to search the ship?",
                -1,
                0,
                -1,
                0,
                -1
            );
            if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
                switch (cell->m_objectMetadata) {
                    case SKELETON_EMPTY:
                        EventWindow(
                            -1,
                            NORMAL_DIALOG_INFO,
                            "Upon defeating the Skeletons you spend several hours sifting through "
                            "the debris and find nothing.  Such a despicable act reduces your "
                            "army's morale.",
                            NORMAL_DIALOG_MORALE_PENALTY,
                            0,
                            -1,
                            0,
                            -1
                        );
                        if (!HAS(eventHero2->m_eventFlags, HERO_EVENT_DERELICT_SHIP)) {
                            eventHero2->m_eventFlags =
                                eventHero2->m_eventFlags | HERO_EVENT_DERELICT_SHIP;
                            eventHero2->m_morale--;
                        }
                        break;
                    default:
                        skeletonCell1 = GetCell(
                            x - normalDirTable[IDX(eventHero2->m_direction)].x,
                            y - normalDirTable[IDX(eventHero2->m_direction)].y
                        );
                        if (SkeletonEvent(
                                eventHero2,
                                skeletonCell1,
                                "Upon defeating the Skeletons you sift through the debris and find "
                                "something!",
                                x,
                                y
                            ))
                            cell->m_objectMetadata = SKELETON_EMPTY;
                        break;
                }
            }
            break;
        }

        case MAP_OBJECT_PYRAMID:
            EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
            EventWindow(
                -1,
                NORMAL_DIALOG_CONFIRM,
                "You come upon the pyramid of a great and ancient king.  You are tempted to search "
                "it for treasure, but all the old stories warn of fearful curses and undead "
                "guardians.  Will you search?",
                -1,
                0,
                -1,
                0,
                -1
            );
            if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
                if (!cell->m_objectMetadata) {
                    NormalDialog(
                        "You come upon the pyramid of a great and ancient king.  Routine "
                        "exploration reveals that the pyramid is completely empty.",
                        NORMAL_DIALOG_INFO,
                        -1,
                        -1,
                        NORMAL_DIALOG_LUCK_PENALTY,
                        0,
                        NORMAL_DIALOG_LUCK_PENALTY,
                        0,
                        -1,
                        0
                    );
                    if (!HAS(eventHero2->m_eventFlags, HERO_EVENT_PYRAMID)) {
                        eventHero2->m_eventFlags = HeroEventFlag(
                            static_cast<i32>(eventHero2->m_eventFlags) | IDX(HERO_EVENT_PYRAMID)
                        );
                        eventHero2->m_luck -= PYRAMID_LUCK_PENALTY;
                    }
                } else {
                    if (CombatMonsterEvent(
                            eventHero2,
                            CREATURE_ROYAL_MUMMY,
                            PYRAMID_ROYAL_MUMMY_COUNT,
                            cell,
                            x,
                            y,
                            0,
                            x,
                            y,
                            CREATURE_VAMPIRE_LORD,
                            PYRAMID_VAMPIRE_LORD_COUNT,
                            PYRAMID_VAMPIRE_LORD_STACKS,
                            CREATURE_NONE,
                            0,
                            0
                        )
                        == COMBAT_RESULT_ATTACKER) {
                        eventHero2->CheckLevel();
                        eventValue1 = cell->m_objectMetadata - 1;
                        sprintf(
                            eventText,
                            "%s'%s'.  ",
                            "Upon defeating the monsters, you decipher an ancient glyph on the "
                            "wall, telling the secret of the spell - ",
                            gSpellNames[cell->m_objectMetadata - 1]
                        );
                        if (!eventHero2->HasArtifact(ARTIFACT_MAGIC_BOOK)) {
                            strcat(
                                eventText,
                                "  Unfortunately, you have no Magic Book to record the spell with."
                            );
                            EventWindow(-1, NORMAL_DIALOG_INFO, eventText, -1, 0, -1, 0, -1);
                        } else if (eventHero2->m_secondarySkills[IDX(HERO_SKILL_WISDOM)]
                                   < HERO_SKILL_LEVEL_EXPERT) {
                            strcat(
                                eventText,
                                "  Unfortunately, you do not have the wisdom to understand the "
                                "spell, and you are unable to learn it.  "
                            );
                            EventWindow(-1, NORMAL_DIALOG_INFO, eventText, -1, 0, -1, 0, -1);
                        } else {
                            eventHero2->AddSpell(
                                static_cast<SpellType>(eventValue1),
                                eventHero2->Stats(HERO_PRIMARY_KNOWLEDGE)
                            );
                            EventWindow(
                                -1,
                                NORMAL_DIALOG_INFO,
                                eventText,
                                NORMAL_DIALOG_SPELL,
                                eventValue1,
                                -1,
                                0,
                                -1
                            );
                        }
                        cell->m_objectMetadata = 0;
                    }
                }
            }
            break;

        case MAP_OBJECT_ABANDONED_MINE:
            EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
            EventWindow(
                -1,
                NORMAL_DIALOG_CONFIRM,
                "You come upon an abandoned gold mine.  The mine appears to be haunted.  Do you "
                "wish to enter?",
                -1,
                0,
                -1,
                0,
                -1
            );
            if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
                if (CombatMonsterEvent(
                        eventHero2,
                        gpGame->m_mines[cell->m_objectMetadata].guardianType,
                        gpGame->m_mines[cell->m_objectMetadata].guardianCount,
                        cell,
                        x,
                        y,
                        0,
                        x,
                        y,
                        CREATURE_NONE,
                        0,
                        0,
                        CREATURE_NONE,
                        0,
                        0
                    )
                    == COMBAT_RESULT_ATTACKER) {
                    eventHero2->CheckLevel();
                    EventWindow(
                        -1,
                        NORMAL_DIALOG_INFO,
                        "You beat the Ghosts and are able to restore the mine to production.",
                        -1,
                        0,
                        -1,
                        0,
                        -1
                    );
                    gpGame->ConvertObject(
                        x + ABANDONED_MINE_X_MIN,
                        y + ABANDONED_MINE_Y_TOP,
                        x + ABANDONED_MINE_X_MAX,
                        y + ABANDONED_MINE_Y_TOP,
                        ABANDONED_MINE_TILESET_TOP_A,
                        MINE_TOP_A_FIRST_FRAME,
                        MINE_TOP_A_LAST_FRAME,
                        ABANDONED_MINE_TILESET_BOTTOM_A,
                        MINE_BOTTOM_A_FRAME,
                        MAP_OBJECT_ABANDONED_MINE,
                        MAP_OBJECT_MINE
                    );
                    gpGame->ConvertObject(
                        x + ABANDONED_MINE_X_MIN,
                        y,
                        x + ABANDONED_MINE_X_MAX,
                        y,
                        ABANDONED_MINE_TILESET_TOP_A,
                        MINE_SECOND_A_FIRST_FRAME,
                        MINE_SECOND_A_LAST_FRAME,
                        ABANDONED_MINE_TILESET_BOTTOM_A,
                        MINE_SECOND_BOTTOM_A_FRAME,
                        MAP_OBJECT_ABANDONED_MINE,
                        MAP_OBJECT_MINE
                    );
                    gpGame->ConvertObject(
                        x + ABANDONED_MINE_X_MIN,
                        y + ABANDONED_MINE_Y_TOP,
                        x + ABANDONED_MINE_X_MAX,
                        y + ABANDONED_MINE_Y_TOP,
                        ABANDONED_MINE_TILESET_TOP_B,
                        MINE_TOP_B_FIRST_FRAME,
                        MINE_TOP_B_LAST_FRAME,
                        ABANDONED_MINE_TILESET_BOTTOM_B,
                        MINE_BOTTOM_B_FRAME,
                        MAP_OBJECT_ABANDONED_MINE,
                        MAP_OBJECT_MINE
                    );
                    gpGame->ConvertObject(
                        x + ABANDONED_MINE_X_MIN,
                        y,
                        x + ABANDONED_MINE_X_MAX,
                        y,
                        ABANDONED_MINE_TILESET_TOP_B,
                        MINE_SECOND_B_FIRST_FRAME,
                        MINE_SECOND_B_LAST_FRAME,
                        ABANDONED_MINE_TILESET_BOTTOM_B,
                        MINE_SECOND_BOTTOM_B_FRAME,
                        MAP_OBJECT_ABANDONED_MINE,
                        MAP_OBJECT_MINE
                    );
                    gpGame->ConvertObject(
                        x,
                        y,
                        x,
                        y,
                        ABANDONED_MINE_TILESET_CENTER,
                        MINE_CENTER_GHOST_FRAME,
                        MINE_CENTER_GHOST_FRAME,
                        ABANDONED_MINE_TILESET_CENTER,
                        MINE_CENTER_GOLD_FRAME,
                        MAP_OBJECT_ABANDONED_MINE,
                        MAP_OBJECT_MINE
                    );
                    gpGame->m_mines[cell->m_objectMetadata].resourceType = MINE_TYPE_GOLD;
                    gpGame->m_mines[cell->m_objectMetadata].guardianType = CREATURE_NONE;
                    gpGame->m_mines[cell->m_objectMetadata].guardianCount = 0;
                    gpGame->ClaimMine(cell->m_objectMetadata, giCurPlayer);
                }
            }
            break;

        case MAP_OBJECT_BARRIER:
            eraseObject = BarrierEvent(cell, eventHero2);
            break;

        case MAP_OBJECT_TRAVELER_TENT:
            PasswordEvent(cell, eventHero2);
            break;

        case MAP_OBJECT_EXPANSION_OBJECT:
            GenericSiteEvent(cell, eventHero2);
            break;

        case MAP_OBJECT_EXPANSION_DWELLING:
            RecruitSiteEvent(cell, eventHero2);
            break;

        case MAP_OBJECT_JAIL:
            JailEvent(cell, eventHero2, x, y);
    }

    playedSample3 = eventSample_f;
    UpdateRadar(1, 0);
    UpdateHeroLocators(1, 1);
    UpdateTownLocators(1, 1);
    UpdBottomView(1, 1, 1);
    if (eraseObject) {
        EraseObj(cell, x, y);
        FizzleCenter(fizzleType3);
    } else {
        CompleteDraw(0);
    }
    UpdateScreen(0, 0);
    gpSoundManager->SwitchAmbientMusic(giTerrainToMusicTrack[IDX(m_currentTerrain)]);
    WaitEndSample(&eventSample_f, -1);
    CheckEndGame(END_GAME_FORCE_NONE, false);
}

VA(0x00441609, 0x904)
void advManager::EraseObj(class mapCell* cell, i32 x, i32 y) {
    i32 erased = 0;
    mapCellExtra* extras_h[NEIGHBOR_COUNT];
    mapCellExtra* extra_l;
    mapCell* cells_h[NEIGHBOR_COUNT];
    i32 frame_k = NO_FRAME;
    mapCell* currentCell_d;
    i8 isWide_l = 0;
    i32 i_g;
    i32 unused_m[ERASE_COORDINATE_COUNT];
    i32 cellX_l;
    i32 cellY_o;
    i32 changed_f;

    for (i_g = 0; i_g < NEIGHBOR_COUNT; i_g++) {
        cells_h[i_g] = NULL;
        extras_h[i_g] = NULL;
    }

    erased = 1;
    if (cell->m_objectTileset == TILESET_OBJNARTI)
        frame_k = cell->m_objectIndex - 1;
    if (cell->m_objectTileset == TILESET_X_LOC3)
        frame_k = cell->m_objectIndex - 1;
    if (cell->m_objectTileset == TILESET_X_LOC2 && cell->m_objectIndex == 9) {
        frame_k = 9;
        isWide_l = 1;
    }
    if (cell->m_objectTileset == TILESET_OBJNMULT && cell->m_objectIndex == 131)
        frame_k = 124;
    if (cell->m_objectTileset == TILESET_OBJNDSRT && cell->m_objectIndex == 61)
        frame_k = 54;
    if (cell->m_objectTileset == TILESET_OBJNWATR && cell->m_objectIndex == 45)
        frame_k = 38;
    if (cell->m_objectTileset == TILESET_OBJNWATR && cell->m_objectIndex == 19)
        frame_k = 12;
    if (cell->m_objectTileset == TILESET_OBJNRSRC) {
        switch (cell->m_objectIndex) {
            case 1:
                frame_k = 0;
                break;
            case 3:
                frame_k = 2;
                break;
            case 5:
                frame_k = 4;
                break;
            case 7:
                frame_k = 6;
                break;
            case 9:
                frame_k = 8;
                break;
            case 11:
                frame_k = 10;
                break;
            case 13:
                frame_k = 12;
                break;
            case 15:
                frame_k = 14;
                break;
            case 19:
                frame_k = 18;
                break;
        }
    }

    for (i_g = 0; i_g < NEIGHBOR_COUNT; i_g++) {
        if (isWide_l)
            frame_k--;
        else if (i_g > 0)
            break;

        if (frame_k != NO_FRAME) {
            if (isWide_l) {
                cellX_l = frame_k > 6 ? x + frame_k - 9 : x + frame_k - 6;
                cellY_o = frame_k > 6 ? y : y - 1;
            } else {
                cellX_l = x - 1;
                cellY_o = y;
            }

            if (cellX_l >= 0) {
                if (cellY_o >= 0) {
                    cells_h[i_g] = gpGame->m_worldMap.GetCell(cellX_l, cellY_o);
                    if (i_g > 1) {
                        cells_h[i_g]->m_overlayTileset = TILESET_NONE;
                        cells_h[i_g]->m_overlayIndex = EMPTY_INDEX;
                    } else if (cells_h[i_g]->m_objectIndex != EMPTY_INDEX) {
                        if (cells_h[i_g]->m_objectIndex == frame_k
                            && cells_h[i_g]->m_objectTileset == cell->m_objectTileset) {
                            cells_h[i_g]->m_objectIndex = 0;
                            cells_h[i_g]->m_objectTileset = TILESET_DUMMY;
                            cells_h[i_g]->m_animatedObject = 0;
                        }

                        if (cells_h[i_g]->m_extraIndex
                            && m_mapData->Extra(cells_h[i_g]->m_extraIndex)->objectIndex
                                   != EMPTY_INDEX)
                            extras_h[i_g] = m_mapData->Extra(cells_h[i_g]->m_extraIndex);
                        else
                            extras_h[i_g] = NULL;

                        while (extras_h[i_g]) {
                            if (extras_h[i_g]->objectIndex == frame_k
                                && extras_h[i_g]->objectTileset == cell->m_objectTileset) {
                                extras_h[i_g]->objectIndex = 0;
                                extras_h[i_g]->objectTileset = TILESET_DUMMY;
                                extras_h[i_g]->animatedObject = 0;
                            }

                            if (extras_h[i_g]->nextIndex
                                && m_mapData->Extra(extras_h[i_g]->nextIndex)->objectIndex
                                       != EMPTY_INDEX)
                                extras_h[i_g] = m_mapData->Extra(extras_h[i_g]->nextIndex);
                            else
                                extras_h[i_g] = NULL;
                        }
                    }
                }
            }
        }
    }

    cell->m_triggerType = MAP_OBJECT_NONE;
    cell->m_objectIndex = 0;
    cell->m_objectTileset = TILESET_DUMMY;
    cell->m_animatedObject = 0;

    for (i_g = 0; i_g < CELL_COUNT; i_g++) {
        if (i_g == 0)
            currentCell_d = cell;
        else
            currentCell_d = cells_h[i_g - 1];
        if (!currentCell_d)
            continue;
        if (currentCell_d->m_objectTileset != TILESET_DUMMY)
            continue;

        if (currentCell_d->m_extraIndex
            && m_mapData->Extra(currentCell_d->m_extraIndex)->objectIndex != EMPTY_INDEX)
            extra_l = m_mapData->Extra(currentCell_d->m_extraIndex);
        else
            continue;

        if (extra_l->objectTileset == TILESET_DUMMY || extra_l->objectIndex == EMPTY_INDEX)
            continue;

        currentCell_d->m_objectIndex = extra_l->objectIndex;
        currentCell_d->m_objectTileset = extra_l->objectTileset;
        currentCell_d->m_animatedObject = extra_l->animatedObject;
        currentCell_d->m_objectLayerBit0 = extra_l->objectLayerBit0;
        currentCell_d->m_objectLayerBit1 = extra_l->objectLayerBit1;
        extra_l->objectIndex = 0;
        extra_l->objectTileset = TILESET_DUMMY;
        extra_l->animatedObject = 0;
    }

    for (i_g = 0; i_g < CELL_COUNT; i_g++) {
        if (i_g == 0)
            currentCell_d = cell;
        else
            currentCell_d = cells_h[i_g - 1];
        if (!currentCell_d)
            continue;

        if (currentCell_d->m_objectTileset != TILESET_DUMMY
            && currentCell_d->m_objectIndex != EMPTY_INDEX && !currentCell_d->m_objectLayerBit1)
            goto cellDone;

        if (currentCell_d->m_extraIndex
            && m_mapData->Extra(currentCell_d->m_extraIndex)->objectIndex != EMPTY_INDEX)
            extra_l = m_mapData->Extra(currentCell_d->m_extraIndex);
        else
            extra_l = NULL;

        while (extra_l) {
            if (extra_l->objectTileset != TILESET_DUMMY && extra_l->objectIndex != EMPTY_INDEX
                && !extra_l->objectLayerBit1)
                goto cellDone;

            if (extra_l->nextIndex
                && m_mapData->Extra(extra_l->nextIndex)->objectIndex != EMPTY_INDEX)
                extra_l = m_mapData->Extra(extra_l->nextIndex);
            else
                extra_l = NULL;
        }

        currentCell_d->m_flags |= IDX(MAP_CELL_OBJECT_SHADOW_ONLY);
    cellDone:
        changed_f = 0;
    }

    SendMapChange(MAP_CHANGE_ERASE_OBJECT, 0, x, y, MAP_CHANGE_VALUE, 0, 0);
    SetEnvironmentOrigin(m_mapOriginX + ENVIRONMENT_BORDER, m_mapOriginY + ENVIRONMENT_BORDER, 1);
    gpGame->SetupAdjacentMons();
}

VA(0x00441f0d, 0xaf)
void advManager::HeroSwap(hero* firstHero, hero* secondHero) {
    swapManager* swapWindow = new swapManager(firstHero, secondHero);
    if (swapWindow == NULL)
        MemError();
    gpExec->DoDialog(swapWindow);
    delete swapWindow;
    RedrawAdvScreen(1, 0);
}

VA(0x00441fbc, 0x125)
i32 advManager::BarrierEvent(mapCell* cell, hero*) {
    SAMPLE2 eventSample = NULL;
    i32 colorIndex = cell->m_objectMetadata;
    colorIndex &= COLOR_MASK;
    i32 passwordIndex = cell->m_objectMetadata;
    passwordIndex >>= PASSWORD_SHIFT;
    char word[INPUT_BUFFER_SIZE];

    sprintf(
        gText,
        "\xc4\xee\xf0\xee\xe3\xf3 \xe2\xe0\xec \xef\xf0\xe5\xe3\xf0\xe0\xe6\xe4\xe0\xe5\xf2 %s "
        "\xec\xe0\xe3\xe8\xf7\xe5\xf1\xea\xe8\xe9 \xe1\xe0\xf0\xfc\xe5\xf0. \xd0\xf3\xed\xfb "
        "\xed\xe0 \xed\xe5\xec \xe3\xeb\xe0\xf1\xff\xf2: \"\xd1\xea\xe0\xe6\xe8 \xf1\xeb\xee\xe2\xee "
        "\xe8 \xf1\xec\xee\xe6\xe5\xf8\xfc \xef\xf0\xee\xe9\xf2\xe8.\"",
        xBarrierColor[colorIndex]
    );
    GetDataEntry(gText, word, INPUT_LENGTH, NULL, 0, 1);
    if (StrEqNoCase(word, xPasswordStrings[passwordIndex])
        && (gpCurPlayer->m_barrierTents & (1 << colorIndex))) {
        EventSound(cell->m_triggerType & MAP_TRIGGER_TYPE_MASK, colorIndex, &eventSample);
        NormalDialog(
            "\xc5\xe4\xe2\xe0 \xe2\xfb \xef\xf0\xee\xe8\xe7\xed\xe5\xf1\xeb\xe8 "
            "\xe2\xee\xeb\xf8\xe5\xe1\xed\xee\xe5 \xf1\xeb\xee\xe2\xee, \xea\xe0\xea "
            "\xf1\xe2\xe5\xf0\xea\xe0\xfe\xf9\xe8\xe9 \xe1\xe0\xf0\xfc\xe5\xf0 \xe8\xf1\xf7\xe5\xe7 "
            "\xe2 \xef\xf3\xf1\xf2\xee\xf2\xe5.",
            NORMAL_DIALOG_INFO,
            -1,
            -1,
            -1,
            0,
            -1,
            0,
            -1,
            0
        );
        return 1;
    } else {
        NormalDialog(
            "\xc2\xfb \xef\xf0\xee\xe8\xe7\xed\xe5\xf1\xeb\xe8 \xf1\xeb\xee\xe2\xee, \xed\xee \xed\xe8\xf7\xe5\xe3\xee \xed\xe5 \xef\xf0\xee\xe8\xe7\xee\xf8\xeb\xee.",
            NORMAL_DIALOG_INFO,
            -1,
            -1,
            -1,
            0,
            -1,
            0,
            -1,
            0
        );
        return 0;
    }
}

VA(0x004420e1, 0x95)
i8 StrEqNoCase(char* firstString, char* sndString) {
    char* firstPtr = firstString;
    char* secondPtr = sndString;
    i32 chCount = 0;
    char fstUpper;
    char sndUpper;

    while (1) {
        chCount++;
        if (chCount == SITE_STRING_LIMIT)
            return 1;
        fstUpper = static_cast<char>(toupper(static_cast<i32>(*firstPtr)));
        sndUpper = static_cast<char>(toupper(static_cast<i32>(*secondPtr)));
        if (fstUpper == sndUpper) {
            if (fstUpper == 0)
                return 1;
            firstPtr++;
            secondPtr++;
        } else {
            return 0;
        }
    }
}

VA(0x00442176, 0xe0)
void advManager::PasswordEvent(mapCell* cell, hero*) {
    SAMPLE2 playSample = NULL;
    i32 color = cell->m_objectMetadata;
    color &= COLOR_MASK;
    i32 passwordIndex = cell->m_objectMetadata;
    passwordIndex >>= PASSWORD_SHIFT;

    EventSound(cell->m_triggerType & MAP_TRIGGER_TYPE_MASK, color, &playSample);
    /* "Вы зашли в шатер и увидели пожилую женщину с магическим камнем в руках.
       Посмотрев на вас, она сказала: "Странствуя, я познала тайную магию.
       Великий оракул даровал мне знания и у меня есть ответ на твой вопрос.
       Волшебное слово, снимающее %s барьер - '%s'."" */
    sprintf(
        gText,
        "\xc2\xfb \xe7\xe0\xf8\xeb\xe8 \xe2 \xf8\xe0\xf2\xe5\xf0 \xe8 \xf3\xe2\xe8\xe4\xe5\xeb\xe8 \xef\xee\xe6\xe8\xeb\xf3\xfe \xe6\xe5\xed\xf9\xe8\xed\xf3 \xf1 \xec\xe0\xe3\xe8\xf7\xe5\xf1\xea\xe8\xec \xea\xe0\xec\xed\xe5\xec \xe2 \xf0\xf3\xea\xe0\xf5. \xcf\xee\xf1\xec\xee\xf2\xf0\xe5\xe2 \xed\xe0 \xe2\xe0\xf1, \xee\xed\xe0 \xf1\xea\xe0\xe7\xe0\xeb\xe0: \"\xd1\xf2\xf0\xe0\xed\xf1\xf2\xe2\xf3\xff, \xff \xef\xee\xe7\xed\xe0\xeb\xe0 \xf2\xe0\xe9\xed\xf3\xfe \xec\xe0\xe3\xe8\xfe. \xc2\xe5\xeb\xe8\xea\xe8\xe9 \xee\xf0\xe0\xea\xf3\xeb \xe4\xe0\xf0\xee\xe2\xe0\xeb \xec\xed\xe5 \xe7\xed\xe0\xed\xe8\xff \xe8 \xf3 \xec\xe5\xed\xff \xe5\xf1\xf2\xfc \xee\xf2\xe2\xe5\xf2 \xed\xe0 \xf2\xe2\xee\xe9 \xe2\xee\xef\xf0\xee\xf1.  \xc2\xee\xeb\xf8\xe5\xe1\xed\xee\xe5 \xf1\xeb\xee\xe2\xee, \xf1\xed\xe8\xec\xe0\xfe\xf9\xe5\xe5 %s \xe1\xe0\xf0\xfc\xe5\xf0 - '%s'.\"",
        xBarrierColor[color],
        xPasswordStrings[passwordIndex]
    );
    NormalDialog(gText, NORMAL_DIALOG_INFO, -1, -1, -1, 0, -1, 0, -1, 0);
    gpCurPlayer->m_barrierTents |= 1 << color;
}

VA(0x00442256, 0x6fc)
void advManager::GenericSiteEvent(mapCell* cell, hero* eventHero) {
    i32 primaryStat15;
    i32 cursedArtifactCount2;
    i32 siteLevel8;
    SAMPLE2 eventSample9;
    i32 index8;
    GenericSiteType siteType4;
    i32 currentSiteType10;
    i32 mapX37;
    H2_ENUM_STORAGE(StableVisitResult, i32) unusedTwo1;
    i32 mapY14;
    mapCell* currentCell36;
    i32 unusedSite;
    H2_ENUM_STORAGE(StableVisitResult, i32) unusedOne18;
    i32 oldQuantity3;
    H2_ENUM_STORAGE(StableVisitResult, i8) stableResult26;
    CreatureType creatureType;
    i32 experience11;

    cursedArtifactCount2 = 0;
    eventSample9 = NULL;
    siteType4 = static_cast<GenericSiteType>(cell->m_objectMetadata);
    siteType4 = static_cast<GenericSiteType>(IDX(siteType4) & GENERIC_SITE_TYPE_MASK);
    siteLevel8 = cell->m_objectMetadata;
    siteLevel8 >>= GENERIC_SITE_LEVEL_SHIFT;

    switch (siteType4) {
        case GENERIC_SITE_ALCHEMIST_TOWER:
            for (index8 = 0; index8 < HERO_ARTIFACT_SLOT_COUNT; index8++) {
                if (IsCursedItem(eventHero->m_artifacts[index8]))
                    cursedArtifactCount2++;
            }
            if (cursedArtifactCount2 != 0) {
                EventSound(
                    cell->m_triggerType & MAP_TRIGGER_TYPE_MASK, IDX(siteType4), &eventSample9
                );
                if (cursedArtifactCount2 == 1) {
                    sprintf(
                        gText,
                        "\xc5\xe4\xe2\xe0 \xe2\xfb \xe2\xee\xf8\xeb\xe8 \xe2 \xe1\xe0\xf8\xed\xfe \xe0\xeb\xf5\xe8\xec\xe8\xea\xe0, \xea\xe0\xea \xed\xe0 \xe2\xf1\xf2\xf0\xe5\xf7\xf3 \xe2\xe0\xec \xe2\xfb\xf8\xe5\xeb \xf5\xf0\xee\xec\xee\xe9 \xe8 \xf1\xe5\xe4\xee\xe9 \xec\xf3\xe6\xf7\xe8\xed\xe0. \xce\xed \xef\xf0\xee\xe2\xe5\xf0\xe8\xeb \xe2\xe0\xf8\xe8 \xe2\xe5\xf9\xe8 \xe8 \xe7\xe0\xec\xe5\xf2\xe8\xeb, \xf7\xf2\xee \xf1\xf0\xe5\xe4\xe8 \xed\xe8\xf5 \xe5\xf1\xf2\xfc \xee\xe4\xed\xe0 \xef\xf0\xee\xea\xeb\xff\xf2\xe0\xff \xe2\xe5\xf9\xfc. \xc7\xe0 750 \xe7\xee\xeb\xee\xf2\xfb\xf5 \xe0\xeb\xf5\xe8\xec\xe8\xea \xf1\xee\xe3\xeb\xe0\xf1\xe5\xed \xe7\xe0\xe1\xf0\xe0\xf2\xfc \xe5\xe5 \xf3 \xe2\xe0\xf1. \xc2\xfb \xe7\xe0\xef\xeb\xe0\xf2\xe8\xf2\xe5?"
                    );
                } else {
                    sprintf(
                        gText,
                        " \xc5\xe4\xe2\xe0 \xe2\xfb \xe2\xee\xf8\xeb\xe8 \xe2 \xe1\xe0\xf8\xed\xfe \xe0\xeb\xf5\xe8\xec\xe8\xea\xe0, \xea\xe0\xea \xed\xe0 \xe2\xf1\xf2\xf0\xe5\xf7\xf3 \xe2\xe0\xec \xe2\xfb\xf8\xe5\xeb \xf5\xf0\xee\xec\xee\xe9 \xe8 \xf1\xe5\xe4\xee\xe9 \xec\xf3\xe6\xf7\xe8\xed\xe0. \xce\xed \xef\xf0\xee\xe2\xe5\xf0\xe8\xeb \xe2\xe0\xf8\xe8 \xe2\xe5\xf9\xe8 \xe8 \xe7\xe0\xec\xe5\xf2\xe8\xeb, \xf7\xf2\xee \xf1\xf0\xe5\xe4\xe8 \xed\xe8\xf5 %d \xef\xf0\xee\xea\xeb\xff\xf2\xfb\xe5 \xe2\xe5\xf9\xe8. \xc7\xe0 750 \xe7\xee\xeb\xee\xf2\xfb\xf5 \xe0\xeb\xf5\xe8\xec\xe8\xea \xf1\xee\xe3\xeb\xe0\xf1\xe5\xed \xe7\xe0\xe1\xf0\xe0\xf2\xfc \xe8\xf5 \xf3 \xe2\xe0\xf1. \xc2\xfb \xe7\xe0\xef\xeb\xe0\xf2\xe8\xf2\xe5?",
                        cursedArtifactCount2
                    );
                }
                NormalDialog(gText, NORMAL_DIALOG_CONFIRM, -1, -1, -1, 0, -1, 0, -1, 0);
                if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
                    if (gpCurPlayer->m_resources[IDX(RES_GOLD)] >= SITE_ALCHEMIST_COST) {
                        for (index8 = 0; index8 < HERO_ARTIFACT_SLOT_COUNT; index8++) {
                            if (IsCursedItem(eventHero->m_artifacts[index8])) {
                                GiveTakeArtifactStat(
                                    eventHero, eventHero->m_artifacts[index8], EVENT_ARTIFACT_TAKE
                                );
                                eventHero->m_artifacts[index8] = ARTIFACT_NONE;
                            }
                        }
                        gpCurPlayer->m_resources[IDX(RES_GOLD)] -= SITE_ALCHEMIST_COST;
                    } else {
                        NormalDialog(
                            "\xc2\xfb \xf1\xeb\xfb\xf8\xe8\xf2\xe5 \xe3\xee\xeb\xee\xf1 \xe8\xe7-\xe7\xe0 \xe7\xe0\xef\xe5\xf0\xf2\xee\xe9 \xe4\xe2\xe5\xf0\xe8: \"\xd3 \xe2\xe0\xf1 \xed\xe5\xe4\xee\xf1\xf2\xe0\xf2\xee\xf7\xed\xee \xe7\xee\xeb\xee\xf2\xe0 \xe4\xeb\xff \xec\xee\xe8\xf5 \xf3\xf1\xeb\xf3\xe3.\"",
                            NORMAL_DIALOG_INFO,
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
                NormalDialog(
                    "\xc2\xfb \xf1\xeb\xfb\xf8\xe8\xf2\xe5 \xe3\xee\xeb\xee\xf1 \xf1 \xe2\xe5\xf0\xf8\xe8\xed\xfb \xe1\xe0\xf8\xed\xe8: \"\xd3\xe1\xe8\xf0\xe0\xe9\xf2\xe5\xf1\xfc! \xdf \xed\xe5 \xec\xee\xe3\xf3 \xe2\xe0\xec \xef\xee\xec\xee\xf7\xfc!\"",
                    NORMAL_DIALOG_INFO,
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
            break;

        case GENERIC_SITE_ARENA:
            if (HAS(eventHero->m_eventFlags, HERO_EVENT_ARENA)) {
                NormalDialog(
                    "\xd1\xf2\xf0\xe0\xe6\xe0 \xc0\xf0\xe5\xed\xfb \xef\xf0\xe5\xe3\xf0\xe0\xe4\xe8\xeb\xe0 \xe2\xe0\xec \xe4\xee\xf0\xee\xe3\xf3 \xe8 \xed\xe5 \xef\xf3\xf1\xf2\xe8\xeb\xe0 \xe2\xed\xf3\xf2\xf0\xfc.",
                    NORMAL_DIALOG_INFO,
                    -1,
                    -1,
                    -1,
                    0,
                    -1,
                    0,
                    -1,
                    0
                );
            } else {
                EventSound(
                    cell->m_triggerType & MAP_TRIGGER_TYPE_MASK, IDX(siteType4), &eventSample9
                );
                eventHero->m_eventFlags = HeroEventFlag(
                    static_cast<i32>(eventHero->m_eventFlags) | IDX(HERO_EVENT_ARENA)
                );
                primaryStat15 = DoArenaDialog();
                eventHero->m_primaryStats[primaryStat15]++;
            }
            break;

        case GENERIC_SITE_MERMAID:
            if (HAS(eventHero->m_eventFlags, HERO_EVENT_MERMAID)) {
                NormalDialog(
                    "\xd0\xf3\xf1\xe0\xeb\xea\xe8 \xec\xee\xeb\xf7\xe0\xeb\xe8\xe2\xee \xe4\xe0\xeb\xe8 \xe2\xe0\xec \xef\xee\xed\xff\xf2\xfc, \xf7\xf2\xee\xe1\xfb \xe2\xfb \xef\xf0\xe8\xf5\xee\xe4\xe8\xeb\xe8 \xe2 \xe4\xf0\xf3\xe3\xee\xe9 \xf0\xe0\xe7, \xf2\xee\xe3\xe4\xe0 \xee\xed\xe8 \xe1\xeb\xe0\xe3\xee\xf1\xeb\xee\xe2\xff\xf2 \xe2\xe0\xf1.",
                    NORMAL_DIALOG_INFO,
                    -1,
                    -1,
                    -1,
                    0,
                    -1,
                    0,
                    -1,
                    0
                );
            } else {
                EventSound(
                    cell->m_triggerType & MAP_TRIGGER_TYPE_MASK, IDX(siteType4), &eventSample9
                );
                eventHero->m_eventFlags = HeroEventFlag(
                    static_cast<i32>(eventHero->m_eventFlags) | IDX(HERO_EVENT_MERMAID)
                );
                eventHero->m_luck++;
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    "\xcc\xe0\xe3\xe8\xf7\xe5\xf1\xea\xe0\xff \xea\xf0\xe0\xf1\xee\xf2\xe0 \xf0\xf3\xf1\xe0\xeb\xee\xea \xee\xe1\xe2\xee\xeb\xee\xea\xeb\xe0 \xe2\xe0\xf1 \xe8 \xe2\xe0\xf8\xf3 \xea\xee\xec\xe0\xed\xe4\xf3. \xcd\xe0 \xea\xe0\xea\xee\xe9-\xf2\xee \xec\xe8\xe3 \xe2\xfb \xef\xee\xe7\xe0\xe1\xfb\xeb\xe8 \xee \xe2\xe0\xf8\xe8\xf5 \xef\xf0\xee\xe1\xeb\xe5\xec\xe0\xf5 \xe8 \xed\xe0\xf1\xeb\xe0\xe4\xe8\xeb\xe8\xf1\xfc \xfd\xf2\xe8\xec \xec\xe3\xed\xee\xe2\xe5\xed\xe8\xe5\xec \xea\xf0\xe0\xf1\xee\xf2\xfb. \xd7\xe0\xf0\xfb \xf0\xf3\xf1\xe0\xeb\xee\xea \xe1\xeb\xe0\xe3\xee\xf1\xeb\xee\xe2\xe8\xeb\xe8 \xe2\xe0\xf1 \xed\xe0 \xf3\xe4\xe0\xf7\xf3 \xe2 \xf1\xeb\xe5\xe4\xf3\xfe\xf9\xe5\xec \xe1\xee\xfe.",
                    SITE_MERMAID_WINDOW_ICON,
                    0,
                    -1,
                    0,
                    -1
                );
            }
            break;

        case GENERIC_SITE_HUT_OF_MAGI:
            EventSound(cell->m_triggerType & MAP_TRIGGER_TYPE_MASK, IDX(siteType4), &eventSample9);
            NormalDialog(
                "\xc2\xfb \xe7\xe0\xf8\xeb\xe8 \xe2 \xe2\xe5\xf2\xf5\xf3\xfe \xf5\xe8\xe1\xe0\xf0\xf3 \xe8 \xe7\xe0\xe3\xee\xe2\xee\xf0\xe8\xeb\xe8 \xf1 \xec\xe0\xe3\xee\xec, \xe6\xe8\xe2\xf3\xf9\xe8\xec \xf2\xf3\xf2. \xce\xed \xef\xee\xe2\xe5\xe4\xe0\xeb \xe2\xe0\xec \xee \xec\xe5\xf1\xf2\xe0\xf5, \xea\xee\xf2\xee\xf0\xfb\xe5 \xe5\xec\xf3 \xe4\xee\xe2\xee\xe4\xe8\xeb\xee\xf1\xfc \xe2\xe8\xe4\xe5\xf2\xfc. \xdd\xf2\xee \xec\xee\xe6\xe5\xf2 \xef\xf0\xe8\xe3\xee\xe4\xe8\xf2\xfc\xf1\xff \xe2\xe0\xec \xe2 \xe2\xe0\xf8\xe8\xf5 \xef\xf3\xf2\xe5\xf8\xe5\xf1\xf2\xe2\xe8\xff\xf5.",
                NORMAL_DIALOG_INFO,
                -1,
                -1,
                -1,
                0,
                -1,
                0,
                -1,
                0
            );
            for (mapX37 = 0; mapX37 < MAP_WIDTH; mapX37++) {
                for (mapY14 = 0; mapY14 < MAP_HEIGHT; mapY14++) {
                    currentCell36 = gpGame->m_worldMap.GetCell(mapX37, mapY14);
                    currentSiteType10 = currentCell36->m_objectMetadata;
                    currentSiteType10 &= COLOR_MASK;
                    if ((currentCell36->m_triggerType & MAP_TRIGGER_TYPE_MASK)
                            == MAP_OBJECT_EXPANSION_OBJECT
                        && currentSiteType10 == SITE_HUT_COLOR) {
                        gpGame->SetVisibility(mapX37, mapY14, giCurPlayer, SITE_VISIBILITY_RADIUS);
                        CompleteDraw(0);
                        UpdateScreen(0, 0);
                    }
                }
            }
            break;

        case GENERIC_SITE_EYE_OF_MAGI:
            NormalDialog(
                "\xca\xe0\xe6\xe5\xf2\xf1\xff, \xfd\xf2\xee\xf2 \xe3\xeb\xe0\xe7 \xe2\xed\xe8\xec\xe0\xf2\xe5\xeb\xfc\xed\xee \xe8\xe7\xf3\xf7\xe0\xe5\xf2 \xee\xea\xf0\xe5\xf1\xf2\xed\xee\xf1\xf2\xe8.",
                NORMAL_DIALOG_INFO,
                -1,
                -1,
                -1,
                0,
                -1,
                0,
                -1,
                0
            );
            break;

        case GENERIC_SITE_SIRENS:
            if (HAS(eventHero->m_eventFlags, HERO_EVENT_SIRENS)) {
                NormalDialog(
                    "\xc2\xfb \xef\xf0\xe8\xea\xe0\xe7\xe0\xeb\xe8 \xf1\xe2\xee\xe5\xe9 \xea\xee\xec\xe0\xed\xe4\xe5 \xe7\xe0\xeb\xe8\xf2\xfc \xf3\xf8\xe8 \xe2\xee\xf1\xea\xee\xec, \xef\xf0\xe5\xe6\xe4\xe5 \xf7\xe5\xec \xef\xee\xe4\xef\xeb\xfb\xf2\xfc \xea \xf1\xe8\xf0\xe5\xed\xe0\xec, \xf7\xf2\xee\xe1\xfb \xef\xee\xf1\xeb\xf3\xf8\xe0\xf2\xfc \xe8\xf5 \xef\xe5\xed\xe8\xe5, \xea\xee\xf2\xee\xf0\xee\xe5 \xe7\xe0\xef\xf0\xee\xf1\xf2\xee \xec\xee\xe6\xe5\xf2 \xef\xee\xe3\xf3\xe1\xe8\xf2\xfc \xe2\xf1\xe5\xf5 \xe2 \xec\xee\xf0\xf1\xea\xee\xe9 \xef\xf3\xf7\xe8\xed\xe5.",
                    NORMAL_DIALOG_INFO,
                    -1,
                    -1,
                    -1,
                    0,
                    -1,
                    0,
                    -1,
                    0
                );
            } else {
                experience11 = 0;
                for (index8 = 0; index8 < ARMY_GROUP_SLOT_COUNT; index8++) {
                    creatureType = eventHero->m_army.m_creatureTypes[index8];
                    if (creatureType != CREATURE_NONE) {
                        oldQuantity3 = eventHero->m_army.m_quantities[index8];
                        if (oldQuantity3 > 1) {
                            eventHero->m_army.m_quantities[index8] =
                                static_cast<i16>(oldQuantity3 * GENERIC_SITE_SIREN_ARMY_REMAINDER);
                            experience11 +=
                                gMonsterDatabase[IDX(creatureType)].hitPoints
                                * (oldQuantity3 - eventHero->m_army.m_quantities[index8]);
                        }
                    }
                }
                if (experience11 != 0) {
                    EventSound(
                        cell->m_triggerType & MAP_TRIGGER_TYPE_MASK,
                        IDX(siteType4),
                        &eventSample9
                    );
                    sprintf(
                        gText,
                        "\xcd\xe0\xe4 \xea\xe0\xec\xed\xff\xec\xe8 \xf0\xe0\xe7\xe4\xe0\xeb\xe0\xf1\xfc \xe6\xf3\xf2\xea\xe0\xff \xef\xe5\xf1\xed\xff \xf1\xe8\xf0\xe5\xed. \xcc\xed\xee\xe3\xe8\xe5 \xe8\xe7 \xe2\xe0\xf8\xe5\xe9 \xea\xee\xec\xe0\xed\xe4\xfb \xf0\xee\xf1\xe8\xeb\xe8\xf1\xfc \xe2 \xe2\xee\xe4\xf3 \xe4\xe5\xe9\xf1\xf2\xe2\xe8\xe5\xec \xfd\xf2\xe8\xf5 \xf7\xe0\xf0 \xe8 \xf3\xf2\xee\xed\xf3\xeb\xe8 \xe2 \xec\xee\xf0\xf1\xea\xee\xe9 \xef\xf3\xf7\xe8\xed\xe5. \xc4\xeb\xff \xe2\xe0\xf1 \xfd\xf2\xee \xe1\xfb\xeb \xf5\xee\xf0\xee\xf8\xe8\xe9 \xf3\xf0\xee\xea, \xe4\xe0\xe2\xf8\xe8\xe9 %d \xee\xf7\xea\xee\xe2 \xee\xef\xfb\xf2\xe0.",
                        experience11
                    );
                    NormalDialog(gText, NORMAL_DIALOG_INFO, -1, -1, -1, 0, -1, 0, -1, 0);
                    GiveExperience(eventHero, experience11, 1);
                } else {
                    NormalDialog(
                        "\xc5\xe4\xe2\xe0 \xf1\xe8\xf0\xe5\xed\xfb \xe7\xe0\xf2\xff\xed\xf3\xeb\xe8 \xf1\xe2\xee\xfe \xf3\xe6\xe0\xf1\xed\xf3\xfe \xef\xe5\xf1\xed\xfe, \xea\xe0\xea \xe2\xe0\xf8\xe0 \xea\xee\xec\xe0\xed\xe4\xe0 \xef\xee\xf1\xe8\xeb\xfc\xed\xe5\xe5 \xed\xe0\xeb\xe5\xe3\xeb\xe0 \xed\xe0 \xe2\xe5\xf1\xeb\xe0 \xe8 \xf3\xe2\xe5\xeb\xe0 \xef\xee\xe4\xe0\xeb\xfc\xf8\xe5 \xea\xee\xf0\xe0\xe1\xeb\xfc \xe2 \xee\xf2\xea\xf0\xfb\xf2\xee \xec\xee\xf0\xe5.",
                        NORMAL_DIALOG_INFO,
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
                eventHero->m_eventFlags = HeroEventFlag(
                    static_cast<i32>(eventHero->m_eventFlags) | IDX(HERO_EVENT_SIRENS)
                );
            }
            break;

        case GENERIC_SITE_STABLES:
            unusedOne18 = STABLE_VISIT_MOBILITY;
            unusedTwo1 = STABLE_VISIT_UPGRADE;
            stableResult26 = STABLE_VISIT_NONE;
            if (HAS(eventHero->m_eventFlags, HERO_EVENT_STABLES) == 0) {
                eventHero->m_eventFlags = HeroEventFlag(
                    static_cast<i32>(eventHero->m_eventFlags) | IDX(HERO_EVENT_STABLES)
                );
                eventHero->m_mobility += SITE_STABLE_MOBILITY;
                eventHero->m_remainingMobility += SITE_STABLE_MOBILITY;
                stableResult26 |= STABLE_VISIT_MOBILITY;
            }
            if (eventHero->CreatureTypeCount(CREATURE_CAVALRY)) {
                eventHero->UpgradeCreatures(CREATURE_CAVALRY, CREATURE_CHAMPION);
                stableResult26 |= STABLE_VISIT_UPGRADE;
            }
            if (stableResult26 != STABLE_VISIT_NONE) {
                EventSound(
                    cell->m_triggerType & MAP_TRIGGER_TYPE_MASK, IDX(siteType4), &eventSample9
                );
            }
            sprintf(gText, xStableText[IDX(stableResult26)]);
            if (HAS(stableResult26, STABLE_VISIT_UPGRADE)) {
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    gText,
                    NORMAL_DIALOG_MONSTER,
                    SITE_STABLE_REWARD_VALUE,
                    -1,
                    0,
                    -1
                );
            } else {
                EventWindow(-1, NORMAL_DIALOG_INFO, gText, -1, 0, -1, 0, -1);
            }
            break;
    }
}

VA(0x00442952, 0x170)
void advManager::RecruitSiteEvent(mapCell* cell, hero* eventHero) {
    SAMPLE2 recruitSample = NULL;
    H2_ENUM_STORAGE(RecruitSiteType, u32) recruitSiteType;
    i16 quantity;
    CreatureType creatureType1;
    u32 index;
    u32 packedValue;

    recruitSiteType = cell->m_objectMetadata;
    recruitSiteType &= static_cast<RecruitSiteType>(EVENT_RECRUIT_TYPE_MASK);
    quantity = static_cast<i16>(cell->m_objectMetadata);
    quantity >>= EVENT_RECRUIT_COUNT_SHIFT;

    switch (recruitSiteType) {
        case RECRUITMENT_SITE_BARROW_MOUNDS:
            creatureType1 = CREATURE_GHOST;
            break;
        case RECRUITMENT_SITE_EARTH_ALTAR:
            creatureType1 = CREATURE_EARTH_ELEMENTAL;
            break;
        case RECRUITMENT_SITE_AIR_ALTAR:
            creatureType1 = CREATURE_AIR_ELEMENTAL;
            break;
        case RECRUITMENT_SITE_FIRE_ALTAR:
            creatureType1 = CREATURE_FIRE_ELEMENTAL;
            break;
        case RECRUITMENT_SITE_WATER_ALTAR:
            creatureType1 = CREATURE_WATER_ELEMENTAL;
            break;
    }

    index = IDX(recruitSiteType);
    if (quantity == 0) {
        EventWindow(-1, NORMAL_DIALOG_INFO, xRecruitEmpty[index], -1, 0, -1, 0, -1);
    } else {
        EventSound(cell->m_triggerType & MAP_TRIGGER_TYPE_MASK, quantity, &recruitSample);
        EventWindow(-1, NORMAL_DIALOG_CONFIRM, xRecruitBuy[index], -1, 0, -1, 0, -1);
        if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
            ExpansionRecruitEvent(eventHero, creatureType1, &quantity);
            packedValue = (quantity << EVENT_RECRUIT_COUNT_SHIFT) | IDX(recruitSiteType);
            cell->m_objectMetadata = packedValue;
        }
    }
}

VA(0x00442ac2, 0xaa)
void advManager::ExpansionRecruitEvent(
    hero* eventHero, H2_ENUM_PARAM(CreatureType, i32) creatureType, i16* availableCount
) {
    tag_message dialogMessage;
    baseManager* recruitWindow = new recruitUnit(&eventHero->m_army, creatureType, availableCount);
    i32 dialogResult;
    if (recruitWindow == NULL)
        MemError();
    gpExec->DoDialog(recruitWindow);
    delete recruitWindow;
}

VA(0x00442b6c, 0x21a)
void advManager::JailEvent(mapCell* cell, hero* eventHero, i32 x, i32 y) {
    SAMPLE2 eventSample = NULL;
    i32 heroId;
    hero* freedHero;

    heroId = cell->m_objectMetadata;
    if (gpGame->m_availableHeroes[heroId] != EVENT_JAILED_HERO) {
        NormalDialog(
            "\xd2\xfe\xf0\xe5\xec\xf9\xe8\xea \xf1\xea\xe0\xe7\xe0\xeb \xe2\xe0\xec, "
            "\xf7\xf2\xee \xe3\xe5\xf0\xee\xe9, \xf2\xee\xec\xe8\xe2\xf8\xe8\xe9\xf1\xff "
            "\xf2\xf3\xf2 \xe2 \xef\xeb\xe5\xed\xf3 \xe1\xfb\xeb \xee\xf1\xe2\xee\xe1\xee\xe6\xe4\xe5\xed "
            "\xe5\xe3\xee \xea\xee\xf0\xee\xeb\xe5\xec.",
            NORMAL_DIALOG_INFO,
            -1,
            -1,
            -1,
            0,
            -1,
            0,
            -1,
            0
        );
        EraseObj(cell, x, y);
        return;
    }

    if (gpCurPlayer->m_heroCount >= EVENT_HERO_LIMIT) {
        NormalDialog(
            "\xd3 \xe2\xe0\xf1 \xf3\xe6\xe5 8 \xe3\xe5\xf0\xee\xe5\xe2. "
            "\xca \xf1\xee\xe6\xe0\xeb\xe5\xed\xe8\xfe, \xe2\xe0\xec \xef\xf0\xe8\xe4\xe5\xf2\xf1\xff "
            "\xee\xf1\xf2\xe0\xe2\xe8\xf2\xfc \xf2\xee\xec\xe8\xf2\xfc\xf1\xff "
            "\xfd\xf2\xee\xe3\xee \xe3\xe5\xf0\xee\xff \xe2 \xf2\xe5\xec\xed\xe8\xf6\xe5 "
            "\xe5\xf9\xe5 \xed\xe5\xee\xef\xf0\xe5\xe4\xe5\xeb\xe5\xed\xed\xee\xe5 "
            "\xe2\xf0\xe5\xec\xff.",
            NORMAL_DIALOG_INFO,
            -1,
            -1,
            -1,
            0,
            -1,
            0,
            -1,
            0
        );
        return;
    }

    EventSound(cell->m_triggerType & MAP_TRIGGER_TYPE_MASK, 0, &eventSample);
    NormalDialog(
        "\xd1 \xee\xf1\xeb\xe5\xef\xe8\xf2\xe5\xeb\xfc\xed\xee\xe9 \xee\xf2\xe2\xe0\xe3\xee\xe9 "
        "\xe2\xfb \xe2\xee\xf0\xe2\xe0\xeb\xe8\xf1\xfc \xe2 \xec\xe5\xf1\xf2\xed\xf3\xfe "
        "\xf2\xe5\xec\xed\xe8\xf6\xf3 \xe8 \xee\xf1\xe2\xee\xe1\xee\xe4\xe8\xeb\xe8 "
        "\xe3\xe5\xf0\xee\xff, \xf2\xee\xec\xff\xf9\xe5\xe3\xee\xf1\xff "
        "\xe7\xe4\xe5\xf1\xfc \xe2 \xef\xeb\xe5\xed\xf3. "
        "\xc2 \xe1\xeb\xe0\xe3\xee\xe4\xe0\xf0\xed\xee\xf1\xf2\xfc "
        "\xee\xed \xef\xee\xea\xeb\xff\xeb\xf1\xff \xf1\xeb\xf3\xe6\xe8\xf2\xfc "
        "\xe2\xe0\xec.",
        NORMAL_DIALOG_INFO,
        -1,
        -1,
        -1,
        0,
        -1,
        0,
        -1,
        0
    );
    gpGame->m_heroRecs[heroId].m_owner = eventHero->m_owner;
    gpGame->m_availableHeroes[heroId] = eventHero->m_owner;
    freedHero = &gpGame->m_heroRecs[heroId];
    EraseObj(cell, x, y);
    gpCurPlayer->m_heroIds[gpCurPlayer->m_heroCount] = static_cast<i8>(heroId);
    gpCurPlayer->m_heroCount++;
    freedHero->m_x = x;
    freedHero->m_y = y;
    freedHero->m_eventFlags = HERO_EVENT_NONE;
    freedHero->m_direction = MAP_DIRECTION_EAST;
    freedHero->m_remainingMobility = freedHero->CalcMobility();
    freedHero->m_mobility = freedHero->m_remainingMobility;
    freedHero->m_locationType = cell->m_triggerType;
    freedHero->m_occupiedTown = cell->m_objectMetadata;
    cell->m_triggerType = MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_HERO_INTERACTION;
    cell->m_objectMetadata = heroId;
    SendMapChange(
        MAP_CHANGE_RECRUIT_HERO,
        static_cast<i8>(heroId),
        x,
        y,
        MAP_CHANGE_CURRENT_PLAYER,
        0,
        0
    );
}

VA(0x00442d86, 0x1a7)
void advManager::TownEvent(mapCell* cell, i32 x, i32 y) {
    hero* attackingHero;
    CombatResult combatResult1;
    hero* defendingHero;
    town* eventTown;

    eventTown = gpGame->GetTown(cell->m_objectMetadata);
    attackingHero = gpGame->GetHero(gpCurPlayer->m_currentHero);
    DemobilizeCurrHero();
    if (eventTown->m_owner == giCurPlayer) {
        eventTown->m_occupyingHeroId = gpCurPlayer->CurrentHero();
        eventTown->View(0);
    } else if (eventTown->HasGarrison()) {
        defendingHero = eventTown->m_occupyingHeroId == -1
                            ? NULL
                            : gpGame->GetHero(eventTown->m_occupyingHeroId);
        combatResult1 = DoCombat(
            x,
            y,
            attackingHero,
            &attackingHero->m_army,
            eventTown,
            defendingHero,
            &eventTown->m_army,
            x,
            y,
            -1,
            1
        );
        if (combatResult1 == COMBAT_RESULT_ATTACKER)
            gpGame->ClaimTown(eventTown->m_id, giCurPlayer, 0);
    } else {
        gpGame->ClaimTown(eventTown->m_id, giCurPlayer, 0);
        if (gbGameOver)
            return;
        UpdateRadar(1, 0);
        UpdateHeroLocators(1, 1);
        UpdateTownLocators(1, 1);
        eventTown->m_occupyingHeroId = gpCurPlayer->CurrentHero();
        eventTown->View(0);
    }
    eventTown->GiveSpells(NULL);
    attackingHero->CheckLevel();
}

VA(0x00442f2d, 0x413)
void advManager::EventSound(
    H2_ENUM_PARAM(MapObjectType, i32) eventType,
    i32 eventData,
    SAMPLE2* outSample
) {
    const i32 treasureSnd = SOUND_TREASURE;
    const i32 expSound = SOUND_EXPERIENCE;
    const i32 moraleWav = SOUND_MORALE;
    const i32 luckSound = SOUND_LUCK;
    const i32 pickupSound = SOUND_PICKUP;
    const i32 mineSound = SOUND_MINE;
    char trackName[SOUND_FILENAME_LENGTH];
    strcpy(trackName, "");
    i32 musicTrack = SOUND_NONE;

    switch (eventType) {
        case MAP_OBJECT_WAGON:
        case MAP_OBJECT_LEAN_TO:
            musicTrack = pickupSound;
        case MAP_OBJECT_ARCHER_HOUSE:
        case MAP_OBJECT_GOBLIN_HUT:
        case MAP_OBJECT_DWARF_COTTAGE:
        case MAP_OBJECT_PEASANT_HUT:
        case MAP_OBJECT_LOG_CABIN:
        case MAP_OBJECT_WATCH_TOWER:
        case MAP_OBJECT_TREE_HOUSE:
        case MAP_OBJECT_SIRENS:
        case MAP_OBJECT_HALFLING_HOLE:
        case MAP_OBJECT_EXCAVATION:
        case MAP_OBJECT_CAVE:
            musicTrack = expSound;
            break;
        case MAP_OBJECT_LIGHTHOUSE:
        case MAP_OBJECT_XANADU:
            musicTrack = SOUND_TRACK_40;
            break;
        case MAP_OBJECT_SHRINE_FIRST_CIRCLE:
        case MAP_OBJECT_SHRINE_SECOND_CIRCLE:
        case MAP_OBJECT_SHRINE_THIRD_CIRCLE:
            musicTrack = treasureSnd;
            break;
        case MAP_OBJECT_ARTIFACT:
            if (static_cast<EventSoundVariant>(eventData) == SOUND_VARIANT_1)
                musicTrack = treasureSnd;
            break;
        case MAP_OBJECT_SKELETON:
        case MAP_OBJECT_WATER_WHEEL:
        case MAP_OBJECT_WINDMILL:
            musicTrack = treasureSnd;
            break;
        case MAP_OBJECT_DAEMON_CAVE:
            musicTrack = SOUND_TRACK_36;
            break;
        case MAP_OBJECT_ORACLE:
        case MAP_OBJECT_ARTESIAN_SPRING:
        case MAP_OBJECT_MAGIC_WELL:
            musicTrack = SOUND_TRACK_32;
            break;
        case MAP_OBJECT_FAERIE_RING:
        case MAP_OBJECT_FOUNTAIN:
        case MAP_OBJECT_IDOL:
            musicTrack = luckSound;
            break;
        case MAP_OBJECT_BUOY:
        case MAP_OBJECT_OASIS:
        case MAP_OBJECT_TEMPLE:
        case MAP_OBJECT_WATERING_HOLE:
            musicTrack = moraleWav;
            break;
        case MAP_OBJECT_GAZEBO:
        case MAP_OBJECT_TREE_OF_KNOWLEDGE:
        case MAP_OBJECT_WITCH_HUT:
            musicTrack = SOUND_TRACK_37;
            break;
        case MAP_OBJECT_ANCIENT_LAMP:
        case MAP_OBJECT_SPHINX:
            musicTrack = SOUND_TRACK_33;
            break;
        case MAP_OBJECT_GRAVEYARD:
        case MAP_OBJECT_SHIPWRECK:
        case MAP_OBJECT_ABANDONED_MINE:
        case MAP_OBJECT_MAGELLAN_MAPS:
        case MAP_OBJECT_DERELICT_SHIP:
        case MAP_OBJECT_OBSERVATION_TOWER:
            musicTrack = SOUND_TRACK_39;
            break;
        case MAP_OBJECT_DRAGON_CITY:
        case MAP_OBJECT_PYRAMID:
        case MAP_OBJECT_CITY_OF_DEAD:
        case MAP_OBJECT_TROLL_BRIDGE:
            musicTrack = SOUND_TRACK_31;
            break;
        case MAP_OBJECT_OBELISK:
            musicTrack = expSound;
            break;
        case MAP_OBJECT_FORT:
        case MAP_OBJECT_STANDING_STONES:
        case MAP_OBJECT_WITCH_DOCTOR_HUT:
        case MAP_OBJECT_MERCENARY_CAMP:
            musicTrack = SOUND_TRACK_38;
            break;
        case MAP_OBJECT_DESERT_TENT:
        case MAP_OBJECT_HILL_FORT:
            musicTrack = SOUND_TRACK_34;
            break;
        case MAP_OBJECT_STONE_LITHS:
            musicTrack = SOUND_TRACK_1;
            break;
        case MAP_OBJECT_WAGON_CAMP:
        case MAP_OBJECT_TREE_CITY:
        case MAP_OBJECT_MAGIC_GARDEN:
            musicTrack = SOUND_TRACK_35;
            break;
        case MAP_OBJECT_BARRIER:
            musicTrack = expSound;
            break;
        case MAP_OBJECT_TRAVELER_TENT:
            musicTrack = expSound;
            break;
        case MAP_OBJECT_EXPANSION_OBJECT:
            switch (static_cast<EventSoundVariant>(eventData)) {
                case SOUND_VARIANT_0:
                    musicTrack = expSound;
                    break;
                case SOUND_VARIANT_1:
                    musicTrack = expSound;
                    break;
                case SOUND_VARIANT_5:
                    musicTrack = luckSound;
                    break;
                case SOUND_VARIANT_2:
                    musicTrack = expSound;
                    break;
                case SOUND_VARIANT_3:
                    break;
                case SOUND_VARIANT_6:
                    musicTrack = expSound;
                    break;
                case SOUND_VARIANT_4:
                    musicTrack = expSound;
                    break;
                default:;
            }
            break;
        case MAP_OBJECT_EXPANSION_DWELLING:
            musicTrack = expSound;
            break;
        case MAP_OBJECT_JAIL:
            musicTrack = expSound;
            break;
        default:
            musicTrack = SOUND_NONE;
            break;
    }

    if (musicTrack != SOUND_NONE) {
        switch (musicTrack) {
            case SOUND_TREASURE:
                strcpy(trackName, "treasure.82m");
                break;
            case SOUND_EXPERIENCE:
                strcpy(trackName, "expernce.82m");
                break;
            case SOUND_MORALE:
                strcpy(trackName, "goodmrle.82m");
                break;
            case SOUND_LUCK:
                strcpy(trackName, "goodluck.82m");
                break;
            case SOUND_PICKUP:
                strcpy(trackName, "pickup01.82m");
                break;
            case SOUND_MINE:
                strcpy(trackName, "h2mine.82m");
                break;
            default:
                gpSoundManager->SwitchAmbientMusic(musicTrack);
                break;
        }

        if (strlen(trackName) > SOUND_SAMPLE_NAME_EMPTY_LIMIT)
            *outSample = LoadPlaySample(trackName);
    }
}

VA(0x00443340, 0xa9)
void advManager::EventWindow(
    i32 eventId,
    i32 buttons,
    char* text,
    i32 type1,
    i32 value1,
    i32 type2,
    i32 value2,
    i32 type3
) {
    i32 eventWindowUnused4;
    i32 eventWindowUnused3;
    i32 eventDone;
    i32 eventWindowUnused8;
    i32 eventWindowUnused7;
    i32 eventWindowUnused6;
    i32 eventWindowUnused5;
    char eventText[EVENT_TEXT_BUFFER_SIZE];
    i32 unusedStyle9;

    eventDone = 0;
    unusedStyle9 = 1;

    if (eventId >= 0 && eventId < EVENT_TEXT_COUNT)
        sprintf(eventText, gEventText[eventId]);
    else if (eventId == MAP_EVENT_REWARD_NONE)
        sprintf(eventText, text);
    else
        sprintf(eventText, "Event ID %d", eventId);

    NormalDialog(eventText, buttons, -1, -1, IDX(type1), value1, type2, value2, type3, 0);
}

VA(0x004433e9, 0x92)
i32 GiveArtifact(hero* eventHero, ArtifactType artifact, b32 checkEndGame, i8 artifactExtra) {
    i32 artifactSlot;

    for (artifactSlot = 0; artifactSlot < EVENT_ARTIFACT_SLOT_COUNT; artifactSlot++) {
        if (eventHero->m_artifacts[artifactSlot] == ARTIFACT_NONE) {
            break;
        }
    }

    if (artifactSlot == EVENT_ARTIFACT_SLOT_COUNT) {
        return IDX(ARTIFACT_NONE);
    }

    eventHero->m_artifacts[artifactSlot] = artifact;
    eventHero->m_artifactExtra[artifactSlot] = artifactExtra;
    GiveTakeArtifactStat(eventHero, artifact, 0);
    eventHero->CheckAnduranPieces(0);
    if (checkEndGame)
        CheckEndGame(END_GAME_FORCE_NONE, false);
    return artifactSlot;
}

VA(0x0044347b, 0x4e)
ArtifactType advManager::GiveRandomArtifact(hero* eventHero) {
    ArtifactType artifactId =
        ArtifactType(gpGame->GetRandomArtifactId(ARTIFACT_LEVEL_RANDOM, true));

    if (artifactId == ARTIFACT_NONE)
        GiveResource(eventHero, RES_GOLD, EVENT_RANDOM_ARTIFACT_GOLD);
    else
        GiveArtifact(eventHero, artifactId, true, -1);
    return artifactId;
}

VA(0x004434c9, 0x61)
i32 advManager::GiveExperience(hero* eventHero, i32 experience, i32 checkLevel) {
    i32 oldLevel;
    i32 unusedLevel2;
    i32 unusedLevel1;
    i32 newLevel1;
    i32 levelGap1;

    oldLevel = eventHero->GetLevel(eventHero->m_experience);
    eventHero->m_level = static_cast<i16>(oldLevel);
    eventHero->m_experience += experience;
    newLevel1 = eventHero->GetLevel(eventHero->m_experience);
    if (checkLevel)
        eventHero->CheckLevel();
    return newLevel1 - oldLevel;
}

VA(0x0044352a, 0x83)
void advManager::GiveResource(hero* eventHero, ResourceType resourceType, i32 amount) {
    if (resourceType >= RES_WOOD && resourceType <= RES_GOLD)
        gpGame->m_players[eventHero->m_owner].m_resources[IDX(resourceType)] += amount;
    if (resourceType == RES_GOLD && gbHumanPlayer[eventHero->m_owner])
        CheckEndGame(END_GAME_FORCE_NONE, false);
}

VA(0x004435ad, 0xe0)
void advManager::RecruitEvent(
    hero* eventHero, H2_ENUM_PARAM(CreatureType, i32) creatureType, mapCell* cell
) {
    tag_message recruitMessage;
    i16 availableCount = static_cast<i16>(cell->m_objectMetadata);
    baseManager* recruitWindow =
        new recruitUnit(&eventHero->m_army, creatureType, &availableCount);
    i32 eventResult;

    if (recruitWindow == NULL)
        MemError();
    gpExec->DoDialog(recruitWindow);
    delete recruitWindow;
    cell->m_objectMetadata = static_cast<u16>(availableCount);
}

VA(0x0044368d, 0x232)
i32 advManager::SkeletonEvent(hero* eventHero, mapCell* cell, char* text, i32 x, i32 y) {
    ArtifactType artifactId;

    switch (static_cast<UndeadEventLevel>(cell->m_objectMetadata)) {
        case EVENT_LEVEL_SMALL:
            if (CombatMonsterEvent(
                    eventHero,
                    CREATURE_SKELETON,
                    SKELETON_EVENT_SMALL_COUNT,
                    cell,
                    x,
                    y,
                    0,
                    x,
                    y,
                    CREATURE_NONE,
                    0,
                    0,
                    CREATURE_NONE,
                    0,
                    0
                )
                == COMBAT_RESULT_ATTACKER) {
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    text,
                    IDX(RES_GOLD),
                    SKELETON_EVENT_SMALL_GOLD,
                    -1,
                    0,
                    -1
                );
                GiveResource(eventHero, RES_GOLD, SKELETON_EVENT_SMALL_GOLD);
                eventHero->CheckLevel();
                return 1;
            }
            break;
        case EVENT_LEVEL_MEDIUM:
            if (CombatMonsterEvent(
                    eventHero,
                    CREATURE_SKELETON,
                    SKELETON_EVENT_MEDIUM_COUNT,
                    cell,
                    x,
                    y,
                    0,
                    x,
                    y,
                    CREATURE_NONE,
                    0,
                    0,
                    CREATURE_NONE,
                    0,
                    0
                )
                == COMBAT_RESULT_ATTACKER) {
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    text,
                    IDX(RES_GOLD),
                    SKELETON_EVENT_MEDIUM_GOLD,
                    -1,
                    0,
                    -1
                );
                GiveResource(eventHero, RES_GOLD, SKELETON_EVENT_MEDIUM_GOLD);
                eventHero->CheckLevel();
                return 1;
            }
            break;
        case EVENT_LEVEL_LARGE:
            if (CombatMonsterEvent(
                    eventHero,
                    CREATURE_SKELETON,
                    SKELETON_EVENT_LARGE_COUNT,
                    cell,
                    x,
                    y,
                    0,
                    x,
                    y,
                    CREATURE_NONE,
                    0,
                    0,
                    CREATURE_NONE,
                    0,
                    0
                )
                == COMBAT_RESULT_ATTACKER) {
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    text,
                    IDX(RES_GOLD),
                    SKELETON_EVENT_LARGE_GOLD,
                    -1,
                    0,
                    -1
                );
                GiveResource(eventHero, RES_GOLD, SKELETON_EVENT_LARGE_GOLD);
                eventHero->CheckLevel();
                return 1;
            }
            break;
        default:
            if (CombatMonsterEvent(
                    eventHero,
                    CREATURE_SKELETON,
                    SKELETON_EVENT_HUGE_COUNT,
                    cell,
                    x,
                    y,
                    0,
                    x,
                    y,
                    CREATURE_NONE,
                    0,
                    0,
                    CREATURE_NONE,
                    0,
                    0
                )
                == COMBAT_RESULT_ATTACKER) {
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    text,
                    IDX(RES_GOLD),
                    SKELETON_EVENT_HUGE_GOLD,
                    -1,
                    0,
                    -1
                );
                GiveResource(eventHero, RES_GOLD, SKELETON_EVENT_HUGE_GOLD);
                eventHero->CheckLevel();
                return 1;
            }
            break;
    }
    return 0;
}

VA(0x004438bf, 0x26b)
i32 advManager::ZombieEvent(hero* eventHero, mapCell* cell, char* text, i32 x, i32 y) {
    ArtifactType artifactId;
    switch (static_cast<UndeadEventLevel>(cell->m_objectMetadata)) {
        case EVENT_LEVEL_SMALL:
            if (CombatMonsterEvent(
                    eventHero,
                    CREATURE_ZOMBIE,
                    ZOMBIE_EVENT_SMALL_COUNT,
                    cell,
                    x,
                    y,
                    0,
                    x,
                    y,
                    CREATURE_MUTANT_ZOMBIE,
                    EVENT_SMALL_SUPPORT_COUNT,
                    EVENT_SUPPORT_STACKS,
                    CREATURE_NONE,
                    0,
                    0
                )
                == COMBAT_RESULT_ATTACKER) {
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    text,
                    IDX(RES_GOLD),
                    ZOMBIE_EVENT_SMALL_GOLD,
                    -1,
                    0,
                    -1
                );
                GiveResource(eventHero, RES_GOLD, ZOMBIE_EVENT_SMALL_GOLD);
                eventHero->CheckLevel();
                return 1;
            }
            break;
        case EVENT_LEVEL_MEDIUM:
            if (CombatMonsterEvent(
                    eventHero,
                    CREATURE_MUTANT_ZOMBIE,
                    ZOMBIE_EVENT_MEDIUM_COUNT,
                    cell,
                    x,
                    y,
                    0,
                    x,
                    y,
                    CREATURE_MUTANT_ZOMBIE,
                    EVENT_MEDIUM_SUPPORT_COUNT,
                    EVENT_SUPPORT_STACKS,
                    CREATURE_NONE,
                    0,
                    0
                )
                == COMBAT_RESULT_ATTACKER) {
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    text,
                    IDX(RES_GOLD),
                    ZOMBIE_EVENT_MEDIUM_GOLD,
                    -1,
                    0,
                    -1
                );
                GiveResource(eventHero, RES_GOLD, ZOMBIE_EVENT_MEDIUM_GOLD);
                eventHero->CheckLevel();
                return 1;
            }
            break;
        case EVENT_LEVEL_LARGE:
            if (CombatMonsterEvent(
                    eventHero,
                    CREATURE_MUTANT_ZOMBIE,
                    ZOMBIE_EVENT_LARGE_COUNT,
                    cell,
                    x,
                    y,
                    0,
                    x,
                    y,
                    CREATURE_MUTANT_ZOMBIE,
                    EVENT_LARGE_SUPPORT_COUNT,
                    EVENT_SUPPORT_STACKS,
                    CREATURE_NONE,
                    0,
                    0
                )
                == COMBAT_RESULT_ATTACKER) {
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    text,
                    IDX(RES_GOLD),
                    ZOMBIE_EVENT_LARGE_GOLD,
                    -1,
                    0,
                    -1
                );
                GiveResource(eventHero, RES_GOLD, ZOMBIE_EVENT_LARGE_GOLD);
                eventHero->CheckLevel();
                return 1;
            }
            break;
        default:
            if (CombatMonsterEvent(
                    eventHero,
                    CREATURE_MUTANT_ZOMBIE,
                    ZOMBIE_EVENT_HUGE_COUNT,
                    cell,
                    x,
                    y,
                    0,
                    x,
                    y,
                    CREATURE_MUTANT_ZOMBIE,
                    EVENT_HUGE_SUPPORT_COUNT,
                    EVENT_SUPPORT_STACKS,
                    CREATURE_NONE,
                    0,
                    0
                )
                == COMBAT_RESULT_ATTACKER) {
                artifactId = GiveRandomArtifact(eventHero);
                if (artifactId != ARTIFACT_NONE)
                    EventWindow(
                        -1,
                        NORMAL_DIALOG_INFO,
                        text,
                        IDX(RES_GOLD),
                        ZOMBIE_EVENT_HUGE_GOLD,
                        MAP_EVENT_REWARD_ARTIFACT,
                        IDX(artifactId),
                        -1
                    );
                else
                    EventWindow(
                        -1,
                        NORMAL_DIALOG_INFO,
                        text,
                        IDX(RES_GOLD),
                        ZOMBIE_EVENT_HUGE_GOLD,
                        -1,
                        0,
                        -1
                    );
                GiveResource(eventHero, RES_GOLD, ZOMBIE_EVENT_HUGE_GOLD);
                eventHero->CheckLevel();
                return 1;
            }
            break;
    }
    return 0;
}

VA(0x00443b2a, 0x2cc)
i32 advManager::GhostEvent(hero* eventHero, mapCell* cell, char* text, i32 x, i32 y) {
    ArtifactType artifactId;
    switch (static_cast<UndeadEventLevel>(cell->m_objectMetadata)) {
        case EVENT_LEVEL_SMALL:
            if (CombatMonsterEvent(
                    eventHero,
                    CREATURE_GHOST,
                    GHOST_EVENT_SMALL_COUNT,
                    cell,
                    x,
                    y,
                    0,
                    x,
                    y,
                    CREATURE_NONE,
                    0,
                    0,
                    CREATURE_NONE,
                    0,
                    0
                )
                == COMBAT_RESULT_ATTACKER) {
                sprintf(gText, "%s", text);
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    gText,
                    IDX(RES_GOLD),
                    GHOST_EVENT_SMALL_GOLD,
                    -1,
                    0,
                    -1
                );
                GiveResource(eventHero, RES_GOLD, GHOST_EVENT_SMALL_GOLD);
                eventHero->CheckLevel();
                return 1;
            }
            break;
        case EVENT_LEVEL_MEDIUM:
            if (CombatMonsterEvent(
                    eventHero,
                    CREATURE_GHOST,
                    GHOST_EVENT_MEDIUM_COUNT,
                    cell,
                    x,
                    y,
                    0,
                    x,
                    y,
                    CREATURE_NONE,
                    0,
                    0,
                    CREATURE_NONE,
                    0,
                    0
                )
                == COMBAT_RESULT_ATTACKER) {
                sprintf(gText, "%s", text);
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    gText,
                    IDX(RES_GOLD),
                    GHOST_EVENT_MEDIUM_GOLD,
                    -1,
                    0,
                    -1
                );
                GiveResource(eventHero, RES_GOLD, GHOST_EVENT_MEDIUM_GOLD);
                eventHero->CheckLevel();
                return 1;
            }
            break;
        case EVENT_LEVEL_LARGE:
            if (CombatMonsterEvent(
                    eventHero,
                    CREATURE_GHOST,
                    GHOST_EVENT_LARGE_COUNT,
                    cell,
                    x,
                    y,
                    0,
                    x,
                    y,
                    CREATURE_NONE,
                    0,
                    0,
                    CREATURE_NONE,
                    0,
                    0
                )
                == COMBAT_RESULT_ATTACKER) {
                sprintf(gText, "%s", text);
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    gText,
                    IDX(RES_GOLD),
                    GHOST_EVENT_LARGE_GOLD,
                    -1,
                    0,
                    -1
                );
                GiveResource(eventHero, RES_GOLD, GHOST_EVENT_LARGE_GOLD);
                eventHero->CheckLevel();
                return 1;
            }
            break;
        default:
            if (CombatMonsterEvent(
                    eventHero,
                    CREATURE_GHOST,
                    GHOST_EVENT_HUGE_COUNT,
                    cell,
                    x,
                    y,
                    0,
                    x,
                    y,
                    CREATURE_NONE,
                    0,
                    0,
                    CREATURE_NONE,
                    0,
                    0
                )
                == COMBAT_RESULT_ATTACKER) {
                artifactId = GiveRandomArtifact(eventHero);
                sprintf(gText, "%s", text);
                if (artifactId != ARTIFACT_NONE)
                    EventWindow(
                        -1,
                        NORMAL_DIALOG_INFO,
                        gText,
                        IDX(RES_GOLD),
                        GHOST_EVENT_HUGE_GOLD,
                        MAP_EVENT_REWARD_ARTIFACT,
                        IDX(artifactId),
                        -1
                    );
                else
                    EventWindow(
                        -1,
                        NORMAL_DIALOG_INFO,
                        gText,
                        IDX(RES_GOLD),
                        GHOST_EVENT_HUGE_GOLD,
                        -1,
                        0,
                        -1
                    );
                GiveResource(eventHero, RES_GOLD, GHOST_EVENT_HUGE_GOLD);
                eventHero->CheckLevel();
                return 1;
            }
            break;
    }
    return 0;
}

VA(0x00443df6, 0x245)
void advManager::HouseEvent(hero* eventHero, mapCell* cell) {
    HouseRecruitmentSite siteIndex = RECRUIT_ARCHER;
    CreatureType creatureTypes[IDX(RECRUIT_SITE_COUNT)];

    switch (cell->m_triggerType & MAP_TRIGGER_TYPE_MASK) {
        case MAP_OBJECT_ARCHER_HOUSE:
            siteIndex = RECRUIT_ARCHER;
            break;
        case MAP_OBJECT_GOBLIN_HUT:
            siteIndex = RECRUIT_GOBLIN;
            break;
        case MAP_OBJECT_PEASANT_HUT:
            siteIndex = RECRUIT_PEASANT;
            break;
        case MAP_OBJECT_DWARF_COTTAGE:
        case MAP_OBJECT_SIRENS:
            siteIndex = RECRUIT_DWARF;
            break;
        case MAP_OBJECT_LOG_CABIN:
            siteIndex = RECRUIT_LOG_CABIN;
            break;
        case MAP_OBJECT_TREE_HOUSE:
            siteIndex = RECRUIT_TREE_HOUSE;
            break;
        case MAP_OBJECT_HALFLING_HOLE:
            siteIndex = RECRUIT_HALFLING;
            break;
        case MAP_OBJECT_WATCH_TOWER:
            siteIndex = RECRUIT_WATCH_TOWER;
            break;
        case MAP_OBJECT_CAVE:
            siteIndex = RECRUIT_CAVE;
            break;
        case MAP_OBJECT_EXCAVATION:
            siteIndex = RECRUIT_EXCAVATION;
            break;
    }

    if (cell->m_objectMetadata == 0) {
        EventWindow(
            IDX(siteIndex) * EVENT_HOUSE_DIALOG_STRIDE + EVENT_EMPTY_DIALOG_BASE,
            NORMAL_DIALOG_INFO,
            "",
            -1,
            0,
            -1,
            0,
            -1
        );
    } else {
        creatureTypes[IDX(RECRUIT_ARCHER)] = CREATURE_ARCHER;
        creatureTypes[IDX(RECRUIT_GOBLIN)] = CREATURE_GOBLIN;
        creatureTypes[IDX(RECRUIT_PEASANT)] = CREATURE_PEASANT;
        creatureTypes[IDX(RECRUIT_DWARF)] = CREATURE_DWARF;
        creatureTypes[IDX(RECRUIT_LOG_CABIN)] = CREATURE_DWARF;
        creatureTypes[IDX(RECRUIT_TREE_HOUSE)] = CREATURE_SPRITE;
        creatureTypes[IDX(RECRUIT_HALFLING)] = CREATURE_HALFLING;
        creatureTypes[IDX(RECRUIT_WATCH_TOWER)] = CREATURE_ORC;
        creatureTypes[IDX(RECRUIT_CAVE)] = CREATURE_CENTAUR;
        creatureTypes[IDX(RECRUIT_EXCAVATION)] = CREATURE_SKELETON;

        EventWindow(
            IDX(siteIndex) * EVENT_HOUSE_DIALOG_STRIDE + EVENT_RECRUIT_DIALOG_BASE,
            NORMAL_DIALOG_CONFIRM,
            "",
            -1,
            0,
            -1,
            0,
            -1
        );
        if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
            if (eventHero->m_army.CanJoin(creatureTypes[IDX(siteIndex)])) {
                eventHero->m_army.Add(
                    creatureTypes[IDX(siteIndex)], cell->m_objectMetadata, -1
                );
                cell->m_objectMetadata = 0;
            } else {
                EventWindow(
                    IDX(siteIndex) * EVENT_HOUSE_DIALOG_STRIDE + EVENT_ARMY_FULL_DIALOG_BASE,
                    NORMAL_DIALOG_INFO,
                    "",
                    -1,
                    0,
                    -1,
                    0,
                    -1
                );
            }
        }
    }
}

VA(0x0044403b, 0x536)
CombatResult advManager::CombatMonsterEvent(
    hero* eventHero,
    CreatureType monsterType,
    i32 monsterCount,
    mapCell*,
    i32 mapX,
    i32 mapY,
    i32 defender,
    i32 combatX,
    i32 combatY,
    CreatureType secondaryType,
    i32 secondaryCount,
    i32 secondaryStacks,
    CreatureType tertiaryType,
    i32 tertiaryCount,
    i32 tertiaryStacks
) {
    i32 placement4[MONSTER_ARMY_SLOTS];
    i32 combatUnused;
    i32 lastCount;
    i32 groupCount;
    i32 stackCount;
    CreatureType savedTypes[MONSTER_ARMY_SLOTS];
    CombatResult battleOutcome;
    i32 savedCounts[MONSTER_ARMY_SLOTS];
    i32 stackIdx;
    i32 combatUnused0;

    DemobilizeCurrHero();
    if (combatX == -1) {
        combatX = mapX;
        combatY = mapY;
    } else {
        m_lastQuickViewX = combatX;
        m_lastQuickViewY = combatY;
        m_mineGuardianFacingLeft = eventHero->m_x < combatX;
        if (ComboDraw(0))
            UpdateScreen(0, 0);
        m_lastQuickViewX = -1;
    }

    memset(gpMonGroup->m_creatureTypes, IDX(CREATURE_NONE), MONSTER_ARMY_SLOTS);
    memset(gpMonGroup->m_creatureCounts, 0, MONSTER_ARMY_SLOTS * sizeof(i16));
    stackCount = MONSTER_ARMY_SLOTS - secondaryStacks - tertiaryStacks;
    if (stackCount < 1)
        stackCount = 1;
    groupCount = 0;
    SRand(combatX + combatY);
    if (stackCount == MONSTER_ARMY_SLOTS
        && HAS(gMonsterDatabase[IDX(monsterType)].attributes, MONSTER_ATTRIBUTE_RANGED) == 0) {
        i32 roll = SRandom(0, MONSTER_RANDOM_MAX);
        if (roll < MONSTER_REDUCED_STACK_CHANCE)
            stackCount = MONSTER_REDUCED_STACK_COUNT;
        else if (roll < MONSTER_FOUR_STACK_THRESHOLD)
            stackCount = MONSTER_FOUR_STACK_COUNT;
    }

    for (stackIdx = 0; stackIdx < stackCount; stackIdx++) {
        if (stackIdx == (stackCount >> 1)
            && (monsterType == CREATURE_ARCHER || monsterType == CREATURE_PIKEMAN
                || monsterType == CREATURE_SWORDSMAN || monsterType == CREATURE_CAVALRY
                || monsterType == CREATURE_PALADIN || monsterType == CREATURE_ORC
                || monsterType == CREATURE_OGRE || monsterType == CREATURE_TROLL
                || monsterType == CREATURE_DWARF || monsterType == CREATURE_ELF
                || monsterType == CREATURE_DRUID || monsterType == CREATURE_MINOTAUR
                || monsterType == CREATURE_GREEN_DRAGON || monsterType == CREATURE_RED_DRAGON
                || monsterType == CREATURE_IRON_GOLEM || monsterType == CREATURE_MAGE
                || monsterType == CREATURE_GIANT || monsterType == CREATURE_ZOMBIE
                || monsterType == CREATURE_MUMMY || monsterType == CREATURE_VAMPIRE
                || monsterType == CREATURE_LICH)
            && SRandom(0, MONSTER_RANDOM_MAX) < MONSTER_UPGRADE_CHANCE && secondaryCount == 0
            && tertiaryCount == 0)
            gpMonGroup->m_creatureTypes[groupCount + stackIdx] =
                NextCreatureType(monsterType);
        else
            gpMonGroup->m_creatureTypes[groupCount + stackIdx] =
                monsterType;
        gpMonGroup->m_creatureCounts[groupCount + stackIdx] =
            static_cast<i16>(monsterCount / stackCount + (monsterCount % stackCount > stackIdx));
    }

    groupCount += stackCount;
    if (secondaryStacks != 0) {
        stackCount = secondaryStacks;
        for (stackIdx = 0; stackIdx < stackCount; stackIdx++) {
            gpMonGroup->m_creatureTypes[groupCount + stackIdx] =
                secondaryType;
            gpMonGroup->m_creatureCounts[groupCount + stackIdx] =
                static_cast<i16>(
                    secondaryCount / stackCount + (secondaryCount % stackCount > stackIdx)
                );
        }
    }

    groupCount += stackCount;
    if (tertiaryStacks != 0) {
        stackCount = tertiaryStacks;
        for (stackIdx = 0; stackIdx < stackCount; stackIdx++) {
            gpMonGroup->m_creatureTypes[groupCount + stackIdx] =
                tertiaryType;
            gpMonGroup->m_creatureCounts[groupCount + stackIdx] =
                static_cast<i16>(
                    tertiaryCount / stackCount + (secondaryCount % stackCount > stackIdx)
                );
        }
    }
    lastCount = stackCount;

    for (stackIdx = 0; stackIdx < MONSTER_ARMY_SLOTS; stackIdx++) {
        if (gpMonGroup->m_creatureCounts[stackIdx] <= 0)
            gpMonGroup->m_creatureTypes[stackIdx] = CREATURE_NONE;
    }
    for (stackIdx = 0; stackIdx < MONSTER_ARMY_SLOTS; stackIdx++)
        placement4[stackIdx] = stackIdx;

    if (lastCount == 1) {
        placement4[2] = 0;
        placement4[0] = 2;
    } else if (lastCount == 2) {
        placement4[1] = 1;
        placement4[3] = 0;
        placement4[0] = 3;
    } else if (lastCount == 3) {
        placement4[0] = 3;
        placement4[1] = 0;
        placement4[2] = 1;
        placement4[3] = 2;
    } else if (tertiaryStacks == 1 && secondaryStacks == 1) {
        placement4[1] = 4;
        placement4[4] = 1;
    } else if (tertiaryStacks == 1 && secondaryStacks == 2) {
        placement4[1] = 2;
        placement4[2] = 4;
        placement4[3] = 3;
        placement4[4] = 1;
    } else if (secondaryStacks == 2) {
        placement4[1] = 4;
        placement4[4] = 1;
    }

    for (stackIdx = 0; stackIdx < MONSTER_ARMY_SLOTS; stackIdx++) {
        savedTypes[stackIdx] = gpMonGroup->m_creatureTypes[stackIdx];
        savedCounts[stackIdx] = gpMonGroup->m_creatureCounts[stackIdx];
    }
    for (stackIdx = 0; stackIdx < MONSTER_ARMY_SLOTS; stackIdx++) {
        gpMonGroup->m_creatureTypes[stackIdx] = savedTypes[placement4[stackIdx]];
        gpMonGroup->m_creatureCounts[stackIdx] =
            static_cast<i16>(savedCounts[placement4[stackIdx]]);
    }

    if (defender != 0)
        battleOutcome = DoCombat(
            combatX,
            combatY,
            NULL,
            gpMonGroup,
            NULL,
            eventHero,
            &eventHero->m_army,
            mapX,
            mapY,
            combatX + combatY,
            1
        );
    else
        battleOutcome = DoCombat(
            combatX,
            combatY,
            eventHero,
            &eventHero->m_army,
            NULL,
            NULL,
            gpMonGroup,
            mapX,
            mapY,
            combatX + combatY,
            1
        );
    MobilizeCurrHero(0);
    return battleOutcome;
}

VA(0x00444571, 0x3d8)
void GiveTakeArtifactStat(hero* targetHero, ArtifactType artifact, b32 take) {
    i32 stats[EVENT_ARTIFACT_PRIMARY_STAT_COUNT];
    i32 i;
    i32 maxSpellPoints;

    if (artifact == ARTIFACT_NONE)
        return;
    stats[IDX(HERO_PRIMARY_ATTACK)] = 0;
    stats[IDX(HERO_PRIMARY_DEFENSE)] = 0;
    stats[IDX(HERO_PRIMARY_SPELL_POWER)] = 0;
    stats[IDX(HERO_PRIMARY_KNOWLEDGE)] = 0;

    switch (artifact) {
        case ARTIFACT_ULTIMATE_BOOK:
            stats[IDX(HERO_PRIMARY_KNOWLEDGE)] = 12;
            break;
        case ARTIFACT_ULTIMATE_SWORD:
            stats[IDX(HERO_PRIMARY_ATTACK)] = 12;
            break;
        case ARTIFACT_ULTIMATE_CLOAK:
            stats[IDX(HERO_PRIMARY_DEFENSE)] = 12;
            break;
        case ARTIFACT_ULTIMATE_WAND:
            stats[IDX(HERO_PRIMARY_SPELL_POWER)] = 12;
            break;
        case ARTIFACT_ULTIMATE_SHIELD:
            stats[IDX(HERO_PRIMARY_ATTACK)] = 6;
            stats[IDX(HERO_PRIMARY_DEFENSE)] = 6;
            break;
        case ARTIFACT_ULTIMATE_STAFF:
            stats[IDX(HERO_PRIMARY_SPELL_POWER)] = 6;
            stats[IDX(HERO_PRIMARY_KNOWLEDGE)] = 6;
            break;
        case ARTIFACT_ULTIMATE_CROWN:
            stats[IDX(HERO_PRIMARY_ATTACK)] = 4;
            stats[IDX(HERO_PRIMARY_DEFENSE)] = 4;
            stats[IDX(HERO_PRIMARY_SPELL_POWER)] = 4;
            stats[IDX(HERO_PRIMARY_KNOWLEDGE)] = 4;
            break;
        case ARTIFACT_GOLDEN_GOOSE:
            break;
        case ARTIFACT_ARCANE_NECKLACE:
            stats[IDX(HERO_PRIMARY_SPELL_POWER)] = 4;
            break;
        case ARTIFACT_CASTER_BRACELET:
            stats[IDX(HERO_PRIMARY_SPELL_POWER)] = 2;
            break;
        case ARTIFACT_MAGE_RING:
            stats[IDX(HERO_PRIMARY_SPELL_POWER)] = 2;
            break;
        case ARTIFACT_WITCHES_BROACH:
            stats[IDX(HERO_PRIMARY_SPELL_POWER)] = 3;
            break;
        case ARTIFACT_MEDAL_OF_VALOR:
            break;
        case ARTIFACT_MEDAL_OF_COURAGE:
            break;
        case ARTIFACT_MEDAL_OF_HONOR:
            break;
        case ARTIFACT_MEDAL_OF_DISTINCTION:
            break;
        case ARTIFACT_FIZBIN_OF_MISFORTUNE:
            break;
        case ARTIFACT_THUNDER_MACE:
            stats[IDX(HERO_PRIMARY_ATTACK)] = 1;
            break;
        case ARTIFACT_ARMORED_GAUNTLETS:
            stats[IDX(HERO_PRIMARY_DEFENSE)] = 1;
            break;
        case ARTIFACT_DEFENDER_HELM:
            stats[IDX(HERO_PRIMARY_DEFENSE)] = 1;
            break;
        case ARTIFACT_GIANT_FLAIL:
            stats[IDX(HERO_PRIMARY_ATTACK)] = 1;
            break;
        case ARTIFACT_BALLISTA:
            break;
        case ARTIFACT_STEALTH_SHIELD:
            stats[IDX(HERO_PRIMARY_DEFENSE)] = 2;
            break;
        case ARTIFACT_DRAGON_SWORD:
            stats[IDX(HERO_PRIMARY_ATTACK)] = 3;
            break;
        case ARTIFACT_POWER_AXE:
            stats[IDX(HERO_PRIMARY_ATTACK)] = 2;
            break;
        case ARTIFACT_DIVINE_BREASTPLATE:
            stats[IDX(HERO_PRIMARY_DEFENSE)] = 3;
            break;
        case ARTIFACT_MINOR_SCROLL:
            stats[IDX(HERO_PRIMARY_KNOWLEDGE)] = 2;
            break;
        case ARTIFACT_MAJOR_SCROLL:
            stats[IDX(HERO_PRIMARY_KNOWLEDGE)] = 3;
            break;
        case ARTIFACT_SUPERIOR_SCROLL:
            stats[IDX(HERO_PRIMARY_KNOWLEDGE)] = 4;
            break;
        case ARTIFACT_FOREMOST_SCROLL:
            stats[IDX(HERO_PRIMARY_KNOWLEDGE)] = 5;
            break;
        case ARTIFACT_ENDLESS_SACK_GOLD:
            break;
        case ARTIFACT_ENDLESS_BAG_GOLD:
            break;
        case ARTIFACT_ENDLESS_PURSE_GOLD:
            break;
        case ARTIFACT_NOMAD_BOOTS:
            break;
        case ARTIFACT_TRAVELER_BOOTS:
            break;
        case ARTIFACT_RABBIT_FOOT:
            break;
        case ARTIFACT_GOLDEN_HORSESHOE:
            break;
        case ARTIFACT_GAMBLERS_COIN:
            break;
        case ARTIFACT_FOUR_LEAF_CLOVER:
            break;
        case ARTIFACT_TRUE_COMPASS:
            break;
        case ARTIFACT_SAILORS_ASTROLABE:
            break;
        case ARTIFACT_EVIL_EYE:
            break;
        case ARTIFACT_ENCHANTED_HOURGLASS:
            break;
        case ARTIFACT_GOLD_WATCH:
            break;
        case ARTIFACT_SKULLCAP:
            break;
        case ARTIFACT_ICE_CLOAK:
            break;
        case ARTIFACT_FIRE_CLOAK:
            break;
        case ARTIFACT_LIGHTNING_HELM:
            break;
        case ARTIFACT_EVERCOLD_ICICLE:
            break;
        case ARTIFACT_EVERHOT_LAVA_ROCK:
            break;
        case ARTIFACT_LIGHTNING_ROD:
            break;
        case ARTIFACT_SNAKE_RING:
            break;
        case ARTIFACT_ANKH:
            break;
        case ARTIFACT_BOOK_ELEMENTS:
            break;
        case ARTIFACT_ELEMENTAL_RING:
            break;
        case ARTIFACT_HOLY_PENDANT:
            break;
        case ARTIFACT_PENDANT_FREE_WILL:
            break;
        case ARTIFACT_PENDANT_LIFE:
            break;
        case ARTIFACT_SERENITY_PENDANT:
            break;
        case ARTIFACT_SEEING_EYE_PENDANT:
            break;
        case ARTIFACT_KINETIC_PENDANT:
            break;
        case ARTIFACT_PENDANT_DEATH:
            break;
        case ARTIFACT_WAND_NEGATION:
            break;
        case ARTIFACT_GOLDEN_BOW:
            break;
        case ARTIFACT_TELESCOPE:
            break;
        case ARTIFACT_STATESMANS_QUILL:
            break;
        case ARTIFACT_WIZARD_HAT:
            break;
        case ARTIFACT_POWER_RING:
            break;
        case ARTIFACT_AMMO_CART:
            break;
        case ARTIFACT_TAX_LIEN:
            break;
        case ARTIFACT_HIDEOUS_MASK:
            break;
        case ARTIFACT_ENDLESS_POUCH_SULFUR:
            break;
        case ARTIFACT_ENDLESS_VIAL_MERCURY:
            break;
        case ARTIFACT_ENDLESS_POUCH_GEMS:
            break;
        case ARTIFACT_ENDLESS_CORD_WOOD:
            break;
        case ARTIFACT_ENDLESS_CART_ORE:
            break;
        case ARTIFACT_ENDLESS_POUCH_CRYSTAL:
            break;
        case ARTIFACT_SPIKED_HELM:
            stats[IDX(HERO_PRIMARY_ATTACK)] = 1;
            stats[IDX(HERO_PRIMARY_DEFENSE)] = 1;
            break;
        case ARTIFACT_SPIKED_SHIELD:
            stats[IDX(HERO_PRIMARY_ATTACK)] = 2;
            stats[IDX(HERO_PRIMARY_DEFENSE)] = 2;
            break;
        case ARTIFACT_WHITE_PEARL:
            stats[IDX(HERO_PRIMARY_SPELL_POWER)] = 1;
            stats[IDX(HERO_PRIMARY_KNOWLEDGE)] = 1;
            break;
        case ARTIFACT_BLACK_PEARL:
            stats[IDX(HERO_PRIMARY_SPELL_POWER)] = 2;
            stats[IDX(HERO_PRIMARY_KNOWLEDGE)] = 2;
            break;
        case ARTIFACT_MAGIC_BOOK:
            break;
        case ARTIFACT_SPELL_SCROLL:
            break;
        case ARTIFACT_ARM_OF_MARTYR:
            stats[IDX(HERO_PRIMARY_SPELL_POWER)] = 3;
            break;
        case ARTIFACT_BREASTPLATE_ANDURAN:
            stats[IDX(HERO_PRIMARY_DEFENSE)] = 5;
            break;
        case ARTIFACT_BROACH_SHIELDING:
            stats[IDX(HERO_PRIMARY_SPELL_POWER)] = -2;
            break;
        case ARTIFACT_BATTLE_GARB:
            stats[IDX(HERO_PRIMARY_SPELL_POWER)] = 5;
            stats[IDX(HERO_PRIMARY_DEFENSE)] = 5;
            stats[IDX(HERO_PRIMARY_ATTACK)] = 5;
            break;
        case ARTIFACT_CRYSTAL_BALL:
            break;
        case ARTIFACT_HEART_FIRE:
            break;
        case ARTIFACT_HEART_ICE:
            break;
        case ARTIFACT_HELMET_ANDURAN:
            stats[IDX(HERO_PRIMARY_SPELL_POWER)] = 5;
            break;
        case ARTIFACT_HOLY_HAMMER:
            stats[IDX(HERO_PRIMARY_ATTACK)] = 5;
            break;
        case ARTIFACT_LEGENDARY_SCEPTER:
            stats[IDX(HERO_PRIMARY_SPELL_POWER)] = 2;
            stats[IDX(HERO_PRIMARY_ATTACK)] = 2;
            stats[IDX(HERO_PRIMARY_DEFENSE)] = 2;
            stats[IDX(HERO_PRIMARY_KNOWLEDGE)] = 2;
            break;
        case ARTIFACT_MASTHEAD:
            break;
        case ARTIFACT_SPHERE_NEGATION:
            break;
        case ARTIFACT_STAFF_WIZARDRY:
            stats[IDX(HERO_PRIMARY_SPELL_POWER)] = 5;
            break;
        case ARTIFACT_SWORD_BREAKER:
            stats[IDX(HERO_PRIMARY_DEFENSE)] = 4;
            stats[IDX(HERO_PRIMARY_ATTACK)] = 1;
            break;
        case ARTIFACT_SWORD_ANDURAN:
            stats[IDX(HERO_PRIMARY_ATTACK)] = 5;
            break;
        case ARTIFACT_SPADE_NECROMANCY:
            break;
        default:
            break;
    }

    for (i = 0; i < EVENT_ARTIFACT_PRIMARY_STAT_COUNT; i++) {
        targetHero->m_primaryStats[i] += (take == EVENT_ARTIFACT_TAKE ? -1 : 1) * stats[i];
        if (i == IDX(HERO_PRIMARY_KNOWLEDGE) && take == EVENT_ARTIFACT_TAKE) {
            maxSpellPoints =
                targetHero->Stats(HERO_PRIMARY_KNOWLEDGE) * EVENT_ARTIFACT_SPELL_POINT_MULTIPLIER;
            if (targetHero->m_spellPoints > maxSpellPoints)
                targetHero->m_spellPoints = static_cast<i16>(maxSpellPoints);
        }
    }
}

VA(0x00444949, 0x1b2)
void advManager::TransferArtifacts(hero* sourceHero, hero* destinationHero) {
    i32 targetSlot;
    i32 sourceArtifactSlot;

    if (sourceHero == NULL || destinationHero == NULL) {
        return;
    }
    for (targetSlot = 0; targetSlot < EVENT_ARTIFACT_SLOT_COUNT; targetSlot++) {
        if (destinationHero->m_artifacts[targetSlot] == ARTIFACT_NONE) {
            for (sourceArtifactSlot = 0; sourceArtifactSlot < EVENT_ARTIFACT_SLOT_COUNT;
                 sourceArtifactSlot++) {
                if (sourceHero->m_artifacts[sourceArtifactSlot] != ARTIFACT_NONE
                    && sourceHero->m_artifacts[sourceArtifactSlot] != ARTIFACT_MAGIC_BOOK) {
                    if (sourceHero->m_artifacts[sourceArtifactSlot] <= ARTIFACT_GOLDEN_GOOSE) {
                        if (gbThisNetHumanPlayer[sourceHero->m_owner]
                            || gbThisNetHumanPlayer[destinationHero->m_owner]) {
                            sprintf(
                                gText,
                                "As you reach for the %s, it mysteriously disappears.",
                                gArtifactNames[IDX(sourceHero->m_artifacts[sourceArtifactSlot])]
                            );
                            NormalDialog(
                                gText,
                                NORMAL_DIALOG_INFO,
                                -1,
                                -1,
                                NORMAL_DIALOG_ARTIFACT,
                                IDX(sourceHero->m_artifacts[sourceArtifactSlot]),
                                -1,
                                0,
                                -1,
                                0
                            );
                        }
                    } else {
                        GiveTakeArtifactStat(
                            destinationHero,
                            sourceHero->m_artifacts[sourceArtifactSlot],
                            false
                        );
                        destinationHero->m_artifacts[targetSlot] =
                            sourceHero->m_artifacts[sourceArtifactSlot];
                        destinationHero->m_artifactExtra[targetSlot] =
                            sourceHero->m_artifactExtra[sourceArtifactSlot];
                    }
                    GiveTakeArtifactStat(
                        sourceHero,
                        sourceHero->m_artifacts[sourceArtifactSlot],
                        true
                    );
                    sourceHero->m_artifacts[sourceArtifactSlot] = ARTIFACT_NONE;
                    sourceHero->m_artifactExtra[sourceArtifactSlot] = IDX(ARTIFACT_NONE);
                    break;
                }
            }
        }
    }
    destinationHero->CheckAnduranPieces(0);
}

VA(0x00444afb, 0x6d)
void advManager::HeroLoses(hero* lostHero) {
    if (lostHero == NULL)
        return;
    {
        CompleteDraw(m_mapOriginX, m_mapOriginY, 0, 1);
        UpdateScreen(0, 0);
        lostHero->Deallocate(1);
        FizzleCenter(EVENT_FIZZLE_HERO_LOSS);
        UpdateRadar(1, 0);
        UpdateHeroLocators(1, 1);
    }
}

VA(0x00444b68, 0x118)
void advManager::DoWhirlpool(hero* eventHero) {
    i32 selectedSlot;
    i32 slotNo;
    i32 groupValues[MONSTER_ARMY_SLOTS];
    i32 lowestValue;
    i32 creatureValue;

    if (gbHumanPlayer[eventHero->m_owner] == 0)
        return;
    if (Random(EVENT_WHIRLPOOL_TRIGGER_ROLL, EVENT_WHIRLPOOL_TRIGGER_MAX)
        != EVENT_WHIRLPOOL_TRIGGER_ROLL)
        return;
    {
        lowestValue = EVENT_WHIRLPOOL_ARMY_VALUE_LIMIT;
        selectedSlot = -1;
        for (slotNo = 0; slotNo < MONSTER_ARMY_SLOTS; slotNo++) {
            if (eventHero->m_army.m_creatureCounts[slotNo] > 0) {
                creatureValue = eventHero->m_army.m_creatureCounts[slotNo]
                    * gMonsterDatabase[IDX(eventHero->m_army.m_creatureTypes[slotNo])].fightValue;
                if (creatureValue < lowestValue) {
                    lowestValue = creatureValue;
                    selectedSlot = slotNo;
                }
            }
        }
        if (eventHero->m_army.GetNumArmies() > 1) {
            eventHero->m_army.m_creatureCounts[selectedSlot] >>= 1;
            if (eventHero->m_army.m_creatureCounts[selectedSlot] == 0)
                eventHero->m_army.m_creatureTypes[selectedSlot] = CREATURE_NONE;
        } else if (eventHero->m_army.m_creatureCounts[selectedSlot] > 1) {
            eventHero->m_army.m_creatureCounts[selectedSlot] >>= 1;
        }
    }
}

VA(0x00444c80, 0xf3)
void advManager::FizzleCenter(i32 fizzleType) {
    SAMPLE2 fizzleSample;
    i32 fizzleStepCount;

    if (bShowIt == 0)
        return;
    {
        switch (fizzleType) {
            case EVENT_FIZZLE_HERO_LOSS:
                sprintf(gText, "killfade.82M");
                break;
            case EVENT_FIZZLE_ARTIFACT:
                sprintf(
                    gText,
                    "pickup%02d.82M",
                    Random(ARTIFACT_PICKUP_SOUND_FIRST, ARTIFACT_PICKUP_SOUND_LAST)
                );
                break;
            default:
                return;
        }
        fizzleSample = LoadPlaySample(gText);
        gpMouseManager->HideColorPointer();
        gpWindowManager->SaveFizzleSource(
            EVENT_FIZZLE_X,
            EVENT_FIZZLE_Y,
            EVENT_FIZZLE_WIDTH,
            EVENT_FIZZLE_HEIGHT
        );
        CompleteDraw(0);
        fizzleStepCount = EVENT_FIZZLE_STEPS;
        gpWindowManager->FizzleForward(
            EVENT_FIZZLE_X,
            EVENT_FIZZLE_Y,
            EVENT_FIZZLE_WIDTH,
            EVENT_FIZZLE_HEIGHT,
            fizzleStepCount,
            NULL,
            NULL
        );
        gpMouseManager->ShowColorPointer();
        WaitEndSample(&fizzleSample, -1);
    }
}

VA(0x00444d73, 0x2b6c)
void advManager::DoAIEvent(mapCell* cell, hero* eventHero, i32 x, i32 y) {
    float spellValueFactor_i;
    u32 resourceAmount_o;
    i32 unusedEventResult_e;
    i32 unusedEventValue_o;
    mapCell* exitCell_d;
    i32 teleportDistance_o;
    i32 spell_g;
    u32 guardianCount_i;
    float attackerLoss_c;
    float defenderLoss_k;
    i32 levelExperience_g;
    i32 heroLevel_e;
    i32 creatureCosts_a[IDX(RES_COUNT)];
    i32 spellPower_j;
    i32 adjacentMonster_j;
    i32 artifactResource_p;
    hero* otherHero_e;
    i32 artifactGuardResult_e;
    CombatResult heroCombatResult_h;
    i32 exitY_d;
    ArtifactType artifact_g;
    i32 heroInteractionResult;
    ResourceType resourceType_a;
    i32 exitX;
    i32 exitCount;
    mapEventExtra* eventExtra_o;
    CreatureType artifactGuardCount_b;
    i32 survivingCount_a;
    boatRecord* boat_k;
    i32 savedShowIt_e;
    i32 mineId_j;
    i32 rewardWork_e[ARMY_GROUP_SLOT_COUNT];
    i32 resourceWork_p[IDX(RES_COUNT)];
    i32 eventResults[AI_EVENT_RESULT_COUNT];
    i32 abandonedMineValue_f;
    i32 index_h;
    town* occupiedTown_b;
    i32 combatResult_d;
    i32 eventWork_o[IDX(RES_COUNT)];
    i32 battleWon_j;
    i32 pyramidBattleValue_l;
    i32 oldPlayer_o;
    MapObjectType eventType_g;
    playerData* oldPlayerData_h;
    i32 battleResult_l;
    i32 purchaseCost_i;
    i32 purchaseValue_a;
    CreatureType creatureType_i;
    i32 creatureFlag_l;

    occupiedTown_b = NULL;
    eventType_g = cell->m_triggerType & MAP_TRIGGER_TYPE_MASK;
    eventResults[AI_EVENT_RESULT_ERASE_OBJECT] = 0;
    eventResults[AI_EVENT_RESULT_UNUSED] = 0;
    oldPlayer_o = giCurPlayer;
    oldPlayerData_h = gpCurPlayer;

    if (x == eventHero->m_destinationX && y == eventHero->m_destinationY) {
        eventHero->m_destinationY = -1;
        eventHero->m_destinationX = -1;
    }
    --eventHero->m_remainingMobility;
    switch (eventType_g) {
        case MAP_OBJECT_COAST:
            if (HAS(eventHero->m_eventFlags, HERO_EVENT_EMBARKED)) {
                eventHero->m_eventFlags = HeroEventFlag(
                    static_cast<i32>(eventHero->m_eventFlags) & ~IDX(HERO_EVENT_EMBARKED)
                );
                eventHero->m_remainingMobility = 0;
                eventHero->m_direction = m_cursorDirection;
                m_cursorType = eventHero->m_cursorType;
                m_cursorFrame = GetCursorBaseFrame(m_cursorDirection);
                m_cursorActive = 1;
                CheckAdjacentMon(&adjacentMonster_j);
            }
            break;

        case MAP_OBJECT_BOAT:
            boat_k = &gpGame->m_boats[cell->m_objectMetadata];
            gpGame->RestoreCell(
                -1,
                -1,
                boat_k->savedTriggerType,
                boat_k->savedEventData,
                cell,
                EVENT_BOAT_RESTORE_MODE
            );
            eventHero->m_eventFlags = HeroEventFlag(
                static_cast<i32>(eventHero->m_eventFlags) | IDX(HERO_EVENT_EMBARKED)
            );
            eventHero->m_remainingMobility = 0;
            boat_k->heroId = eventHero->m_id;
            boat_k->owner = eventHero->m_owner;
            m_cursorType = HERO_TYPE_BOAT;
            m_cursorDirection = boat_k->direction;
            m_cursorFrame = GetCursorBaseFrame(m_cursorDirection);
            m_cursorActive = 1;
            break;

        case MAP_OBJECT_ALCHEMIST_LAB:
        case MAP_OBJECT_MINE:
        case MAP_OBJECT_SAWMILL:
            if (gpGame->m_mineOwners[cell->m_objectMetadata] == giCurPlayer)
                break;
            if (gpGame->m_mines[cell->m_objectMetadata].guardianType != CREATURE_NONE) {
                index_h = gpGame->m_mines[cell->m_objectMetadata].guardianCount;
                combatResult_d = gpPhilAI->CombatMonsterEvent(
                    eventHero,
                    gpGame->m_mines[cell->m_objectMetadata].guardianType,
                    &index_h,
                    cell
                );
                if (combatResult_d == 0)
                    break;
                gpGame->m_mines[cell->m_objectMetadata].guardianType = CREATURE_NONE;
                gpGame->m_mines[cell->m_objectMetadata].guardianCount = 0;
                eventHero->CheckLevel();
            }
            gpGame->ClaimMine(cell->m_objectMetadata, giCurPlayer);
            if (eventType_g == MAP_OBJECT_MINE) {
                for (index_h = IDX(SPELL_SET_EARTH_GUARDIAN);
                     index_h < IDX(SPELL_SET_WATER_GUARDIAN);
                     ++index_h) {
                    if (eventHero->HasSpell(SpellType(index_h))
                        && eventHero->m_spellPoints > GetManaCost(SpellType(index_h), eventHero)) {
                        eventHero->m_spellPoints -= GetManaCost(SpellType(index_h), eventHero);
                        gpGame->m_mines[cell->m_objectMetadata].guardianType =
                            static_cast<CreatureType>(index_h + 1);
                        spellPower_j = eventHero->Stats(HERO_PRIMARY_SPELL_POWER);
                        if (spellPower_j > EVENT_MINE_SPELL_POWER_MAX)
                            spellPower_j = EVENT_MINE_SPELL_POWER_MAX;
                        gpGame->m_mines[cell->m_objectMetadata].guardianCount =
                            static_cast<u8>(spellPower_j * EVENT_MINE_SPELL_COUNT_SCALE);
                        index_h = EVENT_MINE_SPELL_LOOP_END;
                    }
                }
            }
            break;

        case MAP_OBJECT_LIGHTHOUSE:
            if (gpGame->m_mineOwners[cell->m_objectMetadata] == giCurPlayer)
                break;
            gpGame->ClaimMine(cell->m_objectMetadata, giCurPlayer);
            break;

        case MAP_OBJECT_TREASURE_CHEST:
            if (cell->m_objectMetadata & CHEST_ARTIFACT_FLAG) {
                if (eventHero->NumArtifacts() >= HERO_ARTIFACT_SLOT_COUNT) {
                    cell->m_objectMetadata = CHEST_REWARD_SMALL;
                    goto chestGoldOrExperience;
                }
                GiveArtifact(
                    eventHero,
                    ArtifactType(cell->m_objectMetadata & CHEST_ARTIFACT_MASK),
                    1,
                    -1
                );
            } else {
            chestGoldOrExperience:
                if (gpPhilAI->ChooseGoldOrExperience(
                        cell->m_objectMetadata * CHEST_GOLD_MULTIPLIER,
                        (cell->m_objectMetadata - CHEST_EXPERIENCE_LEVEL_OFFSET)
                            * CHEST_EXPERIENCE_MULTIPLIER
                    )) {
                    GiveResource(
                        eventHero,
                        RES_GOLD,
                        cell->m_objectMetadata * CHEST_GOLD_MULTIPLIER
                    );
                } else {
                    GiveExperience(
                        eventHero,
                        (cell->m_objectMetadata - CHEST_EXPERIENCE_LEVEL_OFFSET)
                            * CHEST_EXPERIENCE_MULTIPLIER,
                        1
                    );
                    eventHero->CheckLevel();
                }
            }
            eventResults[AI_EVENT_RESULT_ERASE_OBJECT] = 1;
            break;

        case MAP_OBJECT_WATERING_HOLE:
            if (HAS(eventHero->m_eventFlags, HERO_EVENT_WATERING_HOLE) == 0) {
                eventHero->m_mobility += WATERING_HOLE_MOBILITY_BONUS;
                eventHero->m_remainingMobility += WATERING_HOLE_MOBILITY_BONUS;
                eventHero->m_eventFlags = HeroEventFlag(
                    static_cast<i32>(eventHero->m_eventFlags) | IDX(HERO_EVENT_WATERING_HOLE)
                );
                ++eventHero->m_morale;
            }
            break;

        case MAP_OBJECT_BUOY:
            if (HAS(eventHero->m_eventFlags, HERO_EVENT_BUOY) == 0) {
                eventHero->m_eventFlags = HeroEventFlag(
                    static_cast<i32>(eventHero->m_eventFlags) | IDX(HERO_EVENT_BUOY)
                );
                ++eventHero->m_morale;
            }
            break;

        case MAP_OBJECT_FAERIE_RING:
            if (HAS(eventHero->m_eventFlags, HERO_EVENT_FAERIE_RING) == 0) {
                eventHero->m_eventFlags = HeroEventFlag(
                    static_cast<i32>(eventHero->m_eventFlags) | IDX(HERO_EVENT_FAERIE_RING)
                );
                ++eventHero->m_luck;
            }
            break;

        case MAP_OBJECT_IDOL:
            if (HAS(eventHero->m_eventFlags, HERO_EVENT_IDOL) == 0) {
                eventHero->m_eventFlags = HeroEventFlag(
                    static_cast<i32>(eventHero->m_eventFlags) | IDX(HERO_EVENT_IDOL)
                );
                ++eventHero->m_luck;
            }
            break;

        case MAP_OBJECT_FOUNTAIN:
            if (HAS(eventHero->m_eventFlags, HERO_EVENT_FOUNTAIN) == 0) {
                eventHero->m_eventFlags = HeroEventFlag(
                    static_cast<i32>(eventHero->m_eventFlags) | IDX(HERO_EVENT_FOUNTAIN)
                );
                ++eventHero->m_luck;
            }
            break;

        case MAP_OBJECT_OASIS:
            if (HAS(eventHero->m_eventFlags, HERO_EVENT_OASIS) == 0) {
                eventHero->m_eventFlags = HeroEventFlag(
                    static_cast<i32>(eventHero->m_eventFlags) | IDX(HERO_EVENT_OASIS)
                );
                ++eventHero->m_morale;
                eventHero->m_mobility += OASIS_MOBILITY_BONUS;
                eventHero->m_remainingMobility += OASIS_MOBILITY_BONUS;
            }
            break;

        case MAP_OBJECT_TEMPLE:
            if (HAS(eventHero->m_eventFlags, HERO_EVENT_TEMPLE) == 0) {
                eventHero->m_eventFlags = HeroEventFlag(
                    static_cast<i32>(eventHero->m_eventFlags) | IDX(HERO_EVENT_TEMPLE)
                );
                eventHero->m_morale += TEMPLE_MORALE_BONUS;
            }
            break;

        case MAP_OBJECT_SKELETON:
            switch (cell->m_objectMetadata) {
                case SKELETON_EMPTY:
                    break;
                default:
                    GiveArtifact(
                        eventHero,
                        ArtifactType(cell->m_objectMetadata - SKELETON_ARTIFACT_OFFSET),
                        1,
                        -1
                    );
                    cell->m_objectMetadata = SKELETON_EMPTY;
                    break;
            }
            break;

        case MAP_OBJECT_MAGIC_GARDEN:
            if (cell->m_objectMetadata != MAP_EVENT_DATA_EMPTY) {
                GiveResource(
                    eventHero,
                    static_cast<ResourceType>(
                        cell->m_objectMetadata - MAP_EVENT_RESOURCE_OFFSET
                    ),
                    static_cast<ResourceType>(
                        cell->m_objectMetadata - MAP_EVENT_RESOURCE_OFFSET
                    ) == RES_GOLD
                        ? MAP_EVENT_GOLD_AMOUNT
                        : MAP_EVENT_RESOURCE_AMOUNT
                );
                cell->m_objectMetadata = MAP_EVENT_DATA_EMPTY;
            }
            break;

        case MAP_OBJECT_LEAN_TO:
            if (cell->m_objectMetadata != MAP_EVENT_DATA_EMPTY) {
                GiveResource(
                    eventHero,
                    ResourceType((cell->m_objectMetadata & IDX(ARTIFACT_EVENT_MODE_MASK)) - 1),
                    (cell->m_objectMetadata & ARTIFACT_EVENT_RESOURCE_MASK)
                        >> ARTIFACT_EVENT_RESOURCE_SHIFT
                );
                cell->m_objectMetadata = MAP_EVENT_DATA_EMPTY;
            }
            break;

        case MAP_OBJECT_WAGON:
            if (cell->m_objectMetadata == MAP_EVENT_DATA_EMPTY) {
                break;
            } else {
                if (cell->m_objectMetadata & WAGON_ARTIFACT_FLAG) {
                    if (eventHero->NumArtifacts() != HERO_ARTIFACT_SLOT_COUNT) {
                        artifact_g = ArtifactType(cell->m_objectMetadata & WAGON_ARTIFACT_MASK);
                        GiveArtifact(eventHero, artifact_g, 1, -1);
                    }
                    cell->m_objectMetadata = MAP_EVENT_DATA_EMPTY;
                } else {
                    GiveResource(
                        eventHero,
                        ResourceType((cell->m_objectMetadata & IDX(ARTIFACT_EVENT_MODE_MASK)) - 1),
                        (cell->m_objectMetadata & ARTIFACT_EVENT_RESOURCE_MASK)
                            >> ARTIFACT_EVENT_RESOURCE_SHIFT
                    );
                    cell->m_objectMetadata = MAP_EVENT_DATA_EMPTY;
                }
            }
            break;

        case MAP_OBJECT_SEA_CHEST:
            if ((cell->m_objectMetadata & CHEST_ARTIFACT_FLAG) != 0
                && eventHero->NumArtifacts() < HERO_ARTIFACT_SLOT_COUNT) {
                GiveArtifact(
                    eventHero,
                    ArtifactType(cell->m_objectMetadata & CHEST_ARTIFACT_MASK),
                    1,
                    -1
                );
                GiveResource(eventHero, RES_GOLD, EVENT_SEA_CHEST_ARTIFACT_GOLD);
            } else if (cell->m_objectMetadata != 0) {
                GiveResource(eventHero, RES_GOLD, EVENT_SEA_CHEST_GOLD);
            }
            eventResults[AI_EVENT_RESULT_ERASE_OBJECT] = 1;
            break;

        case MAP_OBJECT_FLOTSAM:
            switch (static_cast<FlotsamReward>(cell->m_objectMetadata)) {
                case FLOTSAM_EMPTY:
                    break;
                case FLOTSAM_WOOD:
                    GiveResource(eventHero, RES_WOOD, FLOTSAM_WOOD_AMOUNT);
                    break;
                case FLOTSAM_WOOD_AND_GOLD:
                    GiveResource(eventHero, RES_WOOD, FLOTSAM_WOOD_AMOUNT);
                    GiveResource(eventHero, RES_GOLD, FLOTSAM_GOLD_AMOUNT);
                    break;
                case FLOTSAM_LARGE_TREASURE:
                    GiveResource(eventHero, RES_WOOD, FLOTSAM_LARGE_WOOD_AMOUNT);
                    GiveResource(eventHero, RES_GOLD, FLOTSAM_LARGE_GOLD_AMOUNT);
                    break;
            }
            eventResults[AI_EVENT_RESULT_ERASE_OBJECT] = 1;
            break;

        case MAP_OBJECT_CAMPFIRE:
            GiveResource(
                eventHero,
                RES_GOLD,
                (cell->m_objectMetadata >> CAMPFIRE_AMOUNT_SHIFT) * CAMPFIRE_GOLD_MULTIPLIER
            );
            GiveResource(
                eventHero,
                ResourceType(cell->m_objectMetadata & IDX(CAMPFIRE_RESOURCE_MASK)),
                cell->m_objectMetadata >> CAMPFIRE_AMOUNT_SHIFT
            );
            eventResults[AI_EVENT_RESULT_ERASE_OBJECT] = 1;
            break;

        case MAP_OBJECT_FORT:
            if ((eventHero->m_fortVisits & (1U << cell->m_objectMetadata)) == 0) {
                ++eventHero->m_primaryStats[IDX(HERO_PRIMARY_DEFENSE)];
                eventHero->m_fortVisits |= 1U << cell->m_objectMetadata;
            }
            break;

        case MAP_OBJECT_XANADU:
            if ((eventHero->m_xanaduVisits & (1U << cell->m_objectMetadata)) == 0
                && eventHero->m_level
                           + IDX(eventHero->m_secondarySkills[IDX(HERO_SKILL_DIPLOMACY)])
                                 * XANADU_DIPLOMACY_MULTIPLIER
                       >= XANADU_ADMISSION_LEVEL) {
                ++eventHero->m_primaryStats[IDX(HERO_PRIMARY_ATTACK)];
                ++eventHero->m_primaryStats[IDX(HERO_PRIMARY_DEFENSE)];
                ++eventHero->m_primaryStats[IDX(HERO_PRIMARY_KNOWLEDGE)];
                ++eventHero->m_primaryStats[IDX(HERO_PRIMARY_SPELL_POWER)];
                eventHero->m_xanaduVisits |= 1U << cell->m_objectMetadata;
            }
            break;

        case MAP_OBJECT_STANDING_STONES:
            if ((eventHero->m_standingStoneVisits & (1U << cell->m_objectMetadata)) == 0) {
                ++eventHero->m_primaryStats[IDX(HERO_PRIMARY_SPELL_POWER)];
                eventHero->m_standingStoneVisits |= 1U << cell->m_objectMetadata;
            }
            break;

        case MAP_OBJECT_WITCH_DOCTOR_HUT:
            if ((eventHero->m_witchDoctorVisits & (1U << cell->m_objectMetadata)) == 0) {
                ++eventHero->m_primaryStats[IDX(HERO_PRIMARY_KNOWLEDGE)];
                eventHero->m_witchDoctorVisits |= 1U << cell->m_objectMetadata;
            }
            break;

        case MAP_OBJECT_MERCENARY_CAMP:
            if ((eventHero->m_mercenaryCampVisits & (1U << cell->m_objectMetadata)) == 0) {
                ++eventHero->m_primaryStats[IDX(HERO_PRIMARY_ATTACK)];
                eventHero->m_mercenaryCampVisits |= 1U << cell->m_objectMetadata;
            }
            break;

        case MAP_OBJECT_GAZEBO:
            if ((eventHero->m_gazeboVisits & (1U << cell->m_objectMetadata)) == 0) {
                GiveExperience(eventHero, GAZEBO_EXPERIENCE, 1);
                eventHero->m_gazeboVisits |= 1U << cell->m_objectMetadata;
                eventHero->CheckLevel();
            }
            break;

        case MAP_OBJECT_WATER_WHEEL:
            if (cell->m_objectMetadata != 0) {
                GiveResource(eventHero, RES_GOLD, cell->m_objectMetadata * CHEST_GOLD_MULTIPLIER);
                cell->m_objectMetadata = MAP_EVENT_DATA_EMPTY;
            }
            break;

        case MAP_OBJECT_RESOURCE:
            resourceType_a = static_cast<ResourceType>(cell->m_objectIndex / 2);
            GiveResource(
                eventHero,
                resourceType_a,
                resourceType_a == RES_GOLD ? cell->m_objectMetadata * CAMPFIRE_GOLD_MULTIPLIER
                                           : cell->m_objectMetadata
            );
            eventResults[AI_EVENT_RESULT_ERASE_OBJECT] = 1;
            break;

        case MAP_OBJECT_WINDMILL:
            if (cell->m_objectMetadata != EVENT_WINDMILL_EMPTY) {
                GiveResource(
                    eventHero,
                    ResourceType(cell->m_objectMetadata),
                    WINDMILL_RESOURCE_AMOUNT
                );
                cell->m_objectMetadata = EVENT_WINDMILL_EMPTY;
            }
            break;

        case MAP_OBJECT_HILL_FORT:
            eventHero->UpgradeCreatures(CREATURE_OGRE, CREATURE_OGRE_LORD);
            eventHero->UpgradeCreatures(CREATURE_ORC, CREATURE_ORC_CHIEF);
            eventHero->UpgradeCreatures(CREATURE_DWARF, CREATURE_BATTLE_DWARF);
            break;

        case MAP_OBJECT_FREEMANS_FOUNDRY:
            eventHero->UpgradeCreatures(CREATURE_IRON_GOLEM, CREATURE_STEEL_GOLEM);
            eventHero->UpgradeCreatures(CREATURE_PIKEMAN, CREATURE_VETERAN_PIKEMAN);
            eventHero->UpgradeCreatures(CREATURE_SWORDSMAN, CREATURE_MASTER_SWORDSMAN);
            break;

        case MAP_OBJECT_TREE_CITY:
            creatureType_i = CREATURE_SPRITE;
            creatureFlag_l = 0;
            goto creaturePurchase;
        case MAP_OBJECT_RUINS:
            creatureType_i = CREATURE_MEDUSA;
            creatureFlag_l = 0;
            goto creaturePurchase;
        case MAP_OBJECT_TROLL_BRIDGE:
            if (cell->m_objectMetadata & DWELLING_GUARDED_FLAG)
                break;
            creatureType_i = CREATURE_TROLL;
            creatureFlag_l = 0;
            goto creaturePurchase;
        case MAP_OBJECT_CITY_OF_DEAD:
            if (cell->m_objectMetadata & DWELLING_GUARDED_FLAG)
                break;
            creatureType_i = CREATURE_POWER_LICH;
            creatureFlag_l = 0;
            goto creaturePurchase;
        case MAP_OBJECT_DRAGON_CITY:
            if (cell->m_objectMetadata & DWELLING_GUARDED_FLAG)
                break;
            creatureType_i = CREATURE_RED_DRAGON;
            creatureFlag_l = 0;
            goto creaturePurchase;
        case MAP_OBJECT_HALFLING_HOLE:
            creatureType_i = CREATURE_HALFLING;
            creatureFlag_l = 1;
            goto creaturePurchase;
        case MAP_OBJECT_ANCIENT_LAMP:
            creatureType_i = CREATURE_GENIE;
            creatureFlag_l = 0;
            goto creaturePurchase;
        case MAP_OBJECT_WAGON_CAMP:
            creatureType_i = CREATURE_ROGUE;
            creatureFlag_l = 0;
            goto creaturePurchase;
        case MAP_OBJECT_DESERT_TENT:
            creatureType_i = CREATURE_NOMAD;
            creatureFlag_l = 0;
            goto creaturePurchase;
        case MAP_OBJECT_WATCH_TOWER:
            creatureType_i = CREATURE_ORC;
            creatureFlag_l = 1;
            goto creaturePurchase;
        case MAP_OBJECT_TREE_HOUSE:
            creatureType_i = CREATURE_SPRITE;
            creatureFlag_l = 1;
            goto creaturePurchase;
        case MAP_OBJECT_ARCHER_HOUSE:
            creatureType_i = CREATURE_ARCHER;
            creatureFlag_l = 1;
            goto creaturePurchase;
        case MAP_OBJECT_GOBLIN_HUT:
            creatureType_i = CREATURE_GOBLIN;
            creatureFlag_l = 1;
            goto creaturePurchase;
        case MAP_OBJECT_PEASANT_HUT:
            creatureType_i = CREATURE_PEASANT;
            creatureFlag_l = 1;
            goto creaturePurchase;
        case MAP_OBJECT_DWARF_COTTAGE:
        case MAP_OBJECT_SIRENS:
            creatureType_i = CREATURE_DWARF;
            creatureFlag_l = 1;
            goto creaturePurchase;
        case MAP_OBJECT_CAVE:
            creatureType_i = CREATURE_CENTAUR;
            creatureFlag_l = 1;
            goto creaturePurchase;
        case MAP_OBJECT_EXCAVATION:
            creatureType_i = CREATURE_SKELETON;
            creatureFlag_l = 1;
        creaturePurchase:
            if (cell->m_objectMetadata != 0) {
                gpPhilAI->EvaluateOneTimeCreaturePurchase(
                    creatureType_i,
                    cell->m_objectMetadata,
                    creatureFlag_l,
                    eventResults[AI_EVENT_RESULT_PURCHASE_COUNT],
                    purchaseCost_i,
                    purchaseValue_a
                );
                if (eventResults[AI_EVENT_RESULT_PURCHASE_COUNT] > 0) {
                    gpGame->GiveArmy(
                        &eventHero->m_army,
                        creatureType_i,
                        eventResults[AI_EVENT_RESULT_PURCHASE_COUNT],
                        purchaseValue_a
                    );
                    cell->m_objectMetadata -= eventResults[AI_EVENT_RESULT_PURCHASE_COUNT];
                    if (creatureFlag_l == 0) {
                        GetMonsterCost(creatureType_i, creatureCosts_a);
                        for (index_h = 0; index_h < IDX(RES_COUNT); ++index_h)
                            gpCurPlayer->m_resources[index_h] -=
                                eventResults[AI_EVENT_RESULT_PURCHASE_COUNT]
                                * creatureCosts_a[index_h];
                    }
                }
            }
            if (cell->m_objectMetadata == 0 && eventType_g == MAP_OBJECT_ANCIENT_LAMP)
                eventResults[AI_EVENT_RESULT_ERASE_OBJECT] = 1;
            break;

        case MAP_OBJECT_MONSTER:
            ComputerMonsterInteract(cell, eventHero, eventResults);
            break;

        case MAP_OBJECT_TREE_OF_KNOWLEDGE:
            if ((eventHero->m_treeKnowledgeVisits
                 & (1U << (cell->m_objectMetadata & TREE_KNOWLEDGE_VISIT_INDEX_MASK)))
                == 0) {
                heroLevel_e = eventHero->GetLevel(eventHero->m_experience);
                levelExperience_g = eventHero->GetExperience(heroLevel_e + 1)
                                    - eventHero->GetExperience(heroLevel_e);
                switch (cell->m_objectMetadata >> TREE_KNOWLEDGE_MODE_SHIFT) {
                    case TREE_KNOWLEDGE_FREE:
                        GiveExperience(eventHero, levelExperience_g, 1);
                        eventHero->m_treeKnowledgeVisits |=
                            1U << (cell->m_objectMetadata & TREE_KNOWLEDGE_VISIT_INDEX_MASK);
                        eventHero->CheckLevel();
                        break;
                    case TREE_KNOWLEDGE_GOLD:
                        if (gpCurPlayer->m_resources[IDX(RES_GOLD)] > EVENT_TREE_GOLD) {
                            gpCurPlayer->m_resources[IDX(RES_GOLD)] -= EVENT_TREE_GOLD;
                            GiveExperience(eventHero, levelExperience_g, 1);
                            eventHero->m_treeKnowledgeVisits |=
                                1U << (cell->m_objectMetadata & TREE_KNOWLEDGE_VISIT_INDEX_MASK);
                            eventHero->CheckLevel();
                        }
                        break;
                    case TREE_KNOWLEDGE_GEMS:
                        if (gpCurPlayer->m_resources[IDX(RES_GEMS)] > EVENT_TREE_GEMS) {
                            gpCurPlayer->m_resources[IDX(RES_GEMS)] -= EVENT_TREE_GEMS;
                            GiveExperience(eventHero, levelExperience_g, 1);
                            eventHero->m_treeKnowledgeVisits |=
                                1U << (cell->m_objectMetadata & TREE_KNOWLEDGE_VISIT_INDEX_MASK);
                            eventHero->CheckLevel();
                        }
                        break;
                }
            }
            break;

        case MAP_OBJECT_OBELISK:
            if ((gpGame->m_obeliskVisitors[cell->m_objectMetadata - 1] & giCurPlayerBit) == 0) {
                gpGame->m_obeliskVisitors[cell->m_objectMetadata - 1] |= giCurPlayerBit;
                ComputeUALoc(giCurPlayer);
            }
            break;

        case MAP_OBJECT_ORACLE:
            break;

        case MAP_OBJECT_SHRINE_FIRST_CIRCLE:
        case MAP_OBJECT_SHRINE_SECOND_CIRCLE:
        case MAP_OBJECT_SHRINE_THIRD_CIRCLE:
            if (eventHero->HasArtifact(ARTIFACT_MAGIC_BOOK)
                && IDX(gsSpellInfo[cell->m_objectMetadata - 1].level)
                       <= IDX(eventHero->m_secondarySkills[IDX(HERO_SKILL_WISDOM)])
                              + WISDOM_SPELL_LEVEL_BONUS) {
                eventHero->AddSpell(
                    static_cast<SpellType>(cell->m_objectMetadata - 1),
                    eventHero->Stats(HERO_PRIMARY_KNOWLEDGE)
                );
            }
            break;

        case MAP_OBJECT_CASTLE:
            gpPhilAI->TownEvent(cell, eventHero, x, y);
            break;

        case MAP_OBJECT_WHIRLPOOL:
            DoWhirlpool(eventHero);
        case MAP_OBJECT_STONE_LITHS:
            exitCount = 0;
            for (exitY_d = 0; exitY_d < MAP_HEIGHT; ++exitY_d) {
                for (exitX = 0; exitX < MAP_WIDTH; ++exitX) {
                    if (gpGame->m_worldMap.GetCell(exitX, exitY_d)->m_triggerType
                            == static_cast<u8>(eventType_g | MAP_TRIGGER_ACTION_FLAG)
                        && gpGame->m_worldMap.GetCell(exitX, exitY_d)->m_objectIndex
                               == cell->m_objectIndex
                        && abs(exitX - x) + abs(exitY_d - y)
                               > (eventType_g == MAP_OBJECT_STONE_LITHS
                                      ? EVENT_TELEPORT_STONE_DISTANCE
                                      : EVENT_TELEPORT_WHIRLPOOL_DISTANCE)) {
                        ++exitCount;
                    }
                }
            }
            if (exitCount > 0) {
                if (exitCount > 1)
                    exitCount = Random(1, exitCount);
                for (exitY_d = 0; exitY_d < MAP_HEIGHT; ++exitY_d) {
                    for (exitX = 0; exitX < MAP_WIDTH; ++exitX) {
                        if (gpGame->m_worldMap.GetCell(exitX, exitY_d)->m_triggerType
                                == static_cast<u8>(eventType_g | MAP_TRIGGER_ACTION_FLAG)
                            && gpGame->m_worldMap.GetCell(exitX, exitY_d)->m_objectIndex
                                   == cell->m_objectIndex
                            && abs(exitX - x) + abs(exitY_d - y)
                                   > (eventType_g == MAP_OBJECT_STONE_LITHS
                                          ? EVENT_TELEPORT_STONE_DISTANCE
                                          : EVENT_TELEPORT_WHIRLPOOL_DISTANCE)
                            && --exitCount < 1) {
                            goto teleportDestination;
                        }
                    }
                }
            teleportDestination:
                StopCursor(1);
                gpAdvManager->TeleportTo(eventHero, exitX, exitY_d, 0, 0);
            }
            break;

        case MAP_OBJECT_ARTIFACT:
            artifactResource_p = (cell->m_objectMetadata & ARTIFACT_EVENT_RESOURCE_MASK)
                                 >> ARTIFACT_EVENT_RESOURCE_SHIFT;
            artifact_g = static_cast<ArtifactType>(cell->m_objectIndex / 2);
            artifactGuardCount_b = static_cast<CreatureType>(
                cell->m_objectMetadata & ARTIFACT_EVENT_MONSTER_MASK
            );
            if (eventHero->NumArtifacts() == HERO_ARTIFACT_SLOT_COUNT)
                break;
            if (artifact_g == ARTIFACT_SPELL_SCROLL) {
                GiveArtifact(eventHero, artifact_g, 1, static_cast<i8>(cell->m_objectMetadata));
                eventResults[AI_EVENT_RESULT_ERASE_OBJECT] = 1;
                break;
            }
            if (cell->m_objectMetadata & ARTIFACT_EVENT_GUARDED_FLAG) {
                if (artifactGuardCount_b == CREATURE_ROGUE) {
                    artifactGuardResult_e = EVENT_ROGUE_COUNT;
                    goto artifactFight;
                }
                artifactGuardResult_e = 1;
                if (gpPhilAI->ChooseToFightForArtifact(artifact_g, artifactGuardCount_b, 1)) {
                artifactFight:
                    if (gpPhilAI->CombatMonsterEvent(
                            eventHero,
                            artifactGuardCount_b,
                            &artifactGuardResult_e,
                            cell
                        ))
                        goto artifactPickup;
                }
                break;
            }
            switch (cell->m_objectMetadata & ARTIFACT_EVENT_MODE_MASK) {
                case ARTIFACT_EVENT_MODE_WISDOM:
                    if (eventHero->m_secondarySkills[IDX(HERO_SKILL_WISDOM)]
                        != HERO_SKILL_LEVEL_NONE)
                        goto artifactPickup;
                    break;
                case ARTIFACT_EVENT_MODE_LEADERSHIP:
                    if (eventHero->m_secondarySkills[IDX(HERO_SKILL_LEADERSHIP)]
                        != HERO_SKILL_LEVEL_NONE)
                        goto artifactPickup;
                    break;
                case ARTIFACT_EVENT_MODE_PICKUP:
                artifactPickup:
                    for (index_h = 0; index_h < IDX(RES_COUNT); ++index_h) {
                        if (gpCurPlayer->m_resources[index_h] < 0)
                            gpCurPlayer->m_resources[index_h] = 0;
                    }
                    GiveArtifact(eventHero, artifact_g, 1, -1);
                    eventResults[AI_EVENT_RESULT_ERASE_OBJECT] = 1;
                    break;
                case ARTIFACT_EVENT_MODE_GOLD:
                    if (gpPhilAI->NetValueOfArtifact(IDX(artifact_g), EVENT_ARTIFACT_GOLD, 0, 0)) {
                        gpGame->m_players[eventHero->m_owner].m_resources[IDX(RES_GOLD)] -=
                            EVENT_ARTIFACT_GOLD;
                        goto artifactPickup;
                    }
                    break;
                case ARTIFACT_EVENT_MODE_RESOURCE_3:
                    if (gpPhilAI->NetValueOfArtifact(
                            IDX(artifact_g),
                            EVENT_ARTIFACT_RESOURCE_3_GOLD,
                            artifactResource_p,
                            EVENT_ARTIFACT_RESOURCE_3
                        )) {
                        gpGame->m_players[eventHero->m_owner].m_resources[IDX(RES_GOLD)] -=
                            EVENT_ARTIFACT_RESOURCE_3_GOLD;
                        gpGame->m_players[eventHero->m_owner].m_resources[artifactResource_p] -=
                            EVENT_ARTIFACT_RESOURCE_3;
                        goto artifactPickup;
                    }
                    break;
                case ARTIFACT_EVENT_MODE_RESOURCE_5:
                    if (gpPhilAI->NetValueOfArtifact(
                            IDX(artifact_g),
                            EVENT_ARTIFACT_RESOURCE_5_GOLD,
                            artifactResource_p,
                            EVENT_ARTIFACT_RESOURCE_5
                        )) {
                        gpGame->m_players[eventHero->m_owner].m_resources[IDX(RES_GOLD)] -=
                            EVENT_ARTIFACT_RESOURCE_5_GOLD;
                        gpGame->m_players[eventHero->m_owner].m_resources[artifactResource_p] -=
                            EVENT_ARTIFACT_RESOURCE_5;
                        goto artifactPickup;
                    }
                    break;
            }
            break;

        case MAP_OBJECT_HERO_INTERACTION:
            otherHero_e = gpGame->GetHero(cell->m_objectMetadata);
            savedShowIt_e = bShowIt;
            if (otherHero_e->m_owner == giCurPlayer) {
                gpPhilAI->HeroInteractionAtHero(eventHero, otherHero_e, 0, &heroInteractionResult);
                return;
            }
            if (otherHero_e->m_locationType
                == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_CASTLE))
                occupiedTown_b = gpGame->GetTown(otherHero_e->m_occupiedTown);

            if (gbHumanPlayer[otherHero_e->m_owner] == 0) {
                combatResult_d = gpPhilAI->QuickCombat(
                    &eventHero->m_army,
                    eventHero,
                    &otherHero_e->m_army,
                    otherHero_e,
                    0,
                    0,
                    attackerLoss_c,
                    defenderLoss_k
                );
                if (combatResult_d != 0 && occupiedTown_b != NULL) {
                    combatResult_d = gpPhilAI->QuickCombat(
                        &eventHero->m_army,
                        eventHero,
                        &occupiedTown_b->m_army,
                        NULL,
                        1,
                        occupiedTown_b->m_id,
                        attackerLoss_c,
                        defenderLoss_k
                    );
                }
            } else {
                if (occupiedTown_b != NULL)
                    occupiedTown_b->m_occupyingHeroId = otherHero_e->m_id;
                heroCombatResult_h = DoCombat(
                    x,
                    y,
                    eventHero,
                    &eventHero->m_army,
                    occupiedTown_b,
                    otherHero_e,
                    &otherHero_e->m_army,
                    x,
                    y,
                    -1,
                    1
                );
                if (heroCombatResult_h == COMBAT_RESULT_ATTACKER && occupiedTown_b != NULL)
                    gpGame->ClaimTown(occupiedTown_b->m_id, giCurPlayer, 0);
            }
            CompleteDraw(0);
            break;

        case MAP_OBJECT_SIGN:
            break;

        case MAP_OBJECT_BOTTLE:
            eventResults[AI_EVENT_RESULT_ERASE_OBJECT] = 1;
            break;

        case MAP_OBJECT_DAEMON_CAVE:
            switch (cell->m_objectMetadata) {
                case DAEMON_CAVE_EMPTY:
                    break;
                case DAEMON_REWARD_EXPERIENCE:
                    GiveExperience(eventHero, DAEMON_EXPERIENCE, 1);
                    break;
                case DAEMON_REWARD_ARTIFACT:
                    GiveExperience(eventHero, DAEMON_EXPERIENCE, 1);
                    GiveRandomArtifact(eventHero);
                    break;
                case DAEMON_REWARD_EXPERIENCE_GOLD:
                    GiveExperience(eventHero, DAEMON_EXPERIENCE, 1);
                    GiveResource(eventHero, RES_GOLD, EVENT_DAEMON_GOLD);
                    break;
                case DAEMON_REWARD_RANSOM:
                    if (gpGame->m_players[eventHero->m_owner].m_resources[IDX(RES_GOLD)]
                        >= EVENT_DAEMON_GOLD) {
                        if (gpPhilAI->ChooseToPayRansomOnHero(EVENT_DAEMON_GOLD)) {
                            gpGame->m_players[eventHero->m_owner].m_resources[IDX(RES_GOLD)] -=
                                EVENT_DAEMON_GOLD;
                        } else {
                            HeroLoses(eventHero);
                        }
                    } else {
                        HeroLoses(eventHero);
                    }
                    break;
            }
            cell->m_objectMetadata = DAEMON_CAVE_EMPTY;
            break;

        case MAP_OBJECT_PYRAMID:
            if (cell->m_objectMetadata == 0)
                break;
            if (eventHero->HasSpell(SpellType(cell->m_objectMetadata - 1)))
                break;
            for (index_h = 0; index_h < ARMY_GROUP_SLOT_COUNT; ++index_h) {
                gpMonGroup->m_creatureTypes[index_h] = CREATURE_ROYAL_MUMMY;
                gpMonGroup->m_creatureCounts[index_h] = PYRAMID_GUARD_STACK_QUANTITY;
            }
            index_h = cell->m_objectMetadata - 1;
            if (HAS(gsSpellInfo[index_h].attributes, SPELL_INFO_ATTRIBUTE_POWER)) {
                spellValueFactor_i =
                    eventHero->Stats(HERO_PRIMARY_SPELL_POWER) <= AI_BATTLE_STAT_MAX
                        ? gfBattleStat[eventHero->Stats(HERO_PRIMARY_SPELL_POWER)]
                        : gfBattleStat[AI_BATTLE_STAT_MAX];
            } else {
                spellValueFactor_i = 1.0f;
            }
            pyramidBattleValue_l = static_cast<i32>(
                gsSpellInfo[index_h].aiValue * spellValueFactor_i
                * gpCurPlayer->m_aiData.m_upgradeValueWeight
            );
            gpPhilAI->ChooseEvaluateBattle(
                &eventHero->m_army,
                eventHero,
                gpMonGroup,
                NULL,
                0,
                0,
                pyramidBattleValue_l,
                battleWon_j,
                battleResult_l
            );
            if (battleWon_j != 0) {
                index_h = PYRAMID_GUARD_COUNT;
                combatResult_d = gpPhilAI->CombatMonsterEvent(
                    eventHero,
                    CREATURE_ROYAL_MUMMY,
                    &index_h,
                    cell
                );
                if (combatResult_d != 0) {
                    eventHero->AddSpell(
                        static_cast<SpellType>(cell->m_objectMetadata - 1),
                        eventHero->Stats(HERO_PRIMARY_KNOWLEDGE)
                    );
                    cell->m_objectMetadata = MAP_EVENT_DATA_EMPTY;
                }
            }
            break;

        case MAP_OBJECT_GRAVEYARD:
        case MAP_OBJECT_SHIPWRECK:
        case MAP_OBJECT_DERELICT_SHIP:
            gpPhilAI->FightEvent(eventHero, cell, 0);
            break;

        case MAP_OBJECT_ABANDONED_MINE:
            for (index_h = 0; index_h < ARMY_GROUP_SLOT_COUNT; ++index_h) {
                gpMonGroup->m_creatureTypes[index_h] =
                    gpGame->m_mines[cell->m_objectMetadata].guardianType;
                gpMonGroup->m_creatureCounts[index_h] = static_cast<i16>(
                    gpGame->m_mines[cell->m_objectMetadata].guardianCount
                    / EVENT_ABANDONED_MINE_ARMY_DIVISOR
                );
            }
            abandonedMineValue_f = static_cast<i32>(
                gMineCharacteristics[IDX(RES_GOLD)] * gafAITurnCostResource[IDX(RES_GOLD)]
                * gaiTurnValueOfMine[x + y * MAP_WIDTH]
            );
            gpPhilAI->ChooseEvaluateBattle(
                &eventHero->m_army,
                eventHero,
                gpMonGroup,
                NULL,
                0,
                0,
                abandonedMineValue_f,
                battleWon_j,
                battleResult_l
            );
            if (battleWon_j != 0) {
                survivingCount_a = gpGame->m_mines[cell->m_objectMetadata].guardianCount;
                combatResult_d = gpPhilAI->CombatMonsterEvent(
                    eventHero,
                    gpGame->m_mines[cell->m_objectMetadata].guardianType,
                    &survivingCount_a,
                    cell
                );
                if (survivingCount_a > EVENT_GUARD_COUNT_MAX)
                    survivingCount_a = EVENT_GUARD_COUNT_MAX;
                gpGame->m_mines[cell->m_objectMetadata].guardianCount =
                    static_cast<u8>(survivingCount_a);
                if (combatResult_d != 0) {
                    eventHero->CheckLevel();
                    gpGame->ConvertObject(
                        x + ABANDONED_MINE_X_MIN,
                        y + ABANDONED_MINE_Y_TOP,
                        x + ABANDONED_MINE_X_MAX,
                        y + ABANDONED_MINE_Y_TOP,
                        ABANDONED_MINE_TILESET_TOP_A,
                        MINE_TOP_A_FIRST_FRAME,
                        MINE_TOP_A_LAST_FRAME,
                        ABANDONED_MINE_TILESET_BOTTOM_A,
                        MINE_BOTTOM_A_FRAME,
                        MAP_OBJECT_ABANDONED_MINE,
                        MAP_OBJECT_MINE
                    );
                    gpGame->ConvertObject(
                        x + ABANDONED_MINE_X_MIN,
                        y,
                        x + ABANDONED_MINE_X_MAX,
                        y,
                        ABANDONED_MINE_TILESET_TOP_A,
                        MINE_SECOND_A_FIRST_FRAME,
                        MINE_SECOND_A_LAST_FRAME,
                        ABANDONED_MINE_TILESET_BOTTOM_A,
                        MINE_SECOND_BOTTOM_A_FRAME,
                        MAP_OBJECT_ABANDONED_MINE,
                        MAP_OBJECT_MINE
                    );
                    gpGame->ConvertObject(
                        x + ABANDONED_MINE_X_MIN,
                        y + ABANDONED_MINE_Y_TOP,
                        x + ABANDONED_MINE_X_MAX,
                        y + ABANDONED_MINE_Y_TOP,
                        ABANDONED_MINE_TILESET_TOP_B,
                        MINE_TOP_B_FIRST_FRAME,
                        MINE_TOP_B_LAST_FRAME,
                        ABANDONED_MINE_TILESET_BOTTOM_B,
                        MINE_BOTTOM_B_FRAME,
                        MAP_OBJECT_ABANDONED_MINE,
                        MAP_OBJECT_MINE
                    );
                    gpGame->ConvertObject(
                        x + ABANDONED_MINE_X_MIN,
                        y,
                        x + ABANDONED_MINE_X_MAX,
                        y,
                        ABANDONED_MINE_TILESET_TOP_B,
                        MINE_SECOND_B_FIRST_FRAME,
                        MINE_SECOND_B_LAST_FRAME,
                        ABANDONED_MINE_TILESET_BOTTOM_B,
                        MINE_SECOND_BOTTOM_B_FRAME,
                        MAP_OBJECT_ABANDONED_MINE,
                        MAP_OBJECT_MINE
                    );
                    gpGame->ConvertObject(
                        x,
                        y,
                        x,
                        y,
                        ABANDONED_MINE_TILESET_CENTER,
                        MINE_CENTER_GHOST_FRAME,
                        MINE_CENTER_GHOST_FRAME,
                        ABANDONED_MINE_TILESET_CENTER,
                        MINE_CENTER_GOLD_FRAME,
                        MAP_OBJECT_ABANDONED_MINE,
                        MAP_OBJECT_MINE
                    );
                    gpGame->m_mines[cell->m_objectMetadata].resourceType = MINE_TYPE_GOLD;
                    gpGame->m_mines[cell->m_objectMetadata].guardianType = CREATURE_NONE;
                    gpGame->m_mines[cell->m_objectMetadata].guardianCount = 0;
                    gpGame->ClaimMine(cell->m_objectMetadata, giCurPlayer);
                }
            }
            break;

        case MAP_OBJECT_TAR_PIT:
            break;

        case MAP_OBJECT_OBSERVATION_TOWER:
            gpGame->SetVisibility(x, y, giCurPlayer, EVENT_OBSERVATION_RADIUS);
            break;

        case MAP_OBJECT_SHIPWRECK_SURVIVOR:
            if (eventHero->NumArtifacts() < HERO_ARTIFACT_SLOT_COUNT)
                GiveArtifact(eventHero, ArtifactType(cell->m_objectMetadata), 1, -1);
            eventResults[AI_EVENT_RESULT_ERASE_OBJECT] = 1;
            break;

        case MAP_OBJECT_ARTESIAN_SPRING:
            if (cell->m_objectMetadata != 0) {
                cell->m_objectMetadata = MAP_EVENT_DATA_EMPTY;
                spellPower_j =
                    eventHero->Stats(HERO_PRIMARY_KNOWLEDGE) * HERO_SPELL_POINTS_PER_KNOWLEDGE;
                if (eventHero->m_spellPoints < spellPower_j * ARTESIAN_SPRING_MANA_MULTIPLIER)
                    eventHero->m_spellPoints =
                        static_cast<i16>(spellPower_j * ARTESIAN_SPRING_MANA_MULTIPLIER);
            }
            break;

        case MAP_OBJECT_MAGIC_WELL:
            if (HAS(eventHero->m_eventFlags, HERO_EVENT_MAGIC_WELL) == 0) {
                cell->m_objectMetadata = MAP_EVENT_DATA_EMPTY;
                spellPower_j =
                    eventHero->Stats(HERO_PRIMARY_KNOWLEDGE) * HERO_SPELL_POINTS_PER_KNOWLEDGE;
                if (eventHero->m_spellPoints < spellPower_j) {
                    eventHero->m_eventFlags = HeroEventFlag(
                        static_cast<i32>(eventHero->m_eventFlags) | IDX(HERO_EVENT_MAGIC_WELL)
                    );
                    eventHero->m_spellPoints = static_cast<i16>(spellPower_j);
                }
            }
            break;

        case MAP_OBJECT_WITCH_HUT:
            if (eventHero->m_secondarySkills[cell->m_objectMetadata] == HERO_SKILL_LEVEL_NONE)
                eventHero->GiveSS(
                    static_cast<HeroSecondarySkill>(cell->m_objectMetadata),
                    HERO_SKILL_LEVEL_BASIC
                );
            break;

        case MAP_OBJECT_MAGELLAN_MAPS:
            break;

        case MAP_OBJECT_SPHINX:
            eventExtra_o = reinterpret_cast<mapEventExtra*>(ppMapExtra[cell->m_objectMetadata]);
            if (eventExtra_o->active != 0) {
                if (Random(0, EVENT_RANDOM_PERCENT_MAX) < EVENT_RANDOM_EVENT_SUCCESS) {
                    for (index_h = 0; index_h < IDX(RES_COUNT); ++index_h) {
                        gpGame->m_players[giCurPlayer].m_resources[index_h] +=
                            eventExtra_o->resources[index_h];
                        if (gpGame->m_players[giCurPlayer].m_resources[index_h] < 0)
                            gpGame->m_players[giCurPlayer].m_resources[index_h] = 0;
                    }
                    if (eventExtra_o->artifact != -1
                        && eventHero->NumArtifacts() < HERO_ARTIFACT_SLOT_COUNT) {
                        GiveArtifact(eventHero, ArtifactType(eventExtra_o->artifact), 1, -1);
                    }
                    eventExtra_o->active = 0;
                } else {
                    HeroLoses(eventHero);
                }
            }
            break;

        case MAP_OBJECT_TRADING_POST:
            break;

        case MAP_OBJECT_BARRIER:
            eventResults[AI_EVENT_RESULT_ERASE_OBJECT] = BarrierAIEvent(cell, eventHero);
            break;
        case MAP_OBJECT_TRAVELER_TENT:
            PasswordAIEvent(cell, eventHero);
            break;
        case MAP_OBJECT_EXPANSION_OBJECT:
            GenericSiteAIEvent(cell, eventHero);
            break;
        case MAP_OBJECT_EXPANSION_DWELLING:
            RecruitSiteAIEvent(cell, eventHero);
            break;
        case MAP_OBJECT_JAIL:
            JailAIEvent(cell, eventHero, x, y);
            break;

        default:
            break;
    }

    if (eventResults[AI_EVENT_RESULT_ERASE_OBJECT] != 0)
        EraseObj(cell, x, y);
    giCurPlayer = oldPlayer_o;
    gpCurPlayer = oldPlayerData_h;
    if (eventHero->m_owner != EVENT_NO_OWNER)
        eventHero->CheckLevel();
    CheckEndGame(END_GAME_FORCE_NONE, false);
}

VA(0x004478df, 0x56)
i32 advManager::BarrierAIEvent(mapCell* cell, hero*) {
    i32 color = cell->m_objectMetadata;
    color &= EVENT_BARRIER_COLOR_MASK;
    if (gpCurPlayer->m_barrierTents & (1 << color))
        return 1;
    else
        return 0;
    return 0;
}

VA(0x00447935, 0x54)
void advManager::PasswordAIEvent(mapCell* cell, hero*) {
    i32 unusedPassword6[1];
    i32 color = cell->m_objectMetadata;
    color &= EVENT_BARRIER_COLOR_MASK;
    gpCurPlayer->m_barrierTents |= (1 << color);
}

VA(0x00447989, 0x351)
void advManager::GenericSiteAIEvent(mapCell* cell, hero* eventHero) {
    i32 siteLevel6;
    HeroPrimaryStat primaryStat16;
    i32 artifactIndex14;
    GenericSiteType siteType3;
    i32 unusedPair8[2];
    i32 cursedArtifactCount3;
    i32 quantity1;
    i32 unusedTriple26[3];
    CreatureType creatureType3;
    i32 armyValue7;

    cursedArtifactCount3 = 0;
    siteType3 = static_cast<GenericSiteType>(cell->m_objectMetadata);
    siteType3 = static_cast<GenericSiteType>(IDX(siteType3) & GENERIC_SITE_TYPE_MASK);
    siteLevel6 = cell->m_objectMetadata;
    siteLevel6 >>= GENERIC_SITE_LEVEL_SHIFT;

    switch (siteType3) {
        case GENERIC_SITE_ALCHEMIST_TOWER:
            for (artifactIndex14 = 0; artifactIndex14 < HERO_ARTIFACT_SLOT_COUNT; artifactIndex14++) {
                if (IsCursedItem(eventHero->m_artifacts[artifactIndex14]))
                    cursedArtifactCount3++;
            }
            if (cursedArtifactCount3 != 0
                && gpCurPlayer->m_resources[IDX(RES_GOLD)]
                       >= EVENT_CURSED_ARTIFACT_GOLD_THRESHOLD) {
                for (artifactIndex14 = 0; artifactIndex14 < HERO_ARTIFACT_SLOT_COUNT;
                     artifactIndex14++) {
                    if (IsCursedItem(eventHero->m_artifacts[artifactIndex14]))
                        eventHero->m_artifacts[artifactIndex14] = ARTIFACT_NONE;
                }
                gpCurPlayer->m_resources[IDX(RES_GOLD)] -= EVENT_CURSED_ARTIFACT_COST;
            }
            break;
        case GENERIC_SITE_ARENA:
            if (!(eventHero->m_eventFlags & HERO_EVENT_ARENA)) {
                eventHero->m_eventFlags = HeroEventFlag(
                    static_cast<i32>(eventHero->m_eventFlags) | IDX(HERO_EVENT_ARENA)
                );
                switch (eventHero->m_cursorType) {
                    case FACTION_SORCERESS:
                    case FACTION_WARLOCK:
                    case FACTION_WIZARD:
                    case FACTION_NECROMANCER:
                        primaryStat16 = HERO_PRIMARY_SPELL_POWER;
                        break;
                    case FACTION_BARBARIAN:
                        primaryStat16 = HERO_PRIMARY_ATTACK;
                        break;
                    case FACTION_KNIGHT:
                        primaryStat16 = HERO_PRIMARY_DEFENSE;
                        break;
                    default:
                        primaryStat16 = HERO_PRIMARY_ATTACK;
                }
                eventHero->m_primaryStats[IDX(primaryStat16)]++;
            }
            break;
        case GENERIC_SITE_MERMAID:
            if (!(eventHero->m_eventFlags & HERO_EVENT_MERMAID)) {
                eventHero->m_eventFlags = HeroEventFlag(
                    static_cast<i32>(eventHero->m_eventFlags) | IDX(HERO_EVENT_MERMAID)
                );
                eventHero->m_luck++;
            }
            break;
        case GENERIC_SITE_HUT_OF_MAGI:
        case GENERIC_SITE_EYE_OF_MAGI:
            break;
        case GENERIC_SITE_SIRENS:
            if (!(eventHero->m_eventFlags & HERO_EVENT_SIRENS)) {
                armyValue7 = 0;
                for (artifactIndex14 = 0; artifactIndex14 < ARMY_GROUP_SLOT_COUNT; artifactIndex14++) {
                    creatureType3 = eventHero->m_army.m_creatureTypes[artifactIndex14];
                    if (creatureType3 != CREATURE_NONE) {
                        quantity1 = eventHero->m_army.m_quantities[artifactIndex14];
                        if (quantity1 > EVENT_SIRENS_MIN_ARMY_QUANTITY) {
                            eventHero->m_army.m_quantities[artifactIndex14] =
                                static_cast<i16>(quantity1 * AI_GENERIC_SITE_SIRENS_ARMY_REMAINDER);
                            armyValue7 +=
                                gMonsterDatabase[IDX(creatureType3)].hitPoints
                                * (quantity1 - eventHero->m_army.m_quantities[artifactIndex14]);
                        }
                    }
                }
                if (armyValue7 != 0)
                    GiveExperience(eventHero, armyValue7, 1);
                eventHero->m_eventFlags = HeroEventFlag(
                    static_cast<i32>(eventHero->m_eventFlags) | IDX(HERO_EVENT_SIRENS)
                );
            }
            break;
        case GENERIC_SITE_STABLES:
            if (!(eventHero->m_eventFlags & HERO_EVENT_STABLES)) {
                eventHero->m_eventFlags = HeroEventFlag(
                    static_cast<i32>(eventHero->m_eventFlags) | IDX(HERO_EVENT_STABLES)
                );
                eventHero->m_mobility += EVENT_CREATURE_UPGRADE_MOBILITY;
                eventHero->m_remainingMobility += EVENT_CREATURE_UPGRADE_MOBILITY;
            }
            if (eventHero->CreatureTypeCount(CREATURE_CAVALRY))
                eventHero->UpgradeCreatures(CREATURE_CAVALRY, CREATURE_CHAMPION);
            break;
    }
}

VA(0x00447cda, 0x189)
void advManager::RecruitSiteAIEvent(mapCell* cell, hero* eventHero) {
    i32 replacementSlot;
    CreatureType monsterType;
    i32 buyCount;
    u32 packedSite;
    H2_ENUM_STORAGE_STEPPED(ResourceType, i32) resource;
    H2_ENUM_STORAGE(RecruitSiteType, u32) recruitType;
    i32 cost[IDX(RES_COUNT)];
    i32 totalValue;
    i16 quantity;

    recruitType = cell->m_objectMetadata;
    recruitType &= static_cast<RecruitSiteType>(EVENT_RECRUIT_TYPE_MASK);
    quantity = static_cast<i16>(cell->m_objectMetadata);
    quantity >>= EVENT_RECRUIT_COUNT_SHIFT;

    switch (recruitType) {
        case RECRUITMENT_SITE_BARROW_MOUNDS:
            monsterType = CREATURE_GHOST;
            break;
        case RECRUITMENT_SITE_EARTH_ALTAR:
            monsterType = CREATURE_EARTH_ELEMENTAL;
            break;
        case RECRUITMENT_SITE_AIR_ALTAR:
            monsterType = CREATURE_AIR_ELEMENTAL;
            break;
        case RECRUITMENT_SITE_FIRE_ALTAR:
            monsterType = CREATURE_FIRE_ELEMENTAL;
            break;
        case RECRUITMENT_SITE_WATER_ALTAR:
            monsterType = CREATURE_WATER_ELEMENTAL;
            break;
    }

    if (quantity != 0) {
        gpPhilAI->EvaluateOneTimeCreaturePurchase(
            monsterType,
            quantity,
            0,
            buyCount,
            totalValue,
            replacementSlot
        );
        if (buyCount > 0) {
            gpGame->GiveArmy(
                &eventHero->m_army,
                monsterType,
                buyCount,
                replacementSlot
            );
            quantity -= buyCount;
            packedSite = (quantity << EVENT_RECRUIT_COUNT_SHIFT) | IDX(recruitType);
            cell->m_objectMetadata = packedSite;
            GetMonsterCost(monsterType, cost);
            for (resource = 0; resource < RES_COUNT; resource++) {
                gpCurPlayer->m_resources[IDX(resource)] -=
                    buyCount * cost[IDX(resource)];
            }
        }
    }
}

VA(0x00447e63, 0x19c)
void advManager::JailAIEvent(mapCell* cell, hero* eventHero, i32 x, i32 y) {
    i32 heroId9;
    hero* releasedHero13;

    heroId9 = cell->m_objectMetadata;
    if (gpGame->m_availableHeroes[heroId9] != EVENT_JAILED_HERO) {
        EraseObj(cell, x, y);
        return;
    }
    if (gpCurPlayer->m_heroCount >= EVENT_HERO_LIMIT)
        return;
    gpGame->m_heroRecs[heroId9].m_owner = eventHero->m_owner;
    gpGame->m_availableHeroes[heroId9] = eventHero->m_owner;
    releasedHero13 = &gpGame->m_heroRecs[heroId9];
    EraseObj(cell, x, y);
    gpCurPlayer->m_heroIds[gpCurPlayer->m_heroCount] = static_cast<i8>(heroId9);
    gpCurPlayer->m_heroCount++;
    releasedHero13->m_x = x;
    releasedHero13->m_y = y;
    releasedHero13->m_eventFlags = HERO_EVENT_NONE;
    releasedHero13->m_direction = MAP_DIRECTION_EAST;
    releasedHero13->m_remainingMobility = releasedHero13->CalcMobility();
    releasedHero13->m_mobility = releasedHero13->m_remainingMobility;
    releasedHero13->m_locationType = cell->m_triggerType;
    releasedHero13->m_occupiedTown = cell->m_objectMetadata;
    cell->m_triggerType = MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_HERO_INTERACTION;
    cell->m_objectMetadata = heroId9;
    SendMapChange(
        MAP_CHANGE_RECRUIT_HERO,
        static_cast<i8>(heroId9),
        x,
        y,
        MAP_CHANGE_CURRENT_PLAYER,
        0,
        0
    );
}

VA(0x00447fff, 0x731)
void advManager::PlayerMonsterInteract(
    mapCell* cell,
    mapCell* combatCell,
    hero* eventHero,
    i32* handled,
    i32 x,
    i32 y,
    i32 unused,
    i32 combatX,
    i32 combatY
) {
    CreatureType monsterType;
    CombatResult result;
    i32 creatureCount;
    float armyRatio;
    i32 forceJoin;
    i32 joiningCost;
    char monsterText[MONSTER_OFFER_BUFFER_SIZE];
    i32 numJoining;

    unused = 0;
    gpMouseManager->ShowColorPointer();
    monsterType = static_cast<CreatureType>(cell->m_objectIndex);
    forceJoin = cell->m_objectMetadata & MONSTER_JOIN_FORCED;
    creatureCount = cell->m_objectMetadata & MONSTER_COUNT_MASK;
    armyRatio = static_cast<double>(
                    gpPhilAI->FightValueOfStack(&eventHero->m_army, eventHero, 0, 0, 0, 0)
                )
                / static_cast<double>(
                    creatureCount * gMonsterDatabase[IDX(monsterType)].fightValue
                );

    if (gbInCampaign
        && ((gpGame->m_campaignAwards[IDX(CAMPAIGN_AWARD_DWARVEN_ALLIANCE)]
             && (monsterType == CREATURE_DWARF || monsterType == CREATURE_BATTLE_DWARF))
            || (gpGame->m_campaignAwards[IDX(CAMPAIGN_AWARD_OGRE_ALLIANCE)]
                && (monsterType == CREATURE_OGRE || monsterType == CREATURE_OGRE_LORD))
            || (gpGame->m_campaignAwards[IDX(CAMPAIGN_AWARD_DRAGON_ALLIANCE)]
                && (monsterType == CREATURE_GREEN_DRAGON || monsterType == CREATURE_RED_DRAGON
                    || monsterType == CREATURE_BLACK_DRAGON)))) {
        if (!eventHero->m_army.CanJoin(monsterType)) {
            if (monsterType == CREATURE_DWARF || monsterType == CREATURE_BATTLE_DWARF)
                NormalDialog(
                    "\xc3\xed\xee\xec\xfb \xef\xf0\xe8\xe2\xe5\xf2\xf1\xf2\xe2\xf3\xfe "
                    "\xe2\xe0\xf1: \"\xc4\xf0\xf3\xe3 \xd0\xee\xeb\xe0\xed\xe4\xe0 "
                    "- \xed\xe0\xf8 \xe4\xf0\xf3\xe3. \xc2\xfb \xec\xee\xe6\xe5\xf2\xe5 "
                    "\xef\xf0\xee\xe9\xf2\xe8.\"",
                    NORMAL_DIALOG_INFO,
                    -1,
                    -1,
                    -1,
                    0,
                    -1,
                    0,
                    -1,
                    0
                );
            else if (monsterType == CREATURE_OGRE || monsterType == CREATURE_OGRE_LORD)
                NormalDialog(
                    "\xce\xe3\xf0\xfb \xef\xf0\xe8\xe7\xed\xe0\xeb\xe8 \xe2\xe0\xf1: "
                    "\"\xd1\xee\xfe\xe7\xed\xe8\xea\xe8 \xc0\xf0\xf7\xe8\xe1\xe0\xeb\xfc\xe4\xe0 "
                    "\xec\xee\xe3\xf3\xf2 \xef\xf0\xee\xe9\xf2\xe8.\"",
                    NORMAL_DIALOG_INFO,
                    -1,
                    -1,
                    -1,
                    0,
                    -1,
                    0,
                    -1,
                    0
                );
            else
                NormalDialog(
                    "\xc4\xf0\xe0\xea\xee\xed\xfb, \xef\xee\xf1\xec\xee\xf2\xf0\xe5\xe2 "
                    "\xed\xe0 \xe2\xe0\xf1, \xef\xf0\xee\xec\xee\xeb\xe2\xe8\xeb\xe8: "
                    "\"\xcd\xe0\xf8 \xe0\xeb\xfc\xff\xed\xf1 \xf1 \xc0\xf0\xf7\xe8\xe1\xe0\xeb\xfc\xe4\xee\xec "
                    "\xe2\xfb\xed\xf3\xe6\xe4\xe0\xe5\xf2 \xed\xe0\xf1 \xef\xf0\xe8\xf1\xee\xe5\xe4\xe8\xed\xe8\xf2\xfc\xf1\xff "
                    "\xea \xe2\xe0\xec.  \xca \xed\xe5\xf1\xf7\xe0\xf1\xf2\xfc\xfe, "
                    "\xf3 \xe2\xe0\xf1 \xed\xe5\xf2 \xe4\xeb\xff \xed\xe0\xf1 \xec\xe5\xf1\xf2\xe0. "
                    "\xca\xe0\xea\xe0\xff \xe6\xe0\xeb\xee\xf1\xf2\xfc!\"  \xc8 "
                    "\xe1\xfb\xf1\xf2\xf0\xee \xf3\xeb\xe5\xf2\xe5\xeb\xe8.",
                    NORMAL_DIALOG_INFO,
                    -1,
                    -1,
                    -1,
                    0,
                    -1,
                    0,
                    -1,
                    0
                );
            *handled = 1;
        } else {
            if (monsterType == CREATURE_DWARF || monsterType == CREATURE_BATTLE_DWARF)
                NormalDialog(
                    "\xc3\xed\xee\xec\xfb \xef\xf0\xe8\xe7\xed\xe0\xeb\xe8 \xf1\xe2\xee\xe8\xf5 "
                    "\xf1\xee\xfe\xe7\xed\xe8\xea\xee\xe2 \xe8 \xf0\xe0\xe4\xfb "
                    "\xef\xf0\xe8\xf1\xee\xe5\xe4\xe8\xed\xe8\xf2\xfc\xf1\xff \xea "
                    "\xed\xe8\xec.",
                    NORMAL_DIALOG_INFO,
                    -1,
                    -1,
                    -1,
                    0,
                    -1,
                    0,
                    -1,
                    0
                );
            else if (monsterType == CREATURE_OGRE || monsterType == CREATURE_OGRE_LORD)
                NormalDialog(
                    "\xce\xe3\xf0\xfb \xef\xf0\xe8\xe7\xed\xe0\xeb\xe8 \xed\xe0\xf1, "
                    "\xea\xe0\xea \xe2\xf0\xe0\xe3\xee\xe2 \xe3\xed\xee\xec\xee\xe2 "
                    "\xe8 \xf0\xe0\xe4\xfb \xef\xf0\xe8\xf1\xee\xe5\xe4\xe8\xed\xe8\xf2\xfc\xf1\xff "
                    "\xea \xe2\xe0\xec. ",
                    NORMAL_DIALOG_INFO,
                    -1,
                    -1,
                    -1,
                    0,
                    -1,
                    0,
                    -1,
                    0
                );
            else
                NormalDialog(
                    "\xc3\xee\xf0\xe4\xe5\xeb\xe8\xe2\xfb\xe5 \xe4\xf0\xe0\xea\xee\xed\xfb "
                    "\xe2\xfb\xed\xf3\xe6\xe4\xe5\xed\xfb \xe1\xfb\xeb\xe8 \xef\xf0\xe8\xe7\xed\xe0\xf2\xfc "
                    "\xe2 \xe2\xe0\xf1 \xf1\xe2\xee\xe8\xf5 '\xf1\xee\xfe\xe7\xed\xe8\xea\xee\xe2' "
                    "\xe8 \xef\xf0\xe8\xf1\xee\xe5\xe4\xe8\xed\xe8\xf2\xfc\xf1\xff "
                    "\xea \xe2\xe0\xec.",
                    NORMAL_DIALOG_INFO,
                    -1,
                    -1,
                    -1,
                    0,
                    -1,
                    0,
                    -1,
                    0
                );
            eventHero->m_army.Add(monsterType, creatureCount, -1);
            *handled = 1;
        }
        return;
    }

    if (gbInCampaign && gpGame->m_campaignAwards[IDX(CAMPAIGN_AWARD_DWARFBANE)]
        && (monsterType == CREATURE_DWARF || monsterType == CREATURE_BATTLE_DWARF)) {
        NormalDialog(
            "\"\xc2\xf0\xe0\xe3\xe8 \xe3\xed\xee\xec\xee\xe2! \xc1\xe5\xe3\xe8\xf2\xe5, "
            "\xe5\xf1\xeb\xe8 \xe6\xe8\xe7\xed\xfc \xe4\xee\xf0\xee\xe3\xe0.\"",
            NORMAL_DIALOG_INFO,
            -1,
            -1,
            -1,
            0,
            -1,
            0,
            -1,
            0
        );
        *handled = 1;
        return;
    }

    if (xIsPlayingExpansionCampaign && xCampaign.HasAward(AWARD_ELVEN_ALLIANCE)
        && (monsterType == CREATURE_ELF || monsterType == CREATURE_GRAND_ELF)) {
        *handled = 1;
        if (eventHero->m_army.CanJoin(monsterType)) {
            NormalDialog(
                "\xc5\xe4\xe2\xe0 \xe2\xfb \xef\xee\xe4\xee\xf8\xeb\xe8 \xea "
                "\xee\xf2\xf0\xff\xe4\xf3 \xfd\xeb\xfc\xf4\xee\xe2, \xea\xe0\xea "
                "\xe8\xf5 \xe2\xee\xe6\xe0\xea \xef\xf0\xe8\xe7\xe2\xe0\xeb "
                "\xf1\xe2\xee\xe9 \xee\xf2\xf0\xff\xe4 \xea \xee\xf0\xf3\xe6\xe8\xfe. "
                "\xce\xed \xf1\xea\xe0\xe7\xe0\xeb \xe8\xec: \"\xca\xf2\xee "
                "\xe8\xe7 \xe2\xe0\xf1 \xe4\xee\xf1\xf2\xe0\xf2\xee\xf7\xed\xee "
                "\xf1\xec\xe5\xeb, \xf7\xf2\xee\xe1\xfb \xe2\xf1\xf2\xf3\xef\xe8\xf2\xfc "
                "\xe2 \xe0\xf0\xec\xe8\xfe \xed\xe0\xf8\xe5\xe3\xee \xec\xf3\xe6\xe5\xf1\xf2\xe2\xe5\xed\xed\xee\xe3\xee "
                "\xf1\xee\xfe\xe7\xed\xe8\xea\xe0?\" \xc2\xe5\xf1\xfc \xee\xf2\xf0\xff\xe4 "
                "\xe5\xe4\xe8\xed\xee\xe4\xf3\xf8\xed\xee \xe2\xf1\xf2\xf3\xef\xe8\xeb "
                "\xe2 \xe2\xe0\xf8\xf3 \xe0\xf0\xec\xe8\xfe.",
                NORMAL_DIALOG_INFO,
                -1,
                -1,
                -1,
                0,
                -1,
                0,
                -1,
                0
            );
            eventHero->m_army.Add(monsterType, creatureCount, -1);
        } else {
            NormalDialog(
                "\xdd\xeb\xfc\xf4\xfb \xed\xe0\xf1\xf2\xee\xf0\xee\xe6\xe8\xeb\xe8\xf1\xfc "
                "\xef\xf0\xe8 \xe2\xe0\xf8\xe5\xec \xef\xf0\xe8\xe1\xeb\xe8\xe6\xe5\xed\xe8\xe8. "
                "\xc8\xf5 \xe2\xee\xe6\xe0\xea \xee\xe1\xf0\xe0\xf2\xe8\xeb\xf1\xff "
                "\xea \xe2\xe0\xec \xf1\xee \xf1\xeb\xee\xe2\xe0\xec\xe8: \"\xcc\xfb "
                "\xed\xe5 \xec\xee\xe6\xe5\xec \xef\xf0\xe8\xed\xff\xf2\xfc "
                "\xf3\xf7\xe0\xf1\xf2\xe8\xe5 \xe2 \xe2\xe0\xf8\xe8\xf5 \xe4\xe5\xeb\xe0\xf5, "
                "\xf1\xee\xfe\xe7\xed\xe8\xea! \xcf\xf0\xee\xf5\xee\xe4\xe8, "
                "\xed\xe0\xf8\xe0 \xef\xee\xe1\xe5\xe4\xe0 \xe1\xf3\xe4\xe5\xf2 "
                "\xe2\xe0\xf8\xe5\xe9.\"",
                NORMAL_DIALOG_INFO,
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
        return;
    }

    if (eventHero->m_army.CanJoin(monsterType)
        && armyRatio
            > MONSTER_STRENGTH_JOIN
        && !eventHero->HasArtifact(ARTIFACT_HIDEOUS_MASK) && monsterType != CREATURE_GHOST
        && monsterType != CREATURE_EARTH_ELEMENTAL && monsterType != CREATURE_AIR_ELEMENTAL
        && monsterType != CREATURE_FIRE_ELEMENTAL && monsterType != CREATURE_WATER_ELEMENTAL) {
        if (forceJoin) {
            sprintf(gText, gEventText[EVENT_TEXT_FOLLOWERS], gArmyNamesPlural[IDX(monsterType)]);
            EventWindow(-1, NORMAL_DIALOG_CONFIRM, gText, -1, 0, -1, 0, -1);
            if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
                eventHero->m_army.Add(monsterType, creatureCount, -1);
                *handled = 1;
                return;
            } else {
                EventWindow(EVENT_TEXT_MONSTER_REFUSAL, NORMAL_DIALOG_INFO, "", -1, 0, -1, 0, -1);
                goto fightMonsters;
            }
        } else if (eventHero->m_secondarySkills[IDX(HERO_SKILL_DIPLOMACY)]
                   != HERO_SKILL_LEVEL_NONE) {
            if (eventHero->m_secondarySkills[IDX(HERO_SKILL_DIPLOMACY)]
                == HERO_SKILL_LEVEL_EXPERT)
                numJoining = creatureCount;
            else if (eventHero->m_secondarySkills[IDX(HERO_SKILL_DIPLOMACY)]
                     == HERO_SKILL_LEVEL_ADVANCED)
                numJoining = creatureCount / MONSTER_DIPLOMACY_ADVANCED_JOIN_DIVISOR;
            else
                numJoining = creatureCount / MONSTER_DIPLOMACY_BASIC_JOIN_DIVISOR;
            if (!numJoining)
                numJoining = 1;

            joiningCost = gMonsterDatabase[IDX(monsterType)].cost * creatureCount;
            if (joiningCost > gpGame->m_players[eventHero->m_owner].m_resources[IDX(RES_GOLD)]) {
                if (armyRatio
                    > MONSTER_STRENGTH_FLEE)
                    goto monstersFlee;
                else
                    goto fightMonsters;
            }

            if (creatureCount == 1) {
                sprintf(
                    gText,
                    "%s \xed\xe0\xf5\xee\xe4\xe8\xf2\xf1\xff \xef\xee\xe4 \xe2\xef\xe5\xf7\xe0\xf2\xeb\xe5\xed\xe8\xe5\xec "
                    "\xe2\xe0\xf8\xe5\xe9 \xe4\xe8\xef\xeb\xee\xec\xe0\xf2\xe8\xf7\xed\xee\xf1\xf2\xe8, "
                    "\xe8 \xef\xf0\xe5\xe4\xeb\xe0\xe3\xe0\xe5\xf2 \xf1\xe2\xee\xe8 "
                    "\xf3\xf1\xeb\xf3\xe3\xe8 \xe2\xe0\xf8\xe5\xec\xf3 \xe2\xee\xe9\xf1\xea\xf3 "
                    "\xe7\xe0 %d \xe7\xee\xeb\xee\xf2\xfb\xf5. \xc2\xfb \xf1\xee\xe3\xeb\xe0\xf1\xed\xfb?",
                    gArmyNames[IDX(monsterType)],
                    joiningCost
                );
            } else {
                sprintf(
                    gText,
                    "\xc2\xee\xe8\xed\xfb \xef\xee\xea\xee\xf0\xe5\xed\xfb \xe2\xe0\xf8\xe8\xec "
                    "\xf8\xe0\xf0\xec\xee\xec \xe8 \xe2\xfb\xe4\xe2\xe8\xed\xf3\xeb\xe8 "
                    "\xef\xf0\xe5\xe4\xeb\xee\xe6\xe5\xed\xe8\xe5:\n\n"
                );
                if (numJoining == creatureCount)
                    sprintf(
                        monsterText,
                        "\xce\xf2\xf0\xff\xe4 \xe8\xe7 %d %s \xe2\xf1\xf2\xf3\xef\xff\xf2 "
                        "\xe2 \xf0\xff\xe4\xfb \xe2\xe0\xf8\xe5\xe9 \xe0\xf0\xec\xe8\xe8 "
                        "\xe7\xe0 %d \xe7\xee\xeb\xee\xf2\xfb\xf5. \xc2\xfb \xf1\xee\xe3\xeb\xe0\xf1\xed\xfb "
                        "\xef\xf0\xe8\xed\xff\xf2\xfc \xe8\xf5?",
                        creatureCount,
                        gArmyNamesPlural[IDX(monsterType)],
                        joiningCost
                    );
                else
                    sprintf(
                        monsterText,
                        "%d \xe8\xe7 %d %s \xe2\xf1\xf2\xf3\xef\xff\xf2 \xe2 \xf0\xff\xe4\xfb "
                        "\xe2\xe0\xf8\xe5\xe9 \xe0\xf0\xec\xe8\xe8, \xe0 \xee\xf1\xf2\xe0\xe2\xf8\xe8\xe5\xf1\xff "
                        "\xee\xf1\xf2\xe0\xe2\xff\xf2 \xe2\xe0\xf1 \xe2 \xef\xee\xea\xee\xe5 "
                        "\xe7\xe0 %d \xe7\xee\xeb\xee\xf2\xfb\xf5. \xc2\xfb \xf1\xee\xe3\xeb\xe0\xf1\xed\xfb?",
                        numJoining,
                        creatureCount,
                        gArmyNamesPlural[IDX(monsterType)],
                        joiningCost
                    );
                strcat(gText, monsterText);
            }

            NormalDialog(
                gText,
                NORMAL_DIALOG_CONFIRM,
                -1,
                -1,
                IDX(RES_GOLD),
                joiningCost,
                -1,
                0,
                -1,
                0
            );
            if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
                eventHero->m_army.Add(monsterType, numJoining, -1);
                *handled = 1;
                gpGame->m_players[eventHero->m_owner].m_resources[IDX(RES_GOLD)] -= joiningCost;
                return;
            } else {
                EventWindow(EVENT_TEXT_MONSTER_REFUSAL, NORMAL_DIALOG_INFO, "", -1, 0, -1, 0, -1);
                goto fightMonsters;
            }
        }
    }

    if (armyRatio > MONSTER_STRENGTH_FLEE) {
    monstersFlee:
        sprintf(
            gText,
            "\xc3\xf0\xf3\xef\xef\xe0 %s, \xf1\xf2\xf0\xe0\xf8\xe0\xf1\xfc "
            "\xec\xee\xf9\xe8 \xe2\xe0\xf8\xe5\xe3\xee \xe2\xee\xe8\xed\xf1\xf2\xe2\xe0, "
            "\xe1\xf0\xee\xf1\xe8\xeb\xe0\xf1\xfc \xe2\xf0\xe0\xf1\xf1\xfb\xef\xed\xf3\xfe. "
            "\xd1\xee\xe8\xe7\xe2\xee\xeb\xe8\xf2\xe5 \xe8\xe7\xeb\xee\xe2\xe8\xf2\xfc "
            "\xe8\xf5 \xe8 \xe7\xe0\xf1\xf2\xe0\xe2\xe8\xf2\xfc \xe4\xf0\xe0\xf2\xfc\xf1\xff?",
            gArmyNamesPlural[IDX(monsterType)]
        );
        EventWindow(-1, NORMAL_DIALOG_CONFIRM, gText, -1, 0, -1, 0, -1);
        if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES)
            goto fightMonsters;
        *handled = 1;
        return;
    }

fightMonsters:
    result = CombatMonsterEvent(
        eventHero,
        monsterType,
        creatureCount,
        combatCell,
        x,
        y,
        unused,
        combatX,
        combatY,
        CREATURE_NONE,
        0,
        0,
        CREATURE_NONE,
        0,
        0
    );
    if (result == COMBAT_RESULT_ATTACKER || result == COMBAT_RESULT_DRAW)
        *handled = 1;
}

VA(0x00448730, 0x3d4)
void advManager::ComputerMonsterInteract(mapCell* cell, hero* eventHero, i32* handled) {
    CreatureType monsterType;
    i32 replacementSlot;
    i32 creatureCount[MONSTER_COMBAT_VALUE_COUNT];
    i32 bought;
    float armyRatio;
    u32 forceJoin;
    i32 purchaseWorth;
    i32 joiningCost;
    i32 joiningCount;

    monsterType = static_cast<CreatureType>(cell->m_objectIndex);
    creatureCount[MONSTER_COMBAT_REMAINING_COUNT] = cell->m_objectMetadata & MONSTER_COUNT_MASK;
    forceJoin = cell->m_objectMetadata & MONSTER_JOIN_FORCED;
    armyRatio = static_cast<double>(
                    gpPhilAI->FightValueOfStack(&eventHero->m_army, eventHero, 0, 0, 0, 0)
                )
                / static_cast<double>(
                    creatureCount[MONSTER_COMBAT_REMAINING_COUNT]
                    * gMonsterDatabase[IDX(monsterType)].fightValue
                );

    if (eventHero->m_army.CanJoin(monsterType)
        && !eventHero->HasArtifact(ARTIFACT_HIDEOUS_MASK) && armyRatio > MONSTER_STRENGTH_JOIN
        && monsterType != CREATURE_GHOST && monsterType != CREATURE_EARTH_ELEMENTAL
        && monsterType != CREATURE_AIR_ELEMENTAL && monsterType != CREATURE_FIRE_ELEMENTAL
        && monsterType != CREATURE_WATER_ELEMENTAL) {
        if (forceJoin) {
            gpPhilAI->EvaluateOneTimeCreaturePurchase(
                monsterType,
                creatureCount[MONSTER_COMBAT_REMAINING_COUNT],
                1,
                bought,
                purchaseWorth,
                replacementSlot
            );
            if (bought > 0) {
                gpGame->GiveArmy(
                    &eventHero->m_army,
                    monsterType,
                    creatureCount[MONSTER_COMBAT_REMAINING_COUNT],
                    replacementSlot
                );
                *handled = 1;
            } else {
                *handled = 1;
            }
        } else {
            if (eventHero->m_secondarySkills[IDX(HERO_SKILL_DIPLOMACY)]
                != HERO_SKILL_LEVEL_NONE) {
                if (eventHero->m_secondarySkills[IDX(HERO_SKILL_DIPLOMACY)]
                    == HERO_SKILL_LEVEL_EXPERT)
                    joiningCount = creatureCount[MONSTER_COMBAT_REMAINING_COUNT];
                else if (eventHero->m_secondarySkills[IDX(HERO_SKILL_DIPLOMACY)]
                         == HERO_SKILL_LEVEL_ADVANCED)
                    joiningCount = creatureCount[MONSTER_COMBAT_REMAINING_COUNT]
                                   / MONSTER_DIPLOMACY_ADVANCED_JOIN_DIVISOR;
                else
                    joiningCount = creatureCount[MONSTER_COMBAT_REMAINING_COUNT]
                                   / MONSTER_DIPLOMACY_BASIC_JOIN_DIVISOR;
                if (!joiningCount)
                    joiningCount = 1;

                joiningCost = static_cast<i32>(
                    gMonsterDatabase[IDX(monsterType)].cost
                    * creatureCount[MONSTER_COMBAT_REMAINING_COUNT]
                    * MONSTER_AI_JOIN_COST_FRACTION
                );
                if (joiningCost
                    > gpGame->m_players[eventHero->m_owner].m_resources[IDX(RES_GOLD)]) {
                    if (armyRatio > MONSTER_STRENGTH_FLEE)
                        goto computerMonstersFlee;
                    else
                        goto fightComputerMonsters;
                }
                gpPhilAI->EvaluateOneTimeCreaturePurchase(
                    monsterType,
                    creatureCount[MONSTER_COMBAT_REMAINING_COUNT],
                    1,
                    bought,
                    purchaseWorth,
                    replacementSlot
                );
                if (bought > 0) {
                    gpGame->m_players[eventHero->m_owner].m_resources[IDX(RES_GOLD)] -= joiningCost;
                    gpGame->GiveArmy(
                        &eventHero->m_army,
                        monsterType,
                        joiningCount,
                        replacementSlot
                    );
                    *handled = 1;
                } else
                    goto fightComputerMonsters;
            }
        }
    }

    if (armyRatio > MONSTER_STRENGTH_FLEE) {
    computerMonstersFlee:
        gpAdvManager->GiveExperience(
            eventHero,
            creatureCount[MONSTER_COMBAT_REMAINING_COUNT]
                * gMonsterDatabase[IDX(monsterType)].hitPoints,
            1
        );
        eventHero->CheckLevel();
        if (eventHero->GetSSLevel(HERO_SKILL_NECROMANCY)
            && eventHero->m_army.CanJoin(CREATURE_SKELETON)) {
            gpGame->GiveArmy(
                &eventHero->m_army,
                CREATURE_SKELETON,
                static_cast<i32>(
                    static_cast<double>(creatureCount[MONSTER_COMBAT_REMAINING_COUNT])
                    * MONSTER_NECROMANCY_FRACTION
                    * eventHero->GetSSLevel(HERO_SKILL_NECROMANCY)
                ),
                -1
            );
        }
        *handled = 1;
        return;
    } else {
    fightComputerMonsters:
        creatureCount[MONSTER_COMBAT_RESULT] =
            gpPhilAI->CombatMonsterEvent(eventHero, monsterType, creatureCount, cell);
        if (creatureCount[MONSTER_COMBAT_RESULT] != 0) {
            *handled = 1;
        } else {
            cell->m_objectMetadata =
                (cell->m_objectMetadata & MONSTER_FLAGS_MASK)
                + (creatureCount[MONSTER_COMBAT_REMAINING_COUNT] & MONSTER_COUNT_MASK);
        }
    }
}

VA(0x00448b04, 0x191)
i32 advManager::DoNetCombat(char* packet) {
    hero* secondHro;
    i32 randSeed;
    i32 firstSide;
    town* battleTown;
    H2_ENUM_STORAGE(CombatResult, i8) combatRes;
    i32 initCombatX;
    i32 netUnused7;
    armyGroup* secondArmy;
    i32 battleX;
    armyGroup* troopFirst;
    i32 otherPlr;
    i32 setupBattleY;
    hero* firstHero;
    i32 combatY;
    i32 outcome;

    firstHero = NULL;
    troopFirst = NULL;
    battleTown = NULL;
    secondHro = NULL;
    secondArmy = NULL;
    ReceiveHeroTownData(
        packet,
        &otherPlr,
        &battleX,
        &combatY,
        &firstHero,
        &troopFirst,
        &battleTown,
        &secondHro,
        &secondArmy,
        &initCombatX,
        &setupBattleY,
        &randSeed,
        &combatRes,
        &gbRetreatWin,
        &gbCombatSurrender
    );
    firstSide = firstHero->m_owner;
    combatRes = DoCombat(
        battleX,
        combatY,
        firstHero,
        troopFirst,
        battleTown,
        secondHro,
        secondArmy,
        initCombatX,
        setupBattleY,
        randSeed,
        0
    );
    if (!gbHumanPlayer[firstSide]) {
        SendHeroTownData(
            battleX,
            combatY,
            firstHero,
            troopFirst,
            battleTown,
            secondHro,
            secondArmy,
            initCombatX,
            setupBattleY,
            randSeed,
            otherPlr,
            combatRes,
            gbRetreatWin,
            gbCombatSurrender
        );
    }
    if (troopFirst)
        H2_FREE(troopFirst);
    if (secondArmy)
        H2_FREE(secondArmy);
    if (battleTown)
        H2_FREE(battleTown);
    if (secondHro)
        H2_FREE(secondHro);
    if (firstHero)
        H2_FREE(firstHero);
    gbRetreatWin = false;
    return 1;
}

VA(0x00448c95, 0x5e7)
CombatResult advManager::DoCombat(
    i32 x,
    i32 y,
    hero* firstHero,
    armyGroup* firstArmy,
    town* combatTown,
    hero* secondHero,
    armyGroup* secondArmy,
    i32 setupCombatX,
    i32 setupCombatY,
    i32 randomSeed,
    i32 processLosses
) {
    armyGroup* receivedSecondArmy2;
    armyGroup* receivedFirstArmy;
    hero* receivedSecondHero9;
    hero* receivedFirstHero1;
    town* receivedTown;
    char* packet_j;
    i32 remotePlayer;
    tag_message message9;
    H2_ENUM_STORAGE(CombatResult, i8) combatResult3;
    i32 savedShowIt_f;
    i32 secondPlayer8;
    i32 savedPlayer1;
    i32 unusedCombat_a;
    i32 firstPlayer4;

    if (giDebugLevel == COMBAT_AUTO_RESOLVE_DEBUG_LEVEL)
        return AutoResolveCombat(
            x,
            y,
            firstHero,
            firstArmy,
            combatTown,
            secondHero,
            secondArmy,
            setupCombatX,
            setupCombatY,
            randomSeed,
            processLosses
        );
    gbInCombat = true;
    firstPlayer4 = firstHero ? firstHero->m_owner : -1;
    if (secondHero)
        secondPlayer8 = secondHero->m_owner;
    else if (combatTown)
        secondPlayer8 = combatTown->m_owner;
    else
        secondPlayer8 = -1;
    if (randomSeed == -1)
        randomSeed = Random(1, COMBAT_RANDOM_SEED_MAX);
    DemobilizeCurrHero();
    savedPlayer1 = giCurPlayer;
    savedShowIt_f = bShowIt;

    if (firstPlayer4 >= 0 && secondPlayer8 >= 0 && gbHumanPlayer[secondPlayer8]) {
        if (!gbThisNetHumanPlayer[secondPlayer8]) {
            iCombatControlNetPos[IDX(COMBAT_ATTACKER_SIDE)] = giThisNetPos;
            iCombatControlNetPos[IDX(COMBAT_DEFENDER_SIDE)] = gbGamePosToNetPos[secondPlayer8];
            SendHeroTownData(
                x,
                y,
                firstHero,
                firstArmy,
                combatTown,
                secondHero,
                secondArmy,
                setupCombatX,
                setupCombatY,
                randomSeed,
                gbGamePosToNetPos[secondPlayer8],
                COMBAT_RESULT_ATTACKER,
                0,
                0
            );
            if (!gbHumanPlayer[firstPlayer4]) {
                while (1) {
                    PollSound();
                    FillBitmapArea(
                        gpWindowManager->m_screen,
                        COMBAT_NETWORK_POLL_X,
                        COMBAT_NETWORK_POLL_Y,
                        COMBAT_NETWORK_POLL_WIDTH,
                        COMBAT_NETWORK_POLL_HEIGHT,
                        0
                    );
                    packet_j = CheckHandleNet();
                    if (packet_j) {
                        switch (EVENTS_REMOTE_MESSAGE(packet_j)->command) {
                            case REMOTE_COMMAND:
                                ReceiveHeroTownData(
                                    packet_j,
                                    &remotePlayer,
                                    &x,
                                    &y,
                                    &receivedFirstHero1,
                                    &receivedFirstArmy,
                                    &receivedTown,
                                    &receivedSecondHero9,
                                    &receivedSecondArmy2,
                                    &setupCombatX,
                                    &setupCombatY,
                                    &randomSeed,
                                    &combatResult3,
                                    &gbRetreatWin,
                                    &gbCombatSurrender
                                );
                                if (receivedFirstArmy) {
                                    memcpy(firstArmy, receivedFirstArmy, sizeof(armyGroup));
                                    H2_FREE(receivedFirstArmy);
                                }
                                if (receivedSecondArmy2) {
                                    memcpy(secondArmy, receivedSecondArmy2, sizeof(armyGroup));
                                    H2_FREE(receivedSecondArmy2);
                                }
                                if (receivedTown) {
                                    memcpy(combatTown, receivedTown, sizeof(town));
                                    H2_FREE(receivedTown);
                                }
                                if (receivedSecondHero9) {
                                    memcpy(secondHero, receivedSecondHero9, sizeof(hero));
                                    H2_FREE(receivedSecondHero9);
                                }
                                if (receivedFirstHero1) {
                                    memcpy(firstHero, receivedFirstHero1, sizeof(hero));
                                    H2_FREE(receivedFirstHero1);
                                }
                                gpCombatManager->m_combatResult = combatResult3;
                                goto combatFinished;
                        }
                    }
                    Process1WindowsMessage();
                    message9 = gpInputManager->GetEvent();
                    CheckHandleNetPlayerWait(message9, 1);
                }
            }
        } else if (!gbThisNetHumanPlayer[firstPlayer4]) {
            bShowIt = 1;
            gpGame->TurnOffAIMusic();
            sprintf(
                gText,
                "\xe3\xee\xf0\xee\xe4",
                cPlayerNames[secondPlayer8],
                combatTown ? "\xe3\xe5\xf0\xee\xe9" : "%s, \xe2\xe0\xf8 %s \xe0\xf2\xe0\xea\xee\xe2\xe0\xed!"
            );
            gpGame->WaitForPlayer(gText, secondPlayer8);
        }
    }

    bShowIt = 1;
    gpCombatManager->SetupCombat(
        x,
        y,
        firstHero,
        firstArmy,
        combatTown,
        secondHero,
        secondArmy,
        x,
        y,
        randomSeed
    );
    if (giHighMemBuffer > COMBAT_HIGH_MEMORY_LIMIT) {
        gAdvDisposeLevel = ADV_DISPOSE_FULL;
    } else if (giHighMemBuffer > COMBAT_LOW_MEMORY_LIMIT)
        gAdvDisposeLevel = ADV_DISPOSE_PARTIAL;
    gpExec->CallManager(gpCombatManager);
    gpMouseManager->SetPointer(const_cast<char*>("advmice.mse"), 0, MOUSE_AUTO_CURSOR_TYPE);
    gAdvDisposeLevel = ADV_DISPOSE_NONE;

combatFinished:
    if (firstHero)
        firstHero->CheckLevel();
    if (secondHero)
        secondHero->CheckLevel();
    if (processLosses) {
        switch (gpCombatManager->m_combatResult) {
            case COMBAT_RESULT_ATTACKER:
                if (!gbRetreatWin)
                    TransferArtifacts(secondHero, firstHero);
                HeroLoses(secondHero);
                break;
            case COMBAT_RESULT_DEFENDER:
                if (!gbRetreatWin)
                    TransferArtifacts(firstHero, secondHero);
                HeroLoses(firstHero);
                break;
            case COMBAT_RESULT_DRAW:
                HeroLoses(firstHero);
                HeroLoses(secondHero);
                break;
            case COMBAT_RESULT_PENDING:
                break;
        }
    }
    bShowIt = savedShowIt_f;
    giCurPlayer = savedPlayer1;
    if (!gbHumanPlayer[giCurPlayer]) {
        gpGame->ShowComputerScreen();
        gpGame->TurnOnAIMusic();
        SetNoDialogMenus(0);
    } else {
        SetNoDialogMenus(1);
    }
    MobilizeCurrHero(0);
    if (processLosses)
        gbRetreatWin = false;
    gbInCombat = false;
    while (gpMouseManager->m_hideCount)
        gpMouseManager->ShowColorPointer();
    return gpCombatManager->m_combatResult;
}

VA(0x0044927c, 0x306)
void advManager::SendHeroTownData(
    i32 x,
    i32 y,
    hero* firstHero,
    armyGroup* firstArmy,
    town* combatTown,
    hero* secondHero,
    armyGroup* secondArmy,
    i32 setupCombatX,
    i32 setupCombatY,
    i32 randomSeed,
    i32 remotePlayer,
    H2_ENUM_PARAM(CombatResult, i32) combatResult,
    i32 retreatWin,
    i32 combatSurrender
) {
    char* reply;
    i32 result;
    combatRemoteData* buffer = NULL;

    buffer = static_cast<combatRemoteData*>(H2_ALLOC(COMBAT_REMOTE_BUFFER_SIZE));
    reply = NULL;
    buffer->fragment = 0;
    buffer->x = static_cast<i8>(x);
    buffer->y = static_cast<i8>(y);
    buffer->hasFirstHero = firstHero != NULL;
    buffer->hasTown = combatTown != NULL;
    buffer->hasSecondHero = secondHero != NULL;
    buffer->setupCombatX = static_cast<i8>(setupCombatX);
    buffer->setupCombatY = static_cast<i8>(setupCombatY);
    buffer->randomSeed = randomSeed;
    buffer->combatResult = combatResult;
    buffer->retreatWin = static_cast<i8>(retreatWin);
    buffer->combatSurrender = static_cast<i8>(combatSurrender);
    buffer->firstOwner = static_cast<i8>(firstHero ? firstHero->m_owner : -1);
    buffer->firstGold =
        firstHero ? gpGame->m_players[firstHero->m_owner].m_resources[IDX(RES_GOLD)] : 0;
    buffer->secondOwner = static_cast<i8>(secondHero ? secondHero->m_owner : -1);
    buffer->secondGold =
        secondHero ? gpGame->m_players[secondHero->m_owner].m_resources[IDX(RES_GOLD)] : 0;
    memcpy(&buffer->firstArmy, firstArmy, sizeof(armyGroup));
    memcpy(&buffer->secondArmy, secondArmy, sizeof(armyGroup));
    if (combatTown)
        memcpy(&buffer->combatTown, combatTown, sizeof(town));

    result = TransmitAndWait(
        reinterpret_cast<char*>(buffer),
        remotePlayer,
        COMBAT_REMOTE_HEADER_SIZE,
        REMOTE_COMMAND,
        REMOTE_CONFIRM_COMMAND,
        &reply
    );
    if (!result)
        ShutDown(NULL);

    if (firstHero) {
        EVENTS_HERO_BUFFER(buffer)->fragment = REMOTE_FIRST_HERO_FIRST;
        memcpy(EVENTS_HERO_BUFFER(buffer)->data, firstHero, COMBAT_REMOTE_HERO_FIRST_SIZE);
        result = TransmitRemoteData(
            reinterpret_cast<char*>(buffer),
            remotePlayer,
            COMBAT_REMOTE_HERO_FIRST_SIZE + 1,
            REMOTE_COMMAND,
            COMBAT_REMOTE_FRAGMENT_TYPE,
            COMBAT_REMOTE_FRAGMENT_TYPE,
            REMOTE_MESSAGE_DEFAULT
        );
        if (!result)
            ShutDown(NULL);
        EVENTS_HERO_BUFFER(buffer)->fragment = REMOTE_FIRST_HERO_SECOND;
        memcpy(
            EVENTS_HERO_BUFFER(buffer)->data,
            &firstHero->m_spells[COMBAT_REMOTE_HERO_SECOND_SPELL_INDEX],
            COMBAT_REMOTE_HERO_SECOND_SIZE
        );
        result = TransmitRemoteData(
            reinterpret_cast<char*>(buffer),
            remotePlayer,
            COMBAT_REMOTE_HERO_SECOND_SIZE + 1,
            REMOTE_COMMAND,
            COMBAT_REMOTE_FRAGMENT_TYPE,
            COMBAT_REMOTE_FRAGMENT_TYPE,
            REMOTE_MESSAGE_DEFAULT
        );
        if (!result)
            ShutDown(NULL);
    }
    if (secondHero) {
        EVENTS_HERO_BUFFER(buffer)->fragment = REMOTE_SECOND_HERO_FIRST;
        memcpy(EVENTS_HERO_BUFFER(buffer)->data, secondHero, COMBAT_REMOTE_HERO_FIRST_SIZE);
        result = TransmitRemoteData(
            reinterpret_cast<char*>(buffer),
            remotePlayer,
            COMBAT_REMOTE_HERO_FIRST_SIZE + 1,
            REMOTE_COMMAND,
            COMBAT_REMOTE_FRAGMENT_TYPE,
            COMBAT_REMOTE_FRAGMENT_TYPE,
            REMOTE_MESSAGE_DEFAULT
        );
        if (!result)
            ShutDown(NULL);
        EVENTS_HERO_BUFFER(buffer)->fragment = REMOTE_SECOND_HERO_SECOND;
        memcpy(
            EVENTS_HERO_BUFFER(buffer)->data,
            &secondHero->m_spells[COMBAT_REMOTE_HERO_SECOND_SPELL_INDEX],
            COMBAT_REMOTE_HERO_SECOND_SIZE
        );
        result = TransmitRemoteData(
            reinterpret_cast<char*>(buffer),
            remotePlayer,
            COMBAT_REMOTE_HERO_SECOND_SIZE + 1,
            REMOTE_COMMAND,
            COMBAT_REMOTE_FRAGMENT_TYPE,
            COMBAT_REMOTE_FRAGMENT_TYPE,
            REMOTE_MESSAGE_DEFAULT
        );
        if (!result)
            ShutDown(NULL);
    }
    H2_FREE(buffer);
}

VA(0x00449582, 0x3de)
void advManager::ReceiveHeroTownData(
    char* packet,
    i32* remotePlayer,
    i32* x,
    i32* y,
    hero** firstHero,
    armyGroup** firstArmy,
    town** combatTown,
    hero** secondHero,
    armyGroup** secondArmy,
    i32* setupCombatX,
    i32* setupCombatY,
    i32* randomSeed,
    H2_ENUM_STORAGE(CombatResult, i8)* combatResult,
    i8* retreatWin,
    i8* combatSurrender
) {
    i32 hasFirstHero4;
    i32 gotSecondHeroFirst2;
    i32 gotFirstHeroSecond;
    i32 firstOwner8;
    i32 gotSecondHeroSecond2;
    i32 result7;
    i32 hasTown2;
    i32 hasSecondHero0;
    i32 gotFirstHeroFirst0;
    i32l lastPacketTime7;
    i32 secondOwner;

    *firstHero = NULL;
    *firstArmy = NULL;
    *combatTown = NULL;
    *secondHero = NULL;
    *secondArmy = NULL;
    hasFirstHero4 = hasSecondHero0 = hasTown2 = 0;
    *remotePlayer = EVENTS_REMOTE_MESSAGE(packet)->sender;
    *x = EVENTS_REMOTE_COMBAT(packet)->x;
    *y = EVENTS_REMOTE_COMBAT(packet)->y;
    hasFirstHero4 = EVENTS_REMOTE_COMBAT(packet)->hasFirstHero;
    hasTown2 = EVENTS_REMOTE_COMBAT(packet)->hasTown;
    hasSecondHero0 = EVENTS_REMOTE_COMBAT(packet)->hasSecondHero;
    *setupCombatX = EVENTS_REMOTE_COMBAT(packet)->setupCombatX;
    *setupCombatY = EVENTS_REMOTE_COMBAT(packet)->setupCombatY;
    *randomSeed = EVENTS_REMOTE_COMBAT(packet)->randomSeed;
    *combatResult = EVENTS_REMOTE_COMBAT(packet)->combatResult;
    *retreatWin = EVENTS_REMOTE_COMBAT(packet)->retreatWin;
    *combatSurrender = EVENTS_REMOTE_COMBAT(packet)->combatSurrender;
    firstOwner8 = EVENTS_REMOTE_COMBAT(packet)->firstOwner;
    if (firstOwner8 > 0)
        gpGame->m_players[firstOwner8].m_resources[IDX(RES_GOLD)] =
            EVENTS_REMOTE_COMBAT(packet)->firstGold;
    secondOwner = EVENTS_REMOTE_COMBAT(packet)->secondOwner;
    if (secondOwner > 0)
        gpGame->m_players[secondOwner].m_resources[IDX(RES_GOLD)] =
            EVENTS_REMOTE_COMBAT(packet)->secondGold;

    *firstArmy = static_cast<armyGroup*>(H2_ALLOC(sizeof(armyGroup)));
    memcpy(*firstArmy, &EVENTS_REMOTE_COMBAT(packet)->firstArmy, sizeof(armyGroup));
    *secondArmy = static_cast<armyGroup*>(H2_ALLOC(sizeof(armyGroup)));
    memcpy(*secondArmy, &EVENTS_REMOTE_COMBAT(packet)->secondArmy, sizeof(armyGroup));
    if (hasTown2) {
        *combatTown = static_cast<town*>(H2_ALLOC(sizeof(town)));
        memcpy(*combatTown, &EVENTS_REMOTE_COMBAT(packet)->combatTown, sizeof(town));
    }

    iCombatControlNetPos[IDX(COMBAT_ATTACKER_SIDE)] = *remotePlayer;
    iCombatControlNetPos[IDX(COMBAT_DEFENDER_SIDE)] = giThisNetPos;
    result7 = TransmitRemoteData(
        NULL,
        *remotePlayer,
        0,
        REMOTE_CONFIRM_COMMAND,
        COMBAT_REMOTE_FRAGMENT_TYPE,
        COMBAT_REMOTE_FRAGMENT_TYPE,
        REMOTE_MESSAGE_DEFAULT
    );
    if (!result7)
        ShutDown(NULL);

    lastPacketTime7 = KBTickCount();
    gotFirstHeroFirst0 = 1;
    gotFirstHeroSecond = 1;
    gotSecondHeroFirst2 = 1;
    gotSecondHeroSecond2 = 1;
    if (hasFirstHero4) {
        *firstHero = static_cast<hero*>(H2_ALLOC(sizeof(hero)));
        gotFirstHeroFirst0 = 0;
        gotFirstHeroSecond = 0;
    }
    if (hasSecondHero0) {
        *secondHero = static_cast<hero*>(H2_ALLOC(sizeof(hero)));
        gotSecondHeroFirst2 = 0;
        gotSecondHeroSecond2 = 0;
    }

    while (!gotFirstHeroFirst0 || !gotFirstHeroSecond || !gotSecondHeroFirst2
           || !gotSecondHeroSecond2) {
        PollSound();
        if (lastPacketTime7 + COMBAT_REMOTE_TIMEOUT < KBTickCount()) {
            NormalDialog(
                const_cast<char*>("\xce\xf8\xe8\xe1\xea\xe0 \xef\xee\xeb\xf3\xf7\xe5\xed\xe8\xff \xe8\xed\xf4\xee\xf0\xec\xe0\xf6\xe8\xe8. \xcf\xf0\xee\xe4\xee\xeb\xe6\xe0\xf2\xfc?"),
                NORMAL_DIALOG_CONFIRM,
                -1,
                -1,
                -1,
                0,
                -1,
                0,
                -1,
                0
            );
            if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES)
                lastPacketTime7 = KBTickCount();
            else
                ShutDown(const_cast<char*>("\xc8\xe3\xf0\xe0 \xef\xf0\xe5\xea\xf0\xe0\xf9\xe5\xed\xe0."));
        }
        packet = GetRemoteData(1);
        if (packet && EVENTS_REMOTE_MESSAGE(packet)->type == REMOTE_MESSAGE_RELIABLE
            && EVENTS_REMOTE_MESSAGE(packet)->command == REMOTE_COMMAND) {
            lastPacketTime7 = KBTickCount();
            if (EVENTS_REMOTE_HERO(packet)->fragment == REMOTE_FIRST_HERO_FIRST) {
                memcpy(*firstHero, EVENTS_REMOTE_HERO(packet)->data, COMBAT_REMOTE_HERO_FIRST_SIZE);
                gotFirstHeroFirst0 = 1;
            }
            if (EVENTS_REMOTE_HERO(packet)->fragment == REMOTE_FIRST_HERO_SECOND) {
                memcpy(
                    &(*firstHero)->m_spells[COMBAT_REMOTE_HERO_SECOND_SPELL_INDEX],
                    EVENTS_REMOTE_HERO(packet)->data,
                    COMBAT_REMOTE_HERO_SECOND_SIZE
                );
                gotFirstHeroSecond = 1;
            }
            if (EVENTS_REMOTE_HERO(packet)->fragment == REMOTE_SECOND_HERO_FIRST) {
                memcpy(
                    *secondHero,
                    EVENTS_REMOTE_HERO(packet)->data,
                    COMBAT_REMOTE_HERO_FIRST_SIZE
                );
                gotSecondHeroFirst2 = 1;
            }
            if (EVENTS_REMOTE_HERO(packet)->fragment == REMOTE_SECOND_HERO_SECOND) {
                memcpy(
                    &(*secondHero)->m_spells[COMBAT_REMOTE_HERO_SECOND_SPELL_INDEX],
                    EVENTS_REMOTE_HERO(packet)->data,
                    COMBAT_REMOTE_HERO_SECOND_SIZE
                );
                gotSecondHeroSecond2 = 1;
            }
        }
    }
}

VA(0x00449960, 0x1fc)
CombatResult advManager::AutoResolveCombat(
    i32 x,
    i32 y,
    hero* firstHero,
    armyGroup* firstArmy,
    town* combatTown,
    hero* secondHero,
    armyGroup* secondArmy,
    i32 setupCombatX,
    i32 setupCombatY,
    i32 randomSeed,
    i32 processLosses
) {
    tag_message message;
    i32 savedShowIt;

    gbNoShowCombat = true;
    savedShowIt = bShowIt;
    bShowIt = 0;
    gpMouseManager->SetPointer(0);
    gpMouseManager->m_forcePointerUpdate = 1;
    message.type = MESSAGE_NONE;
    DemobilizeCurrHero();
    gpCombatManager->SetupCombat(
        x,
        y,
        firstHero,
        firstArmy,
        combatTown,
        secondHero,
        secondArmy,
        x,
        y,
        randomSeed
    );
    gpCombatManager->InitNonVisualVars();
    gpCombatManager->m_gridSelectionDisabled = 1;
    while (!gpCombatManager->m_nonVisualCombat)
        gpCombatManager->Main(message);
    gbNoShowCombat = false;
    if (firstHero)
        firstHero->CheckLevel();
    if (secondHero)
        secondHero->CheckLevel();
    if (processLosses) {
        switch (gpCombatManager->m_combatResult) {
            case COMBAT_RESULT_ATTACKER:
                if (!gbRetreatWin)
                    TransferArtifacts(secondHero, firstHero);
                HeroLoses(secondHero);
                break;
            case COMBAT_RESULT_DEFENDER:
                if (!gbRetreatWin)
                    TransferArtifacts(firstHero, secondHero);
                HeroLoses(firstHero);
                break;
            case COMBAT_RESULT_DRAW:
                HeroLoses(firstHero);
                HeroLoses(secondHero);
                break;
            case COMBAT_RESULT_PENDING:
                break;
        }
    }
    bShowIt = savedShowIt;
    if (!gbHumanPlayer[giCurPlayer]) {
        gpGame->ShowComputerScreen();
        gpGame->TurnOnAIMusic();
        SetNoDialogMenus(0);
    } else {
        SetNoDialogMenus(1);
    }
    MobilizeCurrHero(0);
    if (processLosses)
        gbRetreatWin = false;
    gbInCombat = false;
    gpMouseManager->m_forcePointerUpdate = 0;
    return gpCombatManager->m_combatResult;
}

VA(0x00449b5c, 0x8f)
i32 RiddleStringsEqual(char* answer, char* expected) {
    i32 index;
    char expectedPrefix[RIDDLE_EXPECTED_BUFFER_SIZE];
    char answerPrefix[RIDDLE_ANSWER_BUFFER_SIZE];

    strncpy(expectedPrefix, expected, RIDDLE_PREFIX_LENGTH);
    expectedPrefix[RIDDLE_PREFIX_LENGTH] = 0;
    for (index = RIDDLE_PREFIX_LENGTH - 1; index >= 0; index--) {
        if (expectedPrefix[index] != ' ') {
            expectedPrefix[index + 1] = 0;
            break;
        }
    }
    strncpy(answerPrefix, answer, RIDDLE_PREFIX_LENGTH);
    answerPrefix[strlen(expectedPrefix)] = 0;
    return strcmpi(expectedPrefix, answerPrefix) == 0;
}

b32 gbNoShowCombat = false;

