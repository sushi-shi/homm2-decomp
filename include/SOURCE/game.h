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
    signed char id;
    signed char owner;
    signed char resourceType;
    signed char guardianType;
    unsigned char guardianCount;
    unsigned char x;
    unsigned char y;
};
struct boatRecord {
    signed char id;
    signed char x;
    signed char y;
    signed char direction;
    unsigned char savedTriggerType;
    unsigned char savedEventData;
    signed char heroId;
    signed char owner;
};
#pragma pack(pop)
SIZE(boatRecord, 8);

#pragma pack(push, 1)  // recovered layout is byte-packed
class game {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    short m_difficultyRating;  // +0x00
    unsigned char m_campaignType;  // +0x02
    unsigned char m_campaignStartingSide;  // +0x03
    signed char m_campaignScenario;  // +0x04
    unsigned char m_campaignScenarioCompleted[2][12];  // +0x05
    short m_campaignScenarioBonus[2][12];  // +0x1d
    short m_campaignScenarioDays[2][12];  // +0x4d
    char m_unknown7d;  // +0x7d
    unsigned char m_campaignAwards[12];  // +0x7e
    unsigned char m_campaignChoice[2][12];  // +0x8a
    unsigned char m_campaignMapEnabled[2][12];  // +0xa2
    short m_campaignScore;  // +0xba
    short m_campaignCarryoverCreatureTypes[5];  // +0xbc
    short m_campaignCarryoverCreatureCounts[5];  // +0xc6
    unsigned char m_campaignScenarioWon;  // +0xd0
    unsigned char m_campaignCheated;  // +0xd1
    char _pad_0xd2[0x78];
    char m_saveName[0x15f];  // +0x14a
    SMapHeader m_mapHeader;  // +0x2a9
    signed char m_setupPlayerColor[MAP_HEADER_PLAYER_COUNT];  // +0x44d
    signed char m_playerHandicap[MAP_HEADER_PLAYER_COUNT];  // +0x453
    signed char m_setupPlayerRace[MAP_HEADER_PLAYER_COUNT];  // +0x459
    signed char m_setupPlayerNetworkId[MAP_HEADER_PLAYER_COUNT];  // +0x45f
    signed char m_difficulty;  // +0x465
    char m_mapFilename[13];  // +0x466
    signed char m_setupPlayerType[MAP_HEADER_PLAYER_COUNT];  // +0x473
    signed char m_selectedSetupPlayer;  // +0x479
    signed char m_newGameInitialized;  // +0x47a
    signed char m_newGameHumanCount;  // +0x47b
    char _pad_0x47c[0x12];
    signed char m_playerCount;  // +0x48e
    signed char m_deadPlayerCount;  // +0x48f
    signed char m_playerDead[6];  // +0x490
    unsigned short m_day;  // +0x496
    unsigned short m_week;  // +0x498
    unsigned short m_month;  // +0x49a
    class playerData m_players[6];  // +0x49c
    class fullMap m_worldMap;  // +0xb3e
    signed char m_obeliskCount;  // +0xb52
    town m_castleRecs[72];  // 0xb53
    union {
        signed char m_castleOwners[72];  // +0x2773
        signed char m_townOwners[72];
    };
    union {
        char m_dailyEventFlags[9];  // +0x27bb
        unsigned char m_knownTowns[9];
    };
    hero m_heroRecs[54];         // 0x27c4  hero record slots (GetHeroSlot)
    signed char m_availableHeroes[54];  // +0x5c80
    mineRecord m_mines[144];      // 0x5cb6
    signed char m_mineOwners[144];  // 0x60a6
    char m_randomArtifacts[0x67];  // 0x6136
    boatRecord m_boats[48];        // 0x619d
    signed char m_boatSlots[48];   // 0x631d, active boat record indices
    signed char m_obeliskVisitors[48];  // 0x634d
    char m_defaultPlayerNames[24];  // +0x637d
    signed char m_ultimateArtifactX;  // +0x6395
    signed char m_ultimateArtifactY;  // +0x6396
    signed char m_ultimateArtifactId;  // +0x6397
    class heroWindow *m_newGameWindow;  // +0x6398
    char m_pad_0x639c;
    unsigned char m_cheated;  // +0x639d
    char m_pad_0x639e[0xc];
    char m_rumour[0x12d];  // +0x63aa
    unsigned short m_rumourEventCount;  // +0x64d7
    unsigned short m_rumourEventIndices[30];  // +0x64d9
    unsigned short m_timeEventCount;  // +0x6515
    unsigned short m_timeEventIndices[50];  // +0x6517
    class heroWindow *m_viewArmyWindow;  // +0x65e1
    int m_viewArmyResult;  // +0x65e5
    class heroWindow *m_viewSpellsWindow;  // +0x65e9
    class hero *m_viewSpellsHero;  // +0x65ed
    int m_viewSpellsType;  // +0x65f1
    int m_viewSpellsTop[2];  // +0x65f5
    int m_viewSpellsCount[2];  // +0x65fd
    int m_viewSpell;  // +0x6605
    int (*m_viewSpellsCallback)(struct tag_message &);  // +0x6609
    signed char m_viewSpellsReadOnly;  // +0x660d
    // --- methods ---
    void SetupDynamicStuff(int, int, int);
    void SetupNewOverviewType(int, int);
    void SetupResources(void);
    void Overview(void);
    void DoKnob(void);
    int ProcessIconSelect(int, int);
    int SetupCampaignGame(void);
    int SetupBaud(void);
    int SetupComPort(void);
    int SetupHotSeatGame(void);
    int SetupNetworkGame(void);
    int SetupNetworkGame2(void);
    int SetupModemGame(void);
    int SetupMultiPlayerGame(void);
    int SetupGame(void);
    int PickLoadGame(void);
    int HandleCampaignWin(void);
    void PlayPreScenarioSmacker(int, int);
    void ShowCampaignInfo(int, int);
    void CampaignInfoUpdate(int);
    void InitEntireCampaign(int);
    void InitCampaignMap(void);
    int MineTypesOwned(int, int);
    int SetupPuzzlePieces(int, int);
    int IsMobile(int);
    class fullMap * GetWorldMapData(void);
    int CreateBoat(int, int, int);
    int Scan(signed char *, int, int);
    int RandomScan(signed char *, int, int, int, signed char);
    int GetNewHeroId(int, int, int);
    int GetTownId(int, int);
    hero *GetHero(int id) { return &m_heroRecs[id]; }
    town *GetTown(int id) { return &m_castleRecs[id]; }
    hero *GetPlayerHero(int player, int index) {
        return &m_heroRecs[m_players[player].m_heroIds[index]];
    }
    town *GetPlayerTown(int player, int index) {
        return &m_castleRecs[m_players[player].m_townIds[index]];
    }
    signed char GetPlayerColor(int player) { return m_players[player].m_color; }
    int GetMineId(int, int);
    int SaveGame(char *, int, signed char);
    void SetupOrigData(void);
    void LoadGame(char *, int, int);
    void GiveTroopsToNeutralTown(int);
    void GiveTroopsToNeutralTowns(void);
    void NewMap(char *);
    void RandomizeEvents(void);
    void InitializePasswords(void);
    void RandomizeBarrier(class mapCell *);
    void RandomizePassword(class mapCell *);
    int LoadMap(char *);
    void ClaimTown(int, int, int);
    void ClaimMine(int, int);
    int ViewSpells(class hero *, int, int (*)(struct tag_message &), int);
    void UpdateSpellWidgets(void);
    void ViewArmy(int, int, int, int, class town *, int, int, int, class hero *, class army *, class armyGroup *, int);
    int GetRandomNumTroops(int);
    void TurnOnAIMusic(void);
    void TurnOffAIMusic(void);
    void NextPlayer(void);
    int ComputeDailyGold(int);
    void PerDay(void);
    void PerWeek(void);
    void WeeklyRecruitSite(class mapCell *);
    void WeeklyGenericSite(class mapCell *);
    void PerMonth(void);
    void ConvertObject(int, int, int, int, int, int, int, int, int, int, int);
    void RandomizeTown(int, int, int);
    void RandomizeMine(int, int);
    void InitRandomArtifacts(void);
    int GetRandomArtifactId(int, int);
    void RandomizeHeroPool(void);
    void SetRandomHeroArmies(int, int);
    void ProcessRandomObjects(void);
    void SetVisibility(int, int, int, int);
    void MakeAllWaterVisible(int);
    void GiveArmy(class armyGroup *, int, int, int);
    int ExperienceValueOfStack(class armyGroup *, class hero *);
    int GetLuck(class hero *, class army *, class town *);
    void SetupAdjacentMons(void);
    void CancelComputerScreen(void);
    void ShowComputerScreen(void);
    void ShowHeroesLogo(void);
    void WaitForPlayer(char *, int);
    int HasLateOverlay(int, int);
    void ConvertFlagToLateOverlay(int, int);
    int HasObjectTilesetIndex(int, int, int, int);
    void ConvertAllToLateOverlay(int, int);
    void ProcessMapExtra(void);
    void SetupTowns(void);
    void ProcessOnMapHeroes(void);
    void CheckHeroConsistency(void);
    int TransmitSaveGame(int, int, int);
    int ReceiveSaveGame(int, int, int, int);
    void DoNewTurn(void);
    int GetBoatsBuilt(void);
    int GetNumThievesGuilds(int);
    int CalcDifficultyRating(void);
    void RestoreCell(int, int, int, int, class mapCell *, int);
    void SetMapSize(int, int);
    int HeroIDToHeroPos(class playerData *, int);
    int TownIDToTownPos(class playerData *, int);
    void SetupNewRumour(void);
    void CheckForTimeEvent(void);
    int CountShrines(int);
    void ShowMoraleInfo(class hero *, int);
    void ShowLuckInfo(class hero *, int);
    void GetMap(void);
    void ProcessNewMap(struct SMapHeader *);
    void InitNewGame(struct SMapHeader *);
    void SetupNetPlayerNames(void);
    int NewGame(void);
    void CleanUpNewGameWindow(void);
    void InitNewGameWindow(void);
    void UpdateNewGameWindow(void);
    int ProcessNGKeyPress(struct tag_message &);
    void NGKPSetupDisplayString(char *, unsigned short int);
    void DrawNGKPDisplayString(int);
    void ShowScenInfo(void);
    void GetLossConditionText(char *);
    void GetVictoryConditionText(char *);
    int GetSideDesc(char *, int, int);
};
#pragma pack(pop)
// ---- globals (declarations, RVA order) ----
extern class heroWindow *overWin;
extern char gcCurMapName[16];
extern class textWidget **textWidgetDynamic;
extern class iconWidget **iconWidgetDynamic;
extern int giOverviewType;
extern int giOverviewTop[2];
extern class iconWidget *OVScrollKnob;
extern int gbDoModemConfig;
extern short trackXY[2][13][2];
extern class heroWindow *campWin;
extern int gbNewGameDialogOver;
extern int NGKPcursorFlashOn;
extern int iLastDynamicType;
extern int iLastDynamicTop;
extern int iOverviewItems;
extern int giOverviewItems[2];
extern class textWidget *textWidgetTitle[4];
extern int iCurViewSide;
extern int iCampaignTrackType;
extern int bCampaignViewOnly;
extern int iCurViewMap;
extern char *cNGKPDisplay;
extern int gbNewGameShadowHidden;
extern char *cNGKPCore;
extern int NGKPcursorIndex;
extern char *cTextReceivedBuffer[3];
extern class icon *NGKPBkg;

#endif // HOMM2_SOURCE_GAME_H
