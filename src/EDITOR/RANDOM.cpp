// The random map generator: editManager methods that paint terrain, lay
// mountain and tree chains and place towns, objects and treasure. The unit
// name comes from the generator's assertion path (Editor\RANDOM.CPP).
// Descriptive names: every function and datum of this unit.

#include <va.h>
#include <EDITOR/RANDOM.h>
#include <EDITOR/EDITOR.h>
#include <EDITOR/editManager.h>
#include <EDITOR/OVERLAY.h>
#include <EDITOR/EDITMGR.h>
#include <EDITOR/eventsManager.h>
#include <EDITOR/specedit.h>
#include <BASE/Misc.h>
#include <SOURCE/KB.h>
#include <SOURCE/X_GLOBAL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RETAIL_FILE "e:\\Users\\igorl\\VSS\\HMM\\HMM2\\Source\\Editor\\RANDOM.CPP"

// A cell offset, kept as a plain (x, y) pair in the chain tables.
H2_ENUM_BEGIN(MapStepAxis)
    MAP_STEP_X    = 0,
    MAP_STEP_Y    = 1,
    MAP_STEP_AXES = 2
H2_ENUM_END(MapStepAxis)
typedef i32 MapStepPair[MAP_STEP_AXES];

H2_ENUM_BEGIN(RandomMapConstant)
    // GenerateRandomMap retries a map without enough castles this often.
    RANDOM_MAP_ATTEMPTS          = 5,
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
    // bounds its neighbour scan by a medium map's last cell, whatever the size.
    RANDOM_MAP_SMALL_REGION_SIZE = 15,
    RANDOM_MAP_MEDIUM_LAST_CELL = 71,
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
    // PlaceTowns: the land regions it numbers (a castle per player); a
    // scan ends by setting its counters past the map.
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
    RANDOM_MAP_LAND_PER_MONSTER  = 130,
    // GenerateRandomMap varies the finished map's ground at this level (the
    // level Ctrl+5 gives).
    RANDOM_MAP_GROUND_VARIETY    = 5,
    // ScatterDecorations rolls each terrain's chance per mille.
    RANDOM_MAP_DECORATION_ROLL   = 1000
H2_ENUM_END(RandomMapConstant)

// Where PlaceTreasures guards a treasure: the diagonal cell of a corner
// whose two sides are blocked.
H2_ENUM_BEGIN(TreasureGuard)
    TREASURE_UNGUARDED = 0,
    TREASURE_GUARD_NE  = 1,
    TREASURE_GUARD_SE  = 2,
    TREASURE_GUARD_SW  = 3,
    TREASURE_GUARD_NW  = 4
H2_ENUM_END(TreasureGuard)

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
    // overlayType::occupiedRows[OVERLAY_GRID_BOTTOM] of a chain's first
    // piece.
    CHAIN_LINK_SHAPE          = 0xe0f07
H2_ENUM_END(ChainPiece)

// A cell of a new'ed MAP_WIDTH x MAP_HEIGHT byte grid: RemoveSmallRegions'
// visited and region marks, PlaceTowns' region numbers and each castle's
// reach.
#define MAP_GRID_CELL(grid, x, y) (*((grid) + (x) + (y) * MAP_WIDTH))

// The generator's own data: the chain directions' steps and turns, then the
// five mines' resources.
DATA(0x00498ecc)
static MapStepPair gChainSteps[CHAIN_DIRECTION_COUNT] =
    {{1, -2}, {1, -1}, {1, 1}, {1, 2}, {-1, 2}, {-1, 1}, {-1, -1}, {-1, -2}};

DATA(0x00498f0c)
static MapStepPair gChainTurns[CHAIN_DIRECTION_COUNT][CHAIN_TURN_COUNT] = {
    {{1, 3}, {-1, 0}},
    {{1, 0}, {0, 0}},
    {{0, 0}, {1, 0}},
    {{-1, 0}, {-3, 1}},
    {{-1, -3}, {-1, 0}},
    {{0, 0}, {0, 0}},
    {{0, 0}, {0, 0}},
    {{1, 0}, {3, -1}}
};

DATA(0x00498f8c)
static i32 H2_UNUSED(gMineResources)[RANDOM_MAP_MINE_RESOURCE_COUNT] = {
    IDX(RES_ORE), IDX(RES_SULFUR), IDX(RES_CRYSTAL), IDX(RES_GEMS), IDX(RES_GOLD)
};

DATA(0x004a5750) b32 gGeneratingRandomMap;

VA(0x0041cd70, 0x29)
b32 PlaceOverlayAt(overlayType* type, i32 x, i32 y) {
    return PlaceOverlay(type, x - OVERLAY_ANCHOR_X, y - OVERLAY_ANCHOR_Y, true);
}

VA(0x0041cd99, 0x2b)
b32 CanPlaceOverlayAt(overlayType* type, i32 x, i32 y, b32 overObjects) {
    return CanPlaceOverlay(type, x - OVERLAY_ANCHOR_X, y - OVERLAY_ANCHOR_Y, overObjects);
}

#define terrain terrain_g     // frame-slot spelling
#define paintFrom paintFrom_o // frame-slot spelling
VA(0x0041cdc4, 0x3fa)
void editManager::GenerateRandomMap(void) {
    i32 terrain;
    i32 attempt;
    i32 paintFrom;
    b32 done;
    i32 H2_UNUSED(unusedTries);
    double H2_UNUSED(unusedPercent);
    double H2_UNUSED(unusedRatio);

    gGeneratingRandomMap = true;
    if (!NewMapDialog()) {
        DrawMap();
        UpdateMapView();
        DrawRadar(true);
        return;
    }
    if (gGenerateUnseen)
        gGeneratingMap = true;
    InitializeMap(true, MAP_WIDTH, MAP_HEIGHT);
    for (terrain = 0; terrain < GAME_PLAYER_COUNT; terrain++) {
        gEditMapHeader.playerCanHuman[terrain] = true;
        gEditMapHeader.playerCanComputer[terrain] = true;
    }
    done = false;
    attempt = 0;
    while (!done && attempt < RANDOM_MAP_ATTEMPTS) {
        attempt++;
        ResetArea(0, 0, MAP_WIDTH, MAP_HEIGHT);
        DrawMap();
        UpdateMapView();
        DrawRadar(true);
        unusedPercent = NEW_MAP_ALL_PERCENT;
        paintFrom = 0;
        for (terrain = 0; terrain <= IDX(TERRAIN_WASTELAND); terrain++) {
            if (gTerrainPercent[terrain] > 0.0) {
                PaintRandomTerrain(terrain, NEW_MAP_PERCENT, IDX(TERRAIN_WATER));
                paintFrom = terrain + 1;
                terrain = RANDOM_MAP_END_TERRAIN_SCAN;
            }
        }
        for (terrain = paintFrom; terrain <= IDX(TERRAIN_WASTELAND); terrain++) {
            if (gTerrainPercent[terrain] > 0.0) {
                sprintf(
                    gText,
                    localization::Tr("editor.random.status.terrain"),
                    gEditTerrainNames[terrain]
                );
                ShowStatusText(gText);
                PaintRandomTerrain(terrain, gTerrainPercent[terrain], paintFrom - 1);
            }
        }
        ShowStatusText(localization::Tr("editor.random.status.smoothing"));
        RemoveSmallRegions();
        for (terrain = 0; terrain <= IDX(TERRAIN_BEACH); terrain++)
            BlendTerrain(terrain, true, false, true, true);
        BlendTerrain(IDX(TERRAIN_WATER), true, false, false, true);
        ShowStatusText(localization::Tr("editor.random.status.mountains"));
        PlaceObstacleChains(gDensityPercent[RANDOM_MAP_DENSITY_MOUNTAINS], true);
        ShowStatusText(localization::Tr("editor.random.status.trees"));
        PlaceObstacleChains(gDensityPercent[RANDOM_MAP_DENSITY_TREES], false);
        ShowStatusText(localization::Tr("editor.random.status.objects"));
        PlaceRandomObjects(
            gDensityPercent[RANDOM_MAP_DENSITY_OBJECTS],
            gDensityPercent[RANDOM_MAP_DENSITY_MONSTERS]
        );
        ShowStatusText(localization::Tr("editor.random.status.land"));
        PlaceTowns();
        for (terrain = 0; terrain <= IDX(TERRAIN_BEACH); terrain++)
            BlendTerrain(terrain, true, false, true, false);
        gVaryTiles = true;
        BlendTerrain(IDX(TERRAIN_WATER), true, false, false, true);
        RandomizeGround(RANDOM_MAP_GROUND_VARIETY);
        gVaryTiles = false;
        ShowStatusText(localization::Tr("editor.random.status.treasure"));
        PlaceTreasures(
            gDensityPercent[RANDOM_MAP_DENSITY_TREASURE],
            gDensityPercent[RANDOM_MAP_DENSITY_MONSTERS]
        );
        done = HasEnoughCastles();
        if (!done)
            continue;
    }
    if (gGeneratingMap) {
        if (EditMapSpecifications(true) && !SaveMap(gMapFileName)) {
            sprintf(gText, localization::Tr("editor.random.saved"), gEditMapHeader.name);
            NormalDialog(gText, NORMAL_DIALOG_INFO);
        }
        ResetArea(0, 0, MAP_WIDTH, MAP_HEIGHT);
        gGeneratingMap = false;
    }
    DrawMap();
    ShowStatusText("");
    ClearStatusText();
    UpdateMapView();
    DrawRadar(true);
    gGeneratingRandomMap = false;
}
#undef terrain
#undef paintFrom

VA(0x0041d1be, 0x98)
b32 editManager::HasEnoughCastles(void) {
    i32 count;
    i32 x;
    i32 y;
    mapCell* cell;

    count = 0;
    for (y = 0; y < MAP_HEIGHT; y++) {
        for (x = 0; x < MAP_WIDTH; x++) {
            cell = gMap.CellAt(x, y);
            if (cell->m_triggerType == MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_CASTLE))
                count++;
        }
    }
    return count >= gRandomMapPlayers;
}

