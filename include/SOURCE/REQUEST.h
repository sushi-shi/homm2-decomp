#ifndef HOMM2_REQUEST_H
#define HOMM2_REQUEST_H

#include <va.h>
#include <SOURCE/GAME.h>

H2_ENUM_BEGIN(RequestConstant)
    MAP_HEADER_SIZE                 = 0x1a4,
    MAP_HEADER_MAGIC_SIZE           = 4,
    MAP_HEADER_PLAYER_DATA_SIZE     = 0x12,
    MAP_HEADER_VICTORY_DATA_SIZE    = 4,
    MAP_HEADER_CONDITION_DATA_SIZE  = 0x17,
    MAP_HEADER_NAME_SIZE            = 0x3c,
    MAP_HEADER_DESCRIPTION_OFFSET   = 0x76,
    MAP_HEADER_DESCRIPTION_SIZE     = 300,
    MAP_HEADER_PLAYER_COUNT         = IDX(GAME_PLAYER_COUNT),
    MAP_HEADER_MAGIC_BASE_GAME      = 90,
    MAP_HEADER_MAGIC_EXPANSION_GAME = 92
H2_ENUM_END(RequestConstant)

H2_ENUM_CLASS_BEGIN(MapDimension)
    MAP_DIMENSION_SMALL  = 36,
    MAP_DIMENSION_MEDIUM = 72,
    MAP_DIMENSION_LARGE  = 108,
    MAP_DIMENSION_XLARGE = 144
H2_ENUM_CLASS_END(MapDimension)

H2_ENUM_CLASS_BEGIN(MapVictoryCondition)
    MAP_VICTORY_DEFEAT_ALL      = 0,
    MAP_VICTORY_CAPTURE_TOWN    = 1,
    MAP_VICTORY_DEFEAT_HERO     = 2,
    MAP_VICTORY_FIND_ARTIFACT   = 3,
    MAP_VICTORY_DEFEAT_SIDE     = 4,
    MAP_VICTORY_ACCUMULATE_GOLD = 5
H2_ENUM_CLASS_END(MapVictoryCondition)

H2_ENUM_CLASS_BEGIN(MapLossCondition)
    MAP_LOSS_STANDARD = 0,
    MAP_LOSS_TOWN     = 1,
    MAP_LOSS_HERO     = 2,
    MAP_LOSS_TIME     = 3
H2_ENUM_CLASS_END(MapLossCondition)

#pragma pack(push, 1)
struct SMapHeader {
    u32 magic;
    u8 difficulty;
    u8 unknown5;
    u8 width;
    u8 height;
    u8 playerEnabled[MAP_HEADER_PLAYER_COUNT];
    u8 playerCanHuman[MAP_HEADER_PLAYER_COUNT];
    u8 playerCanComputer[MAP_HEADER_PLAYER_COUNT];
    u8 playerCount;
    u8 minHumanPlayers;
    u8 maxHumanPlayers;
    u8 victoryCondition;
    u8 computerAlsoWins;
    u8 allowNormalVictory;
    u16 victoryConditionValue;
    u8 lossCondition;
    u16 lossConditionValue;
    u8 unknown25;
    i8 playerRace[MAP_HEADER_PLAYER_COUNT];
    u16 victoryTownY;
    u16 lossTownY;
    u16 victorySideThreshold;
    u8 reserved32[8];
    char name[MAP_HEADER_NAME_SIZE];
    char description[MAP_HEADER_DESCRIPTION_SIZE];
    u8 rumourCount;
    u8 timeEventCount;
};
#pragma pack(pop)
SIZE(SMapHeader, MAP_HEADER_SIZE);

i32 GetMapHeader(char*, struct SMapHeader*);
i32 CheckSumIsDemoOK(char*);
i32 ShowThisMapGame(char*);
i32 ShowThisMap(char*);

#endif
