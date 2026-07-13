#ifndef HOMM2_TYPES_H
#define HOMM2_TYPES_H
// Hand-maintained shared home for record/model structs that GLOBAL declarations reference,
// so their layout is declared once and cannot drift between TUs. TU-PRIVATE modelling
// structs do NOT belong here — they live in per-TU headers (they often need TU-local
// typedefs). Move each type below to its owning module header as ownership becomes clear.
// Included by _globals.h.
#include <Ints.h>
// forward declarations (was <_all.h>):
class sample;

// Resource-type index for the game's per-resource arrays (gafAITurnCostResource, p[] amounts,
// etc.), in the standard HoMM2 order. MERCURY/ORE/CRYSTAL are confirmed by the retail's
// per-element .rdata float symbols referenced in philAI::RVConversion; the rest follow the
// canonical order.
enum { RES_WOOD, RES_MERCURY, RES_ORE, RES_SULFUR, RES_CRYSTAL, RES_GEMS, RES_GOLD };
typedef enum MonsterFlags {
    MONSTER_FLAGS_WIDE = 0x00010000,
    MONSTER_FLAGS_FLYING = 0x00020000,
    MONSTER_FLAGS_LIGHT_PALETTE = 0x00000100,
    MONSTER_FLAGS_RED_PALETTE = 0x00000200,
    MONSTER_FLAGS_DARK_BROWN_PALETTE = 0x00002000,
    MONSTER_FLAGS_GRAY_PALETTE = 0x00004000,
    MONSTER_FLAGS_SHOOTER = 0x00040000,
    MONSTER_FLAGS_BREATH_ATTACK = 0x00080000,
    MONSTER_FLAGS_DEAD = 0x00100000,
    MONSTER_FLAGS_AI_EXCLUDED = 0x00100000,
    MONSTER_FLAGS_RETALIATED = 0x00400000,
    MONSTER_FLAGS_WOKE_FROM_DAMAGE = 0x00800000,
    MONSTER_FLAGS_FULL_AI_QUANTITY = 0x00800000,
    MONSTER_FLAGS_MIRROR_IMAGE = 0x01000000,
    MONSTER_FLAGS_UNDEAD = 0x04000000,
    MONSTER_FLAGS_NO_MORALE = 0x04000000
} MonsterFlags;

typedef unsigned int UInt32;   // crc32Table[]
struct MemEntry;               // gpMemEntry — full def not yet reconstructed; used via pointer
struct _SAMPLE;                // SAMPLE2::pMem — opaque

