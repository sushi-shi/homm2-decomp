#include <va.h>
#include <SOURCE/KB_TYPES.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <BASE/message.h>
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
#include <BASE/dialog.h>
#include <EDITOR/mapcell.h>

#define GENERIC_SITE_SIREN_ARMY_REMAINDER                                          \
    0.7
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
        EVENT_TELEPORT_STONE_DISTANCE = 1,
        EVENT_TELEPORT_WHIRLPOOL_DISTANCE = 3,
        EVENT_ABANDONED_MINE_ARMY_DIVISOR = 5,
        EVENT_OBSERVATION_RADIUS = 20,
        EVENT_RANDOM_EVENT_SUCCESS = 40,
        EVENT_RANDOM_PERCENT_MAX = 100,
        EVENT_BOAT_RESTORE_MODE = 3,
        EVENT_CURSED_ARTIFACT_COST = 750,
        EVENT_CURSED_ARTIFACT_GOLD_THRESHOLD = 1500,
        EVENT_SIRENS_MIN_ARMY_QUANTITY = 1,
        EVENT_CREATURE_UPGRADE_MOBILITY = 400,
    H2_ENUM_END(AIEventConstant)

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
        RIDDLE_EXPECTED_BUFFER_SIZE = RIDDLE_PREFIX_LENGTH + 1
    H2_ENUM_END(RiddleComparisonConstant)

    H2_ENUM_BEGIN(BarrierEventConstant)
        PASSWORD_SHIFT = 3,
        INPUT_LENGTH = 14
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
        MONSTER_RANDOM_MAX = 100,
        MONSTER_REDUCED_STACK_CHANCE = 25,
        MONSTER_REDUCED_STACK_COUNT = 3,
        MONSTER_FOUR_STACK_THRESHOLD = 50,
        MONSTER_FOUR_STACK_COUNT = 4,
        MONSTER_UPGRADE_CHANCE = 50
    H2_ENUM_END(CombatMonsterEventConstant)

    H2_ENUM_BEGIN(ArtifactPickupSoundRange)
        ARTIFACT_PICKUP_SOUND_FIRST = 1,
        ARTIFACT_PICKUP_SOUND_LAST = 7
    H2_ENUM_END(ArtifactPickupSoundRange)

    H2_ENUM_BEGIN(FlotsamConstant)
        FLOTSAM_WOOD_AMOUNT = 5,
        FLOTSAM_GOLD_AMOUNT = 200,
        FLOTSAM_LARGE_WOOD_AMOUNT = 10,
        FLOTSAM_LARGE_GOLD_AMOUNT = 500
    H2_ENUM_END(FlotsamConstant)

    H2_ENUM_BEGIN(DoEventConstant)
        ARTESIAN_SPRING_MANA_MULTIPLIER = 2,
        XANADU_DIPLOMACY_MULTIPLIER = 2,
        SIGN_MINIMUM_TEXT_LENGTH = 1,
        SIGN_RANDOM_TEXT_COUNT = 4,
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
        DRAGON_CITY_ARCHIBALD_SCENARIO = 7,
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

