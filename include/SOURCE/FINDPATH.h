#ifndef HOMM2_FINDPATH_H
#define HOMM2_FINDPATH_H

#include <Ints.h>
#include <SOURCE/KB_TYPES.h>


typedef enum MapDirectionSet {
    MAP_DIRECTIONS_NORTHWARD = 0x83,
    MAP_DIRECTIONS_SOUTHWARD = 0x38
} MapDirectionSet;

i32 CalcTerrainCost(
    TerrainType terrain,
    i32 diagonal,
    i32 mobility,
    i32 pathfindingLevel,
    i32 sourceHasRoad,
    i32 destinationHasRoad
);

#endif
