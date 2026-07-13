#ifndef HOMM2_PHILAI_TU_H
#define HOMM2_PHILAI_TU_H
#include <va.h>
// AI view/record structs + free functions DEFINED in PHILAI.cpp — single home.
// (tag_tilePoint / monsterRV are shared, in _types.h.)
// forward declarations (was <_all.h>):
class army;
class hero;
class playerData;
class searchArray;
class town;
#include <_types.h>

struct BHC {
    town *pTown;     // 0x00
    int type;        // 0x04  (0=building, 1=hero, 2=creature)
    int what;        // 0x08
    int num;         // 0x0c
};

typedef enum AIResourceValue {
    AI_RV_UNSET = -32001,
    AI_NEARBY_RADIUS = 10,
    AI_HERO_COUNT = 54,
    AI_MAP_SIZE_SMALL = 36,
    AI_MAP_SIZE_LARGE = 108,
    AI_MAP_SIZE_XLARGE = 144
} AIResourceValue;

typedef enum AIObjectType {
    AI_OBJECT_ALCHEMIST_LAB = 0x01,
    AI_OBJECT_SIGN = 0x02,
    AI_OBJECT_BUOY = 0x03,
    AI_OBJECT_SKELETON = 0x04,
    AI_OBJECT_DAEMON_CAVE = 0x05,
    AI_OBJECT_TREASURE_CHEST = 0x06,
    AI_OBJECT_FAERIE_RING = 0x07,
    AI_OBJECT_CAMPFIRE = 0x08,
    AI_OBJECT_FOUNTAIN = 0x09,
    AI_OBJECT_GAZEBO = 0x0a,
    AI_OBJECT_GENIE_LAMP = 0x0b,
    AI_OBJECT_GRAVEYARD = 0x0c,
    AI_OBJECT_ARCHER_HOUSE = 0x0d,
    AI_OBJECT_GOBLIN_HUT = 0x0e,
    AI_OBJECT_DWARF_COTTAGE = 0x0f,
    AI_OBJECT_PEASANT_HUT = 0x10,
    AI_OBJECT_DRAGON_CITY = 0x14,
    AI_OBJECT_LIGHTHOUSE = 0x15,
    AI_OBJECT_WATER_WHEEL = 0x16,
    AI_OBJECT_MINE = 0x17,
    AI_OBJECT_MONSTER = 0x18,
    AI_OBJECT_TOWN_EVENT = 0x23,
    AI_OBJECT_ARTIFACT = 0x29,
    AI_OBJECT_HERO_EVENT = 0x2a,
    AI_OBJECT_BOAT_EVENT = 0x2b,
    AI_OBJECT_TOWN = 0xa3,
    AI_OBJECT_HERO = 0xaa,
    AI_OBJECT_BOAT = 0xab,
    AI_OBJECT_BOAT_TRIGGER = 0x1c,
    AI_OBJECT_OBELISK = 0x19,
    AI_OBJECT_OASIS = 0x1a,
    AI_OBJECT_RESOURCE = 0x1b,
    AI_OBJECT_SAWMILL = 0x1d,
    AI_OBJECT_SHRINE_FIRST = 0x1f,
    AI_OBJECT_SHIPWRECK = 0x20,
    AI_OBJECT_SEA_CHEST = 0x21,
    AI_OBJECT_DESERT_TENT = 0x22,
    AI_OBJECT_CASTLE = 0x23,
    AI_OBJECT_STONE_LITHS = 0x24,
    AI_OBJECT_WAGON_CAMP = 0x25,
    AI_OBJECT_HUT_OF_MAGI = 0x26,
    AI_OBJECT_WHIRLPOOL = 0x27,
    AI_OBJECT_WINDMILL = 0x28,
    AI_OBJECT_RANDOM_ULTIMATE_ARTIFACT = 0x2c,
    AI_OBJECT_WATCH_TOWER = 0x3a,
    AI_OBJECT_TREE_HOUSE = 0x3b,
    AI_OBJECT_TREE_CITY = 0x3c,
    AI_OBJECT_RUINS = 0x3d,
    AI_OBJECT_FORT = 0x3e,
    AI_OBJECT_TRADING_POST = 0x3f,
    AI_OBJECT_ABANDONED_MINE = 0x40,
    AI_OBJECT_SIRENS = 0x41,
    AI_OBJECT_STANDING_STONES = 0x42,
    AI_OBJECT_IDOL = 0x43,
    AI_OBJECT_TREE_OF_KNOWLEDGE = 0x44,
    AI_OBJECT_WITCH_DOCTORS_HUT = 0x45,
    AI_OBJECT_TEMPLE = 0x46,
    AI_OBJECT_HILL_FORT = 0x47,
    AI_OBJECT_HALFLING_HOLE = 0x48,
    AI_OBJECT_MERCENARY_CAMP = 0x49,
    AI_OBJECT_SHRINE_SECOND = 0x4a,
    AI_OBJECT_SHRINE_THIRD = 0x4b,
    AI_OBJECT_PYRAMID = 0x4c,
    AI_OBJECT_CITY_OF_DEAD = 0x4d,
    AI_OBJECT_EXCAVATION = 0x4e,
    AI_OBJECT_SPHINX = 0x4f,
    AI_OBJECT_WAGON = 0x50,
    AI_OBJECT_ARTESIAN_SPRING = 0x52,
    AI_OBJECT_TROLL_BRIDGE = 0x53,
    AI_OBJECT_WATERING_HOLE = 0x54,
    AI_OBJECT_WITCHS_HUT = 0x55,
    AI_OBJECT_XANADU = 0x56,
    AI_OBJECT_CAVE = 0x57,
    AI_OBJECT_LEAN_TO = 0x58,
    AI_OBJECT_MAGELLANS_MAPS = 0x59,
    AI_OBJECT_FLOTSAM = 0x5a,
    AI_OBJECT_DERELICT_SHIP = 0x5b,
    AI_OBJECT_SHIPWRECK_SURVIVOR = 0x5c,
    AI_OBJECT_BOTTLE = 0x5d,
    AI_OBJECT_MAGIC_WELL = 0x5e,
    AI_OBJECT_MAGIC_GARDEN = 0x5f,
    AI_OBJECT_OBSERVATION_TOWER = 0x60,
    AI_OBJECT_FREEMANS_FOUNDRY = 0x61,
    AI_OBJECT_BARRIER = 0x77,
    AI_OBJECT_TRAVELLER_TENT = 0x78,
    AI_OBJECT_EXPANSION_DWELLING = 0x79,
    AI_OBJECT_EXPANSION_OBJECT = 0x7a,
    AI_OBJECT_JAIL = 0x7b,
    AI_EXPANSION_OBJECT_THRESHOLD = 108
} AIObjectType;