// gConfig — game/editor preferences (persisted to the registry; field names are the
// retail registry value names, recovered from ReadPrefsFromRegistry/WritePrefsToRegistry).
#pragma pack(push, 1)
struct exeGfxConfig {          // per-exe window config (Main Game = [0], Editor = [1]); 28B, indexed giCurExe
    int showMenu;              // +0x00  "<exe> Show Menu"
    int x;                     // +0x04  "<exe> X"
    int y;                     // +0x08  "<exe> Y"
    int width;                 // +0x0c  "<exe> Width"
    int height;                // +0x10  "<exe> Height"
    int fullScreen;            // +0x14  "<exe> Full Screen"
    int colorMouseCursor;      // +0x18  "<exe> Color Mouse Cursor"
};
struct configStruct {                    // gConfig, 0x1a0 bytes
    int computerWalkSpeed;               // 0x00  "Computer Walk Speed"
    int walkSpeed;                       // 0x04  "Walk Speed"
    int musicVolume;                     // 0x08  "Music Volume"
    int soundVolume;                     // 0x0c  "Sound Volume"
    int autosave;                        // 0x10  "Autosave"
    int showRoute;                       // 0x14  "Show Route"
    int blackoutComputer;                // 0x18  "Blackout Computer"
    exeGfxConfig gfx[2];                 // 0x1c "Main Game *", 0x38 "Editor *"
    int firstMapOffset;                  // 0x54  "First Map Offset"
    int currentMapOffset;                // 0x58  "Current Map Offset"
    int showObjectBoxes;                 // 0x5c  "Show Object Boxes"
    int editorScreenAnimation;           // 0x60  "Editor Screen Animation"
    int editorPaletteCycling;            // 0x64  "Editor Palette Cycling"
    int showCombatGrid;                  // 0x68  "Show Combat Grid"
    int showCombatMouseHex;              // 0x6c  "Show Combat Mouse Hex"
    int combatShadeLevel;                // 0x70  "Combat Shade Level"
    int combatArmyInfoLevel;             // 0x74  "Combat Army Info Level"
    int evilInterfaceUsage;              // 0x78  "Evil Interface Usage"
    char autoLoadName[0xd];              // 0x7c  default "AUTO"
    char autoSaveName[0x21];             // 0x89  default "AUTO"
    int soundQuality;                    // 0xaa  "Sound Quality"
    char modemInitString[0x64];          // 0xae  "Modem Init String"
    int modemComPort;                    // 0x112 "Modem Com Port"
    int directConnectComPort;            // 0x116 "Direct Connect Com Port"
    int modemBaudRate;                   // 0x11a "Modem Baud Rate"
    int directConnectBaudRate;           // 0x11e "Direct Connect Baud Rate"
    char uniqueSystemID[4];              // 0x122 "Unique System ID" (map-file name prefix)
    int useOpera;                        // 0x126 "Use Opera"
    int quickCombatLevel;                // 0x12a "Quick Combat Level"
    int combatSpeed;                     // 0x12e "Combat Speed"
    int autoCombatUseSpells;             // 0x132 "Auto Combat Use Spells"
    int slowVideo;                       // 0x136 "Slow Video"
    char rmtRLName[0xd];                 // 0x13a "RMT<id>RL.BIN"
    char rmtRCName[0xd];                 // 0x147 "RMT<id>RC.BIN"
    char rmtRDName[0xd];                 // 0x154 "RMT<id>RD.BIN"
    char rmtSLName[0xd];                 // 0x161 "RMT<id>SL.BIN"
    char rmtSCName[0xd];                 // 0x16e "RMT<id>SC.BIN"
    char rmtSDName[0xd];                 // 0x17b "RMT<id>SD.BIN"
    char networkDefaultName[0x18];       // 0x188 "Network Default Name"
};
#pragma pack(pop)
struct SCreatureInfo { unsigned short value; char pad[24]; };               // gCreatureInfo[]
struct tag_tilePoint { signed char x; signed char _1; signed char y; signed char _3; };  // normalDirTable[]
#pragma pack(push, 1)
struct tag_monsterInfo {
    union {
        struct {
            short cost;
            int fightValue;
        };
        int randomValue;
    };
    signed char unknown06;
    signed char growth;
    unsigned short hitPoints;
    signed char race;
    signed char speed;
    signed char attack;
    signed char defense;
    signed char damageMin;
    signed char damageMax;
    signed char shots;
    char unknown11[3];
    union {
        int all;
        struct {
            char unknown14[2];
            signed char abilities;
            signed char attributes;
        } bytes;
    } flags;
    char unknown18[2];
};
#pragma pack(pop)
#pragma pack(push, 1)
struct SCmbtHero {
    short x[2];
    short y[2];
    char unknown8;
    signed char animationFrameCount[8];
    char unknown11[4];
    signed char animationFrames[8][9];
    char unknown5d[0x24];
};
SIZE(SCmbtHero, 0x81);
#pragma pack(pop)
struct SSpellInfo {
    char soundName[9];
    unsigned char level;
    unsigned char iconIndex;
    unsigned char combatEffect;
    short aiValue;
    unsigned char raceChance[6];
    unsigned char attributes;
    unsigned char m_e;
};  // gsSpellInfo[] (sizeof 22)
#pragma pack(push, 1)
typedef enum NetPlayerInfoConstant {
    NET_PLAYER_INFO_NAME_SIZE = 22,
    NET_PLAYER_INFO_RESERVED_SIZE = 6
} NetPlayerInfoConstant;

struct SNetPlayerInfo {
    int networkId;
    char name[NET_PLAYER_INFO_NAME_SIZE];
    signed char useRegularCompression;
    signed char useDiffCompression;
    char reserved[NET_PLAYER_INFO_RESERVED_SIZE];
};
#pragma pack(pop)
SIZE(SNetPlayerInfo, 34);
struct SAMPLE2 { class sample *pSample; struct _SAMPLE *pMem; };            // NULL_SAMPLE2

typedef enum MonsterAttribute {
    MONSTER_ATTRIBUTE_UNKNOWN = 0x01,
    MONSTER_ATTRIBUTE_FLYING = 0x02,
    MONSTER_ATTRIBUTE_RANGED = 0x04
} MonsterAttribute;

#pragma pack(push, 1)
struct monsterRV {
    int rv;
    signed char level;
    char pad5[15];
    int attributes;
    char pad24[2];
};                                                                          // gMonsterInfo[] (26B, pack 1)
struct SWinSetup { unsigned char m_0; unsigned short m_1; char *m_3; };     // gWinSetup[] (7B, pack 1)
#pragma pack(pop)

#endif // HOMM2_TYPES_H
