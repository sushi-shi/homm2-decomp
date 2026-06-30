// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\KB.OBJ   from: (directly linked into exe)
// functions: 71   data: 441
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>

struct tag_message;
int EventWindowHandler(struct tag_message &);

VA(0x00496450, 0x14e)
// @PollSound@0;

VA(0x0049659e, 0x20)
// void ForcePollSound(void);

VA(0x004965be, 0x39e)
// void InitMainClasses(void);

VA(0x0049695c, 0x344)
// void DeleteMainClasses(void);

VA(0x00496ca0, 0x39)
// void EarlyShutdown(char *, char *);

VA(0x00496cd9, 0x148)
// void SetupCDRom(void);

VA(0x00496e21, 0x77)
// int EarlySetup(void);

VA(0x00496e98, 0x16c0)
// int oldmain(void);

VA(0x00498558, 0x44)
// char toupper(char);

VA(0x0049859c, 0x791)
// int InterpretCommandLine(void);

VA(0x00498d2d, 0x698)
// int InitMenuHandler(struct tag_message &);

VA(0x004993c5, 0x1b)
int NullHandler(struct tag_message &msg)
{
    return 1;
}

VA(0x004993e0, 0x1a9)
// int RecruitHeroHandler(struct tag_message &);

VA(0x00499589, 0x1a7)
// char * GetBuildingInfo(int, int, int);

VA(0x00499730, 0xa4)
// char * GetBuildingName(int, int);

VA(0x004997d4, 0x138)
// void GetBuildingCost(int, int, int * const, int);

VA(0x0049990c, 0x20)
// char * GetMonsterName(int);

VA(0x0049992c, 0x140)
// void GetMonsterCost(int, int * const);

VA(0x00499a6c, 0x2b5)
// int CanBuild(class town *, int);

VA(0x00499d21, 0x9a)
// int CanBuy(class town *, int);

VA(0x00499dbb, 0xc6)
// int GetBuildingBaseResourceValue(int, int, int);

VA(0x00499e81, 0x21e)
// int WaitHandler(struct tag_message &);

VA(0x0049a09f, 0x472)
// int EventWindowHandler(struct tag_message &);

VA(0x0049a511, 0x1e)
int TrueFalseDialogHandler(struct tag_message &msg)
{
    return EventWindowHandler(msg);
}

VA(0x0049a52f, 0x192)
// void PlayerDead(int);

VA(0x0049a6c1, 0x19bb)
// void CheckEndGame(int, int);

VA(0x0049c07c, 0x95)
// void QuickViewWait(void);

VA(0x0049c111, 0x201)
// void InitVars(void);

VA(0x0049c312, 0x61b)
// void game::ShowMoraleInfo(class hero *, int);

VA(0x0049c92d, 0x371)
// void game::ShowLuckInfo(class hero *, int);

VA(0x0049cc9e, 0xd7)
// void ClearMapExtra(void);

VA(0x0049cd75, 0x9f)
// int GetMonType(int, int);

VA(0x0049ce14, 0x4ac)
// int AddScoreToHighScore(int, int, int, int, char *);

VA(0x0049d2c0, 0x66)
// void BVResMsg(char *, int, int);

VA(0x0049d326, 0x2d)
// void GOut(char *);

VA(0x0049d353, 0x54)
// int NetPosToGamePos(int);

VA(0x0049d3a7, 0xff)
// int WaitForOtherPlayer(void);

VA(0x0049d4a6, 0xb85)
// void PopNetBox(char *, int);

VA(0x0049e02b, 0xc7)
// void AddNetBoxLine(char *, char);

VA(0x0049e0f2, 0x214)
// void ShutDown(char *);

VA(0x0049e306, 0xa2)
// void FileError(char *);

VA(0x0049e3a8, 0x255)
// void SmackFade(unsigned char *, unsigned char *);

VA(0x0049e5fd, 0x303)
// void ShowCongrats(int);

VA(0x0049e900, 0x99)
// void CongratsWait(void);

VA(0x0049e999, 0x54)
// struct SAMPLE2 LoadPlaySample(char *);

VA(0x0049e9ed, 0x8f)
// void WaitEndSample(struct SAMPLE2, int);

VA(0x0049ea7c, 0x5d)
// void MemError(void);

VA(0x0049ead9, 0x3b)
// char * GetTownName(int);

VA(0x0049eb14, 0x3a)
// void LoadSystemwideIcons(void);

