// Compile-only owner/type assertions; this object is never linked into the game.
#include <SOURCE/GAME.h>
#include <SOURCE/KB.h>
#include <SOURCE/game.h>
#include <SOURCE/hero.h>
#include <SOURCE/playerData.h>
#include <SOURCE/armyGroup.h>
#include <SOURCE/kbwin.h>

typedef char TownHandlerIntWidth[(sizeof(i32) == 4) ? 1 : -1];
typedef char TownHandlerLongWidth[(sizeof(i32l) == 4) ? 1 : -1];
typedef char TownHandlerTickWidth[(sizeof(KBTickCount()) == 4) ? 1 : -1];
typedef char TownHandlerSignedInt[(i32(-1) < 0) ? 1 : -1];
typedef char TownHandlerSignedLong[(i32l(-1) < 0) ? 1 : -1];
typedef char TownHandlerArmySize[(sizeof(armyGroup) == 15) ? 1 : -1];

void TownHandlerByteOwner(i8&);
void TownHandlerWordOwner(i16&);
void TownHandlerTimerOwner(i32&);
void TownHandlerCheckOwners(game* state, hero* currentHero, playerData* player, armyGroup* group) {
    TownHandlerByteOwner(currentHero->m_owner);
    TownHandlerByteOwner(state->m_availableHeroes[0]);
    TownHandlerByteOwner(player->m_availableHeroIds[0]);
    TownHandlerWordOwner(group->m_creatureCounts[0]);
    TownHandlerTimerOwner(glTimers[0]);
}
