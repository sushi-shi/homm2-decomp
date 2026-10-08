#ifndef HOMM2_BASE_MISCENUMS_H
#define HOMM2_BASE_MISCENUMS_H

#include <Domains.h>

enum class CDRomSetupResult : i32 {
    CD_ROM_READY                  = 0,
    CD_ROM_DRIVE_UNAVAILABLE      = 1,
    CD_ROM_EXPANSION_DISC_MISSING = 2,
    CD_ROM_GAME_DIRECTORY_MISSING = 3,
    CD_ROM_DATA_FILES_MISSING     = 4
};
using enum CDRomSetupResult;

#endif