typedef enum AISpellType {
    AI_SPELL_DIMENSION_DOOR = 0x38
} AISpellType;

typedef enum AIArtifactType {
    AI_ARTIFACT_MAGIC_BOOK = 0x51
} AIArtifactType;

typedef enum AICreatureType {
    AI_CREATURE_PEASANT = 0x00,
    AI_CREATURE_ARCHER = 0x01,
    AI_CREATURE_PIKEMAN = 0x03,
    AI_CREATURE_VETERAN_PIKEMAN = 0x04,
    AI_CREATURE_SWORDSMAN = 0x05,
    AI_CREATURE_MASTER_SWORDSMAN = 0x06,
    AI_CREATURE_GOBLIN = 0x0b,
    AI_CREATURE_ORC = 0x0c,
    AI_CREATURE_ORC_CHIEF = 0x0d,
    AI_CREATURE_OGRE = 0x0f,
    AI_CREATURE_OGRE_LORD = 0x10,
    AI_CREATURE_TROLL = 0x11,
    AI_CREATURE_SPRITE = 0x14,
    AI_CREATURE_DWARF = 0x15,
    AI_CREATURE_BATTLE_DWARF = 0x16,
    AI_CREATURE_CENTAUR = 0x1d,
    AI_CREATURE_RED_DRAGON = 0x24,
    AI_CREATURE_HALFLING = 0x26,
    AI_CREATURE_IRON_GOLEM = 0x28,
    AI_CREATURE_STEEL_GOLEM = 0x29,
    AI_CREATURE_SKELETON = 0x2f,
    AI_CREATURE_POWER_LICH = 0x37,
    AI_CREATURE_ROGUE = 0x39,
    AI_CREATURE_NOMAD = 0x3a,
    AI_CREATURE_GENIE = 0x3c,
    AI_CREATURE_MEDUSA = 0x3d
} AICreatureType;