#define seedY seedY_m                       // frame-slot spelling
#define targetCells targetCells_g           // frame-slot spelling
#define upWeight upWeight_n                 // frame-slot spelling
#define maxWeight maxWeight_b               // frame-slot spelling
#define drift drift_b                       // frame-slot spelling
#define horizontalWeight horizontalWeight_d // frame-slot spelling
#define seedX seedX_e                       // frame-slot spelling
#define minWeight minWeight_i               // frame-slot spelling
#define cluster cluster_d                   // frame-slot spelling
VA(0x0041d256, 0x6bd)
void editManager::PaintRandomTerrain(i32 terrain, i32 percent, i32 baseTerrain) {
    i32 perSeed;
    i32 seedY;
    i32 walkX;
    i32 targetCells;
    i32 walkY;
    i32 guard;
    i32 horizontalWeight;
    i32 reserve;
    b32 looking;
    i32 drift;
    i32 maxWeight;
    i32 upWeight;
    i32 balance;
    i32 H2_UNUSED(escapes);
    i32 patches;
    i32 minWeight;
    i32 seedX;
    i32 placed;
    i32 cluster;
    i32 leftWeight;

    seedX = 0;
    seedY = 0;
    if (percent == NEW_MAP_PERCENT) {
        for (walkX = 0; walkX < MAP_WIDTH; walkX++)
            for (walkY = 0; walkY < MAP_HEIGHT; walkY++)
                gMap.CellAt(walkX, walkY)->m_terrainImageIndex =
                    ChooseGroundTile(terrain, EDIT_SHAPE_PLAIN, false, walkX, walkY, false, 1.0f);
    } else {
        targetCells = MAP_WIDTH * MAP_HEIGHT * percent / NEW_MAP_PERCENT;
        patches = Random(0, percent + 51) / 30 + 1;
        balance = targetCells;
        escapes = 0;
        minWeight = gScatterTerrain ? 2 : 3;
        maxWeight = (gScatterTerrain != 0) + 6;
        for (cluster = 0; cluster < patches; cluster++) {
            perSeed = balance / (patches - cluster);
            looking = true;
            guard = 0;
            while (guard < RANDOM_MAP_SEED_TRIES && looking) {
                guard++;
                if (gScatterTerrain)
                    seedX = Random(0, MAP_WIDTH - 1);
                else
                    seedX = (Random(0, MAP_WIDTH - 1) + Random(0, MAP_WIDTH - 1)
                             + Random(0, MAP_WIDTH - 1) + Random(0, MAP_WIDTH - 1))
                            / 4;
                if (gScatterTerrain) {
                    if (terrain == IDX(TERRAIN_DESERT) || terrain == IDX(TERRAIN_WASTELAND)
                        || terrain == IDX(TERRAIN_LAVA))
                        seedY = (Random(0, MAP_HEIGHT - 1) + Random(0, MAP_HEIGHT - 1)
                                 + Random(0, MAP_HEIGHT - 1))
                                / 3;
                    else
                        seedY = Random(0, MAP_HEIGHT - 1);
                } else {
                    seedY = (Random(0, MAP_HEIGHT - 1) + Random(0, MAP_HEIGHT - 1)
                             + Random(0, MAP_HEIGHT - 1) + Random(0, MAP_HEIGHT - 1))
                            / 4;
                }
                if (IDX(CELL_TERRAIN(gMap.CellAt(seedX, seedY))) == baseTerrain)
                    looking = false;
            }
            horizontalWeight = Random(3, 7);
            leftWeight = Random(3, 7);
            upWeight = Random(3, 7);
            reserve = perSeed * 1.5;
            drift = 0;
            for (placed = 0; placed < perSeed; placed++) {
                drift++;
                if ((drift & RANDOM_MAP_SEED_DRIFT_MASK) == RANDOM_MAP_SEED_DRIFT_MASK) {
                    seedX = seedX + Random(0, 2) - 1;
                    seedY = seedY + Random(0, 2) - 1;
                    if (seedX < 0)
                        seedX = 0;
                    if (seedX >= MAP_WIDTH)
                        seedX = MAP_WIDTH - 1;
                    if (seedY < 0)
                        seedY = 0;
                    if (seedY >= MAP_HEIGHT)
                        seedY = MAP_HEIGHT - 1;
                    if ((drift & RANDOM_MAP_WEIGHT_DRIFT_MASK) == RANDOM_MAP_WEIGHT_DRIFT_MASK) {
                        horizontalWeight = horizontalWeight + Random(0, 2) - 1;
                        leftWeight = leftWeight + Random(0, 2) - 1;
                        upWeight = upWeight + Random(0, 2) - 1;
                        if (horizontalWeight < minWeight)
                            horizontalWeight = minWeight;
                        if (horizontalWeight > maxWeight)
                            horizontalWeight = maxWeight;
                        if (leftWeight < minWeight)
                            leftWeight = minWeight;
                        if (leftWeight > maxWeight)
                            leftWeight = maxWeight;
                        if (upWeight < minWeight)
                            upWeight = minWeight;
                        if (upWeight > maxWeight)
                            upWeight = maxWeight;
                    }
                }
                walkX = seedX;
                walkY = seedY;
                guard = 0;
                while (IDX(CELL_TERRAIN(gMap.CellAt(walkX, walkY))) == terrain
                       && guard++ < RANDOM_MAP_WALK_LIMIT) {
                    if (Random(0, 9) < horizontalWeight) {
                        if (walkX == 0) {
                            guard += RANDOM_MAP_EDGE_STEP_COST;
                            walkX++;
                            if (leftWeight > maxWeight)
                                leftWeight = maxWeight;
                        } else if (walkX == MAP_WIDTH - 1) {
                            guard += RANDOM_MAP_EDGE_STEP_COST;
                            walkX--;
                            if (leftWeight < minWeight)
                                leftWeight = minWeight;
                        } else if (Random(0, 9) < leftWeight)
                            walkX--;
                        else
                            walkX++;
                    } else {
                        if (walkY == 0) {
                            guard += RANDOM_MAP_EDGE_STEP_COST;
                            walkY++;
                            if (upWeight > maxWeight)
                                upWeight = maxWeight;
                        } else if (walkY == MAP_HEIGHT - 1) {
                            guard += RANDOM_MAP_EDGE_STEP_COST;
                            walkY--;
                            if (upWeight < minWeight)
                                upWeight = minWeight;
                        } else if (Random(0, 9) < upWeight)
                            walkY--;
                        else
                            walkY++;
                    }
                }
                if (guard >= RANDOM_MAP_ESCAPED_WALK)
                    escapes++;
                if (IDX(CELL_TERRAIN(gMap.CellAt(walkX, walkY))) == baseTerrain)
                    gMap.CellAt(walkX, walkY)->m_terrainImageIndex = ChooseGroundTile(
                        terrain,
                        EDIT_SHAPE_PLAIN,
                        false,
                        walkX,
                        walkY,
                        false,
                        1.0f
                    );
                else if (reserve) {
                    reserve--;
                    placed--;
                } else {
                    balance++;
                    if (cluster + 1 == patches && patches < RANDOM_MAP_SEED_LIMIT)
                        patches++;
                }
            }
            balance -= perSeed;
        }
    }
}
#undef seedY
#undef targetCells
#undef upWeight
#undef maxWeight
#undef drift
#undef horizontalWeight
#undef seedX
#undef minWeight
#undef cluster

#define minX minX_a     // frame-slot spelling
#define countX countX_e // frame-slot spelling
#define countY countY_c // frame-slot spelling
#define y y_n           // frame-slot spelling
#define maxY maxY_f     // frame-slot spelling
#define minY minY_n     // frame-slot spelling
#define x x_a           // frame-slot spelling
VA(0x0041d913, 0x5cb)
void editManager::RemoveSmallRegions(void) {
    i32 minX;
    b32 spread;
    i32 countX;
    i32 neighbourTerrain;
    i32 countY;
    b8* done;
    i32 maxY;
    i32 y;
    i32 ground;
    i32 extent;
    i32 maxX;
    i32 startX;
    i32 x;
    i32 minY;
    i8* inRegion;
    i32 startY;

    done = new b8[MAP_WIDTH * MAP_HEIGHT];
    inRegion = new i8[MAP_WIDTH * MAP_HEIGHT];
    memset(done, 0, MAP_WIDTH * MAP_HEIGHT);
    for (startY = 0; startY < MAP_HEIGHT; startY++) {
        for (startX = 0; startX < MAP_WIDTH; startX++) {
            if (MAP_GRID_CELL(done, startX, startY))
                continue;
            memset(inRegion, 0, MAP_WIDTH * MAP_HEIGHT);
            MAP_GRID_CELL(inRegion, startX, startY)++;
            ground = IDX(CELL_TERRAIN(gMap.CellAt(startX, startY)));
            spread = true;
            extent = 1;
            minX = startX - 1;
            maxX = startX + 1;
            minY = startY - 1;
            maxY = startY + 1;
            neighbourTerrain = IDX(TERRAIN_INVALID);
            while (spread) {
                spread = false;
                if (minX < 0)
                    minX = 0;
                if (maxX >= MAP_WIDTH)
                    maxX = MAP_WIDTH - 1;
                if (minY < 0)
                    minY = 0;
                if (maxY >= MAP_HEIGHT)
                    maxY = MAP_HEIGHT - 1;
                for (y = minY; y <= maxY; y++) {
                    for (x = minX; x <= maxX; x++) {
                        if (IDX(CELL_TERRAIN(gMap.CellAt(x, y))) != ground) {
                            if (neighbourTerrain == IDX(TERRAIN_INVALID))
                                neighbourTerrain = IDX(CELL_TERRAIN(gMap.CellAt(x, y)));
                            continue;
                        }
                        if (MAP_GRID_CELL(inRegion, x, y))
                            continue;
                        if (x < RANDOM_MAP_MEDIUM_LAST_CELL
                            && *(inRegion + x + 1 + y * MAP_WIDTH))
                            MAP_GRID_CELL(inRegion, x, y)++;
                        else if (x > 0 && *(inRegion + x - 1 + y * MAP_WIDTH))
                            MAP_GRID_CELL(inRegion, x, y)++;
                        else if (y < RANDOM_MAP_MEDIUM_LAST_CELL
                                 && MAP_GRID_CELL(inRegion, x, y + 1))
                            MAP_GRID_CELL(inRegion, x, y)++;
                        else if (y > 0 && MAP_GRID_CELL(inRegion, x, y - 1))
                            MAP_GRID_CELL(inRegion, x, y)++;
                        else
                            continue;
                        spread = true;
                        extent++;
                        if (x == minX)
                            minX--;
                        if (x == maxX)
                            maxX++;
                        if (y == minY)
                            minX--;
                        if (y == maxY)
                            maxY++;
                        if (minX < 0)
                            minX = 0;
                        if (maxX >= MAP_WIDTH)
                            maxX = MAP_WIDTH - 1;
                        if (minY < 0)
                            minY = 0;
                        if (maxY >= MAP_HEIGHT)
                            maxY = MAP_HEIGHT - 1;
                    }
                }
            }
            if (extent > RANDOM_MAP_SMALL_REGION_SIZE)
                neighbourTerrain = ground;
            for (y = 0; y < MAP_HEIGHT; y++) {
                for (x = 0; x < MAP_WIDTH; x++) {
                    if (MAP_GRID_CELL(inRegion, x, y)) {
                        MAP_GRID_CELL(done, x, y) = true;
                        gMap.CellAt(x, y)->m_terrainImageIndex = ChooseGroundTile(
                            neighbourTerrain,
                            EDIT_SHAPE_PLAIN,
                            false,
                            y,
                            y,
                            false,
                            1.0f
                        );
                    }
                }
            }
        }
    }
    delete done;
    delete inRegion;
    gLandCellCount = 0;
    for (countX = 0; countX < MAP_WIDTH; countX++)
        for (countY = 0; countY < MAP_HEIGHT; countY++)
            if (CELL_TERRAIN(gMap.CellAt(countX, countY)) != TERRAIN_WATER)
                gLandCellCount++;
}
#undef minX
#undef countX
#undef countY
#undef y
#undef maxY
#undef minY
#undef x

