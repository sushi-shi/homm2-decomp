#ifndef HOMM2_X_GLOBAL_H
#define HOMM2_X_GLOBAL_H

#include <SOURCE/KB_TYPES.h>
#include <SOURCE/REMOTE_TYPES.h>
#include <va.h>
#include <SOURCE/CURSOR.h>
#include <SOURCE/dimPalette.h>
#include <SOURCE/hero.h>

class ExpCampaign;

H2_ENUM_BEGIN(SecondarySkillValueTableConstant)
    SECONDARY_SKILL_VALUE_LEVEL_COUNT = IDX(HERO_SKILL_LEVEL_COUNT) - 1
H2_ENUM_END(SecondarySkillValueTableConstant)

H2_ENUM_BEGIN(ElevationOverlayConstant)
    ELEVATION_OVERLAY_COUNT           = 25,
    ELEVATION_OVERLAY_CELL_COUNT      = 15,
    COMBAT_OBSTACLE_CELL_OFFSET_COUNT = 8
H2_ENUM_END(ElevationOverlayConstant)

H2_ENUM_BEGIN(TownEligibleBuildMaskConstant)
    TOWN_ELIGIBLE_BUILD_MASK_COUNT       = 6,
    TOWN_ELIGIBLE_BUILD_KNIGHT_MASK      = 0x3ff8bf9f,
    TOWN_ELIGIBLE_BUILD_BARBARIAN_MASK   = 0x1bf8bf9f,
    TOWN_ELIGIBLE_BUILD_SORCERESS_MASK   = 0x0ff8bf9f,
    TOWN_ELIGIBLE_BUILD_WARLOCK_MASK     = 0x69f8bf9f,
    TOWN_ELIGIBLE_BUILD_WIZARD_MASK      = 0x35f8bf9f,
    TOWN_ELIGIBLE_BUILD_NECROMANCER_MASK = 0x1ff8bf9b
H2_ENUM_END(TownEligibleBuildMaskConstant)

H2_ENUM_BEGIN(CombatControlConstant)
    COMBAT_CONTROL_SIDE_COUNT = 2
H2_ENUM_END(CombatControlConstant)

#pragma pack(push, 1)
struct SCmbtObstacle {
    u32 terrainMask;
    u8 minimumColumn;
    u8 cellCount;
    i8 cellOffsets[COMBAT_OBSTACLE_CELL_OFFSET_COUNT];
};

struct SElevationOverlay {
    u16 terrainMask;
    i8 cellOffsets[ELEVATION_OVERLAY_CELL_COUNT];
};
#pragma pack(pop)
SIZE(SCmbtObstacle, 0xe);
SIZE(SElevationOverlay, 0x11);

H2_ENUM_BEGIN(GlobalConstant)
    HERO_TYPE_INITIAL_COUNT                 = IDX(FACTION_COUNT),
    EXPANSION_HIGH_SCORE_CAMPAIGN_COUNT     = 4,
    X_GLOBAL_RECRUIT_EMPTY_COUNT            = 6,
    X_GLOBAL_RECRUIT_BUY_COUNT              = 5,
    X_GLOBAL_PASSWORD_STRING_COUNT          = 211,
    X_GLOBAL_STABLE_TEXT_COUNT              = 4,
    X_GLOBAL_BUILDING_RESOURCE_COUNT        = 7,
    X_GLOBAL_SETUP_HELP_COUNT               = 4,
    X_GLOBAL_EXPANSION_CAMPAIGN_COUNT       = 4,
    X_GLOBAL_EXPANSION_CAMPAIGN_MAP_COUNT   = 8,
    X_GLOBAL_EXPANSION_CAMPAIGN_AWARD_COUNT = 12,
    X_GLOBAL_EXPANSION_HERO_NAME_COUNT      = 6,
    X_GLOBAL_SHORT_SKILL_LEVEL_COUNT        = 4,
    X_GLOBAL_NEW_HERO_ALIGNMENT_COUNT       = 16,
    X_GLOBAL_PASSWORD_STRING_INDEX_COUNT    = 8,
    X_GLOBAL_PLAYER_COUNT                   = 6,
    GLOBAL_MAP_NAME_SIZE                    = 0x18,
    GLOBAL_TCP_TEXT_SIZE                    = 0x18,
    GLOBAL_AGGREGATE_PATH_SIZE              = 0x160,
    GLOBAL_COMMAND_LINE_SIZE                = 0x40,
    GLOBAL_GAME_PATH_SIZE                   = 0x18,
    GLOBAL_MAP_PATH_SIZE                    = 0x14,
    GLOBAL_DRIVER_NAME_SIZE                 = 0x10,
    GLOBAL_BOTTOM_VIEW_TEXT_SIZE            = 0x98,
    GLOBAL_SHORT_MAP_NAME_SIZE              = 16,
    GLOBAL_PLAYER_NAME_SIZE                 = 21,
    GLOBAL_WINDOW_TEXT_SIZE                 = 0x12c
H2_ENUM_END(GlobalConstant)

extern i8 xIsExpansionMap;
extern i32 xLastChoice;
extern u8 xIsPlayingExpansionCampaign;
extern class ExpCampaign xCampaign;
extern char* xSetupCampaignGameHelp[X_GLOBAL_SETUP_HELP_COUNT];
extern char* xSetupStandardGameHelp[X_GLOBAL_SETUP_HELP_COUNT];
extern char* xCampaignAwards[X_GLOBAL_EXPANSION_CAMPAIGN_AWARD_COUNT];
extern char* xScenarioName[X_GLOBAL_EXPANSION_CAMPAIGN_COUNT]
                          [X_GLOBAL_EXPANSION_CAMPAIGN_MAP_COUNT];