typedef enum AIPurchaseType {
    AI_PURCHASE_BUILDING = 0,
    AI_PURCHASE_HERO = 1,
    AI_PURCHASE_CREATURE = 2
} AIPurchaseType;

typedef enum AIBattleConstant {
    AI_BATTLE_NO_PLAYER = -1,
    AI_BATTLE_EASY_DIFFICULTY = 0,
    AI_BATTLE_ARTIFACT_SLOT_COUNT = 14,
    AI_BATTLE_BASE_ARTIFACT_LIMIT = 37,
    AI_BATTLE_SPECIAL_ARTIFACT_STATE = 2,
    AI_BATTLE_ATTACKER_ARTIFACT_BASE = 1400,
    AI_BATTLE_DEFENDER_ARTIFACT_BASE = 1250,
    AI_BATTLE_SPECIAL_ARTIFACT_VALUE = 50000,
    AI_BATTLE_DEBUG_LEVEL = 9
} AIBattleConstant;

typedef enum AIHeroInteractionConstant {
    AI_HERO_INTERACTION_HERO_COUNT = 2,
    AI_HERO_INTERACTION_PRIMARY_STAT_COUNT = 4,
    AI_HERO_INTERACTION_PRIMARY_STAT_VALUE = 800
} AIHeroInteractionConstant;

typedef enum AICreaturePurchaseConstant {
    AI_CREATURE_PURCHASE_NO_SLOT = -1,
    AI_CREATURE_PURCHASE_ARMY_SLOT_COUNT = 5,
    AI_CREATURE_PURCHASE_VALUE_LIMIT = 999999
} AICreaturePurchaseConstant;

typedef enum AIQuickCombatConstant {
    AI_QUICK_COMBAT_TOWN_EXPERIENCE = 500,
    AI_QUICK_COMBAT_RANDOM_LIMIT = 100,
    AI_QUICK_COMBAT_RETREAT_CHANCE = 60,
    AI_QUICK_COMBAT_SPELL_COUNT = 65,
    AI_QUICK_COMBAT_LEARNABLE_SPELL = 2
} AIQuickCombatConstant;

typedef enum AIBuildingType {
    AI_BUILDING_MAGE_GUILD = 0,
    AI_BUILDING_SPECIAL_ONE = 1,
    AI_BUILDING_SPECIAL_TWO = 2,
    AI_BUILDING_SPECIAL_THREE = 3,
    AI_BUILDING_SPECIAL_FOUR = 4,
    AI_BUILDING_SPECIAL_SIX = 6,
    AI_BUILDING_SPECIAL_SEVEN = 7,
    AI_BUILDING_FIRST_DWELLING = 19,
    AI_BUILDING_SECOND_DWELLING = 20,
    AI_BUILDING_THIRD_DWELLING = 21,
    AI_BUILDING_FIRST_UPGRADE = 25,
    AI_BUILDING_LAST_UPGRADE = 29,
    AI_BUILDING_LAST_DWELLING = 30
} AIBuildingType;

