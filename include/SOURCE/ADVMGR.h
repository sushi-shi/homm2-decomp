#ifndef HOMM2_ADVMGR_H
#define HOMM2_ADVMGR_H

#include <va.h>
#include <SOURCE/GAME.h>
#include <SOURCE/KB_TYPES.h>

class mapCell;
struct tag_message;

H2_ENUM_CLASS_BEGIN(AdventureScrollDirection)
    ADVMGR_SCROLL_NORTH      = 0,
    ADVMGR_SCROLL_NORTH_EAST = 1,
    ADVMGR_SCROLL_EAST       = 2,
    ADVMGR_SCROLL_SOUTH_EAST = 3,
    ADVMGR_SCROLL_SOUTH      = 4,
    ADVMGR_SCROLL_SOUTH_WEST = 5,
    ADVMGR_SCROLL_WEST       = 6,
    ADVMGR_SCROLL_NORTH_WEST = 7
H2_ENUM_CLASS_END(AdventureScrollDirection)

H2_ENUM_BEGIN(AdventureRemoteConstant)
    ADVMGR_REMOTE_DATA_REQUEST             = 1,
    ADVMGR_REMOTE_PACKET_TYPE_GAME         = 2,
    ADVMGR_REMOTE_PACKET_TYPE_ALT          = 3,
    ADVMGR_REMOTE_COMMAND_SAVE_GAME        = 1,
    ADVMGR_REMOTE_COMMAND_POP_NET_BOX      = 11,
    ADVMGR_REMOTE_COMMAND_COMBAT           = 21,
    ADVMGR_REMOTE_COMMAND_PLAYER_EXIT      = 31,
    ADVMGR_REMOTE_COMMAND_HOST_PLAYER_EXIT = 33,
    ADVMGR_REMOTE_COMMAND_GROUP_MAP_CHANGE = 41,
    ADVMGR_REMOTE_PAYLOAD_VIEW_SIZE        = 16
H2_ENUM_END(AdventureRemoteConstant)

H2_ENUM_CLASS_BEGIN(AdventureEnvironmentSoundId)
    ADVMGR_ENVIRONMENT_SOUND_NONE     = -1,
    ADVMGR_SOUND_BUOY                 = 0,
    ADVMGR_SOUND_SHIPWRECK            = 1,
    ADVMGR_SOUND_COAST                = 2,
    ADVMGR_SOUND_ORACLE               = 3,
    ADVMGR_SOUND_STONE_LITHS          = 4,
    ADVMGR_SOUND_SMALL_VOLCANO        = 5,
    ADVMGR_SOUND_LAVA_POOL            = 6,
    ADVMGR_SOUND_ALCHEMIST_LAB        = 7,
    ADVMGR_SOUND_ALCHEMIST_LAB_ACTION = 8,
    ADVMGR_SOUND_WATER_WHEEL          = 9,
    ADVMGR_SOUND_CAMPFIRE             = 10,
    ADVMGR_SOUND_WINDMILL             = 11,
    ADVMGR_SOUND_FOUNTAIN             = 12,
    ADVMGR_SOUND_WATERING_HOLE        = 13,
    ADVMGR_SOUND_STREAM               = 14,
    ADVMGR_SOUND_MINE                 = 15,
    ADVMGR_SOUND_SAWMILL              = 16,
    ADVMGR_SOUND_DAEMON_CAVE          = 17,
    ADVMGR_SOUND_SHRINE               = 18,
    ADVMGR_SOUND_SEAGULLS             = 19,
    ADVMGR_SOUND_COASTLINE            = 20,
    ADVMGR_SOUND_TAR_PIT              = 21,
    ADVMGR_SOUND_TRADING_POST         = 22,
    ADVMGR_SOUND_DERELICT_SHIP        = 23,
    ADVMGR_SOUND_RUINS                = 24,
    ADVMGR_SOUND_DWELLING             = 25,
    ADVMGR_SOUND_ABANDONED_MINE       = 26,
    ADVMGR_SOUND_LARGE_VOLCANO        = 27,
    ADVMGR_ENVIRONMENT_SOUND_COUNT    = 28
