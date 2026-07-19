#ifndef HOMM2_SOURCE_ADVMANAGER_H
#define HOMM2_SOURCE_ADVMANAGER_H

#include <va.h>
#include <BASE/baseManager.h>
#include <SOURCE/ADVMGR.h>
#include <SOURCE/Viewwrld.h>

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
struct SAMPLE2;
struct SMapChange;
struct tag_message;

struct adventureSoundCell {
    AdventureEnvironmentSoundId soundId;
    i32 volume;
};

H2_ENUM_BEGIN(AdventureManagerStorageConstant)
    ADVMGR_LOCATOR_STATE_COUNT           = 12,
    ADVMGR_BOTTOM_VIEW_ITEM_COUNT        = 5,
    ADVMGR_BOTTOM_VIEW_ICON_PADDING_SIZE = 0x14,
    ADVMGR_BOTTOM_VIEW_TEXT_PADDING_SIZE = 0x18,
    ADVMGR_RUNTIME_ALIGNMENT_SIZE        = 4,
    ADVMGR_OBJECT_ICON_COUNT             = 64,
    ADVMGR_VIEW_BOUND_COUNT              = 4,
    ADVMGR_HERO_ICON_COUNT               = 8,
    ADVMGR_PLAYER_FLAG_ICON_COUNT        = 6,
    ADVMGR_ACTIVE_SOUND_COUNT            = 4,
    ADVMGR_CURSOR_SAMPLE_COUNT           = 9,
    ADVMGR_STEP_PIXEL_COUNT              = 6,
    ADVMGR_STEP_DELAY_COUNT              = 5,
    ADVMGR_VIEW_WORLD_SCALE_COUNT        = 3,
    ADVMGR_VIEW_WORLD_OFFSET_KIND_COUNT  = 6,
    ADVMGR_VIEW_WORLD_AXIS_COUNT         = 2,
    ADVMGR_ARMY_SIZE_NAME_SIZE           = 12,
    ADVMGR_MONSTER_ANIMATION_TABLE_SIZE  = 18
H2_ENUM_END(AdventureManagerStorageConstant)

