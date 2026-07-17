#ifndef HOMM2_SOURCE_CONFIG_TYPES_H
#define HOMM2_SOURCE_CONFIG_TYPES_H

#include <va.h>

// gConfig - game/editor preferences persisted to the registry. Field names are the
// retail registry value names recovered from ReadPrefsFromRegistry/WritePrefsToRegistry.
#pragma pack(push, 1)
struct exeGfxConfig {
    int showMenu;
    int x;
    int y;
    int width;
    int height;
    int fullScreen;
    int colorMouseCursor;
};

typedef enum ConfigMusicSource {
    CONFIG_MUSIC_SOURCE_MIDI = 0,
    CONFIG_MUSIC_SOURCE_CD = 1,
    CONFIG_MUSIC_SOURCE_DEFAULT = 3
} ConfigMusicSource;

typedef enum ConfigOperaMode {
    CONFIG_OPERA_DISABLED = 0,
    CONFIG_OPERA_ENABLED = 1
} ConfigOperaMode;

typedef enum ConfigStorageConstant {
    CONFIG_EXECUTABLE_COUNT = 2,
    CONFIG_GRAPHICS_SIZE = 0x1c,
    CONFIG_PERSISTED_SIZE = 0x19d,
    CONFIG_STRUCT_SIZE = 0x1a0
} ConfigStorageConstant;

typedef enum ConfigConnectionType {
    CONFIG_CONNECTION_MODEM = 0,
    CONFIG_CONNECTION_DIRECT = 1,
    CONFIG_CONNECTION_COUNT = 2
} ConfigConnectionType;

struct configStruct {
    int computerWalkSpeed;
    int walkSpeed;
    int musicVolume;
    int soundVolume;
    int autosave;
    int showRoute;
    int blackoutComputer;
    exeGfxConfig gfx[CONFIG_EXECUTABLE_COUNT];
    int firstMapOffset;
    int currentMapOffset;
    int showObjectBoxes;
    int editorScreenAnimation;
    int editorPaletteCycling;
    int showCombatGrid;
    int showCombatMouseHex;
    int combatShadeLevel;
    int combatArmyInfoLevel;
    int evilInterfaceUsage;
    char autoLoadName[0xd];
    char autoSaveName[0xd];
    char mciError;
    char needsDefaultInitialization;
    char reservedAfterAutoSave[0x12];
    int musicSource;
    char modemInitString[0x64];
    int comPort[CONFIG_CONNECTION_COUNT];
    int baudRate[CONFIG_CONNECTION_COUNT];
    char uniqueSystemID[4];
    int useOpera;
    int quickCombatLevel;
    int combatSpeed;
    int autoCombatUseSpells;
    int slowVideo;
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
