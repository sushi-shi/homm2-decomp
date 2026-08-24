#ifndef HOMM2_REQUEST_H
#define HOMM2_REQUEST_H

#include <Ints.h>
#include <SOURCE/GAME.h>

typedef enum RequestConstant {
    MAP_HEADER_SIZE                 = 0x1a4,
    MAP_HEADER_MAGIC_SIZE           = 4,
    MAP_HEADER_PLAYER_DATA_SIZE     = 0x12,
    MAP_HEADER_VICTORY_DATA_SIZE    = 4,
    MAP_HEADER_CONDITION_DATA_SIZE  = 0x17,
    MAP_HEADER_RESERVED_SIZE        = 8,
    MAP_HEADER_NAME_SIZE            = 0x3c,
    MAP_HEADER_DESCRIPTION_OFFSET   = 0x76,
    MAP_HEADER_DESCRIPTION_SIZE     = 300,
    MAP_HEADER_PLAYER_COUNT         = H2EnumIndex(GAME_PLAYER_COUNT),
    MAP_HEADER_MAGIC_BASE_GAME      = 90,
    MAP_HEADER_MAGIC_EXPANSION_GAME = 92
} RequestConstant;

typedef enum MapDimensionConstant {
    MAP_DIMENSION_SMALL  = 36,
    MAP_DIMENSION_MEDIUM = 72,
    MAP_DIMENSION_LARGE  = 108,
    MAP_DIMENSION_XLARGE = 144
} MapDimensionConstant;

enum class MapVictoryCondition : u8 {
    MAP_VICTORY_DEFEAT_ALL      = 0,
    MAP_VICTORY_CAPTURE_TOWN    = 1,
    MAP_VICTORY_DEFEAT_HERO     = 2,
    MAP_VICTORY_FIND_ARTIFACT   = 3,
    MAP_VICTORY_DEFEAT_SIDE     = 4,
    MAP_VICTORY_ACCUMULATE_GOLD = 5
};
using enum MapVictoryCondition;

enum class MapLossCondition : u8 {
    MAP_LOSS_STANDARD = 0,
    MAP_LOSS_TOWN     = 1,
    MAP_LOSS_HERO     = 2,
    MAP_LOSS_TIME     = 3
};
using enum MapLossCondition;

#pragma pack(push, 1)
struct SMapHeader {
    u32 magic;
    H2EnumStorage<GameDifficulty, u8> difficulty;
    u8 unknown5;
    u8 width;
    u8 height;
    u8 playerEnabled[MAP_HEADER_PLAYER_COUNT];
    u8 playerCanHuman[MAP_HEADER_PLAYER_COUNT];
    u8 playerCanComputer[MAP_HEADER_PLAYER_COUNT];
    u8 playerCount;
    u8 minHumanPlayers;
    u8 maxHumanPlayers;
    MapVictoryCondition victoryCondition;
    u8 computerAlsoWins;
    u8 allowNormalVictory;
    u16 victoryConditionValue;
    MapLossCondition lossCondition;
    u16 lossConditionValue;
    u8 unknown25;
    H2EnumStorage<FactionType, i8> playerRace[MAP_HEADER_PLAYER_COUNT];
    u16 victoryTownY;
    u16 lossTownY;
    u16 victorySideThreshold;
    u8 reserved32[MAP_HEADER_RESERVED_SIZE];
    char name[MAP_HEADER_NAME_SIZE];
    char description[MAP_HEADER_DESCRIPTION_SIZE];
    u8 rumourCount;
    u8 timeEventCount;
};
#pragma pack(pop)

i32 GetMapHeader(char*, struct SMapHeader*);
i32 CheckSumIsDemoOK(char*);
i32 ShowThisMapGame(char*);
i32 ShowThisMap(char*);

#endif
