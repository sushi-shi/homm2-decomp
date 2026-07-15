#ifndef HOMM2_X_GLOBAL_H
#define HOMM2_X_GLOBAL_H
// Globals owned by this data-only TU (from CodeView).
#include <_types.h>
#include <va.h>
#include <SOURCE/CURSOR.h>
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
    HERO_TYPE_INITIAL_COUNT = 8,
    EXPANSION_HIGH_SCORE_CAMPAIGN_COUNT = 4,
    X_GLOBAL_RECRUIT_EMPTY_COUNT = 6,
    X_GLOBAL_RECRUIT_BUY_COUNT = 5,
    X_GLOBAL_PASSWORD_STRING_COUNT = 211,
    X_GLOBAL_STABLE_TEXT_COUNT = 4,
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
extern int xShrineBuildingCost[];

// ---- globals (declarations, RVA order) ----
extern unsigned char giGroundShape[432];
extern unsigned char gColorTableTan[256];
extern unsigned char gColorTableGray[256];
extern unsigned char gColorTableRed[256];
extern unsigned char gColorTableDarkBrown[256];
extern int giMainVideoModeWidth;
extern int giMainVideoModeHeight;
extern unsigned long glMilliCounter;
extern unsigned char gMapColors[];
extern unsigned char *gObjectColors;
extern unsigned char gOwnerColors[];
extern char *gTilesetFiles[64];
extern unsigned char bPuzzleDraw[64];
#include <SOURCE/dimPalette.h>
extern unsigned char gColorTableLighten[256];
extern unsigned char *gColorTableNoCycle;
extern int gbReturnAfterComputeExtent;
extern int gbAllowTextEntryEscape;
extern int giScreenScroll;
extern int giMenuCommand;
extern int gbSendMouseMoveMessages;
extern int gbColorMice;
extern unsigned long gTownEligibleBuildMask[TOWN_ELIGIBLE_BUILD_MASK_COUNT];
extern unsigned char giMapSizes[4];
extern int gbUseEvilInterface;
extern char *cEvilTranslate[37][2];  // CodeView size 0x128 = 37*2*4; array-decayed mangling
extern char gcAnimPath[0x160];
extern char gcGamePath[0x18];
extern char gcMapPath[0x14];
extern int gbDontTryRedbook;
extern int gbDontTryMIDI;
extern int gbDontTryDigital;
extern float gfCombatSpeedMod[3];
extern int giDialogTimeout;
extern int giNewMonsterCycleFrame;
extern int gbLeaveNetBoxAlone;
extern int gbUseWaveout;
extern int gbShowAllMaps;
extern char *gCombatFxNames[32];
extern short horseFrameFlip[16];
extern short boatFrameFlip[16];
extern signed char *gCastleResources;
extern short *gCastleAmounts;
extern short *gVesaMode;
extern int gInitResourcesHuman[5][7];
extern int gInitResourcesComputer[5][7];
extern int gMineCharacteristics[8];
extern int gSSValues[HERO_SKILL_COUNT][SECONDARY_SKILL_VALUE_LEVEL_COUNT];
extern unsigned char gArtifactLevel[104];
extern int gUltArtifactAvgValue;
extern signed char giVisRangeTown;
extern float gfStatPower[42];
extern float gfBattleStat[42];
extern signed char gSpellLimits[5];
extern float gfSpellCastableCombatMod[12];
extern float gfSpellCastNumMod[12];
extern float gfPhilAISpellPowerMod[12];
extern float gfPhilAIDurationMod[12];
extern float gfSpellTypeNumMod[7];
extern signed char gbArrow[8][8];
extern unsigned char giCloudType[256];
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
extern signed char gHeroSkillBonus[6][2][4];
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
extern char *cMonFilename[66];
extern int gbProcessingCombatAction;
extern int iMPNetProtocol;
extern int iLastDiffSendTo;
extern char *cArmyFrameFileNames[66];
extern unsigned char *gcSpellInfluenceIcons;
extern unsigned char giSpellInfluenceToSpell[16];
extern unsigned char giNumPowFrames[32];
extern int giSpellEffectShowType;
extern signed char gcColorToPlayerPos[8];
extern char *cCombatBkgNames[20];
extern struct SCmbtObstacle sCmbtObstacles[32];
extern int gEstatesGoldLevel[4];
extern float gfSSLogisticsMod[HERO_SKILL_LEVEL_COUNT];
extern float gfSSNavigationMod[HERO_SKILL_LEVEL_COUNT];
extern float gfSSArcheryMod[HERO_SKILL_LEVEL_COUNT];
extern float gfSSAIArcheryMod[4];
extern signed char giVisRange[8];
extern unsigned char gStartingHeroStats[6][5];
extern int giTerrainCost[10][4][2];
extern char *gTownPrefixNames[6];
extern char *gTownObjNames[32];
extern int giDebugBuildingToBuild;
extern unsigned char giTerrainToMusicTrack[16];
extern char *cHeroTypeShortName[6];
extern char cHeroTypeInitial[HERO_TYPE_INITIAL_COUNT];
extern int giDeferObjDrawX;
extern int giDeferObjDrawY;
extern class heroWindow *gpInitWin;
extern struct SCmbtHero sCmbtHero[12];
extern unsigned char iWallToHexCell[4];
extern unsigned char iTowerToHexCell[4];
extern unsigned short wallPos[4][2];
extern unsigned short towerPos[4][2];
extern unsigned short doorPos[2][2];
extern float *fTradingPostEfficency;
extern struct SElevationOverlay sElevationOverlay[ELEVATION_OVERLAY_COUNT];
extern int gbDrawingPuzzle;
extern int giWalkingFrom;
extern int giWalkingFrom2;
extern int giWalkingTo;
extern int giWalkingTo2;
extern int giWalkingYMod;
extern unsigned char moatCell[16];
extern char *congratsText;
extern char *gArtifactNames[104];
extern char *gArtifactDesc[104];
extern char *gArtifactEvent[104];
extern char *gStatNames[4];
extern char *gStatDesc[4];
extern char *gAlignmentNames[8];
extern char *gArmyShortNames[66];
extern char *gTerrainNames[10];
extern char *gResourceNames[8];
extern char *gQuickViewText[124];
extern char *gEventText[96];
extern char *gCPanelHelp[6];
extern char *gCSPanelHelp[8];
extern char *gAPanelHelp[6];
extern char *gInitMenuHelp[6];
extern char *gAdvMenuHelp[8];
extern char *gLuckText[8];
extern char *gMoraleText[8];
extern char *onOffText[12];
extern char *walkSpeedText[6];
extern char *gColors[6];
extern char *gMonthNames[10];
extern char *gWeekNames[16];
extern char *cHeroScreen[26];
extern char *cCastleInfo[16];
extern char *cLuckInfo[22];
extern char *IQnames[6];
extern char *cSpellHelp[10];
extern char *speedText[10];
extern char *cArmyDetail[10];
extern char *cWellDetail[10];
extern char *cKingdomOverview[4];
extern char *cNewTurn[8];
extern char *cViewGeneralLabels[8];
extern char *cViewGeneralHelp[8];
extern char *cViewGeneralLongHelp[4];
extern char *cCombatMessage[12];
extern char *cHeroLevel[4];
extern char *cCombatHelp[6];
extern char *cLongCombatHelp[6];
extern char *cTownCommand[28];
extern char *gHeroDefaultNames[54];
extern char *gNewGameHelp[8];
extern char *gSetupBaudHelp[6];
extern char *gSetupComPortHelp[6];
extern char *gSetupDCBaudHelp[6];
extern char *gSetupDCComPortHelp[6];
extern char *gSetupHotSeatGameHelp[6];
extern char *gSetupModemGameHelp[4];
extern char *gSetupDCGameHelp[4];
extern char *gSetupMultiPlayerGameHelp[6];
extern char *gSetupNetworkGameHelp[4];
extern char *gSetupNetworkGame2Help[4];
extern char *gSetupGameHelp[4];
extern char *cBattleResults[12];
extern char *cMoraleInfo[32];
extern char *cMapSize[4];
extern char *cDifficulty[6];
extern char *cStartDifficulty[4];
extern char *cCampaignLeaders[4];
extern char *cWinText[6];
extern char *cHumanDifficulty[6];
extern char *cHumanInfoDifficulty[6];
extern char *musicQualityText[4];
extern char *gSpellDesc[66];
extern char *gSpellNames[66];
extern char *gSecondarySkillLevels[4];
extern char *gSecondarySkills[14];
extern char *cSecSkillDesc[14][3];
extern char *cDirections[10];
extern char *cRumourTerrainDescriptions[10];
extern char *cRandomTavernText[8];
extern char *gInterfaceTypeText[4];
extern char *cBWMouseText[2];
extern char *combatSpeedText[4];
extern char *combatMiniInfoText[4];
extern char *gcCommandLineHelp[14];
extern char *cOverviewText[6];
extern char *cWinComError[6];
extern char *cMiniViewText[10];
extern char *gFileRequestHelp[16];
extern char *cPersonality[4];
extern char *gArmySizeNames[9][3];
extern char *cRandomTavernText[8];
extern char *cRandomSignText[4];
extern char *cCampaignAwards[12];
extern char *cSlowVideoLevelText[2];
extern char *gSPanelHelp[10];
extern char *xBarrierColor[8];
extern char *xGenericSiteNames[8];
extern char *xRecruitEmpty[X_GLOBAL_RECRUIT_EMPTY_COUNT];
extern char *xRecruitBuy[X_GLOBAL_RECRUIT_BUY_COUNT];
extern char *xRecruitmentSiteNames[6];
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
extern unsigned short IMHotSpots[5][4];
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