extern char* xScenarioDescription[X_GLOBAL_EXPANSION_CAMPAIGN_COUNT]
                                 [X_GLOBAL_EXPANSION_CAMPAIGN_MAP_COUNT];
extern char* xShortCampaignNames[X_GLOBAL_EXPANSION_CAMPAIGN_COUNT];
extern char* xHSCampaignNames[EXPANSION_HIGH_SCORE_CAMPAIGN_COUNT];
extern i32 xTheSpell;
extern char* xNecromancerShrine;
extern char* xNecromancerShrineDesc;
extern char* xStableText[X_GLOBAL_STABLE_TEXT_COUNT];
extern char* xJosephName[X_GLOBAL_EXPANSION_HERO_NAME_COUNT];
extern char* xUncleIvanName[X_GLOBAL_EXPANSION_HERO_NAME_COUNT];
extern char* xShortSSLevelNames[X_GLOBAL_SHORT_SKILL_LEVEL_COUNT];
extern char* xPasswordStrings[X_GLOBAL_PASSWORD_STRING_COUNT];
extern u8 xNewHeroAlignment[X_GLOBAL_NEW_HERO_ALIGNMENT_COUNT];
extern i32 xPasswordStringsIndex[X_GLOBAL_PASSWORD_STRING_INDEX_COUNT];
extern i32 xShrineBuildingCost[X_GLOBAL_BUILDING_RESOURCE_COUNT];

H2_ENUM_BEGIN(RadarColorTableCount)
    RADAR_MAP_COLOR_COUNT    = 16,
    RADAR_OBJECT_COLOR_COUNT = 16,
    RADAR_OWNER_COLOR_COUNT  = 8
H2_ENUM_END(RadarColorTableCount)

H2_ENUM_BEGIN(GroundTableConstant)
    GROUND_TILE_IMAGE_COUNT = 432,
    GROUND_SHAPE_FLIPPED    = 0x80
H2_ENUM_END(GroundTableConstant)

H2_ENUM_BEGIN(KbControlTableConstant)
    MOVEMENT_FRAME_FLIP_COUNT          = 16,
    CASTLE_RESOURCE_SLOT_COUNT         = 8,
    CASTLE_AMOUNT_COUNT                = 4,
    VESA_MODE_VALUE_COUNT              = 8,
    NORMAL_DIRECTION_COUNT             = 8,
    RESOURCE_VALUE_COUNT               = 8,
    STARTING_RESOURCE_DIFFICULTY_COUNT = 5,
    STARTING_RESOURCE_TYPE_COUNT       = 7,
    MINE_CHARACTERISTIC_COUNT          = 8,
    VESA_SET_MODE_FUNCTION             = 0x4f02,
    VESA_MODE_640_480_256              = 0x0101
H2_ENUM_END(KbControlTableConstant)

H2_ENUM_BEGIN(KbPaletteTableConstant)
    PUZZLE_DRAW_TABLE_COUNT = 64
H2_ENUM_END(KbPaletteTableConstant)

