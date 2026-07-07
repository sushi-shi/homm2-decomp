#ifndef HOMM2_GLOBALS_H
#define HOMM2_GLOBALS_H
// Consolidated extern declarations of retail globals/.rdata constants referenced
// across TUs. Single source of truth so declarations cannot drift between TUs.
// Staging location — move a global to its owning module header once that is known.
#include <_all.h>
#include <_types.h>   // record/model structs referenced by the globals below

extern int bDataEntryTime;
extern int bDoColorCycle;
extern int bEarlySetupDone;
extern int bHeroBuiltThisTurn;
extern font *bigFont;
extern int bInShutDown;
extern char bMapInitialized;
extern int bShowIt;
extern int bSpecialHideCursor;
extern char *cBuildingInfoNeutral[];
extern char *cDEDest;
extern char cNetBoxColor[];
extern char cNetBoxLine[][140];
extern float  const_000eb2a0;         // 0.0f
extern float  const_000eb2a4;         // 2.0f
extern double const_000eb338;         // 0.5
extern float  const_000eb368;         // 100.0f
extern double const_000eb4a8;         // 1.5
extern float  const_000eb4c0;         // 1250.0f
extern float  const_000eb4c4;         // 48.0f
extern float const_00127c14;          // gafAITurnCostResource[3]
extern float const_00127c1c;          // gafAITurnCostResource[5]
extern float const_00127c20;          // gafAITurnCostResource[6]
extern int const_00128d38;            // 13-byte BSS flag buffer (??_C@_0N@PMOM@...)
extern char *cOutOfMemory;
extern UInt32 crc32Table[256];
extern class heroWindow *DataEntryWin;
extern float gafAITurnCostResource[7];
extern signed char *gaiEnemyHeroReachable;
extern short *gaiHeroEventStratRVOfPos;
extern short *gaiHeroStrategicRVOfPos;
extern short *gaiLiveChanceOfPos;
extern signed char *gaiTurnValueOfMine;
extern char *gArmyNames[];
extern char *gArmyNamesPlural[];
extern int gArtifactBaseRV[100];
extern int gbAllBlack;
extern int gbCheatMenus;
extern int gbClosingApp;
extern signed char gbCombatSurrender;
extern int gbDrawSavedCursor;
extern int gbForegroundApp;
extern int gbFunctionComplete;
extern int gbGameInitialized;
extern signed char gbGamePosToNetPos[];
extern int gbHeroMoving;
extern int gbHumanPlayer[];
extern int gbInMemError;
extern int gbInNewGameSetup;
extern int gbInPollSound;
extern int gbNoCDRom;
extern int gbNoSound;
extern int gbPutzingWithMouseCtr;
extern int gbReduceByReload;
extern int gbRemoteOn;
extern signed char gbRetreatWin;
extern int gbTextEntryEscaped;
extern int gbThisNetGotAdventureControl;
extern signed char gbThisNetHumanPlayer[];
extern char *gBuildingInfoSpecial[];
extern char gbUseDiffCompression;
extern char gbUseRegularCompression;
extern icon *gBuyBuildIcons;
extern char gcBottomViewText[];
extern configStruct gConfig;
extern SCreatureInfo gCreatureInfo[];
extern int gDwellingBaseResourceValues[][12];
extern int gDwellingCosts[][12][7];
extern char *gDwellingNames[][12];
extern signed char gDwellingType[20][12];
extern void *gEventHandle;
extern int gGameCommand;
extern int gHeroGoldCost;
extern unsigned long gHierarchyMask[][12];
extern int giBottomViewOverride;
extern int giBottomViewOverrideEndTime;
extern int giBottomViewResource;
extern int giBottomViewResourceQty;
extern signed char giBuildBoat[6];
extern signed char giBuildBoatStuffTurn[6];
extern signed char giBuildShipyard[6];
extern int giCurPlayer;
extern int giCurTurn;
extern int giCycleType;
extern int giDebugLevel;
extern int giGraphicsType;
extern unsigned char giGroundToTerrain[];
extern int giHighMemBuffer;
extern int giMainVideoModeColorDepth;
extern int giNumHumanPlayers;
extern short giScoreCampaignMon[][2];
extern short giScoreMon[][2];
extern int giTCPHostStatus;
extern int giThisGamePos;
extern int giThisNetPos;
extern int giTotalHighMem;
extern int giWaitType;
extern int glTimers[];
extern int gMageBaseResourceValues[];
extern int gMageBuildingCosts[][7];
extern tag_monsterInfo gMonsterDatabase[];
extern monsterRV gMonsterInfo[100];
extern int gNeutralBaseResourceValues[];
extern int gNeutralBuildingCosts[][7];
extern char *gNeutralBuildingNames[];
extern advManager *gpAdvManager;
extern palette *gPalette;
extern combatManager *gpCombatManager;
extern hero *gpCurAIHero;
extern playerData *gpCurPlayer;
extern executive *gpExec;
extern game *gpGame;
extern inputManager *gpInputManager;
extern MemEntry *gpMemEntry;
extern armyGroup *gpMonGroup;
extern class mouseManager *gpMouseManager;
extern philAI *gpPhilAI;
extern resourceManager *gpResourceManager;
extern searchArray *gpSearchArray;
extern soundManager *gpSoundManager;
extern townManager *gpTownManager;
extern class heroWindowManager *gpWindowManager;
extern int gResourceBaseValue[];
extern float gRVWeightCrystal;        // p[4]
extern float gRVWeightMerc;           // p[1]
extern float gRVWeightOre;            // p[2]
extern icon *gShingleAnim;
extern SNetPlayerInfo gsNetPlayerInfo[];
extern int gSpecialBuildingBaseResourceValues[];
extern int gSpecialBuildingCosts[][7];
extern char *gSpecialBuildingNames[];
extern SSpellInfo gsSpellInfo[];
extern icon *gSystemIcons;
extern char gText[];
extern char *gWellExtraNames[];
extern SWinSetup gWinSetup[];
extern void *hInstApp;
extern void *hmnuAdv;
extern void *hmnuApp;
extern void *hmnuCmbt;
extern void *hmnuDflt;
extern void *hmnuTown;
extern void *hwndApp;
extern int iAlphaMale;
extern int iCDRomErr;
extern int iCurHourGlassPhase;
extern int iDEMaxLen;
extern int iDummy;
extern int iLastFrameRateTimer;
extern int iLastSeed;
extern int iMaxMapExtra;
extern int iMemEntries;
extern int inBoxX;
extern int inBoxY;
extern int iNextShingleAnim;
extern char inName[];
extern int iShingleAnimFrame;
extern int MAP_HEIGHT;
extern int MAP_WIDTH;
extern unsigned char *mapExtra;
extern tag_tilePoint normalDirTable[];
extern SAMPLE2 NULL_SAMPLE2;
extern char outName[];
extern heroWindow *pNormalDialogWindow;
extern void **ppMapExtra;
extern short *pwSizeOfMapExtra;
extern font *smallFont;
extern searchArray SVSearchArray;
extern signed char xIsExpansionMap;
extern char *xNecromancerShrine;
extern char *xNecromancerShrineDesc;
extern int xPasswordStringsIndex[];
extern int xShrineBuildingCost[];

#endif // HOMM2_GLOBALS_H
