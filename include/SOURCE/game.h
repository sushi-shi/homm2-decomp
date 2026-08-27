#ifndef HOMM2_SOURCE_GAME_H
#define HOMM2_SOURCE_GAME_H

#include <Ints.h>
#include <BASE/message.h>
#include <EDITOR/fullMap.h>
#include <SOURCE/KB.h>
#include <SOURCE/combatTypes.h>
#include <SOURCE/REQUEST.h>
#include <SOURCE/hero.h>
#include <SOURCE/playerData.h>
#include <SOURCE/town.h>
#include <SOURCE/Overview.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/GAME.h>

class army;
class armyGroup;
class fullMap;
class hero;
class mapCell;
class playerData;
class town;
struct tag_message;

enum class CampaignTrackType : i32;

enum class PlayerHandicap : i8 {
    PLAYER_HANDICAP_NONE     = 0,
    PLAYER_HANDICAP_MODERATE = 1,
    PLAYER_HANDICAP_SEVERE   = 2,
    PLAYER_HANDICAP_COUNT    = 3
};
using enum PlayerHandicap;
ENABLE_ENUM_STEPS(PlayerHandicap)

#pragma pack(push, 1)
struct mineRecord {
    i8 id;
    i8 owner;
    H2EnumStorage<MineType, i8> resourceType;
    H2EnumStorage<CreatureType, i8> guardianType;
    u8 guardianCount;
    u8 x;
    u8 y;
};
struct boatRecord {
    i8 id;
    i8 x;
    i8 y;
    H2EnumStorage<MapDirection, i8> direction;
    H2EnumStorage<MapObjectType, u8> savedTriggerType;
    u8 savedEventData;
    i8 heroId;
    i8 owner;
};
#pragma pack(pop)

typedef enum GameStateStorageConstant {
    GAME_CAMPAIGN_STATE_PAD_SIZE         = 0x78,
    GAME_SAVE_NAME_SIZE                  = 0x15f,
    GAME_MAP_FILENAME_SIZE               = 13,
    GAME_SETUP_STATE_PAD_SIZE            = 0x12,
    GAME_DAILY_EVENT_FLAG_COUNT          = GAME_TOWN_COUNT / 8,
    GAME_OBELISK_VISITOR_COUNT           = 48,
    GAME_DEFAULT_PLAYER_NAME_SIZE        = 4,
    GAME_DEFAULT_PLAYER_NAMES_SIZE       = GAME_PLAYER_COUNT * GAME_DEFAULT_PLAYER_NAME_SIZE,
    GAME_RUNTIME_PAD_SIZE                = 0xc,
    GAME_RUMOUR_TEXT_SIZE                = 0x12d,
    GAME_RUMOUR_EVENT_CAPACITY           = 30,
    GAME_TIME_EVENT_CAPACITY             = 50,
    GAME_MAP_EVENT_CAPACITY              = 50,
    GAME_CURRENT_MAP_NAME_SIZE           = 16,
    GAME_CAMPAIGN_TRACK_COORDINATE_COUNT = 2,
    GAME_RECEIVED_TEXT_BUFFER_COUNT      = 3
} GameStateStorageConstant;