VA(0x0041dede, 0x51)
void ScaleByDensity(i32* count, i32 density) {
    i32 H2_UNUSED(base);

    base = *count;
    if (density < RANDOM_MAP_NEUTRAL_DENSITY)
        *count = *count * (density + RANDOM_MAP_NEUTRAL_DENSITY) / NEW_MAP_PERCENT;
    else
        *count = *count * density / RANDOM_MAP_NEUTRAL_DENSITY;
}

#define count count_n // frame-slot spelling
VA(0x0041df2f, 0xf0)
i32 editManager::CountNearbyObstacles(i32 x, i32 y) {
    i32 count;
    i32 nearX;
    i32 nearY;
    mapCell* cell;

    count = 0;
    for (nearX = x - RANDOM_MAP_CHAIN_SPACING; nearX < x + RANDOM_MAP_CHAIN_SPACING; nearX++) {
        for (nearY = y - RANDOM_MAP_CHAIN_SPACING; nearY < y + RANDOM_MAP_CHAIN_SPACING;
             nearY++) {
            if (nearX < 0 || nearX >= MAP_WIDTH || nearY < 0 || nearY >= MAP_HEIGHT)
                continue;
            cell = gMap.CellAt(nearX, nearY);
            if (cell->m_triggerType == MAP_OBJECT_MOUNTAINS
                || cell->m_triggerType == MAP_OBJECT_VOLCANO
                || cell->m_triggerType == MAP_OBJECT_TREES) {
                if (nearX == 0 && nearY == 0)
                    return RANDOM_MAP_CROWDED;
                count++;
            }
        }
    }
    return count;
}
#undef count

#define tileset tileset_d           // frame-slot spelling
#define x x_n                       // frame-slot spelling
#define y y_o                       // frame-slot spelling
#define crowdedTries crowdedTries_b // frame-slot spelling
#define chainCount chainCount_h     // frame-slot spelling
#define steep steep_j               // frame-slot spelling
VA(0x0041e01f, 0x523)
void editManager::PlaceObstacleChains(i32 density, b32 mountains) {
    i32 chance;
    b32 going;
    i32 direction;
    i32 H2_UNUSED(unusedStep);
    i32 x;
    char tileset;
    i32 H2_UNUSED(unusedMask);
    i32 ground;
    i32 crowdedTries;
    i32 y;
    i32 budget;
    b32 steep;
    i32 chainCount;
    i32 placed;
    i32 landCells;
    b32 hunting;

    placed = 0;
    tileset = IDX(TILESET_NONE);
    ground = IDX(TERRAIN_WATER);
    landCells = 0;
    for (x = 0; x < MAP_WIDTH; x++)
        for (y = 0; y < MAP_HEIGHT; y++)
            if (CELL_TERRAIN(gMap.CellAt(x, y)) != TERRAIN_WATER)
                landCells++;
    chainCount = landCells / RANDOM_MAP_LAND_PER_CHAIN;
    ScaleByDensity(&chainCount, density);
    budget = chainCount * RANDOM_MAP_CHAIN_BUDGET;
    while (placed < budget) {
        hunting = true;
        crowdedTries = 0;
        while (hunting) {
            hunting = false;
            x = Random(0, MAP_WIDTH - 1);
            y = Random(0, MAP_HEIGHT - 1);
            if (crowdedTries < RANDOM_MAP_CROWDED_TRIES
                && Random(0, 100) < CountNearbyObstacles(x, y) * 2) {
                crowdedTries++;
                hunting = true;
            }
            ground = IDX(CELL_TERRAIN(gMap.CellAt(x, y)));
            if (ground == IDX(TERRAIN_WATER))
                hunting = true;
            if (!mountains && (ground == IDX(TERRAIN_LAVA) || ground == IDX(TERRAIN_WASTELAND))
                && Random(0, 100) < 80)
                hunting = true;
        }
        direction = Random(2, 3) * 2;
        if (Random(0, 100) < 50) {
            if (x > MAP_WIDTH * 0.75)
                direction = CHAIN_DOWN_RIGHT;
            else if (x < MAP_WIDTH * 0.25)
                direction = CHAIN_DOWN_LEFT_STEEP;
        }
        if (Random(1, 100) <= 25)
            direction++;
        going = true;
        tileset = IDX(TILESET_NONE);
        if (mountains) {
            switch (ground) {
                case IDX(TERRAIN_GRASS):
                    tileset = IDX(TILESET_MTNGRAS);
                    break;
                case IDX(TERRAIN_SNOW):
                    tileset = IDX(TILESET_MTNSNOW);
                    break;
                case IDX(TERRAIN_SWAMP):
                    tileset = IDX(TILESET_MTNSWMP);
                    break;
                case IDX(TERRAIN_LAVA):
                    tileset = IDX(TILESET_MTNLAVA);
                    break;
                case IDX(TERRAIN_DESERT):
                    tileset = IDX(TILESET_MTNDSRT);
                    break;
                case IDX(TERRAIN_DIRT):
                    tileset = IDX(TILESET_MTNDIRT);
                    break;
                case IDX(TERRAIN_WASTELAND):
                    tileset = IDX(TILESET_MTNCRCK);
                    break;
                case IDX(TERRAIN_BEACH):
                    tileset = IDX(TILESET_MTNDSRT);
                    break;
                case IDX(TERRAIN_WATER):
                    H2_ASSERT(0, RETAIL_FILE, 638);
                    break;
            }
        } else {
            switch (ground) {
                case IDX(TERRAIN_GRASS):
                    tileset = IDX(TILESET_TREDECI);
                    break;
                case IDX(TERRAIN_SNOW):
                    tileset = IDX(TILESET_TRESNOW);
                    break;
                case IDX(TERRAIN_SWAMP):
                    tileset = IDX(TILESET_TREJNGL);
                    break;
                case IDX(TERRAIN_LAVA):
                    tileset = IDX(TILESET_TREEVIL);
                    break;
                case IDX(TERRAIN_DESERT):
                    tileset = IDX(TILESET_TREEVIL);
                    break;
                case IDX(TERRAIN_DIRT):
                    tileset = IDX(TILESET_TREFALL);
                    break;
                case IDX(TERRAIN_WASTELAND):
                    tileset = IDX(TILESET_TREEVIL);
                    break;
                case IDX(TERRAIN_BEACH):
                    tileset = IDX(TILESET_TREDECI);
                    break;
                case IDX(TERRAIN_WATER):
                    H2_ASSERT(0, RETAIL_FILE, 655);
                    break;
            }
            if (tileset == IDX(TILESET_TREDECI) && Random(0, 100) < 30)
                tileset = IDX(TILESET_TREFIR);
        }
        while (going) {
            if (PlaceChainLink(&x, &y, direction, mountains, tileset)) {
                placed += RANDOM_MAP_CHAIN_LINK_COST;
                if (direction == CHAIN_UP_RIGHT_STEEP || direction == CHAIN_DOWN_RIGHT_STEEP
                    || direction == CHAIN_DOWN_LEFT_STEEP || direction == CHAIN_UP_LEFT_STEEP)
                    steep = true;
                else
                    steep = false;
                if (!mountains) {
                    if (Random(1, 100) < (steep ? 10 : 30))
                        going = false;
                } else {
                    if (Random(1, 100) < (steep ? 20 : 40))
                        going = false;
                }
                if (!mountains) {
                    if (Random(1, 100) < 35) {
                        if (direction < CHAIN_RIGHTWARD_END)
                            x += 2;
                        else
                            x -= 2;
                        chance = 0;
                    } else
                        chance = 50;
                } else
                    chance = 40;
                chance /= 3;
                if (Random(1, 100) < chance) {
                    if (Random(0, 1)) {
                        x += gChainTurns[direction][CHAIN_TURN_CLOCKWISE][MAP_STEP_X];
                        y += gChainTurns[direction][CHAIN_TURN_CLOCKWISE][MAP_STEP_Y];
                        direction = (direction + CHAIN_CLOCKWISE_STEP) % CHAIN_DIRECTION_COUNT;
                    } else {
                        x += gChainTurns[direction][CHAIN_TURN_COUNTERCLOCKWISE][MAP_STEP_X];
                        y += gChainTurns[direction][CHAIN_TURN_COUNTERCLOCKWISE][MAP_STEP_Y];
                        direction
                            = (direction + CHAIN_COUNTERCLOCKWISE_STEP) % CHAIN_DIRECTION_COUNT;
                    }
                }
            } else {
                going = false;
                placed++;
            }
        }
    }
}
#undef tileset
#undef x
#undef y
#undef crowdedTries
#undef chainCount
#undef steep

