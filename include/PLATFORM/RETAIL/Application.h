#ifndef HOMM2_PLATFORM_WIN32_APPLICATION_H
#define HOMM2_PLATFORM_WIN32_APPLICATION_H

#include <SOURCE/KB_TYPES.h>
#include <Ints.h>

typedef enum KbWinConstant {
    KBWIN_COMMAND_LINE_LIMIT      = 0x3c,
    KBWIN_COMMAND_LINE_CLEAR_SIZE = KBWIN_COMMAND_LINE_LIMIT + 1,
    KBWIN_MIN_WIDTH               = 240,
    KBWIN_MIN_HEIGHT              = 160,
    KBWIN_WINDOWED_STYLE          = 0x14cf0000,
    KBWIN_FULLSCREEN_STYLE        = 0x14000000,
    KBWIN_MENU_ENTRY_COUNT        = MENU_ENABLE_STATUS_COUNT,
    KBWIN_WIDTH_640               = 640,
    KBWIN_HEIGHT_480              = 480,
    KBWIN_WIDTH_800               = 800,
    KBWIN_HEIGHT_600              = 600,
    KBWIN_WIDTH_1024              = 1024,
    KBWIN_HEIGHT_768              = 768,
    KBWIN_WIDTH_1280              = 1280,
    KBWIN_HEIGHT_1024             = 1024
} KbWinConstant;

enum class KbWinMenuCommand : i32 {
    KBWIN_MENU_SIZE_640_480   = 0x9c45,
    KBWIN_MENU_SIZE_800_600   = 0x9c46,
    KBWIN_MENU_SIZE_1024_768  = 0x9c47,
    KBWIN_MENU_SIZE_1280_1024 = 0x9c48,
    KBWIN_MENU_FULLSCREEN     = 0x9c49,
    KBWIN_MENU_HELP           = 0x9c74,
    KBWIN_MENU_ABOUT          = 0x9c75
};
using enum KbWinMenuCommand;

void ResizeWindow(i32, i32, i32, i32);
i32l KBTickCount(void);

extern i32 giTotalMemAllocated;
extern u8 giChangeThreshold[];
extern i32 iMainWinScreenHeight;
extern i32 iMainWinScreenWidth;

#endif
