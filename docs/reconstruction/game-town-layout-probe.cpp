// Compile-only owner/layout evidence, never linked into game code.
#include <SOURCE/game.h>
#include <stddef.h>
typedef char GameSize[sizeof(game)==0x660f ? 1 : -1];
typedef char TownRecordsOffset[offsetof(game,m_castleRecs)==0xb53 ? 1 : -1];
typedef char TownOwnerOffset[offsetof(game,m_townOwners)==0x2773 ? 1 : -1];
typedef char TownOwnerSize[sizeof(static_cast<game*>(0)->m_townOwners)==72 ? 1 : -1];
typedef char TownBuiltOffset[offsetof(game,m_townBuiltToday)==0x27bb ? 1 : -1];
typedef char TownBuiltSize[sizeof(static_cast<game*>(0)->m_townBuiltToday)==9 ? 1 : -1];
typedef char HeroRecordsOffset[offsetof(game,m_heroRecs)==0x27c4 ? 1 : -1];
void CheckTownOwnerType(i8&);
void CheckTownBuiltType(u8&);
void CheckTownTypes(game* state) {
    CheckTownOwnerType(state->m_townOwners[0]);
    CheckTownBuiltType(state->m_townBuiltToday[0]);
}