H2_ENUM_BEGIN(KbGameTableConstant)
    KB_MAP_SIZE_COUNT                   = 4,
    KB_INTERFACE_TYPE_COUNT             = 37,
    KB_INTERFACE_VARIANT_COUNT          = 2,
    KB_COMBAT_SPEED_COUNT               = 3,
    KB_COMBAT_FX_COUNT                  = 32,
    KB_TOWN_COMMAND_COUNT               = 28,
    KB_HERO_DEFAULT_NAME_COUNT          = 54,
    KB_ARMY_EFFECT_COUNT                = 20,
    KB_MUSIC_TRACK_COUNT                = 0x3c,
    KB_ARTIFACT_LEVEL_COUNT             = IDX(ARTIFACT_COUNT) + 1,
    KB_ARTIFACT_BASE_VALUE_COUNT        = IDX(ARTIFACT_COUNT),
    KB_STAT_POWER_COUNT                 = 42,
    KB_SPELL_LIMIT_COUNT                = 5,
    KB_SPELL_MOD_COUNT                  = 12,
    KB_QUICK_COMBAT_SPELL_TYPE_COUNT    = 7,
    KB_CLOUD_MASK_COUNT                 = 256,
    KB_HERO_LEVEL_BAND_COUNT            = 2,
    KB_SPELL_INFLUENCE_MAP_COUNT        = 16,
    KB_SPELL_EFFECT_COUNT               = 32,
    KB_COMBAT_BACKGROUND_COUNT          = 20,
    KB_COMBAT_OBSTACLE_COUNT            = 32,
    KB_TERRAIN_TYPE_COUNT               = IDX(TERRAIN_COUNT) + 1,
    KB_TERRAIN_STEP_TYPE_COUNT          = 2,
    KB_TRIGGER_TYPE_COUNT               = 128,
    KB_TOWN_OBJECT_NAME_COUNT           = 32,
    KB_CASTLE_WALL_SEGMENT_COUNT        = 4,
    KB_CASTLE_TOWER_COUNT               = 4,
    KB_CASTLE_DOOR_POSITION_COUNT       = 2,
    KB_COMBAT_COORDINATE_COUNT          = 2,
    KB_TRADING_POST_EFFICIENCY_COUNT    = 11,
    KB_MOAT_CELL_COUNT                  = 9,
    KB_ALIGNMENT_NAME_COUNT             = IDX(FACTION_COUNT) + 2,
    KB_QUICK_VIEW_TEXT_COUNT            = 124,
    KB_EVENT_TEXT_TABLE_COUNT           = 96,
    KB_CONTROL_PANEL_HELP_COUNT         = 6,
    KB_COMBAT_SPELL_PANEL_HELP_COUNT    = 8,
    KB_ADVENTURE_PANEL_HELP_COUNT       = 6,
    KB_INIT_MENU_HELP_COUNT             = 6,
    KB_ADVENTURE_MENU_HELP_COUNT        = 8,
    KB_LUCK_TEXT_COUNT                  = 8,
    KB_MORALE_TEXT_COUNT                = 8,
    KB_ON_OFF_TEXT_COUNT                = 12,
    KB_WALK_SPEED_TEXT_COUNT            = 6,
    KB_MONTH_NAME_COUNT                 = 10,
    KB_WEEK_NAME_COUNT                  = 16,
    KB_HERO_SCREEN_TEXT_COUNT           = 26,
    KB_CASTLE_INFO_TEXT_COUNT           = 16,
    KB_LUCK_INFO_TEXT_COUNT             = 22,
    KB_IQ_NAME_COUNT                    = 6,
    KB_SPELL_HELP_TEXT_COUNT            = 10,
    KB_SPEED_TEXT_COUNT                 = 10,
    KB_ARMY_DETAIL_TEXT_COUNT           = 10,
    KB_WELL_DETAIL_TEXT_COUNT           = 10,
    KB_KINGDOM_OVERVIEW_TEXT_COUNT      = 4,
    KB_NEW_TURN_TEXT_COUNT              = 8,
    KB_VIEW_GENERAL_LABEL_COUNT         = 8,
    KB_VIEW_GENERAL_HELP_COUNT          = 8,
    KB_VIEW_GENERAL_LONG_HELP_COUNT     = 4,
    KB_COMBAT_MESSAGE_COUNT             = 12,
    KB_HERO_LEVEL_TEXT_COUNT            = 4,
    KB_COMBAT_HELP_COUNT                = 6,
    KB_LONG_COMBAT_HELP_COUNT           = 6,
    KB_NEW_GAME_HELP_COUNT              = 8,
    KB_SETUP_BAUD_HELP_COUNT            = 6,
    KB_SETUP_COM_PORT_HELP_COUNT        = 6,
    KB_SETUP_DC_BAUD_HELP_COUNT         = 6,
    KB_SETUP_DC_COM_PORT_HELP_COUNT     = 6,
    KB_SETUP_HOT_SEAT_HELP_COUNT        = 6,
    KB_SETUP_MODEM_HELP_COUNT           = 4,
    KB_SETUP_DIRECT_CONNECT_HELP_COUNT  = 4,
    KB_SETUP_MULTIPLAYER_HELP_COUNT     = 6,
    KB_SETUP_NETWORK_HELP_COUNT         = 4,
    KB_SETUP_NETWORK_SECOND_HELP_COUNT  = 4,
    KB_SETUP_GAME_HELP_COUNT            = 4,
    KB_BATTLE_RESULT_TEXT_COUNT         = 12,
    KB_MORALE_INFO_TEXT_COUNT           = 32,
    KB_MAP_SIZE_TEXT_COUNT              = 4,
    KB_DIFFICULTY_TEXT_COUNT            = 6,
    KB_START_DIFFICULTY_TEXT_COUNT      = 4,
    KB_CAMPAIGN_LEADER_TEXT_COUNT       = 4,
    KB_WIN_TEXT_COUNT                   = 6,
    KB_HUMAN_DIFFICULTY_TEXT_COUNT      = 6,
    KB_HUMAN_INFO_DIFFICULTY_TEXT_COUNT = 6,
    KB_MUSIC_QUALITY_TEXT_COUNT         = 4,
    KB_SPELL_TEXT_COUNT                 = IDX(SPELL_COUNT) + 1,
    KB_SECONDARY_SKILL_LEVEL_TEXT_COUNT = IDX(HERO_SKILL_LEVEL_COUNT),
    KB_SECONDARY_SKILL_TEXT_COUNT       = IDX(HERO_SKILL_COUNT),
    KB_NEUTRAL_BUILDING_TEXT_COUNT      = 20,
    KB_WELL_EXTRA_NAME_COUNT            = 8,
    KB_SPECIAL_BUILDING_NAME_COUNT      = 8,
    KB_NEUTRAL_BUILDING_INFO_COUNT      = 20,
    KB_SPECIAL_BUILDING_INFO_COUNT      = IDX(FACTION_COUNT),
    KB_DIRECTION_TEXT_COUNT             = 10,
    KB_RUMOUR_TERRAIN_DESCRIPTION_COUNT = KB_TERRAIN_TYPE_COUNT,
    KB_INTERFACE_TYPE_TEXT_COUNT        = 4,
    KB_BW_MOUSE_TEXT_COUNT              = 2,
    KB_COMBAT_SPEED_TEXT_COUNT          = KB_COMBAT_SPEED_COUNT + 1,
    KB_COMBAT_MINI_INFO_TEXT_COUNT      = 4,
    KB_COMMAND_LINE_HELP_COUNT          = 14,
    KB_OVERVIEW_TEXT_COUNT              = 6,
    KB_WIN_COM_ERROR_TEXT_COUNT         = 6,
    KB_MINI_VIEW_TEXT_COUNT             = 10,
    KB_FILE_REQUEST_HELP_COUNT          = 16,
    KB_PERSONALITY_TEXT_COUNT           = 4,
    KB_ARMY_SIZE_NAME_COUNT             = 9,
    KB_ARMY_SIZE_NAME_VARIANT_COUNT     = 3,
    KB_RANDOM_TAVERN_TEXT_COUNT         = 8,
    KB_RANDOM_SIGN_TEXT_COUNT           = 4,
    KB_CAMPAIGN_AWARD_TEXT_COUNT        = 12,
    KB_SLOW_VIDEO_LEVEL_TEXT_COUNT      = 2,
    KB_SETTINGS_PANEL_HELP_COUNT        = 10,
    KB_BARRIER_COLOR_NAME_COUNT         = 8,
    KB_GENERIC_SITE_NAME_COUNT          = 8,
    KB_RECRUITMENT_SITE_NAME_COUNT      = 6,
    KB_WIN_SETUP_COUNT                  = 73,
    KB_INIT_MENU_HOTSPOT_COUNT          = 5
