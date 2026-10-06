#ifndef HOMM2_EDITOR_RANDOM_H
#define HOMM2_EDITOR_RANDOM_H

// The random map generator (src/EDITOR/RANDOM.cpp, assertion path
// Editor\RANDOM.CPP): editManager methods that paint terrain, lay mountain
// and tree chains and place towns, objects and treasure.

#include <va.h>
#include <Ints.h>
#include <SOURCE/KB_TYPES.h>

struct overlayType;

// A cell offset, kept as a plain (x, y) pair in the chain tables.
H2_ENUM_BEGIN(MapStepAxis)
    MAP_STEP_X    = 0,
    MAP_STEP_Y    = 1,
    MAP_STEP_AXES = 2
H2_ENUM_END(MapStepAxis)
typedef i32 MapStepPair[MAP_STEP_AXES];

// A cell offset or position.
struct mapStep {
    i32 x;
    i32 y;
};

H2_ENUM_BEGIN(RandomMapConstant)
    // GenerateRandomMap retries a map without enough castles this often.
    RANDOM_MAP_ATTEMPTS          = 5,
    // PaintRandomTerrain's percent that covers the whole map; densities and
    // terrain shares are percents.
    RANDOM_MAP_FULL_PERCENT      = 100,
    // ScaleByDensity leaves a count unchanged at this density.
    RANDOM_MAP_NEUTRAL_DENSITY   = 50,
    // GenerateRandomMap's terrain index past the last terrain once the base
    // terrain is painted.
    RANDOM_MAP_END_TERRAIN_SCAN  = 99,
    // PaintRandomTerrain drifts a seed every eighth step and its walk
    // weights every 64th.
    RANDOM_MAP_SEED_DRIFT_MASK   = 7,
    RANDOM_MAP_WEIGHT_DRIFT_MASK = 0x3f,
    // PaintRandomTerrain gives up a seed search after this many tries and a
    // walk after this many steps (a step against the map's edge costs
    // RANDOM_MAP_EDGE_STEP_COST more); it grows at most this many seeds.
    RANDOM_MAP_SEED_TRIES        = 200,
    RANDOM_MAP_WALK_LIMIT        = 250,
    RANDOM_MAP_EDGE_STEP_COST    = 50,
    RANDOM_MAP_ESCAPED_WALK      = 1000,
    RANDOM_MAP_SEED_LIMIT        = 20,
    // RemoveSmallRegions merges a region of at most this many cells, and
    // scans neighbours against the largest map's last cell.
    RANDOM_MAP_SMALL_REGION_SIZE = 15,
    RANDOM_MAP_LARGEST_LAST_CELL = 71,
    // PlaceObstacleChains: a chain per this many land cells (scaled by the
    // density), each worth this many placements, a link costing this many;
    // a root with mountains or trees within RANDOM_MAP_CHAIN_SPACING cells
    // is rerolled up to this often.
    RANDOM_MAP_LAND_PER_CHAIN    = 30,
    RANDOM_MAP_CHAIN_BUDGET      = 13,
    RANDOM_MAP_CHAIN_LINK_COST   = 12,
    RANDOM_MAP_CHAIN_SPACING     = 5,
    RANDOM_MAP_CROWDED_TRIES     = 10,
    // CountNearbyObstacles' count for the map's corner.
    RANDOM_MAP_CROWDED           = 100,
    // gMineResources: the five mines' resources.
    RANDOM_MAP_MINE_RESOURCE_COUNT = 5,
    // PlaceTowns: a castle per player and the land regions it numbers; a
    // scan ends by setting its counters past the map.
    RANDOM_MAP_CASTLE_SLOTS      = 6,
    RANDOM_MAP_REGION_LIMIT      = 255,
    RANDOM_MAP_END_SCAN          = 999,
    // PlaceTowns: a castle on another region than its peers digs its
    // harbour without a step limit.
    RANDOM_MAP_UNLIMITED_STEPS   = 999,
    // PlaceRandomObjects: a town per this many land cells (at most this
    // many), a resource site per this many and an obelisk per this many,
    // each with this many placement tries.
    RANDOM_MAP_LAND_PER_TOWN     = 640,
    RANDOM_MAP_MAX_TOWNS         = 22,
    RANDOM_MAP_TRIES_PER_OBJECT  = 100,
    RANDOM_MAP_LAND_PER_SITE     = 140,
    RANDOM_MAP_MIN_SITES         = 6,
    RANDOM_MAP_MAX_SITES         = 34,
    RANDOM_MAP_TRIES_PER_SITE    = 1000,
    RANDOM_MAP_LAND_PER_OBELISK = 180,
    RANDOM_MAP_MIN_OBELISKS     = 8,
    RANDOM_MAP_MAX_OBELISKS     = 24,
    // PlaceTreasures: a treasure per this many land cells and a roaming
    // monster per this many, each scaled by its density.
    RANDOM_MAP_LAND_PER_TREASURE = 40,
    RANDOM_MAP_LAND_PER_MONSTER  = 130
