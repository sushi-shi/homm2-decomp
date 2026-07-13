#ifndef HOMM2_SOURCE_GAME_H
#define HOMM2_SOURCE_GAME_H
// Reconstructed class (SOURCE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 114 methods, 0 own-virtual, 0 static data.
#include <va.h>
#include <EDITOR/fullMap.h>
#include <SOURCE/hero.h>
// forward declarations:
class army;
class armyGroup;
class fullMap;
class hero;
class mapCell;
class playerData;
class town;
struct SMapHeader;
struct tag_message;

// game data records owned by the game struct (also declared in KB.h under the same guard)
#ifndef HOMM2_GAME_RECORD_TYPES
#define HOMM2_GAME_RECORD_TYPES
#pragma pack(push, 1)
struct townArmyRecord {
    signed char m_troopTypes[5];
    union {
        unsigned short m_troopCounts[5];
        short m_creatureCounts[5];
    };
};
struct townSlot {
    signed char id;
    signed char owner;
    signed char unknown2;
    signed char race;
    unsigned char x;
    unsigned char y;
    signed char unknown6;
    signed char unknown7;
    union {
        townArmyRecord m_army;
        signed char army[15];
    };
    signed char occupyingHeroId;
    int buildings;
    char unknown1c;
    char unknown1d;
    short dwellingGrowth[12];
    unsigned char m_onMap;
    signed char m_unknown37;
    signed char unknown38;
    signed char m_originalOwner;
    unsigned short m_extraIndex;
    union {
        signed char m_spells[5][4];
        struct {
            char m_spellPad[19];
            signed char m_spellCounts[6];
        };
    };
    short unknown55;
    char m_name[13];
};
#pragma pack(pop)
SIZE(townSlot, 0x64);
#endif

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

#ifndef HOMM2_PLAYER_RECORD_TYPE
#define HOMM2_PLAYER_RECORD_TYPE
#pragma pack(push, 1)
struct playerRec {
    signed char color;
    signed char heroCount;
    signed char currentHero;
    signed char heroWindowTop;
    signed char heroes[8];
    signed char availableHeroes[2];
    signed char unknown0e;
    int unknown0f;
    signed char unknown13;
    char pad14[0x2c];
    signed char unknown40;
    signed char unknown41;
    signed char unknown42;
    signed char daysLeft;
    signed char townCount;
    signed char currentTown;
    signed char townWindowTop;
    signed char towns[72];
    int resources[7];
    signed char evilInterface;
    signed char unknownac;
    char padad[0x3a];
    int secondaryResources[7];
    char pad103[0x18];

    signed char Town(int index) { return towns[index]; }
    signed char Hero(int index) { return heroes[index]; }
    signed char TownCount(void) { return townCount; }
    signed char HeroCount(void) { return heroCount; }
};
#pragma pack(pop)
#endif