H2_ENUM_END(KbGameTableConstant)

H2_ENUM_CLASS_BEGIN(InitMenuHotSpotField)
    INIT_MENU_HOTSPOT_X           = 0,
    INIT_MENU_HOTSPOT_Y           = 1,
    INIT_MENU_HOTSPOT_WIDTH       = 2,
    INIT_MENU_HOTSPOT_HEIGHT      = 3,
    INIT_MENU_HOTSPOT_FIELD_COUNT = 4
H2_ENUM_CLASS_END(InitMenuHotSpotField)

extern u8 giGroundShape[GROUND_TILE_IMAGE_COUNT];
extern u8 gColorTableTan[DIM_PALETTE_COLOR_COUNT];
extern u8 gColorTableGray[DIM_PALETTE_COLOR_COUNT];
extern u8 gColorTableRed[DIM_PALETTE_COLOR_COUNT];
extern u8 gColorTableDarkBrown[DIM_PALETTE_COLOR_COUNT];
extern i32 giMainVideoModeWidth;
extern i32 giMainVideoModeHeight;
extern u32l glMilliCounter;
extern u8 gMapColors[RADAR_MAP_COLOR_COUNT];
extern u8 gObjectColors[RADAR_OBJECT_COLOR_COUNT];
extern u8 gOwnerColors[RADAR_OWNER_COLOR_COUNT];
// Indexes of gTilesetFiles: one id per adventure-object icn tileset.
H2_ENUM_CLASS_BEGIN(TilesetId)
    TILESET_OBJNHAUN                = 10,
    TILESET_OBJNARTI                = 11,
    TILESET_MONS32                  = 12,
    TILESET_ART32                   = 13,
    TILESET_FLAG32                  = 14,
    TILESET_RESSMALL                = 15,
    TILESET_HOURGLAS                = 16,
    TILESET_ROUTE                   = 17,
    TILESET_STONBACK                = 19,
    TILESET_MINIMON                 = 20,
    TILESET_MINIHERO                = 21,
    TILESET_MTNSNOW                 = 22,
    TILESET_MTNSWMP                 = 23,
    TILESET_MTNLAVA                 = 24,
    TILESET_MTNDSRT                 = 25,
    TILESET_MTNDIRT                 = 26,
    TILESET_MTNMULT                 = 27,
    TILESET_EXTRAOVR                = 29,
    TILESET_ROAD                    = 30,
    TILESET_MTNCRCK                 = 31,
    TILESET_MTNGRAS                 = 32,
    TILESET_TREJNGL                 = 33,
    TILESET_TREEVIL                 = 34,
    TILESET_OBJNTOWN                = 35,
    TILESET_OBJNTWBA                = 36,
    TILESET_OBJNTWSH                = 37,
    TILESET_OBJNTWRD                = 38,
    TILESET_OBJNXTRA                = 39,
    TILESET_OBJNWAT2                = 40,
    TILESET_OBJNMUL2                = 41,
    TILESET_TRESNOW                 = 42,
    TILESET_TREFIR                  = 43,
    TILESET_TREFALL                 = 44,
    TILESET_STREAM                  = 45,
    TILESET_OBJNRSRC                = 46,
    TILESET_DUMMY                   = 47,
    TILESET_OBJNGRA2                = 48,
    TILESET_TREDECI                 = 49,
    TILESET_OBJNWATR                = 50,
    TILESET_OBJNGRAS                = 51,
    TILESET_OBJNSNOW                = 52,
    TILESET_OBJNSWMP                = 53,
    TILESET_OBJNLAVA                = 54,
    TILESET_OBJNDSRT                = 55,
    TILESET_OBJNDIRT                = 56,
    TILESET_OBJNCRCK                = 57,
    TILESET_OBJNLAV3                = 58,
    TILESET_OBJNMULT                = 59,
    TILESET_OBJNLAV2                = 60,
    TILESET_X_LOC1                  = 61,
    TILESET_X_LOC2                  = 62,
    TILESET_X_LOC3                  = 63,
    ABANDONED_MINE_TILESET_CENTER   = TILESET_EXTRAOVR,
    ABANDONED_MINE_TILESET_TOP_A    = TILESET_OBJNDIRT,
    ABANDONED_MINE_TILESET_TOP_B    = TILESET_OBJNGRAS,
    ABANDONED_MINE_TILESET_BOTTOM_A = TILESET_MTNDIRT,
    ABANDONED_MINE_TILESET_BOTTOM_B = TILESET_MTNGRAS,
    RANDOM_TOWN_OBJECT_TILESET      = TILESET_OBJNTOWN,
    RANDOM_TOWN_OVERLAY_TILESET     = TILESET_OBJNTWSH,
    RANDOM_TOWN_SOURCE_TILESET      = TILESET_OBJNTWRD,
    TILESET_COUNT                   = 64
H2_ENUM_CLASS_END(TilesetId)

