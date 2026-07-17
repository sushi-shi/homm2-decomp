#ifndef HOMM2_SOURCE_CONFIG_TYPES_H
#define HOMM2_SOURCE_CONFIG_TYPES_H

#include <va.h>

// gConfig - game/editor preferences persisted to the registry. Field names are the
// retail registry value names recovered from ReadPrefsFromRegistry/WritePrefsToRegistry.
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

HOMM2_ENUM_BEGIN(ConfigMusicSource)
    CONFIG_MUSIC_SOURCE_MIDI = 0,
    CONFIG_MUSIC_SOURCE_CD = 1,
    CONFIG_MUSIC_SOURCE_DEFAULT = 3
HOMM2_ENUM_END(ConfigMusicSource)

HOMM2_ENUM_BEGIN(ConfigOperaMode)
    CONFIG_OPERA_DISABLED = 0,
    CONFIG_OPERA_ENABLED = 1
HOMM2_ENUM_END(ConfigOperaMode)

HOMM2_ENUM_VALUES_BEGIN(ConfigStorageConstant)
    CONFIG_EXECUTABLE_COUNT = 2,
    CONFIG_GRAPHICS_SIZE = 0x1c,
    CONFIG_PERSISTED_SIZE = 0x19d,
    CONFIG_STRUCT_SIZE = 0x1a0
HOMM2_ENUM_VALUES_END(ConfigStorageConstant)

HOMM2_ENUM_BEGIN(ConfigConnectionType)
    CONFIG_CONNECTION_MODEM = 0,
    CONFIG_CONNECTION_DIRECT = 1,
    CONFIG_CONNECTION_COUNT = 2
HOMM2_ENUM_END(ConfigConnectionType)

struct configStruct {
    i32 computerWalkSpeed;
    i32 walkSpeed;
    i32 musicVolume;
    i32 soundVolume;
    i32 autosave;
    i32 showRoute;
    i32 blackoutComputer;
    exeGfxConfig gfx[CONFIG_EXECUTABLE_COUNT];
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
    char autoLoadName[0xd];
    char autoSaveName[0xd];
    char mciError;
    char needsDefaultInitialization;
    char reservedAfterAutoSave[0x12];
    i32 musicSource;
    char modemInitString[0x64];
    i32 comPort[IDX(CONFIG_CONNECTION_COUNT)];
    i32 baudRate[IDX(CONFIG_CONNECTION_COUNT)];
    char uniqueSystemID[4];
    i32 useOpera;
    i32 quickCombatLevel;
    i32 combatSpeed;
    i32 autoCombatUseSpells;
    i32 slowVideo;
    char rmtRLName[0xd];
    char rmtRCName[0xd];
    char rmtRDName[0xd];
    char rmtSLName[0xd];
    char rmtSCName[0xd];
    char rmtSDName[0xd];
    char networkDefaultName[0x18];
};
#pragma pack(pop)
SIZE(exeGfxConfig, CONFIG_GRAPHICS_SIZE);
SIZE(configStruct, CONFIG_STRUCT_SIZE);

#endif // HOMM2_SOURCE_CONFIG_TYPES_H