#define AI_MINIMUM_FIGHT_VALUE 100
#define AI_BERSERK_THRESHOLD 30000
#define AI_RELOAD_BASE 2.0f
#define AI_RELOAD_NUMERATOR 3.0f
#define AI_EASY_STRENGTH_FACTOR 1.15
#define AI_BERSERK_FACTOR 0.75
#define AI_HERO_VALUE_SCALE 0.00004
#define AI_HERO_VALUE_BASE 0.4
#define AI_ARTIFACT_POOL_DIVISOR 33.0
#define AI_BATTLE_TOWN_DEFENDER_FACTOR 1.11
#define AI_BATTLE_DIFFICULTY_STEP 0.15
#define AI_BATTLE_BASE_STRENGTH_FACTOR 0.7
#define AI_BATTLE_EASY_STRENGTH_FACTOR 1.08
#define AI_BATTLE_HUMAN_DEFENDER_FACTOR 1.14
#define AI_BATTLE_MINIMUM_STRENGTH 1.0f
#define AI_BATTLE_FULL_CHANCE 1.0f
#define AI_BATTLE_LARGE_STRENGTH 1000000.0f
#define AI_BATTLE_NORMAL_POWER 2.75f
#define AI_BATTLE_LARGE_POWER 2.0f
#define AI_BATTLE_ZERO_CHANCE 0.08
#define AI_BATTLE_LOW_CHANCE 0.12
#define AI_BATTLE_LOW_PENALTY 0.07
#define AI_BATTLE_MEDIUM_CHANCE 0.2
#define AI_BATTLE_MEDIUM_PENALTY 0.05
#define AI_BATTLE_HIGH_CHANCE 0.3
#define AI_BATTLE_HIGH_PENALTY 0.04
#define AI_BATTLE_TOP_CHANCE 0.4
#define AI_BATTLE_TOP_PENALTY 0.02
#define AI_BATTLE_LOSS_FACTOR_BASE 1.33
#define AI_BATTLE_PLAYER_FACTOR_BASE 0.66
#define AI_BATTLE_HERO_EXPERIENCE_FACTOR 0.8
#define AI_BATTLE_PERCENT_SCALE 100.0f
#define AI_QUICK_COMBAT_EXPERIENCE_DIFFICULTY_STEP 0.12
#define AI_QUICK_COMBAT_WIN_BONUS_THRESHOLD 0.6
#define AI_QUICK_COMBAT_WIN_BONUS 0.65
#define AI_QUICK_COMBAT_HIGH_WIN_CHANCE 0.8
#define AI_QUICK_COMBAT_MINIMUM_CASUALTY 0.2
#define AI_QUICK_COMBAT_CASUALTY_CAP_CHANCE 0.96
#define AI_QUICK_COMBAT_CASUALTY_DIVISOR 2.0f
#define AI_QUICK_COMBAT_DAMAGE_PENALTY_LOW 0.6
#define AI_QUICK_COMBAT_DAMAGE_PENALTY_HIGH 0.99
#define AI_QUICK_COMBAT_DAMAGE_PENALTY 0.1
#define AI_QUICK_COMBAT_DEFEAT_THRESHOLD 0.99
#define AI_QUICK_COMBAT_NECROMANCY_THRESHOLD 0.999
#define AI_QUICK_COMBAT_NECROMANCY_FACTOR 0.12
#define AI_TOWN_DEFENSE_THRESHOLD 0.7
#define AI_HERO_INTERACTION_ALPHA_SHARE 0.6
#define AI_HERO_INTERACTION_TARGET_SHARE 0.7f
#define AI_HERO_INTERACTION_MINIMUM_TRANSFER 0.15
#define AI_HERO_INTERACTION_VALUE_FACTOR 0.1
#define AI_PURCHASE_RANDOM_DIVISOR 100.0
#define AI_CREATURE_BALANCE_BASE 0.66
#define AI_HERO_PURCHASE_BONUS 500.0f
#define AI_HERO_PURCHASE_FACTOR 1.3
#define AI_MINIMUM_PURCHASE_VALUE 0.02
#define AI_PLAYER_HERO_IDS_BASE 0x4a0
#define AI_DIMENSION_DOOR_SPELL_POINTS 30
#define AI_MAX_MAGE_GUILD_LEVEL 5
#define AI_TOWN_ARMY_SLOTS 5
#define AI_BUILDING_MAGE_GUILD_MASK 0x00000001
#define AI_BUILDING_SHIPYARD_MASK 0x00000008
#define AI_BUILDING_CASTLE_MASK 0x40
#define AI_BUILDING_LEFT_TURRET_MASK 0x00000100
#define AI_BUILDING_RIGHT_TURRET_MASK 0x00000200
#define AI_MAGIC_BOOK_COST 500
#define AI_MANA_PER_KNOWLEDGE 10
#define AI_EARLY_TOWN_SHARE_TURN 7
#define AI_SECOND_WEEK_END_TURN 14
#define AI_THIRD_WEEK_END_TURN 21
#define AI_MINIMUM_TOWN_SHARE_DIFFERENCE 0.08
#define AI_TOWN_SHARE_DIFFERENCE_FACTOR 0.15
#define AI_TOWN_TRANSFER_BONUS 0.05
#define AI_WEAKER_ARMY_TRANSFER_FACTOR 0.25f
#define AI_STRONGER_ARMY_TRANSFER_FACTOR 0.13f
#define AI_TOWN_TRANSFER_CURVE_OFFSET 1.0f
#define AI_TOWN_TRANSFER_CURVE_CENTER 0.22
#define AI_CONQUERED_HERO_MOBILITY_LIMIT 20
#define AI_UNGUARDED_TOWN_VALUE 500
#define AI_TROOP_EMPTY_SLOT -1
#define AI_TROOP_REDISTRIBUTION_WORST_VALUE -9999
#define AI_TROOP_REDISTRIBUTION_MIN_SPEED 1
#define AI_TROOP_REDISTRIBUTION_MAX_SPEED 9
#define AI_TROOP_REDISTRIBUTION_ROUNDING 0.5
#define AI_TROOP_REDISTRIBUTION_STACK_SHARE 0.65
#define AI_TROOP_REDISTRIBUTION_REMAINDER_FACTOR 0.2
#define AI_TROOP_REDISTRIBUTION_BUDGET_FACTOR 1.2
#define AI_DWELLING_LEVELS 12
#define AI_GOLD_RESOURCE_MULTIPLIER 100
#define AI_CAMPFIRE_RESOURCE_AMOUNT 4.0f
#define AI_CAMPFIRE_GOLD_AMOUNT 400.0f
#define AI_CAMPFIRE_AVERAGE_DIVISOR 2.0f
#define AI_OASIS_VALUE_FACTOR 350.0f
#define AI_MORALE_LUCK_SITE_VALUE_FACTOR 200.0f
#define AI_WATERING_HOLE_VALUE_FACTOR 400.0f
#define AI_GAZEBO_VALUE_FACTOR 1000.0f
#define AI_TRAINING_SITE_VALUE_FACTOR 1000.0f
#define AI_TREE_KNOWLEDGE_VALUE_FACTOR 1500.0f
#define AI_TREE_KNOWLEDGE_GOLD_COST 2000.0f
#define AI_TREE_KNOWLEDGE_GEM_COST 10.0f
#define AI_XANADU_VALUE_FACTOR 8000.0f
#define AI_TRAVEL_GATE_INITIAL_VALUE -9999
#define AI_TRAVEL_GATE_EXIT_SCALE 0.75
#define AI_TRAVEL_GATE_EXIT_RADIUS 3
#define AI_TRAVEL_GATE_EXIT_DEPTH 700
#define AI_TRAVEL_GATE_CURRENT_DEPTH 500
#define AI_TRAVEL_GATE_PENALTY 200
#define AI_MAX_BATTLE_STAT 40
#define AI_DAEMON_FIGHT_VALUE_FACTOR 300.0
#define AI_DAEMON_SECONDARY_FIGHT_VALUE_FACTOR 100.0
#define AI_DAEMON_GOLD_VALUE_FACTOR 2500.0f
#define AI_DAEMON_GOLD_PENALTY -750.0
#define AI_DAEMON_CAVE_GOLD_REQUIRED 2500
#define AI_MAGIC_GARDEN_GOLD_AMOUNT 175.0f
#define AI_FLOTSAM_GOLD_AMOUNT 175.0f
#define AI_FLOTSAM_WOOD_AMOUNT 5.0f
#define AI_SEA_CHEST_ARTIFACT_GOLD_AMOUNT 1000.0f
#define AI_SEA_CHEST_LARGE_GOLD_AMOUNT 1500.0f
#define AI_SEA_CHEST_SMALL_GOLD_AMOUNT 200.0f
#define AI_SEA_CHEST_MINIMUM_VALUE 150
#define AI_WATER_WHEEL_GOLD_AMOUNT 500

