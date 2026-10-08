#ifndef HOMM2_SOURCE_ADVMANAGER_H
#define HOMM2_SOURCE_ADVMANAGER_H

#include <match.h>
#include <Domains.h>
#include <H2/Macros.h>
#include <BASE/baseManager.h>
#include <BASE/widget.h>
#include <SOURCE/Viewwrld.h>
#include <SOURCE/gameTypes.h>
#include <BASE/message.h>
#include <SOURCE/kbTypes.h>
#include <SOURCE/remoteTypes.h>
#include <SOURCE/REQUEST.h>

class mapCell;
struct tag_message;

// Adventure screen geometry: the 480-pixel map frame, the 448-pixel map view
// inside its 16-pixel border, the radar square to the right of the frame, and
// the square around the hero that embarking and disembarking fizzle.
H2_ENUM_BEGIN(AdventureViewportConstant)
    ADVENTURE_VIEWPORT_EXTENT         = 480,
    ADVENTURE_VIEW_BORDER             = 16,
    ADVENTURE_VIEW_SIZE               = 448,
    ADVENTURE_VIEW_END                = ADVENTURE_VIEW_BORDER + ADVENTURE_VIEW_SIZE,
    ADVENTURE_RADAR_LEFT              = ADVENTURE_VIEWPORT_EXTENT,
    ADVENTURE_RADAR_TOP               = ADVENTURE_VIEW_BORDER,
    ADVENTURE_RADAR_SIZE              = MAP_DIMENSION_XLARGE,
    ADVENTURE_RADAR_RIGHT             = ADVENTURE_RADAR_LEFT + ADVENTURE_RADAR_SIZE,
    ADVENTURE_RADAR_BOTTOM            = ADVENTURE_RADAR_TOP + ADVENTURE_RADAR_SIZE,
    ADVENTURE_RADAR_SMALL_CELL_PIXELS  = ADVENTURE_RADAR_SIZE / MAP_DIMENSION_SMALL,
    ADVENTURE_RADAR_MEDIUM_CELL_PIXELS = ADVENTURE_RADAR_SIZE / MAP_DIMENSION_MEDIUM,
    // A large map draws four radar pixels per three cells: one wide cell of
    // each three, so positions scale by 1 + 1/3, rounded up.
    ADVENTURE_RADAR_LARGE_SCALE_DIVISOR  = 3,
    ADVENTURE_RADAR_LARGE_SCALE_ROUNDING = ADVENTURE_RADAR_LARGE_SCALE_DIVISOR - 1,
    ADVENTURE_HERO_FIZZLE_LEFT        = 192,
    ADVENTURE_HERO_FIZZLE_TOP         = 192,
    ADVENTURE_HERO_FIZZLE_SIZE        = 96
H2_ENUM_END(AdventureViewportConstant)

// advmice.mse pointer frames of the adventure screen; frames from SCROLL_FIRST
// up to SCROLL_END are the eight edge-scroll arrows.
H2_ENUM_BEGIN(AdventurePointerFrame)
    ADVENTURE_POINTER_DEFAULT      = 0,
    ADVENTURE_POINTER_HERO         = 2,
    ADVENTURE_POINTER_TOWN         = 3,
    ADVENTURE_POINTER_MOVE         = 4,
    ADVENTURE_POINTER_ATTACK       = 5,
    ADVENTURE_POINTER_SAIL         = 6,
    ADVENTURE_POINTER_DISEMBARK    = 7,
    ADVENTURE_POINTER_SELECT_HERO  = 8,
    ADVENTURE_POINTER_ACTION       = 9,
    ADVENTURE_POINTER_WATER_ACTION = 28,
    ADVENTURE_POINTER_SCROLL_FIRST = 32,
    ADVENTURE_POINTER_SCROLL_END   = 40
H2_ENUM_END(AdventurePointerFrame)

// Current screen and fixed adventure viewport; clipping policy remains explicit.
#define DRAW_ADVENTURE_ICON(pic, x, y, frame, clip)                                                \
    IconToBitmap(                                                                                  \
        (pic),                                                                                     \
        gpWindowManager->m_screen,                                                                 \
        (x),                                                                                       \
        (y),                                                                                       \
        (frame),                                                                                   \
        (clip),                                                                                    \
        0,                                                                                         \
        0,                                                                                         \
        ADVENTURE_VIEWPORT_EXTENT,                                                                 \
        ADVENTURE_VIEWPORT_EXTENT,                                                                 \
        0                                                                                          \
    )