VA(0x0049eb4e, 0x32)
// void UnloadSystemwideIcons(void);

VA(0x0049eb80, 0x10)
void EarlyShutDownSystem(void)
{
}

VA(0x0049eb90, 0x75)
// int GameUnsaved(void);

VA(0x0049ec05, 0xa18)
// int HandleAppSpecificMenuCommands(int);

VA(0x0049f61d, 0x310)
// void UpdateSystemOptionsMenu(void);

VA(0x0049f92d, 0x99)
// void CleanUpMenus(void);

VA(0x0049f9c6, 0x2a)
// void UpdateAppSpecificMenus(void *);

VA(0x0049f9f0, 0x2d)
// void EarlyResizeWindow(int, int, int, int);

VA(0x0049fa1d, 0x53)
// int InMapArea(int, int);

VA(0x0049fa70, 0x6bc)
// void SetupDynamicWindow(int, int, int, int, int, int, int, int *, int *, int *, int *, int *, int *, class heroWindow * *, int);

VA(0x004a012c, 0x108)
// void TestDynamicWindow(int, int);

VA(0x004a0234, 0x91)
// void HandleRemoteDeadPlayerExit(int);

VA(0x004a02c5, 0xaa)
// void HandleRemoteSuddenExit(void);

VA(0x004a036f, 0x62)
// void DropDownToOnePlayer(void);

VA(0x004a03d1, 0x412)
// void ReceiveHostReportsPlayerExit(int, struct SPlayerExit, int);

VA(0x004a07e3, 0x361)
// void ReceiveRemotePlayerExit(struct SPlayerExit);

VA(0x004a0b44, 0x29)
// int CheckMem(void);

VA(0x004a0b6d, 0x109)
// int GetManaCost(int, class hero *);

VA(0x004a0c76, 0x9f)
// void SetWinText(class heroWindow *, int);

VA(0x004a0d15, 0x8a)
// void CheckShingleUpdate(void);

VA(0x004a0d9f, 0x17c6)
// void NormalDialog(char *, int, int, int, int, int, int, int, int, int);

VA(0x004a2565, 0x71)
// void UpdateNormalDialog(char *);

