#ifndef HOMM2_MISC_ENUMS_H
#define HOMM2_MISC_ENUMS_H

#include <Ints.h>

enum class GameDataStatus : i32 {
    GAME_DATA_READY   = 0,
    GAME_DATA_MISSING = 1
};
using enum GameDataStatus;

#endif