#define DRAW_FLIPPED_ADVENTURE_ICON(pic, x, y, frame, clip) \
    FlipIconToBitmap((pic), gpWindowManager->m_screen, (x), (y), (frame), (clip), \
                     0, 0, ADVENTURE_VIEWPORT_EXTENT, ADVENTURE_VIEWPORT_EXTENT, 0)

H2_ENUM_BEGIN(AdventureRemoteConstant)
    ADVMGR_REMOTE_DATA_REQUEST             = 1,
H2_ENUM_END(AdventureRemoteConstant)

H2_ENUM_BEGIN(AdventureBottomViewSharedConstant)
    BOTTOM_VIEW_RESOURCE_MESSAGE_DURATION = 5000
H2_ENUM_END(AdventureBottomViewSharedConstant)

H2_ENUM_CLASS_BEGIN(AdventureEnvironmentSoundId)
    ADVMGR_ENVIRONMENT_SOUND_NONE     = -1,
    ADVMGR_SOUND_BUOY                 = 0,
    ADVMGR_SOUND_SHIPWRECK            = 1,
    ADVMGR_SOUND_COAST                = 2,
    ADVMGR_SOUND_ORACLE               = 3,
    ADVMGR_SOUND_STONE_LITHS          = 4,
    ADVMGR_SOUND_SMALL_VOLCANO        = 5,
    ADVMGR_SOUND_LAVA_POOL            = 6,
    ADVMGR_SOUND_ALCHEMIST_LAB        = 7,
    ADVMGR_SOUND_ALCHEMIST_LAB_ACTION = 8,
    ADVMGR_SOUND_WATER_WHEEL          = 9,
    ADVMGR_SOUND_CAMPFIRE             = 10,
    ADVMGR_SOUND_WINDMILL             = 11,
    ADVMGR_SOUND_FOUNTAIN             = 12,
    ADVMGR_SOUND_WATERING_HOLE        = 13,
    ADVMGR_SOUND_STREAM               = 14,
    ADVMGR_SOUND_MINE                 = 15,
    ADVMGR_SOUND_SAWMILL              = 16,
    ADVMGR_SOUND_DAEMON_CAVE          = 17,
    ADVMGR_SOUND_SHRINE               = 18,
    ADVMGR_SOUND_SEAGULLS             = 19,
    ADVMGR_SOUND_COASTLINE            = 20,
    ADVMGR_SOUND_TAR_PIT              = 21,
    ADVMGR_SOUND_TRADING_POST         = 22,
    ADVMGR_SOUND_DERELICT_SHIP        = 23,
    ADVMGR_SOUND_RUINS                = 24,
    ADVMGR_SOUND_DWELLING             = 25,
    ADVMGR_SOUND_ABANDONED_MINE       = 26,
    ADVMGR_SOUND_LARGE_VOLCANO        = 27,
    ADVMGR_ENVIRONMENT_SOUND_COUNT    = 28
H2_ENUM_CLASS_END(AdventureEnvironmentSoundId)

H2_ENUM_CLASS_BEGIN(AdventureCommand)
    ADVMGR_COMMAND_NONE               = -1,
    ADVMGR_COMMAND_MOVE_TO            = 1,
    ADVMGR_COMMAND_HERO_VIEW          = 2,
    ADVMGR_COMMAND_TOWN_VIEW          = 3,
    ADVMGR_COMMAND_SELECT_HERO        = 4,
    ADVMGR_COMMAND_SELECT_TOWN        = 5,
    ADVMGR_COMMAND_OCCUPIED_TOWN_VIEW = 6,
    ADVMGR_COMMAND_CONTINUE_ROUTE     = 7
H2_ENUM_CLASS_END(AdventureCommand)

H2_ENUM_CLASS_BEGIN(AdventureDrawMask)
    ADVMGR_DRAW_GROUND      = 0x01,
    ADVMGR_DRAW_OBJECT      = 0x02,
    ADVMGR_DRAW_OVERLAY     = 0x04,
    ADVMGR_DRAW_HERO        = 0x08,
    ADVMGR_DRAW_CLOUD       = 0x20,
    ADVMGR_DRAW_OVERLAY_TOP = 0x40,
    ADVMGR_DRAW_HERO_SHADOW = 0x80
H2_ENUM_CLASS_END(AdventureDrawMask)
H2_ENUM_FLAGS(AdventureDrawMask)

