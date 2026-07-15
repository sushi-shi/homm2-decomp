#ifndef HOMM2_X_GLOBAL_H
#define HOMM2_X_GLOBAL_H
// Globals owned by this data-only TU (from CodeView).
#include <_types.h>
#include <va.h>
#include <SOURCE/CURSOR.h>
#include <SOURCE/dimPalette.h>
#include <SOURCE/hero.h>

class ExpCampaign;

typedef enum SecondarySkillValueTableConstant {
    SECONDARY_SKILL_VALUE_LEVEL_COUNT = HERO_SKILL_LEVEL_COUNT - 1
} SecondarySkillValueTableConstant;

typedef enum ElevationOverlayConstant {
    ELEVATION_OVERLAY_COUNT = 25,
    ELEVATION_OVERLAY_CELL_COUNT = 15
} ElevationOverlayConstant;

typedef enum TownEligibleBuildMaskConstant {
    TOWN_ELIGIBLE_BUILD_MASK_COUNT = 6,
    TOWN_ELIGIBLE_BUILD_KNIGHT_MASK = 0x3ff8bf9f,
    TOWN_ELIGIBLE_BUILD_BARBARIAN_MASK = 0x1bf8bf9f,
    TOWN_ELIGIBLE_BUILD_SORCERESS_MASK = 0x0ff8bf9f,
    TOWN_ELIGIBLE_BUILD_WARLOCK_MASK = 0x69f8bf9f,
    TOWN_ELIGIBLE_BUILD_WIZARD_MASK = 0x35f8bf9f,
    TOWN_ELIGIBLE_BUILD_NECROMANCER_MASK = 0x1ff8bf9b
} TownEligibleBuildMaskConstant;

typedef enum CombatControlConstant {
    COMBAT_CONTROL_SIDE_COUNT = 2
} CombatControlConstant;

#pragma pack(push, 1)
struct SCmbtObstacle {
    unsigned int terrainMask;
    unsigned char minimumColumn;
    unsigned char cellCount;
    signed char cellOffsets[8];
};

struct SElevationOverlay {
    unsigned short terrainMask;
    signed char cellOffsets[ELEVATION_OVERLAY_CELL_COUNT];
};
#pragma pack(pop)
SIZE(SCmbtObstacle, 0xe);
SIZE(SElevationOverlay, 0x11);

enum {
    HERO_TYPE_INITIAL_COUNT = HERO_CLASS_COUNT,
    EXPANSION_HIGH_SCORE_CAMPAIGN_COUNT = 4,
    X_GLOBAL_RECRUIT_EMPTY_COUNT = 6,
    X_GLOBAL_RECRUIT_BUY_COUNT = 5,
    X_GLOBAL_PASSWORD_STRING_COUNT = 211,
    X_GLOBAL_STABLE_TEXT_COUNT = 4,
    X_GLOBAL_BUILDING_RESOURCE_COUNT = 7,
    X_GLOBAL_SETUP_HELP_COUNT = 4,
    X_GLOBAL_EXPANSION_CAMPAIGN_COUNT = 4,
    X_GLOBAL_EXPANSION_CAMPAIGN_MAP_COUNT = 8,
    X_GLOBAL_EXPANSION_CAMPAIGN_AWARD_COUNT = 12,
    X_GLOBAL_EXPANSION_HERO_NAME_COUNT = 6,
    X_GLOBAL_SHORT_SKILL_LEVEL_COUNT = 4,
    X_GLOBAL_NEW_HERO_ALIGNMENT_COUNT = 16,
    GLOBAL_MAP_NAME_SIZE = 0x18,
    GLOBAL_TCP_TEXT_SIZE = 0x18,
    GLOBAL_AGGREGATE_PATH_SIZE = 0x160,
    GLOBAL_COMMAND_LINE_SIZE = 0x40
};

// --- globals owned by this TU (moved from _globals.h; CodeView-attributed) ---
extern signed char xIsExpansionMap;
extern int xLastChoice;
extern unsigned char xIsPlayingExpansionCampaign;
extern class ExpCampaign xCampaign;
extern char *xSetupCampaignGameHelp[X_GLOBAL_SETUP_HELP_COUNT];
extern char *xSetupStandardGameHelp[X_GLOBAL_SETUP_HELP_COUNT];
extern char *xCampaignAwards[X_GLOBAL_EXPANSION_CAMPAIGN_AWARD_COUNT];
extern char *xScenarioName[X_GLOBAL_EXPANSION_CAMPAIGN_COUNT]
                          [X_GLOBAL_EXPANSION_CAMPAIGN_MAP_COUNT];
extern char *xScenarioDescription[X_GLOBAL_EXPANSION_CAMPAIGN_COUNT]
                                 [X_GLOBAL_EXPANSION_CAMPAIGN_MAP_COUNT];