extern char* gTilesetFiles[IDX(TILESET_COUNT)];
extern u8 bPuzzleDraw[PUZZLE_DRAW_TABLE_COUNT];
extern u8 gColorTableLighten[DIM_PALETTE_COLOR_COUNT];
extern u8 gColorTableNoCycle[DIM_PALETTE_COLOR_COUNT];
extern b32 gbReturnAfterComputeExtent;
extern b32 gbAllowTextEntryEscape;
extern i32 giScreenScroll;
extern i32 giMenuCommand;
extern b32 gbSendMouseMoveMessages;
extern b32 gbColorMice;
extern u32l gTownEligibleBuildMask[TOWN_ELIGIBLE_BUILD_MASK_COUNT];
extern u8 giMapSizes[KB_MAP_SIZE_COUNT];
extern b32 gbUseEvilInterface;
extern char* cEvilTranslate[KB_INTERFACE_TYPE_COUNT][KB_INTERFACE_VARIANT_COUNT];
extern char gcAnimPath[GLOBAL_AGGREGATE_PATH_SIZE];
extern char gcGamePath[GLOBAL_GAME_PATH_SIZE];
extern char gcMapPath[GLOBAL_MAP_PATH_SIZE];
extern b32 gbDontTryRedbook;
extern b32 gbDontTryMIDI;
extern b32 gbDontTryDigital;
extern float gfCombatSpeedMod[KB_COMBAT_SPEED_COUNT];
extern i32 giDialogTimeout;
extern i32 giNewMonsterCycleFrame;
extern b32 gbLeaveNetBoxAlone;
extern b32 gbUseWaveout;
extern b32 gbShowAllMaps;
extern char* gCombatFxNames[KB_COMBAT_FX_COUNT];
extern i16 horseFrameFlip[MOVEMENT_FRAME_FLIP_COUNT];
extern i16 boatFrameFlip[MOVEMENT_FRAME_FLIP_COUNT];
extern i8 gCastleResources[CASTLE_RESOURCE_SLOT_COUNT];
extern i16 gCastleAmounts[CASTLE_AMOUNT_COUNT];
extern i16 gVesaMode[VESA_MODE_VALUE_COUNT];
extern i32 gInitResourcesHuman[STARTING_RESOURCE_DIFFICULTY_COUNT][STARTING_RESOURCE_TYPE_COUNT];
extern i32 gInitResourcesComputer[STARTING_RESOURCE_DIFFICULTY_COUNT][STARTING_RESOURCE_TYPE_COUNT];
extern i32 gMineCharacteristics[MINE_CHARACTERISTIC_COUNT];
extern i32 gSSValues[IDX(HERO_SKILL_COUNT)][SECONDARY_SKILL_VALUE_LEVEL_COUNT];
extern u8 gArtifactLevel[KB_ARTIFACT_LEVEL_COUNT];
extern i32 gUltArtifactAvgValue;
extern i8 giVisRangeTown;
extern float gfStatPower[KB_STAT_POWER_COUNT];
extern float gfBattleStat[KB_STAT_POWER_COUNT];
extern i8 gSpellLimits[KB_SPELL_LIMIT_COUNT];
extern float gfSpellCastableCombatMod[KB_SPELL_MOD_COUNT];
extern float gfSpellCastNumMod[KB_SPELL_MOD_COUNT];
extern float gfPhilAISpellPowerMod[KB_SPELL_MOD_COUNT];
extern float gfPhilAIDurationMod[KB_SPELL_MOD_COUNT];
extern float gfSpellTypeNumMod[KB_QUICK_COMBAT_SPELL_TYPE_COUNT];
extern b8 gbArrow[NORMAL_DIRECTION_COUNT][NORMAL_DIRECTION_COUNT];
extern u8 giCloudType[KB_CLOUD_MASK_COUNT];
H2_ENUM_CLASS_BEGIN(TownMusicTrack)
    TOWN_MUSIC_NONE        = 0,
    TOWN_MUSIC_WARLOCK     = 5,
    TOWN_MUSIC_WIZARD      = 6,
    TOWN_MUSIC_NECROMANCER = 7,
    TOWN_MUSIC_KNIGHT      = 8,
    TOWN_MUSIC_BARBARIAN   = 9,
    TOWN_MUSIC_SORCERESS   = 10,
    TOWN_MUSIC_TABLE_SIZE  = 8
H2_ENUM_CLASS_END(TownMusicTrack)

extern i8 townTheme[IDX(TOWN_MUSIC_TABLE_SIZE)];
extern i8 gHeroSkillBonus[IDX(FACTION_COUNT)][KB_HERO_LEVEL_BAND_COUNT][HERO_PRIMARY_STAT_COUNT];
extern i32 giMonoIconSkip;
extern i32 giScrollX;
extern i32 giScrollY;
extern b32 gbNoBorder;
extern b32 gbEnlargeScreenBlit;
extern i32 giCurExe;
extern b32 gbInDialog;
extern b32 gbInSetupDialog;
extern b32 gbMinimized;
extern b32 gbInSmackMgr;
extern i32 glBottomRefresh;
extern b32 gbBothMachinesWin95;
extern b32 gbGotFirstHeartbeat;
extern b32 gbProcessingCombatAction;
extern RemoteNetworkProtocol iMPNetProtocol;
extern i32 iLastDiffSendTo;
extern u8 gcSpellInfluenceIcons[KB_SPELL_INFLUENCE_MAP_COUNT];
extern u8 giSpellInfluenceToSpell[KB_SPELL_INFLUENCE_MAP_COUNT];
extern u8 giNumPowFrames[KB_SPELL_EFFECT_COUNT];
extern i32 giSpellEffectShowType;
extern i8 gcColorToPlayerPos[RADAR_OWNER_COLOR_COUNT];
extern char* cCombatBkgNames[KB_COMBAT_BACKGROUND_COUNT];
extern struct SCmbtObstacle sCmbtObstacles[KB_COMBAT_OBSTACLE_COUNT];
extern i32 gEstatesGoldLevel[IDX(HERO_SKILL_LEVEL_COUNT)];
extern float gfSSLogisticsMod[IDX(HERO_SKILL_LEVEL_COUNT)];
extern float gfSSNavigationMod[IDX(HERO_SKILL_LEVEL_COUNT)];
extern float gfSSArcheryMod[IDX(HERO_SKILL_LEVEL_COUNT)];
extern float gfSSAIArcheryMod[IDX(HERO_SKILL_LEVEL_COUNT)];
extern i8 giVisRange[IDX(HERO_SKILL_LEVEL_COUNT)];
extern u8 gStartingHeroStats[IDX(FACTION_COUNT)][HERO_STARTING_STAT_COUNT];
extern i32 giTerrainCost[KB_TERRAIN_TYPE_COUNT][IDX(HERO_SKILL_LEVEL_COUNT)]
                        [KB_TERRAIN_STEP_TYPE_COUNT];