H2_ENUM_BEGIN(AdventureSystemOptionsConstant)
    ADVMGR_SYSTEM_OPTIONS_WINDOW_X                = 160,
    ADVMGR_SYSTEM_OPTIONS_WINDOW_Y                = 33,
    ADVMGR_SYSTEM_OPTIONS_TITLE                   = 2,
    ADVMGR_SYSTEM_OPTIONS_SOUND_FRAME_BASE        = 2,
    ADVMGR_SYSTEM_OPTIONS_SPEED_FRAME_BASE        = 4,
    ADVMGR_SYSTEM_OPTIONS_MUSIC_SOURCE_FRAME_BASE = 10,
    ADVMGR_SYSTEM_OPTIONS_ROUTE_FRAME_BASE        = 13,
    ADVMGR_SYSTEM_OPTIONS_COMPUTER_HIDDEN_FRAME   = 9,
    ADVMGR_SYSTEM_OPTIONS_INTERFACE_FRAME_BASE    = 15,
    ADVMGR_SYSTEM_OPTIONS_VIDEO_FRAME_BASE        = 18,
    ADVMGR_SYSTEM_OPTIONS_CURSOR_FRAME_BASE       = 20,
    ADVMGR_SYSTEM_OPTIONS_TEXT_ID_OFFSET          = 10,
H2_ENUM_END(AdventureSystemOptionsConstant)

H2_ENUM_BEGIN(AdventureAIStorageConstant)
    ADVMGR_PLACE_VISIT_COUNT      = 30,
    ADVMGR_PLACE_COORDINATE_COUNT = 2
H2_ENUM_END(AdventureAIStorageConstant)

class armyGroup;
class hero;
class mapCell;
class fullMap;
class sample;
class town;
class heroWindow;
class icon;
class iconWidget;
class textWidget;
class widget;
class tileset;
struct SMapChange;
struct tag_message;

struct adventureSoundCell {
    AdventureEnvironmentSoundId soundId;
    i32 volume;
};

H2_ENUM_BEGIN(AdventureManagerStorageConstant)
    ADVMGR_BOTTOM_VIEW_WIDGET_COUNT      = 12,
    ADVMGR_BOTTOM_VIEW_BACKGROUND        = 0,
    ADVMGR_BOTTOM_VIEW_FOREGROUND        = 1,
    ADVMGR_BOTTOM_VIEW_ICON_FIRST        = 2,
    ADVMGR_BOTTOM_VIEW_HERO_TEXT_FIRST   = 1,
    ADVMGR_RUNTIME_ALIGNMENT_SIZE        = 4,
    ADVMGR_OBJECT_ICON_COUNT             = 64,
    ADVMGR_ANIMATION_PHASE_COUNT         = 4,
    ADVMGR_HERO_ICON_COUNT               = IDX(FACTION_COUNT) + 2,
    ADVMGR_ACTIVE_SOUND_COUNT            = 4,
    ADVMGR_CURSOR_SAMPLE_COUNT           = 9,
    ADVMGR_STEP_PIXEL_COUNT              = 5,
    ADVMGR_STEP_DELAY_COUNT              = 5,
    ADVMGR_VIEW_WORLD_SCALE_COUNT        = 3,
    ADVMGR_VIEW_WORLD_OFFSET_KIND_COUNT  = 6,
    ADVMGR_ARMY_SIZE_NAME_SIZE           = 12,
    ADVMGR_MONSTER_ANIMATION_TABLE_SIZE  = 18
H2_ENUM_END(AdventureManagerStorageConstant)

H2_ENUM_BEGIN(AdventurePanelButtonConstant)
    ADVMGR_PANEL_BUTTON_FIRST = 1,
    ADVMGR_PANEL_BUTTON_LAST = 6
H2_ENUM_END(AdventurePanelButtonConstant)

