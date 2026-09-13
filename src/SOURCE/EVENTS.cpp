#include <Ints.h>
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

    typedef enum CombatRemoteCommand {
        REMOTE_COMMAND = 0x15,
        REMOTE_CONFIRM_COMMAND = 0x16
    } CombatRemoteCommand;

    typedef enum CombatRemoteFragment {
        REMOTE_FIRST_HERO_FIRST = 1,
        REMOTE_FIRST_HERO_SECOND = 2,
        REMOTE_SECOND_HERO_FIRST = 3,
        REMOTE_SECOND_HERO_SECOND = 4
    } CombatRemoteFragment;

    typedef enum UndeadEventConstant {
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
    } UndeadEventConstant;

    typedef enum AIEventConstant {
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
    } AIEventConstant;

    typedef enum MonsterCombatValueIndex {
        MONSTER_COMBAT_REMAINING_COUNT = 0,
        MONSTER_COMBAT_RESULT = 1,
        MONSTER_COMBAT_VALUE_COUNT = 2
    } MonsterCombatValueIndex;

    typedef enum CombatFlowConstant {
        COMBAT_AUTO_RESOLVE_DEBUG_LEVEL = 4,
        COMBAT_RANDOM_SEED_MAX = 1000,
        COMBAT_NETWORK_POLL_X = 30,
        COMBAT_NETWORK_POLL_Y = 30,
        COMBAT_NETWORK_POLL_WIDTH = 4,
        COMBAT_NETWORK_POLL_HEIGHT = 4,
        COMBAT_LOW_MEMORY_LIMIT = 900,
        COMBAT_HIGH_MEMORY_LIMIT = 2900
    } CombatFlowConstant;

    typedef enum RiddleComparisonConstant {
        RIDDLE_PREFIX_LENGTH = 4,
        RIDDLE_EXPECTED_BUFFER_SIZE = RIDDLE_PREFIX_LENGTH + 1
    } RiddleComparisonConstant;

    typedef enum BarrierEventConstant {
        PASSWORD_SHIFT = 3,
        INPUT_LENGTH = 14
    } BarrierEventConstant;

    typedef enum GenericSiteConstant {
        SITE_ALCHEMIST_COST = 750,
        SITE_VISIBILITY_RADIUS = 10,
        SITE_HUT_COLOR = 3,
        SITE_STABLE_MOBILITY = 400,
        SITE_STABLE_REWARD_VALUE = 8,
        SITE_MERMAID_WINDOW_ICON = 10,
        SITE_STRING_LIMIT = 10
    } GenericSiteConstant;

    enum {
        STABLE_VISIT_NONE = 0,
        STABLE_VISIT_MOBILITY = 1,
        STABLE_VISIT_UPGRADE = 2,
        STABLE_VISIT_MOBILITY_UPGRADE = STABLE_VISIT_MOBILITY | STABLE_VISIT_UPGRADE
    };
typedef i32 StableVisitResult;

    enum {
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
    };
typedef i32 HouseRecruitmentSite;
    typedef enum HouseEventConstant {
        EVENT_HOUSE_DIALOG_STRIDE = 3,
        EVENT_RECRUIT_DIALOG_BASE = 23,
        EVENT_ARMY_FULL_DIALOG_BASE = 24,
        EVENT_EMPTY_DIALOG_BASE = 25
    } HouseEventConstant;

    typedef enum EventSoundConstant {
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
    } EventSoundConstant;

    enum {
        SOUND_VARIANT_0 = 0,
        SOUND_VARIANT_1 = 1,
        SOUND_VARIANT_2 = 2,
        SOUND_VARIANT_3 = 3,
        SOUND_VARIANT_4 = 4,
        SOUND_VARIANT_5 = 5,
        SOUND_VARIANT_6 = 6
    };
typedef i32 EventSoundVariant;
    typedef enum EraseObjectConstant {
        NEIGHBOR_COUNT = 4,
        CELL_COUNT = 5,
        ERASE_COORDINATE_COUNT = 2,
        NO_FRAME = -1,
        EMPTY_INDEX = 0xFF,
        MAP_CHANGE_VALUE = -999,
        ENVIRONMENT_BORDER = 7
    } EraseObjectConstant;

    typedef enum CombatMonsterEventConstant {
        MONSTER_RANDOM_MAX = 100,
        MONSTER_REDUCED_STACK_CHANCE = 25,
        MONSTER_REDUCED_STACK_COUNT = 3,
        MONSTER_FOUR_STACK_THRESHOLD = 50,
        MONSTER_FOUR_STACK_COUNT = 4,
        MONSTER_UPGRADE_CHANCE = 50
    } CombatMonsterEventConstant;

    typedef enum ArtifactPickupSoundRange {
        ARTIFACT_PICKUP_SOUND_FIRST = 1,
        ARTIFACT_PICKUP_SOUND_LAST = 7
    } ArtifactPickupSoundRange;

    typedef enum FlotsamConstant {
        FLOTSAM_WOOD_AMOUNT = 5,
        FLOTSAM_GOLD_AMOUNT = 200,
        FLOTSAM_LARGE_WOOD_AMOUNT = 10,
        FLOTSAM_LARGE_GOLD_AMOUNT = 500
    } FlotsamConstant;

    typedef enum DoEventConstant {
        ARTESIAN_SPRING_MANA_MULTIPLIER = 2,
        XANADU_DIPLOMACY_MULTIPLIER = 2,
        SIGN_MINIMUM_TEXT_LENGTH = 1,
        SIGN_RANDOM_TEXT_COUNT = 4,
        CHEST_EXPERIENCE_LEVEL_OFFSET = 1,
        PRIMARY_BONUS_ATTACK = NORMAL_DIALOG_PRIMARY_BONUS_OFFSET + (HERO_PRIMARY_ATTACK),
        PRIMARY_BONUS_DEFENSE = NORMAL_DIALOG_PRIMARY_BONUS_OFFSET + (HERO_PRIMARY_DEFENSE),
        PRIMARY_BONUS_SPELL_POWER =
            NORMAL_DIALOG_PRIMARY_BONUS_OFFSET + (HERO_PRIMARY_SPELL_POWER),
        PRIMARY_BONUS_KNOWLEDGE = NORMAL_DIALOG_PRIMARY_BONUS_OFFSET + (HERO_PRIMARY_KNOWLEDGE)
    } DoEventConstant;

    typedef enum GuardedDwellingConstant {
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
    } GuardedDwellingConstant;

    typedef enum AbandonedMineConversionConstant {
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
    } AbandonedMineConversionConstant;

}

#define TRADING_POST_EFFICIENCY 0.2f

#define EVENTS_REMOTE_MESSAGE(buffer) (reinterpret_cast<RemoteMessage*>(buffer))
#define EVENTS_REMOTE_COMBAT(buffer)                                                               \
    (reinterpret_cast<combatRemoteData*>(EVENTS_REMOTE_MESSAGE(buffer)->payload))
#define EVENTS_REMOTE_HERO(buffer)                                                                 \
    (reinterpret_cast<combatRemoteHeroFragment*>(EVENTS_REMOTE_MESSAGE(buffer)->payload))