extern char* gTownPrefixNames[IDX(FACTION_COUNT)];
extern char* gTownObjNames[KB_TOWN_OBJECT_NAME_COUNT];
extern i32 giDebugBuildingToBuild;
extern u8 giTerrainToMusicTrack[IDX(TERRAIN_COUNT)];
extern char* cHeroTypeShortName[IDX(FACTION_COUNT)];
extern char cHeroTypeInitial[HERO_TYPE_INITIAL_COUNT];
extern i32 giDeferObjDrawX;
extern i32 giDeferObjDrawY;
extern class heroWindow* gpInitWin;
extern u8 iWallToHexCell[KB_CASTLE_WALL_SEGMENT_COUNT];
extern u8 iTowerToHexCell[KB_CASTLE_TOWER_COUNT];
extern u16 wallPos[KB_CASTLE_WALL_SEGMENT_COUNT][KB_COMBAT_COORDINATE_COUNT];
extern u16 towerPos[KB_CASTLE_TOWER_COUNT][KB_COMBAT_COORDINATE_COUNT];
extern u16 doorPos[KB_CASTLE_DOOR_POSITION_COUNT][KB_COMBAT_COORDINATE_COUNT];
extern float fTradingPostEfficency[KB_TRADING_POST_EFFICIENCY_COUNT];
extern struct SElevationOverlay sElevationOverlay[ELEVATION_OVERLAY_COUNT];
extern b32 gbDrawingPuzzle;
extern i32 giWalkingFrom;
extern i32 giWalkingFrom2;
extern i32 giWalkingTo;
extern i32 giWalkingTo2;
extern i32 giWalkingYMod;
extern u8 moatCell[KB_MOAT_CELL_COUNT];
extern char* congratsText;
extern char* gArtifactNames[KB_ARTIFACT_LEVEL_COUNT];
extern char* gArtifactDesc[KB_ARTIFACT_LEVEL_COUNT];
extern char* gArtifactEvent[KB_ARTIFACT_LEVEL_COUNT];
extern char* gStatNames[HERO_PRIMARY_STAT_COUNT];
extern char* gStatDesc[HERO_PRIMARY_STAT_COUNT];
extern char* gAlignmentNames[KB_ALIGNMENT_NAME_COUNT];
extern char* gTerrainNames[KB_TERRAIN_TYPE_COUNT];
extern char* gResourceNames[RESOURCE_VALUE_COUNT];
extern char* gQuickViewText[KB_QUICK_VIEW_TEXT_COUNT];
extern char* gEventText[KB_EVENT_TEXT_TABLE_COUNT];
extern char* gCPanelHelp[KB_CONTROL_PANEL_HELP_COUNT];
extern char* gCSPanelHelp[KB_COMBAT_SPELL_PANEL_HELP_COUNT];
extern char* gAPanelHelp[KB_ADVENTURE_PANEL_HELP_COUNT];
extern char* gInitMenuHelp[KB_INIT_MENU_HELP_COUNT];
extern char* gAdvMenuHelp[KB_ADVENTURE_MENU_HELP_COUNT];
extern char* gLuckText[KB_LUCK_TEXT_COUNT];
extern char* gMoraleText[KB_MORALE_TEXT_COUNT];
extern char* onOffText[KB_ON_OFF_TEXT_COUNT];
extern char* walkSpeedText[KB_WALK_SPEED_TEXT_COUNT];
extern char* gColors[IDX(FACTION_COUNT)];
extern char* gMonthNames[KB_MONTH_NAME_COUNT];
extern char* gWeekNames[KB_WEEK_NAME_COUNT];
extern char* cHeroScreen[KB_HERO_SCREEN_TEXT_COUNT];
extern char* cCastleInfo[KB_CASTLE_INFO_TEXT_COUNT];
extern char* cLuckInfo[KB_LUCK_INFO_TEXT_COUNT];
extern char* IQnames[KB_IQ_NAME_COUNT];
extern char* cSpellHelp[KB_SPELL_HELP_TEXT_COUNT];
extern char* speedText[KB_SPEED_TEXT_COUNT];
extern char* cArmyDetail[KB_ARMY_DETAIL_TEXT_COUNT];
extern char* cWellDetail[KB_WELL_DETAIL_TEXT_COUNT];
extern char* cKingdomOverview[KB_KINGDOM_OVERVIEW_TEXT_COUNT];
extern char* cNewTurn[KB_NEW_TURN_TEXT_COUNT];
extern char* cViewGeneralLabels[KB_VIEW_GENERAL_LABEL_COUNT];
extern char* cViewGeneralHelp[KB_VIEW_GENERAL_HELP_COUNT];
extern char* cViewGeneralLongHelp[KB_VIEW_GENERAL_LONG_HELP_COUNT];
extern char* cCombatMessage[KB_COMBAT_MESSAGE_COUNT];
extern char* cHeroLevel[KB_HERO_LEVEL_TEXT_COUNT];
extern char* cCombatHelp[KB_COMBAT_HELP_COUNT];
extern char* cLongCombatHelp[KB_LONG_COMBAT_HELP_COUNT];
extern char* cTownCommand[KB_TOWN_COMMAND_COUNT];
extern char* gHeroDefaultNames[KB_HERO_DEFAULT_NAME_COUNT];
extern char* gNewGameHelp[KB_NEW_GAME_HELP_COUNT];
extern char* gSetupBaudHelp[KB_SETUP_BAUD_HELP_COUNT];
extern char* gSetupComPortHelp[KB_SETUP_COM_PORT_HELP_COUNT];
extern char* gSetupDCBaudHelp[KB_SETUP_DC_BAUD_HELP_COUNT];
extern char* gSetupDCComPortHelp[KB_SETUP_DC_COM_PORT_HELP_COUNT];
extern char* gSetupHotSeatGameHelp[KB_SETUP_HOT_SEAT_HELP_COUNT];
extern char* gSetupModemGameHelp[KB_SETUP_MODEM_HELP_COUNT];
extern char* gSetupDCGameHelp[KB_SETUP_DIRECT_CONNECT_HELP_COUNT];
extern char* gSetupMultiPlayerGameHelp[KB_SETUP_MULTIPLAYER_HELP_COUNT];
extern char* gSetupNetworkGameHelp[KB_SETUP_NETWORK_HELP_COUNT];
extern char* gSetupNetworkGame2Help[KB_SETUP_NETWORK_SECOND_HELP_COUNT];
extern char* gSetupGameHelp[KB_SETUP_GAME_HELP_COUNT];
extern char* cBattleResults[KB_BATTLE_RESULT_TEXT_COUNT];
extern char* cMoraleInfo[KB_MORALE_INFO_TEXT_COUNT];
extern char* cMapSize[KB_MAP_SIZE_TEXT_COUNT];
extern char* cDifficulty[KB_DIFFICULTY_TEXT_COUNT];
extern char* cStartDifficulty[KB_START_DIFFICULTY_TEXT_COUNT];
extern char* cCampaignLeaders[KB_CAMPAIGN_LEADER_TEXT_COUNT];
extern char* cWinText[KB_WIN_TEXT_COUNT];
extern char* cHumanDifficulty[KB_HUMAN_DIFFICULTY_TEXT_COUNT];
extern char* cHumanInfoDifficulty[KB_HUMAN_INFO_DIFFICULTY_TEXT_COUNT];
extern char* musicQualityText[KB_MUSIC_QUALITY_TEXT_COUNT];
extern char* gSpellDesc[KB_SPELL_TEXT_COUNT];
extern char* gSpellNames[KB_SPELL_TEXT_COUNT];
extern char* gSecondarySkillLevels[KB_SECONDARY_SKILL_LEVEL_TEXT_COUNT];
extern char* gSecondarySkills[KB_SECONDARY_SKILL_TEXT_COUNT];
extern char* cSecSkillDesc[IDX(HERO_SKILL_COUNT)][SECONDARY_SKILL_VALUE_LEVEL_COUNT];
extern char* cDirections[KB_DIRECTION_TEXT_COUNT];
extern char* cRumourTerrainDescriptions[KB_RUMOUR_TERRAIN_DESCRIPTION_COUNT];
extern char* gInterfaceTypeText[KB_INTERFACE_TYPE_TEXT_COUNT];
extern char* cBWMouseText[KB_BW_MOUSE_TEXT_COUNT];
extern char* combatSpeedText[KB_COMBAT_SPEED_TEXT_COUNT];
extern char* combatMiniInfoText[KB_COMBAT_MINI_INFO_TEXT_COUNT];
extern char* gcCommandLineHelp[KB_COMMAND_LINE_HELP_COUNT];
extern char* cOverviewText[KB_OVERVIEW_TEXT_COUNT];
extern char* cWinComError[KB_WIN_COM_ERROR_TEXT_COUNT];
extern char* cMiniViewText[KB_MINI_VIEW_TEXT_COUNT];
extern char* gFileRequestHelp[KB_FILE_REQUEST_HELP_COUNT];
extern char* cPersonality[KB_PERSONALITY_TEXT_COUNT];
extern char* gArmySizeNames[KB_ARMY_SIZE_NAME_COUNT][KB_ARMY_SIZE_NAME_VARIANT_COUNT];
extern char* cRandomTavernText[KB_RANDOM_TAVERN_TEXT_COUNT];
extern char* cRandomSignText[KB_RANDOM_SIGN_TEXT_COUNT];
extern char* cCampaignAwards[KB_CAMPAIGN_AWARD_TEXT_COUNT];
extern char* cSlowVideoLevelText[KB_SLOW_VIDEO_LEVEL_TEXT_COUNT];
extern char* gSPanelHelp[KB_SETTINGS_PANEL_HELP_COUNT];
extern char* xBarrierColor[KB_BARRIER_COLOR_NAME_COUNT];
extern char* xGenericSiteNames[KB_GENERIC_SITE_NAME_COUNT];
extern char* xRecruitEmpty[X_GLOBAL_RECRUIT_EMPTY_COUNT];
extern char* xRecruitBuy[X_GLOBAL_RECRUIT_BUY_COUNT];
extern char* xRecruitmentSiteNames[KB_RECRUITMENT_SITE_NAME_COUNT];
extern b32 gbHeroWindShowing;
extern b32 gbFullCombatScreenDrawn;
extern b32 gbLimitedCombatUpdatePalette;
extern b32 gbFirstTimeThrough;
extern b32 gbSkipIntro;
extern b32 gbDoMemCheck;
extern b32 gbInCombat;
extern b32 gbDirectConnect;
extern i32 giForceSwitchMusic;
extern b32 gbComputeExtent;
extern b32 gbSaveBiggestExtent;
extern b32 gbLimitToExtent;
extern b32 gbCurrArmyDrawn;
extern i32 gAdvDisposeLevel;
extern i32 giHighScoreRank;
extern i32 giHighScoreType;
extern b32 gbShowHighScore;
extern b32 gbLowMemory;
extern void* gLowPage;
extern b32 gbLowPageGrabbed;
extern i8 xSmackFromNetwork;
extern i32 bKBDone;
extern struct _REDBOOK* hRedbookz;
extern i32 bForceCheckTimeEvent;
extern u16 IMHotSpots[KB_INIT_MENU_HOTSPOT_COUNT][IDX(INIT_MENU_HOTSPOT_FIELD_COUNT)];
extern i32 lastIMHoverID;
extern i32 bInCheckEndGame;
extern b32 gbHitEvent;
extern i32 giMaxExtentX;
extern i32 giMaxExtentY;
extern i32 giRandomClouds;
extern char cOverrideDigitalDriver[GLOBAL_DRIVER_NAME_SIZE];
extern i8 gArmyEffected[COMBAT_CONTROL_SIDE_COUNT][KB_ARMY_EFFECT_COUNT];
extern b32 gbInCampaign;
extern i32 giResExtra1;
extern i32 giResExtra2;
extern i8 puzzlePiecesRemoved[IDX(FACTION_COUNT)];
extern i32 giSeedingValid;
extern i32 giLimitPlayer;
extern i32 giShowClouds;
extern char gcBotViewText[GLOBAL_BOTTOM_VIEW_TEXT_SIZE];
extern i32 giResType1;
extern b32 gbBlackoutPlayer;
extern i32 giResType2;
extern i32 gIndex;
extern i32 giWeekTypeExtra;
extern i32 giTCPType;
extern SpellType gCurLoadedSpellEffect;
extern i32 giCurTempMobility;
extern i32 giOverviewReturnAction;
extern char cOverrideMIDIDriver[GLOBAL_DRIVER_NAME_SIZE];
extern char gMapName[GLOBAL_SHORT_MAP_NAME_SIZE];
extern i32 giMinExtentX;
extern i32 giMinExtentY;
extern MultiplayerBaseType iMPBaseType;
extern b32 gbTCPFirstTime;
extern i32 giHeroScreenSrcIndex;
extern i32 giWeekType;
extern class palette* gpBufferPalette;
extern b32 gbCampaignSideChoice;
extern i32 giMonthTypeExtra;
extern i32 iMPExtendedType;
extern i8 gcColorToSetupPos[RADAR_OWNER_COLOR_COUNT];
extern char gFullMapName[GLOBAL_MAP_NAME_SIZE];
extern char gcTCPName[GLOBAL_TCP_TEXT_SIZE];
extern i32 giShowIntro;
extern i32 giScore;
extern char gcRegAppPath[GLOBAL_AGGREGATE_PATH_SIZE];
extern u32l gTimeMark;
extern char* EXPANSION_AGGREGATE_NAME;
extern i8 xNetHasOldPlayers;
extern char cPlayerNames[X_GLOBAL_PLAYER_COUNT][GLOBAL_PLAYER_NAME_SIZE];
extern class icon* gCurLoadedSpellIcon;
extern u8 bSaveMusicPosition[KB_MUSIC_TRACK_COUNT];
extern char gcTCPAddress[GLOBAL_TCP_TEXT_SIZE];
extern u8 giSetupGameType;
extern char gLastFilename[GLOBAL_AGGREGATE_PATH_SIZE];
extern i32 giFullySeeded;
extern i32 iCombatControlNetPos[COMBAT_CONTROL_SIDE_COUNT];
extern char cExpAggPathName[GLOBAL_AGGREGATE_PATH_SIZE];
extern b32 gbMoveShown;
extern b8 gbSetupGamePosToRealGamePos[RADAR_OWNER_COLOR_COUNT];
extern char gcRegCDRomPath[GLOBAL_AGGREGATE_PATH_SIZE];
extern class heroWindow* heroWin;
extern i32 giOverviewReturnActionExtra;
extern i32 giCurGeneral;
extern b32 gbIconClipOn;
extern b32 gbRemoteGameOpen;
extern i32 giTCPNumPlayers;
extern void* hMainWindow;
extern i32 giCurWindowsStyleFlags;
extern i32 giMonthType;
extern char* DEFAULT_AGGREGATE_NAME;
extern i32 gCurSpellEffectFrame;
extern char cAggPathName[GLOBAL_AGGREGATE_PATH_SIZE];
extern class highScoreManager* gpHighScoreManager;
extern b32 gbIAmGreatest;
extern i32 gMapX;
extern i32 gMapY;
extern char gcWinText[GLOBAL_WINDOW_TEXT_SIZE];
extern i32 bFreshSave;
extern i32 gLowPageScreenSelector;
extern i32 giCurWatchPlayer;
extern char gcCommandLine[GLOBAL_COMMAND_LINE_SIZE];
extern i32 giMapChangeCtr;
extern SMapChange sMapChangeQueue[CURSOR_MAP_CHANGE_QUEUE_COUNT];
extern b32 gbWaitForRemoteReceive;
extern u8 bMusicIsLooping[KB_MUSIC_TRACK_COUNT];

#endif