// Unconditional six-button protocol. Existing active-manager guards stay in callers.
#define SET_ADVENTURE_BUTTON_FLAGS(message, window, cmd) \
    ((message).type = MESSAGE_WIDGET, (message).payload.widget.command = (cmd), \
     (message).payload.widget.data.value = IDX(WIDGET_FLAG_ENABLED), \
     (message).payload.widget.id = ADVMGR_PANEL_BUTTON_FIRST, (window)->BroadcastMessage(message), \
     (message).payload.widget.id = ADVMGR_PANEL_BUTTON_FIRST + 1, (window)->BroadcastMessage(message), \
     (message).payload.widget.id = ADVMGR_PANEL_BUTTON_FIRST + 2, (window)->BroadcastMessage(message), \
     (message).payload.widget.id = ADVMGR_PANEL_BUTTON_FIRST + 3, (window)->BroadcastMessage(message), \
     (message).payload.widget.id = ADVMGR_PANEL_BUTTON_FIRST + 4, (window)->BroadcastMessage(message), \
     (message).payload.widget.id = ADVMGR_PANEL_BUTTON_LAST, (window)->BroadcastMessage(message))

H2_ENUM_CLASS_BEGIN(ArmySizeNameVariant)
    ARMY_SIZE_NAME_TITLE    = 0,
    ARMY_SIZE_NAME_SENTENCE = 1,
    ARMY_SIZE_NAME_INLINE   = 2
H2_ENUM_CLASS_END(ArmySizeNameVariant)