// ---- data / globals / vtables ----
DATA(0x004f8c58)  // unsigned char * giGroundToTerrain
DATA(0x004f8e08)  // unsigned char * giGroundShape
DATA(0x004f8fb8)  // unsigned char * gColorTableTan
DATA(0x004f90b8)  // unsigned char * gColorTableGray
DATA(0x004f91b8)  // unsigned char * gColorTableYellow
DATA(0x004f92b8)  // unsigned char * gColorTableScenWin
DATA(0x004f93b8)  // unsigned char * gColorTableDarkGray
DATA(0x004f94b8)  // unsigned char * gColorTableRed
DATA(0x004f95b8)  // unsigned char * gColorTableDarkBrown
DATA(0x004f96b8)  // int MAP_WIDTH
DATA(0x004f96bc)  // int MAP_HEIGHT
DATA(0x004f96c0)  // unsigned char * mapExtra
DATA(0x004f96c4)  // int gbClosingApp
DATA(0x004f96c8)  // int gbForegroundApp
DATA(0x004f96cc)  // int giMainVideoModeColorDepth
DATA(0x004f96d0)  // int giMainVideoModeWidth
DATA(0x004f96d4)  // int giMainVideoModeHeight
DATA(0x004f96d8)  // unsigned long int glMilliCounter
DATA(0x004f96e0)  // unsigned char * gMapColors
DATA(0x004f96f0)  // unsigned char * gObjectColors
DATA(0x004f9700)  // unsigned char * gOwnerColors
DATA(0x004f9708)  // char * * gTilesetFiles
DATA(0x004f9808)  // unsigned char * bPuzzleDraw
DATA(0x004f9848)  // unsigned char (*)[4][256] uDimPal
DATA(0x004fa448)  // unsigned char * gColorTableLighten
DATA(0x004fa548)  // unsigned char * gColorTableNoCycle
DATA(0x004fa648)  // class font * smallFont
DATA(0x004fa64c)  // class font * bigFont
DATA(0x004fa650)  // int gbReturnAfterComputeExtent
DATA(0x004fa654)  // int gbAllowTextEntryEscape
DATA(0x004fa658)  // int giCycleType
DATA(0x004fa65c)  // int giScreenScroll
DATA(0x004fa660)  // int giMenuCommand
DATA(0x004fa664)  // int gbSendMouseMoveMessages
DATA(0x004fa668)  // int gbColorMice
DATA(0x004fa670)  // unsigned long int * gTownEligibleBuildMask
DATA(0x004fa688)  // unsigned char * giMapSizes
DATA(0x004fa68c)  // int gbUseEvilInterface
DATA(0x004fa690)  // char * (*)[2] cEvilTranslate
DATA(0x004fa7b8)  // char * gcAnimPath
DATA(0x004fa918)  // char * gcGamePath
DATA(0x004fa930)  // char * gcMapPath
DATA(0x004fa944)  // int gbPutzingWithMouseCtr
DATA(0x004fa948)  // int gbDontTryRedbook
DATA(0x004fa94c)  // int gbDontTryMIDI
DATA(0x004fa950)  // int gbDontTryDigital
DATA(0x004fa958)  // float * gfCombatSpeedMod
DATA(0x004fa964)  // class icon * gShingleAnim
DATA(0x004fa968)  // int iNextShingleAnim
DATA(0x004fa96c)  // int giDialogTimeout
DATA(0x004fa970)  // int giNewMonsterCycleFrame
DATA(0x004fa974)  // int gbNoCDRom
DATA(0x004fa978)  // int gbLeaveNetBoxAlone
DATA(0x004fa97c)  // int gbDrawWindowBackground
DATA(0x004fa980)  // int gbCheatMenus
DATA(0x004fa984)  // int gbUseWaveout
DATA(0x004fa988)  // int gbShowAllMaps
DATA(0x004fa990)  // char * * gCombatFxNames
DATA(0x004faa10)  // short int * horseFrameFlip
DATA(0x004faa30)  // short int * boatFrameFlip
DATA(0x004faa50)  // signed char * gCastleResources
DATA(0x004faa58)  // short int * gCastleAmounts
DATA(0x004faa60)  // int gHeroGoldCost
DATA(0x004faa68)  // short int * gVesaMode
DATA(0x004faa78)  // struct tag_tilePoint * normalDirTable
DATA(0x004faa98)  // int * gResourceBaseValue
DATA(0x004faab8)  // int (*)[7] gInitResourcesHuman
DATA(0x004fab48)  // int (*)[7] gInitResourcesComputer
DATA(0x004fabd8)  // int * gMineCharacteristics
DATA(0x004fabf8)  // int (*)[3] gSSValues
DATA(0x004faca0)  // unsigned char * gArtifactLevel
DATA(0x004fad08)  // int * gArtifactBaseRV
DATA(0x004faea4)  // int gUltArtifactAvgValue
DATA(0x004faea8)  // int giDebugLevel
DATA(0x004faeac)  // signed char giVisRangeTown
DATA(0x004faeb0)  // struct tag_monsterInfo * gMonsterDatabase
DATA(0x004fb568)  // float * gfStatPower
DATA(0x004fb610)  // float * gfBattleStat
DATA(0x004fb6b8)  // signed char * gSpellLimits
DATA(0x004fb6c0)  // float * gfSpellCastableCombatMod
DATA(0x004fb6f0)  // float * gfSpellCastNumMod
DATA(0x004fb720)  // float * gfPhilAISpellPowerMod
DATA(0x004fb750)  // float * gfPhilAIDurationMod
DATA(0x004fb780)  // float * gfSpellTypeNumMod
DATA(0x004fb79c)  // int gbDrawSavedCursor
DATA(0x004fb7a0)  // signed char (*)[8] gbArrow
DATA(0x004fb7e0)  // unsigned char * giCloudType
DATA(0x004fb8e0)  // short int (*)[2] giScoreMon
DATA(0x004fb9e8)  // short int (*)[2] giScoreCampaignMon
DATA(0x004fbaf0)  // signed char * townTheme
DATA(0x004fbaf8)  // signed char (*)[2][4] gHeroSkillBonus
DATA(0x004fbb28)  // int gbLoadingMonoIcon
DATA(0x004fbb2c)  // int giMonoIconSkip
DATA(0x004fbb30)  // int giScrollX
DATA(0x004fbb34)  // int giScrollY
DATA(0x004fbb38)  // int gbNoBorder
DATA(0x004fbb3c)  // int gbEnlargeScreenBlit
DATA(0x004fbb40)  // int giCurExe
DATA(0x004fbb44)  // int gbInDialog
DATA(0x004fbb48)  // struct SMenuEnableStatus * gsMenuEnableStatus
DATA(0x004fbd34)  // int gbInSetupDialog
DATA(0x004fbd38)  // int gbMinimized
DATA(0x004fbd3c)  // int gbHeroMoving
DATA(0x004fbd40)  // int gbInSmackMgr
DATA(0x004fbd44)  // int glBottomRefresh
DATA(0x004fbd48)  // int gbBothMachinesWin95
DATA(0x004fbd4c)  // int gbGotFirstHeartbeat
DATA(0x004fbd50)  // void * hmnuDflt
DATA(0x004fbd54)  // void * hmnuCmbt
DATA(0x004fbd58)  // void * hmnuAdv
DATA(0x004fbd5c)  // void * hmnuTown
DATA(0x004fbd60)  // char * * cMonFilename
DATA(0x004fbe68)  // int gbProcessingCombatAction
DATA(0x004fbe6c)  // int iMPNetProtocol
DATA(0x004fbe70)  // int iLastDiffSendTo
DATA(0x004fbe78)  // struct SSpellInfo * gsSpellInfo
DATA(0x004fc410)  // char * * cArmyFrameFileNames
DATA(0x004fc518)  // unsigned char * gcSpellInfluenceIcons
DATA(0x004fc528)  // unsigned char * giSpellInfluenceToSpell
DATA(0x004fc538)  // unsigned char * giNumPowFrames
DATA(0x004fc558)  // int giSpellEffectShowType
DATA(0x004fc560)  // signed char * gcColorToPlayerPos
DATA(0x004fc568)  // char * * cCombatBkgNames
DATA(0x004fc5b8)  // struct SCmbtObstacle * sCmbtObstacles
DATA(0x004fc778)  // int * gEstatesGoldLevel
DATA(0x004fc788)  // float * gfSSLogisticsMod
DATA(0x004fc798)  // float * gfSSNavigationMod
DATA(0x004fc7a8)  // float * gfSSArcheryMod
DATA(0x004fc7b8)  // float * gfSSAIArcheryMod
DATA(0x004fc7c8)  // signed char * giVisRange
DATA(0x004fc7d0)  // unsigned char (*)[5] gStartingHeroStats
DATA(0x004fc7f0)  // int (*)[4][2] giTerrainCost
DATA(0x004fc930)  // unsigned char * bStopOnTrigger
DATA(0x004fc9b0)  // char * * gTownPrefixNames
DATA(0x004fc9c8)  // char * * gTownObjNames
DATA(0x004fca48)  // signed char (*)[12] gDwellingType
DATA(0x004fca90)  // int (*)[7] gMageBuildingCosts
DATA(0x004fcb38)  // int (*)[7] gSpecialBuildingCosts
DATA(0x004fcbe0)  // int (*)[7] gNeutralBuildingCosts
DATA(0x004fcda0)  // int * gMageBaseResourceValues
DATA(0x004fcdb8)  // int * gNeutralBaseResourceValues
DATA(0x004fce08)  // int * gSpecialBuildingBaseResourceValues
DATA(0x004fce20)  // int (*)[12] gDwellingBaseResourceValues
DATA(0x004fcf40)  // int (*)[12][7] gDwellingCosts
DATA(0x004fd720)  // unsigned long int (*)[12] gHierarchyMask
DATA(0x004fd840)  // int giDebugBuildingToBuild
DATA(0x004fd848)  // unsigned char * giTerrainToMusicTrack
DATA(0x004fd858)  // char * * cHeroTypeShortName
DATA(0x004fd870)  // char * cHeroTypeInitial
DATA(0x004fd878)  // int giDeferObjDrawX
DATA(0x004fd87c)  // int giDeferObjDrawY
DATA(0x004fd880)  // class heroWindow * gpInitWin
DATA(0x004fd888)  // unsigned char (*)[6] iGetSSByAlignment
DATA(0x004fd8e0)  // struct SCmbtHero * sCmbtHero
DATA(0x004fdeec)  // unsigned char * iWallToHexCell
DATA(0x004fdef0)  // unsigned char * iTowerToHexCell
DATA(0x004fdef8)  // unsigned short int (*)[2] wallPos
DATA(0x004fdf08)  // unsigned short int (*)[2] towerPos
DATA(0x004fdf18)  // unsigned short int (*)[2] doorPos
DATA(0x004fdf20)  // float * fTradingPostEfficency
DATA(0x004fdf50)  // struct SElevationOverlay * sElevationOverlay
DATA(0x004fe100)  // signed char (*)[4] captainStats
DATA(0x004fe118)  // int gbDrawingPuzzle
DATA(0x004fe11c)  // int giWalkingFrom
DATA(0x004fe120)  // int giWalkingFrom2
DATA(0x004fe124)  // int giWalkingTo
DATA(0x004fe128)  // int giWalkingTo2
DATA(0x004fe12c)  // int giWalkingYMod
DATA(0x004fe130)  // unsigned char * moatCell
DATA(0x004fe140)  // struct SCampaignChoice (*)[12][3] campaignChoices
DATA(0x004fe2a8)  // char * congratsText
DATA(0x004fe2b0)  // char * * gArtifactNames
DATA(0x004fe450)  // char * * gArtifactDesc
DATA(0x004fe5f0)  // char * * gArtifactEvent
DATA(0x004fe790)  // char * * gStatNames
DATA(0x004fe7a0)  // char * * gStatDesc
DATA(0x004fe7b0)  // char * * gAlignmentNames
DATA(0x004fe7d0)  // char * * gArmyShortNames
DATA(0x004fe8d8)  // char * * gArmyNames
DATA(0x004fe9e0)  // char * * gArmyNamesPlural
DATA(0x004feae8)  // char * * gTerrainNames
DATA(0x004feb10)  // char * * gResourceNames
DATA(0x004feb30)  // char * * gQuickViewText
DATA(0x004fed20)  // char * * gEventText
DATA(0x004feea0)  // char * * gCPanelHelp
DATA(0x004feeb8)  // char * * gCSPanelHelp
DATA(0x004feed8)  // char * * gAPanelHelp
DATA(0x004feef0)  // char * * gInitMenuHelp
DATA(0x004fef08)  // char * * gAdvMenuHelp
DATA(0x004fef28)  // char * * gLuckText
DATA(0x004fef48)  // char * * gMoraleText
DATA(0x004fef68)  // char * * onOffText
DATA(0x004fef98)  // char * * walkSpeedText
DATA(0x004fefb0)  // char * * gColors
DATA(0x004fefc8)  // char * * gMonthNames
DATA(0x004feff0)  // char * * gWeekNames
DATA(0x004ff030)  // char * * cHeroScreen
DATA(0x004ff098)  // char * * cCastleInfo
DATA(0x004ff0d8)  // char * * cLuckInfo
DATA(0x004ff130)  // char * * IQnames
DATA(0x004ff148)  // char * * cSpellHelp
DATA(0x004ff170)  // char * * speedText
DATA(0x004ff198)  // char * * cArmyDetail
DATA(0x004ff1c0)  // char * * cWellDetail
DATA(0x004ff1e8)  // char * * cKingdomOverview
DATA(0x004ff1f8)  // char * * cNewTurn
DATA(0x004ff218)  // char * * cViewGeneralLabels
DATA(0x004ff238)  // char * * cViewGeneralHelp
DATA(0x004ff258)  // char * * cViewGeneralLongHelp
DATA(0x004ff268)  // char * * cCombatMessage
DATA(0x004ff298)  // char * * cHeroLevel
DATA(0x004ff2a8)  // char * * cCombatHelp
DATA(0x004ff2c0)  // char * * cLongCombatHelp
DATA(0x004ff2d8)  // char * * cTownCommand
DATA(0x004ff348)  // char * * gHeroDefaultNames
DATA(0x004ff420)  // char * * gNewGameHelp
DATA(0x004ff440)  // char * * gSetupBaudHelp
DATA(0x004ff458)  // char * * gSetupComPortHelp
DATA(0x004ff470)  // char * * gSetupDCBaudHelp
DATA(0x004ff488)  // char * * gSetupDCComPortHelp
DATA(0x004ff4a0)  // char * * gSetupHotSeatGameHelp
DATA(0x004ff4b8)  // char * * gSetupModemGameHelp
DATA(0x004ff4c8)  // char * * gSetupDCGameHelp
DATA(0x004ff4d8)  // char * * gSetupMultiPlayerGameHelp
DATA(0x004ff4f0)  // char * * gSetupNetworkGameHelp
DATA(0x004ff500)  // char * * gSetupNetworkGame2Help
DATA(0x004ff510)  // char * * gSetupGameHelp
DATA(0x004ff520)  // char * * cBattleResults
DATA(0x004ff550)  // char * * cMoraleInfo
DATA(0x004ff5d0)  // char * * cMapSize
DATA(0x004ff5e0)  // char * * cDifficulty
DATA(0x004ff5f8)  // char * * cStartDifficulty
DATA(0x004ff608)  // char * * cCampaignLeaders
DATA(0x004ff618)  // char * * cWinText
DATA(0x004ff630)  // char * * cHumanDifficulty
DATA(0x004ff648)  // char * * cHumanInfoDifficulty
DATA(0x004ff660)  // char * * musicQualityText
DATA(0x004ff670)  // char * * gSpellDesc
DATA(0x004ff778)  // char * * gSpellNames
DATA(0x004ff880)  // char * * gSecondarySkillLevels
DATA(0x004ff890)  // char * * gSecondarySkills
DATA(0x004ff8c8)  // char * * gNeutralBuildingNames
DATA(0x004ff918)  // char * * gWellExtraNames
DATA(0x004ff938)  // char * * gSpecialBuildingNames
DATA(0x004ff958)  // char * (*)[12] gDwellingNames
DATA(0x004ffa78)  // char * (*)[3] cSecSkillDesc
DATA(0x004ffb20)  // char * * cBuildingInfoNeutral
DATA(0x004ffb70)  // char * * gBuildingInfoSpecial
DATA(0x004ffb88)  // char * * cDirections
DATA(0x004ffbb0)  // char * * cRumourTerrainDescriptions
DATA(0x004ffbd8)  // char * * gInterfaceTypeText
DATA(0x004ffbe8)  // char * * cBWMouseText
DATA(0x004ffbf0)  // char * * combatSpeedText
DATA(0x004ffc00)  // char * * combatMiniInfoText
DATA(0x004ffc10)  // char * * gcCommandLineHelp
DATA(0x004ffc48)  // char * * cOverviewText
DATA(0x004ffc60)  // char * * cWinComError
DATA(0x004ffc78)  // char * * cMiniViewText
DATA(0x004ffca0)  // char * * gFileRequestHelp
DATA(0x004ffce0)  // char * * cPersonality
DATA(0x004ffcf0)  // char * (*)[3] gArmySizeNames
DATA(0x004ffd60)  // char * * cRandomTavernText
DATA(0x004ffd80)  // char * * cRandomSignText
DATA(0x004ffd90)  // char * * cCampaignAwards
DATA(0x004ffdc0)  // char * (*)[12] cCampaignName
DATA(0x004ffe20)  // char * (*)[12] cCampaignDescription
DATA(0x004ffe80)  // char * cOutOfMemory
DATA(0x004ffe88)  // char * * cSlowVideoLevelText
DATA(0x004ffe90)  // char * * gSPanelHelp
DATA(0x004ffeb8)  // char * * xBarrierColor
DATA(0x004ffed8)  // char * * xGenericSiteNames
DATA(0x004ffef8)  // char * * xRecruitmentSiteNames
DATA(0x004fff10)  // struct SWinSetup * gWinSetup
DATA(0x00500110)  // int gbHeroWindShowing
DATA(0x00500114)  // int gbFullCombatScreenDrawn
DATA(0x00500118)  // int gbLimitedCombatUpdatePalette
DATA(0x0050011c)  // int gbFirstTimeThrough
DATA(0x00500120)  // int gbSkipIntro
DATA(0x00500124)  // int gbDoMemCheck
DATA(0x00500128)  // int gbAllBlack
DATA(0x0050012c)  // int gbInCombat
DATA(0x00500130)  // int gbDirectConnect
DATA(0x00500134)  // int giForceSwitchMusic
DATA(0x00500138)  // int gbComputeExtent
DATA(0x0050013c)  // int gbSaveBiggestExtent
DATA(0x00500140)  // int gbLimitToExtent
DATA(0x00500144)  // int gbCurrArmyDrawn
DATA(0x00500148)  // int gAdvDisposeLevel
DATA(0x0050014c)  // int gbRemoteOn
DATA(0x00500150)  // int gbGameInitialized
DATA(0x00500154)  // int giHighScoreRank
DATA(0x00500158)  // int giHighScoreType
DATA(0x0050015c)  // int gbShowHighScore
DATA(0x00500160)  // int gbLowMemory
DATA(0x00500164)  // int giHighMemBuffer
DATA(0x00500168)  // void * gLowPage
DATA(0x0050016c)  // int gbLowPageGrabbed
DATA(0x00500170)  // signed char xSmackFromNetwork
DATA(0x00500174)  // int gbInPollSound
DATA(0x005157a8)  // int iCDRomErr
DATA(0x005157ac)  // int bEarlySetupDone
DATA(0x005159f8)  // int bKBDone
DATA(0x005159fc)  // struct _REDBOOK * hRedbookz
DATA(0x00515a00)  // int bForceCheckTimeEvent
DATA(0x00515ca0)  // unsigned short int (*)[4] IMHotSpots
DATA(0x00515cc8)  // int lastIMHoverID
DATA(0x00515f78)  // int bInCheckEndGame
DATA(0x005165dc)  // int bInShutDown
DATA(0x00516810)  // int gbInMemError
DATA(0x00516d1c)  // int iShingleAnimFrame
DATA(0x00528598)  // int * gbHumanPlayer
DATA(0x005285b0)  // int gbHitEvent
DATA(0x005285b4)  // int giMaxExtentX
DATA(0x005285b8)  // int giMaxExtentY
DATA(0x005285bc)  // int giRandomClouds
DATA(0x005285c0)  // char * cOverrideDigitalDriver
DATA(0x005285d0)  // int giBottomViewOverrideEndTime
DATA(0x005285d8)  // signed char (*)[20] gArmyEffected
DATA(0x00528600)  // int giBottomViewResource
DATA(0x00528604)  // int gbInCampaign
DATA(0x00528608)  // int giResExtra1
DATA(0x0052860c)  // int giResExtra2
DATA(0x00528610)  // signed char * puzzlePiecesRemoved
DATA(0x00528618)  // int giSeedingValid
DATA(0x0052861c)  // int giLimitPlayer
DATA(0x00528620)  // int giShowClouds
DATA(0x00528624)  // int bDoColorCycle
DATA(0x00528628)  // class inputManager * gpInputManager
DATA(0x00528630)  // struct SAMPLE2 NULL_SAMPLE2
DATA(0x00528638)  // int iMaxMapExtra
DATA(0x0052863c)  // class palette * gPalette
DATA(0x00528640)  // class resourceManager * gpResourceManager
DATA(0x00528648)  // char * gcBotViewText
DATA(0x005286e0)  // int bSpecialHideCursor
DATA(0x005286e4)  // class searchArray * gpSearchArray
DATA(0x005286e8)  // int giResType1
DATA(0x005286ec)  // int gbBlackoutPlayer
DATA(0x005286f0)  // int giResType2
DATA(0x005286f8)  // char (*)[140] cNetBoxLine
DATA(0x00528928)  // int gIndex
DATA(0x0052892c)  // int giWeekTypeExtra
DATA(0x00528930)  // class philAI * gpPhilAI
DATA(0x00528934)  // int giTCPType
DATA(0x00528938)  // int gCurLoadedSpellEffect
DATA(0x0052893c)  // class mouseManager * gpMouseManager
DATA(0x00528940)  // int giCurTempMobility
DATA(0x00528944)  // int giOverviewReturnAction
DATA(0x00528948)  // char * cOverrideMIDIDriver
DATA(0x00528958)  // class icon * gSystemIcons
DATA(0x0052895c)  // signed char gbCombatSurrender
DATA(0x00528960)  // class heroWindow * pNormalDialogWindow
DATA(0x00528964)  // int giTCPHostStatus
DATA(0x00528968)  // char * gMapName
DATA(0x00528978)  // int giMinExtentX
DATA(0x0052897c)  // int giMinExtentY
DATA(0x00528980)  // int iMPBaseType
DATA(0x00528984)  // int gbTCPFirstTime
DATA(0x00528988)  // short int * pwSizeOfMapExtra
DATA(0x0052898c)  // int giHeroScreenSrcIndex
DATA(0x00528990)  // int giWeekType
DATA(0x00528998)  // char * gText
DATA(0x00528c98)  // int gbInNewGameSetup
DATA(0x00528c9c)  // class palette * gpBufferPalette
DATA(0x00528ca0)  // int gbCampaignSideChoice
DATA(0x00528ca4)  // char * cNetBoxColor
DATA(0x00528ca8)  // int giMonthTypeExtra
DATA(0x00528cac)  // int iMPExtendedType
DATA(0x00528cb0)  // signed char * gcColorToSetupPos
DATA(0x00528cb8)  // char * gFullMapName
DATA(0x00528cd0)  // char * gcTCPName
DATA(0x00528ce8)  // int giShowIntro
DATA(0x00528cf0)  // int * glTimers
DATA(0x00528d18)  // int giScore
DATA(0x00528d1c)  // class armyGroup * gpMonGroup
DATA(0x00528d20)  // struct configStruct gConfig
DATA(0x00528ec0)  // char * gcRegAppPath
DATA(0x00529020)  // unsigned long int gTimeMark
DATA(0x00529024)  // char * EXPANSION_AGGREGATE_NAME
DATA(0x00529028)  // char (*)[21] cPlayerNames
DATA(0x005290a8)  // class game * gpGame
DATA(0x005290ac)  // signed char gbRetreatWin
DATA(0x005290b0)  // int giWaitType
DATA(0x005290b4)  // class icon * gCurLoadedSpellIcon
DATA(0x005290b8)  // unsigned char * bSaveMusicPosition
DATA(0x005290f4)  // int giBottomViewOverride
DATA(0x005290f8)  // char * gcTCPAddress
DATA(0x00529110)  // unsigned char giSetupGameType
DATA(0x00529118)  // char * gLastFilename
DATA(0x00529278)  // int giFullySeeded
DATA(0x0052927c)  // class icon * gBuyBuildIcons
DATA(0x00529280)  // int gbNoSound
DATA(0x00529288)  // int * iCombatControlNetPos
DATA(0x00529290)  // char * cExpAggPathName
DATA(0x005293f0)  // int gbMoveShown
DATA(0x005293f4)  // void * * ppMapExtra
DATA(0x005293f8)  // char * gcBottomViewText
DATA(0x00529454)  // int giThisNetPos
DATA(0x00529458)  // signed char * gbSetupGamePosToRealGamePos
DATA(0x00529460)  // char * gcRegCDRomPath
DATA(0x005295c0)  // class heroWindow * heroWin
DATA(0x005295c4)  // int giOverviewReturnActionExtra
DATA(0x005295c8)  // int giCurGeneral
DATA(0x005295cc)  // int giThisGamePos
DATA(0x005295d0)  // int giNumHumanPlayers
DATA(0x005295d4)  // int gbIconClipOn
DATA(0x005295d8)  // int gbRemoteGameOpen
DATA(0x005295dc)  // class combatManager * gpCombatManager
DATA(0x005295e0)  // int giTCPNumPlayers
DATA(0x005295e4)  // class executive * gpExec
DATA(0x005295e8)  // void * hMainWindow
DATA(0x005295ec)  // int giCurWindowsStyleFlags
DATA(0x005295f0)  // int gGameCommand
DATA(0x005295f4)  // int giMonthType
DATA(0x005295f8)  // char * DEFAULT_AGGREGATE_NAME
DATA(0x005295fc)  // int gCurSpellEffectFrame
DATA(0x00529600)  // signed char * gbThisNetHumanPlayer
DATA(0x00529608)  // char * cAggPathName
DATA(0x00529768)  // class highScoreManager * gpHighScoreManager
DATA(0x0052976c)  // int gbFunctionComplete
DATA(0x00529770)  // int gbIAmGreatest
DATA(0x00529774)  // int gbTextEntryEscaped
DATA(0x00529778)  // int giTotalHighMem
DATA(0x0052977c)  // int gMapX
DATA(0x00529780)  // int gMapY
DATA(0x00529788)  // char * gcWinText
DATA(0x005298b4)  // int bFreshSave
DATA(0x005298b8)  // int bShowIt
DATA(0x005298bc)  // int gLowPageScreenSelector
DATA(0x005298c0)  // class heroWindowManager * gpWindowManager
DATA(0x005298c4)  // int giCurWatchPlayer
DATA(0x005298c8)  // char * gcCommandLine
DATA(0x00529908)  // int giBottomViewResourceQty
DATA(0x0052990c)  // class soundManager * gpSoundManager
DATA(0x00529910)  // int gbThisNetGotAdventureControl
DATA(0x00529914)  // int giMapChangeCtr
DATA(0x00529918)  // struct SMapChange * sMapChangeQueue
DATA(0x0052a184)  // int gbWaitForRemoteReceive
DATA(0x0052a188)  // unsigned char * bMusicIsLooping
DATA(0x0052a1c4)  // class townManager * gpTownManager
DATA(0x0052a1c8)  // class advManager * gpAdvManager
DATA(0x0052a1d0)  // signed char * gbGamePosToNetPos
