#ifndef HOMM2_SOURCE_GAME_H
#define HOMM2_SOURCE_GAME_H
// Reconstructed class (SOURCE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 114 methods, 0 own-virtual, 0 static data.
#include <va.h>
#include <EDITOR/fullMap.h>
#include <SOURCE/REQUEST.h>
#include <SOURCE/hero.h>
#include <SOURCE/playerData.h>
#include <SOURCE/town.h>
#include <SOURCE/Overview.h>
// forward declarations:
class army;
class armyGroup;
class fullMap;
class hero;
class mapCell;
class playerData;
class town;
struct tag_message;

#pragma pack(push, 1)
struct mineRecord {
    i8 id;
    i8 owner;
    i8 resourceType;
    i8 guardianType;
    u8 guardianCount;
    u8 x;
    u8 y;
};
struct boatRecord {
    i8 id;
    i8 x;
    i8 y;
    i8 direction;
    u8 savedTriggerType;
    u8 savedEventData;
    i8 heroId;
    i8 owner;
};
#pragma pack(pop)
SIZE(boatRecord, 8);

#pragma pack(push, 1) // recovered layout is byte-packed
class game {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    i16 m_difficultyRating;                   // +0x00
    u8 m_campaignType;                        // +0x02
    u8 m_campaignStartingSide;                // +0x03
    i8 m_campaignScenario;                    // +0x04
    u8 m_campaignScenarioCompleted[2][12];    // +0x05
    i16 m_campaignScenarioBonus[2][12];       // +0x1d
    i16 m_campaignScenarioDays[2][12];        // +0x4d
    char m_unknown7d;                         // +0x7d never referenced
    u8 m_campaignAwards[12];                  // +0x7e
    u8 m_campaignChoice[2][12];               // +0x8a
    u8 m_campaignMapEnabled[2][12];           // +0xa2
    i16 m_campaignScore;                      // +0xba
    i16 m_campaignCarryoverCreatureTypes[5];  // +0xbc
    i16 m_campaignCarryoverCreatureCounts[5]; // +0xc6
    u8 m_campaignScenarioWon;                 // +0xd0
    u8 m_campaignCheated;                     // +0xd1
    char _pad_0xd2[0x78];
    char m_saveName[0x15f];                             // +0x14a
    SMapHeader m_mapHeader;                             // +0x2a9
    i8 m_setupPlayerColor[MAP_HEADER_PLAYER_COUNT];     // +0x44d
    i8 m_playerHandicap[MAP_HEADER_PLAYER_COUNT];       // +0x453
    i8 m_setupPlayerRace[MAP_HEADER_PLAYER_COUNT];      // +0x459
    i8 m_setupPlayerNetworkId[MAP_HEADER_PLAYER_COUNT]; // +0x45f
    i8 m_difficulty;                                    // +0x465
    char m_mapFilename[13];                             // +0x466
    i8 m_setupPlayerType[MAP_HEADER_PLAYER_COUNT];      // +0x473
    i8 m_selectedSetupPlayer;                           // +0x479
    i8 m_newGameInitialized;                            // +0x47a
    i8 m_newGameHumanCount;                             // +0x47b
    char _pad_0x47c[0x12];
    i8 m_playerCount;              // +0x48e
    i8 m_deadPlayerCount;          // +0x48f
    i8 m_playerDead[6];            // +0x490
    u16 m_day;                     // +0x496
    u16 m_week;                    // +0x498
    u16 m_month;                   // +0x49a
    class playerData m_players[6]; // +0x49c
    class fullMap m_worldMap;      // +0xb3e
    i8 m_obeliskCount;             // +0xb52
    town m_castleRecs[72];         // 0xb53
    union {
        i8 m_castleOwners[72]; // +0x2773
        i8 m_townOwners[72];
    };
    union {
        char m_dailyEventFlags[9]; // +0x27bb
        u8 m_knownTowns[9];
    };
    hero m_heroRecs[54];                         // 0x27c4  hero record slots (GetHeroSlot)
    i8 m_availableHeroes[54];                    // +0x5c80
    mineRecord m_mines[144];                     // 0x5cb6
    i8 m_mineOwners[144];                        // 0x60a6
    char m_randomArtifacts[IDX(ARTIFACT_COUNT)]; // 0x6136
    boatRecord m_boats[48];                      // 0x619d
    i8 m_boatSlots[48];                          // 0x631d, active boat record indices
    i8 m_obeliskVisitors[48];                    // 0x634d
    char m_defaultPlayerNames[24];               // +0x637d
    i8 m_ultimateArtifactX;                      // +0x6395
    i8 m_ultimateArtifactY;                      // +0x6396
    i8 m_ultimateArtifactId;                     // +0x6397
    class heroWindow* m_newGameWindow;           // +0x6398
    char m_pad_0x639c;
    u8 m_cheated; // +0x639d
    char m_pad_0x639e[0xc];
    char m_rumour[0x12d];                             // +0x63aa
    u16 m_rumourEventCount;                           // +0x64d7
    u16 m_rumourEventIndices[30];                     // +0x64d9
    u16 m_timeEventCount;                             // +0x6515
    u16 m_timeEventIndices[50];                       // +0x6517
    u16 m_mapEventCount;                              // +0x657b
    u16 m_mapEventIndices[50];                        // +0x657d
    class heroWindow* m_viewArmyWindow;               // +0x65e1
    i32 m_viewArmyResult;                             // +0x65e5
    class heroWindow* m_viewSpellsWindow;             // +0x65e9
    class hero* m_viewSpellsHero;                     // +0x65ed
    i32 m_viewSpellsType;                             // +0x65f1
    i32 m_viewSpellsTop[2];                           // +0x65f5
    i32 m_viewSpellsCount[2];                         // +0x65fd
    SpellType m_viewSpell;                                  // +0x6605
    i32 (*m_viewSpellsCallback)(struct tag_message&); // +0x6609
    i8 m_viewSpellsReadOnly;                          // +0x660d
    u8 m_gameLoaded;                                  // +0x660e
    // --- methods ---
    void SetupDynamicStuff(i32, i32, i32);
    void SetupNewOverviewType(i32, i32);
    void SetupResources(void);
    void Overview(void);
    void DoKnob(void);
    i32 ProcessIconSelect(i32, i32);
    i32 SetupCampaignGame(void);
    i32 SetupBaud(void);
    i32 SetupComPort(void);
    i32 SetupHotSeatGame(void);
    i32 SetupNetworkGame(void);
    i32 SetupNetworkGame2(void);
    i32 SetupModemGame(void);
    i32 SetupMultiPlayerGame(void);
    i32 SetupGame(void);
    i32 PickLoadGame(void);
    i32 HandleCampaignWin(void);
    void PlayPreScenarioSmacker(i32, i32);
    void ShowCampaignInfo(i32, i32);
    void CampaignInfoUpdate(i32);
    void InitEntireCampaign(i32);
    void InitCampaignMap(void);
    i32 MineTypesOwned(i32, i32);
    i32 SetupPuzzlePieces(i32, i32);
    i32 IsMobile(i32);
    class fullMap* GetWorldMapData(void);
    i32 CreateBoat(i32, i32, i32);
    i32 Scan(i8*, i32, i32);
    i32 RandomScan(i8*, i32, i32, i32, i8);
    i32 GetNewHeroId(i32, i32, i32);
    i32 GetTownId(i32, i32);
    hero* GetHero(i32 id) {
        return &m_heroRecs[id];
    }
    town* GetTown(i32 id) {
        return &m_castleRecs[id];
    }
    hero* GetPlayerHero(i32 player, i32 index) {
        return &m_heroRecs[m_players[player].m_heroIds[index]];
    }
    town* GetPlayerTown(i32 player, i32 index) {
        return &m_castleRecs[m_players[player].m_townIds[index]];
    }
    i8 GetPlayerColor(i32 player) {
        return m_players[player].m_color;
    }
    i32 GetMineId(i32, i32);
    i32 SaveGame(char*, i32, i8);
    void SetupOrigData(void);
    void LoadGame(char*, i32, i32);
    void GiveTroopsToNeutralTown(i32);
    void GiveTroopsToNeutralTowns(void);
    void NewMap(char*);
    void RandomizeEvents(void);
    void InitializePasswords(void);
    void RandomizeBarrier(class mapCell*);
    void RandomizePassword(class mapCell*);
    i32 LoadMap(char*);
    void ClaimTown(i32, i32, i32);
    void ClaimMine(i32, i32);
    SpellType ViewSpells(class hero*, i32, i32 (*)(struct tag_message&), i32);
    void UpdateSpellWidgets(void);
    void ViewArmy(
        i32,
        i32,
        i32,
        i32,
        class town*,
        i32,
        i32,
        i32,
        class hero*,
        class army*,
        class armyGroup*,
        i32
    );
    i32 GetRandomNumTroops(i32);
    void TurnOnAIMusic(void);
    void TurnOffAIMusic(void);
    void NextPlayer(void);
    i32 ComputeDailyGold(i32);
    void PerDay(void);
    void PerWeek(void);
    void WeeklyRecruitSite(class mapCell*);
    void WeeklyGenericSite(class mapCell*);
    void PerMonth(void);
    void ConvertObject(i32, i32, i32, i32, i32, i32, i32, i32, i32, i32, i32);
    void RandomizeTown(i32, i32, i32);
    void RandomizeMine(i32, i32);
    void InitRandomArtifacts(void);
    i32 GetRandomArtifactId(i32, i32);
    void RandomizeHeroPool(void);
    void SetRandomHeroArmies(i32, i32);
    void ProcessRandomObjects(void);
    void SetVisibility(i32, i32, i32, i32);
    void MakeAllWaterVisible(i32);
    void GiveArmy(class armyGroup*, i32, i32, i32);
    i32 ExperienceValueOfStack(class armyGroup*, class hero*);
    i32 GetLuck(class hero*, class army*, class town*);
    void SetupAdjacentMons(void);
    void CancelComputerScreen(void);
    void ShowComputerScreen(void);
    void ShowHeroesLogo(void);
    void WaitForPlayer(char*, i32);
    i32 HasLateOverlay(i32, i32);
    void ConvertFlagToLateOverlay(i32, i32);
    i32 HasObjectTilesetIndex(i32, i32, i32, i32);
    void ConvertAllToLateOverlay(i32, i32);
    void ProcessMapExtra(void);
    void SetupTowns(void);
    void ProcessOnMapHeroes(void);
    void CheckHeroConsistency(void);
    i32 TransmitSaveGame(i32, i32, i32);
    i32 ReceiveSaveGame(i32, i32, i32, i32);
    void DoNewTurn(void);
    i32 GetBoatsBuilt(void);
    i32 GetNumThievesGuilds(i32);
    i32 CalcDifficultyRating(void);
    void RestoreCell(i32, i32, i32, i32, class mapCell*, i32);
    void SetMapSize(i32, i32);
    i32 HeroIDToHeroPos(class playerData*, i32);
    i32 TownIDToTownPos(class playerData*, i32);
    void SetupNewRumour(void);
    void CheckForTimeEvent(void);
    i32 CountShrines(i32);
    void ShowMoraleInfo(class hero*, i32);
    void ShowLuckInfo(class hero*, i32);
    void GetMap(void);
    void ProcessNewMap(struct SMapHeader*);
    void InitNewGame(struct SMapHeader*);
    void SetupNetPlayerNames(void);
    i32 NewGame(void);
    void CleanUpNewGameWindow(void);
    void InitNewGameWindow(void);
    void UpdateNewGameWindow(void);
    i32 ProcessNGKeyPress(struct tag_message&);
    void NGKPSetupDisplayString(char*, u16);
    void DrawNGKPDisplayString(i32);
    void ShowScenInfo(void);
    void GetLossConditionText(char*);
    void GetVictoryConditionText(char*);
    i32 GetSideDesc(char*, i32, i32);
};
#pragma pack(pop)
SIZE(game, 0x660f);
// ---- globals (declarations, RVA order) ----
extern class heroWindow* overWin;
extern char gcCurMapName[16];
extern class textWidget** textWidgetDynamic;
extern class iconWidget** iconWidgetDynamic;
extern OverviewType giOverviewType;
extern i32 giOverviewTop[2];
extern class iconWidget* OVScrollKnob;
extern b32 gbDoModemConfig;
extern i16 trackXY[2][13][2];
extern class heroWindow* campWin;
extern b32 gbNewGameDialogOver;
extern i32 NGKPcursorFlashOn;
extern i32 iLastDynamicType;
extern i32 iLastDynamicTop;
extern i32 iOverviewItems;
extern i32 giOverviewItems[2];
extern class textWidget* textWidgetTitle[4];
extern i32 iCurViewSide;
extern i32 iCampaignTrackType;
extern i32 bCampaignViewOnly;
extern i32 iCurViewMap;
extern char* cNGKPDisplay;
extern b32 gbNewGameShadowHidden;
extern char* cNGKPCore;
extern i32 NGKPcursorIndex;
extern char* cTextReceivedBuffer[3];
extern class icon* NGKPBkg;

#endif // HOMM2_SOURCE_GAME_H