#pragma pack(push, 1)
class advManager H2_FINAL : public baseManager {
public:
    AdventureCommand m_pendingCommand;
    class widget* m_bottomViewPrimaryWidgets[ADVMGR_BOTTOM_VIEW_WIDGET_COUNT];
    class widget* m_bottomViewSecondaryWidgets[ADVMGR_BOTTOM_VIEW_WIDGET_COUNT];
    class heroWindow* m_adventureWindow;
    u16* m_visibilityMap;
    b32 m_visibilityMapValid;
    H2_ENUM_STORAGE(TerrainType, i32) m_currentTerrain;
    char _pad_0xaa[ADVMGR_RUNTIME_ALIGNMENT_SIZE];
    class fullMap* m_mapData;
    class iconWidget* m_scrollLeftButton;
    class iconWidget* m_scrollRightButton;
    u8* m_adventureBorder;
    char _pad_0xbe[ADVMGR_RUNTIME_ALIGNMENT_SIZE];
    class tileset* m_groundTiles;
    class tileset* m_cloudTiles;
    class tileset* m_stoneTiles;
    class icon* m_objectIcons[ADVMGR_OBJECT_ICON_COUNT];
    class icon* m_puzzleIcon;
    class icon* m_cloudOverlayIcon;
    i32 m_mapOriginX;
    i32 m_mapOriginY;
    i32 m_previousOriginX;
    i32 m_previousOriginY;
    i32 m_hoverCellX;
    i32 m_hoverCellY;
    i32 m_commandTargetX;
    i32 m_commandTargetY;
    i32 m_scrollOffsetX;
    i32 m_scrollOffsetY;
    i32 m_animationTick;
    i32 m_animationFrame;
    i32 m_updatePending;
    i32 m_animationPhases[ADVMGR_ANIMATION_PHASE_COUNT];
    class icon* m_heroIcons[ADVMGR_HERO_ICON_COUNT];
    class icon* m_shadowIcon;
    class icon* m_boatShadowIcon;
    class icon* m_flagIcons[GAME_PLAYER_COUNT];
    class icon* m_boatFlagIcons[GAME_PLAYER_COUNT];
    b32 m_cursorActive;
    i32 m_drawHeroShadows;
    H2_ENUM_STORAGE(HeroCursorType, i32) m_cursorType;
    H2_ENUM_STORAGE(MapDirection, i32) m_cursorDirection;
    i32 m_cursorFrame;
    i32 m_cursorFrameCount;
    i32 m_cursorCycle;
    i32 m_cursorTurning;
    i32 m_cursorMapX;
    i32 m_previousCursorMapX;
    i32 m_cursorMapY;
    i32 m_previousCursorMapY;
    b32 m_comboHeroDrawn;
    b32 m_heroContextLocked;
    i32 m_townContextLocked;
    b32 m_forceCompleteDraw;
    i32 m_lastQuickViewX;
    i32 m_lastQuickViewY;
    b32 m_mineGuardianFacingLeft;
    i32 m_activeSoundMask;
    adventureSoundCell m_activeSounds[ADVMGR_ACTIVE_SOUND_COUNT];
    class sample* m_loopingSamples[IDX(ADVMGR_ENVIRONMENT_SOUND_COUNT)];
    class sample* m_cursorSamples[ADVMGR_CURSOR_SAMPLE_COUNT];
    b32 m_identifyHeroActive;
    // The heroes logo covers the radar; UpdateRadar clears it.
    b32 m_heroesLogoShown;
    advManager(void);
    virtual i32 Open(i32 id) OVERRIDE;
    virtual void Close(void) OVERRIDE;
    virtual MessageDispatchResult Main(struct tag_message& message) OVERRIDE;
    void StartCursor(H2_ENUM_PARAM(MapDirection, i32) direction);
    void StopCursor(i32 stopSound);
    void DrawCursor(void);
    void DrawCursorShadow(void);
    i32 GetCursorBaseFrame(H2_ENUM_PARAM(MapDirection, i32) direction);
    void TurnTo(H2_ENUM_PARAM(MapDirection, i32) direction);
    b32 GetMoveShowIt(class hero* movingHero, H2_ENUM_PARAM(MapDirection, i32) direction);
    class mapCell* MoveHero(
        H2_ENUM_PARAM(MapDirection, i32) direction,
        i32 stopAfterMove,
        i32* eventX,
        i32* eventY,
        i32* outOfMobility,
        i32 processEvent,
        i32* adjacentMonster,
        i32 forceMove
    );
    void CheckAdjacentMon(i32* adjacentMonster);
    i32 ValidMoveWithEvent(class hero* movingHero, H2_ENUM_PARAM(MapDirection, i32) direction);
    i32 ValidMove(H2_ENUM_PARAM(MapDirection, i32) direction, i32 eventMode);
    void MoveOrigin(i32 directionX, i32 directionY);
    void ProcessMapChange(struct SMapChange change);
    void ProcessIncomingSingleMapChange(struct SMapChange* incoming);
    void ProcessIncomingGroupMapChange(char* incomingData);
    void PurgeMapChangeQueue(void);
    void UnwindMapChangeQueue(i32 maximumToUnwind, i32 processChanges);
    void ViewWorld(SpellType whatToDraw, b32 drawAllObjects, b32 drawAllTerrains);
    void VWCleanup(void);
    void VWInit(i32 centerX, i32 centerY);
    void VWCompleteDraw(void);
    void GetCursorSampleSet(ConfigWalkSpeed sampleSet);
    class mapCell* DoAdvCommand(void);
    i32 GetCommandTargetX(void) {
        return m_commandTargetX;
    }
    i32 GetCommandTargetY(void) {
        return m_commandTargetY;
    }
    void CheckSetEvilInterface(i32 redraw, i32 player);
    void Reseed(i32, i32);
    MessageDispatchResult ProcessSelect(struct tag_message* message, class mapCell** eventCell);
    MessageDispatchResult ProcessDeSelect(struct tag_message* message, i32* result, class mapCell** eventCell);
    i32 ProcessSearch(i32 x, i32 y);
    MessageDispatchResult ProcessHover(i32 mouseX, i32 mouseY);
    void UpdateScreen(i32, i32 forceUpdate);
    void CompleteDraw(i32 originX, i32 originY, i32 forceDraw, i32 updateBottomView);
    void CompleteDraw(i32 update);
    i32 GetCloudLookup(i32 x, i32 y);
    void DrawCell(i32 mapX, i32 mapY, i32 screenX, i32 screenY, AdventureDrawMask drawMask, i32 forceDraw);
    class mapCell* GetCell(i32 x, i32 y);
    void UpdateRadar(i32 updateScreen, i32 partial);
    void QuickInfo(i32 cellX, i32 cellY);
    void UpdateHeroLocator(i32 locatorSlot, i32 drawWindow, i32 updateScreen);
    void UpdateHeroLocators(i32 drawWindow, i32 updateScreen);
    void UpdateTownLocators(i32 drawWindow, i32 updateScreen);
    void UpdBottomView(b32 forceUpdate, b32 drawWindow, b32 updateScreen);
    void ClearBottomView(void);
    i32 UpdBottomViewEnemyTurn(void);
    i32 UpdBottomViewNewTurn(void);
    i32 UpdBottomViewResMsg(void);
    i32 UpdBottomViewKingdom(void);
    i32 UpdBottomViewHero(void);
    void HeroQuickView(i32 heroId, i32 locatorSlot, i32 windowX, i32 windowY);
    H2_CONST char* GetArmySizeName(i32 armySize, H2_ENUM_PARAM(ArmySizeNameVariant, i32) grammar);
    void TownQuickView(i32 townId, i32 locatorSlot, i32 windowX, i32 windowY);
    void RedrawAdvScreen(i32 update, i32 freeBorder);
    void DeactivateCurrTown(void);
    void DeactivateCurrHero(void);
    void MobilizeCurrHero(i32 update);
    void DemobilizeCurrHero(void);
    void SetTownContext(i32 townId);
    void SetHeroContext(i32 heroId, i32 update);
    void DoHeroKnob(void);
    void DoTownKnob(void);
    void CastSpell(SpellType spell);
    void CheckCastSpell(void);
    i32 ComboDraw(i32 originX, i32 originY, i32 animate);
    i32 ComboDraw(i32 update);
    void SetEnvironmentOrigin(i32 originX, i32 originY, i32 stopSounds);
    void CheckLoadSample(i32 index);
    AdventureEnvironmentSoundId GetSoundId(i32 x, i32 y);
    void InsertSound(i32 x, i32 mapY, i32 distance, i32 soundLayer);
    void TeleportTo(class hero* mapHero, i32 destinationX, i32 destinationY, i32, i32 skipMapChange);
    void DimensionDoor(void);
    void TownGate(SpellType spellId);
    void SummonBoat(void);
    void ShowRoute(i32 redraw, i32, i32 updateButton);
    void HideRoute(i32 redraw, i32 clearDestination, i32 updateButton);
    void CheckDimHero(void);
    void CheckDimNextHeroBut(void);
    void SeedTo(i32 targetX, i32 targetY);
    void ForceNewHover(void);
    void ScreenScroll(H2_ENUM_PARAM(MapDirection, i32) direction, i32 updatePointer);
    void CheckScreenScroll(void);
    i32 MouseInScrollZone(void);
    void SetInitialMapOrigin(void);
    void LoadRemote(void);
    char* CheckHandleNet(void);
    MessageDispatchResult CheckHandleNetPlayerWait(struct tag_message& message, i32 doMain);
    void TrimLoopingSounds(i32 maxSamples);
    void DisableButtons(void);
    void EnableButtons(void);
    void SaveAdventureBorder(void);
    void DrawAdventureBorder(void);
    i32 FindAdjacentMonster(i32 originX, i32 originY, i32* monsterX, i32* monsterY, i32 excludedX, i32 excludedY);
    void ViewPuzzle(void);
    void PuzzleDraw(i32 left, i32 top, i32 right, i32 bottom);
    void AdvPanel(void);
    i32 ControlPanel(void);
    void SystemOptions(void);
    i32 DoVisions(class hero* visionHero);
    i32 IsCrystalBallInEffect(i32 x, i32 y, i32 radius);
    void DoEvent(class mapCell* cell, i32 x, i32 y);
    void EraseObj(class mapCell* cell, i32 x, i32 y);
    void HeroSwap(class hero* firstHero, class hero* secondHero);
    i32 BarrierEvent(class mapCell* cell, class hero*);
    void PasswordEvent(class mapCell* cell, class hero*);
    void GenericSiteEvent(class mapCell* cell, class hero* eventHero);
    void RecruitSiteEvent(class mapCell* cell, class hero* eventHero);
    void ExpansionRecruitEvent(class hero* eventHero, H2_ENUM_PARAM(CreatureType, i32) creatureType, i16* availableCount);
    void JailEvent(class mapCell* cell, class hero* eventHero, i32 x, i32 y);
    void TownEvent(class mapCell* cell, i32 x, i32 y);
    void EventSound(H2_ENUM_PARAM(MapObjectType, i32) eventType, i32 eventData, SAMPLE2* outSample);
    void EventWindow(i32 eventId, i32 buttons, H2_CONST char* text, i32 type1, i32 value1, i32 type2, i32 value2, i32 type3);
    ArtifactType GiveRandomArtifact(class hero* eventHero);
    i32 GiveExperience(class hero* eventHero, i32 experience, i32 checkLevel);
    void GiveResource(class hero* eventHero, ResourceType resourceType, i32 amount);
    void RecruitEvent(class hero* eventHero, H2_ENUM_PARAM(CreatureType, i32) creatureType, class mapCell* cell);
    i32 SkeletonEvent(class hero* eventHero, class mapCell* cell, H2_CONST char* text, i32 x, i32 y);
    i32 ZombieEvent(class hero* eventHero, class mapCell* cell, H2_CONST char* text, i32 x, i32 y);
    i32 GhostEvent(class hero* eventHero, class mapCell* cell, H2_CONST char* text, i32 x, i32 y);
    void HouseEvent(class hero* eventHero, class mapCell* cell);
    CombatResult CombatMonsterEvent(
        class hero* eventHero,
        CreatureType monsterType,
        i32 monsterCount,
        class mapCell*,
        i32 mapX,
        i32 mapY,
        i32 defender,
        i32 combatX,
        i32 combatY,
        H2_ENUM_PARAM(CreatureType, i32) secondaryType,
        i32 secondaryCount,
        i32 secondaryStacks,
        H2_ENUM_PARAM(CreatureType, i32) tertiaryType,
        i32 tertiaryCount,
        i32 tertiaryStacks
    );
    void TransferArtifacts(class hero* sourceHero, class hero* destinationHero);
    void HeroLoses(class hero* lostHero);
    void DoWhirlpool(class hero* eventHero);
    void FizzleCenter(i32 fizzleType);
    void DoAIEvent(class mapCell* cell, class hero* eventHero, i32 x, i32 y);
    i32 BarrierAIEvent(class mapCell* cell, class hero*);
    void PasswordAIEvent(class mapCell* cell, class hero*);
    void GenericSiteAIEvent(class mapCell* cell, class hero* eventHero);
    void RecruitSiteAIEvent(class mapCell* cell, class hero* eventHero);
    void JailAIEvent(class mapCell* cell, class hero* eventHero, i32 x, i32 y);
    void PlayerMonsterInteract(
        class mapCell* cell,
        class mapCell* combatCell,
        class hero* eventHero,
        i32* removeMonsterObject,
        i32 x,
        i32 y,
        i32 defender,
        i32 combatX,
        i32 combatY
    );
    void ComputerMonsterInteract(class mapCell* cell, class hero* eventHero, i32* removeMonsterObject);
    i32 DoNetCombat(char* packet);
    CombatResult DoCombat(
        i32 x,
        i32 y,
        class hero* firstHero,
        class armyGroup* firstArmy,
        class town* combatTown,
        class hero* secondHero,
        class armyGroup* secondArmy,
        i32 setupCombatX,
        i32 setupCombatY,
        i32 randomSeed,
        i32 processLosses
    );
    void SendHeroTownData(
        i32 x,
        i32 y,
        class hero* firstHero,
        class armyGroup* firstArmy,
        class town* combatTown,
        class hero* secondHero,
        class armyGroup* secondArmy,
        i32 setupCombatX,
        i32 setupCombatY,
        i32 randomSeed,
        i32 remotePlayer,
        H2_ENUM_PARAM(CombatResult, i32) combatResult,
        i32 retreatWin,
        i32 combatSurrender
    );
    void ReceiveHeroTownData(
        char* packet,
        i32* remotePlayer,
        i32* x,
        i32* y,
        class hero** firstHero,
        class armyGroup** firstArmy,
        class town** combatTown,
        class hero** secondHero,
        class armyGroup** secondArmy,
        i32* setupCombatX,
        i32* setupCombatY,
        i32* randomSeed,
        H2_ENUM_STORAGE(CombatResult, i8)* combatResult,
        i8* retreatWin,
        i8* combatSurrender
    );
    CombatResult AutoResolveCombat(
        i32 x,
        i32 y,
        class hero* firstHero,
        class armyGroup* firstArmy,
        class town* combatTown,
        class hero* secondHero,
        class armyGroup* secondArmy,
        i32 setupCombatX,
        i32 setupCombatY,
        i32 randomSeed,
        i32 processLosses
    );
};
#pragma pack(pop)
SIZE(advManager, 0x37e);