extern char *xShortCampaignNames[X_GLOBAL_EXPANSION_CAMPAIGN_COUNT];
extern char *xHSCampaignNames[EXPANSION_HIGH_SCORE_CAMPAIGN_COUNT];
extern int xTheSpell;
extern char *xNecromancerShrine;
extern char *xNecromancerShrineDesc;
extern char *xStableText[X_GLOBAL_STABLE_TEXT_COUNT];
extern char *xJosephName[X_GLOBAL_EXPANSION_HERO_NAME_COUNT];
extern char *xUncleIvanName[X_GLOBAL_EXPANSION_HERO_NAME_COUNT];
extern char *xShortSSLevelNames[X_GLOBAL_SHORT_SKILL_LEVEL_COUNT];
extern char *xPasswordStrings[X_GLOBAL_PASSWORD_STRING_COUNT];
extern unsigned char xNewHeroAlignment[X_GLOBAL_NEW_HERO_ALIGNMENT_COUNT];
extern int xPasswordStringsIndex[];
extern int xShrineBuildingCost[X_GLOBAL_BUILDING_RESOURCE_COUNT];

// ---- globals (declarations, RVA order) ----
typedef enum RadarColorTableCount {
    RADAR_MAP_COLOR_COUNT = 16,
    RADAR_OBJECT_COLOR_COUNT = 16,
    RADAR_OWNER_COLOR_COUNT = 8
} RadarColorTableCount;

typedef enum GroundTerrainType {
    GROUND_TERRAIN_WATER = 0,
    GROUND_TERRAIN_GRASS = 1,
    GROUND_TERRAIN_SNOW = 2,
    GROUND_TERRAIN_SWAMP = 3,
    GROUND_TERRAIN_LAVA = 4,
    GROUND_TERRAIN_DESERT = 5,
    GROUND_TERRAIN_DIRT = 6,
    GROUND_TERRAIN_WASTELAND = 7,
    GROUND_TERRAIN_BEACH = 8,
    GROUND_TERRAIN_TYPE_COUNT = 9
} GroundTerrainType;

typedef enum GroundTableConstant {
    GROUND_TILE_IMAGE_COUNT = 432,
    GROUND_SHAPE_FLIPPED = 0x80
} GroundTableConstant;

typedef enum KbControlTableConstant {
    MOVEMENT_FRAME_FLIP_COUNT = 16,
    CASTLE_RESOURCE_SLOT_COUNT = 8,
    CASTLE_AMOUNT_COUNT = 4,
    VESA_MODE_VALUE_COUNT = 8,
    NORMAL_DIRECTION_COUNT = 8,
    RESOURCE_VALUE_COUNT = 8,
    STARTING_RESOURCE_DIFFICULTY_COUNT = 5,
    STARTING_RESOURCE_TYPE_COUNT = 7,
    MINE_CHARACTERISTIC_COUNT = 8,
    VESA_SET_MODE_FUNCTION = 0x4f02,
    VESA_MODE_640_480_256 = 0x0101
} KbControlTableConstant;

typedef enum KbPaletteTableConstant {
    PUZZLE_DRAW_TABLE_COUNT = 64
} KbPaletteTableConstant;