#define EVENTS_HERO_BUFFER(buffer) (reinterpret_cast<combatRemoteHeroFragment*>(buffer))

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
    tag_message unusedEventMessage [[maybe_unused]];
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
                    "{Волшебный сад}\n\nВы забрели в волшебный сад, из тех, где так обожают резвиться лепреконы с феями, но сегодня здесь никого. Может, на следующей неделе вам повезет больше.",
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
                    "{Волшебный сад}\n\nВы поймали лепрекона, так беспечно заснувшего под шляпкой волшебного гриба. В обмен на свободу он рассказал вам, где лежит горшочек со всякими ценными вещицами.",
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
                    "{Сфинкс}\n\nВы подошли к огромному Сфинксу, но он даже не шелохнулся.",
                    NORMAL_DIALOG_INFO
                );
                break;
            } else {
                sprintf(
                    gText,
                    "\"Есть у меня загадка для тебя,\"сказал Сфинкс. \"Ответишь верно - получишь награду. Ошибешься - я сожру тебя. Принимаешь ли ты мой вызов?\""
                );
                NormalDialog(gText, NORMAL_DIALOG_CONFIRM);
                if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_YES) {
                    sprintf(
                        gText,
                        "Сфинкс загадал вам следующую загадку::\n\n'%s'\n\nВаш ответ?",
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
                            "Несколько разочарованно Сфинкс промолвил. Ты дал верный ответ, вот твоя награда. А теперь убирайся.",
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
                            "\"Твоя догадка ошибочна,\" сказал, улыбаясь, Сфинкс. Ухмыляющийся Сфинкс повалил тебя на землю и мир окутала непроглядная тьма.",
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
                "{Обзорная башня}\n\nС вершины обзорной башни вы смогли разглядеть дальние земли.",
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
                        "{Форт на холме}\n\nДля всех %s, %s и %s вашего войска проходят обучение у мастеров боя из этого форта. Теперь в вашей армии имеются отряды %s, %s, и %s",
                        gArmyNamesPlural[(firstUpgrade)],
                        gArmyNamesPlural[(secondUpgrade)],
                        gArmyNamesPlural[(thirdUpgrade)],
                        gArmyNamesPlural[(firstUpgrade) + 1],
                        gArmyNamesPlural[(secondUpgrade) + 1],
                        gArmyNamesPlural[(thirdUpgrade) + 1]
                    );
                } else if (secondUpgrade != CREATURE_NONE) {
                    sprintf(
                        gText,
                        "{Форт на холме}\n\nВсе отряды %s и %s вашего войска прошли обучение у наставников этого форта. Теперь в вашей армии появились отряды %s и %s.",
                        gArmyNamesPlural[(firstUpgrade)],
                        gArmyNamesPlural[(secondUpgrade)],
                        gArmyNamesPlural[(firstUpgrade) + 1],
                        gArmyNamesPlural[(secondUpgrade) + 1]
                    );
                } else {
                    sprintf(
                        gText,
                        "{Форт на холме}\n\nВсе отряды %s вашего войска проходят обучение у боевых наставников этого форта. Теперь в вашей армии есть отряд %s.",
                        gArmyNamesPlural[(firstUpgrade)],
                        gArmyNamesPlural[(firstUpgrade) + 1]
                    );
                }
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    gText,
                    NORMAL_DIALOG_MONSTER,
                    (firstUpgrade) + 1,
                    secondUpgrade == CREATURE_NONE ? -1 : NORMAL_DIALOG_MONSTER,
                    (secondUpgrade) + 1,
                    -1
                );
            } else {
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    "{Форт на холме}\n\nНеобычный союз огров и гномов предлагает вам потренировать (улучшить) любые подобные им войска. К сожалению, у вас таких при себе нет.",
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
                        "{Вольная литейная}\n\nВсе ваши отряды %s, %s и %s теперь стали отрядами %s, %s, и %s",
                        gArmyNamesPlural[(firstUpgrade)],
                        gArmyNamesPlural[(secondUpgrade)],
                        gArmyNamesPlural[(thirdUpgrade)],
                        gArmyNamesPlural[(firstUpgrade) + 1],
                        gArmyNamesPlural[(secondUpgrade) + 1],
                        gArmyNamesPlural[(thirdUpgrade) + 1]
                    );
                } else if (secondUpgrade != CREATURE_NONE) {
                    sprintf(
                        gText,
                        "{Вольная литейная}\n\nВсе ваши отряды %s и %s теперь стали отрядами %s и %s",
                        gArmyNamesPlural[(firstUpgrade)],
                        gArmyNamesPlural[(secondUpgrade)],
                        gArmyNamesPlural[(firstUpgrade) + 1],
                        gArmyNamesPlural[(secondUpgrade) + 1]
                    );
                } else {
                    sprintf(
                        gText,
                        "{Вольная литейная}\n\nВсе ваши отряды %s теперь стали отрядами %s.",
                        gArmyNamesPlural[(firstUpgrade)],
                        gArmyNamesPlural[(firstUpgrade) + 1]
                    );
                }
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    gText,
                    NORMAL_DIALOG_MONSTER,
                    (firstUpgrade) + 1,
                    secondUpgrade == CREATURE_NONE ? -1 : NORMAL_DIALOG_MONSTER,
                    (secondUpgrade) + 1,
                    -1
                );
            } else {
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    "{Вольная литейная}\n\nКузнец при этой литейной предлагает заменить оружие копейщиков и мечников с железного на стальное. Он также сказал, что владеет технологией повышения железных големов до стальных. К сожалению, никого из них нет в вашей армии, и он не могут помочь вам.",
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
                    "{Сундук}\n\nПроведя часы, пытаясь выловить сундук из воды, вы наконец открыли его. Внутри были %s и 1000 золотых.",
                    gArtifactNames[cell->m_objectMetadata & CHEST_ARTIFACT_MASK]
                );
                NormalDialog(
                    gText,
                    NORMAL_DIALOG_INFO,
                    -1,
                    -1,
                    MAP_EVENT_REWARD_ARTIFACT,
                    cell->m_objectMetadata & CHEST_ARTIFACT_MASK,
                    (RES_GOLD),
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
                    "{Сундук}\n\nПроведя часы, пытаясь выловить сундук из воды, вы наконец открыли его и нашли внутри 1500 золотых.",
                    NORMAL_DIALOG_INFO,
                    -1,
                    -1,
                    (RES_GOLD),
                    SEA_CHEST_GOLD,
                    -1,
                    0,
                    -1,
                    0
                );
                GiveResource(eventHero, RES_GOLD, SEA_CHEST_GOLD);
            } else {
                NormalDialog(
                    "{Сундук}\n\nПотратив часы на то, чтобы выловить сундук из моря, вы наконец открыли его лишь за тем, чтобы увидеть: внутри пусто.",
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
                        "{Обломки}\n\nВы исследовали плавающие обломки, но ничего не нашли.",
                        NORMAL_DIALOG_INFO
                    );
                    break;
                case FLOTSAM_WOOD:
                    NormalDialog(
                        "{Обломки}\n\nВы исследовали плавающие обломки и добыли немного древесины.",
                        NORMAL_DIALOG_INFO,
                        -1,
                        -1,
                        (RES_WOOD),
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
                        "{Обломки}\n\nВы исследовали плавающие обломки и нашли немного золота и древесины.",
                        NORMAL_DIALOG_INFO,
                        -1,
                        -1,
                        (RES_WOOD),
                        FLOTSAM_WOOD_AMOUNT,
                        (RES_GOLD),
                        FLOTSAM_GOLD_AMOUNT,
                        -1,
                        0
                    );
                    GiveResource(eventHero, RES_WOOD, FLOTSAM_WOOD_AMOUNT);
                    GiveResource(eventHero, RES_GOLD, FLOTSAM_GOLD_AMOUNT);
                    break;
                case FLOTSAM_LARGE_TREASURE:
                    NormalDialog(
                        "{Обломки}\n\nВы исследовали плавающие обломки и нашли немного древесины и золота.",
                        NORMAL_DIALOG_INFO,
                        -1,
                        -1,
                        (RES_WOOD),
                        FLOTSAM_LARGE_WOOD_AMOUNT,
                        (RES_GOLD),
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
                    "{Потерпевший кораблекрушение}\n\nВы спасли жертву кораблекрушения от неминуемой смерти в безжалостном океане. Награда за вашу доброту - %s.",
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
                    "{Потерпевший кораблекрушение}\n\nВы спасли жертву кораблекрушения от неминуемой смерти в безжалостном океане. Исполненный благодарности, бедняга сказал: \"Я бы наградил вас артефактом, но у вас нет для него места.\"",
                    NORMAL_DIALOG_INFO
                );
            }
            fizzleType = true;
            eraseObject = 1;
            break;

        case MAP_OBJECT_MAGELLAN_MAPS:
            if (gpCurPlayer->m_resources[(RES_GOLD)] < MAGELLAN_MAP_COST) {
                NormalDialog(
                    "{Карты Магеллана}\n\nКапитан вздыхает. \"Что, деньжат не хватает, да? Ты же не думаешь, будто я отдам тебе свои карты бесплатно!\"",
                    NORMAL_DIALOG_INFO
                );
            } else {
                EventSound(eventType, cell->m_objectMetadata, &eventSample);
                NormalDialog(
                    "{Карты Магеллана}\n\nБывший капитан, живущий на этом подновленном рыболовном причале, предлагает вам карты, составленные в прежние дни, за 1000 золотых. Желаете их купить?",
                    NORMAL_DIALOG_CONFIRM
                );
                if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_YES) {
                    gpCurPlayer->m_resources[(RES_GOLD)] -= MAGELLAN_MAP_COST;
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
                    "{Хижина ведьмы}\n\nПодойдя к хижине и заглянув в окно, вы увидели ведьму, склонившуюся над древней книгой под названием %s. Когда вы вошли, она обернулась и молвила: \"Вы уже знаете то, чему я могла бы научить. Больше ничем помочь не смогу.\"",
                    gSecondarySkills[cell->m_objectMetadata]
                );
                NormalDialog(gText, NORMAL_DIALOG_INFO);
            } else if (eventHero->m_secondarySkillCount >= HERO_SECONDARY_SKILL_CAPACITY) {
                sprintf(
                    gText,
                    "{Хижина ведьмы}\n\nПодойдя к хижине и заглянув в окно, вы увидели ведьму, склонившуюся над древней книгой под названием %s. Когда вы вошли, она обернулась и наставила на вас свой стеклянный глаз. \"Ты уже и так знаешь все, чего заслуживаешь!\" - завопила ведьма. \"- А теперь убирайся из моего дома!\"",
                    gSecondarySkills[cell->m_objectMetadata]
                );
                NormalDialog(gText, NORMAL_DIALOG_INFO);
            } else {
                EventSound(eventType, cell->m_objectMetadata, &eventSample);
                sprintf(
                    gText,
                    "{Хижина ведьмы}\n\nДряхлая, но бессмертная ведьма, живущая в этой хижине на курьих ножках, по каким-то непонятным соображениям решила, что %s вам пригодится.",
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
                    "{Артезианский источник}\n\nЭтот родник восполняется раз в неделю, а кто-то уже прикладывался к нему на этой неделе.",
                    NORMAL_DIALOG_INFO
                );
            } else {
                EventSound(eventType, cell->m_objectMetadata, &eventSample);
                cell->m_objectMetadata = 0;
                springSpellPoints = HERO_NORMAL_SPELL_POINTS(*eventHero);
                if (eventHero->m_spellPoints
                    >= springSpellPoints * ARTESIAN_SPRING_MANA_MULTIPLIER) {
                    NormalDialog(
                        "{Артезианский источник}\n\nГлоток из родника обычно вдвое повышает вашу магическую энергию, но вы уже прибываете в подобном состоянии.",
                        NORMAL_DIALOG_INFO
                    );
                } else {
                    NormalDialog(
                        "{Артезианский источник}\n\nГлоток из родника наполняет вашу кровь магией! Теперь у вас в запасе вдвое больше обычного магической энергии.",
                        NORMAL_DIALOG_INFO
                    );
                    eventHero->m_spellPoints = springSpellPoints * ARTESIAN_SPRING_MANA_MULTIPLIER;
                }
            }
            break;

        case MAP_OBJECT_MAGIC_WELL:
            if ((((eventHero->m_eventFlags) & (HERO_EVENT_MAGIC_WELL)))) {
                NormalDialog(
                    "{Волшебный колодец}\n\nВолшебный колодец}\n\nВторой глоток из колодца за день вам не поможет.",
                    NORMAL_DIALOG_INFO
                );
            } else {
                EventSound(eventType, cell->m_objectMetadata, &eventSample);
                cell->m_objectMetadata = 0;
                wellSpellPoints = HERO_NORMAL_SPELL_POINTS(*eventHero);
                if (eventHero->m_spellPoints >= wellSpellPoints) {
                    NormalDialog(
                        "{Волшебный колодец}\n\nГлоток из колодца обычно восстанавливает магическую энергию, но сейчас она у вас и так на пределе.",
                        NORMAL_DIALOG_INFO
                    );
                } else {
                    NormalDialog(
                        "{Волшебный колодец}\n\nГлоток из колодца полностью восстановил вашу магическую энергию.",
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
                (gpGame->m_mines[cell->m_objectMetadata].resourceType)
                    + MINE_RESOURCE_ICON_OFFSET,
                NORMAL_DIALOG_INFO,
                "",
                (gpGame->m_mines[cell->m_objectMetadata].resourceType),
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
                (RES_MERCURY),
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
                (RES_WOOD),
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
                    "{Ларец}\n\nИсследуя окрестности, вы наткнулись на зарытый ларец, а в нем - древний артефакт '%s'",
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
                    "{Ларец}\n\nИсследуя окрестности, вы наткнулись на древний ларец. Золото можно оставить себе или раздать крестьянам в обмен на опыт. Оставите себе золото?",
                    (RES_GOLD),
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
            if ((((eventHero->m_eventFlags) & (HERO_EVENT_BUOY)))) {
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
            if ((((eventHero->m_eventFlags) & (HERO_EVENT_FAERIE_RING)))) {
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
            if ((((eventHero->m_eventFlags) & (HERO_EVENT_IDOL)))) {
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    "{Идол}\n\nВы нашли древнего идола. Говорят, он приносит удачу тем, кто его навещает, но поскольку звезды и так покровительствуют вам, идол ничего нового вам не дал.",
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
                    "{Идол}\n\nВы нашли древнего идола. Говорят, если его поцеловать, это принесет удачу - вы так и поступили. Камень оказался очень холоден для губ.",
                    NORMAL_DIALOG_LUCK_BONUS,
                    0,
                    -1,
                    0,
                    -1
                );
            }
            break;

        case MAP_OBJECT_FOUNTAIN:
            if ((((eventHero->m_eventFlags) & (HERO_EVENT_FOUNTAIN)))) {
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
            if ((((eventHero->m_eventFlags) & (HERO_EVENT_WATERING_HOLE)))) {
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    "{Промоина}\n\nВыпивка освежает, но иных благ не приносит. Источник снова поможет вам после следующей битвы.",
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
                    "{Промоина}\n\nДобрый глоток исполнил ваши войска силы и поднял дух. Сегодня вы сможете пройти чуть дальше.",
                    NORMAL_DIALOG_MORALE_BONUS,
                    0,
                    -1,
                    0,
                    -1
                );
            }
            break;

        case MAP_OBJECT_OASIS:
            if ((((eventHero->m_eventFlags) & (HERO_EVENT_OASIS)))) {
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    "{Оазис}\n\nГлоток влаги в оазисе освежает, но иной пользы не приносит. Этот оазис, возможно, пригодится вам после следующей битвы.",
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
                    "{Оазис}\n\nГлоток воды в оазисе наполняет ваших воинов силой и поднимает дух. Сегодня вы сможете пройти чуть больше.",
                    NORMAL_DIALOG_MORALE_BONUS,
                    0,
                    -1,
                    0,
                    -1
                );
            }
            break;

        case MAP_OBJECT_TEMPLE:
            if ((((eventHero->m_eventFlags) & (HERO_EVENT_TEMPLE)))) {
                NormalDialog(
                    "{Храм}\n\nДвойная молитва войне не подспорье. Заходите после битвы.",
                    NORMAL_DIALOG_INFO
                );
            } else {
                EventSound(eventType, cell->m_objectMetadata, &eventSample);
                eventHero->m_eventFlags = eventHero->m_eventFlags | HERO_EVENT_TEMPLE;
                eventHero->m_morale += TEMPLE_MORALE_BONUS;
                NormalDialog(
                    "{Храм}\n\nПосещение храма и молитва подняли мораль ваших войск.",
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
                    "{Навес}\n\nЭто сооружение давно брошено. Ничего ценного тут нет.",
                    NORMAL_DIALOG_INFO
                );
            } else {
                EventSound(eventType, cell->m_objectMetadata, &eventSample);
                NormalDialog(
                    "{Навес}\n\nВы нашли брошенный навес. Поискав вокруг, вы обнаружили кое-какие ресурсы, припрятанные поблизости.",
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
                    "{Повозка}\n\nВы набрели на старую повозку, не доехавшую до торной дороги и брошенную купцом. К сожалению, кто-то нашел ее до вас, повозка пуста.",
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
                    "{Повозка}\n\nВы набрели на старую повозку, не доехавшую до торной дороги и брошенную купцом. В ней вы нашли %s.",
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
                    "{Повозка}\n\nВы набрели на старую повозку, не доехавшую до торной дороги и брошенную купцом. В ней же вы нашли до сих пор не тронутый груз.",
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
                            "Сокровище"
                        );
                        EventWindow(
                            -1,
                            NORMAL_DIALOG_INFO,
                            gText,
                            (RES_GOLD),
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
                (RES_GOLD),
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
                ResourceType(cell->m_objectMetadata & (CAMPFIRE_RESOURCE_MASK)),
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
                    "{Ксанаду}\n\nУзнав вас, лакей отказался пустить внутрь. \"Наставник,\" - сказал он, - \"не принимает одного и того же студента дважды.\"",
                    NORMAL_DIALOG_INFO
                );
            } else {
                if (eventHero->m_level
                        + (eventHero->m_secondarySkills[(HERO_SKILL_DIPLOMACY)])
                              * XANADU_DIPLOMACY_MULTIPLIER
                    >= XANADU_ADMISSION_LEVEL) {
                    EventSound(eventType, cell->m_objectMetadata, &eventSample);
                    NormalDialog(
                        "{Ксанаду}\n\nЛакей допустил вас до аудиенции с хозяином дома. Занятия с ним повысили четыре умения, необходимые каждому герою.",
                        NORMAL_DIALOG_INFO
                    );
                    eventHero->m_primaryStats[(HERO_PRIMARY_ATTACK)]++;
                    eventHero->m_primaryStats[(HERO_PRIMARY_DEFENSE)]++;
                    eventHero->m_primaryStats[(HERO_PRIMARY_KNOWLEDGE)]++;
                    eventHero->m_primaryStats[(HERO_PRIMARY_SPELL_POWER)]++;
                    eventHero->m_xanaduVisits |= 1 << cell->m_objectMetadata;
                } else {
                    NormalDialog(
                        "{Ксанаду}\n\nЛакей открыл дверь и оглядел вас с ног до головы. \"Вы не так знамениты и не такой уж дипломат, чтобы мой хозяин принял вас,\" - фыркнул он. - \"Возвращайтесь, когда сочтете себя достойным.\"",
                        NORMAL_DIALOG_INFO
                    );
                }
            }
            break;

        case MAP_OBJECT_FORT:
            if (eventHero->m_fortVisits & (1 << cell->m_objectMetadata)) {
                NormalDialog(
                    "{Форт}\n\n \"Просите, сэр,\" - Сказал предводитель воинов, - \"но вы уже знаете все, чему мы способны научить.\"",
                    NORMAL_DIALOG_INFO
                );
            } else {
                EventSound(eventType, cell->m_objectMetadata, &eventSample);
                NormalDialog(
                    "{Форт}\n\n Воины, живущие в этом форте, научили вас паре новых защитных приемов.",
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
                eventHero->m_primaryStats[(HERO_PRIMARY_DEFENSE)]++;
                eventHero->m_fortVisits |= 1 << cell->m_objectMetadata;
            }
            break;

        case MAP_OBJECT_STANDING_STONES:
            if (eventHero->m_standingStoneVisits & (1 << cell->m_objectMetadata)) {
                NormalDialog(
                    "{Менгир}\n\nВы застали группу друидов за обрядом в одном из их диковинных каменных сооружений. Друиды, не нарушая безмолвия, показали жестами, что им больше нечему вас учить.",
                    NORMAL_DIALOG_INFO
                );
            } else {
                EventSound(eventType, cell->m_objectMetadata, &eventSample);
                NormalDialog(
                    "{Менгир}\n\nВы застали группу друидов за обрядом в одном из их диковинных каменных сооружений. Не нарушая безмолвия, они все же научили вас новым способам колдовства.",
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
                eventHero->m_primaryStats[(HERO_PRIMARY_SPELL_POWER)]++;
                eventHero->m_standingStoneVisits |= 1 << cell->m_objectMetadata;
            }
            break;

        case MAP_OBJECT_WITCH_DOCTOR_HUT:
            if (eventHero->m_witchDoctorVisits & (1 << cell->m_objectMetadata)) {
                NormalDialog(
                    "{Хижина ведьмы}\n\n\"Убирайся!\" -  рявкнула ведьма, \"ты знаешь все, что знаю я.\"",
                    NORMAL_DIALOG_INFO
                );
            } else {
                EventSound(eventType, cell->m_objectMetadata, &eventSample);
                NormalDialog(
                    "{Хижина ведьмы}\n\nОрчая ведьма, живущая в этой хижине, обогатила ваши познания в магии, показав, как гадать на камнях, читать знамения и извлекать сущность бытия из сложного переплетения цыплячьих потрохов.",
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
                eventHero->m_primaryStats[(HERO_PRIMARY_KNOWLEDGE)]++;
                eventHero->m_witchDoctorVisits |= 1 << cell->m_objectMetadata;
            }
            break;

        case MAP_OBJECT_MERCENARY_CAMP:
            if (eventHero->m_mercenaryCampVisits & (1 << cell->m_objectMetadata)) {
                NormalDialog(
                    "{Лагерь наемников}\n\nВы пришли в лагерь наемников, где воины отрабатывают тактику. \"Для нас вы слишком умелый боец,\" -сказал капитан наемников. - \"Больше мы ничему вас научить не можем.\"",
                    NORMAL_DIALOG_INFO
                );
            } else {
                EventSound(eventType, cell->m_objectMetadata, &eventSample);
                NormalDialog(
                    "{Лагерь наемников}\n\nВы пришли в лагерь наемников, отрабатывающих свою тактику. Наемники поприветствовали вас и пригласили вместе с вашим отрядом позаниматься вместе с ними.",
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
                eventHero->m_primaryStats[(HERO_PRIMARY_ATTACK)]++;
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
                    (RES_GOLD),
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
            strcpy(sphinxAnswer, gResourceNames[(eventResourceType)]);
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
            if (cell->m_objectMetadata <= (RES_GOLD)) {
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
                    "{Древо-город}\n\nВы нашли древесный город фей. К сожалению, ни одна фея не захотела присоединиться к вашей армии. Может быть на следующей неделе они передумают.",
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
                    "{Древо-город}\n\nНекоторые феи из этого города на деревьях желают вступить в вашу армию за некоторое вознаграждение. Желаете нанять фей?",
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
                    "{Руины}\n\nВы обыскали руины, но убедились лишь, что жившие тут медузы ушли. Может быть вам повезет больше на следующей неделе.",
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
                    "{Руины}\n\nВы обыскали руины и нашли нескольких медуз, обитающих тут. Они согласны вступить в вашу армию за вознаграждение. Желаете нанять медуз?",
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
                    "{Мост троллей}\n\nВы нашли один из тех мостов, под которыми так любят селиться тролли, но сейчас тут никого нет. Может, на следующей неделе появятся.",
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
                    "{Мост троллей}\n\nТролли, живущие под этим мостом бросают вам вызов. Сразитесь с ними?",
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
                        "{Мост троллей}\n\nОсталось лишь несколько троллей, в страхе забившихся под мост. Осмелев, они предложили свои услуги в качестве наемников. Желаете нанять троллей?",
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
                    "{Мост троллей}\n\nКое-кто из троллей, живущих под мостом, желает присоединиться к вашей армии, но за плату.  Желаете нанять троллей?",
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
                    "{Град мертвых}\n\nВ этом городе мертвецов жизни не наблюдается, нежити тоже. Может, на следующей неделе кто-то из нежити забредет сюда.",
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
                    "{Град мертвых}\n\nВы нашли руины древнего города, ныне населенного лишь нежитью. Обследовать город?",
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
                    "{Град мертвых}\n\nУцелевшие личи прониклись вашей победой над их собратьями и предложили свои услуги за плату. Желаете нанять личей?",
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
                    "{Град мертвых}\n\nКое-кто из личей, обитающих здесь, желает вступить в вашу армию за плату. Нанять личей?",
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
                    "{Драконий город}\n\nНа этой неделе в городе нет драконов, желающих примкнуть к вам. На следующей неделе, возможно, кто-то появится.",
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
                    "{Драконий город}\n\nВы стоите пред драконьим городом, местом, запретным для простых смертных. Соблаговолите ли вы нарушить это правило и бросить вызов драконам?",
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
                            "{Драконий город}\n\nПосле вашей победы над лучшими драконьими воинами отцы города согласились за плату предоставить вашему войску драконов. Желаете нанять драконов?",
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
                    "{Драконий город}\n\nДраконий город готов предложить вашему войску драконов, не бесплатно. Желаете нанять драконов?",
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
                    "{Древо познания}\n\nПри вашем приближении древесные глаза засияли восторгом.  \"Рад видеть тебя, мой ученик.  Надеюсь, моя наука пошла тебе на пользу.\"",
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
                            "{Древо познания}\n\nПри вашем приближении древесные глаза засветились восторгом. \"А, странник! Позволь преподать тебе малую толику того, что я выучил за годы.\"",
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
                        if (gpCurPlayer->m_resources[(RES_GOLD)] >= TREE_KNOWLEDGE_GOLD_COST) {
                            NormalDialog(
                                "{Древо познания}\n\nПри вашем приближении древесные глаза засияли восторгом. \"А, путник! Я с радостью научу тебя хотя бы малой части того, что усвоил за годы, всего за 2000 золотых.\" (Просто зарой их у моих корней).",
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
                                gpCurPlayer->m_resources[(RES_GOLD)] -= TREE_KNOWLEDGE_GOLD_COST;
                                GiveExperience(eventHero, levelExperience, 0);
                                eventHero->m_treeKnowledgeVisits |=
                            1U << (cell->m_objectMetadata & TREE_KNOWLEDGE_VISIT_INDEX_MASK);
                            }
                        } else {
                            NormalDialog(
                                "{Древо познания}\n\nГлаза дерева наполнились слезами. \"Мне нужно 2000 золотых.\" - прошептало оно. (вздох) - \"Возвращайся, когда сможешь внести плату.\"",
                                NORMAL_DIALOG_INFO
                            );
                        }
                        break;

                    case TREE_KNOWLEDGE_GEMS:
                        if (gpCurPlayer->m_resources[(RES_GEMS)] >= TREE_KNOWLEDGE_GEM_COST) {
                            NormalDialog(
                                "{Древо познания}\n\nА, путник! Я с радостью преподам тебе хоть малую часть того, что выучил за годы, всего за 10 самоцветов.\" (Просто зарой их у моих корней).",
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
                                gpCurPlayer->m_resources[(RES_GEMS)] -= TREE_KNOWLEDGE_GEM_COST;
                                GiveExperience(eventHero, levelExperience, 0);
                                eventHero->m_treeKnowledgeVisits |=
                            1U << (cell->m_objectMetadata & TREE_KNOWLEDGE_VISIT_INDEX_MASK);
                            }
                        } else {
                            NormalDialog(
                                "{Древо познания}\n\nСлезы переполняют древесные глаза. \"Мне нужно 10 самоцветов.\" - прошептало оно. (Вздох). \"Что ж, приходи, когда сможешь заплатить.\"",
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
                "Святилище - информация об игроках"
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
                "{Святилище 1-го Круга}\n\nВы набрели на маленькое святилище, где служат молодые послушники.  В обмен на защиту они согласились научить вас простому заклинанию - ",
                gSpellNames[cell->m_objectMetadata - 1]
            );
            goto shrineSpell;

        case MAP_OBJECT_SHRINE_SECOND_CIRCLE:
            sprintf(
                gText,
                "%s'%s'.  ",
                "{Святилище 2-го Круга}\n\nВы набрели на богато расписанное святилище, где служат полноправные братья веры.  В обмен на защиту они согласились научить вас новому заклинанию - ",
                gSpellNames[cell->m_objectMetadata - 1]
            );
            goto shrineSpell;

        case MAP_OBJECT_SHRINE_THIRD_CIRCLE:
            sprintf(
                gText,
                "%s'%s'.  ",
                "{Святилище 3-го Круга}\n\nВы набрели на роскошное святилище, где служат высшие жрецы.  В обмен на защиту они согласились научить вас премудрому заклинанию - ",
                gSpellNames[cell->m_objectMetadata - 1]
            );
        shrineSpell:
            if (eventHero->HasArtifact(ARTIFACT_MAGIC_BOOK)) {
                if ((gsSpellInfo[cell->m_objectMetadata - 1].level)
                    <= (eventHero->m_secondarySkills[(HERO_SKILL_WISDOM)])
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
                        "К сожалению, у вас не хватает мудрости, чтобы понять заклинание, и вы не можете выучить его."
                    );
                    EventWindow(-1, NORMAL_DIALOG_INFO, gText, -1, 0, -1, 0, -1);
                }
            } else {
                strcat(
                    gText,
                    "К сожалению, у вас нет Волшебной книги, чтобы записать заклинание в нее."
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
                    if (((gpGame->m_worldMap.GetCell(teleportX, teleportY))->m_triggerType).value() == static_cast<u8>(eventType | MAP_TRIGGER_ACTION_FLAG)
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
                        if (((gpGame->m_worldMap.GetCell(teleportX, teleportY))
                                            ->m_triggerType).value()
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
                    "Вы не можете подобрать этот артефакт, у вас нет места!",
                    NORMAL_DIALOG_INFO
                );
                break;
            }

            if (xIsPlayingExpansionCampaign && xCampaign.IsSpecialGoldenBow(x, y)) {
                EventSound(eventType, cell->m_objectMetadata, &eventSample);
                strcpy(
                    gText,
                    "Сказочный золотой лук эльфов просто валяется здесь, в грязи. Вы подобрали его и отвезли в эльфийский город. В благодарность за этот подвиг король эльфов пообещал вам помощь своего народа в любую минуту, когда вам это понадобится."
                );
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    gText,
                    MAP_EVENT_REWARD_ARTIFACT,
                    (eventArtifact),
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
                    gArtifactEvent[(eventArtifact)],
                    MAP_EVENT_REWARD_ARTIFACT,
                    (eventArtifact),
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
                        "{Артефакт}\n\nВы заметили древний артефакт. Но едва вы подошли к нему, из кустов выскочила шайка разбойников, охраняющих свое награбленное добро.",
                        NORMAL_DIALOG_INFO
                    );
                    guardedCount = ARTIFACT_EVENT_GUARD_ROGUE_COUNT;
                    goto artifactFight;
                } else {
                    guardedCount = 1;
                    sprintf(
                        gText,
                        "{Артефакт}\n\nНа прогалине вы приметили древний артефакт. К сожалению, его сторожит отряд %s. Не пугает ли вас сила %s, и будете ли вы сражаться за артефакт?",
                        gArmyNamesPlural[(guardedMonster)],
                        gArmyNamesPlural[(guardedMonster)]
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
                            "Одержав победу, вы получили свой трофей - %s.",
                            gArtifactNames[(eventArtifact)]
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
                        "Главное в доблести - благоразумие, поэтому вы решили пока что воздержаться от этой битвы.",
                        NORMAL_DIALOG_INFO
                    );
                }
                break;
            } else {
                switch (cell->m_objectMetadata & ARTIFACT_EVENT_MODE_MASK) {
                    case ARTIFACT_EVENT_MODE_WISDOM:
                        if (eventHero->m_secondarySkills[(HERO_SKILL_WISDOM)]
                            != HERO_SKILL_LEVEL_NONE)
                            goto artifactPickup;
                        sprintf(
                            gText,
                            "{Аретфакт}\n\nВы нашли убогий скит изможденного отшельника. Тот поведал вам, что его %s достанется первому же встречному великому мудрецу.",
                            gArtifactNames[(eventArtifact)]
                        );
                        NormalDialog(gText, NORMAL_DIALOG_INFO);
                        break;

                    case ARTIFACT_EVENT_MODE_LEADERSHIP:
                        if (eventHero->m_secondarySkills[(HERO_SKILL_LEADERSHIP)]
                            != HERO_SKILL_LEVEL_NONE)
                            goto artifactPickup;
                        sprintf(
                            gText,
                            "{Артефакт}\n\nВы навестили старого солдата в его скромном жилище. Солдат поведал вам, что его %s достанется первому настоящему лидеру, которого он встретит.",
                            gArtifactNames[(eventArtifact)]
                        );
                        NormalDialog(gText, NORMAL_DIALOG_INFO);
                        break;

                    case ARTIFACT_EVENT_MODE_PICKUP:
                    artifactPickup:
                        EventSound(eventType, cell->m_objectMetadata, &eventSample);
                        EventWindow(
                            -1,
                            NORMAL_DIALOG_INFO,
                            gArtifactEvent[(eventArtifact)],
                            MAP_EVENT_REWARD_ARTIFACT,
                            (eventArtifact),
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
                            "{Артефакт}\n\nЛепрекон предлагает вам артефакт - %s, всего за 2000 золотых. Купите его?",
                            gArtifactNames[(eventArtifact)]
                        );
                        EventWindow(
                            -1,
                            NORMAL_DIALOG_CONFIRM,
                            gText,
                            MAP_EVENT_REWARD_ARTIFACT,
                            (eventArtifact),
                            -1,
                            0,
                            -1
                        );
                        if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_YES) {
                            if (gpGame->m_players[(eventHero->m_owner)].m_resources[(RES_GOLD)]
                                >= ARTIFACT_EVENT_GOLD_COST) {
                                gpGame->m_players[(eventHero->m_owner)].m_resources[(RES_GOLD)] -=
                                    ARTIFACT_EVENT_GOLD_COST;
                                goto giveArtifact;
                            }
                            NormalDialog(
                                "Вы и хотели бы расплатиться с лепреконом, но поняли, что сделка вам не по карману. Лепрекон затопал ногами и обиженно надулся.",
                                NORMAL_DIALOG_INFO
                            );
                        } else {
                            NormalDialog(
                                "Оскорбленный вашим отказом от его щедрого предложения, лекреком притопнул ногой и отвернулся от вас.",
                                NORMAL_DIALOG_INFO
                            );
                        }
                        break;

                    case ARTIFACT_EVENT_MODE_RESOURCE_3:
                        EventSound(eventType, cell->m_objectMetadata, &eventSample);
                        sprintf(sphinxAnswer, gResourceNames[(artifactResourceType)]);
                        sphinxAnswer[0] = CyrillicToLower(sphinxAnswer[0]);
                        sprintf(
                            gText,
                            "{Артефакт}\n\nЛепрекон предлагает вам артефакт - %s за 2500 золотых и просит %s - 3 единицы. Желаете купить предмет?",
                            gArtifactNames[(eventArtifact)],
                            sphinxAnswer
                        );
                        NormalDialog(
                            gText,
                            NORMAL_DIALOG_CONFIRM,
                            -1,
                            -1,
                            MAP_EVENT_REWARD_ARTIFACT,
                            (eventArtifact),
                            -1,
                            0,
                            -1,
                            0
                        );
                        if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_YES) {
                            if (gpGame->m_players[(eventHero->m_owner)].m_resources[(RES_GOLD)]
                                    >= ARTIFACT_EVENT_RESOURCE_3_GOLD_COST
                                && gpGame->m_players[(eventHero->m_owner)]
                                           .m_resources[(artifactResourceType)]
                                       >= ARTIFACT_EVENT_RESOURCE_3_AMOUNT) {
                                gpGame->m_players[(eventHero->m_owner)].m_resources[(RES_GOLD)] -=
                                    ARTIFACT_EVENT_RESOURCE_3_GOLD_COST;
                                gpGame->m_players[(eventHero->m_owner)]
                                    .m_resources[(artifactResourceType)] -=
                                    ARTIFACT_EVENT_RESOURCE_3_AMOUNT;
                                goto giveArtifact;
                            }
                            NormalDialog(
                                "Вы и хотели бы расплатиться с лепреконом, но поняли, что сделка вам не по карману. Лепрекон затопал ногами и обиженно надулся.",
                                NORMAL_DIALOG_INFO
                            );
                        } else {
                            NormalDialog(
                                "Оскорбленный вашим отказом от его щедрого предложения, лекреком притопнул ногой и отвернулся от вас.",
                                NORMAL_DIALOG_INFO
                            );
                        }
                        break;

                    case ARTIFACT_EVENT_MODE_RESOURCE_5:
                        EventSound(eventType, cell->m_objectMetadata, &eventSample);
                        sprintf(sphinxAnswer, gResourceNames[(artifactResourceType)]);
                        sphinxAnswer[0] = CyrillicToLower(sphinxAnswer[0]);
                        sprintf(
                            gText,
                            "{Артефакт}\n\nЛепрекон предлагает вам артефакт - %s за 3000 золотых и просит %s - 5 единиц. Желаете купить предмет?",
                            gArtifactNames[(eventArtifact)],
                            sphinxAnswer
                        );
                        NormalDialog(
                            gText,
                            NORMAL_DIALOG_CONFIRM,
                            -1,
                            -1,
                            MAP_EVENT_REWARD_ARTIFACT,
                            (eventArtifact),
                            -1,
                            0,
                            -1,
                            0
                        );
                        if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_YES) {
                            if (gpGame->m_players[(eventHero->m_owner)].m_resources[(RES_GOLD)]
                                    >= ARTIFACT_EVENT_RESOURCE_5_GOLD_COST
                                && gpGame->m_players[(eventHero->m_owner)]
                                           .m_resources[(artifactResourceType)]
                                       >= ARTIFACT_EVENT_RESOURCE_5_AMOUNT) {
                                gpGame->m_players[(eventHero->m_owner)].m_resources[(RES_GOLD)] -=
                                    ARTIFACT_EVENT_RESOURCE_5_GOLD_COST;
                                gpGame->m_players[(eventHero->m_owner)]
                                    .m_resources[(artifactResourceType)] -=
                                    ARTIFACT_EVENT_RESOURCE_5_AMOUNT;
                                goto giveArtifact;
                            }
                            NormalDialog(
                                "Вы и хотели бы расплатиться с лепреконом, но поняли, что сделка вам не по карману. Лепрекон затопал ногами и обиженно надулся.",
                                NORMAL_DIALOG_INFO
                            );
                        } else {
                            NormalDialog(
                                "Оскорбленный вашим отказом от его щедрого предложения, лекреком притопнул ногой и отвернулся от вас.",
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
                "{Пещера демона}\n\nВход в пещеру зияет черной дырой, из которой тянет тошнотворным сернистым зловонием. Отважитесь ли вы войти?",
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
                    "Если не считать следов ужасной битвы, пещера пуста.",
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
                "В пещере вы обнаружили грозного до нелепости демона. \"Сегодня,\" - прорычал он, - \"тебя ждут бой и верная смерть. Но я позволю тебе выбрать смерть. Ты можешь драться со мной или же с моими слугами. Предпочитаешь сразиться с моими слугами?\""
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
                        "Одолев слуг демона, вы нашли клад в размере 2500 золотых.",
                        NORMAL_DIALOG_INFO,
                        -1,
                        -1,
                        (RES_GOLD),
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
                        "Демон выкрикнул свой вызов и бросился в бой! После краткой, но отчаянной схватки вы прикончили чудовище и получили 1000 очков опыта.",
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
                    eventValue = (GiveRandomArtifact(eventHero));
                    sprintf(
                        gText,
                        "Демон выкрикнул свой вызов и бросился в бой! После непродолжительной, но отчаянной битвы вы прикончили чудовище, а в глубине пещеры нашли %s.",
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
                        "Демон выкрикнул свой вызов и бросился в бой! После непродолжительной, но отчаянной схватки вы прикончили чудовище, получив 1000 очков опыта и 2500 золотых.",
                        (RES_GOLD),
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
                        "Демон набросился на вас и вцепился когтями в горло прежде, чем вы обнажили меч. \"Твоя жизнь в моих руках,\" - прорычал он. - \"Я верну ее тебе за 2500 золотых.\"",
                        -1,
                        0,
                        -1,
                        0,
                        -1
                    );
                    if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_YES) {
                        if (gpGame->m_players[(eventHero->m_owner)].m_resources[(RES_GOLD)]
                            < DAEMON_GOLD) {
                            EventWindow(
                                -1,
                                NORMAL_DIALOG_INFO,
                                "Поняв, что у вас нет 2500 золотых, демон растерзал вас своими клыками, и последнее, что вы видели, была алая пелена перед глазами.",
                                -1,
                                0,
                                -1,
                                0,
                                -1
                            );
                            HeroLoses(eventHero);
                        } else {
                            gpGame->m_players[(eventHero->m_owner)].m_resources[(RES_GOLD)] -=
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
                "{Кораблекрушение}\n\nГниющий остов огромного пиратского корабля зловеще поскрипывает, покачиваемый прибоем на скалах. Желаете обыскать обломки?",
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
                            "Совладав с призраками, вы потратили несколько часов на рытье в мусоре, но так ничего и не нашли. Столь неблаговидное занятие снизило маораль вашей армии.",
                            NORMAL_DIALOG_MORALE_PENALTY,
                            0,
                            -1,
                            0,
                            -1
                        );
                        if (!(((eventHero->m_eventFlags) & (HERO_EVENT_SHIPWRECK)))) {
                            eventHero->m_eventFlags =
                                eventHero->m_eventFlags | HERO_EVENT_SHIPWRECK;
                            eventHero->m_morale--;
                        }
                        break;
                    default:
                        if (GhostEvent(
                                eventHero,
                                cell,
                                "Одолев призраков, вы обыскали останки корабля и кое-что нашли!",
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
                        if (!(((eventHero->m_eventFlags) & (HERO_EVENT_GRAVEYARD)))) {
                            eventHero->m_eventFlags =
                                eventHero->m_eventFlags | HERO_EVENT_GRAVEYARD;
                            eventHero->m_morale--;
                        }
                        break;
                    default:
                        zombieCell = GetCell(
                            x - normalDirTable[(eventHero->m_direction)].x,
                            y - normalDirTable[(eventHero->m_direction)].y
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
                "{Брошенный корабль}\n\nГниющий остов огромного пиратского корабля зловеще скрипит, покачиваемый волнами на скалах. Желаете обыскать корабль?",
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
                            "Усмирив скелетов, вы потратили несколько часов на поиски среди гнилых обломков - и впустую. Столь грязная работа плохо сказалась на морали вашей армии.",
                            NORMAL_DIALOG_MORALE_PENALTY,
                            0,
                            -1,
                            0,
                            -1
                        );
                        if (!(((eventHero->m_eventFlags) & (HERO_EVENT_DERELICT_SHIP)))) {
                            eventHero->m_eventFlags =
                                eventHero->m_eventFlags | HERO_EVENT_DERELICT_SHIP;
                            eventHero->m_morale--;
                        }
                        break;
                    default:
                        skeletonCell = GetCell(
                            x - normalDirTable[(eventHero->m_direction)].x,
                            y - normalDirTable[(eventHero->m_direction)].y
                        );
                        if (SkeletonEvent(
                                eventHero,
                                skeletonCell,
                                "Совладав со скелетами, вы обыскали обломки и кое-что нашли!",
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
                "Вы нашли пирамиду древнего великого царя. Искушение поискать в ней сокровища велико, но вы слышали все эти сказки об ужасных проклятиях и неупокоенных стражей. Хотите обследовать пирамиду?",
                -1,
                0,
                -1,
                0,
                -1
            );
            if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_YES) {
                if (!cell->m_objectMetadata) {
                    NormalDialog(
                        "Вы нашли пирамиду древнего великого царя. Первое же обследование пирамиды показало, что в ней абсолютно ничего нет.",
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
                    if (!(((eventHero->m_eventFlags) & (HERO_EVENT_PYRAMID)))) {
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
                            "Одолев чудовищ, вы расшифровали иероглифы на стене, сообщающие секрет заклинания - ",
                            gSpellNames[cell->m_objectMetadata - 1]
                        );
                        if (!eventHero->HasArtifact(ARTIFACT_MAGIC_BOOK)) {
                            strcat(
                                eventText,
                                "  К сожалению, у вас нет Волшебной книги, чтобы записать в нее это заклинание."
                            );
                            EventWindow(-1, NORMAL_DIALOG_INFO, eventText, -1, 0, -1, 0, -1);
                        } else if (eventHero->m_secondarySkills[(HERO_SKILL_WISDOM)]
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
                                "  К сожалению, у вас недостаточно мудрости, чтобы понять заклинание, ивы не можете выучить его."
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
                "Вы обнаружили заброшенную золотую шахту. Кажется, она населена призраками. Желаете войти внутрь?",
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
                        "Вы разбили призраков и смогли наладить добычу золота.",
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
    gpSoundManager->SwitchAmbientMusic(giTerrainToMusicTrack[(m_currentTerrain)]);
    WaitEndSample(&eventSample);
    CheckEndGame(END_GAME_FORCE_NONE, false);
}

void advManager::EraseObj(class mapCell* cell, i32 x, i32 y) {
    b32 erased [[maybe_unused]] = false;
    mapCellExtra* extras[NEIGHBOR_COUNT];
    mapCellExtra* extra;
    mapCell* cells[NEIGHBOR_COUNT];
    i32 frame = NO_FRAME;
    mapCell* currentCell;
    b8 isWide = false;
    i32 neighborIndex;
    i32 unusedValues [[maybe_unused]][ERASE_COORDINATE_COUNT];
    i32 cellX;
    i32 cellY;
    i32 changed [[maybe_unused]];

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

        currentCell->m_flags |= (MAP_CELL_OBJECT_SHADOW_ONLY);
    cellDone:
        ;
        changed = 0;
    }

    SendMapChange(MAP_CHANGE_ERASE_OBJECT, 0, x, y, MAP_CHANGE_VALUE, 0, 0);
    SetEnvironmentOrigin(m_mapOriginX + ENVIRONMENT_BORDER, m_mapOriginY + ENVIRONMENT_BORDER, 1);
    gpGame->SetupAdjacentMons();
}

void advManager::HeroSwap(hero* firstHero, hero* secondHero) {
    swapManager* swapWindow = new swapManager(firstHero, secondHero);
    if (swapWindow == NULL)
        MemError();
    gpExec->DoDialog(swapWindow);
    delete swapWindow;
    RedrawAdvScreen(1, 0);
}

i32 advManager::BarrierEvent(mapCell* cell, hero*) {
    SAMPLE2 eventSample = NULL;
    i32 colorIndex = cell->m_objectMetadata;
    colorIndex &= EVENT_BARRIER_COLOR_MASK;
    i32 passwordIndex = cell->m_objectMetadata;
    passwordIndex >>= PASSWORD_SHIFT;
    char word[INPUT_LENGTH];

    sprintf(
        gText,
        "Дорогу вам преграждает %s магический барьер. Руны на нем гласят: \"Скажи слово и сможешь пройти.\"",
        xBarrierColor[colorIndex]
    );
    GetDataEntry(gText, word, INPUT_LENGTH, NULL, 0, 1);
    if (StrEqNoCase(word, xPasswordStrings[passwordIndex])
        && PLAYER_HAS_VISITED_TENT(*gpCurPlayer, colorIndex)) {
        EventSound(cell->m_triggerType & MAP_TRIGGER_TYPE_MASK, colorIndex, &eventSample);
        NormalDialog(
            "Едва вы произнесли волшебное слово, как сверкающий барьер исчез в пустоте.",
            NORMAL_DIALOG_INFO
        );
        return 1;
    } else {
        NormalDialog(
            "Вы произнесли слово, но ничего не произошло.",
            NORMAL_DIALOG_INFO
        );
        return 0;
    }
}

i8 StrEqNoCase(const char* firstString, const char* sndString) {
    const char* firstPointer = firstString;
    const char* secondPointer = sndString;
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

void advManager::PasswordEvent(mapCell* cell, hero*) {
    SAMPLE2 playSample = NULL;
    i32 color = cell->m_objectMetadata;
    color &= EVENT_BARRIER_COLOR_MASK;
    i32 passwordIndex = cell->m_objectMetadata;
    passwordIndex >>= PASSWORD_SHIFT;

    EventSound(cell->m_triggerType & MAP_TRIGGER_TYPE_MASK, color, &playSample);


    sprintf(
        gText,
        "Вы зашли в шатер и увидели пожилую женщину с магическим камнем в руках. Посмотрев на вас, она сказала: \"Странствуя, я познала тайную магию. Великий оракул даровал мне знания и у меня есть ответ на твой вопрос.  Волшебное слово, снимающее %s барьер - '%s'.\"",
        xBarrierColor[color],
        xPasswordStrings[passwordIndex]
    );
    NormalDialog(gText, NORMAL_DIALOG_INFO);
    gpCurPlayer->m_barrierTents |= 1 << color;
}

void advManager::GenericSiteEvent(mapCell* cell, hero* eventHero) {
    i32 primaryStat;
    i32 cursedArtifactCount;
    i32 siteLevel [[maybe_unused]];
    SAMPLE2 eventSample;
    i32 index;
    GenericSiteType siteType;
    i32 currentSiteType;
    i32 mapX;
    i32 unusedTwo;
    i32 mapY;
    mapCell* currentCell;
    i32 unusedSite [[maybe_unused]];
    i32 unusedOne;
    i32 oldQuantity;
    i8 stableResult;
    CreatureType creatureType;
    i32 experience;

    cursedArtifactCount = 0;
    eventSample = NULL;
    siteType = static_cast<GenericSiteType>(cell->m_objectMetadata);
    siteType = static_cast<GenericSiteType>((siteType) & GENERIC_SITE_TYPE_MASK);
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
                    cell->m_triggerType & MAP_TRIGGER_TYPE_MASK, (siteType), &eventSample
                );
                if (cursedArtifactCount == 1) {
                    sprintf(
                        gText,
                        "Едва вы вошли в башню алхимика, как на встречу вам вышел хромой и седой мужчина. Он проверил ваши вещи и заметил, что среди них есть одна проклятая вещь. За 750 золотых алхимик согласен забрать ее у вас. Вы заплатите?"
                    );
                } else {
                    sprintf(
                        gText,
                        " Едва вы вошли в башню алхимика, как на встречу вам вышел хромой и седой мужчина. Он проверил ваши вещи и заметил, что среди них %d проклятые вещи. За 750 золотых алхимик согласен забрать их у вас. Вы заплатите?",
                        cursedArtifactCount
                    );
                }
                NormalDialog(gText, NORMAL_DIALOG_CONFIRM);
                if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_YES) {
                    if (gpCurPlayer->m_resources[(RES_GOLD)] >= SITE_ALCHEMIST_COST) {
                        for (index = 0; index < HERO_ARTIFACT_SLOT_COUNT; index++) {
                            if (IsCursedItem(eventHero->m_artifacts[index])) {
                                GiveTakeArtifactStat(
                                    eventHero, eventHero->m_artifacts[index], EVENT_ARTIFACT_TAKE
                                );
                                eventHero->m_artifacts[index] = ARTIFACT_NONE;
                            }
                        }
                        gpCurPlayer->m_resources[(RES_GOLD)] -= SITE_ALCHEMIST_COST;
                    } else {
                        NormalDialog(
                            "Вы слышите голос из-за запертой двери: \"У вас недостаточно золота для моих услуг.\"",
                            NORMAL_DIALOG_INFO
                        );
                    }
                }
            } else {
                NormalDialog(
                    "Вы слышите голос с вершины башни: \"Убирайтесь! Я не могу вам помочь!\"",
                    NORMAL_DIALOG_INFO
                );
            }
            break;

        case GENERIC_SITE_ARENA:
            if ((((eventHero->m_eventFlags) & (HERO_EVENT_ARENA)))) {
                NormalDialog(
                    "Стража Арены преградила вам дорогу и не пустила внутрь.",
                    NORMAL_DIALOG_INFO
                );
            } else {
                EventSound(
                    cell->m_triggerType & MAP_TRIGGER_TYPE_MASK, (siteType), &eventSample
                );
                eventHero->m_eventFlags = eventHero->m_eventFlags | HERO_EVENT_ARENA;
                primaryStat = DoArenaDialog();
                eventHero->m_primaryStats[primaryStat]++;
            }
            break;

        case GENERIC_SITE_MERMAID:
            if ((((eventHero->m_eventFlags) & (HERO_EVENT_MERMAID)))) {
                NormalDialog(
                    "Русалки молчаливо дали вам понять, чтобы вы приходили в другой раз, тогда они благословят вас.",
                    NORMAL_DIALOG_INFO
                );
            } else {
                EventSound(
                    cell->m_triggerType & MAP_TRIGGER_TYPE_MASK, (siteType), &eventSample
                );
                eventHero->m_eventFlags = eventHero->m_eventFlags | HERO_EVENT_MERMAID;
                eventHero->m_luck++;
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    "Магическая красота русалок обволокла вас и вашу команду. На какой-то миг вы позабыли о ваших проблемах и насладились этим мгновением красоты. Чары русалок благословили вас на удачу в следующем бою.",
                    SITE_MERMAID_WINDOW_ICON,
                    0,
                    -1,
                    0,
                    -1
                );
            }
            break;

        case GENERIC_SITE_HUT_OF_MAGI:
            EventSound(cell->m_triggerType & MAP_TRIGGER_TYPE_MASK, (siteType), &eventSample);
            NormalDialog(
                "Вы зашли в ветхую хибару и заговорили с магом, живущим тут. Он поведал вам о местах, которые ему доводилось видеть. Это может пригодиться вам в ваших путешествиях.",
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
                "Кажется, этот глаз внимательно изучает окрестности.",
                NORMAL_DIALOG_INFO
            );
            break;

        case GENERIC_SITE_SIRENS:
            if ((((eventHero->m_eventFlags) & (HERO_EVENT_SIRENS)))) {
                NormalDialog(
                    "Вы приказали своей команде залить уши воском, прежде чем подплыть к сиренам, чтобы послушать их пение, которое запросто может погубить всех в морской пучине.",
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
                                gMonsterDatabase[(creatureType)].hitPoints
                                * (oldQuantity - eventHero->m_army.m_creatureCounts[index]);
                        }
                    }
                }
                if (experience != 0) {
                    EventSound(
                        cell->m_triggerType & MAP_TRIGGER_TYPE_MASK,
                        (siteType),
                        &eventSample
                    );
                    sprintf(
                        gText,
                        "Над камнями раздалась жуткая песня сирен. Многие из вашей команды росились в воду действием этих чар и утонули в морской пучине. Для вас это был хороший урок, давший %d очков опыта.",
                        experience
                    );
                    NormalDialog(gText, NORMAL_DIALOG_INFO);
                    GiveExperience(eventHero, experience, 1);
                } else {
                    NormalDialog(
                        "Едва сирены затянули свою ужасную песню, как ваша команда посильнее налегла на весла и увела подальше корабль в открыто море.",
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
            if ((((eventHero->m_eventFlags) & (HERO_EVENT_STABLES))) == 0) {
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
                    cell->m_triggerType & MAP_TRIGGER_TYPE_MASK, (siteType), &eventSample
                );
            }
            sprintf(gText, xStableText[(stableResult)]);
            if ((((stableResult) & (STABLE_VISIT_UPGRADE)))) {
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

void advManager::RecruitSiteEvent(mapCell* cell, hero* eventHero) {
    SAMPLE2 recruitSample = NULL;
    u32 recruitSiteType;
    i16 quantity;
    CreatureType creatureType;
    u32 index;
    u32 packedValue;

    recruitSiteType = cell->m_objectMetadata;
    recruitSiteType &= (EVENT_RECRUIT_TYPE_MASK);
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

    index = (recruitSiteType);
    if (quantity == 0) {
        EventWindow(-1, NORMAL_DIALOG_INFO, xRecruitEmpty[index], -1, 0, -1, 0, -1);
    } else {
        EventSound(cell->m_triggerType & MAP_TRIGGER_TYPE_MASK, quantity, &recruitSample);
        EventWindow(-1, NORMAL_DIALOG_CONFIRM, xRecruitBuy[index], -1, 0, -1, 0, -1);
        if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_YES) {
            ExpansionRecruitEvent(eventHero, creatureType, &quantity);
            packedValue = (quantity << EVENT_RECRUIT_COUNT_SHIFT) | (recruitSiteType);
            cell->m_objectMetadata = packedValue;
        }
    }
}

void advManager::ExpansionRecruitEvent(
    hero* eventHero, CreatureType creatureType, i16* availableCount
) {
    tag_message dialogMessage [[maybe_unused]];
    recruitUnit* recruitWindow = new recruitUnit(&eventHero->m_army, creatureType, availableCount);
    i32 dialogResult [[maybe_unused]];
    if (recruitWindow == NULL)
        MemError();
    gpExec->DoDialog(recruitWindow);
    delete recruitWindow;
}

void advManager::JailEvent(mapCell* cell, hero* eventHero, i32 x, i32 y) {
    SAMPLE2 eventSample = NULL;
    i32 heroId;
    hero* freedHero;

    heroId = cell->m_objectMetadata;
    if (gpGame->m_availableHeroes[heroId] != HERO_AVAILABILITY_JAILED) {
        NormalDialog(
            "Тюремщик сказал вам, что герой, томившийся тут в плену был освобожден его королем.",
            NORMAL_DIALOG_INFO
        );
        EraseObj(cell, x, y);
        return;
    }

    if (gpCurPlayer->m_heroCount >= PLAYER_HERO_CAPACITY) {
        NormalDialog(
            "У вас уже 8 героев. К сожалению, вам придется оставить томиться этого героя в темнице еще неопределенное время.",
            NORMAL_DIALOG_INFO
        );
        return;
    }

    EventSound(cell->m_triggerType & MAP_TRIGGER_TYPE_MASK, 0, &eventSample);
    NormalDialog(
        "С ослепительной отвагой вы ворвались в местную темницу и освободили героя, томящегося здесь в плену. В благодарность он поклялся служить вам.",
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

void advManager::EventSound(
    MapObjectType eventType,
    i32 eventData,
    SAMPLE2* outSample
) {
    const i32 treasureSnd = SOUND_TREASURE;
    const i32 expSound = SOUND_EXPERIENCE;
    const i32 moraleWav = SOUND_MORALE;
    const i32 luckSound = SOUND_LUCK;
    const i32 pickupSound = SOUND_PICKUP;
    const i32 mineSound [[maybe_unused]] = SOUND_MINE;
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

void advManager::EventWindow(
    i32 eventId,
    i32 buttons,
    const char* text,
    i32 type1,
    i32 value1,
    i32 type2,
    i32 value2,
    i32 type3
) {
    i32 unusedValue1 [[maybe_unused]];
    i32 unusedValue2 [[maybe_unused]];
    i32 eventDone [[maybe_unused]];
    i32 unusedValue3 [[maybe_unused]];
    i32 unusedValue4 [[maybe_unused]];
    i32 unusedValue5 [[maybe_unused]];
    i32 unusedValue6 [[maybe_unused]];
    char eventText[EVENT_TEXT_BUFFER_SIZE];
    i32 unusedStyle [[maybe_unused]];

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

    NormalDialog(eventText, buttons, -1, -1, (type1), value1, type2, value2, type3, 0);
}

i32 GiveArtifact(hero* eventHero, ArtifactType artifact, b32 checkEndGame, i8 artifactExtra) {
    i32 artifactSlot;

    for (artifactSlot = 0; artifactSlot < HERO_ARTIFACT_SLOT_COUNT; artifactSlot++) {
        if (eventHero->m_artifacts[artifactSlot] == ARTIFACT_NONE) {
            break;
        }
    }

    if (artifactSlot == HERO_ARTIFACT_SLOT_COUNT) {
        return (ARTIFACT_NONE);
    }

    eventHero->m_artifacts[artifactSlot] = artifact;
    eventHero->m_artifactExtra[artifactSlot] = artifactExtra;
    GiveTakeArtifactStat(eventHero, artifact, false);
    eventHero->CheckAnduranPieces(false);
    if (checkEndGame)
        CheckEndGame(END_GAME_FORCE_NONE, false);
    return artifactSlot;
}

ArtifactType advManager::GiveRandomArtifact(hero* eventHero) {
    ArtifactType artifactId =
        ArtifactType(gpGame->GetRandomArtifactId(ARTIFACT_LEVEL_RANDOM, true));

    if (artifactId == ARTIFACT_NONE)
        GiveResource(eventHero, RES_GOLD, EVENT_RANDOM_ARTIFACT_GOLD);
    else
        GiveArtifact(eventHero, artifactId, true);
    return artifactId;
}

i32 advManager::GiveExperience(hero* eventHero, i32 experience, i32 checkLevel) {
    i32 oldLevel;
    i32 unusedValue1 [[maybe_unused]];
    i32 unusedValue2 [[maybe_unused]];
    i32 newLevel;
    i32 levelGap [[maybe_unused]];

    oldLevel = eventHero->GetLevel(eventHero->m_experience);
    eventHero->m_level = oldLevel;
    eventHero->m_experience += experience;
    newLevel = eventHero->GetLevel(eventHero->m_experience);
    if (checkLevel)
        eventHero->CheckLevel();
    return newLevel - oldLevel;
}

void advManager::GiveResource(hero* eventHero, ResourceType resourceType, i32 amount) {
    if (resourceType >= RES_WOOD && resourceType <= RES_GOLD)
        gpGame->m_players[(eventHero->m_owner)].m_resources[(resourceType)] += amount;
    if (resourceType == RES_GOLD && gbHumanPlayer[(eventHero->m_owner)])
        CheckEndGame(END_GAME_FORCE_NONE, false);
}

void advManager::RecruitEvent(
    hero* eventHero, CreatureType creatureType, mapCell* cell
) {
    tag_message recruitMessage [[maybe_unused]];
    i16 availableCount = cell->m_objectMetadata;
    recruitUnit* recruitWindow =
        new recruitUnit(&eventHero->m_army, creatureType, &availableCount);
    i32 eventResult [[maybe_unused]];

    if (recruitWindow == NULL)
        MemError();
    gpExec->DoDialog(recruitWindow);
    delete recruitWindow;
    cell->m_objectMetadata = availableCount;
}

i32 advManager::SkeletonEvent(hero* eventHero, mapCell* cell, const char* text, i32 x, i32 y) {
    ArtifactType artifactId [[maybe_unused]];

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
                    (RES_GOLD),
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
                    (RES_GOLD),
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
                    (RES_GOLD),
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
                    (RES_GOLD),
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

i32 advManager::ZombieEvent(hero* eventHero, mapCell* cell, const char* text, i32 x, i32 y) {
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
                    (RES_GOLD),
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
                    (RES_GOLD),
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
                    (RES_GOLD),
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
                        (RES_GOLD),
                        ZOMBIE_EVENT_HUGE_GOLD,
                        MAP_EVENT_REWARD_ARTIFACT,
                        (artifactId),
                        -1
                    );
                else
                    EventWindow(
                        -1,
                        NORMAL_DIALOG_INFO,
                        text,
                        (RES_GOLD),
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

i32 advManager::GhostEvent(hero* eventHero, mapCell* cell, const char* text, i32 x, i32 y) {
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
                    (RES_GOLD),
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
                    (RES_GOLD),
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
                    (RES_GOLD),
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
                        (RES_GOLD),
                        GHOST_EVENT_HUGE_GOLD,
                        MAP_EVENT_REWARD_ARTIFACT,
                        (artifactId),
                        -1
                    );
                else
                    EventWindow(
                        -1,
                        NORMAL_DIALOG_INFO,
                        gText,
                        (RES_GOLD),
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

void advManager::HouseEvent(hero* eventHero, mapCell* cell) {
    HouseRecruitmentSite siteIndex = RECRUIT_ARCHER;
    CreatureType creatureTypes[(RECRUIT_SITE_COUNT)];

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
            (siteIndex) * EVENT_HOUSE_DIALOG_STRIDE + EVENT_EMPTY_DIALOG_BASE,
            NORMAL_DIALOG_INFO,
            "",
            -1,
            0,
            -1,
            0,
            -1
        );
    } else {
        creatureTypes[(RECRUIT_ARCHER)] = CREATURE_ARCHER;
        creatureTypes[(RECRUIT_GOBLIN)] = CREATURE_GOBLIN;
        creatureTypes[(RECRUIT_PEASANT)] = CREATURE_PEASANT;
        creatureTypes[(RECRUIT_DWARF)] = CREATURE_DWARF;
        creatureTypes[(RECRUIT_LOG_CABIN)] = CREATURE_DWARF;
        creatureTypes[(RECRUIT_TREE_HOUSE)] = CREATURE_SPRITE;
        creatureTypes[(RECRUIT_HALFLING)] = CREATURE_HALFLING;
        creatureTypes[(RECRUIT_WATCH_TOWER)] = CREATURE_ORC;
        creatureTypes[(RECRUIT_CAVE)] = CREATURE_CENTAUR;
        creatureTypes[(RECRUIT_EXCAVATION)] = CREATURE_SKELETON;

        EventWindow(
            (siteIndex) * EVENT_HOUSE_DIALOG_STRIDE + EVENT_RECRUIT_DIALOG_BASE,
            NORMAL_DIALOG_CONFIRM,
            "",
            -1,
            0,
            -1,
            0,
            -1
        );
        if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_YES) {
            if (eventHero->m_army.CanJoin(creatureTypes[(siteIndex)])) {
                eventHero->m_army.Add(
                    creatureTypes[(siteIndex)], cell->m_objectMetadata, -1
                );
                cell->m_objectMetadata = 0;
            } else {
                EventWindow(
                    (siteIndex) * EVENT_HOUSE_DIALOG_STRIDE + EVENT_ARMY_FULL_DIALOG_BASE,
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
    i32 combatUnused [[maybe_unused]];
    i32 lastCount;
    i32 groupCount;
    i32 stackCount;
    CreatureType savedTypes[ARMY_GROUP_SLOT_COUNT];
    CombatResult battleOutcome;
    i32 savedCounts[ARMY_GROUP_SLOT_COUNT];
    i32 stackIndex;
    i32 unusedValue [[maybe_unused]];

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
        && (((gMonsterDatabase[(monsterType)].attributes) & (MONSTER_FLAGS_SHOOTER))) == 0) {
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

void GiveTakeArtifactStat(hero* targetHero, ArtifactType artifact, b32 take) {
    i32 stats[HERO_PRIMARY_STAT_COUNT];
    i32 i;
    i32 maxSpellPoints;

    if (artifact == ARTIFACT_NONE)
        return;
    stats[(HERO_PRIMARY_ATTACK)] = 0;
    stats[(HERO_PRIMARY_DEFENSE)] = 0;
    stats[(HERO_PRIMARY_SPELL_POWER)] = 0;
    stats[(HERO_PRIMARY_KNOWLEDGE)] = 0;

    switch (artifact) {
        case ARTIFACT_ULTIMATE_BOOK:
            stats[(HERO_PRIMARY_KNOWLEDGE)] = 12;
            break;
        case ARTIFACT_ULTIMATE_SWORD:
            stats[(HERO_PRIMARY_ATTACK)] = 12;
            break;
        case ARTIFACT_ULTIMATE_CLOAK:
            stats[(HERO_PRIMARY_DEFENSE)] = 12;
            break;
        case ARTIFACT_ULTIMATE_WAND:
            stats[(HERO_PRIMARY_SPELL_POWER)] = 12;
            break;
        case ARTIFACT_ULTIMATE_SHIELD:
            stats[(HERO_PRIMARY_ATTACK)] = 6;
            stats[(HERO_PRIMARY_DEFENSE)] = 6;
            break;
        case ARTIFACT_ULTIMATE_STAFF:
            stats[(HERO_PRIMARY_SPELL_POWER)] = 6;
            stats[(HERO_PRIMARY_KNOWLEDGE)] = 6;
            break;
        case ARTIFACT_ULTIMATE_CROWN:
            stats[(HERO_PRIMARY_ATTACK)] = 4;
            stats[(HERO_PRIMARY_DEFENSE)] = 4;
            stats[(HERO_PRIMARY_SPELL_POWER)] = 4;
            stats[(HERO_PRIMARY_KNOWLEDGE)] = 4;
            break;
        case ARTIFACT_GOLDEN_GOOSE:
            break;
        case ARTIFACT_ARCANE_NECKLACE:
            stats[(HERO_PRIMARY_SPELL_POWER)] = 4;
            break;
        case ARTIFACT_CASTER_BRACELET:
            stats[(HERO_PRIMARY_SPELL_POWER)] = 2;
            break;
        case ARTIFACT_MAGE_RING:
            stats[(HERO_PRIMARY_SPELL_POWER)] = 2;
            break;
        case ARTIFACT_WITCHES_BROACH:
            stats[(HERO_PRIMARY_SPELL_POWER)] = 3;
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
            stats[(HERO_PRIMARY_ATTACK)] = 1;
            break;
        case ARTIFACT_ARMORED_GAUNTLETS:
            stats[(HERO_PRIMARY_DEFENSE)] = 1;
            break;
        case ARTIFACT_DEFENDER_HELM:
            stats[(HERO_PRIMARY_DEFENSE)] = 1;
            break;
        case ARTIFACT_GIANT_FLAIL:
            stats[(HERO_PRIMARY_ATTACK)] = 1;
            break;
        case ARTIFACT_BALLISTA:
            break;
        case ARTIFACT_STEALTH_SHIELD:
            stats[(HERO_PRIMARY_DEFENSE)] = 2;
            break;
        case ARTIFACT_DRAGON_SWORD:
            stats[(HERO_PRIMARY_ATTACK)] = 3;
            break;
        case ARTIFACT_POWER_AXE:
            stats[(HERO_PRIMARY_ATTACK)] = 2;
            break;
        case ARTIFACT_DIVINE_BREASTPLATE:
            stats[(HERO_PRIMARY_DEFENSE)] = 3;
            break;
        case ARTIFACT_MINOR_SCROLL:
            stats[(HERO_PRIMARY_KNOWLEDGE)] = 2;
            break;
        case ARTIFACT_MAJOR_SCROLL:
            stats[(HERO_PRIMARY_KNOWLEDGE)] = 3;
            break;
        case ARTIFACT_SUPERIOR_SCROLL:
            stats[(HERO_PRIMARY_KNOWLEDGE)] = 4;
            break;
        case ARTIFACT_FOREMOST_SCROLL:
            stats[(HERO_PRIMARY_KNOWLEDGE)] = 5;
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
            stats[(HERO_PRIMARY_ATTACK)] = 1;
            stats[(HERO_PRIMARY_DEFENSE)] = 1;
            break;
        case ARTIFACT_SPIKED_SHIELD:
            stats[(HERO_PRIMARY_ATTACK)] = 2;
            stats[(HERO_PRIMARY_DEFENSE)] = 2;
            break;
        case ARTIFACT_WHITE_PEARL:
            stats[(HERO_PRIMARY_SPELL_POWER)] = 1;
            stats[(HERO_PRIMARY_KNOWLEDGE)] = 1;
            break;
        case ARTIFACT_BLACK_PEARL:
            stats[(HERO_PRIMARY_SPELL_POWER)] = 2;
            stats[(HERO_PRIMARY_KNOWLEDGE)] = 2;
            break;
        case ARTIFACT_MAGIC_BOOK:
            break;
        case ARTIFACT_SPELL_SCROLL:
            break;
        case ARTIFACT_ARM_OF_MARTYR:
            stats[(HERO_PRIMARY_SPELL_POWER)] = 3;
            break;
        case ARTIFACT_BREASTPLATE_ANDURAN:
            stats[(HERO_PRIMARY_DEFENSE)] = 5;
            break;
        case ARTIFACT_BROACH_SHIELDING:
            stats[(HERO_PRIMARY_SPELL_POWER)] = -2;
            break;
        case ARTIFACT_BATTLE_GARB:
            stats[(HERO_PRIMARY_SPELL_POWER)] = 5;
            stats[(HERO_PRIMARY_DEFENSE)] = 5;
            stats[(HERO_PRIMARY_ATTACK)] = 5;
            break;
        case ARTIFACT_CRYSTAL_BALL:
            break;
        case ARTIFACT_HEART_FIRE:
            break;
        case ARTIFACT_HEART_ICE:
            break;
        case ARTIFACT_HELMET_ANDURAN:
            stats[(HERO_PRIMARY_SPELL_POWER)] = 5;
            break;
        case ARTIFACT_HOLY_HAMMER:
            stats[(HERO_PRIMARY_ATTACK)] = 5;
            break;
        case ARTIFACT_LEGENDARY_SCEPTER:
            stats[(HERO_PRIMARY_SPELL_POWER)] = 2;
            stats[(HERO_PRIMARY_ATTACK)] = 2;
            stats[(HERO_PRIMARY_DEFENSE)] = 2;
            stats[(HERO_PRIMARY_KNOWLEDGE)] = 2;
            break;
        case ARTIFACT_MASTHEAD:
            break;
        case ARTIFACT_SPHERE_NEGATION:
            break;
        case ARTIFACT_STAFF_WIZARDRY:
            stats[(HERO_PRIMARY_SPELL_POWER)] = 5;
            break;
        case ARTIFACT_SWORD_BREAKER:
            stats[(HERO_PRIMARY_DEFENSE)] = 4;
            stats[(HERO_PRIMARY_ATTACK)] = 1;
            break;
        case ARTIFACT_SWORD_ANDURAN:
            stats[(HERO_PRIMARY_ATTACK)] = 5;
            break;
        case ARTIFACT_SPADE_NECROMANCY:
            break;
        default:
            break;
    }

    for (i = 0; i < HERO_PRIMARY_STAT_COUNT; i++) {
        targetHero->m_primaryStats[i] += (take == EVENT_ARTIFACT_TAKE ? -1 : 1) * stats[i];
        if (i == (HERO_PRIMARY_KNOWLEDGE) && take == EVENT_ARTIFACT_TAKE) {
            maxSpellPoints = HERO_NORMAL_SPELL_POINTS(*targetHero);
            if (targetHero->m_spellPoints > maxSpellPoints)
                targetHero->m_spellPoints = maxSpellPoints;
        }
    }
}

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
                        if (gbThisNetHumanPlayer[(sourceHero->m_owner)]
                            || gbThisNetHumanPlayer[(destinationHero->m_owner)]) {
                            sprintf(
                                gText,
                                "Это %s! Едва вы добрались до артефакта, как предмет мистическим образом исчез."
                                    ,
                                gArtifactNames[(sourceHero->m_artifacts[sourceArtifactSlot])]
                            );
                            NormalDialog(
                                gText,
                                NORMAL_DIALOG_INFO,
                                -1,
                                -1,
                                NORMAL_DIALOG_ARTIFACT,
                                (sourceHero->m_artifacts[sourceArtifactSlot]),
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
                    sourceHero->m_artifactExtra[sourceArtifactSlot] = (ARTIFACT_NONE);
                    break;
                }
            }
        }
    }
    destinationHero->CheckAnduranPieces(false);
}

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

void advManager::DoWhirlpool(hero* eventHero) {
    i32 selectedSlot;
    i32 slotNo;
    i32 groupValues [[maybe_unused]][ARMY_GROUP_SLOT_COUNT];
    i32 lowestValue;
    i32 creatureValue;

    if (gbHumanPlayer[(eventHero->m_owner)] == 0)
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
                    * gMonsterDatabase[(eventHero->m_army.m_creatureTypes[slotNo])].fightValue;
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

void advManager::DoAIEvent(mapCell* cell, hero* eventHero, i32 x, i32 y) {
    i32 secondaryAmount [[maybe_unused]];
    i32 secondaryReward [[maybe_unused]];
    i32 primaryReward [[maybe_unused]];
    i32 primaryAmount [[maybe_unused]];
    i32 wellSpellPoints;
    i32 springSpellPoints;
    float defenderLoss;
    float attackerLoss;
    i32 heroLevel;
    i32 levelExperience;
    i32 creatureCosts[(RES_COUNT)];
    i32 spellPower;
    i32 adjacentMonster;
    b32 savedShowIt [[maybe_unused]];
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
    i32 eventWork [[maybe_unused]][(RES_COUNT)];
    i32 eraseObject;
    i32 battleResult;
    b32 creatureFlag;
    i32 oldPlayer;
    i32 purchaseCost;
    i32 battleWon;
    i32 pyramidBattleValue;
    i32 unusedResult [[maybe_unused]];
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
                for (index = (SPELL_SET_EARTH_GUARDIAN);
                     index < (SPELL_SET_WATER_GUARDIAN);
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
            if ((((eventHero->m_eventFlags) & (HERO_EVENT_WATERING_HOLE))) == 0) {
                eventHero->m_mobility += WATERING_HOLE_MOBILITY_BONUS;
                eventHero->m_remainingMobility += WATERING_HOLE_MOBILITY_BONUS;
                eventHero->m_eventFlags = eventHero->m_eventFlags | HERO_EVENT_WATERING_HOLE;
                ++eventHero->m_morale;
            }
            break;

        case MAP_OBJECT_BUOY:
            if ((((eventHero->m_eventFlags) & (HERO_EVENT_BUOY))) == 0) {
                eventHero->m_eventFlags = eventHero->m_eventFlags | HERO_EVENT_BUOY;
                ++eventHero->m_morale;
            }
            break;

        case MAP_OBJECT_FAERIE_RING:
            if ((((eventHero->m_eventFlags) & (HERO_EVENT_FAERIE_RING))) == 0) {
                eventHero->m_eventFlags = eventHero->m_eventFlags | HERO_EVENT_FAERIE_RING;
                ++eventHero->m_luck;
            }
            break;

        case MAP_OBJECT_IDOL:
            if ((((eventHero->m_eventFlags) & (HERO_EVENT_IDOL))) == 0) {
                eventHero->m_eventFlags = eventHero->m_eventFlags | HERO_EVENT_IDOL;
                ++eventHero->m_luck;
            }
            break;

        case MAP_OBJECT_FOUNTAIN:
            if ((((eventHero->m_eventFlags) & (HERO_EVENT_FOUNTAIN))) == 0) {
                eventHero->m_eventFlags = eventHero->m_eventFlags | HERO_EVENT_FOUNTAIN;
                ++eventHero->m_luck;
            }
            break;

        case MAP_OBJECT_OASIS:
            if ((((eventHero->m_eventFlags) & (HERO_EVENT_OASIS))) == 0) {
                eventHero->m_eventFlags = eventHero->m_eventFlags | HERO_EVENT_OASIS;
                ++eventHero->m_morale;
                eventHero->m_mobility += OASIS_MOBILITY_BONUS;
                eventHero->m_remainingMobility += OASIS_MOBILITY_BONUS;
            }
            break;

        case MAP_OBJECT_TEMPLE:
            if ((((eventHero->m_eventFlags) & (HERO_EVENT_TEMPLE))) == 0) {
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
                    ResourceType((cell->m_objectMetadata & (ARTIFACT_EVENT_MODE_MASK)) - 1),
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
                        ResourceType((cell->m_objectMetadata & (ARTIFACT_EVENT_MODE_MASK)) - 1),
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
                ResourceType(cell->m_objectMetadata & (CAMPFIRE_RESOURCE_MASK)),
                cell->m_objectMetadata >> CAMPFIRE_AMOUNT_SHIFT
            );
            eraseObject = 1;
            break;

        case MAP_OBJECT_FORT:
            if ((eventHero->m_fortVisits & (1U << cell->m_objectMetadata)) == 0) {
                ++eventHero->m_primaryStats[(HERO_PRIMARY_DEFENSE)];
                eventHero->m_fortVisits |= 1U << cell->m_objectMetadata;
            }
            break;

        case MAP_OBJECT_XANADU:
            if ((eventHero->m_xanaduVisits & (1U << cell->m_objectMetadata)) == 0
                && eventHero->m_level
                           + (eventHero->m_secondarySkills[(HERO_SKILL_DIPLOMACY)])
                                 * XANADU_DIPLOMACY_MULTIPLIER
                       >= XANADU_ADMISSION_LEVEL) {
                ++eventHero->m_primaryStats[(HERO_PRIMARY_ATTACK)];
                ++eventHero->m_primaryStats[(HERO_PRIMARY_DEFENSE)];
                ++eventHero->m_primaryStats[(HERO_PRIMARY_KNOWLEDGE)];
                ++eventHero->m_primaryStats[(HERO_PRIMARY_SPELL_POWER)];
                eventHero->m_xanaduVisits |= 1U << cell->m_objectMetadata;
            }
            break;

        case MAP_OBJECT_STANDING_STONES:
            if ((eventHero->m_standingStoneVisits & (1U << cell->m_objectMetadata)) == 0) {
                ++eventHero->m_primaryStats[(HERO_PRIMARY_SPELL_POWER)];
                eventHero->m_standingStoneVisits |= 1U << cell->m_objectMetadata;
            }
            break;

        case MAP_OBJECT_WITCH_DOCTOR_HUT:
            if ((eventHero->m_witchDoctorVisits & (1U << cell->m_objectMetadata)) == 0) {
                ++eventHero->m_primaryStats[(HERO_PRIMARY_KNOWLEDGE)];
                eventHero->m_witchDoctorVisits |= 1U << cell->m_objectMetadata;
            }
            break;

        case MAP_OBJECT_MERCENARY_CAMP:
            if ((eventHero->m_mercenaryCampVisits & (1U << cell->m_objectMetadata)) == 0) {
                ++eventHero->m_primaryStats[(HERO_PRIMARY_ATTACK)];
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
                        for (index = 0; index < (RES_COUNT); ++index)
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
                        if (gpCurPlayer->m_resources[(RES_GOLD)] > TREE_KNOWLEDGE_GOLD_COST) {
                            gpCurPlayer->m_resources[(RES_GOLD)] -= TREE_KNOWLEDGE_GOLD_COST;
                            GiveExperience(eventHero, levelExperience, 1);
                            eventHero->m_treeKnowledgeVisits |=
                                1U << (cell->m_objectMetadata & TREE_KNOWLEDGE_VISIT_INDEX_MASK);
                            eventHero->CheckLevel();
                        }
                        break;
                    case TREE_KNOWLEDGE_GEMS:
                        if (gpCurPlayer->m_resources[(RES_GEMS)] > TREE_KNOWLEDGE_GEM_COST) {
                            gpCurPlayer->m_resources[(RES_GEMS)] -= TREE_KNOWLEDGE_GEM_COST;
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
                && (gsSpellInfo[cell->m_objectMetadata - 1].level)
                       <= (eventHero->m_secondarySkills[(HERO_SKILL_WISDOM)])
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
                    if ((gpGame->m_worldMap.GetCell(exitX, exitY)->m_triggerType).value()
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
                        if ((gpGame->m_worldMap.GetCell(exitX, exitY)->m_triggerType).value()
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
                    if (eventHero->m_secondarySkills[(HERO_SKILL_WISDOM)]
                        != HERO_SKILL_LEVEL_NONE)
                        goto artifactPickup;
                    break;
                case ARTIFACT_EVENT_MODE_LEADERSHIP:
                    if (eventHero->m_secondarySkills[(HERO_SKILL_LEADERSHIP)]
                        != HERO_SKILL_LEVEL_NONE)
                        goto artifactPickup;
                    break;
                case ARTIFACT_EVENT_MODE_PICKUP:
                artifactPickup:
                    for (index = 0; index < (RES_COUNT); ++index) {
                        if (gpCurPlayer->m_resources[index] < 0)
                            gpCurPlayer->m_resources[index] = 0;
                    }
                    GiveArtifact(eventHero, eventArtifact, true);
                    eraseObject = 1;
                    break;
                case ARTIFACT_EVENT_MODE_GOLD:
                    if (gpPhilAI->NetValueOfArtifact((eventArtifact), ARTIFACT_EVENT_GOLD_COST, 0, 0)) {
                        gpGame->m_players[(eventHero->m_owner)].m_resources[(RES_GOLD)] -=
                            ARTIFACT_EVENT_GOLD_COST;
                        goto artifactPickup;
                    }
                    break;
                case ARTIFACT_EVENT_MODE_RESOURCE_3:
                    if (gpPhilAI->NetValueOfArtifact(
                            (eventArtifact),
                            ARTIFACT_EVENT_RESOURCE_3_GOLD_COST,
                            artifactResource,
                            ARTIFACT_EVENT_RESOURCE_3_AMOUNT
                        )) {
                        gpGame->m_players[(eventHero->m_owner)].m_resources[(RES_GOLD)] -=
                            ARTIFACT_EVENT_RESOURCE_3_GOLD_COST;
                        gpGame->m_players[(eventHero->m_owner)].m_resources[(artifactResource)] -=
                            ARTIFACT_EVENT_RESOURCE_3_AMOUNT;
                        goto artifactPickup;
                    }
                    break;
                case ARTIFACT_EVENT_MODE_RESOURCE_5:
                    if (gpPhilAI->NetValueOfArtifact(
                            (eventArtifact),
                            ARTIFACT_EVENT_RESOURCE_5_GOLD_COST,
                            artifactResource,
                            ARTIFACT_EVENT_RESOURCE_5_AMOUNT
                        )) {
                        gpGame->m_players[(eventHero->m_owner)].m_resources[(RES_GOLD)] -=
                            ARTIFACT_EVENT_RESOURCE_5_GOLD_COST;
                        gpGame->m_players[(eventHero->m_owner)].m_resources[(artifactResource)] -=
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

            if (gbHumanPlayer[(otherHero->m_owner)] == 0) {
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
                    if (gpGame->m_players[(eventHero->m_owner)].m_resources[(RES_GOLD)]
                        >= DAEMON_GOLD) {
                        if (gpPhilAI->ChooseToPayRansomOnHero(DAEMON_GOLD)) {
                            gpGame->m_players[(eventHero->m_owner)].m_resources[(RES_GOLD)] -=
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
                * ((((gsSpellInfo[index].attributes) & (SPELL_INFO_ATTRIBUTE_POWER)))
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
                gMineCharacteristics[(RES_GOLD)] * gafAITurnCostResource[(RES_GOLD)]
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
            if ((((eventHero->m_eventFlags) & (HERO_EVENT_MAGIC_WELL))) == 0) {
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
                for (index = 0; index < (RES_COUNT); ++index) {
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

i32 advManager::BarrierAIEvent(mapCell* cell, hero*) {
    i32 color = cell->m_objectMetadata;
    color &= EVENT_BARRIER_COLOR_MASK;
    if (PLAYER_HAS_VISITED_TENT(*gpCurPlayer, color))
        return 1;
    else
        return 0;
    return 0;
}

void advManager::PasswordAIEvent(mapCell* cell, hero*) {
    i32 unusedPassword [[maybe_unused]][1];
    i32 color = cell->m_objectMetadata;
    color &= EVENT_BARRIER_COLOR_MASK;
    gpCurPlayer->m_barrierTents |= (1 << color);
}

void advManager::GenericSiteAIEvent(mapCell* cell, hero* eventHero) {
    i32 siteLevel [[maybe_unused]];
    HeroPrimaryStat primaryStat;
    i32 artifactIndex;
    GenericSiteType siteType;
    i32 unusedPair [[maybe_unused]][2];
    i32 cursedArtifactCount;
    i32 quantity;
    i32 unusedTriple [[maybe_unused]][3];
    CreatureType creatureType;
    i32 armyValue;

    cursedArtifactCount = 0;
    siteType = static_cast<GenericSiteType>(cell->m_objectMetadata);
    siteType = static_cast<GenericSiteType>((siteType) & GENERIC_SITE_TYPE_MASK);
    siteLevel = cell->m_objectMetadata;
    siteLevel >>= GENERIC_SITE_LEVEL_SHIFT;

    switch (siteType) {
        case GENERIC_SITE_ALCHEMIST_TOWER:
            for (artifactIndex = 0; artifactIndex < HERO_ARTIFACT_SLOT_COUNT; artifactIndex++) {
                if (IsCursedItem(eventHero->m_artifacts[artifactIndex]))
                    cursedArtifactCount++;
            }
            if (cursedArtifactCount != 0
                && gpCurPlayer->m_resources[(RES_GOLD)]
                       >= EVENT_CURSED_ARTIFACT_GOLD_THRESHOLD) {
                for (artifactIndex = 0; artifactIndex < HERO_ARTIFACT_SLOT_COUNT;
                     artifactIndex++) {
                    if (IsCursedItem(eventHero->m_artifacts[artifactIndex]))
                        eventHero->m_artifacts[artifactIndex] = ARTIFACT_NONE;
                }
                gpCurPlayer->m_resources[(RES_GOLD)] -= EVENT_CURSED_ARTIFACT_COST;
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
                eventHero->m_primaryStats[(primaryStat)]++;
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
                                gMonsterDatabase[(creatureType)].hitPoints
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

void advManager::RecruitSiteAIEvent(mapCell* cell, hero* eventHero) {
    i32 replacementSlot;
    CreatureType monsterType;
    i32 buyCount;
    u32 packedSite;
    i32 resource;
    u32 recruitType;
    i32 cost[(RES_COUNT)];
    i32 totalValue;
    i16 quantity;

    recruitType = cell->m_objectMetadata;
    recruitType &= (EVENT_RECRUIT_TYPE_MASK);
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
            packedSite = (quantity << EVENT_RECRUIT_COUNT_SHIFT) | (recruitType);
            cell->m_objectMetadata = packedSite;
            GetMonsterCost(monsterType, cost);
            for (resource = 0; resource < RES_COUNT; resource++) {
                gpCurPlayer->m_resources[(resource)] -=
                    buyCount * cost[(resource)];
            }
        }
    }
}

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
    creatureCount = cell->m_objectMetadata & (MAP_MONSTER_COUNT_MASK);
    armyRatio = static_cast<double>(
                    gpPhilAI->FightValueOfStack(&eventHero->m_army, eventHero, 0)
                )
                / (creatureCount * gMonsterDatabase[(monsterType)].fightValue);

    if (gbInCampaign
        && ((gpGame->m_campaignAwards[(CAMPAIGN_AWARD_DWARVEN_ALLIANCE)]
             && (monsterType == CREATURE_DWARF || monsterType == CREATURE_BATTLE_DWARF))
            || (gpGame->m_campaignAwards[(CAMPAIGN_AWARD_OGRE_ALLIANCE)]
                && (monsterType == CREATURE_OGRE || monsterType == CREATURE_OGRE_LORD))
            || (gpGame->m_campaignAwards[(CAMPAIGN_AWARD_DRAGON_ALLIANCE)]
                && (monsterType == CREATURE_GREEN_DRAGON || monsterType == CREATURE_RED_DRAGON
                    || monsterType == CREATURE_BLACK_DRAGON)))) {
        if (!eventHero->m_army.CanJoin(monsterType)) {
            if (monsterType == CREATURE_DWARF || monsterType == CREATURE_BATTLE_DWARF)
                NormalDialog(
                    "Гномы приветствую вас: \"Друг Роланда - наш друг. Вы можете пройти.\"",
                    NORMAL_DIALOG_INFO
                );
            else if (monsterType == CREATURE_OGRE || monsterType == CREATURE_OGRE_LORD)
                NormalDialog(
                    "Огры признали вас: \"Союзники Арчибальда могут пройти.\"",
                    NORMAL_DIALOG_INFO
                );
            else
                NormalDialog(
                    "Драконы, посмотрев на вас, промолвили: \"Наш альянс с Арчибальдом вынуждает нас присоединиться к вам.  К несчастью, у вас нет для нас места. Какая жалость!\"  И быстро улетели.",
                    NORMAL_DIALOG_INFO
                );
            *handled = 1;
        } else {
            if (monsterType == CREATURE_DWARF || monsterType == CREATURE_BATTLE_DWARF)
                NormalDialog(
                    "Гномы признали своих союзников и рады присоединиться к ним.",
                    NORMAL_DIALOG_INFO
                );
            else if (monsterType == CREATURE_OGRE || monsterType == CREATURE_OGRE_LORD)
                NormalDialog(
                    "Огры признали нас, как врагов гномов и рады присоединиться к вам. ",
                    NORMAL_DIALOG_INFO
                );
            else
                NormalDialog(
                    "Горделивые драконы вынуждены были признать в вас своих 'союзников' и присоединиться к вам.",
                    NORMAL_DIALOG_INFO
                );
            eventHero->m_army.Add(monsterType, creatureCount, -1);
            *handled = 1;
        }
        return;
    }

    if (gbInCampaign && gpGame->m_campaignAwards[(CAMPAIGN_AWARD_DWARFBANE)]
        && (monsterType == CREATURE_DWARF || monsterType == CREATURE_BATTLE_DWARF)) {
        NormalDialog(
            "\"Враги гномов! Бегите, если жизнь дорога.\"",
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
                "Едва вы подошли к отряду эльфов, как их вожак призвал свой отряд к оружию. Он сказал им: \"Кто из вас достаточно смел, чтобы вступить в армию нашего мужественного союзника?\" Весь отряд единодушно вступил в вашу армию.",
                NORMAL_DIALOG_INFO
            );
            eventHero->m_army.Add(monsterType, creatureCount, -1);
        } else {
            NormalDialog(
                "Эльфы насторожились при вашем приближении. Их вожак обратился к вам со словами: \"Мы не можем принять участие в ваших делах, союзник! Проходи, наша победа будет вашей.\"",
                NORMAL_DIALOG_INFO
            );
        }
        return;
    }

    if (eventHero->m_army.CanJoin(monsterType) && armyRatio > 2.0
        && !eventHero->HasArtifact(ARTIFACT_HIDEOUS_MASK) && monsterType != CREATURE_GHOST
        && !IS_ELEMENTAL_CREATURE(monsterType)) {
        if (forceJoin) {
            sprintf(gText, gEventText[EVENT_TEXT_FOLLOWERS], gArmyNamesPlural[(monsterType)]);
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
        } else if (eventHero->m_secondarySkills[(HERO_SKILL_DIPLOMACY)]
                   != HERO_SKILL_LEVEL_NONE) {
            if (eventHero->m_secondarySkills[(HERO_SKILL_DIPLOMACY)]
                == HERO_SKILL_LEVEL_EXPERT)
                numJoining = creatureCount;
            else if (eventHero->m_secondarySkills[(HERO_SKILL_DIPLOMACY)]
                     == HERO_SKILL_LEVEL_ADVANCED)
                numJoining = creatureCount / MONSTER_DIPLOMACY_ADVANCED_JOIN_DIVISOR;
            else
                numJoining = creatureCount / MONSTER_DIPLOMACY_BASIC_JOIN_DIVISOR;
            if (!numJoining)
                numJoining = 1;

            joiningCost = gMonsterDatabase[(monsterType)].cost * creatureCount;
            if (joiningCost > gpGame->m_players[(eventHero->m_owner)].m_resources[(RES_GOLD)]) {
                if (armyRatio
                    > 5.0  )
                    goto monstersFlee;
                else
                    goto fightMonsters;
            }

            if (creatureCount == 1) {
                sprintf(
                    gText,
                    "%s находится под впечатлением вашей дипломатичности, и предлагает свои услуги вашему войску за %d золотых. Вы согласны?",
                    gArmyNames[(monsterType)],
                    joiningCost
                );
            } else {
                sprintf(
                    gText,
                    "Воины покорены вашим шармом и выдвинули предложение:\n\n"
                );
                if (numJoining == creatureCount)
                    sprintf(
                        monsterText,
                        "Отряд из %d %s вступят в ряды вашей армии за %d золотых. Вы согласны принять их?",
                        creatureCount,
                        gArmyNamesPlural[(monsterType)],
                        joiningCost
                    );
                else
                    sprintf(
                        monsterText,
                        "%d из %d %s вступят в ряды вашей армии, а оставшиеся оставят вас в покое за %d золотых. Вы согласны?",
                        numJoining,
                        creatureCount,
                        gArmyNamesPlural[(monsterType)],
                        joiningCost
                    );
                strcat(gText, monsterText);
            }

            NormalDialog(
                gText,
                NORMAL_DIALOG_CONFIRM,
                -1,
                -1,
                (RES_GOLD),
                joiningCost,
                -1,
                0,
                -1,
                0
            );
            if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_YES) {
                eventHero->m_army.Add(monsterType, numJoining, -1);
                *handled = 1;
                gpGame->m_players[(eventHero->m_owner)].m_resources[(RES_GOLD)] -= joiningCost;
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
            "Группа %s, страшась мощи вашего воинства, бросилась врассыпную. Соизволите изловить их и заставить драться?",
            gArmyNamesPlural[(monsterType)]
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
    creatureCount[MONSTER_COMBAT_REMAINING_COUNT] = cell->m_objectMetadata & (MAP_MONSTER_COUNT_MASK);
    forceJoin = cell->m_objectMetadata & MONSTER_JOIN_FORCED;
    armyRatio = static_cast<double>(
                    gpPhilAI->FightValueOfStack(&eventHero->m_army, eventHero, 0)
                )
                / (creatureCount[MONSTER_COMBAT_REMAINING_COUNT]
                    * gMonsterDatabase[(monsterType)].fightValue);

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
            if (eventHero->m_secondarySkills[(HERO_SKILL_DIPLOMACY)]
                != HERO_SKILL_LEVEL_NONE) {
                if (eventHero->m_secondarySkills[(HERO_SKILL_DIPLOMACY)]
                    == HERO_SKILL_LEVEL_EXPERT)
                    joiningCount = creatureCount[MONSTER_COMBAT_REMAINING_COUNT];
                else if (eventHero->m_secondarySkills[(HERO_SKILL_DIPLOMACY)]
                         == HERO_SKILL_LEVEL_ADVANCED)
                    joiningCount = creatureCount[MONSTER_COMBAT_REMAINING_COUNT]
                                   / MONSTER_DIPLOMACY_ADVANCED_JOIN_DIVISOR;
                else
                    joiningCount = creatureCount[MONSTER_COMBAT_REMAINING_COUNT]
                                   / MONSTER_DIPLOMACY_BASIC_JOIN_DIVISOR;
                if (!joiningCount)
                    joiningCount = 1;

                joiningCost = static_cast<i32>(
                    gMonsterDatabase[(monsterType)].cost
                    * creatureCount[MONSTER_COMBAT_REMAINING_COUNT]
                    * 0.75
                );
                if (joiningCost
                    > gpGame->m_players[(eventHero->m_owner)].m_resources[(RES_GOLD)]) {
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
                    gpGame->m_players[(eventHero->m_owner)].m_resources[(RES_GOLD)] -= joiningCost;
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
                * gMonsterDatabase[(monsterType)].hitPoints,
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
                    * 0.1
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
                + (creatureCount[MONSTER_COMBAT_REMAINING_COUNT] & (MAP_MONSTER_COUNT_MASK));
        }
    }
}

i32 advManager::DoNetCombat(char* packet) {
    hero* secondHro;
    i32 randSeed;
    i32 firstSide;
    town* battleTown;
    i8 combatRes;
    i32 initCombatX;
    i32 netUnused [[maybe_unused]];
    armyGroup* secondArmy;
    i32 battleX;
    armyGroup* troopFirst;
    i32 otherPlr;
    i32 setupBattleY;
    hero* firstHero;
    i32 combatY;
    i32 outcome [[maybe_unused]];

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
    i8 combatResult;
    b32 savedShowIt;
    i32 secondPlayer;
    i32 savedPlayer;
    i32 unusedCombat [[maybe_unused]];
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
            iCombatControlNetPos[(COMBAT_ATTACKER_SIDE)] = giThisNetPos;
            iCombatControlNetPos[(COMBAT_DEFENDER_SIDE)] = gbGamePosToNetPos[secondPlayer];
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
                "%s, ваш %s атакован!"
                    ,
                cPlayerNames[secondPlayer],
                combatTown
                    ? "город"
                    : "герой"
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
    CombatResult combatResult,
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
        firstHero ? gpGame->m_players[(firstHero->m_owner)].m_resources[(RES_GOLD)] : 0;
    buffer->secondOwner = secondHero ? secondHero->m_owner : -1;
    buffer->secondGold =
        secondHero ? gpGame->m_players[(secondHero->m_owner)].m_resources[(RES_GOLD)] : 0;
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
    i8* combatResult,
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
        gpGame->m_players[firstHeroOwner].m_resources[(RES_GOLD)] =
            EVENTS_REMOTE_COMBAT(packet)->firstGold;
    secondOwner = EVENTS_REMOTE_COMBAT(packet)->secondOwner;
    if (secondOwner > 0)
        gpGame->m_players[secondOwner].m_resources[(RES_GOLD)] =
            EVENTS_REMOTE_COMBAT(packet)->secondGold;

    *firstArmy = static_cast<armyGroup*>(H2_ALLOC(sizeof(armyGroup)));
    memcpy(*firstArmy, &EVENTS_REMOTE_COMBAT(packet)->firstArmy, sizeof(armyGroup));
    *secondArmy = static_cast<armyGroup*>(H2_ALLOC(sizeof(armyGroup)));
    memcpy(*secondArmy, &EVENTS_REMOTE_COMBAT(packet)->secondArmy, sizeof(armyGroup));
    if (townPresent) {
        *combatTown = static_cast<town*>(H2_ALLOC(sizeof(town)));
        memcpy(*combatTown, &EVENTS_REMOTE_COMBAT(packet)->combatTown, sizeof(town));
    }

    iCombatControlNetPos[(COMBAT_ATTACKER_SIDE)] = *remotePlayer;
    iCombatControlNetPos[(COMBAT_DEFENDER_SIDE)] = giThisNetPos;
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
                "Ошибка получения информации. Продолжать?",
                NORMAL_DIALOG_CONFIRM
            );
            if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_YES)
                lastPacketTime = KBTickCount();
            else
                ShutDown("Игра прекращена.");
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

CombatResult advManager::AutoResolveCombat(
    i32 x,
    i32 y,
    hero* firstHero,
    armyGroup* firstArmy,
    town* combatTown,
    hero* secondHero,
    armyGroup* secondArmy,
    i32 setupCombatX [[maybe_unused]],
    i32 setupCombatY [[maybe_unused]],
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

i32 RiddleStringsEqual(const char* answer, const char* expected) {
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

b32 gbNoShowCombat = false;