extern b32 bMoveSoundMade;
extern i32 giPixelsPerStep[ADVMGR_STEP_PIXEL_COUNT];
extern i32 giStepDelay[ADVMGR_STEP_DELAY_COUNT];
extern u8 EveryOther;
extern i32 startVals[ADVMGR_VIEW_WORLD_SCALE_COUNT];
extern i8 iVWHalf[ADVMGR_VIEW_WORLD_SCALE_COUNT][ADVMGR_VIEW_WORLD_OFFSET_KIND_COUNT]
                     [IDX(COORDINATE_AXIS_COUNT)];
extern ViewWorldScale giViewWorldScale;
extern i32 giViewWorldScaleLookup;
extern b32 gbInViewWorld;
extern i32 giLimitUpdMinX;
extern i32 iLastScrollTime;
extern i32 iSandAnim;
extern i32 giLastHourGlassUpdateTime;
extern i32 TrigX;
extern i32 TrigY;
extern BottomViewMode iCurBottomView;
extern i32 iCurBottomViewEnemy;
extern i32 iCurHourGlassPhase;
extern i32 iLastHourGlassPhase;
extern b32 gbForceUpdate;
extern i32 giCheatSeq;
extern i32 iQWE;
extern u8 monAnimDrawFrame[ADVMGR_MONSTER_ANIMATION_TABLE_SIZE];
extern i32 iLastSandAnimTime;
extern i32 iLastNewSandAnimTime;
extern i32 giFrameCount;
extern b32 gbNoShowCombat;
extern i32 S1cursorCycle;
extern i32 S1cursorFrameCount;
extern i32 S1cursorTurning;
extern i32 S1cursorBaseFrame;
extern H2_ENUM_STORAGE(MapDirection, i32) S1cursorDirection;
extern class icon* pVWMisc;
extern class icon* pVWLetters;
extern i32 iVWYPixelOffset;
extern class icon* pVWGround;
extern i32 iVWViewableCells;
extern class icon* pVWFlags;
extern b32 iVWDrawAllTerrains;
extern H2_ENUM_STORAGE(SpellType, i32) iVWWhatToDraw;
extern b32 iVWDrawAllObjs;
extern i32 iVWMapOriginX;
extern i32 iVWMapOriginY;
extern i32 iVWCenterOffset;
extern i32 iVWXPixelOffset;
extern class heroWindow* cPanel;
extern struct tag_message USMsg;
extern i32 iThisMaxY;
extern i32 giTownPortalChoice;
extern i32 iThisMinY;
extern class heroWindow* townPortalWin;
extern i32 giFrameStep;
extern char cArmySizeName[ADVMGR_ARMY_SIZE_NAME_SIZE];
extern i32 giLimitUpdMaxX;
extern i32 giLimitUpdMaxY;
extern b32 bPrefsChanged;
extern i32 giLimitUpdMinY;
extern struct tag_message CDMsg;
extern i8 bComboDraw[ADVMGR_MONSTER_ANIMATION_TABLE_SIZE]
                    [ADVMGR_MONSTER_ANIMATION_TABLE_SIZE];