typedef enum KbGameTableConstant {
    KB_MAP_SIZE_COUNT = 4,
    KB_INTERFACE_TYPE_COUNT = 37,
    KB_INTERFACE_VARIANT_COUNT = 2,
    KB_COMBAT_SPEED_COUNT = 3,
    KB_COMBAT_FX_COUNT = 32,
    KB_ARTIFACT_LEVEL_COUNT = 104,
    KB_ARTIFACT_BASE_VALUE_COUNT = 100,
    KB_STAT_POWER_COUNT = 42,
    KB_SPELL_LIMIT_COUNT = 5,
    KB_SPELL_MOD_COUNT = 12,
    KB_QUICK_COMBAT_SPELL_TYPE_COUNT = 7,
    KB_CLOUD_MASK_COUNT = 256,
    KB_HERO_LEVEL_BAND_COUNT = 2,
    KB_SPELL_INFLUENCE_MAP_COUNT = 16,
    KB_SPELL_EFFECT_COUNT = 32,
    KB_COMBAT_BACKGROUND_COUNT = 20,
    KB_COMBAT_OBSTACLE_COUNT = 32,
    KB_TERRAIN_TYPE_COUNT = GROUND_TERRAIN_TYPE_COUNT + 1,
    KB_TERRAIN_STEP_TYPE_COUNT = 2,
    KB_TRIGGER_TYPE_COUNT = 128,
    KB_TOWN_OBJECT_NAME_COUNT = 32,
    KB_COMBAT_HERO_VARIANT_COUNT = 2,
    KB_COMBAT_HERO_SPRITE_COUNT = HERO_CLASS_COUNT * KB_COMBAT_HERO_VARIANT_COUNT,
    KB_CASTLE_WALL_SEGMENT_COUNT = 4,
    KB_CASTLE_TOWER_COUNT = 4,
    KB_CASTLE_DOOR_POSITION_COUNT = 2,
    KB_COMBAT_COORDINATE_COUNT = 2,
    KB_TRADING_POST_EFFICIENCY_COUNT = 11,
    KB_MOAT_CELL_COUNT = 9,
    KB_ALIGNMENT_NAME_COUNT = HERO_CLASS_COUNT + 2,
    KB_QUICK_VIEW_TEXT_COUNT = 124,
    KB_EVENT_TEXT_TABLE_COUNT = 96,
    KB_CONTROL_PANEL_HELP_COUNT = 6,
    KB_COMBAT_SPELL_PANEL_HELP_COUNT = 8,
    KB_ADVENTURE_PANEL_HELP_COUNT = 6,
    KB_INIT_MENU_HELP_COUNT = 6,
    KB_ADVENTURE_MENU_HELP_COUNT = 8,
    KB_LUCK_TEXT_COUNT = 8,
    KB_MORALE_TEXT_COUNT = 8,
    KB_ON_OFF_TEXT_COUNT = 12,
    KB_WALK_SPEED_TEXT_COUNT = 6,
    KB_MONTH_NAME_COUNT = 10,
    KB_WEEK_NAME_COUNT = 16,
    KB_HERO_SCREEN_TEXT_COUNT = 26,
    KB_CASTLE_INFO_TEXT_COUNT = 16,
    KB_LUCK_INFO_TEXT_COUNT = 22,
    KB_IQ_NAME_COUNT = 6,
    KB_SPELL_HELP_TEXT_COUNT = 10,
    KB_SPEED_TEXT_COUNT = 10,
    KB_ARMY_DETAIL_TEXT_COUNT = 10,
    KB_WELL_DETAIL_TEXT_COUNT = 10,
    KB_KINGDOM_OVERVIEW_TEXT_COUNT = 4,
    KB_NEW_TURN_TEXT_COUNT = 8,
    KB_VIEW_GENERAL_LABEL_COUNT = 8,
    KB_VIEW_GENERAL_HELP_COUNT = 8,
    KB_VIEW_GENERAL_LONG_HELP_COUNT = 4,
    KB_COMBAT_MESSAGE_COUNT = 12,
    KB_HERO_LEVEL_TEXT_COUNT = 4,
    KB_COMBAT_HELP_COUNT = 6,
    KB_LONG_COMBAT_HELP_COUNT = 6,
    KB_NEW_GAME_HELP_COUNT = 8,
    KB_SETUP_BAUD_HELP_COUNT = 6,
    KB_SETUP_COM_PORT_HELP_COUNT = 6,
    KB_SETUP_DC_BAUD_HELP_COUNT = 6,
    KB_SETUP_DC_COM_PORT_HELP_COUNT = 6,
    KB_SETUP_HOT_SEAT_HELP_COUNT = 6,
    KB_SETUP_MODEM_HELP_COUNT = 4,
    KB_SETUP_DIRECT_CONNECT_HELP_COUNT = 4,
    KB_SETUP_MULTIPLAYER_HELP_COUNT = 6,
    KB_SETUP_NETWORK_HELP_COUNT = 4,
    KB_SETUP_NETWORK_SECOND_HELP_COUNT = 4,
    KB_SETUP_GAME_HELP_COUNT = 4,
    KB_BATTLE_RESULT_TEXT_COUNT = 12,
    KB_MORALE_INFO_TEXT_COUNT = 32,
    KB_MAP_SIZE_TEXT_COUNT = 4,
    KB_DIFFICULTY_TEXT_COUNT = 6,
    KB_START_DIFFICULTY_TEXT_COUNT = 4,
    KB_CAMPAIGN_LEADER_TEXT_COUNT = 4,
    KB_WIN_TEXT_COUNT = 6,
    KB_HUMAN_DIFFICULTY_TEXT_COUNT = 6,
    KB_HUMAN_INFO_DIFFICULTY_TEXT_COUNT = 6,
    KB_MUSIC_QUALITY_TEXT_COUNT = 4,
    KB_SPELL_TEXT_COUNT = HERO_SPELL_COUNT + 1,
    KB_SECONDARY_SKILL_LEVEL_TEXT_COUNT = HERO_SKILL_LEVEL_COUNT,
    KB_SECONDARY_SKILL_TEXT_COUNT = HERO_SKILL_COUNT,
    KB_NEUTRAL_BUILDING_TEXT_COUNT = 20,
    KB_WELL_EXTRA_NAME_COUNT = 8,
    KB_SPECIAL_BUILDING_NAME_COUNT = 8,
    KB_NEUTRAL_BUILDING_INFO_COUNT = 20,
    KB_SPECIAL_BUILDING_INFO_COUNT = HERO_CLASS_COUNT,
    KB_DIRECTION_TEXT_COUNT = 10,
    KB_RUMOUR_TERRAIN_DESCRIPTION_COUNT = KB_TERRAIN_TYPE_COUNT,
    KB_INTERFACE_TYPE_TEXT_COUNT = 4,
    KB_BW_MOUSE_TEXT_COUNT = 2,
    KB_COMBAT_SPEED_TEXT_COUNT = KB_COMBAT_SPEED_COUNT + 1,
    KB_COMBAT_MINI_INFO_TEXT_COUNT = 4,
    KB_COMMAND_LINE_HELP_COUNT = 14,
    KB_OVERVIEW_TEXT_COUNT = 6,
    KB_WIN_COM_ERROR_TEXT_COUNT = 6,
    KB_MINI_VIEW_TEXT_COUNT = 10,
    KB_FILE_REQUEST_HELP_COUNT = 16,
    KB_PERSONALITY_TEXT_COUNT = 4,
    KB_ARMY_SIZE_NAME_COUNT = 9,
    KB_ARMY_SIZE_NAME_VARIANT_COUNT = 3,
    KB_RANDOM_TAVERN_TEXT_COUNT = 8,
    KB_RANDOM_SIGN_TEXT_COUNT = 4,
    KB_CAMPAIGN_AWARD_TEXT_COUNT = 12,
    KB_SLOW_VIDEO_LEVEL_TEXT_COUNT = 2,
    KB_SETTINGS_PANEL_HELP_COUNT = 10,
    KB_BARRIER_COLOR_NAME_COUNT = 8,
    KB_GENERIC_SITE_NAME_COUNT = 8,
    KB_RECRUITMENT_SITE_NAME_COUNT = 6,
    KB_WIN_SETUP_COUNT = 73,
    KB_INIT_MENU_HOTSPOT_COUNT = 5
} KbGameTableConstant;