H2_ENUM_CLASS_END(AdventureEnvironmentSoundId)

H2_ENUM_CLASS_BEGIN(AdventureSystemOption)
    ADVMGR_SYSTEM_OPTION_MUSIC_VOLUME   = 10,
    ADVMGR_SYSTEM_OPTION_SOUND_VOLUME   = 11,
    ADVMGR_SYSTEM_OPTION_HERO_SPEED     = 12,
    ADVMGR_SYSTEM_OPTION_MUSIC_SOURCE   = 13,
    ADVMGR_SYSTEM_OPTION_SHOW_ROUTE     = 14,
    ADVMGR_SYSTEM_OPTION_COMPUTER_SPEED = 15,
    ADVMGR_SYSTEM_OPTION_INTERFACE      = 16,
    ADVMGR_SYSTEM_OPTION_VIDEO          = 17,
    ADVMGR_SYSTEM_OPTION_COLOR_CURSOR   = 18,
    ADVMGR_SYSTEM_OPTION_FIRST          = IDX(ADVMGR_SYSTEM_OPTION_MUSIC_VOLUME),
    ADVMGR_SYSTEM_OPTION_LAST           = IDX(ADVMGR_SYSTEM_OPTION_COLOR_CURSOR),
    ADVMGR_SYSTEM_OPTION_COUNT          =
        IDX(ADVMGR_SYSTEM_OPTION_LAST) - IDX(ADVMGR_SYSTEM_OPTION_FIRST) + 1
H2_ENUM_CLASS_END(AdventureSystemOption)

#ifdef HOMM2_STRICT_ENUM_TYPES
inline i32 EncodeAdventureSystemOption(AdventureSystemOption option) {
    return static_cast<i32>(option);
}

inline AdventureSystemOption DecodeAdventureSystemOption(i32 value) {
    return static_cast<AdventureSystemOption>(value);
}
#else
#define EncodeAdventureSystemOption(option) (option)
#define DecodeAdventureSystemOption(value) (value)
#endif

H2_ENUM_CLASS_BEGIN(AdventureCommand)
    ADVMGR_INVALID_CELL               = -1,
    ADVMGR_COMMAND_NONE               = -1,
    ADVMGR_COMMAND_MOVE_TO            = 1,
    ADVMGR_COMMAND_HERO_VIEW          = 2,
    ADVMGR_COMMAND_TOWN_VIEW          = 3,
    ADVMGR_COMMAND_SELECT_HERO        = 4,
    ADVMGR_COMMAND_SELECT_TOWN        = 5,
    ADVMGR_COMMAND_OCCUPIED_TOWN_VIEW = 6,
    ADVMGR_COMMAND_CONTINUE_ROUTE     = 7
H2_ENUM_CLASS_END(AdventureCommand)

H2_ENUM_CLASS_BEGIN(AdventureDrawMask)
    ADVMGR_DRAW_GROUND      = 0x01,
    ADVMGR_DRAW_OBJECT      = 0x02,
    ADVMGR_DRAW_OVERLAY     = 0x04,
    ADVMGR_DRAW_HERO        = 0x08,
    ADVMGR_DRAW_CLOUD       = 0x20,
    ADVMGR_DRAW_OVERLAY_TOP = 0x40,
    ADVMGR_DRAW_HERO_SHADOW = 0x80
H2_ENUM_CLASS_END(AdventureDrawMask)
H2_ENUM_FLAGS(AdventureDrawMask)