extern i32 iLastAnimFrame;

i32 SaveGame(void);
MessageDispatchResult DimensionDoorHandler(struct tag_message& message);
MessageDispatchResult TownPortalHandler(struct tag_message& message);
void ComputeAdvNetControl(void);
i32 MapExtraPosAndAdjacentsSet(i32 x, i32 y, u8 mask);
MessageDispatchResult APanelHandler(struct tag_message& message);
MessageDispatchResult CPanelHandler(struct tag_message& message);
void UpdateSystemOptions(i32 initialDraw);
MessageDispatchResult SystemOptionsHandler(struct tag_message& message);
i32 GetMobilityFrame(i32 mobility);
i32 GetManaFrame(i32 mana);
u8 StopOnTrigger(class mapCell* cell);

extern float fFirstWeekTownFV;
extern i32 iVepCacheHits;
extern i32 iTotalVepHits;
extern b32 giShowComputerRoute;
extern i32l glLastStartTick;
extern i32l glCurTicks;
extern i32l glTotalTicks;
extern float gfAttackHumanBonus;
extern float gfAttackComputerBonus;
extern b32 bSVSearchArrayInUse;
extern b32 bEvaluatingTravelGates;
extern b32 gbReduceByBerserk;
extern float fBerserkFactor;
extern i32 giMaxHeroesForThisPlayer;
extern float fReduceFactor;
extern i32 giBestShipyardDist;
extern i16 gaiHeroLiveChance[GAME_HERO_COUNT];
extern i32 giHumanTownConquered;
extern i32 costTemp[IDX(RES_COUNT)];
extern b32 gbPossibleShipyardFound;
extern i32 iCurPlaceToVisit;
extern i32 giBestShipyardId;
extern b32 gbActualBoatFound;
extern float gfHeroInteractionBonus[GAME_HERO_COUNT];
extern b32 gbBerserk;
extern i32 giCurAIHeroMorale;
extern i32 iPlacesVisited[ADVMGR_PLACE_VISIT_COUNT][ADVMGR_PLACE_COORDINATE_COUNT];
extern b32 gbTroopReload;
extern i32 giCurAIHeroLuck;
extern b32 gbActualShipyardFound;

#endif
