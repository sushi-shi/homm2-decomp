#ifndef HOMM2_FINDPATH_H
#define HOMM2_FINDPATH_H

#include <Ints.h>
#include <SOURCE/KB_TYPES.h>

i32 CalcTerrainCost(
    H2_ENUM_PARAM(TerrainType, i32) terrain,
    i32 diagonal,
    i32 mobility,
    i32 pathfindingLevel,
    i32 useRoad,
    i32 usePathfinding
);

#endif