#define piece piece_p         // frame-slot spelling
#define unusedBit unusedBit_o // frame-slot spelling
VA(0x0041e542, 0x1c7)
b32 editManager::PlaceChainLink(i32* x, i32* y, i32 direction, b32 H2_UNUSED(mountains), char tileset) {
    i32 piece;
    overlayType* link;
    i32 H2_UNUSED(unusedBit);
    i32 H2_UNUSED(ground);
    i32 n;

    if (*x < 0 || *x > MAP_WIDTH - 1 || *y < 0 || *y > MAP_HEIGHT - 1)
        return false;
    piece = 0;
    ground = IDX(CELL_TERRAIN(gMap.CellAt(*x, *y)));
    H2_ASSERT(tileset != IDX(TILESET_NONE), RETAIL_FILE, 750);
    if (direction == CHAIN_UP_RIGHT_STEEP)
        piece = CHAIN_PIECE_STEEP_RISING;
    if (direction == CHAIN_UP_RIGHT)
        piece = CHAIN_PIECE_RISING;
    if (direction == CHAIN_DOWN_RIGHT)
        piece = CHAIN_PIECE_FALLING;
    if (direction == CHAIN_DOWN_RIGHT_STEEP)
        piece = CHAIN_PIECE_STEEP_FALLING;
    if (direction == CHAIN_DOWN_LEFT_STEEP)
        piece = CHAIN_PIECE_STEEP_RISING;
    if (direction == CHAIN_DOWN_LEFT)
        piece = CHAIN_PIECE_RISING;
    if (direction == CHAIN_UP_LEFT)
        piece = CHAIN_PIECE_FALLING;
    if (direction == CHAIN_UP_LEFT_STEEP)
        piece = CHAIN_PIECE_STEEP_FALLING;
    link = NULL;
    for (n = 0; n < OVERLAY_TYPE_COUNT; n++) {
        if (!link && gOverlayTypes[n].tileset == tileset
            && gOverlayTypes[n].occupiedRows[OVERLAY_GRID_BOTTOM] == CHAIN_LINK_SHAPE)
            link = &gOverlayTypes[n + piece];
    }
    if (CanPlaceOverlayAt(link, *x, *y, true)) {
        PlaceOverlayAt(link, *x, *y);
        *x += gChainSteps[direction][MAP_STEP_X];
        *y += gChainSteps[direction][MAP_STEP_Y];
        return true;
    }
    return false;
}
#undef piece
#undef unusedBit