#pragma pack(push, 1)
class game {
public:
    i16 m_difficultyRating;
    H2EnumStorage<CampaignSide, u8> m_campaignType;
    H2EnumStorage<CampaignSide, u8> m_campaignStartingSide;
    i8 m_campaignScenario;
    u8 m_campaignScenarioCompleted[H2EnumIndex(CAMPAIGN_SIDE_COUNT)][CAMPAIGN_MAP_COUNT];
    i16 m_campaignScenarioBonus[H2EnumIndex(CAMPAIGN_SIDE_COUNT)][CAMPAIGN_MAP_COUNT];
    i16 m_campaignScenarioDays[H2EnumIndex(CAMPAIGN_SIDE_COUNT)][CAMPAIGN_MAP_COUNT];
    char m_unknown7d;
    u8 m_campaignAwards[CAMPAIGN_AWARD_COUNT];
    u8 m_campaignChoice[H2EnumIndex(CAMPAIGN_SIDE_COUNT)][CAMPAIGN_MAP_COUNT];
    u8 m_campaignMapEnabled[H2EnumIndex(CAMPAIGN_SIDE_COUNT)][CAMPAIGN_MAP_COUNT];
    i16 m_campaignScore;
    H2EnumStorage<CreatureType, i16> m_campaignCarryoverCreatureTypes[CAMPAIGN_ARMY_SLOT_COUNT];
    i16 m_campaignCarryoverCreatureCounts[CAMPAIGN_ARMY_SLOT_COUNT];
    u8 m_campaignScenarioWon;
    u8 m_campaignCheated;
    char _pad_0xd2[GAME_CAMPAIGN_STATE_PAD_SIZE];
    char m_saveName[GAME_SAVE_NAME_SIZE];
    SMapHeader m_mapHeader;
    i8 m_setupPlayerColor[MAP_HEADER_PLAYER_COUNT];
    H2SteppedEnumStorage<PlayerHandicap, i8> m_playerHandicap[MAP_HEADER_PLAYER_COUNT];
    H2SteppedEnumStorage<FactionType, i8> m_setupPlayerRace[MAP_HEADER_PLAYER_COUNT];
    i8 m_setupPlayerNetworkId[MAP_HEADER_PLAYER_COUNT];
    H2EnumStorage<GameDifficulty, i8> m_difficulty;
    char m_mapFilename[GAME_MAP_FILENAME_SIZE];
    i8 m_setupPlayerType[MAP_HEADER_PLAYER_COUNT];
    i8 m_selectedSetupPlayer;
    b8 m_newGameInitialized;
    i8 m_newGameHumanCount;
    char _pad_0x47c[GAME_SETUP_STATE_PAD_SIZE];
    i8 m_playerCount;
    i8 m_deadPlayerCount;
    i8 m_playerDead[H2EnumIndex(GAME_PLAYER_COUNT)];
    u16 m_day;
    u16 m_week;
    u16 m_month;
    class playerData m_players[H2EnumIndex(GAME_PLAYER_COUNT)];
    class fullMap m_worldMap;
    i8 m_obeliskCount;
    town m_castleRecs[H2EnumIndex(GAME_TOWN_COUNT)];
    union {
        i8 m_castleOwners[H2EnumIndex(GAME_TOWN_COUNT)];
        i8 m_townOwners[H2EnumIndex(GAME_TOWN_COUNT)];
    };
    union {
        char m_dailyEventFlags[GAME_DAILY_EVENT_FLAG_COUNT];
        u8 m_knownTowns[GAME_DAILY_EVENT_FLAG_COUNT];
    };
    hero m_heroRecs[H2EnumIndex(GAME_HERO_COUNT)];
    i8 m_availableHeroes[H2EnumIndex(GAME_HERO_COUNT)];
    mineRecord m_mines[H2EnumIndex(GAME_MINE_COUNT)];
    i8 m_mineOwners[H2EnumIndex(GAME_MINE_COUNT)];
    char m_randomArtifacts[H2EnumIndex(ARTIFACT_COUNT)];
    boatRecord m_boats[H2EnumIndex(GAME_BOAT_COUNT)];
    i8 m_boatSlots[H2EnumIndex(GAME_BOAT_COUNT)];
    i8 m_obeliskVisitors[GAME_OBELISK_VISITOR_COUNT];
    char m_defaultPlayerNames[GAME_DEFAULT_PLAYER_NAMES_SIZE];
    i8 m_ultimateArtifactX;
    i8 m_ultimateArtifactY;
    H2EnumStorage<ArtifactType, i8> m_ultimateArtifactId;
    class heroWindow* m_newGameWindow;
    char m_pad_0x639c;
    u8 m_cheated;
    char m_pad_0x639e[GAME_RUNTIME_PAD_SIZE];
    char m_rumour[GAME_RUMOUR_TEXT_SIZE];
    u16 m_rumourEventCount;
    u16 m_rumourEventIndices[GAME_RUMOUR_EVENT_CAPACITY];
    u16 m_timeEventCount;
    u16 m_timeEventIndices[GAME_TIME_EVENT_CAPACITY];
    u16 m_mapEventCount;
    u16 m_mapEventIndices[GAME_MAP_EVENT_CAPACITY];
    class heroWindow* m_viewArmyWindow;
    i32 m_viewArmyResult;
    class heroWindow* m_viewSpellsWindow;
    class hero* m_viewSpellsHero;
    HeroSpellType m_viewSpellsType;
    i32 m_viewSpellsTop[H2EnumIndex(SPELL_TYPE_CATEGORY_COUNT)];
    i32 m_viewSpellsCount[H2EnumIndex(SPELL_TYPE_CATEGORY_COUNT)];
    H2EnumStorage<SpellType, i32> m_viewSpell;
    MessageDispatchHandler m_viewSpellsCallback;
    i8 m_viewSpellsReadOnly;
    u8 m_gameLoaded;
    void SetupDynamicStuff(i32, i32, i32);
    void SetupNewOverviewType(OverviewType, i32);
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
    void PlayPreScenarioSmacker(CampaignSide, i32);
    void ShowCampaignInfo(i32, i32);
    void CampaignInfoUpdate(i32);
    void InitEntireCampaign(CampaignSide);
    void InitCampaignMap(void);
    i32 MineTypesOwned(i32, MineType);
    i32 SetupPuzzlePieces(i32, i32);
    i32 IsMobile(i32);
    class fullMap* GetWorldMapData(void);
    i32 CreateBoat(i32, i32, i32);
    i32 Scan(i8*, i32, i32);
    i32 RandomScan(i8*, i32, i32, i32, i8);
    i32 GetNewHeroId(i32, FactionType, i32);
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
    i32 GetPlayerColor(i32 player) {
        return m_players[player].m_color;
    }
    i32 GetMineId(i32, i32);
    i32 SaveGame(const char*, i32, i8);
    void SetupOrigData(void);
    void LoadGame(const char*, i32, i32);
    void GiveTroopsToNeutralTown(i32);
    void GiveTroopsToNeutralTowns(void);
    void NewMap(const char*);
    void RandomizeEvents(void);
    void InitializePasswords(void);
    void RandomizeBarrier(class mapCell*);
    void RandomizePassword(class mapCell*);
    i32 LoadMap(const char*);
    void ClaimTown(i32, i32, i32);
    void ClaimMine(i32, i32);
    SpellType ViewSpells(class hero*, HeroSpellType, MessageDispatchHandler, i32);
    void UpdateSpellWidgets(void);
    void ViewArmy(
        i32,
        i32,
        CreatureType,
        i32,
        class town*,
        i32,
        ArmyFacing,
        i32,
        class hero*,
        class army*,
        class armyGroup*,
        i32
    );
    i32 GetRandomNumTroops(CreatureType);
    void TurnOnAIMusic(void);
    void TurnOffAIMusic(void);
    void NextPlayer(void);
    i32 ComputeDailyGold(i32);
    void PerDay(void);
    void PerWeek(void);
    void WeeklyRecruitSite(class mapCell*);
    void WeeklyGenericSite(class mapCell*);
    void PerMonth(void);
    void ConvertObject(
        i32,
        i32,
        i32,
        i32,
        TilesetId,
        i32,
        i32,
        TilesetId,
        i32,
        MapObjectType,
        MapObjectType
    );
    void RandomizeTown(i32, i32, i32);
    void RandomizeMine(i32, i32);
    void InitRandomArtifacts(void);
    i32 GetRandomArtifactId(ArtifactLevelMask, b32);
    void RandomizeHeroPool(void);
    void SetRandomHeroArmies(i32, i32);
    void ProcessRandomObjects(void);
    void SetVisibility(i32, i32, i32, i32);
    void MakeAllWaterVisible(i32);
    void GiveArmy(class armyGroup*, CreatureType, i32, i32);
    i32 ExperienceValueOfStack(class armyGroup*, class hero*);
    i32 GetLuck(class hero*, class army*, class town*);
    void SetupAdjacentMons(void);
    void CancelComputerScreen(void);
    void ShowComputerScreen(void);
    void ShowHeroesLogo(void);
    void WaitForPlayer(const char*, i32);
    i32 HasLateOverlay(i32, i32);
    void ConvertFlagToLateOverlay(i32, i32);
    i32 HasObjectTilesetIndex(i32, i32, TilesetId, i32);
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
    void RestoreCell(
        i32,
        i32,
        MapObjectType,
        i32,
        class mapCell*,
        i32
    );
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
extern class heroWindow* overWin;
extern char gcCurMapName[GAME_CURRENT_MAP_NAME_SIZE];
extern class textWidget** textWidgetDynamic;
extern class iconWidget** iconWidgetDynamic;
extern OverviewType iLastDynamicType;
extern OverviewType giOverviewType;
extern i32 giOverviewTop[H2EnumIndex(OVERVIEW_TYPE_COUNT)];
extern class iconWidget* OVScrollKnob;
extern b32 gbDoModemConfig;
extern i16 trackXY[H2EnumIndex(CAMPAIGN_SIDE_COUNT)][CAMPAIGN_TRACK_POINT_COUNT]
                  [GAME_CAMPAIGN_TRACK_COORDINATE_COUNT];
extern class heroWindow* campWin;
extern b32 gbNewGameDialogOver;
extern i32 NGKPcursorFlashOn;
extern i32 iLastDynamicTop;
extern i32 iOverviewItems;
extern i32 giOverviewItems[H2EnumIndex(OVERVIEW_TYPE_COUNT)];
extern class textWidget* textWidgetTitle[];
extern H2EnumStorage<CampaignSide, i32> iCurViewSide;
extern CampaignTrackType iCampaignTrackType;
extern i32 bCampaignViewOnly;
extern i32 iCurViewMap;
extern char* cNGKPDisplay;
extern b32 gbNewGameShadowHidden;
extern char* cNGKPCore;
extern i32 NGKPcursorIndex;
extern char* cTextReceivedBuffer[GAME_RECEIVED_TEXT_BUFFER_COUNT];
extern class icon* NGKPBkg;

#endif
