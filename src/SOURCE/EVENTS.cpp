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

#define GENERIC_SITE_SIREN_ARMY_REMAINDER DATA_COMPGEN(0x004eb920, aRMYREMAINDERConstant, 0.7)
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
        CHEST_EXPERIENCE_LEVEL_SCALE = 4,
        CHEST_EXPERIENCE_LEVEL_OFFSET = 4,
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

} // namespace

#define TRADING_POST_EFFICIENCY DATA_COMPGEN(0x004eb918, pOSTEFFICIENCYConstant, 0.2f)

#define RETAIL_FILE const_cast<char*>("I:\\Projects\\Heroes\\Prog\\SOURCE\\EVENTS.CPP")
#define EVENTS_REMOTE_MESSAGE(buffer) (reinterpret_cast<RemoteMessage*>(buffer))
#define EVENTS_REMOTE_COMBAT(buffer)                                                               \
    (reinterpret_cast<combatRemoteData*>(EVENTS_REMOTE_MESSAGE(buffer)->payload))
#define EVENTS_REMOTE_HERO(buffer)                                                                 \
    (reinterpret_cast<combatRemoteHeroFragment*>(EVENTS_REMOTE_MESSAGE(buffer)->payload))
#define EVENTS_HERO_BUFFER(buffer) (reinterpret_cast<combatRemoteHeroFragment*>(buffer))

DATA(0x005190a4) static char s_twoStringFormat[] = "%s %s";