#if H2_RETAIL_COMPILER
#define adjacentMonster adjacentMonster_j
#define artifactResourceType artifactResourceType_k
#define boat boat_k
#define correctAnswer correctAnswer_e
#define dragonFactor dragonFactor_i
#define eraseObject eraseObject_l
#define eventArtifact artifact_g
#define eventExtra eventExtra_o
#define eventHero eventHero2
#define eventResourceType resourceType_a
#define eventSample eventSample_f
#define eventText eventText_b
#define eventType eventType_g
#define eventValue eventValue1
#define firstUpgrade firstUpgrade_e
#define fizzleType fizzleType_k
#define guardedCount guardedCount_i
#define guardedMonster guardedMonster_c
#define heroCombatResult heroCombatResult_h
#define heroLevel heroLevel_e
#define levelExperience levelExperience_g
#define mineCombatResult mineCombatResult_b
#define monsterType monsterType_f
#define occupiedTown occupiedTown_b
#define oracleMessage oracleMessage_o
#define oracleWindow oracleWindow_o
#define otherHero otherHero_e
#define playedSample playedSample3
#define primaryAmount primaryAmount_j
#define primaryReward primaryReward_e
#define resourceAmount resourceAmount_o
#define secondUpgrade secondUpgrade1
#define secondaryAmount secondaryAmount_j
#define secondaryReward secondaryReward_k
#define signExtra signExtra_k
#define skeletonCell skeletonCell_e
#define sphinxAnswer sphinxAnswer_a
#define springSpellPoints springSpellPoints_j
#define teleportCount teleportCount_e
#define teleportX teleportX_e
#define teleportY teleportY_e
#define thirdUpgrade thirdUpgrade_f
#define wellSpellPoints wellSpellPoints_o
#define zombieCell zombieCell_o
#endif
VA(0x0043b640, 0x5fc9)
void advManager::DoEvent(mapCell* cell, i32 x, i32 y) {
    char eventText[EVENT_TEXT_BUFFER_SIZE];
    mapCell* skeletonCell;
    mapCell* zombieCell;
    town* occupiedTown;
    i32 heroLevel;
    i32 levelExperience;
    i32 dragonFactor;
    CombatResult mineCombatResult;
    i32 adjacentMonster;
    i32 wellSpellPoints;
    i32 springSpellPoints;
    i32 secondaryAmount;
    i32 secondaryReward;
    i32 primaryReward;
    i32 primaryAmount;
    b32 correctAnswer;
    boatRecord* boat;
    i32 teleportCount;
    ResourceType eventResourceType;
    heroWindow* oracleWindow;
    i32 guardedCount;
    i32 teleportY;
    i32 teleportX;
    ArtifactType eventArtifact;
    i32 artifactResourceType;
    CreatureType monsterType;
    hero* otherHero;
    CombatResult heroCombatResult;
    mapEventExtra* eventExtra;
    i32 resourceAmount;
    CreatureType guardedMonster;
    tag_message oracleMessage;
    MapObjectType eventType;
    tag_message H2_UNUSED(unusedEventMessage);
    i32 eraseObject;
    char sphinxAnswer[SPHINX_ANSWER_BUFFER_SIZE];
    CreatureType secondUpgrade;
    SAMPLE2 playedSample;
    CreatureType firstUpgrade;
    b32 fizzleType;
    i32 eventValue;
    signEventExtra* signExtra;
    CreatureType thirdUpgrade;
    hero* eventHero;
    SAMPLE2 eventSample;

    eventHero = &gpGame->m_heroRecs[gpCurPlayer->m_currentHero];
    eventType = cell->m_triggerType & MAP_TRIGGER_TYPE_MASK;
    eraseObject = 0;
    fizzleType = false;
    playedSample = NULL;
    eventSample = NULL;
    gpMouseManager->ShowColorPointer();
    gpMouseManager->SetPointer(0);

    switch (eventType) {
        case MAP_OBJECT_TRADING_POST:
            EventSound(eventType, cell->m_objectMetadata, &eventSample);
            DoTradingPost(0, TRADING_POST_EFFICIENCY);
            break;

        case MAP_OBJECT_MAGIC_GARDEN:
            if (cell->m_objectMetadata == MAP_EVENT_DATA_EMPTY) {
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    localization::Tr("event.inline.14d0296e539d777e"),
                    -1,
                    0,
                    -1,
                    0,
                    -1
                );
            } else {
                EventSound(eventType, cell->m_objectMetadata, &eventSample);
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    localization::Tr("event.inline.835273ee016ddfeb"),
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

        case MAP_OBJECT_SPHINX:
            EventSound(eventType, cell->m_objectMetadata, &eventSample);
            eventExtra = static_cast<mapEventExtra*>(ppMapExtra[cell->m_objectMetadata]);
            if (!eventExtra->active) {
                NormalDialog(
                    localization::Tr("event.inline.1553297e35c659a6"),
                    NORMAL_DIALOG_INFO
                );
                break;
            } else {
                sprintf(
                    gText,
                    localization::Tr("event.inline.c15ae0e09ee99d18")
                );
                NormalDialog(gText, NORMAL_DIALOG_CONFIRM);
                if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_YES) {
                    sprintf(
                        gText,
                        localization::Tr("event.inline.785dc53c14bdbc91"),
                        eventExtra->riddle
                    );
                    GetDataEntry(gText, sphinxAnswer, SPHINX_INPUT_LENGTH, NULL, 0, 1);
                    correctAnswer = false;
                    for (eventValue = 0; eventValue < eventExtra->answerCount; eventValue++) {
                        if (RiddleStringsEqual(sphinxAnswer, eventExtra->answers[eventValue]))
                            correctAnswer = true;
                    }

                    if (correctAnswer) {
                        primaryReward = MAP_EVENT_REWARD_NONE;
                        primaryAmount = 0;
                        secondaryReward = MAP_EVENT_REWARD_NONE;
                        secondaryAmount = 0;
                        for (eventValue = 0; eventValue < SPHINX_RESOURCE_COUNT;
                             eventValue++) {
                            gpGame->m_players[giCurPlayer].m_resources[eventValue] +=
                                eventExtra->resources[eventValue];
                            if (gpGame->m_players[giCurPlayer].m_resources[eventValue] < 0)
                                gpGame->m_players[giCurPlayer].m_resources[eventValue] = 0;
                            if (eventExtra->resources[eventValue] != 0) {
                                if (primaryReward != MAP_EVENT_REWARD_NONE) {
                                    secondaryReward = primaryReward;
                                    secondaryAmount = primaryAmount;
                                }
                                primaryReward = eventValue;
                                primaryAmount = eventExtra->resources[eventValue];
                            }
                        }

                        if (eventExtra->artifact != MAP_EVENT_REWARD_NONE
                            && eventHero->NumArtifacts() < HERO_ARTIFACT_SLOT_COUNT) {
                            GiveArtifact(eventHero, ArtifactType(eventExtra->artifact), true);
                            if (primaryReward != MAP_EVENT_REWARD_NONE) {
                                secondaryReward = primaryReward;
                                secondaryAmount = primaryAmount;
                            }
                            primaryReward = MAP_EVENT_REWARD_ARTIFACT;
                            primaryAmount = eventExtra->artifact;
                        }

                        NormalDialog(
                            localization::Tr("event.inline.b34883b51b1f43be"),
                            NORMAL_DIALOG_INFO,
                            -1,
                            -1,
                            primaryReward,
                            primaryAmount,
                            secondaryReward,
                            secondaryAmount,
                            -1,
                            0
                        );
                        eventExtra->active = 0;
                    } else {
                        NormalDialog(
                            localization::Tr("event.inline.da87bc41bfb2530c"),
                            NORMAL_DIALOG_INFO
                        );
                        HeroLoses(eventHero);
                    }
                }
            }
            break;

        case MAP_OBJECT_OBSERVATION_TOWER:
            EventSound(eventType, cell->m_objectMetadata, &eventSample);
            NormalDialog(
                localization::Tr("event.inline.fff2b25d391ab29d"),
                NORMAL_DIALOG_INFO
            );
            gpGame->SetVisibility(x, y, giCurPlayer, OBSERVATION_TOWER_RADIUS);
            CompleteDraw(0);
            UpdateScreen(0, 0);
            break;

        case MAP_OBJECT_HILL_FORT:
            firstUpgrade = secondUpgrade = thirdUpgrade = CREATURE_NONE;
            if (eventHero->CreatureTypeCount(CREATURE_OGRE))
                firstUpgrade = CREATURE_OGRE;
            if (eventHero->CreatureTypeCount(CREATURE_ORC)) {
                if (firstUpgrade == CREATURE_NONE)
                    firstUpgrade = CREATURE_ORC;
                else
                    secondUpgrade = CREATURE_ORC;
            }
            if (eventHero->CreatureTypeCount(CREATURE_DWARF)) {
                if (firstUpgrade == CREATURE_NONE)
                    firstUpgrade = CREATURE_DWARF;
                else if (secondUpgrade == CREATURE_NONE)
                    secondUpgrade = CREATURE_DWARF;
                else
                    thirdUpgrade = CREATURE_DWARF;
            }

            if (firstUpgrade != CREATURE_NONE) {
                EventSound(eventType, cell->m_objectMetadata, &eventSample);
                eventHero->UpgradeCreatures(CREATURE_OGRE, CREATURE_OGRE_LORD);
                eventHero->UpgradeCreatures(CREATURE_ORC, CREATURE_ORC_CHIEF);
                eventHero->UpgradeCreatures(CREATURE_DWARF, CREATURE_BATTLE_DWARF);
                if (thirdUpgrade != CREATURE_NONE) {
                    sprintf(
                        gText,
                        localization::Tr("event.inline.36d8db3120534c34"),
                        gArmyNamesPlural[IDX(firstUpgrade)],
                        gArmyNamesPlural[IDX(secondUpgrade)],
                        gArmyNamesPlural[IDX(thirdUpgrade)],
                        gArmyNamesPlural[IDX(firstUpgrade) + 1],
                        gArmyNamesPlural[IDX(secondUpgrade) + 1],
                        gArmyNamesPlural[IDX(thirdUpgrade) + 1]
                    );
                } else if (secondUpgrade != CREATURE_NONE) {
                    sprintf(
                        gText,
                        localization::Tr("event.inline.6ec6a4f4693dcb8d"),
                        gArmyNamesPlural[IDX(firstUpgrade)],
                        gArmyNamesPlural[IDX(secondUpgrade)],
                        gArmyNamesPlural[IDX(firstUpgrade) + 1],
                        gArmyNamesPlural[IDX(secondUpgrade) + 1]
                    );
                } else {
                    sprintf(
                        gText,
                        localization::Tr("event.inline.6ffe5f935a4c8867"),
                        gArmyNamesPlural[IDX(firstUpgrade)],
                        gArmyNamesPlural[IDX(firstUpgrade) + 1]
                    );
                }
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    gText,
                    NORMAL_DIALOG_MONSTER,
                    IDX(firstUpgrade) + 1,
                    secondUpgrade == CREATURE_NONE ? -1 : NORMAL_DIALOG_MONSTER,
                    IDX(secondUpgrade) + 1,
                    -1
                );
            } else {
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    localization::Tr("event.inline.dfaca12192a50638"),
                    -1,
                    0,
                    -1,
                    0,
                    -1
                );
            }
            break;

        case MAP_OBJECT_FREEMANS_FOUNDRY:
            firstUpgrade = secondUpgrade = thirdUpgrade = CREATURE_NONE;
            if (eventHero->CreatureTypeCount(CREATURE_PIKEMAN))
                firstUpgrade = CREATURE_PIKEMAN;
            if (eventHero->CreatureTypeCount(CREATURE_SWORDSMAN)) {
                if (firstUpgrade == CREATURE_NONE)
                    firstUpgrade = CREATURE_SWORDSMAN;
                else
                    secondUpgrade = CREATURE_SWORDSMAN;
            }
            if (eventHero->CreatureTypeCount(CREATURE_IRON_GOLEM)) {
                if (firstUpgrade == CREATURE_NONE)
                    firstUpgrade = CREATURE_IRON_GOLEM;
                else if (secondUpgrade == CREATURE_NONE)
                    secondUpgrade = CREATURE_IRON_GOLEM;
                else
                    thirdUpgrade = CREATURE_IRON_GOLEM;
            }

            if (firstUpgrade != CREATURE_NONE) {
                EventSound(eventType, cell->m_objectMetadata, &eventSample);
                eventHero->UpgradeCreatures(CREATURE_PIKEMAN, CREATURE_VETERAN_PIKEMAN);
                eventHero->UpgradeCreatures(
                    CREATURE_SWORDSMAN,
                    CREATURE_MASTER_SWORDSMAN
                );
                eventHero->UpgradeCreatures(CREATURE_IRON_GOLEM, CREATURE_STEEL_GOLEM);
                if (thirdUpgrade != CREATURE_NONE) {
                    sprintf(
                        gText,
                        localization::Tr("event.inline.16a4b831c7bd6a44"),
                        gArmyNamesPlural[IDX(firstUpgrade)],
                        gArmyNamesPlural[IDX(secondUpgrade)],
                        gArmyNamesPlural[IDX(thirdUpgrade)],
                        gArmyNamesPlural[IDX(firstUpgrade) + 1],
                        gArmyNamesPlural[IDX(secondUpgrade) + 1],
                        gArmyNamesPlural[IDX(thirdUpgrade) + 1]
                    );
                } else if (secondUpgrade != CREATURE_NONE) {
                    sprintf(
                        gText,
                        localization::Tr("event.inline.49edf5c404ac2672"),
                        gArmyNamesPlural[IDX(firstUpgrade)],
                        gArmyNamesPlural[IDX(secondUpgrade)],
                        gArmyNamesPlural[IDX(firstUpgrade) + 1],
                        gArmyNamesPlural[IDX(secondUpgrade) + 1]
                    );
                } else {
                    sprintf(
                        gText,
                        localization::Tr("event.inline.95b379d46e666d70"),
                        gArmyNamesPlural[IDX(firstUpgrade)],
                        gArmyNamesPlural[IDX(firstUpgrade) + 1]
                    );
                }
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    gText,
                    NORMAL_DIALOG_MONSTER,
                    IDX(firstUpgrade) + 1,
                    secondUpgrade == CREATURE_NONE ? -1 : NORMAL_DIALOG_MONSTER,
                    IDX(secondUpgrade) + 1,
                    -1
                );
            } else {
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    localization::Tr("event.inline.e7075cc43429178b"),
                    -1,
                    0,
                    -1,
                    0,
                    -1
                );
            }
            break;

        case MAP_OBJECT_SEA_CHEST:
            if ((cell->m_objectMetadata & CHEST_ARTIFACT_FLAG)
                && eventHero->NumArtifacts() < HERO_ARTIFACT_SLOT_COUNT) {
                sprintf(
                    gText,
                    localization::Tr("event.inline.c96f2e927c9e47ef"),
                    gArtifactNames[cell->m_objectMetadata & CHEST_ARTIFACT_MASK]
                );
                NormalDialog(
                    gText,
                    NORMAL_DIALOG_INFO,
                    -1,
                    -1,
                    MAP_EVENT_REWARD_ARTIFACT,
                    cell->m_objectMetadata & CHEST_ARTIFACT_MASK,
                    IDX(RES_GOLD),
                    SEA_CHEST_ARTIFACT_GOLD,
                    -1,
                    0
                );
                GiveArtifact(
                    eventHero,
                    ArtifactType(cell->m_objectMetadata & CHEST_ARTIFACT_MASK),
                    true
                );
                GiveResource(eventHero, RES_GOLD, SEA_CHEST_ARTIFACT_GOLD);
            } else if (cell->m_objectMetadata != SEA_CHEST_OUTCOME_EMPTY) {
                NormalDialog(
                    localization::Tr("event.inline.333bff0ee2591ce7"),
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
                GiveResource(eventHero, RES_GOLD, SEA_CHEST_GOLD);
            } else {
                NormalDialog(
                    localization::Tr("event.inline.d2a7adee0f4b9a8a"),
                    NORMAL_DIALOG_INFO
                );
            }
            fizzleType = true;
            eraseObject = 1;
            break;

        case MAP_OBJECT_FLOTSAM: {
            switch (static_cast<FlotsamReward>(cell->m_objectMetadata)) {
                case FLOTSAM_EMPTY:
                    NormalDialog(
                        localization::Tr("event.inline.fafa7917b1fb1445"),
                        NORMAL_DIALOG_INFO
                    );
                    break;
                case FLOTSAM_WOOD:
                    NormalDialog(
                        localization::Tr("event.inline.021d355fcf4f31af"),
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
                    GiveResource(eventHero, RES_WOOD, FLOTSAM_WOOD_AMOUNT);
                    break;
                case FLOTSAM_WOOD_AND_GOLD:
                    NormalDialog(
                        localization::Tr("event.inline.d19251bcd8000e2e"),
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
                    GiveResource(eventHero, RES_WOOD, FLOTSAM_WOOD_AMOUNT);
                    GiveResource(eventHero, RES_GOLD, FLOTSAM_GOLD_AMOUNT);
                    break;
                case FLOTSAM_LARGE_TREASURE:
                    NormalDialog(
                        localization::Tr("event.inline.bd36cf6691ab88e7"),
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
                    GiveResource(eventHero, RES_WOOD, FLOTSAM_LARGE_WOOD_AMOUNT);
                    GiveResource(eventHero, RES_GOLD, FLOTSAM_LARGE_GOLD_AMOUNT);
                    break;
            }
            fizzleType = true;
            eraseObject = 1;
            break;
        }

        case MAP_OBJECT_SHIPWRECK_SURVIVOR:
            if (eventHero->NumArtifacts() < HERO_ARTIFACT_SLOT_COUNT) {
                sprintf(
                    gText,
                    localization::Tr("event.inline.4967a4ae1966d928"),
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
                GiveArtifact(eventHero, ArtifactType(cell->m_objectMetadata), true);
            } else {
                NormalDialog(
                    localization::Tr("event.inline.12821ba5d6cffff7"),
                    NORMAL_DIALOG_INFO
                );
            }
            fizzleType = true;
            eraseObject = 1;
            break;

        case MAP_OBJECT_MAGELLAN_MAPS:
            if (gpCurPlayer->m_resources[IDX(RES_GOLD)] < MAGELLAN_MAP_COST) {
                NormalDialog(
                    localization::Tr("event.inline.e81b091ba4b1417f"),
                    NORMAL_DIALOG_INFO
                );
            } else {
                EventSound(eventType, cell->m_objectMetadata, &eventSample);
                NormalDialog(
                    localization::Tr("event.inline.19dbe84af3193778"),
                    NORMAL_DIALOG_CONFIRM
                );
                if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_YES) {
                    gpCurPlayer->m_resources[IDX(RES_GOLD)] -= MAGELLAN_MAP_COST;
                    gpGame->MakeAllWaterVisible(giCurPlayer);
                    CompleteDraw(0);
                    UpdateScreen(0, 0);
                }
            }
            break;

        case MAP_OBJECT_WITCH_HUT:
            if (eventHero->m_secondarySkills[cell->m_objectMetadata] != HERO_SKILL_LEVEL_NONE) {
                sprintf(
                    gText,
                    localization::Tr("event.inline.68e1e92f9a9d12d5"),
                    gSecondarySkills[cell->m_objectMetadata]
                );
                NormalDialog(gText, NORMAL_DIALOG_INFO);
            } else if (eventHero->m_secondarySkillCount >= HERO_SECONDARY_SKILL_CAPACITY) {
                sprintf(
                    gText,
                    localization::Tr("event.inline.8432c04b1a757522"),
                    gSecondarySkills[cell->m_objectMetadata]
                );
                NormalDialog(gText, NORMAL_DIALOG_INFO);
            } else {
                EventSound(eventType, cell->m_objectMetadata, &eventSample);
                sprintf(
                    gText,
                    localization::Tr("event.inline.4f16d62af9eceb3d"),
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
                eventHero->GiveSS(
                    static_cast<HeroSecondarySkill>(cell->m_objectMetadata),
                    HERO_SKILL_LEVEL_BASIC
                );
            }
            break;

        case MAP_OBJECT_ARTESIAN_SPRING:
            if (!cell->m_objectMetadata) {
                NormalDialog(
                    localization::Tr("event.inline.87ca55017c24c23b"),
                    NORMAL_DIALOG_INFO
                );
            } else {
                EventSound(eventType, cell->m_objectMetadata, &eventSample);
                cell->m_objectMetadata = 0;
                springSpellPoints = HERO_NORMAL_SPELL_POINTS(*eventHero);
                if (eventHero->m_spellPoints
                    >= springSpellPoints * ARTESIAN_SPRING_MANA_MULTIPLIER) {
                    NormalDialog(
                        localization::Tr("event.inline.ee47b77e069dbc64"),
                        NORMAL_DIALOG_INFO
                    );
                } else {
                    NormalDialog(
                        localization::Tr("event.inline.db769672add76670"),
                        NORMAL_DIALOG_INFO
                    );
                    eventHero->m_spellPoints = springSpellPoints * ARTESIAN_SPRING_MANA_MULTIPLIER;
                }
            }
            break;

        case MAP_OBJECT_MAGIC_WELL:
            if (HAS(eventHero->m_eventFlags, HERO_EVENT_MAGIC_WELL)) {
                NormalDialog(
                    localization::Tr("event.inline.6e512f3b213f3c96"),
                    NORMAL_DIALOG_INFO
                );
            } else {
                EventSound(eventType, cell->m_objectMetadata, &eventSample);
                cell->m_objectMetadata = 0;
                wellSpellPoints = HERO_NORMAL_SPELL_POINTS(*eventHero);
                if (eventHero->m_spellPoints >= wellSpellPoints) {
                    NormalDialog(
                        localization::Tr("event.inline.fa57992068d990d5"),
                        NORMAL_DIALOG_INFO
                    );
                } else {
                    NormalDialog(
                        localization::Tr("event.inline.0ec7b47d176de1a3"),
                        NORMAL_DIALOG_INFO
                    );
                    eventHero->m_eventFlags = eventHero->m_eventFlags | HERO_EVENT_MAGIC_WELL;
                    eventHero->m_spellPoints = wellSpellPoints;
                }
            }
            break;

        case MAP_OBJECT_COAST:
            if (eventHero->IsEmbarked()) {
                eventHero->m_eventFlags = eventHero->m_eventFlags & ~HERO_EVENT_EMBARKED;
                eventHero->m_remainingMobility = 0;
                eventHero->m_direction = m_cursorDirection;
                m_cursorType = eventHero->m_cursorType;
                m_cursorFrame = GetCursorBaseFrame(m_cursorDirection);
                m_cursorActive = true;
                playedSample = LoadPlaySample("killfade.82m");
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
                WaitEndSample(&playedSample);
                CheckAdjacentMon(&adjacentMonster);
            }
            break;

        case MAP_OBJECT_BOAT:
            boat = &gpGame->m_boats[cell->m_objectMetadata];
            gpGame->RestoreCell(
                -1,
                -1,
                boat->savedTriggerType,
                boat->savedEventData,
                cell,
                BOAT_RESTORE_MODE
            );
            eventHero->m_eventFlags = eventHero->m_eventFlags | HERO_EVENT_EMBARKED;
            eventHero->m_remainingMobility = 0;
            boat->heroId = eventHero->m_id;
            boat->owner = eventHero->m_owner;
            m_cursorType = HERO_TYPE_BOAT;
            m_cursorDirection = boat->direction;
            m_cursorFrame = GetCursorBaseFrame(m_cursorDirection);
            m_cursorActive = true;
            CompleteDraw(m_mapOriginX, m_mapOriginY, 0, 1);
            UpdateScreen(0, 0);
            break;

        case MAP_OBJECT_MINE:
            if (gpGame->m_mineOwners[cell->m_objectMetadata] == giCurPlayer)
                break;
            if (gpGame->m_mines[cell->m_objectMetadata].guardianType != CREATURE_NONE) {
                mineCombatResult = CombatMonsterEvent(
                    eventHero,
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
                if (mineCombatResult == COMBAT_RESULT_ATTACKER) {
                    gpGame->m_mines[cell->m_objectMetadata].guardianType = CREATURE_NONE;
                    eventHero->CheckLevel();
                } else {
                    break;
                }
            }
            EventSound(eventType, cell->m_objectMetadata, &eventSample);
            if (gpGame->m_mines[cell->m_objectMetadata].resourceType == MINE_TYPE_GOLD)
                resourceAmount = MINE_GOLD_INCOME;
            else if (gpGame->m_mines[cell->m_objectMetadata].resourceType == MINE_TYPE_ORE)
                resourceAmount = MINE_ORE_INCOME;
            else
                resourceAmount = 1;
            EventWindow(
                IDX(gpGame->m_mines[cell->m_objectMetadata].resourceType)
                    + MINE_RESOURCE_ICON_OFFSET,
                NORMAL_DIALOG_INFO,
                "",
                IDX(gpGame->m_mines[cell->m_objectMetadata].resourceType),
                -resourceAmount,
                -1,
                0,
                -1
            );
            goto claimMine;

        case MAP_OBJECT_ALCHEMIST_LAB:
            if (gpGame->m_mineOwners[cell->m_objectMetadata] == giCurPlayer)
                break;
            EventSound(eventType, cell->m_objectMetadata, &eventSample);
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

        case MAP_OBJECT_SAWMILL:
            if (gpGame->m_mineOwners[cell->m_objectMetadata] == giCurPlayer)
                break;
            EventSound(eventType, cell->m_objectMetadata, &eventSample);
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
        claimMine:
            gpGame->ClaimMine(cell->m_objectMetadata, giCurPlayer);
            break;

        case MAP_OBJECT_LIGHTHOUSE:
            if (gpGame->m_mineOwners[cell->m_objectMetadata] == giCurPlayer)
                break;
            EventSound(eventType, cell->m_objectMetadata, &eventSample);
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
            break;

        case MAP_OBJECT_TREASURE_CHEST:
            if (cell->m_objectMetadata & CHEST_ARTIFACT_FLAG) {
                if (eventHero->NumArtifacts() >= HERO_ARTIFACT_SLOT_COUNT) {
                    cell->m_objectMetadata = CHEST_REWARD_SMALL;
                    goto chestGold;
                }
                sprintf(
                    gText,
                    localization::Tr("event.inline.5ad75e9e7fadd1a6"),
                    gArtifactNames[cell->m_objectMetadata & CHEST_ARTIFACT_MASK]
                );
                NormalDialog(
                    gText,
                    NORMAL_DIALOG_INFO,
                    -1,
                    -1,
                    MAP_EVENT_REWARD_ARTIFACT,
                    cell->m_objectMetadata & CHEST_ARTIFACT_MASK,
                    -1,
                    0,
                    -1,
                    0
                );
                GiveArtifact(
                    eventHero,
                    ArtifactType(cell->m_objectMetadata & CHEST_ARTIFACT_MASK),
                    true
                );
            } else {
            chestGold:
                EventWindow(
                    -1,
                    NORMAL_DIALOG_CONFIRM,
                    localization::Tr("event.inline.f28640c80e1c5d9c"),
                    IDX(RES_GOLD),
                    cell->m_objectMetadata * CHEST_GOLD_MULTIPLIER,
                    NORMAL_DIALOG_EXPERIENCE,
                    (cell->m_objectMetadata - CHEST_EXPERIENCE_LEVEL_OFFSET)
                        * CHEST_EXPERIENCE_MULTIPLIER,
                    NORMAL_DIALOG_SHOW_OR_TEXT
                );
                if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_YES)
                    GiveResource(
                        eventHero,
                        RES_GOLD,
                        cell->m_objectMetadata * CHEST_GOLD_MULTIPLIER
                    );
                else
                    GiveExperience(
                        eventHero,
                        (cell->m_objectMetadata - CHEST_EXPERIENCE_LEVEL_OFFSET)
                            * CHEST_EXPERIENCE_MULTIPLIER,
                        0
                    );
            }
            eraseObject = 1;
            fizzleType = true;
            eventHero->CheckLevel();
            break;

        case MAP_OBJECT_BUOY:
            if (HAS(eventHero->m_eventFlags, HERO_EVENT_BUOY)) {
                EventWindow(
                    EVENT_TEXT_BUOY_VISITED,
                    NORMAL_DIALOG_INFO,
                    "",
                    -1,
                    0,
                    -1,
                    0,
                    -1
                );
            } else {
                EventSound(eventType, cell->m_objectMetadata, &eventSample);
                eventHero->m_eventFlags = eventHero->m_eventFlags | HERO_EVENT_BUOY;
                eventHero->m_morale++;
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
            }
            break;

        case MAP_OBJECT_FAERIE_RING:
            if (HAS(eventHero->m_eventFlags, HERO_EVENT_FAERIE_RING)) {
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
            } else {
                EventSound(eventType, cell->m_objectMetadata, &eventSample);
                eventHero->m_eventFlags = eventHero->m_eventFlags | HERO_EVENT_FAERIE_RING;
                eventHero->m_luck++;
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
            }
            break;

        case MAP_OBJECT_IDOL:
            if (HAS(eventHero->m_eventFlags, HERO_EVENT_IDOL)) {
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    localization::Tr("event.inline.37291a931defe30b"),
                    -1,
                    0,
                    -1,
                    0,
                    -1
                );
            } else {
                EventSound(eventType, cell->m_objectMetadata, &eventSample);
                eventHero->m_eventFlags = eventHero->m_eventFlags | HERO_EVENT_IDOL;
                eventHero->m_luck++;
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    localization::Tr("event.inline.c47e37b66bb1b531"),
                    NORMAL_DIALOG_LUCK_BONUS,
                    0,
                    -1,
                    0,
                    -1
                );
            }
            break;

        case MAP_OBJECT_FOUNTAIN:
            if (HAS(eventHero->m_eventFlags, HERO_EVENT_FOUNTAIN)) {
                EventWindow(
                    EVENT_TEXT_FOUNTAIN_VISITED,
                    NORMAL_DIALOG_INFO,
                    "",
                    -1,
                    0,
                    -1,
                    0,
                    -1
                );
            } else {
                EventSound(eventType, cell->m_objectMetadata, &eventSample);
                eventHero->m_eventFlags = eventHero->m_eventFlags | HERO_EVENT_FOUNTAIN;
                eventHero->m_luck++;
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
            }
            break;

        case MAP_OBJECT_WATERING_HOLE:
            if (HAS(eventHero->m_eventFlags, HERO_EVENT_WATERING_HOLE)) {
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    localization::Tr("event.inline.96fabc1a4bb74b61"),
                    -1,
                    0,
                    -1,
                    0,
                    -1
                );
            } else {
                EventSound(eventType, cell->m_objectMetadata, &eventSample);
                eventHero->m_eventFlags = eventHero->m_eventFlags | HERO_EVENT_WATERING_HOLE;
                eventHero->m_morale++;
                eventHero->m_mobility += WATERING_HOLE_MOBILITY_BONUS;
                eventHero->m_remainingMobility += WATERING_HOLE_MOBILITY_BONUS;
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    localization::Tr("event.inline.d665709349b9262f"),
                    NORMAL_DIALOG_MORALE_BONUS,
                    0,
                    -1,
                    0,
                    -1
                );
            }
            break;

        case MAP_OBJECT_OASIS:
            if (HAS(eventHero->m_eventFlags, HERO_EVENT_OASIS)) {
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    localization::Tr("event.inline.5bee846fe950642e"),
                    -1,
                    0,
                    -1,
                    0,
                    -1
                );
            } else {
                EventSound(eventType, cell->m_objectMetadata, &eventSample);
                eventHero->m_eventFlags = eventHero->m_eventFlags | HERO_EVENT_OASIS;
                eventHero->m_morale++;
                eventHero->m_mobility += OASIS_MOBILITY_BONUS;
                eventHero->m_remainingMobility += OASIS_MOBILITY_BONUS;
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    localization::Tr("event.inline.4a92b05b80d8fdb1"),
                    NORMAL_DIALOG_MORALE_BONUS,
                    0,
                    -1,
                    0,
                    -1
                );
            }
            break;

        case MAP_OBJECT_TEMPLE:
            if (HAS(eventHero->m_eventFlags, HERO_EVENT_TEMPLE)) {
                NormalDialog(
                    localization::Tr("event.inline.9824059f04898b92"),
                    NORMAL_DIALOG_INFO
                );
            } else {
                EventSound(eventType, cell->m_objectMetadata, &eventSample);
                eventHero->m_eventFlags = eventHero->m_eventFlags | HERO_EVENT_TEMPLE;
                eventHero->m_morale += TEMPLE_MORALE_BONUS;
                NormalDialog(
                    localization::Tr("event.inline.229894cb0c595023"),
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
            }
            break;

        case MAP_OBJECT_LEAN_TO:
            if (!cell->m_objectMetadata) {
                NormalDialog(
                    localization::Tr("event.inline.74555e8e4156f4be"),
                    NORMAL_DIALOG_INFO
                );
            } else {
                EventSound(eventType, cell->m_objectMetadata, &eventSample);
                NormalDialog(
                    localization::Tr("event.inline.c451cfe18d2f7330"),
                    NORMAL_DIALOG_INFO,
                    -1,
                    -1,
                    (cell->m_objectMetadata & CAMPFIRE_RESOURCE_MASK) - 1,
                    (cell->m_objectMetadata & DAEMON_SERVANT_MASK) >> DAEMON_SERVANT_SHIFT,
                    -1,
                    0,
                    -1,
                    0
                );
                GiveResource(
                    eventHero,
                    static_cast<ResourceType>(
                        (cell->m_objectMetadata & CAMPFIRE_RESOURCE_MASK) - 1
                    ),
                    (cell->m_objectMetadata & DAEMON_SERVANT_MASK) >> DAEMON_SERVANT_SHIFT
                );
                cell->m_objectMetadata = 0;
            }
            break;

        case MAP_OBJECT_WAGON:
            if (cell->m_objectMetadata == 0) {
            wagonEmpty:
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    localization::Tr("event.inline.640141be28ff3fd6"),
                    -1,
                    0,
                    -1,
                    0,
                    -1
                );
                cell->m_objectMetadata = 0;
                break;
            }
            if (cell->m_objectMetadata & WAGON_ARTIFACT_FLAG) {
                if (eventHero->NumArtifacts() == HERO_ARTIFACT_SLOT_COUNT)
                    goto wagonEmpty;
                EventSound(eventType, cell->m_objectMetadata, &eventSample);
                eventValue = cell->m_objectMetadata & WAGON_ARTIFACT_MASK;
                sprintf(
                    gText,
                    localization::Tr("event.inline.b7b134ab6de2bf9c"),
                    gArtifactNames[eventValue]
                );
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    gText,
                    MAP_EVENT_REWARD_ARTIFACT,
                    eventValue,
                    -1,
                    0,
                    -1
                );
                GiveArtifact(eventHero, ArtifactType(eventValue), true);
                cell->m_objectMetadata = 0;
            } else {
                EventSound(eventType, cell->m_objectMetadata, &eventSample);
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    localization::Tr("event.inline.cb89443bb85658f3"),
                    (cell->m_objectMetadata & CAMPFIRE_RESOURCE_MASK) - 1,
                    (cell->m_objectMetadata & DAEMON_SERVANT_MASK) >> DAEMON_SERVANT_SHIFT,
                    -1,
                    0,
                    -1
                );
                GiveResource(
                    eventHero,
                    static_cast<ResourceType>(
                        (cell->m_objectMetadata & CAMPFIRE_RESOURCE_MASK) - 1
                    ),
                    (cell->m_objectMetadata & DAEMON_SERVANT_MASK) >> DAEMON_SERVANT_SHIFT
                );
                cell->m_objectMetadata = 0;
            }
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
                    EventSound(eventType, cell->m_objectMetadata, &eventSample);
                    if (eventHero->NumArtifacts() == HERO_ARTIFACT_SLOT_COUNT) {
                        sprintf(
                            gText,
                            "%s.",
                            localization::Tr("common.treasure")
                        );
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
                        GiveResource(eventHero, RES_GOLD, SKELETON_GOLD);
                    } else {
                        eventValue = cell->m_objectMetadata - SKELETON_ARTIFACT_OFFSET;
                        sprintf(
                            gText,
                            "%s %s",
                            gEventText[EVENT_TEXT_SKELETON_REWARD],
                            gArtifactNames[eventValue]
                        );
                        GiveArtifact(eventHero, ArtifactType(eventValue), true);
                        EventWindow(
                            -1,
                            NORMAL_DIALOG_INFO,
                            gText,
                            MAP_EVENT_REWARD_ARTIFACT,
                            eventValue,
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
                eventHero,
                RES_GOLD,
                (cell->m_objectMetadata >> CAMPFIRE_AMOUNT_SHIFT) * CAMPFIRE_GOLD_MULTIPLIER
            );
            GiveResource(
                eventHero,
                ResourceType(cell->m_objectMetadata & IDX(CAMPFIRE_RESOURCE_MASK)),
                cell->m_objectMetadata >> CAMPFIRE_AMOUNT_SHIFT
            );
            eraseObject = 1;
            fizzleType = true;
            SetEnvironmentOrigin(
                m_mapOriginX + ENVIRONMENT_BORDER,
                m_mapOriginY + ENVIRONMENT_BORDER,
                1
            );
            break;

        case MAP_OBJECT_XANADU:
            if (eventHero->m_xanaduVisits & (1 << cell->m_objectMetadata)) {
                NormalDialog(
                    localization::Tr("event.inline.da378498c0963c83"),
                    NORMAL_DIALOG_INFO
                );
            } else {
                if (eventHero->m_level
                        + IDX(eventHero->m_secondarySkills[IDX(HERO_SKILL_DIPLOMACY)])
                              * XANADU_DIPLOMACY_MULTIPLIER
                    >= XANADU_ADMISSION_LEVEL) {
                    EventSound(eventType, cell->m_objectMetadata, &eventSample);
                    NormalDialog(
                        localization::Tr("event.inline.6e67be895f4b2e0b"),
                        NORMAL_DIALOG_INFO
                    );
                    eventHero->m_primaryStats[IDX(HERO_PRIMARY_ATTACK)]++;
                    eventHero->m_primaryStats[IDX(HERO_PRIMARY_DEFENSE)]++;
                    eventHero->m_primaryStats[IDX(HERO_PRIMARY_KNOWLEDGE)]++;
                    eventHero->m_primaryStats[IDX(HERO_PRIMARY_SPELL_POWER)]++;
                    eventHero->m_xanaduVisits |= 1 << cell->m_objectMetadata;
                } else {
                    NormalDialog(
                        localization::Tr("event.inline.cfee7ef6dc2dfe29"),
                        NORMAL_DIALOG_INFO
                    );
                }
            }
            break;

        case MAP_OBJECT_FORT:
            if (eventHero->m_fortVisits & (1 << cell->m_objectMetadata)) {
                NormalDialog(
                    localization::Tr("event.inline.41d63febc86c9f57"),
                    NORMAL_DIALOG_INFO
                );
            } else {
                EventSound(eventType, cell->m_objectMetadata, &eventSample);
                NormalDialog(
                    localization::Tr("event.inline.2ba84ac8f84d579f"),
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
                eventHero->m_primaryStats[IDX(HERO_PRIMARY_DEFENSE)]++;
                eventHero->m_fortVisits |= 1 << cell->m_objectMetadata;
            }
            break;

        case MAP_OBJECT_STANDING_STONES:
            if (eventHero->m_standingStoneVisits & (1 << cell->m_objectMetadata)) {
                NormalDialog(
                    localization::Tr("event.inline.abd64140cf81cf64"),
                    NORMAL_DIALOG_INFO
                );
            } else {
                EventSound(eventType, cell->m_objectMetadata, &eventSample);
                NormalDialog(
                    localization::Tr("event.inline.21050646fbb7b09b"),
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
                eventHero->m_primaryStats[IDX(HERO_PRIMARY_SPELL_POWER)]++;
                eventHero->m_standingStoneVisits |= 1 << cell->m_objectMetadata;
            }
            break;

        case MAP_OBJECT_WITCH_DOCTOR_HUT:
            if (eventHero->m_witchDoctorVisits & (1 << cell->m_objectMetadata)) {
                NormalDialog(
                    localization::Tr("event.inline.adb07f262cb5a288"),
                    NORMAL_DIALOG_INFO
                );
            } else {
                EventSound(eventType, cell->m_objectMetadata, &eventSample);
                NormalDialog(
                    localization::Tr("event.inline.d0681d4d11aa03cd"),
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
                eventHero->m_primaryStats[IDX(HERO_PRIMARY_KNOWLEDGE)]++;
                eventHero->m_witchDoctorVisits |= 1 << cell->m_objectMetadata;
            }
            break;

        case MAP_OBJECT_MERCENARY_CAMP:
            if (eventHero->m_mercenaryCampVisits & (1 << cell->m_objectMetadata)) {
                NormalDialog(
                    localization::Tr("event.inline.66fa174294ad02d5"),
                    NORMAL_DIALOG_INFO
                );
            } else {
                EventSound(eventType, cell->m_objectMetadata, &eventSample);
                NormalDialog(
                    localization::Tr("event.inline.412b3e9f25bdd6fb"),
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
                eventHero->m_primaryStats[IDX(HERO_PRIMARY_ATTACK)]++;
                eventHero->m_mercenaryCampVisits |= 1 << cell->m_objectMetadata;
            }
            break;

        case MAP_OBJECT_GAZEBO:
            if (eventHero->m_gazeboVisits & (1 << cell->m_objectMetadata)) {
                EventWindow(
                    EVENT_TEXT_GAZEBO_VISITED,
                    NORMAL_DIALOG_INFO,
                    "",
                    -1,
                    0,
                    -1,
                    0,
                    -1
                );
            } else {
                EventSound(eventType, cell->m_objectMetadata, &eventSample);
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
                GiveExperience(eventHero, GAZEBO_EXPERIENCE, 0);
                eventHero->m_gazeboVisits |= 1 << cell->m_objectMetadata;
                eventHero->CheckLevel();
            }
            break;

        case MAP_OBJECT_WATER_WHEEL:
            if (!cell->m_objectMetadata) {
                EventWindow(
                    EVENT_TEXT_WATER_WHEEL_EMPTY,
                    NORMAL_DIALOG_INFO,
                    "",
                    -1,
                    0,
                    -1,
                    0,
                    -1
                );
            } else {
                EventSound(eventType, cell->m_objectMetadata, &eventSample);
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
                GiveResource(eventHero, RES_GOLD, cell->m_objectMetadata * MAP_EVENT_GOLD_AMOUNT);
                cell->m_objectMetadata = 0;
            }
            break;

        case MAP_OBJECT_RESOURCE:
            eventResourceType = static_cast<ResourceType>(cell->m_objectIndex / 2);
            GiveResource(
                eventHero,
                eventResourceType,
                eventResourceType == RES_GOLD ? cell->m_objectMetadata * CAMPFIRE_GOLD_MULTIPLIER
                                         : cell->m_objectMetadata
            );
            strcpy(sphinxAnswer, gResourceNames[IDX(eventResourceType)]);
            sphinxAnswer[0] = CyrillicToLower(sphinxAnswer[0]);
            sprintf(gText, gEventText[EVENT_TEXT_RESOURCE_PICKUP], sphinxAnswer);
            BVResMsg(
                gText,
                eventResourceType,
                eventResourceType == RES_GOLD ? cell->m_objectMetadata * CAMPFIRE_GOLD_MULTIPLIER
                                         : cell->m_objectMetadata
            );
            eraseObject = 1;
            fizzleType = true;
            break;

        case MAP_OBJECT_WINDMILL:
            if (cell->m_objectMetadata <= IDX(RES_GOLD)) {
                EventSound(eventType, cell->m_objectMetadata, &eventSample);
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
                    eventHero,
                    ResourceType(cell->m_objectMetadata),
                    WINDMILL_RESOURCE_AMOUNT
                );
                cell->m_objectMetadata = WINDMILL_EMPTY;
            } else {
                EventWindow(
                    EVENT_TEXT_WINDMILL_EMPTY,
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

        case MAP_OBJECT_ANCIENT_LAMP:
            EventSound(eventType, cell->m_objectMetadata, &eventSample);
            EventWindow(
                EVENT_TEXT_GENIE_LAMP,
                NORMAL_DIALOG_CONFIRM,
                "",
                -1,
                0,
                -1,
                0,
                -1
            );
            if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_YES) {
                RecruitEvent(eventHero, CREATURE_GENIE, cell);
                if (!cell->m_objectMetadata) {
                    eraseObject = 1;
                    fizzleType = true;
                }
            }
            break;

        case MAP_OBJECT_TREE_CITY:
            if (!cell->m_objectMetadata) {
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    localization::Tr("event.inline.b586887c923c2b52"),
                    -1,
                    0,
                    -1,
                    0,
                    -1
                );
            } else {
                EventSound(eventType, cell->m_objectMetadata, &eventSample);
                EventWindow(
                    -1,
                    NORMAL_DIALOG_CONFIRM,
                    localization::Tr("event.inline.b74e2c6dcdfc9414"),
                    -1,
                    0,
                    -1,
                    0,
                    -1
                );
                if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_YES)
                    RecruitEvent(eventHero, CREATURE_SPRITE, cell);
            }
            break;

        case MAP_OBJECT_RUINS:
            if (!cell->m_objectMetadata) {
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    localization::Tr("event.inline.9cf24d321bfa6701"),
                    -1,
                    0,
                    -1,
                    0,
                    -1
                );
            } else {
                EventSound(eventType, cell->m_objectMetadata, &eventSample);
                EventWindow(
                    -1,
                    NORMAL_DIALOG_CONFIRM,
                    localization::Tr("event.inline.1836ce3164cbf13b"),
                    -1,
                    0,
                    -1,
                    0,
                    -1
                );
                if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_YES)
                    RecruitEvent(eventHero, CREATURE_MEDUSA, cell);
            }
            break;

        case MAP_OBJECT_TROLL_BRIDGE:
            if (!cell->m_objectMetadata) {
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    localization::Tr("event.inline.6674f02fddf78450"),
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
                    localization::Tr("event.inline.e7cd159b9a0d9237"),
                    -1,
                    0,
                    -1,
                    0,
                    -1
                );
                if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_YES
                    && CombatMonsterEvent(
                           eventHero,
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
                       ) == COMBAT_RESULT_ATTACKER) {
                    eventHero->CheckLevel();
                    cell->m_objectMetadata -= DWELLING_GUARDED_FLAG;
                    EventSound(eventType, cell->m_objectMetadata, &eventSample);
                    EventWindow(
                        -1,
                        NORMAL_DIALOG_CONFIRM,
                        localization::Tr("event.inline.ead7ae60b21cca0c"),
                        -1,
                        0,
                        -1,
                        0,
                        -1
                    );
                    if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_YES)
                        goto recruitTroll;
                }
                break;
            } else {
                EventSound(eventType, cell->m_objectMetadata, &eventSample);
                EventWindow(
                    -1,
                    NORMAL_DIALOG_CONFIRM,
                    localization::Tr("event.inline.fd6fbeb8bebaabb7"),
                    -1,
                    0,
                    -1,
                    0,
                    -1
                );
                if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_YES) {
                recruitTroll:
                    RecruitEvent(eventHero, CREATURE_TROLL, cell);
                }
            }
            break;

        case MAP_OBJECT_CITY_OF_DEAD:
            if (!cell->m_objectMetadata) {
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    localization::Tr("event.inline.59f3f5aaa52a791b"),
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
                    localization::Tr("event.inline.95f1f95998dc4804"),
                    -1,
                    0,
                    -1,
                    0,
                    -1
                );
                if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_YES) {
                    if (CombatMonsterEvent(
                            eventHero,
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
                        == COMBAT_RESULT_ATTACKER) {
                    eventHero->CheckLevel();
                cell->m_objectMetadata -= DWELLING_GUARDED_FLAG;
                EventSound(eventType, cell->m_objectMetadata, &eventSample);
                EventWindow(
                    -1,
                    NORMAL_DIALOG_CONFIRM,
                    localization::Tr("event.inline.c9a9da0253003e88"),
                    -1,
                    0,
                    -1,
                    0,
                    -1
                );
                if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_YES)
                    goto recruitLich;
                    }
                }
                break;
            } else {
                EventSound(eventType, cell->m_objectMetadata, &eventSample);
                EventWindow(
                    -1,
                    NORMAL_DIALOG_CONFIRM,
                    localization::Tr("event.inline.280d4b94ba469861"),
                    -1,
                    0,
                    -1,
                    0,
                    -1
                );
                if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_YES) {
                recruitLich:
                    RecruitEvent(eventHero, CREATURE_POWER_LICH, cell);
                }
            }
            break;

        case MAP_OBJECT_DRAGON_CITY:
            if (!cell->m_objectMetadata) {
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    localization::Tr("event.inline.20d6ea6df7166f59"),
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
                    localization::Tr("event.inline.78bd160e3f266ea6"),
                    -1,
                    0,
                    -1,
                    0,
                    -1
                );
                if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_YES) {
                    dragonFactor = DRAGON_CITY_BASE_FACTOR;
                    if (gbInCampaign && gpGame->m_campaignType == CAMPAIGN_ARCHIBALD
                        && gpGame->m_campaignScenario + 1 == DRAGON_CITY_ARCHIBALD_SCENARIO)
                        dragonFactor = DRAGON_CITY_CAMPAIGN_FACTOR;
                    if (CombatMonsterEvent(
                            eventHero,
                            CREATURE_GREEN_DRAGON,
                            dragonFactor * DRAGON_CITY_GREEN_COUNT_FACTOR,
                            cell,
                            x,
                            y,
                            0,
                            x,
                            y,
                            CREATURE_RED_DRAGON,
                            dragonFactor,
                            DRAGON_CITY_DRAGON_STACKS,
                            CREATURE_BLACK_DRAGON,
                            dragonFactor,
                            DRAGON_CITY_DRAGON_STACKS
                        )
                        == COMBAT_RESULT_ATTACKER) {
                        CheckEndGame(END_GAME_FORCE_NONE, true);
                        if (gbGameOver)
                            break;
                        eventHero->CheckLevel();
                        cell->m_objectMetadata -= DWELLING_GUARDED_FLAG;
                        EventSound(eventType, cell->m_objectMetadata, &eventSample);
                        EventWindow(
                            -1,
                            NORMAL_DIALOG_CONFIRM,
                            localization::Tr("event.inline.5ae328bcb860b385"),
                            -1,
                            0,
                            -1,
                            0,
                            -1
                        );
                        if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_YES)
                            goto recruitDragon;
                    }
                }
                break;
            } else {
                EventSound(eventType, cell->m_objectMetadata, &eventSample);
                EventWindow(
                    -1,
                    NORMAL_DIALOG_CONFIRM,
                    localization::Tr("event.inline.d4b1f646aae1de64"),
                    -1,
                    0,
                    -1,
                    0,
                    -1
                );
                if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_YES) {
                recruitDragon:
                    RecruitEvent(eventHero, CREATURE_RED_DRAGON, cell);
                }
            }
            break;

        case MAP_OBJECT_WAGON_CAMP:
            if (!cell->m_objectMetadata) {
                EventWindow(
                    EVENT_TEXT_WAGON_EMPTY,
                    NORMAL_DIALOG_INFO,
                    "",
                    -1,
                    0,
                    -1,
                    0,
                    -1
                );
            } else {
                EventSound(eventType, cell->m_objectMetadata, &eventSample);
                EventWindow(
                    EVENT_TEXT_WAGON_RECRUIT,
                    NORMAL_DIALOG_CONFIRM,
                    "",
                    -1,
                    0,
                    -1,
                    0,
                    -1
                );
                if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_YES)
                    RecruitEvent(eventHero, CREATURE_ROGUE, cell);
            }
            break;

        case MAP_OBJECT_DESERT_TENT:
            if (!cell->m_objectMetadata) {
                EventWindow(
                    EVENT_TEXT_DESERT_TENT_EMPTY,
                    NORMAL_DIALOG_INFO,
                    "",
                    -1,
                    0,
                    -1,
                    0,
                    -1
                );
            } else {
                EventSound(eventType, cell->m_objectMetadata, &eventSample);
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
                if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_YES)
                    RecruitEvent(eventHero, CREATURE_NOMAD, cell);
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
                EventSound(eventType, cell->m_objectMetadata, &eventSample);
            HouseEvent(eventHero, cell);
            break;

        case MAP_OBJECT_MONSTER:
            PlayerMonsterInteract(cell, cell, eventHero, &eraseObject, x, y, 0, x, y);
            break;

        case MAP_OBJECT_OBELISK:
            if (!(gpGame->m_obeliskVisitors[cell->m_objectMetadata - 1]
                  & (1 << eventHero->m_owner))) {
                EventSound(eventType, cell->m_objectMetadata, &eventSample);
                gpGame->m_obeliskVisitors[cell->m_objectMetadata - 1] |= 1 << eventHero->m_owner;
                EventWindow(
                    EVENT_TEXT_OBELISK_REWARD,
                    NORMAL_DIALOG_INFO,
                    "",
                    -1,
                    0,
                    -1,
                    0,
                    -1
                );
                ViewPuzzle();
            } else {
                EventWindow(
                    EVENT_TEXT_OBELISK_VISITED,
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

        case MAP_OBJECT_TREE_OF_KNOWLEDGE: {
            if ((eventHero->m_treeKnowledgeVisits
                 & (1U << (cell->m_objectMetadata & TREE_KNOWLEDGE_VISIT_INDEX_MASK)))
                != 0) {
                NormalDialog(
                    localization::Tr("event.inline.87d5d0b84d168064"),
                    NORMAL_DIALOG_INFO
                );
            } else {
                EventSound(eventType, cell->m_objectMetadata, &eventSample);
                heroLevel = eventHero->GetLevel(eventHero->m_experience);
                levelExperience = eventHero->GetExperience(heroLevel + 1)
                                   - eventHero->GetExperience(heroLevel);
                switch (cell->m_objectMetadata >> TREE_KNOWLEDGE_MODE_SHIFT) {
                    case TREE_KNOWLEDGE_FREE:
                        NormalDialog(
                            localization::Tr("event.inline.51b8add2648e9e2f"),
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
                        GiveExperience(eventHero, levelExperience, 0);
                        eventHero->m_treeKnowledgeVisits |=
                            1U << (cell->m_objectMetadata & TREE_KNOWLEDGE_VISIT_INDEX_MASK);
                        break;

                    case TREE_KNOWLEDGE_GOLD:
                        if (gpCurPlayer->m_resources[IDX(RES_GOLD)] >= TREE_KNOWLEDGE_GOLD_COST) {
                            NormalDialog(
                                localization::Tr("event.inline.ce98145109683d0e"),
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
                            if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_YES) {
                                gpCurPlayer->m_resources[IDX(RES_GOLD)] -= TREE_KNOWLEDGE_GOLD_COST;
                                GiveExperience(eventHero, levelExperience, 0);
                                eventHero->m_treeKnowledgeVisits |=
                            1U << (cell->m_objectMetadata & TREE_KNOWLEDGE_VISIT_INDEX_MASK);
                            }
                        } else {
                            NormalDialog(
                                localization::Tr("event.inline.924f82c98b13def7"),
                                NORMAL_DIALOG_INFO
                            );
                        }
                        break;

                    case TREE_KNOWLEDGE_GEMS:
                        if (gpCurPlayer->m_resources[IDX(RES_GEMS)] >= TREE_KNOWLEDGE_GEM_COST) {
                            NormalDialog(
                                localization::Tr("event.inline.e5ffff5a51e4796f"),
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
                            if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_YES) {
                                gpCurPlayer->m_resources[IDX(RES_GEMS)] -= TREE_KNOWLEDGE_GEM_COST;
                                GiveExperience(eventHero, levelExperience, 0);
                                eventHero->m_treeKnowledgeVisits |=
                            1U << (cell->m_objectMetadata & TREE_KNOWLEDGE_VISIT_INDEX_MASK);
                            }
                        } else {
                            NormalDialog(
                                localization::Tr("event.inline.ee1576513fcb95bc"),
                                NORMAL_DIALOG_INFO
                            );
                        }
                        break;
                }
            }
            eventHero->CheckLevel();
            break;
        }

        case MAP_OBJECT_ORACLE:
            EventSound(eventType, cell->m_objectMetadata, &eventSample);
            EventWindow(
                EVENT_TEXT_ORACLE,
                NORMAL_DIALOG_INFO,
                "",
                -1,
                0,
                -1,
                0,
                -1
            );
            oracleWindow = new heroWindow(
                0,
                0,
                "thiefwin.bin"
            );
            if (oracleWindow == NULL)
                MemError();
            SetWinText(oracleWindow, ORACLE_WINDOW_TEXT_ID);
            gpTownManager->SetupThievesGuild(oracleWindow, ORACLE_THIEVES_GUILD_RANK);
            strcpy(
                gText,
                localization::Tr("event.inline.bec395b49e2bdfa9")
            );
            SET_WIDGET_MESSAGE(oracleMessage, WIDGET_COMMAND_SET_TEXT, 0);
            oracleMessage.payload.widget.data.text = gText;
            oracleWindow->BroadcastMessage(oracleMessage);
            gpWindowManager->DoDialog(oracleWindow, TrueFalseDialogHandler, 0);
            delete oracleWindow;
            RedrawAdvScreen(1, 0);
            break;

        case MAP_OBJECT_SHRINE_FIRST_CIRCLE:
            sprintf(
                gText,
                "%s'%s'.  ",
                localization::Tr("event.inline.57f459c519bb0e07"),
                gSpellNames[cell->m_objectMetadata - 1]
            );
            goto shrineSpell;

        case MAP_OBJECT_SHRINE_SECOND_CIRCLE:
            sprintf(
                gText,
                "%s'%s'.  ",
                localization::Tr("event.inline.74e932ad068a3630"),
                gSpellNames[cell->m_objectMetadata - 1]
            );
            goto shrineSpell;

        case MAP_OBJECT_SHRINE_THIRD_CIRCLE:
            sprintf(
                gText,
                "%s'%s'.  ",
                localization::Tr("event.inline.8e7c1487402719c9"),
                gSpellNames[cell->m_objectMetadata - 1]
            );
        shrineSpell:
            if (eventHero->HasArtifact(ARTIFACT_MAGIC_BOOK)) {
                if (IDX(gsSpellInfo[cell->m_objectMetadata - 1].level)
                    <= IDX(eventHero->m_secondarySkills[IDX(HERO_SKILL_WISDOM)])
                           + HERO_BASE_LEARNABLE_SPELL_LEVEL) {
                    EventSound(eventType, cell->m_objectMetadata, &eventSample);
                    eventHero->AddSpell(
                        static_cast<SpellType>(cell->m_objectMetadata - 1),
                        eventHero->Stats(HERO_PRIMARY_KNOWLEDGE)
                    );
                    EventWindow(
                        -1,
                        NORMAL_DIALOG_INFO,
                        gText,
                        NORMAL_DIALOG_SPELL,
                        cell->m_objectMetadata - 1,
                        -1,
                        0,
                        -1
                    );
                } else {
                    strcat(
                        gText,
                        localization::Tr("event.inline.b2fd77737182a2d9")
                    );
                    EventWindow(-1, NORMAL_DIALOG_INFO, gText, -1, 0, -1, 0, -1);
                }
            } else {
                strcat(
                    gText,
                    localization::Tr("event.inline.81a2a5818237074f")
                );
                EventWindow(-1, NORMAL_DIALOG_INFO, gText, -1, 0, -1, 0, -1);
            }
            break;

        case MAP_OBJECT_CASTLE:
            TownEvent(cell, x, y);
            break;

        case MAP_OBJECT_WHIRLPOOL:
            EventSound(eventType, cell->m_objectMetadata, &eventSample);
            DoWhirlpool(eventHero);
            goto findTeleportDestination;

        case MAP_OBJECT_STONE_LITHS:
            EventSound(eventType, cell->m_objectMetadata, &eventSample);
        findTeleportDestination:
            teleportCount = 0;
            for (teleportY = 0; teleportY < MAP_HEIGHT; teleportY++) {
                for (teleportX = 0; teleportX < MAP_WIDTH; teleportX++) {
                    if (H2_ENUM_RAW(
                            (gpGame->m_worldMap.GetCell(teleportX, teleportY))->m_triggerType
                        ) == static_cast<u8>(eventType | MAP_TRIGGER_ACTION_FLAG)
                        && (gpGame->m_worldMap.GetCell(teleportX, teleportY))->m_objectIndex
                               == cell->m_objectIndex
                        && MANHATTAN_LENGTH(teleportX - x, teleportY - y)
                               > (eventType == MAP_OBJECT_STONE_LITHS ? STONE_LITHS_MIN_DISTANCE
                                                                        : WHIRLPOOL_MIN_DISTANCE)) {
                        teleportCount++;
                    }
                }
            }
            if (teleportCount >= 1) {
                if (teleportCount > 1)
                    teleportCount = Random(1, teleportCount);
                for (teleportY = 0; teleportY < MAP_HEIGHT; teleportY++) {
                    for (teleportX = 0; teleportX < MAP_WIDTH; teleportX++) {
                        if (H2_ENUM_RAW((gpGame->m_worldMap.GetCell(teleportX, teleportY))
                                            ->m_triggerType)
                                == static_cast<u8>(eventType | MAP_TRIGGER_ACTION_FLAG)
                            && (gpGame->m_worldMap.GetCell(teleportX, teleportY))->m_objectIndex
                                   == cell->m_objectIndex
                            && (teleportX != x || teleportY != y)
                            && MANHATTAN_LENGTH(teleportX - x, teleportY - y)
                                   > (eventType == MAP_OBJECT_STONE_LITHS
                                          ? STONE_LITHS_MIN_DISTANCE
                                          : WHIRLPOOL_MIN_DISTANCE)
                            && --teleportCount <= 0) {
                            goto teleportHero;
                        }
                    }
                }
            teleportHero:
                StopCursor(1);
                gpAdvManager->TeleportTo(eventHero, teleportX, teleportY, 1, 0);
            }
            break;

        case MAP_OBJECT_ARTIFACT: {
            artifactResourceType = (cell->m_objectMetadata & ARTIFACT_EVENT_RESOURCE_MASK)
                                   >> ARTIFACT_EVENT_RESOURCE_SHIFT;
            eventArtifact = static_cast<ArtifactType>(cell->m_objectIndex / 2);
            guardedMonster =
                static_cast<CreatureType>(cell->m_objectMetadata & ARTIFACT_EVENT_MONSTER_MASK);
            if (eventHero->NumArtifacts() == HERO_ARTIFACT_SLOT_COUNT) {
                NormalDialog(
                    localization::Tr("event.inline.b880ecbc34dcdb8b"),
                    NORMAL_DIALOG_INFO
                );
                break;
            }

            if (xIsPlayingExpansionCampaign && xCampaign.IsSpecialGoldenBow(x, y)) {
                EventSound(eventType, cell->m_objectMetadata, &eventSample);
                strcpy(
                    gText,
                    localization::Tr("event.inline.4e929a833e525afd")
                );
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    gText,
                    MAP_EVENT_REWARD_ARTIFACT,
                    IDX(eventArtifact),
                    -1,
                    0,
                    -1
                );
                GiveArtifact(eventHero, eventArtifact, true, cell->m_objectMetadata);
                eraseObject = 1;
                fizzleType = true;
                break;
            }

            if (eventArtifact == ARTIFACT_SPELL_SCROLL) {
                EventSound(eventType, 1, &eventSample);
                xTheSpell = cell->m_objectMetadata;
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    gArtifactEvent[IDX(eventArtifact)],
                    MAP_EVENT_REWARD_ARTIFACT,
                    IDX(eventArtifact),
                    -1,
                    0,
                    -1
                );
                GiveArtifact(eventHero, eventArtifact, true, cell->m_objectMetadata);
                eraseObject = 1;
                fizzleType = true;
                break;
            }

            if (cell->m_objectMetadata & MAP_EVENT_ARTIFACT_GUARD_FLAG) {
                EventSound(eventType, cell->m_objectMetadata, &eventSample);
                if (guardedMonster == CREATURE_ROGUE) {
                    NormalDialog(
                        localization::Tr("event.inline.47fbd3f75db6688a"),
                        NORMAL_DIALOG_INFO
                    );
                    guardedCount = ARTIFACT_EVENT_GUARD_ROGUE_COUNT;
                    goto artifactFight;
                } else {
                    guardedCount = 1;
                    sprintf(
                        gText,
                        localization::Tr("event.inline.580a8ff25d58cea2"),
                        gArmyNamesPlural[IDX(guardedMonster)],
                        gArmyNamesPlural[IDX(guardedMonster)]
                    );
                    NormalDialog(gText, NORMAL_DIALOG_CONFIRM);
                }
                if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_YES) {
                artifactFight:
                    if (CombatMonsterEvent(
                            eventHero,
                            guardedMonster,
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
                        == COMBAT_RESULT_ATTACKER) {
                        eventHero->CheckLevel();
                        sprintf(
                            gText,
                            localization::Tr("event.inline.db0baf49ee252efe"),
                            gArtifactNames[IDX(eventArtifact)]
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
                        goto giveArtifact;
                    }
                } else {
                    NormalDialog(
                        localization::Tr("event.inline.29ceb4fc2c520780"),
                        NORMAL_DIALOG_INFO
                    );
                }
                break;
            } else {
                switch (cell->m_objectMetadata & ARTIFACT_EVENT_MODE_MASK) {
                    case ARTIFACT_EVENT_MODE_WISDOM:
                        if (eventHero->m_secondarySkills[IDX(HERO_SKILL_WISDOM)]
                            != HERO_SKILL_LEVEL_NONE)
                            goto artifactPickup;
                        sprintf(
                            gText,
                            localization::Tr("event.inline.114341b70ac61964"),
                            gArtifactNames[IDX(eventArtifact)]
                        );
                        NormalDialog(gText, NORMAL_DIALOG_INFO);
                        break;

                    case ARTIFACT_EVENT_MODE_LEADERSHIP:
                        if (eventHero->m_secondarySkills[IDX(HERO_SKILL_LEADERSHIP)]
                            != HERO_SKILL_LEVEL_NONE)
                            goto artifactPickup;
                        sprintf(
                            gText,
                            localization::Tr("event.inline.85b58cafd168a4e6"),
                            gArtifactNames[IDX(eventArtifact)]
                        );
                        NormalDialog(gText, NORMAL_DIALOG_INFO);
                        break;

                    case ARTIFACT_EVENT_MODE_PICKUP:
                    artifactPickup:
                        EventSound(eventType, cell->m_objectMetadata, &eventSample);
                        EventWindow(
                            -1,
                            NORMAL_DIALOG_INFO,
                            gArtifactEvent[IDX(eventArtifact)],
                            MAP_EVENT_REWARD_ARTIFACT,
                            IDX(eventArtifact),
                            -1,
                            0,
                            -1
                        );
                    giveArtifact:
                        GiveArtifact(eventHero, eventArtifact, true);
                        eraseObject = 1;
                        fizzleType = true;
                        break;

                    case ARTIFACT_EVENT_MODE_GOLD:
                        EventSound(eventType, cell->m_objectMetadata, &eventSample);
                        sprintf(
                            gText,
                            localization::Tr("event.inline.219ab198b8338466"),
                            gArtifactNames[IDX(eventArtifact)]
                        );
                        EventWindow(
                            -1,
                            NORMAL_DIALOG_CONFIRM,
                            gText,
                            MAP_EVENT_REWARD_ARTIFACT,
                            IDX(eventArtifact),
                            -1,
                            0,
                            -1
                        );
                        if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_YES) {
                            if (gpGame->m_players[IDX(eventHero->m_owner)].m_resources[IDX(RES_GOLD)]
                                >= ARTIFACT_EVENT_GOLD_COST) {
                                gpGame->m_players[IDX(eventHero->m_owner)].m_resources[IDX(RES_GOLD)] -=
                                    ARTIFACT_EVENT_GOLD_COST;
                                goto giveArtifact;
                            }
                            NormalDialog(
                                localization::Tr("event.inline.eb595f1324302ad8"),
                                NORMAL_DIALOG_INFO
                            );
                        } else {
                            NormalDialog(
                                localization::Tr("event.inline.d2d604b822f6a317"),
                                NORMAL_DIALOG_INFO
                            );
                        }
                        break;

                    case ARTIFACT_EVENT_MODE_RESOURCE_3:
                        EventSound(eventType, cell->m_objectMetadata, &eventSample);
                        sprintf(sphinxAnswer, gResourceNames[IDX(artifactResourceType)]);
                        sphinxAnswer[0] = CyrillicToLower(sphinxAnswer[0]);
                        sprintf(
                            gText,
                            localization::Tr("event.inline.6ab4a1f0b7bda188"),
                            gArtifactNames[IDX(eventArtifact)],
                            sphinxAnswer
                        );
                        NormalDialog(
                            gText,
                            NORMAL_DIALOG_CONFIRM,
                            -1,
                            -1,
                            MAP_EVENT_REWARD_ARTIFACT,
                            IDX(eventArtifact),
                            -1,
                            0,
                            -1,
                            0
                        );
                        if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_YES) {
                            if (gpGame->m_players[IDX(eventHero->m_owner)].m_resources[IDX(RES_GOLD)]
                                    >= ARTIFACT_EVENT_RESOURCE_3_GOLD_COST
                                && gpGame->m_players[IDX(eventHero->m_owner)]
                                           .m_resources[IDX(artifactResourceType)]
                                       >= ARTIFACT_EVENT_RESOURCE_3_AMOUNT) {
                                gpGame->m_players[IDX(eventHero->m_owner)].m_resources[IDX(RES_GOLD)] -=
                                    ARTIFACT_EVENT_RESOURCE_3_GOLD_COST;
                                gpGame->m_players[IDX(eventHero->m_owner)]
                                    .m_resources[IDX(artifactResourceType)] -=
                                    ARTIFACT_EVENT_RESOURCE_3_AMOUNT;
                                goto giveArtifact;
                            }
                            NormalDialog(
                                localization::Tr("event.inline.eb595f1324302ad8"),
                                NORMAL_DIALOG_INFO
                            );
                        } else {
                            NormalDialog(
                                localization::Tr("event.inline.d2d604b822f6a317"),
                                NORMAL_DIALOG_INFO
                            );
                        }
                        break;

                    case ARTIFACT_EVENT_MODE_RESOURCE_5:
                        EventSound(eventType, cell->m_objectMetadata, &eventSample);
                        sprintf(sphinxAnswer, gResourceNames[IDX(artifactResourceType)]);
                        sphinxAnswer[0] = CyrillicToLower(sphinxAnswer[0]);
                        sprintf(
                            gText,
                            localization::Tr("event.inline.d5ae03d7c7901a43"),
                            gArtifactNames[IDX(eventArtifact)],
                            sphinxAnswer
                        );
                        NormalDialog(
                            gText,
                            NORMAL_DIALOG_CONFIRM,
                            -1,
                            -1,
                            MAP_EVENT_REWARD_ARTIFACT,
                            IDX(eventArtifact),
                            -1,
                            0,
                            -1,
                            0
                        );
                        if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_YES) {
                            if (gpGame->m_players[IDX(eventHero->m_owner)].m_resources[IDX(RES_GOLD)]
                                    >= ARTIFACT_EVENT_RESOURCE_5_GOLD_COST
                                && gpGame->m_players[IDX(eventHero->m_owner)]
                                           .m_resources[IDX(artifactResourceType)]
                                       >= ARTIFACT_EVENT_RESOURCE_5_AMOUNT) {
                                gpGame->m_players[IDX(eventHero->m_owner)].m_resources[IDX(RES_GOLD)] -=
                                    ARTIFACT_EVENT_RESOURCE_5_GOLD_COST;
                                gpGame->m_players[IDX(eventHero->m_owner)]
                                    .m_resources[IDX(artifactResourceType)] -=
                                    ARTIFACT_EVENT_RESOURCE_5_AMOUNT;
                                goto giveArtifact;
                            }
                            NormalDialog(
                                localization::Tr("event.inline.eb595f1324302ad8"),
                                NORMAL_DIALOG_INFO
                            );
                        } else {
                            NormalDialog(
                                localization::Tr("event.inline.d2d604b822f6a317"),
                                NORMAL_DIALOG_INFO
                            );
                        }
                        break;
                }
                eventHero->CheckLevel();
            }
            break;
        }

        case MAP_OBJECT_HERO_INTERACTION:
            DemobilizeCurrHero();
            otherHero = gpGame->GetHero(cell->m_objectMetadata);
            if (otherHero->m_owner == giCurPlayer) {
                HeroSwap(eventHero, otherHero);
            } else {
                occupiedTown = NULL;
                if (otherHero->m_locationType
                    == (MAP_ACTION_TRIGGER(MAP_OBJECT_CASTLE))) {
                    occupiedTown = gpGame->GetTown(otherHero->m_occupiedTown);
                    occupiedTown->m_occupyingHeroId = otherHero->m_id;
                }
                heroCombatResult = DoCombat(
                    x,
                    y,
                    eventHero,
                    &eventHero->m_army,
                    occupiedTown,
                    otherHero,
                    &otherHero->m_army,
                    x,
                    y,
                    -1,
                    1
                );
                if (heroCombatResult == COMBAT_RESULT_ATTACKER && occupiedTown != NULL)
                    gpGame->ClaimTown(occupiedTown->m_id, giCurPlayer, 0);
            }
            break;

        case MAP_OBJECT_BOTTLE:
            if (cell->m_objectMetadata) {
                signExtra = static_cast<signEventExtra*>(ppMapExtra[cell->m_objectMetadata]);
                if (strlen(signExtra->text) > SIGN_MINIMUM_TEXT_LENGTH)
                    EventWindow(-1, NORMAL_DIALOG_INFO, signExtra->text, -1, 0, -1, 0, -1);
                else
                    EventWindow(
                        -1,
                        NORMAL_DIALOG_INFO,
                        cRandomSignText[x % SIGN_RANDOM_TEXT_COUNT],
                        -1,
                        0,
                        -1,
                        0,
                        -1
                    );
            }
            fizzleType = true;
            eraseObject = 1;
            break;

        case MAP_OBJECT_SIGN:
            if (cell->m_objectMetadata) {
                signExtra = static_cast<signEventExtra*>(ppMapExtra[cell->m_objectMetadata]);
                if (strlen(signExtra->text) > SIGN_MINIMUM_TEXT_LENGTH)
                    EventWindow(-1, NORMAL_DIALOG_INFO, signExtra->text, -1, 0, -1, 0, -1);
                else
                    EventWindow(
                        -1,
                        NORMAL_DIALOG_INFO,
                        cRandomSignText[x % SIGN_RANDOM_TEXT_COUNT],
                        -1,
                        0,
                        -1,
                        0,
                        -1
                    );
            }
            break;

        case MAP_OBJECT_DAEMON_CAVE: {
            EventSound(eventType, cell->m_objectMetadata, &eventSample);
            EventWindow(
                -1,
                NORMAL_DIALOG_CONFIRM,
                localization::Tr("event.inline.967de2778f210f98"),
                -1,
                0,
                -1,
                0,
                -1
            );
            if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_NO)
                break;
            if (cell->m_objectMetadata == DAEMON_CAVE_EMPTY) {
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    localization::Tr("event.inline.a55956dc91df9373"),
                    -1,
                    0,
                    -1,
                    0,
                    -1
                );
                break;
            }

            monsterType = static_cast<CreatureType>(
                ((cell->m_objectMetadata & DAEMON_SERVANT_MASK) >> DAEMON_SERVANT_SHIFT)
                + DAEMON_SERVANT_BASE
            );
            sprintf(
                gText,
                localization::Tr("event.inline.bf2299125ded4930")
            );
            EventWindow(-1, NORMAL_DIALOG_CONFIRM, gText, -1, 0, -1, 0, -1);
            if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_YES) {
                if (CombatMonsterEvent(
                        eventHero,
                        monsterType,
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
                    eventHero->CheckLevel();
                    NormalDialog(
                        localization::Tr("event.inline.cb3ce775e0c55740"),
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
                    GiveResource(eventHero, RES_GOLD, DAEMON_GOLD);
                    cell->m_objectMetadata = DAEMON_CAVE_EMPTY;
                }
                break;
            }

            switch (cell->m_objectMetadata & DAEMON_REWARD_MASK) {
                case DAEMON_REWARD_EXPERIENCE:
                    GiveExperience(eventHero, DAEMON_EXPERIENCE, 0);
                    EventWindow(
                        -1,
                        NORMAL_DIALOG_INFO,
                        localization::Tr("event.inline.2c0fc1c524da5be1"),
                        NORMAL_DIALOG_EXPERIENCE,
                        DAEMON_EXPERIENCE,
                        -1,
                        0,
                        -1
                    );
                    cell->m_objectMetadata = DAEMON_CAVE_EMPTY;
                    eventHero->CheckLevel();
                    break;

                case DAEMON_REWARD_ARTIFACT:
                    if (eventHero->NumArtifacts() == HERO_ARTIFACT_SLOT_COUNT)
                        goto daemonExperienceGold;
                    if (gpGame->GetRandomArtifactId(ARTIFACT_LEVEL_RANDOM, true) == -1)
                        goto daemonExperienceGold;
                    GiveExperience(eventHero, DAEMON_EXPERIENCE, 0);
                    eventValue = IDX(GiveRandomArtifact(eventHero));
                    sprintf(
                        gText,
                        localization::Tr("event.inline.ea1d54ae68a8c04a"),
                        gArtifactNames[eventValue]
                    );
                    EventWindow(
                        -1,
                        NORMAL_DIALOG_INFO,
                        gText,
                        MAP_EVENT_REWARD_ARTIFACT,
                        eventValue,
                        NORMAL_DIALOG_EXPERIENCE,
                        DAEMON_EXPERIENCE,
                        -1
                    );
                    cell->m_objectMetadata = DAEMON_CAVE_EMPTY;
                    eventHero->CheckLevel();
                    break;

                case DAEMON_REWARD_EXPERIENCE_GOLD:
                daemonExperienceGold:
                    EventWindow(
                        -1,
                        NORMAL_DIALOG_INFO,
                        localization::Tr("event.inline.92b0881ba080eff9"),
                        IDX(RES_GOLD),
                        DAEMON_GOLD,
                        NORMAL_DIALOG_EXPERIENCE,
                        DAEMON_EXPERIENCE,
                        -1
                    );
                    GiveExperience(eventHero, DAEMON_EXPERIENCE, 0);
                    GiveResource(eventHero, RES_GOLD, DAEMON_GOLD);
                    cell->m_objectMetadata = DAEMON_CAVE_EMPTY;
                    eventHero->CheckLevel();
                    break;

                case DAEMON_REWARD_RANSOM:
                    EventWindow(
                        -1,
                        NORMAL_DIALOG_CONFIRM,
                        localization::Tr("event.inline.1b32f445f3853c55"),
                        -1,
                        0,
                        -1,
                        0,
                        -1
                    );
                    if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_YES) {
                        if (gpGame->m_players[IDX(eventHero->m_owner)].m_resources[IDX(RES_GOLD)]
                            < DAEMON_GOLD) {
                            EventWindow(
                                -1,
                                NORMAL_DIALOG_INFO,
                                localization::Tr("event.inline.33c78d108003798f"),
                                -1,
                                0,
                                -1,
                                0,
                                -1
                            );
                            HeroLoses(eventHero);
                        } else {
                            gpGame->m_players[IDX(eventHero->m_owner)].m_resources[IDX(RES_GOLD)] -=
                                DAEMON_GOLD;
                        }
                    } else {
                        HeroLoses(eventHero);
                    }
                    break;
            }
            cell->m_objectMetadata = DAEMON_CAVE_EMPTY;
            break;
        }

        case MAP_OBJECT_SHIPWRECK: {
            EventSound(eventType, cell->m_objectMetadata, &eventSample);
            EventWindow(
                -1,
                NORMAL_DIALOG_CONFIRM,
                localization::Tr("event.inline.b84c63468364c5dd"),
                -1,
                0,
                -1,
                0,
                -1
            );
            if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_YES) {
                switch (cell->m_objectMetadata) {
                    case SKELETON_EMPTY:
                        EventWindow(
                            -1,
                            NORMAL_DIALOG_INFO,
                            localization::Tr("event.inline.af08a748652258d4"),
                            NORMAL_DIALOG_MORALE_PENALTY,
                            0,
                            -1,
                            0,
                            -1
                        );
                        if (!HAS(eventHero->m_eventFlags, HERO_EVENT_SHIPWRECK)) {
                            eventHero->m_eventFlags =
                                eventHero->m_eventFlags | HERO_EVENT_SHIPWRECK;
                            eventHero->m_morale--;
                        }
                        break;
                    default:
                        if (GhostEvent(
                                eventHero,
                                cell,
                                localization::Tr("event.inline.6b614dcca0dde223"),
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
            EventSound(eventType, cell->m_objectMetadata, &eventSample);
            EventWindow(
                EVENT_TEXT_GRAVEYARD_PROMPT,
                NORMAL_DIALOG_CONFIRM,
                "",
                -1,
                0,
                -1,
                0,
                -1
            );
            if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_YES) {
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
                        if (!HAS(eventHero->m_eventFlags, HERO_EVENT_GRAVEYARD)) {
                            eventHero->m_eventFlags =
                                eventHero->m_eventFlags | HERO_EVENT_GRAVEYARD;
                            eventHero->m_morale--;
                        }
                        break;
                    default:
                        zombieCell = GetCell(
                            x - normalDirTable[IDX(eventHero->m_direction)].x,
                            y - normalDirTable[IDX(eventHero->m_direction)].y
                        );
                        if (ZombieEvent(
                                eventHero,
                                zombieCell,
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
            EventSound(eventType, cell->m_objectMetadata, &eventSample);
            EventWindow(
                -1,
                NORMAL_DIALOG_CONFIRM,
                localization::Tr("event.inline.1fe5ee69b3203322"),
                -1,
                0,
                -1,
                0,
                -1
            );
            if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_YES) {
                switch (cell->m_objectMetadata) {
                    case SKELETON_EMPTY:
                        EventWindow(
                            -1,
                            NORMAL_DIALOG_INFO,
                            localization::Tr("event.inline.b2f13c84edb58954"),
                            NORMAL_DIALOG_MORALE_PENALTY,
                            0,
                            -1,
                            0,
                            -1
                        );
                        if (!HAS(eventHero->m_eventFlags, HERO_EVENT_DERELICT_SHIP)) {
                            eventHero->m_eventFlags =
                                eventHero->m_eventFlags | HERO_EVENT_DERELICT_SHIP;
                            eventHero->m_morale--;
                        }
                        break;
                    default:
                        skeletonCell = GetCell(
                            x - normalDirTable[IDX(eventHero->m_direction)].x,
                            y - normalDirTable[IDX(eventHero->m_direction)].y
                        );
                        if (SkeletonEvent(
                                eventHero,
                                skeletonCell,
                                localization::Tr("event.inline.85fbba93bc78c682"),
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
            EventSound(eventType, cell->m_objectMetadata, &eventSample);
            EventWindow(
                -1,
                NORMAL_DIALOG_CONFIRM,
                localization::Tr("event.inline.f275576ff90522c2"),
                -1,
                0,
                -1,
                0,
                -1
            );
            if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_YES) {
                if (!cell->m_objectMetadata) {
                    NormalDialog(
                        localization::Tr("event.inline.414c8dd999ff1206"),
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
                    if (!HAS(eventHero->m_eventFlags, HERO_EVENT_PYRAMID)) {
                        eventHero->m_eventFlags = eventHero->m_eventFlags | HERO_EVENT_PYRAMID;
                        eventHero->m_luck -= PYRAMID_LUCK_PENALTY;
                    }
                    break;
                } else {
                    if (CombatMonsterEvent(
                            eventHero,
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
                        eventHero->CheckLevel();
                                    sprintf(
                            eventText,
                            "%s'%s'.  ",
                            localization::Tr("event.inline.3c1ac9096798469d"),
                            gSpellNames[cell->m_objectMetadata - 1]
                        );
                        if (!eventHero->HasArtifact(ARTIFACT_MAGIC_BOOK)) {
                            strcat(
                                eventText,
                                localization::Tr("event.inline.bdc50da8baa48d7e")
                            );
                            EventWindow(-1, NORMAL_DIALOG_INFO, eventText, -1, 0, -1, 0, -1);
                        } else if (eventHero->m_secondarySkills[IDX(HERO_SKILL_WISDOM)]
                                   >= HERO_SKILL_LEVEL_EXPERT) {
                            eventHero->AddSpell(
                                static_cast<SpellType>(cell->m_objectMetadata - 1),
                                eventHero->Stats(HERO_PRIMARY_KNOWLEDGE)
                            );
                            EventWindow(
                                -1,
                                NORMAL_DIALOG_INFO,
                                eventText,
                                NORMAL_DIALOG_SPELL,
                                cell->m_objectMetadata - 1,
                                -1,
                                0,
                                -1
                            );
                        } else {
                            strcat(
                                eventText,
                                localization::Tr("event.inline.b748cab66a3bf25b")
                            );
                            EventWindow(-1, NORMAL_DIALOG_INFO, eventText, -1, 0, -1, 0, -1);
                        }
                        cell->m_objectMetadata = 0;
                    }
                }
            }
            break;

        case MAP_OBJECT_ABANDONED_MINE:
            EventSound(eventType, cell->m_objectMetadata, &eventSample);
            EventWindow(
                -1,
                NORMAL_DIALOG_CONFIRM,
                localization::Tr("event.inline.1a911306b482a58b"),
                -1,
                0,
                -1,
                0,
                -1
            );
            if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_YES) {
                if (CombatMonsterEvent(
                        eventHero,
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
                    eventHero->CheckLevel();
                    EventWindow(
                        -1,
                        NORMAL_DIALOG_INFO,
                        localization::Tr("event.inline.d57cce3fb667b859"),
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
                        TILESET_OBJNDIRT,
                        MINE_TOP_A_FIRST_FRAME,
                        MINE_TOP_A_LAST_FRAME,
                        TILESET_MTNDIRT,
                        MINE_BOTTOM_A_FRAME,
                        MAP_OBJECT_ABANDONED_MINE,
                        MAP_OBJECT_MINE
                    );
                    gpGame->ConvertObject(
                        x + ABANDONED_MINE_X_MIN,
                        y,
                        x + ABANDONED_MINE_X_MAX,
                        y,
                        TILESET_OBJNDIRT,
                        MINE_SECOND_A_FIRST_FRAME,
                        MINE_SECOND_A_LAST_FRAME,
                        TILESET_MTNDIRT,
                        MINE_SECOND_BOTTOM_A_FRAME,
                        MAP_OBJECT_ABANDONED_MINE,
                        MAP_OBJECT_MINE
                    );
                    gpGame->ConvertObject(
                        x + ABANDONED_MINE_X_MIN,
                        y + ABANDONED_MINE_Y_TOP,
                        x + ABANDONED_MINE_X_MAX,
                        y + ABANDONED_MINE_Y_TOP,
                        TILESET_OBJNGRAS,
                        MINE_TOP_B_FIRST_FRAME,
                        MINE_TOP_B_LAST_FRAME,
                        TILESET_MTNGRAS,
                        MINE_BOTTOM_B_FRAME,
                        MAP_OBJECT_ABANDONED_MINE,
                        MAP_OBJECT_MINE
                    );
                    gpGame->ConvertObject(
                        x + ABANDONED_MINE_X_MIN,
                        y,
                        x + ABANDONED_MINE_X_MAX,
                        y,
                        TILESET_OBJNGRAS,
                        MINE_SECOND_B_FIRST_FRAME,
                        MINE_SECOND_B_LAST_FRAME,
                        TILESET_MTNGRAS,
                        MINE_SECOND_BOTTOM_B_FRAME,
                        MAP_OBJECT_ABANDONED_MINE,
                        MAP_OBJECT_MINE
                    );
                    gpGame->ConvertObject(
                        x,
                        y,
                        x,
                        y,
                        TILESET_EXTRAOVR,
                        MINE_CENTER_GHOST_FRAME,
                        MINE_CENTER_GHOST_FRAME,
                        TILESET_EXTRAOVR,
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
            eraseObject = BarrierEvent(cell, eventHero);
            break;

        case MAP_OBJECT_TRAVELER_TENT:
            PasswordEvent(cell, eventHero);
            break;

        case MAP_OBJECT_EXPANSION_OBJECT:
            GenericSiteEvent(cell, eventHero);
            break;

        case MAP_OBJECT_EXPANSION_DWELLING:
            RecruitSiteEvent(cell, eventHero);
            break;

        case MAP_OBJECT_JAIL:
            JailEvent(cell, eventHero, x, y);
    }

    UpdateRadar(1, 0);
    UpdateHeroLocators(1, 1);
    UpdateTownLocators(1, 1);
    UpdBottomView(true, true, true);
    if (eraseObject) {
        EraseObj(cell, x, y);
        FizzleCenter(fizzleType);
    } else {
        CompleteDraw(0);
    }
    UpdateScreen(0, 0);
    gpSoundManager->SwitchAmbientMusic(giTerrainToMusicTrack[IDX(m_currentTerrain)]);
    WaitEndSample(&eventSample);
    CheckEndGame(END_GAME_FORCE_NONE, false);
}
#if H2_RETAIL_COMPILER
#undef adjacentMonster
#undef artifactResourceType
#undef boat
#undef correctAnswer
#undef dragonFactor
#undef eraseObject
#undef eventArtifact
#undef eventExtra
#undef eventHero
#undef eventResourceType
#undef eventSample
#undef eventText
#undef eventType
#undef eventValue
#undef firstUpgrade
#undef fizzleType
#undef guardedCount
#undef guardedMonster
#undef heroCombatResult
#undef heroLevel
#undef levelExperience
#undef mineCombatResult
#undef monsterType
#undef occupiedTown
#undef oracleMessage
#undef oracleWindow
#undef otherHero
#undef playedSample
#undef primaryAmount
#undef primaryReward
#undef resourceAmount
#undef secondUpgrade
#undef secondaryAmount
#undef secondaryReward
#undef signExtra
#undef skeletonCell
#undef sphinxAnswer
#undef springSpellPoints
#undef teleportCount
#undef teleportX
#undef teleportY
#undef thirdUpgrade
#undef wellSpellPoints
#undef zombieCell
#endif

#if H2_RETAIL_COMPILER
#define cellX cellX_l
#define cellY cellY_o
#define cells cells_h
#define changed changed_f
#define currentCell currentCell_d
#define extra extra_l
#define extras extras_h
#define frame frame_k
#define isWide isWide_l
#define neighborIndex i_g
#define unusedValues unused_m
#endif
VA(0x00441609, 0x904)
void advManager::EraseObj(class mapCell* cell, i32 x, i32 y) {
    b32 H2_UNUSED(erased) = false;
    mapCellExtra* extras[NEIGHBOR_COUNT];
    mapCellExtra* extra;
    mapCell* cells[NEIGHBOR_COUNT];
    i32 frame = NO_FRAME;
    mapCell* currentCell;
    b8 isWide = false;
    i32 neighborIndex;
    i32 H2_UNUSED(unusedValues)[ERASE_COORDINATE_COUNT];
    i32 cellX;
    i32 cellY;
    i32 H2_UNUSED(changed);

    for (neighborIndex = 0; neighborIndex < NEIGHBOR_COUNT; neighborIndex++) {
        cells[neighborIndex] = NULL;
        extras[neighborIndex] = NULL;
    }

    erased = true;
    if (cell->m_objectTileset == TILESET_OBJNARTI)
        frame = cell->m_objectIndex - 1;
    if (cell->m_objectTileset == TILESET_X_LOC3)
        frame = cell->m_objectIndex - 1;
    if (cell->m_objectTileset == TILESET_X_LOC2 && cell->m_objectIndex == 9) {
        frame = 9;
        isWide = true;
    }
    if (cell->m_objectTileset == TILESET_OBJNMULT && cell->m_objectIndex == 131)
        frame = 124;
    if (cell->m_objectTileset == TILESET_OBJNDSRT && cell->m_objectIndex == 61)
        frame = 54;
    if (cell->m_objectTileset == TILESET_OBJNWATR && cell->m_objectIndex == 45)
        frame = 38;
    if (cell->m_objectTileset == TILESET_OBJNWATR && cell->m_objectIndex == 19)
        frame = 12;
    if (cell->m_objectTileset == TILESET_OBJNRSRC) {
        switch (cell->m_objectIndex) {
            case 1:
                frame = 0;
                break;
            case 3:
                frame = 2;
                break;
            case 5:
                frame = 4;
                break;
            case 7:
                frame = 6;
                break;
            case 9:
                frame = 8;
                break;
            case 11:
                frame = 10;
                break;
            case 13:
                frame = 12;
                break;
            case 15:
                frame = 14;
                break;
            case 19:
                frame = 18;
                break;
        }
    }

    for (neighborIndex = 0; neighborIndex < NEIGHBOR_COUNT; neighborIndex++) {
        if (isWide)
            frame--;
        else if (neighborIndex > 0)
            break;

        if (frame != NO_FRAME) {
            if (isWide) {
                cellX = frame > 6 ? x + frame - 9 : x + frame - 6;
                cellY = frame > 6 ? y : y - 1;
            } else {
                cellX = x - 1;
                cellY = y;
            }

            if (cellX >= 0) {
                if (cellY >= 0) {
                    cells[neighborIndex] = gpGame->m_worldMap.GetCell(cellX, cellY);
                    if (neighborIndex > 1) {
                        cells[neighborIndex]->m_overlayTileset = TILESET_NONE;
                        cells[neighborIndex]->m_overlayIndex = EMPTY_INDEX;
                    } else if (cells[neighborIndex]->m_objectIndex != EMPTY_INDEX) {
                        if (cells[neighborIndex]->m_objectIndex == frame
                            && cells[neighborIndex]->m_objectTileset == cell->m_objectTileset) {
                            cells[neighborIndex]->m_objectIndex = 0;
                            cells[neighborIndex]->m_objectTileset = TILESET_DUMMY;
                            cells[neighborIndex]->m_animatedObject = 0;
                        }

                        if (cells[neighborIndex]->m_extraIndex
                            && m_mapData->Extra(cells[neighborIndex]->m_extraIndex)->objectIndex
                                   != EMPTY_INDEX)
                            extras[neighborIndex] = m_mapData->Extra(cells[neighborIndex]->m_extraIndex);
                        else
                            extras[neighborIndex] = NULL;

                        while (extras[neighborIndex]) {
                            if (extras[neighborIndex]->objectIndex == frame
                                && extras[neighborIndex]->objectTileset == cell->m_objectTileset) {
                                extras[neighborIndex]->objectIndex = 0;
                                extras[neighborIndex]->objectTileset = TILESET_DUMMY;
                                extras[neighborIndex]->animatedObject = 0;
                            }

                            if (extras[neighborIndex]->nextIndex
                                && m_mapData->Extra(extras[neighborIndex]->nextIndex)->objectIndex
                                       != EMPTY_INDEX)
                                extras[neighborIndex] = m_mapData->Extra(extras[neighborIndex]->nextIndex);
                            else
                                extras[neighborIndex] = NULL;
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

    for (neighborIndex = 0; neighborIndex < CELL_COUNT; neighborIndex++) {
        if (neighborIndex == 0)
            currentCell = cell;
        else
            currentCell = cells[neighborIndex - 1];
        if (!currentCell)
            continue;
        if (currentCell->m_objectTileset != TILESET_DUMMY)
            continue;

        if (currentCell->m_extraIndex
            && m_mapData->Extra(currentCell->m_extraIndex)->objectIndex != EMPTY_INDEX)
            extra = m_mapData->Extra(currentCell->m_extraIndex);
        else
            continue;

        if (extra->objectTileset == TILESET_DUMMY || extra->objectIndex == EMPTY_INDEX)
            continue;

        currentCell->m_objectIndex = extra->objectIndex;
        currentCell->m_objectTileset = extra->objectTileset;
        currentCell->m_animatedObject = extra->animatedObject;
        currentCell->m_objectLayerBit0 = extra->objectLayerBit0;
        currentCell->m_objectLayerBit1 = extra->objectLayerBit1;
        extra->objectIndex = 0;
        extra->objectTileset = TILESET_DUMMY;
        extra->animatedObject = 0;
    }

    for (neighborIndex = 0; neighborIndex < CELL_COUNT; neighborIndex++) {
        if (neighborIndex == 0)
            currentCell = cell;
        else
            currentCell = cells[neighborIndex - 1];
        if (!currentCell)
            continue;

        if (currentCell->m_objectTileset != TILESET_DUMMY
            && currentCell->m_objectIndex != EMPTY_INDEX && !currentCell->m_objectLayerBit1)
            goto cellDone;

        if (currentCell->m_extraIndex
            && m_mapData->Extra(currentCell->m_extraIndex)->objectIndex != EMPTY_INDEX)
            extra = m_mapData->Extra(currentCell->m_extraIndex);
        else
            extra = NULL;

        while (extra) {
            if (extra->objectTileset != TILESET_DUMMY && extra->objectIndex != EMPTY_INDEX
                && !extra->objectLayerBit1)
                goto cellDone;

            if (extra->nextIndex
                && m_mapData->Extra(extra->nextIndex)->objectIndex != EMPTY_INDEX)
                extra = m_mapData->Extra(extra->nextIndex);
            else
                extra = NULL;
        }

        currentCell->m_flags |= IDX(MAP_CELL_OBJECT_SHADOW_ONLY);
    cellDone:
        ;
        changed = 0;
    }

    SendMapChange(MAP_CHANGE_ERASE_OBJECT, 0, x, y, MAP_CHANGE_VALUE, 0, 0);
    SetEnvironmentOrigin(m_mapOriginX + ENVIRONMENT_BORDER, m_mapOriginY + ENVIRONMENT_BORDER, 1);
    gpGame->SetupAdjacentMons();
}
#if H2_RETAIL_COMPILER
#undef cellX
#undef cellY
#undef cells
#undef changed
#undef currentCell
#undef extra
#undef extras
#undef frame
#undef isWide
#undef neighborIndex
#undef unusedValues
#endif

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
    colorIndex &= EVENT_BARRIER_COLOR_MASK;
    i32 passwordIndex = cell->m_objectMetadata;
    passwordIndex >>= PASSWORD_SHIFT;
    char word[INPUT_LENGTH];

    sprintf(
        gText,
        localization::Tr("event.inline.95e1a0a20e2b7676"),
        xBarrierColor[colorIndex]
    );
    GetDataEntry(gText, word, INPUT_LENGTH, NULL, 0, 1);
    if (StrEqNoCase(word, xPasswordStrings[passwordIndex])
        && PLAYER_HAS_VISITED_TENT(*gpCurPlayer, colorIndex)) {
        EventSound(cell->m_triggerType & MAP_TRIGGER_TYPE_MASK, colorIndex, &eventSample);
        NormalDialog(
            localization::Tr("event.inline.1ee518efd93cbc65"),
            NORMAL_DIALOG_INFO
        );
        return 1;
    } else {
        NormalDialog(
            localization::Tr("event.inline.907f9f79f5b78059"),
            NORMAL_DIALOG_INFO
        );
        return 0;
    }
}

#if H2_RETAIL_COMPILER
#define firstPointer firstPtr
#define secondPointer secondPtr
#endif
VA(0x004420e1, 0x95)
i8 StrEqNoCase(H2_CONST char* firstString, H2_CONST char* sndString) {
    H2_CONST char* firstPointer = firstString;
    H2_CONST char* secondPointer = sndString;
    i32 chCount = 0;
    char fstUpper;
    char sndUpper;

    while (1) {
        chCount++;
        if (chCount == SITE_STRING_LIMIT)
            return 1;
        fstUpper = toupper(static_cast<i32>(*firstPointer));
        sndUpper = toupper(static_cast<i32>(*secondPointer));
        if (fstUpper == sndUpper) {
            if (fstUpper == 0)
                return 1;
            firstPointer++;
            secondPointer++;
        } else {
            return 0;
        }
    }
}
#if H2_RETAIL_COMPILER
#undef firstPointer
#undef secondPointer
#endif

VA(0x00442176, 0xe0)
void advManager::PasswordEvent(mapCell* cell, hero*) {
    SAMPLE2 playSample = NULL;
    i32 color = cell->m_objectMetadata;
    color &= EVENT_BARRIER_COLOR_MASK;
    i32 passwordIndex = cell->m_objectMetadata;
    passwordIndex >>= PASSWORD_SHIFT;

    EventSound(cell->m_triggerType & MAP_TRIGGER_TYPE_MASK, color, &playSample);
    /* "Вы зашли в шатер и увидели пожилую женщину с магическим камнем в руках.
       Посмотрев на вас, она сказала: "Странствуя, я познала тайную магию.
       Великий оракул даровал мне знания и у меня есть ответ на твой вопрос.
       Волшебное слово, снимающее %s барьер - '%s'."" */
    sprintf(
        gText,
        localization::Tr("event.inline.536b0d04d7ca290d"),
        xBarrierColor[color],
        xPasswordStrings[passwordIndex]
    );
    NormalDialog(gText, NORMAL_DIALOG_INFO);
    gpCurPlayer->m_barrierTents |= 1 << color;
}

#if H2_RETAIL_COMPILER
#define currentCell currentCell36
#define currentSiteType currentSiteType10
#define cursedArtifactCount cursedArtifactCount2
#define eventSample eventSample9
#define experience experience11
#define index index8
#define mapX mapX37
#define mapY mapY14
#define oldQuantity oldQuantity3
#define primaryStat primaryStat15
#define siteLevel siteLevel8
#define siteType siteType4
#define stableResult stableResult26
#define unusedOne unusedOne18
#define unusedTwo unusedTwo1
#endif
VA(0x00442256, 0x6fc)
void advManager::GenericSiteEvent(mapCell* cell, hero* eventHero) {
    i32 primaryStat;
    i32 cursedArtifactCount;
    i32 H2_UNUSED(siteLevel);
    SAMPLE2 eventSample;
    i32 index;
    GenericSiteType siteType;
    i32 currentSiteType;
    i32 mapX;
    H2_ENUM_STORAGE(StableVisitResult, i32) unusedTwo;
    i32 mapY;
    mapCell* currentCell;
    i32 H2_UNUSED(unusedSite);
    H2_ENUM_STORAGE(StableVisitResult, i32) unusedOne;
    i32 oldQuantity;
    H2_ENUM_STORAGE(StableVisitResult, i8) stableResult;
    CreatureType creatureType;
    i32 experience;

    cursedArtifactCount = 0;
    eventSample = NULL;
    siteType = static_cast<GenericSiteType>(cell->m_objectMetadata);
    siteType = static_cast<GenericSiteType>(IDX(siteType) & GENERIC_SITE_TYPE_MASK);
    siteLevel = cell->m_objectMetadata;
    siteLevel >>= GENERIC_SITE_LEVEL_SHIFT;

    switch (siteType) {
        case GENERIC_SITE_ALCHEMIST_TOWER:
            for (index = 0; index < HERO_ARTIFACT_SLOT_COUNT; index++) {
                if (IsCursedItem(eventHero->m_artifacts[index]))
                    cursedArtifactCount++;
            }
            if (cursedArtifactCount != 0) {
                EventSound(
                    cell->m_triggerType & MAP_TRIGGER_TYPE_MASK, IDX(siteType), &eventSample
                );
                if (cursedArtifactCount == 1) {
                    sprintf(
                        gText,
                        localization::Tr("event.inline.8db49d8e3d5c5308")
                    );
                } else {
                    sprintf(
                        gText,
                        localization::Tr("event.inline.1c9a29276fc4ba9c"),
                        cursedArtifactCount
                    );
                }
                NormalDialog(gText, NORMAL_DIALOG_CONFIRM);
                if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_YES) {
                    if (gpCurPlayer->m_resources[IDX(RES_GOLD)] >= SITE_ALCHEMIST_COST) {
                        for (index = 0; index < HERO_ARTIFACT_SLOT_COUNT; index++) {
                            if (IsCursedItem(eventHero->m_artifacts[index])) {
                                GiveTakeArtifactStat(
                                    eventHero, eventHero->m_artifacts[index], EVENT_ARTIFACT_TAKE
                                );
                                eventHero->m_artifacts[index] = ARTIFACT_NONE;
                            }
                        }
                        gpCurPlayer->m_resources[IDX(RES_GOLD)] -= SITE_ALCHEMIST_COST;
                    } else {
                        NormalDialog(
                            localization::Tr("event.inline.35891c3b946b11c8"),
                            NORMAL_DIALOG_INFO
                        );
                    }
                }
            } else {
                NormalDialog(
                    localization::Tr("event.inline.4be252d59fc6debb"),
                    NORMAL_DIALOG_INFO
                );
            }
            break;

        case GENERIC_SITE_ARENA:
            if (HAS(eventHero->m_eventFlags, HERO_EVENT_ARENA)) {
                NormalDialog(
                    localization::Tr("event.inline.5e4d96b705906aad"),
                    NORMAL_DIALOG_INFO
                );
            } else {
                EventSound(
                    cell->m_triggerType & MAP_TRIGGER_TYPE_MASK, IDX(siteType), &eventSample
                );
                eventHero->m_eventFlags = eventHero->m_eventFlags | HERO_EVENT_ARENA;
                primaryStat = DoArenaDialog();
                eventHero->m_primaryStats[primaryStat]++;
            }
            break;

        case GENERIC_SITE_MERMAID:
            if (HAS(eventHero->m_eventFlags, HERO_EVENT_MERMAID)) {
                NormalDialog(
                    localization::Tr("event.inline.87faaee8cff00588"),
                    NORMAL_DIALOG_INFO
                );
            } else {
                EventSound(
                    cell->m_triggerType & MAP_TRIGGER_TYPE_MASK, IDX(siteType), &eventSample
                );
                eventHero->m_eventFlags = eventHero->m_eventFlags | HERO_EVENT_MERMAID;
                eventHero->m_luck++;
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    localization::Tr("event.inline.fae6f0b9369f8351"),
                    SITE_MERMAID_WINDOW_ICON,
                    0,
                    -1,
                    0,
                    -1
                );
            }
            break;

        case GENERIC_SITE_HUT_OF_MAGI:
            EventSound(cell->m_triggerType & MAP_TRIGGER_TYPE_MASK, IDX(siteType), &eventSample);
            NormalDialog(
                localization::Tr("event.inline.fd826cb8b8a55aeb"),
                NORMAL_DIALOG_INFO
            );
            for (mapX = 0; mapX < MAP_WIDTH; mapX++) {
                for (mapY = 0; mapY < MAP_HEIGHT; mapY++) {
                    currentCell = gpGame->m_worldMap.GetCell(mapX, mapY);
                    currentSiteType = currentCell->m_objectMetadata;
                    currentSiteType &= EVENT_BARRIER_COLOR_MASK;
                    if ((currentCell->m_triggerType & MAP_TRIGGER_TYPE_MASK)
                            == MAP_OBJECT_EXPANSION_OBJECT
                        && currentSiteType == SITE_HUT_COLOR) {
                        gpGame->SetVisibility(mapX, mapY, giCurPlayer, SITE_VISIBILITY_RADIUS);
                        CompleteDraw(0);
                        UpdateScreen(0, 0);
                    }
                }
            }
            break;

        case GENERIC_SITE_EYE_OF_MAGI:
            NormalDialog(
                localization::Tr("event.inline.90cd00d12dc7f24b"),
                NORMAL_DIALOG_INFO
            );
            break;

        case GENERIC_SITE_SIRENS:
            if (HAS(eventHero->m_eventFlags, HERO_EVENT_SIRENS)) {
                NormalDialog(
                    localization::Tr("event.inline.acc859e4d069b705"),
                    NORMAL_DIALOG_INFO
                );
            } else {
                experience = 0;
                for (index = 0; index < ARMY_GROUP_SLOT_COUNT; index++) {
                    creatureType = eventHero->m_army.m_creatureTypes[index];
                    if (creatureType != CREATURE_NONE) {
                        oldQuantity = eventHero->m_army.m_creatureCounts[index];
                        if (oldQuantity > 1) {
                            eventHero->m_army.m_creatureCounts[index] =
                                oldQuantity * GENERIC_SITE_SIREN_ARMY_REMAINDER;
                            experience +=
                                gMonsterDatabase[IDX(creatureType)].hitPoints
                                * (oldQuantity - eventHero->m_army.m_creatureCounts[index]);
                        }
                    }
                }
                if (experience != 0) {
                    EventSound(
                        cell->m_triggerType & MAP_TRIGGER_TYPE_MASK,
                        IDX(siteType),
                        &eventSample
                    );
                    sprintf(
                        gText,
                        localization::Tr("event.inline.d211349fcd9060a7"),
                        experience
                    );
                    NormalDialog(gText, NORMAL_DIALOG_INFO);
                    GiveExperience(eventHero, experience, 1);
                } else {
                    NormalDialog(
                        localization::Tr("event.inline.ee7fce7250a9d986"),
                        NORMAL_DIALOG_INFO
                    );
                }
                eventHero->m_eventFlags = eventHero->m_eventFlags | HERO_EVENT_SIRENS;
            }
            break;

        case GENERIC_SITE_STABLES:
            unusedOne = STABLE_VISIT_MOBILITY;
            unusedTwo = STABLE_VISIT_UPGRADE;
            stableResult = STABLE_VISIT_NONE;
            if (HAS(eventHero->m_eventFlags, HERO_EVENT_STABLES) == 0) {
                eventHero->m_eventFlags = eventHero->m_eventFlags | HERO_EVENT_STABLES;
                eventHero->m_mobility += SITE_STABLE_MOBILITY;
                eventHero->m_remainingMobility += SITE_STABLE_MOBILITY;
                stableResult |= STABLE_VISIT_MOBILITY;
            }
            if (eventHero->CreatureTypeCount(CREATURE_CAVALRY)) {
                eventHero->UpgradeCreatures(CREATURE_CAVALRY, CREATURE_CHAMPION);
                stableResult |= STABLE_VISIT_UPGRADE;
            }
            if (stableResult != STABLE_VISIT_NONE) {
                EventSound(
                    cell->m_triggerType & MAP_TRIGGER_TYPE_MASK, IDX(siteType), &eventSample
                );
            }
            sprintf(gText, xStableText[IDX(stableResult)]);
            if (HAS(stableResult, STABLE_VISIT_UPGRADE)) {
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
#if H2_RETAIL_COMPILER
#undef currentCell
#undef currentSiteType
#undef cursedArtifactCount
#undef eventSample
#undef experience
#undef index
#undef mapX
#undef mapY
#undef oldQuantity
#undef primaryStat
#undef siteLevel
#undef siteType
#undef stableResult
#undef unusedOne
#undef unusedTwo
#endif

#if H2_RETAIL_COMPILER
#define creatureType creatureType1
#endif
VA(0x00442952, 0x170)
void advManager::RecruitSiteEvent(mapCell* cell, hero* eventHero) {
    SAMPLE2 recruitSample = NULL;
    H2_ENUM_STORAGE(RecruitSiteType, u32) recruitSiteType;
    i16 quantity;
    CreatureType creatureType;
    u32 index;
    u32 packedValue;

    recruitSiteType = cell->m_objectMetadata;
    recruitSiteType &= IDX(EVENT_RECRUIT_TYPE_MASK);
    quantity = cell->m_objectMetadata;
    quantity >>= EVENT_RECRUIT_COUNT_SHIFT;

    switch (recruitSiteType) {
        case RECRUITMENT_SITE_BARROW_MOUNDS:
            creatureType = CREATURE_GHOST;
            break;
        case RECRUITMENT_SITE_EARTH_ALTAR:
            creatureType = CREATURE_EARTH_ELEMENTAL;
            break;
        case RECRUITMENT_SITE_AIR_ALTAR:
            creatureType = CREATURE_AIR_ELEMENTAL;
            break;
        case RECRUITMENT_SITE_FIRE_ALTAR:
            creatureType = CREATURE_FIRE_ELEMENTAL;
            break;
        case RECRUITMENT_SITE_WATER_ALTAR:
            creatureType = CREATURE_WATER_ELEMENTAL;
            break;
    }

    index = IDX(recruitSiteType);
    if (quantity == 0) {
        EventWindow(-1, NORMAL_DIALOG_INFO, xRecruitEmpty[index], -1, 0, -1, 0, -1);
    } else {
        EventSound(cell->m_triggerType & MAP_TRIGGER_TYPE_MASK, quantity, &recruitSample);
        EventWindow(-1, NORMAL_DIALOG_CONFIRM, xRecruitBuy[index], -1, 0, -1, 0, -1);
        if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_YES) {
            ExpansionRecruitEvent(eventHero, creatureType, &quantity);
            packedValue = (quantity << EVENT_RECRUIT_COUNT_SHIFT) | IDX(recruitSiteType);
            cell->m_objectMetadata = packedValue;
        }
    }
}
#if H2_RETAIL_COMPILER
#undef creatureType
#endif

VA(0x00442ac2, 0xaa)
void advManager::ExpansionRecruitEvent(
    hero* eventHero, H2_ENUM_PARAM(CreatureType, i32) creatureType, i16* availableCount
) {
    tag_message H2_UNUSED(dialogMessage);
    recruitUnit* recruitWindow = new recruitUnit(&eventHero->m_army, creatureType, availableCount);
    i32 H2_UNUSED(dialogResult);
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
    if (gpGame->m_availableHeroes[heroId] != HERO_AVAILABILITY_JAILED) {
        NormalDialog(
            localization::Tr("event.inline.dd448d43b4794460"),
            NORMAL_DIALOG_INFO
        );
        EraseObj(cell, x, y);
        return;
    }

    if (gpCurPlayer->m_heroCount >= PLAYER_HERO_CAPACITY) {
        NormalDialog(
            localization::Tr("event.inline.30d6f33566016021"),
            NORMAL_DIALOG_INFO
        );
        return;
    }

    EventSound(cell->m_triggerType & MAP_TRIGGER_TYPE_MASK, 0, &eventSample);
    NormalDialog(
        localization::Tr("event.inline.5aa1a1227a3f148e"),
        NORMAL_DIALOG_INFO
    );
    gpGame->m_heroRecs[heroId].m_owner = eventHero->m_owner;
    gpGame->m_availableHeroes[heroId] = eventHero->m_owner;
    freedHero = &gpGame->m_heroRecs[heroId];
    EraseObj(cell, x, y);
    gpCurPlayer->m_heroIds[gpCurPlayer->m_heroCount] = heroId;
    gpCurPlayer->m_heroCount++;
    freedHero->m_x = x;
    freedHero->m_y = y;
    freedHero->m_eventFlags = HERO_EVENT_NONE;
    freedHero->m_direction = MAP_DIRECTION_EAST;
    freedHero->m_remainingMobility = freedHero->CalcMobility();
    freedHero->m_mobility = freedHero->m_remainingMobility;
    freedHero->m_locationType = cell->m_triggerType;
    freedHero->m_occupiedTown = cell->m_objectMetadata;
    cell->m_triggerType = MAP_ACTION_TRIGGER(MAP_OBJECT_HERO_INTERACTION);
    cell->m_objectMetadata = heroId;
    SendMapChange(
        MAP_CHANGE_RECRUIT_HERO,
        heroId,
        x,
        y,
        MAP_CHANGE_CURRENT_PLAYER,
        0,
        0
    );
}

#if H2_RETAIL_COMPILER
#define combatResult combatResult1
#endif
VA(0x00442d86, 0x1a7)
void advManager::TownEvent(mapCell* cell, i32 x, i32 y) {
    hero* attackingHero;
    CombatResult combatResult;
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
        combatResult = DoCombat(
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
        if (combatResult == COMBAT_RESULT_ATTACKER)
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
#if H2_RETAIL_COMPILER
#undef combatResult
#endif

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
    const i32 H2_UNUSED(mineSound) = SOUND_MINE;
    char trackName[SOUND_FILENAME_LENGTH];
    strcpy(
        trackName,
        ""
    );
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
                strcpy(
                    trackName,
                    "treasure.82m"
                );
                break;
            case SOUND_EXPERIENCE:
                strcpy(
                    trackName,
                    "expernce.82m"
                );
                break;
            case SOUND_MORALE:
                strcpy(
                    trackName,
                    "goodmrle.82m"
                );
                break;
            case SOUND_LUCK:
                strcpy(
                    trackName,
                    "goodluck.82m"
                );
                break;
            case SOUND_PICKUP:
                strcpy(
                    trackName,
                    "pickup01.82m"
                );
                break;
            case SOUND_MINE:
                strcpy(
                    trackName,
                    "h2mine.82m"
                );
                break;
            default:
                gpSoundManager->SwitchAmbientMusic(musicTrack);
                break;
        }

        if (strlen(trackName) > SOUND_SAMPLE_NAME_EMPTY_LIMIT)
            *outSample = LoadPlaySample(trackName);
    }
}

#if H2_RETAIL_COMPILER
#define unusedStyle unusedStyle9
#define unusedValue1 eventWindowUnused4
#define unusedValue2 eventWindowUnused3
#define unusedValue3 eventWindowUnused8
#define unusedValue4 eventWindowUnused7
#define unusedValue5 eventWindowUnused6
#define unusedValue6 eventWindowUnused5
#endif
VA(0x00443340, 0xa9)
void advManager::EventWindow(
    i32 eventId,
    i32 buttons,
    H2_CONST char* text,
    i32 type1,
    i32 value1,
    i32 type2,
    i32 value2,
    i32 type3
) {
    i32 H2_UNUSED(unusedValue1);
    i32 H2_UNUSED(unusedValue2);
    i32 H2_UNUSED(eventDone);
    i32 H2_UNUSED(unusedValue3);
    i32 H2_UNUSED(unusedValue4);
    i32 H2_UNUSED(unusedValue5);
    i32 H2_UNUSED(unusedValue6);
    char eventText[EVENT_TEXT_BUFFER_SIZE];
    i32 H2_UNUSED(unusedStyle);

    eventDone = 0;
    unusedStyle = 1;

    if (eventId >= 0 && eventId < KB_EVENT_TEXT_TABLE_COUNT)
        sprintf(eventText, gEventText[eventId]);
    else if (eventId == MAP_EVENT_REWARD_NONE)
        sprintf(eventText, text);
    else
        sprintf(
            eventText,
            "Event ID %d",
            eventId
        );

    NormalDialog(eventText, buttons, -1, -1, IDX(type1), value1, type2, value2, type3, 0);
}
#if H2_RETAIL_COMPILER
#undef unusedStyle
#undef unusedValue1
#undef unusedValue2
#undef unusedValue3
#undef unusedValue4
#undef unusedValue5
#undef unusedValue6
#endif

VA(0x004433e9, 0x92)
i32 GiveArtifact(hero* eventHero, ArtifactType artifact, b32 checkEndGame, i8 artifactExtra) {
    i32 artifactSlot;

    for (artifactSlot = 0; artifactSlot < HERO_ARTIFACT_SLOT_COUNT; artifactSlot++) {
        if (eventHero->m_artifacts[artifactSlot] == ARTIFACT_NONE) {
            break;
        }
    }

    if (artifactSlot == HERO_ARTIFACT_SLOT_COUNT) {
        return IDX(ARTIFACT_NONE);
    }

    eventHero->m_artifacts[artifactSlot] = artifact;
    eventHero->m_artifactExtra[artifactSlot] = artifactExtra;
    GiveTakeArtifactStat(eventHero, artifact, false);
    eventHero->CheckAnduranPieces(false);
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
        GiveArtifact(eventHero, artifactId, true);
    return artifactId;
}

#if H2_RETAIL_COMPILER
#define levelGap levelGap1
#define newLevel newLevel1
#define unusedValue1 unusedLevel2
#define unusedValue2 unusedLevel1
#endif
VA(0x004434c9, 0x61)
i32 advManager::GiveExperience(hero* eventHero, i32 experience, i32 checkLevel) {
    i32 oldLevel;
    i32 H2_UNUSED(unusedValue1);
    i32 H2_UNUSED(unusedValue2);
    i32 newLevel;
    i32 H2_UNUSED(levelGap);

    oldLevel = eventHero->GetLevel(eventHero->m_experience);
    eventHero->m_level = oldLevel;
    eventHero->m_experience += experience;
    newLevel = eventHero->GetLevel(eventHero->m_experience);
    if (checkLevel)
        eventHero->CheckLevel();
    return newLevel - oldLevel;
}
#if H2_RETAIL_COMPILER
#undef levelGap
#undef newLevel
#undef unusedValue1
#undef unusedValue2
#endif

VA(0x0044352a, 0x83)
void advManager::GiveResource(hero* eventHero, ResourceType resourceType, i32 amount) {
    if (resourceType >= RES_WOOD && resourceType <= RES_GOLD)
        gpGame->m_players[IDX(eventHero->m_owner)].m_resources[IDX(resourceType)] += amount;
    if (resourceType == RES_GOLD && gbHumanPlayer[IDX(eventHero->m_owner)])
        CheckEndGame(END_GAME_FORCE_NONE, false);
}

VA(0x004435ad, 0xe0)
void advManager::RecruitEvent(
    hero* eventHero, H2_ENUM_PARAM(CreatureType, i32) creatureType, mapCell* cell
) {
    tag_message H2_UNUSED(recruitMessage);
    i16 availableCount = cell->m_objectMetadata;
    recruitUnit* recruitWindow =
        new recruitUnit(&eventHero->m_army, creatureType, &availableCount);
    i32 H2_UNUSED(eventResult);

    if (recruitWindow == NULL)
        MemError();
    gpExec->DoDialog(recruitWindow);
    delete recruitWindow;
    cell->m_objectMetadata = availableCount;
}

VA(0x0044368d, 0x232)
i32 advManager::SkeletonEvent(hero* eventHero, mapCell* cell, H2_CONST char* text, i32 x, i32 y) {
    ArtifactType H2_UNUSED(artifactId);

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
i32 advManager::ZombieEvent(hero* eventHero, mapCell* cell, H2_CONST char* text, i32 x, i32 y) {
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
i32 advManager::GhostEvent(hero* eventHero, mapCell* cell, H2_CONST char* text, i32 x, i32 y) {
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
                sprintf(
                    gText,
                    "%s",
                    text
                );
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
                sprintf(
                    gText,
                    "%s",
                    text
                );
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
                sprintf(
                    gText,
                    "%s",
                    text
                );
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
                sprintf(
                    gText,
                    "%s",
                    text
                );
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
        if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_YES) {
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

#if H2_RETAIL_COMPILER
#define placement placement4
#define stackIndex stackIdx
#define unusedValue combatUnused0
#endif
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
    i32 placement[ARMY_GROUP_SLOT_COUNT];
    i32 H2_UNUSED(combatUnused);
    i32 lastCount;
    i32 groupCount;
    i32 stackCount;
    CreatureType savedTypes[ARMY_GROUP_SLOT_COUNT];
    CombatResult battleOutcome;
    i32 savedCounts[ARMY_GROUP_SLOT_COUNT];
    i32 stackIndex;
    i32 H2_UNUSED(unusedValue);

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

    CLEAR_ARMY_GROUP(*gpMonGroup);
    stackCount = ARMY_GROUP_SLOT_COUNT - secondaryStacks - tertiaryStacks;
    if (stackCount < 1)
        stackCount = 1;
    groupCount = 0;
    SRand(combatX + combatY);
    if (stackCount == ARMY_GROUP_SLOT_COUNT
        && HAS(gMonsterDatabase[IDX(monsterType)].attributes, MONSTER_FLAGS_SHOOTER) == 0) {
        i32 roll = SRandom(0, MONSTER_RANDOM_MAX);
        if (roll < MONSTER_REDUCED_STACK_CHANCE)
            stackCount = MONSTER_REDUCED_STACK_COUNT;
        else if (roll < MONSTER_FOUR_STACK_THRESHOLD)
            stackCount = MONSTER_FOUR_STACK_COUNT;
    }

    for (stackIndex = 0; stackIndex < stackCount; stackIndex++) {
        if (stackIndex == (stackCount >> 1)
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
            gpMonGroup->m_creatureTypes[groupCount + stackIndex] =
                NextCreatureType(monsterType);
        else
            gpMonGroup->m_creatureTypes[groupCount + stackIndex] =
                monsterType;
        gpMonGroup->m_creatureCounts[groupCount + stackIndex] =
            monsterCount / stackCount + (monsterCount % stackCount > stackIndex);
    }

    groupCount += stackCount;
    if (secondaryStacks != 0) {
        stackCount = secondaryStacks;
        for (stackIndex = 0; stackIndex < stackCount; stackIndex++) {
            gpMonGroup->m_creatureTypes[groupCount + stackIndex] =
                secondaryType;
            gpMonGroup->m_creatureCounts[groupCount + stackIndex] =
                secondaryCount / stackCount + (secondaryCount % stackCount > stackIndex);
        }
    }

    groupCount += stackCount;
    if (tertiaryStacks != 0) {
        stackCount = tertiaryStacks;
        for (stackIndex = 0; stackIndex < stackCount; stackIndex++) {
            gpMonGroup->m_creatureTypes[groupCount + stackIndex] =
                tertiaryType;
            gpMonGroup->m_creatureCounts[groupCount + stackIndex] =
                tertiaryCount / stackCount + (secondaryCount % stackCount > stackIndex);
        }
    }
    lastCount = stackCount;

    for (stackIndex = 0; stackIndex < ARMY_GROUP_SLOT_COUNT; stackIndex++) {
        if (gpMonGroup->m_creatureCounts[stackIndex] <= 0)
            gpMonGroup->m_creatureTypes[stackIndex] = CREATURE_NONE;
    }
    for (stackIndex = 0; stackIndex < ARMY_GROUP_SLOT_COUNT; stackIndex++)
        placement[stackIndex] = stackIndex;

    if (lastCount == 1) {
        placement[2] = 0;
        placement[0] = 2;
    } else if (lastCount == 2) {
        placement[1] = 1;
        placement[3] = 0;
        placement[0] = 3;
    } else if (lastCount == 3) {
        placement[0] = 3;
        placement[1] = 0;
        placement[2] = 1;
        placement[3] = 2;
    } else if (tertiaryStacks == 1 && secondaryStacks == 1) {
        placement[1] = 4;
        placement[4] = 1;
    } else if (tertiaryStacks == 1 && secondaryStacks == 2) {
        placement[1] = 2;
        placement[2] = 4;
        placement[3] = 3;
        placement[4] = 1;
    } else if (secondaryStacks == 2) {
        placement[1] = 4;
        placement[4] = 1;
    }

    for (stackIndex = 0; stackIndex < ARMY_GROUP_SLOT_COUNT; stackIndex++) {
        savedTypes[stackIndex] = gpMonGroup->m_creatureTypes[stackIndex];
        savedCounts[stackIndex] = gpMonGroup->m_creatureCounts[stackIndex];
    }
    for (stackIndex = 0; stackIndex < ARMY_GROUP_SLOT_COUNT; stackIndex++) {
        gpMonGroup->m_creatureTypes[stackIndex] = savedTypes[placement[stackIndex]];
        gpMonGroup->m_creatureCounts[stackIndex] =
            savedCounts[placement[stackIndex]];
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
#if H2_RETAIL_COMPILER
#undef placement
#undef stackIndex
#undef unusedValue
#endif

VA(0x00444571, 0x3d8)
void GiveTakeArtifactStat(hero* targetHero, ArtifactType artifact, b32 take) {
    i32 stats[HERO_PRIMARY_STAT_COUNT];
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

    for (i = 0; i < HERO_PRIMARY_STAT_COUNT; i++) {
        targetHero->m_primaryStats[i] += (take == EVENT_ARTIFACT_TAKE ? -1 : 1) * stats[i];
        if (i == IDX(HERO_PRIMARY_KNOWLEDGE) && take == EVENT_ARTIFACT_TAKE) {
            maxSpellPoints = HERO_NORMAL_SPELL_POINTS(*targetHero);
            if (targetHero->m_spellPoints > maxSpellPoints)
                targetHero->m_spellPoints = maxSpellPoints;
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
    for (targetSlot = 0; targetSlot < HERO_ARTIFACT_SLOT_COUNT; targetSlot++) {
        if (destinationHero->m_artifacts[targetSlot] == ARTIFACT_NONE) {
            for (sourceArtifactSlot = 0; sourceArtifactSlot < HERO_ARTIFACT_SLOT_COUNT;
                 sourceArtifactSlot++) {
                if (sourceHero->m_artifacts[sourceArtifactSlot] != ARTIFACT_NONE
                    && sourceHero->m_artifacts[sourceArtifactSlot] != ARTIFACT_MAGIC_BOOK) {
                    if (sourceHero->m_artifacts[sourceArtifactSlot] <= ARTIFACT_GOLDEN_GOOSE) {
                        if (gbThisNetHumanPlayer[IDX(sourceHero->m_owner)]
                            || gbThisNetHumanPlayer[IDX(destinationHero->m_owner)]) {
                            sprintf(
                                gText,
                                localization::Tr("event.inline.aa7c539682b9fd51")
                                    ,
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
    destinationHero->CheckAnduranPieces(false);
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
    i32 H2_UNUSED(groupValues)[ARMY_GROUP_SLOT_COUNT];
    i32 lowestValue;
    i32 creatureValue;

    if (gbHumanPlayer[IDX(eventHero->m_owner)] == 0)
        return;
    if (Random(EVENT_WHIRLPOOL_TRIGGER_ROLL, EVENT_WHIRLPOOL_TRIGGER_MAX)
        != EVENT_WHIRLPOOL_TRIGGER_ROLL)
        return;
    {
        lowestValue = EVENT_WHIRLPOOL_ARMY_VALUE_LIMIT;
        selectedSlot = -1;
        for (slotNo = 0; slotNo < ARMY_GROUP_SLOT_COUNT; slotNo++) {
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
                sprintf(
                    gText,
                    "killfade.82M"
                );
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
        WaitEndSample(&fizzleSample);
    }
}

#if H2_RETAIL_COMPILER
#define abandonedMineValue abandonedMineValue_f
#define adjacentMonster adjacentMonster_j
#define artifactGuardCount artifactGuardCount_b
#define artifactGuardResult artifactGuardResult_e
#define artifactResource artifactResource_p
#define attackerLoss attackerLoss_c
#define battleResult battleResult_l
#define battleWon battleWon_j
#define boat boat_k
#define combatResult combatResult_d
#define creatureCosts creatureCosts_a
#define creatureFlag creatureFlag_l
#define creatureType creatureType_i
#define defenderLoss defenderLoss_k
#define eraseObject eraseObject_l
#define eventArtifact artifact_g
#define eventExtra eventExtra_o
#define eventResourceType resourceType_a
#define eventType eventType_g
#define eventWork eventWork_o
#define exitY exitY_d
#define heroCombatResult heroCombatResult_h
#define heroLevel heroLevel_e
#define index index_h
#define levelExperience levelExperience_g
#define occupiedTown occupiedTown_b
#define oldPlayer oldPlayer_o
#define oldPlayerData oldPlayerData_h
#define otherHero otherHero_e
#define primaryAmount primaryAmount_j
#define primaryReward primaryReward_e
#define purchaseCost purchaseCost_i
#define purchaseCount purchaseCount_o
#define purchaseValue purchaseValue_a
#define pyramidBattleValue pyramidBattleValue_l
#define savedShowIt savedShowIt_e
#define secondaryAmount secondaryAmount_j
#define secondaryReward secondaryReward_k
#define spellPower spellPower_j
#define springSpellPoints springSpellPoints_j
#define survivingCount survivingCount_a
#define unusedResult unusedResult_m
#define wellSpellPoints wellSpellPoints_o
#endif
VA(0x00444d73, 0x2b6c)
void advManager::DoAIEvent(mapCell* cell, hero* eventHero, i32 x, i32 y) {
    i32 H2_UNUSED(secondaryAmount);
    i32 H2_UNUSED(secondaryReward);
    i32 H2_UNUSED(primaryReward);
    i32 H2_UNUSED(primaryAmount);
    i32 wellSpellPoints;
    i32 springSpellPoints;
    float defenderLoss;
    float attackerLoss;
    i32 heroLevel;
    i32 levelExperience;
    i32 creatureCosts[IDX(RES_COUNT)];
    i32 spellPower;
    i32 adjacentMonster;
    b32 H2_UNUSED(savedShowIt);
    boatRecord* boat;
    i32 exitCount;
    ResourceType eventResourceType;
    i32 artifactGuardResult;
    i32 exitY;
    i32 exitX;
    ArtifactType eventArtifact;
    i32 artifactResource;
    hero* otherHero;
    CombatResult heroCombatResult;
    i32 heroInteractionResult;
    CreatureType artifactGuardCount;
    i32 survivingCount;
    mapEventExtra* eventExtra;
    MapObjectType eventType;
    i32 H2_UNUSED(eventWork)[IDX(RES_COUNT)];
    i32 eraseObject;
    i32 battleResult;
    b32 creatureFlag;
    i32 oldPlayer;
    i32 purchaseCost;
    i32 battleWon;
    i32 pyramidBattleValue;
    i32 H2_UNUSED(unusedResult);
    i32 index;
    i32 purchaseCount;
    CreatureType creatureType;
    i32 purchaseValue;
    i32 abandonedMineValue;
    town* occupiedTown;
    playerData* oldPlayerData;
    i32 combatResult;

    occupiedTown = NULL;
    eventType = cell->m_triggerType & MAP_TRIGGER_TYPE_MASK;
    eraseObject = 0;
    unusedResult = 0;
    oldPlayer = giCurPlayer;
    oldPlayerData = gpCurPlayer;

    if (x == eventHero->m_destinationX && y == eventHero->m_destinationY) {
        eventHero->m_destinationY = -1;
        eventHero->m_destinationX = -1;
    }
    --eventHero->m_remainingMobility;
    switch (eventType) {
        case MAP_OBJECT_COAST:
            if (eventHero->IsEmbarked()) {
                eventHero->m_eventFlags = eventHero->m_eventFlags & ~HERO_EVENT_EMBARKED;
                eventHero->m_remainingMobility = 0;
                eventHero->m_direction = m_cursorDirection;
                m_cursorType = eventHero->m_cursorType;
                m_cursorFrame = GetCursorBaseFrame(m_cursorDirection);
                m_cursorActive = true;
                CheckAdjacentMon(&adjacentMonster);
            }
            break;

        case MAP_OBJECT_BOAT:
            boat = &gpGame->m_boats[cell->m_objectMetadata];
            gpGame->RestoreCell(
                -1,
                -1,
                boat->savedTriggerType,
                boat->savedEventData,
                cell,
                EVENT_BOAT_RESTORE_MODE
            );
            eventHero->m_eventFlags = eventHero->m_eventFlags | HERO_EVENT_EMBARKED;
            eventHero->m_remainingMobility = 0;
            boat->heroId = eventHero->m_id;
            boat->owner = eventHero->m_owner;
            m_cursorType = HERO_TYPE_BOAT;
            m_cursorDirection = boat->direction;
            m_cursorFrame = GetCursorBaseFrame(m_cursorDirection);
            m_cursorActive = true;
            break;

        case MAP_OBJECT_ALCHEMIST_LAB:
        case MAP_OBJECT_MINE:
        case MAP_OBJECT_SAWMILL:
            if (gpGame->m_mineOwners[cell->m_objectMetadata] == giCurPlayer)
                break;
            if (gpGame->m_mines[cell->m_objectMetadata].guardianType != CREATURE_NONE) {
                index = gpGame->m_mines[cell->m_objectMetadata].guardianCount;
                combatResult = gpPhilAI->CombatMonsterEvent(
                    eventHero,
                    gpGame->m_mines[cell->m_objectMetadata].guardianType,
                    &index,
                    cell
                );
                if (combatResult == 0)
                    break;
                gpGame->m_mines[cell->m_objectMetadata].guardianType = CREATURE_NONE;
                gpGame->m_mines[cell->m_objectMetadata].guardianCount = 0;
                eventHero->CheckLevel();
            }
            gpGame->ClaimMine(cell->m_objectMetadata, giCurPlayer);
            if (eventType == MAP_OBJECT_MINE) {
                for (index = IDX(SPELL_SET_EARTH_GUARDIAN);
                     index < IDX(SPELL_SET_WATER_GUARDIAN);
                     ++index) {
                    if (eventHero->HasSpell(SpellType(index))
                        && eventHero->m_spellPoints > GetManaCost(SpellType(index), eventHero)) {
                        eventHero->m_spellPoints -= GetManaCost(SpellType(index), eventHero);
                        gpGame->m_mines[cell->m_objectMetadata].guardianType =
                            static_cast<CreatureType>(index + 1);
                        spellPower = eventHero->Stats(HERO_PRIMARY_SPELL_POWER);
                        if (spellPower > EVENT_MINE_SPELL_POWER_MAX)
                            spellPower = EVENT_MINE_SPELL_POWER_MAX;
                        gpGame->m_mines[cell->m_objectMetadata].guardianCount =
                            spellPower * EVENT_MINE_SPELL_COUNT_SCALE;
                        index = EVENT_MINE_SPELL_LOOP_END;
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
                    true
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
            eraseObject = 1;
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
                        true
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
                        index = cell->m_objectMetadata & WAGON_ARTIFACT_MASK;
                        GiveArtifact(eventHero, ArtifactType(index), true);
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
                    true
                );
                GiveResource(eventHero, RES_GOLD, SEA_CHEST_ARTIFACT_GOLD);
            } else if (cell->m_objectMetadata != 0) {
                GiveResource(eventHero, RES_GOLD, SEA_CHEST_GOLD);
            }
            eraseObject = 1;
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
            eraseObject = 1;
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
            eraseObject = 1;
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
            eventResourceType = static_cast<ResourceType>(cell->m_objectIndex / 2);
            GiveResource(
                eventHero,
                eventResourceType,
                eventResourceType == RES_GOLD ? cell->m_objectMetadata * CAMPFIRE_GOLD_MULTIPLIER
                                           : cell->m_objectMetadata
            );
            eraseObject = 1;
            break;

        case MAP_OBJECT_WINDMILL:
            if (cell->m_objectMetadata != WINDMILL_EMPTY) {
                GiveResource(
                    eventHero,
                    ResourceType(cell->m_objectMetadata),
                    WINDMILL_RESOURCE_AMOUNT
                );
                cell->m_objectMetadata = WINDMILL_EMPTY;
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
            creatureType = CREATURE_SPRITE;
            creatureFlag = false;
            goto creaturePurchase;
        case MAP_OBJECT_RUINS:
            creatureType = CREATURE_MEDUSA;
            creatureFlag = false;
            goto creaturePurchase;
        case MAP_OBJECT_TROLL_BRIDGE:
            if (cell->m_objectMetadata & DWELLING_GUARDED_FLAG)
                break;
            creatureType = CREATURE_TROLL;
            creatureFlag = false;
            goto creaturePurchase;
        case MAP_OBJECT_CITY_OF_DEAD:
            if (cell->m_objectMetadata & DWELLING_GUARDED_FLAG)
                break;
            creatureType = CREATURE_POWER_LICH;
            creatureFlag = false;
            goto creaturePurchase;
        case MAP_OBJECT_DRAGON_CITY:
            if (cell->m_objectMetadata & DWELLING_GUARDED_FLAG)
                break;
            creatureType = CREATURE_RED_DRAGON;
            creatureFlag = false;
            goto creaturePurchase;
        case MAP_OBJECT_HALFLING_HOLE:
            creatureType = CREATURE_HALFLING;
            creatureFlag = true;
            goto creaturePurchase;
        case MAP_OBJECT_ANCIENT_LAMP:
            creatureType = CREATURE_GENIE;
            creatureFlag = false;
            goto creaturePurchase;
        case MAP_OBJECT_WAGON_CAMP:
            creatureType = CREATURE_ROGUE;
            creatureFlag = false;
            goto creaturePurchase;
        case MAP_OBJECT_DESERT_TENT:
            creatureType = CREATURE_NOMAD;
            creatureFlag = false;
            goto creaturePurchase;
        case MAP_OBJECT_WATCH_TOWER:
            creatureType = CREATURE_ORC;
            creatureFlag = true;
            goto creaturePurchase;
        case MAP_OBJECT_TREE_HOUSE:
            creatureType = CREATURE_SPRITE;
            creatureFlag = true;
            goto creaturePurchase;
        case MAP_OBJECT_ARCHER_HOUSE:
            creatureType = CREATURE_ARCHER;
            creatureFlag = true;
            goto creaturePurchase;
        case MAP_OBJECT_GOBLIN_HUT:
            creatureType = CREATURE_GOBLIN;
            creatureFlag = true;
            goto creaturePurchase;
        case MAP_OBJECT_PEASANT_HUT:
            creatureType = CREATURE_PEASANT;
            creatureFlag = true;
            goto creaturePurchase;
        case MAP_OBJECT_DWARF_COTTAGE:
        case MAP_OBJECT_SIRENS:
            creatureType = CREATURE_DWARF;
            creatureFlag = true;
            goto creaturePurchase;
        case MAP_OBJECT_CAVE:
            creatureType = CREATURE_CENTAUR;
            creatureFlag = true;
            goto creaturePurchase;
        case MAP_OBJECT_EXCAVATION:
            creatureType = CREATURE_SKELETON;
            creatureFlag = true;
            goto creaturePurchase;

        creaturePurchase:
            if (cell->m_objectMetadata != 0) {
                gpPhilAI->EvaluateOneTimeCreaturePurchase(
                    creatureType,
                    cell->m_objectMetadata,
                    creatureFlag,
                    purchaseCount,
                    purchaseCost,
                    purchaseValue
                );
                if (purchaseCount > 0) {
                    gpGame->GiveArmy(
                        &eventHero->m_army,
                        creatureType,
                        purchaseCount,
                        purchaseValue
                    );
                    cell->m_objectMetadata -= purchaseCount;
                    if (creatureFlag == 0) {
                        GetMonsterCost(creatureType, creatureCosts);
                        for (index = 0; index < IDX(RES_COUNT); ++index)
                            gpCurPlayer->m_resources[index] -=
                                purchaseCount
                                * creatureCosts[index];
                    }
                }
            }
            if (cell->m_objectMetadata == 0 && eventType == MAP_OBJECT_ANCIENT_LAMP)
                eraseObject = 1;
            break;

        case MAP_OBJECT_MONSTER:
            ComputerMonsterInteract(cell, eventHero, &eraseObject);
            break;

        case MAP_OBJECT_TREE_OF_KNOWLEDGE:
            if ((eventHero->m_treeKnowledgeVisits
                 & (1U << (cell->m_objectMetadata & TREE_KNOWLEDGE_VISIT_INDEX_MASK)))
                == 0) {
                heroLevel = eventHero->GetLevel(eventHero->m_experience);
                levelExperience = eventHero->GetExperience(heroLevel + 1)
                                    - eventHero->GetExperience(heroLevel);
                switch (cell->m_objectMetadata >> TREE_KNOWLEDGE_MODE_SHIFT) {
                    case TREE_KNOWLEDGE_FREE:
                        GiveExperience(eventHero, levelExperience, 1);
                        eventHero->m_treeKnowledgeVisits |=
                            1U << (cell->m_objectMetadata & TREE_KNOWLEDGE_VISIT_INDEX_MASK);
                        eventHero->CheckLevel();
                        break;
                    case TREE_KNOWLEDGE_GOLD:
                        if (gpCurPlayer->m_resources[IDX(RES_GOLD)] > TREE_KNOWLEDGE_GOLD_COST) {
                            gpCurPlayer->m_resources[IDX(RES_GOLD)] -= TREE_KNOWLEDGE_GOLD_COST;
                            GiveExperience(eventHero, levelExperience, 1);
                            eventHero->m_treeKnowledgeVisits |=
                                1U << (cell->m_objectMetadata & TREE_KNOWLEDGE_VISIT_INDEX_MASK);
                            eventHero->CheckLevel();
                        }
                        break;
                    case TREE_KNOWLEDGE_GEMS:
                        if (gpCurPlayer->m_resources[IDX(RES_GEMS)] > TREE_KNOWLEDGE_GEM_COST) {
                            gpCurPlayer->m_resources[IDX(RES_GEMS)] -= TREE_KNOWLEDGE_GEM_COST;
                            GiveExperience(eventHero, levelExperience, 1);
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
                              + HERO_BASE_LEARNABLE_SPELL_LEVEL) {
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
            for (exitY = 0; exitY < MAP_HEIGHT; ++exitY) {
                for (exitX = 0; exitX < MAP_WIDTH; ++exitX) {
                    if (H2_ENUM_RAW(gpGame->m_worldMap.GetCell(exitX, exitY)->m_triggerType)
                            == static_cast<u8>(eventType | MAP_TRIGGER_ACTION_FLAG)
                        && gpGame->m_worldMap.GetCell(exitX, exitY)->m_objectIndex
                               == cell->m_objectIndex
                        && MANHATTAN_LENGTH(exitX - x, exitY - y)
                               > (eventType == MAP_OBJECT_STONE_LITHS
                                      ? EVENT_TELEPORT_STONE_DISTANCE
                                      : EVENT_TELEPORT_WHIRLPOOL_DISTANCE)) {
                        ++exitCount;
                    }
                }
            }
            if (exitCount >= 1) {
                if (exitCount > 1)
                    exitCount = Random(1, exitCount);
                for (exitY = 0; exitY < MAP_HEIGHT; ++exitY) {
                    for (exitX = 0; exitX < MAP_WIDTH; ++exitX) {
                        if (H2_ENUM_RAW(gpGame->m_worldMap.GetCell(exitX, exitY)->m_triggerType)
                                == static_cast<u8>(eventType | MAP_TRIGGER_ACTION_FLAG)
                            && gpGame->m_worldMap.GetCell(exitX, exitY)->m_objectIndex
                                   == cell->m_objectIndex
                            && MANHATTAN_LENGTH(exitX - x, exitY - y)
                                   > (eventType == MAP_OBJECT_STONE_LITHS
                                          ? EVENT_TELEPORT_STONE_DISTANCE
                                          : EVENT_TELEPORT_WHIRLPOOL_DISTANCE)
                            && --exitCount <= 0) {
                            goto teleportDestination;
                        }
                    }
                }
            teleportDestination:
                StopCursor(1);
                gpAdvManager->TeleportTo(eventHero, exitX, exitY, 0, 0);
            }
            break;

        case MAP_OBJECT_ARTIFACT:
            artifactResource = (cell->m_objectMetadata & ARTIFACT_EVENT_RESOURCE_MASK)
                                 >> ARTIFACT_EVENT_RESOURCE_SHIFT;
            eventArtifact = static_cast<ArtifactType>(cell->m_objectIndex / 2);
            artifactGuardCount = static_cast<CreatureType>(
                cell->m_objectMetadata & ARTIFACT_EVENT_MONSTER_MASK
            );
            if (eventHero->NumArtifacts() == HERO_ARTIFACT_SLOT_COUNT)
                break;
            if (eventArtifact == ARTIFACT_SPELL_SCROLL) {
                GiveArtifact(eventHero, eventArtifact, true, cell->m_objectMetadata);
                eraseObject = 1;
                break;
            }
            if (cell->m_objectMetadata & MAP_EVENT_ARTIFACT_GUARD_FLAG) {
                if (artifactGuardCount == CREATURE_ROGUE) {
                    artifactGuardResult = EVENT_ROGUE_COUNT;
                    goto artifactFight;
                }
                artifactGuardResult = 1;
                if (gpPhilAI->ChooseToFightForArtifact(eventArtifact, artifactGuardCount, 1)) {
                artifactFight:
                    if (gpPhilAI->CombatMonsterEvent(
                            eventHero,
                            artifactGuardCount,
                            &artifactGuardResult,
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
                    for (index = 0; index < IDX(RES_COUNT); ++index) {
                        if (gpCurPlayer->m_resources[index] < 0)
                            gpCurPlayer->m_resources[index] = 0;
                    }
                    GiveArtifact(eventHero, eventArtifact, true);
                    eraseObject = 1;
                    break;
                case ARTIFACT_EVENT_MODE_GOLD:
                    if (gpPhilAI->NetValueOfArtifact(IDX(eventArtifact), ARTIFACT_EVENT_GOLD_COST, 0, 0)) {
                        gpGame->m_players[IDX(eventHero->m_owner)].m_resources[IDX(RES_GOLD)] -=
                            ARTIFACT_EVENT_GOLD_COST;
                        goto artifactPickup;
                    }
                    break;
                case ARTIFACT_EVENT_MODE_RESOURCE_3:
                    if (gpPhilAI->NetValueOfArtifact(
                            IDX(eventArtifact),
                            ARTIFACT_EVENT_RESOURCE_3_GOLD_COST,
                            artifactResource,
                            ARTIFACT_EVENT_RESOURCE_3_AMOUNT
                        )) {
                        gpGame->m_players[IDX(eventHero->m_owner)].m_resources[IDX(RES_GOLD)] -=
                            ARTIFACT_EVENT_RESOURCE_3_GOLD_COST;
                        gpGame->m_players[IDX(eventHero->m_owner)].m_resources[IDX(artifactResource)] -=
                            ARTIFACT_EVENT_RESOURCE_3_AMOUNT;
                        goto artifactPickup;
                    }
                    break;
                case ARTIFACT_EVENT_MODE_RESOURCE_5:
                    if (gpPhilAI->NetValueOfArtifact(
                            IDX(eventArtifact),
                            ARTIFACT_EVENT_RESOURCE_5_GOLD_COST,
                            artifactResource,
                            ARTIFACT_EVENT_RESOURCE_5_AMOUNT
                        )) {
                        gpGame->m_players[IDX(eventHero->m_owner)].m_resources[IDX(RES_GOLD)] -=
                            ARTIFACT_EVENT_RESOURCE_5_GOLD_COST;
                        gpGame->m_players[IDX(eventHero->m_owner)].m_resources[IDX(artifactResource)] -=
                            ARTIFACT_EVENT_RESOURCE_5_AMOUNT;
                        goto artifactPickup;
                    }
                    break;
            }
            break;

        case MAP_OBJECT_HERO_INTERACTION:
            otherHero = gpGame->GetHero(cell->m_objectMetadata);
            savedShowIt = bShowIt;
            if (otherHero->m_owner == giCurPlayer) {
                gpPhilAI->HeroInteractionAtHero(eventHero, otherHero, 0, &heroInteractionResult);
                return;
            }
            if (otherHero->m_locationType
                == (MAP_ACTION_TRIGGER(MAP_OBJECT_CASTLE)))
                occupiedTown = gpGame->GetTown(otherHero->m_occupiedTown);

            if (gbHumanPlayer[IDX(otherHero->m_owner)] == 0) {
                combatResult = gpPhilAI->QuickCombat(
                    &eventHero->m_army,
                    eventHero,
                    &otherHero->m_army,
                    otherHero,
                    0,
                    0,
                    attackerLoss,
                    defenderLoss
                );
                if (combatResult != 0 && occupiedTown != NULL) {
                    combatResult = gpPhilAI->QuickCombat(
                        &eventHero->m_army,
                        eventHero,
                        &occupiedTown->m_army,
                        NULL,
                        1,
                        occupiedTown->m_id,
                        attackerLoss,
                        defenderLoss
                    );
                }
            } else {
                if (occupiedTown != NULL)
                    occupiedTown->m_occupyingHeroId = otherHero->m_id;
                heroCombatResult = DoCombat(
                    x,
                    y,
                    eventHero,
                    &eventHero->m_army,
                    occupiedTown,
                    otherHero,
                    &otherHero->m_army,
                    x,
                    y,
                    -1,
                    1
                );
                if (heroCombatResult == COMBAT_RESULT_ATTACKER && occupiedTown != NULL)
                    gpGame->ClaimTown(occupiedTown->m_id, giCurPlayer, 0);
            }
            CompleteDraw(0);
            break;

        case MAP_OBJECT_SIGN:
            break;

        case MAP_OBJECT_BOTTLE:
            eraseObject = 1;
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
                    GiveResource(eventHero, RES_GOLD, DAEMON_GOLD);
                    break;
                case DAEMON_REWARD_RANSOM:
                    if (gpGame->m_players[IDX(eventHero->m_owner)].m_resources[IDX(RES_GOLD)]
                        >= DAEMON_GOLD) {
                        if (gpPhilAI->ChooseToPayRansomOnHero(DAEMON_GOLD)) {
                            gpGame->m_players[IDX(eventHero->m_owner)].m_resources[IDX(RES_GOLD)] -=
                                DAEMON_GOLD;
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
            for (index = 0; index < ARMY_GROUP_SLOT_COUNT; ++index) {
                gpMonGroup->m_creatureTypes[index] = CREATURE_ROYAL_MUMMY;
                gpMonGroup->m_creatureCounts[index] = PYRAMID_GUARD_STACK_QUANTITY;
            }
            index = cell->m_objectMetadata - 1;
            pyramidBattleValue = static_cast<i32>(
                gsSpellInfo[index].aiValue
                * (HAS(gsSpellInfo[index].attributes, SPELL_INFO_ATTRIBUTE_POWER)
                       ? (eventHero->Stats(HERO_PRIMARY_SPELL_POWER) <= BATTLE_STAT_TABLE_MAX_INDEX
                              ? gfBattleStat[eventHero->Stats(HERO_PRIMARY_SPELL_POWER)]
                              : gfBattleStat[BATTLE_STAT_TABLE_MAX_INDEX])
                       : 1.0f)
                * gpCurPlayer->m_aiData.m_upgradeValueWeight
            );
            gpPhilAI->ChooseEvaluateBattle(
                &eventHero->m_army,
                eventHero,
                gpMonGroup,
                NULL,
                0,
                0,
                pyramidBattleValue,
                battleWon,
                battleResult
            );
            if (battleWon != 0) {
                index = PYRAMID_GUARD_COUNT;
                combatResult = gpPhilAI->CombatMonsterEvent(
                    eventHero,
                    CREATURE_ROYAL_MUMMY,
                    &index,
                    cell
                );
                if (combatResult != 0) {
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
            for (index = 0; index < ARMY_GROUP_SLOT_COUNT; ++index) {
                gpMonGroup->m_creatureTypes[index] =
                    gpGame->m_mines[cell->m_objectMetadata].guardianType;
                gpMonGroup->m_creatureCounts[index] = gpGame->m_mines[cell->m_objectMetadata].guardianCount
                    / EVENT_ABANDONED_MINE_ARMY_DIVISOR;
            }
            abandonedMineValue = static_cast<i32>(
                gMineCharacteristics[IDX(RES_GOLD)] * gafAITurnCostResource[IDX(RES_GOLD)]
                * *(gaiTurnValueOfMine + x + y * MAP_WIDTH)
            );
            gpPhilAI->ChooseEvaluateBattle(
                &eventHero->m_army,
                eventHero,
                gpMonGroup,
                NULL,
                0,
                0,
                abandonedMineValue,
                battleWon,
                battleResult
            );
            if (battleWon == 0)
                break;
            survivingCount = gpGame->m_mines[cell->m_objectMetadata].guardianCount;
            combatResult = gpPhilAI->CombatMonsterEvent(
                eventHero,
                gpGame->m_mines[cell->m_objectMetadata].guardianType,
                &survivingCount,
                cell
            );
            if (survivingCount > EVENT_GUARD_COUNT_MAX)
                survivingCount = EVENT_GUARD_COUNT_MAX;
            gpGame->m_mines[cell->m_objectMetadata].guardianCount =
                survivingCount;
            if (combatResult != 0) {
                eventHero->CheckLevel();
                gpGame->ConvertObject(
                    x + ABANDONED_MINE_X_MIN,
                    y + ABANDONED_MINE_Y_TOP,
                    x + ABANDONED_MINE_X_MAX,
                    y + ABANDONED_MINE_Y_TOP,
                    TILESET_OBJNDIRT,
                    MINE_TOP_A_FIRST_FRAME,
                    MINE_TOP_A_LAST_FRAME,
                    TILESET_MTNDIRT,
                    MINE_BOTTOM_A_FRAME,
                    MAP_OBJECT_ABANDONED_MINE,
                    MAP_OBJECT_MINE
                );
                gpGame->ConvertObject(
                    x + ABANDONED_MINE_X_MIN,
                    y,
                    x + ABANDONED_MINE_X_MAX,
                    y,
                    TILESET_OBJNDIRT,
                    MINE_SECOND_A_FIRST_FRAME,
                    MINE_SECOND_A_LAST_FRAME,
                    TILESET_MTNDIRT,
                    MINE_SECOND_BOTTOM_A_FRAME,
                    MAP_OBJECT_ABANDONED_MINE,
                    MAP_OBJECT_MINE
                );
                gpGame->ConvertObject(
                    x + ABANDONED_MINE_X_MIN,
                    y + ABANDONED_MINE_Y_TOP,
                    x + ABANDONED_MINE_X_MAX,
                    y + ABANDONED_MINE_Y_TOP,
                    TILESET_OBJNGRAS,
                    MINE_TOP_B_FIRST_FRAME,
                    MINE_TOP_B_LAST_FRAME,
                    TILESET_MTNGRAS,
                    MINE_BOTTOM_B_FRAME,
                    MAP_OBJECT_ABANDONED_MINE,
                    MAP_OBJECT_MINE
                );
                gpGame->ConvertObject(
                    x + ABANDONED_MINE_X_MIN,
                    y,
                    x + ABANDONED_MINE_X_MAX,
                    y,
                    TILESET_OBJNGRAS,
                    MINE_SECOND_B_FIRST_FRAME,
                    MINE_SECOND_B_LAST_FRAME,
                    TILESET_MTNGRAS,
                    MINE_SECOND_BOTTOM_B_FRAME,
                    MAP_OBJECT_ABANDONED_MINE,
                    MAP_OBJECT_MINE
                );
                gpGame->ConvertObject(
                    x,
                    y,
                    x,
                    y,
                    TILESET_EXTRAOVR,
                    MINE_CENTER_GHOST_FRAME,
                    MINE_CENTER_GHOST_FRAME,
                    TILESET_EXTRAOVR,
                    MINE_CENTER_GOLD_FRAME,
                    MAP_OBJECT_ABANDONED_MINE,
                    MAP_OBJECT_MINE
                );
                gpGame->m_mines[cell->m_objectMetadata].resourceType = MINE_TYPE_GOLD;
                gpGame->m_mines[cell->m_objectMetadata].guardianType = CREATURE_NONE;
                gpGame->m_mines[cell->m_objectMetadata].guardianCount = 0;
                gpGame->ClaimMine(cell->m_objectMetadata, giCurPlayer);
            }
            break;

        case MAP_OBJECT_TAR_PIT:
            break;

        case MAP_OBJECT_OBSERVATION_TOWER:
            gpGame->SetVisibility(x, y, giCurPlayer, EVENT_OBSERVATION_RADIUS);
            break;

        case MAP_OBJECT_SHIPWRECK_SURVIVOR:
            if (eventHero->NumArtifacts() < HERO_ARTIFACT_SLOT_COUNT)
                GiveArtifact(eventHero, ArtifactType(cell->m_objectMetadata), true);
            eraseObject = 1;
            break;

        case MAP_OBJECT_ARTESIAN_SPRING:
            if (cell->m_objectMetadata == 0)
                break;
            cell->m_objectMetadata = MAP_EVENT_DATA_EMPTY;
            springSpellPoints = HERO_NORMAL_SPELL_POINTS(*eventHero);
            if (eventHero->m_spellPoints < springSpellPoints * ARTESIAN_SPRING_MANA_MULTIPLIER)
                eventHero->m_spellPoints =
                    springSpellPoints * ARTESIAN_SPRING_MANA_MULTIPLIER;
            break;

        case MAP_OBJECT_MAGIC_WELL:
            if (HAS(eventHero->m_eventFlags, HERO_EVENT_MAGIC_WELL) == 0) {
                cell->m_objectMetadata = MAP_EVENT_DATA_EMPTY;
                wellSpellPoints = HERO_NORMAL_SPELL_POINTS(*eventHero);
                if (eventHero->m_spellPoints < wellSpellPoints) {
                    eventHero->m_eventFlags = eventHero->m_eventFlags | HERO_EVENT_MAGIC_WELL;
                    eventHero->m_spellPoints = wellSpellPoints;
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
            eventExtra = static_cast<mapEventExtra*>(ppMapExtra[cell->m_objectMetadata]);
            if (eventExtra->active == 0)
                break;
            if (Random(0, EVENT_RANDOM_PERCENT_MAX) < EVENT_RANDOM_EVENT_SUCCESS) {
                primaryReward = MAP_EVENT_REWARD_NONE;
                primaryAmount = 0;
                secondaryReward = MAP_EVENT_REWARD_NONE;
                secondaryAmount = 0;
                for (index = 0; index < IDX(RES_COUNT); ++index) {
                    gpGame->m_players[giCurPlayer].m_resources[index] +=
                        eventExtra->resources[index];
                    if (gpGame->m_players[giCurPlayer].m_resources[index] < 0)
                        gpGame->m_players[giCurPlayer].m_resources[index] = 0;
                }
                if (eventExtra->artifact != -1
                    && eventHero->NumArtifacts() < HERO_ARTIFACT_SLOT_COUNT) {
                    GiveArtifact(eventHero, ArtifactType(eventExtra->artifact), true);
                }
                eventExtra->active = 0;
            } else {
                HeroLoses(eventHero);
            }
            break;

        case MAP_OBJECT_TRADING_POST:
            break;

        case MAP_OBJECT_BARRIER:
            eraseObject = BarrierAIEvent(cell, eventHero);
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

    if (eraseObject != 0)
        EraseObj(cell, x, y);
    giCurPlayer = oldPlayer;
    gpCurPlayer = oldPlayerData;
    if (eventHero->m_owner != EVENT_NO_OWNER)
        eventHero->CheckLevel();
    CheckEndGame(END_GAME_FORCE_NONE, false);
}
#if H2_RETAIL_COMPILER
#undef abandonedMineValue
#undef adjacentMonster
#undef artifactGuardCount
#undef artifactGuardResult
#undef artifactResource
#undef attackerLoss
#undef battleResult
#undef battleWon
#undef boat
#undef combatResult
#undef creatureCosts
#undef creatureFlag
#undef creatureType
#undef defenderLoss
#undef eraseObject
#undef eventArtifact
#undef eventExtra
#undef eventResourceType
#undef eventType
#undef eventWork
#undef exitY
#undef heroCombatResult
#undef heroLevel
#undef index
#undef levelExperience
#undef occupiedTown
#undef oldPlayer
#undef oldPlayerData
#undef otherHero
#undef primaryAmount
#undef primaryReward
#undef purchaseCost
#undef purchaseCount
#undef purchaseValue
#undef pyramidBattleValue
#undef savedShowIt
#undef secondaryAmount
#undef secondaryReward
#undef spellPower
#undef springSpellPoints
#undef survivingCount
#undef unusedResult
#undef wellSpellPoints
#endif

VA(0x004478df, 0x56)
i32 advManager::BarrierAIEvent(mapCell* cell, hero*) {
    i32 color = cell->m_objectMetadata;
    color &= EVENT_BARRIER_COLOR_MASK;
    if (PLAYER_HAS_VISITED_TENT(*gpCurPlayer, color))
        return 1;
    else
        return 0;
    return 0;
}

#if H2_RETAIL_COMPILER
#define unusedPassword unusedPassword6
#endif
VA(0x00447935, 0x54)
void advManager::PasswordAIEvent(mapCell* cell, hero*) {
    i32 H2_UNUSED(unusedPassword)[1];
    i32 color = cell->m_objectMetadata;
    color &= EVENT_BARRIER_COLOR_MASK;
    gpCurPlayer->m_barrierTents |= (1 << color);
}
#if H2_RETAIL_COMPILER
#undef unusedPassword
#endif

#if H2_RETAIL_COMPILER
#define armyValue armyValue7
#define artifactIndex artifactIndex14
#define creatureType creatureType3
#define cursedArtifactCount cursedArtifactCount3
#define primaryStat primaryStat16
#define quantity quantity1
#define siteLevel siteLevel6
#define siteType siteType3
#define unusedPair unusedPair8
#define unusedTriple unusedTriple26
#endif
VA(0x00447989, 0x351)
void advManager::GenericSiteAIEvent(mapCell* cell, hero* eventHero) {
    i32 H2_UNUSED(siteLevel);
    HeroPrimaryStat primaryStat;
    i32 artifactIndex;
    GenericSiteType siteType;
    i32 H2_UNUSED(unusedPair)[2];
    i32 cursedArtifactCount;
    i32 quantity;
    i32 H2_UNUSED(unusedTriple)[3];
    CreatureType creatureType;
    i32 armyValue;

    cursedArtifactCount = 0;
    siteType = static_cast<GenericSiteType>(cell->m_objectMetadata);
    siteType = static_cast<GenericSiteType>(IDX(siteType) & GENERIC_SITE_TYPE_MASK);
    siteLevel = cell->m_objectMetadata;
    siteLevel >>= GENERIC_SITE_LEVEL_SHIFT;

    switch (siteType) {
        case GENERIC_SITE_ALCHEMIST_TOWER:
            for (artifactIndex = 0; artifactIndex < HERO_ARTIFACT_SLOT_COUNT; artifactIndex++) {
                if (IsCursedItem(eventHero->m_artifacts[artifactIndex]))
                    cursedArtifactCount++;
            }
            if (cursedArtifactCount != 0
                && gpCurPlayer->m_resources[IDX(RES_GOLD)]
                       >= EVENT_CURSED_ARTIFACT_GOLD_THRESHOLD) {
                for (artifactIndex = 0; artifactIndex < HERO_ARTIFACT_SLOT_COUNT;
                     artifactIndex++) {
                    if (IsCursedItem(eventHero->m_artifacts[artifactIndex]))
                        eventHero->m_artifacts[artifactIndex] = ARTIFACT_NONE;
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
                        primaryStat = HERO_PRIMARY_SPELL_POWER;
                        break;
                    case FACTION_BARBARIAN:
                        primaryStat = HERO_PRIMARY_ATTACK;
                        break;
                    case FACTION_KNIGHT:
                        primaryStat = HERO_PRIMARY_DEFENSE;
                        break;
                    default:
                        primaryStat = HERO_PRIMARY_ATTACK;
                }
                eventHero->m_primaryStats[IDX(primaryStat)]++;
            }
            break;
        case GENERIC_SITE_MERMAID:
            if (!(eventHero->m_eventFlags & HERO_EVENT_MERMAID)) {
                eventHero->m_eventFlags = eventHero->m_eventFlags | HERO_EVENT_MERMAID;
                eventHero->m_luck++;
            }
            break;
        case GENERIC_SITE_HUT_OF_MAGI:
        case GENERIC_SITE_EYE_OF_MAGI:
            break;
        case GENERIC_SITE_SIRENS:
            if (!(eventHero->m_eventFlags & HERO_EVENT_SIRENS)) {
                armyValue = 0;
                for (artifactIndex = 0; artifactIndex < ARMY_GROUP_SLOT_COUNT; artifactIndex++) {
                    creatureType = eventHero->m_army.m_creatureTypes[artifactIndex];
                    if (creatureType != CREATURE_NONE) {
                        quantity = eventHero->m_army.m_creatureCounts[artifactIndex];
                        if (quantity > EVENT_SIRENS_MIN_ARMY_QUANTITY) {
                            eventHero->m_army.m_creatureCounts[artifactIndex] =
                                quantity * AI_GENERIC_SITE_SIRENS_ARMY_REMAINDER;
                            armyValue +=
                                gMonsterDatabase[IDX(creatureType)].hitPoints
                                * (quantity - eventHero->m_army.m_creatureCounts[artifactIndex]);
                        }
                    }
                }
                if (armyValue != 0)
                    GiveExperience(eventHero, armyValue, 1);
                eventHero->m_eventFlags = eventHero->m_eventFlags | HERO_EVENT_SIRENS;
            }
            break;
        case GENERIC_SITE_STABLES:
            if (!(eventHero->m_eventFlags & HERO_EVENT_STABLES)) {
                eventHero->m_eventFlags = eventHero->m_eventFlags | HERO_EVENT_STABLES;
                eventHero->m_mobility += EVENT_CREATURE_UPGRADE_MOBILITY;
                eventHero->m_remainingMobility += EVENT_CREATURE_UPGRADE_MOBILITY;
            }
            if (eventHero->CreatureTypeCount(CREATURE_CAVALRY))
                eventHero->UpgradeCreatures(CREATURE_CAVALRY, CREATURE_CHAMPION);
            break;
    }
}
#if H2_RETAIL_COMPILER
#undef armyValue
#undef artifactIndex
#undef creatureType
#undef cursedArtifactCount
#undef primaryStat
#undef quantity
#undef siteLevel
#undef siteType
#undef unusedPair
#undef unusedTriple
#endif

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
    recruitType &= IDX(EVENT_RECRUIT_TYPE_MASK);
    quantity = cell->m_objectMetadata;
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

#if H2_RETAIL_COMPILER
#define heroId heroId9
#define releasedHero releasedHero13
#endif
VA(0x00447e63, 0x19c)
void advManager::JailAIEvent(mapCell* cell, hero* eventHero, i32 x, i32 y) {
    i32 heroId;
    hero* releasedHero;

    heroId = cell->m_objectMetadata;
    if (gpGame->m_availableHeroes[heroId] != HERO_AVAILABILITY_JAILED) {
        EraseObj(cell, x, y);
        return;
    }
    if (gpCurPlayer->m_heroCount >= PLAYER_HERO_CAPACITY)
        return;
    gpGame->m_heroRecs[heroId].m_owner = eventHero->m_owner;
    gpGame->m_availableHeroes[heroId] = eventHero->m_owner;
    releasedHero = &gpGame->m_heroRecs[heroId];
    EraseObj(cell, x, y);
    gpCurPlayer->m_heroIds[gpCurPlayer->m_heroCount] = heroId;
    gpCurPlayer->m_heroCount++;
    releasedHero->m_x = x;
    releasedHero->m_y = y;
    releasedHero->m_eventFlags = HERO_EVENT_NONE;
    releasedHero->m_direction = MAP_DIRECTION_EAST;
    releasedHero->m_remainingMobility = releasedHero->CalcMobility();
    releasedHero->m_mobility = releasedHero->m_remainingMobility;
    releasedHero->m_locationType = cell->m_triggerType;
    releasedHero->m_occupiedTown = cell->m_objectMetadata;
    cell->m_triggerType = MAP_ACTION_TRIGGER(MAP_OBJECT_HERO_INTERACTION);
    cell->m_objectMetadata = heroId;
    SendMapChange(
        MAP_CHANGE_RECRUIT_HERO,
        heroId,
        x,
        y,
        MAP_CHANGE_CURRENT_PLAYER,
        0,
        0
    );
}
#if H2_RETAIL_COMPILER
#undef heroId
#undef releasedHero
#endif

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
    creatureCount = cell->m_objectMetadata & IDX(MAP_MONSTER_COUNT_MASK);
    armyRatio = static_cast<double>(
                    gpPhilAI->FightValueOfStack(&eventHero->m_army, eventHero, 0)
                )
                / (creatureCount * gMonsterDatabase[IDX(monsterType)].fightValue);

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
                    localization::Tr("event.inline.bc81eedbd225f1cc"),
                    NORMAL_DIALOG_INFO
                );
            else if (monsterType == CREATURE_OGRE || monsterType == CREATURE_OGRE_LORD)
                NormalDialog(
                    localization::Tr("event.inline.cf9fdd5bf3abd972"),
                    NORMAL_DIALOG_INFO
                );
            else
                NormalDialog(
                    localization::Tr("event.inline.f68d63dd41307454"),
                    NORMAL_DIALOG_INFO
                );
            *handled = 1;
        } else {
            if (monsterType == CREATURE_DWARF || monsterType == CREATURE_BATTLE_DWARF)
                NormalDialog(
                    localization::Tr("event.inline.ca210d925c2d4171"),
                    NORMAL_DIALOG_INFO
                );
            else if (monsterType == CREATURE_OGRE || monsterType == CREATURE_OGRE_LORD)
                NormalDialog(
                    localization::Tr("event.inline.7e5ecd33e75d57cd"),
                    NORMAL_DIALOG_INFO
                );
            else
                NormalDialog(
                    localization::Tr("event.inline.f0128914e00d4e7f"),
                    NORMAL_DIALOG_INFO
                );
            eventHero->m_army.Add(monsterType, creatureCount, -1);
            *handled = 1;
        }
        return;
    }

    if (gbInCampaign && gpGame->m_campaignAwards[IDX(CAMPAIGN_AWARD_DWARFBANE)]
        && (monsterType == CREATURE_DWARF || monsterType == CREATURE_BATTLE_DWARF)) {
        NormalDialog(
            localization::Tr("event.inline.53220bf577626eb3"),
            NORMAL_DIALOG_INFO
        );
        *handled = 1;
        return;
    }

    if (xIsPlayingExpansionCampaign && xCampaign.HasAward(AWARD_ELVEN_ALLIANCE)
        && (monsterType == CREATURE_ELF || monsterType == CREATURE_GRAND_ELF)) {
        *handled = 1;
        if (eventHero->m_army.CanJoin(monsterType)) {
            NormalDialog(
                localization::Tr("event.inline.4a9a02fcd852e2b3"),
                NORMAL_DIALOG_INFO
            );
            eventHero->m_army.Add(monsterType, creatureCount, -1);
        } else {
            NormalDialog(
                localization::Tr("event.inline.b957bcfea39c68b5"),
                NORMAL_DIALOG_INFO
            );
        }
        return;
    }

    if (eventHero->m_army.CanJoin(monsterType) && armyRatio > 2.0 /* MONSTER_STRENGTH_JOIN */
        && !eventHero->HasArtifact(ARTIFACT_HIDEOUS_MASK) && monsterType != CREATURE_GHOST
        && !IS_ELEMENTAL_CREATURE(monsterType)) {
        if (forceJoin) {
            sprintf(gText, gEventText[EVENT_TEXT_FOLLOWERS], gArmyNamesPlural[IDX(monsterType)]);
            EventWindow(-1, NORMAL_DIALOG_CONFIRM, gText, -1, 0, -1, 0, -1);
            if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_YES) {
                eventHero->m_army.Add(monsterType, creatureCount, -1);
                *handled = 1;
                return;
            } else {
                EventWindow(
                    EVENT_TEXT_MONSTER_REFUSAL,
                    NORMAL_DIALOG_INFO,
                    "",
                    -1,
                    0,
                    -1,
                    0,
                    -1
                );
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
            if (joiningCost > gpGame->m_players[IDX(eventHero->m_owner)].m_resources[IDX(RES_GOLD)]) {
                if (armyRatio
                    > 5.0 /* MONSTER_STRENGTH_FLEE */)
                    goto monstersFlee;
                else
                    goto fightMonsters;
            }

            if (creatureCount == 1) {
                sprintf(
                    gText,
                    localization::Tr("event.inline.61cb3563007fe53a"),
                    gArmyNames[IDX(monsterType)],
                    joiningCost
                );
            } else {
                sprintf(
                    gText,
                    localization::Tr("event.inline.459e418255ebdf2c")
                );
                if (numJoining == creatureCount)
                    sprintf(
                        monsterText,
                        localization::Tr("event.inline.ddc9cf3652f5dd02"),
                        creatureCount,
                        gArmyNamesPlural[IDX(monsterType)],
                        joiningCost
                    );
                else
                    sprintf(
                        monsterText,
                        localization::Tr("event.inline.2f685e157f228f6f"),
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
            if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_YES) {
                eventHero->m_army.Add(monsterType, numJoining, -1);
                *handled = 1;
                gpGame->m_players[IDX(eventHero->m_owner)].m_resources[IDX(RES_GOLD)] -= joiningCost;
                return;
            } else {
                EventWindow(
                    EVENT_TEXT_MONSTER_REFUSAL,
                    NORMAL_DIALOG_INFO,
                    "",
                    -1,
                    0,
                    -1,
                    0,
                    -1
                );
                goto fightMonsters;
            }
        }
    }

    if (armyRatio > MONSTER_STRENGTH_FLEE) {
    monstersFlee:
        sprintf(
            gText,
            localization::Tr("event.inline.f87a6ac73c24c595"),
            gArmyNamesPlural[IDX(monsterType)]
        );
        EventWindow(-1, NORMAL_DIALOG_CONFIRM, gText, -1, 0, -1, 0, -1);
        if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_YES)
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
    creatureCount[MONSTER_COMBAT_REMAINING_COUNT] = cell->m_objectMetadata & IDX(MAP_MONSTER_COUNT_MASK);
    forceJoin = cell->m_objectMetadata & MONSTER_JOIN_FORCED;
    armyRatio = static_cast<double>(
                    gpPhilAI->FightValueOfStack(&eventHero->m_army, eventHero, 0)
                )
                / (creatureCount[MONSTER_COMBAT_REMAINING_COUNT]
                    * gMonsterDatabase[IDX(monsterType)].fightValue);

    if (eventHero->m_army.CanJoin(monsterType) && !eventHero->HasArtifact(ARTIFACT_HIDEOUS_MASK)
        && armyRatio > MONSTER_STRENGTH_JOIN && monsterType != CREATURE_GHOST
        && !IS_ELEMENTAL_CREATURE(monsterType)) {
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
                    * 0.75 /* MONSTER_AI_JOIN_COST_FRACTION */
                );
                if (joiningCost
                    > gpGame->m_players[IDX(eventHero->m_owner)].m_resources[IDX(RES_GOLD)]) {
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
                    gpGame->m_players[IDX(eventHero->m_owner)].m_resources[IDX(RES_GOLD)] -= joiningCost;
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
                    creatureCount[MONSTER_COMBAT_REMAINING_COUNT]
                    * 0.1 /* MONSTER_NECROMANCY_FRACTION */
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
                + (creatureCount[MONSTER_COMBAT_REMAINING_COUNT] & IDX(MAP_MONSTER_COUNT_MASK));
        }
    }
}

#if H2_RETAIL_COMPILER
#define netUnused netUnused7
#endif
VA(0x00448b04, 0x191)
i32 advManager::DoNetCombat(char* packet) {
    hero* secondHro;
    i32 randSeed;
    i32 firstSide;
    town* battleTown;
    H2_ENUM_STORAGE(CombatResult, i8) combatRes;
    i32 initCombatX;
    i32 H2_UNUSED(netUnused);
    armyGroup* secondArmy;
    i32 battleX;
    armyGroup* troopFirst;
    i32 otherPlr;
    i32 setupBattleY;
    hero* firstHero;
    i32 combatY;
    i32 H2_UNUSED(outcome);

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
#if H2_RETAIL_COMPILER
#undef netUnused
#endif

#if H2_RETAIL_COMPILER
#define combatResult combatResult3
#define firstPlayer firstPlayer4
#define message message9
#define receivedPacket packet_j
#define receivedFirstHero receivedFirstHero1
#define receivedSecondArmy receivedSecondArmy2
#define receivedSecondHero receivedSecondHero9
#define savedPlayer savedPlayer1
#define savedShowIt savedShowIt_f
#define secondPlayer secondPlayer8
#define unusedCombat unusedCombat_a
#endif
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
    armyGroup* receivedSecondArmy;
    armyGroup* receivedFirstArmy;
    hero* receivedSecondHero;
    hero* receivedFirstHero;
    town* receivedTown;
    char* receivedPacket;
    i32 remotePlayer;
    tag_message message;
    H2_ENUM_STORAGE(CombatResult, i8) combatResult;
    b32 savedShowIt;
    i32 secondPlayer;
    i32 savedPlayer;
    i32 H2_UNUSED(unusedCombat);
    i32 firstPlayer;

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
    firstPlayer = firstHero ? firstHero->m_owner : -1;
    if (secondHero)
        secondPlayer = secondHero->m_owner;
    else if (combatTown)
        secondPlayer = combatTown->m_owner;
    else
        secondPlayer = -1;
    if (randomSeed == -1)
        randomSeed = Random(1, COMBAT_RANDOM_SEED_MAX);
    DemobilizeCurrHero();
    savedPlayer = giCurPlayer;
    savedShowIt = bShowIt;

    if (firstPlayer >= 0 && secondPlayer >= 0 && gbHumanPlayer[secondPlayer]) {
        if (!gbThisNetHumanPlayer[secondPlayer]) {
            iCombatControlNetPos[IDX(COMBAT_ATTACKER_SIDE)] = giThisNetPos;
            iCombatControlNetPos[IDX(COMBAT_DEFENDER_SIDE)] = gbGamePosToNetPos[secondPlayer];
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
                gbGamePosToNetPos[secondPlayer],
                COMBAT_RESULT_ATTACKER,
                0,
                0
            );
            if (!gbHumanPlayer[firstPlayer]) {
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
                    receivedPacket = CheckHandleNet();
                    if (receivedPacket) {
                        switch (EVENTS_REMOTE_MESSAGE(receivedPacket)->command) {
                            case REMOTE_COMMAND:
                                ReceiveHeroTownData(
                                    receivedPacket,
                                    &remotePlayer,
                                    &x,
                                    &y,
                                    &receivedFirstHero,
                                    &receivedFirstArmy,
                                    &receivedTown,
                                    &receivedSecondHero,
                                    &receivedSecondArmy,
                                    &setupCombatX,
                                    &setupCombatY,
                                    &randomSeed,
                                    &combatResult,
                                    &gbRetreatWin,
                                    &gbCombatSurrender
                                );
                                if (receivedFirstArmy) {
                                    memcpy(firstArmy, receivedFirstArmy, sizeof(armyGroup));
                                    H2_FREE(receivedFirstArmy);
                                }
                                if (receivedSecondArmy) {
                                    memcpy(secondArmy, receivedSecondArmy, sizeof(armyGroup));
                                    H2_FREE(receivedSecondArmy);
                                }
                                if (receivedTown) {
                                    memcpy(combatTown, receivedTown, sizeof(town));
                                    H2_FREE(receivedTown);
                                }
                                if (receivedSecondHero) {
                                    memcpy(secondHero, receivedSecondHero, sizeof(hero));
                                    H2_FREE(receivedSecondHero);
                                }
                                if (receivedFirstHero) {
                                    memcpy(firstHero, receivedFirstHero, sizeof(hero));
                                    H2_FREE(receivedFirstHero);
                                }
                                gpCombatManager->m_combatResult = combatResult;
                                goto combatFinished;
                        }
                    }
                    Process1WindowsMessage();
                    message = gpInputManager->GetEvent();
                    CheckHandleNetPlayerWait(message, 1);
                }
            }
        } else if (!gbThisNetHumanPlayer[firstPlayer]) {
            bShowIt = true;
            gpGame->TurnOffAIMusic();
            sprintf(
                gText,
                localization::Tr("network.attack.owner.buka")
                    ,
                cPlayerNames[secondPlayer],
                combatTown
                    ? localization::Tr("scenario.fragment.town")
                    : localization::Tr("scenario.fragment.hero")
            );
            gpGame->WaitForPlayer(gText, secondPlayer);
        }
    }

    bShowIt = true;
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
    gpMouseManager->SetPointer(
        "advmice.mse",
        0,
        MOUSE_AUTO_CURSOR_TYPE
    );
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
    bShowIt = savedShowIt;
    giCurPlayer = savedPlayer;
    if (!gbHumanPlayer[giCurPlayer]) {
        gpGame->ShowComputerScreen();
        gpGame->TurnOnAIMusic();
        SetNoDialogMenus(false);
    } else {
        SetNoDialogMenus(true);
    }
    MobilizeCurrHero(0);
    if (processLosses)
        gbRetreatWin = false;
    gbInCombat = false;
    while (gpMouseManager->m_hideCount)
        gpMouseManager->ShowColorPointer();
    return gpCombatManager->m_combatResult;
}
#if H2_RETAIL_COMPILER
#undef combatResult
#undef firstPlayer
#undef message
#undef receivedPacket
#undef receivedFirstHero
#undef receivedSecondArmy
#undef receivedSecondHero
#undef savedPlayer
#undef savedShowIt
#undef secondPlayer
#undef unusedCombat
#endif

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
    buffer->x = x;
    buffer->y = y;
    buffer->hasFirstHero = firstHero != NULL;
    buffer->hasTown = combatTown != NULL;
    buffer->hasSecondHero = secondHero != NULL;
    buffer->setupCombatX = setupCombatX;
    buffer->setupCombatY = setupCombatY;
    buffer->randomSeed = randomSeed;
    buffer->combatResult = combatResult;
    buffer->retreatWin = retreatWin;
    buffer->combatSurrender = combatSurrender;
    buffer->firstOwner = firstHero ? firstHero->m_owner : -1;
    buffer->firstGold =
        firstHero ? gpGame->m_players[IDX(firstHero->m_owner)].m_resources[IDX(RES_GOLD)] : 0;
    buffer->secondOwner = secondHero ? secondHero->m_owner : -1;
    buffer->secondGold =
        secondHero ? gpGame->m_players[IDX(secondHero->m_owner)].m_resources[IDX(RES_GOLD)] : 0;
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
            COMBAT_REMOTE_FRAGMENT_TYPE
        );
        if (!result)
            ShutDown(NULL);
        EVENTS_HERO_BUFFER(buffer)->fragment = REMOTE_FIRST_HERO_SECOND;
        memcpy(
            EVENTS_HERO_BUFFER(buffer)->data,
            reinterpret_cast<char*>(firstHero) + COMBAT_REMOTE_HERO_FIRST_SIZE,
            COMBAT_REMOTE_HERO_SECOND_SIZE
        );
        result = TransmitRemoteData(
            reinterpret_cast<char*>(buffer),
            remotePlayer,
            COMBAT_REMOTE_HERO_SECOND_SIZE + 1,
            REMOTE_COMMAND,
            COMBAT_REMOTE_FRAGMENT_TYPE
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
            COMBAT_REMOTE_FRAGMENT_TYPE
        );
        if (!result)
            ShutDown(NULL);
        EVENTS_HERO_BUFFER(buffer)->fragment = REMOTE_SECOND_HERO_SECOND;
        memcpy(
            EVENTS_HERO_BUFFER(buffer)->data,
            reinterpret_cast<char*>(secondHero) + COMBAT_REMOTE_HERO_FIRST_SIZE,
            COMBAT_REMOTE_HERO_SECOND_SIZE
        );
        result = TransmitRemoteData(
            reinterpret_cast<char*>(buffer),
            remotePlayer,
            COMBAT_REMOTE_HERO_SECOND_SIZE + 1,
            REMOTE_COMMAND,
            COMBAT_REMOTE_FRAGMENT_TYPE
        );
        if (!result)
            ShutDown(NULL);
    }
    H2_FREE(buffer);
}

#if H2_RETAIL_COMPILER
#define firstHeroOwner firstOwner8
#define firstHeroPresent hasFirstHero4
#define gotFirstHeroFirst gotFirstHeroFirst0
#define gotSecondHeroFirst gotSecondHeroFirst2
#define gotSecondHeroSecond gotSecondHeroSecond2
#define lastPacketTime lastPacketTime7
#define result result7
#define secondHeroPresent hasSecondHero0
#define townPresent hasTown2
#endif
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
    b32 firstHeroPresent;
    b32 gotSecondHeroFirst;
    b32 gotFirstHeroSecond;
    i32 firstHeroOwner;
    b32 gotSecondHeroSecond;
    i32 result;
    b32 townPresent;
    b32 secondHeroPresent;
    b32 gotFirstHeroFirst;
    i32l lastPacketTime;
    i32 secondOwner;

    *firstHero = NULL;
    *firstArmy = NULL;
    *combatTown = NULL;
    *secondHero = NULL;
    *secondArmy = NULL;
    firstHeroPresent = secondHeroPresent = townPresent = false;
    *remotePlayer = EVENTS_REMOTE_MESSAGE(packet)->sender;
    *x = EVENTS_REMOTE_COMBAT(packet)->x;
    *y = EVENTS_REMOTE_COMBAT(packet)->y;
    firstHeroPresent = EVENTS_REMOTE_COMBAT(packet)->hasFirstHero;
    townPresent = EVENTS_REMOTE_COMBAT(packet)->hasTown;
    secondHeroPresent = EVENTS_REMOTE_COMBAT(packet)->hasSecondHero;
    *setupCombatX = EVENTS_REMOTE_COMBAT(packet)->setupCombatX;
    *setupCombatY = EVENTS_REMOTE_COMBAT(packet)->setupCombatY;
    *randomSeed = EVENTS_REMOTE_COMBAT(packet)->randomSeed;
    *combatResult = EVENTS_REMOTE_COMBAT(packet)->combatResult;
    *retreatWin = EVENTS_REMOTE_COMBAT(packet)->retreatWin;
    *combatSurrender = EVENTS_REMOTE_COMBAT(packet)->combatSurrender;
    firstHeroOwner = EVENTS_REMOTE_COMBAT(packet)->firstOwner;
    if (firstHeroOwner > 0)
        gpGame->m_players[firstHeroOwner].m_resources[IDX(RES_GOLD)] =
            EVENTS_REMOTE_COMBAT(packet)->firstGold;
    secondOwner = EVENTS_REMOTE_COMBAT(packet)->secondOwner;
    if (secondOwner > 0)
        gpGame->m_players[secondOwner].m_resources[IDX(RES_GOLD)] =
            EVENTS_REMOTE_COMBAT(packet)->secondGold;

    *firstArmy = static_cast<armyGroup*>(H2_ALLOC(sizeof(armyGroup)));
    memcpy(*firstArmy, &EVENTS_REMOTE_COMBAT(packet)->firstArmy, sizeof(armyGroup));
    *secondArmy = static_cast<armyGroup*>(H2_ALLOC(sizeof(armyGroup)));
    memcpy(*secondArmy, &EVENTS_REMOTE_COMBAT(packet)->secondArmy, sizeof(armyGroup));
    if (townPresent) {
        *combatTown = static_cast<town*>(H2_ALLOC(sizeof(town)));
        memcpy(*combatTown, &EVENTS_REMOTE_COMBAT(packet)->combatTown, sizeof(town));
    }

    iCombatControlNetPos[IDX(COMBAT_ATTACKER_SIDE)] = *remotePlayer;
    iCombatControlNetPos[IDX(COMBAT_DEFENDER_SIDE)] = giThisNetPos;
    result = TransmitRemoteData(
        NULL,
        *remotePlayer,
        0,
        REMOTE_CONFIRM_COMMAND,
        COMBAT_REMOTE_FRAGMENT_TYPE
    );
    if (!result)
        ShutDown(NULL);

    lastPacketTime = KBTickCount();
    gotFirstHeroFirst = true;
    gotFirstHeroSecond = true;
    gotSecondHeroFirst = true;
    gotSecondHeroSecond = true;
    if (firstHeroPresent) {
        *firstHero = static_cast<hero*>(H2_ALLOC(sizeof(hero)));
        gotFirstHeroFirst = false;
        gotFirstHeroSecond = false;
    }
    if (secondHeroPresent) {
        *secondHero = static_cast<hero*>(H2_ALLOC(sizeof(hero)));
        gotSecondHeroFirst = false;
        gotSecondHeroSecond = false;
    }

    while (!gotFirstHeroFirst || !gotFirstHeroSecond || !gotSecondHeroFirst
           || !gotSecondHeroSecond) {
        PollSound();
        if (lastPacketTime + COMBAT_REMOTE_TIMEOUT < KBTickCount()) {
            NormalDialog(
                localization::Tr("event.inline.2b436715930a57ee"),
                NORMAL_DIALOG_CONFIRM
            );
            if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_YES)
                lastPacketTime = KBTickCount();
            else
                ShutDown(localization::Tr("event.inline.d7ee33967c36bb8b"));
        }
        packet = GetRemoteData(1);
        if (packet && EVENTS_REMOTE_MESSAGE(packet)->type == REMOTE_MESSAGE_RELIABLE
            && EVENTS_REMOTE_MESSAGE(packet)->command == REMOTE_COMMAND) {
            lastPacketTime = KBTickCount();
            if (EVENTS_REMOTE_HERO(packet)->fragment == REMOTE_FIRST_HERO_FIRST) {
                memcpy(*firstHero, EVENTS_REMOTE_HERO(packet)->data, COMBAT_REMOTE_HERO_FIRST_SIZE);
                gotFirstHeroFirst = true;
            }
            if (EVENTS_REMOTE_HERO(packet)->fragment == REMOTE_FIRST_HERO_SECOND) {
                memcpy(
                    reinterpret_cast<char*>((*firstHero)) + COMBAT_REMOTE_HERO_FIRST_SIZE,
                    EVENTS_REMOTE_HERO(packet)->data,
                    COMBAT_REMOTE_HERO_SECOND_SIZE
                );
                gotFirstHeroSecond = true;
            }
            if (EVENTS_REMOTE_HERO(packet)->fragment == REMOTE_SECOND_HERO_FIRST) {
                memcpy(
                    *secondHero,
                    EVENTS_REMOTE_HERO(packet)->data,
                    COMBAT_REMOTE_HERO_FIRST_SIZE
                );
                gotSecondHeroFirst = true;
            }
            if (EVENTS_REMOTE_HERO(packet)->fragment == REMOTE_SECOND_HERO_SECOND) {
                memcpy(
                    reinterpret_cast<char*>((*secondHero)) + COMBAT_REMOTE_HERO_FIRST_SIZE,
                    EVENTS_REMOTE_HERO(packet)->data,
                    COMBAT_REMOTE_HERO_SECOND_SIZE
                );
                gotSecondHeroSecond = true;
            }
        }
    }
}
#if H2_RETAIL_COMPILER
#undef firstHeroOwner
#undef firstHeroPresent
#undef gotFirstHeroFirst
#undef gotSecondHeroFirst
#undef gotSecondHeroSecond
#undef lastPacketTime
#undef result
#undef secondHeroPresent
#undef townPresent
#endif

VA(0x00449960, 0x1fc)
CombatResult advManager::AutoResolveCombat(
    i32 x,
    i32 y,
    hero* firstHero,
    armyGroup* firstArmy,
    town* combatTown,
    hero* secondHero,
    armyGroup* secondArmy,
    i32 H2_UNUSED(setupCombatX),
    i32 H2_UNUSED(setupCombatY),
    i32 randomSeed,
    i32 processLosses
) {
    tag_message message;
    b32 savedShowIt;

    gbNoShowCombat = true;
    savedShowIt = bShowIt;
    bShowIt = false;
    gpMouseManager->SetPointer(0);
    gpMouseManager->m_forcePointerUpdate = true;
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
    gpCombatManager->m_gridSelectionDisabled = true;
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
        SetNoDialogMenus(false);
    } else {
        SetNoDialogMenus(true);
    }
    MobilizeCurrHero(0);
    if (processLosses)
        gbRetreatWin = false;
    gbInCombat = false;
    gpMouseManager->m_forcePointerUpdate = false;
    return gpCombatManager->m_combatResult;
}

VA(0x00449b5c, 0x8f)
i32 RiddleStringsEqual(H2_CONST char* answer, H2_CONST char* expected) {
    i32 index;
    char expectedPrefix[RIDDLE_EXPECTED_BUFFER_SIZE];
    char answerPrefix[RIDDLE_PREFIX_LENGTH + 1];

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

DATA(0x005244b4) b32 gbNoShowCombat = false;