#pragma pack(push, 1)
class advManager : public baseManager {
public:
    AdventureCommand m_selectedCell;
    union {
        i32 m_heroLocatorState[ADVMGR_LOCATOR_STATE_COUNT];
        class widget* m_bottomViewPrimaryWidgets[ADVMGR_LOCATOR_STATE_COUNT];
        struct {
            class iconWidget* m_bottomViewBackground;
            class iconWidget* m_bottomViewHourglassBackground;
            class iconWidget* m_bottomViewIcons[ADVMGR_BOTTOM_VIEW_ITEM_COUNT];
            char m_bottomViewIconPadding[ADVMGR_BOTTOM_VIEW_ICON_PADDING_SIZE];
        };
    };
    union {
        i32 m_townLocatorState[ADVMGR_LOCATOR_STATE_COUNT];
        class widget* m_bottomViewSecondaryWidgets[ADVMGR_LOCATOR_STATE_COUNT];
        class textWidget* m_bottomViewAllTexts[ADVMGR_LOCATOR_STATE_COUNT];
        struct {
            i32 m_bottomViewTextReserved;
            class textWidget* m_bottomViewTexts[ADVMGR_BOTTOM_VIEW_ITEM_COUNT];
            char m_bottomViewTextPadding[ADVMGR_BOTTOM_VIEW_TEXT_PADDING_SIZE];
        };
    };
    class heroWindow* m_adventureWindow;
    u16* m_visibilityMap;
    i32 m_visibilityMapValid;
    i32 m_currentTerrain;
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
    i32 m_viewBounds[ADVMGR_VIEW_BOUND_COUNT];
    class icon* m_heroIcons[ADVMGR_HERO_ICON_COUNT];
    class icon* m_shadowIcon;
    class icon* m_boatShadowIcon;
    class icon* m_flagIcons[ADVMGR_PLAYER_FLAG_ICON_COUNT];
    class icon* m_boatFlagIcons[ADVMGR_PLAYER_FLAG_ICON_COUNT];
    i32 m_cursorActive;
    i32 m_drawHeroShadows;
    i32 m_cursorType;
    i32 m_cursorDirection;
    i32 m_cursorFrame;
    i32 m_cursorFrameCount;
    i32 m_cursorCycle;
    i32 m_cursorTurning;
    i32 m_cursorMapX;
    i32 m_previousCursorMapX;
    i32 m_cursorMapY;
    i32 m_previousCursorMapY;
    i32 m_comboHeroDrawn;
    i32 m_heroContextLocked;
    i32 m_townContextLocked;
    i32 m_forceCompleteDraw;
    i32 m_lastQuickViewX;
    i32 m_lastQuickViewY;
    i32 m_mineGuardianFacingLeft;
    i32 m_activeSoundMask;
    adventureSoundCell m_activeSounds[ADVMGR_ACTIVE_SOUND_COUNT];
    class sample* m_loopingSamples[IDX(ADVMGR_ENVIRONMENT_SOUND_COUNT)];
    class sample* m_cursorSamples[ADVMGR_CURSOR_SAMPLE_COUNT];
    i32 m_identifyHeroActive;
    i32 m_openState;
    advManager(void);
    virtual i32 Open(i32) OVERRIDE;
    virtual void Close(void) OVERRIDE;
    virtual i32 Main(struct tag_message&) OVERRIDE;
    void StartCursor(i32);
    void StopCursor(i32);
    void DrawCursor(void);
    void DrawCursorShadow(void);
    i32 GetCursorBaseFrame(i32);
    void TurnTo(i32);
    i32 GetMoveShowIt(class hero*, i32);
    class mapCell* MoveHero(i32, i32, i32*, i32*, i32*, i32, i32*, i32);
    void CheckAdjacentMon(i32*);
    i32 ValidMoveWithEvent(class hero*, i32);
    i32 ValidMove(i32, i32);
    void MoveOrigin(i32, i32);
    void ProcessMapChange(struct SMapChange);
    void ProcessIncomingSingleMapChange(struct SMapChange*);
    void ProcessIncomingGroupMapChange(char*);
    void PurgeMapChangeQueue(void);
    void UnwindMapChangeQueue(i32, i32);
    void ViewWorld(ViewWorldMode, b32, b32);
    void VWCleanup(void);
    void VWInit(i32, i32);
    void VWCompleteDraw(void);
    void GetCursorSampleSet(i32);
    class mapCell* DoAdvCommand(void);
    i32 GetCommandTargetX(void) {
        return m_commandTargetX;
    }
    i32 GetCommandTargetY(void) {
        return m_commandTargetY;
    }
    void CheckSetEvilInterface(i32, i32);
    void Reseed(i32, i32);
    i32 ProcessSelect(struct tag_message*, class mapCell**);
    i32 ProcessDeSelect(struct tag_message*, i32*, class mapCell**);
    i32 ProcessSearch(i32, i32);
    i32 ProcessHover(i32, i32);
    void UpdateScreen(i32, i32);
    void CompleteDraw(i32, i32, i32, i32);
    void CompleteDraw(i32);
    i32 GetCloudLookup(i32, i32);
    void DrawCell(i32, i32, i32, i32, AdventureDrawMask, i32);
    class mapCell* GetCell(i32, i32);
    void UpdateRadar(i32, i32);
    void QuickInfo(i32, i32);
    void UpdateHeroLocator(i32, i32, i32);
    void UpdateHeroLocators(i32, i32);
    void UpdateTownLocators(i32, i32);
    void UpdBottomView(i32, i32, i32);
    void ClearBottomView(void);
    i32 UpdBottomViewEnemyTurn(void);
    i32 UpdBottomViewNewTurn(void);
    i32 UpdBottomViewResMsg(void);
    i32 UpdBottomViewKingdom(void);
    i32 UpdBottomViewHero(void);
    void HeroQuickView(i32, i32, i32, i32);
    char* GetArmySizeName(i32, i32);
    void TownQuickView(i32, i32, i32, i32);
    void RedrawAdvScreen(i32, i32);
    void DeactivateCurrTown(void);
    void DeactivateCurrHero(void);
    void MobilizeCurrHero(i32);
    void DemobilizeCurrHero(void);
    void SetTownContext(i32);
    void SetHeroContext(i32, i32);
    void DoHeroKnob(void);
    void DoTownKnob(void);
    void CastSpell(SpellType);
    void CheckCastSpell(void);
    i32 ComboDraw(i32, i32, i32);
    i32 ComboDraw(i32);
    void SetEnvironmentOrigin(i32, i32, i32);
    void CheckLoadSample(i32);
    AdventureEnvironmentSoundId GetSoundId(i32, i32);
    void InsertSound(i32, i32, i32, i32);
    void TeleportTo(class hero*, i32, i32, i32, i32);
    void DimensionDoor(void);
    void TownGate(SpellType);
    void SummonBoat(void);
    void ShowRoute(i32, i32, i32);
    void HideRoute(i32, i32, i32);
    void CheckDimHero(void);
    void CheckDimNextHeroBut(void);
    void SeedTo(i32, i32);
    void ForceNewHover(void);
    void ScreenScroll(AdventureScrollDirection, i32);
    void CheckScreenScroll(void);
    i32 MouseInScrollZone(void);
    void SetInitialMapOrigin(void);
    void LoadRemote(void);
    char* CheckHandleNet(void);
    i32 CheckHandleNetPlayerWait(struct tag_message&, i32);
    void TrimLoopingSounds(i32);
    void DisableButtons(void);
    void EnableButtons(void);
    void SaveAdventureBorder(void);
    void DrawAdventureBorder(void);
    i32 FindAdjacentMonster(i32, i32, i32*, i32*, i32, i32);
    void ViewPuzzle(void);
    void PuzzleDraw(i32, i32, i32, i32);
    void AdvPanel(void);
    i32 ControlPanel(void);
    void SystemOptions(void);
    i32 DoVisions(class hero*);
    i32 IsCrystalBallInEffect(i32, i32, i32);
    void DoEvent(class mapCell*, i32, i32);
    void EraseObj(class mapCell*, i32, i32);
    void HeroSwap(class hero*, class hero*);
    i32 BarrierEvent(class mapCell*, class hero*);
    void PasswordEvent(class mapCell*, class hero*);
    void GenericSiteEvent(class mapCell*, class hero*);
    void RecruitSiteEvent(class mapCell*, class hero*);
    void ExpansionRecruitEvent(class hero*, i32, i16*);
    void JailEvent(class mapCell*, class hero*, i32, i32);
    void TownEvent(class mapCell*, i32, i32);
    void EventSound(i32, i32, struct SAMPLE2*);
    void EventWindow(i32, i32, char*, i32, i32, i32, i32, i32);
    ArtifactType GiveRandomArtifact(class hero*);
    i32 GiveExperience(class hero*, i32, i32);
    void GiveResource(class hero*, ResourceType, i32);
    void RecruitEvent(class hero*, i32, class mapCell*);
    i32 SkeletonEvent(class hero*, class mapCell*, char*, i32, i32);
    i32 ZombieEvent(class hero*, class mapCell*, char*, i32, i32);
    i32 GhostEvent(class hero*, class mapCell*, char*, i32, i32);
    void HouseEvent(class hero*, class mapCell*);
    i32 CombatMonsterEvent(
        class hero*,
        CreatureType,
        i32,
        class mapCell*,
        i32,
        i32,
        i32,
        i32,
        i32,
        i32,
        i32,
        i32,
        i32,
        i32,
        i32
    );
    void TransferArtifacts(class hero*, class hero*);
    void HeroLoses(class hero*);
    void DoWhirlpool(class hero*);
    void FizzleCenter(i32);
    void DoAIEvent(class mapCell*, class hero*, i32, i32);
    i32 BarrierAIEvent(class mapCell*, class hero*);
    void PasswordAIEvent(class mapCell*, class hero*);
    void GenericSiteAIEvent(class mapCell*, class hero*);
    void RecruitSiteAIEvent(class mapCell*, class hero*);
    void JailAIEvent(class mapCell*, class hero*, i32, i32);
    void PlayerMonsterInteract(
        class mapCell*,
        class mapCell*,
        class hero*,
        i32*,
        i32,
        i32,
        i32,
        i32,
        i32
    );
    void ComputerMonsterInteract(class mapCell*, class hero*, i32*);
    i32 DoNetCombat(char*);
    i32 DoCombat(
        i32,
        i32,
        class hero*,
        class armyGroup*,
        class town*,
        class hero*,
        class armyGroup*,
        i32,
        i32,
        i32,
        i32
    );
    void SendHeroTownData(
        i32,
        i32,
        class hero*,
        class armyGroup*,
        class town*,
        class hero*,
        class armyGroup*,
        i32,
        i32,
        i32,
        i32,
        i32,
        i32,
        i32
    );
    void ReceiveHeroTownData(
        char*,
        i32*,
        i32*,
        i32*,
        class hero**,
        class armyGroup**,
        class town**,
        class hero**,
        class armyGroup**,
        i32*,
        i32*,
        i32*,
        i8*,
        i8*,
        i8*
    );
    i32 AutoResolveCombat(
        i32,
        i32,
        class hero*,
        class armyGroup*,
        class town*,
        class hero*,
        class armyGroup*,
        i32,
        i32,
        i32,
        i32
    );
};
#pragma pack(pop)
SIZE(advManager, 0x37e);