// ---- AI member views: the recovered classes carry no field layout, so model the
// exact offsets the /Od code touches as packed views and cast to them. ----
#pragma pack(push, 1)
struct pdView {                  // playerData
    char _0;
    signed char numHeroes;       // 0x01
    char _2[2];
    signed char heroIds[8];      // 0x04
    char _c[2];
    unsigned char minimumHeroes; // 0x0e
    int difficulty;              // 0x0f
    char _13[0x40 - 0x13];
    signed char routeLength;       // 0x40
    signed char routeX;            // 0x41
    signed char routeY;            // 0x42
    char _43;
    signed char numCastles;      // 0x44
    char _45[2];
    signed char castleIds[8];    // 0x47
    char _4f[0x8f - 0x4f];
    int resources[7];            // 0x8f (gold = [6] @ 0xa7)
    char _ab;
    signed char barrierTents;    // 0xac
    char _ad[0xbf - 0xad];
    float buildingFactor;        // 0xbf
    float baseUpgradeFactor;     // 0xc3
    char _c7[0xe7 - 0xc7];
    int income[7];               // 0xe7
    int obeliskValue;            // 0x103
    int totalObeliskValue;       // 0x107
    int unexploredValue;         // 0x10b
    float upgradeFactor;         // 0x10f
    float artifactValue;         // 0x113
    float artifactPoolShare;     // 0x117
    signed char getHeroId(int i) { return heroIds[i]; }
    signed char getCastleId(int i) { return castleIds[i]; }
};
struct gameObeliskView {         // game
    char _0[0x634c];
    signed char obeliskFlags[1]; // 0x634c
};
struct heroView {                // hero
    signed short curMana;        // 0x00
    unsigned char heroId;        // 0x02
    char _3[0x65 - 0x03];
    signed char army[0x74 - 0x65]; // 0x65 (embedded armyGroup creature types)
    signed char skills[0x90 - 0x74]; // 0x74
    int level;                   // 0x90
};
struct aiHeroPositionRecord {
    char _0[0x19];
    int x;                       // 0x19
    int y;                       // 0x1d
    char _21[250 - 0x21];
};
struct gameHeroPositionView {    // game hero position records used by PHILAI
    char _0[0x27cc];
    aiHeroPositionRecord heroRecs[AI_HERO_COUNT];
};
struct armyView {                // armyGroup
    signed char creatureTypes[5]; // 0x00
    signed short quantities[5];   // 0x05
};
struct townView {                // town
    signed char id;              // 0x00
    signed char owner;           // 0x01
    signed char threat;           // 0x02
    signed char race;            // 0x03
    unsigned char castleX;       // 0x04
    unsigned char castleY;       // 0x05
    unsigned char dockX;         // 0x06
    unsigned char dockY;         // 0x07
    armyView army;               // 0x08
    signed char visitingHero;    // 0x17
    int buildings;               // 0x18
    signed char buildState;      // 0x1c
    char _1d;
    signed short garrison[5];    // 0x1e
    char _28[0x3c - 0x28];
    signed char mageGuildSpells[AI_MAX_MAGE_GUILD_LEVEL * 4]; // 0x3c
    signed char mageGuildSpellCounts[AI_MAX_MAGE_GUILD_LEVEL]; // 0x50
    unsigned short turnsOwned;   // 0x55
    signed char getVisitingHero() { return visitingHero; }
};
struct taView {                  // per-player AI "turn attention" record (gpGame+0x49e + player*283)
    float f0, f4, f8, fc, f10, f14; // 0x00..0x14
    char _18[0x34 - 0x18];
    int field34[7];              // 0x34..0x4f
    int field50;                 // 0x50
};
struct mapCellView {             // mapCell
    char _0[4];
    unsigned short _bits : 3;
    unsigned short tentColor : 13; // bits 3..15 of word @ 0x04
};
struct gameTV {                  // game fields used by ValueOfTown (modelled so /Od evals L->R)
    char _0[0x2c6];
    unsigned char f2c6;          // 0x2c6
    char _2c7[2];
    unsigned short f2c9;         // 0x2c9
    unsigned char f2cb;          // 0x2cb
    unsigned short f2cc;         // 0x2cc
    char _2ce[0x2d5 - 0x2ce];
    unsigned short f2d5;         // 0x2d5
    unsigned short f2d7;         // 0x2d7
};
#pragma pack(pop)

