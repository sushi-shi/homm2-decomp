#include <Ints.h>
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
        RIDDLE_EXPECTED_BUFFER_SIZE = RIDDLE_PREFIX_LENGTH + 1,
        RIDDLE_ANSWER_BUFFER_SIZE = 8
    } RiddleComparisonConstant;

    typedef enum BarrierEventConstant {
        COLOR_MASK = 7,
        PASSWORD_SHIFT = 3,
        INPUT_LENGTH = 14,
        INPUT_BUFFER_SIZE = 16
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
        MONSTER_ARMY_SLOTS = 5,
        MONSTER_RANDOM_MAX = 100,
        MONSTER_REDUCED_STACK_CHANCE = 25,
        MONSTER_REDUCED_STACK_COUNT = 3,
        MONSTER_FOUR_STACK_THRESHOLD = 50,
        MONSTER_FOUR_STACK_COUNT = 4,
        MONSTER_UPGRADE_CHANCE = 50
    } CombatMonsterEventConstant;

    typedef enum FizzleCenterConstant {
        ARTIFACT_PICKUP_SOUND_FIRST = 1,
        ARTIFACT_PICKUP_SOUND_LAST = 7
    } FizzleCenterConstant;

    typedef enum FlotsamConstant {
        FLOTSAM_WOOD_AMOUNT = 5,
        FLOTSAM_GOLD_AMOUNT = 200,
        FLOTSAM_LARGE_WOOD_AMOUNT = 10,
        FLOTSAM_LARGE_GOLD_AMOUNT = 500
    } FlotsamConstant;

    typedef enum DoEventConstant {
        ARTESIAN_SPRING_MANA_MULTIPLIER = 2,
        XANADU_DIPLOMACY_MULTIPLIER = 2,
        TEMPLE_MORALE_BONUS = 2,
        PYRAMID_LUCK_PENALTY = 2,
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

    typedef enum Cp1251Letter {
        CP1251_CAPITAL_YO = 0xa8,
        CP1251_SMALL_YO = 0xb8,
        CP1251_CAPITAL_A = 0xc0,
        CP1251_CAPITAL_YA = 0xdf,
        CP1251_CASE_STEP = 0x20
    } Cp1251Letter;


    inline char ToLowerCp1251(u8 letter) {
        char smallLetter;

        if (letter >= 'A' && letter <= 'Z')
            smallLetter = letter + CP1251_CASE_STEP;
        else if (letter >= CP1251_CAPITAL_A && letter <= CP1251_CAPITAL_YA)
            smallLetter = letter + CP1251_CASE_STEP;
        else if (letter == CP1251_CAPITAL_YO)
            smallLetter = CP1251_SMALL_YO;
        else
            smallLetter = letter;
        return smallLetter;
    }

}

#define TRADING_POST_EFFICIENCY 0.2f

#define EVENTS_REMOTE_MESSAGE(buffer) (reinterpret_cast<RemoteMessage*>(buffer))
#define EVENTS_REMOTE_COMBAT(buffer)                                                               \
    (reinterpret_cast<combatRemoteData*>(EVENTS_REMOTE_MESSAGE(buffer)->payload))
#define EVENTS_REMOTE_HERO(buffer)                                                                 \
    (reinterpret_cast<combatRemoteHeroFragment*>(EVENTS_REMOTE_MESSAGE(buffer)->payload))
#define EVENTS_HERO_BUFFER(buffer) (reinterpret_cast<combatRemoteHeroFragment*>(buffer))

