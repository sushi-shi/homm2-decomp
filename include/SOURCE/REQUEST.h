#ifndef HOMM2_REQUEST_H
#define HOMM2_REQUEST_H
// Declarations of the free functions DEFINED in REQUEST.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).
// forward declarations (was <_all.h>):

#include <va.h>

enum {
    MAP_HEADER_SIZE = 0x1a4
};

#pragma pack(push, 1)
struct SMapHeader {
    unsigned char magicAndDifficulty[6];
    unsigned char mapSize;
    unsigned char height;
    unsigned char playerData[0x13];
    unsigned char minHumanPlayers;
    unsigned char maxHumanPlayers;
    unsigned char mapData[MAP_HEADER_SIZE - 0x1d];
};
#pragma pack(pop)
SIZE(SMapHeader, MAP_HEADER_SIZE);

int GetMapHeader(char *, struct SMapHeader *);
int CheckSumIsDemoOK(char *);
int ShowThisMapGame(char *);
int ShowThisMap(char *);

#endif // HOMM2_REQUEST_H
