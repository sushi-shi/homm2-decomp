#include <Ints.h>
#include <PLATFORM/Strings.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/Utf8.h>
#include <BASE/bmap2.h>
#include <BASE/executive.h>
#include <BASE/widget.h>
#include <BASE/inputManager.h>
#include <BASE/Misc.h>
#include <BASE/mouseManager.h>
#include <BASE/soundManager.h>
#include <EDITOR/fullMap.h>
#include <IRONFIST/artifacts.h>
#include <IRONFIST/dialog.h>
#include <IRONFIST/hooks.h>
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
#include <SOURCE/Localization.h>
#include <SOURCE/PHILAI.h>
#include <SOURCE/philAI.h>
#include <SOURCE/REMOTE.h>
#include <SOURCE/combatManager.h>
#include <PLATFORM/Runtime.h>
#include <SOURCE/playerData.h>
#include <SOURCE/recruitUnit.h>
#include <SOURCE/swapManager.h>
#include <SOURCE/tradpost.h>
#include <SOURCE/town.h>
#include <SOURCE/EVENTS_TYPES.h>

#include <string>
#include <SOURCE/townManager.h>
#include <SOURCE/TOWNMGR.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/x_arena.h>
#include <SOURCE/Localization.h>


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

    typedef enum ShipyardEventConstant {
        DIALOG_BUILD_BOAT = 0x7802
    } ShipyardEventConstant;

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

    enum class StableVisitResult : u8 {
        STABLE_VISIT_NONE = 0,
        STABLE_VISIT_MOBILITY = 1,
        STABLE_VISIT_UPGRADE = 2,
        STABLE_VISIT_MOBILITY_UPGRADE = STABLE_VISIT_MOBILITY | STABLE_VISIT_UPGRADE
    };