VA(0x004a8530, 0x5adb)
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

    eventHero2 = &gpGame->m_heroRecs[gpCurPlayer->CurrentHero()];
    eventType_g = cell->m_triggerType & MAP_TRIGGER_TYPE_MASK;
    eraseObject = 0;
    fizzleType3 = 0;
    playedSample3 = NULL_SAMPLE2;
    gpMouseManager->ShowColorPointer();
    gpMouseManager->SetPointer(0);
    eventSample_f = NULL_SAMPLE2;

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
                    DATA_COMPGEN(0x00517620, doEventMagicGardenYouVeFoundA, "{Magic Garden}\n\nYou've found a magic garden, the kind of place that "
                    "leprechauns and faeries like to cavort in, but there is no one here today.  "
                    "Perhaps you should try again next week."),
                    -1,
                    0,
                    -1,
                    0,
                    -1
                );
            } else {
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                resourceType = static_cast<ResourceType>(
                    cell->m_objectMetadata - MAP_EVENT_RESOURCE_OFFSET
                );
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    DATA_COMPGEN(0x005176d8, doEventMagicGardenYouCatchALeprechaun, "{Magic Garden}\n\nYou catch a leprechaun foolishly sleeping amidst a cluster "
                    "of magic mushrooms.  In exchange for his freedom, he guides you to a small "
                    "pot filled with precious things."),
                    IDX(resourceType),
                    resourceType == RES_GOLD ? MAP_EVENT_GOLD_AMOUNT : MAP_EVENT_RESOURCE_AMOUNT,
                    -1,
                    0,
                    -1
                );
                GiveResource(
                    eventHero2,
                    resourceType,
                    resourceType == RES_GOLD ? MAP_EVENT_GOLD_AMOUNT : MAP_EVENT_RESOURCE_AMOUNT
                );
                cell->m_objectMetadata = MAP_EVENT_DATA_EMPTY;
            }
            break;

        case MAP_OBJECT_SPHINX:
            EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
            eventExtra1 = reinterpret_cast<mapEventExtra*>(ppMapExtra[cell->m_objectMetadata]);
            if (!eventExtra1->active) {
                NormalDialog(
                    DATA_COMPGEN(0x00517790, doEventSphinxYouComeAcrossAGiant, "{Sphinx}\n\nYou come across a giant Sphinx.  The Sphinx remains strangely "
                    "quiet."),
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
                    DATA_COMPGEN(0x005177e0, doEventIHaveARiddleForYou, "\"I have a riddle for you,\" the Sphinx says.  \"Answer correctly, and you "
                    "shall be rewarded.  Answer incorrectly, and you shall be eaten.  Do you "
                    "accept the challenge?\"")
                );
                NormalDialog(gText, NORMAL_DIALOG_CONFIRM, -1, -1, -1, 0, -1, 0, -1, 0);
                if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
                    sprintf(
                        gText,
                        DATA_COMPGEN(0x00517888, doEventTheSphinxAsksYouTheFollowing, "The Sphinx asks you the following riddle:\n\n'%s'\n\nYour answer?"),
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
                            DATA_COMPGEN(0x005178c8, doEventLookingSomewhatDisappointedTheSphinxSighs, "Looking somewhat disappointed, the Sphinx sighs.  You've answered my "
                            "riddle so here's your reward.  Now begone."),
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
                            DATA_COMPGEN(0x00517938, doEventYouGuessedIncorrectlyTheSphinxSays, "\"You guessed incorrectly,\" the Sphinx says, smiling.  The Sphinx "
                            "swipes at you with a paw, knocking you to the ground.  Another blow "
                            "makes the world go black, and you know no more."),
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
                DATA_COMPGEN(0x005179f0, doEventObservationTowerFromTheObservationTower, "{Observation Tower}\n\nFrom the observation tower, you are able to see distant "
                "lands."),
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
            thirdUpgrade = CREATURE_NONE;
            secondUpgrade1 = CREATURE_NONE;
            firstUpgrade_e = CREATURE_NONE;
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

            if (firstUpgrade_e == CREATURE_NONE) {
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    DATA_COMPGEN(0x00517bf4, doEventHillFortAnUnusualAllianceOf, "{Hill Fort}\n\nAn unusual alliance of Orcs, Ogres, and Dwarves offer to train "
                    "(upgrade) any such troops brought to them.  Unfortunately, you have none with "
                    "you."),
                    -1,
                    0,
                    -1,
                    0,
                    -1
                );
            } else {
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                eventHero2->UpgradeCreatures(CREATURE_OGRE, CREATURE_OGRE_LORD);
                eventHero2->UpgradeCreatures(CREATURE_ORC, CREATURE_ORC_CHIEF);
                eventHero2->UpgradeCreatures(CREATURE_DWARF, CREATURE_BATTLE_DWARF);
                if (thirdUpgrade == CREATURE_NONE) {
                    if (secondUpgrade1 == CREATURE_NONE) {
                        sprintf(
                            gText,
                            DATA_COMPGEN(0x00517b70, doEventHillFortAllOfTheS, "{Hill Fort}\n\nAll of the %s you have in your army have been trained "
                            "by the battle masters of the fort.  Your army now contains %s."),
                            gArmyNamesPlural[IDX(firstUpgrade_e)],
                            gArmyNamesPlural[IDX(firstUpgrade_e) + 1]
                        );
                    } else {
                        sprintf(
                            gText,
                            DATA_COMPGEN(0x00517ae0, doEventHillFortAllOfTheS2, "{Hill Fort}\n\nAll of the %s and %s you have in your army have been "
                            "trained by the battle masters of the fort.  Your army now contains %s "
                            "and %s."),
                            gArmyNamesPlural[IDX(firstUpgrade_e)],
                            gArmyNamesPlural[IDX(secondUpgrade1)],
                            gArmyNamesPlural[IDX(firstUpgrade_e) + 1],
                            gArmyNamesPlural[IDX(secondUpgrade1) + 1]
                        );
                    }
                } else {
                    sprintf(
                        gText,
                        DATA_COMPGEN(0x00517a44, doEventHillFortAllOfTheS3, "{Hill Fort}\n\n All of the %s, %s and %s you have in your army have been "
                        "trained by the battle masters of the fort.  Your army now contains %s, "
                        "%s, and %s."),
                        gArmyNamesPlural[IDX(firstUpgrade_e)],
                        gArmyNamesPlural[IDX(secondUpgrade1)],
                        gArmyNamesPlural[IDX(thirdUpgrade)],
                        gArmyNamesPlural[IDX(firstUpgrade_e) + 1],
                        gArmyNamesPlural[IDX(secondUpgrade1) + 1],
                        gArmyNamesPlural[IDX(thirdUpgrade) + 1]
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

            if (firstUpgrade_e == CREATURE_NONE) {
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    DATA_COMPGEN(0x00517d7c, doEventFreemanSFoundryABlacksmithWorking, "{Freeman's Foundry}\n\nA blacksmith working at the foundry offers to convert "
                    "all Pikemen and Swordsmen's weapons brought to him from iron to steel. He "
                    "also says that he knows a process that will convert Iron Golems into Steel "
                    "Golems.  Unfortunately, you have none of these troops in your army, so he "
                    "can't help you."),
                    -1,
                    0,
                    -1,
                    0,
                    -1
                );
            } else {
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                eventHero2->UpgradeCreatures(CREATURE_PIKEMAN, CREATURE_VETERAN_PIKEMAN);
                eventHero2->UpgradeCreatures(
                    CREATURE_SWORDSMAN,
                    CREATURE_MASTER_SWORDSMAN
                );
                eventHero2->UpgradeCreatures(CREATURE_IRON_GOLEM, CREATURE_STEEL_GOLEM);
                if (thirdUpgrade == CREATURE_NONE) {
                    if (secondUpgrade1 == CREATURE_NONE) {
                        sprintf(
                            gText,
                            DATA_COMPGEN(0x00517d3c, doEventFreemanSFoundryAllOfYour, "{Freeman's Foundry}\n\nAll of your %s have been upgraded into %s."),
                            gArmyNamesPlural[IDX(firstUpgrade_e)],
                            gArmyNamesPlural[IDX(firstUpgrade_e) + 1]
                        );
                    } else {
                        sprintf(
                            gText,
                            DATA_COMPGEN(0x00517cec, doEventFreemanSFoundryAllOfYour2, "{Freeman's Foundry}\n\nAll of your %s and %s have been upgraded into "
                            "%s and %s."),
                            gArmyNamesPlural[IDX(firstUpgrade_e)],
                            gArmyNamesPlural[IDX(secondUpgrade1)],
                            gArmyNamesPlural[IDX(firstUpgrade_e) + 1],
                            gArmyNamesPlural[IDX(secondUpgrade1) + 1]
                        );
                    }
                } else {
                    sprintf(
                        gText,
                        DATA_COMPGEN(0x00517c94, doEventFreemanSFoundryAllOfYour3, "{Freeman's Foundry}\n\nAll of your  %s, %s and %s have been upgraded into "
                        "%s, %s, and %s."),
                        gArmyNamesPlural[IDX(firstUpgrade_e)],
                        gArmyNamesPlural[IDX(secondUpgrade1)],
                        gArmyNamesPlural[IDX(thirdUpgrade)],
                        gArmyNamesPlural[IDX(firstUpgrade_e) + 1],
                        gArmyNamesPlural[IDX(secondUpgrade1) + 1],
                        gArmyNamesPlural[IDX(thirdUpgrade) + 1]
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
            }
            break;

        case MAP_OBJECT_SEA_CHEST:
            if (!(cell->m_objectMetadata & CHEST_ARTIFACT_FLAG)
                || eventHero2->NumArtifacts() >= EVENT_ARTIFACT_CAPACITY) {
                if (cell->m_objectMetadata == SEA_CHEST_OUTCOME_EMPTY) {
                    NormalDialog(
                        DATA_COMPGEN(0x00517f9c, doEventChestAfterSpendingHoursTryingTo, "{Chest}\n\nAfter spending hours trying to fish the chest out of the sea, "
                        "you open it, only to find it empty."),
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
                        DATA_COMPGEN(0x00517f2c, doEventChestAfterSpendingHoursTryingTo2, "{Chest}\n\nAfter spending hours trying to fish the chest out of the sea, "
                        "you open it and find 1500 gold pieces."),
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
                    DATA_COMPGEN(0x00517eb8, doEventChestAfterSpendingHoursTryingTo3, "{Chest}\n\nAfter spending hours trying to fish the chest out of the sea, you "
                    "open it and find 1000 gold and the %s"),
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
                        DATA_COMPGEN(0x00518008, doEventFlotsamYouSearchThroughTheFlotsam, "{Flotsam}\n\nYou search through the flotsam, but find nothing."),
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
                        DATA_COMPGEN(0x00518048, doEventFlotsamYouSearchThroughTheFlotsam2, "{Flotsam}\n\nYou search through the flotsam, and find some wood."),
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
                        DATA_COMPGEN(0x00518088, doEventFlotsamYouSearchThroughTheFlotsam3, "{Flotsam}\n\nYou search through the flotsam, and find some wood and some "
                        "gold."),
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
                        DATA_COMPGEN(0x005180d8, doEventFlotsamYouSearchThroughTheFlotsam4, "{Flotsam}\n\nYou search through the flotsam and find some wood and some "
                        "gold."),
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
                    DATA_COMPGEN(0x00518124, doEventShipwreckSurvivorYouVePulledA, "{Shipwreck Survivor}\n\nYou've pulled a shipwreck survivor from certain death "
                    "in an unforgiving ocean.  Grateful, he rewards you for your act of kindness "
                    "by giving you the %s."),
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
                    DATA_COMPGEN(0x005181d4, doEventShipwreckSurvivorYouVePulledA2, "{Shipwreck Survivor}\n\nYou've pulled a shipwreck survivor from certain death "
                    "in an unforgiving ocean.  Grateful, he says, \"I would give you an artifact "
                    "as a reward, but you're all full.\""),
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
                    DATA_COMPGEN(0x00518290, doEventMagellanSMapsTheCaptainSighs, "{Magellan's Maps}\n\nThe captain sighs.  \"You don't have enough money, eh?  "
                    "You can't expect me to give my maps away for free!\""),
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
                    DATA_COMPGEN(0x00518310, doEventMagellanSMapsARetiredCaptain, "{Magellan's Maps}\n\nA retired captain living on this refurbished fishing "
                    "platform offers to sell you maps of the sea he made in his younger days for "
                    "1,000 gold.  Do you wish to buy the maps?"),
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
            if (eventHero2->m_secondarySkills[cell->m_objectMetadata]
                == HERO_SKILL_LEVEL_NONE) {
                if (eventHero2->m_secondarySkillCount < HERO_SECONDARY_SKILL_LIMIT) {
                    EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                    sprintf(
                        gText,
                        DATA_COMPGEN(0x005185ac, doEventWitchSHutAnAncientAnd, "{Witch's Hut}\n\nAn ancient and immortal witch living in a hut with "
                        "bird's legs for stilts teaches you %s for her own inscrutable purposes."),
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
                } else {
                    sprintf(
                        gText,
                        DATA_COMPGEN(0x005184a4, doEventWitchSHutYouApproachThe, "{Witch's Hut}\n\nYou approach the hut and observe a witch inside studying "
                        "an ancient tome on %s.  As you approach, she turns and focuses her one "
                        "glass eye on you.  \"You already know everything you deserve to learn!\" "
                        "the witch screeches. \"NOW GET OUT OF MY HOUSE!\""),
                        gSecondarySkills[cell->m_objectMetadata]
                    );
                    NormalDialog(gText, NORMAL_DIALOG_INFO, -1, -1, -1, 0, -1, 0, -1, 0);
                }
            } else {
                sprintf(
                    gText,
                    DATA_COMPGEN(0x005183d0, doEventWitchSHutYouApproachThe2, "{Witch's Hut}\n\nYou approach the hut and observe a witch inside studying an "
                    "ancient tome on %s.  As you approach, she turns and speaks.  \"You already "
                    "know that which I would teach you. I can help you no further.\""),
                    gSecondarySkills[cell->m_objectMetadata]
                );
                NormalDialog(gText, NORMAL_DIALOG_INFO, -1, -1, -1, 0, -1, 0, -1, 0);
            }
            break;

        case MAP_OBJECT_ARTESIAN_SPRING:
            if (!cell->m_objectMetadata) {
                NormalDialog(
                    DATA_COMPGEN(0x00518638, doEventArtesianSpringTheSpringOnlyRefills, "{Artesian Spring}\n\nThe spring only refills once a week, and someone's "
                    "already been here this week."),
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
                        DATA_COMPGEN(0x00518720, doEventArtesianSpringADrinkFromThe, "{Artesian Spring}\n\nA drink from the spring fills your blood with magic! "
                        " You have twice your normal spell points in reserve."),
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
                        DATA_COMPGEN(0x0051869c, doEventArtesianSpringADrinkAtThe, "{Artesian Spring}\n\nA drink at the spring is supposed to give you twice "
                        "your normal spell points, but you are already at that level."),
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
                        DATA_COMPGEN(0x00518854, doEventMagicWellADrinkFromThe, "{Magic Well}\n\nA drink from the well has restored your spell points to "
                        "maximum."),
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
                    eventHero2->m_eventFlags = eventHero2->m_eventFlags | HERO_EVENT_MAGIC_WELL;
                    eventHero2->m_spellPoints = wellSpellPoints5;
                } else {
                    NormalDialog(
                        DATA_COMPGEN(0x005187e8, doEventMagicWellADrinkAtThe, "{Magic Well}\n\nA drink at the well is supposed to restore your spell "
                        "points, but you are already at maximum."),
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
                    DATA_COMPGEN(0x005187a0, doEventMagicWellASecondDrinkAt, "{Magic Well}\n\nA second drink at the well in one day will not help you."),
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
                eventHero2->m_eventFlags = eventHero2->m_eventFlags & ~HERO_EVENT_EMBARKED;
                eventHero2->m_remainingMobility = 0;
                eventHero2->m_direction = m_cursorDirection;
                m_cursorType = eventHero2->m_cursorType;
                m_cursorFrame = GetCursorBaseFrame(m_cursorDirection);
                m_cursorActive = 1;
                playedSample3 = LoadPlaySample(DATA_COMPGEN(0x005188a4, doEventKillfade82m, "killfade.82m"));
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
                WaitEndSample(playedSample3, -1);
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
            eventHero2->m_eventFlags = eventHero2->m_eventFlags | HERO_EVENT_EMBARKED;
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
                    DATA_COMPGEN(0x0051b4d4, doEventEmptyString, ""),
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
                    DATA_COMPGEN(0x0051b4d8, doEventEmptyString2, ""),
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
                    DATA_COMPGEN(0x005188b4, doEventSecondaryEmptyString, ""),
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
                    DATA_COMPGEN(0x005188b8, doEventSecondaryEmptyString2, ""),
                    -1,
                    0,
                    -1,
                    0,
                    -1
                );
            }
            break;

        case MAP_OBJECT_TREASURE_CHEST:
        chestGold:
            if (!(cell->m_objectMetadata & CHEST_ARTIFACT_FLAG)) {
                EventWindow(
                    -1,
                    NORMAL_DIALOG_CONFIRM,
                    DATA_COMPGEN(0x0051892c, doEventChestAfterScouringTheAreaYou, "{Chest}\n\nAfter scouring the area, you fall upon a hidden treasure cache.  "
                    "You may take the gold or distribute the gold to the peasants for experience.  "
                    "Do you wish to keep the gold?"),
                    IDX(RES_GOLD),
                    cell->m_objectMetadata * CHEST_GOLD_MULTIPLIER,
                    NORMAL_DIALOG_EXPERIENCE,
                    (cell->m_objectMetadata * CHEST_EXPERIENCE_LEVEL_SCALE
                     - CHEST_EXPERIENCE_LEVEL_OFFSET)
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
                        (cell->m_objectMetadata * CHEST_EXPERIENCE_LEVEL_SCALE
                         - CHEST_EXPERIENCE_LEVEL_OFFSET)
                            * CHEST_EXPERIENCE_MULTIPLIER,
                        0
                    );
            } else {
                if (eventHero2->NumArtifacts() >= EVENT_ARTIFACT_CAPACITY) {
                    cell->m_objectMetadata = CHEST_REWARD_SMALL;
                    goto chestGold;
                }
                eventValue1 = cell->m_objectMetadata & CHEST_ARTIFACT_MASK;
                sprintf(
                    gText,
                    DATA_COMPGEN(0x005188c4, doEventChestAfterScouringTheAreaYou2, "{Chest}\n\nAfter scouring the area, you fall upon a hidden chest, containing "
                    "the ancient artifact '%s'"),
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
            }
            eraseObject = 1;
            fizzleType3 = 1;
            eventHero2->CheckLevel();
            break;

        case MAP_OBJECT_BUOY:
            if (!HAS(eventHero2->m_eventFlags, HERO_EVENT_BUOY)) {
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                eventHero2->m_eventFlags = eventHero2->m_eventFlags | HERO_EVENT_BUOY;
                eventHero2->m_morale++;
                EventWindow(
                    EVENT_TEXT_BUOY_REWARD,
                    NORMAL_DIALOG_INFO,
                    DATA_COMPGEN(0x005188bc, doEventEmptyString3, ""),
                    NORMAL_DIALOG_MORALE_BONUS,
                    0,
                    -1,
                    0,
                    -1
                );
            } else {
                EventWindow(EVENT_TEXT_BUOY_VISITED, NORMAL_DIALOG_INFO, DATA_COMPGEN(0x005188c0, doEventEmptyString4, ""), -1, 0, -1, 0, -1);
            }
            break;

        case MAP_OBJECT_FAERIE_RING:
            if (!HAS(eventHero2->m_eventFlags, HERO_EVENT_FAERIE_RING)) {
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                eventHero2->m_eventFlags = eventHero2->m_eventFlags | HERO_EVENT_FAERIE_RING;
                eventHero2->m_luck++;
                EventWindow(
                    EVENT_TEXT_FAERIE_RING_REWARD,
                    NORMAL_DIALOG_INFO,
                    DATA_COMPGEN(0x005189e4, doEventEmptyString5, ""),
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
                    DATA_COMPGEN(0x005189e8, doEventEmptyString6, ""),
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
                eventHero2->m_eventFlags = eventHero2->m_eventFlags | HERO_EVENT_IDOL;
                eventHero2->m_luck++;
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    DATA_COMPGEN(0x00518a9c, doEventIdolYouVeFoundAnAncient, "{Idol}\n\nYou've found an ancient and weathered stone idol.  Kissing it is "
                    "supposed to be lucky, so you do.  The stone is very cold to the touch."),
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
                    DATA_COMPGEN(0x005189f4, doEventIdolYouVeFoundAnAncient2, "{Idol}\n\nYou've found an ancient and weathered stone idol.  It is supposed "
                    "to grant luck to visitors, but since the stars are already smiling upon you, "
                    "it does nothing."),
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
                eventHero2->m_eventFlags = eventHero2->m_eventFlags | HERO_EVENT_FOUNTAIN;
                eventHero2->m_luck++;
                EventWindow(
                    EVENT_TEXT_FOUNTAIN_REWARD,
                    NORMAL_DIALOG_INFO,
                    DATA_COMPGEN(0x005189ec, doEventEmptyString7, ""),
                    NORMAL_DIALOG_LUCK_BONUS,
                    0,
                    -1,
                    0,
                    -1
                );
            } else {
                EventWindow(EVENT_TEXT_FOUNTAIN_VISITED, NORMAL_DIALOG_INFO, DATA_COMPGEN(0x005189f0, doEventEmptyString8, ""), -1, 0, -1, 0, -1);
            }
            break;

        case MAP_OBJECT_WATERING_HOLE:
            if (!HAS(eventHero2->m_eventFlags, HERO_EVENT_WATERING_HOLE)) {
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                eventHero2->m_eventFlags = eventHero2->m_eventFlags | HERO_EVENT_WATERING_HOLE;
                eventHero2->m_morale++;
                eventHero2->m_mobility += WATERING_HOLE_MOBILITY_BONUS;
                eventHero2->m_remainingMobility += WATERING_HOLE_MOBILITY_BONUS;
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    DATA_COMPGEN(0x00518bd4, doEventWateringHoleADrinkAtThe, "{Watering Hole}\n\nA drink at the watering hole fills your troops with "
                    "strength and lifts their spirits.  You can travel a bit further today."),
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
                    DATA_COMPGEN(0x00518b34, doEventWateringHoleTheDrinkAtThe, "{Watering Hole}\n\nThe drink at the watering hole is refreshing, but offers "
                    "no further benefit.  The watering hole might help again if you fought a "
                    "battle first."),
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
                eventHero2->m_eventFlags = eventHero2->m_eventFlags | HERO_EVENT_OASIS;
                eventHero2->m_morale++;
                eventHero2->m_mobility += OASIS_MOBILITY_BONUS;
                eventHero2->m_remainingMobility += OASIS_MOBILITY_BONUS;
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    DATA_COMPGEN(0x00518ce8, doEventOasisADrinkAtTheOasis, "{Oasis}\n\nA drink at the oasis fills your troops with strength and lifts "
                    "their spirits.  You can travel a bit further today."),
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
                    DATA_COMPGEN(0x00518c60, doEventOasisTheDrinkAtTheOasis, "{Oasis}\n\nThe drink at the oasis is refreshing, but offers no further "
                    "benefit.  The oasis might help again if you fought a battle first."),
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
                eventHero2->m_eventFlags = eventHero2->m_eventFlags | HERO_EVENT_TEMPLE;
                eventHero2->m_morale += TEMPLE_MORALE_BONUS;
                NormalDialog(
                    DATA_COMPGEN(0x00518dc0, doEventTempleAVisitAndAPrayer, "{Temple}\n\nA visit and a prayer at the temple raises the morale of your "
                    "troops."),
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
                    DATA_COMPGEN(0x00518d64, doEventTempleItDoesnTHelpTo, "{Temple}\n\nIt doesn't help to pray twice before a battle.  Come back after "
                    "you've fought."),
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
                    DATA_COMPGEN(0x00518e10, doEventLeanToTheLeanToIs, "{Lean To}\n\nThe lean-to is long abandoned.  There is nothing of value here."),
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
                    DATA_COMPGEN(0x00518e5c, doEventLeanToYouVeFoundAn, "{Lean To}\n\nYou've found an abandoned lean-to.  Poking about, you discover "
                    "some resources hidden nearby."),
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
                        DATA_COMPGEN(0x00518ff4, doEventWagonYouComeAcrossAnOld, "{Wagon}\n\nYou come across an old wagon left by a trader who didn't quite "
                        "make it to safe terrain.  Inside, you find some of the wagon's cargo "
                        "still intact."),
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
                        DATA_COMPGEN(0x00518f6c, doEventWagonYouComeAcrossAnOld2, "{Wagon}\n\nYou come across an old wagon left by a trader who didn't quite "
                        "make it to safe terrain.  Searching inside, you find the '%s'."),
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
                DATA_COMPGEN(0x00518ec4, doEventWagonYouComeAcrossAnOld3, "{Wagon}\n\nYou come across an old wagon left by a trader who didn't quite make it "
                "to safe terrain.  Unfortunately, others have found it first, and the wagon is "
                "empty."),
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
                        DATA_COMPGEN(0x00518b2c, doEventEmptyString9, ""),
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
                        sprintf(gText, DATA_COMPGEN(0x005190a0, doEventS, "%s."), DATA_COMPGEN(0x00519094, doEventTreasure, "Treasure"));
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
                DATA_COMPGEN(0x00518b30, doEventEmptyString10, ""),
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
            if (!(eventHero2->m_xanaduVisits & (1 << cell->m_objectMetadata))) {
                if (eventHero2->m_level
                        + IDX(eventHero2->m_secondarySkills[IDX(HERO_SKILL_DIPLOMACY)])
                              * XANADU_DIPLOMACY_MULTIPLIER
                    < XANADU_ADMISSION_LEVEL) {
                    NormalDialog(
                        DATA_COMPGEN(0x005191a4, doEventXanaduTheButlerOpensTheDoor, "{Xanadu}\n\nThe butler opens the door and looks you up and down.  \"You "
                        "are neither famous nor diplomatic enough to be admitted to see my "
                        "master,\" he sniffs.  \"Come back when you think yourself worthy.\""),
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
                        DATA_COMPGEN(0x0051912c, doEventXanaduTheButlerAdmitsYouTo, "{Xanadu}\n\nThe butler admits you to see the master of the house.  He "
                        "trains you in the four skills a hero should know."),
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
            } else {
                NormalDialog(
                    DATA_COMPGEN(0x005190b0, doEventXanaduRecognizingYouTheButlerRefuses, "{Xanadu}\n\nRecognizing you, the butler refuses to admit you.  \"The "
                    "master,\" he says, \"will not see the same student twice.\""),
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

        case MAP_OBJECT_FORT:
            if (!(eventHero2->m_fortVisits & (1 << cell->m_objectMetadata))) {
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                NormalDialog(
                    DATA_COMPGEN(0x005192dc, doEventFortTheSoldiersLivingInThe, "{Fort}\n\n The soldiers living in the fort teach you a few new defensive "
                    "tricks."),
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
            } else {
                NormalDialog(
                    DATA_COMPGEN(0x0051926c, doEventFortIMSorrySirThe, "{Fort}\n\n \"I'm sorry sir,\" The leader of the soldiers says, \"but you "
                    "already know everything we have to teach.\""),
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

        case MAP_OBJECT_STANDING_STONES:
            if (!(eventHero2->m_standingStoneVisits & (1 << cell->m_objectMetadata))) {
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                NormalDialog(
                    DATA_COMPGEN(0x005193e8, doEventStandingStonesYouVeFoundA, "{Standing Stones}\n\nYou've found a group of Druids worshipping at one of "
                    "their strange stone edifices.  Silently, they teach you new ways to cast "
                    "spells."),
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
            } else {
                NormalDialog(
                    DATA_COMPGEN(0x0051932c, doEventStandingStonesYouVeFoundA2, "{Standing Stones}\n\nYou've found a group of Druids worshipping at one of "
                    "their strange stone edifices.  Silently, the Druids turn you away, indicating "
                    "they have nothing new to teach you."),
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

        case MAP_OBJECT_WITCH_DOCTOR_HUT:
            if (!(eventHero2->m_witchDoctorVisits & (1 << cell->m_objectMetadata))) {
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                NormalDialog(
                    DATA_COMPGEN(0x005194d8, doEventWitchDoctorSHutAnOrcish, "{Witch Doctor's Hut}\n\nAn Orcish witch doctor living in the hut deepens your "
                    "knowledge of magic by showing you how to cast stones, read portents, and "
                    "decipher the intricacies of chicken entrails."),
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
            } else {
                NormalDialog(
                    DATA_COMPGEN(0x00519484, doEventWitchDoctorSHutGoWay, "{Witch Doctor's Hut}\n\n\"Go 'way!\", the witch doctor barks, \"you know all "
                    "I know.\""),
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

        case MAP_OBJECT_MERCENARY_CAMP:
            if (!(eventHero2->m_mercenaryCampVisits & (1 << cell->m_objectMetadata))) {
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                NormalDialog(
                    DATA_COMPGEN(0x00519644, doEventMercenaryCampYouVeComeUpon, "{Mercenary Camp}\n\nYou've come upon a mercenary camp practicing their "
                    "tactics.  The mercenaries welcome you and your troops and invite you to train "
                    "with them."),
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
            } else {
                NormalDialog(
                    DATA_COMPGEN(0x0051959c, doEventMercenaryCampYouVeComeUpon2, "{Mercenary Camp}\n\nYou've come upon a mercenary camp practicing their "
                    "tactics.  \"You're too advanced for us,\" the mercenary captain says.  \"We "
                    "can teach nothing more.\""),
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

        case MAP_OBJECT_GAZEBO:
            if (!(eventHero2->m_gazeboVisits & (1 << cell->m_objectMetadata))) {
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                EventWindow(
                    EVENT_TEXT_GAZEBO_REWARD,
                    NORMAL_DIALOG_INFO,
                    DATA_COMPGEN(0x00519090, doEventEmptyString11, ""),
                    NORMAL_DIALOG_EXPERIENCE,
                    GAZEBO_EXPERIENCE,
                    -1,
                    0,
                    -1
                );
                GiveExperience(eventHero2, GAZEBO_EXPERIENCE, 0);
                eventHero2->m_gazeboVisits |= 1 << cell->m_objectMetadata;
                eventHero2->CheckLevel();
            } else {
                EventWindow(EVENT_TEXT_GAZEBO_VISITED, NORMAL_DIALOG_INFO, DATA_COMPGEN(0x005190ac, doEventEmptyString12, ""), -1, 0, -1, 0, -1);
            }
            break;

        case MAP_OBJECT_WATER_WHEEL:
            if (!cell->m_objectMetadata) {
                EventWindow(EVENT_TEXT_WATER_WHEEL_EMPTY, NORMAL_DIALOG_INFO, DATA_COMPGEN(0x005196e4, doEventEmptyString13, ""), -1, 0, -1, 0, -1);
            } else {
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                EventWindow(
                    EVENT_TEXT_WATER_WHEEL_REWARD,
                    NORMAL_DIALOG_INFO,
                    DATA_COMPGEN(0x005196e8, doEventEmptyString14, ""),
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
            resourceType = static_cast<ResourceType>(cell->m_objectIndex >> 1);
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
                    DATA_COMPGEN(0x005196ec, doEventEmptyString15, ""),
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
                EventWindow(EVENT_TEXT_WINDMILL_EMPTY, NORMAL_DIALOG_INFO, DATA_COMPGEN(0x005196f0, doEventEmptyString16, ""), -1, 0, -1, 0, -1);
            }
            break;

        case MAP_OBJECT_ANCIENT_LAMP:
            EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
            EventWindow(EVENT_TEXT_GENIE_LAMP, NORMAL_DIALOG_CONFIRM, DATA_COMPGEN(0x005196f4, doEventEmptyString17, ""), -1, 0, -1, 0, -1);
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
                    DATA_COMPGEN(0x00519700, doEventTreeCityYouVeFoundA, "{Tree City}\n\nYou've found a Sprite Tree City.  Unfortunately, none of the "
                    "Sprites living there wish to join an army.  Maybe next week."),
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
                    DATA_COMPGEN(0x00519788, doEventTreeCitySomeOfTheSprites, "{Tree City}\n\nSome of the Sprites living in the tree city are willing to "
                    "join your army for a price.  Do you want to recruit Sprites?"),
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
                    DATA_COMPGEN(0x00519810, doEventRuinsYouSearchTheRuinsBut, "{Ruins}\n\nYou search the ruins, but the Medusas that used to live here are "
                    "gone.  Perhaps there will be more next week."),
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
                    DATA_COMPGEN(0x00519888, doEventRuinsYouVeFoundSomeMedusas, "{Ruins}\n\nYou've found some Medusas living in the ruins.  They are willing "
                    "to join your army for a price.  Do you want to recruit Medusas?"),
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
                    DATA_COMPGEN(0x00519914, doEventTrollBridgeYouVeFoundOne, "{Troll Bridge}\n\nYou've found one of those bridges that Trolls are so fond "
                    "of living under, but there are none here.  Perhaps there will be some next "
                    "week."),
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
                    DATA_COMPGEN(0x005199b0, doEventTrollBridgeTrollsLivingUnderThe, "{Troll Bridge}\n\nTrolls living under the bridge challenge you.  Will you "
                    "fight them?"),
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
                    DATA_COMPGEN(0x00519a04, doEventTrollBridgeAFewTrollsRemain, "{Troll Bridge}\n\nA few Trolls remain, cowering under the bridge.  They "
                    "approach you and offer to join your forces as mercenaries.  Do you want to "
                    "buy any Trolls?"),
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
                    DATA_COMPGEN(0x00519aa8, doEventTrollBridgeSomeTrollsLivingUnder, "{Troll Bridge}\n\nSome Trolls living under a bridge are willing to join your "
                    "army, but for a price.  Do you want to recruit Trolls?"),
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
                    DATA_COMPGEN(0x00519b2c, doEventCityOfTheDeadTheCity, "{City of the Dead}\n\nThe City of the Dead is empty of life, and empty of "
                    "unlife as well.  Perhaps some undead will move in next week."),
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
                    DATA_COMPGEN(0x00519bb4, doEventCityOfTheDeadYouVe, "{City of the Dead}\n\nYou've found the ruins of an ancient city, now "
                    "inhabited solely by the undead.  Will you search?"),
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
                    DATA_COMPGEN(0x00519c2c, doEventCityOfTheDeadSomeOf, "{City of the Dead}\n\nSome of the surviving Liches are impressed by your "
                    "victory over their fellows, and offer to join you for a price.  Do you want "
                    "to recruit Liches?"),
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
                    DATA_COMPGEN(0x00519cd4, doEventCityOfTheDeadSomeLiches, "{City of the Dead}\n\nSome Liches living here are willing to join your army "
                    "for a price.  Do you want to recruit Liches?"),
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
                    DATA_COMPGEN(0x00519d4c, doEventDragonCityTheDragonCityHas, "{Dragon City}\n\nThe Dragon city has no Dragons willing to join you this "
                    "week.  Perhaps a Dragon will become available next week."),
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
                    DATA_COMPGEN(0x00519dcc, doEventDragonCityYouStandBeforeThe, "{Dragon City}\n\nYou stand before the Dragon City, a place off-limits to mere "
                    "humans.  Do you wish to violate this rule and challenge the Dragons to a "
                    "fight?"),
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
                    DATA_COMPGEN(0x00519e68, doEventDragonCityHavingDefeatedTheDragon, "{Dragon City}\n\nHaving defeated the Dragon champions, the city's leaders "
                    "agree to supply some Dragons to your army for a price.  Do you wish to "
                    "recruit Dragons?"),
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
                    DATA_COMPGEN(0x00519f08, doEventDragonCityTheDragonCityIs, "{Dragon City}\n\nThe Dragon city is willing to offer some Dragons for your "
                    "army for a price.  Do you wish to recruit Dragons?"),
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
                EventWindow(EVENT_TEXT_WAGON_EMPTY, NORMAL_DIALOG_INFO, DATA_COMPGEN(0x005196f8, doEventEmptyString18, ""), -1, 0, -1, 0, -1);
            } else {
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                EventWindow(EVENT_TEXT_WAGON_RECRUIT, NORMAL_DIALOG_CONFIRM, DATA_COMPGEN(0x005196fc, doEventEmptyString19, ""), -1, 0, -1, 0, -1);
                if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES)
                    RecruitEvent(eventHero2, CREATURE_ROGUE, cell);
            }
            break;

        case MAP_OBJECT_DESERT_TENT:
            if (!cell->m_objectMetadata) {
                EventWindow(EVENT_TEXT_DESERT_TENT_EMPTY, NORMAL_DIALOG_INFO, DATA_COMPGEN(0x00519f84, doEventEmptyString20, ""), -1, 0, -1, 0, -1);
            } else {
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                EventWindow(
                    EVENT_TEXT_DESERT_TENT_RECRUIT,
                    NORMAL_DIALOG_CONFIRM,
                    DATA_COMPGEN(0x00519f88, doEventEmptyString21, ""),
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
                EventWindow(EVENT_TEXT_OBELISK_REWARD, NORMAL_DIALOG_INFO, DATA_COMPGEN(0x00519f8c, doEventEmptyString22, ""), -1, 0, -1, 0, -1);
                ViewPuzzle();
            } else {
                EventWindow(EVENT_TEXT_OBELISK_VISITED, NORMAL_DIALOG_INFO, DATA_COMPGEN(0x00519f90, doEventEmptyString23, ""), -1, 0, -1, 0, -1);
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
                            DATA_COMPGEN(0x0051a038, doEventTreeOfKnowledgeUponYourApproach, "{Tree of Knowledge}\n\nUpon your approach, the tree opens its eyes in "
                            "delight.  \"Ahh, an adventurer!  Allow me to teach you a little of "
                            "what I have learned over the ages.\""),
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
                                DATA_COMPGEN(0x0051a1cc, doEventTreeOfKnowledgeTearsBrimIn, "{Tree of Knowledge}\n\nTears brim in the eyes of the tree.  \"I "
                                "need 2000 gold.\" it whispers. (sniff)  \"Well, come back when "
                                "you can pay me.\""),
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
                                DATA_COMPGEN(0x0051a0e4, doEventTreeOfKnowledgeUponYourApproach2, "{Tree of Knowledge}\n\nUpon your approach, the tree opens its "
                                "eyes in delight.  \"Ahh, an adventurer! I will be happy to teach "
                                "you a little of what I have learned over the ages for a mere 2000 "
                                "gold.\"  (Just bury it around my roots.)"),
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
                                DATA_COMPGEN(0x0051a304, doEventTreeOfKnowledgeTearsBrimIn2, "{Tree of Knowledge}\n\nTears brim in the eyes of the tree.  \"I "
                                "need 10 gems.\" it whispers. (sniff)  \"Well, come back when you "
                                "can pay me.\""),
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
                                DATA_COMPGEN(0x0051a258, doEventTreeOfKnowledgeAhhAnAdventurer, "{Tree of Knowledge}\n\nAhh, an adventurer! I will be happy to "
                                "teach you a little of what I have learned over the ages for a "
                                "mere 10 gems.\"  (Just bury it around my roots.)"),
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
                    DATA_COMPGEN(0x00519f9c, doEventTreeOfKnowledgeUponYourApproach3, "{Tree of Knowledge}\n\nUpon your approach, the tree opens its eyes in "
                    "delight.  \"It is good to see you, my student.  I hope my teachings have "
                    "helped you.\""),
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
            EventWindow(EVENT_TEXT_ORACLE, NORMAL_DIALOG_INFO, DATA_COMPGEN(0x00519f94, doEventEmptyString24, ""), -1, 0, -1, 0, -1);
            oracleWindow = new heroWindow(0, 0, DATA_COMPGEN(0x0051a394, doEventThiefwinBin, "thiefwin.bin"));
            if (oracleWindow == NULL)
                MemError();
            SetWinText(oracleWindow, ORACLE_WINDOW_TEXT_ID);
            gpTownManager->SetupThievesGuild(oracleWindow, ORACLE_THIEVES_GUILD_RANK);
            strcpy(gText, DATA_COMPGEN(0x0051a3a4, doEventShrinePlayerRankings, "Shrine - Player Rankings"));
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
                DATA_COMPGEN(0x0051a470, doEventSS, "%s'%s'.  "),
                DATA_COMPGEN(0x0051a3c0, doEventShrineOfThe1stCircleYou, "{Shrine of the 1st Circle}\n\nYou come across a small shrine attended by a group "
                "of novice acolytes.  In exchange for your protection, they agree to teach you a "
                "simple spell - "),
                gSpellNames[cell->m_objectMetadata - 1]
            );
            goto shrineSpell;

        case MAP_OBJECT_SHRINE_SECOND_CIRCLE:
            eventValue1 = cell->m_objectMetadata - 1;
            sprintf(
                gText,
                DATA_COMPGEN(0x0051a524, doEventSS2, "%s'%s'.  "),
                DATA_COMPGEN(0x0051a47c, doEventShrineOfThe2ndCircleYou, "{Shrine of the 2nd Circle}\n\nYou come across an ornate shrine attended by a "
                "group of rotund friars.  In exchange for your protection, they agree to teach you "
                "a spell - "),
                gSpellNames[cell->m_objectMetadata - 1]
            );
            goto shrineSpell;

        case MAP_OBJECT_SHRINE_THIRD_CIRCLE:
            eventValue1 = cell->m_objectMetadata - 1;
            sprintf(
                gText,
                DATA_COMPGEN(0x0051b7f4, doEventSS3, "%s'%s'.  "),
                DATA_COMPGEN(0x0051a530, doEventShrineOfThe3rdCircleYou, "{Shrine of the 3rd Circle}\n\nYou come across a lavish shrine attended by a group "
                "of high priests.  In exchange for your protection, they agree to teach you a "
                "sophisticated spell - "),
                gSpellNames[cell->m_objectMetadata - 1]
            );
        shrineSpell:
            if (!eventHero2->HasArtifact(ARTIFACT_MAGIC_BOOK)) {
                strcat(gText, DATA_COMPGEN(0x0051a658, doEventUnfortunatelyYouHaveNoMagicBook, "Unfortunately, you have no Magic Book to record the spell with."));
                EventWindow(-1, NORMAL_DIALOG_INFO, gText, -1, 0, -1, 0, -1);
            } else if (static_cast<i32>(
                           eventHero2->m_secondarySkills[IDX(HERO_SKILL_WISDOM)]
                       )
                           + SHRINE_WISDOM_BONUS
                       < IDX(gsSpellInfo[eventValue1].level)) {
                strcat(
                    gText,
                    DATA_COMPGEN(0x0051a5f0, doEventUnfortunatelyYouDoNotHaveThe, "Unfortunately, you do not have the wisdom to understand the spell, and you "
                    "are unable to learn it.  ")
                );
                EventWindow(-1, NORMAL_DIALOG_INFO, gText, -1, 0, -1, 0, -1);
            } else {
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
                    if ((gpGame->m_worldMap.Row(teleportY) + teleportX3)->m_triggerType
                            == (eventType_g | MAP_TRIGGER_ACTION_FLAG)
                        && (gpGame->m_worldMap.Row(teleportY) + teleportX3)->m_objectIndex
                               == cell->m_objectIndex
                        && (eventType_g == MAP_OBJECT_STONE_LITHS ? STONE_LITHS_MIN_DISTANCE
                                                                  : WHIRLPOOL_MIN_DISTANCE)
                               < abs(teleportY - y) + abs(teleportX3 - x)) {
                        teleportCount++;
                    }
                }
            }
            if (teleportCount > 0) {
                if (teleportCount > 1)
                    teleportCount = Random(1, teleportCount);
                for (teleportY = 0; teleportY < MAP_HEIGHT; teleportY++) {
                    for (teleportX3 = 0; teleportX3 < MAP_WIDTH; teleportX3++) {
                        if ((gpGame->m_worldMap.Row(teleportY) + teleportX3)->m_triggerType
                                == (eventType_g | MAP_TRIGGER_ACTION_FLAG)
                            && (gpGame->m_worldMap.Row(teleportY) + teleportX3)->m_objectIndex
                                   == cell->m_objectIndex
                            && (teleportX3 != x || teleportY != y)
                            && (eventType_g == MAP_OBJECT_STONE_LITHS ? STONE_LITHS_MIN_DISTANCE
                                                                      : WHIRLPOOL_MIN_DISTANCE)
                                   < abs(teleportY - y) + abs(teleportX3 - x)
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
            artifact8 = static_cast<ArtifactType>(cell->m_objectIndex >> 1);
            guardedMonster5 =
                static_cast<CreatureType>(cell->m_objectMetadata & ARTIFACT_EVENT_MONSTER_MASK);
            if (eventHero2->NumArtifacts() == EVENT_ARTIFACT_CAPACITY) {
                NormalDialog(
                    DATA_COMPGEN(0x0051a698, doEventYouCannotPickUpThisArtifact, "You cannot pick up this artifact, you already have a full load!"),
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
                    DATA_COMPGEN(0x0051a6d8, doEventTheFabledGoldenBowOfThe, "The fabled golden bow of the elves lies here in the dust.  You take it and "
                    "journey back to the elven towns.  They shower you with their graciousness and "
                    "the king promises that his people will aid you whenever you seek help.")
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
                            DATA_COMPGEN(0x0051aaa8, doEventArtifactALeprechaunOffersYouThe, "{Artifact}\n\nA leprechaun offers you the %s for the small price of "
                            "2000 gold.  Do you wish to buy this artifact?"),
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
                                DATA_COMPGEN(0x0051ac6c, doEventYouTryToPayTheLeprechaun, "You try to pay the leprechaun, but realize that you can't afford "
                                "it.  The leprechaun stamps his foot and ignores you."),
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
                                DATA_COMPGEN(0x0051ace4, doEventInsultedByYourRefusalOfHis, "Insulted by your refusal of his generous offer, the leprechaun "
                                "stamps his foot and ignores you."),
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
                            DATA_COMPGEN(0x0051a95c, doEventArtifactYouVeFoundTheHumble, "{Artifact}\n\nYou've found the humble dwelling of a withered hermit.  "
                            "The hermit tells you that he is willing to give the %s to the first "
                            "wise person he meets."),
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
                            DATA_COMPGEN(0x0051a9fc, doEventArtifactYouVeComeAcrossThe, "{Artifact}\n\nYou've come across the spartan quarters of a retired "
                            "soldier.  The soldier tells you that he is willing to pass on the %s "
                            "to the first true leader he meets."),
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
                            DATA_COMPGEN(0x0051abf0, doEventArtifactALeprechaunOffersYouThe2, "{Artifact}\n\nA leprechaun offers you the %s for the small price of "
                            "2500 gold and 3 %s.  Do you wish to buy this artifact?"),
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
                                DATA_COMPGEN(0x0051ab18, doEventLeprechaunPaymentRefusal, "You try to pay the leprechaun, but realize that you can't afford "
                                "it.  The leprechaun stamps his foot and ignores you."),
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
                                DATA_COMPGEN(0x0051ab90, doEventLeprechaunInsultResponse, "Insulted by your refusal of his generous offer, the leprechaun "
                                "stamps his foot and ignores you."),
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
                            DATA_COMPGEN(0x0051ad44, doEventArtifactALeprechaunOffersYouThe3, "{Artifact}\n\nA leprechaun offers you the %s for the small price of "
                            "3000 gold and 5 %s.  Do you wish to buy this artifact?"),
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
                                DATA_COMPGEN(0x0051adc0, doEventYouTryToPayTheLeprechaun2, "You try to pay the leprechaun, but realize that you can't afford "
                                "it.  The leprechaun stamps his foot and ignores you."),
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
                                DATA_COMPGEN(0x0051ae38, doEventInsultedByYourRefusalOfHis2, "Insulted by your refusal of his generous offer, the leprechaun "
                                "stamps his foot and ignores you."),
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
                        DATA_COMPGEN(0x0051a7b8, doEventArtifactYouComeUponAnAncient, "{Artifact}\n\nYou come upon an ancient artifact.  As you reach for it, a "
                        "pack of Rogues leap out of the brush to guard their stolen loot."),
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
                        DATA_COMPGEN(0x0051a840, doEventArtifactThroughAClearingYouObserve, "{Artifact}\n\nThrough a clearing you observe an ancient artifact.  "
                        "Unfortunately, it's guarded by a nearby %s.  Do you want to fight the %s "
                        "for the artifact?"),
                        gArmyNames[IDX(guardedMonster5)],
                        gArmyNames[IDX(guardedMonster5)]
                    );
                    NormalDialog(gText, NORMAL_DIALOG_CONFIRM, -1, -1, -1, 0, -1, 0, -1, 0);
                    if (gpWindowManager->m_dialogResult != MONSTER_DIALOG_YES) {
                        NormalDialog(
                            DATA_COMPGEN(0x0051a904, doEventDiscretionIsTheBetterPartOf, "Discretion is the better part of valor, and you decide to avoid this "
                            "fight for today."),
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
                    DATA_COMPGEN(0x0051a8dc, doEventVictoriousYouTakeYourPrizeThe, "Victorious, you take your prize, the %s"),
                    gArtifactNames[IDX(artifact8)]
                );
                NormalDialog(
                    gText,
                    NORMAL_DIALOG_INFO,
                    -1,
                    -1,
                    MAP_EVENT_REWARD_ARTIFACT,
                    cell->m_objectIndex >> 1,
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
                DATA_COMPGEN(0x0051ae98, doEventDaemonCaveTheEntranceToThe, "{Daemon Cave}\n\nThe entrance to the cave is dark, and a foul, sulfurous smell "
                "issues from the cave mouth.  Will you enter?"),
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
                    DATA_COMPGEN(0x0051af14, doEventExceptForEvidenceOfATerrible, "Except for evidence of a terrible battle, the cave is empty."),
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
                DATA_COMPGEN(0x0051af54, doEventYouFindAPowerfulAndGrotesque, "You find a powerful and grotesque Demon in the cave.  \"Today,\" it rasps, \"you "
                "will fight and surely die.  But I will give you a choice of deaths.  You may "
                "fight me, or you may fight my servants.  Do you prefer to fight my servants?\"")
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
                        DATA_COMPGEN(0x0051b040, doEventUponDefeatingTheDaemonSServants, "Upon defeating the daemon's servants, you find a hidden cache with 2500 "
                        "gold."),
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
                        DATA_COMPGEN(0x0051b090, doEventTheDemonScreamsItsChallengeAnd, "The Demon screams its challenge and attacks!  After a short, desperate "
                        "battle, you slay the monster and receive 1,000 experience points."),
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
                        DATA_COMPGEN(0x0051b11c, doEventTheDemonScreamsItsChallengeAnd2, "The Demon screams its challenge and attacks!  After a short, desperate "
                        "battle, you slay the monster and find the %s in the back of the cave."),
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
                        DATA_COMPGEN(0x0051b1ac, doEventTheDemonScreamsItsChallengeAnd3, "The Demon screams its challenge and attacks!  After a short, desperate "
                        "battle, you slay the monster and receive 1,000 experience points and "
                        "2,500 gold."),
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
                        DATA_COMPGEN(0x0051b244, doEventTheDemonLeapsUponYouAnd, "The Demon leaps upon you and has its claws at your throat before you can "
                        "even draw your sword.  \"Your life is mine,\" it says.  \"I will sell it "
                        "back to you for 2,500 gold.\""),
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
                                DATA_COMPGEN(0x0051b2f0, doEventSeeingThatYouDoNotHave, "Seeing that you do not have 2500 gold, the demon slashes you with "
                                "its claws, and the last thing you see is a red haze."),
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
                DATA_COMPGEN(0x0051b368, doEventShipwreckTheRottingHulkOfA, "{Shipwreck}\n\nThe rotting hulk of a great pirate ship creaks eerily as it is "
                "pushed against the rocks.  Do you wish to search the shipwreck?"),
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
                            DATA_COMPGEN(0x0051b3f4, doEventUponDefeatingTheGhostsYouSpend, "Upon defeating the Ghosts you spend several hours sifting through the "
                            "debris and find nothing.  Such a despicable act reduces your army's "
                            "morale."),
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
                                DATA_COMPGEN(0x0051b488, doEventUponDefeatingTheGhostsYouSift, "Upon defeating the Ghosts you sift through the debris and find "
                                "something!"),
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
            EventWindow(EVENT_TEXT_GRAVEYARD_PROMPT, NORMAL_DIALOG_CONFIRM, DATA_COMPGEN(0x00519f98, doEventEmptyString25, ""), -1, 0, -1, 0, -1);
            if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
                switch (cell->m_objectMetadata) {
                    case SKELETON_EMPTY:
                        EventWindow(
                            EVENT_TEXT_GRAVEYARD_EMPTY,
                            NORMAL_DIALOG_INFO,
                            DATA_COMPGEN(0x0051a390, doEventEmptyString26, ""),
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
                DATA_COMPGEN(0x0051b4dc, doEventDerelictShipTheRottingHulkOf, "{Derelict Ship}\n\nThe rotting hulk of a great pirate ship creaks eerily as it is "
                "pushed against the rocks.  Do you wish to search the ship?"),
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
                            DATA_COMPGEN(0x0051b568, doEventUponDefeatingTheSkeletonsYouSpend, "Upon defeating the Skeletons you spend several hours sifting through "
                            "the debris and find nothing.  Such a despicable act reduces your "
                            "army's morale."),
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
                                DATA_COMPGEN(0x0051b600, doEventUponDefeatingTheSkeletonsYouSift, "Upon defeating the Skeletons you sift through the debris and find "
                                "something!"),
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
                DATA_COMPGEN(0x0051b650, doEventYouComeUponThePyramidOf, "You come upon the pyramid of a great and ancient king.  You are tempted to search "
                "it for treasure, but all the old stories warn of fearful curses and undead "
                "guardians.  Will you search?"),
                -1,
                0,
                -1,
                0,
                -1
            );
            if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
                if (!cell->m_objectMetadata) {
                    NormalDialog(
                        DATA_COMPGEN(0x0051b70c, doEventYouComeUponThePyramidOf2, "You come upon the pyramid of a great and ancient king.  Routine "
                        "exploration reveals that the pyramid is completely empty."),
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
                        eventHero2->m_eventFlags = eventHero2->m_eventFlags | HERO_EVENT_PYRAMID;
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
                            DATA_COMPGEN(0x0051a5e4, doEventNamedMessageFormat, "%s'%s'.  "),
                            DATA_COMPGEN(0x0051b788, doEventUponDefeatingTheMonstersYouDecipher, "Upon defeating the monsters, you decipher an ancient glyph on the "
                            "wall, telling the secret of the spell - "),
                            gSpellNames[cell->m_objectMetadata - 1]
                        );
                        if (!eventHero2->HasArtifact(ARTIFACT_MAGIC_BOOK)) {
                            strcat(
                                eventText,
                                DATA_COMPGEN(0x0051b800, doEventNoMagicBookFollowup, "  Unfortunately, you have no Magic Book to record the spell with.")
                            );
                            EventWindow(-1, NORMAL_DIALOG_INFO, eventText, -1, 0, -1, 0, -1);
                        } else if (eventHero2->m_secondarySkills[IDX(HERO_SKILL_WISDOM)]
                                   < HERO_SKILL_LEVEL_EXPERT) {
                            strcat(
                                eventText,
                                DATA_COMPGEN(0x0051b844, doEventUnfortunatelyYouDoNotHaveThe2, "  Unfortunately, you do not have the wisdom to understand the "
                                "spell, and you are unable to learn it.  ")
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
                DATA_COMPGEN(0x0051b8ac, doEventYouComeUponAnAbandonedGold, "You come upon an abandoned gold mine.  The mine appears to be haunted.  Do you "
                "wish to enter?"),
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
                        DATA_COMPGEN(0x0051b90c, doEventYouBeatTheGhostsAndAre, "You beat the Ghosts and are able to restore the mine to production."),
                        -1,
                        0,
                        -1,
                        0,
                        -1
                    );
                    // Retail conversion topology for the two terrain variants and center tile.
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
    WaitEndSample(playedSample3, -1);
    CheckEndGame(END_GAME_FORCE_NONE, false);
}

VA(0x004ae00b, 0x9f7)
void advManager::EraseObj(class mapCell* cell, i32 x, i32 y) {
    i32 erased_a = 0;
    i32 frame_k = NO_FRAME;
    i8 isWide_d = 0;
    mapCell* cells_h[NEIGHBOR_COUNT];
    mapCellExtra* extras_b[NEIGHBOR_COUNT];
    i32 i_e;
    i32 cellX_b[ERASE_COORDINATE_COUNT];
    i32 cellY_e[ERASE_COORDINATE_COUNT];
    mapCell* currentCell_k;
    mapCellExtra* extra_i;
    volatile i32 changed_i;

    for (i_e = 0; i_e < NEIGHBOR_COUNT; i_e++) {
        cells_h[i_e] = NULL;
        extras_b[i_e] = NULL;
    }

    erased_a = 1;
    // Retail object-to-footprint frame topology. The frame ids are payloads from the
    // corresponding ICN tilesets, not independent gameplay constants.
    // NOLINTBEGIN(readability-magic-numbers)
    if (cell->m_objectTileset == TILESET_OBJNARTI)
        frame_k = cell->m_objectIndex - 1;
    if (cell->m_objectTileset == TILESET_X_LOC3)
        frame_k = cell->m_objectIndex - 1;
    if (cell->m_objectTileset == TILESET_X_LOC2 && cell->m_objectIndex == 9) {
        frame_k = 9;
        isWide_d = 1;
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

    for (i_e = 0; i_e < NEIGHBOR_COUNT; i_e++) {
        if (isWide_d)
            frame_k--;
        else if (i_e > 0)
            break;

        if (frame_k != NO_FRAME) {
            if (isWide_d) {
                if (frame_k > 6)
                    cellX_b[0] = frame_k - 9 + x;
                else
                    cellX_b[0] = frame_k - 6 + x;
                if (frame_k > 6)
                    cellY_e[0] = y;
                else
                    cellY_e[0] = y - 1;
            } else {
                cellX_b[0] = x - 1;
                cellY_e[0] = y;
            }
            // NOLINTEND(readability-magic-numbers)

            if (cellX_b[0] >= 0) {
                if (cellY_e[0] >= 0) {
                    cells_h[i_e] = gpGame->m_worldMap.Row(cellY_e[0]) + cellX_b[0];
                    if (i_e > 1) {
                        cells_h[i_e]->m_overlayTileset = TILESET_NONE;
                        cells_h[i_e]->m_overlayIndex = EMPTY_INDEX;
                    } else if (cells_h[i_e]->m_objectIndex != EMPTY_INDEX) {
                        if (cells_h[i_e]->m_objectIndex == frame_k
                            && cells_h[i_e]->m_objectTileset == cell->m_objectTileset) {
                            cells_h[i_e]->m_objectIndex = 0;
                            cells_h[i_e]->m_objectTileset = TILESET_DUMMY;
                            cells_h[i_e]->m_animatedObject = 0;
                        }

                        if (cells_h[i_e]->m_extraIndex
                            && m_mapData->Extra(cells_h[i_e]->m_extraIndex)->objectIndex
                                   != EMPTY_INDEX)
                            extras_b[i_e] = m_mapData->Extra(cells_h[i_e]->m_extraIndex);
                        else
                            extras_b[i_e] = NULL;

                        while (extras_b[i_e]) {
                            if (extras_b[i_e]->objectIndex == frame_k
                                && extras_b[i_e]->objectTileset == cell->m_objectTileset) {
                                extras_b[i_e]->objectIndex = 0;
                                extras_b[i_e]->objectTileset = TILESET_DUMMY;
                                extras_b[i_e]->animatedObject = 0;
                            }

                            if (extras_b[i_e]->nextIndex
                                && m_mapData->Extra(extras_b[i_e]->nextIndex)->objectIndex
                                       != EMPTY_INDEX)
                                extras_b[i_e] = m_mapData->Extra(extras_b[i_e]->nextIndex);
                            else
                                extras_b[i_e] = NULL;
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

    for (i_e = 0; i_e < CELL_COUNT; i_e++) {
        currentCell_k = i_e == 0 ? cell : cells_h[i_e - 1];
        if (!currentCell_k)
            continue;
        if (currentCell_k->m_objectTileset != TILESET_DUMMY)
            continue;

        if (currentCell_k->m_extraIndex
            && m_mapData->Extra(currentCell_k->m_extraIndex)->objectIndex != EMPTY_INDEX)
            extra_i = m_mapData->Extra(currentCell_k->m_extraIndex);
        else
            continue;

        if (extra_i->objectTileset == TILESET_DUMMY || extra_i->objectIndex == EMPTY_INDEX)
            continue;

        currentCell_k->m_objectIndex = extra_i->objectIndex;
        currentCell_k->m_objectTileset = extra_i->objectTileset;
        currentCell_k->m_animatedObject = extra_i->animatedObject;
        currentCell_k->m_objectLayerBit0 = extra_i->objectLayerBit0;
        currentCell_k->m_objectLayerBit1 = extra_i->objectLayerBit1;
        extra_i->objectIndex = 0;
        extra_i->objectTileset = TILESET_DUMMY;
        extra_i->animatedObject = 0;
    }

    for (i_e = 0; i_e < CELL_COUNT; i_e++) {
        currentCell_k = i_e == 0 ? cell : cells_h[i_e - 1];
        if (!currentCell_k)
            continue;

        if (currentCell_k->m_objectTileset != TILESET_DUMMY
            && currentCell_k->m_objectIndex != EMPTY_INDEX && !currentCell_k->m_objectLayerBit1)
            goto cellDone;

        if (currentCell_k->m_extraIndex
            && m_mapData->Extra(currentCell_k->m_extraIndex)->objectIndex != EMPTY_INDEX)
            extra_i = m_mapData->Extra(currentCell_k->m_extraIndex);
        else
            extra_i = NULL;

        while (extra_i) {
            if (extra_i->objectTileset != TILESET_DUMMY && extra_i->objectIndex != EMPTY_INDEX
                && !extra_i->objectLayerBit1)
                goto cellDone;

            if (extra_i->nextIndex
                && m_mapData->Extra(extra_i->nextIndex)->objectIndex != EMPTY_INDEX)
                extra_i = m_mapData->Extra(extra_i->nextIndex);
            else
                extra_i = NULL;
        }

        currentCell_k->m_flags |= IDX(MAP_CELL_OBJECT_SHADOW_ONLY);
    cellDone:
        changed_i = 0;
    }

    SendMapChange(MAP_CHANGE_ERASE_OBJECT, 0, x, y, MAP_CHANGE_VALUE, 0, 0);
    SetEnvironmentOrigin(m_mapOriginX + ENVIRONMENT_BORDER, m_mapOriginY + ENVIRONMENT_BORDER, 1);
    gpGame->SetupAdjacentMons();
}

VA(0x004aea02, 0x90)
void advManager::HeroSwap(hero* firstHero, hero* secondHero) {
    swapManager* swapWindow = new swapManager(firstHero, secondHero);
    if (swapWindow == NULL)
        MemError();
    gpExec->DoDialog(swapWindow);
    delete swapWindow;
    RedrawAdvScreen(1, 0);
}

VA(0x004aea92, 0x12f)
i32 advManager::BarrierEvent(mapCell* cell, hero*) {
    SAMPLE2 eventSample = NULL_SAMPLE2;
    i32 color = cell->m_objectMetadata;
    color &= COLOR_MASK;
    i32 passwordIndex = cell->m_objectMetadata;
    passwordIndex >>= PASSWORD_SHIFT;
    char response[INPUT_BUFFER_SIZE];

    sprintf(
        gText,
        DATA_COMPGEN(0x0051b950, barrierEventAMagicalSBarrierStandsTall, "A magical %s barrier stands tall before you, blocking your way.  "
        "Runes on the arch read, \"Speak the key and you may pass.\""),
        xBarrierColor[color]
    );
    GetDataEntry(gText, response, INPUT_LENGTH, NULL, 0, 1);
    if (StrEqNoCase(response, xPasswordStrings[passwordIndex])
        && (gpCurPlayer->m_barrierTents & (1 << color))) {
        EventSound(cell->m_triggerType & MAP_TRIGGER_TYPE_MASK, color, &eventSample);
        NormalDialog(
            DATA_COMPGEN(0x0051b9cc, barrierEventAsYouSpeakTheMagicWord, "As you speak the magic word, the glowing barrier dissolves into "
            "nothingness."),
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
            DATA_COMPGEN(0x0051ba1c, barrierEventYouSpeakAndNothingHappens, "You speak, and nothing happens."),
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

VA(0x004aebc1, 0x99)
i8 StrEqNoCase(char* firstString, char* secondString) {
    char* firstPosition = firstString;
    char* secondPos = secondString;
    i32 characterCount = 0;
    char firstUpper;
    char secondUpper;

    while (1) {
        characterCount++;
        if (characterCount == SITE_STRING_LIMIT)
            return 1;
        firstUpper = static_cast<char>(toupper(static_cast<i32>(*firstPosition)));
        secondUpper = static_cast<char>(toupper(static_cast<i32>(*secondPos)));
        if (firstUpper == secondUpper) {
            if (firstUpper == 0)
                return 1;
            firstPosition++;
            secondPos++;
        } else {
            return 0;
        }
    }
}

VA(0x004aec5a, 0xde)
void advManager::PasswordEvent(mapCell* cell, hero*) {
    SAMPLE2 eventSample = NULL_SAMPLE2;
    i32 color = cell->m_objectMetadata;
    color &= COLOR_MASK;
    i32 passwordIndex = cell->m_objectMetadata;
    passwordIndex >>= PASSWORD_SHIFT;

    EventSound(cell->m_triggerType & MAP_TRIGGER_TYPE_MASK, color, &eventSample);
    sprintf(
        gText,
        DATA_COMPGEN(0x0051ba3c, passwordEventYouEnterTheTentAndSee, "You enter the tent and see an old woman gazing into a magic gem.  She "
        "looks up and says, \"In my travels, I have learned much in the way of "
        "arcane magic.  A great oracle taught me his skill.  I have the answer "
        "you seek.  The %s keyword is '%s'.\""),
        xBarrierColor[color],
        xPasswordStrings[passwordIndex]
    );
    NormalDialog(gText, NORMAL_DIALOG_INFO, -1, -1, -1, 0, -1, 0, -1, 0);
    gpCurPlayer->m_barrierTents |= 1 << color;
}

VA(0x004aed38, 0x6fe)
void advManager::GenericSiteEvent(mapCell* cell, hero* eventHero) {
    i32 currentSiteType;
    mapCell* currentCell5;
    i32 index3;
    i32 siteLevel6;
    i32 primaryStat15;
    H2_ENUM_STORAGE(GenericSiteType, i32) siteType2;
    i32 mapY9;
    i32 mapX37;
    i32 unusedSite;
    SAMPLE2 eventSample5;
    i32 cursedArtifactCount9;
    H2_ENUM_STORAGE(StableVisitResult, i32) unusedOne18;
    H2_ENUM_STORAGE(StableVisitResult, i8) stableResult26;
    H2_ENUM_STORAGE(StableVisitResult, i32) unusedTwo6;
    CreatureType creatureType;
    i32 experience11;
    i32 oldQuantity4;

    cursedArtifactCount9 = 0;
    eventSample5 = NULL_SAMPLE2;
    siteType2 = cell->m_objectMetadata;
    siteType2 &= GENERIC_SITE_TYPE_MASK;
    siteLevel6 = cell->m_objectMetadata;
    siteLevel6 >>= GENERIC_SITE_LEVEL_SHIFT;

    switch (siteType2) {
        case GENERIC_SITE_ALCHEMIST_TOWER:
            for (index3 = 0; index3 < HERO_ARTIFACT_SLOT_COUNT; index3++) {
                if (IsCursedItem(eventHero->m_artifacts[index3]))
                    cursedArtifactCount9++;
            }
            if (cursedArtifactCount9 != 0) {
                EventSound(
                    cell->m_triggerType & MAP_TRIGGER_TYPE_MASK, IDX(siteType2), &eventSample5
                );
                if (cursedArtifactCount9 == 1) {
                    sprintf(
                        gText,
                        DATA_COMPGEN(0x0051bb34, genericSiteEventAsYouEnterTheAlchemistS, "As you enter the Alchemist's Tower, a hobbled, graying man "
                        "in a brown cloak makes his way towards you.  He checks your "
                        "pack, and sees that you have 1 cursed item.  For 750 gold, "
                        "the alchemist will remove it for you.  Do you pay?")
                    );
                } else {
                    sprintf(
                        gText,
                        DATA_COMPGEN(0x0051bc1c, genericSiteEventAsYouEnterTheAlchemistS2, "As you enter the Alchemist's Tower, a hobbled, graying man "
                        "in a brown cloak makes his way towards you.  He checks your "
                        "pack, and sees that you have %d cursed items.  For 750 gold, "
                        " the alchemist will remove them for you.  Do you pay?"),
                        cursedArtifactCount9
                    );
                }
                NormalDialog(gText, NORMAL_DIALOG_CONFIRM, -1, -1, -1, 0, -1, 0, -1, 0);
                if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
                    if (gpCurPlayer->m_resources[IDX(RES_GOLD)] >= SITE_ALCHEMIST_COST) {
                        for (index3 = 0; index3 < HERO_ARTIFACT_SLOT_COUNT; index3++) {
                            if (IsCursedItem(eventHero->m_artifacts[index3]))
                                eventHero->m_artifacts[index3] = ARTIFACT_NONE;
                        }
                        gpCurPlayer->m_resources[IDX(RES_GOLD)] -= SITE_ALCHEMIST_COST;
                    } else {
                        NormalDialog(
                            DATA_COMPGEN(0x0051bd08, genericSiteEventYouHearAVoiceFromBehind, "You hear a voice from behind the locked door, \"You "
                            "don't have enough gold to pay for my services.\""),
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
                    DATA_COMPGEN(0x0051bd6c, genericSiteEventYouHearAVoiceFromHigh, "You hear a voice from high above in the tower, \"Go away! I "
                    "can't help you!\""),
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
                    DATA_COMPGEN(0x0051bdb8, genericSiteEventTheArenaGuardsTurnYouAway, "The Arena guards turn you away."),
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
                    cell->m_triggerType & MAP_TRIGGER_TYPE_MASK, IDX(siteType2), &eventSample5
                );
                eventHero->m_eventFlags = eventHero->m_eventFlags | HERO_EVENT_ARENA;
                primaryStat15 = DoArenaDialog();
                eventHero->m_primaryStats[primaryStat15]++;
            }
            break;

        case GENERIC_SITE_MERMAID:
            if (HAS(eventHero->m_eventFlags, HERO_EVENT_MERMAID)) {
                NormalDialog(
                    DATA_COMPGEN(0x0051bdd8, genericSiteEventTheMermaidsSilentlyEnticeYouTo, "The mermaids silently entice you to return later and be "
                    "blessed again."),
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
                    cell->m_triggerType & MAP_TRIGGER_TYPE_MASK, IDX(siteType2), &eventSample5
                );
                eventHero->m_eventFlags = eventHero->m_eventFlags | HERO_EVENT_MERMAID;
                eventHero->m_luck = eventHero->m_luck + 1;
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    DATA_COMPGEN(0x0051be20, genericSiteEventTheMagicalSoothingBeautyOfThe, "The magical, soothing beauty of the Mermaids reaches you and "
                    "your crew.  Just for a moment, you forget your worries and "
                    "bask in the beauty of the moment.  The mermaids charms bless "
                    "you with increased luck for your next combat."),
                    SITE_MERMAID_WINDOW_ICON,
                    0,
                    -1,
                    0,
                    -1
                );
            }
            break;

        case GENERIC_SITE_HUT_OF_MAGI:
            EventSound(cell->m_triggerType & MAP_TRIGGER_TYPE_MASK, IDX(siteType2), &eventSample5);
            NormalDialog(
                DATA_COMPGEN(0x0051bf04, genericSiteEventYouEnterARicketyHutAnd, "You enter a rickety hut and talk to the magician who lives there.  "
                "He tells you of places near and far which may aid you in your "
                "journeys."),
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
                for (mapY9 = 0; OD_STEER(mapY9) < MAP_HEIGHT; mapY9++) {
                    currentCell5 = gpGame->m_worldMap.Row(mapY9) + mapX37;
                    currentSiteType = currentCell5->m_objectMetadata;
                    currentSiteType &= COLOR_MASK;
                    if ((currentCell5->m_triggerType & MAP_TRIGGER_TYPE_MASK)
                            == MAP_OBJECT_EXPANSION_OBJECT
                        && currentSiteType == SITE_HUT_COLOR) {
                        gpGame->SetVisibility(mapX37, mapY9, giCurPlayer, SITE_VISIBILITY_RADIUS);
                        CompleteDraw(0);
                        UpdateScreen(0, 0);
                    }
                }
            }
            break;

        case GENERIC_SITE_EYE_OF_MAGI:
            NormalDialog(
                DATA_COMPGEN(0x0051bf90, genericSiteEventThisEyeSeemsToBeIntently, "This eye seems to be intently studying its surroundings."),
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
                    DATA_COMPGEN(0x0051bfcc, genericSiteEventYouHaveYourCrewStopUp, "You have your crew stop up their ears with wax before the "
                    "sirens' eerie song has any chance of luring them to a watery "
                    "grave."),
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
                for (index3 = 0; index3 < ARMY_GROUP_SLOT_COUNT; index3++) {
                    creatureType = eventHero->m_army.m_creatureTypes[index3];
                    if (creatureType != CREATURE_NONE) {
                        oldQuantity4 = eventHero->m_army.m_quantities[index3];
                        if (oldQuantity4 > 1) {
                            eventHero->m_army.m_quantities[index3] =
                                static_cast<i16>(oldQuantity4 * GENERIC_SITE_SIREN_ARMY_REMAINDER);
                            experience11 += (oldQuantity4 - eventHero->m_army.m_quantities[index3])
                                            * gMonsterDatabase[IDX(creatureType)].hitPoints;
                        }
                    }
                }
                if (experience11 != 0) {
                    EventSound(
                        cell->m_triggerType & MAP_TRIGGER_TYPE_MASK,
                        IDX(siteType2),
                        &eventSample5
                    );
                    sprintf(
                        gText,
                        DATA_COMPGEN(0x0051c04c, genericSiteEventAnEerieWailingSongEmanatesFrom, "An eerie wailing song emanates from the sirens perched "
                        "upon the rocks.  Many of your crew fall under its spell, "
                        "and dive into the water where they drown.  You are now "
                        "wiser for the visit, and gain %d experience."),
                        experience11
                    );
                    NormalDialog(gText, NORMAL_DIALOG_INFO, -1, -1, -1, 0, -1, 0, -1, 0);
                    GiveExperience(eventHero, experience11, 1);
                } else {
                    NormalDialog(
                        DATA_COMPGEN(0x0051c120, genericSiteEventAsTheSirensSingTheirEerie, "As the sirens sing their eerie song, your small, "
                        "determined army manages to overcome the urge to dive "
                        "headlong into the sea."),
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
                eventHero->m_eventFlags = eventHero->m_eventFlags | HERO_EVENT_SIRENS;
            }
            break;

        case GENERIC_SITE_STABLES:
            unusedOne18 = STABLE_VISIT_MOBILITY;
            unusedTwo6 = STABLE_VISIT_UPGRADE;
            stableResult26 = STABLE_VISIT_NONE;
            if (HAS(eventHero->m_eventFlags, HERO_EVENT_STABLES) == 0) {
                eventHero->m_eventFlags = eventHero->m_eventFlags | HERO_EVENT_STABLES;
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
                    cell->m_triggerType & MAP_TRIGGER_TYPE_MASK, IDX(siteType2), &eventSample5
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

VA(0x004af436, 0x191)
void advManager::RecruitSiteEvent(mapCell* cell, hero* eventHero) {
    SAMPLE2 eventSample = NULL_SAMPLE2;
    H2_ENUM_STORAGE(RecruitSiteType, u32) siteType2;
    i16 availableCount;
    CreatureType creatureType1;
    u32 siteIndex;
    u32 packedSite1;

    siteType2 = cell->m_objectMetadata;
    siteType2 &= EVENT_RECRUIT_TYPE_MASK;
    availableCount = static_cast<i16>(cell->m_objectMetadata);
    availableCount >>= EVENT_RECRUIT_COUNT_SHIFT;

    switch (siteType2) {
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

    siteIndex = IDX(siteType2);
    if (availableCount == 0) {
        EventWindow(-1, NORMAL_DIALOG_INFO, xRecruitEmpty[siteIndex], -1, 0, -1, 0, -1);
    } else {
        EventSound(cell->m_triggerType & MAP_TRIGGER_TYPE_MASK, availableCount, &eventSample);
        EventWindow(-1, NORMAL_DIALOG_CONFIRM, xRecruitBuy[siteIndex], -1, 0, -1, 0, -1);
        if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
            ExpansionRecruitEvent(eventHero, creatureType1, &availableCount);
            packedSite1 = (availableCount << EVENT_RECRUIT_COUNT_SHIFT) | IDX(siteType2);
            cell->m_objectMetadata = packedSite1;
        }
    }
}

VA(0x004af5c7, 0x8b)
void advManager::ExpansionRecruitEvent(
    hero* eventHero, H2_ENUM_PARAM(CreatureType, i32) creatureType, i16* availableCount
) {
    tag_message dialogMessage2;
    baseManager* dialogManager = new recruitUnit(&eventHero->m_army, creatureType, availableCount);
    i32 dialogResult;
    if (dialogManager == NULL)
        MemError();
    gpExec->DoDialog(dialogManager);
    delete dialogManager;
}

VA(0x004af652, 0x22a)
void advManager::JailEvent(mapCell* cell, hero* eventHero, i32 x, i32 y) {
    SAMPLE2 eventSample1 = NULL_SAMPLE2;
    i32 heroId9;
    hero* releasedHero1;

    heroId9 = cell->m_objectMetadata;
    if (gpGame->m_availableHeroes[heroId9] != EVENT_JAILED_HERO) {
        NormalDialog(
            DATA_COMPGEN(0x0051c1a0, jailEventTheJailerTellsYouThatThe, "The jailer tells you that the hero who was imprisoned here has been released by the "
            "king who imprisoned him."),
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
            DATA_COMPGEN(0x0051c210, jailEventYouAlreadyHave8HeroesAnd, "You already have 8 heroes, and regretfully must leave the prisoner in this jail to "
            "languish in agony for untold days."),
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

    EventSound(cell->m_triggerType & MAP_TRIGGER_TYPE_MASK, 0, &eventSample1);
    NormalDialog(
        DATA_COMPGEN(0x0051c288, jailEventInADazzlingDisplayOfDaring, "In a dazzling display of daring, you break into the local jail and free the hero "
        "imprisoned there, who, in return, pledges loyalty to your cause."),
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
    gpGame->m_heroRecs[heroId9].m_owner = eventHero->m_owner;
    gpGame->m_availableHeroes[heroId9] = eventHero->m_owner;
    releasedHero1 = &gpGame->m_heroRecs[heroId9];
    EraseObj(cell, x, y);
    gpCurPlayer->m_heroIds[gpCurPlayer->m_heroCount] = static_cast<i8>(heroId9);
    gpCurPlayer->m_heroCount++;
    releasedHero1->m_x = x;
    releasedHero1->m_y = y;
    releasedHero1->m_eventFlags = HERO_EVENT_NONE;
    releasedHero1->m_direction = MAP_DIRECTION_EAST;
    releasedHero1->m_remainingMobility = releasedHero1->CalcMobility();
    releasedHero1->m_mobility = releasedHero1->m_remainingMobility;
    releasedHero1->m_locationType = cell->m_triggerType;
    releasedHero1->m_occupiedTown = cell->m_objectMetadata;
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

VA(0x004af87c, 0x1da)
void advManager::TownEvent(mapCell* cell, i32 x, i32 y) {
    hero* eventHero1;
    CombatResult combatResult1;
    hero* defendingHero;
    town* eventTown1;

    eventTown1 = gpGame->GetTown(cell->m_objectMetadata);
    eventHero1 = gpGame->GetHero(gpCurPlayer->m_currentHero);
    DemobilizeCurrHero();
    if (eventTown1->m_owner == giCurPlayer) {
        eventTown1->m_occupyingHeroId = gpCurPlayer->CurrentHero();
        eventTown1->View(0);
    } else if (eventTown1->HasGarrison()) {
        defendingHero = eventTown1->m_occupyingHeroId == -1
                            ? NULL
                            : gpGame->GetHero(eventTown1->m_occupyingHeroId);
        combatResult1 = DoCombat(
            x,
            y,
            eventHero1,
            &eventHero1->m_army,
            eventTown1,
            defendingHero,
            &eventTown1->m_army,
            x,
            y,
            -1,
            1
        );
        if (combatResult1 == COMBAT_RESULT_ATTACKER)
            gpGame->ClaimTown(eventTown1->m_id, giCurPlayer, 0);
    } else {
        gpGame->ClaimTown(eventTown1->m_id, giCurPlayer, 0);
        if (gbGameOver)
            return;
        UpdateRadar(1, 0);
        UpdateHeroLocators(1, 1);
        UpdateTownLocators(1, 1);
        eventTown1->m_occupyingHeroId = gpCurPlayer->CurrentHero();
        eventTown1->View(0);
    }
    eventTown1->GiveSpells(NULL);
    eventHero1->CheckLevel();
}

VA(0x004afa56, 0x516)
void advManager::EventSound(
    H2_ENUM_PARAM(MapObjectType, i32) eventType,
    i32 eventData,
    struct SAMPLE2* outSample
) {
    const i32 treasureSound_a = SOUND_TREASURE;
    const i32 experienceSound_o = SOUND_EXPERIENCE;
    const i32 moraleSound_e = SOUND_MORALE;
    const i32 luckSound_g = SOUND_LUCK;
    const i32 pickupSound_o = SOUND_PICKUP;
    const i32 mineSound_c = SOUND_MINE;
    char sampleFile_m[SOUND_FILENAME_LENGTH];
    strcpy(sampleFile_m, DATA_COMPGEN(0x0051c31c, eventSoundEmptyString, ""));
    i32 musicTrack_e = SOUND_NONE;

    switch (eventType) {
        case MAP_OBJECT_WAGON:
        case MAP_OBJECT_LEAN_TO:
            musicTrack_e = pickupSound_o;
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
            musicTrack_e = experienceSound_o;
            break;
        case MAP_OBJECT_LIGHTHOUSE:
        case MAP_OBJECT_XANADU:
            musicTrack_e = SOUND_TRACK_40;
            break;
        case MAP_OBJECT_SHRINE_FIRST_CIRCLE:
        case MAP_OBJECT_SHRINE_SECOND_CIRCLE:
        case MAP_OBJECT_SHRINE_THIRD_CIRCLE:
            musicTrack_e = treasureSound_a;
            break;
        case MAP_OBJECT_ARTIFACT:
            if (static_cast<EventSoundVariant>(eventData) == SOUND_VARIANT_1)
                musicTrack_e = treasureSound_a;
            break;
        case MAP_OBJECT_SKELETON:
        case MAP_OBJECT_WATER_WHEEL:
        case MAP_OBJECT_WINDMILL:
            musicTrack_e = treasureSound_a;
            break;
        case MAP_OBJECT_DAEMON_CAVE:
            musicTrack_e = SOUND_TRACK_36;
            break;
        case MAP_OBJECT_ORACLE:
        case MAP_OBJECT_ARTESIAN_SPRING:
        case MAP_OBJECT_MAGIC_WELL:
            musicTrack_e = SOUND_TRACK_32;
            break;
        case MAP_OBJECT_FAERIE_RING:
        case MAP_OBJECT_FOUNTAIN:
        case MAP_OBJECT_IDOL:
            musicTrack_e = luckSound_g;
            break;
        case MAP_OBJECT_BUOY:
        case MAP_OBJECT_OASIS:
        case MAP_OBJECT_TEMPLE:
        case MAP_OBJECT_WATERING_HOLE:
            musicTrack_e = moraleSound_e;
            break;
        case MAP_OBJECT_GAZEBO:
        case MAP_OBJECT_TREE_OF_KNOWLEDGE:
        case MAP_OBJECT_WITCH_HUT:
            musicTrack_e = SOUND_TRACK_37;
            break;
        case MAP_OBJECT_ANCIENT_LAMP:
        case MAP_OBJECT_SPHINX:
            musicTrack_e = SOUND_TRACK_33;
            break;
        case MAP_OBJECT_GRAVEYARD:
        case MAP_OBJECT_SHIPWRECK:
        case MAP_OBJECT_ABANDONED_MINE:
        case MAP_OBJECT_MAGELLAN_MAPS:
        case MAP_OBJECT_DERELICT_SHIP:
        case MAP_OBJECT_OBSERVATION_TOWER:
            musicTrack_e = SOUND_TRACK_39;
            break;
        case MAP_OBJECT_DRAGON_CITY:
        case MAP_OBJECT_PYRAMID:
        case MAP_OBJECT_CITY_OF_DEAD:
        case MAP_OBJECT_TROLL_BRIDGE:
            musicTrack_e = SOUND_TRACK_31;
            break;
        case MAP_OBJECT_OBELISK:
            musicTrack_e = experienceSound_o;
            break;
        case MAP_OBJECT_FORT:
        case MAP_OBJECT_STANDING_STONES:
        case MAP_OBJECT_WITCH_DOCTOR_HUT:
        case MAP_OBJECT_MERCENARY_CAMP:
            musicTrack_e = SOUND_TRACK_38;
            break;
        case MAP_OBJECT_DESERT_TENT:
        case MAP_OBJECT_HILL_FORT:
            musicTrack_e = SOUND_TRACK_34;
            break;
        case MAP_OBJECT_STONE_LITHS:
            musicTrack_e = SOUND_TRACK_1;
            break;
        case MAP_OBJECT_WAGON_CAMP:
        case MAP_OBJECT_TREE_CITY:
        case MAP_OBJECT_MAGIC_GARDEN:
            musicTrack_e = SOUND_TRACK_35;
            break;
        case MAP_OBJECT_BARRIER:
            musicTrack_e = experienceSound_o;
            break;
        case MAP_OBJECT_TRAVELER_TENT:
            musicTrack_e = experienceSound_o;
            break;
        case MAP_OBJECT_EXPANSION_OBJECT:
            switch (static_cast<EventSoundVariant>(eventData)) {
                case SOUND_VARIANT_0:
                    musicTrack_e = experienceSound_o;
                    break;
                case SOUND_VARIANT_1:
                    musicTrack_e = experienceSound_o;
                    break;
                case SOUND_VARIANT_5:
                    musicTrack_e = luckSound_g;
                    break;
                case SOUND_VARIANT_2:
                    musicTrack_e = experienceSound_o;
                    break;
                case SOUND_VARIANT_3:
                    break;
                case SOUND_VARIANT_6:
                    musicTrack_e = experienceSound_o;
                    break;
                case SOUND_VARIANT_4:
                    musicTrack_e = experienceSound_o;
                    break;
                default:;
            }
            break;
        case MAP_OBJECT_EXPANSION_DWELLING:
            musicTrack_e = experienceSound_o;
            break;
        case MAP_OBJECT_JAIL:
            musicTrack_e = experienceSound_o;
            break;
        default:
            musicTrack_e = SOUND_NONE;
            break;
    }

    if (musicTrack_e != SOUND_NONE) {
        switch (musicTrack_e) {
            case SOUND_TREASURE:
                strcpy(sampleFile_m, DATA_COMPGEN(0x0051c320, eventSoundTreasure82m, "treasure.82m"));
                break;
            case SOUND_EXPERIENCE:
                strcpy(sampleFile_m, DATA_COMPGEN(0x0051c330, eventSoundExpernce82m, "expernce.82m"));
                break;
            case SOUND_MORALE:
                strcpy(sampleFile_m, DATA_COMPGEN(0x0051c340, eventSoundGoodmrle82m, "goodmrle.82m"));
                break;
            case SOUND_LUCK:
                strcpy(sampleFile_m, DATA_COMPGEN(0x0051c350, eventSoundGoodluck82m, "goodluck.82m"));
                break;
            case SOUND_PICKUP:
                strcpy(sampleFile_m, DATA_COMPGEN(0x0051c360, eventSoundPickup0182m, "pickup01.82m"));
                break;
            case SOUND_MINE:
                strcpy(sampleFile_m, DATA_COMPGEN(0x0051c370, eventSoundH2mine82m, "h2mine.82m"));
                break;
            default:
                gpSoundManager->SwitchAmbientMusic(musicTrack_e);
                break;
        }

        if (strlen(sampleFile_m) > SOUND_SAMPLE_NAME_EMPTY_LIMIT)
            *outSample = LoadPlaySample(sampleFile_m);
    }
}

VA(0x004aff6c, 0xc7)
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
    i32 dialogState7;
    i32 eventWindowUnused12;
    i32 windowGap1;
    i32 unusedStyle9;
    i32 unusedResult;
    i32 dialogState8;
    i32 eventWindowUnused11;
    i32 unusedStyle;
    char eventText[EVENT_TEXT_BUFFER_SIZE];

    unusedResult = 0;
    unusedStyle9 = 1;

    if (eventId >= 0 && eventId < EVENT_TEXT_COUNT)
        sprintf(eventText, gEventText[eventId]);
    else if (eventId == MAP_EVENT_REWARD_NONE)
        sprintf(eventText, text);
    else
        sprintf(eventText, DATA_COMPGEN(0x0051c37c, eventWindowEventIDD, "Event ID %d"), eventId);

    NormalDialog(eventText, buttons, -1, -1, IDX(type1), value1, type2, value2, type3, 0);
}

VA(0x004b0033, 0xb6)
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

VA(0x004b00e9, 0x5e)
ArtifactType advManager::GiveRandomArtifact(hero* eventHero) {
    ArtifactType artifactId =
        ArtifactType(gpGame->GetRandomArtifactId(ARTIFACT_LEVEL_RANDOM, true));

    if (artifactId == ARTIFACT_NONE)
        GiveResource(eventHero, RES_GOLD, EVENT_RANDOM_ARTIFACT_GOLD);
    else
        GiveArtifact(eventHero, artifactId, true, -1);
    return artifactId;
}

VA(0x004b0147, 0x67)
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

VA(0x004b01ae, 0x80)
void advManager::GiveResource(hero* eventHero, ResourceType resourceType, i32 amount) {
    if (resourceType >= RES_WOOD && resourceType <= RES_GOLD)
        gpGame->m_players[eventHero->m_owner].m_resources[IDX(resourceType)] += amount;
    if (resourceType == RES_GOLD && gbHumanPlayer[eventHero->m_owner])
        CheckEndGame(END_GAME_FORCE_NONE, false);
}

VA(0x004b022e, 0xbb)
void advManager::RecruitEvent(
    hero* eventHero, H2_ENUM_PARAM(CreatureType, i32) creatureType, mapCell* cell
) {
    tag_message dialogMessage2;
    i16 availableCount15 = static_cast<i16>(cell->m_objectMetadata);
    baseManager* dialogManager =
        new recruitUnit(&eventHero->m_army, creatureType, &availableCount15);
    i32 dialogResult;

    if (dialogManager == NULL)
        MemError();
    gpExec->DoDialog(dialogManager);
    delete dialogManager;
    cell->m_objectMetadata = static_cast<u16>(availableCount15 + 0);
}

VA(0x004b02e9, 0x261)
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

VA(0x004b054a, 0x29b)
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

VA(0x004b07e5, 0x2f8)
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
                sprintf(gText, DATA_COMPGEN(0x0051c388, ghostEventS, "%s"), text);
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
                sprintf(gText, DATA_COMPGEN(0x0051c38c, ghostEventS2, "%s"), text);
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
                sprintf(gText, DATA_COMPGEN(0x0051c390, ghostEventS3, "%s"), text);
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
                sprintf(gText, DATA_COMPGEN(0x0051c394, ghostEventS4, "%s"), text);
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

VA(0x004b0add, 0x274)
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
            DATA_COMPGEN(0x0051c398, houseEventEmptyString, ""),
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
            DATA_COMPGEN(0x0051c39c, houseEventEmptyString2, ""),
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
                    DATA_COMPGEN(0x0051c3a0, houseEventEmptyString3, ""),
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

VA(0x004b0d51, 0x62f)
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
    i32 stackCount;
    i32 stackIndex9;
    CombatResult combatResult7;
    i32 lastStackCount;
    CreatureType temporaryTypes7[MONSTER_ARMY_SLOTS];
    i32 temporaryCounts[MONSTER_ARMY_SLOTS];
    i32 placement4[MONSTER_ARMY_SLOTS + 1];
    i32 groupCount;
    i32 stackTotal;

    DemobilizeCurrHero();
    if (combatX == -1) {
        combatX = mapX;
        combatY = mapY;
    } else {
        m_lastQuickViewX = combatX;
        m_lastQuickViewY = combatY;
        if (eventHero->m_x >= combatX)
            m_mineGuardianFacingLeft = 0;
        else
            m_mineGuardianFacingLeft = 1;
        if (ComboDraw(0))
            UpdateScreen(0, 0);
        m_lastQuickViewX = -1;
    }

    memset(gpMonGroup->m_creatureTypes, IDX(CREATURE_NONE), MONSTER_ARMY_SLOTS);
    memset(gpMonGroup->m_creatureCounts, 0, MONSTER_ARMY_SLOTS * sizeof(i16));
    stackCount = MONSTER_ARMY_SLOTS - secondaryStacks - tertiaryStacks;
    if (stackCount < 1)
        stackCount = 1;
    placement4[MONSTER_ARMY_SLOTS] = 0;
    SRand(combatY + combatX);
    if (stackCount == MONSTER_ARMY_SLOTS
        && HAS(gMonsterDatabase[IDX(monsterType)].attributes, MONSTER_ATTRIBUTE_RANGED) == 0) {
        i32 roll = SRandom(0, MONSTER_RANDOM_MAX);
        if (roll < MONSTER_REDUCED_STACK_CHANCE)
            stackCount = MONSTER_REDUCED_STACK_COUNT;
        else if (roll < MONSTER_FOUR_STACK_THRESHOLD)
            stackCount = MONSTER_FOUR_STACK_COUNT;
    }

    for (stackIndex9 = 0; stackIndex9 < stackCount; stackIndex9++) {
        if (stackIndex9 == (stackCount >> 1)
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
            gpMonGroup->m_creatureTypes[placement4[MONSTER_ARMY_SLOTS] + stackIndex9] =
                NextCreatureType(monsterType);
        else
            gpMonGroup->m_creatureTypes[placement4[MONSTER_ARMY_SLOTS] + stackIndex9] =
                monsterType;
        gpMonGroup->m_creatureCounts[placement4[MONSTER_ARMY_SLOTS] + stackIndex9] =
            static_cast<i16>((stackIndex9 < monsterCount % stackCount) + monsterCount / stackCount);
    }

    placement4[MONSTER_ARMY_SLOTS] += stackCount;
    if (secondaryStacks != 0) {
        stackCount = secondaryStacks;
        for (stackIndex9 = 0; stackIndex9 < stackCount; stackIndex9++) {
            gpMonGroup->m_creatureTypes[placement4[MONSTER_ARMY_SLOTS] + stackIndex9] =
                secondaryType;
            gpMonGroup->m_creatureCounts[placement4[MONSTER_ARMY_SLOTS] + stackIndex9] =
                static_cast<i16>(
                    (stackIndex9 < secondaryCount % stackCount) + secondaryCount / stackCount
                );
        }
    }

    placement4[MONSTER_ARMY_SLOTS] += stackCount;
    if (tertiaryStacks != 0) {
        stackCount = tertiaryStacks;
        for (stackIndex9 = 0; stackIndex9 < stackCount; stackIndex9++) {
            gpMonGroup->m_creatureTypes[placement4[MONSTER_ARMY_SLOTS] + stackIndex9] =
                tertiaryType;
            gpMonGroup->m_creatureCounts[placement4[MONSTER_ARMY_SLOTS] + stackIndex9] =
                static_cast<i16>(
                    (stackIndex9 < secondaryCount % stackCount) + tertiaryCount / stackCount
                );
        }
    }
    lastStackCount = stackCount;

    for (stackIndex9 = 0; stackIndex9 < MONSTER_ARMY_SLOTS; stackIndex9++) {
        if (gpMonGroup->m_creatureCounts[stackIndex9] <= 0)
            gpMonGroup->m_creatureTypes[stackIndex9] = CREATURE_NONE;
    }
    for (stackIndex9 = 0; stackIndex9 < MONSTER_ARMY_SLOTS; stackIndex9++)
        placement4[stackIndex9] = stackIndex9;

    // Retail permutations for centering the last creature group in the five army slots.
    // The indices are topology payload, not independent gameplay constants.
    // NOLINTBEGIN(readability-magic-numbers)
    if (lastStackCount == 1) {
        placement4[2] = 0;
        placement4[0] = 2;
    } else if (lastStackCount == 2) {
        placement4[1] = 1;
        placement4[3] = 0;
        placement4[0] = 3;
    } else if (lastStackCount == 3) {
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
    // NOLINTEND(readability-magic-numbers)

    for (stackIndex9 = 0; stackIndex9 < MONSTER_ARMY_SLOTS; stackIndex9++) {
        temporaryTypes7[stackIndex9] = gpMonGroup->m_creatureTypes[stackIndex9];
        temporaryCounts[stackIndex9] = gpMonGroup->m_creatureCounts[stackIndex9];
    }
    for (stackIndex9 = 0; stackIndex9 < MONSTER_ARMY_SLOTS; stackIndex9++) {
        gpMonGroup->m_creatureTypes[stackIndex9] = temporaryTypes7[placement4[stackIndex9]];
        gpMonGroup->m_creatureCounts[stackIndex9] =
            static_cast<i16>(temporaryCounts[placement4[stackIndex9]]);
    }

    if (defender != 0)
        combatResult7 = DoCombat(
            combatX,
            combatY,
            NULL,
            gpMonGroup,
            NULL,
            eventHero,
            &eventHero->m_army,
            mapX,
            mapY,
            combatY + combatX,
            1
        );
    else
        combatResult7 = DoCombat(
            combatX,
            combatY,
            eventHero,
            &eventHero->m_army,
            NULL,
            NULL,
            gpMonGroup,
            mapX,
            mapY,
            combatY + combatX,
            1
        );
    MobilizeCurrHero(0);
    return combatResult7;
}

VA(0x004b1380, 0x5f3)
void GiveTakeArtifactStat(hero* targetHero, ArtifactType artifact, b32 take) {
    i32 statChanges[EVENT_ARTIFACT_PRIMARY_STAT_COUNT + 1];
    i32 maxSpellPoints;

    if (artifact == ARTIFACT_NONE)
        return;
    statChanges[IDX(HERO_PRIMARY_ATTACK)] = 0;
    statChanges[IDX(HERO_PRIMARY_DEFENSE)] = 0;
    statChanges[IDX(HERO_PRIMARY_SPELL_POWER)] = 0;
    statChanges[IDX(HERO_PRIMARY_KNOWLEDGE)] = 0;

    // Artifact primary-stat bonus table. Each value is the payload of its named artifact/stat
    // row; giving every cell another constant name would only duplicate that context.
    // NOLINTBEGIN(readability-magic-numbers)
    switch (artifact) {
        case ARTIFACT_ULTIMATE_BOOK:
            statChanges[IDX(HERO_PRIMARY_KNOWLEDGE)] = 12;
            break;
        case ARTIFACT_ULTIMATE_SWORD:
            statChanges[IDX(HERO_PRIMARY_ATTACK)] = 12;
            break;
        case ARTIFACT_ULTIMATE_CLOAK:
            statChanges[IDX(HERO_PRIMARY_DEFENSE)] = 12;
            break;
        case ARTIFACT_ULTIMATE_WAND:
            statChanges[IDX(HERO_PRIMARY_SPELL_POWER)] = 12;
            break;
        case ARTIFACT_ULTIMATE_SHIELD:
            statChanges[IDX(HERO_PRIMARY_ATTACK)] = 6;
            statChanges[IDX(HERO_PRIMARY_DEFENSE)] = 6;
            break;
        case ARTIFACT_ULTIMATE_STAFF:
            statChanges[IDX(HERO_PRIMARY_SPELL_POWER)] = 6;
            statChanges[IDX(HERO_PRIMARY_KNOWLEDGE)] = 6;
            break;
        case ARTIFACT_ULTIMATE_CROWN:
            statChanges[IDX(HERO_PRIMARY_ATTACK)] = 4;
            statChanges[IDX(HERO_PRIMARY_DEFENSE)] = 4;
            statChanges[IDX(HERO_PRIMARY_SPELL_POWER)] = 4;
            statChanges[IDX(HERO_PRIMARY_KNOWLEDGE)] = 4;
            break;
        case ARTIFACT_GOLDEN_GOOSE:
            break;
        case ARTIFACT_ARCANE_NECKLACE:
            statChanges[IDX(HERO_PRIMARY_SPELL_POWER)] = 4;
            break;
        case ARTIFACT_CASTER_BRACELET:
            statChanges[IDX(HERO_PRIMARY_SPELL_POWER)] = 2;
            break;
        case ARTIFACT_MAGE_RING:
            statChanges[IDX(HERO_PRIMARY_SPELL_POWER)] = 2;
            break;
        case ARTIFACT_WITCHES_BROACH:
            statChanges[IDX(HERO_PRIMARY_SPELL_POWER)] = 3;
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
            statChanges[IDX(HERO_PRIMARY_ATTACK)] = 1;
            break;
        case ARTIFACT_ARMORED_GAUNTLETS:
            statChanges[IDX(HERO_PRIMARY_DEFENSE)] = 1;
            break;
        case ARTIFACT_DEFENDER_HELM:
            statChanges[IDX(HERO_PRIMARY_DEFENSE)] = 1;
            break;
        case ARTIFACT_GIANT_FLAIL:
            statChanges[IDX(HERO_PRIMARY_ATTACK)] = 1;
            break;
        case ARTIFACT_BALLISTA:
            break;
        case ARTIFACT_STEALTH_SHIELD:
            statChanges[IDX(HERO_PRIMARY_DEFENSE)] = 2;
            break;
        case ARTIFACT_DRAGON_SWORD:
            statChanges[IDX(HERO_PRIMARY_ATTACK)] = 3;
            break;
        case ARTIFACT_POWER_AXE:
            statChanges[IDX(HERO_PRIMARY_ATTACK)] = 2;
            break;
        case ARTIFACT_DIVINE_BREASTPLATE:
            statChanges[IDX(HERO_PRIMARY_DEFENSE)] = 3;
            break;
        case ARTIFACT_MINOR_SCROLL:
            statChanges[IDX(HERO_PRIMARY_KNOWLEDGE)] = 2;
            break;
        case ARTIFACT_MAJOR_SCROLL:
            statChanges[IDX(HERO_PRIMARY_KNOWLEDGE)] = 3;
            break;
        case ARTIFACT_SUPERIOR_SCROLL:
            statChanges[IDX(HERO_PRIMARY_KNOWLEDGE)] = 4;
            break;
        case ARTIFACT_FOREMOST_SCROLL:
            statChanges[IDX(HERO_PRIMARY_KNOWLEDGE)] = 5;
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
            statChanges[IDX(HERO_PRIMARY_ATTACK)] = 1;
            statChanges[IDX(HERO_PRIMARY_DEFENSE)] = 1;
            break;
        case ARTIFACT_SPIKED_SHIELD:
            statChanges[IDX(HERO_PRIMARY_ATTACK)] = 2;
            statChanges[IDX(HERO_PRIMARY_DEFENSE)] = 2;
            break;
        case ARTIFACT_WHITE_PEARL:
            statChanges[IDX(HERO_PRIMARY_SPELL_POWER)] = 1;
            statChanges[IDX(HERO_PRIMARY_KNOWLEDGE)] = 1;
            break;
        case ARTIFACT_BLACK_PEARL:
            statChanges[IDX(HERO_PRIMARY_SPELL_POWER)] = 2;
            statChanges[IDX(HERO_PRIMARY_KNOWLEDGE)] = 2;
            break;
        case ARTIFACT_MAGIC_BOOK:
            break;
        case ARTIFACT_SPELL_SCROLL:
            break;
        case ARTIFACT_ARM_OF_MARTYR:
            statChanges[IDX(HERO_PRIMARY_SPELL_POWER)] = 3;
            break;
        case ARTIFACT_BREASTPLATE_ANDURAN:
            statChanges[IDX(HERO_PRIMARY_DEFENSE)] = 5;
            break;
        case ARTIFACT_BROACH_SHIELDING:
            statChanges[IDX(HERO_PRIMARY_SPELL_POWER)] = -2;
            break;
        case ARTIFACT_BATTLE_GARB:
            statChanges[IDX(HERO_PRIMARY_SPELL_POWER)] = 5;
            statChanges[IDX(HERO_PRIMARY_DEFENSE)] = 5;
            statChanges[IDX(HERO_PRIMARY_ATTACK)] = 5;
            break;
        case ARTIFACT_CRYSTAL_BALL:
            break;
        case ARTIFACT_HEART_FIRE:
            break;
        case ARTIFACT_HEART_ICE:
            break;
        case ARTIFACT_HELMET_ANDURAN:
            statChanges[IDX(HERO_PRIMARY_SPELL_POWER)] = 5;
            break;
        case ARTIFACT_HOLY_HAMMER:
            statChanges[IDX(HERO_PRIMARY_ATTACK)] = 5;
            break;
        case ARTIFACT_LEGENDARY_SCEPTER:
            statChanges[IDX(HERO_PRIMARY_SPELL_POWER)] = 2;
            statChanges[IDX(HERO_PRIMARY_ATTACK)] = 2;
            statChanges[IDX(HERO_PRIMARY_DEFENSE)] = 2;
            statChanges[IDX(HERO_PRIMARY_KNOWLEDGE)] = 2;
            break;
        case ARTIFACT_MASTHEAD:
            break;
        case ARTIFACT_SPHERE_NEGATION:
            break;
        case ARTIFACT_STAFF_WIZARDRY:
            statChanges[IDX(HERO_PRIMARY_SPELL_POWER)] = 5;
            break;
        case ARTIFACT_SWORD_BREAKER:
            statChanges[IDX(HERO_PRIMARY_DEFENSE)] = 4;
            statChanges[IDX(HERO_PRIMARY_ATTACK)] = 1;
            break;
        case ARTIFACT_SWORD_ANDURAN:
            statChanges[IDX(HERO_PRIMARY_ATTACK)] = 5;
            break;
        case ARTIFACT_SPADE_NECROMANCY:
            break;
        default:
            break;
    }
    // NOLINTEND(readability-magic-numbers)

    for (statChanges[EVENT_ARTIFACT_PRIMARY_STAT_COUNT] = 0;
         statChanges[EVENT_ARTIFACT_PRIMARY_STAT_COUNT] < EVENT_ARTIFACT_PRIMARY_STAT_COUNT;
         statChanges[EVENT_ARTIFACT_PRIMARY_STAT_COUNT]++) {
        targetHero->m_primaryStats[statChanges[EVENT_ARTIFACT_PRIMARY_STAT_COUNT]] +=
            statChanges[statChanges[EVENT_ARTIFACT_PRIMARY_STAT_COUNT]]
            * (take == EVENT_ARTIFACT_TAKE ? -1 : 1);
        if (statChanges[EVENT_ARTIFACT_PRIMARY_STAT_COUNT] == IDX(HERO_PRIMARY_KNOWLEDGE)
            && take == EVENT_ARTIFACT_TAKE) {
            maxSpellPoints =
                targetHero->Stats(HERO_PRIMARY_KNOWLEDGE) * EVENT_ARTIFACT_SPELL_POINT_MULTIPLIER;
            if (targetHero->m_spellPoints > maxSpellPoints)
                targetHero->m_spellPoints = static_cast<i16>(maxSpellPoints);
        }
    }
}

VA(0x004b1973, 0x1dd)
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
                    if (sourceHero->m_artifacts[sourceArtifactSlot] <= ARTIFACT_ULTIMATE_WAND) {
                        if (gbThisNetHumanPlayer[sourceHero->m_owner]
                            || gbThisNetHumanPlayer[destinationHero->m_owner]) {
                            sprintf(
                                gText,
                                DATA_COMPGEN(0x0051c3a4, transferArtifactsAsYouReachForTheS, "As you reach for the %s, it mysteriously disappears."),
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

VA(0x004b1b50, 0x7f)
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

VA(0x004b1bcf, 0x132)
void advManager::DoWhirlpool(hero* eventHero) {
    i32 armyValue;
    i32 weakestValue;
    i32 selectedSlot;
    i32 slotIndex;
    i32 groupValues[MONSTER_ARMY_SLOTS];

    if (gbHumanPlayer[eventHero->m_owner] == 0)
        return;
    if (Random(EVENT_WHIRLPOOL_TRIGGER_ROLL, EVENT_WHIRLPOOL_TRIGGER_MAX)
        != EVENT_WHIRLPOOL_TRIGGER_ROLL)
        return;
    {
        weakestValue = EVENT_WHIRLPOOL_ARMY_VALUE_LIMIT;
        selectedSlot = -1;
        for (slotIndex = 0; slotIndex < MONSTER_ARMY_SLOTS; slotIndex++) {
            if (eventHero->m_army.m_creatureCounts[slotIndex] > 0) {
                armyValue =
                    gMonsterDatabase[IDX(eventHero->m_army.m_creatureTypes[slotIndex])].fightValue
                    * eventHero->m_army.m_creatureCounts[slotIndex];
                if (armyValue < weakestValue) {
                    weakestValue = armyValue;
                    selectedSlot = slotIndex;
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

VA(0x004b1d01, 0x142)
void advManager::FizzleCenter(i32 fizzleType) {
    SAMPLE2 playedSample;
    i32 fizzleStepCount;

    if (bShowIt == 0)
        return;
    {
        switch (fizzleType) {
            case EVENT_FIZZLE_HERO_LOSS:
                sprintf(gText, DATA_COMPGEN(0x0051c3dc, fizzleCenterKillfade82M, "killfade.82M"));
                break;
            case EVENT_FIZZLE_ARTIFACT:
                sprintf(
                    gText,
                    DATA_COMPGEN(0x0051c3ec, fizzleCenterPickup02d82M, "pickup%02d.82M"),
                    Random(ARTIFACT_PICKUP_SOUND_FIRST, ARTIFACT_PICKUP_SOUND_LAST)
                );
                break;
            default:
                return;
        }
        playedSample = NULL_SAMPLE2;
        playedSample = LoadPlaySample(gText);
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
        WaitEndSample(playedSample, -1);
    }
}

VA(0x004b1e43, 0x2a40)
void advManager::DoAIEvent(mapCell* cell, hero* eventHero, i32 x, i32 y) {
    float battleStatValue_o;
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

    if (eventHero->m_destinationX == x && eventHero->m_destinationY == y) {
        eventHero->m_destinationY = -1;
        eventHero->m_destinationX = eventHero->m_destinationY;
    }
    --eventHero->m_remainingMobility;
    switch (eventType_g) {
        case MAP_OBJECT_COAST:
            if (HAS(eventHero->m_eventFlags, HERO_EVENT_EMBARKED)) {
                eventHero->m_eventFlags = eventHero->m_eventFlags & ~HERO_EVENT_EMBARKED;
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
            eventHero->m_eventFlags = eventHero->m_eventFlags | HERO_EVENT_EMBARKED;
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
                        && GetManaCost(SpellType(index_h), eventHero) < eventHero->m_spellPoints) {
                        eventHero->m_spellPoints = static_cast<i16>(
                            eventHero->m_spellPoints - GetManaCost(SpellType(index_h), eventHero)
                        );
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
            if (gpGame->m_mineOwners[cell->m_objectMetadata] == giCurPlayer) {
            } else {
                gpGame->ClaimMine(cell->m_objectMetadata, giCurPlayer);
            }
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
                        (cell->m_objectMetadata * CHEST_EXPERIENCE_LEVEL_SCALE
                         - CHEST_EXPERIENCE_LEVEL_OFFSET)
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
                        (cell->m_objectMetadata * CHEST_EXPERIENCE_LEVEL_SCALE
                         - CHEST_EXPERIENCE_LEVEL_OFFSET)
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
                eventHero->m_eventFlags = eventHero->m_eventFlags | HERO_EVENT_WATERING_HOLE;
                ++eventHero->m_morale;
            }
            break;

        case MAP_OBJECT_BUOY:
            if (HAS(eventHero->m_eventFlags, HERO_EVENT_BUOY) == 0) {
                eventHero->m_eventFlags = eventHero->m_eventFlags | HERO_EVENT_BUOY;
                ++eventHero->m_morale;
            }
            break;

        case MAP_OBJECT_FAERIE_RING:
            if (HAS(eventHero->m_eventFlags, HERO_EVENT_FAERIE_RING) == 0) {
                eventHero->m_eventFlags = eventHero->m_eventFlags | HERO_EVENT_FAERIE_RING;
                ++eventHero->m_luck;
            }
            break;

        case MAP_OBJECT_IDOL:
            if (HAS(eventHero->m_eventFlags, HERO_EVENT_IDOL) == 0) {
                eventHero->m_eventFlags = eventHero->m_eventFlags | HERO_EVENT_IDOL;
                ++eventHero->m_luck;
            }
            break;

        case MAP_OBJECT_FOUNTAIN:
            if (HAS(eventHero->m_eventFlags, HERO_EVENT_FOUNTAIN) == 0) {
                eventHero->m_eventFlags = eventHero->m_eventFlags | HERO_EVENT_FOUNTAIN;
                ++eventHero->m_luck;
            }
            break;

        case MAP_OBJECT_OASIS:
            if (HAS(eventHero->m_eventFlags, HERO_EVENT_OASIS) == 0) {
                eventHero->m_eventFlags = eventHero->m_eventFlags | HERO_EVENT_OASIS;
                ++eventHero->m_morale;
                eventHero->m_mobility += OASIS_MOBILITY_BONUS;
                eventHero->m_remainingMobility += OASIS_MOBILITY_BONUS;
            }
            break;

        case MAP_OBJECT_TEMPLE:
            if (HAS(eventHero->m_eventFlags, HERO_EVENT_TEMPLE) == 0) {
                eventHero->m_eventFlags = eventHero->m_eventFlags | HERO_EVENT_TEMPLE;
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
                resourceType_a = static_cast<ResourceType>(
                    cell->m_objectMetadata - MAP_EVENT_RESOURCE_OFFSET
                );
                GiveResource(
                    eventHero,
                    resourceType_a,
                    resourceType_a == RES_GOLD ? MAP_EVENT_GOLD_AMOUNT
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
            if (cell->m_objectMetadata != MAP_EVENT_DATA_EMPTY) {
                if (cell->m_objectMetadata & WAGON_ARTIFACT_FLAG) {
                    if (eventHero->NumArtifacts() != HERO_ARTIFACT_SLOT_COUNT)
                        GiveArtifact(
                            eventHero,
                            ArtifactType(cell->m_objectMetadata & WAGON_ARTIFACT_MASK),
                            1,
                            -1
                        );
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
            resourceType_a = static_cast<ResourceType>(cell->m_objectIndex >> 1);
            resourceAmount_o = resourceType_a == RES_GOLD
                                   ? cell->m_objectMetadata * CAMPFIRE_GOLD_MULTIPLIER
                                   : cell->m_objectMetadata;
            GiveResource(eventHero, resourceType_a, resourceAmount_o);
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
                    cell->m_objectData = static_cast<u16>(
                        (cell->m_objectMetadata - eventResults[AI_EVENT_RESULT_PURCHASE_COUNT])
                            * (1 << EVENT_RECRUIT_COUNT_SHIFT)
                        | (cell->m_objectData & EVENT_RECRUIT_TYPE_MASK)
                    );
                    if (creatureFlag_l == 0) {
                        GetMonsterCost(creatureType_i, creatureCosts_a);
                        for (index_h = 0; index_h < IDX(RES_COUNT); ++index_h)
                            gpCurPlayer->m_resources[index_h] -=
                                creatureCosts_a[index_h]
                                * eventResults[AI_EVENT_RESULT_PURCHASE_COUNT];
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
            if ((giCurPlayerBit & gpGame->m_obeliskVisitors[cell->m_objectMetadata - 1]) == 0) {
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
            goto teleportEvent;
        case MAP_OBJECT_STONE_LITHS:
        teleportEvent:
            exitCount = 0;
            for (exitY_d = 0; MAP_HEIGHT > exitY_d; ++exitY_d) {
                for (exitX = 0; MAP_WIDTH > exitX; ++exitX) {
                    if (gpGame->m_worldMap.Row(exitY_d)[exitX].m_triggerType
                            == (eventType_g | MAP_TRIGGER_ACTION_FLAG)
                        && gpGame->m_worldMap.Row(exitY_d)[exitX].m_objectIndex
                               == cell->m_objectIndex
                        && abs(exitY_d - y) + abs(exitX - x)
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
                for (exitY_d = 0; MAP_HEIGHT > exitY_d; ++exitY_d) {
                    for (exitX = 0; MAP_WIDTH > exitX; ++exitX) {
                        if (gpGame->m_worldMap.Row(exitY_d)[exitX].m_triggerType
                                == (eventType_g | MAP_TRIGGER_ACTION_FLAG)
                            && gpGame->m_worldMap.Row(exitY_d)[exitX].m_objectIndex
                                   == cell->m_objectIndex
                            && abs(exitY_d - y) + abs(exitX - x)
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
            artifact_g = static_cast<ArtifactType>(cell->m_objectIndex >> 1);
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
                } else {
                    artifactGuardResult_e = 1;
                    if (gpPhilAI->ChooseToFightForArtifact(artifact_g, artifactGuardCount_b, 1)
                        == 0)
                        break;
                }
                if (gpPhilAI->CombatMonsterEvent(
                        eventHero,
                        artifactGuardCount_b,
                        &artifactGuardResult_e,
                        cell
                    )
                    == 0)
                    break;
                goto artifactPickup;
            }
            switch (cell->m_objectMetadata & ARTIFACT_EVENT_MODE_MASK) {
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
        case MAP_OBJECT_BOTTLE:
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
            if (cell->m_objectMetadata != 0
                && eventHero->HasSpell(SpellType(cell->m_objectMetadata - 1)) == 0) {
                for (index_h = 0; index_h < ARMY_GROUP_SLOT_COUNT; ++index_h) {
                    gpMonGroup->m_creatureTypes[index_h] = CREATURE_ROYAL_MUMMY;
                    gpMonGroup->m_creatureCounts[index_h] = PYRAMID_GUARD_STACK_QUANTITY;
                }
                index_h = cell->m_objectMetadata - 1;
                if (HAS(gsSpellInfo[index_h].attributes, SPELL_INFO_ATTRIBUTE_POWER)) {
                    battleStatValue_o =
                        eventHero->Stats(HERO_PRIMARY_SPELL_POWER) > AI_BATTLE_STAT_MAX
                            ? gfBattleStat[AI_BATTLE_STAT_MAX]
                            : gfBattleStat[eventHero->Stats(HERO_PRIMARY_SPELL_POWER)];
                    spellValueFactor_i = battleStatValue_o;
                } else {
                    spellValueFactor_i = DATA_COMPGEN(0x004eb928, doAIEventConstant, 1.0f);
                }
                pyramidBattleValue_l = static_cast<i32>(
                    gsSpellInfo[index_h].aiValue * gpCurPlayer->m_aiData.m_upgradeValueWeight
                    * spellValueFactor_i
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
                if (battleWon_j == 0) {
                } else {
                    index_h = PYRAMID_GUARD_COUNT;
                    combatResult_d = gpPhilAI->CombatMonsterEvent(
                        eventHero,
                        CREATURE_ROYAL_MUMMY,
                        &index_h,
                        cell
                    );
                    if (combatResult_d == 0) {
                    } else {
                        eventHero->AddSpell(
                            static_cast<SpellType>(cell->m_objectMetadata - 1),
                            eventHero->Stats(HERO_PRIMARY_KNOWLEDGE)
                        );
                        cell->m_objectMetadata = MAP_EVENT_DATA_EMPTY;
                    }
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
                static_cast<float>(gaiTurnValueOfMine[y * MAP_WIDTH + x])
                * gMineCharacteristics[IDX(RES_GOLD)] * gafAITurnCostResource[IDX(RES_GOLD)]
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
                if (combatResult_d == 0) {
                } else {
                    eventHero->CheckLevel();
                    // Same retail conversion topology as the human abandoned-mine event.
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
                spellPower_j = eventHero->Stats(HERO_PRIMARY_KNOWLEDGE);
                if (eventHero->m_spellPoints < spellPower_j * HERO_SPELL_POINTS_PER_KNOWLEDGE
                                                   * ARTESIAN_SPRING_MANA_MULTIPLIER)
                    eventHero->m_spellPoints = static_cast<i16>(
                        spellPower_j * HERO_SPELL_POINTS_PER_KNOWLEDGE
                        * ARTESIAN_SPRING_MANA_MULTIPLIER
                    );
            }
            break;

        case MAP_OBJECT_MAGIC_WELL:
            if (HAS(eventHero->m_eventFlags, HERO_EVENT_MAGIC_WELL) == 0) {
                cell->m_objectMetadata = MAP_EVENT_DATA_EMPTY;
                spellPower_j = eventHero->Stats(HERO_PRIMARY_KNOWLEDGE);
                if (eventHero->m_spellPoints < spellPower_j * HERO_SPELL_POINTS_PER_KNOWLEDGE) {
                    eventHero->m_eventFlags = eventHero->m_eventFlags | HERO_EVENT_MAGIC_WELL;
                    eventHero->m_spellPoints =
                        static_cast<i16>(spellPower_j * HERO_SPELL_POINTS_PER_KNOWLEDGE);
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

VA(0x004b4883, 0x65)
i32 advManager::BarrierAIEvent(mapCell* cell, hero*) {
    // Preserved retail /Od frame slot; this array has no semantic use.
    i32 unusedBarrier15[5]; // NOLINT(readability-magic-numbers)
    i32 color = cell->m_objectMetadata;
    color &= EVENT_BARRIER_COLOR_MASK;
    if (gpCurPlayer->m_barrierTents & (1 << color))
        return 1;
    else
        return 0;
    return 0;
}

VA(0x004b48e8, 0x53)
void advManager::PasswordAIEvent(mapCell* cell, hero*) {
    i32 unusedPassword6[1];
    i32 color = cell->m_objectMetadata;
    color &= EVENT_BARRIER_COLOR_MASK;
    gpCurPlayer->m_barrierTents |= (1 << color);
}

VA(0x004b493b, 0x369)
void advManager::GenericSiteAIEvent(mapCell* cell, hero* eventHero) {
    // Preserved retail /Od frame slots; these arrays have no semantic use.
    i32 unusedPair27[2]; // NOLINT(readability-magic-numbers)
    i32 artifactIndex1;
    i32 siteLevel5;
    HeroPrimaryStat primaryStat16;
    H2_ENUM_STORAGE(GenericSiteType, i32) siteType3;
    i32 unusedTriple26[3]; // NOLINT(readability-magic-numbers)
    i32 cursedArtifactCount5;
    CreatureType creatureType3;
    i32 quantity6;
    i32 armyValue7;

    cursedArtifactCount5 = 0;
    siteType3 = cell->m_objectMetadata;
    siteType3 &= GENERIC_SITE_TYPE_MASK;
    siteLevel5 = cell->m_objectMetadata;
    siteLevel5 >>= GENERIC_SITE_LEVEL_SHIFT;

    switch (siteType3) {
        case GENERIC_SITE_ALCHEMIST_TOWER:
            for (artifactIndex1 = 0; artifactIndex1 < HERO_ARTIFACT_SLOT_COUNT; artifactIndex1++) {
                if (IsCursedItem(eventHero->m_artifacts[artifactIndex1]))
                    cursedArtifactCount5++;
            }
            if (cursedArtifactCount5 != 0
                && gpCurPlayer->m_resources[IDX(RES_GOLD)]
                       >= EVENT_CURSED_ARTIFACT_GOLD_THRESHOLD) {
                for (artifactIndex1 = 0; artifactIndex1 < HERO_ARTIFACT_SLOT_COUNT;
                     artifactIndex1++) {
                    if (IsCursedItem(eventHero->m_artifacts[artifactIndex1]))
                        eventHero->m_artifacts[artifactIndex1] = ARTIFACT_NONE;
                }
                gpCurPlayer->m_resources[IDX(RES_GOLD)] -= EVENT_CURSED_ARTIFACT_COST;
            }
            break;
        case GENERIC_SITE_ARENA:
            if (!(eventHero->m_eventFlags & HERO_EVENT_ARENA)) {
                eventHero->m_eventFlags = eventHero->m_eventFlags | HERO_EVENT_ARENA;
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
                eventHero->m_eventFlags =
                    eventHero->m_eventFlags | HERO_EVENT_MERMAID;
                eventHero->m_luck = eventHero->m_luck + 1;
            }
            break;
        case GENERIC_SITE_HUT_OF_MAGI:
        case GENERIC_SITE_EYE_OF_MAGI:
            break;
        case GENERIC_SITE_SIRENS:
            if (!(eventHero->m_eventFlags & HERO_EVENT_SIRENS)) {
                armyValue7 = 0;
                for (artifactIndex1 = 0; artifactIndex1 < ARMY_GROUP_SLOT_COUNT; artifactIndex1++) {
                    creatureType3 = eventHero->m_army.m_creatureTypes[artifactIndex1];
                    if (creatureType3 != CREATURE_NONE) {
                        quantity6 = eventHero->m_army.m_quantities[artifactIndex1];
                        if (quantity6 > EVENT_SIRENS_MIN_ARMY_QUANTITY) {
                            eventHero->m_army.m_quantities[artifactIndex1] =
                                static_cast<i16>(quantity6 * AI_GENERIC_SITE_SIRENS_ARMY_REMAINDER);
                            armyValue7 +=
                                (quantity6 - eventHero->m_army.m_quantities[artifactIndex1])
                                * gMonsterDatabase[IDX(creatureType3)].hitPoints;
                        }
                    }
                }
                if (armyValue7 != 0)
                    GiveExperience(eventHero, armyValue7, 1);
                eventHero->m_eventFlags = eventHero->m_eventFlags | HERO_EVENT_SIRENS;
            }
            break;
        case GENERIC_SITE_STABLES:
            if (!(eventHero->m_eventFlags & HERO_EVENT_STABLES)) {
                eventHero->m_eventFlags =
                    eventHero->m_eventFlags | HERO_EVENT_STABLES;
                eventHero->m_mobility += EVENT_CREATURE_UPGRADE_MOBILITY;
                eventHero->m_remainingMobility += EVENT_CREATURE_UPGRADE_MOBILITY;
            }
            if (eventHero->CreatureTypeCount(CREATURE_CAVALRY))
                eventHero->UpgradeCreatures(CREATURE_CAVALRY, CREATURE_CHAMPION);
            break;
    }
}

VA(0x004b4ca4, 0x18f)
void advManager::RecruitSiteAIEvent(mapCell* cell, hero* eventHero) {
    i32 cost16[IDX(RES_COUNT)];
    i32 purchaseCount17;
    u32 packedSite17;
    i16 availableCount1;
    H2_ENUM_STORAGE(RecruitSiteType, u32) siteType3;
    CreatureType creatureType13;
    H2_ENUM_STORAGE_STEPPED(ResourceType, i32) resource27;
    i32 purchaseValue5;
    i32 replacementSlot26;

    siteType3 = cell->m_objectMetadata;
    siteType3 &= EVENT_RECRUIT_TYPE_MASK;
    availableCount1 = static_cast<i16>(cell->m_objectMetadata);
    availableCount1 >>= EVENT_RECRUIT_COUNT_SHIFT;

    switch (siteType3) {
        case RECRUITMENT_SITE_BARROW_MOUNDS:
            creatureType13 = CREATURE_GHOST;
            break;
        case RECRUITMENT_SITE_EARTH_ALTAR:
            creatureType13 = CREATURE_EARTH_ELEMENTAL;
            break;
        case RECRUITMENT_SITE_AIR_ALTAR:
            creatureType13 = CREATURE_AIR_ELEMENTAL;
            break;
        case RECRUITMENT_SITE_FIRE_ALTAR:
            creatureType13 = CREATURE_FIRE_ELEMENTAL;
            break;
        case RECRUITMENT_SITE_WATER_ALTAR:
            creatureType13 = CREATURE_WATER_ELEMENTAL;
            break;
    }

    if (availableCount1 != 0) {
        gpPhilAI->EvaluateOneTimeCreaturePurchase(
            creatureType13,
            availableCount1,
            0,
            purchaseCount17,
            purchaseValue5,
            replacementSlot26
        );
        if (purchaseCount17 > 0) {
            gpGame->GiveArmy(
                &eventHero->m_army,
                creatureType13,
                purchaseCount17,
                replacementSlot26
            );
            availableCount1 = static_cast<i16>(availableCount1 - purchaseCount17);
            packedSite17 = (availableCount1 << EVENT_RECRUIT_COUNT_SHIFT) | IDX(siteType3);
            cell->m_objectMetadata = packedSite17;
            GetMonsterCost(creatureType13, cost16);
            for (resource27 = 0; resource27 < RES_COUNT; resource27++) {
                gpCurPlayer->m_resources[IDX(resource27)] -=
                    cost16[IDX(resource27)] * purchaseCount17;
            }
        }
    }
}

VA(0x004b4e33, 0x1a2)
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

VA(0x004b4fd5, 0x82b)
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
    CreatureType monster_n;
    float strengthRatio_p;
    CombatResult combatResult_f;
    i32 forcedJoin_f;
    i32 joining;
    i32 monsterCount_n;
    i32 joiningCost_i;
    char offerText_g[MONSTER_OFFER_BUFFER_SIZE];

    unused = 0;
    gpMouseManager->ShowColorPointer();
    monster_n = static_cast<CreatureType>(cell->m_objectIndex);
    forcedJoin_f = cell->m_objectMetadata & MONSTER_JOIN_FORCED;
    monsterCount_n = cell->m_objectMetadata & MONSTER_COUNT_MASK;
    strengthRatio_p =
        static_cast<float>(gpPhilAI->FightValueOfStack(&eventHero->m_army, eventHero, 0, 0, 0, 0))
        / static_cast<float>(gMonsterDatabase[IDX(monster_n)].fightValue * monsterCount_n);

    if (gbInCampaign
        && ((gpGame->m_campaignAwards[IDX(CAMPAIGN_AWARD_DWARVEN_ALLIANCE)]
             && (monster_n == CREATURE_DWARF || monster_n == CREATURE_BATTLE_DWARF))
            || (gpGame->m_campaignAwards[IDX(CAMPAIGN_AWARD_OGRE_ALLIANCE)]
                && (monster_n == CREATURE_OGRE || monster_n == CREATURE_OGRE_LORD))
            || (gpGame->m_campaignAwards[IDX(CAMPAIGN_AWARD_DRAGON_ALLIANCE)]
                && (monster_n == CREATURE_GREEN_DRAGON || monster_n == CREATURE_RED_DRAGON
                    || monster_n == CREATURE_BLACK_DRAGON)))) {
        if (!eventHero->m_army.CanJoin(monster_n)) {
            if (monster_n == CREATURE_DWARF || monster_n == CREATURE_BATTLE_DWARF)
                NormalDialog(
                    DATA_COMPGEN(0x0051c3fc, playerMonsterInteractTheDwarvesHailYouAnyFriend, "The dwarves hail you, \"Any friend of Roland is a friend of ours.  You may "
                    "pass.\""),
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
            else if (monster_n == CREATURE_OGRE || monster_n == CREATURE_OGRE_LORD)
                NormalDialog(
                    DATA_COMPGEN(0x0051c450, playerMonsterInteractTheOgresGiveYouAGrunt, "The ogres give you a grunt of recognition, \"Archibald's allies may pass.\""),
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
                    DATA_COMPGEN(0x0051c49c, playerMonsterInteractTheDragonsSeeYouAndCall, "The dragons see you and call out.  \"Our alliance with Archibald compels us "
                    "to join you.  Unfortunately you have no room.  A pity!\"  They quickly "
                    "scatter."),
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
            if (monster_n == CREATURE_DWARF || monster_n == CREATURE_BATTLE_DWARF)
                NormalDialog(
                    DATA_COMPGEN(0x0051c538, playerMonsterInteractTheDwarvesRecognizeTheirAlliesAnd, "The dwarves recognize their allies and gladly join your forces."),
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
            else if (monster_n == CREATURE_OGRE || monster_n == CREATURE_OGRE_LORD)
                NormalDialog(
                    DATA_COMPGEN(0x0051c578, playerMonsterInteractTheOgresRecognizeYouAsThe, "The ogres recognize you as the Dwarfbane and lumber over to join you."),
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
                    DATA_COMPGEN(0x0051c5c0, playerMonsterInteractTheDragonsSnarlingAndGrowlingAgree, "The dragons, snarling and growling, agree to join forces with you, their "
                    "'Ally'."),
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
            eventHero->m_army.Add(monster_n, monsterCount_n, -1);
            *handled = 1;
        }
        return;
    }

    if (gbInCampaign && gpGame->m_campaignAwards[IDX(CAMPAIGN_AWARD_DWARFBANE)]
        && (monster_n == CREATURE_DWARF || monster_n == CREATURE_BATTLE_DWARF)) {
        NormalDialog(
            DATA_COMPGEN(0x0051c614, playerMonsterInteractTheDwarfbaneRunForYourLives, "\"The Dwarfbane!!!!, run for your lives.\""),
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
        && (monster_n == CREATURE_ELF || monster_n == CREATURE_GRAND_ELF)) {
        *handled = 1;
        if (eventHero->m_army.CanJoin(monster_n)) {
            NormalDialog(
                DATA_COMPGEN(0x0051c640, playerMonsterInteractAsYouApproachTheGroupOf, "As you approach the group of elves, their leader calls them all to attention.  He "
                "shouts to them, \"Who of you is brave enough to join this fearless ally of "
                "ours?\"  The group explodes with cheers as they run to join your ranks."),
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
            eventHero->m_army.Add(monster_n, monsterCount_n, -1);
        } else {
            NormalDialog(
                DATA_COMPGEN(0x0051c724, playerMonsterInteractTheElvesStandAtAttentionAs, "The elves stand at attention as you approach.  Their leader calls to you and "
                "says, \"Let us not impede your progress, ally!  Move on, and may victory be "
                "yours.\""),
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

    if (eventHero->m_army.CanJoin(monster_n)
        && strengthRatio_p
            > DATA_COMPGEN(0x004eb930, monsterStrengthJoinThreshold, MONSTER_STRENGTH_JOIN)
        && !eventHero->HasArtifact(ARTIFACT_HIDEOUS_MASK) && monster_n != CREATURE_GHOST
        && monster_n != CREATURE_EARTH_ELEMENTAL && monster_n != CREATURE_AIR_ELEMENTAL
        && monster_n != CREATURE_FIRE_ELEMENTAL && monster_n != CREATURE_WATER_ELEMENTAL) {
        if (forcedJoin_f) {
            sprintf(gText, gEventText[EVENT_TEXT_FOLLOWERS], gArmyNamesPlural[IDX(monster_n)]);
            EventWindow(-1, NORMAL_DIALOG_CONFIRM, gText, -1, 0, -1, 0, -1);
            if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
                eventHero->m_army.Add(monster_n, monsterCount_n, -1);
                *handled = 1;
                return;
            } else {
                EventWindow(EVENT_TEXT_MONSTER_REFUSAL, NORMAL_DIALOG_INFO, DATA_COMPGEN(0x0051c7c4, playerMonsterInteractEmptyString, ""), -1, 0, -1, 0, -1);
                goto fightMonsters;
            }
        } else if (eventHero->m_secondarySkills[IDX(HERO_SKILL_DIPLOMACY)]
                   != HERO_SKILL_LEVEL_NONE) {
            if (eventHero->m_secondarySkills[IDX(HERO_SKILL_DIPLOMACY)]
                == HERO_SKILL_LEVEL_EXPERT)
                joining = monsterCount_n;
            else if (eventHero->m_secondarySkills[IDX(HERO_SKILL_DIPLOMACY)]
                     == HERO_SKILL_LEVEL_ADVANCED)
                joining = monsterCount_n / MONSTER_DIPLOMACY_ADVANCED_JOIN_DIVISOR;
            else
                joining = monsterCount_n / MONSTER_DIPLOMACY_BASIC_JOIN_DIVISOR;
            if (!joining)
                joining = 1;

            joiningCost_i = gMonsterDatabase[IDX(monster_n)].cost * monsterCount_n;
            if (joiningCost_i > gpGame->m_players[eventHero->m_owner].m_resources[IDX(RES_GOLD)]) {
                if (strengthRatio_p
                    > DATA_COMPGEN(
                        0x004eb938, monsterStrengthFleeThreshold, MONSTER_STRENGTH_FLEE
                    ))
                    goto monstersFlee;
                else
                    goto fightMonsters;
            }

            if (monsterCount_n == 1) {
                sprintf(
                    gText,
                    DATA_COMPGEN(0x0051c7c8, playerMonsterInteractTheSIsSwayedByYour, "The %s is swayed by your diplomatic tongue, and offers to join your army for "
                    "the sum of %d gold.  Do you accept?"),
                    gArmyNames[IDX(monster_n)],
                    joiningCost_i
                );
            } else {
                sprintf(
                    gText,
                    DATA_COMPGEN(0x0051c83c, playerMonsterInteractTheCreaturesAreSwayedByYour, "The creatures are swayed by your diplomatic tongue, and make you an offer:\n\n")
                );
                if (monsterCount_n == joining)
                    sprintf(
                        offerText_g,
                        DATA_COMPGEN(0x0051c88c, playerMonsterInteractAllDOfTheSWill, "All %d of the %s will join your army for the sum of %d gold.  Do you "
                        "accept?"),
                        monsterCount_n,
                        gArmyNamesPlural[IDX(monster_n)],
                        joiningCost_i
                    );
                else
                    sprintf(
                        offerText_g,
                        DATA_COMPGEN(0x0051c8dc, playerMonsterInteractDOfTheDSWill, "%d of the %d %s will join your army, and the rest will leave you alone, "
                        "for the sum of %d gold.  Do you accept?"),
                        joining,
                        monsterCount_n,
                        gArmyNamesPlural[IDX(monster_n)],
                        joiningCost_i
                    );
                strcat(gText, offerText_g);
            }

            NormalDialog(
                gText,
                NORMAL_DIALOG_CONFIRM,
                -1,
                -1,
                IDX(RES_GOLD),
                joiningCost_i,
                -1,
                0,
                -1,
                0
            );
            if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
                eventHero->m_army.Add(monster_n, joining, -1);
                *handled = 1;
                gpGame->m_players[eventHero->m_owner].m_resources[IDX(RES_GOLD)] -= joiningCost_i;
                return;
            } else {
                EventWindow(EVENT_TEXT_MONSTER_REFUSAL, NORMAL_DIALOG_INFO, DATA_COMPGEN(0x0051c94c, playerMonsterInteractEmptyString2, ""), -1, 0, -1, 0, -1);
                goto fightMonsters;
            }
        }
    }

    if (strengthRatio_p > MONSTER_STRENGTH_FLEE) {
    monstersFlee:
        sprintf(
            gText,
            DATA_COMPGEN(0x0051c950, playerMonsterInteractTheSAwedByThePower, "The %s, awed by the power of your forces, begin to scatter.  Do you wish to pursue "
            "and engage them?"),
            gArmyNamesPlural[IDX(monster_n)]
        );
        EventWindow(-1, NORMAL_DIALOG_CONFIRM, gText, -1, 0, -1, 0, -1);
        if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES)
            goto fightMonsters;
        *handled = 1;
        return;
    }

fightMonsters:
    combatResult_f = CombatMonsterEvent(
        eventHero,
        monster_n,
        monsterCount_n,
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
    if (combatResult_f == COMBAT_RESULT_ATTACKER || combatResult_f == COMBAT_RESULT_DRAW)
        *handled = 1;
}

VA(0x004b5800, 0x440)
void advManager::ComputerMonsterInteract(mapCell* cell, hero* eventHero, i32* handled) {
    i32 joiningCount;
    i32 purchaseCount;
    i32 monsterCount[MONSTER_COMBAT_VALUE_COUNT];
    float strengthRatio;
    i32 purchaseValue;
    i32 replacementSlot;
    u32 forcedJoin;
    CreatureType monsterType;

    monsterType = static_cast<CreatureType>(cell->m_objectIndex);
    monsterCount[MONSTER_COMBAT_REMAINING_COUNT] = cell->m_objectMetadata & MONSTER_COUNT_MASK;
    forcedJoin = cell->m_objectMetadata & MONSTER_JOIN_FORCED;
    strengthRatio =
        static_cast<float>(gpPhilAI->FightValueOfStack(&eventHero->m_army, eventHero, 0, 0, 0, 0))
        / static_cast<float>(
            gMonsterDatabase[IDX(monsterType)].fightValue
            * monsterCount[MONSTER_COMBAT_REMAINING_COUNT]
        );

    if (eventHero->m_army.CanJoin(monsterType)
        && !eventHero->HasArtifact(ARTIFACT_HIDEOUS_MASK) && strengthRatio > MONSTER_STRENGTH_JOIN
        && monsterType != CREATURE_GHOST && monsterType != CREATURE_EARTH_ELEMENTAL
        && monsterType != CREATURE_AIR_ELEMENTAL && monsterType != CREATURE_FIRE_ELEMENTAL
        && monsterType != CREATURE_WATER_ELEMENTAL) {
        if (forcedJoin) {
            gpPhilAI->EvaluateOneTimeCreaturePurchase(
                monsterType,
                monsterCount[MONSTER_COMBAT_REMAINING_COUNT],
                1,
                purchaseCount,
                purchaseValue,
                replacementSlot
            );
            if (purchaseCount > 0) {
                gpGame->GiveArmy(
                    &eventHero->m_army,
                    monsterType,
                    monsterCount[MONSTER_COMBAT_REMAINING_COUNT],
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
                    joiningCount = monsterCount[MONSTER_COMBAT_REMAINING_COUNT];
                else if (eventHero->m_secondarySkills[IDX(HERO_SKILL_DIPLOMACY)]
                         == HERO_SKILL_LEVEL_ADVANCED)
                    joiningCount = monsterCount[MONSTER_COMBAT_REMAINING_COUNT]
                                   / MONSTER_DIPLOMACY_ADVANCED_JOIN_DIVISOR;
                else
                    joiningCount = monsterCount[MONSTER_COMBAT_REMAINING_COUNT]
                                   / MONSTER_DIPLOMACY_BASIC_JOIN_DIVISOR;
                if (!joiningCount)
                    joiningCount = 1;

                i32 joiningCost = static_cast<i32>(
                    gMonsterDatabase[IDX(monsterType)].cost * joiningCount
                    * DATA_COMPGEN(0x004eb940, computerMonsterInteractConstant, MONSTER_AI_JOIN_COST_FRACTION)
                );
                if (gpGame->m_players[eventHero->m_owner].m_resources[IDX(RES_GOLD)]
                    < joiningCost) {
                    if (strengthRatio > MONSTER_STRENGTH_FLEE)
                        goto computerMonstersFlee;
                    goto fightComputerMonsters;
                }
                gpPhilAI->EvaluateOneTimeCreaturePurchase(
                    monsterType,
                    monsterCount[MONSTER_COMBAT_REMAINING_COUNT],
                    1,
                    purchaseCount,
                    purchaseValue,
                    replacementSlot
                );
                if (purchaseCount > 0) {
                    gpGame->m_players[eventHero->m_owner].m_resources[IDX(RES_GOLD)] -= joiningCost;
                    gpGame->GiveArmy(
                        &eventHero->m_army,
                        monsterType,
                        joiningCount,
                        replacementSlot
                    );
                    *handled = 1;
                }
            }
        }
    }

    if (strengthRatio > MONSTER_STRENGTH_FLEE) {
    computerMonstersFlee:
        gpAdvManager->GiveExperience(
            eventHero,
            gMonsterDatabase[IDX(monsterType)].hitPoints
                * monsterCount[MONSTER_COMBAT_REMAINING_COUNT],
            1
        );
        eventHero->CheckLevel();
        if (eventHero->GetSSLevel(HERO_SKILL_NECROMANCY)
            && eventHero->m_army.CanJoin(CREATURE_SKELETON)) {
            gpGame->GiveArmy(
                &eventHero->m_army,
                CREATURE_SKELETON,
                static_cast<i32>(
                    static_cast<double>(monsterCount[MONSTER_COMBAT_REMAINING_COUNT])
                    * eventHero->GetSSLevel(HERO_SKILL_NECROMANCY)
                    * DATA_COMPGEN(0x004eb948, computerMonsterInteractConstant2, MONSTER_NECROMANCY_FRACTION)
                ),
                -1
            );
        }
        *handled = 1;
    } else {
    fightComputerMonsters:
        monsterCount[MONSTER_COMBAT_RESULT] =
            gpPhilAI->CombatMonsterEvent(eventHero, monsterType, monsterCount, cell);
        if (monsterCount[MONSTER_COMBAT_RESULT] != 0) {
            *handled = 1;
            return;
        }
        cell->m_objectMetadata = ((cell->m_objectMetadata & MONSTER_JOIN_FORCED)
                                  + (static_cast<u16>(monsterCount[MONSTER_COMBAT_REMAINING_COUNT])
                                     & MONSTER_COUNT_MASK))
                                 | 0;
    }
}

VA(0x004b5c40, 0x1d0)
i32 advManager::DoNetCombat(char* packet) {
    // Original-source anchor consumed by H2_FREE line reconstruction.
    DATA(0x0051c9b4) static i16 sourceLineBase = 0x1655; // NOLINT(readability-magic-numbers)
    hero* secondHero9;
    i32 setupCombatY8;
    i32 combatX1;
    i32 combatY8;
    i32 randomSeed1;
    H2_ENUM_STORAGE(CombatResult, i8) combatResult16;
    i32 setupCombatX11;
    hero* firstHero29;
    i32 remotePlayer8;
    i32 firstPlayer3;
    armyGroup* secondArmy10;
    armyGroup* firstArmy6;
    town* combatTown;
    i32 size;
    i32 result;

    firstHero29 = NULL;
    firstArmy6 = NULL;
    combatTown = NULL;
    secondHero9 = NULL;
    secondArmy10 = NULL;
    ReceiveHeroTownData(
        packet,
        &remotePlayer8,
        &combatX1,
        &combatY8,
        &firstHero29,
        &firstArmy6,
        &combatTown,
        &secondHero9,
        &secondArmy10,
        &setupCombatX11,
        &setupCombatY8,
        &randomSeed1,
        &combatResult16,
        &gbRetreatWin,
        &gbCombatSurrender
    );
    firstPlayer3 = firstHero29->m_owner;
    combatResult16 = DoCombat(
        combatX1,
        combatY8,
        firstHero29,
        firstArmy6,
        combatTown,
        secondHero9,
        secondArmy10,
        setupCombatX11,
        setupCombatY8,
        randomSeed1,
        0
    );
    if (!gbHumanPlayer[firstPlayer3]) {
        SendHeroTownData(
            combatX1,
            combatY8,
            firstHero29,
            firstArmy6,
            combatTown,
            secondHero9,
            secondArmy10,
            setupCombatX11,
            setupCombatY8,
            randomSeed1,
            remotePlayer8,
            combatResult16,
            gbRetreatWin,
            gbCombatSurrender
        );
    }
    if (firstArmy6)
        H2_FREE_AT(firstArmy6, DATA_COMPGEN(0x0051c9b8, doNetCombatSourceFile, RETAIL_FILE), sourceLineBase + 0x46);
    if (secondArmy10)
        H2_FREE_AT(secondArmy10, DATA_COMPGEN(0x0051c9e4, doNetCombatSourceFile2, RETAIL_FILE), sourceLineBase + 0x49);
    if (combatTown)
        H2_FREE_AT(combatTown, DATA_COMPGEN(0x0051ca10, doNetCombatSourceFile3, RETAIL_FILE), sourceLineBase + 0x4c);
    if (secondHero9)
        H2_FREE_AT(secondHero9, DATA_COMPGEN(0x0051ca3c, doNetCombatSourceFile4, RETAIL_FILE), sourceLineBase + 0x4f);
    if (firstHero29)
        H2_FREE_AT(firstHero29, DATA_COMPGEN(0x0051ca68, doNetCombatSourceFile5, RETAIL_FILE), sourceLineBase + 0x52);
    gbRetreatWin = false;
    return 1;
}

VA(0x004b5e10, 0x64e)
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
    // Original-source anchor consumed by H2_FREE line reconstruction.
    DATA(0x0051ca94) static i16 sourceLineBase = 0x16ba; // NOLINT(readability-magic-numbers)
    armyGroup* receivedSecondArmy2;
    hero* receivedSecondHero9;
    hero* receivedFirstHero1;
    armyGroup* receivedFirstArmy;
    town* receivedTown;
    i32 remotePlayer;
    char* packet8;
    H2_ENUM_STORAGE(CombatResult, i8) combatResult3;
    tag_message message9;
    i32 secondPlayer7;
    i32 firstPlayer4;
    i32 savedPlayer1;
    i32 savedShowIt_e;
    i32 unusedCombat_a;

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
    secondPlayer7 = secondHero ? secondHero->m_owner : (combatTown ? combatTown->m_owner : -1);
    if (randomSeed == -1)
        randomSeed = Random(1, COMBAT_RANDOM_SEED_MAX);
    DemobilizeCurrHero();
    savedPlayer1 = giCurPlayer;
    savedShowIt_e = bShowIt;

    if (firstPlayer4 >= 0 && secondPlayer7 >= 0 && gbHumanPlayer[secondPlayer7]) {
        if (!gbThisNetHumanPlayer[secondPlayer7]) {
            iCombatControlNetPos[IDX(COMBAT_ATTACKER_SIDE)] = giThisNetPos;
            iCombatControlNetPos[IDX(COMBAT_DEFENDER_SIDE)] = gbGamePosToNetPos[secondPlayer7];
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
                gbGamePosToNetPos[secondPlayer7],
                COMBAT_RESULT_ATTACKER, // Placeholder ignored until the remote combat finishes.
                0,
                0
            );
            if (!gbHumanPlayer[firstPlayer4]) {
                for (;;) {
                    PollSound();
                    FillBitmapArea(
                        gpWindowManager->m_screen,
                        COMBAT_NETWORK_POLL_X,
                        COMBAT_NETWORK_POLL_Y,
                        COMBAT_NETWORK_POLL_WIDTH,
                        COMBAT_NETWORK_POLL_HEIGHT,
                        0
                    );
                    packet8 = CheckHandleNet();
                    if (packet8) {
                        switch (EVENTS_REMOTE_MESSAGE(packet8)->command) {
                            case REMOTE_COMMAND:
                                ReceiveHeroTownData(
                                    packet8,
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
                                    H2_FREE_AT(receivedFirstArmy, DATA_COMPGEN(0x0051ca98, doCombatSourceFile, RETAIL_FILE), sourceLineBase + 0x71);
                                }
                                if (receivedSecondArmy2) {
                                    memcpy(secondArmy, receivedSecondArmy2, sizeof(armyGroup));
                                    H2_FREE_AT(receivedSecondArmy2, DATA_COMPGEN(0x0051cac4, doCombatSourceFile2, RETAIL_FILE), sourceLineBase + 0x77);
                                }
                                if (receivedTown) {
                                    memcpy(combatTown, receivedTown, sizeof(town));
                                    H2_FREE_AT(receivedTown, DATA_COMPGEN(0x0051caf0, doCombatSourceFile3, RETAIL_FILE), sourceLineBase + 0x7d);
                                }
                                if (receivedSecondHero9) {
                                    memcpy(secondHero, receivedSecondHero9, sizeof(hero));
                                    H2_FREE_AT(receivedSecondHero9, DATA_COMPGEN(0x0051cb1c, doCombatSourceFile4, RETAIL_FILE), sourceLineBase + 0x83);
                                }
                                if (receivedFirstHero1) {
                                    memcpy(firstHero, receivedFirstHero1, sizeof(hero));
                                    H2_FREE_AT(receivedFirstHero1, DATA_COMPGEN(0x0051cb48, doCombatSourceFile5, RETAIL_FILE), sourceLineBase + 0x89);
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
                DATA_COMPGEN(0x0051cb84, doCombatSSSIsUnderAttack, "%s's %s is under attack!"),
                cPlayerNames[secondPlayer7],
                combatTown ? DATA_COMPGEN(0x0051cb74, doCombatTown, "Town") : DATA_COMPGEN(0x0051cb7c, doCombatHero, "Hero")
            );
            gpGame->WaitForPlayer(gText, secondPlayer7);
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
    gpMouseManager->SetPointer(const_cast<char*>(DATA_COMPGEN(0x0051cba0, doCombatAdvmiceMse, "advmice.mse")), 0, MOUSE_AUTO_CURSOR_TYPE);
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
    bShowIt = savedShowIt_e;
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

VA(0x004b645e, 0x36f)
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
    // Original-source anchor consumed by H2_ALLOC/H2_FREE line reconstruction.
    DATA(0x0051cbac) static i16 sourceLineBase = 0x17cc; // NOLINT(readability-magic-numbers)
    char* reply;
    i32 result;
    combatRemoteData* buffer = NULL;

    buffer = static_cast<combatRemoteData*>(H2_ALLOC_AT(COMBAT_REMOTE_BUFFER_SIZE, DATA_COMPGEN(0x0051cbb0, sendHeroTownDataSourceFile, RETAIL_FILE), sourceLineBase + 3));
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
    if (firstHero) {
        buffer->firstOwner = firstHero->m_owner;
    } else {
        buffer->firstOwner = -1;
    }
    if (firstHero) {
        buffer->firstGold = gpGame->m_players[firstHero->m_owner].m_resources[IDX(RES_GOLD)];
    } else {
        buffer->firstGold = 0;
    }
    if (secondHero) {
        buffer->secondOwner = secondHero->m_owner;
    } else {
        buffer->secondOwner = -1;
    }
    if (secondHero) {
        buffer->secondGold = gpGame->m_players[secondHero->m_owner].m_resources[IDX(RES_GOLD)];
    } else {
        buffer->secondGold = 0;
    }
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
    H2_FREE_AT(buffer, DATA_COMPGEN(0x0051cbdc, sendHeroTownDataSourceFile2, RETAIL_FILE), sourceLineBase + 0x5c);
}

VA(0x004b67cd, 0x462)
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
    // Original-source anchor consumed by H2_ALLOC line reconstruction.
    DATA(0x0051cc08) static i16 sourceLineBase = 0x183d; // NOLINT(readability-magic-numbers)
    i32 hasFirstHero7;
    i32 hasTown0;
    i32 hasSecondHero8;
    i32l lastPacketTime36;
    i32 result7;
    i32 gotFirstHeroFirst3;
    i32 gotFirstHeroSecond9;
    i32 gotSecondHeroFirst13;
    i32 gotSecondHeroSecond6;
    i32 firstOwner29;
    i32 secondOwner28;

    *firstHero = NULL;
    *firstArmy = NULL;
    *combatTown = NULL;
    *secondHero = NULL;
    *secondArmy = NULL;
    hasFirstHero7 = hasSecondHero8 = hasTown0 = 0;
    *remotePlayer = EVENTS_REMOTE_MESSAGE(packet)->sender;
    *x = EVENTS_REMOTE_COMBAT(packet)->x;
    *y = EVENTS_REMOTE_COMBAT(packet)->y;
    hasFirstHero7 = EVENTS_REMOTE_COMBAT(packet)->hasFirstHero;
    hasTown0 = EVENTS_REMOTE_COMBAT(packet)->hasTown;
    hasSecondHero8 = EVENTS_REMOTE_COMBAT(packet)->hasSecondHero;
    *setupCombatX = EVENTS_REMOTE_COMBAT(packet)->setupCombatX;
    *setupCombatY = EVENTS_REMOTE_COMBAT(packet)->setupCombatY;
    *randomSeed = EVENTS_REMOTE_COMBAT(packet)->randomSeed;
    *combatResult = EVENTS_REMOTE_COMBAT(packet)->combatResult;
    *retreatWin = EVENTS_REMOTE_COMBAT(packet)->retreatWin;
    *combatSurrender = EVENTS_REMOTE_COMBAT(packet)->combatSurrender;
    firstOwner29 = EVENTS_REMOTE_COMBAT(packet)->firstOwner;
    if (firstOwner29 > 0)
        gpGame->m_players[firstOwner29].m_resources[IDX(RES_GOLD)] =
            EVENTS_REMOTE_COMBAT(packet)->firstGold;
    secondOwner28 = EVENTS_REMOTE_COMBAT(packet)->secondOwner;
    if (secondOwner28 > 0)
        gpGame->m_players[secondOwner28].m_resources[IDX(RES_GOLD)] =
            EVENTS_REMOTE_COMBAT(packet)->secondGold;

    *firstArmy = static_cast<armyGroup*>(H2_ALLOC_AT(sizeof(armyGroup), DATA_COMPGEN(0x0051cc10, receiveHeroTownDataSourceFile, RETAIL_FILE), sourceLineBase + 0x26));
    memcpy(*firstArmy, &EVENTS_REMOTE_COMBAT(packet)->firstArmy, sizeof(armyGroup));
    *secondArmy = static_cast<armyGroup*>(H2_ALLOC_AT(sizeof(armyGroup), DATA_COMPGEN(0x0051cc3c, receiveHeroTownDataSourceFile2, RETAIL_FILE), sourceLineBase + 0x29));
    memcpy(*secondArmy, &EVENTS_REMOTE_COMBAT(packet)->secondArmy, sizeof(armyGroup));
    if (hasTown0) {
        *combatTown = static_cast<town*>(H2_ALLOC_AT(sizeof(town), DATA_COMPGEN(0x0051cc68, receiveHeroTownDataSourceFile3, RETAIL_FILE), sourceLineBase + 0x2e));
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

    lastPacketTime36 = KBTickCount();
    gotFirstHeroFirst3 = 1;
    gotFirstHeroSecond9 = 1;
    gotSecondHeroFirst13 = 1;
    gotSecondHeroSecond6 = 1;
    if (hasFirstHero7) {
        *firstHero = static_cast<hero*>(H2_ALLOC_AT(sizeof(hero), DATA_COMPGEN(0x0051cc94, receiveHeroTownDataSourceFile4, RETAIL_FILE), sourceLineBase + 0x47));
        gotFirstHeroFirst3 = 0;
        gotFirstHeroSecond9 = 0;
    }
    if (hasSecondHero8) {
        *secondHero = static_cast<hero*>(H2_ALLOC_AT(sizeof(hero), DATA_COMPGEN(0x0051ccc0, receiveHeroTownDataSourceFile5, RETAIL_FILE), sourceLineBase + 0x4d));
        gotSecondHeroFirst13 = 0;
        gotSecondHeroSecond6 = 0;
    }

    while (!gotFirstHeroFirst3 || !gotFirstHeroSecond9 || !gotSecondHeroFirst13
           || !gotSecondHeroSecond6) {
        PollSound();
        if (lastPacketTime36 + COMBAT_REMOTE_TIMEOUT < KBTickCount()) {
            NormalDialog(
                const_cast<char*>(DATA_COMPGEN(0x0051ccec, receiveHeroTownDataErrorReceivingDataKeepTrying, "Error receiving data.  Keep trying?")),
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
                lastPacketTime36 = KBTickCount();
            else
                ShutDown(const_cast<char*>(DATA_COMPGEN(0x0051cd10, receiveHeroTownDataGameCanceled, "Game canceled.")));
        }
        packet = GetRemoteData(1);
        if (packet && EVENTS_REMOTE_MESSAGE(packet)->type == REMOTE_MESSAGE_RELIABLE
            && EVENTS_REMOTE_MESSAGE(packet)->command == REMOTE_COMMAND) {
            lastPacketTime36 = KBTickCount();
            if (EVENTS_REMOTE_HERO(packet)->fragment == REMOTE_FIRST_HERO_FIRST) {
                memcpy(*firstHero, EVENTS_REMOTE_HERO(packet)->data, COMBAT_REMOTE_HERO_FIRST_SIZE);
                gotFirstHeroFirst3 = 1;
            }
            if (EVENTS_REMOTE_HERO(packet)->fragment == REMOTE_FIRST_HERO_SECOND) {
                memcpy(
                    &(*firstHero)->m_spells[COMBAT_REMOTE_HERO_SECOND_SPELL_INDEX],
                    EVENTS_REMOTE_HERO(packet)->data,
                    COMBAT_REMOTE_HERO_SECOND_SIZE
                );
                gotFirstHeroSecond9 = 1;
            }
            if (EVENTS_REMOTE_HERO(packet)->fragment == REMOTE_SECOND_HERO_FIRST) {
                memcpy(
                    *secondHero,
                    EVENTS_REMOTE_HERO(packet)->data,
                    COMBAT_REMOTE_HERO_FIRST_SIZE
                );
                gotSecondHeroFirst13 = 1;
            }
            if (EVENTS_REMOTE_HERO(packet)->fragment == REMOTE_SECOND_HERO_SECOND) {
                memcpy(
                    &(*secondHero)->m_spells[COMBAT_REMOTE_HERO_SECOND_SPELL_INDEX],
                    EVENTS_REMOTE_HERO(packet)->data,
                    COMBAT_REMOTE_HERO_SECOND_SIZE
                );
                gotSecondHeroSecond6 = 1;
            }
        }
    }
}

VA(0x004b6c2f, 0x254)
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
    i32 savedShowIt37;

    gbNoShowCombat = true;
    savedShowIt37 = bShowIt;
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
    bShowIt = savedShowIt37;
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

VA(0x004b6e83, 0xb8)
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

DATA(0x0051cc0c) b32 gbNoShowCombat = false;

#undef RETAIL_FILE
