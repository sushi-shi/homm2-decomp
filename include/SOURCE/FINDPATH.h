#ifndef HOMM2_SOURCE_FINDPATH_H
#define HOMM2_SOURCE_FINDPATH_H

#include <Domains.h>
#include <SOURCE/kbTypes.h>

// MapDirection bit sets: a move north (north-west, north, north-east) leaves
// past the object on its own cell; a move south (south-east, south,
// south-west) enters past the object on the next cell.
H2_ENUM_BEGIN(MapDirectionSet)
    MAP_DIRECTIONS_NORTHWARD = 0x83,
    MAP_DIRECTIONS_SOUTHWARD = 0x38
H2_ENUM_END(MapDirectionSet)

i32 CalcTerrainCost(
    H2_ENUM_PARAM(TerrainType, i32) terrain,
    i32 diagonal,
    i32 mobility,
    i32 pathfindingLevel,
    i32 sourceHasRoad,
    i32 destinationHasRoad
);

#endif // HOMM2_SOURCE_FINDPATH_H