#define roadMask roadMask_g         // frame-slot spelling
#define destY destY_a               // frame-slot spelling
#define castleRegion castleRegion_o // frame-slot spelling
#define attempt attempt_n           // frame-slot spelling
#define castles castles_i           // frame-slot spelling
#define fromY fromY_m               // frame-slot spelling
#define nearY nearY_h               // frame-slot spelling
#define foundX foundX_j             // frame-slot spelling
#define castlePlaced castlePlaced_a // frame-slot spelling
VA(0x0041e709, 0x21c1)
void editManager::PlaceTowns(void) {
    i32 terrain;
    b32 cutOff[GAME_PLAYER_COUNT];
    b32 extraRoads[GAME_PLAYER_COUNT];
    i32 nearX;
    i32 castleRegion[GAME_PLAYER_COUNT];
    i32 tileX;
    i32 reachable[GAME_PLAYER_COUNT];
    i32 regionId;
    double shareValue[RANDOM_MAP_REGION_LIMIT];
    i32 destY;
    b32 coastAt;
    i32 destX;
    i32 continents;
    mapStep keeps[GAME_PLAYER_COUNT];
    b32 tracing;
    i32 fromX;
    i32 H2_UNUSED(roadMask);
    i32 regionsUsed;
    i32 c;
    i32 round;
    i32 H2_UNUSED(unusedValue);
    i16 rank[RANDOM_MAP_REGION_LIMIT + 1];
    i32 dist;
    i32 peerIndex;
    i32 steps;
    overlayType* castles[GAME_PLAYER_COUNT];
    i32 attempt;
    i32 t;
    i32 rating;
    i32 slot;
    i32 tileY;
    i32 H2_UNUSED(unusedIndex);
    b32 filled;
    i32 stepX;
    i32 fromY;
    i32 stepY;
    i32 foundX;
    i32 nearY;
    u8* regionGrid;
    i16 regionSizes[RANDOM_MAP_REGION_LIMIT];
    b32 meet;
    i32 foundY;
    i32 top;
    i32 H2_UNUSED(unusedTotal);
    b32 castlePlaced;
    u8* reachedGrids[GAME_PLAYER_COUNT];

    tileX = tileY = 0;
    for (slot = 0; slot < GAME_PLAYER_COUNT; slot++) {
        cutOff[slot] = false;
        extraRoads[slot] = false;
    }
    castles[PLAYER_COLOR_BLUE] = &gOverlayTypes[OVERLAY_RANDOM_CASTLE_0];
    castles[PLAYER_COLOR_GREEN] = &gOverlayTypes[OVERLAY_RANDOM_CASTLE_1];
    castles[PLAYER_COLOR_RED] = &gOverlayTypes[OVERLAY_RANDOM_CASTLE_2];
    castles[PLAYER_COLOR_YELLOW] = &gOverlayTypes[OVERLAY_RANDOM_CASTLE_3];
    castles[PLAYER_COLOR_ORANGE] = &gOverlayTypes[OVERLAY_RANDOM_CASTLE_4];
    castles[PLAYER_COLOR_PURPLE] = &gOverlayTypes[OVERLAY_RANDOM_CASTLE_5];
    regionGrid = new u8[MAP_WIDTH * MAP_HEIGHT];
    memset(regionGrid, 0, MAP_WIDTH * MAP_HEIGHT);
    for (slot = 0; slot < GAME_PLAYER_COUNT; slot++) {
        reachedGrids[slot] = new u8[MAP_WIDTH * MAP_HEIGHT];
        memset(reachedGrids[slot], 0, MAP_WIDTH * MAP_HEIGHT);
    }
    continents = 0;
    for (regionId = 1; regionId < RANDOM_MAP_REGION_LIMIT; regionId++) {
        foundX = foundY = EDIT_NO_CELL;
        for (tileX = 0; tileX < MAP_WIDTH; tileX++) {
            for (tileY = 0; tileY < MAP_HEIGHT; tileY++) {
                if (CELL_TERRAIN(gMap.CellAt(tileX, tileY)) != TERRAIN_WATER
                    && !MAP_GRID_CELL(regionGrid, tileX, tileY)) {
                    continents++;
                    foundX = tileX;
                    foundY = tileY;
                    MAP_GRID_CELL(regionGrid, tileX, tileY) = regionId;
                    tileX = tileY = RANDOM_MAP_END_SCAN;
                }
            }
        }
        if (foundX >= 0) {
            filled = true;
            while (filled) {
                filled = false;
                for (tileX = 0; tileX < MAP_WIDTH; tileX++) {
                    for (tileY = 0; tileY < MAP_HEIGHT; tileY++) {
                        if (CELL_TERRAIN(gMap.CellAt(tileX, tileY)) != TERRAIN_WATER
                            && !MAP_GRID_CELL(regionGrid, tileX, tileY)) {
                            if (tileX > 0 && *(regionGrid + tileX - 1 + tileY * MAP_WIDTH) > 0)
                                MAP_GRID_CELL(regionGrid, tileX, tileY)
                                    = *(regionGrid + tileX - 1 + tileY * MAP_WIDTH);
                            else if (tileX < MAP_WIDTH - 1
                                     && *(regionGrid + tileX + 1 + tileY * MAP_WIDTH) > 0)
                                MAP_GRID_CELL(regionGrid, tileX, tileY)
                                    = *(regionGrid + tileX + 1 + tileY * MAP_WIDTH);
                            else if (tileY > 0
                                     && MAP_GRID_CELL(regionGrid, tileX, tileY - 1) > 0)
                                MAP_GRID_CELL(regionGrid, tileX, tileY)
                                    = MAP_GRID_CELL(regionGrid, tileX, tileY - 1);
                            else if (tileY < MAP_HEIGHT - 1
                                     && MAP_GRID_CELL(regionGrid, tileX, tileY + 1) > 0)
                                MAP_GRID_CELL(regionGrid, tileX, tileY)
                                    = MAP_GRID_CELL(regionGrid, tileX, tileY + 1);
                            if (MAP_GRID_CELL(regionGrid, tileX, tileY))
                                filled = true;
                        }
                    }
                }
            }
        } else
            regionId = RANDOM_MAP_END_SCAN;
    }
    memset(regionSizes, 0, sizeof(regionSizes));
    for (tileX = 0; tileX < MAP_WIDTH; tileX++)
        for (tileY = 0; tileY < MAP_HEIGHT; tileY++)
            regionSizes[MAP_GRID_CELL(regionGrid, tileX, tileY)]++;
    for (slot = 0; slot < RANDOM_MAP_REGION_LIMIT; slot++)
        shareValue[slot] = 0.0;
    for (slot = 1; slot <= continents; slot++) {
        rank[slot] = slot;
        shareValue[slot] = static_cast<float>(regionSizes[slot])
                           / (static_cast<float>(gLandCellCount)) * 100.0f;
    }
    for (round = 1; round < continents; round++) {
        for (slot = round; slot < continents; slot++) {
            if (regionSizes[rank[slot]] < regionSizes[rank[slot + 1]]) {
                t = rank[slot];
                rank[slot] = rank[slot + 1];
                rank[slot + 1] = t;
            }
        }
    }
    if (shareValue[rank[1]] > 80.0) {
        castleRegion[0] = castleRegion[1] = castleRegion[2] = castleRegion[3] = castleRegion[4]
            = castleRegion[5] = rank[1];
        regionsUsed = 1;
    } else if (shareValue[rank[1]] > 40.0 && shareValue[rank[2]] < 10.0) {
        castleRegion[0] = castleRegion[1] = castleRegion[2] = castleRegion[3] = castleRegion[4]
            = castleRegion[5] = rank[1];
        regionsUsed = 1;
    } else if (shareValue[rank[1]] < 55.0 && shareValue[rank[2]] > 25.0) {
        castleRegion[0] = castleRegion[2] = castleRegion[4] = rank[1];
        castleRegion[1] = castleRegion[3] = castleRegion[5] = rank[2];
        regionsUsed = 2;
    } else if (shareValue[rank[1]] < 50.0 && shareValue[rank[2]] > 15.0
               && shareValue[rank[3]] > 15.0 && shareValue[rank[4]] > 15.0) {
        castleRegion[0] = rank[1];
        castleRegion[1] = rank[2];
        castleRegion[2] = rank[3];
        castleRegion[3] = rank[4];
        castleRegion[4] = rank[1];
        castleRegion[5] = rank[1];
        regionsUsed = 4;
    } else if (shareValue[rank[1]] < 30.0 && shareValue[rank[2]] > 8.0 && shareValue[rank[3]] > 8.0
               && shareValue[rank[4]] > 8.0) {
        castleRegion[0] = rank[1];
        castleRegion[1] = rank[2];
        castleRegion[2] = rank[3];
        castleRegion[3] = rank[4];
        castleRegion[4] = rank[1];
        castleRegion[5] = rank[1];
        regionsUsed = 4;
    } else {
        castleRegion[0] = castleRegion[1] = castleRegion[2] = castleRegion[3] = castleRegion[4]
            = castleRegion[5] = rank[1];
        regionsUsed = 1;
    }
    ShowStatusText(localization::Tr("editor.random.status.castles"));
    for (c = 0; c < gRandomMapPlayers; c++) {
        attempt = 0;
        top = 0;
    retry:
        attempt++;
        for (tileX = 4; tileX < MAP_WIDTH - 4; tileX++) {
            for (tileY = 4; tileY < MAP_HEIGHT - 4; tileY++) {
                if (MAP_GRID_CELL(regionGrid, tileX, tileY) == castleRegion[c]) {
                    rating = Random(1000, 1200);
                    rating += Random(0, attempt * 100 + 1);
                    if (CELL_TERRAIN(gMap.CellAt(tileX - 3, tileY + 1)) == TERRAIN_WATER)
                        rating += regionsUsed > 1 ? 1000 : 500;
                    else if (CELL_TERRAIN(gMap.CellAt(tileX - 4, tileY + 2)) == TERRAIN_WATER)
                        rating += regionsUsed > 1 ? 1000 : 500;
                    for (nearX = tileX - 4; nearX <= tileX + 1; nearX++)
                        for (nearY = tileY - 2; nearY <= tileY; nearY++)
                            if (MAP_GRID_CELL(regionGrid, nearX, nearY) == castleRegion[c])
                                rating += 250;
                    if (rating > top) {
                        for (nearX = 0; nearX < MAP_WIDTH - 1; nearX++) {
                            for (nearY = 0; nearY < MAP_HEIGHT - 1; nearY++) {
                                if (gMap.CellAt(nearX, nearY)->m_objectTileset
                                        == TILESET_OBJNTWRD
                                    && HAS(gMap.CellAt(nearX, nearY)->m_triggerType,
                                           MAP_TRIGGER_ACTION_FLAG)) {
                                    dist = MANHATTAN_LENGTH(nearX - tileX, nearY - tileY);
                                    if (gMap.CellAt(nearX, nearY)->m_triggerType
                                        == MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_CASTLE)) {
                                        if (dist < 8)
                                            rating -= 3000;
                                        else if (dist < 40)
                                            rating -= (40 - dist) * 15;
                                    } else {
                                        if (dist < 6)
                                            rating -= 3000;
                                        else if (dist < 40)
                                            rating -= (40 - dist) * 7;
                                    }
                                }
                            }
                        }
                    }
                    if (rating > top) {
                        keeps[c].x = tileX - 2;
                        keeps[c].y = tileY - 1;
                        top = rating;
                    }
                }
            }
        }
        tileX = keeps[c].x + 2;
        tileY = keeps[c].y + 1;
        if (top < 0)
            ShutDown(localization::Tr("editor.random.castles_failed"));
        terrain = IDX(CELL_TERRAIN(gMap.CellAt(tileX, tileY)));
        gEditManager->ClearArea(tileX - 4, tileY - 2, 5, 4, EDIT_CLEAR_ALL, true, false);
        for (nearX = tileX - 4; nearX <= tileX + 1; nearX++)
            for (nearY = tileY - 2; nearY <= tileY + 1; nearY++)
                gMap.CellAt(nearX, nearY)->m_terrainImageIndex =
                    ChooseGroundTile(terrain, EDIT_SHAPE_PLAIN, false, nearX, nearY, false, 1.0f);
        castlePlaced = PlaceOverlayAt(castles[c], tileX, tileY);
        if (!castlePlaced)
            goto retry;
        tileX = keeps[c].x;
        tileY = keeps[c].y;
        if (regionsUsed > 1)
            steps = RANDOM_MAP_UNLIMITED_STEPS;
        else if (CELL_TERRAIN(gMap.CellAt(tileX - 1, tileY + 2)) == TERRAIN_WATER)
            steps = 1;
        else if (CELL_TERRAIN(gMap.CellAt(tileX - 2, tileY + 3)) == TERRAIN_WATER)
            steps = 3;
        else
            steps = 0;
        if (steps) {
            coastAt = false;
            nearX = tileX - 2;
            nearY = tileY + 2;
            ResetArea(nearX, nearY, 2, 2);
            while (!coastAt && steps) {
                steps--;
                if (CELL_TERRAIN(gMap.CellAt(nearX, nearY)) == TERRAIN_WATER
                    || CELL_TERRAIN(gMap.CellAt(nearX + 1, nearY)) == TERRAIN_WATER)
                    coastAt = true;
                else {
                    gMap.CellAt(nearX, nearY)->m_terrainImageIndex = ChooseGroundTile(
                        IDX(TERRAIN_WATER),
                        EDIT_SHAPE_PLAIN,
                        false,
                        nearX,
                        nearY,
                        false,
                        1.0f
                    );
                    gMap.CellAt(nearX + 1, nearY)->m_terrainImageIndex = ChooseGroundTile(
                        IDX(TERRAIN_WATER),
                        EDIT_SHAPE_PLAIN,
                        false,
                        nearX + 1,
                        nearY,
                        false,
                        1.0f
                    );
                    if (nearX > 0)
                        nearX--;
                    if (nearY < MAP_HEIGHT - 1)
                        nearY++;
                    if (nearX == 0 && nearY == MAP_HEIGHT - 1)
                        coastAt = true;
                }
            }
        }
        for (peerIndex = 0; peerIndex < c; peerIndex++) {
            if (castleRegion[c] != castleRegion[c - 1])
                continue;
            if (keeps[peerIndex].x < keeps[c].x && keeps[peerIndex].y < keeps[c].y) {
                fromX = tileX - 2;
                fromY = tileY - 2;
                destX = keeps[peerIndex].x + 1;
                destY = keeps[peerIndex].x + 1;
            } else if (keeps[peerIndex].x < keeps[c].x && keeps[peerIndex].y >= keeps[c].y) {
                fromX = tileX + 1;
                fromY = tileY + 1;
                destX = keeps[peerIndex].x + 1;
                destY = keeps[peerIndex].x - 2;
            } else if (keeps[peerIndex].x >= keeps[c].x && keeps[peerIndex].y < keeps[c].y) {
                fromX = tileX + 1;
                fromY = tileY - 2;
                destX = keeps[peerIndex].x + 1;
                destY = keeps[peerIndex].x + 1;
            } else {
                fromX = tileX + 1;
                fromY = tileY + 1;
                destX = keeps[peerIndex].x - 1;
                destY = keeps[peerIndex].x - 2;
            }
            nearX = fromX;
            nearY = fromY;
            tracing = true;
            while (tracing) {
                if (nearX == destX && nearY == destY)
                    tracing = false;
                if (destX > nearX)
                    stepX = 1;
                else if (destX < nearX)
                    stepX = -1;
                else
                    stepX = 0;
                if (destY > nearY)
                    stepY = 1;
                else if (destY < nearY)
                    stepY = -1;
                else
                    stepY = 0;
                if (stepX && CELL_TERRAIN(gMap.CellAt(nearX + stepX, nearY)) != TERRAIN_WATER)
                    stepY = 0;
                else if (stepY
                         && CELL_TERRAIN(gMap.CellAt(nearX, nearY + stepY)) != TERRAIN_WATER)
                    stepX = 0;
                else {
                    tracing = false;
                    cutOff[c] = true;
                    cutOff[peerIndex] = true;
                }
                if (tracing
                    && CELL_TERRAIN(gMap.CellAt(nearX + stepX, nearY + stepY)) != TERRAIN_WATER) {
                    nearX += stepX;
                    nearY += stepY;
                    roadMask = EDIT_CLEAR_ROAD_MASK;
                    memset(gClearTilesets, 1, sizeof(gClearTilesets));
                    gClearTilesets[IDX(TILESET_OBJNTOWN)] = 0;
                    gClearTilesets[IDX(TILESET_OBJNTWBA)] = 0;
                    gClearTilesets[IDX(TILESET_OBJNTWSH)] = 0;
                    gClearTilesets[IDX(TILESET_OBJNTWRD)] = 0;
                    gClearTilesets[IDX(TILESET_MINIMON)] = 0;
                    gClearTilesets[IDX(TILESET_OBJNARTI)] = 0;
                    gClearTilesets[IDX(TILESET_MONS32)] = 0;
                    gEditManager->ClearArea(nearX, nearY, 1, 1, 0, false, true);
                }
            }
        }
    }
    if (Random(0, 100) < 50) {
        extraRoads[0] = true;
        extraRoads[1] = true;
        if (Random(0, 100) < 50)
            extraRoads[2] = true;
        if (Random(0, 100) < 50)
            extraRoads[3] = true;
    }
    if (cutOff[0] || cutOff[1] || cutOff[2] || cutOff[3] || cutOff[4] || cutOff[5]
        || extraRoads[0]) {
        ShowStatusText(localization::Tr("editor.random.status.roads"));
        for (slot = 0; slot < gRandomMapPlayers; slot++) {
            cutOff[slot] = false;
            reachable[slot] = 0;
            for (tileX = keeps[slot].x - 2; tileX <= keeps[slot].x + 2; tileX++)
                for (tileY = keeps[slot].y - 2; tileY <= keeps[slot].y + 1; tileY++)
                    MAP_GRID_CELL(reachedGrids[slot], tileX, tileY) = 1;
            filled = true;
            while (filled) {
                filled = false;
                for (tileX = 0; tileX < MAP_WIDTH; tileX++) {
                    for (tileY = 0; tileY < MAP_HEIGHT; tileY++) {
                        if (CELL_TERRAIN(gMap.CellAt(tileX, tileY)) != TERRAIN_WATER
                            && (gMap.CellAt(tileX, tileY)->m_objectTileset == TILESET_NONE
                                || gMap.CellAt(tileX, tileY)->m_objectTileset == TILESET_MONS32)
                            && !MAP_GRID_CELL(reachedGrids[slot], tileX, tileY)) {
                            if (tileX > 0
                                && *(reachedGrids[slot] + tileX - 1 + tileY * MAP_WIDTH) > 0)
                                MAP_GRID_CELL(reachedGrids[slot], tileX, tileY)
                                    = *(reachedGrids[slot] + tileX - 1 + tileY * MAP_WIDTH);
                            else if (tileX < MAP_WIDTH - 1
                                     && *(reachedGrids[slot] + tileX + 1 + tileY * MAP_WIDTH) > 0)
                                MAP_GRID_CELL(reachedGrids[slot], tileX, tileY)
                                    = *(reachedGrids[slot] + tileX + 1 + tileY * MAP_WIDTH);
                            else if (tileY > 0
                                     && MAP_GRID_CELL(reachedGrids[slot], tileX, tileY - 1)
                                            > 0)
                                MAP_GRID_CELL(reachedGrids[slot], tileX, tileY)
                                    = MAP_GRID_CELL(reachedGrids[slot], tileX, tileY - 1);
                            else if (tileY < MAP_HEIGHT - 1
                                     && MAP_GRID_CELL(reachedGrids[slot], tileX, tileY + 1)
                                            > 0)
                                MAP_GRID_CELL(reachedGrids[slot], tileX, tileY)
                                    = MAP_GRID_CELL(reachedGrids[slot], tileX, tileY + 1);
                            if (MAP_GRID_CELL(reachedGrids[slot], tileX, tileY)) {
                                filled = true;
                                reachable[slot]++;
                            }
                        }
                    }
                }
            }
        }
        for (slot = 1; slot < gRandomMapPlayers; slot++) {
            for (t = 0; t < slot; t++) {
                if (castleRegion[slot] == castleRegion[t]) {
                    meet = false;
                    for (tileX = 0; tileX < MAP_WIDTH; tileX++)
                        for (tileY = 0; tileY < MAP_HEIGHT; tileY++)
                            if (MAP_GRID_CELL(reachedGrids[slot], tileX, tileY)
                                && MAP_GRID_CELL(reachedGrids[t], tileX, tileY))
                                meet = true;
                    if (!meet) {
                        cutOff[slot] = true;
                        cutOff[t] = true;
                    }
                }
            }
        }
        for (slot = 0; slot < gRandomMapPlayers; slot++) {
            if (cutOff[slot] || extraRoads[slot]) {
                steps = 20000;
                tracing = true;
                while (tracing) {
                    if (steps-- < 0)
                        tracing = false;
                    tileX = Random(0, MAP_WIDTH - 1);
                    tileY = Random(0, MAP_HEIGHT - 1);
                    if (MAP_GRID_CELL(reachedGrids[slot], tileX, tileY)) {
                        dist = MANHATTAN_LENGTH(tileX - keeps[slot].x, tileY - keeps[slot].y);
                        if (steps < 10000 || Random(0, 100) < dist) {
                            tracing = false;
                            for (nearX = 0; nearX < MAP_WIDTH; nearX++) {
                                for (nearY = 0; nearY < MAP_HEIGHT; nearY++) {
                                    if (gMap.CellAt(nearX, nearY)->m_triggerType
                                        == MAP_ACTION_TRIGGER(MAP_OBJECT_STONE_LITHS)) {
                                        dist = MANHATTAN_LENGTH(tileX - nearX, tileY - nearY);
                                        if (steps > 10000 && dist < 40 && Random(0, 100) > dist)
                                            tracing = true;
                                    }
                                }
                            }
                        }
                    }
                }
                gEditManager->ClearArea(tileX, tileY, 1, 1, EDIT_CLEAR_ALL, true, false);
                PlaceOverlayAt(&gOverlayTypes[OVERLAY_STONE_LITHS], tileX, tileY);
            }
        }
    }
    delete regionGrid;
    for (slot = 0; slot < GAME_PLAYER_COUNT; slot++)
        delete reachedGrids[slot];
}
#undef roadMask
#undef destY
#undef castleRegion
#undef attempt
#undef castles
#undef fromY
#undef nearY
#undef foundX
#undef castlePlaced