typedef enum InitMenuHotSpotField {
    INIT_MENU_HOTSPOT_X = 0,
    INIT_MENU_HOTSPOT_Y = 1,
    INIT_MENU_HOTSPOT_WIDTH = 2,
    INIT_MENU_HOTSPOT_HEIGHT = 3,
    INIT_MENU_HOTSPOT_FIELD_COUNT = 4
} InitMenuHotSpotField;

extern unsigned char giGroundShape[GROUND_TILE_IMAGE_COUNT];
extern unsigned char gColorTableTan[DIM_PALETTE_COLOR_COUNT];
extern unsigned char gColorTableGray[DIM_PALETTE_COLOR_COUNT];
extern unsigned char gColorTableRed[DIM_PALETTE_COLOR_COUNT];
extern unsigned char gColorTableDarkBrown[DIM_PALETTE_COLOR_COUNT];
extern int giMainVideoModeWidth;
extern int giMainVideoModeHeight;
extern unsigned long glMilliCounter;
extern unsigned char gMapColors[RADAR_MAP_COLOR_COUNT];
extern unsigned char gObjectColors[RADAR_OBJECT_COLOR_COUNT];
extern unsigned char gOwnerColors[RADAR_OWNER_COLOR_COUNT];
extern char *gTilesetFiles[64];
extern unsigned char bPuzzleDraw[PUZZLE_DRAW_TABLE_COUNT];
extern unsigned char gColorTableLighten[DIM_PALETTE_COLOR_COUNT];
extern unsigned char gColorTableNoCycle[DIM_PALETTE_COLOR_COUNT];
extern int gbReturnAfterComputeExtent;
extern int gbAllowTextEntryEscape;
extern int giScreenScroll;
extern int giMenuCommand;
extern int gbSendMouseMoveMessages;
extern int gbColorMice;
extern unsigned long gTownEligibleBuildMask[TOWN_ELIGIBLE_BUILD_MASK_COUNT];
extern unsigned char giMapSizes[KB_MAP_SIZE_COUNT];
extern int gbUseEvilInterface;
extern char *cEvilTranslate[KB_INTERFACE_TYPE_COUNT][KB_INTERFACE_VARIANT_COUNT];
extern char gcAnimPath[0x160];
extern char gcGamePath[0x18];
extern char gcMapPath[0x14];
extern int gbDontTryRedbook;
extern int gbDontTryMIDI;
extern int gbDontTryDigital;
extern float gfCombatSpeedMod[KB_COMBAT_SPEED_COUNT];
extern int giDialogTimeout;
extern int giNewMonsterCycleFrame;
extern int gbLeaveNetBoxAlone;
extern int gbUseWaveout;
extern int gbShowAllMaps;
extern char *gCombatFxNames[KB_COMBAT_FX_COUNT];
extern short horseFrameFlip[MOVEMENT_FRAME_FLIP_COUNT];
extern short boatFrameFlip[MOVEMENT_FRAME_FLIP_COUNT];
extern signed char gCastleResources[CASTLE_RESOURCE_SLOT_COUNT];
extern short gCastleAmounts[CASTLE_AMOUNT_COUNT];
extern short gVesaMode[VESA_MODE_VALUE_COUNT];
extern int gInitResourcesHuman[STARTING_RESOURCE_DIFFICULTY_COUNT]
                              [STARTING_RESOURCE_TYPE_COUNT];
