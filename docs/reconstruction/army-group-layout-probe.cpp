// U06: compile-only layout assertions, never linked into the game.
#include <SOURCE/armyGroup.h>
#include <SOURCE/hero.h>
#include <SOURCE/town.h>
#include <stddef.h>

typedef char ArmyGroupSize[sizeof(armyGroup) == 15 ? 1 : -1];
typedef char ArmyGroupTypes[offsetof(armyGroup, m_creatureTypes) == 0 ? 1 : -1];
typedef char ArmyGroupTypeSize[sizeof(static_cast<armyGroup*>(0)->m_creatureTypes) == 5 ? 1 : -1];
typedef char ArmyGroupCounts[offsetof(armyGroup, m_creatureCounts) == 5 ? 1 : -1];
typedef char ArmyGroupCountSize[sizeof(static_cast<armyGroup*>(0)->m_creatureCounts) == 10 ? 1 : -1];
typedef char ArmyHeroSize[sizeof(hero) == 250 ? 1 : -1];
typedef char ArmyTownSize[sizeof(town) == 100 ? 1 : -1];