#define unusedIndex unusedIndex_l   // frame-slot spelling
#define unusedMarker unusedMarker_g // frame-slot spelling
VA(0x004208ca, 0x217)
b32 editManager::PlaceResourceSite(i32 x, i32 y, i32 resource) {
    overlayType* site;
    overlayType* H2_UNUSED(unusedMarker);
    i32 H2_UNUSED(unusedIndex);
    i32 ground;

    site = NULL;
    ground = IDX(CELL_TERRAIN(gMap.CellAt(x, y)));
    if (resource == IDX(RES_WOOD)) {
        switch (ground) {
            case IDX(TERRAIN_SNOW):
                site = &gOverlayTypes[OVERLAY_SAWMILL_SNOW];
                break;
            case IDX(TERRAIN_LAVA):
                site = &gOverlayTypes[OVERLAY_SAWMILL_LAVA];
                break;
            case IDX(TERRAIN_DIRT):
                site = &gOverlayTypes[OVERLAY_SAWMILL_DIRT];
                break;
            case IDX(TERRAIN_DESERT):
                site = &gOverlayTypes[OVERLAY_SAWMILL_DESERT];
                break;
            case IDX(TERRAIN_WASTELAND):
                site = &gOverlayTypes[OVERLAY_SAWMILL_WASTELAND];
                break;
            case IDX(TERRAIN_GRASS):
            case IDX(TERRAIN_SWAMP):
                site = &gOverlayTypes[OVERLAY_SAWMILL_GRASS];
                break;
            default:
                site = &gOverlayTypes[OVERLAY_SAWMILL_DIRT];
                break;
        }
        return PlaceOverlayAt(site, x, y);
    } else if (resource == IDX(RES_MERCURY)) {
        switch (ground) {
            case IDX(TERRAIN_SNOW):
                site = &gOverlayTypes[OVERLAY_ALCHEMIST_LAB_SNOW];
                break;
            default:
                site = &gOverlayTypes[OVERLAY_ALCHEMIST_LAB];
                break;
        }
        return PlaceOverlayAt(site, x, y);
    } else {
        switch (ground) {
            case IDX(TERRAIN_GRASS):
                site = &gOverlayTypes[resource + OVERLAY_MINES_GRASS];
                break;
            case IDX(TERRAIN_SNOW):
                site = &gOverlayTypes[resource + OVERLAY_MINES_SNOW];
                break;
            case IDX(TERRAIN_SWAMP):
                site = &gOverlayTypes[resource + OVERLAY_MINES_SWAMP];
                break;
            case IDX(TERRAIN_LAVA):
                site = &gOverlayTypes[resource + OVERLAY_MINES_LAVA];
                break;
            case IDX(TERRAIN_DESERT):
            case IDX(TERRAIN_BEACH):
                site = &gOverlayTypes[resource + OVERLAY_MINES_DESERT];
                break;
            case IDX(TERRAIN_DIRT):
                site = &gOverlayTypes[resource + OVERLAY_MINES_DIRT];
                break;
            case IDX(TERRAIN_WASTELAND):
                site = &gOverlayTypes[resource + OVERLAY_MINES_WASTELAND];
                break;
            default:
                site = &gOverlayTypes[resource + OVERLAY_MINES_WATER];
                break;
        }
        return PlaceOverlayAt(site, x, y);
    }
}
#undef unusedIndex
#undef unusedMarker