extern int gInitResourcesComputer[STARTING_RESOURCE_DIFFICULTY_COUNT]
                                 [STARTING_RESOURCE_TYPE_COUNT];
extern int gMineCharacteristics[MINE_CHARACTERISTIC_COUNT];
extern int gSSValues[HERO_SKILL_COUNT][SECONDARY_SKILL_VALUE_LEVEL_COUNT];
extern unsigned char gArtifactLevel[KB_ARTIFACT_LEVEL_COUNT];
extern int gUltArtifactAvgValue;
extern signed char giVisRangeTown;
extern float gfStatPower[KB_STAT_POWER_COUNT];
extern float gfBattleStat[KB_STAT_POWER_COUNT];
extern signed char gSpellLimits[KB_SPELL_LIMIT_COUNT];
extern float gfSpellCastableCombatMod[KB_SPELL_MOD_COUNT];
extern float gfSpellCastNumMod[KB_SPELL_MOD_COUNT];
extern float gfPhilAISpellPowerMod[KB_SPELL_MOD_COUNT];
extern float gfPhilAIDurationMod[KB_SPELL_MOD_COUNT];
extern float gfSpellTypeNumMod[KB_QUICK_COMBAT_SPELL_TYPE_COUNT];
extern signed char gbArrow[NORMAL_DIRECTION_COUNT][NORMAL_DIRECTION_COUNT];
extern unsigned char giCloudType[KB_CLOUD_MASK_COUNT];
typedef enum TownMusicTrack {
    TOWN_MUSIC_NONE = 0,
    TOWN_MUSIC_WARLOCK = 5,
    TOWN_MUSIC_WIZARD = 6,
    TOWN_MUSIC_NECROMANCER = 7,
    TOWN_MUSIC_KNIGHT = 8,
    TOWN_MUSIC_BARBARIAN = 9,
    TOWN_MUSIC_SORCERESS = 10,
    TOWN_MUSIC_TABLE_SIZE = 8
} TownMusicTrack;

extern signed char townTheme[TOWN_MUSIC_TABLE_SIZE];
extern signed char gHeroSkillBonus[HERO_CLASS_COUNT][KB_HERO_LEVEL_BAND_COUNT]
                                  [HERO_PRIMARY_STAT_COUNT];
extern int giMonoIconSkip;
extern int giScrollX;
extern int giScrollY;
extern int gbNoBorder;
extern int gbEnlargeScreenBlit;
extern int giCurExe;
extern int gbInDialog;
extern struct SMenuEnableStatus gsMenuEnableStatus[MENU_ENABLE_STATUS_COUNT];
extern int gbInSetupDialog;
extern int gbMinimized;
extern int gbInSmackMgr;
extern int glBottomRefresh;
extern int gbBothMachinesWin95;
extern int gbGotFirstHeartbeat;
extern char *cMonFilename[MONSTER_DATABASE_COUNT];
extern int gbProcessingCombatAction;
extern int iMPNetProtocol;
extern int iLastDiffSendTo;
extern char *cArmyFrameFileNames[MONSTER_DATABASE_COUNT];
extern unsigned char gcSpellInfluenceIcons[KB_SPELL_INFLUENCE_MAP_COUNT];
extern unsigned char giSpellInfluenceToSpell[KB_SPELL_INFLUENCE_MAP_COUNT];
extern unsigned char giNumPowFrames[KB_SPELL_EFFECT_COUNT];
extern int giSpellEffectShowType;
extern signed char gcColorToPlayerPos[RADAR_OWNER_COLOR_COUNT];
extern char *cCombatBkgNames[KB_COMBAT_BACKGROUND_COUNT];
extern struct SCmbtObstacle sCmbtObstacles[KB_COMBAT_OBSTACLE_COUNT];
extern int gEstatesGoldLevel[HERO_SKILL_LEVEL_COUNT];
extern float gfSSLogisticsMod[HERO_SKILL_LEVEL_COUNT];
extern float gfSSNavigationMod[HERO_SKILL_LEVEL_COUNT];
extern float gfSSArcheryMod[HERO_SKILL_LEVEL_COUNT];
extern float gfSSAIArcheryMod[HERO_SKILL_LEVEL_COUNT];
extern signed char giVisRange[HERO_SKILL_LEVEL_COUNT];
extern unsigned char gStartingHeroStats[HERO_CLASS_COUNT][HERO_STARTING_STAT_COUNT];
extern int giTerrainCost[KB_TERRAIN_TYPE_COUNT][HERO_SKILL_LEVEL_COUNT]
                        [KB_TERRAIN_STEP_TYPE_COUNT];
