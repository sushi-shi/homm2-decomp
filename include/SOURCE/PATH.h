#ifndef HOMM2_PATH_H
#define HOMM2_PATH_H

#include <SOURCE/combatTypes.h>

class searchArray;

extern searchArray SVSearchArray;

i32 GetAdjacentCellIndexNoArmy(i32, CombatHexDirection);
CombatHexDirection OppositeDirection(CombatHexDirection);

#endif