#define kind kind_p                           // frame-slot spelling
#define placed placed_b                       // frame-slot spelling
#define tries tries_c                         // frame-slot spelling
#define obelisks obelisks_l                   // frame-slot spelling
#define veryStrongMonster veryStrongMonster_i // frame-slot spelling
#define decorations decorations_a             // frame-slot spelling
#define weakMonster weakMonster_i             // frame-slot spelling
#define unusedEntry unusedEntry_i             // frame-slot spelling
#define mediumMonster mediumMonster_k         // frame-slot spelling
#define sitesPlaced sitesPlaced_o             // frame-slot spelling
#define dist dist_j                           // frame-slot spelling
#define sites sites_m                         // frame-slot spelling
#define randomTown randomTown_c               // frame-slot spelling
VA(0x00420ae1, 0xac8)
void editManager::PlaceRandomObjects(i32 density, i32 monsterDensity) {
    b32 placed;
    b32 valid;
    i32 kind;
    i32 towns;
    overlayType* H2_UNUSED(weakMonster);
    i32 decorations;
    i32 x;
    i32 appeal;
    overlayType* H2_UNUSED(veryStrongMonster);
    overlayType* obelisks[IDX(TERRAIN_COUNT)];
    i32 tries;
    overlayType* strongMonster;
    overlayType* mediumMonster;
    i32 H2_UNUSED(unusedEntry);
    i32 y;
    i32 i;
    overlayType* randomTown;
    i32 sites;
    i32 j;
    i32 dist;
    i32 ground;
    i32 quota[IDX(RES_COUNT)];
    i32 H2_UNUSED(sitesPlaced);
    overlayType* H2_UNUSED(anyMonster);

    monsterDensity = monsterDensity * 1.2;
    anyMonster = &gOverlayTypes[OVERLAY_RANDOM_MONSTER];
    weakMonster = &gOverlayTypes[OVERLAY_RANDOM_MONSTER_WEAK];
    mediumMonster = &gOverlayTypes[OVERLAY_RANDOM_MONSTER_MEDIUM];
    strongMonster = &gOverlayTypes[OVERLAY_RANDOM_MONSTER_STRONG];
    veryStrongMonster = &gOverlayTypes[OVERLAY_RANDOM_MONSTER_VERY_STRONG];
    randomTown = &gOverlayTypes[OVERLAY_RANDOM_NEUTRAL_TOWN];
    sitesPlaced = 0;
    ScatterDecorations();
    obelisks[IDX(TERRAIN_GRASS)] = &gOverlayTypes[OVERLAY_OBELISK_GRASS];
    obelisks[IDX(TERRAIN_SNOW)] = &gOverlayTypes[OVERLAY_OBELISK_SNOW];
    obelisks[IDX(TERRAIN_SWAMP)] = &gOverlayTypes[OVERLAY_OBELISK_SWAMP];
    obelisks[IDX(TERRAIN_LAVA)] = &gOverlayTypes[OVERLAY_OBELISK_LAVA];
    obelisks[IDX(TERRAIN_DESERT)] = &gOverlayTypes[OVERLAY_OBELISK_DESERT];
    obelisks[IDX(TERRAIN_DIRT)] = &gOverlayTypes[OVERLAY_OBELISK_DIRT];
    obelisks[IDX(TERRAIN_WASTELAND)] = &gOverlayTypes[OVERLAY_OBELISK_WASTELAND];
    obelisks[IDX(TERRAIN_BEACH)] = &gOverlayTypes[OVERLAY_OBELISK_DIRT];
    towns = gLandCellCount / RANDOM_MAP_LAND_PER_TOWN;
    if (towns > RANDOM_MAP_MAX_TOWNS)
        towns = RANDOM_MAP_MAX_TOWNS;
    if (towns < 1)
        towns = 1;
    towns = towns * RANDOM_MAP_TRIES_PER_OBJECT;
    while (towns > 0) {
        towns--;
        x = Random(2, MAP_WIDTH - 3);
        y = Random(2, MAP_WIDTH - 2);
        valid = true;
        for (i = -2; i <= 2; i++) {
            for (j = -2; j <= 1; j++) {
                if (CELL_TERRAIN(gMap.CellAt(x + i, y + j)) == TERRAIN_WATER
                    || gMap.CellAt(x + i, y + j)->m_objectTileset == TILESET_OBJNTWRD
                    || gMap.CellAt(x + i, y + j)->m_overlayTileset == TILESET_OBJNTWRD)
                    valid = false;
            }
        }
        for (i = 0; i < MAP_WIDTH - 1; i++) {
            for (j = 0; j < MAP_HEIGHT - 1; j++) {
                if (gMap.CellAt(i, j)->m_objectTileset == TILESET_OBJNTWRD
                    && HAS(gMap.CellAt(i, j)->m_triggerType, MAP_TRIGGER_ACTION_FLAG)) {
                    dist = MANHATTAN_LENGTH(i - x, j - y);
                    if (dist < 10 || dist < Random(0, 40))
                        valid = false;
                }
            }
        }
        if (valid) {
            towns -= RANDOM_MAP_TRIES_PER_OBJECT;
            gEditManager->ClearArea(x - 2, y - 2, 5, 4, EDIT_CLEAR_ALL, true, false);
            PlaceOverlayAt(randomTown, x - 2, y);
        }
    }
    sites = gLandCellCount / RANDOM_MAP_LAND_PER_SITE;
    ScaleByDensity(&sites, density);
    quota[IDX(RES_WOOD)] = (sites - 13) / 7 + 5;
    quota[IDX(RES_ORE)] = (sites - 13) / 7 + 5;
    quota[IDX(RES_GOLD)] = (sites - 13) / 7 + 2;
    quota[IDX(RES_GEMS)] = (sites - 13) / 7 + 2;
    quota[IDX(RES_CRYSTAL)] = (sites - 13) / 7 + 2;
    quota[IDX(RES_SULFUR)] = (sites - 13) / 7 + 2;
    quota[IDX(RES_MERCURY)] = (sites - 13) / 7 + 2;
    if (sites > RANDOM_MAP_MAX_SITES)
        sites = RANDOM_MAP_MAX_SITES;
    if (sites < RANDOM_MAP_MIN_SITES)
        sites = RANDOM_MAP_MIN_SITES;
    sites = sites * RANDOM_MAP_TRIES_PER_SITE;
    while (sites > 0) {
        sites--;
        valid = true;
        x = Random(1, MAP_WIDTH - 2);
        y = Random(1, MAP_WIDTH - 2);
        if (valid && CELL_TERRAIN(gMap.CellAt(x, y)) != TERRAIN_WATER
            && CELL_TERRAIN(gMap.CellAt(x + 1, y)) != TERRAIN_WATER
            && CELL_TERRAIN(gMap.CellAt(x, y - 1)) != TERRAIN_WATER
            && CELL_TERRAIN(gMap.CellAt(x + 1, y - 1)) != TERRAIN_WATER
            && CELL_TERRAIN(gMap.CellAt(x - 1, y + 1)) != TERRAIN_WATER
            && gMap.CellAt(x, y - 1)->m_overlayTileset == TILESET_NONE
            && gMap.CellAt(x + 1, y - 1)->m_overlayTileset == TILESET_NONE
            && CELL_TERRAIN(gMap.CellAt(x, y)) == CELL_TERRAIN(gMap.CellAt(x + 1, y))) {
            tries = 0;
            valid = true;
            while (valid && tries < 10) {
                tries++;
                kind = Random(0, IDX(RES_GOLD));
                appeal = 4;
                if (quota[kind] > 0)
                    appeal += 30;
                if (kind == IDX(RES_WOOD))
                    appeal += 8;
                if (kind == IDX(RES_ORE))
                    appeal += 8;
                for (i = 0; i < MAP_WIDTH; i++) {
                    for (j = 0; j < MAP_WIDTH; j++) {
                        if (gMap.CellAt(i, j)->m_triggerType == MAP_ACTION_TRIGGER(MAP_OBJECT_MINE)
                            || gMap.CellAt(i, j)->m_triggerType
                                   == MAP_ACTION_TRIGGER(MAP_OBJECT_SAWMILL)
                            || gMap.CellAt(i, j)->m_triggerType
                                   == MAP_ACTION_TRIGGER(MAP_OBJECT_ALCHEMIST_LAB)) {
                            dist = MANHATTAN_LENGTH(i - x, j - y);
                            if (dist < 10)
                                appeal -= 10 - dist;
                            if (((kind == IDX(RES_WOOD)
                                  && gMap.CellAt(i, j)->m_triggerType
                                         == MAP_ACTION_TRIGGER(MAP_OBJECT_SAWMILL))
                                 || (kind == IDX(RES_MERCURY)
                                     && gMap.CellAt(i, j)->m_triggerType
                                            == MAP_ACTION_TRIGGER(MAP_OBJECT_ALCHEMIST_LAB)))
                                && dist < 15)
                                appeal -= (15 - dist) * 2;
                        }
                    }
                }
                if (Random(0, 40) < appeal) {
                    gEditManager->ClearArea(x, y - 1, 2, 2, EDIT_CLEAR_ALL, false, false);
                    gEditManager->ClearArea(x - 1, y + 1, 1, 1, EDIT_CLEAR_ALL, false, false);
                    placed = PlaceResourceSite(x, y, kind);
                    if (placed) {
                        sitesPlaced++;
                        quota[kind]--;
                        if (Random(0, 100) < monsterDensity)
                            PlaceOverlayAt(
                                Random(0, 100) < 30 ? mediumMonster : strongMonster, x - 1, y + 1);
                        sites -= RANDOM_MAP_TRIES_PER_SITE;
                    }
                    valid = false;
                }
            }
        }
    }
    decorations = gLandCellCount / RANDOM_MAP_LAND_PER_OBELISK;
    if (decorations > RANDOM_MAP_MAX_OBELISKS)
        decorations = RANDOM_MAP_MAX_OBELISKS;
    if (decorations < RANDOM_MAP_MIN_OBELISKS)
        decorations = RANDOM_MAP_MIN_OBELISKS;
    decorations = decorations * RANDOM_MAP_TRIES_PER_OBJECT;
    while (decorations > 0) {
        decorations--;
        valid = true;
        x = Random(0, MAP_WIDTH - 1);
        y = Random(0, MAP_WIDTH - 1);
        if (CELL_TERRAIN(gMap.CellAt(x, y)) != TERRAIN_WATER) {
            ground = IDX(CELL_TERRAIN(gMap.CellAt(x, y)));
            if (CanPlaceOverlayAt(obelisks[ground], x, y, false)) {
                decorations -= RANDOM_MAP_TRIES_PER_OBJECT;
                PlaceOverlayAt(obelisks[ground], x, y);
            }
        }
    }
}
#undef kind
#undef placed
#undef tries
#undef obelisks
#undef veryStrongMonster
#undef decorations
#undef weakMonster
#undef unusedEntry
#undef mediumMonster
#undef sitesPlaced
#undef dist
#undef sites
#undef randomTown