// gpGame view for the per-player AI hero-slot 2D array at 0x4a0 (row stride 283),
// modelled as a struct member so /Od keeps 0x4a0 as the load displacement and folds
// the row multiply with the column index.
struct gameView {
    char _0[0x4a0];
    signed char heroSlots[40][283];  // 0x4a0
};

// Declarations of the free functions DEFINED in PHILAI.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).
void ResetHeroRVs(int, int, int);
void CheckDoMain(int a1, int doMain);
void ShowStatus(void);
void ValidateHero(hero *pHero);
void InitAIMapVars(void);
void CloseAIMapVars(void);
int OnMySide(int);


// --- globals owned by this TU (moved from _globals.h; CodeView-attributed) ---
extern int bHeroBuiltThisTurn;
extern float gafAITurnCostResource[7];
extern signed char *gaiEnemyHeroReachable;
extern short *gaiHeroEventStratRVOfPos;
extern short *gaiHeroStrategicRVOfPos;
extern short *gaiLiveChanceOfPos;
extern signed char *gaiTurnValueOfMine;
extern int gbReduceByReload;
extern signed char giBuildBoat[6];
extern signed char giBuildBoatStuffTurn[6];
extern signed char giBuildShipyard[6];
extern int giCurPlayer;
extern unsigned char giCurPlayerBit;
extern int giCurTurn;
extern hero *gpCurAIHero;
extern playerData *gpCurPlayer;
extern unsigned char giCurWatchPlayerBit;
extern int iAlphaMale;
extern int iDummy;
extern int iLastFrameRateTimer;
extern searchArray SVSearchArray;

// ---- globals (declarations, RVA order) ----
extern int gbGameOver;
extern signed char giMonType[];
extern int iViewArmyNumTroops;
extern signed char *gbNGHeroType;
extern short giUABaseX;
extern short giUABaseY;
extern int giEndSequence;
extern int gbDismissArmy;
extern signed char *gbNGHuman;
extern int iViewArmyFrame;
extern int gbAllowUpgrade;
extern int iViewArmyType;
extern class hero *viewSpellsHero;
extern int gbUpgradeArmy;
extern short RandMineQty[8];
extern char *gcCurMapName;
extern signed char *gbNGDifficulty;
extern int iViewArmyUpgradeToType;
extern int viewArmyBaseX;
extern int viewArmyBaseY;
extern signed char *gbNGColor;
extern short giUARadius;
extern signed char *gbNGPlayerPos;
extern int viewArmyFacingWIPXMod;

#endif // HOMM2_PHILAI_TU_H
