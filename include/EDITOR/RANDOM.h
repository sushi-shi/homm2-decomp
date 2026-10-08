#ifndef HOMM2_EDITOR_RANDOM_H
#define HOMM2_EDITOR_RANDOM_H

// The random map generator (src/EDITOR/RANDOM.cpp, assertion path
// Editor\RANDOM.CPP): editManager methods that paint terrain, lay mountain
// and tree chains and place towns, objects and treasure.

#include <H2/Ints.h>

struct overlayType;

// A cell offset or position.
struct mapStep {
    i32 x;
    i32 y;
};

// Places the object type with its anchor on cell (x, y); the second form
// asks whether it fits there (over other objects' parts with overObjects).
b32 PlaceOverlayAt(overlayType* type, i32 x, i32 y);
b32 CanPlaceOverlayAt(overlayType* type, i32 x, i32 y, b32 overObjects);
// Scales a count of objects by a density percent: unchanged at 50, halved
// near 0, doubled at 100.
void ScaleByDensity(i32* count, i32 density);

// The generator's running state.
extern b32 gGeneratingRandomMap;

#endif
