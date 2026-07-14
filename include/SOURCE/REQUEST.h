#ifndef HOMM2_REQUEST_H
#define HOMM2_REQUEST_H
// Declarations of the free functions DEFINED in REQUEST.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).
// forward declarations (was <_all.h>):

#include <va.h>

enum {
    MAP_HEADER_SIZE = 0x1a4,
    MAP_HEADER_MAGIC_SIZE = 4,
    MAP_HEADER_PLAYER_DATA_SIZE = 0x12,
    MAP_HEADER_VICTORY_DATA_SIZE = 4,
    MAP_HEADER_CONDITION_DATA_SIZE = 0x17,
    MAP_HEADER_NAME_SIZE = 0x3c,
    MAP_HEADER_DESCRIPTION_OFFSET = 0x76
};

typedef enum MapDimension {
    MAP_DIMENSION_SMALL = 36,
    MAP_DIMENSION_MEDIUM = 72,
    MAP_DIMENSION_LARGE = 108,
    MAP_DIMENSION_XLARGE = 144
} MapDimension;

#pragma pack(push, 1)
struct SMapHeader {
    unsigned char magic[MAP_HEADER_MAGIC_SIZE];
    unsigned char difficulty;
    unsigned char unknown5;
    unsigned char mapSize;
    unsigned char height;
    unsigned char playerData[MAP_HEADER_PLAYER_DATA_SIZE];
    unsigned char playerCount;
    unsigned char minHumanPlayers;
    unsigned char maxHumanPlayers;
    unsigned char victoryCondition;
    unsigned char victoryData[MAP_HEADER_VICTORY_DATA_SIZE];
    unsigned char lossCondition;
    unsigned char conditionData[MAP_HEADER_CONDITION_DATA_SIZE];
    char name[MAP_HEADER_NAME_SIZE];
    char description[MAP_HEADER_SIZE - MAP_HEADER_DESCRIPTION_OFFSET];
};
#pragma pack(pop)
SIZE(SMapHeader, MAP_HEADER_SIZE);

int GetMapHeader(char *, struct SMapHeader *);
int CheckSumIsDemoOK(char *);
int ShowThisMapGame(char *);
int ShowThisMap(char *);

#endif // HOMM2_REQUEST_H
