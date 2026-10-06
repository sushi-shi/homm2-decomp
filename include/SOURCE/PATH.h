#ifndef HOMM2_PATH_H
#define HOMM2_PATH_H

#include <SOURCE/combatTypes.h>

i32 GetAdjacentCellIndexNoArmy(i32 sourceHex, CombatHexDirection direction);
CombatHexDirection OppositeDirection(CombatHexDirection direction);

#endif