using enum StableVisitResult;
    ENABLE_ENUM_FLAGS(StableVisitResult)

    enum class HouseRecruitmentSite : i32 {
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
using enum HouseRecruitmentSite;

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

    enum class EventSoundVariant : i32 {
        SOUND_VARIANT_0 = 0,
        SOUND_VARIANT_1 = 1,
        SOUND_VARIANT_2 = 2,
        SOUND_VARIANT_3 = 3,
        SOUND_VARIANT_4 = 4,
        SOUND_VARIANT_5 = 5,
        SOUND_VARIANT_6 = 6
    };
using enum EventSoundVariant;

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
        PRIMARY_BONUS_ATTACK = NORMAL_DIALOG_PRIMARY_BONUS_OFFSET + H2EnumIndex(HERO_PRIMARY_ATTACK),
        PRIMARY_BONUS_DEFENSE = NORMAL_DIALOG_PRIMARY_BONUS_OFFSET + H2EnumIndex(HERO_PRIMARY_DEFENSE),
        PRIMARY_BONUS_SPELL_POWER =
            NORMAL_DIALOG_PRIMARY_BONUS_OFFSET + H2EnumIndex(HERO_PRIMARY_SPELL_POWER),
        PRIMARY_BONUS_KNOWLEDGE = NORMAL_DIALOG_PRIMARY_BONUS_OFFSET + H2EnumIndex(HERO_PRIMARY_KNOWLEDGE)
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

    // Ironfist's true result suppresses the location handler, not the normal
    // redraw, ambient-audio, and end-game tail of DoEvent.
    if (ironfist::hooks::LocationVisit(cell, x, y))
        goto event_done;

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
                    localization::Tr("event.inline.14d0296e539d777e"),
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
                    localization::Tr("event.inline.1553297e35c659a6"),
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
                utf8::Copy(
                    gText, GLOBAL_TEXT_BUFFER_SIZE,
                    localization::Tr("event.inline.c15ae0e09ee99d18")
                );
                NormalDialog(gText, NORMAL_DIALOG_CONFIRM, -1, -1, -1, 0, -1, 0, -1, 0);
                if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
                    const std::string riddle =
                        localization::DecodeExternalText(eventExtra_o->riddle);
                    sprintf(
                        gText,
                        localization::Tr("event.inline.785dc53c14bdbc91"),
                        riddle.c_str()
                    );
                    GetDataEntry(gText, sphinxAnswer_a, SPHINX_INPUT_LENGTH, NULL, 0, 1);
                    correctAnswer_e = false;
                    for (eventValue1 = 0; eventValue1 < eventExtra_o->answerCount; eventValue1++) {
                        const std::string answer = localization::DecodeExternalText(
                            eventExtra_o->answers[eventValue1]
                        );
                        if (RiddleStringsEqual(
                                sphinxAnswer_a,
                                answer.c_str()
                            ))
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
                            localization::Tr("event.inline.b34883b51b1f43be"),
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
                            localization::Tr("event.inline.da87bc41bfb2530c"),
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
                localization::Tr("event.inline.fff2b25d391ab29d"),
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
                        localization::Tr("event.inline.36d8db3120534c34"),
                        gArmyNamesPlural[H2EnumIndex(firstUpgrade_e)],
                        gArmyNamesPlural[H2EnumIndex(secondUpgrade1)],
                        gArmyNamesPlural[H2EnumIndex(thirdUpgrade_f)],
                        gArmyNamesPlural[H2EnumIndex(firstUpgrade_e) + 1],
                        gArmyNamesPlural[H2EnumIndex(secondUpgrade1) + 1],
                        gArmyNamesPlural[H2EnumIndex(thirdUpgrade_f) + 1]
                    );
                } else if (secondUpgrade1 != CREATURE_NONE) {
                    sprintf(
                        gText,
                        localization::Tr("event.inline.6ec6a4f4693dcb8d"),
                        gArmyNamesPlural[H2EnumIndex(firstUpgrade_e)],
                        gArmyNamesPlural[H2EnumIndex(secondUpgrade1)],
                        gArmyNamesPlural[H2EnumIndex(firstUpgrade_e) + 1],
                        gArmyNamesPlural[H2EnumIndex(secondUpgrade1) + 1]
                    );
                } else {
                    sprintf(
                        gText,
                        localization::Tr("event.inline.6ffe5f935a4c8867"),
                        gArmyNamesPlural[H2EnumIndex(firstUpgrade_e)],
                        gArmyNamesPlural[H2EnumIndex(firstUpgrade_e) + 1]
                    );
                }
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    gText,
                    NORMAL_DIALOG_MONSTER,
                    H2EnumIndex(firstUpgrade_e) + 1,
                    secondUpgrade1 == CREATURE_NONE ? -1 : NORMAL_DIALOG_MONSTER,
                    H2EnumIndex(secondUpgrade1) + 1,
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
                        localization::Tr("event.inline.16a4b831c7bd6a44"),
                        gArmyNamesPlural[H2EnumIndex(firstUpgrade_e)],
                        gArmyNamesPlural[H2EnumIndex(secondUpgrade1)],
                        gArmyNamesPlural[H2EnumIndex(thirdUpgrade_f)],
                        gArmyNamesPlural[H2EnumIndex(firstUpgrade_e) + 1],
                        gArmyNamesPlural[H2EnumIndex(secondUpgrade1) + 1],
                        gArmyNamesPlural[H2EnumIndex(thirdUpgrade_f) + 1]
                    );
                } else if (secondUpgrade1 != CREATURE_NONE) {
                    sprintf(
                        gText,
                        localization::Tr("event.inline.49edf5c404ac2672"),
                        gArmyNamesPlural[H2EnumIndex(firstUpgrade_e)],
                        gArmyNamesPlural[H2EnumIndex(secondUpgrade1)],
                        gArmyNamesPlural[H2EnumIndex(firstUpgrade_e) + 1],
                        gArmyNamesPlural[H2EnumIndex(secondUpgrade1) + 1]
                    );
                } else {
                    sprintf(
                        gText,
                        localization::Tr("event.inline.95b379d46e666d70"),
                        gArmyNamesPlural[H2EnumIndex(firstUpgrade_e)],
                        gArmyNamesPlural[H2EnumIndex(firstUpgrade_e) + 1]
                    );
                }
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    gText,
                    NORMAL_DIALOG_MONSTER,
                    H2EnumIndex(firstUpgrade_e) + 1,
                    secondUpgrade1 == CREATURE_NONE ? -1 : NORMAL_DIALOG_MONSTER,
                    H2EnumIndex(secondUpgrade1) + 1,
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
                && eventHero2->NumArtifacts() < EVENT_ARTIFACT_CAPACITY) {
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
                    H2EnumIndex(RES_GOLD),
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
                    localization::Tr("event.inline.333bff0ee2591ce7"),
                    NORMAL_DIALOG_INFO,
                    -1,
                    -1,
                    H2EnumIndex(RES_GOLD),
                    SEA_CHEST_GOLD,
                    -1,
                    0,
                    -1,
                    0
                );
                GiveResource(eventHero2, RES_GOLD, SEA_CHEST_GOLD);
            } else {
                NormalDialog(
                    localization::Tr("event.inline.d2a7adee0f4b9a8a"),
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
                        localization::Tr("event.inline.fafa7917b1fb1445"),
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
                        localization::Tr("event.inline.021d355fcf4f31af"),
                        NORMAL_DIALOG_INFO,
                        -1,
                        -1,
                        H2EnumIndex(RES_WOOD),
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
                        localization::Tr("event.inline.d19251bcd8000e2e"),
                        NORMAL_DIALOG_INFO,
                        -1,
                        -1,
                        H2EnumIndex(RES_WOOD),
                        FLOTSAM_WOOD_AMOUNT,
                        H2EnumIndex(RES_GOLD),
                        FLOTSAM_GOLD_AMOUNT,
                        -1,
                        0
                    );
                    GiveResource(eventHero2, RES_WOOD, FLOTSAM_WOOD_AMOUNT);
                    GiveResource(eventHero2, RES_GOLD, FLOTSAM_GOLD_AMOUNT);
                    break;
                case FLOTSAM_LARGE_TREASURE:
                    NormalDialog(
                        localization::Tr("event.inline.bd36cf6691ab88e7"),
                        NORMAL_DIALOG_INFO,
                        -1,
                        -1,
                        H2EnumIndex(RES_WOOD),
                        FLOTSAM_LARGE_WOOD_AMOUNT,
                        H2EnumIndex(RES_GOLD),
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
                GiveArtifact(eventHero2, ArtifactType(cell->m_objectMetadata), 1, -1);
            } else {
                NormalDialog(
                    localization::Tr("event.inline.12821ba5d6cffff7"),
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
            if (gpCurPlayer->m_resources[H2EnumIndex(RES_GOLD)] < MAGELLAN_MAP_COST) {
                NormalDialog(
                    localization::Tr("event.inline.e81b091ba4b1417f"),
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
                    localization::Tr("event.inline.19dbe84af3193778"),
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
                    gpCurPlayer->m_resources[H2EnumIndex(RES_GOLD)] -= MAGELLAN_MAP_COST;
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
                    localization::Tr("event.inline.68e1e92f9a9d12d5"),
                    gSecondarySkills[cell->m_objectMetadata]
                );
                NormalDialog(gText, NORMAL_DIALOG_INFO, -1, -1, -1, 0, -1, 0, -1, 0);
            } else if (eventHero2->m_secondarySkillCount >= HERO_SECONDARY_SKILL_LIMIT) {
                sprintf(
                    gText,
                    localization::Tr("event.inline.8432c04b1a757522"),
                    gSecondarySkills[cell->m_objectMetadata]
                );
                NormalDialog(gText, NORMAL_DIALOG_INFO, -1, -1, -1, 0, -1, 0, -1, 0);
            } else {
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
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
                eventHero2->GiveSS(
                    static_cast<HeroSecondarySkill>(cell->m_objectMetadata),
                    HERO_SKILL_LEVEL_BASIC
                );
            }
            break;

        case MAP_OBJECT_ARTESIAN_SPRING:
            if (!cell->m_objectMetadata) {
                NormalDialog(
                    localization::Tr("event.inline.87ca55017c24c23b"),
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
                        localization::Tr("event.inline.ee47b77e069dbc64"),
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
                        localization::Tr("event.inline.db769672add76670"),
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
            if ((H2EnumIndex((eventHero2->m_eventFlags) & (HERO_EVENT_MAGIC_WELL)))) {
                NormalDialog(
                    localization::Tr("event.inline.6e512f3b213f3c96"),
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
                        localization::Tr("event.inline.fa57992068d990d5"),
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
                        localization::Tr("event.inline.0ec7b47d176de1a3"),
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
                        static_cast<i32>(eventHero2->m_eventFlags) | H2EnumIndex(HERO_EVENT_MAGIC_WELL)
                    );
                    eventHero2->m_spellPoints = wellSpellPoints_o;
                }
            }
            break;

        case MAP_OBJECT_COAST:
            if ((H2EnumIndex((eventHero2->m_eventFlags) & (HERO_EVENT_EMBARKED)))) {
                eventHero2->m_eventFlags = HeroEventFlag(
                    static_cast<i32>(eventHero2->m_eventFlags) & ~H2EnumIndex(HERO_EVENT_EMBARKED)
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
                static_cast<i32>(eventHero2->m_eventFlags) | H2EnumIndex(HERO_EVENT_EMBARKED)
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
                H2EnumIndex(gpGame->m_mines[cell->m_objectMetadata].resourceType)
                    + MINE_RESOURCE_ICON_OFFSET,
                NORMAL_DIALOG_INFO,
                "",
                H2EnumIndex(gpGame->m_mines[cell->m_objectMetadata].resourceType),
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
                H2EnumIndex(RES_MERCURY),
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
                H2EnumIndex(RES_WOOD),
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
                    localization::Tr("event.inline.f28640c80e1c5d9c"),
                    H2EnumIndex(RES_GOLD),
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
            if ((H2EnumIndex((eventHero2->m_eventFlags) & (HERO_EVENT_BUOY)))) {
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
                    static_cast<i32>(eventHero2->m_eventFlags) | H2EnumIndex(HERO_EVENT_BUOY)
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
            if ((H2EnumIndex((eventHero2->m_eventFlags) & (HERO_EVENT_FAERIE_RING)))) {
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
                    static_cast<i32>(eventHero2->m_eventFlags) | H2EnumIndex(HERO_EVENT_FAERIE_RING)
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
            if ((H2EnumIndex((eventHero2->m_eventFlags) & (HERO_EVENT_IDOL)))) {
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
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                eventHero2->m_eventFlags = HeroEventFlag(
                    static_cast<i32>(eventHero2->m_eventFlags) | H2EnumIndex(HERO_EVENT_IDOL)
                );
                eventHero2->m_luck++;
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
            if ((H2EnumIndex((eventHero2->m_eventFlags) & (HERO_EVENT_FOUNTAIN)))) {
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
                    static_cast<i32>(eventHero2->m_eventFlags) | H2EnumIndex(HERO_EVENT_FOUNTAIN)
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
            if ((H2EnumIndex((eventHero2->m_eventFlags) & (HERO_EVENT_WATERING_HOLE)))) {
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
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                eventHero2->m_eventFlags = HeroEventFlag(
                    static_cast<i32>(eventHero2->m_eventFlags) | H2EnumIndex(HERO_EVENT_WATERING_HOLE)
                );
                eventHero2->m_morale++;
                eventHero2->m_mobility += WATERING_HOLE_MOBILITY_BONUS;
                eventHero2->m_remainingMobility += WATERING_HOLE_MOBILITY_BONUS;
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
            if ((H2EnumIndex((eventHero2->m_eventFlags) & (HERO_EVENT_OASIS)))) {
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
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                eventHero2->m_eventFlags = HeroEventFlag(
                    static_cast<i32>(eventHero2->m_eventFlags) | H2EnumIndex(HERO_EVENT_OASIS)
                );
                eventHero2->m_morale++;
                eventHero2->m_mobility += OASIS_MOBILITY_BONUS;
                eventHero2->m_remainingMobility += OASIS_MOBILITY_BONUS;
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
            if ((H2EnumIndex((eventHero2->m_eventFlags) & (HERO_EVENT_TEMPLE)))) {
                NormalDialog(
                    localization::Tr("event.inline.9824059f04898b92"),
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
                    static_cast<i32>(eventHero2->m_eventFlags) | H2EnumIndex(HERO_EVENT_TEMPLE)
                );
                eventHero2->m_morale += TEMPLE_MORALE_BONUS;
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
                if (eventHero2->NumArtifacts() == EVENT_ARTIFACT_CAPACITY)
                    goto wagonEmpty;
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                eventValue1 = cell->m_objectMetadata & WAGON_ARTIFACT_MASK;
                sprintf(
                    gText,
                    localization::Tr("event.inline.b7b134ab6de2bf9c"),
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
                    localization::Tr("event.inline.cb89443bb85658f3"),
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
                        sprintf(gText, "%s.", localization::Tr("common.treasure"));
                        EventWindow(
                            -1,
                            NORMAL_DIALOG_INFO,
                            gText,
                            H2EnumIndex(RES_GOLD),
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
                H2EnumIndex(RES_GOLD),
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
                ResourceType(cell->m_objectMetadata & H2EnumIndex(CAMPFIRE_RESOURCE_MASK)),
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
                    localization::Tr("event.inline.da378498c0963c83"),
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
                        + H2EnumIndex(eventHero2->m_secondarySkills[H2EnumIndex(HERO_SKILL_DIPLOMACY)])
                              * XANADU_DIPLOMACY_MULTIPLIER
                    >= XANADU_ADMISSION_LEVEL) {
                    EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                    NormalDialog(
                        localization::Tr("event.inline.6e67be895f4b2e0b"),
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
                    eventHero2->m_primaryStats[H2EnumIndex(HERO_PRIMARY_ATTACK)]++;
                    eventHero2->m_primaryStats[H2EnumIndex(HERO_PRIMARY_DEFENSE)]++;
                    eventHero2->m_primaryStats[H2EnumIndex(HERO_PRIMARY_KNOWLEDGE)]++;
                    eventHero2->m_primaryStats[H2EnumIndex(HERO_PRIMARY_SPELL_POWER)]++;
                    eventHero2->m_xanaduVisits |= 1 << cell->m_objectMetadata;
                } else {
                    NormalDialog(
                        localization::Tr("event.inline.cfee7ef6dc2dfe29"),
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
                    localization::Tr("event.inline.41d63febc86c9f57"),
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
                eventHero2->m_primaryStats[H2EnumIndex(HERO_PRIMARY_DEFENSE)]++;
                eventHero2->m_fortVisits |= 1 << cell->m_objectMetadata;
            }
            break;

        case MAP_OBJECT_STANDING_STONES:
            if (eventHero2->m_standingStoneVisits & (1 << cell->m_objectMetadata)) {
                NormalDialog(
                    localization::Tr("event.inline.abd64140cf81cf64"),
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
                eventHero2->m_primaryStats[H2EnumIndex(HERO_PRIMARY_SPELL_POWER)]++;
                eventHero2->m_standingStoneVisits |= 1 << cell->m_objectMetadata;
            }
            break;

        case MAP_OBJECT_WITCH_DOCTOR_HUT:
            if (eventHero2->m_witchDoctorVisits & (1 << cell->m_objectMetadata)) {
                NormalDialog(
                    localization::Tr("event.inline.adb07f262cb5a288"),
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
                eventHero2->m_primaryStats[H2EnumIndex(HERO_PRIMARY_KNOWLEDGE)]++;
                eventHero2->m_witchDoctorVisits |= 1 << cell->m_objectMetadata;
            }
            break;

        case MAP_OBJECT_MERCENARY_CAMP:
            if (eventHero2->m_mercenaryCampVisits & (1 << cell->m_objectMetadata)) {
                NormalDialog(
                    localization::Tr("event.inline.66fa174294ad02d5"),
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
                eventHero2->m_primaryStats[H2EnumIndex(HERO_PRIMARY_ATTACK)]++;
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
                    H2EnumIndex(RES_GOLD),
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
            strcpy(sphinxAnswer_a, gResourceNames[H2EnumIndex(resourceType_a)]);
            utf8::LowercaseFirst(sphinxAnswer_a);
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
            if (cell->m_objectMetadata <= H2EnumIndex(RES_GOLD)) {
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
                    localization::Tr("event.inline.b586887c923c2b52"),
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
                    localization::Tr("event.inline.b74e2c6dcdfc9414"),
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
                    localization::Tr("event.inline.9cf24d321bfa6701"),
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
                    localization::Tr("event.inline.1836ce3164cbf13b"),
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
                        localization::Tr("event.inline.ead7ae60b21cca0c"),
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
                    localization::Tr("event.inline.fd6fbeb8bebaabb7"),
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
                    localization::Tr("event.inline.c9a9da0253003e88"),
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
                    localization::Tr("event.inline.280d4b94ba469861"),
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
                    localization::Tr("event.inline.5ae328bcb860b385"),
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
                    localization::Tr("event.inline.d4b1f646aae1de64"),
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
                    localization::Tr("event.inline.87d5d0b84d168064"),
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
                        GiveExperience(eventHero2, levelExperience_g, 0);
                        eventHero2->m_treeKnowledgeVisits |=
                            1U << (cell->m_objectMetadata & TREE_KNOWLEDGE_VISIT_INDEX_MASK);
                        break;

                    case TREE_KNOWLEDGE_GOLD:
                        if (gpCurPlayer->m_resources[H2EnumIndex(RES_GOLD)] >= TREE_KNOWLEDGE_GOLD_COST) {
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
                            if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
                                gpCurPlayer->m_resources[H2EnumIndex(RES_GOLD)] -= TREE_KNOWLEDGE_GOLD_COST;
                                GiveExperience(eventHero2, levelExperience_g, 0);
                                eventHero2->m_treeKnowledgeVisits |=
                            1U << (cell->m_objectMetadata & TREE_KNOWLEDGE_VISIT_INDEX_MASK);
                            }
                        } else {
                            NormalDialog(
                                localization::Tr("event.inline.924f82c98b13def7"),
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
                        if (gpCurPlayer->m_resources[H2EnumIndex(RES_GEMS)] >= TREE_KNOWLEDGE_GEM_COST) {
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
                            if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
                                gpCurPlayer->m_resources[H2EnumIndex(RES_GEMS)] -= TREE_KNOWLEDGE_GEM_COST;
                                GiveExperience(eventHero2, levelExperience_g, 0);
                                eventHero2->m_treeKnowledgeVisits |=
                            1U << (cell->m_objectMetadata & TREE_KNOWLEDGE_VISIT_INDEX_MASK);
                            }
                        } else {
                            NormalDialog(
                                localization::Tr("event.inline.ee1576513fcb95bc"),
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
                localization::Tr("event.inline.bec395b49e2bdfa9")
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
            if (eventHero2->HasArtifact(ARTIFACT_MAGIC_BOOK)) {
                if (H2EnumIndex(gsSpellInfo[cell->m_objectMetadata - 1].level)
                    <= static_cast<i32>(eventHero2->m_secondarySkills[H2EnumIndex(HERO_SKILL_WISDOM)])
                           + SHRINE_WISDOM_BONUS
                    && eventHero2->m_cursorType != FACTION_CYBORG) {
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
                    localization::Tr("event.inline.b880ecbc34dcdb8b"),
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
                    localization::Tr("event.inline.4e929a833e525afd")
                );
                EventWindow(
                    -1,
                    NORMAL_DIALOG_INFO,
                    gText,
                    MAP_EVENT_REWARD_ARTIFACT,
                    H2EnumIndex(artifact_g),
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
                    ironfist::GetArtifactEvent(H2EnumIndex(artifact_g)),
                    MAP_EVENT_REWARD_ARTIFACT,
                    H2EnumIndex(artifact_g),
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
                        localization::Tr("event.inline.47fbd3f75db6688a"),
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
                        localization::Tr("event.inline.580a8ff25d58cea2"),
                        gArmyNamesPlural[H2EnumIndex(guardedMonster_c)],
                        gArmyNamesPlural[H2EnumIndex(guardedMonster_c)]
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
                            localization::Tr("event.inline.db0baf49ee252efe"),
                            gArtifactNames[H2EnumIndex(artifact_g)]
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
                        if (eventHero2->m_secondarySkills[H2EnumIndex(HERO_SKILL_WISDOM)]
                            != HERO_SKILL_LEVEL_NONE)
                            goto artifactPickup;
                        sprintf(
                            gText,
                            localization::Tr("event.inline.114341b70ac61964"),
                            gArtifactNames[H2EnumIndex(artifact_g)]
                        );
                        NormalDialog(gText, NORMAL_DIALOG_INFO, -1, -1, -1, 0, -1, 0, -1, 0);
                        break;

                    case ARTIFACT_EVENT_MODE_LEADERSHIP:
                        if (eventHero2->m_secondarySkills[H2EnumIndex(HERO_SKILL_LEADERSHIP)]
                            != HERO_SKILL_LEVEL_NONE)
                            goto artifactPickup;
                        sprintf(
                            gText,
                            localization::Tr("event.inline.85b58cafd168a4e6"),
                            gArtifactNames[H2EnumIndex(artifact_g)]
                        );
                        NormalDialog(gText, NORMAL_DIALOG_INFO, -1, -1, -1, 0, -1, 0, -1, 0);
                        break;

                    case ARTIFACT_EVENT_MODE_PICKUP:
                    artifactPickup:
                        EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                        EventWindow(
                            -1,
                            NORMAL_DIALOG_INFO,
                            ironfist::GetArtifactEvent(H2EnumIndex(artifact_g)),
                            MAP_EVENT_REWARD_ARTIFACT,
                            H2EnumIndex(artifact_g),
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
                            localization::Tr("event.inline.219ab198b8338466"),
                            gArtifactNames[H2EnumIndex(artifact_g)]
                        );
                        EventWindow(
                            -1,
                            NORMAL_DIALOG_CONFIRM,
                            gText,
                            MAP_EVENT_REWARD_ARTIFACT,
                            H2EnumIndex(artifact_g),
                            -1,
                            0,
                            -1
                        );
                        if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
                            if (gpGame->m_players[eventHero2->m_owner].m_resources[H2EnumIndex(RES_GOLD)]
                                >= ARTIFACT_EVENT_GOLD_COST) {
                                gpGame->m_players[eventHero2->m_owner].m_resources[H2EnumIndex(RES_GOLD)] -=
                                    ARTIFACT_EVENT_GOLD_COST;
                                goto giveArtifact;
                            }
                            NormalDialog(
                                localization::Tr("event.inline.eb595f1324302ad8"),
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
                                localization::Tr("event.inline.d2d604b822f6a317"),
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
                        utf8::Copy(sphinxAnswer_a, sizeof(sphinxAnswer_a), gResourceNames[artifactResourceType_k]);
                        utf8::LowercaseFirst(sphinxAnswer_a);
                        sprintf(
                            gText,
                            localization::Tr("event.inline.6ab4a1f0b7bda188"),
                            gArtifactNames[H2EnumIndex(artifact_g)],
                            sphinxAnswer_a
                        );
                        NormalDialog(
                            gText,
                            NORMAL_DIALOG_CONFIRM,
                            -1,
                            -1,
                            MAP_EVENT_REWARD_ARTIFACT,
                            H2EnumIndex(artifact_g),
                            -1,
                            0,
                            -1,
                            0
                        );
                        if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
                            if (gpGame->m_players[eventHero2->m_owner].m_resources[H2EnumIndex(RES_GOLD)]
                                    >= ARTIFACT_EVENT_RESOURCE_3_GOLD_COST
                                && gpGame->m_players[eventHero2->m_owner]
                                           .m_resources[artifactResourceType_k]
                                       >= ARTIFACT_EVENT_RESOURCE_3_AMOUNT) {
                                gpGame->m_players[eventHero2->m_owner].m_resources[H2EnumIndex(RES_GOLD)] -=
                                    ARTIFACT_EVENT_RESOURCE_3_GOLD_COST;
                                gpGame->m_players[eventHero2->m_owner]
                                    .m_resources[artifactResourceType_k] -=
                                    ARTIFACT_EVENT_RESOURCE_3_AMOUNT;
                                goto giveArtifact;
                            }
                            NormalDialog(
                                localization::Tr("event.inline.eb595f1324302ad8"),
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
                                localization::Tr("event.inline.d2d604b822f6a317"),
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
                        utf8::Copy(sphinxAnswer_a, sizeof(sphinxAnswer_a), gResourceNames[artifactResourceType_k]);
                        utf8::LowercaseFirst(sphinxAnswer_a);
                        sprintf(
                            gText,
                            localization::Tr("event.inline.d5ae03d7c7901a43"),
                            gArtifactNames[H2EnumIndex(artifact_g)],
                            sphinxAnswer_a
                        );
                        NormalDialog(
                            gText,
                            NORMAL_DIALOG_CONFIRM,
                            -1,
                            -1,
                            MAP_EVENT_REWARD_ARTIFACT,
                            H2EnumIndex(artifact_g),
                            -1,
                            0,
                            -1,
                            0
                        );
                        if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
                            if (gpGame->m_players[eventHero2->m_owner].m_resources[H2EnumIndex(RES_GOLD)]
                                    >= ARTIFACT_EVENT_RESOURCE_5_GOLD_COST
                                && gpGame->m_players[eventHero2->m_owner]
                                           .m_resources[artifactResourceType_k]
                                       >= ARTIFACT_EVENT_RESOURCE_5_AMOUNT) {
                                gpGame->m_players[eventHero2->m_owner].m_resources[H2EnumIndex(RES_GOLD)] -=
                                    ARTIFACT_EVENT_RESOURCE_5_GOLD_COST;
                                gpGame->m_players[eventHero2->m_owner]
                                    .m_resources[artifactResourceType_k] -=
                                    ARTIFACT_EVENT_RESOURCE_5_AMOUNT;
                                goto giveArtifact;
                            }
                            NormalDialog(
                                localization::Tr("event.inline.eb595f1324302ad8"),
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
                                localization::Tr("event.inline.d2d604b822f6a317"),
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

        case MAP_OBJECT_BOTTLE: {
            if (cell->m_objectMetadata) {
                signExtra_k = reinterpret_cast<signEventExtra*>(ppMapExtra[cell->m_objectMetadata]);
                const std::string signText =
                    localization::DecodeExternalText(signExtra_k->text);
                if (signText.size() > SIGN_MINIMUM_TEXT_LENGTH)
                    EventWindow(
                        -1,
                        NORMAL_DIALOG_INFO,
                        signText.c_str(),
                        -1,
                        0,
                        -1,
                        0,
                        -1
                    );
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
        }

        case MAP_OBJECT_SIGN: {
            if (cell->m_objectMetadata) {
                signExtra_k = reinterpret_cast<signEventExtra*>(ppMapExtra[cell->m_objectMetadata]);
                const std::string signText =
                    localization::DecodeExternalText(signExtra_k->text);
                if (signText.size() > SIGN_MINIMUM_TEXT_LENGTH)
                    EventWindow(
                        -1,
                        NORMAL_DIALOG_INFO,
                        signText.c_str(),
                        -1,
                        0,
                        -1,
                        0,
                        -1
                    );
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
        }

        case MAP_OBJECT_DAEMON_CAVE: {
            EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
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
            if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_NO)
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

            monsterType_f = static_cast<CreatureType>(
                ((cell->m_objectMetadata & DAEMON_SERVANT_MASK) >> DAEMON_SERVANT_SHIFT)
                + DAEMON_SERVANT_BASE
            );
            utf8::Copy(
                gText, GLOBAL_TEXT_BUFFER_SIZE,
                localization::Tr("event.inline.bf2299125ded4930")
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
                        localization::Tr("event.inline.cb3ce775e0c55740"),
                        NORMAL_DIALOG_INFO,
                        -1,
                        -1,
                        H2EnumIndex(RES_GOLD),
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
                        localization::Tr("event.inline.2c0fc1c524da5be1"),
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
                    eventValue1 = H2EnumIndex(GiveRandomArtifact(eventHero2));
                    sprintf(
                        gText,
                        localization::Tr("event.inline.ea1d54ae68a8c04a"),
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
                        localization::Tr("event.inline.92b0881ba080eff9"),
                        H2EnumIndex(RES_GOLD),
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
                        localization::Tr("event.inline.1b32f445f3853c55"),
                        -1,
                        0,
                        -1,
                        0,
                        -1
                    );
                    if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
                        if (gpGame->m_players[eventHero2->m_owner].m_resources[H2EnumIndex(RES_GOLD)]
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
                            HeroLoses(eventHero2);
                        } else {
                            gpGame->m_players[eventHero2->m_owner].m_resources[H2EnumIndex(RES_GOLD)] -=
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
                localization::Tr("event.inline.b84c63468364c5dd"),
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
                            localization::Tr("event.inline.af08a748652258d4"),
                            NORMAL_DIALOG_MORALE_PENALTY,
                            0,
                            -1,
                            0,
                            -1
                        );
                        if (!(H2EnumIndex((eventHero2->m_eventFlags) & (HERO_EVENT_SHIPWRECK)))) {
                            eventHero2->m_eventFlags =
                                eventHero2->m_eventFlags | HERO_EVENT_SHIPWRECK;
                            eventHero2->m_morale--;
                        }
                        break;
                    default:
                        if (GhostEvent(
                                eventHero2,
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
                        if (!(H2EnumIndex((eventHero2->m_eventFlags) & (HERO_EVENT_GRAVEYARD)))) {
                            eventHero2->m_eventFlags =
                                eventHero2->m_eventFlags | HERO_EVENT_GRAVEYARD;
                            eventHero2->m_morale--;
                        }
                        break;
                    default:
                        zombieCell_o = GetCell(
                            x - normalDirTable[H2EnumIndex(eventHero2->m_direction)].x,
                            y - normalDirTable[H2EnumIndex(eventHero2->m_direction)].y
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
                localization::Tr("event.inline.1fe5ee69b3203322"),
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
                            localization::Tr("event.inline.b2f13c84edb58954"),
                            NORMAL_DIALOG_MORALE_PENALTY,
                            0,
                            -1,
                            0,
                            -1
                        );
                        if (!(H2EnumIndex((eventHero2->m_eventFlags) & (HERO_EVENT_DERELICT_SHIP)))) {
                            eventHero2->m_eventFlags =
                                eventHero2->m_eventFlags | HERO_EVENT_DERELICT_SHIP;
                            eventHero2->m_morale--;
                        }
                        break;
                    default:
                        skeletonCell_e = GetCell(
                            x - normalDirTable[H2EnumIndex(eventHero2->m_direction)].x,
                            y - normalDirTable[H2EnumIndex(eventHero2->m_direction)].y
                        );
                        if (SkeletonEvent(
                                eventHero2,
                                skeletonCell_e,
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
            EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
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
            if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
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
                    if (!(H2EnumIndex((eventHero2->m_eventFlags) & (HERO_EVENT_PYRAMID)))) {
                        eventHero2->m_eventFlags = HeroEventFlag(
                            static_cast<i32>(eventHero2->m_eventFlags) | H2EnumIndex(HERO_EVENT_PYRAMID)
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
                            localization::Tr("event.inline.3c1ac9096798469d"),
                            gSpellNames[cell->m_objectMetadata - 1]
                        );
                        if (!eventHero2->HasArtifact(ARTIFACT_MAGIC_BOOK)) {
                            strcat(
                                eventText_b,
                                localization::Tr("event.inline.bdc50da8baa48d7e")
                            );
                            EventWindow(-1, NORMAL_DIALOG_INFO, eventText_b, -1, 0, -1, 0, -1);
                        } else if (eventHero2->m_secondarySkills[H2EnumIndex(HERO_SKILL_WISDOM)]
                                       >= HERO_SKILL_LEVEL_EXPERT
                                   && eventHero2->m_cursorType != FACTION_CYBORG) {
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
                                localization::Tr("event.inline.b748cab66a3bf25b")
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
                localization::Tr("event.inline.1a911306b482a58b"),
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
            break;

        case MAP_OBJECT_SHIPYARD:
            ShipyardEvent(x, y);
            break;
    }

event_done:
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
    gpSoundManager->SwitchAmbientMusic(giTerrainToMusicTrack[H2EnumIndex(m_currentTerrain)]);
    WaitEndSample(&eventSample_f, -1);
    CheckEndGame(END_GAME_FORCE_NONE, false);
}

void advManager::EraseObj(class mapCell* cell, i32 x, i32 y) {

    mapCellExtra* extras_h[NEIGHBOR_COUNT];
    mapCellExtra* extra_l;
    mapCell* cells_h[NEIGHBOR_COUNT];
    i32 frame_k = NO_FRAME;
    mapCell* currentCell_d;
    b8 isWide_l = false;
    i32 i_g;

    i32 cellX_l;
    i32 cellY_o;

    for (i_g = 0; i_g < NEIGHBOR_COUNT; i_g++) {
        cells_h[i_g] = NULL;
        extras_h[i_g] = NULL;
    }

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
                    } else {
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

        currentCell_d->m_flags |= H2EnumIndex(MAP_CELL_OBJECT_SHADOW_ONLY);
    cellDone:

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

    sprintf(
        gText,
        localization::Tr("event.barrier.inspect"),
        xBarrierColor[colorIndex]
    );
    // Ironfist drops the typed password: visiting the tent is enough, and
    // the barrier dissolves on its own.
    ironfist::H2MessageBox(gText);
    if (gpCurPlayer->m_barrierTents & (1 << colorIndex)) {
        EventSound(cell->m_triggerType & MAP_TRIGGER_TYPE_MASK, colorIndex, &eventSample);
        sprintf(
            gText,
            localization::Tr("event.barrier.dissolves"),
            xPasswordStrings[passwordIndex]
        );
        ironfist::H2MessageBox(gText);
        return 1;
    }
    ironfist::H2MessageBox(const_cast<char*>(localization::Tr("event.barrier.no_password")));
    return 0;
}

i8 StrEqNoCase(const char* firstString, const char* sndString) {
    return utf8::EqualIgnoringCase(firstString, sndString, SITE_STRING_LIMIT - 1);
}

void advManager::PasswordEvent(mapCell* cell, hero*) {
    SAMPLE2 playSample = NULL;
    i32 color = cell->m_objectMetadata;
    color &= COLOR_MASK;

    EventSound(cell->m_triggerType & MAP_TRIGGER_TYPE_MASK, color, &playSample);

    sprintf(
        gText,
        localization::Tr("event.barrier.tent_hint"),
        xBarrierColor[color]
    );
    ironfist::H2MessageBox(gText);
    gpCurPlayer->m_barrierTents |= 1 << color;
}

void advManager::GenericSiteEvent(mapCell* cell, hero* eventHero) {
    i32 primaryStat15;
    i32 cursedArtifactCount2;
    SAMPLE2 eventSample9;
    i32 index8;
    GenericSiteType siteType4;
    i32 currentSiteType10;
    i32 mapX37;
    H2EnumStorage<StableVisitResult, i32> unusedTwo1;
    i32 mapY14;
    mapCell* currentCell36;

    H2EnumStorage<StableVisitResult, i32> unusedOne18;
    i32 oldQuantity3;
    H2EnumStorage<StableVisitResult, i8> stableResult26;
    CreatureType creatureType;
    i32 experience11;

    cursedArtifactCount2 = 0;
    eventSample9 = NULL;
    siteType4 = static_cast<GenericSiteType>(cell->m_objectMetadata);
    siteType4 = static_cast<GenericSiteType>(H2EnumIndex(siteType4) & GENERIC_SITE_TYPE_MASK);
    switch (siteType4) {
        case GENERIC_SITE_ALCHEMIST_TOWER:
            for (index8 = 0; index8 < HERO_ARTIFACT_SLOT_COUNT; index8++) {
                if (ironfist::IsCursedArtifact(eventHero->m_artifacts[index8]))
                    cursedArtifactCount2++;
            }
            if (cursedArtifactCount2 != 0) {
                EventSound(
                    cell->m_triggerType & MAP_TRIGGER_TYPE_MASK, H2EnumIndex(siteType4), &eventSample9
                );
                if (cursedArtifactCount2 == 1) {
                    utf8::Copy(
                        gText, GLOBAL_TEXT_BUFFER_SIZE,
                        localization::Tr("event.inline.8db49d8e3d5c5308")
                    );
                } else {
                    sprintf(
                        gText,
                        localization::Tr("event.inline.1c9a29276fc4ba9c"),
                        cursedArtifactCount2
                    );
                }
                NormalDialog(gText, NORMAL_DIALOG_CONFIRM, -1, -1, -1, 0, -1, 0, -1, 0);
                if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
                    if (gpCurPlayer->m_resources[H2EnumIndex(RES_GOLD)] >= SITE_ALCHEMIST_COST) {
                        for (index8 = 0; index8 < HERO_ARTIFACT_SLOT_COUNT; index8++) {
                            if (ironfist::IsCursedArtifact(eventHero->m_artifacts[index8])) {
                                GiveTakeArtifactStat(
                                    eventHero, eventHero->m_artifacts[index8], EVENT_ARTIFACT_TAKE
                                );
                                eventHero->m_artifacts[index8] = ARTIFACT_NONE;
                            }
                        }
                        gpCurPlayer->m_resources[H2EnumIndex(RES_GOLD)] -= SITE_ALCHEMIST_COST;
                    } else {
                        NormalDialog(
                            localization::Tr("event.inline.35891c3b946b11c8"),
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
                    localization::Tr("event.inline.4be252d59fc6debb"),
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
            if ((H2EnumIndex((eventHero->m_eventFlags) & (HERO_EVENT_ARENA)))) {
                NormalDialog(
                    localization::Tr("event.inline.5e4d96b705906aad"),
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
                    cell->m_triggerType & MAP_TRIGGER_TYPE_MASK, H2EnumIndex(siteType4), &eventSample9
                );
                eventHero->m_eventFlags = HeroEventFlag(
                    static_cast<i32>(eventHero->m_eventFlags) | H2EnumIndex(HERO_EVENT_ARENA)
                );
                primaryStat15 = DoArenaDialog();
                eventHero->m_primaryStats[primaryStat15]++;
            }
            break;

        case GENERIC_SITE_MERMAID:
            if ((H2EnumIndex((eventHero->m_eventFlags) & (HERO_EVENT_MERMAID)))) {
                NormalDialog(
                    localization::Tr("event.inline.87faaee8cff00588"),
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
                    cell->m_triggerType & MAP_TRIGGER_TYPE_MASK, H2EnumIndex(siteType4), &eventSample9
                );
                eventHero->m_eventFlags = HeroEventFlag(
                    static_cast<i32>(eventHero->m_eventFlags) | H2EnumIndex(HERO_EVENT_MERMAID)
                );
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
            EventSound(cell->m_triggerType & MAP_TRIGGER_TYPE_MASK, H2EnumIndex(siteType4), &eventSample9);
            NormalDialog(
                localization::Tr("event.inline.fd826cb8b8a55aeb"),
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
                localization::Tr("event.inline.90cd00d12dc7f24b"),
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
            if ((H2EnumIndex((eventHero->m_eventFlags) & (HERO_EVENT_SIRENS)))) {
                NormalDialog(
                    localization::Tr("event.inline.acc859e4d069b705"),
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
                                gMonsterDatabase[H2EnumIndex(creatureType)].hitPoints
                                * (oldQuantity3 - eventHero->m_army.m_quantities[index8]);
                        }
                    }
                }
                if (experience11 != 0) {
                    EventSound(
                        cell->m_triggerType & MAP_TRIGGER_TYPE_MASK,
                        H2EnumIndex(siteType4),
                        &eventSample9
                    );
                    sprintf(
                        gText,
                        localization::Tr("event.inline.d211349fcd9060a7"),
                        experience11
                    );
                    NormalDialog(gText, NORMAL_DIALOG_INFO, -1, -1, -1, 0, -1, 0, -1, 0);
                    GiveExperience(eventHero, experience11, 1);
                } else {
                    NormalDialog(
                        localization::Tr("event.inline.ee7fce7250a9d986"),
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
                    static_cast<i32>(eventHero->m_eventFlags) | H2EnumIndex(HERO_EVENT_SIRENS)
                );
            }
            break;

        case GENERIC_SITE_STABLES:
            unusedOne18 = STABLE_VISIT_MOBILITY;
            unusedTwo1 = STABLE_VISIT_UPGRADE;
            stableResult26 = STABLE_VISIT_NONE;
            if ((H2EnumIndex((eventHero->m_eventFlags) & (HERO_EVENT_STABLES))) == 0) {
                eventHero->m_eventFlags = HeroEventFlag(
                    static_cast<i32>(eventHero->m_eventFlags) | H2EnumIndex(HERO_EVENT_STABLES)
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
                    cell->m_triggerType & MAP_TRIGGER_TYPE_MASK, H2EnumIndex(siteType4), &eventSample9
                );
            }
            utf8::Copy(gText, GLOBAL_TEXT_BUFFER_SIZE, xStableText[H2EnumIndex(stableResult26)]);
            if ((H2EnumIndex((stableResult26) & (STABLE_VISIT_UPGRADE)))) {
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
    H2EnumStorage<RecruitSiteType, u32> recruitSiteType;
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
        default:
            return;
    }

    index = H2EnumIndex(recruitSiteType);
    if (quantity == 0) {
        EventWindow(-1, NORMAL_DIALOG_INFO, xRecruitEmpty[index], -1, 0, -1, 0, -1);
    } else {
        EventSound(cell->m_triggerType & MAP_TRIGGER_TYPE_MASK, quantity, &recruitSample);
        EventWindow(-1, NORMAL_DIALOG_CONFIRM, xRecruitBuy[index], -1, 0, -1, 0, -1);
        if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
            ExpansionRecruitEvent(eventHero, creatureType1, &quantity);
            packedValue = (quantity << EVENT_RECRUIT_COUNT_SHIFT) | H2EnumIndex(recruitSiteType);
            cell->m_objectMetadata = packedValue;
        }
    }
}

void advManager::ExpansionRecruitEvent(
    hero* eventHero, CreatureType creatureType, i16* availableCount
) {

    baseManager* recruitWindow = new recruitUnit(&eventHero->m_army, creatureType, availableCount);

    if (recruitWindow == NULL)
        MemError();
    gpExec->DoDialog(recruitWindow);
    delete recruitWindow;
}

// The shipyard builds a boat on any open water tile around it, for the
// usual thousand gold and ten wood.
void advManager::ShipyardEvent(i32 x, i32 y) {
    static const i32 boatCellOffsets[][2] = {
        {-1, -1},
        {0, -1},
        {1, -1},
        {2, -1},
        {2, 0},
        {2, 1},
        {1, 1},
        {0, 1},
        {-1, 1},
        {-1, 0}
    };
    tag_message boatMessage;
    i32 boatX = 0;
    i32 boatY = 0;
    bool boatPossible = false;

    gpMouseManager->SetPointer(0);
    for (auto& offset : boatCellOffsets) {
        boatX = x + offset[0];
        boatY = y + offset[1];
        mapCell* boatCell = GetCell(boatX, boatY);
        if (giGroundToTerrain[boatCell->m_terrainImageIndex] == TERRAIN_WATER
            && boatCell->m_triggerType == MAP_OBJECT_NONE) {
            boatPossible = true;
            break;
        }
    }

    if (gpGame->GetBoatsBuilt() >= TOWN_MAX_BOATS || !boatPossible) {
        NormalDialog(
            localization::Tr("event.shipyard.cannot_build_boat"),
            1, 208, 40, -1, 0, -1, 0, -1, 0
        );
        return;
    }

    heroWindow* shipWindow = new heroWindow(177, 20, "shipwind.bin");
    if (shipWindow == NULL)
        MemError();
    SetWinText(shipWindow, 12);
    if (gpGame->m_players[giCurPlayer].m_resources[H2EnumIndex(RES_GOLD)] < TOWN_BOAT_GOLD_COST
        || gpGame->m_players[giCurPlayer].m_resources[H2EnumIndex(RES_WOOD)]
               < TOWN_BOAT_WOOD_COST) {
        boatMessage.type = MESSAGE_WIDGET;
        boatMessage.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
        boatMessage.payload.widget.id = DIALOG_BUILD_BOAT;
        boatMessage.payload.widget.data.value = H2EnumIndex(WIDGET_FLAG_GRAYED);
        shipWindow->BroadcastMessage(boatMessage);
        boatMessage.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
        boatMessage.payload.widget.data.value = H2EnumIndex(WIDGET_FLAG_ENABLED);
        shipWindow->BroadcastMessage(boatMessage);
    }
    gpWindowManager->DoDialog(shipWindow, TrueFalseDialogHandler, 0);
    delete shipWindow;

    if (gpWindowManager->m_dialogResult == DIALOG_BUILD_BOAT) {
        if (gpGame->CreateBoat(boatX, boatY, 0) == -1) {
            LogStr("Can't create boat!");
        } else {
            gpGame->m_players[giCurPlayer].m_resources[H2EnumIndex(RES_GOLD)] -=
                TOWN_BOAT_GOLD_COST;
            gpGame->m_players[giCurPlayer].m_resources[H2EnumIndex(RES_WOOD)] -=
                TOWN_BOAT_WOOD_COST;
            if (bShowIt) {
                gpMouseManager->HideColorPointer();
                gpWindowManager->SaveFizzleSource(168, 160, 176, 132);
                CompleteDraw(0);
                gpWindowManager->FizzleForward(168, 160, 176, 132, 65, 0, 0);
                gpMouseManager->ShowColorPointer();
            }
        }
    }
}

void advManager::JailEvent(mapCell* cell, hero* eventHero, i32 x, i32 y) {
    SAMPLE2 eventSample = NULL;
    i32 heroId;
    hero* freedHero;

    heroId = cell->m_objectMetadata;
    if (gpGame->m_availableHeroes[heroId] != EVENT_JAILED_HERO) {
        NormalDialog(
            localization::Tr("event.inline.dd448d43b4794460"),
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
            localization::Tr("event.inline.30d6f33566016021"),
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
        localization::Tr("event.inline.5aa1a1227a3f148e"),
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

    char eventText[EVENT_TEXT_BUFFER_SIZE];

    if (eventId >= 0 && eventId < EVENT_TEXT_COUNT)
        utf8::Copy(eventText, sizeof(eventText), gEventText[eventId]);
    else if (eventId == MAP_EVENT_REWARD_NONE)
        utf8::Copy(eventText, sizeof(eventText), text);
    else
        sprintf(
            eventText,
            "Event ID %d",
            eventId
        );

    NormalDialog(eventText, buttons, -1, -1, H2EnumIndex(type1), value1, type2, value2, type3, 0);
}

i32 GiveArtifact(hero* eventHero, ArtifactType artifact, b32 checkEndGame, i8 artifactExtra) {
    i32 artifactSlot;

    for (artifactSlot = 0; artifactSlot < EVENT_ARTIFACT_SLOT_COUNT; artifactSlot++) {
        if (eventHero->m_artifacts[artifactSlot] == ARTIFACT_NONE) {
            break;
        }
    }

    if (artifactSlot == EVENT_ARTIFACT_SLOT_COUNT) {
        return H2EnumIndex(ARTIFACT_NONE);
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

    i32 newLevel1;

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
        gpGame->m_players[eventHero->m_owner].m_resources[H2EnumIndex(resourceType)] += amount;
    if (resourceType == RES_GOLD && gbHumanPlayer[eventHero->m_owner])
        CheckEndGame(END_GAME_FORCE_NONE, false);
}

void advManager::RecruitEvent(
    hero* eventHero, CreatureType creatureType, mapCell* cell
) {

    i16 availableCount = static_cast<i16>(cell->m_objectMetadata);
    baseManager* recruitWindow =
        new recruitUnit(&eventHero->m_army, creatureType, &availableCount);

    if (recruitWindow == NULL)
        MemError();
    gpExec->DoDialog(recruitWindow);
    delete recruitWindow;
    cell->m_objectMetadata = static_cast<u16>(availableCount);
}

i32 advManager::SkeletonEvent(
    hero* eventHero, mapCell* cell, const char* text, i32 x, i32 y
) {

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
                    H2EnumIndex(RES_GOLD),
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
                    H2EnumIndex(RES_GOLD),
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
                    H2EnumIndex(RES_GOLD),
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
                    H2EnumIndex(RES_GOLD),
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

i32 advManager::ZombieEvent(
    hero* eventHero,
    mapCell* cell,
    const char* text,
    i32 x,
    i32 y
) {
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
                    H2EnumIndex(RES_GOLD),
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
                    H2EnumIndex(RES_GOLD),
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
                    H2EnumIndex(RES_GOLD),
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
                        H2EnumIndex(RES_GOLD),
                        ZOMBIE_EVENT_HUGE_GOLD,
                        MAP_EVENT_REWARD_ARTIFACT,
                        H2EnumIndex(artifactId),
                        -1
                    );
                else
                    EventWindow(
                        -1,
                        NORMAL_DIALOG_INFO,
                        text,
                        H2EnumIndex(RES_GOLD),
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

i32 advManager::GhostEvent(
    hero* eventHero, mapCell* cell, const char* text, i32 x, i32 y
) {
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
                    H2EnumIndex(RES_GOLD),
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
                    H2EnumIndex(RES_GOLD),
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
                    H2EnumIndex(RES_GOLD),
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
                        H2EnumIndex(RES_GOLD),
                        GHOST_EVENT_HUGE_GOLD,
                        MAP_EVENT_REWARD_ARTIFACT,
                        H2EnumIndex(artifactId),
                        -1
                    );
                else
                    EventWindow(
                        -1,
                        NORMAL_DIALOG_INFO,
                        gText,
                        H2EnumIndex(RES_GOLD),
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
    CreatureType creatureTypes[H2EnumIndex(RECRUIT_SITE_COUNT)];

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
            H2EnumIndex(siteIndex) * EVENT_HOUSE_DIALOG_STRIDE + EVENT_EMPTY_DIALOG_BASE,
            NORMAL_DIALOG_INFO,
            "",
            -1,
            0,
            -1,
            0,
            -1
        );
    } else {
        creatureTypes[H2EnumIndex(RECRUIT_ARCHER)] = CREATURE_ARCHER;
        creatureTypes[H2EnumIndex(RECRUIT_GOBLIN)] = CREATURE_GOBLIN;
        creatureTypes[H2EnumIndex(RECRUIT_PEASANT)] = CREATURE_PEASANT;
        creatureTypes[H2EnumIndex(RECRUIT_DWARF)] = CREATURE_DWARF;
        creatureTypes[H2EnumIndex(RECRUIT_LOG_CABIN)] = CREATURE_DWARF;
        creatureTypes[H2EnumIndex(RECRUIT_TREE_HOUSE)] = CREATURE_SPRITE;
        creatureTypes[H2EnumIndex(RECRUIT_HALFLING)] = CREATURE_HALFLING;
        creatureTypes[H2EnumIndex(RECRUIT_WATCH_TOWER)] = CREATURE_ORC;
        creatureTypes[H2EnumIndex(RECRUIT_CAVE)] = CREATURE_CENTAUR;
        creatureTypes[H2EnumIndex(RECRUIT_EXCAVATION)] = CREATURE_SKELETON;

        EventWindow(
            H2EnumIndex(siteIndex) * EVENT_HOUSE_DIALOG_STRIDE + EVENT_RECRUIT_DIALOG_BASE,
            NORMAL_DIALOG_CONFIRM,
            "",
            -1,
            0,
            -1,
            0,
            -1
        );
        if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
            if (eventHero->m_army.CanJoin(creatureTypes[H2EnumIndex(siteIndex)])) {
                eventHero->m_army.Add(
                    creatureTypes[H2EnumIndex(siteIndex)], cell->m_objectMetadata, -1
                );
                cell->m_objectMetadata = 0;
            } else {
                EventWindow(
                    H2EnumIndex(siteIndex) * EVENT_HOUSE_DIALOG_STRIDE + EVENT_ARMY_FULL_DIALOG_BASE,
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

    i32 lastCount;
    i32 groupCount;
    i32 stackCount;
    CreatureType savedTypes[MONSTER_ARMY_SLOTS];
    CombatResult battleOutcome;
    i32 savedCounts[MONSTER_ARMY_SLOTS];
    i32 stackIdx;

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

    memset(gpMonGroup->m_creatureTypes, H2EnumIndex(CREATURE_NONE), MONSTER_ARMY_SLOTS);
    memset(gpMonGroup->m_creatureCounts, 0, MONSTER_ARMY_SLOTS * sizeof(i16));
    stackCount = MONSTER_ARMY_SLOTS - secondaryStacks - tertiaryStacks;
    if (stackCount < 1)
        stackCount = 1;
    groupCount = 0;
    SRand(combatX + combatY);
    if (stackCount == MONSTER_ARMY_SLOTS
        && (H2EnumIndex((gMonsterDatabase[H2EnumIndex(monsterType)].attributes) & (MONSTER_ATTRIBUTE_RANGED))) == 0) {
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

    if (artifact == ARTIFACT_NONE) {
        ironfist::hooks::ArtifactChanged(targetHero, H2EnumIndex(artifact), take);
        return;
    }
    stats[H2EnumIndex(HERO_PRIMARY_ATTACK)] = 0;
    stats[H2EnumIndex(HERO_PRIMARY_DEFENSE)] = 0;
    stats[H2EnumIndex(HERO_PRIMARY_SPELL_POWER)] = 0;
    stats[H2EnumIndex(HERO_PRIMARY_KNOWLEDGE)] = 0;

    switch (artifact) {
        case ARTIFACT_ULTIMATE_BOOK:
            stats[H2EnumIndex(HERO_PRIMARY_KNOWLEDGE)] = 12;
            break;
        case ARTIFACT_ULTIMATE_SWORD:
            stats[H2EnumIndex(HERO_PRIMARY_ATTACK)] = 12;
            break;
        case ARTIFACT_ULTIMATE_CLOAK:
            stats[H2EnumIndex(HERO_PRIMARY_DEFENSE)] = 12;
            break;
        case ARTIFACT_ULTIMATE_WAND:
            stats[H2EnumIndex(HERO_PRIMARY_SPELL_POWER)] = 12;
            break;
        case ARTIFACT_ULTIMATE_SHIELD:
            stats[H2EnumIndex(HERO_PRIMARY_ATTACK)] = 6;
            stats[H2EnumIndex(HERO_PRIMARY_DEFENSE)] = 6;
            break;
        case ARTIFACT_ULTIMATE_STAFF:
            stats[H2EnumIndex(HERO_PRIMARY_SPELL_POWER)] = 6;
            stats[H2EnumIndex(HERO_PRIMARY_KNOWLEDGE)] = 6;
            break;
        case ARTIFACT_ULTIMATE_CROWN:
            stats[H2EnumIndex(HERO_PRIMARY_ATTACK)] = 4;
            stats[H2EnumIndex(HERO_PRIMARY_DEFENSE)] = 4;
            stats[H2EnumIndex(HERO_PRIMARY_SPELL_POWER)] = 4;
            stats[H2EnumIndex(HERO_PRIMARY_KNOWLEDGE)] = 4;
            break;
        case ARTIFACT_GOLDEN_GOOSE:
            break;
        case ARTIFACT_ARCANE_NECKLACE:
            stats[H2EnumIndex(HERO_PRIMARY_SPELL_POWER)] = 4;
            break;
        case ARTIFACT_CASTER_BRACELET:
            stats[H2EnumIndex(HERO_PRIMARY_SPELL_POWER)] = 2;
            break;
        case ARTIFACT_MAGE_RING:
            stats[H2EnumIndex(HERO_PRIMARY_SPELL_POWER)] = 2;
            break;
        case ARTIFACT_WITCHES_BROACH:
            stats[H2EnumIndex(HERO_PRIMARY_SPELL_POWER)] = 3;
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
            stats[H2EnumIndex(HERO_PRIMARY_ATTACK)] = 1;
            break;
        case ARTIFACT_ARMORED_GAUNTLETS:
            stats[H2EnumIndex(HERO_PRIMARY_DEFENSE)] = 1;
            break;
        case ARTIFACT_DEFENDER_HELM:
            stats[H2EnumIndex(HERO_PRIMARY_DEFENSE)] = 1;
            break;
        case ARTIFACT_GIANT_FLAIL:
            stats[H2EnumIndex(HERO_PRIMARY_ATTACK)] = 1;
            break;
        case ARTIFACT_BALLISTA:
            break;
        case ARTIFACT_STEALTH_SHIELD:
            stats[H2EnumIndex(HERO_PRIMARY_DEFENSE)] = 2;
            break;
        case ARTIFACT_DRAGON_SWORD:
            stats[H2EnumIndex(HERO_PRIMARY_ATTACK)] = 3;
            break;
        case ARTIFACT_POWER_AXE:
            stats[H2EnumIndex(HERO_PRIMARY_ATTACK)] = 2;
            break;
        case ARTIFACT_DIVINE_BREASTPLATE:
            stats[H2EnumIndex(HERO_PRIMARY_DEFENSE)] = 3;
            break;
        case ARTIFACT_MINOR_SCROLL:
            stats[H2EnumIndex(HERO_PRIMARY_KNOWLEDGE)] = 2;
            break;
        case ARTIFACT_MAJOR_SCROLL:
            stats[H2EnumIndex(HERO_PRIMARY_KNOWLEDGE)] = 3;
            break;
        case ARTIFACT_SUPERIOR_SCROLL:
            stats[H2EnumIndex(HERO_PRIMARY_KNOWLEDGE)] = 4;
            break;
        case ARTIFACT_FOREMOST_SCROLL:
            stats[H2EnumIndex(HERO_PRIMARY_KNOWLEDGE)] = 5;
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
            stats[H2EnumIndex(HERO_PRIMARY_ATTACK)] = 1;
            stats[H2EnumIndex(HERO_PRIMARY_DEFENSE)] = 1;
            break;
        case ARTIFACT_SPIKED_SHIELD:
            stats[H2EnumIndex(HERO_PRIMARY_ATTACK)] = 2;
            stats[H2EnumIndex(HERO_PRIMARY_DEFENSE)] = 2;
            break;
        case ARTIFACT_WHITE_PEARL:
            stats[H2EnumIndex(HERO_PRIMARY_SPELL_POWER)] = 1;
            stats[H2EnumIndex(HERO_PRIMARY_KNOWLEDGE)] = 1;
            break;
        case ARTIFACT_BLACK_PEARL:
            stats[H2EnumIndex(HERO_PRIMARY_SPELL_POWER)] = 2;
            stats[H2EnumIndex(HERO_PRIMARY_KNOWLEDGE)] = 2;
            break;
        case ARTIFACT_MAGIC_BOOK:
            break;
        case ARTIFACT_SPELL_SCROLL:
            break;
        case ARTIFACT_ARM_OF_MARTYR:
            stats[H2EnumIndex(HERO_PRIMARY_SPELL_POWER)] = 3;
            break;
        case ARTIFACT_BREASTPLATE_ANDURAN:
            stats[H2EnumIndex(HERO_PRIMARY_DEFENSE)] = 5;
            break;
        case ARTIFACT_BROACH_SHIELDING:
            stats[H2EnumIndex(HERO_PRIMARY_SPELL_POWER)] = -2;
            break;
        case ARTIFACT_BATTLE_GARB:
            stats[H2EnumIndex(HERO_PRIMARY_SPELL_POWER)] = 5;
            stats[H2EnumIndex(HERO_PRIMARY_DEFENSE)] = 5;
            stats[H2EnumIndex(HERO_PRIMARY_ATTACK)] = 5;
            break;
        case ARTIFACT_CRYSTAL_BALL:
            break;
        case ARTIFACT_HEART_FIRE:
            break;
        case ARTIFACT_HEART_ICE:
            break;
        case ARTIFACT_HELMET_ANDURAN:
            stats[H2EnumIndex(HERO_PRIMARY_SPELL_POWER)] = 5;
            break;
        case ARTIFACT_HOLY_HAMMER:
            stats[H2EnumIndex(HERO_PRIMARY_ATTACK)] = 5;
            break;
        case ARTIFACT_LEGENDARY_SCEPTER:
            stats[H2EnumIndex(HERO_PRIMARY_SPELL_POWER)] = 2;
            stats[H2EnumIndex(HERO_PRIMARY_ATTACK)] = 2;
            stats[H2EnumIndex(HERO_PRIMARY_DEFENSE)] = 2;
            stats[H2EnumIndex(HERO_PRIMARY_KNOWLEDGE)] = 2;
            break;
        case ARTIFACT_MASTHEAD:
            break;
        case ARTIFACT_SPHERE_NEGATION:
            break;
        case ARTIFACT_STAFF_WIZARDRY:
            stats[H2EnumIndex(HERO_PRIMARY_SPELL_POWER)] = 5;
            break;
        case ARTIFACT_SWORD_BREAKER:
            stats[H2EnumIndex(HERO_PRIMARY_DEFENSE)] = 4;
            stats[H2EnumIndex(HERO_PRIMARY_ATTACK)] = 1;
            break;
        case ARTIFACT_SWORD_ANDURAN:
            stats[H2EnumIndex(HERO_PRIMARY_ATTACK)] = 5;
            break;
        case ARTIFACT_SPADE_NECROMANCY:
            break;
        default:
            break;
    }

    for (i = 0; i < EVENT_ARTIFACT_PRIMARY_STAT_COUNT; i++) {
        targetHero->m_primaryStats[i] += (take == EVENT_ARTIFACT_TAKE ? -1 : 1) * stats[i];
        if (i == H2EnumIndex(HERO_PRIMARY_KNOWLEDGE) && take == EVENT_ARTIFACT_TAKE) {
            maxSpellPoints =
                targetHero->Stats(HERO_PRIMARY_KNOWLEDGE) * EVENT_ARTIFACT_SPELL_POINT_MULTIPLIER;
            if (targetHero->m_spellPoints > maxSpellPoints)
                targetHero->m_spellPoints = static_cast<i16>(maxSpellPoints);
        }
    }
    ironfist::hooks::ArtifactChanged(targetHero, H2EnumIndex(artifact), take);
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
                        if (gbThisNetHumanPlayer[sourceHero->m_owner]
                            || gbThisNetHumanPlayer[destinationHero->m_owner]) {
                            sprintf(
                                gText,
                                localization::Tr("event.inline.aa7c539682b9fd51")
                                     ,
                                gArtifactNames[H2EnumIndex(sourceHero->m_artifacts[sourceArtifactSlot])]
                            );
                            NormalDialog(
                                gText,
                                NORMAL_DIALOG_INFO,
                                -1,
                                -1,
                                NORMAL_DIALOG_ARTIFACT,
                                H2EnumIndex(sourceHero->m_artifacts[sourceArtifactSlot]),
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
                    sourceHero->m_artifactExtra[sourceArtifactSlot] = H2EnumIndex(ARTIFACT_NONE);
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
                    * gMonsterDatabase[H2EnumIndex(eventHero->m_army.m_creatureTypes[slotNo])].fightValue;
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

    i32 wellSpellPoints_o;
    i32 springSpellPoints_j;
    float defenderLoss_k;
    float attackerLoss_c;
    i32 heroLevel_e;
    i32 levelExperience_g;
    i32 creatureCosts_a[H2EnumIndex(RES_COUNT)];
    i32 spellPower_j;
    i32 adjacentMonster_j;

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

    i32 eraseObject_l;
    i32 battleResult_l;
    b32 creatureFlag_l;
    i32 oldPlayer_o;
    i32 purchaseCost_i;
    i32 battleWon_j;
    i32 pyramidBattleValue_l;

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

    oldPlayer_o = giCurPlayer;
    oldPlayerData_h = gpCurPlayer;

    if (x == eventHero->m_destinationX && y == eventHero->m_destinationY) {
        eventHero->m_destinationY = -1;
        eventHero->m_destinationX = -1;
    }
    --eventHero->m_remainingMobility;
    switch (eventType_g) {
        case MAP_OBJECT_COAST:
            if ((H2EnumIndex((eventHero->m_eventFlags) & (HERO_EVENT_EMBARKED)))) {
                eventHero->m_eventFlags = HeroEventFlag(
                    static_cast<i32>(eventHero->m_eventFlags) & ~H2EnumIndex(HERO_EVENT_EMBARKED)
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
                static_cast<i32>(eventHero->m_eventFlags) | H2EnumIndex(HERO_EVENT_EMBARKED)
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
                for (index_h = H2EnumIndex(SPELL_SET_EARTH_GUARDIAN);
                     index_h < H2EnumIndex(SPELL_SET_WATER_GUARDIAN);
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
            if ((H2EnumIndex((eventHero->m_eventFlags) & (HERO_EVENT_WATERING_HOLE))) == 0) {
                eventHero->m_mobility += WATERING_HOLE_MOBILITY_BONUS;
                eventHero->m_remainingMobility += WATERING_HOLE_MOBILITY_BONUS;
                eventHero->m_eventFlags = HeroEventFlag(
                    static_cast<i32>(eventHero->m_eventFlags) | H2EnumIndex(HERO_EVENT_WATERING_HOLE)
                );
                ++eventHero->m_morale;
            }
            break;

        case MAP_OBJECT_BUOY:
            if ((H2EnumIndex((eventHero->m_eventFlags) & (HERO_EVENT_BUOY))) == 0) {
                eventHero->m_eventFlags = HeroEventFlag(
                    static_cast<i32>(eventHero->m_eventFlags) | H2EnumIndex(HERO_EVENT_BUOY)
                );
                ++eventHero->m_morale;
            }
            break;

        case MAP_OBJECT_FAERIE_RING:
            if ((H2EnumIndex((eventHero->m_eventFlags) & (HERO_EVENT_FAERIE_RING))) == 0) {
                eventHero->m_eventFlags = HeroEventFlag(
                    static_cast<i32>(eventHero->m_eventFlags) | H2EnumIndex(HERO_EVENT_FAERIE_RING)
                );
                ++eventHero->m_luck;
            }
            break;

        case MAP_OBJECT_IDOL:
            if ((H2EnumIndex((eventHero->m_eventFlags) & (HERO_EVENT_IDOL))) == 0) {
                eventHero->m_eventFlags = HeroEventFlag(
                    static_cast<i32>(eventHero->m_eventFlags) | H2EnumIndex(HERO_EVENT_IDOL)
                );
                ++eventHero->m_luck;
            }
            break;

        case MAP_OBJECT_FOUNTAIN:
            if ((H2EnumIndex((eventHero->m_eventFlags) & (HERO_EVENT_FOUNTAIN))) == 0) {
                eventHero->m_eventFlags = HeroEventFlag(
                    static_cast<i32>(eventHero->m_eventFlags) | H2EnumIndex(HERO_EVENT_FOUNTAIN)
                );
                ++eventHero->m_luck;
            }
            break;

        case MAP_OBJECT_OASIS:
            if ((H2EnumIndex((eventHero->m_eventFlags) & (HERO_EVENT_OASIS))) == 0) {
                eventHero->m_eventFlags = HeroEventFlag(
                    static_cast<i32>(eventHero->m_eventFlags) | H2EnumIndex(HERO_EVENT_OASIS)
                );
                ++eventHero->m_morale;
                eventHero->m_mobility += OASIS_MOBILITY_BONUS;
                eventHero->m_remainingMobility += OASIS_MOBILITY_BONUS;
            }
            break;

        case MAP_OBJECT_TEMPLE:
            if ((H2EnumIndex((eventHero->m_eventFlags) & (HERO_EVENT_TEMPLE))) == 0) {
                eventHero->m_eventFlags = HeroEventFlag(
                    static_cast<i32>(eventHero->m_eventFlags) | H2EnumIndex(HERO_EVENT_TEMPLE)
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
                    ResourceType((cell->m_objectMetadata & H2EnumIndex(ARTIFACT_EVENT_MODE_MASK)) - 1),
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
                        ResourceType((cell->m_objectMetadata & H2EnumIndex(ARTIFACT_EVENT_MODE_MASK)) - 1),
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
                ResourceType(cell->m_objectMetadata & H2EnumIndex(CAMPFIRE_RESOURCE_MASK)),
                cell->m_objectMetadata >> CAMPFIRE_AMOUNT_SHIFT
            );
            eraseObject_l = 1;
            break;

        case MAP_OBJECT_FORT:
            if ((eventHero->m_fortVisits & (1U << cell->m_objectMetadata)) == 0) {
                ++eventHero->m_primaryStats[H2EnumIndex(HERO_PRIMARY_DEFENSE)];
                eventHero->m_fortVisits |= 1U << cell->m_objectMetadata;
            }
            break;

        case MAP_OBJECT_XANADU:
            if ((eventHero->m_xanaduVisits & (1U << cell->m_objectMetadata)) == 0
                && eventHero->m_level
                           + H2EnumIndex(eventHero->m_secondarySkills[H2EnumIndex(HERO_SKILL_DIPLOMACY)])
                                 * XANADU_DIPLOMACY_MULTIPLIER
                       >= XANADU_ADMISSION_LEVEL) {
                ++eventHero->m_primaryStats[H2EnumIndex(HERO_PRIMARY_ATTACK)];
                ++eventHero->m_primaryStats[H2EnumIndex(HERO_PRIMARY_DEFENSE)];
                ++eventHero->m_primaryStats[H2EnumIndex(HERO_PRIMARY_KNOWLEDGE)];
                ++eventHero->m_primaryStats[H2EnumIndex(HERO_PRIMARY_SPELL_POWER)];
                eventHero->m_xanaduVisits |= 1U << cell->m_objectMetadata;
            }
            break;

        case MAP_OBJECT_STANDING_STONES:
            if ((eventHero->m_standingStoneVisits & (1U << cell->m_objectMetadata)) == 0) {
                ++eventHero->m_primaryStats[H2EnumIndex(HERO_PRIMARY_SPELL_POWER)];
                eventHero->m_standingStoneVisits |= 1U << cell->m_objectMetadata;
            }
            break;

        case MAP_OBJECT_WITCH_DOCTOR_HUT:
            if ((eventHero->m_witchDoctorVisits & (1U << cell->m_objectMetadata)) == 0) {
                ++eventHero->m_primaryStats[H2EnumIndex(HERO_PRIMARY_KNOWLEDGE)];
                eventHero->m_witchDoctorVisits |= 1U << cell->m_objectMetadata;
            }
            break;

        case MAP_OBJECT_MERCENARY_CAMP:
            if ((eventHero->m_mercenaryCampVisits & (1U << cell->m_objectMetadata)) == 0) {
                ++eventHero->m_primaryStats[H2EnumIndex(HERO_PRIMARY_ATTACK)];
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
                        for (index_h = 0; index_h < H2EnumIndex(RES_COUNT); ++index_h)
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
                        if (gpCurPlayer->m_resources[H2EnumIndex(RES_GOLD)] > EVENT_TREE_GOLD) {
                            gpCurPlayer->m_resources[H2EnumIndex(RES_GOLD)] -= EVENT_TREE_GOLD;
                            GiveExperience(eventHero, levelExperience_g, 1);
                            eventHero->m_treeKnowledgeVisits |=
                                1U << (cell->m_objectMetadata & TREE_KNOWLEDGE_VISIT_INDEX_MASK);
                            eventHero->CheckLevel();
                        }
                        break;
                    case TREE_KNOWLEDGE_GEMS:
                        if (gpCurPlayer->m_resources[H2EnumIndex(RES_GEMS)] > EVENT_TREE_GEMS) {
                            gpCurPlayer->m_resources[H2EnumIndex(RES_GEMS)] -= EVENT_TREE_GEMS;
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
                && H2EnumIndex(gsSpellInfo[cell->m_objectMetadata - 1].level)
                       <= H2EnumIndex(eventHero->m_secondarySkills[H2EnumIndex(HERO_SKILL_WISDOM)])
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
                    if (eventHero->m_secondarySkills[H2EnumIndex(HERO_SKILL_WISDOM)]
                        != HERO_SKILL_LEVEL_NONE)
                        goto artifactPickup;
                    break;
                case ARTIFACT_EVENT_MODE_LEADERSHIP:
                    if (eventHero->m_secondarySkills[H2EnumIndex(HERO_SKILL_LEADERSHIP)]
                        != HERO_SKILL_LEVEL_NONE)
                        goto artifactPickup;
                    break;
                case ARTIFACT_EVENT_MODE_PICKUP:
                artifactPickup:
                    for (index_h = 0; index_h < H2EnumIndex(RES_COUNT); ++index_h) {
                        if (gpCurPlayer->m_resources[index_h] < 0)
                            gpCurPlayer->m_resources[index_h] = 0;
                    }
                    GiveArtifact(eventHero, artifact_g, 1, -1);
                    eraseObject_l = 1;
                    break;
                case ARTIFACT_EVENT_MODE_GOLD:
                    if (gpPhilAI->NetValueOfArtifact(H2EnumIndex(artifact_g), EVENT_ARTIFACT_GOLD, 0, 0)) {
                        gpGame->m_players[eventHero->m_owner].m_resources[H2EnumIndex(RES_GOLD)] -=
                            EVENT_ARTIFACT_GOLD;
                        goto artifactPickup;
                    }
                    break;
                case ARTIFACT_EVENT_MODE_RESOURCE_3:
                    if (gpPhilAI->NetValueOfArtifact(
                            H2EnumIndex(artifact_g),
                            EVENT_ARTIFACT_RESOURCE_3_GOLD,
                            artifactResource_p,
                            EVENT_ARTIFACT_RESOURCE_3
                        )) {
                        gpGame->m_players[eventHero->m_owner].m_resources[H2EnumIndex(RES_GOLD)] -=
                            EVENT_ARTIFACT_RESOURCE_3_GOLD;
                        gpGame->m_players[eventHero->m_owner].m_resources[artifactResource_p] -=
                            EVENT_ARTIFACT_RESOURCE_3;
                        goto artifactPickup;
                    }
                    break;
                case ARTIFACT_EVENT_MODE_RESOURCE_5:
                    if (gpPhilAI->NetValueOfArtifact(
                            H2EnumIndex(artifact_g),
                            EVENT_ARTIFACT_RESOURCE_5_GOLD,
                            artifactResource_p,
                            EVENT_ARTIFACT_RESOURCE_5
                        )) {
                        gpGame->m_players[eventHero->m_owner].m_resources[H2EnumIndex(RES_GOLD)] -=
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
                    if (gpGame->m_players[eventHero->m_owner].m_resources[H2EnumIndex(RES_GOLD)]
                        >= EVENT_DAEMON_GOLD) {
                        if (gpPhilAI->ChooseToPayRansomOnHero(EVENT_DAEMON_GOLD)) {
                            gpGame->m_players[eventHero->m_owner].m_resources[H2EnumIndex(RES_GOLD)] -=
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
                * ((H2EnumIndex((gsSpellInfo[index_h].attributes) & (SPELL_INFO_ATTRIBUTE_POWER)))
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
                gMineCharacteristics[H2EnumIndex(RES_GOLD)] * gafAITurnCostResource[H2EnumIndex(RES_GOLD)]
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
            if ((H2EnumIndex((eventHero->m_eventFlags) & (HERO_EVENT_MAGIC_WELL))) == 0) {
                cell->m_objectMetadata = MAP_EVENT_DATA_EMPTY;
                wellSpellPoints_o =
                    eventHero->Stats(HERO_PRIMARY_KNOWLEDGE) * HERO_SPELL_POINTS_PER_KNOWLEDGE;
                if (eventHero->m_spellPoints < wellSpellPoints_o) {
                    eventHero->m_eventFlags = HeroEventFlag(
                        static_cast<i32>(eventHero->m_eventFlags) | H2EnumIndex(HERO_EVENT_MAGIC_WELL)
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

                for (index_h = 0; index_h < H2EnumIndex(RES_COUNT); ++index_h) {
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

    i32 color = cell->m_objectMetadata;
    color &= EVENT_BARRIER_COLOR_MASK;
    gpCurPlayer->m_barrierTents |= (1 << color);
}

void advManager::GenericSiteAIEvent(mapCell* cell, hero* eventHero) {
    HeroPrimaryStat primaryStat16;
    i32 artifactIndex14;
    GenericSiteType siteType3;

    i32 cursedArtifactCount3;
    i32 quantity1;

    CreatureType creatureType3;
    i32 armyValue7;

    cursedArtifactCount3 = 0;
    siteType3 = static_cast<GenericSiteType>(cell->m_objectMetadata);
    siteType3 = static_cast<GenericSiteType>(H2EnumIndex(siteType3) & GENERIC_SITE_TYPE_MASK);
    switch (siteType3) {
        case GENERIC_SITE_ALCHEMIST_TOWER:
            for (artifactIndex14 = 0; artifactIndex14 < HERO_ARTIFACT_SLOT_COUNT; artifactIndex14++) {
                if (ironfist::IsCursedArtifact(eventHero->m_artifacts[artifactIndex14]))
                    cursedArtifactCount3++;
            }
            if (cursedArtifactCount3 != 0
                && gpCurPlayer->m_resources[H2EnumIndex(RES_GOLD)]
                       >= EVENT_CURSED_ARTIFACT_GOLD_THRESHOLD) {
                for (artifactIndex14 = 0; artifactIndex14 < HERO_ARTIFACT_SLOT_COUNT;
                     artifactIndex14++) {
                    if (ironfist::IsCursedArtifact(eventHero->m_artifacts[artifactIndex14]))
                        eventHero->m_artifacts[artifactIndex14] = ARTIFACT_NONE;
                }
                gpCurPlayer->m_resources[H2EnumIndex(RES_GOLD)] -= EVENT_CURSED_ARTIFACT_COST;
            }
            break;
        case GENERIC_SITE_ARENA:
            if (!(eventHero->m_eventFlags & HERO_EVENT_ARENA)) {
                eventHero->m_eventFlags = HeroEventFlag(
                    static_cast<i32>(eventHero->m_eventFlags) | H2EnumIndex(HERO_EVENT_ARENA)
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
                eventHero->m_primaryStats[H2EnumIndex(primaryStat16)]++;
            }
            break;
        case GENERIC_SITE_MERMAID:
            if (!(eventHero->m_eventFlags & HERO_EVENT_MERMAID)) {
                eventHero->m_eventFlags = HeroEventFlag(
                    static_cast<i32>(eventHero->m_eventFlags) | H2EnumIndex(HERO_EVENT_MERMAID)
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
                                gMonsterDatabase[H2EnumIndex(creatureType3)].hitPoints
                                * (quantity1 - eventHero->m_army.m_quantities[artifactIndex14]);
                        }
                    }
                }
                if (armyValue7 != 0)
                    GiveExperience(eventHero, armyValue7, 1);
                eventHero->m_eventFlags = HeroEventFlag(
                    static_cast<i32>(eventHero->m_eventFlags) | H2EnumIndex(HERO_EVENT_SIRENS)
                );
            }
            break;
        case GENERIC_SITE_STABLES:
            if (!(eventHero->m_eventFlags & HERO_EVENT_STABLES)) {
                eventHero->m_eventFlags = HeroEventFlag(
                    static_cast<i32>(eventHero->m_eventFlags) | H2EnumIndex(HERO_EVENT_STABLES)
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
    H2SteppedEnumStorage<ResourceType, i32> resource;
    H2EnumStorage<RecruitSiteType, u32> recruitType;
    i32 cost[H2EnumIndex(RES_COUNT)];
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
        default:
            return;
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
            packedSite = (quantity << EVENT_RECRUIT_COUNT_SHIFT) | H2EnumIndex(recruitType);
            cell->m_objectMetadata = packedSite;
            GetMonsterCost(monsterType, cost);
            for (resource = 0; resource < RES_COUNT; resource++) {
                gpCurPlayer->m_resources[H2EnumIndex(resource)] -=
                    buyCount * cost[H2EnumIndex(resource)];
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
    ironfist::hooks::MonsterInteraction(cell);
    gpMouseManager->ShowColorPointer();
    monsterType = static_cast<CreatureType>(cell->m_objectIndex);
    forceJoin = cell->m_objectMetadata & MONSTER_JOIN_FORCED;
    creatureCount = cell->m_objectMetadata & MONSTER_COUNT_MASK;
    armyRatio = static_cast<double>(
                    gpPhilAI->FightValueOfStack(&eventHero->m_army, eventHero, 0, 0, 0, 0)
                )
                / static_cast<double>(
                    creatureCount * gMonsterDatabase[H2EnumIndex(monsterType)].fightValue
                );

    if (gbInCampaign
        && ((gpGame->m_campaignAwards[H2EnumIndex(CAMPAIGN_AWARD_DWARVEN_ALLIANCE)]
             && (monsterType == CREATURE_DWARF || monsterType == CREATURE_BATTLE_DWARF))
            || (gpGame->m_campaignAwards[H2EnumIndex(CAMPAIGN_AWARD_OGRE_ALLIANCE)]
                && (monsterType == CREATURE_OGRE || monsterType == CREATURE_OGRE_LORD))
            || (gpGame->m_campaignAwards[H2EnumIndex(CAMPAIGN_AWARD_DRAGON_ALLIANCE)]
                && (monsterType == CREATURE_GREEN_DRAGON || monsterType == CREATURE_RED_DRAGON
                    || monsterType == CREATURE_BLACK_DRAGON)))) {
        if (!eventHero->m_army.CanJoin(monsterType)) {
            if (monsterType == CREATURE_DWARF || monsterType == CREATURE_BATTLE_DWARF)
                NormalDialog(
                    localization::Tr("event.inline.bc81eedbd225f1cc"),
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
                    localization::Tr("event.inline.cf9fdd5bf3abd972"),
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
                    localization::Tr("event.inline.f68d63dd41307454"),
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
                    localization::Tr("event.inline.ca210d925c2d4171"),
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
                    localization::Tr("event.inline.7e5ecd33e75d57cd"),
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
                    localization::Tr("event.inline.f0128914e00d4e7f"),
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

    if (gbInCampaign && gpGame->m_campaignAwards[H2EnumIndex(CAMPAIGN_AWARD_DWARFBANE)]
        && (monsterType == CREATURE_DWARF || monsterType == CREATURE_BATTLE_DWARF)) {
        NormalDialog(
            localization::Tr("event.inline.53220bf577626eb3"),
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
                localization::Tr("event.inline.4a9a02fcd852e2b3"),
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
                localization::Tr("event.inline.b957bcfea39c68b5"),
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

    // Join-inclined spawns offer themselves to any army with room outside
    // the retail campaigns, regardless of relative army strength.
    if (forceJoin && eventHero->m_army.CanJoin(monsterType) && !gbInCampaign
        && !(xIsPlayingExpansionCampaign && xCampaign.CampaignID() <= 3)) {
        sprintf(
            gText,
            gEventText[EVENT_TEXT_FOLLOWERS],
            gArmyNamesPlural[H2EnumIndex(monsterType)]
        );
        EventWindow(-1, NORMAL_DIALOG_CONFIRM, gText, -1, 0, -1, 0, -1);
        if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
            eventHero->m_army.Add(monsterType, creatureCount, -1);
            *handled = 1;
            return;
        }
        EventWindow(EVENT_TEXT_MONSTER_REFUSAL, NORMAL_DIALOG_INFO, "", -1, 0, -1, 0, -1);
        goto fightMonsters;
    }

    if (eventHero->m_army.CanJoin(monsterType)
        && armyRatio
            > MONSTER_STRENGTH_JOIN
        && !eventHero->HasArtifact(ARTIFACT_HIDEOUS_MASK) && monsterType != CREATURE_GHOST
        && monsterType != CREATURE_EARTH_ELEMENTAL && monsterType != CREATURE_AIR_ELEMENTAL
        && monsterType != CREATURE_FIRE_ELEMENTAL && monsterType != CREATURE_WATER_ELEMENTAL) {
        if (forceJoin) {
            sprintf(gText, gEventText[EVENT_TEXT_FOLLOWERS], gArmyNamesPlural[H2EnumIndex(monsterType)]);
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
        } else if (eventHero->m_secondarySkills[H2EnumIndex(HERO_SKILL_DIPLOMACY)]
                   != HERO_SKILL_LEVEL_NONE) {
            if (eventHero->m_secondarySkills[H2EnumIndex(HERO_SKILL_DIPLOMACY)]
                == HERO_SKILL_LEVEL_EXPERT)
                numJoining = creatureCount;
            else if (eventHero->m_secondarySkills[H2EnumIndex(HERO_SKILL_DIPLOMACY)]
                     == HERO_SKILL_LEVEL_ADVANCED)
                numJoining = creatureCount / MONSTER_DIPLOMACY_ADVANCED_JOIN_DIVISOR;
            else
                numJoining = creatureCount / MONSTER_DIPLOMACY_BASIC_JOIN_DIVISOR;
            if (!numJoining)
                numJoining = 1;

            joiningCost = gMonsterDatabase[H2EnumIndex(monsterType)].cost * creatureCount;
            if (joiningCost > gpGame->m_players[eventHero->m_owner].m_resources[H2EnumIndex(RES_GOLD)]) {
                if (armyRatio
                    > 5.0  )
                    goto monstersFlee;
                else
                    goto fightMonsters;
            }

            if (creatureCount == 1) {
                sprintf(
                    gText,
                    localization::Tr("event.inline.61cb3563007fe53a"),
                    gArmyNames[H2EnumIndex(monsterType)],
                    joiningCost
                );
            } else {
                utf8::Copy(
                    gText, GLOBAL_TEXT_BUFFER_SIZE,
                    localization::Tr("event.inline.459e418255ebdf2c")
                );
                if (numJoining == creatureCount)
                    sprintf(
                        monsterText,
                        localization::Tr("event.inline.ddc9cf3652f5dd02"),
                        creatureCount,
                        gArmyNamesPlural[H2EnumIndex(monsterType)],
                        joiningCost
                    );
                else
                    sprintf(
                        monsterText,
                        localization::Tr("event.inline.2f685e157f228f6f"),
                        numJoining,
                        creatureCount,
                        gArmyNamesPlural[H2EnumIndex(monsterType)],
                        joiningCost
                    );
                strcat(gText, monsterText);
            }

            NormalDialog(
                gText,
                NORMAL_DIALOG_CONFIRM,
                -1,
                -1,
                H2EnumIndex(RES_GOLD),
                joiningCost,
                -1,
                0,
                -1,
                0
            );
            if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
                eventHero->m_army.Add(monsterType, numJoining, -1);
                *handled = 1;
                gpGame->m_players[eventHero->m_owner].m_resources[H2EnumIndex(RES_GOLD)] -= joiningCost;
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
            gArmyNamesPlural[H2EnumIndex(monsterType)]
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

    ironfist::hooks::MonsterInteraction(cell);
    monsterType = static_cast<CreatureType>(cell->m_objectIndex);
    creatureCount[MONSTER_COMBAT_REMAINING_COUNT] = cell->m_objectMetadata & MONSTER_COUNT_MASK;
    forceJoin = cell->m_objectMetadata & MONSTER_JOIN_FORCED;
    armyRatio = static_cast<double>(
                    gpPhilAI->FightValueOfStack(&eventHero->m_army, eventHero, 0, 0, 0, 0)
                )
                / static_cast<double>(
                    creatureCount[MONSTER_COMBAT_REMAINING_COUNT]
                    * gMonsterDatabase[H2EnumIndex(monsterType)].fightValue
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
            if (eventHero->m_secondarySkills[H2EnumIndex(HERO_SKILL_DIPLOMACY)]
                != HERO_SKILL_LEVEL_NONE) {
                if (eventHero->m_secondarySkills[H2EnumIndex(HERO_SKILL_DIPLOMACY)]
                    == HERO_SKILL_LEVEL_EXPERT)
                    joiningCount = creatureCount[MONSTER_COMBAT_REMAINING_COUNT];
                else if (eventHero->m_secondarySkills[H2EnumIndex(HERO_SKILL_DIPLOMACY)]
                         == HERO_SKILL_LEVEL_ADVANCED)
                    joiningCount = creatureCount[MONSTER_COMBAT_REMAINING_COUNT]
                                   / MONSTER_DIPLOMACY_ADVANCED_JOIN_DIVISOR;
                else
                    joiningCount = creatureCount[MONSTER_COMBAT_REMAINING_COUNT]
                                   / MONSTER_DIPLOMACY_BASIC_JOIN_DIVISOR;
                if (!joiningCount)
                    joiningCount = 1;

                joiningCost = static_cast<i32>(
                    gMonsterDatabase[H2EnumIndex(monsterType)].cost
                    * creatureCount[MONSTER_COMBAT_REMAINING_COUNT]
                    * 0.75
                );
                if (joiningCost
                    > gpGame->m_players[eventHero->m_owner].m_resources[H2EnumIndex(RES_GOLD)]) {
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
                    gpGame->m_players[eventHero->m_owner].m_resources[H2EnumIndex(RES_GOLD)] -= joiningCost;
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
                * gMonsterDatabase[H2EnumIndex(monsterType)].hitPoints,
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
    H2EnumStorage<CombatResult, i8> combatRes;
    i32 initCombatX;

    armyGroup* secondArmy;
    i32 battleX;
    armyGroup* troopFirst;
    i32 otherPlr;
    i32 setupBattleY;
    hero* firstHero;
    i32 combatY;

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
    H2EnumStorage<CombatResult, i8> combatResult3;
    i32 savedShowIt_f;
    i32 secondPlayer8;
    i32 savedPlayer1;

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
            iCombatControlNetPos[H2EnumIndex(COMBAT_ATTACKER_SIDE)] = giThisNetPos;
            iCombatControlNetPos[H2EnumIndex(COMBAT_DEFENDER_SIDE)] = gbGamePosToNetPos[secondPlayer8];
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
                    platform::PumpEvents();
                    message9 = gpInputManager->GetEvent();
                    CheckHandleNetPlayerWait(message9, 1);
                }
            }
        } else if (!gbThisNetHumanPlayer[firstPlayer4]) {
            bShowIt = 1;
            gpGame->TurnOffAIMusic();
            sprintf(
                gText,
                localization::Tr(combatTown ? "network.attack.town" : "network.attack.hero"),
                cPlayerNames[secondPlayer8]
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
    bShowIt = savedShowIt_f;
    giCurPlayer = savedPlayer1;
    if (!gbHumanPlayer[giCurPlayer]) {
        gpGame->ShowComputerScreen();
        gpGame->TurnOnAIMusic();
        platform::SetDialogMenusEnabled(0);
    } else {
        platform::SetDialogMenusEnabled(1);
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
        firstHero ? gpGame->m_players[firstHero->m_owner].m_resources[H2EnumIndex(RES_GOLD)] : 0;
    buffer->secondOwner = static_cast<i8>(secondHero ? secondHero->m_owner : -1);
    buffer->secondGold =
        secondHero ? gpGame->m_players[secondHero->m_owner].m_resources[H2EnumIndex(RES_GOLD)] : 0;
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
    H2EnumStorage<CombatResult, i8>* combatResult,
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
        gpGame->m_players[firstOwner8].m_resources[H2EnumIndex(RES_GOLD)] =
            EVENTS_REMOTE_COMBAT(packet)->firstGold;
    secondOwner = EVENTS_REMOTE_COMBAT(packet)->secondOwner;
    if (secondOwner > 0)
        gpGame->m_players[secondOwner].m_resources[H2EnumIndex(RES_GOLD)] =
            EVENTS_REMOTE_COMBAT(packet)->secondGold;

    *firstArmy = static_cast<armyGroup*>(H2_ALLOC(sizeof(armyGroup)));
    memcpy(*firstArmy, &EVENTS_REMOTE_COMBAT(packet)->firstArmy, sizeof(armyGroup));
    *secondArmy = static_cast<armyGroup*>(H2_ALLOC(sizeof(armyGroup)));
    memcpy(*secondArmy, &EVENTS_REMOTE_COMBAT(packet)->secondArmy, sizeof(armyGroup));
    if (hasTown2) {
        *combatTown = static_cast<town*>(H2_ALLOC(sizeof(town)));
        memcpy(*combatTown, &EVENTS_REMOTE_COMBAT(packet)->combatTown, sizeof(town));
    }

    iCombatControlNetPos[H2EnumIndex(COMBAT_ATTACKER_SIDE)] = *remotePlayer;
    iCombatControlNetPos[H2EnumIndex(COMBAT_DEFENDER_SIDE)] = giThisNetPos;
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

    lastPacketTime7 = platform::Ticks();
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
        if (lastPacketTime7 + COMBAT_REMOTE_TIMEOUT < platform::Ticks()) {
            NormalDialog(
                localization::Tr("event.inline.2b436715930a57ee"),
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
                lastPacketTime7 = platform::Ticks();
            else
                ShutDown(localization::Tr("event.inline.d7ee33967c36bb8b"));
        }
        packet = GetRemoteData(1);
        if (packet && EVENTS_REMOTE_MESSAGE(packet)->type == REMOTE_MESSAGE_RELIABLE
            && EVENTS_REMOTE_MESSAGE(packet)->command == REMOTE_COMMAND) {
            lastPacketTime7 = platform::Ticks();
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
    i32,
    i32,
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
        platform::SetDialogMenusEnabled(0);
    } else {
        platform::SetDialogMenusEnabled(1);
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

    const auto copyPrefix = [](char* destination, const char* source) {
        i32 count = 0;
        for (; count < RIDDLE_PREFIX_LENGTH && source[count] != 0; count++)
            destination[count] = source[count];
        for (; count < RIDDLE_PREFIX_LENGTH; count++)
            destination[count] = 0;
    };

    copyPrefix(expectedPrefix, expected);
    expectedPrefix[RIDDLE_PREFIX_LENGTH] = 0;
    for (index = RIDDLE_PREFIX_LENGTH - 1; index >= 0; index--) {
        if (expectedPrefix[index] != ' ') {
            expectedPrefix[index + 1] = 0;
            break;
        }
    }
    copyPrefix(answerPrefix, answer);
    answerPrefix[strlen(expectedPrefix)] = 0;
    return platform::CompareIgnoringCase(expectedPrefix, answerPrefix) == 0;
}

b32 gbNoShowCombat = false;