H2_ENUM_BEGIN(AdventureSystemOptionsConstant)
    ADVMGR_SYSTEM_OPTIONS_WINDOW_X                = 160,
    ADVMGR_SYSTEM_OPTIONS_WINDOW_Y                = 33,
    ADVMGR_SYSTEM_OPTIONS_TITLE                   = 2,
    ADVMGR_SYSTEM_OPTIONS_SOUND_FRAME_BASE        = 2,
    ADVMGR_SYSTEM_OPTIONS_SPEED_FRAME_BASE        = 4,
    ADVMGR_SYSTEM_OPTIONS_MUSIC_SOURCE_FRAME_BASE = 10,
    ADVMGR_SYSTEM_OPTIONS_ROUTE_FRAME_BASE        = 13,
    ADVMGR_SYSTEM_OPTIONS_COMPUTER_HIDDEN_FRAME   = 9,
    ADVMGR_SYSTEM_OPTIONS_INTERFACE_FRAME_BASE    = 15,
    ADVMGR_SYSTEM_OPTIONS_VIDEO_FRAME_BASE        = 18,
    ADVMGR_SYSTEM_OPTIONS_CURSOR_FRAME_BASE       = 20,
    ADVMGR_SYSTEM_OPTIONS_TEXT_ID_OFFSET          = 10,
    ADVMGR_SYSTEM_OPTIONS_DRAW_MASK               = 0x7fff
H2_ENUM_END(AdventureSystemOptionsConstant)

H2_ENUM_BEGIN(AdventureAIStorageConstant)
    ADVMGR_PLACE_VISIT_COUNT      = 30,
    ADVMGR_PLACE_COORDINATE_COUNT = 2
H2_ENUM_END(AdventureAIStorageConstant)

#ifdef HOMM2_STRICT_ENUM_TYPES
inline i32 EncodeAdventureSystemOptionTextControl(AdventureSystemOption option) {
    return EncodeAdventureSystemOption(option) + ADVMGR_SYSTEM_OPTIONS_TEXT_ID_OFFSET;
}
#else
#define EncodeAdventureSystemOptionTextControl(option)                                             \
    ((option) + ADVMGR_SYSTEM_OPTIONS_TEXT_ID_OFFSET)
#endif

i32 SaveGame(void);
i32 DimensionDoorHandler(struct tag_message&);
i32 TownPortalHandler(struct tag_message&);
void ComputeAdvNetControl(void);
i32 MapExtraPosAndAdjacentsSet(i32, i32, u8);
i32 APanelHandler(struct tag_message&);
i32 CPanelHandler(struct tag_message&);
void UpdateSystemOptions(i32);
i32 SystemOptionsHandler(struct tag_message&);
i32 GetMobilityFrame(i32);
i32 GetManaFrame(i32);
u8 StopOnTrigger(class mapCell*);

extern float fFirstWeekTownFV;
extern i32 iVepCacheHits;
extern i32 iTotalVepHits;
extern i32 giShowComputerRoute;
extern i32l glLastStartTick;
extern i32l glCurTicks;
extern i32l glTotalTicks;
extern float gfAttackHumanBonus;
extern float gfAttackComputerBonus;
extern i32 bSVSearchArrayInUse;
extern i32 bEvaluatingTravelGates;
extern b32 gbReduceByBerserk;
extern float fBerserkFactor;
extern i32 giMaxHeroesForThisPlayer;
extern float fReduceFactor;
extern u8 giCurPlayerBit;
extern i32 giBestShipyardDist;
extern i16 gaiHeroLiveChance[GAME_HERO_COUNT];
extern i32 giHumanTownConquered;
extern i32 costTemp[IDX(RES_COUNT)];
extern b32 gbPossibleShipyardFound;
extern i32 iCurPlaceToVisit;
extern i32 giBestShipyardId;
extern b32 gbActualBoatFound;
extern u8 giCurWatchPlayerBit;
extern float gfHeroInteractionBonus[GAME_HERO_COUNT];
extern b32 gbBerserk;
extern i32 giCurAIHeroMorale;
extern i32 iPlacesVisited[ADVMGR_PLACE_VISIT_COUNT][ADVMGR_PLACE_COORDINATE_COUNT];
extern b32 gbTroopReload;
extern i32 giCurAIHeroLuck;
extern b32 gbActualShipyardFound;

#endif
