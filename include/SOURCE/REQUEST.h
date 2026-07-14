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
    MAP_HEADER_DESCRIPTION_OFFSET = 0x76,
    MAP_HEADER_DESCRIPTION_SIZE = 300,
    MAP_HEADER_PLAYER_COUNT = 6,
    MAP_HEADER_MAGIC_BASE_GAME = 90,
    MAP_HEADER_MAGIC_EXPANSION_GAME = 92
};

typedef enum MapDimension {
    MAP_DIMENSION_SMALL = 36,
    MAP_DIMENSION_MEDIUM = 72,
    MAP_DIMENSION_LARGE = 108,
    MAP_DIMENSION_XLARGE = 144
} MapDimension;

#pragma pack(push, 1)
struct SMapHeader {
    unsigned int magic;
    unsigned char difficulty;
    unsigned char unknown5;
    unsigned char width;
    unsigned char height;
    unsigned char playerEnabled[MAP_HEADER_PLAYER_COUNT];
    unsigned char playerCanHuman[MAP_HEADER_PLAYER_COUNT];
    unsigned char playerCanComputer[MAP_HEADER_PLAYER_COUNT];
    unsigned char playerCount;
    unsigned char minHumanPlayers;
    unsigned char maxHumanPlayers;
    unsigned char victoryCondition;
    unsigned char computerAlsoWins;
    unsigned char allowNormalVictory;
    unsigned short victoryConditionValue;
    unsigned char lossCondition;
    unsigned short lossConditionValue;
    unsigned char unknown25;
    signed char playerRace[MAP_HEADER_PLAYER_COUNT];
    unsigned short victoryTownY;
    unsigned short lossTownY;
    unsigned short victorySideThreshold;
    unsigned char reserved32[8];
    char name[MAP_HEADER_NAME_SIZE];
    char description[MAP_HEADER_DESCRIPTION_SIZE];
    unsigned char rumourCount;
    unsigned char timeEventCount;
};
#pragma pack(pop)
SIZE(SMapHeader, MAP_HEADER_SIZE);

int GetMapHeader(char *, struct SMapHeader *);
int CheckSumIsDemoOK(char *);
int ShowThisMapGame(char *);
int ShowThisMap(char *);

#endif // HOMM2_REQUEST_H