H2_ENUM_END(RandomMapConstant)

// The catalogue's random castle of each player and the stone liths.
H2_ENUM_BEGIN(RandomMapOverlay)
    OVERLAY_STONE_LITHS     = 777,
    OVERLAY_RANDOM_CASTLE_0 = 939,
    OVERLAY_RANDOM_CASTLE_1 = 943,
    OVERLAY_RANDOM_CASTLE_2 = 945,
    OVERLAY_RANDOM_CASTLE_3 = 947,
    OVERLAY_RANDOM_CASTLE_4 = 949,
    OVERLAY_RANDOM_CASTLE_5 = 951,
    // PlaceResourceSite: the sawmill and alchemist's lab of each ground, and
    // where each ground's mines start (a mine is its resource past it).
    OVERLAY_SAWMILL_SNOW        = 556,
    OVERLAY_SAWMILL_LAVA        = 618,
    OVERLAY_SAWMILL_DESERT      = 661,
    OVERLAY_SAWMILL_WASTELAND   = 743,
    OVERLAY_SAWMILL_DIRT        = 774,
    OVERLAY_SAWMILL_GRASS       = 789,
    OVERLAY_ALCHEMIST_LAB_SNOW  = 552,
    OVERLAY_ALCHEMIST_LAB       = 770,
    OVERLAY_MINES_WATER         = 4,
    OVERLAY_MINES_WASTELAND     = 17,
    OVERLAY_MINES_GRASS         = 28,
    OVERLAY_MINES_SNOW          = 39,
    OVERLAY_MINES_SWAMP         = 50,
    OVERLAY_MINES_LAVA          = 61,
    OVERLAY_MINES_DESERT        = 72,
    OVERLAY_MINES_DIRT          = 85,
    // PlaceRandomObjects: the random monsters, the random town and each
    // ground's obelisk.
    OVERLAY_RANDOM_MONSTER             = 214,
    OVERLAY_RANDOM_MONSTER_WEAK        = 215,
    OVERLAY_RANDOM_MONSTER_MEDIUM      = 216,
    OVERLAY_RANDOM_MONSTER_STRONG      = 217,
    OVERLAY_RANDOM_MONSTER_VERY_STRONG = 218,
    OVERLAY_RANDOM_TOWN                = 954,
    OVERLAY_OBELISK_GRASS             = 514,
    OVERLAY_OBELISK_SNOW              = 550,
    OVERLAY_OBELISK_SWAMP             = 598,
    OVERLAY_OBELISK_LAVA              = 615,
    OVERLAY_OBELISK_DESERT            = 655,
    OVERLAY_OBELISK_DIRT              = 709,
    OVERLAY_OBELISK_WASTELAND         = 742,
    // PlaceTreasures: the random artifacts and the treasures.
    OVERLAY_RANDOM_TREASURE_ARTIFACT   = 337,
    OVERLAY_RANDOM_MINOR_ARTIFACT      = 338,
    OVERLAY_RANDOM_MAJOR_ARTIFACT      = 339,
    OVERLAY_ANCIENT_LAMP               = 832,
    OVERLAY_RANDOM_RESOURCE            = 833,
    OVERLAY_TREASURE_CHEST             = 834
H2_ENUM_END(RandomMapOverlay)

