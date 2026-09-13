#ifndef HOMM2_SOURCE_ADVMANAGER_H
#define HOMM2_SOURCE_ADVMANAGER_H

#include <va.h>
#include <BASE/baseManager.h>
#include <BASE/widget.h>
#include <SOURCE/ADVMGR.h>
#include <SOURCE/Viewwrld.h>
#include <SOURCE/GAME.h>

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
    AdventureCommand m_selectedCell;
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
    i32 m_lastHoverCell;
    i32 m_hoverCellY;
    i32 m_commandTargetX;
    i32 m_commandTargetY;
    i32 m_updateMinX;
    i32 m_updateMinY;
    i32 m_updateMaxX;
    i32 m_updateMaxY;
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
    b32 m_openState;
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
        i32* handled,
        i32 x,
        i32 y,
        i32 unused,
        i32 combatX,
        i32 combatY
    );
    void ComputerMonsterInteract(class mapCell* cell, class hero* eventHero, i32* handled);
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

#endif