extern char *gTownPrefixNames[HERO_CLASS_COUNT];
extern char *gTownObjNames[KB_TOWN_OBJECT_NAME_COUNT];
extern int giDebugBuildingToBuild;
extern unsigned char giTerrainToMusicTrack[GROUND_TERRAIN_TYPE_COUNT];
extern char *cHeroTypeShortName[HERO_CLASS_COUNT];
extern char cHeroTypeInitial[HERO_TYPE_INITIAL_COUNT];
extern int giDeferObjDrawX;
extern int giDeferObjDrawY;
extern class heroWindow *gpInitWin;
extern struct SCmbtHero sCmbtHero[KB_COMBAT_HERO_SPRITE_COUNT];
extern unsigned char iWallToHexCell[KB_CASTLE_WALL_SEGMENT_COUNT];
extern unsigned char iTowerToHexCell[KB_CASTLE_TOWER_COUNT];
extern unsigned short wallPos[KB_CASTLE_WALL_SEGMENT_COUNT][KB_COMBAT_COORDINATE_COUNT];
extern unsigned short towerPos[KB_CASTLE_TOWER_COUNT][KB_COMBAT_COORDINATE_COUNT];
extern unsigned short doorPos[KB_CASTLE_DOOR_POSITION_COUNT][KB_COMBAT_COORDINATE_COUNT];
extern float fTradingPostEfficency[KB_TRADING_POST_EFFICIENCY_COUNT];
extern struct SElevationOverlay sElevationOverlay[ELEVATION_OVERLAY_COUNT];
extern int gbDrawingPuzzle;
extern int giWalkingFrom;
extern int giWalkingFrom2;
extern int giWalkingTo;
extern int giWalkingTo2;
extern int giWalkingYMod;
extern unsigned char moatCell[KB_MOAT_CELL_COUNT];
extern char *congratsText;
extern char *gArtifactNames[KB_ARTIFACT_LEVEL_COUNT];
extern char *gArtifactDesc[KB_ARTIFACT_LEVEL_COUNT];
extern char *gArtifactEvent[KB_ARTIFACT_LEVEL_COUNT];
extern char *gStatNames[HERO_PRIMARY_STAT_COUNT];
extern char *gStatDesc[HERO_PRIMARY_STAT_COUNT];
extern char *gAlignmentNames[KB_ALIGNMENT_NAME_COUNT];
extern char *gArmyShortNames[MONSTER_DATABASE_COUNT];
extern char *gTerrainNames[KB_TERRAIN_TYPE_COUNT];
extern char *gResourceNames[RESOURCE_VALUE_COUNT];
extern char *gQuickViewText[KB_QUICK_VIEW_TEXT_COUNT];
extern char *gEventText[KB_EVENT_TEXT_TABLE_COUNT];
extern char *gCPanelHelp[KB_CONTROL_PANEL_HELP_COUNT];
extern char *gCSPanelHelp[KB_COMBAT_SPELL_PANEL_HELP_COUNT];
extern char *gAPanelHelp[KB_ADVENTURE_PANEL_HELP_COUNT];
extern char *gInitMenuHelp[KB_INIT_MENU_HELP_COUNT];
extern char *gAdvMenuHelp[KB_ADVENTURE_MENU_HELP_COUNT];
extern char *gLuckText[KB_LUCK_TEXT_COUNT];
extern char *gMoraleText[KB_MORALE_TEXT_COUNT];
extern char *onOffText[KB_ON_OFF_TEXT_COUNT];
extern char *walkSpeedText[KB_WALK_SPEED_TEXT_COUNT];
extern char *gColors[HERO_CLASS_COUNT];
extern char *gMonthNames[KB_MONTH_NAME_COUNT];
extern char *gWeekNames[KB_WEEK_NAME_COUNT];
extern char *cHeroScreen[KB_HERO_SCREEN_TEXT_COUNT];
extern char *cCastleInfo[KB_CASTLE_INFO_TEXT_COUNT];
extern char *cLuckInfo[KB_LUCK_INFO_TEXT_COUNT];
extern char *IQnames[KB_IQ_NAME_COUNT];
extern char *cSpellHelp[KB_SPELL_HELP_TEXT_COUNT];
extern char *speedText[KB_SPEED_TEXT_COUNT];
extern char *cArmyDetail[KB_ARMY_DETAIL_TEXT_COUNT];
extern char *cWellDetail[KB_WELL_DETAIL_TEXT_COUNT];
extern char *cKingdomOverview[KB_KINGDOM_OVERVIEW_TEXT_COUNT];
extern char *cNewTurn[KB_NEW_TURN_TEXT_COUNT];
extern char *cViewGeneralLabels[KB_VIEW_GENERAL_LABEL_COUNT];
extern char *cViewGeneralHelp[KB_VIEW_GENERAL_HELP_COUNT];
extern char *cViewGeneralLongHelp[KB_VIEW_GENERAL_LONG_HELP_COUNT];
extern char *cCombatMessage[KB_COMBAT_MESSAGE_COUNT];
extern char *cHeroLevel[KB_HERO_LEVEL_TEXT_COUNT];
extern char *cCombatHelp[KB_COMBAT_HELP_COUNT];
extern char *cLongCombatHelp[KB_LONG_COMBAT_HELP_COUNT];
extern char *cTownCommand[28];
extern char *gHeroDefaultNames[54];
extern char *gNewGameHelp[KB_NEW_GAME_HELP_COUNT];
extern char *gSetupBaudHelp[KB_SETUP_BAUD_HELP_COUNT];
extern char *gSetupComPortHelp[KB_SETUP_COM_PORT_HELP_COUNT];
extern char *gSetupDCBaudHelp[KB_SETUP_DC_BAUD_HELP_COUNT];
extern char *gSetupDCComPortHelp[KB_SETUP_DC_COM_PORT_HELP_COUNT];
extern char *gSetupHotSeatGameHelp[KB_SETUP_HOT_SEAT_HELP_COUNT];
extern char *gSetupModemGameHelp[KB_SETUP_MODEM_HELP_COUNT];
extern char *gSetupDCGameHelp[KB_SETUP_DIRECT_CONNECT_HELP_COUNT];
extern char *gSetupMultiPlayerGameHelp[KB_SETUP_MULTIPLAYER_HELP_COUNT];
extern char *gSetupNetworkGameHelp[KB_SETUP_NETWORK_HELP_COUNT];
extern char *gSetupNetworkGame2Help[KB_SETUP_NETWORK_SECOND_HELP_COUNT];
extern char *gSetupGameHelp[KB_SETUP_GAME_HELP_COUNT];
extern char *cBattleResults[KB_BATTLE_RESULT_TEXT_COUNT];
extern char *cMoraleInfo[KB_MORALE_INFO_TEXT_COUNT];
extern char *cMapSize[KB_MAP_SIZE_TEXT_COUNT];
extern char *cDifficulty[KB_DIFFICULTY_TEXT_COUNT];
extern char *cStartDifficulty[KB_START_DIFFICULTY_TEXT_COUNT];
extern char *cCampaignLeaders[KB_CAMPAIGN_LEADER_TEXT_COUNT];
extern char *cWinText[KB_WIN_TEXT_COUNT];
extern char *cHumanDifficulty[KB_HUMAN_DIFFICULTY_TEXT_COUNT];
extern char *cHumanInfoDifficulty[KB_HUMAN_INFO_DIFFICULTY_TEXT_COUNT];
extern char *musicQualityText[KB_MUSIC_QUALITY_TEXT_COUNT];
extern char *gSpellDesc[KB_SPELL_TEXT_COUNT];
extern char *gSpellNames[KB_SPELL_TEXT_COUNT];
extern char *gSecondarySkillLevels[KB_SECONDARY_SKILL_LEVEL_TEXT_COUNT];
extern char *gSecondarySkills[KB_SECONDARY_SKILL_TEXT_COUNT];
extern char *cSecSkillDesc[HERO_SKILL_COUNT][SECONDARY_SKILL_VALUE_LEVEL_COUNT];
extern char *cDirections[KB_DIRECTION_TEXT_COUNT];
extern char *cRumourTerrainDescriptions[KB_RUMOUR_TERRAIN_DESCRIPTION_COUNT];
extern char *gInterfaceTypeText[KB_INTERFACE_TYPE_TEXT_COUNT];
extern char *cBWMouseText[KB_BW_MOUSE_TEXT_COUNT];
extern char *combatSpeedText[KB_COMBAT_SPEED_TEXT_COUNT];
extern char *combatMiniInfoText[KB_COMBAT_MINI_INFO_TEXT_COUNT];
extern char *gcCommandLineHelp[KB_COMMAND_LINE_HELP_COUNT];
extern char *cOverviewText[KB_OVERVIEW_TEXT_COUNT];
extern char *cWinComError[KB_WIN_COM_ERROR_TEXT_COUNT];
extern char *cMiniViewText[KB_MINI_VIEW_TEXT_COUNT];
extern char *gFileRequestHelp[KB_FILE_REQUEST_HELP_COUNT];
extern char *cPersonality[KB_PERSONALITY_TEXT_COUNT];
extern char *gArmySizeNames[KB_ARMY_SIZE_NAME_COUNT][KB_ARMY_SIZE_NAME_VARIANT_COUNT];
extern char *cRandomTavernText[KB_RANDOM_TAVERN_TEXT_COUNT];
extern char *cRandomSignText[KB_RANDOM_SIGN_TEXT_COUNT];
extern char *cCampaignAwards[KB_CAMPAIGN_AWARD_TEXT_COUNT];
extern char *cSlowVideoLevelText[KB_SLOW_VIDEO_LEVEL_TEXT_COUNT];
extern char *gSPanelHelp[KB_SETTINGS_PANEL_HELP_COUNT];
extern char *xBarrierColor[KB_BARRIER_COLOR_NAME_COUNT];
extern char *xGenericSiteNames[KB_GENERIC_SITE_NAME_COUNT];
extern char *xRecruitEmpty[X_GLOBAL_RECRUIT_EMPTY_COUNT];
extern char *xRecruitBuy[X_GLOBAL_RECRUIT_BUY_COUNT];
extern char *xRecruitmentSiteNames[KB_RECRUITMENT_SITE_NAME_COUNT];
extern int gbHeroWindShowing;
extern int gbFullCombatScreenDrawn;
extern int gbLimitedCombatUpdatePalette;
extern int gbFirstTimeThrough;
extern int gbSkipIntro;
extern int gbDoMemCheck;
extern int gbInCombat;
extern int gbDirectConnect;
extern int giForceSwitchMusic;
extern int gbComputeExtent;
extern int gbSaveBiggestExtent;
extern int gbLimitToExtent;
extern int gbCurrArmyDrawn;
extern int gAdvDisposeLevel;
extern int giHighScoreRank;
extern int giHighScoreType;
extern int gbShowHighScore;
extern int gbLowMemory;
extern void *gLowPage;
extern int gbLowPageGrabbed;
extern signed char xSmackFromNetwork;
extern int bKBDone;
extern struct _REDBOOK *hRedbookz;
extern int bForceCheckTimeEvent;
extern unsigned short
    IMHotSpots[KB_INIT_MENU_HOTSPOT_COUNT][INIT_MENU_HOTSPOT_FIELD_COUNT];
