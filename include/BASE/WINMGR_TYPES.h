#ifndef HOMM2_BASE_WINMGR_TYPES_H
#define HOMM2_BASE_WINMGR_TYPES_H

#include <va.h>

HOMM2_ENUM_VALUES_BEGIN(WindowManagerTextSlotSize)
    WINDOW_MANAGER_NAME_SLOT_SIZE = 0x14,
    WINDOW_MANAGER_FORMAT_SLOT_SIZE = 0x10,
    WINDOW_MANAGER_SOURCE_FILE_SLOT_SIZE = 0x28,
    WINDOW_MANAGER_FINAL_SOURCE_FILE_SLOT_SIZE = 0x2c
HOMM2_ENUM_VALUES_END(WindowManagerTextSlotSize)

typedef struct SWindowManagerText {
    char managerName[WINDOW_MANAGER_NAME_SLOT_SIZE];
    char screenshotFormat[WINDOW_MANAGER_FORMAT_SLOT_SIZE];
    char fadePaletteAllocSource[WINDOW_MANAGER_SOURCE_FILE_SLOT_SIZE];
    char cycleTableAllocSource[WINDOW_MANAGER_SOURCE_FILE_SLOT_SIZE];
    char cycleFilenameFormat[WINDOW_MANAGER_FORMAT_SLOT_SIZE];
    char cycleTableFreeSource[WINDOW_MANAGER_SOURCE_FILE_SLOT_SIZE];
    char fadePaletteFreeSource[WINDOW_MANAGER_FINAL_SOURCE_FILE_SLOT_SIZE];
} SWindowManagerText;

SIZE(SWindowManagerText, 0xd8);

#endif