#define treasures treasures_f                 // frame-slot spelling
#define lamp lamp_j                           // frame-slot spelling
#define southEast southEast_g                 // frame-slot spelling
#define northEast northEast_m                 // frame-slot spelling
#define south south_c                         // frame-slot spelling
#define roll roll_a                           // frame-slot spelling
#define veryStrongMonster veryStrongMonster_i // frame-slot spelling
#define weakMonster weakMonster_i             // frame-slot spelling
#define mediumMonster mediumMonster_l         // frame-slot spelling
#define anyMonster anyMonster_j               // frame-slot spelling
#define kind kind_i                           // frame-slot spelling
#define guards guards_h                       // frame-slot spelling
#define placedCount placedCount_p             // frame-slot spelling
#define resource resource_a                   // frame-slot spelling
#define southWest southWest_p                 // frame-slot spelling
#define northWest northWest_c                 // frame-slot spelling
#define minorArtifact minorArtifact_p         // frame-slot spelling
#define guardedCount guardedCount_j           // frame-slot spelling
VA(0x004215a9, 0xbdb)
void editManager::PlaceTreasures(i32 density, i32 monsterDensity) {
    overlayType* lamp;
    i32 treasures;
    b32 south;
    b32 northEast;
    b32 southEast;
    overlayType* chest;
    b32 east;
    overlayType* H2_UNUSED(anyMonster);
    overlayType* mediumMonster;
    i32 x;
    overlayType* weakMonster;
    overlayType* veryStrongMonster;
    i32 roll;
    overlayType* strongMonster;
    i32 placedCount;
    overlayType* majorArtifact;
    i32 guards;
    i32 kind;
    i32 y;
    b32 northWest;
    b32 southWest;
    overlayType* resource;
    b32 north;
    overlayType* minorArtifact;
    b32 west;
    overlayType* treasureArtifact;
    i32 guardedCount;
    i32 layout;

    anyMonster = &gOverlayTypes[OVERLAY_RANDOM_MONSTER];
    weakMonster = &gOverlayTypes[OVERLAY_RANDOM_MONSTER_WEAK];
    mediumMonster = &gOverlayTypes[OVERLAY_RANDOM_MONSTER_MEDIUM];
    strongMonster = &gOverlayTypes[OVERLAY_RANDOM_MONSTER_STRONG];
    veryStrongMonster = &gOverlayTypes[OVERLAY_RANDOM_MONSTER_VERY_STRONG];
    resource = &gOverlayTypes[OVERLAY_RANDOM_RESOURCE];
    treasureArtifact = &gOverlayTypes[OVERLAY_RANDOM_TREASURE_ARTIFACT];
    minorArtifact = &gOverlayTypes[OVERLAY_RANDOM_MINOR_ARTIFACT];
    majorArtifact = &gOverlayTypes[OVERLAY_RANDOM_MAJOR_ARTIFACT];
    chest = &gOverlayTypes[OVERLAY_TREASURE_CHEST];
    lamp = &gOverlayTypes[OVERLAY_ANCIENT_LAMP];
    treasures = gLandCellCount / RANDOM_MAP_LAND_PER_TREASURE;
    ScaleByDensity(&treasures, density);
    treasures = treasures * RANDOM_MAP_TRIES_PER_OBJECT;
    placedCount = 0;
    guardedCount = 0;
    while (treasures > 0) {
        treasures--;
        x = Random(1, MAP_WIDTH - 2);
        y = Random(1, MAP_WIDTH - 2);
        if (CELL_TERRAIN(gMap.CellAt(x, y)) != TERRAIN_WATER
            && gMap.CellAt(x, y)->m_objectTileset == TILESET_NONE) {
            gEditManager->ClearArea(x, y, 1, 1, EDIT_CLEAR_ALL, false, false);
            roll = Random(0, 100);
            northWest = southWest = northEast = southEast = north = south = east = west = false;
            if (y == 0 || CELL_TERRAIN(gMap.CellAt(x, y - 1)) == TERRAIN_WATER
                || (gMap.CellAt(x, y - 1)->m_objectIndex != MAPCELL_SPRITE_NONE
                    && !gMap.CellAt(x, y - 1)->m_objectHighLayer
                    && !gMap.CellAt(x, y - 1)->m_objectShadow))
                north = true;
            if (y == MAP_HEIGHT - 1 || CELL_TERRAIN(gMap.CellAt(x, y + 1)) == TERRAIN_WATER
                || (gMap.CellAt(x, y + 1)->m_objectIndex != MAPCELL_SPRITE_NONE
                    && !gMap.CellAt(x, y + 1)->m_objectHighLayer
                    && !gMap.CellAt(x, y + 1)->m_objectShadow))
                south = true;
            if (x == 0 || CELL_TERRAIN(gMap.CellAt(x - 1, y)) == TERRAIN_WATER
                || (gMap.CellAt(x - 1, y)->m_objectIndex != MAPCELL_SPRITE_NONE
                    && !gMap.CellAt(x - 1, y)->m_objectHighLayer
                    && !gMap.CellAt(x - 1, y)->m_objectShadow))
                west = true;
            if (x == MAP_WIDTH - 1 || CELL_TERRAIN(gMap.CellAt(x + 1, y)) == TERRAIN_WATER
                || (gMap.CellAt(x + 1, y)->m_objectIndex != MAPCELL_SPRITE_NONE
                    && !gMap.CellAt(x + 1, y)->m_objectHighLayer
                    && !gMap.CellAt(x + 1, y)->m_objectShadow))
                east = true;
            if (x < MAP_WIDTH + 1 && y > 0
                && CELL_TERRAIN(gMap.CellAt(x + 1, y - 1)) != TERRAIN_WATER
                && gMap.CellAt(x + 1, y - 1)->m_objectTileset == TILESET_NONE)
                northEast = true;
            if (x < MAP_WIDTH + 1 && y < MAP_HEIGHT - 1
                && CELL_TERRAIN(gMap.CellAt(x + 1, y + 1)) != TERRAIN_WATER
                && gMap.CellAt(x + 1, y + 1)->m_objectTileset == TILESET_NONE)
                southEast = true;
            if (x > 0 && y < MAP_HEIGHT - 1
                && CELL_TERRAIN(gMap.CellAt(x - 1, y + 1)) != TERRAIN_WATER
                && gMap.CellAt(x - 1, y + 1)->m_objectTileset == TILESET_NONE)
                southWest = true;
            if (x > 0 && y > 0 && CELL_TERRAIN(gMap.CellAt(x - 1, y - 1)) != TERRAIN_WATER
                && gMap.CellAt(x - 1, y - 1)->m_objectTileset == TILESET_NONE)
                northWest = true;
            if (!east || !west || !north || !south) {
                placedCount++;
                treasures -= RANDOM_MAP_TRIES_PER_OBJECT;
                layout = TREASURE_UNGUARDED;
                if (guardedCount * 5 < placedCount) {
                    if (west && south && northEast && !southWest && !southEast && !northWest)
                        layout = TREASURE_GUARD_NE;
                    else if (west && north && southEast && !northWest && !southWest && !northEast)
                        layout = TREASURE_GUARD_SE;
                    else if (east && north && southWest && !northEast && !northWest && !southEast)
                        layout = TREASURE_GUARD_SW;
                    else if (east && south && northWest && !southEast && !southWest && !northEast)
                        layout = TREASURE_GUARD_NW;
                }
                if (layout > TREASURE_UNGUARDED) {
                    guardedCount++;
                    if (roll < 20)
                        PlaceOverlayAt(chest, x, y);
                    else if (roll < 75)
                        PlaceOverlayAt(minorArtifact, x, y);
                    else
                        PlaceOverlayAt(majorArtifact, x, y);
                    if (layout == TREASURE_GUARD_NE) {
                        gEditManager->ClearArea(x + 1, y - 1, 1, 1, EDIT_CLEAR_ALL, false, false);
                        PlaceOverlayAt(
                            Random(0, 100) < 50 ? strongMonster : veryStrongMonster, x + 1, y - 1);
                    } else if (layout == TREASURE_GUARD_SE) {
                        gEditManager->ClearArea(x + 1, y + 1, 1, 1, EDIT_CLEAR_ALL, false, false);
                        PlaceOverlayAt(
                            Random(0, 100) < 50 ? strongMonster : veryStrongMonster, x + 1, y + 1);
                    } else if (layout == TREASURE_GUARD_SW) {
                        gEditManager->ClearArea(x - 1, y + 1, 1, 1, EDIT_CLEAR_ALL, false, false);
                        PlaceOverlayAt(
                            Random(0, 100) < 50 ? strongMonster : veryStrongMonster, x - 1, y + 1);
                    } else {
                        gEditManager->ClearArea(x - 1, y - 1, 1, 1, EDIT_CLEAR_ALL, false, false);
                        PlaceOverlayAt(
                            Random(0, 100) < 50 ? strongMonster : veryStrongMonster, x - 1, y - 1);
                    }
                } else if (Random(0, 100) < 90) {
                    treasures += RANDOM_MAP_TRIES_PER_OBJECT + 1;
                    placedCount--;
                } else if (CELL_TERRAIN(gMap.CellAt(x, y)) == TERRAIN_DESERT && roll < 12)
                    PlaceOverlayAt(lamp, x, y);
                else if (roll < 2 && Random(0, 100) < 30)
                    PlaceOverlayAt(lamp, x, y);
                else if (roll < 35)
                    PlaceOverlayAt(chest, x, y);
                else if (roll < 40)
                    PlaceOverlayAt(treasureArtifact, x, y);
                else
                    PlaceOverlayAt(resource, x, y);
            }
        }
    }
    guards = gLandCellCount / RANDOM_MAP_LAND_PER_MONSTER;
    ScaleByDensity(&guards, monsterDensity);
    guards = guards * RANDOM_MAP_TRIES_PER_OBJECT;
    while (guards > 0) {
        guards--;
        x = Random(0, MAP_WIDTH - 1);
        y = Random(0, MAP_WIDTH - 1);
        if (CELL_TERRAIN(gMap.CellAt(x, y)) != TERRAIN_WATER
            && gMap.CellAt(x, y)->m_objectTileset == TILESET_NONE) {
            guards -= RANDOM_MAP_TRIES_PER_OBJECT;
            gEditManager->ClearArea(x, y, 1, 1, EDIT_CLEAR_ALL, false, false);
            kind = Random(0, 100);
            if (kind < 40)
                PlaceOverlayAt(weakMonster, x, y);
            else if (kind < 80)
                PlaceOverlayAt(mediumMonster, x, y);
            else
                PlaceOverlayAt(strongMonster, x, y);
        }
    }
}
#undef treasures
#undef lamp
#undef southEast
#undef northEast
#undef south
#undef roll
#undef veryStrongMonster
#undef weakMonster
#undef mediumMonster
#undef anyMonster
#undef kind
#undef guards
#undef placedCount
#undef resource
#undef southWest
#undef northWest
#undef minorArtifact
#undef guardedCount

#define index index_h // frame-slot spelling
#define type type_o   // frame-slot spelling
#define tries tries_k // frame-slot spelling
VA(0x00422184, 0x228)
void editManager::ScatterDecorations(void) {
    i32 roll;
    i32 tries;
    i32 chance[IDX(TERRAIN_COUNT)];
    overlayType* type;
    i32 terrain;
    i32 index;
    i32 x;
    i32 y;
    mapCell* cell;

    chance[IDX(TERRAIN_WATER)] = 55;
    chance[IDX(TERRAIN_GRASS)] = 120;
    chance[IDX(TERRAIN_SNOW)] = 100;
    chance[IDX(TERRAIN_SWAMP)] = 100;
    chance[IDX(TERRAIN_LAVA)] = 120;
    chance[IDX(TERRAIN_DESERT)] = 80;
    chance[IDX(TERRAIN_DIRT)] = 120;
    chance[IDX(TERRAIN_WASTELAND)] = 80;
    chance[IDX(TERRAIN_BEACH)] = 80;
    for (x = 0; x < MAP_WIDTH; x++) {
        for (y = 0; y < MAP_HEIGHT; y++) {
            cell = gMap.CellAt(x, y);
            terrain = IDX(CELL_TERRAIN(cell));
            if (Random(1, RANDOM_MAP_DECORATION_ROLL) <= chance[IDX(CELL_TERRAIN(cell))]
                && cell->m_objectIndex == MAPCELL_SPRITE_NONE
                && cell->m_overlayIndex == MAPCELL_SPRITE_NONE
                && (giGroundShape[cell->m_terrainImageIndex] == EDIT_SHAPE_PLAIN
                    || giGroundShape[cell->m_terrainImageIndex] == EDIT_SHAPE_DECORATED_SECOND
                    || giGroundShape[cell->m_terrainImageIndex] == EDIT_SHAPE_DECORATED_FOURTH
                    || giGroundShape[cell->m_terrainImageIndex] == EDIT_SHAPE_DECORATED_FIRST
                    || giGroundShape[cell->m_terrainImageIndex] == EDIT_SHAPE_DECORATED_THIRD)) {
                tries = RANDOM_MAP_TRIES_PER_OBJECT;
                while (tries-- > 0) {
                    index = Random(0, OVERLAY_TYPE_COUNT - 1);
                    roll = Random(1, 100);
                    type = &gOverlayTypes[index];
                    if (roll <= type->frequency && type->terrainMask & 1 << terrain
                        && CanPlaceOverlayAt(type, x, y, false)) {
                        PlaceOverlayAt(type, x, y);
                        tries = 0;
                    }
                }
            }
        }
    }
}
#undef index
#undef type
#undef tries