extern int lastIMHoverID;
extern int bInCheckEndGame;
extern int gbHitEvent;
extern int giMaxExtentX;
extern int giMaxExtentY;
extern int giRandomClouds;
extern char *cOverrideDigitalDriver;
extern signed char gArmyEffected[2][20];
extern int gbInCampaign;
extern int giResExtra1;
extern int giResExtra2;
extern signed char puzzlePiecesRemoved[6];
extern int giSeedingValid;
extern int giLimitPlayer;
extern int giShowClouds;
extern char *gcBotViewText;
extern int giResType1;
extern int gbBlackoutPlayer;
extern int giResType2;
extern int gIndex;
extern int giWeekTypeExtra;
extern int giTCPType;
extern int gCurLoadedSpellEffect;
extern int giCurTempMobility;
extern int giOverviewReturnAction;
extern char *cOverrideMIDIDriver;
extern char gMapName[16];
extern int giMinExtentX;
extern int giMinExtentY;
extern int iMPBaseType;
extern int gbTCPFirstTime;
extern int giHeroScreenSrcIndex;
extern int giWeekType;
extern class palette *gpBufferPalette;
extern int gbCampaignSideChoice;
extern int giMonthTypeExtra;
extern int iMPExtendedType;
extern signed char gcColorToSetupPos[8];
extern char gFullMapName[GLOBAL_MAP_NAME_SIZE];
extern char gcTCPName[GLOBAL_TCP_TEXT_SIZE];
extern int giShowIntro;
extern int giScore;
extern char gcRegAppPath[0x160];  // CodeView size 0x160; typed char* (array-decayed)
extern unsigned long gTimeMark;
extern char *EXPANSION_AGGREGATE_NAME;
extern signed char xNetHasOldPlayers;
extern char cPlayerNames[6][21];
extern class icon *gCurLoadedSpellIcon;
extern unsigned char bSaveMusicPosition[0x3c];
extern char gcTCPAddress[GLOBAL_TCP_TEXT_SIZE];
extern unsigned char giSetupGameType;
extern char gLastFilename[GLOBAL_AGGREGATE_PATH_SIZE];
extern int giFullySeeded;
extern int iCombatControlNetPos[COMBAT_CONTROL_SIDE_COUNT];
extern char cExpAggPathName[GLOBAL_AGGREGATE_PATH_SIZE];
extern int gbMoveShown;
extern signed char gbSetupGamePosToRealGamePos[8];
extern char gcRegCDRomPath[0x160];  // CodeView size 0x160; typed char* (array-decayed)
extern class heroWindow *heroWin;
extern int giOverviewReturnActionExtra;
extern int giCurGeneral;
extern int gbIconClipOn;
extern int gbRemoteGameOpen;
extern int giTCPNumPlayers;
extern void *hMainWindow;
extern int giCurWindowsStyleFlags;
extern int giMonthType;
extern char *DEFAULT_AGGREGATE_NAME;
extern int gCurSpellEffectFrame;
extern char cAggPathName[GLOBAL_AGGREGATE_PATH_SIZE];
extern class highScoreManager *gpHighScoreManager;
extern int gbIAmGreatest;
extern int gMapX;
extern int gMapY;
extern char *gcWinText;
extern int bFreshSave;
extern int gLowPageScreenSelector;
extern int giCurWatchPlayer;
extern char gcCommandLine[GLOBAL_COMMAND_LINE_SIZE];
extern int giMapChangeCtr;
extern SMapChange sMapChangeQueue[CURSOR_MAP_CHANGE_QUEUE_COUNT];
extern int gbWaitForRemoteReceive;
extern unsigned char bMusicIsLooping[0x3c];

#endif // HOMM2_X_GLOBAL_H
