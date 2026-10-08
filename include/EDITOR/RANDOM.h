#ifndef HOMM2_EDITOR_RANDOM_H
#define HOMM2_EDITOR_RANDOM_H


#include <H2/Ints.h>

struct overlayType;


struct mapStep {
    i32 x;
    i32 y;
};


b32 PlaceOverlayAt(overlayType* type, i32 x, i32 y);
b32 CanPlaceOverlayAt(overlayType* type, i32 x, i32 y, b32 overObjects);


void ScaleByDensity(i32* count, i32 density);


extern b32 gGeneratingRandomMap;

#endif