// Where PlaceTreasures guards a treasure: the diagonal cell of a corner
// whose two sides are blocked.
H2_ENUM_BEGIN(TreasureGuard)
    TREASURE_UNGUARDED = 0,
    TREASURE_GUARD_NE  = 1,
    TREASURE_GUARD_SE  = 2,
    TREASURE_GUARD_SW  = 3,
    TREASURE_GUARD_NW  = 4
H2_ENUM_END(TreasureGuard)

// giGroundShape's plain ground and the decorated plain variants
// ScatterDecorations may put an object on.
H2_ENUM_BEGIN(RandomMapGroundShape)
    GROUND_SHAPE_PLAIN           = 0,
    GROUND_SHAPE_DECORATED_FIRST = 0x12,
    GROUND_SHAPE_DECORATED_A     = 0x13,
    GROUND_SHAPE_DECORATED_B     = 0x14,
    GROUND_SHAPE_DECORATED_C     = 0x15
H2_ENUM_END(RandomMapGroundShape)

// The eight directions a mountain or tree chain runs (gChainSteps): steep
// directions move two rows per column.
H2_ENUM_BEGIN(ChainDirection)
    CHAIN_UP_RIGHT_STEEP   = 0,
    CHAIN_UP_RIGHT         = 1,
    CHAIN_DOWN_RIGHT       = 2,
    CHAIN_DOWN_RIGHT_STEEP = 3,
    CHAIN_DOWN_LEFT_STEEP  = 4,
    CHAIN_DOWN_LEFT        = 5,
    CHAIN_UP_LEFT          = 6,
    CHAIN_UP_LEFT_STEEP    = 7,
    // Directions below this one run rightwards.
    CHAIN_RIGHTWARD_END    = CHAIN_DOWN_LEFT_STEEP,
    CHAIN_DIRECTION_COUNT  = 8
H2_ENUM_END(ChainDirection)

// A chain's sideways shift when it turns (gChainTurns' second index), and
// what the turn adds to the direction modulo CHAIN_DIRECTION_COUNT.
H2_ENUM_BEGIN(ChainTurn)
    CHAIN_TURN_CLOCKWISE        = 0,
    CHAIN_TURN_COUNTERCLOCKWISE = 1,
    CHAIN_TURN_COUNT            = 2,
    CHAIN_CLOCKWISE_STEP        = 10,
    CHAIN_COUNTERCLOCKWISE_STEP = 6
H2_ENUM_END(ChainTurn)

// The four objects of a mountain or tree chain, in catalogue order from the
// one whose bottom rows have the chain link's shape.
H2_ENUM_BEGIN(ChainPiece)
    CHAIN_PIECE_STEEP_FALLING = 0,
    CHAIN_PIECE_STEEP_RISING  = 1,
    CHAIN_PIECE_FALLING       = 2,
    CHAIN_PIECE_RISING        = 3,
    // overlayType::occupiedRows[OVERLAY_OCCUPIED_BOTTOM] of a chain's first
    // piece.
    CHAIN_LINK_SHAPE          = 0xe0f07
H2_ENUM_END(ChainPiece)


// gDensityPercent's rows.
H2_ENUM_BEGIN(RandomMapDensity)
    RANDOM_MAP_DENSITY_MOUNTAINS = 0,
    RANDOM_MAP_DENSITY_TREES     = 1,
    RANDOM_MAP_DENSITY_OBJECTS   = 2,
    RANDOM_MAP_DENSITY_TREASURE  = 3,
    RANDOM_MAP_DENSITY_MONSTERS  = 4
H2_ENUM_END(RandomMapDensity)

// Places the object type with its anchor on cell (x, y); the second form
// asks whether it fits there.
i32 PlaceOverlayAt(overlayType* type, i32 x, i32 y);
i32 CanPlaceOverlayAt(overlayType* type, i32 x, i32 y, i32 strict);
// Scales a count of objects by a density percent: unchanged at 50, halved
// near 0, doubled at 100.
void ScaleByDensity(i32* count, i32 density);

// The generator's running state.
extern b32 gGeneratingRandomMap;

#endif
