#ifndef HOMM2_SOURCE_CONFIG_TYPES_H
#define HOMM2_SOURCE_CONFIG_TYPES_H

#include <va.h>

#pragma pack(push, 1)
struct exeGfxConfig {
    i32 showMenu;
    i32 x;
    i32 y;
    i32 width;
    i32 height;
    i32 fullScreen;
    i32 colorMouseCursor;
};

H2_ENUM_CLASS_BEGIN(ConfigMusicSource)
    CONFIG_MUSIC_SOURCE_MIDI    = 0,
    CONFIG_MUSIC_SOURCE_CD      = 1,
    CONFIG_MUSIC_SOURCE_DEFAULT = 3
H2_ENUM_CLASS_END(ConfigMusicSource)

H2_ENUM_CLASS_BEGIN(ConfigOperaMode)
    CONFIG_OPERA_DISABLED = 0,
    CONFIG_OPERA_ENABLED  = 1
H2_ENUM_CLASS_END(ConfigOperaMode)

H2_ENUM_CLASS_BEGIN(ConfigExecutable)
    CONFIG_EXECUTABLE_GAME   = 0,
    CONFIG_EXECUTABLE_EDITOR = 1,
    CONFIG_EXECUTABLE_COUNT  = 2
H2_ENUM_CLASS_END(ConfigExecutable)

H2_ENUM_BEGIN(ConfigStorageConstant)
    CONFIG_GRAPHICS_SIZE             = 0x1c,
    CONFIG_PERSISTED_SIZE            = 0x19d,
    CONFIG_STRUCT_SIZE               = 0x1a0,
    CONFIG_SAVE_NAME_SIZE            = 0xd,
    CONFIG_AUTOSAVE_RESERVED_SIZE    = 0x12,
    CONFIG_MODEM_INIT_STRING_SIZE    = 0x64,
    CONFIG_UNIQUE_SYSTEM_ID_SIZE     = 4,
    CONFIG_NETWORK_DEFAULT_NAME_SIZE = 0x18
H2_ENUM_END(ConfigStorageConstant)

H2_ENUM_CLASS_BEGIN(ConfigConnectionType)
    CONFIG_CONNECTION_MODEM  = 0,
    CONFIG_CONNECTION_DIRECT = 1,
    CONFIG_CONNECTION_COUNT  = 2
H2_ENUM_CLASS_END(ConfigConnectionType)

struct configStruct {
    i32 computerWalkSpeed;
    i32 walkSpeed;
    i32 musicVolume;
    i32 soundVolume;
    i32 autosave;
    i32 showRoute;
    i32 blackoutComputer;
    exeGfxConfig gfx[IDX(CONFIG_EXECUTABLE_COUNT)];
    i32 firstMapOffset;
    i32 currentMapOffset;
    i32 showObjectBoxes;
    i32 editorScreenAnimation;
    i32 editorPaletteCycling;
    i32 showCombatGrid;
    i32 showCombatMouseHex;
    i32 combatShadeLevel;
    i32 combatArmyInfoLevel;
    i32 evilInterfaceUsage;
    char autoLoadName[CONFIG_SAVE_NAME_SIZE];
    char autoSaveName[CONFIG_SAVE_NAME_SIZE];
    char mciError;
    char needsDefaultInitialization;
    char reservedAfterAutoSave[CONFIG_AUTOSAVE_RESERVED_SIZE];
    ConfigMusicSource musicSource;
    char modemInitString[CONFIG_MODEM_INIT_STRING_SIZE];
    i32 comPort[IDX(CONFIG_CONNECTION_COUNT)];
    i32 baudRate[IDX(CONFIG_CONNECTION_COUNT)];
    char uniqueSystemID[CONFIG_UNIQUE_SYSTEM_ID_SIZE];
    i32 useOpera;
    i32 quickCombatLevel;
    i32 combatSpeed;
    i32 autoCombatUseSpells;
    i32 slowVideo;
    char rmtRLName[CONFIG_SAVE_NAME_SIZE];
    char rmtRCName[CONFIG_SAVE_NAME_SIZE];
    char rmtRDName[CONFIG_SAVE_NAME_SIZE];
    char rmtSLName[CONFIG_SAVE_NAME_SIZE];
    char rmtSCName[CONFIG_SAVE_NAME_SIZE];
    char rmtSDName[CONFIG_SAVE_NAME_SIZE];
    char networkDefaultName[CONFIG_NETWORK_DEFAULT_NAME_SIZE];
};
#pragma pack(pop)
SIZE(exeGfxConfig, CONFIG_GRAPHICS_SIZE);
SIZE(configStruct, CONFIG_STRUCT_SIZE);

#endif
