#ifndef HOMM2_EVENTS_TYPES_H
#define HOMM2_EVENTS_TYPES_H

#include <va.h>
#include <SOURCE/EVENTS.h>
#include <SOURCE/armyGroup.h>
#include <SOURCE/town.h>

#pragma pack(push, 1)
struct combatRemoteData {
    signed char fragment;
    signed char x;
    signed char y;
    signed char hasFirstHero;
    signed char hasTown;
    signed char hasSecondHero;
    signed char firstSide;
    signed char secondSide;
    int randomSeed;
    signed char combatResult;
    signed char retreatWin;
    signed char combatSurrender;
    signed char firstOwner;
    int firstGold;
    signed char secondOwner;
    int secondGold;
    armyGroup firstArmy;
    armyGroup secondArmy;
    town combatTown;
};

struct combatRemoteHeroFragment {
    signed char fragment;
    unsigned char data[COMBAT_REMOTE_HERO_FIRST_SIZE];
};

#pragma pack(pop)

SIZE(combatRemoteData, COMBAT_REMOTE_HEADER_SIZE);
SIZE(combatRemoteHeroFragment, COMBAT_REMOTE_HERO_FIRST_SIZE + 1);

#endif // HOMM2_EVENTS_TYPES_H
