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
#include <SOURCE/armyGroup.h>

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
    PLAYER_HANDICAP_UNINITIALIZED = -1,
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
    H2OpenCodeStorage<MapTriggerCode, u8> savedTriggerType;
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
    GAME_TOWN_BUILD_FLAG_BYTE_COUNT      = GAME_TOWN_COUNT / 8,
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
    H2EnumStorage<CreatureType, i16> m_campaignCarryoverCreatureTypes[ARMY_GROUP_SLOT_COUNT];
    i16 m_campaignCarryoverCreatureCounts[ARMY_GROUP_SLOT_COUNT];
    u8 m_campaignScenarioWon;
    u8 m_campaignCheated;
    char _pad_0xd2[GAME_CAMPAIGN_STATE_PAD_SIZE];
    char m_saveName[GAME_SAVE_NAME_SIZE];
    SMapHeader m_mapHeader;
    i8 m_setupPlayerColor[GAME_PLAYER_COUNT];
    H2SteppedEnumStorage<PlayerHandicap, i8> m_playerHandicap[GAME_PLAYER_COUNT];
    H2SteppedEnumStorage<FactionType, i8> m_setupPlayerRace[GAME_PLAYER_COUNT];
    i8 m_setupPlayerNetworkId[GAME_PLAYER_COUNT];
    H2EnumStorage<GameDifficulty, i8> m_difficulty;
    char m_mapFilename[GAME_MAP_FILENAME_SIZE];
    i8 m_setupPlayerType[GAME_PLAYER_COUNT];
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
    i8 m_townOwners[H2EnumIndex(GAME_TOWN_COUNT)];
    u8 m_townBuiltToday[GAME_TOWN_BUILD_FLAG_BYTE_COUNT];
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
    b8 m_cheated;
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
    void SetupDynamicStuff(i32 redraw, i32 updateKnob, i32 forceUpdate);
    void SetupNewOverviewType(OverviewType overviewType, i32 redrawFrom);
    void SetupResources(void);
    void Overview(void);
    void DoKnob(void);
    i32 ProcessIconSelect(i32 widgetId, b32 quickView);
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
    void PlayPreScenarioSmacker(CampaignSide side, i32 map);
    void ShowCampaignInfo(i32 viewOnly, i32);
    void CampaignInfoUpdate(i32 redraw);
    void InitEntireCampaign(CampaignSide side);
    void InitCampaignMap(void);
    i32 MineTypesOwned(i32 owner, MineType mineType);
    i32 SetupPuzzlePieces(i32 player, i32 justCount);
    i32 IsMobile(i32 heroId);
    class fullMap* GetWorldMapData(void);
    i32 CreateBoat(i32 x, i32 y, i32 notify);
    i32 Scan(i8* array, i32 start, i32 length);
    i32 RandomScan(i8* array, i32 start, i32 range, i32, i8 target);
    i32 GetNewHeroId(i32, FactionType heroClass, i32 requireExperienced);
    i32 GetTownId(i32 column, i32 row);
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
    i32 GetMineId(i32 column, i32 row);
    i32 SaveGame(const char* filename, i32 generateName, i8 expansionFormat);
    void SetupOrigData(void);
    void LoadGame(const char* filename, i32 loadFromFile, i32);
    void GiveTroopsToNeutralTown(i32 townId);
    void GiveTroopsToNeutralTowns(void);
    void NewMap(const char* filename);
    void RandomizeEvents(void);
    void InitializePasswords(void);
    void RandomizeBarrier(class mapCell* cell);
    void RandomizePassword(class mapCell* cell);
    i32 LoadMap(const char* filename);
    void ClaimTown(i32 townId, i32 player, i32 suppressVisibility);
    void ClaimMine(i32 mineId, i32 player);
    SpellType ViewSpells(class hero* spellHero, HeroSpellType spellType, MessageDispatchHandler callback, i32 readOnly);
    void UpdateSpellWidgets(void);
    void ViewArmy(
        i32 x,
        i32 y,
        CreatureType monsterType,
        i32 numTroops,
        class town* castle,
        i32 disableUpgrade,
        ArmyFacing facing,
        i32 quickView,
        class hero* theHero,
        class army* theArmy,
        class armyGroup* theGroup,
        i32 groupIndex
    );
    i32 GetRandomNumTroops(CreatureType monsterType);
    void TurnOnAIMusic(void);
    void TurnOffAIMusic(void);
    void NextPlayer(void);
    i32 ComputeDailyGold(i32 player);
    void PerDay(void);
    void PerWeek(void);
    void WeeklyRecruitSite(class mapCell* cell);
    void WeeklyGenericSite(class mapCell* cell);
    void PerMonth(void);
    void ConvertObject(
        i32 left,
        i32 top,
        i32 right,
        i32 bottom,
        TilesetId oldTileset,
        i32 oldFirstIndex,
        i32 oldLastIndex,
        TilesetId newTileset,
        i32 newFirstIndex,
        MapObjectType oldTrigger,
        MapObjectType newTrigger
    );
    void RandomizeTown(i32 x, i32 y, i32);
    void RandomizeMine(i32 x, i32 y);
    void InitRandomArtifacts(void);
    i32 GetRandomArtifactId(ArtifactLevelMask levelMask, b32 allowCursed);
    void RandomizeHeroPool(void);
    void SetRandomHeroArmies(i32 heroId, i32 strongArmy);
    void ProcessRandomObjects(void);
    void SetVisibility(i32 x, i32 y, i32 player, i32 radius);
    void MakeAllWaterVisible(i32 player);
    void GiveArmy(class armyGroup* group, CreatureType type, i32 count, i32 slot);
    i32 ExperienceValueOfStack(class armyGroup* group, class hero* heroPointer);
    i32 GetLuck(class hero* heroPointer, class army*, class town* castle);
    void SetupAdjacentMons(void);
    void CancelComputerScreen(void);
    void ShowComputerScreen(void);
    void ShowHeroesLogo(void);
    void WaitForPlayer(const char* text, i32 player);
    i32 HasLateOverlay(i32 column, i32 row);
    void ConvertFlagToLateOverlay(i32 column, i32 row);
    i32 HasObjectTilesetIndex(i32 column, i32 row, TilesetId tileset, i32 index);
    void ConvertAllToLateOverlay(i32 column, i32 row);
    void ProcessMapExtra(void);
    void SetupTowns(void);
    void ProcessOnMapHeroes(void);
    void CheckHeroConsistency(void);
    i32 TransmitSaveGame(i32 remotePlayer, i32 player, i32 useCurrentSave);
    i32 ReceiveSaveGame(i32 dataSize, i32 expectedCrc, i32 expectedTransmitCrc, i32 remotePlayer);
    void DoNewTurn(void);
    i32 GetBoatsBuilt(void);
    i32 GetNumThievesGuilds(i32 color);
    i32 CalcDifficultyRating(void);
    void RestoreCell(
        i32 x,
        i32 y,
        MapTriggerCode objectType,
        i32 barrier,
        class mapCell* passedCell,
        i32
    );
    void SetMapSize(i32 width, i32 height);
    i32 HeroIDToHeroPos(class playerData* player, i32 heroId);
    i32 TownIDToTownPos(class playerData* player, i32 townId);
    void SetupNewRumour(void);
    void CheckForTimeEvent(void);
    i32 CountShrines(i32 player);
    void ShowMoraleInfo(class hero* heroPointer, i32 dialogType);
    void ShowLuckInfo(class hero* heroPointer, i32 dialogType);
    void GetMap(void);
    void ProcessNewMap(struct SMapHeader* header);
    void InitNewGame(struct SMapHeader* header);
    void SetupNetPlayerNames(void);
    i32 NewGame(void);
    void CleanUpNewGameWindow(void);
    void InitNewGameWindow(void);
    void UpdateNewGameWindow(void);
    i32 ProcessNGKeyPress(struct tag_message& message);
    void NGKPSetupDisplayString(char* text, u16 cursor);
    void DrawNGKPDisplayString(i32 updateScreen);
    void ShowScenInfo(void);
    void GetLossConditionText(char* text);
    void GetVictoryConditionText(char* text);
    i32 GetSideDesc(char* text, i32 firstPlayer, i32 lastPlayer);
};
#pragma pack(pop)

#define GAME_DAY_NUMBER(g)                                                                         \
    ((g).m_day + ((g).m_week - 1) * CALENDAR_DAYS_PER_WEEK                                         \
     + ((g).m_month - 1) * CALENDAR_DAYS_PER_MONTH)
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