extern i32 bMoveSoundMade;
extern i32 giPixelsPerStep[ADVMGR_STEP_PIXEL_COUNT];
extern i32 giStepDelay[ADVMGR_STEP_DELAY_COUNT];
extern struct _SAMPLE* hOldWalkSample;
extern struct _SAMPLE* hNewWalkSample;
extern i32 EveryOther;
extern i32 startVals[ADVMGR_VIEW_WORLD_SCALE_COUNT];
extern i8 iVWHalf[ADVMGR_VIEW_WORLD_SCALE_COUNT][ADVMGR_VIEW_WORLD_OFFSET_KIND_COUNT]
                     [ADVMGR_VIEW_WORLD_AXIS_COUNT];
extern ViewWorldScale giViewWorldScale;
extern i32 giViewWorldScaleLookup;
extern b32 gbInViewWorld;
extern i32 giLimitUpdMinX;
extern i32 iLastScrollTime;
extern i32 iSandAnim;
extern i32 giLastHourGlassUpdateTime;
extern i32 TrigX;
extern i32 TrigY;
extern i32 iCurBottomView;
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
extern i32 S1cursorDirection;
extern class icon* pVWMisc;
extern class icon* pVWLetters;
extern i32 iVWYPixelOffset;
extern class icon* pVWGround;
extern i32 iVWViewableCells;
extern class icon* pVWFlags;
extern i32 iVWDrawAllTerrains;
extern ViewWorldMode iVWWhatToDraw;
extern i32 iVWDrawAllObjs;
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
extern i32 bPrefsChanged;
extern i32 giLimitUpdMinY;
extern struct tag_message CDMsg;
extern i8 bComboDraw[ADVMGR_MONSTER_ANIMATION_TABLE_SIZE]
                    [ADVMGR_MONSTER_ANIMATION_TABLE_SIZE];
extern i32 iLastAnimFrame;

#endif
