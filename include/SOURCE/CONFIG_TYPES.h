#ifndef HOMM2_SOURCE_CONFIG_TYPES_H
#define HOMM2_SOURCE_CONFIG_TYPES_H

#include <Ints.h>

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

enum class ConfigWalkSpeed : i32 {
    CONFIG_WALK_SPEED_SLOWEST = 0,
    CONFIG_WALK_SPEED_SLOW    = 1,
    CONFIG_WALK_SPEED_NORMAL  = 2,
    CONFIG_WALK_SPEED_FAST    = 3,
    CONFIG_WALK_SPEED_INSTANT = 4,
    CONFIG_WALK_SPEED_COUNT   = 5
};
using enum ConfigWalkSpeed;
ENABLE_ENUM_STEPS(ConfigWalkSpeed)

enum class ConfigVolumeLevel : i32 {
    CONFIG_VOLUME_MUTED       = 0,
    CONFIG_VOLUME_MIN         = 1,
    CONFIG_VOLUME_MAX         = 10,
    CONFIG_VOLUME_LEVEL_COUNT = CONFIG_VOLUME_MAX + 1
};
using enum ConfigVolumeLevel;
ENABLE_ENUM_STEPS(ConfigVolumeLevel)

enum class ConfigExecutable : i32 {
    CONFIG_EXECUTABLE_GAME   = 0,
    CONFIG_EXECUTABLE_EDITOR = 1,
    CONFIG_EXECUTABLE_COUNT  = 2
};
using enum ConfigExecutable;

typedef enum ConfigStorageConstant {
    CONFIG_GRAPHICS_SIZE             = 0x1c,
    CONFIG_PERSISTED_SIZE            = 0x19d,
    CONFIG_STRUCT_SIZE               = 0x1a0,
    CONFIG_SAVE_NAME_SIZE            = 0xd,
    CONFIG_AUTOSAVE_RESERVED_SIZE    = 0x12,
    CONFIG_MODEM_INIT_STRING_SIZE    = 0x64,
    CONFIG_UNIQUE_SYSTEM_ID_SIZE     = 4,
    CONFIG_NETWORK_DEFAULT_NAME_SIZE = 0x18
} ConfigStorageConstant;

enum class ConfigConnectionType : i32 {
    CONFIG_CONNECTION_MODEM  = 0,
    CONFIG_CONNECTION_DIRECT = 1,
    CONFIG_CONNECTION_COUNT  = 2
};
using enum ConfigConnectionType;

enum class ConfigComPort : i32 {
    CONFIG_COM_PORT_UNCONFIGURED = 0,
    CONFIG_COM_PORT_1            = 1,
    CONFIG_COM_PORT_2            = 2,
    CONFIG_COM_PORT_3            = 3,
    CONFIG_COM_PORT_4            = 4
};
using enum ConfigComPort;

enum class ConfigBaudRate : i32 {
    CONFIG_BAUD_2400  = 2400,
    CONFIG_BAUD_9600  = 9600,
    CONFIG_BAUD_19200 = 19200,
    CONFIG_BAUD_38400 = 38400
};
using enum ConfigBaudRate;

struct configStruct {
    ConfigWalkSpeed computerWalkSpeed;
    ConfigWalkSpeed walkSpeed;
    ConfigVolumeLevel musicVolume;
    ConfigVolumeLevel soundVolume;
    i32 autosave;
    i32 showRoute;
    i32 blackoutComputer;
    exeGfxConfig gfx[H2EnumIndex(CONFIG_EXECUTABLE_COUNT)];
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
    char reservedConfigByte;
    char needsDefaultInitialization;
    char reservedAfterAutoSave[CONFIG_AUTOSAVE_RESERVED_SIZE];
    i32 reservedConfigWord1;
    char modemInitString[CONFIG_MODEM_INIT_STRING_SIZE];
    H2EnumStorage<ConfigComPort, i32> comPort[H2EnumIndex(CONFIG_CONNECTION_COUNT)];
    H2EnumStorage<ConfigBaudRate, i32> baudRate[H2EnumIndex(CONFIG_CONNECTION_COUNT)];
    char uniqueSystemID[CONFIG_UNIQUE_SYSTEM_ID_SIZE];
    i32 reservedConfigWord2;
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

#endif
