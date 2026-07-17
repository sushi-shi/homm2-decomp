#ifndef HOMM2_EVENTS_TYPES_H
#define HOMM2_EVENTS_TYPES_H

#include <va.h>
#include <SOURCE/EVENTS.h>
#include <SOURCE/armyGroup.h>
#include <SOURCE/town.h>

#pragma pack(push, 1)
struct combatRemoteData {
    i8 fragment;
    i8 x;
    i8 y;
    i8 hasFirstHero;
    i8 hasTown;
    i8 hasSecondHero;
    i8 firstSide;
    i8 secondSide;
    i32 randomSeed;
    i8 combatResult;
    i8 retreatWin;
    i8 combatSurrender;
    i8 firstOwner;
    i32 firstGold;
    i8 secondOwner;
    i32 secondGold;
    armyGroup firstArmy;
    armyGroup secondArmy;
    town combatTown;
};

struct combatRemoteHeroFragment {
    i8 fragment;
    u8 data[COMBAT_REMOTE_HERO_FIRST_SIZE];
};

#pragma pack(pop)

SIZE(combatRemoteData, COMBAT_REMOTE_HEADER_SIZE);
SIZE(combatRemoteHeroFragment, COMBAT_REMOTE_HERO_FIRST_SIZE + 1);

#endif // HOMM2_EVENTS_TYPES_H
