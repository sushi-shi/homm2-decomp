// Compile-only layout evidence; never linked into the game.
#include <SOURCE/game.h>
#include <SOURCE/GAME.h>
#include <stddef.h>

typedef GameEventList<GAME_RUMOUR_EVENT_CAPACITY> RumourEvents;
typedef GameEventList<GAME_TIME_EVENT_CAPACITY> TimeEvents;
typedef GameEventList<GAME_MAP_EVENT_CAPACITY> MapEvents;
typedef char EventGameSize[sizeof(game) == 26127 ? 1 : -1];
typedef char RumourEventSize[sizeof(RumourEvents) == 62 ? 1 : -1];
typedef char TimeEventSize[sizeof(TimeEvents) == 102 ? 1 : -1];
typedef char MapEventSize[sizeof(MapEvents) == 102 ? 1 : -1];
typedef char RumourEventOffset[offsetof(game, m_rumourEvents) == 25815 ? 1 : -1];
typedef char TimeEventOffset[offsetof(game, m_timeEvents) == 25877 ? 1 : -1];
typedef char MapEventOffset[offsetof(game, m_mapEvents) == 25979 ? 1 : -1];
typedef char RumourIndexOffset[offsetof(RumourEvents, indices) == 2 ? 1 : -1];
typedef char TimeIndexOffset[offsetof(TimeEvents, indices) == 2 ? 1 : -1];
typedef char MapIndexOffset[offsetof(MapEvents, indices) == 2 ? 1 : -1];
typedef char EventFollowingOwner[offsetof(game, m_viewArmyWindow) == 26081 ? 1 : -1];
typedef char EventFollowingResult[offsetof(game, m_viewArmyResult) == 26085 ? 1 : -1];

void CheckEventWord(u16&);
void CheckEventOwners(game* state) {
    CheckEventWord(state->m_rumourEvents.count);
    CheckEventWord(state->m_rumourEvents.indices[0]);
    CheckEventWord(state->m_timeEvents.count);
    CheckEventWord(state->m_timeEvents.indices[0]);
    CheckEventWord(state->m_mapEvents.count);
    CheckEventWord(state->m_mapEvents.indices[0]);
}
