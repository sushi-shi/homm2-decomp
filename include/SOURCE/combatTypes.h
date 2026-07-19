#ifndef HOMM2_SOURCE_COMBATTYPES_H
#define HOMM2_SOURCE_COMBATTYPES_H

#include <va.h>

H2_ENUM_BEGIN(ArmyDrawState)
    ARMY_DRAW_BEHIND      = 0,
    ARMY_DRAW_NORMAL      = 1,
    ARMY_DRAW_EFFECT      = 2,
    ARMY_DRAW_IN_FRONT    = 3,
    ARMY_DRAW_PHASE_COUNT = 4,
    ARMY_DRAW_ALL         = 100
H2_ENUM_END(ArmyDrawState)

#endif