#pragma pack(push, 1)  // recovered layout is byte-packed
class game {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    short  field_0x0;  // +0x00
    unsigned char m_campaignType;  // +0x02
    char   field_0x3;  // +0x03
    unsigned char m_campaignScenario;  // +0x04
    char _pad_0x5[0xe];
    char   field_0x13;  // +0x13
    char _pad_0x14[0x6a];
    unsigned char m_dwarfAlliance;  // +0x7e
    char   field_0x7f;  // +0x7f
    char   field_0x80;  // +0x80
    char   field_0x81;  // +0x81
    char   field_0x82;  // +0x82
    char   field_0x83;  // +0x83
    char   field_0x84;  // +0x84
    unsigned char m_dwarfbane;  // +0x85
    unsigned char m_ogreAlliance;  // +0x86
    unsigned char m_dragonAlliance;  // +0x87
    char   field_0x88;  // +0x88
    char   field_0x89;  // +0x89
    char _pad_0x8a[0x18];
    char   field_0xa2;  // +0xa2
    char   field_0xa3;  // +0xa3
    char   field_0xa4;  // +0xa4
    char   field_0xa5;  // +0xa5
    char   field_0xa6;  // +0xa6
    char   field_0xa7;  // +0xa7
    char   field_0xa8;  // +0xa8
    char   field_0xa9;  // +0xa9
    char   field_0xaa;  // +0xaa
    char   field_0xab;  // +0xab
    char _pad_0xac[0x1];
    char   field_0xad;  // +0xad
    char   field_0xae;  // +0xae
    char   field_0xaf;  // +0xaf
    char   field_0xb0;  // +0xb0
    char   field_0xb1;  // +0xb1
    char   field_0xb2;  // +0xb2
    char   field_0xb3;  // +0xb3
    char   field_0xb4;  // +0xb4
    char   field_0xb5;  // +0xb5
    char   field_0xb6;  // +0xb6
    char   field_0xb7;  // +0xb7
    char   field_0xb8;  // +0xb8
    char   field_0xb9;  // +0xb9
    short  field_0xba;  // +0xba
    char _pad_0xbc[0x15];
    unsigned char m_campaignCheated;  // +0xd1
    char _pad_0xd2[0x78];
    char m_saveName[0x15f];  // +0x14a
    int    field_0x2a9;  // +0x2a9
    char   field_0x2ad;  // +0x2ad
    char _pad_0x2ae[0x1];
    unsigned char m_mapWidth;  // +0x2af
    unsigned char m_mapHeight;  // +0x2b0
    char   field_0x2b1;  // +0x2b1
    char   field_0x2b2;  // +0x2b2
    char _pad_0x2b3[0x10];
    char   field_0x2c3;  // +0x2c3
    char   field_0x2c4;  // +0x2c4
    char   field_0x2c5;  // +0x2c5
    unsigned char m_victoryConditionType;  // +0x2c6
    char   field_0x2c7;  // +0x2c7
    char   field_0x2c8;  // +0x2c8
    unsigned short m_victoryArtifact;  // +0x2c9, one-based artifact id
    char   field_0x2cb;  // +0x2cb
    short  field_0x2cc;  // +0x2cc
    char   field_0x2ce;  // +0x2ce
    char _pad_0x2cf[0x6];
    short  field_0x2d5;  // +0x2d5
    short  field_0x2d7;  // +0x2d7
    short  field_0x2d9;  // +0x2d9
    char _pad_0x2db[0x170];
    char   field_0x44b;  // +0x44b
    char   field_0x44c;  // +0x44c
    char _pad_0x44d[0x6];
    signed char m_playerHandicap[6];  // +0x453
    signed char m_setupPlayerRace[6];  // +0x459
    char _pad_0x45f[6];
    signed char m_difficulty;  // +0x465
    char _pad_0x466[0x13];
    char   field_0x479;  // +0x479
    char   field_0x47a;  // +0x47a
    char   field_0x47b;  // +0x47b
    char _pad_0x47c[0x12];
    signed char m_playerCount;  // +0x48e
    signed char m_humanPlayerCount;  // +0x48f
    signed char m_playerDead[6];  // +0x490
    unsigned short m_day;  // +0x496
    unsigned short m_week;  // +0x498
    unsigned short m_month;  // +0x49a
    struct playerRec m_players[6];  // +0x49c
    class fullMap m_worldMap;  // +0xb3e
    signed char m_obeliskCount;  // +0xb52
    townSlot m_castleRecs[72];  // 0xb53
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
    char m_pad_0x6398[5];
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
    town *GetTown(int id) { return reinterpret_cast<town *>(&m_castleRecs[id]); }
    signed char GetPlayerColor(int player) { return m_players[player].color; }
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
extern class textWidget **textWidgetDynamic;
extern class iconWidget **iconWidgetDynamic;
extern int giOverviewType;
extern int *giOverviewTop;
extern class iconWidget *OVScrollKnob;
extern int gbDoModemConfig;
extern short trackXY[2][13][2];
extern class heroWindow *campWin;
extern int gbNewGameDialogOver;
extern int NGKPcursorFlashOn;
extern int iLastDynamicType;
extern int iLastDynamicTop;
extern int iOverviewItems;
extern int *giOverviewItems;
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