void advManager::DoEvent(mapCell* cell, i32 x, i32 y) {
    char eventText_b[EVENT_TEXT_BUFFER_SIZE];
    mapCell* skeletonCell_e;
    mapCell* zombieCell_o;
    town* occupiedTown_b;
    i32 heroLevel_e;
    i32 levelExperience_g;
    i32 dragonFactor_i;
    CombatResult mineCombatResult_b;
    i32 adjacentMonster_j;
    i32 wellSpellPoints_o;
    i32 springSpellPoints_j;
    i32 secondaryAmount_j;
    i32 secondaryReward_k;
    i32 primaryReward_e;
    i32 primaryAmount_j;
    b32 correctAnswer_e;
    boatRecord* boat_k;
    i32 teleportCount_e;
    ResourceType resourceType_a;
    heroWindow* oracleWindow_o;
    i32 guardedCount_i;
    i32 teleportY_e;
    i32 teleportX_e;
    ArtifactType artifact_g;
    i32 artifactResourceType_k;
    CreatureType monsterType_f;
    hero* otherHero_e;
    CombatResult heroCombatResult_h;
    mapEventExtra* eventExtra_o;
    i32 resourceAmount_o;
    CreatureType guardedMonster_c;
    tag_message oracleMessage_o;
    MapObjectType eventType_g;
    tag_message unusedEventMessage [[maybe_unused]];
    i32 eraseObject_l;
    char sphinxAnswer_a[SPHINX_ANSWER_BUFFER_SIZE];
    CreatureType secondUpgrade1;
    SAMPLE2 playedSample3;
    CreatureType firstUpgrade_e;
    b32 fizzleType_k;
    i32 eventValue1;
    signEventExtra* signExtra_k;
    CreatureType thirdUpgrade_f;
    hero* eventHero2;
    SAMPLE2 eventSample_f;

    eventHero2 = &gpGame->m_heroRecs[gpCurPlayer->m_currentHero];
    eventType_g = cell->m_triggerType & MAP_TRIGGER_TYPE_MASK;
    eraseObject_l = 0;
    fizzleType_k = false;
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
                    "{Волшебный сад}\n\nВы забрели "
                        "в волшебный сад, из тех, где "
                        "так обожают резвиться лепреконы "
                        "с феями, но сегодня здесь никого."
                        " Может, на следующей неделе "
                        "вам повезет больше.",
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
                    "{Волшебный сад}\n\nВы поймали "
                        "лепрекона, так беспечно заснувшего "
                        "под шляпкой волшебного гриба."
                        " В обмен на свободу он рассказал "
                        "вам, где лежит горшочек со "
                        "всякими ценными вещицами.",
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
            eventExtra_o = reinterpret_cast<mapEventExtra*>(ppMapExtra[cell->m_objectMetadata]);
            if (!eventExtra_o->active) {
                NormalDialog(
                    "{Сфинкс}\n\nВы подошли к огромному "
                        "Сфинксу, но он даже не шелохнулся.",
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
            } else {
                sprintf(
                    gText,
                    "\"Есть у меня загадка для тебя,"
                        "\"сказал Сфинкс. \"Ответишь "
                        "верно - получишь награду. Ошибешься "
                        "- я сожру тебя. Принимаешь ли "
                        "ты мой вызов?\""
                );
                NormalDialog(gText, NORMAL_DIALOG_CONFIRM, -1, -1, -1, 0, -1, 0, -1, 0);
                if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
                    sprintf(
                        gText,
                        "Сфинкс загадал вам следующую "
                            "загадку::\n\n'%s'\n\nВаш ответ?",
                        eventExtra_o->riddle
                    );
                    GetDataEntry(gText, sphinxAnswer_a, SPHINX_INPUT_LENGTH, NULL, 0, 1);
                    correctAnswer_e = false;
                    for (eventValue1 = 0; eventValue1 < eventExtra_o->answerCount; eventValue1++) {
                        if (RiddleStringsEqual(sphinxAnswer_a, eventExtra_o->answers[eventValue1]))
                            correctAnswer_e = true;
                    }

                    if (correctAnswer_e) {
                        primaryReward_e = MAP_EVENT_REWARD_NONE;
                        primaryAmount_j = 0;
                        secondaryReward_k = MAP_EVENT_REWARD_NONE;
                        secondaryAmount_j = 0;
                        for (eventValue1 = 0; eventValue1 < SPHINX_RESOURCE_COUNT;
                             eventValue1++) {
                            gpGame->m_players[giCurPlayer].m_resources[eventValue1] +=
                                eventExtra_o->resources[eventValue1];
                            if (gpGame->m_players[giCurPlayer].m_resources[eventValue1] < 0)
                                gpGame->m_players[giCurPlayer].m_resources[eventValue1] = 0;
                            if (eventExtra_o->resources[eventValue1] != 0) {
                                if (primaryReward_e != MAP_EVENT_REWARD_NONE) {
                                    secondaryReward_k = primaryReward_e;
                                    secondaryAmount_j = primaryAmount_j;
                                }
                                primaryReward_e = eventValue1;
                                primaryAmount_j = eventExtra_o->resources[eventValue1];
                            }
                        }

                        if (eventExtra_o->artifact != MAP_EVENT_REWARD_NONE
                            && eventHero2->NumArtifacts() < EVENT_ARTIFACT_CAPACITY) {
                            GiveArtifact(eventHero2, ArtifactType(eventExtra_o->artifact), 1, -1);
                            if (primaryReward_e != MAP_EVENT_REWARD_NONE) {
                                secondaryReward_k = primaryReward_e;
                                secondaryAmount_j = primaryAmount_j;
                            }
                            primaryReward_e = MAP_EVENT_REWARD_ARTIFACT;
                            primaryAmount_j = eventExtra_o->artifact;
                        }

                        NormalDialog(
                            "Несколько разочарованно "
                                "Сфинкс промолвил. Ты дал верный "
                                "ответ, вот твоя награда. А теперь "
                                "убирайся.",
                            NORMAL_DIALOG_INFO,
                            -1,
                            -1,
                            primaryReward_e,
                            primaryAmount_j,
                            secondaryReward_k,
                            secondaryAmount_j,
                            -1,
                            0
                        );
                        eventExtra_o->active = 0;
                    } else {
                        NormalDialog(
                            "\"Твоя догадка ошибочна,\" сказал,"
                                " улыбаясь, Сфинкс. Ухмыляющийся "
                                "Сфинкс повалил тебя на землю "
                                "и мир окутала непроглядная "
                                "тьма.",
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
                "{Обзорная башня}\n\nС вершины "
                    "обзорной башни вы смогли "
                    "разглядеть дальние земли.",
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
            firstUpgrade_e = secondUpgrade1 = thirdUpgrade_f = CREATURE_NONE;
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
                    thirdUpgrade_f = CREATURE_DWARF;
            }

            if (firstUpgrade_e != CREATURE_NONE) {
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                eventHero2->UpgradeCreatures(CREATURE_OGRE, CREATURE_OGRE_LORD);
                eventHero2->UpgradeCreatures(CREATURE_ORC, CREATURE_ORC_CHIEF);
                eventHero2->UpgradeCreatures(CREATURE_DWARF, CREATURE_BATTLE_DWARF);
                if (thirdUpgrade_f != CREATURE_NONE) {
                    sprintf(
                        gText,
                        "{Форт на холме}\n\nДля всех %s, %s"
                            " и %s вашего войска проходят "
                            "обучение у мастеров боя из "
                            "этого форта. Теперь в вашей "
                            "армии имеются отряды %s, %s, и "
                            "%s",
                        gArmyNamesPlural[(firstUpgrade_e)],
                        gArmyNamesPlural[(secondUpgrade1)],
                        gArmyNamesPlural[(thirdUpgrade_f)],
                        gArmyNamesPlural[(firstUpgrade_e) + 1],
                        gArmyNamesPlural[(secondUpgrade1) + 1],
                        gArmyNamesPlural[(thirdUpgrade_f) + 1]
                    );
                } else if (secondUpgrade1 != CREATURE_NONE) {
                    sprintf(
                        gText,
                        "{Форт на холме}\n\nВсе отряды "
                            "%s и %s вашего войска прошли обучение "
                            "у наставников этого форта."
                            " Теперь в вашей армии появились "
                            "отряды %s и %s.",
                        gArmyNamesPlural[(firstUpgrade_e)],
                        gArmyNamesPlural[(secondUpgrade1)],
                        gArmyNamesPlural[(firstUpgrade_e) + 1],
                        gArmyNamesPlural[(secondUpgrade1) + 1]
                    );
                } else {
                    sprintf(
                        gText,
                        "{Форт на холме}\n\nВсе отряды "
                            "%s вашего войска проходят обучение "
                            "у боевых наставников этого "
                            "форта. Теперь в вашей армии "
                            "есть отряд %s.",
                        gArmyNamesPlural[(firstUpgrade_e)],
                        gArmyNamesPlural[(firstUpgrade_e) + 1]
                    );
                }
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    gText,
                    NORMAL_DIALOG_MONSTER,
                    (firstUpgrade_e) + 1,
                    secondUpgrade1 == CREATURE_NONE ? -1 : NORMAL_DIALOG_MONSTER,
                    (secondUpgrade1) + 1,
                    -1
                );
            } else {
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    "{Форт на холме}\n\nНеобычный "
                        "союз огров и гномов предлагает "
                        "вам потренировать (улучшить)"
                        " любые подобные им войска."
                        " К сожалению, у вас таких при "
                        "себе нет.",
                    -1,
                    0,
                    -1,
                    0,
                    -1
                );
            }
            break;

        case MAP_OBJECT_FREEMANS_FOUNDRY:
            firstUpgrade_e = secondUpgrade1 = thirdUpgrade_f = CREATURE_NONE;
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
                    thirdUpgrade_f = CREATURE_IRON_GOLEM;
            }

            if (firstUpgrade_e != CREATURE_NONE) {
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                eventHero2->UpgradeCreatures(CREATURE_PIKEMAN, CREATURE_VETERAN_PIKEMAN);
                eventHero2->UpgradeCreatures(
                    CREATURE_SWORDSMAN,
                    CREATURE_MASTER_SWORDSMAN
                );
                eventHero2->UpgradeCreatures(CREATURE_IRON_GOLEM, CREATURE_STEEL_GOLEM);
                if (thirdUpgrade_f != CREATURE_NONE) {
                    sprintf(
                        gText,
                        "{Вольная литейная}\n\nВсе ваши "
                            "отряды %s, %s и %s теперь стали отрядами "
                            "%s, %s, и %s",
                        gArmyNamesPlural[(firstUpgrade_e)],
                        gArmyNamesPlural[(secondUpgrade1)],
                        gArmyNamesPlural[(thirdUpgrade_f)],
                        gArmyNamesPlural[(firstUpgrade_e) + 1],
                        gArmyNamesPlural[(secondUpgrade1) + 1],
                        gArmyNamesPlural[(thirdUpgrade_f) + 1]
                    );
                } else if (secondUpgrade1 != CREATURE_NONE) {
                    sprintf(
                        gText,
                        "{Вольная литейная}\n\nВсе ваши "
                            "отряды %s и %s теперь стали отрядами "
                            "%s и %s",
                        gArmyNamesPlural[(firstUpgrade_e)],
                        gArmyNamesPlural[(secondUpgrade1)],
                        gArmyNamesPlural[(firstUpgrade_e) + 1],
                        gArmyNamesPlural[(secondUpgrade1) + 1]
                    );
                } else {
                    sprintf(
                        gText,
                        "{Вольная литейная}\n\nВсе ваши "
                            "отряды %s теперь стали отрядами "
                            "%s.",
                        gArmyNamesPlural[(firstUpgrade_e)],
                        gArmyNamesPlural[(firstUpgrade_e) + 1]
                    );
                }
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    gText,
                    NORMAL_DIALOG_MONSTER,
                    (firstUpgrade_e) + 1,
                    secondUpgrade1 == CREATURE_NONE ? -1 : NORMAL_DIALOG_MONSTER,
                    (secondUpgrade1) + 1,
                    -1
                );
            } else {
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    "{Вольная литейная}\n\nКузнец "
                        "при этой литейной предлагает "
                        "заменить оружие копейщиков "
                        "и мечников с железного на "
                        "стальное. Он также сказал,"
                        " что владеет технологией "
                        "повышения железных големов "
                        "до стальных. К сожалению, никого "
                        "из них нет в вашей армии, и "
                        "он не могут помочь вам.",
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
                && eventHero2->NumArtifacts() < EVENT_ARTIFACT_CAPACITY) {
                sprintf(
                    gText,
                    "{Сундук}\n\nПроведя часы, пытаясь "
                        "выловить сундук из воды, вы "
                        "наконец открыли его. Внутри "
                        "были %s и 1000 золотых.",
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
                    eventHero2,
                    ArtifactType(cell->m_objectMetadata & CHEST_ARTIFACT_MASK),
                    1,
                    -1
                );
                GiveResource(eventHero2, RES_GOLD, SEA_CHEST_ARTIFACT_GOLD);
            } else if (cell->m_objectMetadata != SEA_CHEST_OUTCOME_EMPTY) {
                NormalDialog(
                    "{Сундук}\n\nПроведя часы, пытаясь "
                        "выловить сундук из воды, вы "
                        "наконец открыли его и нашли "
                        "внутри 1500 золотых.",
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
                GiveResource(eventHero2, RES_GOLD, SEA_CHEST_GOLD);
            } else {
                NormalDialog(
                    "{Сундук}\n\nПотратив часы на "
                        "то, чтобы выловить сундук "
                        "из моря, вы наконец открыли "
                        "его лишь за тем, чтобы увидеть:"
                        " внутри пусто.",
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
            fizzleType_k = true;
            eraseObject_l = 1;
            break;

        case MAP_OBJECT_FLOTSAM: {
            switch (static_cast<FlotsamReward>(cell->m_objectMetadata)) {
                case FLOTSAM_EMPTY:
                    NormalDialog(
                        "{Обломки}\n\nВы исследовали "
                            "плавающие обломки, но ничего "
                            "не нашли.",
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
                        "{Обломки}\n\nВы исследовали "
                            "плавающие обломки и добыли "
                            "немного древесины.",
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
                    GiveResource(eventHero2, RES_WOOD, FLOTSAM_WOOD_AMOUNT);
                    break;
                case FLOTSAM_WOOD_AND_GOLD:
                    NormalDialog(
                        "{Обломки}\n\nВы исследовали "
                            "плавающие обломки и нашли "
                            "немного золота и древесины.",
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
                    GiveResource(eventHero2, RES_WOOD, FLOTSAM_WOOD_AMOUNT);
                    GiveResource(eventHero2, RES_GOLD, FLOTSAM_GOLD_AMOUNT);
                    break;
                case FLOTSAM_LARGE_TREASURE:
                    NormalDialog(
                        "{Обломки}\n\nВы исследовали "
                            "плавающие обломки и нашли "
                            "немного древесины и золота.",
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
                    GiveResource(eventHero2, RES_WOOD, FLOTSAM_LARGE_WOOD_AMOUNT);
                    GiveResource(eventHero2, RES_GOLD, FLOTSAM_LARGE_GOLD_AMOUNT);
                    break;
            }
            fizzleType_k = true;
            eraseObject_l = 1;
            break;
        }

        case MAP_OBJECT_SHIPWRECK_SURVIVOR:
            if (eventHero2->NumArtifacts() < EVENT_ARTIFACT_CAPACITY) {
                sprintf(
                    gText,
                    "{Потерпевший кораблекрушение}"
                        "\n\nВы спасли жертву кораблекрушения "
                        "от неминуемой смерти в безжалостном "
                        "океане. Награда за вашу доброту "
                        "- %s.",
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
                    "{Потерпевший кораблекрушение}"
                        "\n\nВы спасли жертву кораблекрушения "
                        "от неминуемой смерти в безжалостном "
                        "океане. Исполненный благодарности,"
                        " бедняга сказал: \"Я бы наградил "
                        "вас артефактом, но у вас нет "
                        "для него места.\"",
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
            fizzleType_k = true;
            eraseObject_l = 1;
            break;

        case MAP_OBJECT_MAGELLAN_MAPS:
            if (gpCurPlayer->m_resources[(RES_GOLD)] < MAGELLAN_MAP_COST) {
                NormalDialog(
                    "{Карты Магеллана}\n\nКапитан "
                        "вздыхает. \"Что, деньжат не "
                        "хватает, да? Ты же не думаешь,"
                        " будто я отдам тебе свои карты "
                        "бесплатно!\"",
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
                    "{Карты Магеллана}\n\nБывший "
                        "капитан, живущий на этом подновленном "
                        "рыболовном причале, предлагает "
                        "вам карты, составленные в "
                        "прежние дни, за 1000 золотых. Желаете "
                        "их купить?",
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
                    gpCurPlayer->m_resources[(RES_GOLD)] -= MAGELLAN_MAP_COST;
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
                    "{Хижина ведьмы}\n\nПодойдя к "
                        "хижине и заглянув в окно, вы "
                        "увидели ведьму, склонившуюся "
                        "над древней книгой под названием "
                        "%s. Когда вы вошли, она обернулась "
                        "и молвила: \"Вы уже знаете то,"
                        " чему я могла бы научить. Больше "
                        "ничем помочь не смогу.\"",
                    gSecondarySkills[cell->m_objectMetadata]
                );
                NormalDialog(gText, NORMAL_DIALOG_INFO, -1, -1, -1, 0, -1, 0, -1, 0);
            } else if (eventHero2->m_secondarySkillCount >= HERO_SECONDARY_SKILL_LIMIT) {
                sprintf(
                    gText,
                    "{Хижина ведьмы}\n\nПодойдя к "
                        "хижине и заглянув в окно, вы "
                        "увидели ведьму, склонившуюся "
                        "над древней книгой под названием "
                        "%s. Когда вы вошли, она обернулась "
                        "и наставила на вас свой стеклянный "
                        "глаз. \"Ты уже и так знаешь все,"
                        " чего заслуживаешь!\" - завопила "
                        "ведьма. \"- А теперь убирайся "
                        "из моего дома!\"",
                    gSecondarySkills[cell->m_objectMetadata]
                );
                NormalDialog(gText, NORMAL_DIALOG_INFO, -1, -1, -1, 0, -1, 0, -1, 0);
            } else {
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                sprintf(
                    gText,
                    "{Хижина ведьмы}\n\nДряхлая, но "
                        "бессмертная ведьма, живущая "
                        "в этой хижине на курьих ножках,"
                        " по каким-то непонятным соображениям "
                        "решила, что %s вам пригодится.",
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
            }
            break;

        case MAP_OBJECT_ARTESIAN_SPRING:
            if (!cell->m_objectMetadata) {
                NormalDialog(
                    "{Артезианский источник}\n\nЭтот "
                        "родник восполняется раз в "
                        "неделю, а кто-то уже прикладывался "
                        "к нему на этой неделе.",
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
                springSpellPoints_j =
                    eventHero2->Stats(HERO_PRIMARY_KNOWLEDGE) * HERO_SPELL_POINTS_PER_KNOWLEDGE;
                if (eventHero2->m_spellPoints
                    >= springSpellPoints_j * ARTESIAN_SPRING_MANA_MULTIPLIER) {
                    NormalDialog(
                        "{Артезианский источник}\n\nГлоток "
                            "из родника обычно вдвое повышает "
                            "вашу магическую энергию, "
                            "но вы уже прибываете в подобном "
                            "состоянии.",
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
                        "{Артезианский источник}\n\nГлоток "
                            "из родника наполняет вашу "
                            "кровь магией! Теперь у вас "
                            "в запасе вдвое больше обычного "
                            "магической энергии.",
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
                    eventHero2->m_spellPoints = springSpellPoints_j * ARTESIAN_SPRING_MANA_MULTIPLIER;
                }
            }
            break;

        case MAP_OBJECT_MAGIC_WELL:
            if ((((eventHero2->m_eventFlags) & (HERO_EVENT_MAGIC_WELL)))) {
                NormalDialog(
                    "{Волшебный колодец}\n\nВолшебный "
                        "колодец}\n\nВторой глоток из "
                        "колодца за день вам не поможет.",
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
                wellSpellPoints_o =
                    eventHero2->Stats(HERO_PRIMARY_KNOWLEDGE) * HERO_SPELL_POINTS_PER_KNOWLEDGE;
                if (eventHero2->m_spellPoints >= wellSpellPoints_o) {
                    NormalDialog(
                        "{Волшебный колодец}\n\nГлоток "
                            "из колодца обычно восстанавливает "
                            "магическую энергию, но сейчас "
                            "она у вас и так на пределе.",
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
                        "{Волшебный колодец}\n\nГлоток "
                            "из колодца полностью восстановил "
                            "вашу магическую энергию.",
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
                        static_cast<i32>(eventHero2->m_eventFlags) | (HERO_EVENT_MAGIC_WELL)
                    );
                    eventHero2->m_spellPoints = wellSpellPoints_o;
                }
            }
            break;

        case MAP_OBJECT_COAST:
            if ((((eventHero2->m_eventFlags) & (HERO_EVENT_EMBARKED)))) {
                eventHero2->m_eventFlags = HeroEventFlag(
                    static_cast<i32>(eventHero2->m_eventFlags) & ~(HERO_EVENT_EMBARKED)
                );
                eventHero2->m_remainingMobility = 0;
                eventHero2->m_direction = m_cursorDirection;
                m_cursorType = eventHero2->m_cursorType;
                m_cursorFrame = GetCursorBaseFrame(m_cursorDirection);
                m_cursorActive = true;
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
                BOAT_RESTORE_MODE
            );
            eventHero2->m_eventFlags = HeroEventFlag(
                static_cast<i32>(eventHero2->m_eventFlags) | (HERO_EVENT_EMBARKED)
            );
            eventHero2->m_remainingMobility = 0;
            boat_k->heroId = eventHero2->m_id;
            boat_k->owner = eventHero2->m_owner;
            m_cursorType = HERO_TYPE_BOAT;
            m_cursorDirection = boat_k->direction;
            m_cursorFrame = GetCursorBaseFrame(m_cursorDirection);
            m_cursorActive = true;
            CompleteDraw(m_mapOriginX, m_mapOriginY, 0, 1);
            UpdateScreen(0, 0);
            break;

        case MAP_OBJECT_MINE:
            if (gpGame->m_mineOwners[cell->m_objectMetadata] == giCurPlayer)
                break;
            if (gpGame->m_mines[cell->m_objectMetadata].guardianType != CREATURE_NONE) {
                mineCombatResult_b = CombatMonsterEvent(
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
                if (mineCombatResult_b == COMBAT_RESULT_ATTACKER) {
                    gpGame->m_mines[cell->m_objectMetadata].guardianType = CREATURE_NONE;
                    eventHero2->CheckLevel();
                } else {
                    break;
                }
            }
            EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
            if (gpGame->m_mines[cell->m_objectMetadata].resourceType == MINE_TYPE_GOLD)
                resourceAmount_o = MINE_GOLD_INCOME;
            else if (gpGame->m_mines[cell->m_objectMetadata].resourceType == MINE_TYPE_ORE)
                resourceAmount_o = MINE_ORE_INCOME;
            else
                resourceAmount_o = 1;
            EventWindow(
                (gpGame->m_mines[cell->m_objectMetadata].resourceType)
                    + MINE_RESOURCE_ICON_OFFSET,
                NORMAL_DIALOG_INFO,
                "",
                (gpGame->m_mines[cell->m_objectMetadata].resourceType),
                -resourceAmount_o,
                -1,
                0,
                -1
            );
            goto claimMine;

        case MAP_OBJECT_ALCHEMIST_LAB:
            if (gpGame->m_mineOwners[cell->m_objectMetadata] == giCurPlayer)
                break;
            EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
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
            EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
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
            break;

        case MAP_OBJECT_TREASURE_CHEST:
            if (cell->m_objectMetadata & CHEST_ARTIFACT_FLAG) {
                if (eventHero2->NumArtifacts() >= EVENT_ARTIFACT_CAPACITY) {
                    cell->m_objectMetadata = CHEST_REWARD_SMALL;
                    goto chestGold;
                }
                sprintf(
                    gText,
                    "{Ларец}\n\nИсследуя окрестности,"
                        " вы наткнулись на зарытый "
                        "ларец, а в нем - древний артефакт "
                        "'%s'",
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
                    eventHero2,
                    ArtifactType(cell->m_objectMetadata & CHEST_ARTIFACT_MASK),
                    1,
                    -1
                );
            } else {
            chestGold:
                EventWindow(
                    -1,
                    NORMAL_DIALOG_CONFIRM,
                    "{Ларец}\n\nИсследуя окрестности,"
                        " вы наткнулись на древний "
                        "ларец. Золото можно оставить "
                        "себе или раздать крестьянам "
                        "в обмен на опыт. Оставите себе "
                        "золото?",
                    (RES_GOLD),
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
            eraseObject_l = 1;
            fizzleType_k = true;
            eventHero2->CheckLevel();
            break;

        case MAP_OBJECT_BUOY:
            if ((((eventHero2->m_eventFlags) & (HERO_EVENT_BUOY)))) {
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
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                eventHero2->m_eventFlags = HeroEventFlag(
                    static_cast<i32>(eventHero2->m_eventFlags) | (HERO_EVENT_BUOY)
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
            }
            break;

        case MAP_OBJECT_FAERIE_RING:
            if ((((eventHero2->m_eventFlags) & (HERO_EVENT_FAERIE_RING)))) {
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
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                eventHero2->m_eventFlags = HeroEventFlag(
                    static_cast<i32>(eventHero2->m_eventFlags) | (HERO_EVENT_FAERIE_RING)
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
            }
            break;

        case MAP_OBJECT_IDOL:
            if ((((eventHero2->m_eventFlags) & (HERO_EVENT_IDOL)))) {
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    "{Идол}\n\nВы нашли древнего идола."
                        " Говорят, он приносит удачу "
                        "тем, кто его навещает, но поскольку "
                        "звезды и так покровительствуют "
                        "вам, идол ничего нового вам "
                        "не дал.",
                    -1,
                    0,
                    -1,
                    0,
                    -1
                );
            } else {
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                eventHero2->m_eventFlags = HeroEventFlag(
                    static_cast<i32>(eventHero2->m_eventFlags) | (HERO_EVENT_IDOL)
                );
                eventHero2->m_luck++;
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    "{Идол}\n\nВы нашли древнего идола."
                        " Говорят, если его поцеловать,"
                        " это принесет удачу - вы так "
                        "и поступили. Камень оказался "
                        "очень холоден для губ.",
                    NORMAL_DIALOG_LUCK_BONUS,
                    0,
                    -1,
                    0,
                    -1
                );
            }
            break;

        case MAP_OBJECT_FOUNTAIN:
            if ((((eventHero2->m_eventFlags) & (HERO_EVENT_FOUNTAIN)))) {
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
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                eventHero2->m_eventFlags = HeroEventFlag(
                    static_cast<i32>(eventHero2->m_eventFlags) | (HERO_EVENT_FOUNTAIN)
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
            }
            break;

        case MAP_OBJECT_WATERING_HOLE:
            if ((((eventHero2->m_eventFlags) & (HERO_EVENT_WATERING_HOLE)))) {
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    "{Промоина}\n\nВыпивка освежает,"
                        " но иных благ не приносит. Источник "
                        "снова поможет вам после следующей "
                        "битвы.",
                    -1,
                    0,
                    -1,
                    0,
                    -1
                );
            } else {
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                eventHero2->m_eventFlags = HeroEventFlag(
                    static_cast<i32>(eventHero2->m_eventFlags) | (HERO_EVENT_WATERING_HOLE)
                );
                eventHero2->m_morale++;
                eventHero2->m_mobility += WATERING_HOLE_MOBILITY_BONUS;
                eventHero2->m_remainingMobility += WATERING_HOLE_MOBILITY_BONUS;
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    "{Промоина}\n\nДобрый глоток "
                        "исполнил ваши войска силы "
                        "и поднял дух. Сегодня вы сможете "
                        "пройти чуть дальше.",
                    NORMAL_DIALOG_MORALE_BONUS,
                    0,
                    -1,
                    0,
                    -1
                );
            }
            break;

        case MAP_OBJECT_OASIS:
            if ((((eventHero2->m_eventFlags) & (HERO_EVENT_OASIS)))) {
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    "{Оазис}\n\nГлоток влаги в оазисе "
                        "освежает, но иной пользы не "
                        "приносит. Этот оазис, возможно,"
                        " пригодится вам после следующей "
                        "битвы.",
                    -1,
                    0,
                    -1,
                    0,
                    -1
                );
            } else {
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                eventHero2->m_eventFlags = HeroEventFlag(
                    static_cast<i32>(eventHero2->m_eventFlags) | (HERO_EVENT_OASIS)
                );
                eventHero2->m_morale++;
                eventHero2->m_mobility += OASIS_MOBILITY_BONUS;
                eventHero2->m_remainingMobility += OASIS_MOBILITY_BONUS;
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    "{Оазис}\n\nГлоток воды в оазисе "
                        "наполняет ваших воинов силой "
                        "и поднимает дух. Сегодня вы "
                        "сможете пройти чуть больше.",
                    NORMAL_DIALOG_MORALE_BONUS,
                    0,
                    -1,
                    0,
                    -1
                );
            }
            break;

        case MAP_OBJECT_TEMPLE:
            if ((((eventHero2->m_eventFlags) & (HERO_EVENT_TEMPLE)))) {
                NormalDialog(
                    "{Храм}\n\nДвойная молитва войне "
                        "не подспорье. Заходите после "
                        "битвы.",
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
                eventHero2->m_eventFlags = HeroEventFlag(
                    static_cast<i32>(eventHero2->m_eventFlags) | (HERO_EVENT_TEMPLE)
                );
                eventHero2->m_morale += TEMPLE_MORALE_BONUS;
                NormalDialog(
                    "{Храм}\n\nПосещение храма и молитва "
                        "подняли мораль ваших войск.",
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
                    "{Навес}\n\nЭто сооружение давно "
                        "брошено. Ничего ценного тут "
                        "нет.",
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
                    "{Навес}\n\nВы нашли брошенный "
                        "навес. Поискав вокруг, вы обнаружили "
                        "кое-какие ресурсы, припрятанные "
                        "поблизости.",
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
                    eventHero2,
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
                    "{Повозка}\n\nВы набрели на старую "
                        "повозку, не доехавшую до торной "
                        "дороги и брошенную купцом."
                        " К сожалению, кто-то нашел ее "
                        "до вас, повозка пуста.",
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
                if (eventHero2->NumArtifacts() == EVENT_ARTIFACT_CAPACITY)
                    goto wagonEmpty;
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                eventValue1 = cell->m_objectMetadata & WAGON_ARTIFACT_MASK;
                sprintf(
                    gText,
                    "{Повозка}\n\nВы набрели на старую "
                        "повозку, не доехавшую до торной "
                        "дороги и брошенную купцом."
                        " В ней вы нашли %s.",
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
            } else {
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    "{Повозка}\n\nВы набрели на старую "
                        "повозку, не доехавшую до торной "
                        "дороги и брошенную купцом."
                        " В ней же вы нашли до сих пор "
                        "не тронутый груз.",
                    (cell->m_objectMetadata & CAMPFIRE_RESOURCE_MASK) - 1,
                    (cell->m_objectMetadata & DAEMON_SERVANT_MASK) >> DAEMON_SERVANT_SHIFT,
                    -1,
                    0,
                    -1
                );
                GiveResource(
                    eventHero2,
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
                    EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                    if (eventHero2->NumArtifacts() == EVENT_ARTIFACT_CAPACITY) {
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
                        GiveResource(eventHero2, RES_GOLD, SKELETON_GOLD);
                    } else {
                        eventValue1 = cell->m_objectMetadata - SKELETON_ARTIFACT_OFFSET;
                        sprintf(
                            gText,
                            "%s %s",
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
                (RES_GOLD),
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
                ResourceType(cell->m_objectMetadata & (CAMPFIRE_RESOURCE_MASK)),
                cell->m_objectMetadata >> CAMPFIRE_AMOUNT_SHIFT
            );
            eraseObject_l = 1;
            fizzleType_k = true;
            SetEnvironmentOrigin(
                m_mapOriginX + ENVIRONMENT_BORDER,
                m_mapOriginY + ENVIRONMENT_BORDER,
                1
            );
            break;

        case MAP_OBJECT_XANADU:
            if (eventHero2->m_xanaduVisits & (1 << cell->m_objectMetadata)) {
                NormalDialog(
                    "{Ксанаду}\n\nУзнав вас, лакей "
                        "отказался пустить внутрь."
                        " \"Наставник,\" - сказал он, - \"не "
                        "принимает одного и того же "
                        "студента дважды.\"",
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
                if (eventHero2->m_level
                        + (eventHero2->m_secondarySkills[(HERO_SKILL_DIPLOMACY)])
                              * XANADU_DIPLOMACY_MULTIPLIER
                    >= XANADU_ADMISSION_LEVEL) {
                    EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                    NormalDialog(
                        "{Ксанаду}\n\nЛакей допустил "
                            "вас до аудиенции с хозяином "
                            "дома. Занятия с ним повысили "
                            "четыре умения, необходимые "
                            "каждому герою.",
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
                    eventHero2->m_primaryStats[(HERO_PRIMARY_ATTACK)]++;
                    eventHero2->m_primaryStats[(HERO_PRIMARY_DEFENSE)]++;
                    eventHero2->m_primaryStats[(HERO_PRIMARY_KNOWLEDGE)]++;
                    eventHero2->m_primaryStats[(HERO_PRIMARY_SPELL_POWER)]++;
                    eventHero2->m_xanaduVisits |= 1 << cell->m_objectMetadata;
                } else {
                    NormalDialog(
                        "{Ксанаду}\n\nЛакей открыл дверь "
                            "и оглядел вас с ног до головы."
                            " \"Вы не так знамениты и не такой "
                            "уж дипломат, чтобы мой хозяин "
                            "принял вас,\" - фыркнул он. - \"Возвращайтесь,"
                            " когда сочтете себя достойным."
                            "\"",
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

        case MAP_OBJECT_FORT:
            if (eventHero2->m_fortVisits & (1 << cell->m_objectMetadata)) {
                NormalDialog(
                    "{Форт}\n\n \"Просите, сэр,\" - Сказал "
                        "предводитель воинов, - \"но "
                        "вы уже знаете все, чему мы способны "
                        "научить.\"",
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
                    "{Форт}\n\n Воины, живущие в этом "
                        "форте, научили вас паре новых "
                        "защитных приемов.",
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
                eventHero2->m_primaryStats[(HERO_PRIMARY_DEFENSE)]++;
                eventHero2->m_fortVisits |= 1 << cell->m_objectMetadata;
            }
            break;

        case MAP_OBJECT_STANDING_STONES:
            if (eventHero2->m_standingStoneVisits & (1 << cell->m_objectMetadata)) {
                NormalDialog(
                    "{Менгир}\n\nВы застали группу "
                        "друидов за обрядом в одном "
                        "из их диковинных каменных "
                        "сооружений. Друиды, не нарушая "
                        "безмолвия, показали жестами,"
                        " что им больше нечему вас учить.",
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
                    "{Менгир}\n\nВы застали группу "
                        "друидов за обрядом в одном "
                        "из их диковинных каменных "
                        "сооружений. Не нарушая безмолвия,"
                        " они все же научили вас новым "
                        "способам колдовства.",
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
                eventHero2->m_primaryStats[(HERO_PRIMARY_SPELL_POWER)]++;
                eventHero2->m_standingStoneVisits |= 1 << cell->m_objectMetadata;
            }
            break;

        case MAP_OBJECT_WITCH_DOCTOR_HUT:
            if (eventHero2->m_witchDoctorVisits & (1 << cell->m_objectMetadata)) {
                NormalDialog(
                    "{Хижина ведьмы}\n\n\"Убирайся!"
                        "\" -  рявкнула ведьма, \"ты знаешь "
                        "все, что знаю я.\"",
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
                    "{Хижина ведьмы}\n\nОрчая ведьма,"
                        " живущая в этой хижине, обогатила "
                        "ваши познания в магии, показав,"
                        " как гадать на камнях, читать "
                        "знамения и извлекать сущность "
                        "бытия из сложного переплетения "
                        "цыплячьих потрохов.",
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
                eventHero2->m_primaryStats[(HERO_PRIMARY_KNOWLEDGE)]++;
                eventHero2->m_witchDoctorVisits |= 1 << cell->m_objectMetadata;
            }
            break;

        case MAP_OBJECT_MERCENARY_CAMP:
            if (eventHero2->m_mercenaryCampVisits & (1 << cell->m_objectMetadata)) {
                NormalDialog(
                    "{Лагерь наемников}\n\nВы пришли "
                        "в лагерь наемников, где воины "
                        "отрабатывают тактику. \"Для "
                        "нас вы слишком умелый боец,"
                        "\" -сказал капитан наемников."
                        " - \"Больше мы ничему вас научить "
                        "не можем.\"",
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
                    "{Лагерь наемников}\n\nВы пришли "
                        "в лагерь наемников, отрабатывающих "
                        "свою тактику. Наемники поприветствовали "
                        "вас и пригласили вместе с "
                        "вашим отрядом позаниматься "
                        "вместе с ними.",
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
                eventHero2->m_primaryStats[(HERO_PRIMARY_ATTACK)]++;
                eventHero2->m_mercenaryCampVisits |= 1 << cell->m_objectMetadata;
            }
            break;

        case MAP_OBJECT_GAZEBO:
            if (eventHero2->m_gazeboVisits & (1 << cell->m_objectMetadata)) {
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
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
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
                GiveResource(eventHero2, RES_GOLD, cell->m_objectMetadata * MAP_EVENT_GOLD_AMOUNT);
                cell->m_objectMetadata = 0;
            }
            break;

        case MAP_OBJECT_RESOURCE:
            resourceType_a = static_cast<ResourceType>(cell->m_objectIndex / 2);
            GiveResource(
                eventHero2,
                resourceType_a,
                resourceType_a == RES_GOLD ? cell->m_objectMetadata * CAMPFIRE_GOLD_MULTIPLIER
                                         : cell->m_objectMetadata
            );
            strcpy(sphinxAnswer_a, gResourceNames[(resourceType_a)]);
            sphinxAnswer_a[0] = ToLowerCp1251(sphinxAnswer_a[0]);
            sprintf(gText, gEventText[EVENT_TEXT_RESOURCE_PICKUP], sphinxAnswer_a);
            BVResMsg(
                gText,
                resourceType_a,
                resourceType_a == RES_GOLD ? cell->m_objectMetadata * CAMPFIRE_GOLD_MULTIPLIER
                                         : cell->m_objectMetadata
            );
            eraseObject_l = 1;
            fizzleType_k = true;
            break;

        case MAP_OBJECT_WINDMILL:
            if (cell->m_objectMetadata <= (RES_GOLD)) {
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
            EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
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
            if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
                RecruitEvent(eventHero2, CREATURE_GENIE, cell);
                if (!cell->m_objectMetadata) {
                    eraseObject_l = 1;
                    fizzleType_k = true;
                }
            }
            break;

        case MAP_OBJECT_TREE_CITY:
            if (!cell->m_objectMetadata) {
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    "{Древо-город}\n\nВы нашли древесный "
                        "город фей. К сожалению, ни одна "
                        "фея не захотела присоединиться "
                        "к вашей армии. Может быть на "
                        "следующей неделе они передумают.",
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
                    "{Древо-город}\n\nНекоторые феи "
                        "из этого города на деревьях "
                        "желают вступить в вашу армию "
                        "за некоторое вознаграждение."
                        " Желаете нанять фей?",
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
                    "{Руины}\n\nВы обыскали руины,"
                    " но убедились лишь, что жившие "
                    "тут медузы ушли. Может быть "
                    "вам повезет больше на следующей "
                        "неделе.",
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
                    "{Руины}\n\nВы обыскали руины "
                    "и нашли нескольких медуз,"
                    " обитающих тут. Они согласны "
                    "вступить в вашу армию за вознаграждение."
                        " Желаете нанять медуз?",
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
                    "{Мост троллей}\n\nВы нашли один "
                    "из тех мостов, под которыми "
                    "так любят селиться тролли,"
                    " но сейчас тут никого нет. Может,"
                        " на следующей неделе появятся.",
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
                    "{Мост троллей}\n\nТролли, живущие "
                    "под этим мостом бросают вам "
                        "вызов. Сразитесь с ними?",
                    -1,
                    0,
                    -1,
                    0,
                    -1
                );
                if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES
                    && CombatMonsterEvent(
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
                           == COMBAT_RESULT_ATTACKER) {
                    eventHero2->CheckLevel();
                    cell->m_objectMetadata -= DWELLING_GUARDED_FLAG;
                    EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                    EventWindow(
                        -1,
                        NORMAL_DIALOG_CONFIRM,
                        "{Мост троллей}\n\nОсталось лишь "
                        "несколько троллей, в страхе "
                        "забившихся под мост. Осмелев,"
                        " они предложили свои услуги "
                        "в качестве наемников. Желаете "
                            "нанять троллей?",
                        -1,
                        0,
                        -1,
                        0,
                        -1
                    );
                    if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES)
                        goto recruitTroll;
                }
                break;
            } else {
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                EventWindow(
                    -1,
                    NORMAL_DIALOG_CONFIRM,
                    "{Мост троллей}\n\nКое-кто из троллей,"
                    " живущих под мостом, желает "
                    "присоединиться к вашей армии,"
                    " но за плату.  Желаете нанять "
                        "троллей?",
                    -1,
                    0,
                    -1,
                    0,
                    -1
                );
                if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
                recruitTroll:
                    RecruitEvent(eventHero2, CREATURE_TROLL, cell);
                }
            }
            break;

        case MAP_OBJECT_CITY_OF_DEAD:
            if (!cell->m_objectMetadata) {
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    "{Град мертвых}\n\nВ этом городе "
                    "мертвецов жизни не наблюдается,"
                    " нежити тоже. Может, на следующей "
                    "неделе кто-то из нежити забредет "
                        "сюда.",
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
                    "{Град мертвых}\n\nВы нашли руины "
                    "древнего города, ныне населенного "
                    "лишь нежитью. Обследовать "
                        "город?",
                    -1,
                    0,
                    -1,
                    0,
                    -1
                );
                if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
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
                        == COMBAT_RESULT_ATTACKER) {
                    eventHero2->CheckLevel();
                cell->m_objectMetadata -= DWELLING_GUARDED_FLAG;
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                EventWindow(
                    -1,
                    NORMAL_DIALOG_CONFIRM,
                    "{Град мертвых}\n\nУцелевшие "
                    "личи прониклись вашей победой "
                    "над их собратьями и предложили "
                    "свои услуги за плату. Желаете "
                        "нанять личей?",
                    -1,
                    0,
                    -1,
                    0,
                    -1
                );
                if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES)
                    goto recruitLich;
                    }
                }
                break;
            } else {
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                EventWindow(
                    -1,
                    NORMAL_DIALOG_CONFIRM,
                    "{Град мертвых}\n\nКое-кто из личей,"
                    " обитающих здесь, желает вступить "
                    "в вашу армию за плату. Нанять "
                        "личей?",
                    -1,
                    0,
                    -1,
                    0,
                    -1
                );
                if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
                recruitLich:
                    RecruitEvent(eventHero2, CREATURE_POWER_LICH, cell);
                }
            }
            break;

        case MAP_OBJECT_DRAGON_CITY:
            if (!cell->m_objectMetadata) {
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    "{Драконий город}\n\nНа этой неделе "
                    "в городе нет драконов, желающих "
                    "примкнуть к вам. На следующей "
                        "неделе, возможно, кто-то появится.",
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
                    "{Драконий город}\n\nВы стоите "
                    "пред драконьим городом, местом,"
                    " запретным для простых смертных."
                    " Соблаговолите ли вы нарушить "
                    "это правило и бросить вызов "
                        "драконам?",
                    -1,
                    0,
                    -1,
                    0,
                    -1
                );
                if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
                dragonFactor_i = DRAGON_CITY_BASE_FACTOR;
                if (gbInCampaign && gpGame->m_campaignType == CAMPAIGN_ARCHIBALD
                    && gpGame->m_campaignScenario + 1 == DRAGON_CITY_ARCHIBALD_SCENARIO)
                    dragonFactor_i = DRAGON_CITY_CAMPAIGN_FACTOR;
                if (CombatMonsterEvent(
                        eventHero2,
                        CREATURE_GREEN_DRAGON,
                        dragonFactor_i * DRAGON_CITY_GREEN_COUNT_FACTOR,
                        cell,
                        x,
                        y,
                        0,
                        x,
                        y,
                        CREATURE_RED_DRAGON,
                        dragonFactor_i,
                        DRAGON_CITY_DRAGON_STACKS,
                        CREATURE_BLACK_DRAGON,
                        dragonFactor_i,
                        DRAGON_CITY_DRAGON_STACKS
                    )
                    == COMBAT_RESULT_ATTACKER) {
                CheckEndGame(END_GAME_FORCE_NONE, true);
                if (gbGameOver)
                    break;
                eventHero2->CheckLevel();
                cell->m_objectMetadata -= DWELLING_GUARDED_FLAG;
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                EventWindow(
                    -1,
                    NORMAL_DIALOG_CONFIRM,
                    "{Драконий город}\n\nПосле вашей "
                    "победы над лучшими драконьими "
                    "воинами отцы города согласились "
                    "за плату предоставить вашему "
                    "войску драконов. Желаете "
                        "нанять драконов?",
                    -1,
                    0,
                    -1,
                    0,
                    -1
                );
                if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES)
                    goto recruitDragon;
                }
                }
                break;
            } else {
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                EventWindow(
                    -1,
                    NORMAL_DIALOG_CONFIRM,
                    "{Драконий город}\n\nДраконий "
                    "город готов предложить вашему "
                    "войску драконов, не бесплатно."
                        " Желаете нанять драконов?",
                    -1,
                    0,
                    -1,
                    0,
                    -1
                );
                if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
                recruitDragon:
                    RecruitEvent(eventHero2, CREATURE_RED_DRAGON, cell);
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
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
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
                if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES)
                    RecruitEvent(eventHero2, CREATURE_ROGUE, cell);
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
            PlayerMonsterInteract(cell, cell, eventHero2, &eraseObject_l, x, y, 0, x, y);
            break;

        case MAP_OBJECT_OBELISK:
            if (!(gpGame->m_obeliskVisitors[cell->m_objectMetadata - 1]
                  & (1 << eventHero2->m_owner))) {
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                gpGame->m_obeliskVisitors[cell->m_objectMetadata - 1] |= 1 << eventHero2->m_owner;
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
            if ((eventHero2->m_treeKnowledgeVisits
                 & (1U << (cell->m_objectMetadata & TREE_KNOWLEDGE_VISIT_INDEX_MASK)))
                != 0) {
                NormalDialog(
                    "{Древо познания}\n\nПри вашем "
                    "приближении древесные глаза "
                    "засияли восторгом.  \"Рад видеть "
                    "тебя, мой ученик.  Надеюсь, "
                    "моя наука пошла тебе на пользу."
                        "\"",
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
                heroLevel_e = eventHero2->GetLevel(eventHero2->m_experience);
                levelExperience_g = eventHero2->GetExperience(heroLevel_e + 1)
                                   - eventHero2->GetExperience(heroLevel_e);
                switch (cell->m_objectMetadata >> TREE_KNOWLEDGE_MODE_SHIFT) {
                    case TREE_KNOWLEDGE_FREE:
                        NormalDialog(
                            "{Древо познания}\n\nПри вашем "
                            "приближении древесные глаза "
                            "засветились восторгом. \"А,"
                            " странник! Позволь преподать "
                            "тебе малую толику того, что "
                                "я выучил за годы.\"",
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
                        GiveExperience(eventHero2, levelExperience_g, 0);
                        eventHero2->m_treeKnowledgeVisits |=
                            1U << (cell->m_objectMetadata & TREE_KNOWLEDGE_VISIT_INDEX_MASK);
                        break;

                    case TREE_KNOWLEDGE_GOLD:
                        if (gpCurPlayer->m_resources[(RES_GOLD)] >= TREE_KNOWLEDGE_GOLD_COST) {
                            NormalDialog(
                                "{Древо познания}\n\nПри вашем "
                                "приближении древесные глаза "
                                "засияли восторгом. \"А, путник!"
                                " Я с радостью научу тебя хотя "
                                "бы малой части того, что усвоил "
                                "за годы, всего за 2000 золотых."
                                "\" (Просто зарой их у моих корней)"
                                    ".",
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
                                gpCurPlayer->m_resources[(RES_GOLD)] -= TREE_KNOWLEDGE_GOLD_COST;
                                GiveExperience(eventHero2, levelExperience_g, 0);
                                eventHero2->m_treeKnowledgeVisits |=
                            1U << (cell->m_objectMetadata & TREE_KNOWLEDGE_VISIT_INDEX_MASK);
                            }
                        } else {
                            NormalDialog(
                                "{Древо познания}\n\nГлаза дерева "
                                "наполнились слезами. \"Мне "
                                "нужно 2000 золотых.\" - прошептало "
                                "оно. (вздох) - \"Возвращайся, когда "
                                    "сможешь внести плату.\"",
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

                    case TREE_KNOWLEDGE_GEMS:
                        if (gpCurPlayer->m_resources[(RES_GEMS)] >= TREE_KNOWLEDGE_GEM_COST) {
                            NormalDialog(
                                "{Древо познания}\n\nА, путник!"
                                " Я с радостью преподам тебе "
                                "хоть малую часть того, что "
                                "выучил за годы, всего за 10 самоцветов."
                                "\" (Просто зарой их у моих корней)"
                                    ".",
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
                                gpCurPlayer->m_resources[(RES_GEMS)] -= TREE_KNOWLEDGE_GEM_COST;
                                GiveExperience(eventHero2, levelExperience_g, 0);
                                eventHero2->m_treeKnowledgeVisits |=
                            1U << (cell->m_objectMetadata & TREE_KNOWLEDGE_VISIT_INDEX_MASK);
                            }
                        } else {
                            NormalDialog(
                                "{Древо познания}\n\nСлезы переполняют "
                                "древесные глаза. \"Мне нужно "
                                "10 самоцветов.\" - прошептало "
                                "оно. (Вздох). \"Что ж, приходи,"
                                " когда сможешь заплатить."
                                    "\"",
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
            }
            eventHero2->CheckLevel();
            break;
        }

        case MAP_OBJECT_ORACLE:
            EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
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
            oracleWindow_o = new heroWindow(
                0,
                0,
                "thiefwin.bin"
            );
            if (oracleWindow_o == NULL)
                MemError();
            SetWinText(oracleWindow_o, ORACLE_WINDOW_TEXT_ID);
            gpTownManager->SetupThievesGuild(oracleWindow_o, ORACLE_THIEVES_GUILD_RANK);
            strcpy(
                gText,
                "Святилище - информация об "
                    "игроках"
            );
            oracleMessage_o.type = MESSAGE_WIDGET;
            oracleMessage_o.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
            oracleMessage_o.payload.widget.id = 0;
            oracleMessage_o.payload.widget.data.text = gText;
            oracleWindow_o->BroadcastMessage(oracleMessage_o);
            gpWindowManager->DoDialog(oracleWindow_o, TrueFalseDialogHandler, 0);
            delete oracleWindow_o;
            RedrawAdvScreen(1, 0);
            break;

        case MAP_OBJECT_SHRINE_FIRST_CIRCLE:
            sprintf(
                gText,
                "%s'%s'.  ",
                "{Святилище 1-го Круга}\n\nВы набрели "
                "на маленькое святилище, где "
                "служат молодые послушники."
                "  В обмен на защиту они согласились "
                "научить вас простому заклинанию "
                    "- ",
                gSpellNames[cell->m_objectMetadata - 1]
            );
            goto shrineSpell;

        case MAP_OBJECT_SHRINE_SECOND_CIRCLE:
            sprintf(
                gText,
                "%s'%s'.  ",
                "{Святилище 2-го Круга}\n\nВы набрели "
                "на богато расписанное святилище,"
                " где служат полноправные "
                "братья веры.  В обмен на защиту "
                "они согласились научить вас "
                    "новому заклинанию - ",
                gSpellNames[cell->m_objectMetadata - 1]
            );
            goto shrineSpell;

        case MAP_OBJECT_SHRINE_THIRD_CIRCLE:
            sprintf(
                gText,
                "%s'%s'.  ",
                "{Святилище 3-го Круга}\n\nВы набрели "
                "на роскошное святилище, где "
                "служат высшие жрецы.  В обмен "
                "на защиту они согласились "
                "научить вас премудрому заклинанию "
                    "- ",
                gSpellNames[cell->m_objectMetadata - 1]
            );
        shrineSpell:
            if (eventHero2->HasArtifact(ARTIFACT_MAGIC_BOOK)) {
                if ((gsSpellInfo[cell->m_objectMetadata - 1].level)
                    <= static_cast<i32>(eventHero2->m_secondarySkills[(HERO_SKILL_WISDOM)])
                           + SHRINE_WISDOM_BONUS) {
                    EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                    eventHero2->AddSpell(
                        static_cast<SpellType>(cell->m_objectMetadata - 1),
                        eventHero2->Stats(HERO_PRIMARY_KNOWLEDGE)
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
                        "К сожалению, у вас не хватает "
                            "мудрости, чтобы понять заклинание,"
                            " и вы не можете выучить его."
                    );
                    EventWindow(-1, NORMAL_DIALOG_INFO, gText, -1, 0, -1, 0, -1);
                }
            } else {
                strcat(
                    gText,
                    "К сожалению, у вас нет Волшебной "
                        "книги, чтобы записать заклинание "
                        "в нее."
                );
                EventWindow(-1, NORMAL_DIALOG_INFO, gText, -1, 0, -1, 0, -1);
            }
            break;

        case MAP_OBJECT_CASTLE:
            TownEvent(cell, x, y);
            break;

        case MAP_OBJECT_WHIRLPOOL:
            EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
            DoWhirlpool(eventHero2);
            goto findTeleportDestination;

        case MAP_OBJECT_STONE_LITHS:
            EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
        findTeleportDestination:
            teleportCount_e = 0;
            for (teleportY_e = 0; teleportY_e < MAP_HEIGHT; teleportY_e++) {
                for (teleportX_e = 0; teleportX_e < MAP_WIDTH; teleportX_e++) {
                    if ((gpGame->m_worldMap.GetCell(teleportX_e, teleportY_e))->m_triggerType
                            == (eventType_g | MAP_TRIGGER_ACTION_FLAG)
                        && (gpGame->m_worldMap.GetCell(teleportX_e, teleportY_e))->m_objectIndex
                               == cell->m_objectIndex
                        && abs(teleportX_e - x) + abs(teleportY_e - y)
                               > (eventType_g == MAP_OBJECT_STONE_LITHS ? STONE_LITHS_MIN_DISTANCE
                                                                        : WHIRLPOOL_MIN_DISTANCE)) {
                        teleportCount_e++;
                    }
                }
            }
            if (teleportCount_e >= 1) {
                if (teleportCount_e > 1)
                    teleportCount_e = Random(1, teleportCount_e);
                for (teleportY_e = 0; teleportY_e < MAP_HEIGHT; teleportY_e++) {
                    for (teleportX_e = 0; teleportX_e < MAP_WIDTH; teleportX_e++) {
                        if ((gpGame->m_worldMap.GetCell(teleportX_e, teleportY_e))->m_triggerType
                                == (eventType_g | MAP_TRIGGER_ACTION_FLAG)
                            && (gpGame->m_worldMap.GetCell(teleportX_e, teleportY_e))->m_objectIndex
                                   == cell->m_objectIndex
                            && (teleportX_e != x || teleportY_e != y)
                            && abs(teleportX_e - x) + abs(teleportY_e - y)
                                   > (eventType_g == MAP_OBJECT_STONE_LITHS
                                          ? STONE_LITHS_MIN_DISTANCE
                                          : WHIRLPOOL_MIN_DISTANCE)
                            && --teleportCount_e <= 0) {
                            goto teleportHero;
                        }
                    }
                }
            teleportHero:
                StopCursor(1);
                gpAdvManager->TeleportTo(eventHero2, teleportX_e, teleportY_e, 1, 0);
            }
            break;

        case MAP_OBJECT_ARTIFACT: {
            artifactResourceType_k = (cell->m_objectMetadata & ARTIFACT_EVENT_RESOURCE_MASK)
                                   >> ARTIFACT_EVENT_RESOURCE_SHIFT;
            artifact_g = static_cast<ArtifactType>(cell->m_objectIndex / 2);
            guardedMonster_c =
                static_cast<CreatureType>(cell->m_objectMetadata & ARTIFACT_EVENT_MONSTER_MASK);
            if (eventHero2->NumArtifacts() == EVENT_ARTIFACT_CAPACITY) {
                NormalDialog(
                    "Вы не можете подобрать этот "
                        "артефакт, у вас нет места!",
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
                    "Сказочный золотой лук эльфов "
                    "просто валяется здесь, в грязи."
                    " Вы подобрали его и отвезли "
                    "в эльфийский город. В благодарность "
                    "за этот подвиг король эльфов "
                    "пообещал вам помощь своего "
                    "народа в любую минуту, когда "
                        "вам это понадобится."
                );
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    gText,
                    MAP_EVENT_REWARD_ARTIFACT,
                    (artifact_g),
                    -1,
                    0,
                    -1
                );
                GiveArtifact(eventHero2, artifact_g, 1, static_cast<i8>(cell->m_objectMetadata));
                eraseObject_l = 1;
                fizzleType_k = true;
                break;
            }

            if (artifact_g == ARTIFACT_SPELL_SCROLL) {
                EventSound(eventType_g, 1, &eventSample_f);
                xTheSpell = cell->m_objectMetadata;
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    gArtifactEvent[(artifact_g)],
                    MAP_EVENT_REWARD_ARTIFACT,
                    (artifact_g),
                    -1,
                    0,
                    -1
                );
                GiveArtifact(eventHero2, artifact_g, 1, static_cast<i8>(cell->m_objectMetadata));
                eraseObject_l = 1;
                fizzleType_k = true;
                break;
            }

            if (cell->m_objectMetadata & ARTIFACT_EVENT_GUARDED_FLAG) {
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                if (guardedMonster_c == CREATURE_ROGUE) {
                    NormalDialog(
                        "{Артефакт}\n\nВы заметили древний "
                        "артефакт. Но едва вы подошли "
                        "к нему, из кустов выскочила "
                        "шайка разбойников, охраняющих "
                            "свое награбленное добро.",
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
                    guardedCount_i = ARTIFACT_EVENT_GUARD_ROGUE_COUNT;
                    goto artifactFight;
                } else {
                    guardedCount_i = 1;
                    sprintf(
                        gText,
                        "{Артефакт}\n\nНа прогалине вы "
                        "приметили древний артефакт."
                        " К сожалению, его сторожит "
                        "отряд %s. Не пугает ли вас сила "
                        "%s, и будете ли вы сражаться "
                            "за артефакт?",
                        gArmyNamesPlural[(guardedMonster_c)],
                        gArmyNamesPlural[(guardedMonster_c)]
                    );
                    NormalDialog(gText, NORMAL_DIALOG_CONFIRM, -1, -1, -1, 0, -1, 0, -1, 0);
                }
                if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
                artifactFight:
                    if (CombatMonsterEvent(
                            eventHero2,
                            guardedMonster_c,
                            guardedCount_i,
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
                        sprintf(
                            gText,
                            "Одержав победу, вы получили "
                                "свой трофей - %s.",
                            gArtifactNames[(artifact_g)]
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
                        "Главное в доблести - благоразумие,"
                            " поэтому вы решили пока что "
                            "воздержаться от этой битвы.",
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
            } else {
                switch (cell->m_objectMetadata & ARTIFACT_EVENT_MODE_MASK) {
                    case ARTIFACT_EVENT_MODE_WISDOM:
                        if (eventHero2->m_secondarySkills[(HERO_SKILL_WISDOM)]
                            != HERO_SKILL_LEVEL_NONE)
                            goto artifactPickup;
                        sprintf(
                            gText,
                            "{Аретфакт}\n\nВы нашли убогий "
                            "скит изможденного отшельника."
                            " Тот поведал вам, что его %s достанется "
                            "первому же встречному великому "
                                "мудрецу.",
                            gArtifactNames[(artifact_g)]
                        );
                        NormalDialog(gText, NORMAL_DIALOG_INFO, -1, -1, -1, 0, -1, 0, -1, 0);
                        break;

                    case ARTIFACT_EVENT_MODE_LEADERSHIP:
                        if (eventHero2->m_secondarySkills[(HERO_SKILL_LEADERSHIP)]
                            != HERO_SKILL_LEVEL_NONE)
                            goto artifactPickup;
                        sprintf(
                            gText,
                            "{Артефакт}\n\nВы навестили старого "
                            "солдата в его скромном жилище."
                            " Солдат поведал вам, что его "
                            "%s достанется первому настоящему "
                                "лидеру, которого он встретит.",
                            gArtifactNames[(artifact_g)]
                        );
                        NormalDialog(gText, NORMAL_DIALOG_INFO, -1, -1, -1, 0, -1, 0, -1, 0);
                        break;

                    case ARTIFACT_EVENT_MODE_PICKUP:
                    artifactPickup:
                        EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                        EventWindow(
                            -1,
                            NORMAL_DIALOG_INFO,
                            gArtifactEvent[(artifact_g)],
                            MAP_EVENT_REWARD_ARTIFACT,
                            (artifact_g),
                            -1,
                            0,
                            -1
                        );
                    giveArtifact:
                        GiveArtifact(eventHero2, artifact_g, 1, -1);
                        eraseObject_l = 1;
                        fizzleType_k = true;
                        break;

                    case ARTIFACT_EVENT_MODE_GOLD:
                        EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                        sprintf(
                            gText,
                            "{Артефакт}\n\nЛепрекон предлагает "
                                "вам артефакт - %s, всего за 2000 золотых."
                                " Купите его?",
                            gArtifactNames[(artifact_g)]
                        );
                        EventWindow(
                            -1,
                            NORMAL_DIALOG_CONFIRM,
                            gText,
                            MAP_EVENT_REWARD_ARTIFACT,
                            (artifact_g),
                            -1,
                            0,
                            -1
                        );
                        if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
                            if (gpGame->m_players[(eventHero2->m_owner)].m_resources[(RES_GOLD)]
                                >= ARTIFACT_EVENT_GOLD_COST) {
                                gpGame->m_players[(eventHero2->m_owner)].m_resources[(RES_GOLD)] -=
                                    ARTIFACT_EVENT_GOLD_COST;
                                goto giveArtifact;
                            }
                            NormalDialog(
                                "Вы и хотели бы расплатиться "
                                    "с лепреконом, но поняли, что "
                                    "сделка вам не по карману. Лепрекон "
                                    "затопал ногами и обиженно "
                                    "надулся.",
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
                                "Оскорбленный вашим отказом "
                                    "от его щедрого предложения,"
                                    " лекреком притопнул ногой "
                                    "и отвернулся от вас.",
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

                    case ARTIFACT_EVENT_MODE_RESOURCE_3:
                        EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                        sprintf(sphinxAnswer_a, gResourceNames[(artifactResourceType_k)]);
                        sphinxAnswer_a[0] = ToLowerCp1251(sphinxAnswer_a[0]);
                        sprintf(
                            gText,
                            "{Артефакт}\n\nЛепрекон предлагает "
                                "вам артефакт - %s за 2500 золотых "
                                "и просит %s - 3 единицы. Желаете "
                                "купить предмет?",
                            gArtifactNames[(artifact_g)],
                            sphinxAnswer_a
                        );
                        NormalDialog(
                            gText,
                            NORMAL_DIALOG_CONFIRM,
                            -1,
                            -1,
                            MAP_EVENT_REWARD_ARTIFACT,
                            (artifact_g),
                            -1,
                            0,
                            -1,
                            0
                        );
                        if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
                            if (gpGame->m_players[(eventHero2->m_owner)].m_resources[(RES_GOLD)]
                                    >= ARTIFACT_EVENT_RESOURCE_3_GOLD_COST
                                && gpGame->m_players[(eventHero2->m_owner)]
                                           .m_resources[(artifactResourceType_k)]
                                       >= ARTIFACT_EVENT_RESOURCE_3_AMOUNT) {
                                gpGame->m_players[(eventHero2->m_owner)].m_resources[(RES_GOLD)] -=
                                    ARTIFACT_EVENT_RESOURCE_3_GOLD_COST;
                                gpGame->m_players[(eventHero2->m_owner)]
                                    .m_resources[(artifactResourceType_k)] -=
                                    ARTIFACT_EVENT_RESOURCE_3_AMOUNT;
                                goto giveArtifact;
                            }
                            NormalDialog(
                                "Вы и хотели бы расплатиться "
                                    "с лепреконом, но поняли, что "
                                    "сделка вам не по карману. Лепрекон "
                                    "затопал ногами и обиженно "
                                    "надулся.",
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
                                "Оскорбленный вашим отказом "
                                    "от его щедрого предложения,"
                                    " лекреком притопнул ногой "
                                    "и отвернулся от вас.",
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
                        sprintf(sphinxAnswer_a, gResourceNames[(artifactResourceType_k)]);
                        sphinxAnswer_a[0] = ToLowerCp1251(sphinxAnswer_a[0]);
                        sprintf(
                            gText,
                            "{Артефакт}\n\nЛепрекон предлагает "
                                "вам артефакт - %s за 3000 золотых "
                                "и просит %s - 5 единиц. Желаете "
                                "купить предмет?",
                            gArtifactNames[(artifact_g)],
                            sphinxAnswer_a
                        );
                        NormalDialog(
                            gText,
                            NORMAL_DIALOG_CONFIRM,
                            -1,
                            -1,
                            MAP_EVENT_REWARD_ARTIFACT,
                            (artifact_g),
                            -1,
                            0,
                            -1,
                            0
                        );
                        if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
                            if (gpGame->m_players[(eventHero2->m_owner)].m_resources[(RES_GOLD)]
                                    >= ARTIFACT_EVENT_RESOURCE_5_GOLD_COST
                                && gpGame->m_players[(eventHero2->m_owner)]
                                           .m_resources[(artifactResourceType_k)]
                                       >= ARTIFACT_EVENT_RESOURCE_5_AMOUNT) {
                                gpGame->m_players[(eventHero2->m_owner)].m_resources[(RES_GOLD)] -=
                                    ARTIFACT_EVENT_RESOURCE_5_GOLD_COST;
                                gpGame->m_players[(eventHero2->m_owner)]
                                    .m_resources[(artifactResourceType_k)] -=
                                    ARTIFACT_EVENT_RESOURCE_5_AMOUNT;
                                goto giveArtifact;
                            }
                            NormalDialog(
                                "Вы и хотели бы расплатиться "
                                    "с лепреконом, но поняли, что "
                                    "сделка вам не по карману. Лепрекон "
                                    "затопал ногами и обиженно "
                                    "надулся.",
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
                                "Оскорбленный вашим отказом "
                                    "от его щедрого предложения,"
                                    " лекреком притопнул ногой "
                                    "и отвернулся от вас.",
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
                eventHero2->CheckLevel();
            }
            break;
        }

        case MAP_OBJECT_HERO_INTERACTION:
            DemobilizeCurrHero();
            otherHero_e = gpGame->GetHero(cell->m_objectMetadata);
            if (otherHero_e->m_owner == giCurPlayer) {
                HeroSwap(eventHero2, otherHero_e);
            } else {
                occupiedTown_b = NULL;
                if (otherHero_e->m_locationType
                    == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_CASTLE)) {
                    occupiedTown_b = gpGame->GetTown(otherHero_e->m_occupiedTown);
                    occupiedTown_b->m_occupyingHeroId = otherHero_e->m_id;
                }
                heroCombatResult_h = DoCombat(
                    x,
                    y,
                    eventHero2,
                    &eventHero2->m_army,
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
            break;

        case MAP_OBJECT_BOTTLE:
            if (cell->m_objectMetadata) {
                signExtra_k = reinterpret_cast<signEventExtra*>(ppMapExtra[cell->m_objectMetadata]);
                if (strlen(signExtra_k->text) > SIGN_MINIMUM_TEXT_LENGTH)
                    EventWindow(-1, NORMAL_DIALOG_INFO, signExtra_k->text, -1, 0, -1, 0, -1);
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
            fizzleType_k = true;
            eraseObject_l = 1;
            break;

        case MAP_OBJECT_SIGN:
            if (cell->m_objectMetadata) {
                signExtra_k = reinterpret_cast<signEventExtra*>(ppMapExtra[cell->m_objectMetadata]);
                if (strlen(signExtra_k->text) > SIGN_MINIMUM_TEXT_LENGTH)
                    EventWindow(-1, NORMAL_DIALOG_INFO, signExtra_k->text, -1, 0, -1, 0, -1);
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
            EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
            EventWindow(
                -1,
                NORMAL_DIALOG_CONFIRM,
                "{Пещера демона}\n\nВход в пещеру "
                    "зияет черной дырой, из которой "
                    "тянет тошнотворным сернистым "
                    "зловонием. Отважитесь ли "
                    "вы войти?",
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
                    "Если не считать следов ужасной "
                        "битвы, пещера пуста.",
                    -1,
                    0,
                    -1,
                    0,
                    -1
                );
                break;
            }

            monsterType_f = static_cast<CreatureType>(
                ((cell->m_objectMetadata & DAEMON_SERVANT_MASK) >> DAEMON_SERVANT_SHIFT)
                + DAEMON_SERVANT_BASE
            );
            sprintf(
                gText,
                "В пещере вы обнаружили грозного "
                    "до нелепости демона. \"Сегодня,"
                    "\" - прорычал он, - \"тебя ждут бой "
                    "и верная смерть. Но я позволю "
                    "тебе выбрать смерть. Ты можешь "
                    "драться со мной или же с моими "
                    "слугами. Предпочитаешь сразиться "
                    "с моими слугами?\""
            );
            EventWindow(-1, NORMAL_DIALOG_CONFIRM, gText, -1, 0, -1, 0, -1);
            if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
                if (CombatMonsterEvent(
                        eventHero2,
                        monsterType_f,
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
                        "Одолев слуг демона, вы нашли "
                            "клад в размере 2500 золотых.",
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
                        "Демон выкрикнул свой вызов "
                            "и бросился в бой! После краткой,"
                            " но отчаянной схватки вы прикончили "
                            "чудовище и получили 1000 очков "
                            "опыта.",
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
                    if (eventHero2->NumArtifacts() == EVENT_ARTIFACT_CAPACITY)
                        goto daemonExperienceGold;
                    if (gpGame->GetRandomArtifactId(ARTIFACT_LEVEL_RANDOM, true) == -1)
                        goto daemonExperienceGold;
                    GiveExperience(eventHero2, DAEMON_EXPERIENCE, 0);
                    eventValue1 = (GiveRandomArtifact(eventHero2));
                    sprintf(
                        gText,
                        "Демон выкрикнул свой вызов "
                            "и бросился в бой! После непродолжительной,"
                            " но отчаянной битвы вы прикончили "
                            "чудовище, а в глубине пещеры "
                            "нашли %s.",
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
                        "Демон выкрикнул свой вызов "
                            "и бросился в бой! После непродолжительной,"
                            " но отчаянной схватки вы прикончили "
                            "чудовище, получив 1000 очков "
                            "опыта и 2500 золотых.",
                        (RES_GOLD),
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
                        "Демон набросился на вас и "
                            "вцепился когтями в горло "
                            "прежде, чем вы обнажили меч."
                            " \"Твоя жизнь в моих руках,\" -"
                            " прорычал он. - \"Я верну ее тебе "
                            "за 2500 золотых.\"",
                        -1,
                        0,
                        -1,
                        0,
                        -1
                    );
                    if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
                        if (gpGame->m_players[(eventHero2->m_owner)].m_resources[(RES_GOLD)]
                            < DAEMON_GOLD) {
                            EventWindow(
                                -1,
                                NORMAL_DIALOG_INFO,
                                "Поняв, что у вас нет 2500 золотых,"
                                    " демон растерзал вас своими "
                                    "клыками, и последнее, что вы "
                                    "видели, была алая пелена перед "
                                    "глазами.",
                                -1,
                                0,
                                -1,
                                0,
                                -1
                            );
                            HeroLoses(eventHero2);
                        } else {
                            gpGame->m_players[(eventHero2->m_owner)].m_resources[(RES_GOLD)] -=
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
                "{Кораблекрушение}\n\nГниющий "
                    "остов огромного пиратского "
                    "корабля зловеще поскрипывает,"
                    " покачиваемый прибоем на "
                    "скалах. Желаете обыскать "
                    "обломки?",
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
                            "Совладав с призраками, вы "
                                "потратили несколько часов "
                                "на рытье в мусоре, но так ничего "
                                "и не нашли. Столь неблаговидное "
                                "занятие снизило маораль вашей "
                                "армии.",
                            NORMAL_DIALOG_MORALE_PENALTY,
                            0,
                            -1,
                            0,
                            -1
                        );
                        if (!(((eventHero2->m_eventFlags) & (HERO_EVENT_SHIPWRECK)))) {
                            eventHero2->m_eventFlags =
                                eventHero2->m_eventFlags | HERO_EVENT_SHIPWRECK;
                            eventHero2->m_morale--;
                        }
                        break;
                    default:
                        if (GhostEvent(
                                eventHero2,
                                cell,
                                "Одолев призраков, вы обыскали "
                                    "останки корабля и кое-что "
                                    "нашли!",
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
                        if (!(((eventHero2->m_eventFlags) & (HERO_EVENT_GRAVEYARD)))) {
                            eventHero2->m_eventFlags =
                                eventHero2->m_eventFlags | HERO_EVENT_GRAVEYARD;
                            eventHero2->m_morale--;
                        }
                        break;
                    default:
                        zombieCell_o = GetCell(
                            x - normalDirTable[(eventHero2->m_direction)].x,
                            y - normalDirTable[(eventHero2->m_direction)].y
                        );
                        if (ZombieEvent(
                                eventHero2,
                                zombieCell_o,
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
                "{Брошенный корабль}\n\nГниющий "
                    "остов огромного пиратского "
                    "корабля зловеще скрипит, "
                    "покачиваемый волнами на скалах."
                    " Желаете обыскать корабль?",
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
                            "Усмирив скелетов, вы потратили "
                                "несколько часов на поиски "
                                "среди гнилых обломков - и впустую."
                                " Столь грязная работа плохо "
                                "сказалась на морали вашей "
                                "армии.",
                            NORMAL_DIALOG_MORALE_PENALTY,
                            0,
                            -1,
                            0,
                            -1
                        );
                        if (!(((eventHero2->m_eventFlags) & (HERO_EVENT_DERELICT_SHIP)))) {
                            eventHero2->m_eventFlags =
                                eventHero2->m_eventFlags | HERO_EVENT_DERELICT_SHIP;
                            eventHero2->m_morale--;
                        }
                        break;
                    default:
                        skeletonCell_e = GetCell(
                            x - normalDirTable[(eventHero2->m_direction)].x,
                            y - normalDirTable[(eventHero2->m_direction)].y
                        );
                        if (SkeletonEvent(
                                eventHero2,
                                skeletonCell_e,
                                "Совладав со скелетами, вы "
                                    "обыскали обломки и кое-что "
                                    "нашли!",
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
                "Вы нашли пирамиду древнего "
                    "великого царя. Искушение "
                    "поискать в ней сокровища "
                    "велико, но вы слышали все эти "
                    "сказки об ужасных проклятиях "
                    "и неупокоенных стражей. Хотите "
                    "обследовать пирамиду?",
                -1,
                0,
                -1,
                0,
                -1
            );
            if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
                if (!cell->m_objectMetadata) {
                    NormalDialog(
                        "Вы нашли пирамиду древнего "
                            "великого царя. Первое же обследование "
                            "пирамиды показало, что в ней "
                            "абсолютно ничего нет.",
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
                    if (!(((eventHero2->m_eventFlags) & (HERO_EVENT_PYRAMID)))) {
                        eventHero2->m_eventFlags = HeroEventFlag(
                            static_cast<i32>(eventHero2->m_eventFlags) | (HERO_EVENT_PYRAMID)
                        );
                        eventHero2->m_luck -= PYRAMID_LUCK_PENALTY;
                    }
                    break;
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
                                    sprintf(
                            eventText_b,
                            "%s'%s'.  ",
                            "Одолев чудовищ, вы расшифровали "
                                "иероглифы на стене, сообщающие "
                                "секрет заклинания - ",
                            gSpellNames[cell->m_objectMetadata - 1]
                        );
                        if (!eventHero2->HasArtifact(ARTIFACT_MAGIC_BOOK)) {
                            strcat(
                                eventText_b,
                                "  К сожалению, у вас нет Волшебной "
                                    "книги, чтобы записать в нее "
                                    "это заклинание."
                            );
                            EventWindow(-1, NORMAL_DIALOG_INFO, eventText_b, -1, 0, -1, 0, -1);
                        } else if (eventHero2->m_secondarySkills[(HERO_SKILL_WISDOM)]
                                   >= HERO_SKILL_LEVEL_EXPERT) {
                            eventHero2->AddSpell(
                                static_cast<SpellType>(cell->m_objectMetadata - 1),
                                eventHero2->Stats(HERO_PRIMARY_KNOWLEDGE)
                            );
                            EventWindow(
                                -1,
                                NORMAL_DIALOG_INFO,
                                eventText_b,
                                NORMAL_DIALOG_SPELL,
                                cell->m_objectMetadata - 1,
                                -1,
                                0,
                                -1
                            );
                        } else {
                            strcat(
                                eventText_b,
                                "  К сожалению, у вас недостаточно "
                                    "мудрости, чтобы понять заклинание,"
                                    " ивы не можете выучить его."
                            );
                            EventWindow(-1, NORMAL_DIALOG_INFO, eventText_b, -1, 0, -1, 0, -1);
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
                "Вы обнаружили заброшенную "
                    "золотую шахту. Кажется, она "
                    "населена призраками. Желаете "
                    "войти внутрь?",
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
                        "Вы разбили призраков и смогли "
                            "наладить добычу золота.",
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
            eraseObject_l = BarrierEvent(cell, eventHero2);
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

    UpdateRadar(1, 0);
    UpdateHeroLocators(1, 1);
    UpdateTownLocators(1, 1);
    UpdBottomView(1, 1, 1);
    if (eraseObject_l) {
        EraseObj(cell, x, y);
        FizzleCenter(fizzleType_k);
    } else {
        CompleteDraw(0);
    }
    UpdateScreen(0, 0);
    gpSoundManager->SwitchAmbientMusic(giTerrainToMusicTrack[(m_currentTerrain)]);
    WaitEndSample(&eventSample_f, -1);
    CheckEndGame(END_GAME_FORCE_NONE, false);
}

void advManager::EraseObj(class mapCell* cell, i32 x, i32 y) {
    b32 erased [[maybe_unused]] = false;
    mapCellExtra* extras_h[NEIGHBOR_COUNT];
    mapCellExtra* extra_l;
    mapCell* cells_h[NEIGHBOR_COUNT];
    i32 frame_k = NO_FRAME;
    mapCell* currentCell_d;
    b8 isWide_l = false;
    i32 i_g;
    i32 unused_m [[maybe_unused]][ERASE_COORDINATE_COUNT];
    i32 cellX_l;
    i32 cellY_o;
    i32 changed_f [[maybe_unused]];

    for (i_g = 0; i_g < NEIGHBOR_COUNT; i_g++) {
        cells_h[i_g] = NULL;
        extras_h[i_g] = NULL;
    }

    erased = true;
    if (cell->m_objectTileset == TILESET_OBJNARTI)
        frame_k = cell->m_objectIndex - 1;
    if (cell->m_objectTileset == TILESET_X_LOC3)
        frame_k = cell->m_objectIndex - 1;
    if (cell->m_objectTileset == TILESET_X_LOC2 && cell->m_objectIndex == 9) {
        frame_k = 9;
        isWide_l = true;
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

        currentCell_d->m_flags |= (MAP_CELL_OBJECT_SHADOW_ONLY);
    cellDone:
        ;
        changed_f = 0;
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
    colorIndex &= COLOR_MASK;
    i32 passwordIndex = cell->m_objectMetadata;
    passwordIndex >>= PASSWORD_SHIFT;
    char word[INPUT_BUFFER_SIZE];

    sprintf(
        gText,
        "Дорогу вам преграждает %s "
            "магический барьер. Руны "
            "на нем гласят: \"Скажи слово "
            "и сможешь пройти.\"",
        xBarrierColor[colorIndex]
    );
    GetDataEntry(gText, word, INPUT_LENGTH, NULL, 0, 1);
    if (StrEqNoCase(word, xPasswordStrings[passwordIndex])
        && (gpCurPlayer->m_barrierTents & (1 << colorIndex))) {
        EventSound(cell->m_triggerType & MAP_TRIGGER_TYPE_MASK, colorIndex, &eventSample);
        NormalDialog(
            "Едва вы произнесли "
                "волшебное слово, как "
                "сверкающий барьер исчез "
                "в пустоте.",
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
            "Вы произнесли слово, но ничего не произошло.",
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

i8 StrEqNoCase(const char* firstString, const char* sndString) {
    const char* firstPtr = firstString;
    const char* secondPtr = sndString;
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

void advManager::PasswordEvent(mapCell* cell, hero*) {
    SAMPLE2 playSample = NULL;
    i32 color = cell->m_objectMetadata;
    color &= COLOR_MASK;
    i32 passwordIndex = cell->m_objectMetadata;
    passwordIndex >>= PASSWORD_SHIFT;

    EventSound(cell->m_triggerType & MAP_TRIGGER_TYPE_MASK, color, &playSample);


    sprintf(
        gText,
        "Вы зашли в шатер и увидели пожилую женщину с магическим камнем в руках. Посмотрев на вас, она сказала: \"Странствуя, я познала тайную магию. Великий оракул даровал мне знания и у меня есть ответ на твой вопрос.  Волшебное слово, снимающее %s барьер - '%s'.\"",
        xBarrierColor[color],
        xPasswordStrings[passwordIndex]
    );
    NormalDialog(gText, NORMAL_DIALOG_INFO, -1, -1, -1, 0, -1, 0, -1, 0);
    gpCurPlayer->m_barrierTents |= 1 << color;
}

void advManager::GenericSiteEvent(mapCell* cell, hero* eventHero) {
    i32 primaryStat15;
    i32 cursedArtifactCount2;
    i32 siteLevel8 [[maybe_unused]];
    SAMPLE2 eventSample9;
    i32 index8;
    GenericSiteType siteType4;
    i32 currentSiteType10;
    i32 mapX37;
    i32 unusedTwo1;
    i32 mapY14;
    mapCell* currentCell36;
    i32 unusedSite [[maybe_unused]];
    i32 unusedOne18;
    i32 oldQuantity3;
    i8 stableResult26;
    CreatureType creatureType;
    i32 experience11;

    cursedArtifactCount2 = 0;
    eventSample9 = NULL;
    siteType4 = static_cast<GenericSiteType>(cell->m_objectMetadata);
    siteType4 = static_cast<GenericSiteType>((siteType4) & GENERIC_SITE_TYPE_MASK);
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
                    cell->m_triggerType & MAP_TRIGGER_TYPE_MASK, (siteType4), &eventSample9
                );
                if (cursedArtifactCount2 == 1) {
                    sprintf(
                        gText,
                        "Едва вы вошли в башню алхимика, как на встречу вам вышел хромой и седой мужчина. Он проверил ваши вещи и заметил, что среди них есть одна проклятая вещь. За 750 золотых алхимик согласен забрать ее у вас. Вы заплатите?"
                    );
                } else {
                    sprintf(
                        gText,
                        " Едва вы вошли в башню алхимика, как на встречу вам вышел хромой и седой мужчина. Он проверил ваши вещи и заметил, что среди них %d проклятые вещи. За 750 золотых алхимик согласен забрать их у вас. Вы заплатите?",
                        cursedArtifactCount2
                    );
                }
                NormalDialog(gText, NORMAL_DIALOG_CONFIRM, -1, -1, -1, 0, -1, 0, -1, 0);
                if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
                    if (gpCurPlayer->m_resources[(RES_GOLD)] >= SITE_ALCHEMIST_COST) {
                        for (index8 = 0; index8 < HERO_ARTIFACT_SLOT_COUNT; index8++) {
                            if (IsCursedItem(eventHero->m_artifacts[index8])) {
                                GiveTakeArtifactStat(
                                    eventHero, eventHero->m_artifacts[index8], EVENT_ARTIFACT_TAKE
                                );
                                eventHero->m_artifacts[index8] = ARTIFACT_NONE;
                            }
                        }
                        gpCurPlayer->m_resources[(RES_GOLD)] -= SITE_ALCHEMIST_COST;
                    } else {
                        NormalDialog(
                            "Вы слышите голос из-за запертой двери: \"У вас недостаточно золота для моих услуг.\"",
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
                    "Вы слышите голос с вершины башни: \"Убирайтесь! Я не могу вам помочь!\"",
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
            if ((((eventHero->m_eventFlags) & (HERO_EVENT_ARENA)))) {
                NormalDialog(
                    "Стража Арены преградила вам дорогу и не пустила внутрь.",
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
                    cell->m_triggerType & MAP_TRIGGER_TYPE_MASK, (siteType4), &eventSample9
                );
                eventHero->m_eventFlags = HeroEventFlag(
                    static_cast<i32>(eventHero->m_eventFlags) | (HERO_EVENT_ARENA)
                );
                primaryStat15 = DoArenaDialog();
                eventHero->m_primaryStats[primaryStat15]++;
            }
            break;

        case GENERIC_SITE_MERMAID:
            if ((((eventHero->m_eventFlags) & (HERO_EVENT_MERMAID)))) {
                NormalDialog(
                    "Русалки молчаливо дали вам понять, чтобы вы приходили в другой раз, тогда они благословят вас.",
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
                    cell->m_triggerType & MAP_TRIGGER_TYPE_MASK, (siteType4), &eventSample9
                );
                eventHero->m_eventFlags = HeroEventFlag(
                    static_cast<i32>(eventHero->m_eventFlags) | (HERO_EVENT_MERMAID)
                );
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
            EventSound(cell->m_triggerType & MAP_TRIGGER_TYPE_MASK, (siteType4), &eventSample9);
            NormalDialog(
                "Вы зашли в ветхую хибару и заговорили с магом, живущим тут. Он поведал вам о местах, которые ему доводилось видеть. Это может пригодиться вам в ваших путешествиях.",
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
                "Кажется, этот глаз внимательно изучает окрестности.",
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
            if ((((eventHero->m_eventFlags) & (HERO_EVENT_SIRENS)))) {
                NormalDialog(
                    "Вы приказали своей команде залить уши воском, прежде чем подплыть к сиренам, чтобы послушать их пение, которое запросто может погубить всех в морской пучине.",
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
                                gMonsterDatabase[(creatureType)].hitPoints
                                * (oldQuantity3 - eventHero->m_army.m_quantities[index8]);
                        }
                    }
                }
                if (experience11 != 0) {
                    EventSound(
                        cell->m_triggerType & MAP_TRIGGER_TYPE_MASK,
                        (siteType4),
                        &eventSample9
                    );
                    sprintf(
                        gText,
                        "Над камнями раздалась жуткая песня сирен. Многие из вашей команды росились в воду действием этих чар и утонули в морской пучине. Для вас это был хороший урок, давший %d очков опыта.",
                        experience11
                    );
                    NormalDialog(gText, NORMAL_DIALOG_INFO, -1, -1, -1, 0, -1, 0, -1, 0);
                    GiveExperience(eventHero, experience11, 1);
                } else {
                    NormalDialog(
                        "Едва сирены затянули свою ужасную песню, как ваша команда посильнее налегла на весла и увела подальше корабль в открыто море.",
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
                    static_cast<i32>(eventHero->m_eventFlags) | (HERO_EVENT_SIRENS)
                );
            }
            break;

        case GENERIC_SITE_STABLES:
            unusedOne18 = STABLE_VISIT_MOBILITY;
            unusedTwo1 = STABLE_VISIT_UPGRADE;
            stableResult26 = STABLE_VISIT_NONE;
            if ((((eventHero->m_eventFlags) & (HERO_EVENT_STABLES))) == 0) {
                eventHero->m_eventFlags = HeroEventFlag(
                    static_cast<i32>(eventHero->m_eventFlags) | (HERO_EVENT_STABLES)
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
                    cell->m_triggerType & MAP_TRIGGER_TYPE_MASK, (siteType4), &eventSample9
                );
            }
            sprintf(gText, xStableText[(stableResult26)]);
            if ((((stableResult26) & (STABLE_VISIT_UPGRADE)))) {
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

    index = (recruitSiteType);
    if (quantity == 0) {
        EventWindow(-1, NORMAL_DIALOG_INFO, xRecruitEmpty[index], -1, 0, -1, 0, -1);
    } else {
        EventSound(cell->m_triggerType & MAP_TRIGGER_TYPE_MASK, quantity, &recruitSample);
        EventWindow(-1, NORMAL_DIALOG_CONFIRM, xRecruitBuy[index], -1, 0, -1, 0, -1);
        if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
            ExpansionRecruitEvent(eventHero, creatureType1, &quantity);
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
    if (gpGame->m_availableHeroes[heroId] != EVENT_JAILED_HERO) {
        NormalDialog(
            "Тюремщик сказал вам, "
                "что герой, томившийся "
                "тут в плену был освобожден "
                "его королем.",
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
            "У вас уже 8 героев. "
                "К сожалению, вам придется "
                "оставить томиться "
                "этого героя в темнице "
                "еще неопределенное "
                "время.",
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
        "С ослепительной отвагой "
            "вы ворвались в местную "
            "темницу и освободили "
            "героя, томящегося "
            "здесь в плену. "
            "В благодарность "
            "он поклялся служить "
            "вам.",
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
    i32 eventWindowUnused4 [[maybe_unused]];
    i32 eventWindowUnused3 [[maybe_unused]];
    i32 eventDone [[maybe_unused]];
    i32 eventWindowUnused8 [[maybe_unused]];
    i32 eventWindowUnused7 [[maybe_unused]];
    i32 eventWindowUnused6 [[maybe_unused]];
    i32 eventWindowUnused5 [[maybe_unused]];
    char eventText[EVENT_TEXT_BUFFER_SIZE];
    i32 unusedStyle9 [[maybe_unused]];

    eventDone = 0;
    unusedStyle9 = 1;

    if (eventId >= 0 && eventId < EVENT_TEXT_COUNT)
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

    for (artifactSlot = 0; artifactSlot < EVENT_ARTIFACT_SLOT_COUNT; artifactSlot++) {
        if (eventHero->m_artifacts[artifactSlot] == ARTIFACT_NONE) {
            break;
        }
    }

    if (artifactSlot == EVENT_ARTIFACT_SLOT_COUNT) {
        return (ARTIFACT_NONE);
    }

    eventHero->m_artifacts[artifactSlot] = artifact;
    eventHero->m_artifactExtra[artifactSlot] = artifactExtra;
    GiveTakeArtifactStat(eventHero, artifact, 0);
    eventHero->CheckAnduranPieces(0);
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
        GiveArtifact(eventHero, artifactId, true, -1);
    return artifactId;
}

i32 advManager::GiveExperience(hero* eventHero, i32 experience, i32 checkLevel) {
    i32 oldLevel;
    i32 unusedLevel2 [[maybe_unused]];
    i32 unusedLevel1 [[maybe_unused]];
    i32 newLevel1;
    i32 levelGap1 [[maybe_unused]];

    oldLevel = eventHero->GetLevel(eventHero->m_experience);
    eventHero->m_level = static_cast<i16>(oldLevel);
    eventHero->m_experience += experience;
    newLevel1 = eventHero->GetLevel(eventHero->m_experience);
    if (checkLevel)
        eventHero->CheckLevel();
    return newLevel1 - oldLevel;
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
    i16 availableCount = static_cast<i16>(cell->m_objectMetadata);
    recruitUnit* recruitWindow =
        new recruitUnit(&eventHero->m_army, creatureType, &availableCount);
    i32 eventResult [[maybe_unused]];

    if (recruitWindow == NULL)
        MemError();
    gpExec->DoDialog(recruitWindow);
    delete recruitWindow;
    cell->m_objectMetadata = static_cast<u16>(availableCount);
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
        if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
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
    i32 placement4[MONSTER_ARMY_SLOTS];
    i32 combatUnused [[maybe_unused]];
    i32 lastCount;
    i32 groupCount;
    i32 stackCount;
    CreatureType savedTypes[MONSTER_ARMY_SLOTS];
    CombatResult battleOutcome;
    i32 savedCounts[MONSTER_ARMY_SLOTS];
    i32 stackIdx;
    i32 combatUnused0 [[maybe_unused]];

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

    memset(gpMonGroup->m_creatureTypes, (CREATURE_NONE), MONSTER_ARMY_SLOTS);
    memset(gpMonGroup->m_creatureCounts, 0, MONSTER_ARMY_SLOTS * sizeof(i16));
    stackCount = MONSTER_ARMY_SLOTS - secondaryStacks - tertiaryStacks;
    if (stackCount < 1)
        stackCount = 1;
    groupCount = 0;
    SRand(combatX + combatY);
    if (stackCount == MONSTER_ARMY_SLOTS
        && (((gMonsterDatabase[(monsterType)].attributes) & (MONSTER_ATTRIBUTE_RANGED))) == 0) {
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

void GiveTakeArtifactStat(hero* targetHero, ArtifactType artifact, b32 take) {
    i32 stats[EVENT_ARTIFACT_PRIMARY_STAT_COUNT];
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

    for (i = 0; i < EVENT_ARTIFACT_PRIMARY_STAT_COUNT; i++) {
        targetHero->m_primaryStats[i] += (take == EVENT_ARTIFACT_TAKE ? -1 : 1) * stats[i];
        if (i == (HERO_PRIMARY_KNOWLEDGE) && take == EVENT_ARTIFACT_TAKE) {
            maxSpellPoints =
                targetHero->Stats(HERO_PRIMARY_KNOWLEDGE) * EVENT_ARTIFACT_SPELL_POINT_MULTIPLIER;
            if (targetHero->m_spellPoints > maxSpellPoints)
                targetHero->m_spellPoints = static_cast<i16>(maxSpellPoints);
        }
    }
}

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
                        if (gbThisNetHumanPlayer[(sourceHero->m_owner)]
                            || gbThisNetHumanPlayer[(destinationHero->m_owner)]) {
                            sprintf(
                                gText,
                                "Это %s! Едва вы добрались до "
                                    "артефакта, как предмет "
                                    "мистическим образом исчез."
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
    destinationHero->CheckAnduranPieces(0);
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
    i32 groupValues [[maybe_unused]][MONSTER_ARMY_SLOTS];
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
        for (slotNo = 0; slotNo < MONSTER_ARMY_SLOTS; slotNo++) {
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
        WaitEndSample(&fizzleSample, -1);
    }
}

void advManager::DoAIEvent(mapCell* cell, hero* eventHero, i32 x, i32 y) {
    i32 secondaryAmount_j [[maybe_unused]];
    i32 secondaryReward_k [[maybe_unused]];
    i32 primaryReward_e [[maybe_unused]];
    i32 primaryAmount_j [[maybe_unused]];
    i32 wellSpellPoints_o;
    i32 springSpellPoints_j;
    float defenderLoss_k;
    float attackerLoss_c;
    i32 heroLevel_e;
    i32 levelExperience_g;
    i32 creatureCosts_a[(RES_COUNT)];
    i32 spellPower_j;
    i32 adjacentMonster_j;
    i32 savedShowIt_e [[maybe_unused]];
    boatRecord* boat_k;
    i32 exitCount;
    ResourceType resourceType_a;
    i32 artifactGuardResult_e;
    i32 exitY_d;
    i32 exitX;
    ArtifactType artifact_g;
    i32 artifactResource_p;
    hero* otherHero_e;
    CombatResult heroCombatResult_h;
    i32 heroInteractionResult;
    CreatureType artifactGuardCount_b;
    i32 survivingCount_a;
    mapEventExtra* eventExtra_o;
    MapObjectType eventType_g;
    i32 eventWork_o [[maybe_unused]][(RES_COUNT)];
    i32 eraseObject_l;
    i32 battleResult_l;
    b32 creatureFlag_l;
    i32 oldPlayer_o;
    i32 purchaseCost_i;
    i32 battleWon_j;
    i32 pyramidBattleValue_l;
    i32 unusedResult_m [[maybe_unused]];
    i32 index_h;
    i32 purchaseCount_o;
    CreatureType creatureType_i;
    i32 purchaseValue_a;
    i32 abandonedMineValue_f;
    town* occupiedTown_b;
    playerData* oldPlayerData_h;
    i32 combatResult_d;

    occupiedTown_b = NULL;
    eventType_g = cell->m_triggerType & MAP_TRIGGER_TYPE_MASK;
    eraseObject_l = 0;
    unusedResult_m = 0;
    oldPlayer_o = giCurPlayer;
    oldPlayerData_h = gpCurPlayer;

    if (x == eventHero->m_destinationX && y == eventHero->m_destinationY) {
        eventHero->m_destinationY = -1;
        eventHero->m_destinationX = -1;
    }
    --eventHero->m_remainingMobility;
    switch (eventType_g) {
        case MAP_OBJECT_COAST:
            if ((((eventHero->m_eventFlags) & (HERO_EVENT_EMBARKED)))) {
                eventHero->m_eventFlags = HeroEventFlag(
                    static_cast<i32>(eventHero->m_eventFlags) & ~(HERO_EVENT_EMBARKED)
                );
                eventHero->m_remainingMobility = 0;
                eventHero->m_direction = m_cursorDirection;
                m_cursorType = eventHero->m_cursorType;
                m_cursorFrame = GetCursorBaseFrame(m_cursorDirection);
                m_cursorActive = true;
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
                static_cast<i32>(eventHero->m_eventFlags) | (HERO_EVENT_EMBARKED)
            );
            eventHero->m_remainingMobility = 0;
            boat_k->heroId = eventHero->m_id;
            boat_k->owner = eventHero->m_owner;
            m_cursorType = HERO_TYPE_BOAT;
            m_cursorDirection = boat_k->direction;
            m_cursorFrame = GetCursorBaseFrame(m_cursorDirection);
            m_cursorActive = true;
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
                for (index_h = (SPELL_SET_EARTH_GUARDIAN);
                     index_h < (SPELL_SET_WATER_GUARDIAN);
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
            eraseObject_l = 1;
            break;

        case MAP_OBJECT_WATERING_HOLE:
            if ((((eventHero->m_eventFlags) & (HERO_EVENT_WATERING_HOLE))) == 0) {
                eventHero->m_mobility += WATERING_HOLE_MOBILITY_BONUS;
                eventHero->m_remainingMobility += WATERING_HOLE_MOBILITY_BONUS;
                eventHero->m_eventFlags = HeroEventFlag(
                    static_cast<i32>(eventHero->m_eventFlags) | (HERO_EVENT_WATERING_HOLE)
                );
                ++eventHero->m_morale;
            }
            break;

        case MAP_OBJECT_BUOY:
            if ((((eventHero->m_eventFlags) & (HERO_EVENT_BUOY))) == 0) {
                eventHero->m_eventFlags = HeroEventFlag(
                    static_cast<i32>(eventHero->m_eventFlags) | (HERO_EVENT_BUOY)
                );
                ++eventHero->m_morale;
            }
            break;

        case MAP_OBJECT_FAERIE_RING:
            if ((((eventHero->m_eventFlags) & (HERO_EVENT_FAERIE_RING))) == 0) {
                eventHero->m_eventFlags = HeroEventFlag(
                    static_cast<i32>(eventHero->m_eventFlags) | (HERO_EVENT_FAERIE_RING)
                );
                ++eventHero->m_luck;
            }
            break;

        case MAP_OBJECT_IDOL:
            if ((((eventHero->m_eventFlags) & (HERO_EVENT_IDOL))) == 0) {
                eventHero->m_eventFlags = HeroEventFlag(
                    static_cast<i32>(eventHero->m_eventFlags) | (HERO_EVENT_IDOL)
                );
                ++eventHero->m_luck;
            }
            break;

        case MAP_OBJECT_FOUNTAIN:
            if ((((eventHero->m_eventFlags) & (HERO_EVENT_FOUNTAIN))) == 0) {
                eventHero->m_eventFlags = HeroEventFlag(
                    static_cast<i32>(eventHero->m_eventFlags) | (HERO_EVENT_FOUNTAIN)
                );
                ++eventHero->m_luck;
            }
            break;

        case MAP_OBJECT_OASIS:
            if ((((eventHero->m_eventFlags) & (HERO_EVENT_OASIS))) == 0) {
                eventHero->m_eventFlags = HeroEventFlag(
                    static_cast<i32>(eventHero->m_eventFlags) | (HERO_EVENT_OASIS)
                );
                ++eventHero->m_morale;
                eventHero->m_mobility += OASIS_MOBILITY_BONUS;
                eventHero->m_remainingMobility += OASIS_MOBILITY_BONUS;
            }
            break;

        case MAP_OBJECT_TEMPLE:
            if ((((eventHero->m_eventFlags) & (HERO_EVENT_TEMPLE))) == 0) {
                eventHero->m_eventFlags = HeroEventFlag(
                    static_cast<i32>(eventHero->m_eventFlags) | (HERO_EVENT_TEMPLE)
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
                        index_h = cell->m_objectMetadata & WAGON_ARTIFACT_MASK;
                        GiveArtifact(eventHero, ArtifactType(index_h), 1, -1);
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
                    1,
                    -1
                );
                GiveResource(eventHero, RES_GOLD, EVENT_SEA_CHEST_ARTIFACT_GOLD);
            } else if (cell->m_objectMetadata != 0) {
                GiveResource(eventHero, RES_GOLD, EVENT_SEA_CHEST_GOLD);
            }
            eraseObject_l = 1;
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
            eraseObject_l = 1;
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
            eraseObject_l = 1;
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
            resourceType_a = static_cast<ResourceType>(cell->m_objectIndex / 2);
            GiveResource(
                eventHero,
                resourceType_a,
                resourceType_a == RES_GOLD ? cell->m_objectMetadata * CAMPFIRE_GOLD_MULTIPLIER
                                           : cell->m_objectMetadata
            );
            eraseObject_l = 1;
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
            creatureFlag_l = false;
            goto creaturePurchase;
        case MAP_OBJECT_RUINS:
            creatureType_i = CREATURE_MEDUSA;
            creatureFlag_l = false;
            goto creaturePurchase;
        case MAP_OBJECT_TROLL_BRIDGE:
            if (cell->m_objectMetadata & DWELLING_GUARDED_FLAG)
                break;
            creatureType_i = CREATURE_TROLL;
            creatureFlag_l = false;
            goto creaturePurchase;
        case MAP_OBJECT_CITY_OF_DEAD:
            if (cell->m_objectMetadata & DWELLING_GUARDED_FLAG)
                break;
            creatureType_i = CREATURE_POWER_LICH;
            creatureFlag_l = false;
            goto creaturePurchase;
        case MAP_OBJECT_DRAGON_CITY:
            if (cell->m_objectMetadata & DWELLING_GUARDED_FLAG)
                break;
            creatureType_i = CREATURE_RED_DRAGON;
            creatureFlag_l = false;
            goto creaturePurchase;
        case MAP_OBJECT_HALFLING_HOLE:
            creatureType_i = CREATURE_HALFLING;
            creatureFlag_l = true;
            goto creaturePurchase;
        case MAP_OBJECT_ANCIENT_LAMP:
            creatureType_i = CREATURE_GENIE;
            creatureFlag_l = false;
            goto creaturePurchase;
        case MAP_OBJECT_WAGON_CAMP:
            creatureType_i = CREATURE_ROGUE;
            creatureFlag_l = false;
            goto creaturePurchase;
        case MAP_OBJECT_DESERT_TENT:
            creatureType_i = CREATURE_NOMAD;
            creatureFlag_l = false;
            goto creaturePurchase;
        case MAP_OBJECT_WATCH_TOWER:
            creatureType_i = CREATURE_ORC;
            creatureFlag_l = true;
            goto creaturePurchase;
        case MAP_OBJECT_TREE_HOUSE:
            creatureType_i = CREATURE_SPRITE;
            creatureFlag_l = true;
            goto creaturePurchase;
        case MAP_OBJECT_ARCHER_HOUSE:
            creatureType_i = CREATURE_ARCHER;
            creatureFlag_l = true;
            goto creaturePurchase;
        case MAP_OBJECT_GOBLIN_HUT:
            creatureType_i = CREATURE_GOBLIN;
            creatureFlag_l = true;
            goto creaturePurchase;
        case MAP_OBJECT_PEASANT_HUT:
            creatureType_i = CREATURE_PEASANT;
            creatureFlag_l = true;
            goto creaturePurchase;
        case MAP_OBJECT_DWARF_COTTAGE:
        case MAP_OBJECT_SIRENS:
            creatureType_i = CREATURE_DWARF;
            creatureFlag_l = true;
            goto creaturePurchase;
        case MAP_OBJECT_CAVE:
            creatureType_i = CREATURE_CENTAUR;
            creatureFlag_l = true;
            goto creaturePurchase;
        case MAP_OBJECT_EXCAVATION:
            creatureType_i = CREATURE_SKELETON;
            creatureFlag_l = true;
            goto creaturePurchase;

        creaturePurchase:
            if (cell->m_objectMetadata != 0) {
                gpPhilAI->EvaluateOneTimeCreaturePurchase(
                    creatureType_i,
                    cell->m_objectMetadata,
                    creatureFlag_l,
                    purchaseCount_o,
                    purchaseCost_i,
                    purchaseValue_a
                );
                if (purchaseCount_o > 0) {
                    gpGame->GiveArmy(
                        &eventHero->m_army,
                        creatureType_i,
                        purchaseCount_o,
                        purchaseValue_a
                    );
                    cell->m_objectMetadata -= purchaseCount_o;
                    if (creatureFlag_l == 0) {
                        GetMonsterCost(creatureType_i, creatureCosts_a);
                        for (index_h = 0; index_h < (RES_COUNT); ++index_h)
                            gpCurPlayer->m_resources[index_h] -=
                                purchaseCount_o
                                * creatureCosts_a[index_h];
                    }
                }
            }
            if (cell->m_objectMetadata == 0 && eventType_g == MAP_OBJECT_ANCIENT_LAMP)
                eraseObject_l = 1;
            break;

        case MAP_OBJECT_MONSTER:
            ComputerMonsterInteract(cell, eventHero, &eraseObject_l);
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
                        if (gpCurPlayer->m_resources[(RES_GOLD)] > EVENT_TREE_GOLD) {
                            gpCurPlayer->m_resources[(RES_GOLD)] -= EVENT_TREE_GOLD;
                            GiveExperience(eventHero, levelExperience_g, 1);
                            eventHero->m_treeKnowledgeVisits |=
                                1U << (cell->m_objectMetadata & TREE_KNOWLEDGE_VISIT_INDEX_MASK);
                            eventHero->CheckLevel();
                        }
                        break;
                    case TREE_KNOWLEDGE_GEMS:
                        if (gpCurPlayer->m_resources[(RES_GEMS)] > EVENT_TREE_GEMS) {
                            gpCurPlayer->m_resources[(RES_GEMS)] -= EVENT_TREE_GEMS;
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
                && (gsSpellInfo[cell->m_objectMetadata - 1].level)
                       <= (eventHero->m_secondarySkills[(HERO_SKILL_WISDOM)])
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
                            == (eventType_g | MAP_TRIGGER_ACTION_FLAG)
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
            if (exitCount >= 1) {
                if (exitCount > 1)
                    exitCount = Random(1, exitCount);
                for (exitY_d = 0; exitY_d < MAP_HEIGHT; ++exitY_d) {
                    for (exitX = 0; exitX < MAP_WIDTH; ++exitX) {
                        if (gpGame->m_worldMap.GetCell(exitX, exitY_d)->m_triggerType
                                == (eventType_g | MAP_TRIGGER_ACTION_FLAG)
                            && gpGame->m_worldMap.GetCell(exitX, exitY_d)->m_objectIndex
                                   == cell->m_objectIndex
                            && abs(exitX - x) + abs(exitY_d - y)
                                   > (eventType_g == MAP_OBJECT_STONE_LITHS
                                          ? EVENT_TELEPORT_STONE_DISTANCE
                                          : EVENT_TELEPORT_WHIRLPOOL_DISTANCE)
                            && --exitCount <= 0) {
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
                eraseObject_l = 1;
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
                    for (index_h = 0; index_h < (RES_COUNT); ++index_h) {
                        if (gpCurPlayer->m_resources[index_h] < 0)
                            gpCurPlayer->m_resources[index_h] = 0;
                    }
                    GiveArtifact(eventHero, artifact_g, 1, -1);
                    eraseObject_l = 1;
                    break;
                case ARTIFACT_EVENT_MODE_GOLD:
                    if (gpPhilAI->NetValueOfArtifact((artifact_g), EVENT_ARTIFACT_GOLD, 0, 0)) {
                        gpGame->m_players[(eventHero->m_owner)].m_resources[(RES_GOLD)] -=
                            EVENT_ARTIFACT_GOLD;
                        goto artifactPickup;
                    }
                    break;
                case ARTIFACT_EVENT_MODE_RESOURCE_3:
                    if (gpPhilAI->NetValueOfArtifact(
                            (artifact_g),
                            EVENT_ARTIFACT_RESOURCE_3_GOLD,
                            artifactResource_p,
                            EVENT_ARTIFACT_RESOURCE_3
                        )) {
                        gpGame->m_players[(eventHero->m_owner)].m_resources[(RES_GOLD)] -=
                            EVENT_ARTIFACT_RESOURCE_3_GOLD;
                        gpGame->m_players[(eventHero->m_owner)].m_resources[(artifactResource_p)] -=
                            EVENT_ARTIFACT_RESOURCE_3;
                        goto artifactPickup;
                    }
                    break;
                case ARTIFACT_EVENT_MODE_RESOURCE_5:
                    if (gpPhilAI->NetValueOfArtifact(
                            (artifact_g),
                            EVENT_ARTIFACT_RESOURCE_5_GOLD,
                            artifactResource_p,
                            EVENT_ARTIFACT_RESOURCE_5
                        )) {
                        gpGame->m_players[(eventHero->m_owner)].m_resources[(RES_GOLD)] -=
                            EVENT_ARTIFACT_RESOURCE_5_GOLD;
                        gpGame->m_players[(eventHero->m_owner)].m_resources[(artifactResource_p)] -=
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

            if (gbHumanPlayer[(otherHero_e->m_owner)] == 0) {
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
            eraseObject_l = 1;
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
                    if (gpGame->m_players[(eventHero->m_owner)].m_resources[(RES_GOLD)]
                        >= EVENT_DAEMON_GOLD) {
                        if (gpPhilAI->ChooseToPayRansomOnHero(EVENT_DAEMON_GOLD)) {
                            gpGame->m_players[(eventHero->m_owner)].m_resources[(RES_GOLD)] -=
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
            pyramidBattleValue_l = static_cast<i32>(
                gsSpellInfo[index_h].aiValue
                * ((((gsSpellInfo[index_h].attributes) & (SPELL_INFO_ATTRIBUTE_POWER)))
                       ? (eventHero->Stats(HERO_PRIMARY_SPELL_POWER) <= AI_BATTLE_STAT_MAX
                              ? gfBattleStat[eventHero->Stats(HERO_PRIMARY_SPELL_POWER)]
                              : gfBattleStat[AI_BATTLE_STAT_MAX])
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
                abandonedMineValue_f,
                battleWon_j,
                battleResult_l
            );
            if (battleWon_j == 0)
                break;
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
            break;

        case MAP_OBJECT_TAR_PIT:
            break;

        case MAP_OBJECT_OBSERVATION_TOWER:
            gpGame->SetVisibility(x, y, giCurPlayer, EVENT_OBSERVATION_RADIUS);
            break;

        case MAP_OBJECT_SHIPWRECK_SURVIVOR:
            if (eventHero->NumArtifacts() < HERO_ARTIFACT_SLOT_COUNT)
                GiveArtifact(eventHero, ArtifactType(cell->m_objectMetadata), 1, -1);
            eraseObject_l = 1;
            break;

        case MAP_OBJECT_ARTESIAN_SPRING:
            if (cell->m_objectMetadata == 0)
                break;
            cell->m_objectMetadata = MAP_EVENT_DATA_EMPTY;
            springSpellPoints_j =
                eventHero->Stats(HERO_PRIMARY_KNOWLEDGE) * HERO_SPELL_POINTS_PER_KNOWLEDGE;
            if (eventHero->m_spellPoints < springSpellPoints_j * ARTESIAN_SPRING_MANA_MULTIPLIER)
                eventHero->m_spellPoints =
                    static_cast<i16>(springSpellPoints_j * ARTESIAN_SPRING_MANA_MULTIPLIER);
            break;

        case MAP_OBJECT_MAGIC_WELL:
            if ((((eventHero->m_eventFlags) & (HERO_EVENT_MAGIC_WELL))) == 0) {
                cell->m_objectMetadata = MAP_EVENT_DATA_EMPTY;
                wellSpellPoints_o =
                    eventHero->Stats(HERO_PRIMARY_KNOWLEDGE) * HERO_SPELL_POINTS_PER_KNOWLEDGE;
                if (eventHero->m_spellPoints < wellSpellPoints_o) {
                    eventHero->m_eventFlags = HeroEventFlag(
                        static_cast<i32>(eventHero->m_eventFlags) | (HERO_EVENT_MAGIC_WELL)
                    );
                    eventHero->m_spellPoints = static_cast<i16>(wellSpellPoints_o);
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
            if (eventExtra_o->active == 0)
                break;
            if (Random(0, EVENT_RANDOM_PERCENT_MAX) < EVENT_RANDOM_EVENT_SUCCESS) {
                primaryReward_e = MAP_EVENT_REWARD_NONE;
                primaryAmount_j = 0;
                secondaryReward_k = MAP_EVENT_REWARD_NONE;
                secondaryAmount_j = 0;
                for (index_h = 0; index_h < (RES_COUNT); ++index_h) {
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
            break;

        case MAP_OBJECT_TRADING_POST:
            break;

        case MAP_OBJECT_BARRIER:
            eraseObject_l = BarrierAIEvent(cell, eventHero);
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

    if (eraseObject_l != 0)
        EraseObj(cell, x, y);
    giCurPlayer = oldPlayer_o;
    gpCurPlayer = oldPlayerData_h;
    if (eventHero->m_owner != EVENT_NO_OWNER)
        eventHero->CheckLevel();
    CheckEndGame(END_GAME_FORCE_NONE, false);
}

i32 advManager::BarrierAIEvent(mapCell* cell, hero*) {
    i32 color = cell->m_objectMetadata;
    color &= EVENT_BARRIER_COLOR_MASK;
    if (gpCurPlayer->m_barrierTents & (1 << color))
        return 1;
    else
        return 0;
    return 0;
}

void advManager::PasswordAIEvent(mapCell* cell, hero*) {
    i32 unusedPassword6 [[maybe_unused]][1];
    i32 color = cell->m_objectMetadata;
    color &= EVENT_BARRIER_COLOR_MASK;
    gpCurPlayer->m_barrierTents |= (1 << color);
}

void advManager::GenericSiteAIEvent(mapCell* cell, hero* eventHero) {
    i32 siteLevel6 [[maybe_unused]];
    HeroPrimaryStat primaryStat16;
    i32 artifactIndex14;
    GenericSiteType siteType3;
    i32 unusedPair8 [[maybe_unused]][2];
    i32 cursedArtifactCount3;
    i32 quantity1;
    i32 unusedTriple26 [[maybe_unused]][3];
    CreatureType creatureType3;
    i32 armyValue7;

    cursedArtifactCount3 = 0;
    siteType3 = static_cast<GenericSiteType>(cell->m_objectMetadata);
    siteType3 = static_cast<GenericSiteType>((siteType3) & GENERIC_SITE_TYPE_MASK);
    siteLevel6 = cell->m_objectMetadata;
    siteLevel6 >>= GENERIC_SITE_LEVEL_SHIFT;

    switch (siteType3) {
        case GENERIC_SITE_ALCHEMIST_TOWER:
            for (artifactIndex14 = 0; artifactIndex14 < HERO_ARTIFACT_SLOT_COUNT; artifactIndex14++) {
                if (IsCursedItem(eventHero->m_artifacts[artifactIndex14]))
                    cursedArtifactCount3++;
            }
            if (cursedArtifactCount3 != 0
                && gpCurPlayer->m_resources[(RES_GOLD)]
                       >= EVENT_CURSED_ARTIFACT_GOLD_THRESHOLD) {
                for (artifactIndex14 = 0; artifactIndex14 < HERO_ARTIFACT_SLOT_COUNT;
                     artifactIndex14++) {
                    if (IsCursedItem(eventHero->m_artifacts[artifactIndex14]))
                        eventHero->m_artifacts[artifactIndex14] = ARTIFACT_NONE;
                }
                gpCurPlayer->m_resources[(RES_GOLD)] -= EVENT_CURSED_ARTIFACT_COST;
            }
            break;
        case GENERIC_SITE_ARENA:
            if (!(eventHero->m_eventFlags & HERO_EVENT_ARENA)) {
                eventHero->m_eventFlags = HeroEventFlag(
                    static_cast<i32>(eventHero->m_eventFlags) | (HERO_EVENT_ARENA)
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
                eventHero->m_primaryStats[(primaryStat16)]++;
            }
            break;
        case GENERIC_SITE_MERMAID:
            if (!(eventHero->m_eventFlags & HERO_EVENT_MERMAID)) {
                eventHero->m_eventFlags = HeroEventFlag(
                    static_cast<i32>(eventHero->m_eventFlags) | (HERO_EVENT_MERMAID)
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
                                gMonsterDatabase[(creatureType3)].hitPoints
                                * (quantity1 - eventHero->m_army.m_quantities[artifactIndex14]);
                        }
                    }
                }
                if (armyValue7 != 0)
                    GiveExperience(eventHero, armyValue7, 1);
                eventHero->m_eventFlags = HeroEventFlag(
                    static_cast<i32>(eventHero->m_eventFlags) | (HERO_EVENT_SIRENS)
                );
            }
            break;
        case GENERIC_SITE_STABLES:
            if (!(eventHero->m_eventFlags & HERO_EVENT_STABLES)) {
                eventHero->m_eventFlags = HeroEventFlag(
                    static_cast<i32>(eventHero->m_eventFlags) | (HERO_EVENT_STABLES)
                );
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
                    creatureCount * gMonsterDatabase[(monsterType)].fightValue
                );

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
                    "Гномы приветствую "
                        "вас: \"Друг Роланда "
                        "- наш друг. Вы можете "
                        "пройти.\"",
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
                    "Огры признали вас: "
                        "\"Союзники Арчибальда "
                        "могут пройти.\"",
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
                    "Драконы, посмотрев "
                        "на вас, промолвили: "
                        "\"Наш альянс с Арчибальдом "
                        "вынуждает нас присоединиться "
                        "к вам.  К несчастью, "
                        "у вас нет для нас места. "
                        "Какая жалость!\"  И "
                        "быстро улетели.",
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
                    "Гномы признали своих "
                        "союзников и рады "
                        "присоединиться к "
                        "ним.",
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
                    "Огры признали нас, "
                        "как врагов гномов "
                        "и рады присоединиться "
                        "к вам. ",
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
                    "Горделивые драконы "
                        "вынуждены были признать "
                        "в вас своих 'союзников' "
                        "и присоединиться "
                        "к вам.",
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

    if (gbInCampaign && gpGame->m_campaignAwards[(CAMPAIGN_AWARD_DWARFBANE)]
        && (monsterType == CREATURE_DWARF || monsterType == CREATURE_BATTLE_DWARF)) {
        NormalDialog(
            "\"Враги гномов! Бегите, "
                "если жизнь дорога.\"",
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
                "Едва вы подошли к "
                    "отряду эльфов, как "
                    "их вожак призвал "
                    "свой отряд к оружию. "
                    "Он сказал им: \"Кто "
                    "из вас достаточно "
                    "смел, чтобы вступить "
                    "в армию нашего мужественного "
                    "союзника?\" Весь отряд "
                    "единодушно вступил "
                    "в вашу армию.",
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
                "Эльфы насторожились "
                    "при вашем приближении. "
                    "Их вожак обратился "
                    "к вам со словами: \"Мы "
                    "не можем принять "
                    "участие в ваших делах, "
                    "союзник! Проходи, "
                    "наша победа будет "
                    "вашей.\"",
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
            > 2.0
        && !eventHero->HasArtifact(ARTIFACT_HIDEOUS_MASK) && monsterType != CREATURE_GHOST
        && monsterType != CREATURE_EARTH_ELEMENTAL && monsterType != CREATURE_AIR_ELEMENTAL
        && monsterType != CREATURE_FIRE_ELEMENTAL && monsterType != CREATURE_WATER_ELEMENTAL) {
        if (forceJoin) {
            sprintf(gText, gEventText[EVENT_TEXT_FOLLOWERS], gArmyNamesPlural[(monsterType)]);
            EventWindow(-1, NORMAL_DIALOG_CONFIRM, gText, -1, 0, -1, 0, -1);
            if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
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
                    "%s находится под впечатлением "
                        "вашей дипломатичности, "
                        "и предлагает свои "
                        "услуги вашему войску "
                        "за %d золотых. Вы согласны?",
                    gArmyNames[(monsterType)],
                    joiningCost
                );
            } else {
                sprintf(
                    gText,
                    "Воины покорены вашим "
                        "шармом и выдвинули "
                        "предложение:\n\n"
                );
                if (numJoining == creatureCount)
                    sprintf(
                        monsterText,
                        "Отряд из %d %s вступят "
                            "в ряды вашей армии "
                            "за %d золотых. Вы согласны "
                            "принять их?",
                        creatureCount,
                        gArmyNamesPlural[(monsterType)],
                        joiningCost
                    );
                else
                    sprintf(
                        monsterText,
                        "%d из %d %s вступят в ряды "
                            "вашей армии, а оставшиеся "
                            "оставят вас в покое "
                            "за %d золотых. Вы согласны?",
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
            if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
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
            "Группа %s, страшась "
                "мощи вашего воинства, "
                "бросилась врассыпную. "
                "Соизволите изловить "
                "их и заставить драться?",
            gArmyNamesPlural[(monsterType)]
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
                    * gMonsterDatabase[(monsterType)].fightValue
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
                    static_cast<double>(creatureCount[MONSTER_COMBAT_REMAINING_COUNT])
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
                + (creatureCount[MONSTER_COMBAT_REMAINING_COUNT] & MONSTER_COUNT_MASK);
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
    i32 netUnused7 [[maybe_unused]];
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
    armyGroup* receivedSecondArmy2;
    armyGroup* receivedFirstArmy;
    hero* receivedSecondHero9;
    hero* receivedFirstHero1;
    town* receivedTown;
    char* packet_j;
    i32 remotePlayer;
    tag_message message9;
    i8 combatResult3;
    i32 savedShowIt_f;
    i32 secondPlayer8;
    i32 savedPlayer1;
    i32 unusedCombat_a [[maybe_unused]];
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
            iCombatControlNetPos[(COMBAT_ATTACKER_SIDE)] = giThisNetPos;
            iCombatControlNetPos[(COMBAT_DEFENDER_SIDE)] = gbGamePosToNetPos[secondPlayer8];
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
                "%s, ваш %s атакован!"
                     ,
                cPlayerNames[secondPlayer8],
                combatTown
                    ? "город"
                    : "герой"
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
    gpMouseManager->SetPointer(
        const_cast<char*>("advmice.mse"),
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
        firstHero ? gpGame->m_players[(firstHero->m_owner)].m_resources[(RES_GOLD)] : 0;
    buffer->secondOwner = static_cast<i8>(secondHero ? secondHero->m_owner : -1);
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
    b32 hasFirstHero4;
    b32 gotSecondHeroFirst2;
    b32 gotFirstHeroSecond;
    i32 firstOwner8;
    b32 gotSecondHeroSecond2;
    i32 result7;
    b32 hasTown2;
    b32 hasSecondHero0;
    b32 gotFirstHeroFirst0;
    i32l lastPacketTime7;
    i32 secondOwner;

    *firstHero = NULL;
    *firstArmy = NULL;
    *combatTown = NULL;
    *secondHero = NULL;
    *secondArmy = NULL;
    hasFirstHero4 = hasSecondHero0 = hasTown2 = false;
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
        gpGame->m_players[firstOwner8].m_resources[(RES_GOLD)] =
            EVENTS_REMOTE_COMBAT(packet)->firstGold;
    secondOwner = EVENTS_REMOTE_COMBAT(packet)->secondOwner;
    if (secondOwner > 0)
        gpGame->m_players[secondOwner].m_resources[(RES_GOLD)] =
            EVENTS_REMOTE_COMBAT(packet)->secondGold;

    *firstArmy = static_cast<armyGroup*>(H2_ALLOC(sizeof(armyGroup)));
    memcpy(*firstArmy, &EVENTS_REMOTE_COMBAT(packet)->firstArmy, sizeof(armyGroup));
    *secondArmy = static_cast<armyGroup*>(H2_ALLOC(sizeof(armyGroup)));
    memcpy(*secondArmy, &EVENTS_REMOTE_COMBAT(packet)->secondArmy, sizeof(armyGroup));
    if (hasTown2) {
        *combatTown = static_cast<town*>(H2_ALLOC(sizeof(town)));
        memcpy(*combatTown, &EVENTS_REMOTE_COMBAT(packet)->combatTown, sizeof(town));
    }

    iCombatControlNetPos[(COMBAT_ATTACKER_SIDE)] = *remotePlayer;
    iCombatControlNetPos[(COMBAT_DEFENDER_SIDE)] = giThisNetPos;
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
    gotFirstHeroFirst0 = true;
    gotFirstHeroSecond = true;
    gotSecondHeroFirst2 = true;
    gotSecondHeroSecond2 = true;
    if (hasFirstHero4) {
        *firstHero = static_cast<hero*>(H2_ALLOC(sizeof(hero)));
        gotFirstHeroFirst0 = false;
        gotFirstHeroSecond = false;
    }
    if (hasSecondHero0) {
        *secondHero = static_cast<hero*>(H2_ALLOC(sizeof(hero)));
        gotSecondHeroFirst2 = false;
        gotSecondHeroSecond2 = false;
    }

    while (!gotFirstHeroFirst0 || !gotFirstHeroSecond || !gotSecondHeroFirst2
           || !gotSecondHeroSecond2) {
        PollSound();
        if (lastPacketTime7 + COMBAT_REMOTE_TIMEOUT < KBTickCount()) {
            NormalDialog(
                const_cast<char*>("Ошибка получения информации. Продолжать?"),
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
                ShutDown(const_cast<char*>("Игра прекращена."));
        }
        packet = GetRemoteData(1);
        if (packet && EVENTS_REMOTE_MESSAGE(packet)->type == REMOTE_MESSAGE_RELIABLE
            && EVENTS_REMOTE_MESSAGE(packet)->command == REMOTE_COMMAND) {
            lastPacketTime7 = KBTickCount();
            if (EVENTS_REMOTE_HERO(packet)->fragment == REMOTE_FIRST_HERO_FIRST) {
                memcpy(*firstHero, EVENTS_REMOTE_HERO(packet)->data, COMBAT_REMOTE_HERO_FIRST_SIZE);
                gotFirstHeroFirst0 = true;
            }
            if (EVENTS_REMOTE_HERO(packet)->fragment == REMOTE_FIRST_HERO_SECOND) {
                memcpy(
                    &(*firstHero)->m_spells[COMBAT_REMOTE_HERO_SECOND_SPELL_INDEX],
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
                gotSecondHeroFirst2 = true;
            }
            if (EVENTS_REMOTE_HERO(packet)->fragment == REMOTE_SECOND_HERO_SECOND) {
                memcpy(
                    &(*secondHero)->m_spells[COMBAT_REMOTE_HERO_SECOND_SPELL_INDEX],
                    EVENTS_REMOTE_HERO(packet)->data,
                    COMBAT_REMOTE_HERO_SECOND_SIZE
                );
                gotSecondHeroSecond2 = true;
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

i32 RiddleStringsEqual(const char* answer, const char* expected) {
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
